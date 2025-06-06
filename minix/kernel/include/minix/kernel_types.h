#ifndef _MINIX_KERNEL_TYPES_H
#define _MINIX_KERNEL_TYPES_H

/*
 * This file consolidates kernel-specific type definitions.
 * It is intended to replace userspace types like size_t, pid_t, etc.
 * within the kernel.
 */

/* Integer types for sizes and results */
typedef unsigned long k_size_t;   /* For object sizes, counts */
typedef long          k_ssize_t;  /* For results that can be negative (e.g., error codes) */
typedef long          k_off_t;    /* For file offsets */

/* Process and user/group identifiers */
typedef int           k_pid_t;    /* Process ID */
typedef unsigned int  k_uid_t;    /* User ID */
typedef unsigned int  k_gid_t;    /* Group ID */

/* Time-related types */
typedef long          k_time_t;   /* For time in seconds */
typedef long          k_clock_t;  /* For system clock ticks or higher resolution time */

/* Signal types - simplified for kernel use */
typedef unsigned long k_sigset_t; /* For signal sets */

/* Fixed-width integer types */
/* These are crucial for ensuring type sizes across different parts of the kernel */
/* and for interfacing with hardware or specific data structures. */

/* Unsigned fixed-width types */
typedef unsigned char  k_uint8_t;
typedef unsigned short k_uint16_t;
typedef unsigned int   k_uint32_t;
typedef unsigned long long k_uint64_t;

/* Signed fixed-width types */
typedef signed char    k_int8_t;
typedef signed short   k_int16_t;
typedef signed int     k_int32_t;
typedef signed long long   k_int64_t;

/* Architecture-specific types */
#if defined(__i386__)
typedef unsigned int k_paddr_t; /* Physical address type for i386 */
typedef unsigned int k_vaddr_t; /* Virtual address type for i386 */
#elif defined(__x86_64__)
typedef unsigned long k_paddr_t; /* Physical address type for x86_64 */
typedef unsigned long k_vaddr_t; /* Virtual address type for x86_64 */
#else
#error "Unsupported architecture for k_paddr_t and k_vaddr_t"
#endif

#endif /* _MINIX_KERNEL_TYPES_H */
