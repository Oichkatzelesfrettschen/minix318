#include "kernel/system.h"
#include "kernel/kernel.h" // For kprintf_stub, KASSERT, proc_ptr, get_cpulocal_var
#include "kernel/proc.h"   // For struct proc, proc_addr, isokendpt, rs_isokendpt, _ENDPOINT_P
#include "kernel/cap_table.h" // For cap_table_t, capability_t, cap_alloc, MAX_SERVICE_EPOCHS, k_atomic_load_relaxed, global_service_epochs
#include "kernel/cap_rights.h" // For permission definitions
#include <minix/com.h>      // For message structure, endpoint fields, SELF, OK, E*, etc.
#include <minix/sysutil.h>  // For kprintf_stub (if not in kernel.h)

// Handler for the SYS_CREATE_USER_CAPABILITY kernel call.
// This call is made by a trusted service (e.g., RamdiskFS) to create a capability
// in a client process's capability table.
//
// Expected message m_in from service:
// m_in.m1_i1:  endpoint_t client_ep  (target client process)
// m_in.m1_i2:  uint32_t   permissions (for the new capability)
// m_in.m1_i3:  uintptr_t  object_handle (service-specific handle for the object)
// m_in.m2_i1:  uint32_t   service_id_for_cap (the service's own ID, for epoch tracking)
// m_in.m2_i2:  uint32_t   current_service_epoch (current epoch of the service)
//
// Reply message m_out to service:
// m_out.m_type: OK or error code (will be set by kernel_call_finish based on return value)
// m_out.m1_i1:  uint32_t created_cap_idx (if OK)

int do_create_user_capability(void) {
    // Note: This function uses m_in and m_out as globals, consistent with some kernel call handlers.
    // If system.c passes message *m_ptr, adjust to use m_ptr-> instead of m_in.
    // Given the prototype `int do_create_user_capability(void);` this global usage is implied.

    struct proc *caller_ptr = get_cpulocal_var(proc_ptr); // The service calling this
    struct proc *client_proc_ptr;
    endpoint_t client_ep;
    int client_proc_nr_n; // Use _n suffix for "number" to avoid conflict if proc_nr is a macro
    uint32_t permissions;
    uintptr_t object_handle;
    uint32_t service_id_for_cap;
    uint32_t current_service_epoch;
    capability_t cap_template;
    uint32_t new_cap_idx;
    int r;

    // FIXME TODO: Add privilege check: Only certain trusted system services
    // (like RamdiskFS, VFS, PM) should be allowed to call this.
    // For Phase 0.5, assume caller is trusted if it can make kernel calls.
    // A more robust check might involve checking caller_ptr->p_priv->s_flags & SYS_PROC.
    // if (!(priv(caller_ptr)->s_flags & SYS_PROC)) {
    //     kprintf_stub("KERNEL: SYS_CREATE_USER_CAPABILITY: Denied for non-system process EP %d\n", caller_ptr->p_endpoint);
    //     return EPERM;
    // }


    client_ep = (endpoint_t)m_in.m1_i1;
    permissions = (uint32_t)m_in.m1_i2;
    object_handle = (uintptr_t)m_in.m1_i3; // This is a service-specific value
    service_id_for_cap = (uint32_t)m_in.m2_i1; // This should be the caller's own service ID
    current_service_epoch = (uint32_t)m_in.m2_i2;

    // For security, verify that the service_id_for_cap claimed by the caller matches its actual service_id.
    // The service_id is typically _ENDPOINT_P(caller_ptr->p_endpoint).
    // However, some services might manage multiple logical service IDs.
    // For now, we'll assume a direct match for simplicity in Phase 0.5.
    // A more complex system might have a registration mechanism where a service declares which SIDs it manages.
    if (service_id_for_cap != _ENDPOINT_P(caller_ptr->p_endpoint)) {
         kprintf_stub("KERNEL: SYS_CREATE_USER_CAPABILITY: Caller EP %d (actual SID %u) is trying to create cap for SID %u. Denied.\n",
                      caller_ptr->p_endpoint, _ENDPOINT_P(caller_ptr->p_endpoint), service_id_for_cap);
         return EPERM;
    }


    kprintf_stub("KERNEL: SYS_CREATE_USER_CAPABILITY: Caller EP %d (SID %u) requests cap for client EP %d. Epoch %u, Perms 0x%x, ObjHandle 0x%lx\n",
                 caller_ptr->p_endpoint, service_id_for_cap, client_ep, current_service_epoch, permissions, (unsigned long)object_handle);

    if (rs_isokendpt(client_ep, &client_proc_nr_n) != OK) {
        kprintf_stub("KERNEL: SYS_CREATE_USER_CAPABILITY: Invalid client_ep %d\n", client_ep);
        return EINVAL;
    }
    client_proc_ptr = proc_addr(client_proc_nr_n);
    if (!client_proc_ptr || (client_proc_ptr->p_rts_flags & RTS_SLOT_FREE)) {
        kprintf_stub("KERNEL: SYS_CREATE_USER_CAPABILITY: Client process for EP %d not found or slot free.\n", client_ep);
        return ESRCH;
    }

    if (service_id_for_cap >= MAX_SERVICE_EPOCHS) {
        kprintf_stub("KERNEL: SYS_CREATE_USER_CAPABILITY: service_id_for_cap %u out of bounds (max %u).\n",
                     service_id_for_cap, MAX_SERVICE_EPOCHS);
        return EINVAL;
    }

    uint32_t kernel_view_of_service_epoch = k_atomic_load_relaxed(&global_service_epochs[service_id_for_cap]);
    if (kernel_view_of_service_epoch == 0) {
         kprintf_stub("KERNEL: SYS_CREATE_USER_CAPABILITY: Service ID %u has no registered epoch (is 0). RS must initialize its epoch with kernel first.\n", service_id_for_cap);
         return EFAULT;
    }
    if (current_service_epoch != kernel_view_of_service_epoch) {
        kprintf_stub("KERNEL: SYS_CREATE_USER_CAPABILITY: Caller EP %d (SID %u) provided epoch %u, but kernel global epoch is %u. Stale service? Capability creation denied.\n",
                     caller_ptr->p_endpoint, service_id_for_cap, current_service_epoch, kernel_view_of_service_epoch);
        return EPERM;
    }

    // Prepare the capability template
    cap_template.service_id = service_id_for_cap;
    cap_template.epoch = current_service_epoch;
    cap_template.permissions = permissions;
    cap_template.endpoint_or_obj = object_handle;

    r = cap_alloc(&client_proc_ptr->p_cap_table, &cap_template, &new_cap_idx);
    if (r != OK) {
        kprintf_stub("KERNEL: SYS_CREATE_USER_CAPABILITY: cap_alloc for client EP %d failed: %d\n", client_ep, r);
        // r is likely -ENOMEM if cap table is full, or other errors.
        // cap_alloc should return negative errno on failure.
        return (r < 0 ? r : EGENERIC); // Ensure it's a negative error code
    }

    kprintf_stub("KERNEL: SYS_CREATE_USER_CAPABILITY: Granted cap_idx %u to client EP %d for SID %u (obj 0x%lx, perms 0x%x)\n",
                 new_cap_idx, client_ep, service_id_for_cap, (unsigned long)object_handle, permissions);

    // Return the new capability index to the calling service via m_out
    // The caller (RamdiskFS) expects the new capability index in m_out.m1_i1
    m_out.m1_i1 = new_cap_idx;
    return OK;
}
