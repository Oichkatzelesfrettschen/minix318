#include <sys/cdefs.h>
#include <lib.h>                // For _syscall, message, endpoint_t, vir_bytes, SELF
#include <string.h>             // For strlen, memset
#include <fcntl.h>              // For O_CREAT, O_RDWR etc.
#include <sys/types.h>          // For mode_t
#include <errno.h>              // For errno, EFAULT
#include <minix/ramdiskfsif.h>  // For RAMDISKFS_OPEN and message field macros
#include <minix/com.h>          // For RAMDISKFS_PROC_NR (placeholder)

// FIXME TODO: Define RAMDISKFS_PROC_NR in minix/com.h
#ifndef RAMDISKFS_PROC_NR
#define RAMDISKFS_PROC_NR ((endpoint_t) 12) // EXAMPLE PLACEHOLDER ENDPOINT
#warning "Using placeholder RAMDISKFS_PROC_NR in open_cap.c"
#endif

// Returns a handle (capability index for the file object) on success, or -1 on error.
int open_cap(const char *path, int flags, mode_t mode) {
    message m;
    int result;

    if (path == NULL) {
        errno = EFAULT;
        return -1;
    }

    memset(&m, 0, sizeof(m));

    m.M_RAMDISKFS_OPEN_PATH_ADDR = (vir_bytes)path; // Pass user-space path pointer
    m.M_RAMDISKFS_OPEN_PATH_LEN = strlen(path) + 1;
    m.M_RAMDISKFS_OPEN_FLAGS = flags;
    m.M_RAMDISKFS_OPEN_REQ_ENDPT = SELF; // Server needs our EP to make grant for path
    if (flags & O_CREAT) {
        m.M_RAMDISKFS_OPEN_MODE = mode;
    }

    // FIXME TODO: The _syscall target should eventually be determined via a capability
    // for the RamdiskFS service. For Phase 0.5, we use a known endpoint.
    result = _syscall(RAMDISKFS_PROC_NR, RAMDISKFS_OPEN, &m);

    if (result < 0) {
        // errno is set by _syscall
        return -1;
    }

    // On success, RamdiskFS should reply with m.m_type = OK (or a specific success code like RAMDISKFS_OPEN_REPLY),
    // and the new file object capability index in M_RAMDISKFS_OPEN_REPLY_CAP_IDX.
    // _syscall returns m.m_type on success.
    // The previous version of open_cap.c expected RAMDISKFS_OPEN_REPLY.
    // The updated ramdiskfsif.h notes this expectation.
    // Let's assume the server is expected to reply with RAMDISKFS_OPEN as m_type for success,
    // or OK (0) if it's a very simple success case.
    // The key is that M_RAMDISKFS_OPEN_REPLY_CAP_IDX must be populated.
    if (m.m_type == OK || m.m_type == RAMDISKFS_OPEN) {
        // The "file descriptor" returned by open_cap will be this capability index.
        return m.M_RAMDISKFS_OPEN_REPLY_CAP_IDX;
    } else {
        // If _syscall didn't return <0, but m_type is an error from server or unexpected positive reply type.
        errno = EIO; // Generic I/O error for unexpected positive reply type
        return -1;
    }
}
