/**
 * @file iommu_unified.c
 * @brief Consolidated IOMMU source from multiple components.
 *
 * This file aggregates logic from:
 *  - libos/iommu.c
 *  - minix/iommu.c
 *  - minix/kernel/iommu.c
 *  - minix/kernel/iommu_v2.c
 */

/* ==== libos/iommu.c ==== */
#include "iommu.h"
#include "ipc.h"

int iommu_map_call(uintptr_t iova, uintptr_t pa, size_t len) {
  zipc_msg_t m = {0};
  m.w0 = IOMMU_OP_MAP;
  m.w1 = iova;
  m.w2 = pa;
  m.w3 = len;
  zipc_call(&m);
  return (int)m.w0;
}

int iommu_unmap_call(uintptr_t iova, size_t len) {
  zipc_msg_t m = {0};
  m.w0 = IOMMU_OP_UNMAP;
  m.w1 = iova;
  m.w2 = len;
  zipc_call(&m);
  return (int)m.w0;
}

int iommu_revoke_call(void) {
  zipc_msg_t m = {0};
  m.w0 = IOMMU_OP_REVOKE;
  zipc_call(&m);
  return (int)m.w0;
}

/* ==== minix/iommu.c ==== */
#include "defs.h"
#include "iommu.h"
#include "mmu.h"
#include <stdint.h>
#include <string.h>

int iommu_map(struct iommu_dom *dom, uintptr_t iova, uintptr_t pa, size_t len,
              int perm) {
  if (!dom || !dom->pt || (iova % PGSIZE) || (pa % PGSIZE) || (len % PGSIZE))
    return -1;
  size_t pages = len / PGSIZE;
  for (size_t i = 0; i < pages; i++) {
    size_t idx = (iova / PGSIZE) + i;
    if (idx >= dom->npages)
      return -1;
    if (dom->pt[idx] & IOMMU_PTE_P)
      return -1;
    dom->pt[idx] = (pa + i * PGSIZE) | perm | IOMMU_PTE_P;
  }
  iommu_sync(dom);
  return 0;
}

int iommu_unmap(struct iommu_dom *dom, uintptr_t iova, size_t len) {
  if (!dom || !dom->pt || (iova % PGSIZE) || (len % PGSIZE))
    return -1;
  size_t pages = len / PGSIZE;
  for (size_t i = 0; i < pages; i++) {
    size_t idx = (iova / PGSIZE) + i;
    if (idx >= dom->npages || !(dom->pt[idx] & IOMMU_PTE_P))
      return -1;
    dom->pt[idx] = 0;
  }
  iommu_sync(dom);
  return 0;
}

void iommu_sync(struct iommu_dom *dom) {
  (void)dom;
  // In hardware this would flush IOTLB; no-op in mock implementation.
}

void iommu_revoke(struct iommu_dom *dom) {
  if (!dom || !dom->pt)
    return;
  for (size_t i = 0; i < dom->npages; i++)
    dom->pt[i] = 0;
  iommu_sync(dom);
}

/* ==== minix/kernel/iommu.c ==== */
#include "iommu.h"
#include <stdlib.h>

struct mapping {
  uintptr_t iova;
  uintptr_t pa;
  size_t len;
  uint32_t perms;
  struct mapping *next;
};

static struct mapping **map_list(struct iommu_dom *d) {
  return (struct mapping **)&d->pml_root; /* treat pml_root as list head */
}

static void tlb_invalidate_domain(uint16_t asid) { (void)asid; }

int iommu_map(struct iommu_dom *dom, uintptr_t iova, uintptr_t pa, size_t len,
              uint32_t perms) {
  if (!dom || !len)
    return -1;
  struct mapping *m = malloc(sizeof(*m));
  if (!m)
    return -1;
  m->iova = iova;
  m->pa = pa;
  m->len = len;
  m->perms = perms;
  spin_lock(&dom->lock);
  m->next = *map_list(dom);
  *map_list(dom) = m;
  dom->epoch++;
  spin_unlock(&dom->lock);
  tlb_invalidate_domain(dom->asid);
  return 0;
}

int iommu_unmap(struct iommu_dom *dom, uintptr_t iova, size_t len) {
  if (!dom || !len)
    return -1;
  spin_lock(&dom->lock);
  struct mapping **p = map_list(dom);
  while (*p) {
    if ((*p)->iova == iova && (*p)->len == len) {
      struct mapping *tmp = *p;
      *p = tmp->next;
      free(tmp);
      dom->epoch++;
      spin_unlock(&dom->lock);
      tlb_invalidate_domain(dom->asid);
      return 0;
    }
    p = &(*p)->next;
  }
  spin_unlock(&dom->lock);
  return -1;
}

int iommu_bulk_map(struct iommu_dom *dom, const uintptr_t *iovas,
                   const uintptr_t *pas, const size_t *lens,
                   const uint32_t *perms, size_t count) {
  if (!dom)
    return -1;
  for (size_t i = 0; i < count; ++i)
    if (iommu_map(dom, iovas[i], pas[i], lens[i], perms[i]) != 0)
      return -1;
  return 0;
}

/* ==== minix/kernel/iommu_v2.c ==== */
#include "iommu_v2.h"
#include <stdlib.h>

struct mapping {
  uintptr_t iova;
  uintptr_t pa;
  size_t len;
  uint32_t perms;
  struct mapping *next;
};

static struct mapping **map_list(struct iommu_dom *d) {
  return (struct mapping **)&d->pml_root; /* treat pml_root as list head */
}

static void tlb_invalidate_domain(uint16_t asid) { (void)asid; }

int iommu_map(struct iommu_dom *dom, uintptr_t iova, uintptr_t pa, size_t len,
              uint32_t perms) {
  if (!dom || !len)
    return -1;
  struct mapping *m = malloc(sizeof(*m));
  if (!m)
    return -1;
  m->iova = iova;
  m->pa = pa;
  m->len = len;
  m->perms = perms;
  spin_lock(&dom->lock);
  m->next = *map_list(dom);
  *map_list(dom) = m;
  dom->epoch++;
  spin_unlock(&dom->lock);
  tlb_invalidate_domain(dom->asid);
  return 0;
}

int iommu_unmap(struct iommu_dom *dom, uintptr_t iova, size_t len) {
  if (!dom || !len)
    return -1;
  spin_lock(&dom->lock);
  struct mapping **p = map_list(dom);
  while (*p) {
    if ((*p)->iova == iova && (*p)->len == len) {
      struct mapping *tmp = *p;
      *p = tmp->next;
      free(tmp);
      dom->epoch++;
      spin_unlock(&dom->lock);
      tlb_invalidate_domain(dom->asid);
      return 0;
    }
    p = &(*p)->next;
  }
  spin_unlock(&dom->lock);
  return -1;
}

int iommu_bulk_map(struct iommu_dom *dom, const uintptr_t *iovas,
                   const uintptr_t *pas, const size_t *lens,
                   const uint32_t *perms, size_t count) {
  if (!dom)
    return -1;
  for (size_t i = 0; i < count; ++i)
    if (iommu_map(dom, iovas[i], pas[i], lens[i], perms[i]) != 0)
      return -1;
  return 0;
}
