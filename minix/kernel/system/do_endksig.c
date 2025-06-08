/* The kernel call that is implemented in this file:
 *	m_type: SYS_ENDKSIG
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt	# process for which PM is done
 */

#include "kernel/system.h"

// Added kernel headers
#include <minix/kernel_types.h> // For k_errno_t or similar if error codes are mapped
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <klib/include/kmemory.h>


#if USE_ENDKSIG 

/**
 * @brief Handle the SYS_ENDKSIG kernel call.
 * @param caller Pointer to the calling process structure (the signal manager).
 * @param m_ptr  Pointer to the message containing the endpoint of the process
 *               for which signal processing is ending.
 * @return OK if successful, or an error code if the target process is invalid,
 *         the caller is not the authorized signal manager, or the process
 *         was not in a signal-pending state.
 *
 * This function is called by a signal manager (typically PM) to indicate that
 * it has finished processing signals for a given target process.
 * It checks if new signals have arrived for the target process (RTS_SIGNALED is set).
 * If no new signals have arrived, it clears the RTS_SIG_PENDING flag, indicating
 * the process is no longer considered to be actively undergoing signal processing by PM.
 * If new signals *have* arrived, RTS_SIG_PENDING remains set (as cause_sig would
 * have set it again and notified PM), and PM is expected to call do_getksig again.
 * Operations on RTS flags are protected by the target process's p_sig_lock.
 */
int do_endksig(struct proc * caller, message * m_ptr)
{
  register struct proc *rp;
  int proc_nr;
  int flags; /* For spin_lock_irqsave */

  /* Get process pointer and verify that it had signals pending. If the 
   * process is already dead its flags will be reset. 
   */
  if(!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr))
	return EINVAL; // EINVAL might be undefined
	return EINVAL; // EINVAL might be undefined

  rp = proc_addr(proc_nr);
  if (caller->p_endpoint != priv(rp)->s_sig_mgr) return(EPERM); // EPERM might be undefined
  if (!RTS_ISSET(rp, RTS_SIG_PENDING)) return(EINVAL); // EINVAL might be undefined

  /* The signal manager has finished one kernel signal. Is the process ready? */
  /* If no new signal has arrived (RTS_SIGNALED is clear), then clear SIG_PENDING. */
  if (!RTS_ISSET(rp, RTS_SIGNALED)) {
	RTS_UNSET(rp, RTS_SIG_PENDING);	/* remove pending flag */
  }
  /* If RTS_SIGNALED is set, it means a new signal arrived after do_getksig
   * cleared it. cause_sig would have set RTS_SIG_PENDING again and sent a
   * notification. So, do_endksig doesn't need to clear RTS_SIG_PENDING here,
   * nor does it need to re-notify.
   */
  spin_unlock_irqrestore(&rp->p_sig_lock, flags);
  return(OK);
}

#endif /* USE_ENDKSIG */
