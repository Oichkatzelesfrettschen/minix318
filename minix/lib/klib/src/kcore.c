/**
 * @file kcore.c
 * @brief Core klib functions, including CPU feature detection initialization.
 */

#include <klib.h>  // For kcpu_features_t, kbool, kmemset prototype

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
    g_kcpu_features;  // Definition, will be initialized by kcpu_detect_features

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
