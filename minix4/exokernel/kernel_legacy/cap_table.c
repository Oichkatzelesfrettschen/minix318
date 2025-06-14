#include "kernel.h"         // For KASSERT, spinlock_init/lock/unlock, irq_context_t etc.
#include "cap_table.h"      // Our header
#include <klib/include/kmemory.h> // For k_memset

// Definition of the global array for service epochs.
// Initialized to 0 by BSS. RS will update this via a secure mechanism.
k_atomic_uint global_service_epochs[MAX_SERVICE_EPOCHS];

void cap_table_init(cap_table_t *ct, capability_t *cap_buffer, uint32_t *free_list_buffer, uint32_t size) {
    KASSERT(ct != NULL);
    KASSERT(cap_buffer != NULL);
    KASSERT(free_list_buffer != NULL);
    KASSERT(size > 0 && (size & (size - 1)) == 0); // Must be power of 2 for mask logic if used

    ct->caps = cap_buffer;
    ct->free_list = free_list_buffer;
    ct->size = size;
    ct->mask = size - 1; // Useful if indexing directly, e.g. cap_idx & ct->mask
                         // but with free_list, direct indexing isn't the primary lookup path.

    // Initialize free list: each entry free_list[i] stores the index of a free capability slot.
    // All capability slots are initially free.
    for (uint32_t i = 0; i < size; i++) {
        ct->free_list[i] = i; // The i-th entry in free_list stores cap_idx 'i'
    }
    // free_head indicates the number of available free slots in the free_list.
    // It points to the next position in free_list where a freed cap_idx would be stored.
    // Or, equivalently, free_list[free_head-1] is the next cap_idx to allocate.
    k_atomic_store_relaxed(&ct->free_head, size);

    spin_lock_init(&ct->lock);

    // Initialize capability memory to a known state (e.g., all permissions zero).
    k_memset(cap_buffer, 0, sizeof(capability_t) * size);
}

// Allocates a capability slot and copies the template content.
// Returns 0 on success, -ENOMEM if no free slots.
// The cap_idx_out will contain the index of the allocated capability.
int cap_alloc(cap_table_t *ct, const capability_t *cap_template, uint32_t *cap_idx_out) {
    int ret = -ENOMEM;
    irq_context_t irq_ctx;

    spin_lock_irqsave(&ct->lock, &irq_ctx);

    uint32_t current_free_head = k_atomic_load_relaxed(&ct->free_head);
    if (current_free_head > 0) {
        current_free_head--; // Consume a free slot index from the top of the stack
        uint32_t new_cap_idx = ct->free_list[current_free_head]; // Get the actual capability index

        // Copy template to the allocated capability slot
        ct->caps[new_cap_idx].service_id = cap_template->service_id;
        // CRITICAL: The epoch in cap_template MUST be the CURRENT global epoch for that service_id.
        // This should be set by the capability granting code, not assumed from template blindly unless intended.
        // For Phase 0.25, the caller of cap_alloc is responsible for setting cap_template.epoch correctly.
        ct->caps[new_cap_idx].epoch = cap_template->epoch;
        ct->caps[new_cap_idx].permissions = cap_template->permissions;
        ct->caps[new_cap_idx].endpoint_or_obj = cap_template->endpoint_or_obj;
        // Padding is implicitly part of the struct, no need to copy.

        *cap_idx_out = new_cap_idx;
        k_atomic_store_relaxed(&ct->free_head, current_free_head);
        ret = 0; // OK
    }

    spin_unlock_irqrestore(&ct->lock, irq_ctx);
    return ret;
}

// Frees a capability slot.
// Returns 0 on success, -EINVAL if cap_idx is invalid.
int cap_free(cap_table_t *ct, uint32_t cap_idx) {
    irq_context_t irq_ctx;
    spin_lock_irqsave(&ct->lock, &irq_ctx);

    if (cap_idx >= ct->size) { // Basic bounds check
        spin_unlock_irqrestore(&ct->lock, irq_ctx);
        return -EINVAL;
    }

    // TODO: Add a check to prevent double-freeing if an "active" flag is added to capability_t.
    // For now, assume caller ensures cap_idx is valid and allocated.

    uint32_t current_free_head = k_atomic_load_relaxed(&ct->free_head);
    if (current_free_head < ct->size) { // Ensure free_list is not full
        ct->free_list[current_free_head] = cap_idx; // Add cap_idx back to the free list stack
        current_free_head++;
        k_atomic_store_relaxed(&ct->free_head, current_free_head);

        // Invalidate the freed capability slot for security
        ct->caps[cap_idx].permissions = 0;
        ct->caps[cap_idx].endpoint_or_obj = 0; // Or a defined invalid/NULL handle
        ct->caps[cap_idx].service_id = (uint32_t)-1; // Mark as invalid service
        ct->caps[cap_idx].epoch = 0; // Zero out epoch

        spin_unlock_irqrestore(&ct->lock, irq_ctx);
        return 0; // OK
    }

    // Should not happen if freeing a valid cap unless free_list logic is flawed or table was over-filled.
    spin_unlock_irqrestore(&ct->lock, irq_ctx);
    return -EFAULT; // Internal error / list full
}

// Unsafe lookup: returns pointer to capability_t or NULL.
// Does NOT lock. Does NOT check if slot is currently "active" from free_list perspective.
// Intended for use where index is known to be valid (e.g., after alloc, before free)
// or in performance-critical paths after initial validation.
// The crucial `is_capability_valid_for_ipc` (which includes epoch check) should be called by IPC logic.
capability_t* cap_lookup(cap_table_t *ct, uint32_t cap_idx) {
    if (cap_idx >= ct->size) {
        return NULL;
    }
    return &ct->caps[cap_idx];
}
