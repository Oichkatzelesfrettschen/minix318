#include <sys/cdefs.h>
#include <lib.h>                // For _syscall, message, endpoint_t, SELF
#include <string.h>             // For memset
#include <sys/types.h>          // For size_t, ssize_t, off_t
#include <errno.h>              // For errno, EFAULT
#include <minix/u64.h>          // For make64, ex64lo, ex64hi
#include <minix/ramdiskfsif.h>  // For RAMDISKFS_READ and message field macros
#include <minix/com.h>          // For RAMDISKFS_PROC_NR (placeholder)

// FIXME TODO: Define RAMDISKFS_PROC_NR in minix/com.h
#ifndef RAMDISKFS_PROC_NR
#define RAMDISKFS_PROC_NR ((endpoint_t) 12) // EXAMPLE PLACEHOLDER ENDPOINT
#warning "Using placeholder RAMDISKFS_PROC_NR in read_cap.c"
#endif

// file_handle_cap_idx is the capability index for the open ramdisk file object
ssize_t read_cap(uint32_t file_handle_cap_idx, void *buffer, size_t nbytes, off_t offset) {
    message m;
    int result;

    if (nbytes == 0) {
        return 0;
    }
    if (buffer == NULL) {
        errno = EFAULT;
        return -1;
    }

    memset(&m, 0, sizeof(m));
    m.M_RAMDISKFS_READ_CAP_IDX = file_handle_cap_idx;
    m.M_RAMDISKFS_READ_NBYTES = nbytes;
    m.M_RAMDISKFS_READ_BUF_ADDR = (vir_bytes)buffer; // Server will create grant for this
    m.M_RAMDISKFS_READ_REQ_ENDPT = SELF;           // Server needs our EP to make grant
    m.M_RAMDISKFS_READ_OFFSET_LO = ex64lo(offset);
    m.M_RAMDISKFS_READ_OFFSET_HI = ex64hi(offset);

    // FIXME TODO: The _syscall target should eventually be determined via a capability
    // for the RamdiskFS service. For Phase 0.5, we use a known endpoint.
    // The kernel's IPC path for RAMDISKFS_PROC_NR and RAMDISKFS_READ would then
    // check the caller's capability for this service.
    result = _syscall(RAMDISKFS_PROC_NR, RAMDISKFS_READ, &m);

    // If result < 0, _syscall already set errno.
    // If result >= 0, it's the number of bytes read (as per ramdiskfsif.h for RAMDISKFS_READ replies).
    return (ssize_t)result;
}
