/**
 * @file arch_do_vmctl.c
 * @brief ARM-specific virtual memory control operations for the MINIX kernel
 *
 * This file implements the SYS_VMCTL kernel call for ARM architecture,
 * providing virtual memory management functionality including page table
 * management and TLB operations.
 */

/**
 * @brief Set the Translation Table Base Register (TTBR) for a process
 *
 * Updates the process's page table base register and virtual address mapping.
 * If the process is currently running, immediately updates the hardware TTBR0.
 * For the VM process, enables paging. Clears the VMINHIBIT flag to allow
 * the process to run with virtual memory enabled.
 *
 * @param p Pointer to the process structure
 * @param ttbr Physical address of the translation table base
 * @param v Virtual address pointer to the translation table
 */

/**
 * @brief ARM-specific virtual memory control handler
 *
 * Handles VM control operations for ARM architecture including:
 * - VMCTL_GET_PDBR: Retrieves the process's page directory base register (TTBR)
 * - VMCTL_SETADDRSPACE: Sets up new address space by configuring TTBR
 * - VMCTL_FLUSHTLB: Flushes the Translation Lookaside Buffer
 *
 * @param m_ptr Pointer to the request message containing operation parameters
 * @param p Pointer to the target process structure
 * @return OK on success, EINVAL for unsupported parameters
 *
 * @note Uses SVMCTL_PARAM to determine the operation type
 * @note Returns the requested value in SVMCTL_VALUE for GET operations
 */
/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#include "kernel/system.h"
// #include <assert.h> // Replaced
#include <minix/type.h> // Kept for now, may need future review

#include "arch_proto.h"

// Added kernel headers
#include <sys/kassert.h>
#include <klib/include/kprintf.h> // For KASSERT_PLACEHOLDER and kprintf_stub
#include <minix/kernel_types.h>
#include <klib/include/kstring.h> // Precautionary
#include <klib/include/kmemory.h> // Precautionary


static void set_ttbr(struct proc *p, u32_t ttbr, u32_t *v)
{
	/* Set process TTBR. */
	p->p_seg.p_ttbr = ttbr;
	KASSERT(p->p_seg.p_ttbr);
	p->p_seg.p_ttbr_v = v;
	if(p == get_cpulocal_var(ptproc)) {
		write_ttbr0(p->p_seg.p_ttbr);
	}
	if(p->p_nr == VM_PROC_NR) {
		if (arch_enable_paging(p) != OK)
			panic("arch_enable_paging failed");
	}
	RTS_UNSET(p, RTS_VMINHIBIT);
}

/*===========================================================================*
 *				arch_do_vmctl				     *
 *===========================================================================*/
int arch_do_vmctl(
  register message *m_ptr,	/* pointer to request message */
  struct proc *p
)
{
  switch(m_ptr->SVMCTL_PARAM) {
	case VMCTL_GET_PDBR:
		/* Get process page directory base reg (TTBR). */
		m_ptr->SVMCTL_VALUE = p->p_seg.p_ttbr;
		return OK;
	case VMCTL_SETADDRSPACE:
		set_ttbr(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		return OK;
	case VMCTL_FLUSHTLB:
	{
		reload_ttbr0();
		return OK;
	}
  }

  kprintf_stub("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM); // MODIFIED
  kprintf_stub("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM); // MODIFIED
  return EINVAL;
}
