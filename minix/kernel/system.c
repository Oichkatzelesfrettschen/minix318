// minix/kernel/system.c

// Keep existing includes from the original system.c
#include "kernel/system.h"
#include "kernel/clock.h"
#include "kernel/vm.h"
#include <klib/include/kmemory.h>
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <minix/endpoint.h>
#include <minix/kernel_types.h>
#include <minix/safecopies.h>
#include <sys/kassert.h>

#include "kernel/clock.h"
#include "kernel/vm.h"

/*---------------------------------------------------------------------------*
 *  Call vector and dispatcher                                               *
 *---------------------------------------------------------------------------*/
static int (*call_vec[NR_SYS_CALLS])(struct proc *caller, message *m_ptr);

/** Map a kernel call number to its handler. */
#define map(call_nr, handler)                                                  \
  do {                                                                         \
    int idx = (call_nr) - KERNEL_CALL;                                         \
    KASSERT(idx >= 0 && idx < NR_SYS_CALLS, "map: call %d out of range",       \
            call_nr);                                                          \
    call_vec[idx] = handler;                                                   \
  } while (0)

/**
 * @brief Finalize a kernel call: handle VMSUSPEND or reply to user.
 *
 * Includes assertions for validating caller and message states, especially
 * around VMSUSPEND conditions and message delivery pointers.
 * (Copied from original system.c as it's used by original kernel_call)
 */
static void kernel_call_finish(struct proc *caller, message *msg, int result) {
  KASSERT(caller && proc_ptr_ok(caller));
  KASSERT(msg);
  if (result == VMSUSPEND) {
    KASSERT(RTS_ISSET(caller, RTS_VMREQUEST));
    KASSERT(caller->p_vmrequest.type == VMSTYPE_KERNELCALL);
    caller->p_vmrequest.saved.reqmsg = *msg;
    caller->p_misc_flags |= MF_KCALL_RESUME;
  } else {
    caller->p_vmrequest.saved.reqmsg.m_source = NONE;
    if (result != EDONTREPLY) {
      KASSERT(caller->p_delivermsg_vir);
      msg->m_source = SYSTEM;
      msg->m_type = result;
#if DEBUG_IPC_HOOK
      hook_ipc_msgkresult(msg, caller);
#endif
      if (copy_msg_to_user(msg, (message *)caller->p_delivermsg_vir)) {
        kprintf_stub(
            "WARNING: bad user ptr 0x%08lx from %s/%d in kernel_call_finish\n",
            (unsigned long)caller->p_delivermsg_vir, caller->p_name,
            caller->p_endpoint);
        cause_sig(proc_nr(caller), SIGSEGV);
      }
    }
  }
}

/**
 * @brief Validate and dispatch a kernel call to its handler.
 */
static int kernel_call_dispatch(struct proc *caller, message *msg) {
  int call_nr = msg->m_type - KERNEL_CALL;
  KASSERT(caller && proc_ptr_ok(caller) && priv(caller));
  KASSERT(call_nr >= 0 && call_nr < NR_SYS_CALLS);
#if DEBUG_IPC_HOOK
  hook_ipc_msgkcall(msg, caller);
#endif
  if (!GET_BIT(priv(caller)->s_k_call_mask, call_nr)) {
    kprintf_stub("SYSTEM: denied call %d from %d\n", call_nr, msg->m_source);
    return ECALLDENIED;
  }
  if (!call_vec[call_nr]) {
    kprintf_stub("SYSTEM: unused call %d from %d\n", call_nr,
                 caller->p_endpoint);
    return EBADREQUEST;
  }
  return call_vec[call_nr](caller, msg);
}

/* kernel_call function from original system.c, modified to use do_system_call *
 * Handles kernel call traps from user processes. Includes assertions for
 * message and caller validity.
 */
void kernel_call(message *m_user, struct proc *caller) {
  message msg;
  int result;

  KASSERT(caller && proc_ptr_ok(caller) && m_user);
  caller->p_delivermsg_vir = (vir_bytes)m_user;
  if (copy_msg_from_user(m_user, &msg) != OK) {
    kprintf_stub("WARNING: bad user ptr 0x%08x from %s/%d\n", m_user,
                 caller->p_name, caller->p_endpoint);
    cause_sig(proc_nr(caller), SIGSEGV);
    return;
  }
  msg.m_source = caller->p_endpoint;
  result = kernel_call_dispatch(caller, &msg);
  kbill_kcall = caller;
  kernel_call_finish(caller, &msg, result);
}

/**
 * @brief Initialize system task: IRQ hooks, alarms, call vector.
 */
void system_init(void) {
  int i;
  struct priv *sp;

  /* IRQ hooks */
  for (i = 0; i < NR_IRQ_HOOKS; i++)
    irq_hooks[i].proc_nr_e = NONE;

  /* Alarm timers */
  for (sp = BEG_PRIV_ADDR; sp < END_PRIV_ADDR; sp++)
    tmr_inittimer(&sp->s_alarm_timer);

  /* Call vector */
  for (i = 0; i < NR_SYS_CALLS; i++)
    call_vec[i] = NULL;

  /* Map system calls to handlers */
  map(SYS_FORK, do_fork);
  map(SYS_EXEC, do_exec);
  map(SYS_CLEAR, do_clear);
  map(SYS_EXIT, do_exit);
  map(SYS_PRIVCTL, do_privctl);
  map(SYS_TRACE, do_trace);
  map(SYS_SETGRANT, do_setgrant);
  map(SYS_RUNCTL, do_runctl);
  map(SYS_UPDATE, do_update);
  map(SYS_STATECTL, do_statectl);

  map(SYS_KILL, do_kill);
  map(SYS_GETKSIG, do_getksig);
  map(SYS_ENDKSIG, do_endksig);
  map(SYS_SIGSEND, do_sigsend);
  map(SYS_SIGRETURN, do_sigreturn);

  map(SYS_IRQCTL, do_irqctl);
#if defined(__i386__)
  map(SYS_DEVIO, do_devio);
  map(SYS_VDEVIO, do_vdevio);
#endif

  map(SYS_MEMSET, do_memset);
  map(SYS_VMCTL, do_vmctl);

  map(SYS_UMAP, do_umap);
  map(SYS_UMAP_REMOTE, do_umap_remote);
  map(SYS_VUMAP, do_vumap);
  map(SYS_VIRCOPY, do_vircopy);
  map(SYS_PHYSCOPY, do_copy);
  map(SYS_SAFECOPYFROM, do_safecopy_from);
  map(SYS_SAFECOPYTO, do_safecopy_to);
  map(SYS_VSAFECOPY, do_vsafecopy);
  map(SYS_SAFEMEMSET, do_safememset);

  map(SYS_TIMES, do_times);
  map(SYS_SETALARM, do_setalarm);
  map(SYS_STIME, do_stime);
  map(SYS_SETTIME, do_settime);
  map(SYS_VTIMER, do_vtimer);

  map(SYS_ABORT, do_abort);
  map(SYS_GETINFO, do_getinfo);
  map(SYS_DIAGCTL, do_diagctl);

  map(SYS_SPROF, do_sprofile);

#if defined(__arm__)
  map(SYS_PADCONF, do_padconf);
#endif
#if defined(__i386__)
  map(SYS_READBIOS, do_readbios);
  map(SYS_IOPENABLE, do_iopenable);
  map(SYS_SDEVIO, do_sdevio);
#endif

  map(SYS_SETMCONTEXT, do_setmcontext);
  map(SYS_GETMCONTEXT, do_getmcontext);

  map(SYS_SCHEDULE, do_schedule);
  map(SYS_SCHEDCTL, do_schedctl);
}

/*---------------------------------------------------------------------------*
 *  Privilege and IPC mask management                                        *
 *---------------------------------------------------------------------------*/
/**
 * @brief Allocate or assign a privilege slot to a new process.
 */
int get_priv(struct proc *rc, int priv_id) {
  struct priv *sp;
  if (priv_id == NULL_PRIV_ID) {
    for (sp = BEG_DYN_PRIV_ADDR; sp < END_DYN_PRIV_ADDR; sp++)
      if (sp->s_proc_nr == NONE)
        break;
    if (sp >= END_DYN_PRIV_ADDR)
      return ENOSPC;
  } else {
    if (!is_static_priv_id(priv_id))
      return EINVAL;
    if (priv[priv_id].s_proc_nr != NONE)
      return EBUSY;
    sp = &priv[priv_id];
  }
  rc->p_priv = sp;
  sp->s_proc_nr = proc_nr(rc);
  return OK;
}

/**
 * @brief Grant a process permission to send IPC to a privilege ID.
 */
void set_sendto_bit(const struct proc *rp, int id) {
  if (id_to_nr(id) == NONE || priv_id(rp) == id) {
    unset_sys_bit(priv(rp)->s_ipc_to, id);
    return;
  }
  set_sys_bit(priv(rp)->s_ipc_to, id);
  if (priv_addr(id)->s_trap_mask & ~(1 << RECEIVE))
    set_sys_bit(priv_addr(id)->s_ipc_to, priv_id(rp));
}

/**
 * @brief Revoke a process’s permission to send IPC to a privilege ID.
 */
void unset_sendto_bit(const struct proc *rp, int id) {
  unset_sys_bit(priv(rp)->s_ipc_to, id);
  unset_sys_bit(priv_addr(id)->s_ipc_to, priv_id(rp));
}

/**
 * @brief Bulk update a process’s IPC send mask from a bitmap.
 */
void fill_sendto_mask(const struct proc *rp, sys_map_t *map) {
  for (int i = 0; i < NR_SYS_PROCS; i++) {
    if (get_sys_bit(*map, i))
      set_sendto_bit(rp, i);
    else
      unset_sendto_bit(rp, i);
  }
}

/*---------------------------------------------------------------------------*
 *  Signal and notification handling                                         *
 *---------------------------------------------------------------------------*/
/**
 * @brief Send a notification to a system process.
 */
int send_sig(endpoint_t ep, int sig_nr) {
  int slot;
  if (sig_nr <= 0 || sig_nr >= _NSIG)
    KASSERT(false, "send_sig: bad sig %d", sig_nr);
  if (!isokendpt(ep, &slot) || isemptyn(slot))
    return EINVAL;

  struct proc *rp = proc_addr(slot);
  KASSERT(rp && rp->p_magic == PMAGIC && priv(rp));
  return mini_notify(proc_addr(SYSTEM), rp->p_endpoint);
}

/**
 * @brief Cause a signal to be delivered to a process or its signal manager.
 */
void cause_sig(proc_nr_t proc_nr, int sig_nr) {
  KASSERT(sig_nr > 0 && sig_nr < _NSIG);
  struct proc *rp = proc_addr(proc_nr);
  KASSERT(rp && rp->p_magic == PMAGIC && priv(rp));
  endpoint_t mgr = priv(rp)->s_sig_mgr;
  if (mgr == SELF)
    mgr = rp->p_endpoint;

  unsigned flags = spin_lock_irqsave(&rp->p_sig_lock);
  if (rp->p_endpoint == mgr) {
    /* Self-managed: queue for kernel-level handling */
    k_sigaddset(&priv(rp)->s_sig_pending, sig_nr);
    RTS_SET(rp, RTS_SIGNALED | RTS_SIG_PENDING);
    spin_unlock_irqrestore(&rp->p_sig_lock, flags);
    if (send_sig(rp->p_endpoint, SIGKSIGSM) != OK)
      panic("send_sig failed");
    return;
  }
  /* External manager */
  if (!k_sigismember(&rp->p_pending, sig_nr)) {
    k_sigaddset(&rp->p_pending, sig_nr);
    RTS_SET(rp, RTS_SIGNALED | RTS_SIG_PENDING);
    spin_unlock_irqrestore(&rp->p_sig_lock, flags);
    if (send_sig(mgr, SIGKSIG) != OK)
      panic("send_sig failed");
  } else {
    spin_unlock_irqrestore(&rp->p_sig_lock, flags);
  }
}

/** @brief Complete delayed signal delivery. */
void sig_delay_done(struct proc *rp) {
  rp->p_misc_flags &= ~MF_SIG_DELAY;
  cause_sig(proc_nr(rp), SIGSNDELAY);
}

/** @brief Broadcast a diagnostic signal to interested services. */
void send_diag_sig(void) {
  for (struct priv *p = BEG_PRIV_ADDR; p < END_PRIV_ADDR; p++) {
    if (p->s_proc_nr != NONE && p->s_diag_sig) {
      endpoint_t ep = proc_addr(p->s_proc_nr)->p_endpoint;
      send_sig(ep, SIGKMESS);
    }
  }
}

/*---------------------------------------------------------------------------*
 *  Endpoint and IPC cleanup                                                 *
 *---------------------------------------------------------------------------*/
/** @brief Remove VM request state for a process. */
static void clear_memreq(struct proc *rp) {
  if (!RTS_ISSET(rp, RTS_VMREQUEST))
    return;
  for (struct proc **pp = &vmrequest; *pp;
       pp = &(*pp)->p_vmrequest.nextrequestor) {
    if (*pp == rp) {
      *pp = rp->p_vmrequest.nextrequestor;
      break;
    }
  }
  RTS_UNSET(rp, RTS_VMREQUEST);
}

/** @brief Remove a process from any IPC queues. */
static void clear_ipc(struct proc *rc) {
  if (RTS_ISSET(rc, RTS_SENDING)) {
    int dst;
    okendpt(rc->p_sendto_e, &dst);
    for (struct proc **q = &proc_addr(dst)->p_caller_q; *q;
         q = &(*q)->p_q_link) {
      if (*q == rc) {
        *q = rc->p_q_link;
        break;
      }
    }
    RTS_UNSET(rc, RTS_SENDING);
  }
  RTS_UNSET(rc, RTS_RECEIVING);
}

/** @brief Fully clear IPC and VM state for an exiting process. */
void clear_endpoint(struct proc *rc) {
  if (isemptyp(rc))
    panic("clear_endpoint: empty slot %d", rc->p_endpoint);
  RTS_SET(rc, RTS_NO_ENDPOINT);
  if (priv(rc)->s_flags & SYS_PROC)
    priv(rc)->s_asynsize = 0;
  clear_ipc(rc);
  clear_ipc_refs(rc, EDEADSRCDST);
  clear_memreq(rc);
}

/**
 * @brief Remove IPC dependencies on an exiting process.
 */
void clear_ipc_refs(struct proc *rc, int caller_ret) {
  int src_id;
  /* Cancel pending async sends */
  while ((src_id = has_pending_asend(rc, ANY)) != NULL_PRIV_ID) {
    cancel_async(proc_addr(id_to_nr(src_id)), rc);
  }
  /* Clear all queues and notify blocked callers */
  for (struct proc *rp = BEG_PROC_ADDR; rp < END_PROC_ADDR; rp++) {
    if (isemptyp(rp))
      continue;
    unset_sys_bit(priv(rp)->s_notify_pending, priv(rc)->s_id);
    unset_sys_bit(priv(rp)->s_asyn_pending, priv(rc)->s_id);
    if (P_BLOCKEDON(rp) == rc->p_endpoint) {
      rp->p_reg.retreg = caller_ret;
      clear_ipc(rp);
    }
  }
}

/*---------------------------------------------------------------------------*
 *  Kernel call resumption (VM)                                               *
 *---------------------------------------------------------------------------*/
/** @brief Resume a previously suspended kernel call. */
void kernel_call_resume(struct proc *caller) {
  KASSERT(!RTS_ISSET(caller, RTS_SLOT_FREE));
  KASSERT(RTS_ISSET(caller, RTS_VMREQUEST));
  int result = kernel_call_dispatch(caller, &caller->p_vmrequest.saved.reqmsg);
  caller->p_misc_flags &= ~MF_KCALL_RESUME;
  kernel_call_finish(caller, &caller->p_vmrequest.saved.reqmsg, result);
}

/*---------------------------------------------------------------------------*
 *  Scheduling control                                                      *
 *---------------------------------------------------------------------------*/
/**
 * @brief Change scheduling parameters of a process.
 */
int sched_proc(struct proc *p, int priority, int quantum, int cpu, int niced) {
  if ((priority < 0 || priority >= NR_SCHED_QUEUES) && priority != -1)
    return EINVAL;
  if ((quantum < 1) && quantum != -1)
    return EINVAL;
#ifdef CONFIG_SMP
  if (cpu != -1 && ((unsigned)cpu >= ncpus || !cpu_is_ready(cpu)))
    return EBADCPU;
#endif
  if (proc_is_runnable(p))
    RTS_SET(p, RTS_NO_QUANTUM);
  if (priority != -1)
    p->p_priority = priority;
  if (quantum != -1) {
    p->p_quantum_size_ms = quantum;
    p->p_cpu_time_left = ms_2_cpu_time(quantum);
  }
#ifdef CONFIG_SMP
  if (cpu != -1)
    p->p_cpu = cpu;
#endif
  p->p_misc_flags =
      niced ? (p->p_misc_flags | MF_NICED) : (p->p_misc_flags & ~MF_NICED);
  RTS_UNSET(p, RTS_NO_QUANTUM);
  return OK;
}

/*---------------------------------------------------------------------------*
 *  IPC filter management                                                    *
 *---------------------------------------------------------------------------*/
int add_ipc_filter(struct proc *rp, int type, vir_bytes addr, size_t len) {
  int n = len / sizeof(ipc_filter_el_t);
  if ((type != IPCF_BLACKLIST && type != IPCF_WHITELIST) ||
      len % sizeof(ipc_filter_el_t) != 0 || n < 1 || n > IPCF_MAX_ELEMENTS)
    return EINVAL;
  ipc_filter_t *f;
  IPCF_POOL_ALLOCATE_SLOT(type, &f);
  if (!f)
    return ENOMEM;
  f->num_elements = n;
  f->next = NULL;
  int r = data_copy(rp->p_endpoint, addr, KERNEL, (vir_bytes)f->elements, len);
  if (r == OK)
    r = check_ipc_filter(f, TRUE);
  if (r != OK) {
    IPCF_POOL_FREE_SLOT(f);
    return r;
  }
  for (ipc_filter_t **fp = &priv(rp)->s_ipcf; *fp; fp = &(*fp)->next) {
  }
  *fp = f;
  return OK;
}

void clear_ipc_filters(struct proc *rp) {
  ipc_filter_t *f = priv(rp)->s_ipcf, *next;
  while (f) {
    next = f->next;
    IPCF_POOL_FREE_SLOT(f);
    f = next;
  }
  priv(rp)->s_ipcf = NULL;
  if (rp->p_endpoint == VM_PROC_NR && vmrequest)
    if (send_sig(VM_PROC_NR, SIGKMEM) != OK)
      panic("send_sig failed");
}

/** Check IPC filter structure integrity. */
int check_ipc_filter(ipc_filter_t *f, int fill_flags) {
  if (!f)
    return OK;
  int flags = 0;
  for (int i = 0; i < f->num_elements; i++) {
    if (!IPCF_EL_CHECK(&f->elements[i]))
      return EINVAL;
    flags |= f->elements[i].flags;
  }
  if (fill_flags)
    f->flags = flags;
  else if (f->flags != flags)
    return EINVAL;
  return OK;
}

/**
 * @brief Determine whether a message passes the process's IPC filters.
 */
int allow_ipc_filtered_msg(struct proc *rp, endpoint_t src_e, vir_bytes m_src_v,
                           message *m_src_p) {
  ipc_filter_t *f = priv(rp)->s_ipcf;
  if (!f)
    return TRUE;
  message buf;
  if (!m_src_p) {
    int need_type = (f->flags & IPCF_MATCH_M_TYPE);
    if (need_type) {
      if (data_copy(src_e, m_src_v + K_OFFSETOF(message, m_type), KERNEL,
                    (vir_bytes)&buf.m_type, sizeof(buf.m_type)) != OK)
        return TRUE;
      m_src_p = &buf;
    } else {
      return TRUE;
    }
  }
  m_src_p->m_source = src_e;
  int allow = (f->type == IPCF_BLACKLIST);
  for (; f; f = f->next) {
    if ((f->type == IPCF_WHITELIST) ^ allow) {
      for (int i = 0; i < f->num_elements; i++) {
        if (IPCF_EL_MATCH(&f->elements[i], m_src_p)) {
          allow = (f->type == IPCF_WHITELIST);
          break;
        }
      }
    }
  }
  return allow;
}

/**
 * @brief Determine whether a VM memory request should be delivered.
 */
int allow_ipc_filtered_memreq(struct proc *src, struct proc *dst) {
  struct proc *vm = proc_addr(VM_PROC_NR);
  if (!priv(vm)->s_ipcf)
    return TRUE;
  message notify = {.m_type = NOTIFY_MESSAGE};
  if (!allow_ipc_filtered_msg(vm, SYSTEM, 0, &notify))
    return FALSE;
  return TRUE;
}

/*---------------------------------------------------------------------------*
 *  Privilege helpers                                                        *
 *---------------------------------------------------------------------------*/
int priv_add_irq(struct proc *rp, int irq) {
  struct priv *p = priv(rp);
  p->s_flags |= CHECK_IRQ;
  for (int i = 0; i < p->s_nr_irq; i++)
    if (p->s_irq_tab[i] == irq)
      return OK;
  if (p->s_nr_irq >= NR_IRQ) {
    kprintf_stub("do_privctl: %d has %d IRQs\n", rp->p_endpoint, p->s_nr_irq);
    return ENOMEM;
  }
  p->s_irq_tab[p->s_nr_irq++] = irq;
  return OK;
}

int priv_add_io(struct proc *rp, struct io_range *ior) {
  struct priv *p = priv(rp);
  p->s_flags |= CHECK_IO_PORT;
  for (int i = 0; i < p->s_nr_io_range; i++)
    if (p->s_io_tab[i].ior_base == ior->ior_base &&
        p->s_io_tab[i].ior_limit == ior->ior_limit)
      return OK;
  if (p->s_nr_io_range >= NR_IO_RANGE) {
    kprintf_stub("do_privctl: %d has %d I/O ranges\n", rp->p_endpoint,
                 p->s_nr_io_range);
    return ENOMEM;
  }
  p->s_io_tab[p->s_nr_io_range++] = *ior;
  return OK;
}

int priv_add_mem(struct proc *rp, struct minix_mem_range *mr) {
  struct priv *p = priv(rp);
  p->s_flags |= CHECK_MEM;
  for (int i = 0; i < p->s_nr_mem_range; i++)
    if (p->s_mem_tab[i].mr_base == mr->mr_base &&
        p->s_mem_tab[i].mr_limit == mr->mr_limit)
      return OK;
  if (p->s_nr_mem_range >= NR_MEM_RANGE) {
    kprintf_stub("do_privctl: %d has %d mem ranges\n", rp->p_endpoint,
                 p->s_nr_mem_range);
    return ENOMEM;
  }
  p->s_mem_tab[p->s_nr_mem_range++] = *mr;
  return OK;
}
