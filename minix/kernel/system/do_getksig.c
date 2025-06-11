/**
 * @file do_getksig.c
 * @brief SYS_GETKSIG kernel call: retrieve pending signals for a managed
 * process.
 *
 * This kernel call scans all user processes for one that:
 *  - Has its RTS_SIGNALED flag set (i.e., has pending signals).
 *  - Is managed by the caller (its s_sig_mgr matches the caller endpoint).
 *
 * For the first matching process:
 *  - Locks its p_sig_lock.
 *  - Copies its p_pending signal set into m_sigcalls.map.
 *  - Clears p_pending via k_sigemptyset().
 *  - Asserts the set is now empty.
 *  - Clears its RTS_SIGNALED flag.
 *  - Unlocks p_sig_lock.
 *  - Returns OK with m_sigcalls.endpt set to the target endpoint.
 *
 * If no such process is found:
 *  - Sets m_sigcalls.endpt = NONE.
 *  - Returns OK.
 *
 * @see RTS_ISSET, RTS_UNSET, k_sigemptyset
 *      spin_lock_irqsave, spin_unlock_irqrestore
 */

#include <klib/include/kmemory.h>
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <minix/com.h>         /* isemptyp() */
#include <minix/const.h>       /* BEG_USER_ADDR, END_PROC_ADDR */
#include <minix/endpoint.h>    /* endpoint_t, NONE */
#include <minix/kernel_types.h>/* k_sigset_t */
#include <stddef.h>            /* NULL */
#include <sys/kassert.h>       /* KASSERT */

#include "kernel/k_spinlock_irq.h" /* spinlock_irq_t, spin_lock_irqsave, spin_unlock_irqrestore */
#include "kernel/system.h"

#if USE_GETKSIG

/**
 * @brief Handle the SYS_GETKSIG kernel call.
 * @param caller  The signal manager process invoking this call.
 * @param m_ptr   The message structure:
 *                   - m_sigcalls.endpt : out endpoint of the target process
 *                   - m_sigcalls.map   : out bitmap of pending signals
 * @return Always returns OK.
 */
int do_getksig(struct proc *caller, message *m_ptr) {
  struct proc *rp;
  unsigned int flags;

  /* Scan all user processes for pending signals */
  for (rp = BEG_USER_ADDR; rp < END_PROC_ADDR; rp++) {
    if (isemptyp(rp)) {
      continue; /* Skip free slots */
    }
    if (!RTS_ISSET(rp, RTS_SIGNALED)) {
      continue; /* No pending signals */
    }

    /* Only the registered signal manager may retrieve signals */
    KASSERT(caller->p_endpoint == priv(rp)->s_sig_mgr,
            "do_getksig: caller %d not s_sig_mgr %d for ep %d",
            caller->p_endpoint, priv(rp)->s_sig_mgr, rp->p_endpoint);
    if (caller->p_endpoint != priv(rp)->s_sig_mgr) {
      continue;
    }

    /* Acquire the target's signal lock */
    flags = spin_lock_irqsave(&rp->p_sig_lock);

    /* Return the signaled process and its pending signals */
    m_ptr->m_sigcalls.endpt = rp->p_endpoint;
    m_ptr->m_sigcalls.map = rp->p_pending;

    /* Clear the pending set in the kernel */
    k_sigemptyset(&rp->p_pending);
    KASSERT(k_sigisempty(&rp->p_pending),
            "do_getksig: p_pending not empty for proc %d/ep %d", proc_nr(rp),
            rp->p_endpoint);

    /* Clear the signaled flag */
    RTS_UNSET(rp, RTS_SIGNALED);

    /* Release the lock */
    spin_unlock_irqrestore(&rp->p_sig_lock, flags);

    return OK;
  }

  /* No process had pending signals for this manager */
  m_ptr->m_sigcalls.endpt = NONE;
  return OK;
}

#endif /* USE_GETKSIG */
