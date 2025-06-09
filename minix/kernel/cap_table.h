#ifndef _MINIX_KERNEL_CAP_TABLE_H
#define _MINIX_KERNEL_CAP_TABLE_H

#include <minix/config.h>       // For CONFIG_MAX_CPUS, NR_PROCS etc. if needed
#include <minix/kernel_types.h> // For k_atomic_uint, spinlock_t, fixed-width types
#include <machine/archtypes.h>  // For CACHE_LINE_SIZE (platform specific)

// Define CACHE_LINE_SIZE if not provided by archtypes.h (common default)
#ifndef CACHE_LINE_SIZE
#define CACHE_LINE_SIZE 64
#endif

// Max number of services whose epochs are globally tracked.
// Should be large enough to cover all system services + potentially some dynamic ones.
#ifndef MAX_SERVICE_EPOCHS
#define MAX_SERVICE_EPOCHS 64
#endif

// Global array of current epochs for each service ID.
// Indexed by a service_id (which needs to be mapped from endpoint_t or a new ID scheme).
// RS updates this via a secure mechanism (e.g., dedicated syscall).
extern k_atomic_uint global_service_epochs[MAX_SERVICE_EPOCHS];

// Capability Structure
// Ensure total size is CACHE_LINE_SIZE to prevent false sharing when in an array.
#define CAP_STRUCT_SIZE_CHECK (sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uintptr_t))
#if CAP_STRUCT_SIZE_CHECK > CACHE_LINE_SIZE
#error "Core capability structure fields exceed cache line size even before padding!"
#endif
#define CAP_RESERVED_PADDING (CACHE_LINE_SIZE - CAP_STRUCT_SIZE_CHECK)

typedef struct capability {
    uint32_t  service_id;      // Identifier for the target service/object type for epoch tracking.
    uint32_t  epoch;           // Epoch number of the service when this capability was granted.
    uint32_t  permissions;     // Bitmask of allowed operations on the object.
    uintptr_t endpoint_or_obj; // Actual IPC endpoint_t, or pointer/handle to a kernel object.
    // Padding to fill a cache line.
    char      padding[CAP_RESERVED_PADDING];
} capability_t __attribute__((aligned(CACHE_LINE_SIZE)));

// Per-process/per-LibOS Capability Table
typedef struct cap_table {
    capability_t* caps;       // Pointer to an array of capability_t slots.
    uint32_t*     free_list;  // Array acting as a stack of free indices into 'caps'.
    uint32_t      size;       // Total number of capability slots (must be power of 2).
    uint32_t      mask;       // 'size - 1' for fast modulo indexing (if idx is not from free_list).
    k_atomic_uint free_head;  // Index to the top of the free_list stack (number of free slots).
    spinlock_t    lock;       // Spinlock for concurrent access to free_list & free_head.
} cap_table_t;

// Function prototypes for capability table management (to be in cap_table.c)
void cap_table_init(cap_table_t *ct, capability_t *cap_buffer, uint32_t *free_list_buffer, uint32_t size);
int cap_alloc(cap_table_t *ct, const capability_t *cap_template, uint32_t *cap_idx_out);
int cap_free(cap_table_t *ct, uint32_t cap_idx);
capability_t* cap_lookup(cap_table_t *ct, uint32_t cap_idx); // Unsafe lookup, assumes valid index.

// Inline function for epoch check (to be used in IPC path by kernel)
// Returns 1 if valid, 0 if not.
static inline int is_capability_valid_for_ipc(const cap_table_t *ct, uint32_t cap_idx) {
    // Basic bounds check using mask (if cap_idx is not from trusted source like free_list during alloc)
    // However, cap_lookup should ideally be used which does a size check.
    // For IPC, cap_idx comes from user, so needs rigorous check.
    if (cap_idx >= ct->size) return 0;

    const capability_t *cap = &ct->caps[cap_idx]; // Direct access after bounds check

    // Check if service_id is valid before indexing global_service_epochs
    if (cap->service_id >= MAX_SERVICE_EPOCHS) return 0;

    // Atomically load the current epoch for the service.
    // k_atomic_load_relaxed might be appropriate if within kernel critical section
    // or if global_service_epochs updates are guaranteed to be visible.
    uint32_t current_service_epoch = k_atomic_load_relaxed(&global_service_epochs[cap->service_id]);

    return cap->epoch == current_service_epoch;
}

// Example permission flags (to be expanded based on actual resources/operations)
#define CAP_PERM_IPC_SEND    (1 << 0) // Permission to send an IPC message via this cap
#define CAP_PERM_IPC_RECEIVE (1 << 1) // Permission to receive (less common for send caps)
// ... other permissions e.g., for memory, device access ...

#endif // _MINIX_KERNEL_CAP_TABLE_H
