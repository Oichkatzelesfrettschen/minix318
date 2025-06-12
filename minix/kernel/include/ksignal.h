#ifndef _KERNEL_KSIGNAL_H
#define _KERNEL_KSIGNAL_H

#include <minix/kernel_types.h>
#include <minix/signal.h> /* For sigset_t definition, assuming it's kernel-safe or will be k_sigset_t */
                          /* If not, k_sigset_t is in kernel_types.h */

/*
 * Kernel-internal structure to receive parameters for SYS_SIGSEND call
 * from the Process Manager (PM). This avoids direct use of userspace
 * struct sigmsg if its definition is problematic for the kernel.
 */
typedef struct {
    k_sigset_t sm_mask;     /* mask to restore when handler returns */
    vir_bytes sm_sighandler; /* address of handler */
    vir_bytes sm_sigreturn;  /* address of _sigreturn in C library */
    vir_bytes sm_stkptr;     /* user stack pointer */
    int sm_signo;            /* signal number being sent */
    /* Other fields from original sigmsg are omitted if not used by kernel */
    /* or if they are part of sigcontext_t which is handled separately. */
} k_sigmsg_t;

/*
 * Kernel-internal structure for sigcontext.
 * This should mirror the register layout needed for context switching
 * and FPU state if applicable. It's what the kernel can safely
 * restore from the user's stack after a signal handler.
 */
typedef struct {
#if defined(__i386__)
    k_uint32_t sc_gs;
    k_uint32_t sc_fs;
    k_uint32_t sc_es;
    k_uint32_t sc_ds;
    k_uint32_t sc_edi;
    k_uint32_t sc_esi;
    k_uint32_t sc_ebp;
    k_uint32_t sc_ebx;
    k_uint32_t sc_edx;
    k_uint32_t sc_ecx;
    k_uint32_t sc_eax;
    k_uint32_t sc_eip;
    k_uint32_t sc_cs;
    k_uint32_t sc_eflags;
    k_uint32_t sc_esp;
    k_uint32_t sc_ss;
    k_uint32_t sc_fpu_flags; /* To indicate FPU state presence/validity */
    unsigned char sc_fpu_state[512]; /* Aligned FPU save area size (FXSAVE) */
                                     /* Must match FPU_XFP_SIZE from arch */
    int sc_trap_style; /* Kernel trap style used for entry */
#elif defined(__arm__)
    k_uint32_t sc_spsr;
    k_uint32_t sc_r0;
    k_uint32_t sc_r1;
    k_uint32_t sc_r2;
    k_uint32_t sc_r3;
    k_uint32_t sc_r4;
    k_uint32_t sc_r5;
    k_uint32_t sc_r6;
    k_uint32_t sc_r7;
    k_uint32_t sc_r8;
    k_uint32_t sc_r9;
    k_uint32_t sc_r10;
    k_uint32_t sc_r11; /* Frame Pointer (fp) */
    k_uint32_t sc_r12; /* Intra-Procedure Call scratch register (ip) */
    k_uint32_t sc_usr_sp;
    k_uint32_t sc_usr_lr;
    k_uint32_t sc_pc;   /* Program Counter (r15) */
    int sc_trap_style; /* Kernel trap style used for entry */
    /* ARM FPU state can be added here if needed */
#else
#error "Unsupported architecture for k_sigcontext_t"
#endif
    k_sigset_t sc_mask; /* Signal mask to restore */
    k_uint32_t sc_magic;  /* Magic number to validate the context */
} k_sigcontext_t;

#define KSIGCONTEXT_MAGIC 0x4B534358 /* KSCX */

/* Placeholder for machine-dependent flag indicating FPU state is in sigcontext */
/* This would typically come from <machine/mcontext.h> or similar */
#define K_MC_FPU_SAVED (1 << 0) /* Example value, actual bit might differ */

/* Define signal constants if not available from a safe kernel header.
 * These are typically 1-based. _NSIG should be defined elsewhere (e.g. <signal.h> if safe, or limits.h).
 * For now, relying on system headers and hoping they are sanitized or will be.
 * If _NSIG is needed for k_sigset_t operations and not available, it's an issue.
 * kernel_types.h defines k_sigset_t as unsigned long, implying bitmask.
 * Max signal number should be less than (sizeof(k_sigset_t) * 8).
 */

// Placeholder for _NSIG if not found, adjust as per actual system limits
// #define K_NSIG (_SIGMAX + 1) // Common pattern, but _SIGMAX is from userspace <signal.h>
// For now, assume signal numbers passed are validated by PM and fit in k_sigset_t.

#endif /* _KERNEL_KSIGNAL_H */
