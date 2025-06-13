#include "syslib.h"
#include <assert.h>
#include <machine/param.h>
#include <machine/vmparam.h>
#include <minix/sysutil.h>
#include <minix/vm.h>
#include <stdarg.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

/**
 * @file vm_unified.c
 * @brief Consolidated VM interface routines.
 */

/**
 * Legacy comment preserved:
 * @verbatim
 *===========================================================================*
 *                                vm_exit                                    *
 *===========================================================================*
 * @endverbatim
 */
int vm_exit(endpoint_t ep) {
  message m;
  int result;

  memset(&m, 0, sizeof(m));
  m.VME_ENDPOINT = ep;

  result = _taskcall(VM_PROC_NR, VM_EXIT, &m);
  return result;
}

/**
 * Legacy comment preserved:
 * @verbatim
 *===========================================================================*
 *                                vm_willexit                                *
 *===========================================================================*
 * @endverbatim
 */
int vm_willexit(endpoint_t ep) {
  message m;
  int result;

  memset(&m, 0, sizeof(m));
  m.VMWE_ENDPOINT = ep;

  result = _taskcall(VM_PROC_NR, VM_WILLEXIT, &m);
  return result;
}

/**
 * Legacy comment preserved:
 * @verbatim
 *===========================================================================*
 *                                vm_fork                                    *
 *===========================================================================*
 * @endverbatim
 */
int vm_fork(endpoint_t ep, int slot, endpoint_t *childep) {
  message m;
  int result;

  memset(&m, 0, sizeof(m));
  m.VMF_ENDPOINT = ep;
  m.VMF_SLOTNO = slot;

  result = _taskcall(VM_PROC_NR, VM_FORK, &m);

  *childep = m.VMF_CHILD_ENDPOINT;

  return result;
}

/**
 * @brief Wrapper for VM_GETRUSAGE.
 */
int vm_getrusage(endpoint_t endpt, void *addr, int children) {
  message m;

  memset(&m, 0, sizeof(m));
  m.m_lsys_vm_rusage.endpt = endpt;
  m.m_lsys_vm_rusage.addr = (vir_bytes)addr;
  m.m_lsys_vm_rusage.children = children;

  return _taskcall(VM_PROC_NR, VM_GETRUSAGE, &m);
}

/**
 * Legacy comment preserved:
 * @verbatim
 *===========================================================================*
 *                                vm_info_stats                              *
 *===========================================================================*
 * @endverbatim
 */
int vm_info_stats(struct vm_stats_info *vsi) {
  message m;

  memset(&m, 0, sizeof(m));
  m.m_lsys_vm_info.what = VMIW_STATS;
  m.m_lsys_vm_info.ptr = vsi;

  return _taskcall(VM_PROC_NR, VM_INFO, &m);
}

/**
 * Legacy comment preserved:
 * @verbatim
 *===========================================================================*
 *                                vm_info_usage                              *
 *===========================================================================*
 * @endverbatim
 */
int vm_info_usage(endpoint_t who, struct vm_usage_info *vui) {
  message m;

  memset(&m, 0, sizeof(m));
  m.m_lsys_vm_info.what = VMIW_USAGE;
  m.m_lsys_vm_info.ep = who;
  m.m_lsys_vm_info.ptr = vui;

  return _taskcall(VM_PROC_NR, VM_INFO, &m);
}

/**
 * Legacy comment preserved:
 * @verbatim
 *===========================================================================*
 *                                vm_info_region                             *
 *===========================================================================*
 * @endverbatim
 */
int vm_info_region(endpoint_t who, struct vm_region_info *vri, int count,
                   vir_bytes *next) {
  message m;
  int result;

  memset(&m, 0, sizeof(m));
  m.m_lsys_vm_info.what = VMIW_REGION;
  m.m_lsys_vm_info.ep = who;
  m.m_lsys_vm_info.count = count;
  m.m_lsys_vm_info.ptr = vri;
  m.m_lsys_vm_info.next = *next;

  if ((result = _taskcall(VM_PROC_NR, VM_INFO, &m)) != OK)
    return result;

  *next = m.m_lsys_vm_info.next;
  return m.m_lsys_vm_info.count;
}

/**
 * @brief Map physical memory into the caller's address space.
 */
void *vm_map_phys(endpoint_t who, void *phaddr, size_t len) {
  message m;
  int r;

  memset(&m, 0, sizeof(m));
  m.m_lsys_vm_map_phys.ep = who;
  m.m_lsys_vm_map_phys.phaddr = (phys_bytes)phaddr;
  m.m_lsys_vm_map_phys.len = len;

  r = _taskcall(VM_PROC_NR, VM_MAP_PHYS, &m);

  if (r != OK)
    return MAP_FAILED;

  r = sef_llvm_add_special_mem_region(m.m_lsys_vm_map_phys.reply, len, NULL);
  if (r < 0)
    printf("vm_map_phys: add_special_mem_region failed: %d\n", r);

  return m.m_lsys_vm_map_phys.reply;
}

/**
 * @brief Unmap a previously mapped physical memory region.
 */
int vm_unmap_phys(endpoint_t who, void *vaddr, size_t len) {
  message m;
  int r;

  memset(&m, 0, sizeof(m));
  m.m_lsys_vm_unmap_phys.ep = who;
  m.m_lsys_vm_unmap_phys.vaddr = vaddr;

  r = _taskcall(VM_PROC_NR, VM_UNMAP_PHYS, &m);

  if (r != OK)
    return r;

  r = sef_llvm_del_special_mem_region_by_addr(vaddr);
  if (r < 0)
    printf("vm_map_phys: del_special_mem_region failed: %d\n", r);

  return OK;
}

/**
 * @brief Control VM memory parameters.
 */
int vm_memctl(endpoint_t ep, int req, void **addr, size_t *len) {
  message m;
  int r;

  memset(&m, 0, sizeof(m));
  m.VM_RS_CTL_ENDPT = ep;
  m.VM_RS_CTL_REQ = req;
  m.VM_RS_CTL_ADDR = addr ? *addr : 0;
  m.VM_RS_CTL_LEN = len ? *len : 0;

  r = _taskcall(VM_PROC_NR, VM_RS_MEMCTL, &m);
  if (r != OK)
    return r;
  if (addr)
    *addr = m.VM_RS_CTL_ADDR;
  if (len)
    *len = m.VM_RS_CTL_LEN;

  return OK;
}

/**
 * @brief Prepare a VM instance for update.
 */
int vm_prepare(endpoint_t src_e, endpoint_t dst_e, int flags) {
  message m;

  memset(&m, 0, sizeof(m));
  m.m_lsys_vm_update.src = src_e;
  m.m_lsys_vm_update.dst = dst_e;
  m.m_lsys_vm_update.flags = flags;

  return _taskcall(VM_PROC_NR, VM_RS_PREPARE, &m);
}

/**
 * @brief Inform VM about an update operation.
 */
int vm_update(endpoint_t src_e, endpoint_t dst_e, int flags) {
  message m;

  memset(&m, 0, sizeof(m));
  m.m_lsys_vm_update.src = src_e;
  m.m_lsys_vm_update.dst = dst_e;
  m.m_lsys_vm_update.flags = flags;

  return _taskcall(VM_PROC_NR, VM_RS_UPDATE, &m);
}

/**
 * @brief Configure privileges for a VM endpoint.
 */
int vm_set_priv(endpoint_t ep, void *buf, int sys_proc) {
  message m;

  memset(&m, 0, sizeof(m));
  m.VM_RS_NR = ep;
  m.VM_RS_BUF = (long)buf;
  m.VM_RS_SYS = sys_proc;

  return _taskcall(VM_PROC_NR, VM_RS_SET_PRIV, &m);
}

/**
 * Legacy comment preserved:
 * @verbatim
 *===========================================================================*
 *                                vm_procctl                                 *
 *===========================================================================*
 * @endverbatim
 */
static int vm_procctl(endpoint_t ep, int param, vir_bytes m1, vir_bytes len,
                      int flags) {
  message m;
  int result;

  memset(&m, 0, sizeof(m));

  m.VMPCTL_WHO = ep;
  m.VMPCTL_PARAM = param;
  m.VMPCTL_M1 = m1;
  m.VMPCTL_LEN = len;
  m.VMPCTL_FLAGS = flags;

  result = _taskcall(VM_PROC_NR, VM_PROCCTL, &m);
  return result;
}

/**
 * @brief Clear VM process control flags.
 */
int vm_procctl_clear(endpoint_t ep) {
  return vm_procctl(ep, VMPPARAM_CLEAR, 0, 0, 0);
}

/**
 * @brief Handle memory for a VM process.
 */
int vm_procctl_handlemem(endpoint_t ep, vir_bytes m1, vir_bytes len,
                         int writeflag) {
  return vm_procctl(ep, VMPPARAM_HANDLEMEM, m1, len, writeflag);
}

/**
 * @brief Internal helper for VM cache operations.
 */
static int vm_cachecall(message *m, int call, void *addr, dev_t dev,
                        off_t dev_offset, ino_t ino, off_t ino_offset,
                        u32_t *flags, int blocksize, int setflags) {
  if (blocksize % PAGE_SIZE)
    panic("blocksize %d should be a multiple of pagesize %d\n", blocksize,
          PAGE_SIZE);

  if (ino_offset % PAGE_SIZE)
    panic("inode offset %lld should be a multiple of pagesize %d\n", ino_offset,
          PAGE_SIZE);

  if (dev_offset % PAGE_SIZE)
    panic("dev offset offset %lld should be a multiple of pagesize %d\n",
          dev_offset, PAGE_SIZE);

  memset(m, 0, sizeof(*m));

  assert(dev != NO_DEV);

  m->m_vmmcp.dev_offset = dev_offset;
  m->m_vmmcp.ino_offset = ino_offset;
  m->m_vmmcp.ino = ino;
  m->m_vmmcp.block = addr;
  m->m_vmmcp.flags_ptr = flags;
  m->m_vmmcp.dev = dev;
  m->m_vmmcp.pages = blocksize / PAGE_SIZE;
  m->m_vmmcp.flags = setflags;

  return _taskcall(VM_PROC_NR, call, m);
}

/**
 * @brief Map a cache block into memory.
 */
void *vm_map_cacheblock(dev_t dev, off_t dev_offset, ino_t ino,
                        off_t ino_offset, u32_t *flags, int blocksize) {
  message m;

  if (vm_cachecall(&m, VM_MAPCACHEPAGE, NULL, dev, dev_offset, ino, ino_offset,
                   flags, blocksize, 0) != OK)
    return MAP_FAILED;

  return m.m_vmmcp_reply.addr;
}

/**
 * @brief Set a cache block in VM.
 */
int vm_set_cacheblock(void *block, dev_t dev, off_t dev_offset, ino_t ino,
                      off_t ino_offset, u32_t *flags, int blocksize,
                      int setflags) {
  message m;

  return vm_cachecall(&m, VM_SETCACHEPAGE, block, dev, dev_offset, ino,
                      ino_offset, flags, blocksize, setflags);
}

/**
 * @brief Forget a cache block mapping.
 */
int vm_forget_cacheblock(dev_t dev, off_t dev_offset, int blocksize) {
  message m;

  return vm_cachecall(&m, VM_FORGETCACHEPAGE, NULL, dev, dev_offset,
                      VMC_NO_INODE, 0, 0, blocksize, 0);
}

/**
 * @brief Clear the VM cache for a device.
 */
int vm_clear_cache(dev_t dev) {
  message m;

  assert(dev != NO_DEV);

  memset(&m, 0, sizeof(m));

  m.m_vmmcp.dev = dev;

  return _taskcall(VM_PROC_NR, VM_CLEARCACHE, &m);
}
