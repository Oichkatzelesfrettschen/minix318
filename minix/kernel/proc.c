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

  /* queue for later delivery */
  set_sys_bit(priv(dst)->s_notify_pending, priv(caller)->s_id);
  return OK;
}

/*---------------------------------------------------------------------------*
 *  Pending check helpers                                                    *
 *---------------------------------------------------------------------------*/
static int has_pending(sys_map_t *map, int src_id, bool async) {
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
}
static void unset_notify_pending(struct proc *caller, int src_id) {
  unset_sys_bit(priv(caller)->s_notify_pending, src_id);
}

/*---------------------------------------------------------------------------*
 *  Asynchronous send (mini_senda)                                           *
 *---------------------------------------------------------------------------*/
/** Complain macro when asynmsg operations fail */
#define ASCOMPLAIN(caller, entry, field)                                       \
  kprintf_stub("kernel:%s:%d: asyn failed for %s in %s (%zu/%zu)\n", __FILE__, \
               __LINE__, field, (caller)->p_name, (size_t)entry,               \
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
