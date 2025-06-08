/**
 * @file mathematical_posix.c
 * @brief Implements POSIX-like functions that use mathematical capabilities.
 *
 * This file contains the libc wrappers for system calls that interact with
 * the mathematical capability system in the kernel.
 */

#include <errno.h>  // For errno and error codes
#include <fcntl.h>  // For O_RDONLY, O_WRONLY, O_RDWR, O_CREAT, etc. (and potentially O_MATHEMATICAL)
#include <lib.h>    // For _syscall definition
#include <minix/com.h>  // For SYS_MATH_OPEN, SYSTEM endpoint
#include <minix/ipc.h>  // For message type
#include <minix/vfsif.h>  // For VFS message structures like mess_lc_vfs_open / m_lc_vfs_open
#include <stdarg.h>  // For variadic arguments if mode is passed that way (like open)
#include <string.h>  // For strlen, memset

// If O_MATHEMATICAL is a custom flag, it should be defined, typically in
// <fcntl.h>. For this implementation, we assume it's defined. If not, it might
// be: #ifndef O_MATHEMATICAL #define O_MATHEMATICAL 0x800000 // Example value,
// ensure it's unique and fits #endif

/**
 * @brief Opens a file using mathematical capability checks.
 *
 * This function is the libc wrapper that prepares and executes the
 * SYS_MATH_OPEN system call to the SYSTEM task (kernel). It is intended
 * to be used when file access needs to be mediated by the mathematical
 * capability DAG.
 *
 * The behavior is similar to open(2), but with capability checks enforced
 * directly by the kernel through the SYS_MATH_OPEN call.
 *
 * @param path The pathname of the file to open.
 * @param flags The file access flags (e.g., O_RDONLY, O_WRONLY, O_CREAT).
 *              If O_CREAT is specified, the mode argument is also required.
 * @param ... Optional: mode_t mode, required if O_CREAT is in flags.
 * @return File descriptor on success, -1 on error (errno is set).
 */
int mathematical_open(const char *path, int flags, ...) {
  message m;
  mode_t mode = 0;

  if (path == NULL) {
    errno = EINVAL;
    return -1;
  }

  // Handle variadic argument for mode, similar to open(2)
  if (flags & O_CREAT) {
    va_list argp;
    va_start(argp, flags);
    mode = va_arg(argp, mode_t);
    va_end(argp);
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_MATH_OPEN;

  // Populate message fields for the kernel call.
  // These fields must match what do_open_mathematical in the kernel expects.
  // Assumes m_lc_vfs_open is the correct structure for VFS-like open calls.
  m.m_lc_vfs_open.name = (vir_bytes)path;
  m.m_lc_vfs_open.namelen = strlen(path) + 1;  // Include null terminator
  m.m_lc_vfs_open.flags = flags;  // Pass all flags, kernel might interpret
                                  // O_MATHEMATICAL or specific subsets
  m.m_lc_vfs_open.mode = mode;

  // Make the system call to the KERNEL (SYSTEM task)
  if (_syscall(SYSTEM, &m) < 0) {
    // errno is usually set by _syscall on error
    return -1;
  }

  // The return value from _syscall for a successful open is typically the file
  // descriptor. This is placed in m.m_type or a specific reply field by the
  // kernel. Assuming the kernel's do_open_mathematical returns fd in m.m_type
  // or a dedicated reply field. For many MINIX syscalls, the reply value is
  // m.m_type if it's non-negative, or m_type is the error code if negative.
  // _syscall handles this. If fd is in another field, e.g. m.m_vfs_lc_open.fd,
  // adjust here. Let's assume fd is returned in m_type for simplicity, or
  // _syscall handles it. The default for _syscall is to return m.m_type.

  // If SYS_MATH_OPEN returns fd in a specific field (e.g. m_res_vfs_open.fd),
  // this would be: return m.m_vfs_lc_open_reply.fd; // Check exact reply
  // message structure. For now, assume _syscall returns the fd directly (which
  // it does if kernel puts it in m.m_type).
  return m.m_type;  // This is typical if fd is returned in m_type by kernel.
                    // Or if _syscall itself returns m_type.
                    // Let's assume standard _syscall behavior where positive
                    // m.m_type is return value.
}
