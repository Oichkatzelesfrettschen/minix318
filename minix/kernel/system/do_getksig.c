/* The kernel call that is implemented in this file:
 *	m_type: SYS_GETKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process with pending signals
 *	m_sigcalls.map		# bit map with pending signals
 */

#include "kernel/system.h"
// #include <signal.h> // Replaced
#include <minix/endpoint.h> // Kept

// Added kernel headers
#include <minix/kernel_types.h> // For k_sigset_t
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <klib/include/kmemory.h>
#include "kernel/k_spinlock_irq.h" /* For spinlock_irq_t and IRQ-safe functions */


#if USE_GETKSIG

/**
 * @brief Handle the SYS_GETKSIG kernel call.
 * @param caller Pointer to the calling process structure (the signal manager).
 * @param m_ptr  Pointer to the message containing call parameters and for result.
 * @return OK if a signal was retrieved or no signals are pending,
 *         otherwise an error code.
 *
 * This function is called by a signal manager (typically PM) to retrieve
 * pending signals for processes it manages. It iterates through user processes,
 * looking for one with the RTS_SIGNALED flag set. If found, and if the caller
 * is the authorized signal manager for that process, the pending signal map
 * (p_pending) is copied to the caller's message, p_pending is cleared,
 * and RTS_SIGNALED is unset for the target process. These operations are
 * protected by the target process's p_sig_lock.
 * If no process has pending signals for this manager, m_sigcalls.endpt is set to NONE.
 */
int do_getksig(struct proc * caller, message * m_ptr)
{
  register struct proc *rp;
  int flags; /* For spin_lock_irqsave */

  /* Find the next process with pending signals. */
  for (rp = BEG_USER_ADDR; rp < END_PROC_ADDR; rp++) {
      if (isemptyp(rp)) continue; /* Skip empty slots */
      if (RTS_ISSET(rp, RTS_SIGNALED)) {
          /* KASSERT: Verify caller is the designated signal manager for this process.
           * This is a critical security and correctness check (P3). Only the
           * registered signal manager (s_sig_mgr, usually PM) for a target process
           * should be able to retrieve and thus act upon its pending signals.
           * Allowing unauthorized processes to get signals could lead to information
           * leaks or incorrect signal handling.
           */
          KASSERT(caller->p_endpoint == priv(rp)->s_sig_mgr,
                  "do_getksig: caller %d is not signal manager %d for target %d",
                  caller->p_endpoint, priv(rp)->s_sig_mgr, rp->p_endpoint);
          if (caller->p_endpoint != priv(rp)->s_sig_mgr) continue;

          flags = spin_lock_irqsave(&rp->p_sig_lock);
	  /* store signaled process' endpoint */
          m_ptr->m_sigcalls.endpt = rp->p_endpoint;
          m_ptr->m_sigcalls.map = rp->p_pending;	/* pending signals map */
          k_sigemptyset(&rp->p_pending); 	/* clear map in the kernel */
          /* KASSERT: Ensure the process's pending signal set (p_pending) is empty
           * after k_sigemptyset(). This verifies the correctness of k_sigemptyset (P4)
           * and ensures that no signals are inadvertently lost or left unprocessed
           * from this batch. The signal manager is now responsible for all signals
           * that were in 'map'.
           */
          KASSERT(k_sigisempty(&rp->p_pending),
                  "do_getksig: signal set p_pending for proc %d (ep %d) not properly cleared by k_sigemptyset",
                  proc_nr(rp), rp->p_endpoint);
	  /* The RTS_UNSET macro itself should be SMP-safe or be called
	   * while holding the appropriate lock.
	   */
	  RTS_UNSET(rp, RTS_SIGNALED);		/* remove signaled flag */
          spin_unlock_irqrestore(&rp->p_sig_lock, flags);
          return(OK);
      }
  }

  /* No process with pending signals was found. */
  m_ptr->m_sigcalls.endpt = NONE;
  return(OK);
}
#endif /* USE_GETKSIG */
