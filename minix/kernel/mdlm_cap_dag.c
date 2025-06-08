#include "kernel/kernel.h" // For struct proc, kprintf_stub (via klib/kprintf.h), panic
#include <minix/mdlm_cap_dag.h>
#include <minix/sys_config.h> // For CONFIG_MDLM
#include <minix/capability.h> // For capability_t, OP_SEND (rights definitions)

#ifdef CONFIG_MDLM

void mdlm_cap_dag_init(void) {
    // Initialize any global state for the capability DAG if needed.
    // For this basic parent-child check or rights check, no complex global state
    // is initialized here. This function ensures the component can be initialized.
    // kprintf_stub("MDLM Cap DAG: Initialized (CONFIG_MDLM enabled).\n");
}

// Basic capability access check.
// For this initial MDLM component, the "DAG" aspect is simplified to:
// 1. The capability itself must grant the requested right (op_type).
// 2. Possession of the capability by the caller is implied by cap_lookup succeeding.
// Future enhancements would involve checking derivation paths, delegation rights, etc.
int mdlm_cap_dag_access_check(struct proc *caller_ptr, capability_t *cap, u32_t op_type) {
    if (!caller_ptr || !cap) {
        // This should ideally not be reached if cap_lookup and caller_ptr are valid before calling.
        kprintf_stub("MDLM Cap DAG: ERROR - Null caller_ptr or cap provided to access check.\n");
        return 0; // Deny on bad parameters
    }

    // Check if the capability's rights field includes the requested operation type.
    // Example: if op_type is OP_SEND, check if cap->rights has OP_SEND bit set.
    if ((cap->rights & op_type) == op_type) {
        // Basic rights check passed.
        // kprintf_stub("MDLM Cap DAG: Access OK for proc %d to target %d (cap rights %x, op %x)\n",
        //             caller_ptr->p_endpoint, cap->target, cap->rights, op_type);
        return 1; // Allow
    }

    // kprintf_stub("MDLM Cap DAG: Access DENIED for proc %d to target %d (cap rights %x, op %x) - rights insufficient.\n",
    //              caller_ptr->p_endpoint, cap->target, cap->rights, op_type);
    return 0; // Deny by default if rights do not match
}

#else /* CONFIG_MDLM not defined */

/*
 * Provide non-static stubs for these functions if CONFIG_MDLM is not defined,
 * but they are still referenced by code that isn't fully conditionalized
 * (e.g., if an #ifdef block was missed around a call site).
 * The header file provides static inline stubs which is usually sufficient
 * to optimize out calls. If direct calls from other .c files that don't see
 * the static inline versions are possible, then these non-static stubs ensure linking.
 * However, for this project, header static inlines are preferred.
 * This block can be empty if all call sites are properly conditionalized
 * or use the static inline stubs from the header.
 */

// void mdlm_cap_dag_init(void) { /* No-op */ }
// int mdlm_cap_dag_access_check(struct proc *caller_ptr, capability_t *cap, u32_t op_type) {
//    (void)caller_ptr; (void)cap; (void)op_type;
//    return 1; /* Default allow if MDLM is fully disabled and somehow called */
// }

#endif /* CONFIG_MDLM */
