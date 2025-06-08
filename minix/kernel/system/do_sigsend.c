/* The kernel call that is implemented in this file:
 *	m_type: SYS_SIGSEND
 *
 * The parameters for this kernel call are:
 * 	m_sigcalls.endpt	# process to call signal handler
 *	m_sigcalls.sigctx	# pointer to userspace sigmsg structure
 *	m_sigcalls.sigctx	# pointer to userspace sigmsg structure
 *
 */

#include "kernel/system.h"
// #include <signal.h> // Replaced
// #include <string.h> // Replaced

// Added kernel headers
#include <minix/kernel_types.h> // For k_errno_t, k_sigset_t
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <klib/include/kmemory.h>
#include "kernel/ksignal.h" /* For k_sigmsg_t and signal frame setup */


#if USE_SIGSEND

/*===========================================================================*
 *			      do_sigsend				     *
 *===========================================================================*/
int do_sigsend(struct proc * caller, message * m_ptr)
{
/* Handle sys_sigsend, POSIX-style signal handling. */

  k_sigmsg_t ksm; /* Kernel's copy of crucial sigmsg fields */
  register struct proc *rp;
  /* FIXME: struct sigframe_sigcontext and its fields (sf_scp, sf_sc, etc.)
   * are problematic if not defined in a kernel-safe way.
   * This part of the code needs careful review against actual kernel-compatible
   * structure definitions for signal frames, possibly from <machine/frame.h>
   * or a new kernel-specific signal frame definition.
   * For now, we'll assume struct sigframe_sigcontext is available/compilable
   * but its usage needs to be audited once its definition is clear.
   */
  struct sigframe_sigcontext fr, *frp;
  int proc_nr, r;
#if defined(__i386__)
  reg_t new_fp;
#endif
  vir_bytes userspace_sigmsg_ptr = (vir_bytes)m_ptr->m_sigcalls.sigctx;
  vir_bytes userspace_sigmsg_ptr = (vir_bytes)m_ptr->m_sigcalls.sigctx;

  if (!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr)) return EINVAL;
  if (iskerneln(proc_nr)) return EPERM;
  rp = proc_addr(proc_nr);

  /* Copy necessary fields from userspace sigmsg structure individually. */
  /* This avoids pulling in the full userspace struct sigmsg definition. */
  /* Offsets are assumed based on a typical struct sigmsg layout. */
  /* These need to be defined based on the actual userspace struct sigmsg. */
  /* For demonstration, using direct field names assuming userspace struct sigmsg matches k_sigmsg_t for these fields */
  /* A more robust way would be to use K_OFFSETOF with a known (even if not directly included) userspace
   * struct definition for these offsets, or have PM pass them in the message m_ptr directly.
   * Let's assume PM passes these in the message for now, or they are standard offsets.
   * This part is highly dependent on how struct sigmsg is actually laid out in userspace
   * and what the kernel can safely assume or copy.
   * The example below simplifies this by directly assigning from message fields if they were there,
   * or by copying field-by-field if sigctx points to a structure.
   * Given the message structure, it seems sigctx is a pointer.
   */

  /* Copy sm_signo */
  if ((r = data_copy_vmcheck(caller, caller->p_endpoint, userspace_sigmsg_ptr + K_OFFSETOF(struct sigmsg, sm_signo),
	KERNEL, (vir_bytes)&ksm.sm_signo, sizeof(ksm.sm_signo))) != OK) return r;
  /* Copy sm_mask */
  if ((r = data_copy_vmcheck(caller, caller->p_endpoint, userspace_sigmsg_ptr + K_OFFSETOF(struct sigmsg, sm_mask),
	KERNEL, (vir_bytes)&ksm.sm_mask, sizeof(ksm.sm_mask))) != OK) return r;
  /* Copy sm_sighandler */
  if ((r = data_copy_vmcheck(caller, caller->p_endpoint, userspace_sigmsg_ptr + K_OFFSETOF(struct sigmsg, sm_sighandler),
	KERNEL, (vir_bytes)&ksm.sm_sighandler, sizeof(ksm.sm_sighandler))) != OK) return r;
  /* Copy sm_sigreturn */
  if ((r = data_copy_vmcheck(caller, caller->p_endpoint, userspace_sigmsg_ptr + K_OFFSETOF(struct sigmsg, sm_sigreturn),
	KERNEL, (vir_bytes)&ksm.sm_sigreturn, sizeof(ksm.sm_sigreturn))) != OK) return r;

  /* WARNING: the following code may be run more than once even for a single
   * signal delivery. Do not change registers here. See the comment below.
   */

  /* Compute the user stack pointer where sigframe will start. */
  // FIXME: smsg.sm_stkptr will be problematic
  // FIXME: smsg.sm_stkptr will be problematic
  smsg.sm_stkptr = arch_get_sp(rp);
  frp = (struct sigframe_sigcontext *) arch_get_sp(rp) - 1;
  ksm.sm_stkptr = (vir_bytes)frp; /* Kernel determines the actual user stack ptr for the frame */
  frp = (struct sigframe_sigcontext *) arch_get_sp(rp) - 1;
  ksm.sm_stkptr = (vir_bytes)frp; /* Kernel determines the actual user stack ptr for the frame */

  /* Copy the registers to the sigcontext part of the signal frame. */
  /* This assumes fr.sf_sc is of type k_sigcontext_t or compatible layout. */
  kmemset(&fr, 0, sizeof(fr));
  fr.sf_scp = &frp->sf_sc; /* Pointer to the k_sigcontext_t on user stack */

#if defined(__i386__)
  /* Populate the k_sigcontext_t (fr.sf_sc) using rp->p_reg */
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
  fr.sf_sc.sc_esp = rp->p_reg.sp; /* This is the user SP *before* pushing the frame */
  fr.sf_sc.sc_ss = rp->p_reg.ss;

  fr.sf_fp = rp->p_reg.fp; /* Old frame pointer, for completeness in sigframe */
  fr.sf_signum = ksm.sm_signo; /* Signal number */
  new_fp = (reg_t) &frp->sf_fp; /* New frame ptr will point to sf_fp in the frame on stack */
  fr.sf_scpcopy = fr.sf_scp;    /* Pointer to k_sigcontext on stack */
  fr.sf_ra_sigreturn = ksm.sm_sigreturn; /* Userspace _sigreturn address */
  fr.sf_ra = rp->p_reg.pc;      /* Return address after signal handler (current PC) */

  fr.sf_fp = rp->p_reg.fp; /* Old frame pointer, for completeness in sigframe */
  fr.sf_signum = ksm.sm_signo; /* Signal number */
  new_fp = (reg_t) &frp->sf_fp; /* New frame ptr will point to sf_fp in the frame on stack */
  fr.sf_scpcopy = fr.sf_scp;    /* Pointer to k_sigcontext on stack */
  fr.sf_ra_sigreturn = ksm.sm_sigreturn; /* Userspace _sigreturn address */
  fr.sf_ra = rp->p_reg.pc;      /* Return address after signal handler (current PC) */

  fr.sf_sc.sc_trap_style = rp->p_seg.p_kern_trap_style; /* Save how kernel was entered */
  fr.sf_sc.sc_trap_style = rp->p_seg.p_kern_trap_style; /* Save how kernel was entered */

  if (fr.sf_sc.sc_trap_style == KTS_NONE) {
	kprintf_stub("do_sigsend: sigsend an unsaved process\n");
	return EINVAL;
  }

  if (proc_used_fpu(rp)) {
	save_fpu(rp); /* Ensure FPU state is in rp->p_seg.fpu_state */
	kmemcpy(&fr.sf_sc.sc_fpu_state, rp->p_seg.fpu_state, FPU_XFP_SIZE);
    fr.sf_sc.sc_fpu_flags = _MC_FPU_SAVED; /* Indicate FPU state is present */
  } else {
    fr.sf_sc.sc_fpu_flags = 0;
  }
#endif

#if defined(__arm__)
  /* ARM specific register setup for signal frame */
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
  fr.sf_sc.sc_r11 = rp->p_reg.fp;   /* R11 is usually Frame Pointer */
  fr.sf_sc.sc_r12 = rp->p_reg.r12;  /* R12 is IP */
  fr.sf_sc.sc_usr_sp = rp->p_reg.sp;/* This is the user SP *before* pushing the frame */
  fr.sf_sc.sc_usr_lr = rp->p_reg.lr;/* User mode Link Register */
  fr.sf_sc.sc_pc = rp->p_reg.pc;    /* PC to return to after signal (current PC) */
  fr.sf_sc.sc_trap_style = rp->p_seg.p_kern_trap_style; /* Save how kernel was entered */
  /* ARM FPU state would be handled here if supported */
#endif

  /* Finish the k_sigcontext_t initialization. */
  fr.sf_sc.sc_mask = ksm.sm_mask;
  /* sc_fpu_flags handled in arch-specific block for i386 */
  fr.sf_sc.sc_magic = KSIGCONTEXT_MAGIC;

  /* Initialize the sigframe structure (arch-specific parts if any beyond sc). */
  /* fpu_sigcontext might be an arch-specific call to finalize FPU parts in frame */
  fpu_sigcontext(rp, &fr, &fr.sf_sc); /* This call might need adjustment if fr.sf_sc is now k_sigcontext_t */

  /* Copy the sigframe structure to the user's stack. */
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
  rp->p_reg.pc = (reg_t) ksm.sm_sighandler;

#if defined(__i386__)
  rp->p_reg.fp = new_fp;
#elif defined(__arm__)
  /* use the ARM link register to set the return address from the signal
   * handler
   */
  rp->p_reg.lr = (reg_t) ksm.sm_sigreturn;
  if(rp->p_reg.lr & 1) { kprintf_stub("sigsend: LSB LR makes no sense.\n"); }

  /* pass signal handler parameters in registers */
  rp->p_reg.retreg = (reg_t) ksm.sm_signo;
  rp->p_reg.r1 = 0;	/* sf_code */
  rp->p_reg.r2 = (reg_t) fr.sf_scp;
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
