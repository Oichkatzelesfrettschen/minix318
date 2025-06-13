/**
 * @file kernel_constants.h
 * @brief MINIX4 Kernel Constants and Configuration
 * @details System-wide constants and configuration parameters
 * @version 4.0.0
 * @date 2025-06-13
 */

#pragma once

#define _POSIX_C_SOURCE 202311L

#include <stdint.h>
#include <stddef.h>

/*===========================================================================*
 *                        SYSTEM LIMITS AND CONSTANTS                       *
 *===========================================================================*/

/* Process and task limits */
#define NR_TASKS            32      /**< Number of system tasks */
#define NR_PROCS            256     /**< Number of user processes */
#define NR_TOTAL_PROCS      (NR_TASKS + NR_PROCS)
#define MAX_PROC_NAME       16      /**< Maximum process name length */
#define PROC_STACK_SIZE     (8192)  /**< Default process stack size */

/* Memory management constants */
#define PAGE_SIZE           4096    /**< System page size */
#define PAGE_SHIFT          12      /**< log2(PAGE_SIZE) */
#define PAGE_MASK           (PAGE_SIZE - 1)
#define KERNEL_STACK_SIZE   (16384) /**< Kernel stack size per process */
#define MAX_PHYS_MEMORY     (16ULL * 1024 * 1024 * 1024) /**< 16GB max */

/* IPC and messaging constants */
#define IPC_MAX_MESSAGE_SIZE    1024    /**< Maximum IPC message size */
#define IPC_MAX_QUEUE_SIZE      64      /**< Maximum queued messages */
#define IPC_TIMEOUT_INFINITE    UINT64_MAX
#define IPC_TIMEOUT_DEFAULT_MS  5000    /**< Default IPC timeout */

/* File system constants */
#define MAX_PATH_LENGTH     1024    /**< Maximum path length */
#define MAX_FILENAME_LENGTH 255     /**< Maximum filename length */
#define MAX_OPEN_FILES      1024    /**< Maximum open files per process */
#define MAX_INODES          4096    /**< Maximum inodes in system */

/* Network constants */
#define MAX_SOCKETS         1024    /**< Maximum sockets per process */
#define MAX_PACKET_SIZE     1500    /**< Maximum network packet size */
#define TCP_BUFFER_SIZE     (64 * 1024) /**< TCP buffer size */

/* Device and driver constants */
#define MAX_DEVICES         256     /**< Maximum device count */
#define MAX_DEVICE_NAME     32      /**< Maximum device name length */
#define IRQ_VECTOR_COUNT    256     /**< Number of IRQ vectors */

/* Scheduling constants */
#define SCHEDULER_HZ        100     /**< Scheduler frequency (Hz) */
#define TIME_SLICE_MS       10      /**< Default time slice (milliseconds) */
#define PRIORITY_LEVELS     8       /**< Number of priority levels */
#define IDLE_PRIORITY       7       /**< Idle process priority */

/* Security and capability constants */
#define MAX_CAPABILITIES    64      /**< Maximum capability bits */
#define SECURITY_LEVELS     4       /**< Number of security levels */

/*===========================================================================*
 *                          ARCHITECTURE CONSTANTS                          *
 *===========================================================================*/

/* x86-64 specific constants */
#if defined(ARCH_X86_64)
#define ARCH_POINTER_SIZE   8
#define ARCH_CACHE_LINE     64
#define ARCH_PAGE_LEVELS    4
#define ARCH_VIRT_BITS      48
#define ARCH_PHYS_BITS      52
#define KERNEL_VIRTUAL_BASE 0xFFFFFFFF80000000UL
#define USER_VIRTUAL_BASE   0x0000000000400000UL

/* x86-32 specific constants */
#elif defined(ARCH_I386)
#define ARCH_POINTER_SIZE   4
#define ARCH_CACHE_LINE     32
#define ARCH_PAGE_LEVELS    2
#define ARCH_VIRT_BITS      32
#define ARCH_PHYS_BITS      36  /* With PAE */
#define KERNEL_VIRTUAL_BASE 0x80000000UL
#define USER_VIRTUAL_BASE   0x00400000UL

/* ARM specific constants */
#elif defined(ARCH_ARM)
#define ARCH_POINTER_SIZE   4
#define ARCH_CACHE_LINE     32
#define ARCH_PAGE_LEVELS    2
#define ARCH_VIRT_BITS      32
#define ARCH_PHYS_BITS      32
#define KERNEL_VIRTUAL_BASE 0x80000000UL
#define USER_VIRTUAL_BASE   0x00008000UL

/* AArch64 specific constants */
#elif defined(ARCH_AARCH64)
#define ARCH_POINTER_SIZE   8
#define ARCH_CACHE_LINE     64
#define ARCH_PAGE_LEVELS    3
#define ARCH_VIRT_BITS      48
#define ARCH_PHYS_BITS      48
#define KERNEL_VIRTUAL_BASE 0xFFFF800000000000UL
#define USER_VIRTUAL_BASE   0x0000000000400000UL

#else
#error "Unsupported architecture"
#endif

/*===========================================================================*
 *                            MAGIC NUMBERS                                 *
 *===========================================================================*/

#define MINIX4_MAGIC        0x4D494E58  /**< "MINX" */
#define PROC_MAGIC          0x50524F43  /**< "PROC" */
#define MESSAGE_MAGIC       0x4D534721  /**< "MSG!" */
#define MEMORY_MAGIC        0x4D454D21  /**< "MEM!" */
#define IPC_MAGIC           0x49504321  /**< "IPC!" */
#define VFS_MAGIC           0x56465321  /**< "VFS!" */

/*===========================================================================*
 *                           ERROR CODES                                    *
 *===========================================================================*/

/* MINIX4 specific error codes (negative values) */
#define MINIX_OK            0       /**< Success */
#define MINIX_EINVAL        -22     /**< Invalid argument */
#define MINIX_ENOMEM        -12     /**< Out of memory */
#define MINIX_ENOSYS        -38     /**< Function not implemented */
#define MINIX_EPERM         -1      /**< Operation not permitted */
#define MINIX_ENOENT        -2      /**< No such file or directory */
#define MINIX_ESRCH         -3      /**< No such process */
#define MINIX_EINTR         -4      /**< Interrupted system call */
#define MINIX_EIO           -5      /**< I/O error */
#define MINIX_EAGAIN        -11     /**< Try again */
#define MINIX_EDEADLK       -35     /**< Resource deadlock would occur */
#define MINIX_ETIMEDOUT     -110    /**< Connection timed out */

/*===========================================================================*
 *                        COMPILE-TIME ASSERTIONS                           *
 *===========================================================================*/

/* Verify critical assumptions at compile time */
_Static_assert(PAGE_SIZE >= 4096, "Page size must be at least 4KB");
_Static_assert((PAGE_SIZE & PAGE_MASK) == 0, "Page size must be power of 2");
_Static_assert(NR_TOTAL_PROCS <= 65536, "Too many processes for PID space");
_Static_assert(IPC_MAX_MESSAGE_SIZE >= 64, "IPC messages too small");
_Static_assert(KERNEL_STACK_SIZE >= 8192, "Kernel stack too small");
_Static_assert(sizeof(void*) == ARCH_POINTER_SIZE, "Pointer size mismatch");

/*===========================================================================*
 *                            UTILITY MACROS                                *
 *===========================================================================*/

/* Alignment macros */
#define ALIGN_UP(x, align)      (((x) + (align) - 1) & ~((align) - 1))
#define ALIGN_DOWN(x, align)    ((x) & ~((align) - 1))
#define IS_ALIGNED(x, align)    (((x) & ((align) - 1)) == 0)

/* Array size macro */
#define ARRAY_SIZE(arr)         (sizeof(arr) / sizeof((arr)[0]))

/* Min/Max macros */
#define MIN(a, b)               ((a) < (b) ? (a) : (b))
#define MAX(a, b)               ((a) > (b) ? (a) : (b))
#define CLAMP(x, min, max)      (MIN(MAX(x, min), max))

/* Bit manipulation macros */
#define BIT(n)                  (1UL << (n))
#define BITMASK(bits)           (BIT(bits) - 1)
#define SET_BIT(var, bit)       ((var) |= BIT(bit))
#define CLEAR_BIT(var, bit)     ((var) &= ~BIT(bit))
#define TEST_BIT(var, bit)      (((var) & BIT(bit)) != 0)

/* Container of macro */
#define CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

/* Likely/unlikely hints for branch prediction */
#ifdef __GNUC__
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif

/* Function attributes */
#define NORETURN            __attribute__((noreturn))
#define UNUSED              __attribute__((unused))
#define PACKED              __attribute__((packed))
#define ALIGNED(x)          __attribute__((aligned(x)))
#define SECTION(s)          __attribute__((section(s)))

/*===========================================================================*
 *                        CONFIGURATION FLAGS                               *
 *===========================================================================*/

/* Kernel configuration options */
#define CONFIG_SMP              1   /**< Enable SMP support */
#define CONFIG_PREEMPTION       1   /**< Enable kernel preemption */
#define CONFIG_VIRTUAL_MEMORY   1   /**< Enable virtual memory */
#define CONFIG_CAPABILITIES     1   /**< Enable capability security */
#define CONFIG_DEBUGGING        1   /**< Enable debug features */
#define CONFIG_STATISTICS       1   /**< Enable runtime statistics */
#define CONFIG_PROFILING        0   /**< Enable profiling support */

/* Memory configuration */
#define CONFIG_KERNEL_HEAP_SIZE (16 * 1024 * 1024)  /**< 16MB kernel heap */
#define CONFIG_DMA_POOL_SIZE    (4 * 1024 * 1024)   /**< 4MB DMA pool */
#define CONFIG_PAGE_CACHE_SIZE  (64 * 1024 * 1024)  /**< 64MB page cache */

/* Debug configuration */
#ifdef DEBUG
#define CONFIG_ASSERT_ENABLED   1
#define CONFIG_DEBUG_SYMBOLS    1
#define CONFIG_STACK_CANARIES   1
#define CONFIG_MEMORY_DEBUG     1
#else
#define CONFIG_ASSERT_ENABLED   0
#define CONFIG_DEBUG_SYMBOLS    0
#define CONFIG_STACK_CANARIES   0
#define CONFIG_MEMORY_DEBUG     0
#endif

/* Version information */
#define MINIX4_VERSION_MAJOR    4
#define MINIX4_VERSION_MINOR    0
#define MINIX4_VERSION_PATCH    0
#define MINIX4_VERSION_STRING   "4.0.0"
#define MINIX4_BUILD_DATE       __DATE__
#define MINIX4_BUILD_TIME       __TIME__
