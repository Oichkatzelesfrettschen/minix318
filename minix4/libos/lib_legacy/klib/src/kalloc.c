/**
 * @file kalloc.c
 * @brief Canonical kernel memory allocator.
 *
 * This implementation currently defers to the host C library's
 * @c malloc(), @c free() and @c realloc() functions. The interface
 * is kept separate so that future kernel-specific allocators can be
 * dropped in transparently.
 */

#include <kalloc.h>
#include <klib.h>
#include <stdlib.h>

/**
 * @brief Allocate memory from the kernel heap.
 *
 * The current implementation is a thin wrapper around the standard
 * @c malloc() function. It enables a future drop-in replacement
 * without impacting call sites.
 *
 * @param size Number of bytes to allocate.
 * @return Pointer to allocated memory or @c NULL on failure.
 */
void *kalloc(k_size_t size) { return malloc(size); }

/**
 * @brief Release memory obtained with kalloc().
 *
 * @param ptr Allocation to free. May be @c NULL.
 */
void kfree(void *ptr) { free(ptr); }

/**
 * @brief Resize a block allocated by kalloc().
 *
 * @param ptr  Existing allocation or @c NULL for a new block.
 * @param size Desired new size in bytes.
 * @return Pointer to resized block or @c NULL on failure.
 */
void *krealloc(void *ptr, k_size_t size) { return realloc(ptr, size); }
