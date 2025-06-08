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


#if USE_SIGRETURN 

/*===========================================================================*
 *			      do_sigreturn				     *
 *===========================================================================*/
int do_sigreturn(struct proc * caller, message * m_ptr)
{
/* POSIX style signals require sys_sigreturn to put things in order before 
 * the signalled process can resume execution
 */
  struct sigcontext sc; // FIXME: struct sigcontext is likely undefined now
  register struct proc *rp;
  int proc_nr, r;

  if (!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr)) return EINVAL; // EINVAL might be undefined
  if (iskerneln(proc_nr)) return EPERM; // EPERM might be undefined
  rp = proc_addr(proc_nr);

  /* Copy in the sigcontext structure. */
  // FIXME: sizeof(struct sigcontext) will be problematic
  if ((r = data_copy(m_ptr->m_sigcalls.endpt,
		 (vir_bytes)m_ptr->m_sigcalls.sigctx, KERNEL,
		 (vir_bytes)&ksc, sizeof(k_sigcontext_t))) != OK)
	return r;

#if defined(__i386__)
  /* Restore user bits of psw from sc, maintain system bits from proc. */
  // FIXME: sc.sc_eflags will be problematic
  sc.sc_eflags  =  (sc.sc_eflags & X86_FLAGS_USER) |
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

#if defined(__i386__)
  /* Write back registers we allow to be restored, i.e.
   * not the segment ones.
   */
  // FIXME: all sc.sc_* fields will be problematic
  rp->p_reg.di = sc.sc_edi;
  rp->p_reg.si = sc.sc_esi;
  rp->p_reg.fp = sc.sc_ebp;
  rp->p_reg.bx = sc.sc_ebx;
  rp->p_reg.dx = sc.sc_edx;
  rp->p_reg.cx = sc.sc_ecx;
  rp->p_reg.retreg = sc.sc_eax;
  rp->p_reg.pc = sc.sc_eip;
  rp->p_reg.psw = sc.sc_eflags;
  rp->p_reg.sp = sc.sc_esp;
#endif

#if defined(__arm__)
  // FIXME: all sc.sc_* fields will be problematic
  rp->p_reg.psr = sc.sc_spsr;
  rp->p_reg.retreg = sc.sc_r0;
  rp->p_reg.r1 = sc.sc_r1;
  rp->p_reg.r2 = sc.sc_r2;
  rp->p_reg.r3 = sc.sc_r3;
  rp->p_reg.r4 = sc.sc_r4;
  rp->p_reg.r5 = sc.sc_r5;
  rp->p_reg.r6 = sc.sc_r6;
  rp->p_reg.r7 = sc.sc_r7;
  rp->p_reg.r8 = sc.sc_r8;
  rp->p_reg.r9 = sc.sc_r9;
  rp->p_reg.r10 = sc.sc_r10;
  rp->p_reg.fp = sc.sc_r11;
  rp->p_reg.r12 = sc.sc_r12;
  rp->p_reg.sp = sc.sc_usr_sp;
  rp->p_reg.lr = sc.sc_usr_lr;
  rp->p_reg.pc = sc.sc_pc;
#endif

  /* Restore the registers. */
  // FIXME: sc.trap_style will be problematic
  arch_proc_setcontext(rp, &rp->p_reg, 1, sc.trap_style);

  // FIXME: sc.sc_magic and SC_MAGIC will be problematic
  if(sc.sc_magic != 0 /* SC_MAGIC */) { kprintf_stub("kernel sigreturn: corrupt signal context\n"); } // MODIFIED

#if defined(__i386__)
  // FIXME: sc.sc_flags and sc.sc_fpu_state will be problematic
  if (sc.sc_flags & MF_FPU_INITIALIZED)
  {
	kmemcpy(rp->p_seg.fpu_state, &sc.sc_fpu_state, FPU_XFP_SIZE); // MODIFIED
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
