/*
 * kernel_ipc.c
 *
 * Core process scheduling and IPC (message-passing) implementation
 * for the MINIX kernel. Together with mpx.s, this forms the lowest
 * layer of the operating system.
 *
 * Entry point:
 *   sys_call()   -- invoked on software interrupt (INT), dispatches
 *                   IPC-related system calls.
 *
 * History of major rewrites:
 *   2004-10-28  nonblocking send/receive          (J. Herder)
 *   2005-05-24  notification syscall              (J. Herder)
 *   2005-05-26  message passing functions         (J. Herder)
 *   2005-07-25  syscall handling                  (J. Herder)
 *   2005-08-19  scheduling code                   (J. Herder)
 *
 * This file is performance-critical.  Many list operations use pointer-
 * pointers to avoid head/tail special cases.
 */

#define _POSIX_C_SOURCE 202311L /* for C23 features */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Kernel abstractions */
#include <minix/kernel_types.h>
#include <sys/kassert.h>

#include "arch_proto.h"
#include "clock.h"
#include "spinlock.h"
#include "vm.h"

/* KLIB: memory, string, and printing utilities */
#include <klib/include/kmemory.h>
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>

/*---------------------------------------------------------------------------*
 *  Forward declarations                                                   *
 *---------------------------------------------------------------------------*/
static void idle(void);
static void switch_address_space_idle(void);

static int do_sync_ipc(struct proc *caller, int call_nr, endpoint_t src_dst,
                       message *m_ptr);
static int mini_send(struct proc *caller, endpoint_t dst, message *m_ptr,
                     int flags);
static int mini_receive(struct proc *caller, endpoint_t src, message *m_ptr,
                        int flags);
// Modifying mini_notify to accept user_m_ptr for badge extraction
static int mini_notify(const struct proc *caller, endpoint_t dst,
                       message *user_m_ptr);
static int mini_senda(struct proc *caller, asynmsg_t *table, size_t size);
static int try_async(struct proc *caller);
static int try_one(endpoint_t receive_e, struct proc *src, struct proc *dst);
static int try_deliver_senda(struct proc *caller, asynmsg_t *table,
                             size_t size);
static int cancel_async(struct proc *src, struct proc *dst);

static int deadlock(int function, struct proc *caller, endpoint_t peer);
static int has_pending(sys_map_t *map, int src_id, bool async);
static void unset_notify_pending(struct proc *caller, int src_id);

/* Scheduling queues */
static struct priv idle_priv; /* shared idle-CPU priv struct */

/*---------------------------------------------------------------------------*
 *  Initialization                                                          *
 *---------------------------------------------------------------------------*/

/**
 * Initialize process and privilege tables; set up idle tasks for each CPU.
 */
void proc_init(void) {
  struct proc *rp;
  struct priv *sp;
  int i;

  /* Clear process table */
  for (rp = BEG_PROC_ADDR, i = -NR_TASKS; rp < END_PROC_ADDR; ++rp, ++i) {
    rp->p_rts_flags = RTS_SLOT_FREE;
    rp->p_magic = PMAGIC;
    rp->p_nr = i;
    rp->p_endpoint = _ENDPOINT(0, i);
    rp->p_scheduler = NULL;
    rp->p_priority = 0;
    rp->p_quantum_size_ms = 0;

    /* init per-proc spinlock for signals */
    spin_lock_irq_init(&rp->p_sig_lock);
    arch_proc_reset(rp);
    init_proc_capabilities(rp); // Initialize capability table for the process
  }

  /* Clear privilege table */
  for (sp = BEG_PRIV_ADDR, i = 0; sp < END_PRIV_ADDR; ++sp, ++i) {
    sp->s_proc_nr = NONE;
    sp->s_id = (sys_id_t)i;
    ppriv_addr[i] = sp;
    sp->s_sig_mgr = NONE;
    sp->s_bak_sig_mgr = NONE;

    /* Initialize new pending notification store */
    for (int j = 0; j < MAX_PENDING_NOTIFICATIONS; ++j) {
      sp->s_pending_notifications[j].pn_in_use = 0;
      sp->s_pending_notifications[j].pn_source = NONE; /* Endpoint NONE */
      sp->s_pending_notifications[j].pn_badge = 0;
    }
  }

  /* Idle task for each CPU */
  idle_priv.s_flags = IDL_F;
  for (i = 0; i < CONFIG_MAX_CPUS; i++) {
    struct proc *ip = get_cpu_var_ptr(i, idle_proc);
    ip->p_endpoint = IDLE;
    ip->p_priv = &idle_priv;
    ip->p_rts_flags |= RTS_PROC_STOP;
    snprintf(ip->p_name, sizeof(ip->p_name), "idle%d", i);
  }
}

/**
 * Idle loop: invoked when no runnable processes; halts CPU and accounts time.
 */
static void idle(void) {
  struct proc *p = get_cpulocal_var_ptr(idle_proc);
  get_cpulocal_var(proc_ptr) = p;
  if (priv(p)->s_flags & BILLABLE)
    get_cpulocal_var(bill_ptr) = p;

  switch_address_space_idle();

#ifdef CONFIG_SMP
  get_cpulocal_var(cpu_is_idle) = 1;
  if (cpuid != bsp_cpu_id)
    stop_local_timer();
  else
#endif
    restart_local_timer();

  context_stop(proc_addr(KERNEL));

#if !SPROFILE
  halt_cpu();
#else
  if (!sprofiling)
    halt_cpu();
  else {
    volatile int *v = get_cpulocal_var_ptr(idle_interrupted);
    interrupts_enable();
    while (!*v)
      arch_pause();
    interrupts_disable();
    *v = 0;
  }
#endif
}

/**
 * Switch address space to VM's for idle.
 */
static void switch_address_space_idle(void) {
#ifdef CONFIG_SMP
  switch_address_space(proc_addr(VM_PROC_NR));
#endif
}

/*---------------------------------------------------------------------------*
 *  System call dispatch                                                    *
 *---------------------------------------------------------------------------*/

/**
 * Entry point: invoked on software trap. Dispatch IPC-related calls.
 */
int do_ipc(reg_t r1, reg_t r2, reg_t r3) {
  struct proc *caller = get_cpulocal_var(proc_ptr);
  int call_nr = (int)r1;

  KASSERT(!RTS_ISSET(caller, RTS_SLOT_FREE));
  kbill_ipc = caller;

  /* handle syscall tracing/defer flags */
  if (caller->p_misc_flags & (MF_SC_TRACE | MF_SC_DEFER)) {
    if ((caller->p_misc_flags & (MF_SC_TRACE | MF_SC_DEFER)) == MF_SC_TRACE) {
      caller->p_misc_flags &= ~MF_SC_TRACE;
      caller->p_misc_flags |= MF_SC_DEFER;
      caller->p_defer.r1 = r1;
      caller->p_defer.r2 = r2;
      caller->p_defer.r3 = r3;
      cause_sig(proc_nr(caller), SIGTRAP);
      return caller->p_reg.retreg;
    }
    caller->p_misc_flags &= ~MF_SC_DEFER;
    caller->p_misc_flags |= MF_SC_ACTIVE;
  }

  if (caller->p_misc_flags & MF_DELIVERMSG) {
    panic("do_ipc: pending delivermsg for %s/%d", caller->p_name,
          caller->p_endpoint);
  }

  switch (call_nr) {
  case SENDREC:
  case SEND:
  case RECEIVE:
  case NOTIFY:
  case SENDNB:
    caller->p_accounting.ipc_sync++;
    return do_sync_ipc(caller, call_nr, (endpoint_t)r2, (message *)r3);

  case SENDA: {
    size_t msg_size = (size_t)r2;
    caller->p_accounting.ipc_async++;
    if (msg_size > 16 * (NR_TASKS + NR_PROCS))
      return EDOM;
    return mini_senda(caller, (asynmsg_t *)r3, msg_size);
  }

  case MINIX_KERNINFO:
    if (!minix_kerninfo_user)
      return EBADCALL;
    arch_set_secondary_ipc_return(caller, minix_kerninfo_user);
    return OK;

  case SYS_IPC_SEND_CAP: /* New capability-based send syscall */
    // r1 is call_nr (SYS_IPC_SEND_CAP)
    // r2 is cap_idx (int)
    // r3 is user_msg_ptr (message *)
    // caller is proc_ptr (struct proc *)
    return sys_ipc_send_cap(caller, (int)r2, (message *)r3);

  default:
    return EBADCALL;
  }
}

/**
 * Handler for synchronous IPC calls (SEND, RECEIVE, SENDREC, NOTIFY).
 */
static int do_sync_ipc(struct proc *caller, int call_nr, endpoint_t src_dst,
                       message *m_ptr) {
  int result, target_slot;
  const char *callname = ipc_call_names[call_nr];

  /* validate call number and permissions... */
  KASSERT(call_nr >= 0 && call_nr < IPCNO_HIGHEST && ipc_call_names[call_nr]);

  /* map endpoint to slot */
  if (src_dst != ANY) {
    if (!isokendpt(src_dst, &target_slot))
      return EDEADSRCDST;
    if (!may_send_to(caller, target_slot) && call_nr != RECEIVE)
      return ECALLDENIED;
  } else if (call_nr != RECEIVE) {
    return EINVAL;
  } else {
    target_slot = ANY;
  }

  /* check trap mask */
  if (!(priv(caller)->s_trap_mask & (1 << call_nr)))
    return ETRAPDENIED;

  switch (call_nr) {
  case SENDREC:
    caller->p_misc_flags |= MF_REPLY_PEND;
    /* fall through */
  case SEND:
    result = mini_send(caller, src_dst, m_ptr, 0);
    if (call_nr == SEND || result != OK)
      break;
    /* fall through for SENDREC */
  case RECEIVE:
    if (call_nr == RECEIVE) {
      caller->p_misc_flags &= ~MF_REPLY_PEND;
      IPC_STATUS_CLEAR(caller);
    }
    result = mini_receive(caller, src_dst, m_ptr, 0);
    break;

  case NOTIFY:
    result = mini_notify(caller, src_dst);
    break;
  case NOTIFY:
    // Pass m_ptr (user's message) to mini_notify for badge extraction
    result = mini_notify(caller, src_dst, m_ptr);
    break;

  case SENDNB:
    result = mini_send(caller, src_dst, m_ptr, NON_BLOCKING);
    break;

  default:
    result = EBADCALL;
  }

  return result;
}

/*---------------------------------------------------------------------------*
 *  deadlock detection                                                       *
 *---------------------------------------------------------------------------*/
static int deadlock(int function, struct proc *caller, endpoint_t peer_e) {
  struct proc *xp;
  int group_size = 1;
  endpoint_t next = peer_e;

  while (next != ANY) {
    int peer_slot;
    okendpt(next, &peer_slot);
    xp = proc_addr(peer_slot);
    KASSERT(proc_ptr_ok(xp));

    /* no dependency → no cycle */
    next = P_BLOCKEDON(xp);
    if (next == NONE)
      return 0;

    group_size++;
    if (next == caller->p_endpoint) {
      /* two-party case: SEND & RECEIVE pairing is OK */
      if (group_size == 2 &&
          ((xp->p_rts_flags ^ (function << 2)) & RTS_SENDING)) {
        return 0;
      }
      /* otherwise → deadlock */
      return group_size;
    }
  }

  return 0;
}

/*---------------------------------------------------------------------------*
 *  Notification macros                                                      *
 *---------------------------------------------------------------------------*/
/** Zero and setup a notification message for dst_proc from src_id. */
#define BuildNotifyMessage(msg, src_id, dst_proc)                              \
  do {                                                                         \
    kmemset((msg), 0, sizeof(*(msg)));                                         \
    (msg)->m_type = NOTIFY_MESSAGE;                                            \
    (msg)->m_notify.timestamp = get_monotonic();                               \
    if ((src_id) == HARDWARE) {                                                \
      (msg)->m_notify.interrupts = priv(dst_proc)->s_int_pending;              \
      priv(dst_proc)->s_int_pending = 0;                                       \
    } else {                                                                   \
      kmemcpy(&(msg)->m_notify.sigset, &priv(dst_proc)->s_sig_pending,         \
              sizeof(k_sigset_t));                                             \
      k_sigemptyset(&priv(dst_proc)->s_sig_pending);                           \
    }                                                                          \
  } while (0)
/** Zero and setup a notification message for dst_proc from src_proc_nr,
 * including badge. */
#define BuildNotifyMessageWithBadge(msg, src_proc_nr_or_hw, dst_proc_ptr,      \
                                    badge_val)                                 \
  do {                                                                         \
    kmemset((msg), 0, sizeof(*(msg)));                                         \
    (msg)->m_type = NOTIFY_MESSAGE;                                            \
    /* m_source will be set by the caller of this macro (e.g. mini_receive     \
     * from pending) */                                                        \
    (msg)->m_notify.timestamp = get_monotonic();                               \
    (msg)->m_notify.badge = (badge_val);                                       \
    if ((src_proc_nr_or_hw) == HARDWARE) {                                     \
      (msg)->m_notify.interrupts = priv(dst_proc_ptr)->s_int_pending;          \
      priv(dst_proc_ptr)->s_int_pending = 0;                                   \
      (msg)->m_notify.sigset = 0; /* Clear */                                  \
    } else if ((src_proc_nr_or_hw) == SYSTEM) {                                \
      kmemcpy(&(msg)->m_notify.sigset, &priv(dst_proc_ptr)->s_sig_pending,     \
              sizeof(k_sigset_t));                                             \
      k_sigemptyset(&priv(dst_proc_ptr)->s_sig_pending);                       \
      (msg)->m_notify.interrupts = 0; /* Clear */                              \
    } else { /* General notification from a process */                         \
      (msg)->m_notify.interrupts = 0;                                          \
      (msg)->m_notify.sigset = 0;                                              \
    }                                                                          \
  } while (0)

/*---------------------------------------------------------------------------*
 *  Notification delivery                                                   *
 *---------------------------------------------------------------------------*/
/**
 * Send or queue asynchronous notification.
 */
static int mini_notify(const struct proc *caller, endpoint_t dst_e) {
  int dst_slot;
  if (!isokendpt(dst_e, &dst_slot))
    return EDEADSRCDST;
  static int mini_notify(const struct proc *caller, endpoint_t dst_e,
                         message *user_m_ptr) {
    int dst_slot;
    u32_t badge_value = 0;

    if (!isokendpt(dst_e, &dst_slot))
      return EDEADSRCDST;

    struct proc *dst = proc_addr(dst_slot);
    message m; /* temp buffer */

    if (WILLRECEIVE(caller->p_endpoint, dst, 0, &m) &&
        !(dst->p_misc_flags & MF_REPLY_PEND)) {
      BuildNotifyMessage(&dst->p_delivermsg, proc_nr(caller), dst);
      dst->p_delivermsg.m_source = caller->p_endpoint;
      dst->p_misc_flags |= MF_DELIVERMSG;
      IPC_STATUS_ADD_CALL(dst, NOTIFY);
      RTS_UNSET(dst, RTS_RECEIVING);
      return OK;
    }
    struct proc *dst_ptr =
        proc_addr(dst_slot); // Renamed to dst_ptr for clarity

    // Extract badge from user_m_ptr. Assuming user sets m_notify.badge.
    // If kernel internal call (user_m_ptr is NULL), badge remains 0.
    if (user_m_ptr != NULL) {
      badge_value = user_m_ptr->m_notify.badge;
    }

    // Attempt immediate delivery
    if (WILLRECEIVE(
            caller->p_endpoint, dst_ptr, 0,
            NULL) && // Pass NULL for message, not needed for WILLRECEIVE check
        !(dst_ptr->p_misc_flags &
          MF_REPLY_PEND)) { // Not busy with SENDREC reply object

      // Build message directly into dst_ptr->p_delivermsg
      BuildNotifyMessageWithBadge(
          &dst_ptr->p_delivermsg,
          proc_nr(caller), /* Use proc_nr for src_id type calls */
          dst_ptr, badge_value);
      dst_ptr->p_delivermsg.m_source =
          caller->p_endpoint; // Set the true source endpoint
      dst_ptr->p_misc_flags |= MF_DELIVERMSG;
      IPC_STATUS_ADD_CALL(dst_ptr, NOTIFY);
      RTS_UNSET(dst_ptr, RTS_RECEIVING); // Unblock receiver
      return OK;
    }

    /* queue for later delivery */
    set_sys_bit(priv(dst)->s_notify_pending, priv(caller)->s_id);
    return OK;
    // Immediate delivery failed, try to queue using new s_pending_notifications
    // array
    struct priv *dst_priv = priv(dst_ptr);
    int i;
    int stored = 0;
    for (i = 0; i < MAX_PENDING_NOTIFICATIONS; ++i) {
      if (!dst_priv->s_pending_notifications[i].pn_in_use) {
        dst_priv->s_pending_notifications[i].pn_source = caller->p_endpoint;
        dst_priv->s_pending_notifications[i].pn_badge = badge_value;
        dst_priv->s_pending_notifications[i].pn_in_use = 1;
        stored = 1;
        break;
      }
    }

    if (!stored) {
      // Notification queue is full, drop the notification.
      // Optional: kprintf warning about dropped notification.
      kprintf("mini_notify: Pending notification queue for %d full, dropping "
              "notification from %d with badge %u\n",
              dst_e, caller->p_endpoint, badge_value);
    }

    // NOTIFY is best-effort, so return OK even if dropped or if it replaced
    // s_notify_pending logic. The old set_sys_bit for s_notify_pending is now
    // fully replaced.
    return OK;
  }

  /*---------------------------------------------------------------------------*
   *  Pending check helpers                                                    *
   *---------------------------------------------------------------------------*/
  static int has_pending(sys_map_t * map, int src_id, bool async) {
    if (src_id != ANY) {
      if (get_sys_bit(*map, nr_to_id(src_id)))
        return src_id;
      return NULL_PRIV_ID;
    }
    for (int i = 0; i < NR_SYS_PROCS; ++i) {
      if (get_sys_bit(*map, i)) {
        struct proc *p = proc_addr(id_to_nr(i));
        if (async && RTS_ISSET(p, RTS_VMINHIBIT)) {
          p->p_misc_flags |= MF_SENDA_VM_MISS;
          continue;
        }
        return i;
      }
    }
    return NULL_PRIV_ID;
    static int has_pending(sys_map_t * map, int src_id, bool async) {
      // This function was for the old sys_map_t s_notify_pending and
      // s_asyn_pending. It might need to be adapted or is partially deprecated
      // if s_notify_pending is fully removed. For now, keep its logic for
      // s_asyn_pending if that's still in use. If only s_notify_pending is
      // changed, then this function's use for notifications will be replaced by
      // iterating the new s_pending_notifications array.
      if (src_id != ANY) {
        // Assuming 'map' could be s_asyn_pending. If it was s_notify_pending,
        // this path is dead for notifications.
        if (get_sys_bit(*map, nr_to_id(src_id)))
          return src_id;
        return NULL_PRIV_ID;
      }
      for (int i = 0; i < NR_SYS_PROCS; ++i) {
        if (get_sys_bit(*map, i)) {
          struct proc *p = proc_addr(id_to_nr(i));
          if (async &&
              RTS_ISSET(p, RTS_VMINHIBIT)) { // async seems to relate to SENDA /
                                             // s_asyn_pending
            p->p_misc_flags |= MF_SENDA_VM_MISS;
            continue;
          }
          return i;
        }
      }
      return NULL_PRIV_ID;
    }
    static void unset_notify_pending(struct proc * caller, int src_id) {
      unset_sys_bit(priv(caller)->s_notify_pending, src_id);
      static void unset_notify_pending(struct proc * caller, int src_id) {
        // This function was for the old s_notify_pending.
        // It should be removed or adapted if a similar "clear one specific
        // source" is needed for the new s_pending_notifications array (which
        // would involve finding it and clearing pn_in_use). For now, commenting
        // out its body as direct bit manipulation is no longer correct for
        // notifications.
        /* unset_sys_bit(priv(caller)->s_notify_pending, src_id); */
        // New logic: Iterate s_pending_notifications, find matching src_id, and
        // clear it. This function might not be directly called anymore if
        // mini_receive clears slots directly.
        struct priv *caller_priv = priv(caller);
        for (int i = 0; i < MAX_PENDING_NOTIFICATIONS; ++i) {
          if (caller_priv->s_pending_notifications[i].pn_in_use &&
              caller_priv->s_pending_notifications[i].pn_source ==
                  _ENDPOINT(0, src_id)) { // Assuming src_id is proc_nr
            caller_priv->s_pending_notifications[i].pn_in_use = 0;
            caller_priv->s_pending_notifications[i].pn_source = NONE;
            caller_priv->s_pending_notifications[i].pn_badge = 0;
            // break; // Assuming only one match, or clear all from this source?
            // The old unset_sys_bit only cleared one bit.
          }
        }
      }

/*---------------------------------------------------------------------------*
 *  Asynchronous send (mini_senda)                                           *
 *---------------------------------------------------------------------------*/
/** Complain macro when asynmsg operations fail */
#define ASCOMPLAIN(caller, entry, field)                                       \
  kprintf_stub("kernel:%s:%d: asyn failed for %s in %s (%zu/%zu)\n", __FILE__, \
               __LINE__, field, (caller)->p_name, (size_t)entry,               \
               (size_t)(caller)->p_priv->s_asynsize)
#define ASCOMPLAIN(caller, entry, field)                                       \
  kprintf("kernel:%s:%d: asyn failed for %s in %s (%zu/%zu)\n", __FILE__,      \
          __LINE__, field, (caller)->p_name, (size_t)entry,                    \
          (size_t)(caller)->p_priv->s_asynsize)

/* ... Implementation of try_deliver_senda, try_async, try_one,
   cancel_async, mini_senda follows the same pattern: retrieve table,
   iterate entries, handle copy, deliver or queue, set/clear flags,
   notify ASYNCM when done. Omitted here for brevity. ... */

/*---------------------------------------------------------------------------*
 *  Scheduling queue operations                                              *
 *---------------------------------------------------------------------------*/
/* enqueue, dequeue, pick_proc and related functions omitted for brevity */

/*---------------------------------------------------------------------------*
 *  Context switch: switch_to_user                                           *
 *---------------------------------------------------------------------------*/
/* switch_to_user and FPU exception handler omitted for brevity */

// Placeholder for mini_receive modification - this is complex and needs full
// context The diff below will only focus on mini_notify and related helpers for
// now. The actual modification of mini_receive to iterate
// s_pending_notifications will be done in the next diff block.

/*===========================================================================*
 * MDLM and Capability PoC Functions                                         *
 *===========================================================================*/
#include <minix/capability.h>
#include <minix/ipc_mdlm.h> // For ipc_state_t (if used directly by proc) and CONFIG_MDLM
#include <minix/syslib.h> // For data_copy
#ifdef CONFIG_MDLM
#include <minix/mdlm_cap_dag.h> // For mdlm_cap_dag_access_check
#endif
#include <minix/profile.h> // For Profiling Macros

      // Note: The old static pm_capability_instance and the cap_lookup stub
      // that were here are now removed. The actual cap_lookup is in
      // capability.c and uses per-process tables. init_proc_capabilities (in
      // capability.c) sets up initial capabilities.

      // The old mdlm_access_check stub is removed.
      // Calls will now go to mdlm_cap_dag_access_check from mdlm_cap_dag.c

#ifdef CONFIG_MDLM
// No local stub for mdlm_access_check anymore.
#endif

      // Core IPC fast send logic (skeleton)
      static int ipc_fast_send(struct proc * caller_ptr, capability_t * cap,
                               message * msg) {
        if (!cap) {
          return EINVAL; // No capability provided
        }

        // Target endpoint from capability
        endpoint_t target_ep = cap->target;
        int target_proc_nr;

        // Validate target endpoint and get process number
        // isokendpt also fills in the process number (slot)
        if (!isokendpt(target_ep, &target_proc_nr)) {
          return EDEADSRCDST;
        }

        // For this PoC, we are simplifying. A real fast path might have its own
        // streamlined checks and delivery logic, possibly bypassing some parts
        // of mini_send. Here, we'll just print and conceptually call mini_send.
        // kprintf("IPC: ipc_fast_send from %d to %d (proc %d), type %d\n",
        //         caller_ptr->p_endpoint, target_ep, target_proc_nr,
        //         msg->m_type);

        // For the getpid() PoC, the target is PM.
        // The actual message delivery for this PoC would be to make PM handle a
        // GETPID request. This simplified ipc_fast_send will just use
        // mini_send. A true "fast path" might avoid some overhead of mini_send
        // if certain conditions are met. (e.g. if receiver is already waiting,
        // copy directly, no full queue/state changes) This is a placeholder for
        // where that optimized logic would go. For now, it demonstrates the
        // capability check and then calls the existing path.
        return mini_send(caller_ptr, target_ep, msg, 0 /* flags */);
      }

      // New syscall handler for capability-based send
      // This would be registered or called from do_ipc if it were a real
      // syscall. Parameters: cap_idx (from user), user_msg_ptr (pointer to user
      // message) Caller is implicit (current_proc from get_cpulocal_var).
      int sys_ipc_send_cap(struct proc * caller_ptr, int cap_idx,
                           message *user_msg_ptr) {
        capability_t *cap;
        message k_message; // Kernel copy of the message
        int result;

        PROF_START(PROF_ID_IPC_SYSCALL_ENTRY);

        PROF_START(PROF_ID_IPC_MSG_COPY);
        // 1. Copy message from user space to kernel space
        // In MINIX, data_copy is typically used. It's available via syslib.h
        // for user space, but in kernel, direct memory copy or specific
        // kernel_phys_copy might be used if dealing with physical addresses, or
        // direct struct copy if virtual addresses are fine. Let's assume
        // data_copy or equivalent is available/adapted for kernel->kernel or
        // user->kernel. For copying from user caller_ptr to kernel's k_message:
        result =
            data_copy(caller_ptr->p_endpoint, (vir_bytes)user_msg_ptr, KERNEL,
                      (vir_bytes)&k_message, (phys_bytes)sizeof(message));
        if (result != OK) {
          kprintf(
              "sys_ipc_send_cap: data_copy failed from user %d with error %d\n",
              caller_ptr->p_endpoint, result);
          PROF_END(PROF_ID_IPC_MSG_COPY);
          PROF_END(PROF_ID_IPC_SYSCALL_ENTRY);
          return result; // Propagate EFAULT or other error from data_copy
        }
        PROF_END(PROF_ID_IPC_MSG_COPY);
        k_message.m_source =
            caller_ptr
                ->p_endpoint; // Set source endpoint in kernel message copy

        PROF_START(PROF_ID_IPC_CAP_LOOKUP);
        cap = cap_lookup(caller_ptr->p_endpoint, cap_idx);
        PROF_END(PROF_ID_IPC_CAP_LOOKUP);

        if (!cap) {
          PROF_END(PROF_ID_IPC_SYSCALL_ENTRY);
          return EINVAL; // Invalid capability index or lookup failed
        }

#ifdef CONFIG_MDLM
        PROF_START(PROF_ID_IPC_MDLM_CHECK);
        if (!mdlm_cap_dag_access_check(caller_ptr, cap,
                                       OP_SEND)) { // Updated call
          PROF_END(PROF_ID_IPC_MDLM_CHECK);
          PROF_END(PROF_ID_IPC_SYSCALL_ENTRY);
          return EACCES; // MDLM permission denied
        }
        PROF_END(PROF_ID_IPC_MDLM_CHECK);
#endif

        PROF_START(PROF_ID_IPC_FAST_SEND);
        result = ipc_fast_send(caller_ptr, cap, &k_message);
        PROF_END(PROF_ID_IPC_FAST_SEND);

        PROF_END(PROF_ID_IPC_SYSCALL_ENTRY);
        return result;
      }

      // Skeleton for ipc_fast_reply
      // This function is intended to be called by a syscall handler for replies
      // in the new IPC mechanism. For the getpid() PoC, the app calls
      // SYS_IPC_SEND_CAP, then a regular RECEIVE. PM calls a regular send to
      // reply. So this ipc_fast_reply might not be directly used by PM in this
      // specific PoC flow, but is part of the conceptual design. If the app
      // were to use a new SYS_IPC_RECEIVE_CAP, that new syscall might
      // internally note that it's expecting a reply via this path, and a
      // corresponding SYS_IPC_REPLY_CAP from PM would then perhaps call
      // ipc_fast_reply. For now, it's a conceptual counterpart to
      // ipc_fast_send.
      static int ipc_fast_reply(struct proc * replier_ptr,
                                endpoint_t original_caller_ep,
                                message * reply_msg) {
        // kprintf("IPC: ipc_fast_reply from %d to %d, type %d\n",
        //         replier_ptr->p_endpoint, original_caller_ep,
        //         reply_msg->m_type);

        // Ensure the reply message has its source set correctly.
        reply_msg->m_source = replier_ptr->p_endpoint;

        // For this PoC, just use mini_send to send the reply.
        // A more optimized version might have different logic.
        return mini_send(replier_ptr, original_caller_ep, reply_msg,
                         0 /* flags for blocking send */);
      }
