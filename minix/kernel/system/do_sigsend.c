/* The kernel call that is implemented in this file:
 *	m_type: SYS_SIGSEND
 *
 * The parameters for this kernel call are:
 * 	m_sigcalls.endpt	# process to call signal handler
 *	m_sigcalls.sigctx	# pointer to userspace sigmsg structure
 *
 */

#include "kernel/system.h"
// #include <signal.h> // Replaced
// #include <string.h> // Replaced

// Added kernel headers
#include <minix/kernel_types.h> // For k_errno_t, k_sigset_t (if sigcontext parts use it)
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <klib/include/kmemory.h>


#if USE_SIGSEND

/*===========================================================================*
 *			      do_sigsend				     *
 *===========================================================================*/
int do_sigsend(struct proc * caller, message * m_ptr)
{
/* Handle sys_sigsend, POSIX-style signal handling. */

  struct sigmsg smsg; // FIXME: struct sigmsg is likely undefined now
  register struct proc *rp;
  struct sigframe_sigcontext fr, *frp; // FIXME: struct sigframe_sigcontext is likely undefined now
  int proc_nr, r;
#if defined(__i386__)
  reg_t new_fp;
#endif
  vir_bytes userspace_sigmsg_ptr = (vir_bytes)m_ptr->m_sigcalls.sigctx;

  if (!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr)) return EINVAL; // EINVAL might be undefined
  if (iskerneln(proc_nr)) return EPERM; // EPERM might be undefined
  rp = proc_addr(proc_nr);

  /* Get the sigmsg structure into our address space.  */
  // FIXME: sizeof(struct sigmsg) will be problematic
  if ((r = data_copy_vmcheck(caller, caller->p_endpoint,
		(vir_bytes)m_ptr->m_sigcalls.sigctx, KERNEL,
		(vir_bytes)&smsg, (phys_bytes) sizeof(struct sigmsg))) != OK)
	return r;

  /* WARNING: the following code may be run more than once even for a single
   * signal delivery. Do not change registers here. See the comment below.
   */

  /* Compute the user stack pointer where sigframe will start. */
  // FIXME: smsg.sm_stkptr will be problematic
  // FIXME: smsg.sm_stkptr will be problematic
  smsg.sm_stkptr = arch_get_sp(rp);
  frp = (struct sigframe_sigcontext *) arch_get_sp(rp) - 1;
  ksm.sm_stkptr = (vir_bytes)frp; /* Kernel determines the actual user stack ptr for the frame */

  /* Copy the registers to the sigcontext structure. */
  kmemset(&fr, 0, sizeof(fr)); // MODIFIED
  fr.sf_scp = &frp->sf_sc; // FIXME: sf_scp, sf_sc might be problematic

#if defined(__i386__)
  // FIXME: all fr.sf_sc.sc_* and fr.sf_* fields will be problematic
  fr.sf_sc.sc_gs = rp->p_reg.gs;
  fr.sf_sc.sc_fs = rp->p_reg.fs;
  fr.sf_sc.sc_es = rp->p_reg.es;
  fr.sf_sc.sc_ds = rp->p_reg.ds;
  fr.sf_sc.sc_edi = rp->p_reg.di;
  fr.sf_sc.sc_esi = rp->p_reg.si;
  fr.sf_sc.sc_ebp = rp->p_reg.fp;
  fr.sf_sc.sc_ebx = rp->p_reg.bx;
  fr.sf_sc.sc_edx = rp->p_reg.dx;
  fr.sf_sc.sc_ecx = rp->p_reg.cx;
  fr.sf_sc.sc_eax = rp->p_reg.retreg;
  fr.sf_sc.sc_eip = rp->p_reg.pc;
  fr.sf_sc.sc_cs = rp->p_reg.cs;
  fr.sf_sc.sc_eflags = rp->p_reg.psw;
  fr.sf_sc.sc_esp = rp->p_reg.sp; /* This is the user SP *before* pushing the frame */
  fr.sf_sc.sc_ss = rp->p_reg.ss;

  fr.sf_fp = rp->p_reg.fp; /* Old frame pointer, for completeness in sigframe */
  fr.sf_signum = ksm.sm_signo; /* Signal number */
  new_fp = (reg_t) &frp->sf_fp; /* New frame ptr will point to sf_fp in the frame on stack */
  fr.sf_scpcopy = fr.sf_scp;    /* Pointer to k_sigcontext on stack */
  fr.sf_ra_sigreturn = ksm.sm_sigreturn; /* Userspace _sigreturn address */
  fr.sf_ra = rp->p_reg.pc;      /* Return address after signal handler (current PC) */

  fr.sf_sc.sc_trap_style = rp->p_seg.p_kern_trap_style; /* Save how kernel was entered */

  if (fr.sf_sc.trap_style == KTS_NONE) {
	kprintf_stub("do_sigsend: sigsend an unsaved process\n"); // MODIFIED
	return EINVAL; // EINVAL might be undefined
  }

  if (proc_used_fpu(rp)) {
	/* save the FPU context before saving it to the sig context */
	save_fpu(rp);
	kmemcpy(&fr.sf_sc.sc_fpu_state, rp->p_seg.fpu_state, FPU_XFP_SIZE); // MODIFIED
  }
#endif

#if defined(__arm__)
  // FIXME: all fr.sf_sc.sc_* fields will be problematic
  fr.sf_sc.sc_spsr = rp->p_reg.psr;
  fr.sf_sc.sc_r0 = rp->p_reg.retreg;
  fr.sf_sc.sc_r1 = rp->p_reg.r1;
  fr.sf_sc.sc_r2 = rp->p_reg.r2;
  fr.sf_sc.sc_r3 = rp->p_reg.r3;
  fr.sf_sc.sc_r4 = rp->p_reg.r4;
  fr.sf_sc.sc_r5 = rp->p_reg.r5;
  fr.sf_sc.sc_r6 = rp->p_reg.r6;
  fr.sf_sc.sc_r7 = rp->p_reg.r7;
  fr.sf_sc.sc_r8 = rp->p_reg.r8;
  fr.sf_sc.sc_r9 = rp->p_reg.r9;
  fr.sf_sc.sc_r10 = rp->p_reg.r10;
  fr.sf_sc.sc_r11 = rp->p_reg.fp;   /* R11 is usually Frame Pointer */
  fr.sf_sc.sc_r12 = rp->p_reg.r12;  /* R12 is IP */
  fr.sf_sc.sc_usr_sp = rp->p_reg.sp;/* This is the user SP *before* pushing the frame */
  fr.sf_sc.sc_usr_lr = rp->p_reg.lr;/* User mode Link Register */
  fr.sf_sc.sc_pc = rp->p_reg.pc;    /* PC to return to after signal (current PC) */
  fr.sf_sc.sc_trap_style = rp->p_seg.p_kern_trap_style; /* Save how kernel was entered */
  /* ARM FPU state would be handled here if supported */
#endif

  /* Finish the sigcontext initialization. */
  // FIXME: smsg.sm_mask, fr.sf_sc.sc_mask, fr.sf_sc.sc_flags, fr.sf_sc.sc_magic, SC_MAGIC will be problematic
  fr.sf_sc.sc_mask = smsg.sm_mask;
  fr.sf_sc.sc_flags = rp->p_misc_flags & MF_FPU_INITIALIZED;
  fr.sf_sc.sc_magic = 0; /* SC_MAGIC might be undefined */

  /* Initialize the sigframe structure (arch-specific parts if any beyond sc). */
  /* fpu_sigcontext might be an arch-specific call to finalize FPU parts in frame */
  fpu_sigcontext(rp, &fr, &fr.sf_sc); /* This call might need adjustment if fr.sf_sc is now k_sigcontext_t */

  /* Copy the sigframe structure to the user's stack. */
  // FIXME: sizeof(struct sigframe_sigcontext) will be problematic
  if ((r = data_copy_vmcheck(caller, KERNEL, (vir_bytes)&fr,
		m_ptr->m_sigcalls.endpt, (vir_bytes)frp,
		(vir_bytes)sizeof(struct sigframe_sigcontext))) != OK)
      return r;

  /* WARNING: up to the statement above, the code may run multiple times, since
   * copying out the frame/context may fail with VMSUSPEND the first time. For
   * that reason, changes to process registers *MUST* be deferred until after
   * this last copy -- otherwise, these changes will be made several times,
   * possibly leading to corrupted process state.
   */

  /* Reset user registers to execute the signal handler. */
  rp->p_reg.sp = (reg_t) frp;
  rp->p_reg.pc = (reg_t) smsg.sm_sighandler; // FIXME: smsg.sm_sighandler problematic

#if defined(__i386__)
  rp->p_reg.fp = new_fp;
#elif defined(__arm__)
  /* use the ARM link register to set the return address from the signal
   * handler
   */
  rp->p_reg.lr = (reg_t) smsg.sm_sigreturn; // FIXME: smsg.sm_sigreturn problematic
  if(rp->p_reg.lr & 1) { kprintf_stub("sigsend: LSB LR makes no sense.\n"); } // MODIFIED

  /* pass signal handler parameters in registers */
  rp->p_reg.retreg = (reg_t) smsg.sm_signo; // FIXME: smsg.sm_signo problematic
  rp->p_reg.r1 = 0;	/* sf_code */
  rp->p_reg.r2 = (reg_t) fr.sf_scp; // FIXME: fr.sf_scp problematic
  rp->p_misc_flags |= MF_CONTEXT_SET;
#endif

  /* Signal handler should get clean FPU. */
  rp->p_misc_flags &= ~MF_FPU_INITIALIZED;

  if(!RTS_ISSET(rp, RTS_PROC_STOP)) {
	kprintf_stub("system: warning: sigsend a running process\n"); // MODIFIED
	kprintf_stub("caller stack: "); // MODIFIED
	kprintf_stub("system: warning: sigsend a running process\n"); // MODIFIED
	kprintf_stub("caller stack: "); // MODIFIED
	proc_stacktrace(caller);
  }

  return OK;
}

#endif /* USE_SIGSEND */
