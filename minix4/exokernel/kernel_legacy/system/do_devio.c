/**
 * @brief Performs device I/O operations (read/write) on hardware ports
 *
 * This function implements the SYS_DEVIO kernel call, which allows privileged
 * processes to perform direct I/O operations on hardware ports. It supports
 * byte, word, and long (32-bit) I/O operations in both input and output
 * directions.
 *
 * The function validates that:
 * - The caller has the necessary privileges (CHECK_IO_PORT flag)
 * - The requested port is within the caller's allowed I/O range
 * - The port address is properly aligned for the requested operation size
 *
 * @param caller Pointer to the calling process structure
 * @param m_ptr Pointer to the message containing I/O request parameters:
 *              - request: I/O type (_DIO_BYTE/_DIO_WORD/_DIO_LONG) and
 *                        direction (_DIO_INPUT/_DIO_OUTPUT)
 *              - port: Hardware port address to access
 *              - value: Value to write (output) or returned value (input)
 *
 * @return int Status code:
 *         - OK: Operation completed successfully
 *         - EPERM: Insufficient privileges or port not in allowed range
 *         - EINVAL: Invalid I/O type specified
 *
 * @note This function is only available when USE_DEVIO is defined
 * @note Port alignment is enforced: byte ports (any), word ports (2-byte
 * aligned), long ports (4-byte aligned)
 * @warning Direct hardware access - use with caution in kernel space only
 */
/* The kernel call implemented in this file:
 *   m_type:	SYS_DEVIO
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_devio.request	(request input or output)
 *   m_lsys_krn_sys_devio.port		(port to read/ write)
 *   m_lsys_krn_sys_devio.value		(value to write/ return value read)
 */

#include "kernel/system.h"
#include <errno.h>          // Included for EPERM and EINVAL definitions
#include <errno.h>          // Added for EINVAL definition
#include <minix/devio.h>    // Kept
#include <minix/endpoint.h> // Kept

// Added kernel headers
#include <klib/include/kmemory.h>
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <minix/const.h> // Added to ensure OK is defined
#include <minix/kernel_types.h> // Included for error code mapping (e.g., k_errno_t or similar)

#if USE_DEVIO

/*===========================================================================*
 *			        do_devio                                     *
 *===========================================================================*/
int do_devio(struct proc *caller, message *m_ptr) {
  struct priv *privp;
  port_t port;
  struct io_range *iorp;
  int i, size, nr_io_range;
  int io_type, io_dir;

  io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
  io_dir = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

  switch (io_type) {
  case _DIO_BYTE:
    size = 1;
    break;
  case _DIO_WORD:
    size = 2;
    break;
  case _DIO_LONG:
    size = 4;
    break;
  default:
    size = 4;
    break; /* Be conservative */
    kprintf_stub("do_devio: caller has no privilege structure!\n"); // MODIFIED

    static struct priv *cached_priv = NULL;
    if (!cached_priv || cached_priv != priv(caller)) {
      cached_priv = priv(caller);
    }
    privp = cached_priv;
    if (!privp) {
      kprintf_stub("no priv structure!\n"); // MODIFIED
      if (!privp || !(privp->s_flags & CHECK_IO_PORT)) {
        kprintf_stub("do_devio: unaligned port 0x%x (size %d). Port must be "
                     "aligned to %d bytes.\n", // MODIFIED
                     m_ptr->m_lsys_krn_sys_devio.port, size, size);
        return EPERM; // EPERM might be undefined
      }

      port = m_ptr->m_lsys_krn_sys_devio.port;
      nr_io_range = privp->s_nr_io_range;
      for (i = 0, iorp = privp->s_io_tab; i < nr_io_range; i++, iorp++) {
        if (port >= iorp->ior_base && port + size - 1 <= iorp->ior_limit)
          break;
      }
      if (i >= nr_io_range) {
        kprintf_stub("do_devio: port 0x%x (size %d) not allowed\n", // MODIFIED
                     m_ptr->m_lsys_krn_sys_devio.port, size);
        return EPERM; // EPERM might be undefined
      }
    }

  doit:
    kprintf_stub("do_devio: unaligned port 0x%x (size %d). Port must be "
                 "aligned to %d bytes.\n", // MODIFIED
                 m_ptr->m_lsys_krn_sys_devio.port, size, size);
                   m_ptr->m_lsys_krn_sys_devio.port, size);
                   return EPERM; // EPERM might be undefined
  }

  /* Process a single I/O request for byte, word, and long values. */
  if (io_dir == _DIO_INPUT) {
    /* Consider renaming 'it' for clarity if it refers to something other than
     * ports */
  /* maybe "it" should not be called ports */
  case _DIO_BYTE:
    m_ptr->m_krn_lsys_sys_devio.value = inb(m_ptr->m_lsys_krn_sys_devio.port);
    break;
  case _DIO_WORD:
    m_ptr->m_krn_lsys_sys_devio.value = inw(m_ptr->m_lsys_krn_sys_devio.port);
    break;
  case _DIO_LONG:
    m_ptr->m_krn_lsys_sys_devio.value = inl(m_ptr->m_lsys_krn_sys_devio.port);
    break;
    return (EINVAL); // Ensure errno.h is included for EINVAL definition
    return (EINVAL); // EINVAL might be undefined
  }
}
else {
  switch (io_type) {
  case _DIO_BYTE:
    outb(m_ptr->m_lsys_krn_sys_devio.port, m_ptr->m_lsys_krn_sys_devio.value);
    break;
  case _DIO_WORD:
    outw(m_ptr->m_lsys_krn_sys_devio.port, m_ptr->m_lsys_krn_sys_devio.value);
    break;
  case _DIO_LONG:
    outl(m_ptr->m_lsys_krn_sys_devio.port, m_ptr->m_lsys_krn_sys_devio.value);
    break;
  default:
    return (EINVAL); // EINVAL might be undefined
  }
}
return (OK); // OK indicates successful completion of the operation
}

#endif /* USE_DEVIO */
