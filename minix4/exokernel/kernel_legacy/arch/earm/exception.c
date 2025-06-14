/**
 * @file exception.c
 * @brief ARM exception handler for the MINIX kernel
 * 
 * This file implements exception handling for ARM architecture in MINIX kernel.
 * It handles various types of exceptions including data aborts, prefetch aborts,
 * and other ARM-specific exceptions. User process exceptions are converted to
 * signals, while kernel exceptions cause system panics.
 * 
 * Exception types handled:
 * - Reset
 * - Undefined instruction
 * - Supervisor call (SVC)
 * - Prefetch abort (instruction fetch fault)
 * - Data abort (data access fault)
 * - Hypervisor call
 * - Interrupt/Fast interrupt
 * 
 * The handler distinguishes between nested (kernel-level) and non-nested 
 * (user-level) exceptions, applying appropriate recovery mechanisms or
 * signaling processes as needed.
 * 
 * Key features:
 * - Page fault handling with VM integration
 * - Stack trace generation for debugging
 * - Special handling for kernel copy operations
 * - FPU exception management
 * - Support for both user and kernel space exceptions
 */
/* This file contains a simple exception handler.  Exceptions in user
 * processes are converted to signals. Exceptions in a kernel task cause
 * a panic.
 */

#include "kernel/kernel.h"
#include "arch_proto.h"
// #include <signal.h> // Removed
// #include <string.h> // Removed
// #include <assert.h> // Replaced
// #include <signal.h> // Removed
// #include <string.h> // Removed
// #include <assert.h> // Replaced
#include "kernel/proc.h"
#include "kernel/proto.h"
#include <machine/vm.h> // Kept for now

// Added kernel headers
#include <minix/kernel_types.h> // For k_sigset_t (not used directly, but signal.h was present)
#include <sys/kassert.h>
#include <klib/include/kprintf.h> // For KASSERT_PLACEHOLDER and kprintf_stub
#include <klib/include/kstring.h> // Precautionary
#include <klib/include/kmemory.h> // Precautionary


struct ex_s {
	char *msg;
	int signum;
};

static struct ex_s ex_data[] = {
	{ "Reset", 0},
	{ "Undefined instruction", SIGILL}, // SIGILL might be undefined
	{ "Undefined instruction", SIGILL}, // SIGILL might be undefined
	{ "Supervisor call", 0},
	{ "Prefetch Abort", SIGILL}, // SIGILL might be undefined
	{ "Data Abort", SIGSEGV},    // SIGSEGV might be undefined
	{ "Prefetch Abort", SIGILL}, // SIGILL might be undefined
	{ "Data Abort", SIGSEGV},    // SIGSEGV might be undefined
	{ "Hypervisor call", 0},
	{ "Interrupt", 0},
	{ "Fast Interrupt", 0},
};

static void inkernel_disaster(struct proc *saved_proc,
	reg_t *saved_lr, struct ex_s *ep, int is_nested);

extern int catch_pagefaults;

static void proc_stacktrace_execute(struct proc *whichproc, reg_t v_bp, reg_t pc);

static void pagefault( struct proc *pr,
			reg_t *saved_lr,
			int is_nested,
			u32_t pagefault_addr,
			u32_t pagefault_status)
{
	int in_physcopy = 0, in_memset = 0;

	message m_pagefault;
	int err;

	in_physcopy = (*saved_lr > (vir_bytes) phys_copy) &&
	   (*saved_lr < (vir_bytes) phys_copy_fault);

	in_memset = (*saved_lr > (vir_bytes) phys_memset) &&
	   (*saved_lr < (vir_bytes) memset_fault);

	if((is_nested || iskernelp(pr)) &&
		catch_pagefaults && (in_physcopy || in_memset)) {
		if (is_nested) {
			if(in_physcopy) {
				KASSERT(!in_memset);
				*saved_lr = (reg_t) phys_copy_fault_in_kernel;
			} else {
				*saved_lr = (reg_t) memset_fault_in_kernel;
			}
		}
		else {
			pr->p_reg.pc = (reg_t) phys_copy_fault;
			pr->p_reg.retreg = pagefault_addr;
		}

		return;
	}

	if(is_nested) {
		kprintf_stub("pagefault in kernel at pc 0x%lx address 0x%lx\n", // MODIFIED
		kprintf_stub("pagefault in kernel at pc 0x%lx address 0x%lx\n", // MODIFIED
			*saved_lr, pagefault_addr);
		inkernel_disaster(pr, saved_lr, NULL, is_nested);
	}

	/* VM can't handle page faults. */
	if(pr->p_endpoint == VM_PROC_NR) {
		/* Page fault we can't / don't want to
		 * handle.
		 */
		kprintf_stub("pagefault for VM on CPU %d, " // MODIFIED
		kprintf_stub("pagefault for VM on CPU %d, " // MODIFIED
			"pc = 0x%x, addr = 0x%x, flags = 0x%x, is_nested %d\n",
			cpuid, pr->p_reg.pc, pagefault_addr, pagefault_status,
			is_nested);
		proc_stacktrace(pr);
		kprintf_stub("pc of pagefault: 0x%lx\n", pr->p_reg.pc); // MODIFIED
		kprintf_stub("pc of pagefault: 0x%lx\n", pr->p_reg.pc); // MODIFIED
		panic("pagefault in VM");

		return;
	}

	/* Don't schedule this process until pagefault is handled. */
	RTS_SET(pr, RTS_PAGEFAULT);

	/* tell Vm about the pagefault */
	m_pagefault.m_source = pr->p_endpoint;
	m_pagefault.m_type   = VM_PAGEFAULT;
	m_pagefault.VPF_ADDR = pagefault_addr;
	m_pagefault.VPF_FLAGS = pagefault_status;

	if ((err = mini_send(pr, VM_PROC_NR,
					&m_pagefault, FROM_KERNEL))) {
		panic("WARNING: pagefault: mini_send returned %d\n", err);
	}

	return;
}

static void
data_abort(int is_nested, struct proc *pr, reg_t *saved_lr,
		       struct ex_s *ep, u32_t dfar, u32_t dfsr)
{
	/* Extract fault status bit [0:3, 10] from DFSR */
	u32_t fs = dfsr & 0x0F;
	fs |= ((dfsr >> 6) & 0x10);

	 /* Translation and permission faults are handled as pagefaults. */
	if (is_trans_fault(fs) || is_perm_fault(fs)) {
		pagefault(pr, saved_lr, is_nested, dfar, dfsr);
	} else if (!is_nested) {
		/* A user process caused some other kind of data abort. */
		int signum = SIGSEGV; // SIGSEGV might be undefined
		int signum = SIGSEGV; // SIGSEGV might be undefined

		if (is_align_fault(fs)) {
			signum = SIGBUS; // SIGBUS might be undefined
			signum = SIGBUS; // SIGBUS might be undefined
		} else {
			kprintf_stub("KERNEL: unknown data abort by proc %d sending " // MODIFIED
			kprintf_stub("KERNEL: unknown data abort by proc %d sending " // MODIFIED
			       "SIGSEGV (dfar=0x%lx dfsr=0x%lx fs=0x%lx)\n",
			       proc_nr(pr), dfar, dfsr, fs);
		}
		cause_sig(proc_nr(pr), signum);
	} else { /* is_nested */
		kprintf_stub("KERNEL: inkernel data abort - disaster (dfar=0x%lx " // MODIFIED
		kprintf_stub("KERNEL: inkernel data abort - disaster (dfar=0x%lx " // MODIFIED
		       "dfsr=0x%lx fs=0x%lx)\n", dfar, dfsr, fs);
		inkernel_disaster(pr, saved_lr, ep, is_nested);
	}
}

static void inkernel_disaster(struct proc *saved_proc,
	reg_t *saved_lr, struct ex_s *ep,
	int is_nested)
{
#if USE_SYSDEBUG
  if(ep)
	kprintf_stub("\n%s\n", ep->msg); // MODIFIED
	kprintf_stub("\n%s\n", ep->msg); // MODIFIED

  kprintf_stub("cpu %d is_nested = %d ", cpuid, is_nested); // MODIFIED
  kprintf_stub("cpu %d is_nested = %d ", cpuid, is_nested); // MODIFIED

  if (saved_proc) {
	  kprintf_stub("scheduled was: process %d (%s), ", saved_proc->p_endpoint, saved_proc->p_name); // MODIFIED
	  kprintf_stub("pc = 0x%x\n", (unsigned) saved_proc->p_reg.pc); // MODIFIED
	  kprintf_stub("scheduled was: process %d (%s), ", saved_proc->p_endpoint, saved_proc->p_name); // MODIFIED
	  kprintf_stub("pc = 0x%x\n", (unsigned) saved_proc->p_reg.pc); // MODIFIED
	  proc_stacktrace(saved_proc);

	  panic("Unhandled kernel exception");
  }

  /* in an early stage of boot process we don't have processes yet */
  panic("exception in kernel while booting, no saved_proc yet");

#endif /* USE_SYSDEBUG */
}

void exception_handler(int is_nested, reg_t *saved_lr, int vector)
{
/* An exception or unexpected interrupt has occurred. */
  struct ex_s *ep;
  struct proc *saved_proc;

  saved_proc = get_cpulocal_var(proc_ptr);

  ep = &ex_data[vector];

  KASSERT((vir_bytes) saved_lr >= kinfo.vir_kern_start);

  /*
   * handle special cases for nested problems as they might be tricky or filter
   * them out quickly if the traps are not nested
   */
  if (is_nested) {
	/*
	 * if a problem occurred while copying a message from userspace because
	 * of a wrong pointer supplied by userland, handle it the only way we
	 * can handle it ...
	 */
	if (((void*)*saved_lr >= (void*)copy_msg_to_user &&
			(void*)*saved_lr <= (void*)__copy_msg_to_user_end) ||
			((void*)*saved_lr >= (void*)copy_msg_from_user &&
			(void*)*saved_lr <= (void*)__copy_msg_from_user_end)) {
		switch(vector) {
		/* these error are expected */
		case DATA_ABORT_VECTOR:
			*saved_lr = (reg_t) __user_copy_msg_pointer_failure;
			return;
		default:
			panic("Copy involving a user pointer failed unexpectedly!");
		}
	}
  }

  if (vector == DATA_ABORT_VECTOR) {
	data_abort(is_nested, saved_proc, saved_lr, ep, read_dfar(), read_dfsr());
	return;
  }

  if (!is_nested && vector == PREFETCH_ABORT_VECTOR) {
	static int warned = FALSE;
	reg_t ifar = read_ifar(), ifsr = read_ifsr();

	/* The saved_lr is the instruction we're going to execute after
	 * the fault is handled; IFAR is the address that pagefaulted
	 * while fetching the instruction. As far as we know the two
	 * should be the same, if not this assumption will lead to very
	 * hard to debug problems (instruction executing being off by one)
	 * and this assumption needs re-examining.
	 *
	 * UPDATE: at least qemu-linaro does in fact sometimes generate faults
	 * with LR and IFAR differing by as many as 64 bytes.  While the page
	 * fault resolution code below handles this case just fine, the cause
	 * of this behavior is unknown.  We have not yet seen the same on
	 * actual hardware, which is why we warn about this problem once.
	 */
	if (*saved_lr != ifar && !warned) {
		kprintf_stub("KERNEL: prefetch abort with differing IFAR and LR\n"); // MODIFIED
		kprintf_stub("KERNEL: IFSR %"PRIx32" IFAR %"PRIx32" LR %"PRIx32" in " // MODIFIED
		kprintf_stub("KERNEL: prefetch abort with differing IFAR and LR\n"); // MODIFIED
		kprintf_stub("KERNEL: IFSR %"PRIx32" IFAR %"PRIx32" LR %"PRIx32" in " // MODIFIED
		    "%s/%d\n", ifsr, ifar, *saved_lr, saved_proc->p_name,
		    saved_proc->p_endpoint);
		warned = TRUE;
	}
	pagefault(saved_proc, saved_lr, is_nested, ifar, ifsr);
	return;
  }

  /* If an exception occurs while running a process, the is_nested variable
   * will be zero. Exceptions in interrupt handlers or system traps will make
   * is_nested non-zero.
   */
  if (is_nested == 0 && ! iskernelp(saved_proc)) {
	cause_sig(proc_nr(saved_proc), ep->signum);
	return;
  }

  /* Exception in system code. This is not supposed to happen. */
  inkernel_disaster(saved_proc, saved_lr, ep, is_nested);

  panic("return from inkernel_disaster");
}

#if USE_SYSDEBUG
/*===========================================================================*
 *				proc_stacktrace_execute			     *
 *===========================================================================*/
static void proc_stacktrace_execute(struct proc *whichproc, reg_t v_bp, reg_t pc)
{
	kprintf_stub("%-8.8s %6d 0x%lx \n", // MODIFIED
	kprintf_stub("%-8.8s %6d 0x%lx \n", // MODIFIED
		whichproc->p_name, whichproc->p_endpoint, pc);
}
#endif

void proc_stacktrace(struct proc *whichproc)
{
#if USE_SYSDEBUG
	proc_stacktrace_execute(whichproc, whichproc->p_reg.fp, whichproc->p_reg.pc);
#endif /* USE_SYSDEBUG */
}

void enable_fpu_exception(void)
{
}

void disable_fpu_exception(void)
{
}
