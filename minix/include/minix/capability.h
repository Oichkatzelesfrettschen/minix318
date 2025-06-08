#ifndef _MINIX_CAPABILITY_H
#define _MINIX_CAPABILITY_H

#include <minix/types.h> // For endpoint_t, u32_t, u8_t

// Forward declare struct proc if needed by prototypes, or include "kernel/proc.h" in .c files
struct proc;

#define MAX_CAPABILITIES_PER_PROC 16 // Example size for basic table
#define PM_CAP_IDX 0                 // Well-known index for PM capability

typedef struct {
    endpoint_t target;
    u32_t rights;
    u8_t in_use; // 0 = free, 1 = in use
    // u32_t epoch; // For future epoch checking (as per RS integration hint)
} capability_t;

#define OP_SEND 0x0001 // Example right

// Prototype for cap_lookup - will be implemented in capability.c
capability_t* cap_lookup(struct proc *caller_ptr, int cap_idx);

// Prototype for capability table initialization for a process
void init_proc_capabilities(struct proc *pr);

#endif /* _MINIX_CAPABILITY_H */
