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
/* Using __SIZE_TYPE__ and other builtins for architecture neutrality where possible */
typedef __SIZE_TYPE__ ksize_t;
typedef __UINTPTR_TYPE__ kuintptr_t;
typedef __INTPTR_TYPE__ kintptr_t;
typedef __PTRDIFF_TYPE__ kssize_t; /* For differences between pointers */
typedef unsigned int kuint_t; /* General purpose unsigned int */
typedef int kint_t;           /* General purpose signed int */
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
#define KNULL ((void*)0)

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
void kpanic(const char* fmt, ...); /* Declaration for KASSERT */

#ifdef KDEBUG
#define KASSERT(expr) \
    do { \
        if (!(expr)) { \
            kpanic("KASSERT failed: %s at %s:%d", #expr, __FILE__, (kint_t)__LINE__); \
        } \
    } while(kfalse)
#else
#define KASSERT(expr) ((void)kfalse)
#endif

#define KASSERT_ALIGNED(ptr, alignment) KASSERT(((kuintptr_t)(ptr) % (alignment)) == 0)

/* Core KLIB function declarations */

/* Memory manipulation */
void* kmemcpy(void* dest, const void* src, ksize_t n);
void* kmemset(void* s, kint_t c, ksize_t n);
kint_t kmemcmp(const void* s1, const void* s2, ksize_t n);
void* kmemmove(void* dest, const void* src, ksize_t n); /* For overlapping regions */

/* Hardware-optimized/specialized memory variants */
void* kmemcpy_fast(void* dest, const void* src, ksize_t n); /* Chooses best non-overlapping */
void* kmemcpy_aligned(void* dest, const void* src, ksize_t n); /* Assumes alignment */
void* kmemset_aligned(void* s, kint_t c, ksize_t n); /* Assumes alignment */

/* String manipulation */
ksize_t kstrlen(const char* s);
kint_t kstrcmp(const char* s1, const char* s2);
kint_t kstrncmp(const char* s1, const char* s2, ksize_t n);
char* kstrcpy(char* dest, const char* src);
char* kstrncpy(char* dest, const char* src, ksize_t n);
ksize_t kstrlcpy(char* dst, const char* src, ksize_t size);
char* kstrcat(char* dest, const char* src, ksize_t dst_size); /* Be careful with buffer sizes! */
char* kstrncat(char* dest, const char* src, ksize_t n);
const char* kstrchr(const char* s, kint_t c);
const char* kstrrchr(const char* s, kint_t c);

/* String/Number Conversion */
kint_t katoi(const char* str);
ku64_t kstrtoul(const char* str, char** endptr, kint_t base);

/* Formatted output - to be implemented carefully for kernel space */
/* kprintf might output to a kernel buffer or serial port */
void kprintf(const char* fmt, ...);
kint_t ksnprintf(char* buf, ksize_t size, const char* fmt, ...);
/* Add ksnprintf, kvsnprintf later if needed, ensuring they are safe */

/* Kernel utility functions */
void kdebug_print(const char* fmt, ...); /* For debug-only messages */

/* CPU feature detection function */
void kcpu_detect_features(void);

#endif /* _KLIB_H */
