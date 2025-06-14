/**
 * @file do_sigsend.c
 * @brief SYS_SIGSEND: deliver a POSIX‐style signal to a target process.
 *
 * This kernel call is invoked by a signal manager (typically PM) to
 * arrange user‐level signal delivery. It performs the following steps:
 *  1. Validate the target endpoint and ensure it is a user process.
 *  2. Copy a minimal sigmsg (signo, mask, handler, sigreturn) from the
 *     caller’s address space into a kernel sigmsg struct.
 *  3. Allocate space on the target’s user stack for a k_sigframe_sigcontext.
 *  4. Initialize the frame with the target’s current register state,
 *     the new signal context, and the information from the sigmsg.
 *  5. Copy the frame into the target’s address space (may suspend/resume VM).
 *  6. Update the target’s registers (SP, PC, FP/LR, args) to invoke the
 *     signal handler with the correct context.
 *  7. Clear the FPU‐initialized flag so the handler starts with a clean FPU.
 */

#include "kernel/system.h"

#include <minix/endpoint.h>          /* endpoint_t, isokendpt(), NONE */
#include <minix/const.h>             /* BEG_USER_ADDR, END_PROC_ADDR */
#include <minix/com.h>               /* iskerneln(), isemptyp() */
#include <minix/kernel_types.h>      /* k_sigset_t, k_sigmsg_t */
#include <klib/include/kmemory.h>    /* kmemset(), kmemcpy() */
#include <klib/include/kprintf.h>    /* kprintf_stub() */
#include <klib/include/kstring.h>    /* arch_get_sp() */
#include "kernel/ksignal.h"          /* k_sigmsg_t, k_sigcontext_t,
                                        k_sigframe_sigcontext,
                                        fpu_sigcontext() */
#include "kernel/k_spinlock_irq.h"   /* spinlock_irq_t,
                                        spin_lock_irqsave(),
                                        spin_unlock_irqrestore() */
#include <sys/kassert.h>             /* KASSERT() */

#if USE_SIGSEND

/**
 * @brief Handle the SYS_SIGSEND kernel call.
 * @param caller The signal manager invoking this call (e.g., PM).
 * @param m_ptr  Message with:
 *                 - m_sigcalls.endpt  : target endpoint
 *                 - m_sigcalls.sigctx : userspace pointer to struct sigmsg
 * @return OK on success, or a negative error code.
 *
 * Performs:
 *  - Endpoint validation (user only).
 *  - Field‐by‐field copy of struct sigmsg (signo, mask, handler, sigreturn).
 *  - Signal frame allocation and initialization on the target’s stack.
 *  - Copy of the frame into the target’s memory.
 *  - Register updates to transfer control to the handler.
 *  - FPU context flag reset.
 *  - Optional warning if the target was still runnable.
 */
int do_sigsend(struct proc *caller, message *m_ptr)
{
    endpoint_t target_ep = m_ptr->m_sigcalls.endpt;
    vir_bytes   user_sigmsg = (vir_bytes)m_ptr->m_sigcalls.sigctx;
    struct proc *rp;
    k_sigmsg_t   ksm = { 0 };
    struct sigframe_sigcontext frame = { 0 };
    struct sigframe_sigcontext *frame_ptr;
    int          target_nr, err;
#if defined(__i386__)
    reg_t        new_fp;
#endif

    /* 1) Validate the target endpoint */
    if (!isokendpt(target_ep, &target_nr)) {
        return EINVAL;
    }
    if (iskerneln(target_nr)) {
        return EPERM;
    }
    rp = proc_addr(target_nr);
    KASSERT(rp != NULL, "do_sigsend: invalid proc pointer for ep %d", target_ep);

    /* 2) Copy minimal sigmsg fields from user space */
    err = data_copy_vmcheck(caller, caller->p_endpoint,
                            user_sigmsg + K_OFFSETOF(struct sigmsg, sm_signo),
                            KERNEL, (vir_bytes)&ksm.sm_signo,
                            sizeof(ksm.sm_signo));
    if (err != OK) return err;

    err = data_copy_vmcheck(caller, caller->p_endpoint,
                            user_sigmsg + K_OFFSETOF(struct sigmsg, sm_mask),
                            KERNEL, (vir_bytes)&ksm.sm_mask,
                            sizeof(ksm.sm_mask));
    if (err != OK) return err;

    err = data_copy_vmcheck(caller, caller->p_endpoint,
                            user_sigmsg + K_OFFSETOF(struct sigmsg, sm_sighandler),
                            KERNEL, (vir_bytes)&ksm.sm_sighandler,
                            sizeof(ksm.sm_sighandler));
    if (err != OK) return err;

    err = data_copy_vmcheck(caller, caller->p_endpoint,
                            user_sigmsg + K_OFFSETOF(struct sigmsg, sm_sigreturn),
                            KERNEL, (vir_bytes)&ksm.sm_sigreturn,
                            sizeof(ksm.sm_sigreturn));
    if (err != OK) return err;

    /* 3) Allocate space on the user stack for the signal frame */
    frame_ptr    = (struct sigframe_sigcontext *)arch_get_sp(rp) - 1;
    ksm.sm_stkptr = (vir_bytes)frame_ptr;

    /* 4) Initialize the in‐kernel frame template */
    frame.sf_scp = &frame_ptr->sf_sc;

#if defined(__i386__)
    /* Copy general registers */
    frame.sf_sc.sc_gs     = rp->p_reg.gs;
    frame.sf_sc.sc_fs     = rp->p_reg.fs;
    frame.sf_sc.sc_es     = rp->p_reg.es;
    frame.sf_sc.sc_ds     = rp->p_reg.ds;
    frame.sf_sc.sc_edi    = rp->p_reg.di;
    frame.sf_sc.sc_esi    = rp->p_reg.si;
    frame.sf_sc.sc_ebp    = rp->p_reg.fp;
    frame.sf_sc.sc_ebx    = rp->p_reg.bx;
    frame.sf_sc.sc_edx    = rp->p_reg.dx;
    frame.sf_sc.sc_ecx    = rp->p_reg.cx;
    frame.sf_sc.sc_eax    = rp->p_reg.retreg;
    frame.sf_sc.sc_eip    = rp->p_reg.pc;
    frame.sf_sc.sc_cs     = rp->p_reg.cs;
    frame.sf_sc.sc_eflags = rp->p_reg.psw;
    frame.sf_sc.sc_esp    = rp->p_reg.sp;
    frame.sf_sc.sc_ss     = rp->p_reg.ss;

    frame.sf_fp           = rp->p_reg.fp;
    frame.sf_signum       = ksm.sm_signo;
    frame.sf_scpcopy      = frame.sf_scp;
    frame.sf_ra_sigreturn = ksm.sm_sigreturn;
    frame.sf_ra           = rp->p_reg.pc;

    frame.sf_sc.sc_trap_style = rp->p_seg.p_kern_trap_style;
    if (frame.sf_sc.sc_trap_style == KTS_NONE) {
        kprintf_stub("do_sigsend: unsaved trap style\n");
        return EINVAL;
    }

    /* Save or clear FPU state */
    if (proc_used_fpu(rp)) {
        save_fpu(rp);
        kmemcpy(&frame.sf_sc.sc_fpu_state,
               rp->p_seg.fpu_state,
               FPU_XFP_SIZE);
        frame.sf_sc.sc_fpu_flags = _MC_FPU_SAVED;
    } else {
        frame.sf_sc.sc_fpu_flags = 0;
    }

    new_fp = (reg_t)&frame_ptr->sf_fp;

#elif defined(__arm__)
    /* ARM register copy */
    frame.sf_sc.sc_spsr   = rp->p_reg.psr;
    frame.sf_sc.sc_r0     = rp->p_reg.retreg;
    frame.sf_sc.sc_r1     = rp->p_reg.r1;
    frame.sf_sc.sc_r2     = rp->p_reg.r2;
    frame.sf_sc.sc_r3     = rp->p_reg.r3;
    frame.sf_sc.sc_r4     = rp->p_reg.r4;
    frame.sf_sc.sc_r5     = rp->p_reg.r5;
    frame.sf_sc.sc_r6     = rp->p_reg.r6;
    frame.sf_sc.sc_r7     = rp->p_reg.r7;
    frame.sf_sc.sc_r8     = rp->p_reg.r8;
    frame.sf_sc.sc_r9     = rp->p_reg.r9;
    frame.sf_sc.sc_r10    = rp->p_reg.r10;
    frame.sf_sc.sc_r11    = rp->p_reg.fp;
    frame.sf_sc.sc_r12    = rp->p_reg.r12;
    frame.sf_sc.sc_usr_sp = rp->p_reg.sp;
    frame.sf_sc.sc_usr_lr = rp->p_reg.lr;
    frame.sf_sc.sc_pc     = rp->p_reg.pc;
    frame.sf_sc.sc_trap_style = rp->p_seg.p_kern_trap_style;

    frame.sf_fp           = rp->p_reg.fp;
    frame.sf_signum       = ksm.sm_signo;
    frame.sf_scpcopy      = frame.sf_scp;
    frame.sf_ra_sigreturn = ksm.sm_sigreturn;
    frame.sf_ra           = rp->p_reg.pc;

#else
#  error "Unsupported architecture for do_sigsend"
#endif

    /* Finalize k_sigcontext fields */
    frame.sf_sc.sc_mask  = ksm.sm_mask;
    frame.sf_sc.sc_magic = KSIGCONTEXT_MAGIC;
    fpu_sigcontext(rp, &frame, &frame.sf_sc);

    /* 5) Copy the frame into the target’s user stack */
    err = data_copy_vmcheck(caller, KERNEL, (vir_bytes)&frame,
                            target_ep, (vir_bytes)frame_ptr,
                            sizeof(frame));
    if (err != OK) {
        return err;
    }

    /* 6) Update the target’s registers for handler entry */
    rp->p_reg.sp = (reg_t)frame_ptr;
    rp->p_reg.pc = (reg_t)ksm.sm_sighandler;
#if defined(__i386__)
    rp->p_reg.fp = new_fp;
#elif defined(__arm__)
    rp->p_reg.lr       = (reg_t)ksm.sm_sigreturn;
    rp->p_reg.retreg  = (reg_t)ksm.sm_signo;
    rp->p_reg.r1      = 0;               /* code = 0 */
    rp->p_reg.r2      = (reg_t)frame.sf_scp;
    rp->p_misc_flags |= MF_CONTEXT_SET;
#endif

    /* 7) Clear FPU‐initialized flag for a clean FPU in handler */
    rp->p_misc_flags &= ~MF_FPU_INITIALIZED;

    /* Warn if the target was still runnable */
    if (!RTS_ISSET(rp, RTS_PROC_STOP)) {
        kprintf_stub("sigsend: delivering to running process %d\n", target_ep);
        proc_stacktrace(caller);
    }

    return OK;
}

#endif /* USE_SIGSEND */
