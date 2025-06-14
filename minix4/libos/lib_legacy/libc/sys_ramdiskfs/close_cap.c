#include <sys/cdefs.h>
#include <lib.h>                // For _syscall, message, endpoint_t
#include <string.h>             // For memset
#include <sys/types.h>          // For uint32_t (though not strictly needed as int suffices for cap_idx)
#include <errno.h>              // For errno
#include <minix/ramdiskfsif.h>  // For RAMDISKFS_CLOSE and message field macros
#include <minix/com.h>          // For RAMDISKFS_PROC_NR (placeholder)

// FIXME TODO: Define RAMDISKFS_PROC_NR in minix/com.h
#ifndef RAMDISKFS_PROC_NR
#define RAMDISKFS_PROC_NR ((endpoint_t) 12) // EXAMPLE PLACEHOLDER ENDPOINT
#warning "Using placeholder RAMDISKFS_PROC_NR in close_cap.c"
#endif

// FIXME TODO: Define libc_free_capability or equivalent exokernel primitive call.
// This function would tell the kernel that the calling process is done with this
// capability index, allowing the slot in its capability table to be reused.
// For Phase 0.5, this is a conceptual call.
// static void libc_free_capability(uint32_t cap_idx) {
//     message m;
//     memset(&m, 0, sizeof(m));
//     m.m1_i1 = cap_idx; // Pass capability index to free
//     // _kernel_call(SYS_DEBUG_FREE_CAP, &m); // Conceptual: SYS_DEBUG_FREE_CAP needs to be defined
//     // For example, if there was a generic capability free syscall:
//     // _syscall(PM_PROC_NR, SYS_CAP_FREE, &m); // Or directly to KERNEL if it's a kernel capability
//     // Note: In a real system, this would likely be an exokernel call, not to PM.
//     // For debugging/logging:
//     // printf("libc_free_capability: Conceptually freed cap_idx %u (syscall not implemented)\n", cap_idx);
// }


// file_handle_cap_idx is the capability index for the open ramdisk file object
int close_cap(uint32_t file_handle_cap_idx) {
    message m;
    int result;

    memset(&m, 0, sizeof(m));
    m.M_RAMDISKFS_CLOSE_CAP_IDX = file_handle_cap_idx;

    // FIXME TODO: The _syscall target should eventually be determined via a capability
    // for the RamdiskFS service.
    result = _syscall(RAMDISKFS_PROC_NR, RAMDISKFS_CLOSE, &m);

    if (result == OK) {
        // Successfully closed on the server side.
        // Now, the client-side capability slot should be freed.
        // FIXME TODO: Implement actual capability freeing mechanism.
        // For Phase 0.5, this is a conceptual step. The capability index (file_handle_cap_idx)
        // should be returned to the process's capability table's free list.
        // This might be done by the LibOS's fd management layer, or an exokernel call.
        // libc_free_capability(file_handle_cap_idx); // Conceptual: Call the function discussed above.
        // This conceptual free would be something like:
        // message free_cap_msg;
        // memset(&free_cap_msg, 0, sizeof(free_cap_msg));
        // free_cap_msg.m1_i1 = file_handle_cap_idx;
        // _syscall(EXOKERNEL_PROC_NR_PLACEHOLDER, EXO_CAP_FREE, &free_cap_msg); // Purely conceptual

        return 0;
    } else {
        // errno is set by _syscall based on RamdiskFS reply m.m_type (if < 0)
        // or if _syscall itself returned an error.
        // If result was positive but not OK (e.g. a custom positive error from server),
        // _syscall should have converted it to a negative error and set errno.
        // So, if result is not OK, it should be -1 and errno set.
        // If _syscall returns the raw m_type and it's a positive error,
        // we might need to map it here, but _syscall is expected to handle this.
        return -1;
    }
}
