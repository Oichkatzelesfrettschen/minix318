#include <sys/cdefs.h>
#include <lib.h>                // For _syscall, message, endpoint_t, SELF
#include <string.h>             // For memset
#include <sys/types.h>          // For size_t, ssize_t, off_t
#include <errno.h>              // For errno, EFAULT
#include <minix/u64.h>          // For make64, ex64lo, ex64hi
#include <minix/ramdiskfsif.h>  // For RAMDISKFS_WRITE and message field macros
#include <minix/com.h>          // For RAMDISKFS_PROC_NR (placeholder)

// FIXME TODO: Define RAMDISKFS_PROC_NR in minix/com.h
#ifndef RAMDISKFS_PROC_NR
#define RAMDISKFS_PROC_NR ((endpoint_t) 12) // EXAMPLE PLACEHOLDER ENDPOINT
#warning "Using placeholder RAMDISKFS_PROC_NR in write_cap.c"
#endif

// file_handle_cap_idx is the capability index for the open ramdisk file object
ssize_t write_cap(uint32_t file_handle_cap_idx, const void *buffer, size_t nbytes, off_t offset) {
    message m;
    int result;

    if (buffer == NULL && nbytes > 0) { // Writing 0 bytes with NULL buffer is okay for some truncate-like ops
        errno = EFAULT;
        return -1;
    }
    // Writing 0 bytes is a no-op, return 0.
    if (nbytes == 0) {
        return 0;
    }

    memset(&m, 0, sizeof(m));
    m.M_RAMDISKFS_WRITE_CAP_IDX = file_handle_cap_idx;
    m.M_RAMDISKFS_WRITE_NBYTES = nbytes;
    m.M_RAMDISKFS_WRITE_BUF_ADDR = (vir_bytes)buffer; // Server will create grant for this
    m.M_RAMDISKFS_WRITE_REQ_ENDPT = SELF;           // Server needs our EP to make grant
    m.M_RAMDISKFS_WRITE_OFFSET_LO = ex64lo(offset);
    m.M_RAMDISKFS_WRITE_OFFSET_HI = ex64hi(offset);

    // FIXME TODO: The _syscall target should eventually be determined via a capability
    // for the RamdiskFS service.
    result = _syscall(RAMDISKFS_PROC_NR, RAMDISKFS_WRITE, &m);

    // If result < 0, _syscall already set errno.
    // If result >= 0, it's the number of bytes written (as per ramdiskfsif.h for RAMDISKFS_WRITE replies).
    return (ssize_t)result;
}
