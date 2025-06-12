#ifndef _MINIX_KERNEL_CAP_RIGHTS_H
#define _MINIX_KERNEL_CAP_RIGHTS_H

// General Capability Permissions (can be combined)
#define CAP_PERM_NONE         0x0000
#define CAP_PERM_IPC_SEND     (1 << 0) // Allowed to send IPC messages via this cap
#define CAP_PERM_IPC_RECEIVE  (1 << 1) // Allowed to receive (less common for client caps)
// Add more generic permissions as needed: E.g., for memory, device access for Exokernel

// Service-Specific Permissions for RamdiskFS
// For capabilities TO the RamdiskFS service itself (CAP_RAMDISK_FS_SERVICE type)
#define RAMDISKFS_SERV_PERM_OPEN    (1 << 8)  // Allowed to open/create files
#define RAMDISKFS_SERV_PERM_UNLINK  (1 << 9)  // Allowed to unlink files (future)
#define RAMDISKFS_SERV_PERM_STAT    (1 << 10) // Allowed to stat files (future)

// For capabilities TO a RamdiskFS file object (CAP_RAMDISK_FILE_OBJECT type)
#define RAMDISKFS_FILE_PERM_READ    (1 << 16) // Allowed to read from the file object
#define RAMDISKFS_FILE_PERM_WRITE   (1 << 17) // Allowed to write to the file object
// #define RAMDISKFS_FILE_PERM_STAT (1 << 18) // Allowed to fstat the file object (future)

// FIXME TODO: Define actual capability *type* identifiers if the kernel capability_t
// needs to store a type field distinct from service_id for more fine-grained typing.
// For now, permissions are associated with capabilities to a (service_id, object_handle) pair.
// The service_id (e.g., RamdiskFS_SERVICE_ID) implicitly defines the "type" of service.
// The object_handle (from cap->endpoint_or_obj) further specifies.
// File object capabilities would have RamdiskFS_SERVICE_ID and obj_handle being the file_idx.

#endif // _MINIX_KERNEL_CAP_RIGHTS_H
