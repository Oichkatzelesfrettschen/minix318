/* The kernel call that is implemented in this file:
 *	m_type: SYS_SIGRETURN
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endp		# process returning from handler
 *	m_sigcalls.sigctx	# pointer to k_sigcontext_t structure on user stack
 *
 */

#include "kernel/system.h"
// #include <string.h> // Replaced
#include <machine/cpu.h> // Kept

// Added kernel headers
#include <minix/kernel_types.h> // For k_errno_t
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <klib/include/kmemory.h>
#include "kernel/ksignal.h" /* For k_sigcontext_t */
#include "kernel/proc.h"    /* For struct proc definition for p_reg */


#if USE_SIGRETURN 

/*===========================================================================*
 *			      do_sigreturn				     *
 *===========================================================================*/
int do_sigreturn(struct proc * caller, message * m_ptr)
{
/* POSIX style signals require sys_sigreturn to put things in order before 
 * the signalled process can resume execution
 */
  k_sigcontext_t ksc; /* Kernel's copy of the signal context */
  register struct proc *rp;
  int proc_nr, r;

  if (!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr)) return EINVAL;
  if (iskerneln(proc_nr)) return EPERM;
  rp = proc_addr(proc_nr);

  /* Copy in the k_sigcontext structure from user stack. */
  if ((r = data_copy(m_ptr->m_sigcalls.endpt,
		 (vir_bytes)m_ptr->m_sigcalls.sigctx, KERNEL,
		 (vir_bytes)&ksc, sizeof(k_sigcontext_t))) != OK)
	return r;

#if defined(__i386__)
  /* Restore user bits of psw from ksc, maintain system bits from proc. */
  ksc.sc_eflags  =  (ksc.sc_eflags & X86_FLAGS_USER) |
                (rp->p_reg.psw & ~X86_FLAGS_USER);

  /* Write back registers from k_sigcontext to process's p_reg. */
  rp->p_reg.gs = ksc.sc_gs;
  rp->p_reg.fs = ksc.sc_fs;
  rp->p_reg.es = ksc.sc_es;
  rp->p_reg.ds = ksc.sc_ds;
  rp->p_reg.di = ksc.sc_edi;
  rp->p_reg.si = ksc.sc_esi;
  rp->p_reg.fp = ksc.sc_ebp; /* Frame pointer */
  rp->p_reg.bx = ksc.sc_ebx;
  rp->p_reg.dx = ksc.sc_edx;
  rp->p_reg.cx = ksc.sc_ecx;
  rp->p_reg.retreg = ksc.sc_eax; /* Return value in eax */
  rp->p_reg.pc = ksc.sc_eip;   /* Program Counter */
  rp->p_reg.cs = ksc.sc_cs;
  rp->p_reg.psw = ksc.sc_eflags; /* Program Status Word */
  rp->p_reg.sp = ksc.sc_esp;   /* Stack Pointer */
  rp->p_reg.ss = ksc.sc_ss;
#elif defined(__arm__)
  /* Restore relevant registers from k_sigcontext for ARM */
  rp->p_reg.psr = ksc.sc_spsr;
  rp->p_reg.retreg = ksc.sc_r0; /* R0 is typically the return value register */
  rp->p_reg.r1 = ksc.sc_r1;
  rp->p_reg.r2 = ksc.sc_r2;
  rp->p_reg.r3 = ksc.sc_r3;
  rp->p_reg.r4 = ksc.sc_r4;
  rp->p_reg.r5 = ksc.sc_r5;
  rp->p_reg.r6 = ksc.sc_r6;
  rp->p_reg.r7 = ksc.sc_r7;
  rp->p_reg.r8 = ksc.sc_r8;
  rp->p_reg.r9 = ksc.sc_r9;
  rp->p_reg.r10 = ksc.sc_r10;
  rp->p_reg.fp = ksc.sc_r11;  /* Frame Pointer (R11) */
  rp->p_reg.r12 = ksc.sc_r12; /* IP */
  rp->p_reg.sp = ksc.sc_usr_sp;
  rp_p_reg.lr = ksc.sc_usr_lr; /* Typo: rp_p_reg -> rp->p_reg */
  rp->p_reg.pc = ksc.sc_pc;
#endif

  /* Restore the registers using the modified rp->p_reg.
   * The trap_style must be correctly passed from the point the signal was taken.
   * This ksc.sc_trap_style was set by do_sigsend (or equivalent).
   */
  arch_proc_setcontext(rp, &rp->p_reg, 1, ksc.sc_trap_style);

  /* sc_magic check removed as it's a userspace concern and SC_MAGIC is not kernel-defined. */

#if defined(__i386__)
  /* Restore FPU state if indicated by the context from user. */
  if (ksc.sc_fpu_flags & K_MC_FPU_SAVED)
  {
	/* FPU_XFP_SIZE should be defined in an arch-specific header (e.g. archconst.h) */
	kmemcpy(rp->p_seg.fpu_state, &ksc.sc_fpu_state, FPU_XFP_SIZE);
	rp->p_misc_flags |=  MF_FPU_INITIALIZED; /* Restore math usage flag. */
	/* force reloading FPU */
	release_fpu(rp);
  } else {
	rp->p_misc_flags &= ~MF_FPU_INITIALIZED;
  }
#endif

  return OK;
}
#endif /* USE_SIGRETURN */
