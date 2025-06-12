#ifndef _MINIX_KLIB_H
#define _MINIX_KLIB_H

#include <kassert_metrics.h>    // For assertion metrics collection functions
#include <kerrno.h>             // For K_EPERM, K_EINVAL etc.
#include <ksignal.h>            // For K_SIGHUP, K_SIGINT etc.
#include <kernel_types.h> // For k_size_t
#include <stdint.h>             // For uint32_t, etc.

// Define kbool as _Bool from C23
typedef _Bool kbool;

/**
 * @struct kcpu_features_t
 * @brief Holds detected CPU features.
 *
 * This structure is populated at kernel startup with information about
 * the CPU's capabilities.
 */
typedef struct kcpu_features_t {
  /** @brief True if FPU (x87 Floating Point Unit) is present. */
  kbool fpu;
  /** @brief True if MMX (MultiMedia eXtensions) is present. */
  kbool mmx;
  /** @brief True if SSE (Streaming SIMD Extensions) is present. */
  kbool sse;
  /** @brief True if SSE2 (Streaming SIMD Extensions 2) is present. */
  kbool sse2;
  /** @brief True if SSE3 (Streaming SIMD Extensions 3) is present. */
  kbool sse3;
  /** @brief True if SSSE3 (Supplemental Streaming SIMD Extensions 3) is
   * present. */
  kbool ssse3;
  /** @brief True if SSE4.1 (Streaming SIMD Extensions 4.1) is present. */
  kbool sse4_1;
  /** @brief True if SSE4.2 (Streaming SIMD Extensions 4.2) is present. */
  kbool sse4_2;

  // Cache sizes
  /** @brief L1 Data Cache size in bytes. */
  uint32_t l1_data_cache_size;
  /** @brief L1 Instruction Cache size in bytes. */
  uint32_t l1_instruction_cache_size;
  /** @brief L2 Cache size in bytes. */
  uint32_t l2_cache_size;
  /** @brief L3 Cache size in bytes. */
  uint32_t l3_cache_size;

  /** @brief True if AVX (Advanced Vector Extensions) is present. */
  kbool avx;
  /** @brief True if F16C (half-precision floating-point conversion) is present.
   */
  kbool f16c;
  /** @brief True if PCLMULQDQ (carry-less multiplication) is present. */
  kbool pclmulqdq;
  /** @brief True if AESNI (AES New Instructions) is present. */
  kbool aesni;
  /** @brief True if VAES (Vector AES) instructions are present. */
  kbool vaes;
  /** @brief True if GFNI (Galois Field New Instructions) is present. */
  kbool gfni;
  /** @brief True if POPCNT (population count) instruction is present. */
  kbool popcnt;
  /** @brief True if VPOPCNTDQ (vector population count) is present. */
  kbool vpopcntdq;

  /** @brief True if BMI1 (Bit Manipulation Instructions 1) is present. */
  kbool bmi1;
  /** @brief True if BMI2 (Bit Manipulation Instructions 2) is present. */
  kbool bmi2;
  /** @brief True if LZCNT (leading zero count) instruction is present. */
  kbool lzcnt;
  /** @brief True if ADX (Multi-Precision Add-Carry Instruction Extensions) is
   * present. */
  kbool adx;
  /** @brief True if SHA (SHA Extensions) is present. */
  kbool sha;

  /** @brief True if AVX512F (AVX-512 Foundation) is present. */
  kbool avx512f;
  /** @brief True if AVX512DQ (AVX-512 Doubleword and Quadword Instructions) is
   * present. */
  kbool avx512dq;
  /** @brief True if AVX512CD (AVX-512 Conflict Detection Instructions) is
   * present. */
  kbool avx512cd;
  /** @brief True if AVX512BW (AVX-512 Byte and Word Instructions) is present.
   */
  kbool avx512bw;
  /** @brief True if AVX512VL (AVX-512 Vector Length Extensions) is present. */
  kbool avx512vl;
  /** @brief True if AVX512VNNI (AVX-512 Vector Neural Network Instructions) is
   * present. */
  kbool avx512vnni;
  /** @brief True if AVX512_IFMA (AVX-512 Integer Fused Multiply-Add) is
   * present. */
  kbool avx512_ifma;
  /** @brief True if AVX512_VBMI (AVX-512 Vector Bit Manipulation Instructions)
   * is present. */
  kbool avx512_vbmi;
  /** @brief True if AVX512_VBMI2 (AVX-512 Vector Bit Manipulation Instructions
   * 2) is present. */
  kbool avx512_vbmi2;
  /** @brief True if BITALG (Bit Algorithm instructions like VPOPCNTB/W,
   * VPSHUFBITQMB) is present. */
  kbool bitalg;
  /** @brief True if AVX512_VP2INTERSECT (AVX-512 Vector Pair Intersection to a
   * Mask) is present. */
  kbool avx512_vp2intersect;
  /** @brief True if AVX512_BF16 (AVX-512 BFloat16 Instructions) is present. */
  kbool avx512_bf16;
  /** @brief True if AVX512_FP16 (AVX-512 Half-Precision Floating-Point
   * Instructions) is present. */
  kbool avx512_fp16;

  /** @brief True if XOP (eXtended Operations) instructions are present (AMD).
   */
  kbool xop;
  /** @brief True if TBM (Trailing Bit Manipulation) instructions are present
   * (AMD). */
  kbool tbm;

} kcpu_features_t;

/**
 * @brief Global instance of the CPU features structure.
 *
 * This variable is populated by kcpu_detect_features_arch() at startup
 * and holds the feature flags for the currently running CPU.
 * It is declared here as extern and defined in an architecture-specific
 * C file (e.g., kcpu_detect_features_arch.c or kcore.c).
 */
extern kcpu_features_t g_kcpu_features;

/**
 * @brief Halts the system after printing a panic message.
 *
 * This function is called when an unrecoverable kernel error occurs.
 * It disables interrupts, prints a panic message prefixed with "KERNEL PANIC:
 * ", and then halts the system. This function does not return.
 *
 * @param format The format string for the panic message. Must not be NULL.
 * @param ... Variadic arguments for the format string (passed to kprintf_stub).
 * @note C23 compliant. Asserts format is not NULL. Uses kprintf_stub for
 * output.
 */
void kpanic(const char *restrict format, ...) __attribute__((noreturn));

/**
 * @brief Sets the first n bytes of the memory area pointed to by s to the
 * specified value c.
 * @param s Pointer to the memory area.
 * @param c Value to be set (converted to unsigned char).
 * @param n Number of bytes to be set.
 * @return A pointer to the memory area s.
 * @note C23 compliant version. Asserts s is not NULL.
 */
void *kmemset_c23(void *s, int c, k_size_t n);

/**
 * @brief Copies n bytes from memory area src to memory area dest.
 *
 * The memory areas may overlap; the copy is done in a non-destructive manner.
 * Uses 'restrict' keyword as per C23 for optimization hints.
 *
 * @param dest Pointer to the destination memory area. Must not be NULL.
 * @param src Pointer to the source memory area. Must not be NULL.
 * @param n Number of bytes to copy.
 * @return A pointer to the destination memory area dest.
 * @note C23 compliant version. Asserts dest and src are not NULL.
 */
void *kmemcpy_c23(void *restrict dest, const void *restrict src, k_size_t n);

/**
 * @brief Compares the first n bytes of the memory areas s1 and s2.
 *
 * @param s1 Pointer to the first memory area. Must not be NULL.
 * @param s2 Pointer to the second memory area. Must not be NULL.
 * @param n Number of bytes to compare.
 * @return An integer less than, equal to, or greater than zero if the first n
 *         bytes of s1 is found, respectively, to be less than, to match, or be
 *         greater than the first n bytes of s2.
 * @note C23 compliant version. Asserts s1 and s2 are not NULL.
 */
int kmemcmp_c23(const void *s1, const void *s2, k_size_t n);

/**
 * @brief Calculates the length of a null-terminated string.
 *
 * @param s Pointer to the null-terminated string. Must not be NULL.
 * @return The number of bytes in the string, excluding the null terminator.
 * @note C23 compliant version. Asserts s is not NULL.
 */
k_size_t kstrlen_c23(const char *s);

/**
 * @brief Compares two null-terminated strings lexicographically.
 *
 * @param s1 Pointer to the first null-terminated string. Must not be NULL.
 * @param s2 Pointer to the second null-terminated string. Must not be NULL.
 * @return An integer less than, equal to, or greater than zero if s1 is found,
 *         respectively, to be less than, to match, or be greater than s2.
 * @note C23 compliant version. Asserts s1 and s2 are not NULL.
 */
int kstrcmp_c23(const char *s1, const char *s2);

/**
 * @brief Compares up to n characters of two null-terminated strings
 * lexicographically.
 *
 * @param s1 Pointer to the first null-terminated string. Must not be NULL.
 * @param s2 Pointer to the second null-terminated string. Must not be NULL.
 * @param n Maximum number of characters to compare.
 * @return An integer less than, equal to, or greater than zero if the first n
 *         bytes of s1 (or fewer if a null terminator is encountered) is found,
 *         respectively, to be less than, to match, or be greater than s2.
 * @note C23 compliant version. Asserts s1 and s2 are not NULL. Handles n=0
 * correctly.
 */
int kstrncmp_c23(const char *s1, const char *s2, k_size_t n);

/**
 * @brief Copies a string to a fixed-size buffer, ensuring null termination.
 *
 * Copies up to 'size - 1' characters from the NUL-terminated string 'src'
 * to 'dest', NUL-terminating the result.
 *
 * @param dest Pointer to the destination buffer. Must not be NULL.
 * @param src Pointer to the source NUL-terminated string. Must not be NULL.
 * @param size The total size of the destination buffer 'dest'.
 * @return The total length of the string that was attempted to create;
 *         that is the length of 'src'. If the return value is >= size,
 *         truncation occurred.
 * @note C23 compliant version. Asserts dest and src are not NULL. Handles
 * size=0 correctly. Uses kstrlen_c23 and kmemcpy_c23 internally.
 */
k_size_t kstrlcpy_c23(char *restrict dest, const char *restrict src,
                      k_size_t size);

/**
 * @brief Converts a string to an integer.
 *
 * Parses the string 'str', interpreting its content as an integral number,
 * which is returned as a value of type int. Handles leading whitespace and
 * an optional sign. Uses checked arithmetic to detect overflow.
 *
 * @param str The string to convert. Must not be NULL.
 * @return The converted integer value.
 * @note C23 compliant version. Asserts str is not NULL. Calls kpanic on
 * overflow.
 */
int katoi_c23(const char *str);

/**
 * @brief Converts a string to an unsigned long integer.
 *
 * Parses the string 'str', interpreting its content as an integral number of
 * the specified 'base', which is returned as a value of type unsigned long.
 * Handles leading whitespace and optional "0x" or "0X" prefix for base 16.
 *
 * @param str The string to convert. Must not be NULL.
 * @param endptr If not NULL, stores the address of the first invalid character
 * in *endptr.
 * @param base The base for conversion (e.g., 10 for decimal, 16 for
 * hexadecimal). If base is 0, the base is determined from the string's prefix.
 *             Supported bases are 0, 8, 10, 16.
 * @return The converted unsigned long value.
 * @note C23 compliant version. Asserts str is not NULL. Calls kpanic on
 * overflow or invalid base.
 */
unsigned long kstrtoul_c23(const char *restrict str, char **restrict endptr,
                           int base);

/**
 * @brief Kernel printf stub for early debug messages.
 *
 * This function provides a basic printing capability for use before the
 * full kprintf is available. Its actual implementation might be in
 * a kernel debug module (e.g., kdebug.c).
 *
 * @param format The format string. Must not be NULL.
 * @param ... Variadic arguments (currently ignored by the basic stub).
 * @return The number of characters in the format string (notionally printed),
 * or 0.
 * @note C23 compliant. Asserts format is not NULL.
 *       This initial stub version prints only the format string itself.
 */
int kprintf_stub(const char *restrict format, ...);

// --- Capability DAG Test Runner ---
/**
 * @brief Runs mathematical/validation tests on the capability DAG
 * implementation.
 *
 * This function is intended for use during kernel development and testing
 * phases. It executes a series of predefined test cases to verify the
 * correctness of DAG operations, including node/edge creation, cycle detection,
 * rights propagation, and security policy enforcement. Test results are
 * typically printed to the kernel console via kprintf_stub.
 *
 * @note This function should be called from a controlled test environment
 * within the kernel, usually during early initialization.
 */
void kcapability_dag_run_mathematical_tests(void);

#endif /* _MINIX_KLIB_H */
/* minix/lib/klib/include/klib.h - The Pure Kernel Library Foundation */

#ifndef _KLIB_H
#define _KLIB_H

/*
 * KLIB: Pure Kernel Library for MINIX Microkernel
 *
 * This library provides kernel-space optimized implementations of
 * essential functions without any userspace assumptions. Every
 * function here is designed for:
 *
 * 1. Kernel space operation (no system calls)
 * 2. Hardware optimization (cache-aware, SIMD where appropriate)
 * 3. Mathematical correctness (formal verification friendly)
 * 4. Zero external dependencies (self-contained)
 */

/* Kernel-specific types - no dependency on userspace types */
/* Using __SIZE_TYPE__ and other builtins for architecture neutrality where
 * possible */
typedef __SIZE_TYPE__ ksize_t;
typedef __UINTPTR_TYPE__ kuintptr_t;
typedef __INTPTR_TYPE__ kintptr_t;
typedef __PTRDIFF_TYPE__ kssize_t; /* For differences between pointers */
typedef unsigned int kuint_t;      /* General purpose unsigned int */
typedef int kint_t;                /* General purpose signed int */
typedef unsigned char ku8_t;
typedef unsigned short ku16_t;
typedef unsigned int ku32_t;
typedef unsigned long long ku64_t;
typedef signed char ki8_t;
typedef signed short ki16_t;
typedef signed int ki32_t;
typedef signed long long ki64_t;

/* Boolean type for clarity */
typedef enum { kfalse = 0, ktrue = 1 } kbool_t;
#define KNULL ((void *)0)

/* Hardware-aware CPU feature detection */
typedef struct kcpu_features {
  kbool_t has_sse;
  kbool_t has_sse2;
  kbool_t has_avx;
  kbool_t has_avx2;
  kbool_t has_avx512f; /* AVX-512 Foundation */
  /* Add other specific AVX-512 features as needed, e.g., avx512dq, avx512bw */
  kint_t cache_line_size;
  kint_t l1d_cache_size; /* L1 Data Cache */
  kint_t l1i_cache_size; /* L1 Instruction Cache */
  kint_t l2_cache_size;
  kint_t l3_cache_size;
  /* Add more features: e.g., TSX, CET, MPX, NUMA info */
} kcpu_features_t;

/* Global CPU features - detected at kernel initialization */
extern kcpu_features_t g_kcpu_features;

/* Kernel assertions - more aggressive than userspace */
/* Note: kpanic needs to be implemented */
void kpanic(const char *fmt, ...); /* Declaration for KASSERT */

#ifdef KDEBUG
#define KASSERT(expr)                                                          \
  do {                                                                         \
    if (!(expr)) {                                                             \
      kpanic("KASSERT failed: %s at %s:%d", #expr, __FILE__,                   \
             (kint_t)__LINE__);                                                \
    }                                                                          \
  } while (kfalse)
#else
#define KASSERT(expr) ((void)kfalse)
#endif

#define KASSERT_ALIGNED(ptr, alignment)                                        \
  KASSERT(((kuintptr_t)(ptr) % (alignment)) == 0)

/* Core KLIB function declarations */

/* Memory manipulation */
void *kmemcpy(void *dest, const void *src, ksize_t n);
void *kmemset(void *s, kint_t c, ksize_t n);
kint_t kmemcmp(const void *s1, const void *s2, ksize_t n);
void *kmemmove(void *dest, const void *src,
               ksize_t n); /* For overlapping regions */

/* Hardware-optimized/specialized memory variants */
void *kmemcpy_fast(void *dest, const void *src,
                   ksize_t n); /* Chooses best non-overlapping */
void *kmemcpy_aligned(void *dest, const void *src,
                      ksize_t n);                    /* Assumes alignment */
void *kmemset_aligned(void *s, kint_t c, ksize_t n); /* Assumes alignment */

/* String manipulation */
ksize_t kstrlen(const char *s);
kint_t kstrcmp(const char *s1, const char *s2);
kint_t kstrncmp(const char *s1, const char *s2, ksize_t n);
char *kstrcpy(char *dest, const char *src);
char *kstrncpy(char *dest, const char *src, ksize_t n);
ksize_t kstrlcpy(char *dst, const char *src, ksize_t size);
char *kstrcat(char *dest, const char *src,
              ksize_t dst_size); /* Be careful with buffer sizes! */
char *kstrncat(char *dest, const char *src, ksize_t n);
const char *kstrchr(const char *s, kint_t c);
const char *kstrrchr(const char *s, kint_t c);

/* String/Number Conversion */
kint_t katoi(const char *str);
ku64_t kstrtoul(const char *str, char **endptr, kint_t base);

/* Formatted output - to be implemented carefully for kernel space */
/* kprintf might output to a kernel buffer or serial port */
void kprintf(const char *fmt, ...);
kint_t ksnprintf(char *buf, ksize_t size, const char *fmt, ...);
/* Add ksnprintf, kvsnprintf later if needed, ensuring they are safe */

/* Kernel utility functions */
void kdebug_print(const char *fmt, ...); /* For debug-only messages */

/* CPU feature detection function */
void kcpu_detect_features(void);

#endif /* _KLIB_H */
#ifndef MINIX_KLIB_H
#define MINIX_KLIB_H

#include <stdbool.h> // For bool type
#include <stddef.h>  // For size_t

// Memory functions
void *kmemcpy_c23(void *restrict dest, const void *restrict src, size_t n);
// void *kmemset_c23(void *dest, int c, size_t n); // Placeholder for future
// void *kmemmove_c23(void *dest, const void *src, size_t n); // Placeholder for
// future

// String functions
size_t kstrlen_c23(const char *s);
// char *kstrlcpy_c23(char * restrict dest, const char * restrict src, size_t
// size); // Placeholder for future int kstrcmp_c23(const char *s1, const char
// *s2); // Placeholder for future

// For Capability DAG functionalities, include <minix/kcapability_dag.h>
// directly. However, the test suite runner can be exposed via klib.h for
// convenience.
void kcapability_dag_run_mathematical_tests(void);

// Kernel utility functions (example declarations for self-containment)
// These might be defined in more specific kernel headers in a full system.

// Basic console output (stub version)
void kprintf_stub(const char *fmt,
                  ...); // Use kprintf_stub if kprintf is complex for now

// Kernel panic
void kpanic(const char *s);

// Memory allocation (example signatures)
void *kmalloc(size_t size);
void *kcalloc(size_t nmemb, size_t size);
void *krealloc(void *ptr, size_t new_size);
void kfree(void *ptr);

// KASSERT macro definition (example)
#ifdef NDEBUG
#define KASSERT(condition, message) ((void)0)
#else
#define KASSERT(condition, message)                                            \
  do {                                                                         \
    if (!(condition)) {                                                        \
      kprintf_stub("Assertion failed: %s, file %s, line %d: %s\n", #condition, \
                   __FILE__, __LINE__, message);                               \
      kpanic("KASSERT failed");                                                \
    }                                                                          \
  } while (0)
#endif // NDEBUG

#endif /* MINIX_KLIB_H */
/* minix/lib/klib/include/klib.h - The Pure Kernel Library Foundation */

#ifndef _KLIB_H
#define _KLIB_H

/*
 * KLIB: Pure Kernel Library for MINIX Microkernel
 *
 * This library provides kernel-space optimized implementations of
 * essential functions without any userspace assumptions. Every
 * function here is designed for:
 *
 * 1. Kernel space operation (no system calls)
 * 2. Hardware optimization (cache-aware, SIMD where appropriate)
 * 3. Mathematical correctness (formal verification friendly)
 * 4. Zero external dependencies (self-contained)
 */

/* Kernel-specific types - no dependency on userspace types */
/* Using __SIZE_TYPE__ and other builtins for architecture neutrality where
 * possible */
typedef __SIZE_TYPE__ ksize_t;
typedef __UINTPTR_TYPE__ kuintptr_t;
typedef __INTPTR_TYPE__ kintptr_t;
typedef __PTRDIFF_TYPE__ kssize_t; /* For differences between pointers */
typedef unsigned int kuint_t;      /* General purpose unsigned int */
typedef int kint_t;                /* General purpose signed int */
typedef unsigned char ku8_t;
typedef unsigned short ku16_t;
typedef unsigned int ku32_t;
typedef unsigned long long ku64_t;
typedef signed char ki8_t;
typedef signed short ki16_t;
typedef signed int ki32_t;
typedef signed long long ki64_t;

/* Boolean type for clarity */
typedef enum { kfalse = 0, ktrue = 1 } kbool_t;
#define KNULL ((void *)0)

/* Hardware-aware CPU feature detection */
typedef struct kcpu_features {
  kbool_t has_sse;
  kbool_t has_sse2;
  kbool_t has_avx;
  kbool_t has_avx2;
  kbool_t has_avx512f; /* AVX-512 Foundation */
  /* Add other specific AVX-512 features as needed, e.g., avx512dq, avx512bw */
  kint_t cache_line_size;
  kint_t l1d_cache_size; /* L1 Data Cache */
  kint_t l1i_cache_size; /* L1 Instruction Cache */
  kint_t l2_cache_size;
  kint_t l3_cache_size;
  /* Add more features: e.g., TSX, CET, MPX, NUMA info */
} kcpu_features_t;

/* Global CPU features - detected at kernel initialization */
extern kcpu_features_t g_kcpu_features;

/* Kernel assertions - more aggressive than userspace */
/* Note: kpanic needs to be implemented */
void kpanic(const char *fmt, ...); /* Declaration for KASSERT */

#ifdef KDEBUG
#define KASSERT(expr)                                                          \
  do {                                                                         \
    if (!(expr)) {                                                             \
      kpanic("KASSERT failed: %s at %s:%d", #expr, __FILE__,                   \
             (kint_t)__LINE__);                                                \
    }                                                                          \
  } while (kfalse)
#else
#define KASSERT(expr) ((void)kfalse)
#endif

#define KASSERT_ALIGNED(ptr, alignment)                                        \
  KASSERT(((kuintptr_t)(ptr) % (alignment)) == 0)

/* Core KLIB function declarations */

/* Memory manipulation */
void *kmemcpy(void *dest, const void *src, ksize_t n);
void *kmemset(void *s, kint_t c, ksize_t n);
kint_t kmemcmp(const void *s1, const void *s2, ksize_t n);
void *kmemmove(void *dest, const void *src,
               ksize_t n); /* For overlapping regions */

/* Hardware-optimized/specialized memory variants */
void *kmemcpy_fast(void *dest, const void *src,
                   ksize_t n); /* Chooses best non-overlapping */
void *kmemcpy_aligned(void *dest, const void *src,
                      ksize_t n);                    /* Assumes alignment */
void *kmemset_aligned(void *s, kint_t c, ksize_t n); /* Assumes alignment */

/* String manipulation */
ksize_t kstrlen(const char *s);
kint_t kstrcmp(const char *s1, const char *s2);
kint_t kstrncmp(const char *s1, const char *s2, ksize_t n);
char *kstrcpy(char *dest, const char *src);
char *kstrncpy(char *dest, const char *src, ksize_t n);
ksize_t kstrlcpy(char *dst, const char *src, ksize_t size);
char *kstrcat(char *dest, const char *src,
              ksize_t dst_size); /* Be careful with buffer sizes! */
char *kstrncat(char *dest, const char *src, ksize_t n);
const char *kstrchr(const char *s, kint_t c);
const char *kstrrchr(const char *s, kint_t c);

/* String/Number Conversion */
kint_t katoi(const char *str);
ku64_t kstrtoul(const char *str, char **endptr, kint_t base);

/* Formatted output - to be implemented carefully for kernel space */
/* kprintf might output to a kernel buffer or serial port */
void kprintf(const char *fmt, ...);
kint_t ksnprintf(char *buf, ksize_t size, const char *fmt, ...);
/* Add ksnprintf, kvsnprintf later if needed, ensuring they are safe */

/* Kernel utility functions */
void kdebug_print(const char *fmt, ...); /* For debug-only messages */

/* CPU feature detection function */
void kcpu_detect_features(void);

#endif /* _KLIB_H */
