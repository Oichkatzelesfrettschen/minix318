#include "kernel/kernel.h" // Includes proc.h (for struct proc definition, PM_PROC_NR, INIT_PROC_NR, NONE)
                           // and kprintf.h (for kprintf_stub)
#include <minix/capability.h> // For capability_t, MAX_CAPABILITIES_PER_PROC, PM_CAP_IDX, OP_SEND
#include <minix/com.h>        // For PM_PROC_NR, INIT_PROC_NR, NONE if not in kernel.h/proc.h indirectly

// This function initializes the capability table for a given process.
// It will be called when a process is created/initialized.
void init_proc_capabilities(struct proc *pr) {
    if (!pr) return;

    // struct proc should have: capability_t p_capabilities[MAX_CAPABILITIES_PER_PROC];
    // This field needs to be added to struct proc via proc.h modification.

    for (int i = 0; i < MAX_CAPABILITIES_PER_PROC; i++) {
        pr->p_capabilities[i].in_use = 0;
        pr->p_capabilities[i].target = NONE; // Assuming NONE is a valid endpoint_t for uninitialized
        pr->p_capabilities[i].rights = 0;
        // pr->p_capabilities[i].epoch = 0; // For future
    }

    // For the getpid() PoC, grant PM capability to INIT_PROC_NR by default.
    // In a more developed system, capabilities might be inherited or granted by a security server.
    // Granting to all processes for simplicity of PoC if INIT_PROC_NR is too restrictive early on.
    // The spec says INIT_PROC_NR, so let's stick to that.
    // Other processes would get capabilities via inheritance or explicit grant (future).
    if (pr->p_endpoint == INIT_PROC_NR) {
        if (PM_CAP_IDX < MAX_CAPABILITIES_PER_PROC) { // Bounds check
            pr->p_capabilities[PM_CAP_IDX].target = PM_PROC_NR;
            pr->p_capabilities[PM_CAP_IDX].rights = OP_SEND; // Basic send right
            pr->p_capabilities[PM_CAP_IDX].in_use = 1;
        } else {
            // This would be a configuration error (PM_CAP_IDX too large)
            kprintf_stub("init_proc_capabilities: PM_CAP_IDX %d is out of bounds for process %d\n",
                         PM_CAP_IDX, pr->p_endpoint);
        }
    }
    // For other processes, the table is initially empty (all in_use = 0).
}

// Lookup a capability in the caller's capability table.
capability_t* cap_lookup(struct proc *caller_ptr, int cap_idx) {
    if (!caller_ptr) {
        // kprintf_stub("cap_lookup: called with NULL caller_ptr\n");
        return NULL;
    }

    // Assumption: caller_ptr->p_capabilities exists and is initialized.
    if (cap_idx >= 0 && cap_idx < MAX_CAPABILITIES_PER_PROC) {
        if (caller_ptr->p_capabilities[cap_idx].in_use) {
            return &caller_ptr->p_capabilities[cap_idx];
        } else {
            // kprintf_stub("cap_lookup: cap_idx %d not in use for proc %d\n", cap_idx, caller_ptr->p_endpoint);
        }
    } else {
        // kprintf_stub("cap_lookup: invalid cap_idx %d (out of bounds) for proc %d\n", cap_idx, caller_ptr->p_endpoint);
    }
    return NULL; // Return NULL if not found, not in use, or index out of bounds
}
