// minix/kernel/capability/math_syscalls_extended.c
#include "../system.h" // For proc, message, endpoint_t, etc. (Path relative to kernel dir)
#include "capability_syscalls.h" // For capability_proof_t, kcapability_node_t, etc.
#include "../../lib/klib/include/kcapability_posix.h" // For CAP_WRITE, etc.

// Define necessary constants if not available through includes (e.g., from fcntl.h or errno.h equivalents)
#ifndef EBADF
#define EBADF 9
#endif
#ifndef EACCES
#define EACCES 13
#endif
#ifndef ENOSYS
#define ENOSYS 38 // Or appropriate Minix value
#endif

// Prototypes for functions assumed to exist or be stubbed elsewhere (e.g. VFS or other kernel modules)
// These stubs are more detailed than simple ENOSYS for functions used by the implemented handlers.
// Stubs for VFS interaction functions (these would call actual VFS functions)
static int do_write_with_capability(struct proc *caller, message *m_ptr, kcapability_node_t *cap) {
    (void)caller; (void)m_ptr; (void)cap;
    kprintf("STUB: do_write_with_capability called for FD %d\n", m_ptr->m_lc_vfs_fd);
    // In a real system, this would return the result of the write operation.
    return m_ptr->m_lc_vfs_len; // Simulate successful write of all bytes
}

static int do_close(struct proc *caller, message *m_ptr) {
    (void)caller; (void)m_ptr;
    kprintf("STUB: do_close called for FD %d\n", m_ptr->m_lc_vfs_fd);
    return 0; // Success
}

// Stubs for capability management functions (these would interact with capability data structures)
static void revoke_capability(kcapability_node_t *cap) {
    (void)cap;
    // kprintf("STUB: revoke_capability called\n");
    // Actual logic to invalidate or remove the capability
}

static void remove_fd_capability(struct proc *caller, int fd) {
    (void)caller; (void)fd;
    // kprintf("STUB: remove_fd_capability called for FD %d by PID %d\n", fd, caller->p_pid);
    // Actual logic to remove the fd to capability mapping for the process
}

// extern kcapability_node_t* get_fd_capability(struct proc* caller, int fd); // Declared in math_syscalls.c stubs
// extern uint64_t get_kernel_timestamp(void); // Assumed to be globally available or from khelpers
// extern bool verify_mathematical_proof(const capability_proof_t* proof); // From capability_verify.c
// extern int do_math_open(struct proc* caller, message* m_ptr); // From math_syscalls.c

// Implementations from user feedback:
int do_math_write(struct proc *caller, message *m_ptr) {
    int fd = m_ptr->m_lc_vfs_fd;
    // const void *buf = (const void *) m_ptr->m_lc_vfs_buf; // Not directly used by this logic layer
    // size_t count = m_ptr->m_lc_vfs_len; // Not directly used by this logic layer

    kcapability_node_t *fd_cap = get_fd_capability(caller, fd); // Assumed available from math_syscalls.c stubs
    if (!fd_cap) {
        return -EBADF;
    }

    if (!(fd_cap->rights & CAP_WRITE)) {
        kprintf("FD %d lacks write capability\n", fd);
        return -EACCES;
    }

    if (fd_cap->expiry_time != 0 &&
        get_kernel_timestamp() > fd_cap->expiry_time) {
        kprintf("FD %d capability expired\n", fd);
        return -EACCES;
    }
    return do_write_with_capability(caller, m_ptr, fd_cap);
}

int do_math_close(struct proc *caller, message *m_ptr) {
    int fd = m_ptr->m_lc_vfs_fd;

    kcapability_node_t *fd_cap = get_fd_capability(caller, fd);
    if (!fd_cap) {
        // Allowing close on an unknown fd to follow POSIX behavior (do_close will handle EBADF)
        // However, if we *only* want to close fds with capabilities, this could be -EBADF.
        // For now, let's assume we still delegate to do_close for EBADF handling there.
    } else {
        // If a capability exists, revoke it and remove it from the process.
        revoke_capability(fd_cap);
        remove_fd_capability(caller, fd);
    }
    return do_close(caller, m_ptr); // Delegate to normal close
}

int do_math_open_cached(struct proc *caller, message *m_ptr) {
    capability_proof_t *cached_proof =
        (capability_proof_t *) m_ptr->m_lc_vfs_extra; // As per feedback for ipc.h

    if (!cached_proof || !verify_mathematical_proof(cached_proof)) { // Added null check for cached_proof
        kprintf("Cached proof invalid or null, falling back to full verification for path %s\n", (char *)m_ptr->m_lc_vfs_path);
        return do_math_open(caller, m_ptr); // Fallback
    }

    // extern int do_open_with_capability(struct proc* caller, message* m_ptr, capability_proof_t* proof); // From math_syscalls.c stubs
    return do_open_with_capability(caller, m_ptr, cached_proof); // Fast path
}

// Stubs for other new syscall handlers (moved from system.c for organization)
// These were previously defined in system.c as simple ENOSYS stubs.
// Now they are proper functions in this file.
int do_math_open_batch(struct proc *caller, message *m_ptr) {
    (void)caller; (void)m_ptr;
    kprintf("STUB: do_math_open_batch called\n");
    return -ENOSYS;
}

int do_cap_derive(struct proc *caller, message *m_ptr) {
    (void)caller; (void)m_ptr;
    kprintf("STUB: do_cap_derive called\n");
    return -ENOSYS;
}

int do_cap_restrict(struct proc *caller, message *m_ptr) {
    (void)caller; (void)m_ptr;
    kprintf("STUB: do_cap_restrict called\n");
    return -ENOSYS;
}

int do_cap_revoke(struct proc *caller, message *m_ptr) {
    (void)caller; (void)m_ptr;
    kprintf("STUB: do_cap_revoke called\n");
    return -ENOSYS;
}

int do_cap_delegate(struct proc *caller, message *m_ptr) {
    (void)caller; (void)m_ptr;
    kprintf("STUB: do_cap_delegate called\n");
    return -ENOSYS;
}

// Kernel handler for SYS_CAPABILITY_QUERY
int do_capability_query(struct proc *caller, message *m_ptr) {
    (void)caller; // Not used in this simple version

    // Respond with current kernel policy.
    // For now, assume kernel supports the feature and it's not mandatory by default.
    // This would be configurable or determined by kernel build options in a real scenario.
    int kernel_policy = POLICY_AVAILABLE;
    // Example: could also add POLICY_MANDATORY_PRIV if geteuid() of caller was 0, etc.
    // kernel_policy |= POLICY_CACHE_KERNEL; // If kernel has its own cache

    // The placeholder M_POLICY_FIELD in mathematical_init.c was m1_i1
    m_ptr->m1_i1 = kernel_policy;

    return 0; // Success
}
