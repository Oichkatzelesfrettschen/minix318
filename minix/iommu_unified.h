/**
 * @file iommu_unified.h
 * @brief Consolidated IOMMU header definitions.
 *
 * This header aggregates declarations from:
 *  - minix/include/iommu.h
 *  - minix/iommu.h
 *  - minix/kernel/iommu.h
 *  - minix/kernel/iommu_v2.h
 */

/* ==== minix/include/iommu.h ==== */
#pragma once
#include <stddef.h>
#include <stdint.h>
#define IOMMU_PTE_P 0x1

typedef uint64_t iommu_pte_t;

struct iommu_dom {
  iommu_pte_t *pt;
  size_t npages;
};

#define IOMMU_OP_MAP 0xA0
#define IOMMU_OP_UNMAP 0xA1
#define IOMMU_OP_REVOKE 0xA2

int iommu_map(struct iommu_dom *dom, uintptr_t iova, uintptr_t pa, size_t len,
              int perm);
int iommu_unmap(struct iommu_dom *dom, uintptr_t iova, size_t len);
void iommu_sync(struct iommu_dom *dom);
void iommu_revoke(struct iommu_dom *dom);

#ifdef __cplusplus
extern "C" {
#endif

int iommu_map_call(uintptr_t iova, uintptr_t pa, size_t len);
int iommu_unmap_call(uintptr_t iova, size_t len);
int iommu_revoke_call(void);

#ifdef __cplusplus
}
#endif

/* ==== minix/iommu.h ==== */
#pragma once
#include <stddef.h>
#include <stdint.h>
#define IOMMU_PTE_P 0x1

typedef uint64_t iommu_pte_t;

struct iommu_dom {
  iommu_pte_t *pt;
  size_t npages;
};

#define IOMMU_OP_MAP 0xA0
#define IOMMU_OP_UNMAP 0xA1
#define IOMMU_OP_REVOKE 0xA2

int iommu_map(struct iommu_dom *dom, uintptr_t iova, uintptr_t pa, size_t len,
              int perm);
int iommu_unmap(struct iommu_dom *dom, uintptr_t iova, size_t len);
void iommu_sync(struct iommu_dom *dom);
void iommu_revoke(struct iommu_dom *dom);

#ifdef __cplusplus
extern "C" {
#endif

int iommu_map_call(uintptr_t iova, uintptr_t pa, size_t len);
int iommu_unmap_call(uintptr_t iova, size_t len);
int iommu_revoke_call(void);

#ifdef __cplusplus
}
#endif

/* ==== minix/kernel/iommu.h ==== */
#ifndef IOMMU_H
#define IOMMU_H

#include <stddef.h>
#include <stdint.h>

#ifndef SPINLOCK_T_DEFINED
#define SPINLOCK_T_DEFINED
typedef struct {
  volatile int locked;
} spinlock_t;

static inline void spin_lock_init(spinlock_t *l) { l->locked = 0; }
static inline void spin_lock(spinlock_t *l) {
  while (__sync_lock_test_and_set(&l->locked, 1))
    ;
}
static inline void spin_unlock(spinlock_t *l) {
  __sync_lock_release(&l->locked);
}
#endif

struct iommu_dom {
  uintptr_t pml_root;
  uint16_t asid;
  unsigned long epoch;
  uint32_t flags;
  spinlock_t lock;
};

int iommu_map(struct iommu_dom *, uintptr_t iova, uintptr_t pa, size_t len,
              uint32_t perms);
int iommu_unmap(struct iommu_dom *, uintptr_t iova, size_t len);
int iommu_bulk_map(struct iommu_dom *, const uintptr_t *iovas,
                   const uintptr_t *pas, const size_t *lens,
                   const uint32_t *perms, size_t count);

#endif /* IOMMU_H */

/* ==== minix/kernel/iommu_v2.h ==== */
#ifndef IOMMU_V2_H
#define IOMMU_V2_H

#include <stddef.h>
#include <stdint.h>

#ifndef SPINLOCK_T_DEFINED
#define SPINLOCK_T_DEFINED
typedef struct {
  volatile int locked;
} spinlock_t;

static inline void spin_lock_init(spinlock_t *l) { l->locked = 0; }
static inline void spin_lock(spinlock_t *l) {
  while (__sync_lock_test_and_set(&l->locked, 1))
    ;
}
static inline void spin_unlock(spinlock_t *l) {
  __sync_lock_release(&l->locked);
}
#endif

struct iommu_dom {
  uintptr_t pml_root;
  uint16_t asid;
  unsigned long epoch;
  uint32_t flags;
  spinlock_t lock;
};

int iommu_map(struct iommu_dom *, uintptr_t iova, uintptr_t pa, size_t len,
              uint32_t perms);
int iommu_unmap(struct iommu_dom *, uintptr_t iova, size_t len);
int iommu_bulk_map(struct iommu_dom *, const uintptr_t *iovas,
                   const uintptr_t *pas, const size_t *lens,
                   const uint32_t *perms, size_t count);

#endif /* IOMMU_V2_H */
