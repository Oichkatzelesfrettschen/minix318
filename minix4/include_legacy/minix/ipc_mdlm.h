#ifndef _MINIX_IPC_MDLM_H
#define _MINIX_IPC_MDLM_H

#include <minix/config.h> // For CONFIG_MDLM
#include <minix/types.h>
#include <minix/capability.h> // For capability_t

// Placeholder for MDLM handle type
typedef void* mdlm_handle_t;

// Per-process IPC state relevant to MDLM and capabilities
// This would typically be part of 'struct proc' or a parallel structure.
// For this subtask, we define it; integration into struct proc is a next step.
typedef struct {
#ifdef CONFIG_MDLM
    mdlm_handle_t mdlm_context;
#endif
    capability_t *cap_table; // Pointer to this process's capability table
    int cap_table_size;
    // ... other fields
} ipc_state_t;

// MDLM access check function (stub) - to be implemented in an MDLM .c file
#ifdef CONFIG_MDLM
int mdlm_access_check(endpoint_t P_ID_CALLER_PROCESS, endpoint_t P_ID_TARGET_PROCESS, u32_t OP_TYPE);
#endif

#endif /* _MINIX_IPC_MDLM_H */
