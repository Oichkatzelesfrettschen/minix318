/**
 * Handle the SYS_ABORT kernel call to initiate system shutdown.
 *
 * This function processes abort requests that indicate MINIX is unable to
 * continue normal operation. The abort can originate from various sources
 * such as the Process Manager (PM) for normal abort conditions or the
 * TTY driver (e.g., after CTRL-ALT-DEL key sequence).
 *
 * @param caller Pointer to the process structure of the calling process
 *               that initiated the abort request
 * @param m_ptr  Pointer to the message structure containing abort parameters.
 *               The field m_lsys_krn_sys_abort.how specifies the abort mode
 *               and shutdown behavior
 *
 * @return Always returns EDISASTER to indicate catastrophic system failure
 *         requiring immediate shutdown
 *
 * @note This function is only compiled when USE_ABORT is defined
 * @see prepare_shutdown() for the actual shutdown preparation logic
 */
/* The kernel call implemented in this file:
 *   m_type:	SYS_ABORT
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_abort.how 	(how to abort, possibly fetch monitor params)
 */

#include "kernel/system.h"
// #include <unistd.h> // Removed because the required functionality is now
// provided by other kernel headers.
// Included kernel headers for necessary functionality:
// - <minix/kernel_types.h>: Provides definitions for kernel-specific types.
// - <klib/include/kprintf.h>: Enables kernel-level formatted printing for
// debugging.
// - <klib/include/kstring.h>: Provides string manipulation utilities used in
// kernel operations.
// - <klib/include/kmemory.h>: Offers memory management functions required for
// kernel tasks.
#include <klib/include/kmemory.h>
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <minix/kernel_types.h>
/*===========================================================================*
 *				do_abort				     *
 *===========================================================================*/
/**
 * Handle the SYS_ABORT kernel call, which indicates that MINIX is unable to
 * continue operation. This function prepares the system for shutdown.
 *
 * @param caller Pointer to the process structure of the calling process.
 * @param m_ptr  Pointer to the message structure containing the abort details.
 *               Specifically, m_lsys_krn_sys_abort.how indicates how to abort.
  int abort_mode = m_ptr->m_lsys_krn_sys_abort.how;

  /* Now prepare to shutdown MINIX. */
prepare_shutdown(abort_mode);
#if USE_ABORT

/*===========================================================================*
 *				do_abort				     *
 *===========================================================================*/
int do_abort(struct proc *caller, message *m_ptr) {
  /* Handle sys_abort. MINIX is unable to continue. This can originate e.g.
   * in the PM (normal abort) or TTY (after CTRL-ALT-DEL).
   */
  int how = m_ptr->m_lsys_krn_sys_abort.how;

  /* Now prepare to shutdown MINIX. */
  prepare_shutdown(how);
  return (EDISASTER); /* Return EDISASTER as indicated by the comment */
}

#endif /* USE_ABORT */ /* End of conditional compilation for USE_ABORT */
