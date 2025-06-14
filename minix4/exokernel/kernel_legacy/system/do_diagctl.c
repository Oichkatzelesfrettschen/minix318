/**
 * @brief Handle diagnostic control system calls
 *
 * Implements the SYS_DIAGCTL kernel call which provides diagnostic
 * functionality including output, stack tracing, and diagnostic signal
 * registration.
 *
 * @param caller Pointer to the calling process structure
 * @param m_ptr Pointer to the message containing the request parameters:
 *              - m_lsys_krn_sys_diagctl.code: The diagnostic operation code
 *              - m_lsys_krn_sys_diagctl.buf: Buffer address for data operations
 *              - m_lsys_krn_sys_diagctl.len: Length of data buffer
 *              - m_lsys_krn_sys_diagctl.endpt: Target endpoint for operations
 *
 * Supported diagnostic codes:
 * - DIAGCTL_CODE_DIAG: Output diagnostic data to kernel console
 * - DIAGCTL_CODE_STACKTRACE: Print stack trace for specified process
 * - DIAGCTL_CODE_REGISTER: Register caller for diagnostic signal notifications
 * - DIAGCTL_CODE_UNREGISTER: Unregister caller from diagnostic notifications
 *
 * @return OK on success, or appropriate error code:
 *         - EINVAL: Invalid parameters (length out of range, invalid endpoint,
 * unknown code)
 *         - EPERM: Permission denied (caller lacks SYS_PROC privileges)
 *         - Other: Error from data_copy_vmcheck operation
 *
 * @note For DIAGCTL_CODE_DIAG, data is copied from user space and output
 *       character by character to the kernel console, terminated with
 * END_OF_KMESS
 * @note Registration operations require SYS_PROC privileges and manage
 *       the s_diag_sig flag for diagnostic signal delivery
 */
/* The kernel call implemented in this file:
 *   m_type:	SYS_DIAGCTL
 *
 * The parameters for this kernel call are:
 * 	m_lsys_krn_sys_diagctl.code	request
 * and then request-specific arguments in
 *	m_lsys_krn_sys_diagctl.buf
 *	m_lsys_krn_sys_diagctl.len
 *	m_lsys_krn_sys_diagctl.endpt
 */

#include "kernel/system.h"

// Added kernel headers
#include <klib/include/kmemory.h>
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <minix/kernel_types.h> // For k_errno_t or similar if error codes are mapped, and k_sigset_t for SIGKMESS

/*===========================================================================*
 *			        do_diagctl				     *
 *===========================================================================*/
int do_diagctl(struct proc *caller, message *m_ptr) {
  vir_bytes len, buf;
  static char mybuf[DIAG_BUFSIZE];
  int s, i, proc_nr;

  switch (m_ptr->m_lsys_krn_sys_diagctl.code) {
  case DIAGCTL_CODE_DIAG:
    buf = m_ptr->m_lsys_krn_sys_diagctl.buf;
    len = m_ptr->m_lsys_krn_sys_diagctl.len;
    if (len < 1 || len > DIAG_BUFSIZE) {
      kprintf_stub("do_diagctl: diag for %d: len %d out of range\n", // MODIFIED
                   caller->p_endpoint, len);
      return EINVAL; // EINVAL might be undefined
    }
    if ((s = data_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
                               (vir_bytes)mybuf, len)) != OK) {
      kprintf_stub(
          "do_diagctl: diag for %d: len %d: copy failed: %d\n", // MODIFIED
          caller->p_endpoint, len, s);
      return s;
    }
    for (i = 0; i < len; i++)
      kputc(mybuf[i]);
    kputc(END_OF_KMESS);
    return OK;
  case DIAGCTL_CODE_STACKTRACE:
    if (!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
      return EINVAL; // EINVAL might be undefined
    proc_stacktrace(proc_addr(proc_nr));
    return OK;
  case DIAGCTL_CODE_REGISTER:
    if (!(priv(caller)->s_flags & SYS_PROC))
      return EPERM; // EPERM might be undefined
    priv(caller)->s_diag_sig = TRUE;
    /* If the message log is not empty, send a first notification
     * immediately. After bootup the log is basically never empty.
     */
    if (kmess.km_size > 0 && !kinfo.do_serial_debug)
      send_sig(caller->p_endpoint, SIGKMESS); // SIGKMESS might be undefined
    return OK;
  case DIAGCTL_CODE_UNREGISTER:
    if (!(priv(caller)->s_flags & SYS_PROC))
      return EPERM; // EPERM might be undefined
    priv(caller)->s_diag_sig = FALSE;
    return OK;
  default:
    kprintf_stub("do_diagctl: invalid request %d\n",
                 m_ptr->m_lsys_krn_sys_diagctl.code); // MODIFIED
    return (EINVAL); // EINVAL might be undefined
  }
}
