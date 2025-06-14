#ifndef _MINIX_KERNEL_TYPES_H
#define _MINIX_KERNEL_TYPES_H

/*
 * This file consolidates kernel-specific type definitions.
 * It is intended to replace userspace types like size_t, pid_t, etc.
 * within the kernel.
 */

/* Integer types for sizes and results */
typedef unsigned long k_size_t; /* For object sizes, counts */
typedef long
    k_ssize_t;        /* For results that can be negative (e.g., error codes) */
typedef long k_off_t; /* For file offsets */

/* Process and user/group identifiers */
typedef int k_pid_t;          /* Process ID */
typedef unsigned int k_uid_t; /* User ID */
typedef unsigned int k_gid_t; /* Group ID */

/* Time-related types */
typedef long k_time_t;  /* For time in seconds */
typedef long k_clock_t; /* For system clock ticks or higher resolution time */

/* Signal types - simplified for kernel use */
typedef unsigned long k_sigset_t; /* For signal sets */

/* Fixed-width integer types */
/* These are crucial for ensuring type sizes across different parts of the
 * kernel */
/* and for interfacing with hardware or specific data structures. */

/* Unsigned fixed-width types */
typedef unsigned char k_uint8_t;
typedef unsigned short k_uint16_t;
typedef unsigned int k_uint32_t;
typedef unsigned long long k_uint64_t;

/* Signed fixed-width types */
typedef signed char k_int8_t;
typedef signed short k_int16_t;
typedef signed int k_int32_t;
typedef signed long long k_int64_t;

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

/* Atomic types (architecture might need to provide actual atomic operations) */
typedef struct {
  volatile long counter;
} __attribute__((
    aligned(8))) k_atomic_t; /* Generic atomic type, often for counters */

typedef struct {
    volatile k_int64_t counter; // Changed to k_int64_t for true 64-bit atomic
                           // Could also be arch-specific size.
} __attribute__((aligned(8))) k_atomic_long_t; /* Explicitly long atomic type */


/**
 * @struct k_exec_info_s
 * @brief Kernel-internal information about a program to be executed.
 *
 * This structure holds essential parameters that the kernel part of exec
 * needs, such as entry point, stack pointer, and segment information
 * for the new process image. It also includes the program name.
 */
typedef struct k_exec_info_s {
    k_vaddr_t stack_top;        /**< Top of the initial user stack (highest address). */
    k_vaddr_t initial_eip;      /**< Initial instruction pointer (entry point of the program). */

    k_vaddr_t text_vaddr;       /**< Virtual address of the text segment start. */
    k_size_t  text_size;        /**< Size of the text segment in bytes. */

    k_vaddr_t data_vaddr;       /**< Virtual address of the data segment start. */
    k_size_t  data_size;        /**< Total size of the data segment (data + bss) in bytes. */

    k_vaddr_t bss_start_vaddr;  /**< Virtual address where the BSS section starts (within data segment). */
    k_size_t  bss_size;         /**< Size of the BSS section in bytes. */

    /**
     * @brief Program name, typically the full path of the executable.
     * Truncated if longer than the buffer. Using a common default length.
     * A kernel-specific PATH_MAX or NAME_MAX would be preferable if available.
     * PROC_NAME_LEN is often too short for paths.
     */
    char prog_name[256];

    // Future considerations for more detailed exec parameters passed from PM to kernel:
    // k_vaddr_t user_argv_ptr;   /**< Pointer to argv array in user space of the new process image. */
    // k_vaddr_t user_envp_ptr;   /**< Pointer to envp array in user space of the new process image. */
    // k_size_t  initial_stack_frame_size; /**< Size of initial arguments/environment copied to the user stack. */

} k_exec_info_t;


/* Opaque handle types (forward declarations) */
/* These declare pointer types to incomplete structs, hiding implementation
 * details. */
/* The actual struct definitions would be elsewhere (e.g., proc.h, thread.h). */
typedef struct k_proc_handle *k_proc_handle_t;
typedef struct k_thread_handle *k_thread_handle_t;
typedef struct k_mem_region_handle
    *k_mem_region_handle_t; // Renamed from k_mem_handle for clarity

/* Memory alignment macros */
#define K_CACHE_LINE_SIZE                                                      \
  64 /* Common cache line size, may need arch-specific versions */
#define __k_cacheline_aligned __attribute__((aligned(K_CACHE_LINE_SIZE)))

/* Offset calculation without stdlib (for C11 _Static_assert might need careful
 * use with this) */
/* This macro is generally safe for standard-layout types. */
#define K_OFFSETOF(type, member) ((k_size_t)&((type *)0)->member)

/* Kernel-specific signal set manipulation macros */
/* Assumes signo is 1-based and fits within unsigned long bitmask. */
#define k_sigemptyset(set)    (*(set) = 0UL)
#define k_sigaddset(set, signo)   (*(set) |= (1UL << ((signo) - 1)))
#define k_sigdelset(set, signo)   (*(set) &= ~(1UL << ((signo) - 1))) /* Added for completeness */
#define k_sigismember(set, signo) ((*(set) & (1UL << ((signo) - 1))) != 0)
#define k_sigisempty(set)      (*(set) == 0UL) /* Added for completeness */

/* Kernel-specific signal set manipulation macros */
/* Assumes signo is 1-based and fits within unsigned long bitmask. */
#define k_sigemptyset(set)    (*(set) = 0UL)
#define k_sigaddset(set, signo)   (*(set) |= (1UL << ((signo) - 1)))
#define k_sigdelset(set, signo)   (*(set) &= ~(1UL << ((signo) - 1))) /* Added for completeness */
#define k_sigismember(set, signo) ((*(set) & (1UL << ((signo) - 1))) != 0)
#define k_sigisempty(set)      (*(set) == 0UL) /* Added for completeness */

/* Kernel-specific signal set manipulation macros */
/* Assumes signo is 1-based and fits within unsigned long bitmask. */
#define k_sigemptyset(set)    (*(set) = 0UL)
#define k_sigaddset(set, signo)   (*(set) |= (1UL << ((signo) - 1)))
#define k_sigdelset(set, signo)   (*(set) &= ~(1UL << ((signo) - 1))) /* Added for completeness */
#define k_sigismember(set, signo) ((*(set) & (1UL << ((signo) - 1))) != 0)
#define k_sigisempty(set)      (*(set) == 0UL) /* Added for completeness */

#endif /* _MINIX_KERNEL_TYPES_H */
