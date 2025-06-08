// minix/include/minix/vfs_cap_ipc.h
#ifndef _MINIX_VFS_CAP_IPC_H
#define _MINIX_VFS_CAP_IPC_H

#include <minix/ipc.h>    // For message structure and cp_grant_id_t
#include <sys/types.h>  // For uid_t, gid_t, time_t

// Message types for VFS to Kernel (Capability Management Bridge)
// These are sent to the SYSTEM endpoint.
// Pick a base offset that doesn't clash with KERNEL_CALL or other SYSTEM messages.
// KERNEL_CALL base is 0x600. Let's use a higher range, e.g., 0x6F0.
#define VFS_CAP_IPC_BASE 0x6F0

#define VFS_KC_CONSTRUCT_PROOF   (VFS_CAP_IPC_BASE + 0)
#define VFS_KC_VERIFY_PROOF      (VFS_CAP_IPC_BASE + 1) // If VFS holds full proofs
#define VFS_KC_VALIDATE_HANDLE   (VFS_CAP_IPC_BASE + 2) // If kernel gives opaque handles
#define VFS_KC_RELEASE_HANDLE    (VFS_CAP_IPC_BASE + 3) // Optional: VFS informs kernel handle no longer needed

/*
 * Message structure for VFS -> Kernel: Request to construct/get capability proof.
 * Sent with m_type = VFS_KC_CONSTRUCT_PROOF.
 */
typedef struct {
    endpoint_t user_proc_ep;     // m1_i1: User process endpoint
    uid_t user_uid;               // m1_i2: Effective UID of the user process
    gid_t user_gid;               // m1_i3: Effective GID of the user process
    int requested_rights;         // m2_i1: CAP_READ, CAP_WRITE, etc.
    int path_len;                 // m2_i2: Length of the path string (not including null)
    cp_grant_id_t path_grant;     // m2_l1: Grant ID for path string in VFS space
    int open_flags;               // m2_i3: Original open flags (O_CREAT, etc.) for context
    uint8_t padding[MESSAGE_SIZE - (sizeof(endpoint_t) + sizeof(uid_t) + sizeof(gid_t) +
                                  3 * sizeof(int) + sizeof(cp_grant_id_t))];
} mess_vfs_kc_req_construct;
_ASSERT_MSG_SIZE(mess_vfs_kc_req_construct);


/*
 * Message structure for Kernel -> VFS: Reply for proof construction.
 * Sent with m_type = result code (OK, or error like EACCES, ENOSYS).
 */
typedef struct {
    int status;                   // m1_i1: Overall status (OK, ECAP*, other errno)
                                  // This might be redundant if m_type itself is the status.
                                  // If m_type is status, this field can be used for granted_rights.
    int granted_rights;           // m1_i1 or m1_i2: Actual rights granted by the capability
    uint64_t cap_kernel_handle;   // m1_l1: Kernel-issued opaque capability handle/ID (if successful)
    time_t expiry_time;           // m1_l2: Expiry time of this capability/handle (absolute)
    unsigned char proof_hash[32]; // m3_ca1 (example): Optional hash of proof for VFS/libc caching
                                  // This needs careful fitting into message struct.
                                  // For now, let's simplify: handle, rights, expiry. Hash later if needed.
    uint8_t padding[MESSAGE_SIZE - (2 * sizeof(int) + sizeof(uint64_t) + sizeof(time_t))];
} mess_kc_vfs_reply_construct; // Fits if proof_hash is omitted for now
_ASSERT_MSG_SIZE(mess_kc_vfs_reply_construct);


/*
 * Message structure for VFS -> Kernel: Request to validate a cached capability handle.
 * Sent with m_type = VFS_KC_VALIDATE_HANDLE.
 */
typedef struct {
    endpoint_t user_proc_ep;     // m1_i1
    uid_t user_uid;               // m1_i2
    gid_t user_gid;               // m1_i3
    uint64_t cap_kernel_handle;   // m1_l1: Handle from libc's cache (via m_lc_vfs_cached_cap)
    int requested_rights;         // m2_i1: Rights being requested now
    int path_len;                 // m2_i2: Path length (for revalidation against path if needed)
    cp_grant_id_t path_grant;     // m2_l1: Path grant
    uint8_t padding[MESSAGE_SIZE - (sizeof(endpoint_t) + sizeof(uid_t) + sizeof(gid_t) +
                                  sizeof(uint64_t) + 2 * sizeof(int) + sizeof(cp_grant_id_t))];
} mess_vfs_kc_req_validate;
_ASSERT_MSG_SIZE(mess_vfs_kc_req_validate);

/*
 * Message structure for Kernel -> VFS: Reply for handle validation.
 * Sent with m_type = result code.
 */
typedef struct {
    int status;                   // m1_i1 or m_type
    int granted_rights;           // m1_i1 or m1_i2
    time_t expiry_time;           // m1_l2 (if handle is still valid, new expiry might be returned)
    uint8_t padding[MESSAGE_SIZE - (2 * sizeof(int) + sizeof(time_t))];
} mess_kc_vfs_reply_validate; // Fits
_ASSERT_MSG_SIZE(mess_kc_vfs_reply_validate);


/*
 * Message structure for VFS -> Kernel: Notify release of a capability handle. (Optional)
 * Sent with m_type = VFS_KC_RELEASE_HANDLE
 */
typedef struct {
    uint64_t cap_kernel_handle; // m1_l1
    uint8_t padding[MESSAGE_SIZE - sizeof(uint64_t)];
} mess_vfs_kc_notify_release;
_ASSERT_MSG_SIZE(mess_vfs_kc_notify_release);


// Define message field accessors for clarity, mapping to generic message fields
// For mess_vfs_kc_req_construct:
#define m_vfs_kc_rcon_userep    m1_i1
#define m_vfs_kc_rcon_uid       m1_i2
#define m_vfs_kc_rcon_gid       m1_i3
#define m_vfs_kc_rcon_rights    m2_i1
#define m_vfs_kc_rcon_pathlen   m2_i2
#define m_vfs_kc_rcon_pathgr    m2_l1
#define m_vfs_kc_rcon_openfl    m2_i3

// For mess_kc_vfs_reply_construct: (assuming m_type is status)
#define m_kc_vfs_pcon_rights    m1_i1
#define m_kc_vfs_pcon_handle    m1_l1
#define m_kc_vfs_pcon_expiry    m1_l2
// If m_kc_vfs_pcon_status is needed as a field: m1_i2 (adjust others)

// For mess_vfs_kc_req_validate:
#define m_vfs_kc_rval_userep    m1_i1
#define m_vfs_kc_rval_uid       m1_i2
#define m_vfs_kc_rval_gid       m1_i3
#define m_vfs_kc_rval_handle    m1_l1
#define m_vfs_kc_rval_rights    m2_i1
#define m_vfs_kc_rval_pathlen   m2_i2
#define m_vfs_kc_rval_pathgr    m2_l1

// For mess_kc_vfs_reply_validate: (assuming m_type is status)
#define m_kc_vfs_pval_rights    m1_i1
#define m_kc_vfs_pval_expiry    m1_l2
// If m_kc_vfs_pval_status is needed: m1_i2 (adjust others)

// For mess_vfs_kc_notify_release:
#define m_vfs_kc_nrel_handle    m1_l1

#endif /* _MINIX_VFS_CAP_IPC_H */
