/**
 * @file vm_unified.h
 * @brief Prototypes and definitions for VM interface.
 */

#ifndef _MINIX_VM_H
#define _MINIX_VM_H

#include <minix/endpoint.h>
#include <sys/types.h>

/** Request VM to exit the given endpoint. */
int vm_exit(endpoint_t ep);
/** Fork a VM endpoint. */
int vm_fork(endpoint_t ep, int slotno, endpoint_t *child_ep);
/** Retrieve resource usage information. */
int vm_getrusage(endpoint_t endpt, void *addr, int children);
/** Notify VM that the process will exit soon. */
int vm_willexit(endpoint_t ep);
/** Add a DMA region to a process. */
int vm_adddma(endpoint_t proc_e, phys_bytes start, phys_bytes size);
/** Remove a DMA region from a process. */
int vm_deldma(endpoint_t proc_e, phys_bytes start, phys_bytes size);
/** Get DMA region information. */
int vm_getdma(endpoint_t *procp, phys_bytes *basep, phys_bytes *sizep);
/** Map physical memory into the address space. */
void *vm_map_phys(endpoint_t who, void *physaddr, size_t len);
/** Unmap previously mapped physical memory. */
int vm_unmap_phys(endpoint_t who, void *vaddr, size_t len);

/** Set privileges for an endpoint. */
int vm_set_priv(endpoint_t ep, void *buf, int sys_proc);
/** Update VM state across endpoints. */
int vm_update(endpoint_t src_e, endpoint_t dst_e, int flags);
/** Control VM memory settings. */
int vm_memctl(endpoint_t ep, int req, void **addr, size_t *len);
/** Prepare for a VM update. */
int vm_prepare(endpoint_t src_e, endpoint_t dst_e, int flags);
/** Map a file from the VFS. */
int minix_vfs_mmap(endpoint_t who, off_t offset, size_t len, dev_t dev,
                   ino_t ino, int fd, u32_t vaddr, u16_t clearend, u16_t flags);

/** Map memory on behalf of another process. */
void *minix_mmap_for(endpoint_t forwhom, void *addr, size_t len, int prot,
                     int flags, int fd, off_t offset);
/** Map a file from the VFS (alternative prototype). */
int minix_vfs_mmap(endpoint_t who, off_t offset, size_t len, dev_t dev,
                   ino_t ino, int fd, u32_t vaddr, u16_t clearend, u16_t flags);

/* minix vfs mmap flags */
#define MVM_WRITABLE 0x8000

/* VM kernel request types. */
#define VMPTYPE_NONE 0
#define VMPTYPE_CHECK 1

struct vm_stats_info {
  unsigned int vsi_pagesize; /* page size */
  unsigned long vsi_total;   /* total number of memory pages */
  unsigned long vsi_free;    /* number of free pages */
  unsigned long vsi_largest; /* largest number of consecutive free pages */
  unsigned long vsi_cached;  /* number of pages cached for file systems */
};

struct vm_usage_info {
  vir_bytes vui_total;    /* total amount of mapped process memory */
  vir_bytes vui_common;   /* part of memory mapped in more than once */
  vir_bytes vui_shared;   /* shared (non-COW) part of common memory */
  vir_bytes vui_virtual;  /* total size of virtual address space */
  vir_bytes vui_mvirtual; /* idem but minus unmapped stack pages */
  uint64_t vui_maxrss;    /* maximum resident set size (in KB) */
  uint64_t vui_minflt;    /* minor page faults */
  uint64_t vui_majflt;    /* major page faults */
};

struct vm_region_info {
  vir_bytes vri_addr;   /* base address of region */
  vir_bytes vri_length; /* length of region */
  int vri_prot;         /* protection flags (PROT_) */
  int vri_flags;        /* memory flags (subset of MAP_) */
};

#define MAX_VRI_COUNT 64 /* max. number of regions provided at once */

/** Retrieve VM statistics. */
int vm_info_stats(struct vm_stats_info *vfi);
/** Retrieve VM usage information for an endpoint. */
int vm_info_usage(endpoint_t who, struct vm_usage_info *vui);
/** Query VM region information. */
int vm_info_region(endpoint_t who, struct vm_region_info *vri, int count,
                   vir_bytes *next);
/** Clear process control data for an endpoint. */
int vm_procctl_clear(endpoint_t ep);
/** Manage VM memory handling for an endpoint. */
int vm_procctl_handlemem(endpoint_t ep, vir_bytes m1, vir_bytes m2, int wr);

/** Set a cache block in VM. */
int vm_set_cacheblock(void *block, dev_t dev, off_t dev_offset, ino_t ino,
                      off_t ino_offset, u32_t *flags, int blocksize,
                      int setflags);
/** Map a cache block into memory. */
void *vm_map_cacheblock(dev_t dev, off_t dev_offset, ino_t ino,
                        off_t ino_offset, u32_t *flags, int blocksize);
/** Forget a cache block mapping. */
int vm_forget_cacheblock(dev_t dev, off_t dev_offset, int blocksize);
/** Clear the VM cache for a device. */
int vm_clear_cache(dev_t dev);

/* flags for vm cache functions */
#define VMMC_FLAGS_LOCKED                                                      \
  0x01                    /* someone is updating the flags; don't read/write */
#define VMMC_DIRTY 0x02   /* dirty buffer and it may not be evicted */
#define VMMC_EVICTED 0x04 /* VM has evicted the buffer and it's invalid */
#define VMMC_BLOCK_LOCKED                                                      \
  0x08 /* client is using it and it may not be evicted */

/* special inode number for vm cache functions */
#define VMC_NO_INODE 0 /* to reference a disk block, no associated file */

/* setflags for vm_set_cacheblock, also used internally in VM */
#define VMSF_ONCE 0x01 /* discard block after one-time use */

#endif /* _MINIX_VM_H */
