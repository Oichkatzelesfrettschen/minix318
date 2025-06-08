#include <errno.h>
#include <stdarg.h>
#include <string.h>     // For memset
#include <stdlib.h>     // For getenv, NULL
#include <time.h>       // For time_t

// Minix headers
#include <minix/ipc.h>
#include <minix/com.h>
#include <minix/callnr.h>
#include <sys/types.h>
#include <minix/fcntl.h> // For O_CREAT, O_RDWR, O_MATHEMATICAL etc.

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
static int mathematical_open_internal(const char *path, int flags, mode_t parsed_mode) {
    message m;

    // This function is called after __should_use_mathematical() has confirmed we should proceed
    // and potentially after __math_state.mode == MATH_MODE_MANDATORY check with kernel_support.
    // If kernel doesn't support it AND mode is mandatory, open() or mathematical_open() should prevent calling this.
    if (__math_state.mode == MATH_MODE_MANDATORY && !__math_state.kernel_support) {
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
        if (__math_state.mode == MATH_MODE_MANDATORY && !__math_state.kernel_support) {
            errno = ENOSYS; // Or EACCES, depending on desired behavior for mandatory failure
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
    if (__math_state.mode == MATH_MODE_DISABLED && !(__math_state.policy & POLICY_MANDATORY_ALL)) {
         // If not system-wide mandatory, and current mode is disabled, this explicit call could be disallowed.
         // However, an explicit call usually means "I want this feature".
         // For now, let it proceed if kernel supports it, respecting MANDATORY.
    }

    if (__math_state.mode == MATH_MODE_MANDATORY && !__math_state.kernel_support) {
        errno = ENOSYS;
        return -1;
    }
     if (!__math_state.kernel_support && __math_state.mode != MATH_MODE_DISABLED) {
        // If any math mode is on but kernel doesn't support, explicit calls should fail.
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
    if (!__math_state.kernel_support) { // Mandatory or Enabled but no kernel support
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
    // Close might be special: even if math mode is off, if it was opened with math,
    // the kernel might need to do special cleanup via SYS_MATH_CLOSE.
    // For now, if math mode is generally off, or no kernel support, let it be a normal close.
    // This assumes that a standard close would be sufficient if math features weren't used.
    // The kernel handler for SYS_MATH_CLOSE should be idempotent or handle non-math FDs gracefully.
    if (__math_state.mode == MATH_MODE_DISABLED || !__math_state.kernel_support) {
        // Fallback to standard close if math features are off or not supported.
        // Standard close is just 'close(fd)' which itself is a syscall.
        // This file is providing mathematical_close, so it should perhaps do its own logic or fail.
        // For now, if disabled or not supported, make SYS_MATH_CLOSE fail.
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

int mathematical_open_with_expiry(const char* path, int flags, mode_t mode, time_t expiry) {
    // message m; // For when SYS_MATH_OPEN_EXPIRY is defined and handled

    if (!__math_state.initialized) {
        __mathematical_init();
    }
    if (__math_state.mode == MATH_MODE_DISABLED) {
        errno = EACCES;
        return -1;
    }
    if (!__math_state.kernel_support) { // Or if SYS_MATH_OPEN_EXPIRY specifically isn't supported
        errno = ENOSYS;
        return -1;
    }

    // TODO: Implement with a new SYS_MATH_OPEN_EXPIRY call
    // memset(&m, 0, sizeof(m));
    // m.m_type = SYS_MATH_OPEN_EXPIRY;
    // m.m_lc_vfs_path = (vir_bytes)path;
    // m.m_lc_vfs_flags = flags | O_MATHEMATICAL; // Assuming expiry implies mathematical
    // m.m_lc_vfs_mode = mode;
    // m.m2_l1 = expiry; // Example field for expiry
    // return _syscall(VFS_PROC_NR, SYS_MATH_OPEN_EXPIRY, &m); // Or SYSTEM if direct kernel call

    (void)path; (void)flags; (void)mode; (void)expiry; // Suppress unused warnings for now
    errno = ENOSYS; // Not implemented yet
    return -1;
}

// Functions that might be added to mathposix.h and implemented here or in math_control.c
// int mathematical_get_mode(void) {
//     if (!__math_state.initialized) __mathematical_init();
//     return __math_state.mode;
// }

// int mathematical_was_used(int fd) {
//     // This would require per-fd state, possibly via another syscall to query fd properties
//     (void)fd;
//     if (!__math_state.initialized) __mathematical_init();
//     // For now, assume if math mode is on, it was used. This is a simplification.
//     return (__math_state.mode != MATH_MODE_DISABLED && __math_state.kernel_support);
// }
