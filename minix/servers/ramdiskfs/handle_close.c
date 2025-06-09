#include "ramdiskfs.h"

// Conceptual Exokernel/Kernel call prototypes (placeholders)
// Ensure this matches the one in handle_readwrite.c or a common internal header
static void exo_free_mem_pages_placeholder(void* ptr, size_t bytes) {
    // FIXME TODO: This is a placeholder. Real implementation uses Exokernel call.
    // For Phase 0.5 testing, if using malloc, this should be free.
    printf("RamdiskFS: Placeholder exo_free_mem_pages_placeholder for %p (%zu bytes)\n", ptr, bytes);
    if (ptr) free(ptr); // Placeholder for malloc based allocation
}

int handle_close(void) {
    uint32_t cap_idx = m_in.M_RAMDISKFS_CLOSE_CAP_IDX;

    printf("RamdiskFS: handle_close for cap_idx %u\n", cap_idx);

    // Validate the capability index (which is used as a direct index into open_files for Phase 0.5)
    if (cap_idx >= MAX_RAMDISK_FILES) {
        printf("RamdiskFS: handle_close: cap_idx %u out of bounds (max %d)\n",
               cap_idx, MAX_RAMDISK_FILES);
        return EBADF;
    }
    if (!open_files[cap_idx].in_use) {
        printf("RamdiskFS: handle_close: cap_idx %u already closed or not in use.\n", cap_idx);
        return EBADF;
    }

    ramdisk_file_t *rf = &open_files[cap_idx];

    // FIXME TODO: Add reference counting for file objects if multiple opens can point to the same ramdisk_file_t.
    // If ref_count > 1, just decrement ref_count and don't free data yet.
    // For Phase 0.5, assume close always means full release of this specific "open instance".
    // If the "object_handle" stored in the capability was truly just the slot index,
    // then this simple model is okay for now, as each open gets a new capability.

    if (rf->data_ptr) {
        printf("RamdiskFS: Closing file '%s' (cap_idx %u was associated with slot %u), freeing %zu bytes of data at %p.\n",
               rf->name, cap_idx, cap_idx, /* Assumes cap_idx directly maps to slot_idx for Phase 0.5 */
               rf->allocated_size, rf->data_ptr);
        exo_free_mem_pages_placeholder(rf->data_ptr, rf->allocated_size);
        rf->data_ptr = NULL;
    }

    rf->in_use = 0;
    rf->name[0] = '\0'; // Clear name
    rf->current_size = 0;
    rf->allocated_size = 0;

    // The client-side (Libc/LibOS) is responsible for instructing the kernel
    // to free the actual capability slot (represented by 'cap_idx' it received from open)
    // in its capability table. RamdiskFS just marks its internal resources associated
    // with this open instance (identified by the object_handle in the capability, which
    // for Phase 0.5 is the slot index itself) as free.

    printf("RamdiskFS: File resources for slot %u (associated with closed cap_idx %u) released.\n", cap_idx, cap_idx);
    return OK;
}
