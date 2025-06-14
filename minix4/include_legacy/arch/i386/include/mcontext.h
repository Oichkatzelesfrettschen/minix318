#ifndef _MACHINE_MCONTEXT_H
#define _MACHINE_MCONTEXT_H

#include <minix/types.h>
#include <arch/i386/include/fpu.h>

/* Flag indicating that FPU state has been saved. */
#define _MC_FPU_SAVED 0x01

/* Minimal machine context definition used by the kernel. */
typedef struct mcontext {
    u32_t mc_flags; /* context flags */
    struct {
        struct {
            u8_t __fp_reg_set[FPU_XFP_SIZE]; /* raw FPU state */
        } __fpregs;
    };
} mcontext_t;

#endif /* _MACHINE_MCONTEXT_H */
