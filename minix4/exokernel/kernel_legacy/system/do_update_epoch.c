#include "kernel/system.h" // Or "kernel/kernel.h" for proc_ptr, kprintf_stub etc.
#include "kernel/proc.h"   // For proc_addr, RS_PROC_NR, _ENDPOINT_P
#include "kernel/cap_table.h"// For global_service_epochs, MAX_SERVICE_EPOCHS
#include <minix/com.h>      // For message structure, RS_PROC_NR, call numbers
#include <minix/syslib.h>   // For _ENDPOINT_P

// Handler for the SYS_UPDATE_SERVICE_EPOCH kernel call.
// Parameters are expected in the message m_in (from the caller, RS).
// m_in.m1_i1: endpoint_t service_ep (actually, this will be a service_id for epoch array)
// m_in.m1_i2: uint32_t new_epoch
// For this Phase 0.25, RS will send service_id directly in m1_i1.
int do_update_service_epoch(struct proc *caller, message *m_ptr) {
    endpoint_t service_ep_param; // Parameter from RS, might be raw endpoint or direct service_id
    uint32_t new_epoch;
    uint32_t service_id_for_epoch;

    // 1. Verify caller is RS_PROC_NR
    if (caller->p_endpoint != RS_PROC_NR) {
        kprintf_stub("KERNEL: SYS_UPDATE_SERVICE_EPOCH denied for non-RS caller EP %d\n",
                     caller->p_endpoint);
        return EPERM;
    }

    // 2. Extract parameters from the incoming message (m_ptr, which is m_in from kernel_call)
    service_ep_param = m_ptr->m1_i1;
    new_epoch = (uint32_t)m_ptr->m1_i2;

    // 3. For Phase 0.25, RS is assumed to send the correct service_id directly.
    // Later phases might require RS to send an endpoint, and kernel resolves it to a service_id.
    service_id_for_epoch = (uint32_t)service_ep_param;

    if (service_id_for_epoch >= MAX_SERVICE_EPOCHS) {
        kprintf_stub("KERNEL: SYS_UPDATE_SERVICE_EPOCH service_id %u out of bounds. Max is %u.\n",
                     service_id_for_epoch, MAX_SERVICE_EPOCHS);
        return EINVAL; // service_id out of bounds for global_service_epochs
    }

    // 4. Atomically update the global_service_epochs array
    //    k_atomic_store_release ensures that prior writes by this CPU are visible
    //    to other CPUs before this store, and this store is visible before subsequent
    //    loads on other CPUs that use k_atomic_load_acquire on the same location.
    //    For single-core, or if epoch checks are always under kernel lock, relaxed might be okay.
    //    Using release semantics for the store is safer for potential SMP scenarios.
    k_atomic_store_release(&global_service_epochs[service_id_for_epoch], new_epoch);

    kprintf_stub("KERNEL: SYS_UPDATE_SERVICE_EPOCH: SID %u epoch updated to %u by RS (EP %d).\n",
                 service_id_for_epoch, new_epoch, caller->p_endpoint);

    return OK;
}
