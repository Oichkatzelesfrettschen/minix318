/**
 * @file kmemory.h
 * @brief Kernel Memory Management Functions
 * @details Safe memory allocation and manipulation for kernel use
 * @version 4.0.0
 * @date 2025-06-12
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Memory allocation */
void *kmalloc(size_t size);
void *kcalloc(size_t nmemb, size_t size);
void *krealloc(void *ptr, size_t size);
void kfree(void *ptr);

/* Aligned memory allocation */
void *kmalloc_aligned(size_t size, size_t alignment);
void *kmalloc_dma(size_t size);

/* Memory manipulation */
void *kmemset(void *s, int c, size_t n);
void *kmemcpy(void *dest, const void *src, size_t n);
void *kmemmove(void *dest, const void *src, size_t n);

/* Memory utilities */
void kzero_memory(void *ptr, size_t size);
bool kmemory_is_valid(const void *ptr, size_t size);
size_t kmemory_usage(void);
size_t kmemory_available(void);

/* Page-aligned memory operations */
void *kpage_alloc(size_t num_pages);
void kpage_free(void *ptr, size_t num_pages);
void *kpage_map(uint64_t phys_addr, size_t num_pages, uint32_t flags);
void kpage_unmap(void *virt_addr, size_t num_pages);

/* Memory debugging */
void kmemory_dump_stats(void);
bool kmemory_check_corruption(void);
void kmemory_mark_readonly(void *ptr, size_t size);
void kmemory_mark_readwrite(void *ptr, size_t size);

/* Memory allocation flags */
#define KMEM_ZERO       (1U << 0)   /* Zero allocated memory */
#define KMEM_ATOMIC     (1U << 1)   /* Atomic allocation */
#define KMEM_DMA        (1U << 2)   /* DMA-capable memory */
#define KMEM_CONTIGUOUS (1U << 3)   /* Physically contiguous */

/* Extended allocation with flags */
void *kmalloc_flags(size_t size, uint32_t flags);

/* Memory pool management */
typedef struct kmem_pool kmem_pool_t;

kmem_pool_t *kmem_pool_create(size_t obj_size, size_t alignment, uint32_t flags);
void kmem_pool_destroy(kmem_pool_t *pool);
void *kmem_pool_alloc(kmem_pool_t *pool);
void kmem_pool_free(kmem_pool_t *pool, void *obj);

/* Convenience macros */
#define KMALLOC(type) \
    ((type *)kmalloc(sizeof(type)))

#define KCALLOC(type, count) \
    ((type *)kcalloc(count, sizeof(type)))

#define KFREE_AND_NULL(ptr) \
    do { kfree(ptr); (ptr) = NULL; } while(0)
