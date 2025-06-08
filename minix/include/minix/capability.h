// minix/include/minix/capability.h
#ifndef _MINIX_CAPABILITY_H
#define _MINIX_CAPABILITY_H

#include <fcntl.h> // For O_ flags, including O_MATHEMATICAL (should be <minix/fcntl.h>)
#include <stdarg.h> // For va_list, va_start, va_end in inline functions
#include <sys/types.h> // For mode_t, ssize_t, time_t (though time_t not directly here)

// To ensure O_MATHEMATICAL flags are definitely included if fcntl.h above isn't
// minix/fcntl.h This assumes minix/fcntl.h is the final location.
#ifndef O_MATHEMATICAL
#include <minix/fcntl.h>
#endif

// Forward declare struct proc if needed by prototypes, or include
// "kernel/proc.h" in .c files
struct proc;

#define MAX_CAPABILITIES_PER_PROC 16 // Example size for basic table
#define PM_CAP_IDX 0                 // Well-known index for PM capability

typedef struct {
  endpoint_t target;
  u32_t rights;
  u8_t in_use; // 0 = free, 1 = in use
  // u32_t epoch; // For future epoch checking (as per RS integration hint)
} capability_t;

#define OP_SEND 0x0001 // Example right

// Prototype for cap_lookup - will be implemented in capability.c
capability_t *cap_lookup(struct proc *caller_ptr, int cap_idx);

// Prototype for capability table initialization for a process
void init_proc_capabilities(struct proc *pr);

#endif /* _MINIX_CAPABILITY_H */

/* --- Capability Rights --- */
// These values should align with what the kernel expects (e.g., in
// capability_syscalls.h or kcapability_posix.h) For user-space, these define
// the API.
#define CAP_READ 0x01
#define CAP_WRITE 0x02
#define CAP_CREATE 0x04 // Corresponds to O_CREAT and write access to parent dir
#define CAP_TRUNCATE 0x08 // Corresponds to O_TRUNC and write access
#define CAP_EXECUTE                                                            \
  0x10 // For exec-like operations if capabilities extend there

/* --- Capability Error Codes --- */
// These should be distinct from standard POSIX errno values.
// Typically, custom errors are positive, and functions return -1 and set errno.
// Or, they can be negative values returned directly if _syscall convention
// allows. For now, defining them as potential errno values.
#define ECAPABILITY 1000 /* Generic capability error (placeholder value) */
#define ECAPDENIED 1001  /* Capability check failed */
#define ECAPEXPIRED 1002 /* Capability expired */

/* --- User-Space Capability Type --- */
// This is an opaque handle for user-space. The actual proof structure is
// kernel-internal. It might hold a reference or ID that libc can pass to the
// kernel (e.g., via m_lc_vfs_cached_cap). For now, a simple struct. If it's
// truly opaque and only passed around, void* might do. But the feedback's
// cap_cache_entry used `capability_t capability;` suggesting it holds
// something.
typedef struct {
  unsigned long id;       // Example: could be a hash or an ID from the kernel
  void *internal_details; // Opaque pointer to more data if needed by libc, not
                          // user
} capability_t;

/* --- Runtime Control Functions --- */
/* (Implemented in libc/sys-minix/math_control.c) */

/**
 * @brief Enables mathematical verification for subsequent relevant operations
 * in the current process. This typically sets the mode to MATH_MODE_ENABLED.
 * @return 0 on success, -1 on error (and errno is set).
 */
int enable_mathematical_verification(void);

/**
 * @brief Disables mathematical verification for subsequent relevant operations.
 * Sets the mode to MATH_MODE_DISABLED.
 * @return 0 on success, -1 on error.
 */
int disable_mathematical_verification(void);

/**
 * @brief Requires mathematical verification for subsequent relevant operations.
 * Sets the mode to MATH_MODE_MANDATORY. Operations may fail if verification
 * is not possible or not supported by the kernel.
 * @return 0 on success, -1 on error.
 */
int require_mathematical_verification(void);

/**
 * @brief Checks if the underlying kernel supports mathematical verification
 * capabilities.
 * @return 1 if available, 0 otherwise.
 */
int mathematical_verification_available(void);

/**
 * @brief Gets the current mathematical verification mode for the process.
 * @return The current mode (MATH_MODE_DISABLED, MATH_MODE_ENABLED,
 * MATH_MODE_MANDATORY).
 */
int mathematical_get_mode(void);

/**
 * @brief Checks if mathematical verification was (or would be) applied for an
 * operation. Note: For an existing fd, this is a simplified check based on
 * current global state. A detailed check would require kernel support per fd.
 * @param fd File descriptor (currently unused in simplified version).
 * @return 1 if mathematical verification would be/was applied, 0 otherwise.
 */
int mathematical_was_used(int fd);

/* --- Explicit Mathematical POSIX Variants --- */

/**
 * @brief Opens a file with mathematical verification explicitly requested and
 * required. This is an inline wrapper around open(2) that adds O_MATHEMATICAL
 * and O_MATHEMATICAL_REQUIRED.
 */
static inline int mathematical_open(const char *path, int flags, ...) {
  mode_t mode = 0;
  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, mode_t); // mode_t is int in Minix
    va_end(ap);
    return open(path, flags | O_MATHEMATICAL | O_MATHEMATICAL_REQUIRED, mode);
  }
  return open(path, flags | O_MATHEMATICAL | O_MATHEMATICAL_REQUIRED);
}

/*
 * For mathematical_read, mathematical_write, mathematical_close:
 * These currently use dedicated SYS_MATH_READ, SYS_MATH_WRITE, SYS_MATH_CLOSE
 * syscalls as implemented in mathematical_posix.c. They are not simple
 * flag-based wrappers around the standard read/write/close syscalls. So, they
 * are declared as normal functions.
 */
ssize_t mathematical_read(int fd, void *buf, size_t count);
ssize_t mathematical_write(int fd, const void *buf, size_t count);
int mathematical_close(int fd);

// This was also a dedicated syscall, not a flag-based wrapper.
int mathematical_open_with_expiry(const char *path, int flags, mode_t mode,
                                  time_t expiry);

/* --- Capability Management Functions --- */
/* (Implemented in libc/sys-minix/mathematical_posix.c using direct kernel
 * syscalls) */

/**
 * @brief Derives a new capability from an existing one (represented by
 * source_fd).
 * @param source_fd File descriptor associated with the parent capability.
 * @param new_rights The rights for the new derived capability.
 * @return A new file descriptor for the derived capability, or -1 on error.
 */
int capability_derive(int source_fd, int new_rights);

/**
 * @brief Restricts the rights of an existing capability.
 * @param fd File descriptor associated with the capability to restrict.
 * @param rights_mask A mask of rights to keep (others are removed).
 * @return 0 on success, -1 on error.
 */
int capability_restrict(int fd, int rights_mask);

/**
 * @brief Revokes an existing capability.
 * @param fd File descriptor associated with the capability to revoke.
 * @return 0 on success, -1 on error.
 */
int capability_revoke(int fd);

#endif /* _MINIX_CAPABILITY_H */
// minix/include/minix/capability.h
#ifndef _MINIX_CAPABILITY_H
#define _MINIX_CAPABILITY_H

#include <fcntl.h> // For O_ flags, including O_MATHEMATICAL (should be <minix/fcntl.h>)
#include <stdarg.h> // For va_list, va_start, va_end in inline functions
#include <sys/types.h> // For mode_t, ssize_t, time_t (though time_t not directly here)

// To ensure O_MATHEMATICAL flags are definitely included if fcntl.h above isn't
// minix/fcntl.h This assumes minix/fcntl.h is the final location.
#ifndef O_MATHEMATICAL
#include <minix/fcntl.h>
#endif

/* --- Capability Rights --- */
// These values should align with what the kernel expects (e.g., in
// capability_syscalls.h or kcapability_posix.h) For user-space, these define
// the API.
#define CAP_READ 0x01
#define CAP_WRITE 0x02
#define CAP_CREATE 0x04 // Corresponds to O_CREAT and write access to parent dir
#define CAP_TRUNCATE 0x08 // Corresponds to O_TRUNC and write access
#define CAP_EXECUTE                                                            \
  0x10 // For exec-like operations if capabilities extend there

/* --- Capability Error Codes --- */
// These should be distinct from standard POSIX errno values.
// Typically, custom errors are positive, and functions return -1 and set errno.
// Or, they can be negative values returned directly if _syscall convention
// allows. For now, defining them as potential errno values.
#define ECAPABILITY 1000 /* Generic capability error (placeholder value) */
#define ECAPDENIED 1001  /* Capability check failed */
#define ECAPEXPIRED 1002 /* Capability expired */

/* --- User-Space Capability Type --- */
// This is an opaque handle for user-space. The actual proof structure is
// kernel-internal. It might hold a reference or ID that libc can pass to the
// kernel (e.g., via m_lc_vfs_cached_cap). For now, a simple struct. If it's
// truly opaque and only passed around, void* might do. But the feedback's
// cap_cache_entry used `capability_t capability;` suggesting it holds
// something.
typedef struct {
  unsigned long id;       // Example: could be a hash or an ID from the kernel
  void *internal_details; // Opaque pointer to more data if needed by libc, not
                          // user
} capability_t;

/* --- Runtime Control Functions --- */
/* (Implemented in libc/sys-minix/math_control.c) */

/**
 * @brief Enables mathematical verification for subsequent relevant operations
 * in the current process. This typically sets the mode to MATH_MODE_ENABLED.
 * @return 0 on success, -1 on error (and errno is set).
 */
int enable_mathematical_verification(void);

/**
 * @brief Disables mathematical verification for subsequent relevant operations.
 * Sets the mode to MATH_MODE_DISABLED.
 * @return 0 on success, -1 on error.
 */
int disable_mathematical_verification(void);

/**
 * @brief Requires mathematical verification for subsequent relevant operations.
 * Sets the mode to MATH_MODE_MANDATORY. Operations may fail if verification
 * is not possible or not supported by the kernel.
 * @return 0 on success, -1 on error.
 */
int require_mathematical_verification(void);

/**
 * @brief Checks if the underlying kernel supports mathematical verification
 * capabilities.
 * @return 1 if available, 0 otherwise.
 */
int mathematical_verification_available(void);

/**
 * @brief Gets the current mathematical verification mode for the process.
 * @return The current mode (MATH_MODE_DISABLED, MATH_MODE_ENABLED,
 * MATH_MODE_MANDATORY).
 */
int mathematical_get_mode(void);

/**
 * @brief Checks if mathematical verification was (or would be) applied for an
 * operation. Note: For an existing fd, this is a simplified check based on
 * current global state. A detailed check would require kernel support per fd.
 * @param fd File descriptor (currently unused in simplified version).
 * @return 1 if mathematical verification would be/was applied, 0 otherwise.
 */
int mathematical_was_used(int fd);

/* --- Explicit Mathematical POSIX Variants --- */

/**
 * @brief Opens a file with mathematical verification explicitly requested and
 * required. This is an inline wrapper around open(2) that adds O_MATHEMATICAL
 * and O_MATHEMATICAL_REQUIRED.
 */
static inline int mathematical_open(const char *path, int flags, ...) {
  mode_t mode = 0;
  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, mode_t); // mode_t is int in Minix
    va_end(ap);
    return open(path, flags | O_MATHEMATICAL | O_MATHEMATICAL_REQUIRED, mode);
  }
  return open(path, flags | O_MATHEMATICAL | O_MATHEMATICAL_REQUIRED);
}

/*
 * For mathematical_read, mathematical_write, mathematical_close:
 * These currently use dedicated SYS_MATH_READ, SYS_MATH_WRITE, SYS_MATH_CLOSE
 * syscalls as implemented in mathematical_posix.c. They are not simple
 * flag-based wrappers around the standard read/write/close syscalls. So, they
 * are declared as normal functions.
 */
ssize_t mathematical_read(int fd, void *buf, size_t count);
ssize_t mathematical_write(int fd, const void *buf, size_t count);
int mathematical_close(int fd);

// This was also a dedicated syscall, not a flag-based wrapper.
int mathematical_open_with_expiry(const char *path, int flags, mode_t mode,
                                  time_t expiry);

/* --- Capability Management Functions --- */
/* (Implemented in libc/sys-minix/mathematical_posix.c using direct kernel
 * syscalls) */

/**
 * @brief Derives a new capability from an existing one (represented by
 * source_fd).
 * @param source_fd File descriptor associated with the parent capability.
 * @param new_rights The rights for the new derived capability.
 * @return A new file descriptor for the derived capability, or -1 on error.
 */
int capability_derive(int source_fd, int new_rights);

/**
 * @brief Restricts the rights of an existing capability.
 * @param fd File descriptor associated with the capability to restrict.
 * @param rights_mask A mask of rights to keep (others are removed).
 * @return 0 on success, -1 on error.
 */
int capability_restrict(int fd, int rights_mask);

/**
 * @brief Revokes an existing capability.
 * @param fd File descriptor associated with the capability to revoke.
 * @return 0 on success, -1 on error.
 */
int capability_revoke(int fd);

#endif /* _MINIX_CAPABILITY_H */
