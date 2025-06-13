/**
 * @file config_minix4.h
 * @brief MINIX4 Configuration Header
 * @version 4.0.0
 * @date 2025-06-12
 */

#pragma once

// MINIX4 version information
#define MINIX4_VERSION_MAJOR    4
#define MINIX4_VERSION_MINOR    0
#define MINIX4_VERSION_PATCH    0
#define MINIX4_VERSION_STRING   "4.0.0-C23"
#define MINIX4_BOOT_MAGIC       0x4D494E5834UL  // "MINX4"

// Feature configuration
#define CONFIG_C23              1    /**< Enable C23 features */
#define CONFIG_SMP              1    /**< Symmetric multiprocessing */
#define CONFIG_CAPABILITY       1    /**< Capability-based security */
#define CONFIG_EXOKERNEL        1    /**< Exokernel features */
#define CONFIG_FAST_IPC         1    /**< Fast IPC mechanisms */
#define CONFIG_NETWORKING       0    /**< Network subsystem (optional) */
#define CONFIG_DEBUGGING        1    /**< Debug features */

// Architecture configuration
#if defined(__x86_64__)
    #define CONFIG_ARCH_X86_64  1
    #define CONFIG_ARCH_NAME    "x86_64"
#elif defined(__aarch64__)
    #define CONFIG_ARCH_AARCH64 1
    #define CONFIG_ARCH_NAME    "aarch64"
#elif defined(__riscv)
    #define CONFIG_ARCH_RISCV   1
    #define CONFIG_ARCH_NAME    "riscv"
#else
    #error "Unsupported architecture"
#endif

// Memory configuration
#define CONFIG_PAGE_SIZE        4096
#define CONFIG_KERNEL_STACK     8192
#define CONFIG_USER_STACK       8192
#define CONFIG_MAX_MEMORY       (4ULL * 1024 * 1024 * 1024)  // 4GB max

// Process configuration
#define CONFIG_MAX_PROCESSES    1024
#define CONFIG_MAX_THREADS      4096
#define CONFIG_MAX_FILES        65536

// IPC configuration
#define CONFIG_MAX_ENDPOINTS    2048
#define CONFIG_MAX_MESSAGE_SIZE 65536
#define CONFIG_FAST_IPC_BUFFER  (1024 * 1024)  // 1MB

// Capability configuration
#define CONFIG_MAX_CAPABILITIES 16384
#define CONFIG_CAP_DEPTH        32

// Security configuration
#define CONFIG_STACK_PROTECTION 1
#define CONFIG_HEAP_PROTECTION  1
#define CONFIG_ASLR             1    /**< Address space layout randomization */

// Debug configuration
#ifdef DEBUG
    #define CONFIG_ASSERTIONS   1
    #define CONFIG_DEBUG_LOCKS  1
    #define CONFIG_DEBUG_MEMORY 1
    #define CONFIG_STACK_TRACE  1
#else
    #define CONFIG_ASSERTIONS   0
    #define CONFIG_DEBUG_LOCKS  0
    #define CONFIG_DEBUG_MEMORY 0
    #define CONFIG_STACK_TRACE  0
#endif

// Compiler-specific configuration
#if defined(__clang__)
    #define COMPILER_CLANG      1
    #define COMPILER_NAME       "Clang"
#elif defined(__GNUC__)
    #define COMPILER_GCC        1
    #define COMPILER_NAME       "GCC"
#else
    #warning "Unknown compiler"
    #define COMPILER_UNKNOWN    1
    #define COMPILER_NAME       "Unknown"
#endif

// C23 feature detection
#if __STDC_VERSION__ >= 202311L
    #define HAS_C23             1
    #define HAS_TYPEOF          1
    #define HAS_DECIMAL_FP      1
    #define HAS_BINARY_LITERALS 1
#else
    #define HAS_C23             0
    #warning "C23 support recommended for full feature set"
#endif

// Attribute macros for better portability
#define ALIGNED(n)              __attribute__((aligned(n)))
#define PACKED                  __attribute__((packed))
#define UNUSED                  __attribute__((unused))
#define USED                    __attribute__((used))
#define SECTION(s)              __attribute__((section(s)))
#define LIKELY(x)               __builtin_expect(!!(x), 1)
#define UNLIKELY(x)             __builtin_expect(!!(x), 0)

// Optimization hints
#define ALWAYS_INLINE           __attribute__((always_inline)) inline
#define NEVER_INLINE            __attribute__((noinline))
#define PURE_FUNCTION           __attribute__((pure))
#define CONST_FUNCTION          __attribute__((const))

// Safety macros
#define ARRAY_SIZE(arr)         (sizeof(arr) / sizeof((arr)[0]))
#define CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

// Build configuration
#define BUILD_DATE              __DATE__
#define BUILD_TIME              __TIME__

#if CONFIG_DEBUGGING
    #define BUILD_TYPE          "Debug"
#else
    #define BUILD_TYPE          "Release"
#endif

// Runtime checks
#if CONFIG_ASSERTIONS
    #define STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)
#else
    #define STATIC_ASSERT(cond, msg)
#endif

// Memory barriers and synchronization
#define MEMORY_BARRIER()        __sync_synchronize()
#define READ_BARRIER()          __asm__ volatile("" ::: "memory")
#define WRITE_BARRIER()         __asm__ volatile("" ::: "memory")

// Cache line alignment
#define CACHE_LINE_SIZE         64
#define CACHE_ALIGNED           ALIGNED(CACHE_LINE_SIZE)

// Error codes
#define MINIX4_SUCCESS          0
#define MINIX4_ERROR            -1
#define MINIX4_NOMEM            -2
#define MINIX4_INVALID          -3
#define MINIX4_PERMISSION       -4
#define MINIX4_TIMEOUT          -5

// Validation of configuration
STATIC_ASSERT(CONFIG_PAGE_SIZE >= 4096, "Page size must be at least 4KB");
STATIC_ASSERT(CONFIG_MAX_PROCESSES > 0, "Must support at least one process");
STATIC_ASSERT(CONFIG_MAX_MESSAGE_SIZE <= 1048576, "Message size too large");

#if CONFIG_SMP && CONFIG_MAX_PROCESSES < 2
    #error "SMP requires support for multiple processes"
#endif

#if CONFIG_CAPABILITY && CONFIG_MAX_CAPABILITIES == 0
    #error "Capability system requires non-zero capability limit"
#endif
