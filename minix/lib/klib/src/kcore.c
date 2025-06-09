/**
 * @file kcore.c
 * @brief Core klib functions, including CPU feature detection initialization.
 */

#include <klib.h> // For kcpu_features_t, kbool, kmemset prototype

// The definition of kmemset would typically be in another klib source file
// (e.g., kstring.c or kmemory.c) For now, ensure it's declared in klib.h. We
// assume it will be linked in.

/**
 * @var g_kcpu_features
 * @brief Global storage for detected CPU features.
 *
 * This structure is populated at kernel startup by kcpu_detect_features()
 * and holds boolean flags for various CPU capabilities, as well as cache sizes.
 * It is initialized to all zeros before architecture-specific detection.
 */
kcpu_features_t
    g_kcpu_features; // Definition, will be initialized by kcpu_detect_features

// Prototype for the architecture-specific feature detection function.
// This function is expected to be implemented in, e.g.,
// minix/lib/klib/arch/<arch>/kcpu_detect_features_arch.c
void kcpu_detect_features_arch(void);

/**
 * @brief Initializes the g_kcpu_features structure.
 *
 * This function clears the global CPU features structure and then calls
 * an architecture-specific function to perform the actual hardware feature
 * detection. It should be called early in the kernel initialization sequence.
 */
void kcpu_detect_features(void) {
  // Initialize all features to false (0).
  // kmemset is declared in klib.h. Its definition should be in a klib
  // string/memory utility file. If kmemset_c23 is not yet implemented, this
  // might cause a link error later, or this part would need to be a manual
  // zeroing loop. For now, we assume kmemset_c23 will be available.
  kmemset_c23(&g_kcpu_features, 0, sizeof(g_kcpu_features));

  // Call the architecture-specific detection logic.
  kcpu_detect_features_arch();
}
/* minix/lib/klib/src/kcore.c - KLIB Core Kernel Functions */
#include "klib.h"

/* Define g_kcpu_features here, as it's extern in klib.h */
kcpu_features_t g_kcpu_features;

/*
 * kcpu_detect_features - Detect CPU features.
 * This is a stub. Actual implementation would use CPUID instruction (x86)
 * or read system registers (ARM).
 */
void kcpu_detect_features(void) {
  // STUB: Initialize with some default conservative values
  g_kcpu_features.has_sse = kfalse;
  g_kcpu_features.has_sse2 = kfalse;
  g_kcpu_features.has_avx = kfalse;
  g_kcpu_features.has_avx2 = kfalse;
  g_kcpu_features.has_avx512f = kfalse;
  g_kcpu_features.cache_line_size = 64;       // Common default
  g_kcpu_features.l1d_cache_size = 32 * 1024; // Common default (e.g., 32KB)
  g_kcpu_features.l1i_cache_size = 32 * 1024; // Common default
  g_kcpu_features.l2_cache_size = 256 * 1024; // Common default (e.g., 256KB)
  g_kcpu_features.l3_cache_size = 2 * 1024 * 1024; // Common default (e.g., 2MB)

  // Actual implementation would populate these based on CPUID results or system
  // registers. For example (x86): kuint32_t eax, ebx, ecx, edx;
  // __asm__ volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) :
  // "0"(1)); if (edx & (1 << 26)) g_kcpu_features.has_sse2 = ktrue;
  // ... and so on for other features.
}
/* minix/lib/klib/src/kcore.c - KLIB Core Kernel Functions */
#include "klib.h"

/* Define g_kcpu_features here, as it's extern in klib.h */
kcpu_features_t g_kcpu_features;

/*
 * kcpu_detect_features - Detect CPU features.
 * This is a stub. Actual implementation would use CPUID instruction (x86)
 * or read system registers (ARM).
 */
void kcpu_detect_features(void) {
  // STUB: Initialize with some default conservative values
  g_kcpu_features.has_sse = kfalse;
  g_kcpu_features.has_sse2 = kfalse;
  g_kcpu_features.has_avx = kfalse;
  g_kcpu_features.has_avx2 = kfalse;
  g_kcpu_features.has_avx512f = kfalse;
  g_kcpu_features.cache_line_size = 64;       // Common default
  g_kcpu_features.l1d_cache_size = 32 * 1024; // Common default (e.g., 32KB)
  g_kcpu_features.l1i_cache_size = 32 * 1024; // Common default
  g_kcpu_features.l2_cache_size = 256 * 1024; // Common default (e.g., 256KB)
  g_kcpu_features.l3_cache_size = 2 * 1024 * 1024; // Common default (e.g., 2MB)

  // Actual implementation would populate these based on CPUID results or system
  // registers. For example (x86): kuint32_t eax, ebx, ecx, edx;
  // __asm__ volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) :
  // "0"(1)); if (edx & (1 << 26)) g_kcpu_features.has_sse2 = ktrue;
  // ... and so on for other features.
}
/* minix/lib/klib/src/kcore.c - KLIB Core Kernel Functions */
#include "klib.h"

/* Define g_kcpu_features here, as it's extern in klib.h */
kcpu_features_t g_kcpu_features;

/*
 * kcpu_detect_features - Detect CPU features.
 * This is a stub. Actual implementation would use CPUID instruction (x86)
 * or read system registers (ARM).
 */
void kcpu_detect_features(void) {
  // STUB: Initialize with some default conservative values
  g_kcpu_features.has_sse = kfalse;
  g_kcpu_features.has_sse2 = kfalse;
  g_kcpu_features.has_avx = kfalse;
  g_kcpu_features.has_avx2 = kfalse;
  g_kcpu_features.has_avx512f = kfalse;
  g_kcpu_features.cache_line_size = 64;       // Common default
  g_kcpu_features.l1d_cache_size = 32 * 1024; // Common default (e.g., 32KB)
  g_kcpu_features.l1i_cache_size = 32 * 1024; // Common default
  g_kcpu_features.l2_cache_size = 256 * 1024; // Common default (e.g., 256KB)
  g_kcpu_features.l3_cache_size = 2 * 1024 * 1024; // Common default (e.g., 2MB)

  // Actual implementation would populate these based on CPUID results or system
  // registers. For example (x86): kuint32_t eax, ebx, ecx, edx;
  // __asm__ volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) :
  // "0"(1)); if (edx & (1 << 26)) g_kcpu_features.has_sse2 = ktrue;
  // ... and so on for other features.
}
/* minix/lib/klib/src/kcore.c - KLIB Core Kernel Functions */
#include "klib.h"

/* Define g_kcpu_features here, as it's extern in klib.h */
kcpu_features_t g_kcpu_features;

/*
 * kcpu_detect_features - Detect CPU features.
 * This is a stub. Actual implementation would use CPUID instruction (x86)
 * or read system registers (ARM).
 */
void kcpu_detect_features(void) {
  // STUB: Initialize with some default conservative values
  g_kcpu_features.has_sse = kfalse;
  g_kcpu_features.has_sse2 = kfalse;
  g_kcpu_features.has_avx = kfalse;
  g_kcpu_features.has_avx2 = kfalse;
  g_kcpu_features.has_avx512f = kfalse;
  g_kcpu_features.cache_line_size = 64;       // Common default
  g_kcpu_features.l1d_cache_size = 32 * 1024; // Common default (e.g., 32KB)
  g_kcpu_features.l1i_cache_size = 32 * 1024; // Common default
  g_kcpu_features.l2_cache_size = 256 * 1024; // Common default (e.g., 256KB)
  g_kcpu_features.l3_cache_size = 2 * 1024 * 1024; // Common default (e.g., 2MB)

  // Actual implementation would populate these based on CPUID results or system
  // registers. For example (x86): kuint32_t eax, ebx, ecx, edx;
  // __asm__ volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) :
  // "0"(1)); if (edx & (1 << 26)) g_kcpu_features.has_sse2 = ktrue;
  // ... and so on for other features.
}
/* minix/lib/klib/src/kcore.c - KLIB Core Kernel Functions */
#include "klib.h"

/* Define g_kcpu_features here, as it's extern in klib.h */
kcpu_features_t g_kcpu_features;

/*
 * kcpu_detect_features - Detect CPU features.
 * This is a stub. Actual implementation would use CPUID instruction (x86)
 * or read system registers (ARM).
 */
void kcpu_detect_features(void) {
  // STUB: Initialize with some default conservative values
  g_kcpu_features.has_sse = kfalse;
  g_kcpu_features.has_sse2 = kfalse;
  g_kcpu_features.has_avx = kfalse;
  g_kcpu_features.has_avx2 = kfalse;
  g_kcpu_features.has_avx512f = kfalse;
  g_kcpu_features.cache_line_size = 64;       // Common default
  g_kcpu_features.l1d_cache_size = 32 * 1024; // Common default (e.g., 32KB)
  g_kcpu_features.l1i_cache_size = 32 * 1024; // Common default
  g_kcpu_features.l2_cache_size = 256 * 1024; // Common default (e.g., 256KB)
  g_kcpu_features.l3_cache_size = 2 * 1024 * 1024; // Common default (e.g., 2MB)

  // Actual implementation would populate these based on CPUID results or system
  // registers. For example (x86): kuint32_t eax, ebx, ecx, edx;
  // __asm__ volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) :
  // "0"(1)); if (edx & (1 << 26)) g_kcpu_features.has_sse2 = ktrue;
  // ... and so on for other features.
}
