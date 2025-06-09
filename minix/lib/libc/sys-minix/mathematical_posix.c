#include <errno.h>
#include <stdarg.h>
#include <stdlib.h> // For getenv, NULL
#include <string.h> // For memset
#include <time.h>   // For time_t

// Minix headers
#include <minix/callnr.h>
#include <minix/com.h>
#include <minix/fcntl.h> // For O_CREAT, O_RDWR, O_MATHEMATICAL etc.
#include <minix/ipc.h>
#include <sys/types.h>

// Internal libc header for shared math capability state and functions
#include "mathematical_internal.h" // Provides __math_state, __mathematical_init, __should_use_mathematical

// Project-specific public header
#include <minix/capability.h>

// Assumed to provide _syscall and possibly common message setup utilities
#include <lib.h>

/* --- Traditional open (fallback) --- */
static int _open_traditional(const char *path, int flags, mode_t mode) {
  message m;
  memset(&m, 0, sizeof(m));
  m.m_type = VFS_OPEN;
  m.m_lc_vfs_path = (vir_bytes)path;
  m.m_lc_vfs_flags = flags;
  m.m_lc_vfs_mode = mode;
  return _syscall(VFS_PROC_NR, VFS_OPEN, &m);
}

/* --- Internal implementation for mathematical open --- */
static int mathematical_open_internal(const char *path, int flags,
                                      mode_t parsed_mode) {
  message m;

  // This function is called after __should_use_mathematical() has confirmed we
  // should proceed and potentially after __math_state.mode ==
  // MATH_MODE_MANDATORY check with kernel_support. If kernel doesn't support it
  // AND mode is mandatory, open() or mathematical_open() should prevent calling
  // this.
  if (__math_state.mode == MATH_MODE_MANDATORY &&
      !__math_state.kernel_support) {
    errno = ENOSYS; // Kernel doesn't support the feature, and it's mandatory.
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = VFS_OPEN;
  m.m_lc_vfs_path = (vir_bytes)path;
  m.m_lc_vfs_flags = flags | O_MATHEMATICAL;
  m.m_lc_vfs_mode = parsed_mode;
  return _syscall(VFS_PROC_NR, VFS_OPEN, &m);
}

/* --- Overridden open() --- */
int open(const char *path, int flags, ...) {
  mode_t mode = 0;
  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, mode_t);
    va_end(ap);
  }

  if (!__math_state.initialized) {
    __mathematical_init();
  }

  if (__should_use_mathematical(flags)) {
    // If mandatory and not supported by kernel, this call should fail.
    if (__math_state.mode == MATH_MODE_MANDATORY &&
        !__math_state.kernel_support) {
      errno = ENOSYS; // Or EACCES, depending on desired behavior for mandatory
                      // failure
      return -1;
    }
    return mathematical_open_internal(path, flags, mode);
  }
  return _open_traditional(path, flags, mode);
}

/* --- Explicit mathematical_open() --- */
int mathematical_open(const char *path, int flags, ...) {
  mode_t mode = 0;
  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, mode_t);
    va_end(ap);
  }

  if (!__math_state.initialized) {
    __mathematical_init();
  }

  // If disabled, this explicit call should probably still try, or error.
  // If mandatory and not supported, it must error.
  if (__math_state.mode == MATH_MODE_DISABLED &&
      !(__math_state.policy & POLICY_MANDATORY_ALL)) {
    // If not system-wide mandatory, and current mode is disabled, this explicit
    // call could be disallowed. However, an explicit call usually means "I want
    // this feature". For now, let it proceed if kernel supports it, respecting
    // MANDATORY.
  }

  if (__math_state.mode == MATH_MODE_MANDATORY &&
      !__math_state.kernel_support) {
    errno = ENOSYS;
    return -1;
  }
  if (!__math_state.kernel_support && __math_state.mode != MATH_MODE_DISABLED) {
    // If any math mode is on but kernel doesn't support, explicit calls should
    // fail.
    errno = ENOSYS;
    return -1;
  }

  return mathematical_open_internal(path, flags, mode);
}

/* --- Implementations for other mathematical_ functions from mathposix.h --- */
ssize_t mathematical_read(int fd, void *buf, size_t count) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES; // Or ENOSYS: feature explicitly disabled for this call
    return -1;
  }
  if (!__math_state
           .kernel_support) { // Mandatory or Enabled but no kernel support
    errno = ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_MATH_READ;
  m.m_lc_vfs_fd = fd;
  m.m_lc_vfs_buf = (vir_bytes)buf;
  m.m_lc_vfs_len = count;
  return _syscall(SYSTEM, SYS_MATH_READ, &m);
}

ssize_t mathematical_write(int fd, const void *buf, size_t count) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES;
    return -1;
  }
  if (!__math_state.kernel_support) {
    errno = ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_MATH_WRITE;
  m.m_lc_vfs_fd = fd;
  m.m_lc_vfs_buf = (vir_bytes)buf;
  m.m_lc_vfs_len = count;
  return _syscall(SYSTEM, SYS_MATH_WRITE, &m);
}

int mathematical_close(int fd) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  // Close might be special: even if math mode is off, if it was opened with
  // math, the kernel might need to do special cleanup via SYS_MATH_CLOSE. For
  // now, if math mode is generally off, or no kernel support, let it be a
  // normal close. This assumes that a standard close would be sufficient if
  // math features weren't used. The kernel handler for SYS_MATH_CLOSE should be
  // idempotent or handle non-math FDs gracefully.
  if (__math_state.mode == MATH_MODE_DISABLED || !__math_state.kernel_support) {
    // Fallback to standard close if math features are off or not supported.
    // Standard close is just 'close(fd)' which itself is a syscall.
    // This file is providing mathematical_close, so it should perhaps do its
    // own logic or fail. For now, if disabled or not supported, make
    // SYS_MATH_CLOSE fail.
    errno = (__math_state.mode == MATH_MODE_DISABLED) ? EACCES : ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_MATH_CLOSE;
  m.m_lc_vfs_fd = fd;
  return _syscall(SYSTEM, SYS_MATH_CLOSE, &m);
}

int mathematical_derive(int parent_fd, int new_rights) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES;
    return -1;
  }
  if (!__math_state.kernel_support) {
    errno = ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_CAP_DERIVE;
  m.m1_i1 = parent_fd;
  m.m1_i2 = new_rights;
  return _syscall(SYSTEM, SYS_CAP_DERIVE, &m);
}

int mathematical_open_with_expiry(const char *path, int flags, mode_t mode,
                                  time_t expiry) {
  // message m; // For when SYS_MATH_OPEN_EXPIRY is defined and handled

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES;
    return -1;
  }
  if (!__math_state.kernel_support) { // Or if SYS_MATH_OPEN_EXPIRY specifically
                                      // isn't supported
    errno = ENOSYS;
    return -1;
  }

  // TODO: Implement with a new SYS_MATH_OPEN_EXPIRY call
  // memset(&m, 0, sizeof(m));
  // m.m_type = SYS_MATH_OPEN_EXPIRY;
  // m.m_lc_vfs_path = (vir_bytes)path;
  // m.m_lc_vfs_flags = flags | O_MATHEMATICAL; // Assuming expiry implies
  // mathematical m.m_lc_vfs_mode = mode; m.m2_l1 = expiry; // Example field for
  // expiry return _syscall(VFS_PROC_NR, SYS_MATH_OPEN_EXPIRY, &m); // Or SYSTEM
  // if direct kernel call

  (void)path;
  (void)flags;
  (void)mode;
  (void)expiry;   // Suppress unused warnings for now
  errno = ENOSYS; // Not implemented yet
  return -1;
}

// Functions that might be added to mathposix.h and implemented here or in
// math_control.c int mathematical_get_mode(void) {
//     if (!__math_state.initialized) __mathematical_init();
//     return __math_state.mode;
// }

// int mathematical_was_used(int fd) {
//     // This would require per-fd state, possibly via another syscall to query
//     fd properties (void)fd; if (!__math_state.initialized)
//     __mathematical_init();
//     // For now, assume if math mode is on, it was used. This is a
//     simplification. return (__math_state.mode != MATH_MODE_DISABLED &&
//     __math_state.kernel_support);
// }
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h> // For getenv, NULL
#include <string.h> // For memset
#include <time.h>   // For time_t

// Minix headers
#include <minix/callnr.h>
#include <minix/com.h>
#include <minix/fcntl.h> // For O_CREAT, O_RDWR, O_MATHEMATICAL etc.
#include <minix/ipc.h>
#include <sys/types.h>

// Internal libc header for shared math capability state and functions
#include "mathematical_internal.h" // Provides __math_state, __mathematical_init, __should_use_mathematical

// Project-specific public header
#include <minix/capability.h>

// Assumed to provide _syscall and possibly common message setup utilities
#include <lib.h>

/* --- Traditional open (fallback) --- */
static int _open_traditional(const char *path, int flags, mode_t mode) {
  message m;
  memset(&m, 0, sizeof(m));
  m.m_type = VFS_OPEN;
  m.m_lc_vfs_path = (vir_bytes)path;
  m.m_lc_vfs_flags = flags;
  m.m_lc_vfs_mode = mode;
  return _syscall(VFS_PROC_NR, VFS_OPEN, &m);
}

/* --- Internal implementation for mathematical open --- */
static int mathematical_open_internal(const char *path, int flags,
                                      mode_t parsed_mode) {
  message m;

  // This function is called after __should_use_mathematical() has confirmed we
  // should proceed and potentially after __math_state.mode ==
  // MATH_MODE_MANDATORY check with kernel_support. If kernel doesn't support it
  // AND mode is mandatory, open() or mathematical_open() should prevent calling
  // this.
  if (__math_state.mode == MATH_MODE_MANDATORY &&
      !__math_state.kernel_support) {
    errno = ENOSYS; // Kernel doesn't support the feature, and it's mandatory.
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = VFS_OPEN;
  m.m_lc_vfs_path = (vir_bytes)path;
  m.m_lc_vfs_flags = flags | O_MATHEMATICAL;
  m.m_lc_vfs_mode = parsed_mode;
  return _syscall(VFS_PROC_NR, VFS_OPEN, &m);
}

/* --- Overridden open() --- */
int open(const char *path, int flags, ...) {
  mode_t mode = 0;
  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, mode_t);
    va_end(ap);
  }

  if (!__math_state.initialized) {
    __mathematical_init();
  }

  if (__should_use_mathematical(flags)) {
    // If mandatory and not supported by kernel, this call should fail.
    if (__math_state.mode == MATH_MODE_MANDATORY &&
        !__math_state.kernel_support) {
      errno = ENOSYS; // Or EACCES, depending on desired behavior for mandatory
                      // failure
      return -1;
    }
    return mathematical_open_internal(path, flags, mode);
  }
  return _open_traditional(path, flags, mode);
}

/* --- Explicit mathematical_open() --- */
int mathematical_open(const char *path, int flags, ...) {
  mode_t mode = 0;
  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, mode_t);
    va_end(ap);
  }

  if (!__math_state.initialized) {
    __mathematical_init();
  }

  // If disabled, this explicit call should probably still try, or error.
  // If mandatory and not supported, it must error.
  if (__math_state.mode == MATH_MODE_DISABLED &&
      !(__math_state.policy & POLICY_MANDATORY_ALL)) {
    // If not system-wide mandatory, and current mode is disabled, this explicit
    // call could be disallowed. However, an explicit call usually means "I want
    // this feature". For now, let it proceed if kernel supports it, respecting
    // MANDATORY.
  }

  if (__math_state.mode == MATH_MODE_MANDATORY &&
      !__math_state.kernel_support) {
    errno = ENOSYS;
    return -1;
  }
  if (!__math_state.kernel_support && __math_state.mode != MATH_MODE_DISABLED) {
    // If any math mode is on but kernel doesn't support, explicit calls should
    // fail.
    errno = ENOSYS;
    return -1;
  }

  return mathematical_open_internal(path, flags, mode);
}

/* --- Implementations for other mathematical_ functions from mathposix.h --- */
ssize_t mathematical_read(int fd, void *buf, size_t count) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES; // Or ENOSYS: feature explicitly disabled for this call
    return -1;
  }
  if (!__math_state
           .kernel_support) { // Mandatory or Enabled but no kernel support
    errno = ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_MATH_READ;
  m.m_lc_vfs_fd = fd;
  m.m_lc_vfs_buf = (vir_bytes)buf;
  m.m_lc_vfs_len = count;
  return _syscall(SYSTEM, SYS_MATH_READ, &m);
}

ssize_t mathematical_write(int fd, const void *buf, size_t count) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES;
    return -1;
  }
  if (!__math_state.kernel_support) {
    errno = ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_MATH_WRITE;
  m.m_lc_vfs_fd = fd;
  m.m_lc_vfs_buf = (vir_bytes)buf;
  m.m_lc_vfs_len = count;
  return _syscall(SYSTEM, SYS_MATH_WRITE, &m);
}

int mathematical_close(int fd) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  // Close might be special: even if math mode is off, if it was opened with
  // math, the kernel might need to do special cleanup via SYS_MATH_CLOSE. For
  // now, if math mode is generally off, or no kernel support, let it be a
  // normal close. This assumes that a standard close would be sufficient if
  // math features weren't used. The kernel handler for SYS_MATH_CLOSE should be
  // idempotent or handle non-math FDs gracefully.
  if (__math_state.mode == MATH_MODE_DISABLED || !__math_state.kernel_support) {
    // Fallback to standard close if math features are off or not supported.
    // Standard close is just 'close(fd)' which itself is a syscall.
    // This file is providing mathematical_close, so it should perhaps do its
    // own logic or fail. For now, if disabled or not supported, make
    // SYS_MATH_CLOSE fail.
    errno = (__math_state.mode == MATH_MODE_DISABLED) ? EACCES : ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_MATH_CLOSE;
  m.m_lc_vfs_fd = fd;
  return _syscall(SYSTEM, SYS_MATH_CLOSE, &m);
}

int mathematical_derive(int parent_fd, int new_rights) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES;
    return -1;
  }
  if (!__math_state.kernel_support) {
    errno = ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_CAP_DERIVE;
  m.m1_i1 = parent_fd;
  m.m1_i2 = new_rights;
  return _syscall(SYSTEM, SYS_CAP_DERIVE, &m);
}

int mathematical_open_with_expiry(const char *path, int flags, mode_t mode,
                                  time_t expiry) {
  // message m; // For when SYS_MATH_OPEN_EXPIRY is defined and handled

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES;
    return -1;
  }
  if (!__math_state.kernel_support) { // Or if SYS_MATH_OPEN_EXPIRY specifically
                                      // isn't supported
    errno = ENOSYS;
    return -1;
  }

  // TODO: Implement with a new SYS_MATH_OPEN_EXPIRY call
  // memset(&m, 0, sizeof(m));
  // m.m_type = SYS_MATH_OPEN_EXPIRY;
  // m.m_lc_vfs_path = (vir_bytes)path;
  // m.m_lc_vfs_flags = flags | O_MATHEMATICAL; // Assuming expiry implies
  // mathematical m.m_lc_vfs_mode = mode; m.m2_l1 = expiry; // Example field for
  // expiry return _syscall(VFS_PROC_NR, SYS_MATH_OPEN_EXPIRY, &m); // Or SYSTEM
  // if direct kernel call

  (void)path;
  (void)flags;
  (void)mode;
  (void)expiry;   // Suppress unused warnings for now
  errno = ENOSYS; // Not implemented yet
  return -1;
}

// Functions that might be added to mathposix.h and implemented here or in
// math_control.c int mathematical_get_mode(void) {
//     if (!__math_state.initialized) __mathematical_init();
//     return __math_state.mode;
// }

// int mathematical_was_used(int fd) {
//     // This would require per-fd state, possibly via another syscall to query
//     fd properties (void)fd; if (!__math_state.initialized)
//     __mathematical_init();
//     // For now, assume if math mode is on, it was used. This is a
//     simplification. return (__math_state.mode != MATH_MODE_DISABLED &&
//     __math_state.kernel_support);
// }
/**
 * @file mathematical_posix.c
 * @brief Implements POSIX-like functions that use mathematical capabilities.
 *
 * This file contains the libc wrappers for system calls that interact with
 * the mathematical capability system in the kernel.
 */

#include <errno.h> // For errno and error codes
#include <fcntl.h> // For O_RDONLY, O_WRONLY, O_RDWR, O_CREAT, etc. (and potentially O_MATHEMATICAL)
#include <lib.h>   // For _syscall definition
#include <minix/com.h> // For SYS_MATH_OPEN, SYSTEM endpoint
#include <minix/ipc.h> // For message type
#include <minix/vfsif.h> // For VFS message structures like mess_lc_vfs_open / m_lc_vfs_open
#include <stdarg.h> // For variadic arguments if mode is passed that way (like open)
#include <string.h> // For strlen, memset

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
  m.m_lc_vfs_open.namelen = strlen(path) + 1; // Include null terminator
  m.m_lc_vfs_open.flags = flags; // Pass all flags, kernel might interpret
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
  return m.m_type; // This is typical if fd is returned in m_type by kernel.
                   // Or if _syscall itself returns m_type.
                   // Let's assume standard _syscall behavior where positive
                   // m.m_type is return value.
}
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h> // For getenv, NULL
#include <string.h> // For memset
#include <time.h>   // For time_t

// Minix headers
#include <minix/callnr.h>
#include <minix/com.h>
#include <minix/fcntl.h> // For O_CREAT, O_RDWR, O_MATHEMATICAL etc.
#include <minix/ipc.h>
#include <sys/types.h>

// Internal libc header for shared math capability state and functions
#include "mathematical_internal.h" // Provides __math_state, __mathematical_init, __should_use_mathematical

// Project-specific public header
#include <minix/capability.h>

// Assumed to provide _syscall and possibly common message setup utilities
#include <lib.h>

/* --- Traditional open (fallback) --- */
static int _open_traditional(const char *path, int flags, mode_t mode) {
  message m;
  memset(&m, 0, sizeof(m));
  m.m_type = VFS_OPEN;
  m.m_lc_vfs_path = (vir_bytes)path;
  m.m_lc_vfs_flags = flags;
  m.m_lc_vfs_mode = mode;
  return _syscall(VFS_PROC_NR, VFS_OPEN, &m);
}

/* --- Internal implementation for mathematical open --- */
static int mathematical_open_internal(const char *path, int flags,
                                      mode_t parsed_mode) {
  message m;

  // This function is called after __should_use_mathematical() has confirmed we
  // should proceed and potentially after __math_state.mode ==
  // MATH_MODE_MANDATORY check with kernel_support. If kernel doesn't support it
  // AND mode is mandatory, open() or mathematical_open() should prevent calling
  // this.
  if (__math_state.mode == MATH_MODE_MANDATORY &&
      !__math_state.kernel_support) {
    errno = ENOSYS; // Kernel doesn't support the feature, and it's mandatory.
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = VFS_OPEN;
  m.m_lc_vfs_path = (vir_bytes)path;
  m.m_lc_vfs_flags = flags | O_MATHEMATICAL;
  m.m_lc_vfs_mode = parsed_mode;
  return _syscall(VFS_PROC_NR, VFS_OPEN, &m);
}

/* --- Overridden open() --- */
int open(const char *path, int flags, ...) {
  mode_t mode = 0;
  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, mode_t);
    va_end(ap);
  }

  if (!__math_state.initialized) {
    __mathematical_init();
  }

  if (__should_use_mathematical(flags)) {
    // If mandatory and not supported by kernel, this call should fail.
    if (__math_state.mode == MATH_MODE_MANDATORY &&
        !__math_state.kernel_support) {
      errno = ENOSYS; // Or EACCES, depending on desired behavior for mandatory
                      // failure
      return -1;
    }
    return mathematical_open_internal(path, flags, mode);
  }
  return _open_traditional(path, flags, mode);
}

/* --- Explicit mathematical_open() --- */
int mathematical_open(const char *path, int flags, ...) {
  mode_t mode = 0;
  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, mode_t);
    va_end(ap);
  }

  if (!__math_state.initialized) {
    __mathematical_init();
  }

  // If disabled, this explicit call should probably still try, or error.
  // If mandatory and not supported, it must error.
  if (__math_state.mode == MATH_MODE_DISABLED &&
      !(__math_state.policy & POLICY_MANDATORY_ALL)) {
    // If not system-wide mandatory, and current mode is disabled, this explicit
    // call could be disallowed. However, an explicit call usually means "I want
    // this feature". For now, let it proceed if kernel supports it, respecting
    // MANDATORY.
  }

  if (__math_state.mode == MATH_MODE_MANDATORY &&
      !__math_state.kernel_support) {
    errno = ENOSYS;
    return -1;
  }
  if (!__math_state.kernel_support && __math_state.mode != MATH_MODE_DISABLED) {
    // If any math mode is on but kernel doesn't support, explicit calls should
    // fail.
    errno = ENOSYS;
    return -1;
  }

  return mathematical_open_internal(path, flags, mode);
}

/* --- Implementations for other mathematical_ functions from mathposix.h --- */
ssize_t mathematical_read(int fd, void *buf, size_t count) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES; // Or ENOSYS: feature explicitly disabled for this call
    return -1;
  }
  if (!__math_state
           .kernel_support) { // Mandatory or Enabled but no kernel support
    errno = ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_MATH_READ;
  m.m_lc_vfs_fd = fd;
  m.m_lc_vfs_buf = (vir_bytes)buf;
  m.m_lc_vfs_len = count;
  return _syscall(SYSTEM, SYS_MATH_READ, &m);
}

ssize_t mathematical_write(int fd, const void *buf, size_t count) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES;
    return -1;
  }
  if (!__math_state.kernel_support) {
    errno = ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_MATH_WRITE;
  m.m_lc_vfs_fd = fd;
  m.m_lc_vfs_buf = (vir_bytes)buf;
  m.m_lc_vfs_len = count;
  return _syscall(SYSTEM, SYS_MATH_WRITE, &m);
}

int mathematical_close(int fd) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  // Close might be special: even if math mode is off, if it was opened with
  // math, the kernel might need to do special cleanup via SYS_MATH_CLOSE. For
  // now, if math mode is generally off, or no kernel support, let it be a
  // normal close. This assumes that a standard close would be sufficient if
  // math features weren't used. The kernel handler for SYS_MATH_CLOSE should be
  // idempotent or handle non-math FDs gracefully.
  if (__math_state.mode == MATH_MODE_DISABLED || !__math_state.kernel_support) {
    // Fallback to standard close if math features are off or not supported.
    // Standard close is just 'close(fd)' which itself is a syscall.
    // This file is providing mathematical_close, so it should perhaps do its
    // own logic or fail. For now, if disabled or not supported, make
    // SYS_MATH_CLOSE fail.
    errno = (__math_state.mode == MATH_MODE_DISABLED) ? EACCES : ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_MATH_CLOSE;
  m.m_lc_vfs_fd = fd;
  return _syscall(SYSTEM, SYS_MATH_CLOSE, &m);
}

int mathematical_derive(int parent_fd, int new_rights) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES;
    return -1;
  }
  if (!__math_state.kernel_support) {
    errno = ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_CAP_DERIVE;
  m.m1_i1 = parent_fd;
  m.m1_i2 = new_rights;
  return _syscall(SYSTEM, SYS_CAP_DERIVE, &m);
}

int mathematical_open_with_expiry(const char *path, int flags, mode_t mode,
                                  time_t expiry) {
  // message m; // For when SYS_MATH_OPEN_EXPIRY is defined and handled

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES;
    return -1;
  }
  if (!__math_state.kernel_support) { // Or if SYS_MATH_OPEN_EXPIRY specifically
                                      // isn't supported
    errno = ENOSYS;
    return -1;
  }

  // TODO: Implement with a new SYS_MATH_OPEN_EXPIRY call
  // memset(&m, 0, sizeof(m));
  // m.m_type = SYS_MATH_OPEN_EXPIRY;
  // m.m_lc_vfs_path = (vir_bytes)path;
  // m.m_lc_vfs_flags = flags | O_MATHEMATICAL; // Assuming expiry implies
  // mathematical m.m_lc_vfs_mode = mode; m.m2_l1 = expiry; // Example field for
  // expiry return _syscall(VFS_PROC_NR, SYS_MATH_OPEN_EXPIRY, &m); // Or SYSTEM
  // if direct kernel call

  (void)path;
  (void)flags;
  (void)mode;
  (void)expiry;   // Suppress unused warnings for now
  errno = ENOSYS; // Not implemented yet
  return -1;
}

// Functions that might be added to mathposix.h and implemented here or in
// math_control.c int mathematical_get_mode(void) {
//     if (!__math_state.initialized) __mathematical_init();
//     return __math_state.mode;
// }

// int mathematical_was_used(int fd) {
//     // This would require per-fd state, possibly via another syscall to query
//     fd properties (void)fd; if (!__math_state.initialized)
//     __mathematical_init();
//     // For now, assume if math mode is on, it was used. This is a
//     simplification. return (__math_state.mode != MATH_MODE_DISABLED &&
//     __math_state.kernel_support);
// }
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h> // For getenv, NULL
#include <string.h> // For memset
#include <time.h>   // For time_t

// Minix headers
#include <minix/callnr.h>
#include <minix/com.h>
#include <minix/fcntl.h> // For O_CREAT, O_RDWR, O_MATHEMATICAL etc.
#include <minix/ipc.h>
#include <sys/types.h>

// Internal libc header for shared math capability state and functions
#include "mathematical_internal.h" // Provides __math_state, __mathematical_init, __should_use_mathematical

// Project-specific public header
#include <minix/capability.h>

// Assumed to provide _syscall and possibly common message setup utilities
#include <lib.h>

/* --- Traditional open (fallback) --- */
static int _open_traditional(const char *path, int flags, mode_t mode) {
  message m;
  memset(&m, 0, sizeof(m));
  m.m_type = VFS_OPEN;
  m.m_lc_vfs_path = (vir_bytes)path;
  m.m_lc_vfs_flags = flags;
  m.m_lc_vfs_mode = mode;
  return _syscall(VFS_PROC_NR, VFS_OPEN, &m);
}

/* --- Internal implementation for mathematical open --- */
static int mathematical_open_internal(const char *path, int flags,
                                      mode_t parsed_mode) {
  message m;

  // This function is called after __should_use_mathematical() has confirmed we
  // should proceed and potentially after __math_state.mode ==
  // MATH_MODE_MANDATORY check with kernel_support. If kernel doesn't support it
  // AND mode is mandatory, open() or mathematical_open() should prevent calling
  // this.
  if (__math_state.mode == MATH_MODE_MANDATORY &&
      !__math_state.kernel_support) {
    errno = ENOSYS; // Kernel doesn't support the feature, and it's mandatory.
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = VFS_OPEN;
  m.m_lc_vfs_path = (vir_bytes)path;
  m.m_lc_vfs_flags = flags | O_MATHEMATICAL;
  m.m_lc_vfs_mode = parsed_mode;
  return _syscall(VFS_PROC_NR, VFS_OPEN, &m);
}

/* --- Overridden open() --- */
int open(const char *path, int flags, ...) {
  mode_t mode = 0;
  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, mode_t);
    va_end(ap);
  }

  if (!__math_state.initialized) {
    __mathematical_init();
  }

  if (__should_use_mathematical(flags)) {
    // If mandatory and not supported by kernel, this call should fail.
    if (__math_state.mode == MATH_MODE_MANDATORY &&
        !__math_state.kernel_support) {
      errno = ENOSYS; // Or EACCES, depending on desired behavior for mandatory
                      // failure
      return -1;
    }
    return mathematical_open_internal(path, flags, mode);
  }
  return _open_traditional(path, flags, mode);
}

/* --- Explicit mathematical_open() --- */
int mathematical_open(const char *path, int flags, ...) {
  mode_t mode = 0;
  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, mode_t);
    va_end(ap);
  }

  if (!__math_state.initialized) {
    __mathematical_init();
  }

  // If disabled, this explicit call should probably still try, or error.
  // If mandatory and not supported, it must error.
  if (__math_state.mode == MATH_MODE_DISABLED &&
      !(__math_state.policy & POLICY_MANDATORY_ALL)) {
    // If not system-wide mandatory, and current mode is disabled, this explicit
    // call could be disallowed. However, an explicit call usually means "I want
    // this feature". For now, let it proceed if kernel supports it, respecting
    // MANDATORY.
  }

  if (__math_state.mode == MATH_MODE_MANDATORY &&
      !__math_state.kernel_support) {
    errno = ENOSYS;
    return -1;
  }
  if (!__math_state.kernel_support && __math_state.mode != MATH_MODE_DISABLED) {
    // If any math mode is on but kernel doesn't support, explicit calls should
    // fail.
    errno = ENOSYS;
    return -1;
  }

  return mathematical_open_internal(path, flags, mode);
}

/* --- Implementations for other mathematical_ functions from mathposix.h --- */
ssize_t mathematical_read(int fd, void *buf, size_t count) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES; // Or ENOSYS: feature explicitly disabled for this call
    return -1;
  }
  if (!__math_state
           .kernel_support) { // Mandatory or Enabled but no kernel support
    errno = ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_MATH_READ;
  m.m_lc_vfs_fd = fd;
  m.m_lc_vfs_buf = (vir_bytes)buf;
  m.m_lc_vfs_len = count;
  return _syscall(SYSTEM, SYS_MATH_READ, &m);
}

ssize_t mathematical_write(int fd, const void *buf, size_t count) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES;
    return -1;
  }
  if (!__math_state.kernel_support) {
    errno = ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_MATH_WRITE;
  m.m_lc_vfs_fd = fd;
  m.m_lc_vfs_buf = (vir_bytes)buf;
  m.m_lc_vfs_len = count;
  return _syscall(SYSTEM, SYS_MATH_WRITE, &m);
}

int mathematical_close(int fd) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  // Close might be special: even if math mode is off, if it was opened with
  // math, the kernel might need to do special cleanup via SYS_MATH_CLOSE. For
  // now, if math mode is generally off, or no kernel support, let it be a
  // normal close. This assumes that a standard close would be sufficient if
  // math features weren't used. The kernel handler for SYS_MATH_CLOSE should be
  // idempotent or handle non-math FDs gracefully.
  if (__math_state.mode == MATH_MODE_DISABLED || !__math_state.kernel_support) {
    // Fallback to standard close if math features are off or not supported.
    // Standard close is just 'close(fd)' which itself is a syscall.
    // This file is providing mathematical_close, so it should perhaps do its
    // own logic or fail. For now, if disabled or not supported, make
    // SYS_MATH_CLOSE fail.
    errno = (__math_state.mode == MATH_MODE_DISABLED) ? EACCES : ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_MATH_CLOSE;
  m.m_lc_vfs_fd = fd;
  return _syscall(SYSTEM, SYS_MATH_CLOSE, &m);
}

int mathematical_derive(int parent_fd, int new_rights) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES;
    return -1;
  }
  if (!__math_state.kernel_support) {
    errno = ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_CAP_DERIVE;
  m.m1_i1 = parent_fd;
  m.m1_i2 = new_rights;
  return _syscall(SYSTEM, SYS_CAP_DERIVE, &m);
}

int mathematical_open_with_expiry(const char *path, int flags, mode_t mode,
                                  time_t expiry) {
  // message m; // For when SYS_MATH_OPEN_EXPIRY is defined and handled

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES;
    return -1;
  }
  if (!__math_state.kernel_support) { // Or if SYS_MATH_OPEN_EXPIRY specifically
                                      // isn't supported
    errno = ENOSYS;
    return -1;
  }

  // TODO: Implement with a new SYS_MATH_OPEN_EXPIRY call
  // memset(&m, 0, sizeof(m));
  // m.m_type = SYS_MATH_OPEN_EXPIRY;
  // m.m_lc_vfs_path = (vir_bytes)path;
  // m.m_lc_vfs_flags = flags | O_MATHEMATICAL; // Assuming expiry implies
  // mathematical m.m_lc_vfs_mode = mode; m.m2_l1 = expiry; // Example field for
  // expiry return _syscall(VFS_PROC_NR, SYS_MATH_OPEN_EXPIRY, &m); // Or SYSTEM
  // if direct kernel call

  (void)path;
  (void)flags;
  (void)mode;
  (void)expiry;   // Suppress unused warnings for now
  errno = ENOSYS; // Not implemented yet
  return -1;
}

// Functions that might be added to mathposix.h and implemented here or in
// math_control.c int mathematical_get_mode(void) {
//     if (!__math_state.initialized) __mathematical_init();
//     return __math_state.mode;
// }

// int mathematical_was_used(int fd) {
//     // This would require per-fd state, possibly via another syscall to query
//     fd properties (void)fd; if (!__math_state.initialized)
//     __mathematical_init();
//     // For now, assume if math mode is on, it was used. This is a
//     simplification. return (__math_state.mode != MATH_MODE_DISABLED &&
//     __math_state.kernel_support);
// }
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h> // For getenv, NULL
#include <string.h> // For memset
#include <time.h>   // For time_t

// Minix headers
#include <minix/callnr.h>
#include <minix/com.h>
#include <minix/fcntl.h> // For O_CREAT, O_RDWR, O_MATHEMATICAL etc.
#include <minix/ipc.h>
#include <sys/types.h>

// Internal libc header for shared math capability state and functions
#include "mathematical_internal.h" // Provides __math_state, __mathematical_init, __should_use_mathematical

// Project-specific public header
#include <minix/capability.h>

// Assumed to provide _syscall and possibly common message setup utilities
#include <lib.h>

/* --- Traditional open (fallback) --- */
static int _open_traditional(const char *path, int flags, mode_t mode) {
  message m;
  memset(&m, 0, sizeof(m));
  m.m_type = VFS_OPEN;
  m.m_lc_vfs_path = (vir_bytes)path;
  m.m_lc_vfs_flags = flags;
  m.m_lc_vfs_mode = mode;
  return _syscall(VFS_PROC_NR, VFS_OPEN, &m);
}

/* --- Internal implementation for mathematical open --- */
static int mathematical_open_internal(const char *path, int flags,
                                      mode_t parsed_mode) {
  message m;

  // This function is called after __should_use_mathematical() has confirmed we
  // should proceed and potentially after __math_state.mode ==
  // MATH_MODE_MANDATORY check with kernel_support. If kernel doesn't support it
  // AND mode is mandatory, open() or mathematical_open() should prevent calling
  // this.
  if (__math_state.mode == MATH_MODE_MANDATORY &&
      !__math_state.kernel_support) {
    errno = ENOSYS; // Kernel doesn't support the feature, and it's mandatory.
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = VFS_OPEN;
  m.m_lc_vfs_path = (vir_bytes)path;
  m.m_lc_vfs_flags = flags | O_MATHEMATICAL;
  m.m_lc_vfs_mode = parsed_mode;
  return _syscall(VFS_PROC_NR, VFS_OPEN, &m);
}

/* --- Overridden open() --- */
int open(const char *path, int flags, ...) {
  mode_t mode = 0;
  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, mode_t);
    va_end(ap);
  }

  if (!__math_state.initialized) {
    __mathematical_init();
  }

  if (__should_use_mathematical(flags)) {
    // If mandatory and not supported by kernel, this call should fail.
    if (__math_state.mode == MATH_MODE_MANDATORY &&
        !__math_state.kernel_support) {
      errno = ENOSYS; // Or EACCES, depending on desired behavior for mandatory
                      // failure
      return -1;
    }
    return mathematical_open_internal(path, flags, mode);
  }
  return _open_traditional(path, flags, mode);
}

/* --- Explicit mathematical_open() --- */
int mathematical_open(const char *path, int flags, ...) {
  mode_t mode = 0;
  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, mode_t);
    va_end(ap);
  }

  if (!__math_state.initialized) {
    __mathematical_init();
  }

  // If disabled, this explicit call should probably still try, or error.
  // If mandatory and not supported, it must error.
  if (__math_state.mode == MATH_MODE_DISABLED &&
      !(__math_state.policy & POLICY_MANDATORY_ALL)) {
    // If not system-wide mandatory, and current mode is disabled, this explicit
    // call could be disallowed. However, an explicit call usually means "I want
    // this feature". For now, let it proceed if kernel supports it, respecting
    // MANDATORY.
  }

  if (__math_state.mode == MATH_MODE_MANDATORY &&
      !__math_state.kernel_support) {
    errno = ENOSYS;
    return -1;
  }
  if (!__math_state.kernel_support && __math_state.mode != MATH_MODE_DISABLED) {
    // If any math mode is on but kernel doesn't support, explicit calls should
    // fail.
    errno = ENOSYS;
    return -1;
  }

  return mathematical_open_internal(path, flags, mode);
}

/* --- Implementations for other mathematical_ functions from mathposix.h --- */
ssize_t mathematical_read(int fd, void *buf, size_t count) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES; // Or ENOSYS: feature explicitly disabled for this call
    return -1;
  }
  if (!__math_state
           .kernel_support) { // Mandatory or Enabled but no kernel support
    errno = ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_MATH_READ;
  m.m_lc_vfs_fd = fd;
  m.m_lc_vfs_buf = (vir_bytes)buf;
  m.m_lc_vfs_len = count;
  return _syscall(SYSTEM, SYS_MATH_READ, &m);
}

ssize_t mathematical_write(int fd, const void *buf, size_t count) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES;
    return -1;
  }
  if (!__math_state.kernel_support) {
    errno = ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_MATH_WRITE;
  m.m_lc_vfs_fd = fd;
  m.m_lc_vfs_buf = (vir_bytes)buf;
  m.m_lc_vfs_len = count;
  return _syscall(SYSTEM, SYS_MATH_WRITE, &m);
}

int mathematical_close(int fd) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  // Close might be special: even if math mode is off, if it was opened with
  // math, the kernel might need to do special cleanup via SYS_MATH_CLOSE. For
  // now, if math mode is generally off, or no kernel support, let it be a
  // normal close. This assumes that a standard close would be sufficient if
  // math features weren't used. The kernel handler for SYS_MATH_CLOSE should be
  // idempotent or handle non-math FDs gracefully.
  if (__math_state.mode == MATH_MODE_DISABLED || !__math_state.kernel_support) {
    // Fallback to standard close if math features are off or not supported.
    // Standard close is just 'close(fd)' which itself is a syscall.
    // This file is providing mathematical_close, so it should perhaps do its
    // own logic or fail. For now, if disabled or not supported, make
    // SYS_MATH_CLOSE fail.
    errno = (__math_state.mode == MATH_MODE_DISABLED) ? EACCES : ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_MATH_CLOSE;
  m.m_lc_vfs_fd = fd;
  return _syscall(SYSTEM, SYS_MATH_CLOSE, &m);
}

int mathematical_derive(int parent_fd, int new_rights) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES;
    return -1;
  }
  if (!__math_state.kernel_support) {
    errno = ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_CAP_DERIVE;
  m.m1_i1 = parent_fd;
  m.m1_i2 = new_rights;
  return _syscall(SYSTEM, SYS_CAP_DERIVE, &m);
}

int mathematical_open_with_expiry(const char *path, int flags, mode_t mode,
                                  time_t expiry) {
  // message m; // For when SYS_MATH_OPEN_EXPIRY is defined and handled

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES;
    return -1;
  }
  if (!__math_state.kernel_support) { // Or if SYS_MATH_OPEN_EXPIRY specifically
                                      // isn't supported
    errno = ENOSYS;
    return -1;
  }

  // TODO: Implement with a new SYS_MATH_OPEN_EXPIRY call
  // memset(&m, 0, sizeof(m));
  // m.m_type = SYS_MATH_OPEN_EXPIRY;
  // m.m_lc_vfs_path = (vir_bytes)path;
  // m.m_lc_vfs_flags = flags | O_MATHEMATICAL; // Assuming expiry implies
  // mathematical m.m_lc_vfs_mode = mode; m.m2_l1 = expiry; // Example field for
  // expiry return _syscall(VFS_PROC_NR, SYS_MATH_OPEN_EXPIRY, &m); // Or SYSTEM
  // if direct kernel call

  (void)path;
  (void)flags;
  (void)mode;
  (void)expiry;   // Suppress unused warnings for now
  errno = ENOSYS; // Not implemented yet
  return -1;
}

// Functions that might be added to mathposix.h and implemented here or in
// math_control.c int mathematical_get_mode(void) {
//     if (!__math_state.initialized) __mathematical_init();
//     return __math_state.mode;
// }

// int mathematical_was_used(int fd) {
//     // This would require per-fd state, possibly via another syscall to query
//     fd properties (void)fd; if (!__math_state.initialized)
//     __mathematical_init();
//     // For now, assume if math mode is on, it was used. This is a
//     simplification. return (__math_state.mode != MATH_MODE_DISABLED &&
//     __math_state.kernel_support);
// }
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h> // For getenv, NULL
#include <string.h> // For memset
#include <time.h>   // For time_t

// Minix headers
#include <minix/callnr.h>
#include <minix/com.h>
#include <minix/fcntl.h> // For O_CREAT, O_RDWR, O_MATHEMATICAL etc.
#include <minix/ipc.h>
#include <sys/types.h>

// Internal libc header for shared math capability state and functions
#include "mathematical_internal.h" // Provides __math_state, __mathematical_init, __should_use_mathematical

// Project-specific public header
#include <minix/capability.h>

// Assumed to provide _syscall and possibly common message setup utilities
#include <lib.h>

/* --- Traditional open (fallback) --- */
static int _open_traditional(const char *path, int flags, mode_t mode) {
  message m;
  memset(&m, 0, sizeof(m));
  m.m_type = VFS_OPEN;
  m.m_lc_vfs_path = (vir_bytes)path;
  m.m_lc_vfs_flags = flags;
  m.m_lc_vfs_mode = mode;
  return _syscall(VFS_PROC_NR, VFS_OPEN, &m);
}

/* --- Internal implementation for mathematical open --- */
static int mathematical_open_internal(const char *path, int flags,
                                      mode_t parsed_mode) {
  message m;

  // This function is called after __should_use_mathematical() has confirmed we
  // should proceed and potentially after __math_state.mode ==
  // MATH_MODE_MANDATORY check with kernel_support. If kernel doesn't support it
  // AND mode is mandatory, open() or mathematical_open() should prevent calling
  // this.
  if (__math_state.mode == MATH_MODE_MANDATORY &&
      !__math_state.kernel_support) {
    errno = ENOSYS; // Kernel doesn't support the feature, and it's mandatory.
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = VFS_OPEN;
  m.m_lc_vfs_path = (vir_bytes)path;
  m.m_lc_vfs_flags = flags | O_MATHEMATICAL;
  m.m_lc_vfs_mode = parsed_mode;
  return _syscall(VFS_PROC_NR, VFS_OPEN, &m);
}

/* --- Overridden open() --- */
int open(const char *path, int flags, ...) {
  mode_t mode = 0;
  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, mode_t);
    va_end(ap);
  }

  if (!__math_state.initialized) {
    __mathematical_init();
  }

  if (__should_use_mathematical(flags)) {
    // If mandatory and not supported by kernel, this call should fail.
    if (__math_state.mode == MATH_MODE_MANDATORY &&
        !__math_state.kernel_support) {
      errno = ENOSYS; // Or EACCES, depending on desired behavior for mandatory
                      // failure
      return -1;
    }
    return mathematical_open_internal(path, flags, mode);
  }
  return _open_traditional(path, flags, mode);
}

/* --- Explicit mathematical_open() --- */
int mathematical_open(const char *path, int flags, ...) {
  mode_t mode = 0;
  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, mode_t);
    va_end(ap);
  }

  if (!__math_state.initialized) {
    __mathematical_init();
  }

  // If disabled, this explicit call should probably still try, or error.
  // If mandatory and not supported, it must error.
  if (__math_state.mode == MATH_MODE_DISABLED &&
      !(__math_state.policy & POLICY_MANDATORY_ALL)) {
    // If not system-wide mandatory, and current mode is disabled, this explicit
    // call could be disallowed. However, an explicit call usually means "I want
    // this feature". For now, let it proceed if kernel supports it, respecting
    // MANDATORY.
  }

  if (__math_state.mode == MATH_MODE_MANDATORY &&
      !__math_state.kernel_support) {
    errno = ENOSYS;
    return -1;
  }
  if (!__math_state.kernel_support && __math_state.mode != MATH_MODE_DISABLED) {
    // If any math mode is on but kernel doesn't support, explicit calls should
    // fail.
    errno = ENOSYS;
    return -1;
  }

  return mathematical_open_internal(path, flags, mode);
}

/* --- Implementations for other mathematical_ functions from mathposix.h --- */
ssize_t mathematical_read(int fd, void *buf, size_t count) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES; // Or ENOSYS: feature explicitly disabled for this call
    return -1;
  }
  if (!__math_state
           .kernel_support) { // Mandatory or Enabled but no kernel support
    errno = ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_MATH_READ;
  m.m_lc_vfs_fd = fd;
  m.m_lc_vfs_buf = (vir_bytes)buf;
  m.m_lc_vfs_len = count;
  return _syscall(SYSTEM, SYS_MATH_READ, &m);
}

ssize_t mathematical_write(int fd, const void *buf, size_t count) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES;
    return -1;
  }
  if (!__math_state.kernel_support) {
    errno = ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_MATH_WRITE;
  m.m_lc_vfs_fd = fd;
  m.m_lc_vfs_buf = (vir_bytes)buf;
  m.m_lc_vfs_len = count;
  return _syscall(SYSTEM, SYS_MATH_WRITE, &m);
}

int mathematical_close(int fd) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  // Close might be special: even if math mode is off, if it was opened with
  // math, the kernel might need to do special cleanup via SYS_MATH_CLOSE. For
  // now, if math mode is generally off, or no kernel support, let it be a
  // normal close. This assumes that a standard close would be sufficient if
  // math features weren't used. The kernel handler for SYS_MATH_CLOSE should be
  // idempotent or handle non-math FDs gracefully.
  if (__math_state.mode == MATH_MODE_DISABLED || !__math_state.kernel_support) {
    // Fallback to standard close if math features are off or not supported.
    // Standard close is just 'close(fd)' which itself is a syscall.
    // This file is providing mathematical_close, so it should perhaps do its
    // own logic or fail. For now, if disabled or not supported, make
    // SYS_MATH_CLOSE fail.
    errno = (__math_state.mode == MATH_MODE_DISABLED) ? EACCES : ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_MATH_CLOSE;
  m.m_lc_vfs_fd = fd;
  return _syscall(SYSTEM, SYS_MATH_CLOSE, &m);
}

int mathematical_derive(int parent_fd, int new_rights) {
  message m;

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES;
    return -1;
  }
  if (!__math_state.kernel_support) {
    errno = ENOSYS;
    return -1;
  }

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_CAP_DERIVE;
  m.m1_i1 = parent_fd;
  m.m1_i2 = new_rights;
  return _syscall(SYSTEM, SYS_CAP_DERIVE, &m);
}

int mathematical_open_with_expiry(const char *path, int flags, mode_t mode,
                                  time_t expiry) {
  // message m; // For when SYS_MATH_OPEN_EXPIRY is defined and handled

  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (__math_state.mode == MATH_MODE_DISABLED) {
    errno = EACCES;
    return -1;
  }
  if (!__math_state.kernel_support) { // Or if SYS_MATH_OPEN_EXPIRY specifically
                                      // isn't supported
    errno = ENOSYS;
    return -1;
  }

  // TODO: Implement with a new SYS_MATH_OPEN_EXPIRY call
  // memset(&m, 0, sizeof(m));
  // m.m_type = SYS_MATH_OPEN_EXPIRY;
  // m.m_lc_vfs_path = (vir_bytes)path;
  // m.m_lc_vfs_flags = flags | O_MATHEMATICAL; // Assuming expiry implies
  // mathematical m.m_lc_vfs_mode = mode; m.m2_l1 = expiry; // Example field for
  // expiry return _syscall(VFS_PROC_NR, SYS_MATH_OPEN_EXPIRY, &m); // Or SYSTEM
  // if direct kernel call

  (void)path;
  (void)flags;
  (void)mode;
  (void)expiry;   // Suppress unused warnings for now
  errno = ENOSYS; // Not implemented yet
  return -1;
}

// Functions that might be added to mathposix.h and implemented here or in
// math_control.c int mathematical_get_mode(void) {
//     if (!__math_state.initialized) __mathematical_init();
//     return __math_state.mode;
// }

// int mathematical_was_used(int fd) {
//     // This would require per-fd state, possibly via another syscall to query
//     fd properties (void)fd; if (!__math_state.initialized)
//     __mathematical_init();
//     // For now, assume if math mode is on, it was used. This is a
//     simplification. return (__math_state.mode != MATH_MODE_DISABLED &&
//     __math_state.kernel_support);
// }
