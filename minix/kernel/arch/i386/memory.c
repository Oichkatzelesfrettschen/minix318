#include "kernel/kernel.h"
#include "kernel/vm.h"

#include <machine/vm.h> // Kept (appears twice, will be one)

// #include <minix/syslib.h> // Removed
#include <minix/cpufeature.h> // Kept
// #include <string.h>       // Replaced
// #include <assert.h>       // Replaced
// #include <signal.h>       // Removed
// #include <stdlib.h>       // Removed

// Added kernel headers
#include <minix/kernel_types.h> // For k_size_t, k_errno_t, fixed-width types
#include <sys/kassert.h>
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <klib/include/kmemory.h>


#include "oxpcie.h"
#include "arch_proto.h"

#ifdef USE_APIC
#include "apic.h"
#ifdef USE_WATCHDOG
#include "kernel/watchdog.h"
#endif
#endif

phys_bytes video_mem_vaddr = 0;

#define HASPT(procptr) ((procptr)->p_seg.p_cr3 != 0)
static int nfreepdes = 0;
#define MAXFREEPDES	2
static int freepdes[MAXFREEPDES];

static u32_t phys_get32(phys_bytes v);

void mem_clear_mapcache(void)
{
	int i;
	for(i = 0; i < nfreepdes; i++) {
		struct proc *ptproc = get_cpulocal_var(ptproc);
		int pde = freepdes[i];
		u32_t *ptv;
		KASSERT(ptproc);
		ptv = ptproc->p_seg.p_cr3_v;
		KASSERT(ptv);
		ptv[pde] = 0;
	}
}

/* This function sets up a mapping from within the kernel's address
 * space to any other area of memory, either straight physical
 * memory (pr == NULL) or a process view of memory, in 4MB windows.
 * I.e., it maps in 4MB chunks of virtual (or physical) address space
 * to 4MB chunks of kernel virtual address space.
 *
 * It recognizes pr already being in memory as a special case (no
 * mapping required).
 *
 * The target (i.e. in-kernel) mapping area is one of the freepdes[]
 * VM has earlier already told the kernel about that is available. It is
 * identified as the 'pde' parameter. This value can be chosen freely
 * by the caller, as long as it is in range (i.e. 0 or higher and corresponds
 * to a known freepde slot). It is up to the caller to keep track of which
 * freepde's are in use, and to determine which ones are free to use.
 *
 * The logical number supplied by the caller is translated into an actual
 * pde number to be used, and a pointer to it (linear address) is returned
 * for actual use by phys_copy or memset.
 */
static phys_bytes createpde(
	const struct proc *pr,	/* Requested process, NULL for physical. */
	const phys_bytes linaddr,/* Address after segment translation. */
	phys_bytes *bytes,	/* Size of chunk, function may truncate it. */
	int free_pde_idx,	/* index of the free slot to use */
	int *changed		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_bytes offset;
	int pde;

	KASSERT(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	KASSERT(pde >= 0 && pde < 1024);

	if(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		return linaddr;
	}

	if(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		KASSERT(pr->p_seg.p_cr3_v);
		pdeval = pr->p_seg.p_cr3_v[I386_VM_PDE(linaddr)];
	} else {
		/* Requested address is physical. Make up the PDE entry. */
		pdeval = (linaddr & I386_VM_ADDR_MASK_4MB) | 
			I386_VM_BIGPAGE | I386_VM_PRESENT | 
			I386_VM_WRITE | I386_VM_USER;
	}

	/* Write the pde value that we need into a pde that the kernel
	 * can access, into the currently loaded page table so it becomes
	 * visible.
	 */
	KASSERT(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	if(get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] != pdeval) {
		get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] = pdeval;
		*changed = 1;
	}

	/* Memory is now available, but only the 4MB window of virtual
	 * address space that we have mapped; calculate how much of
	 * the requested range is visible and return that in *bytes,
	 * if that is less than the requested range.
	 */
	offset = linaddr & I386_VM_OFFSET_MASK_4MB; /* Offset in 4MB window. */
	*bytes = MIN(*bytes, I386_BIG_PAGE_SIZE - offset); 

	/* Return the linear address of the start of the new mapping. */
	return I386_BIG_PAGE_SIZE*pde + offset;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
static int check_resumed_caller(struct proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	if (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		KASSERT(caller->p_vmrequest.vmresult != VMSUSPEND);
		return caller->p_vmrequest.vmresult;
	}

	return OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
static int lin_lin_copy(struct proc *srcproc, vir_bytes srclinaddr, 
	struct proc *dstproc, vir_bytes dstlinaddr, vir_bytes bytes)
{
	u32_t addr;
	proc_nr_t procslot;

	KASSERT(get_cpulocal_var(ptproc));
	KASSERT(get_cpulocal_var(proc_ptr));
	KASSERT(read_cr3() == get_cpulocal_var(ptproc)->p_seg.p_cr3);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	KASSERT(procslot >= 0 && procslot < I386_VM_DIR_ENTRIES);

	if(srcproc) KASSERT(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	if(dstproc) KASSERT(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	KASSERT(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	KASSERT(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	if(srcproc) KASSERT(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	if(dstproc) KASSERT(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	while(bytes > 0) {
		phys_bytes srcptr, dstptr;
		vir_bytes chunk = bytes;
		int changed = 0;

#ifdef CONFIG_SMP
		unsigned cpu = cpuid;

		if (srcproc && GET_BIT(srcproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(srcproc->p_stale_tlb, cpu);
		}
		if (dstproc && GET_BIT(dstproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(dstproc->p_stale_tlb, cpu);
		}
#endif

		/* Set up 4MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		if(changed)
			reload_cr3(); 

		/* Check for overflow. */
		if (srcptr + chunk < srcptr) return EFAULT_SRC; // EFAULT_SRC might be undefined
		if (dstptr + chunk < dstptr) return EFAULT_DST; // EFAULT_DST might be undefined

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		if(addr) {
			/* If addr is nonzero, a page fault was caught. */

			if(addr >= srcptr && addr < (srcptr + chunk)) {
				return EFAULT_SRC; // EFAULT_SRC might be undefined
			}
			if(addr >= dstptr && addr < (dstptr + chunk)) {
				return EFAULT_DST; // EFAULT_DST might be undefined
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			return EFAULT; // EFAULT might be undefined
		}

		/* Update counter and addresses for next iteration, if any. */
		bytes -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	if(srcproc) KASSERT(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	if(dstproc) KASSERT(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	KASSERT(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	KASSERT(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);

	return OK;
}


static u32_t phys_get32(phys_bytes addr)
{
	u32_t v;
	int r;

	if((r=lin_lin_copy(NULL, addr, // NULL might be undefined
		proc_addr(SYSTEM), (phys_bytes) &v, sizeof(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	return v;
}

#if 0
static char *cr0_str(u32_t e)
{
	static char str[80];
	(void)kstrlcpy(str, "", sizeof(str)); /* FIXME: strcpy was here ... */ // MODIFIED
#define FLAG(v) do { if(e & (v)) { /* FIXME: strcat was here */ /* kstrcat(str, #v " ", sizeof(str)); */ e &= ~v; } } while(0)
	FLAG(I386_CR0_PE);
	FLAG(I386_CR0_MP);
	FLAG(I386_CR0_EM);
	FLAG(I386_CR0_TS);
	FLAG(I386_CR0_ET);
	FLAG(I386_CR0_PG);
	FLAG(I386_CR0_WP);
	if(e) { /* FIXME: strcat was here */ /* kstrcat(str, " (++)", sizeof(str)); */ }
	return str;
}

static char *cr4_str(u32_t e)
{
	static char str[80];
	(void)kstrlcpy(str, "", sizeof(str)); /* FIXME: strcpy was here ... */ // MODIFIED
	FLAG(I386_CR4_VME);
	FLAG(I386_CR4_PVI);
	FLAG(I386_CR4_TSD);
	FLAG(I386_CR4_DE);
	FLAG(I386_CR4_PSE);
	FLAG(I386_CR4_PAE);
	FLAG(I386_CR4_MCE);
	FLAG(I386_CR4_PGE);
	if(e) { /* FIXME: strcat was here */ /* kstrcat(str, " (++)", sizeof(str)); */ }
	return str;
}
#endif

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_bytes umap_virtual(
  register struct proc *rp,		/* pointer to proc table entry for process */
  int seg,				/* T, D, or S segment */
  vir_bytes vir_addr,			/* virtual address in bytes within the seg */
  vir_bytes bytes			/* # of bytes to be copied */
)
{
	phys_bytes phys = 0;

	if(vm_lookup(rp, vir_addr, &phys, NULL) != OK) { // NULL might be undefined
		kprintf_stub("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr); // MODIFIED
		phys = 0;
	} else {
		if(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	if(phys == 0) {
		kprintf_stub("SYSTEM:umap_virtual: lookup failed\n"); // MODIFIED
		return 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	if(bytes > 0 && vm_lookup_range(rp, vir_addr, NULL, bytes) != bytes) { // NULL might be undefined
		kprintf_stub("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n", // MODIFIED
			rp->p_name, (unsigned long)bytes, vir_addr, vir_addr); // MODIFIED k_size_t cast for %lu
		return 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	KASSERT(phys);

	return phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
int vm_lookup(const struct proc *proc, const vir_bytes virtual,
 phys_bytes *physical, u32_t *ptent)
{
	u32_t *root, *pt;
	int pde, pte;
	u32_t pde_v, pte_v;

	KASSERT(proc);
	KASSERT(physical);
	KASSERT(!isemptyp(proc));
	KASSERT(HASPT(proc));

	/* Retrieve page directory entry. */
	root = (u32_t *) proc->p_seg.p_cr3;
	KASSERT(!((u32_t) root % I386_PAGE_SIZE));
	pde = I386_VM_PDE(virtual);
	KASSERT(pde >= 0 && pde < I386_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (root + pde));

	if(!(pde_v & I386_VM_PRESENT)) {
		return EFAULT; // EFAULT might be undefined
	}

	/* We don't expect to ever see this. */
	if(pde_v & I386_VM_BIGPAGE) {
		*physical = pde_v & I386_VM_ADDR_MASK_4MB;
		if(ptent) *ptent = pde_v;
		*physical += virtual & I386_VM_OFFSET_MASK_4MB;
	} else {
		/* Retrieve page table entry. */
		pt = (u32_t *) I386_VM_PFA(pde_v);
		KASSERT(!((u32_t) pt % I386_PAGE_SIZE));
		pte = I386_VM_PTE(virtual);
		KASSERT(pte >= 0 && pte < I386_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		if(!(pte_v & I386_VM_PRESENT)) {
			return EFAULT; // EFAULT might be undefined
		}

		if(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = I386_VM_PFA(pte_v);
		*physical += virtual % I386_PAGE_SIZE;
	}

	return OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
k_size_t vm_lookup_range(const struct proc *proc, vir_bytes vir_addr, // MODIFIED size_t
	phys_bytes *phys_addr, k_size_t bytes) // MODIFIED size_t
{
	/* Look up the physical address corresponding to linear virtual address
	 * 'vir_addr' for process 'proc'. Return the size of the range covered
	 * by contiguous physical memory starting from that address; this may
	 * be anywhere between 0 and 'bytes' inclusive. If the return value is
	 * nonzero, and 'phys_addr' is non-NULL, 'phys_addr' will be set to the
	 * base physical address of the range. 'vir_addr' and 'bytes' need not
	 * be page-aligned, but the caller must have verified that the given
	 * linear range is valid for the given process at all.
	 */
	phys_bytes phys, next_phys;
	k_size_t len; // MODIFIED size_t

	KASSERT(proc);
	KASSERT(bytes > 0);
	KASSERT(HASPT(proc));

	/* Look up the first page. */
	if (vm_lookup(proc, vir_addr, &phys, NULL) != OK) // NULL might be undefined
		return 0;

	if (phys_addr != NULL) // NULL might be undefined
		*phys_addr = phys;

	len = I386_PAGE_SIZE - (vir_addr % I386_PAGE_SIZE);
	vir_addr += len;
	next_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	while (len < bytes) {
		if (vm_lookup(proc, vir_addr, &phys, NULL) != OK) // NULL might be undefined
			break;

		if (next_phys != phys)
			break;

		len += I386_PAGE_SIZE;
		vir_addr += I386_PAGE_SIZE;
		next_phys += I386_PAGE_SIZE;
	}

	/* We might now have overshot the requested length somewhat. */
	return MIN(bytes, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
int vm_check_range(struct proc *caller, struct proc *target,
	vir_bytes vir_addr, k_size_t bytes, int writeflag) // MODIFIED size_t
{
	/* Public interface to vm_suspend(), for use by kernel calls. On behalf
	 * of 'caller', call into VM to check linear virtual address range of
	 * process 'target', starting at 'vir_addr', for 'bytes' bytes. This
	 * function assumes that it will called twice if VM returned an error
	 * the first time (since nothing has changed in that case), and will
	 * then return the error code resulting from the first call. Upon the
	 * first call, a non-success error code is returned as well.
	 */
	int r;

	if ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		return r;

	vm_suspend(caller, target, vir_addr, bytes, VMSTYPE_KERNELCALL,
		writeflag);

	return VMSUSPEND;
}

#if 0
static char *flagstr(u32_t e, const int dir)
{
	static char str[80];
	(void)kstrlcpy(str, "", sizeof(str)); /* FIXME: strcpy was here ... */ // MODIFIED
#define FLAG(v) do { if(e & (v)) { /* FIXME: strcat was here */ /* kstrcat(str, #v " ", sizeof(str)); */ e &= ~v; } } while(0)
	FLAG(I386_VM_PRESENT);
	FLAG(I386_VM_WRITE);
	FLAG(I386_VM_USER);
	FLAG(I386_VM_PWT);
	FLAG(I386_VM_PCD);
	FLAG(I386_VM_GLOBAL);
	if(dir)
		FLAG(I386_VM_BIGPAGE);	/* Page directory entry only */
	else
		FLAG(I386_VM_DIRTY);	/* Page table entry only */
	return str;
}

static void vm_pt_print(u32_t *pagetable, const u32_t v)
{
	int pte;
	int col = 0;

	KASSERT(!((u32_t) pagetable % I386_PAGE_SIZE));

	for(pte = 0; pte < I386_VM_PT_ENTRIES; pte++) {
		u32_t pte_v, pfa;
		pte_v = phys_get32((u32_t) (pagetable + pte));
		if(!(pte_v & I386_VM_PRESENT))
			continue;
		pfa = I386_VM_PFA(pte_v);
		kprintf_stub("%4d:%08lx:%08lx %2s ", // MODIFIED
			pte, v + I386_PAGE_SIZE*pte, pfa,
			(pte_v & I386_VM_WRITE) ? "rw":"RO");
		col++;
		if(col == 3) { kprintf_stub("\n"); col = 0; } // MODIFIED
	}
	if(col > 0) kprintf_stub("\n"); // MODIFIED

	return;
}

static void vm_print(u32_t *root)
{
	int pde;

	KASSERT(!((u32_t) root % I386_PAGE_SIZE));

	kprintf_stub("page table 0x%lx:\n", root); // MODIFIED

	for(pde = 0; pde < I386_VM_DIR_ENTRIES; pde++) {
		u32_t pde_v;
		u32_t *pte_a;
		pde_v = phys_get32((u32_t) (root + pde));
		if(!(pde_v & I386_VM_PRESENT))
			continue;
		if(pde_v & I386_VM_BIGPAGE) {
			kprintf_stub("%4d: 0x%lx, flags %s\n", // MODIFIED
				pde, I386_VM_PFA(pde_v), flagstr(pde_v, 1));
		} else {
			pte_a = (u32_t *) I386_VM_PFA(pde_v);
			kprintf_stub("%4d: pt %08lx %s\n", // MODIFIED
				pde, pte_a, flagstr(pde_v, 1));
			vm_pt_print(pte_a, pde * I386_VM_PT_ENTRIES * I386_PAGE_SIZE);
			kprintf_stub("\n"); // MODIFIED
		}
	}


	return;
}
#endif

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
int vm_memset(struct proc* caller, endpoint_t who, phys_bytes ph, int c,
	phys_bytes count)
{
	u32_t pattern;
	struct proc *whoptr = NULL; // NULL might be undefined
	phys_bytes cur_ph = ph;
	phys_bytes left = count;
	phys_bytes ptr, chunk, pfa = 0;
	int new_cr3, r = OK;

	if ((r = check_resumed_caller(caller)) != OK)
		return r;

	/* NONE for physical, otherwise virtual */
	if (who != NONE && !(whoptr = endpoint_lookup(who)))
		return ESRCH; // ESRCH might be undefined

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	KASSERT(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	KASSERT(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 4MB chunk we mapped in.
	 */
	while (left > 0) {
		new_cr3 = 0;
		chunk = left;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_cr3);

		if (new_cr3)
			reload_cr3();

		/* If a page fault happens, pfa is non-null */
		if ((pfa = phys_memset(ptr, pattern, chunk))) {

			/* If a process pagefaults, VM may help out */
			if (whoptr) {
				vm_suspend(caller, whoptr, ph, count,
						   VMSTYPE_KERNELCALL, 1);
				KASSERT(catch_pagefaults);
				catch_pagefaults = 0;
				return VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		left -= chunk;
	}

	KASSERT(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	KASSERT(catch_pagefaults);
	catch_pagefaults = 0;

	return OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
int virtual_copy_f(
  struct proc * caller,
  struct vir_addr *src_addr,		/* source virtual address */
  struct vir_addr *dst_addr,		/* destination virtual address */
  vir_bytes bytes,			/* # of bytes to copy  */
  int vmcheck				/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  struct vir_addr *vir_addr[2];	/* virtual source and destination address */
  int i, r;
  struct proc *procs[2];

  KASSERT((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  if (bytes <= 0) return(EDOM); // EDOM might be undefined

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  for (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	int proc_nr;
	struct proc *p;

	if(proc_e == NONE) {
		p = NULL; // NULL might be undefined
	} else {
		if(!isokendpt(proc_e, &proc_nr)) {
			kprintf_stub("virtual_copy: no reasonable endpoint\n"); // MODIFIED
			return ESRCH; // ESRCH might be undefined
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  if ((r = check_resumed_caller(caller)) != OK)
	return r;

  if((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->offset,
  	procs[_DST_], vir_addr[_DST_]->offset, bytes)) != OK) {
	int writeflag;
	struct proc *target = NULL; // NULL might be undefined
  	phys_bytes lin;
	if(r != EFAULT_SRC && r != EFAULT_DST) // EFAULT* might be undefined
  		panic("lin_lin_copy failed: %d",  r);
  	if(!vmcheck || !caller) {
    		return r;
  	}

	if(r == EFAULT_SRC) { // EFAULT_SRC might be undefined
  		lin = vir_addr[_SRC_]->offset;
  		target = procs[_SRC_];
		writeflag = 0;
	} else if(r == EFAULT_DST) { // EFAULT_DST might be undefined
  		lin = vir_addr[_DST_]->offset;
  		target = procs[_DST_];
		writeflag = 1;
  	} else {
  		panic("r strange: %d",  r);
  	}

	KASSERT(caller);
	KASSERT(target);

	vm_suspend(caller, target, lin, bytes, VMSTYPE_KERNELCALL, writeflag);
	return VMSUSPEND;
  }

  return OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
int data_copy(const endpoint_t from_proc, const vir_bytes from_addr,
	const endpoint_t to_proc, const vir_bytes to_addr,
	k_size_t bytes) // MODIFIED size_t
{
  struct vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  KASSERT(src.proc_nr_e != NONE);
  KASSERT(dst.proc_nr_e != NONE);

  return virtual_copy(&src, &dst, bytes);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
int data_copy_vmcheck(struct proc * caller,
	const endpoint_t from_proc, const vir_bytes from_addr,
	const endpoint_t to_proc, const vir_bytes to_addr,
	k_size_t bytes) // MODIFIED size_t
{
  struct vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  KASSERT(src.proc_nr_e != NONE);
  KASSERT(dst.proc_nr_e != NONE);

  return virtual_copy_vmcheck(caller, &src, &dst, bytes);
}

void memory_init(void)
{
	KASSERT(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_start++;
	freepdes[nfreepdes++] = kinfo.freepde_start++;

	KASSERT(kinfo.freepde_start < I386_VM_DIR_ENTRIES);
	KASSERT(nfreepdes == 2);
	KASSERT(nfreepdes <= MAXFREEPDES);
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
void arch_proc_init(struct proc *pr, const u32_t ip, const u32_t sp,
	const u32_t ps_str, char *name)
{
	arch_proc_reset(pr);
	kstrlcpy(pr->p_name, name, sizeof(pr->p_name)); // MODIFIED

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.bx = ps_str;
}

static int oxpcie_mapping_index = -1,
	lapic_mapping_index = -1,
	ioapic_first_index = -1,
	ioapic_last_index = -1,
	video_mem_mapping_index = -1,
	usermapped_glo_index = -1,
	usermapped_index = -1, first_um_idx = -1;

extern char *video_mem;

extern char usermapped_start, usermapped_end, usermapped_nonglo_start;

int arch_phys_map(const int index,
			phys_bytes *addr,
			phys_bytes *len,
			int *flags)
{
	static int first = 1;
	int freeidx = 0;
	static char *ser_var = NULL; // NULL might be undefined
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	if(first) {
		kmemset(&minix_kerninfo, 0, sizeof(minix_kerninfo)); // MODIFIED
		video_mem_mapping_index = freeidx++;
		if(glo_len > 0) {
			usermapped_glo_index = freeidx++;
		}
		
		usermapped_index = freeidx++;
		first_um_idx = usermapped_index;
		if(usermapped_glo_index != -1)
			first_um_idx = usermapped_glo_index;

#ifdef USE_APIC
		if(lapic_addr)
			lapic_mapping_index = freeidx++;
		if (ioapic_enabled) {
			ioapic_first_index = freeidx;
			KASSERT(nioapics > 0);
			freeidx += nioapics;
			ioapic_last_index = freeidx-1;
		}
#endif

#ifdef CONFIG_OXPCIE
		if((ser_var = env_get("oxpcie"))) {
			if(ser_var[0] != '0' || ser_var[1] != 'x') {
				kprintf_stub("oxpcie address in hex please\n"); // MODIFIED
			} else {
				kprintf_stub("oxpcie address is %s\n", ser_var); // MODIFIED
				oxpcie_mapping_index = freeidx++;
			}
		}
#endif

		first = 0;
	}

	if(index == usermapped_glo_index) {
		*addr = vir2phys(&usermapped_start);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		return OK;
	}
	else if(index == usermapped_index) {
		*addr = vir2phys(&usermapped_nonglo_start);
		*len = (u32_t) &usermapped_end -
			(u32_t) &usermapped_nonglo_start;
		*flags = VMMF_USER;
		return OK;
	}
	else if (index == video_mem_mapping_index) {
		/* map video memory in so we can print panic messages */
		*addr = MULTIBOOT_VIDEO_BUFFER;
		*len = I386_PAGE_SIZE;
		*flags = VMMF_WRITE;
		return OK;
	}
#ifdef USE_APIC
	else if (index == lapic_mapping_index) {
		/* map the local APIC if enabled */
		if (!lapic_addr)
			return EINVAL; // EINVAL might be undefined
		*addr = lapic_addr;
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		return OK;
	}
	else if (ioapic_enabled && index >= ioapic_first_index && index <= ioapic_last_index) {
		int ioapic_idx = index - ioapic_first_index;
		*addr = io_apic[ioapic_idx].paddr;
		KASSERT(*addr);
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		kprintf_stub("ioapic map: addr 0x%lx\n", *addr); // MODIFIED
		return OK;
	}
#endif

#if CONFIG_OXPCIE
	if(index == oxpcie_mapping_index) {
		*addr = 0 /* FIXME: strtoul(ser_var+2, NULL, 16) replaced */; // MODIFIED (NULL might be undefined)
		*len = 0x4000;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		return OK;
	}
#endif

	return EINVAL; // EINVAL might be undefined
}

int arch_phys_map_reply(const int index, const vir_bytes addr)
{
#ifdef USE_APIC
	/* if local APIC is enabled */
	if (index == lapic_mapping_index && lapic_addr) {
		lapic_addr_vaddr = addr;
		return OK;
	}
	else if (ioapic_enabled && index >= ioapic_first_index &&
		index <= ioapic_last_index) {
		int i = index - ioapic_first_index;
		io_apic[i].vaddr = addr;
		return OK;
	}
#endif

#if CONFIG_OXPCIE
	if (index == oxpcie_mapping_index) {
		oxpcie_set_vaddr((unsigned char *) addr);
		return OK;
	}
#endif
	if(index == first_um_idx) {
		extern struct minix_ipcvecs minix_ipcvecs_sysenter,
			minix_ipcvecs_syscall,
			minix_ipcvecs_softint;
		extern u32_t usermapped_offset;
		KASSERT(addr > (u32_t) &usermapped_start);
		usermapped_offset = addr - (u32_t) &usermapped_start;
#define FIXEDPTR(ptr) (void *) ((u32_t)ptr + usermapped_offset)
#define FIXPTR(ptr) ptr = FIXEDPTR(ptr)
#define ASSIGN(minixstruct) minix_kerninfo.minixstruct = FIXEDPTR(&minixstruct)
		ASSIGN(kinfo);
		ASSIGN(machine);
		ASSIGN(kmessages);
		ASSIGN(loadinfo);
		ASSIGN(kuserinfo);
		ASSIGN(arm_frclock); /* eh, why not. */
		ASSIGN(kclockinfo);

		/* select the right set of IPC routines to map into processes */
		if(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
			DEBUGBASIC(("kernel: selecting intel sysenter ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_sysenter;
		} else  if(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
			DEBUGBASIC(("kernel: selecting amd syscall ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_syscall;
		} else	{
			DEBUGBASIC(("kernel: selecting fallback (int) ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_softint;
		}

		/* adjust the pointers of the functions and the struct
		 * itself to the user-accessible mapping
		 */
		FIXPTR(minix_kerninfo.minix_ipcvecs->send);
		FIXPTR(minix_kerninfo.minix_ipcvecs->receive);
		FIXPTR(minix_kerninfo.minix_ipcvecs->sendrec);
		FIXPTR(minix_kerninfo.minix_ipcvecs->senda);
		FIXPTR(minix_kerninfo.minix_ipcvecs->sendnb);
		FIXPTR(minix_kerninfo.minix_ipcvecs->notify);
		FIXPTR(minix_kerninfo.minix_ipcvecs->do_kernel_call);
		FIXPTR(minix_kerninfo.minix_ipcvecs);

		minix_kerninfo.kerninfo_magic = KERNINFO_MAGIC;
		minix_kerninfo.minix_feature_flags = minix_feature_flags;
		minix_kerninfo_user = (vir_bytes) FIXEDPTR(&minix_kerninfo);

		/* if libc_ipc is set, disable usermapped ipc functions
		 * and force binaries to use in-libc fallbacks.
		 */
		if(env_get("libc_ipc")) {
			kprintf_stub("kernel: forcing in-libc fallback ipc style\n"); // MODIFIED
			minix_kerninfo.minix_ipcvecs = NULL; // NULL might be undefined
		} else {
			minix_kerninfo.ki_flags |= MINIX_KIF_IPCVECS;
		}

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		return OK;
	}

	if(index == usermapped_index) return OK;

	if (index == video_mem_mapping_index) {
		video_mem_vaddr =  addr;
		return OK;
	}

	return EINVAL; // EINVAL might be undefined
}

int arch_enable_paging(struct proc * caller)
{
	KASSERT(caller->p_seg.p_cr3);

	/* load caller's page table */
	switch_address_space(caller);

	video_mem = (char *) video_mem_vaddr;

#ifdef USE_APIC
	/* start using the virtual addresses */

	/* if local APIC is enabled */
	if (lapic_addr) {
		lapic_addr = lapic_addr_vaddr;
		lapic_eoi_addr = LAPIC_EOI;
	}
	/* if IO apics are enabled */
	if (ioapic_enabled) {
		int i;

		for (i = 0; i < nioapics; i++) {
			io_apic[i].addr = io_apic[i].vaddr;
		}
	}
#if CONFIG_SMP
	barrier();

	wait_for_APs_to_finish_booting();
#endif
#endif

#ifdef USE_WATCHDOG
	/*
	 * We make sure that we don't enable the watchdog until paging is turned
	 * on as we might get an NMI while switching and we might still use wrong
	 * lapic address. Bad things would happen. It is unfortunate but such is
	 * life
	 */
	if (watchdog_enabled)
		i386_watchdog_start();
#endif

	return OK;
}

void release_address_space(struct proc *pr)
{
	pr->p_seg.p_cr3_v = NULL; // NULL might be undefined
}

/* computes a checksum of a buffer of a given length. The byte sum must be zero */
int platform_tbl_checksum_ok(void *ptr, unsigned int length)
{
	u8_t total = 0; // u8_t might be undefined
	unsigned int i;
	for (i = 0; i < length; i++)
		total += ((unsigned char *)ptr)[i];
	return !total;
}

int platform_tbl_ptr(phys_bytes start,
					phys_bytes end,
					unsigned increment,
					void * buff,
					unsigned size, // Should this be k_size_t?
					phys_bytes * phys_addr,
					int ((* cmp_f)(void *)))
{
	phys_bytes addr;

	for (addr = start; addr < end; addr += increment) {
		phys_copy (addr, (phys_bytes) buff, size);
		if (cmp_f(buff)) {
			if (phys_addr)
				*phys_addr = addr;
			return 1;
		}
	}
	return 0;
}
