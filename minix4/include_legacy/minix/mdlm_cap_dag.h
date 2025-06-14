#ifndef _MINIX_MDLM_CAP_DAG_H
#define _MINIX_MDLM_CAP_DAG_H

#include <minix/types.h>
#include <minix/capability.h> // For capability_t
#include <minix/sys_config.h> // For CONFIG_MDLM

// Forward declare struct proc to avoid direct include of proc.h in this header
struct proc;

// For a basic DAG, we might augment capability_t or have a parallel structure.
// Let's assume for now that capability_t itself might store a 'parent_cap_idx'
// or we have a separate table linking capabilities.
// For simplicity in this first step, we'll focus on the check logic
// and assume capability relationships can be determined.

// A more concrete structure might be:
// struct mdlm_capability_node {
//     int capability_idx; // Links to the actual capability_t
//     int parent_idx;     // Index of parent mdlm_capability_node
//     // endpoint_t owner_proc; // Process that owns/created this capability node
// };
// For now, this will be conceptual; the focus is mdlm_access_check logic.

#ifdef CONFIG_MDLM
// Function to initialize the capability DAG component
void mdlm_cap_dag_init(void);

// The actual access check function, replacing the stub
// Takes caller's process, the capability being used, and the operation.
// For this basic version, we might simplify what 'op_type' means.
int mdlm_cap_dag_access_check(struct proc *caller_ptr, capability_t *cap, u32_t op_type);
#else
// Define stubs if CONFIG_MDLM is not set, to prevent linker errors if called from non-ifdef'd code
static inline void mdlm_cap_dag_init(void) {}
static inline int mdlm_cap_dag_access_check(struct proc *caller_ptr, capability_t *cap, u32_t op_type) {
    (void)caller_ptr; (void)cap; (void)op_type;
    return 1; // Default to allow if MDLM is not compiled in (or deny if preferred)
}
#endif /* CONFIG_MDLM */

#endif /* _MINIX_MDLM_CAP_DAG_H */
