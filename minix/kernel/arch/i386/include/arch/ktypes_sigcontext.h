/**
 * @file ktypes_sigcontext.h
 * @brief Kernel-internal definition of the signal context structure for i386.
 *
 * This file defines `k_sigcontext_t` which holds the machine state
 * (registers, signal mask, etc.) saved/restored during signal handling
 * for the i386 architecture.
 */

#ifndef _ARCH_I386_KTYPES_SIGCONTEXT_H
#define _ARCH_I386_KTYPES_SIGCONTEXT_H

#ifdef __i386__

#include <minix/kernel_types.h> // For k_sigset_t, k_uint32_t

/**
 * @struct k_sigcontext_s
 * @brief Machine context for signal handling on i386.
 *
 * This structure stores the state of CPU registers and other relevant
 * information that needs to be preserved across a signal handler and
 * then restored.
 */
struct k_sigcontext_s {
    k_uint32_t sc_gs;  /**< Value of the GS segment register. */
    k_uint32_t sc_fs;  /**< Value of the FS segment register. */
    k_uint32_t sc_es;  /**< Value of the ES segment register. */
    k_uint32_t sc_ds;  /**< Value of the DS segment register. */
    k_uint32_t sc_edi; /**< Value of the EDI register. */
    k_uint32_t sc_esi; /**< Value of the ESI register. */
    k_uint32_t sc_ebp; /**< Value of the EBP register (frame pointer). */
    k_uint32_t sc_esp; /**< Value of the ESP register (kernel stack pointer after trap). */
    k_uint32_t sc_ebx; /**< Value of the EBX register. */
    k_uint32_t sc_edx; /**< Value of the EDX register. */
    k_uint32_t sc_ecx; /**< Value of the ECX register. */
    k_uint32_t sc_eax; /**< Value of the EAX register. */
    k_uint32_t sc_eip; /**< Value of the EIP register (instruction pointer). */
    k_uint32_t sc_cs;  /**< Value of the CS register (code segment). */
    k_uint32_t sc_eflags; /**< Value of the EFLAGS register. */
    k_uint32_t sc_esp_at_signal; /**< User-space ESP at the time of signal. */
    k_sigset_t sc_mask; /**< Signal mask to restore after handler. */
    // k_uint32_t sc_fpu_flags; /* Placeholder for FPU state flags */
    // unsigned char sc_fpu_state[512]; /* Placeholder for FPU state (FXSAVE area) */
    // int sc_trap_style; /* Placeholder for kernel trap style */
    // k_uint32_t sc_magic; /* Placeholder for magic number validation */
};

/**
 * @typedef k_sigcontext_t
 * @brief Typedef for struct k_sigcontext_s.
 */
typedef struct k_sigcontext_s k_sigcontext_t;

#endif /* __i386__ */

#endif /* _ARCH_I386_KTYPES_SIGCONTEXT_H */
