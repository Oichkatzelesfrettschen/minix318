#ifndef _MINIX_KALLOC_H
#define _MINIX_KALLOC_H

#include <klib.h> /* for k_size_t */

/**
 * @brief Allocate a block of memory from the kernel heap.
 *
 * The current implementation forwards to the C library's @c malloc.
 * A dedicated kernel heap manager may replace this in the future.
 *
 * @param size Number of bytes to allocate.
 * @return Pointer to allocated memory or NULL on failure.
 */
void *kalloc(k_size_t size);

/**
 * @brief Free memory previously allocated with kalloc.
 *
 * @param ptr Pointer to memory to free. May be NULL.
 */
void kfree(void *ptr);

/**
 * @brief Resize an allocation obtained from kalloc.
 *
 * This behaves similar to @c realloc.
 *
 * @param ptr  Existing allocation or NULL for a new block.
 * @param size New size in bytes.
 * @return Pointer to resized block or NULL on failure.
 */
void *krealloc(void *ptr, k_size_t size);

#endif /* _MINIX_KALLOC_H */
