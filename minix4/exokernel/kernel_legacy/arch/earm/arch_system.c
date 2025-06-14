/**
 * @file arch_system.c
 * @brief ARM architecture-specific system functions for the MINIX kernel
 * 
 * This file contains ARM-specific implementations of system-dependent functions
 * used throughout the kernel. It handles processor initialization, context
 * switching, floating-point unit management, and other low-level ARM operations.
 * 
 * Key functionality includes:
 * - Process context management and switching
 * - ARM processor identification and initialization
 * - Performance monitoring unit (PMU) configuration
 * - Memory management unit (MMU) address space switching
 * - System call handling for ARM architecture
 * - Serial debugging interface initialization
 * 
 * The file is part of the ARM port of MINIX and provides the hardware
 * abstraction layer between the generic kernel and ARM-specific features.
 * 
 * @note This implementation includes stubs for FPU operations as floating-point
 *       support may not be available on all target ARM platforms.
 * 
 * @warning Some functions contain duplicate lines that should be reviewed
 *          and corrected (marked with MODIFIED comments).
 */
/* system dependent functions for use inside the whole kernel. */

#include "kernel/kernel.h"

// Removed: <unistd.h>, <ctype.h>, <string.h>, <assert.h>, <signal.h>
// Removed: <unistd.h>, <ctype.h>, <string.h>, <assert.h>, <signal.h>
#include <minix/cpufeature.h>
// Kept: <machine/vm.h>, <machine/signal.h>, <arm/armreg.h>, <minix/u64.h>
// Kept: <machine/vm.h>, <machine/signal.h>, <arm/armreg.h>, <minix/u64.h>
#include <machine/vm.h>
#include <machine/signal.h> // May need review if it pulls userspace defs
#include <arm/armreg.h>     // Arch-specific, likely okay
#include <minix/u64.h>      // Minix-specific, likely okay


// Added kernel headers
#include <klib/include/kstring.h>
#include <klib/include/kmemory.h>
#include <sys/kassert.h>
#include <klib/include/kprintf.h>
#include <minix/kernel_types.h>


#include "archconst.h"
#include "arch_proto.h"
#include "kernel/proc.h"
#include "kernel/debug.h"
#include "ccnt.h"
#include "bsp_init.h"
#include "bsp_serial.h"

#include "glo.h"

void * k_stacks;


void fpu_init(void)
{
}

void save_local_fpu(struct proc *pr, int retain)
{
}

void save_fpu(struct proc *pr)
{
}

void arch_proc_reset(struct proc *pr)
{
	KASSERT(pr->p_nr < NR_PROCS);

	/* Clear process state. */
	kmemset(&pr->p_reg, 0, sizeof(pr->p_reg)); // MODIFIED
	kmemset(&pr->p_reg, 0, sizeof(pr->p_reg)); // MODIFIED
	if(iskerneln(pr->p_nr)) {
		pr->p_reg.psr = INIT_TASK_PSR;
	} else {
		pr->p_reg.psr = INIT_PSR;
	}
}

void arch_proc_setcontext(struct proc *p, struct stackframe_s *state,
	int isuser, int trapstyle)
{
        KASSERT(sizeof(p->p_reg) == sizeof(*state));
	if(state != &p->p_reg) {
	        kmemcpy(&p->p_reg, state, sizeof(*state)); // MODIFIED
	        kmemcpy(&p->p_reg, state, sizeof(*state)); // MODIFIED
	}

        /* further code is instructed to not touch the context
         * any more
         */
        p->p_misc_flags |= MF_CONTEXT_SET;

        if(!(p->p_rts_flags)) {
                kprintf_stub("WARNINIG: setting full context of runnable process\n"); // MODIFIED
                kprintf_stub("WARNINIG: setting full context of runnable process\n"); // MODIFIED
                print_proc(p);
                util_stacktrace();
        }
}

void arch_set_secondary_ipc_return(struct proc *p, u32_t val)
{
	p->p_reg.r1 = val;
}

int restore_fpu(struct proc *pr)
{
	return 0;
}

void cpu_identify(void)
{
	u32_t midr;
	unsigned cpu = cpuid;

	asm volatile("mrc p15, 0, %[midr], c0, c0, 0 @ read MIDR\n\t"
		     : [midr] "=r" (midr));

	cpu_info[cpu].implementer = midr >> 24;
	cpu_info[cpu].variant = (midr >> 20) & 0xF;
	cpu_info[cpu].arch = (midr >> 16) & 0xF;
	cpu_info[cpu].part = (midr >> 4) & 0xFFF;
	cpu_info[cpu].revision = midr & 0xF;
	cpu_info[cpu].freq = 660; /* 660 Mhz hardcoded */
}

void arch_init(void)
{
        u32_t value;

	k_stacks = (void*) &k_stacks_start;
	KASSERT(!((vir_bytes) k_stacks % K_STACK_SIZE));

#ifndef CONFIG_SMP
	/*
	 * use stack 0 and cpu id 0 on a single processor machine, SMP
	 * configuration does this in smp_init() for all cpus at once
	 */
	tss_init(0, get_k_stack_top(0));
#endif


        /* enable user space access to cycle counter */
        /* set cycle counter to 0: ARM ARM B4.1.113 and B4.1.117 */
        asm volatile ("MRC p15, 0, %0, c9, c12, 0\t\n": "=r" (value));
        value |= PMU_PMCR_C; /* Reset counter */
        value |= PMU_PMCR_E; /* Enable counter hardware */
        asm volatile ("MCR p15, 0, %0, c9, c12, 0\t\n": : "r" (value));

        /* enable CCNT counting: ARM ARM B4.1.116 */
        value = PMU_PMCNTENSET_C; /* Enable PMCCNTR cycle counter */
        asm volatile ("MCR p15, 0, %0, c9, c12, 1\t\n": : "r" (value));

        /* enable cycle counter in user mode: ARM ARM B4.1.124 */
        value = PMU_PMUSERENR_EN;
        asm volatile ("MCR p15, 0, %0, c9, c14, 0\t\n": : "r" (value));
	bsp_init();
}

/*===========================================================================*
 *				do_ser_debug				     * 
 *===========================================================================*/
void do_ser_debug(void)
{
}

void arch_do_syscall(struct proc *proc)
{
  /* do_ipc assumes that it's running because of the current process */
  KASSERT(proc == get_cpulocal_var(proc_ptr));
  /* Make the system call, for real this time. */
  proc->p_reg.retreg =
	  do_ipc(proc->p_reg.retreg, proc->p_reg.r1, proc->p_reg.r2);
}

reg_t svc_stack;

struct proc * arch_finish_switch_to_user(void)
{
	char * stk;
	struct proc * p;

#ifdef CONFIG_SMP
	stk = (char *)tss[cpuid].sp0;
#else
	stk = (char *)tss[0].sp0;
#endif
	svc_stack = (reg_t)stk;
	/* set pointer to the process to run on the stack */
	p = get_cpulocal_var(proc_ptr);
	*((reg_t *)stk) = (reg_t) p;

	/* turn interrupts on */
        p->p_reg.psr &= ~(PSR_I|PSR_F);

	return p;
}

void fpu_sigcontext(struct proc *pr, struct sigframe_sigcontext *fr, struct sigcontext *sc)
{
}

reg_t arch_get_sp(struct proc *p) { return p->p_reg.sp; }

void get_randomness(struct k_randomness *rand, int source)
{
}

void arch_ser_init(void)
{
	bsp_ser_init();
}

/*===========================================================================*/
/*			      __switch_address_space			     */
/*===========================================================================*/
/*
 * sets the ttbr register to the supplied value if it is not already set to the
 * same value in which case it would only result in an extra TLB flush which is
 * not desirable
 */
void __switch_address_space(struct proc *p, struct proc **__ptproc)
{
	reg_t orig_ttbr, new_ttbr;

	new_ttbr = p->p_seg.p_ttbr;
	if (new_ttbr == 0)
	    return;

	orig_ttbr = read_ttbr0();

	/*
	 * test if ttbr is loaded with the current value to avoid unnecessary
	 * TLB flushes
	 */
	if (new_ttbr == orig_ttbr)
	    return;

	write_ttbr0(new_ttbr);

	*__ptproc = p;

	return;
}
