#include "ramdiskfs.h"
#include <fcntl.h> // For O_ACCMODE, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT, O_EXCL
#include <minix/kernel/cap_rights.h> // For actual permission bits if defined and accessible
                                     // If not directly accessible, use placeholders for now.

// FIXME TODO: This should be a dynamically assigned or well-known registered ID.
// For Phase 0.5, using its own process number part of its endpoint.
// RAMDISKFS_PROC_NR is already defined in ramdiskfs.h (conditionally)
#define RAMDISKFS_SERVICE_ID (_ENDPOINT_P(RAMDISKFS_PROC_NR))

// FIXME TODO: This epoch should be initialized and managed by RS when RamdiskFS starts/restarts.
// For Phase 0.5, the actual epoch would be part of this service's rproc data,
// maintained by RS. RamdiskFS needs a way to query this.
// static uint32_t ramdiskfs_current_epoch = 1; // REMOVED - RamdiskFS must get its real epoch


// Conceptual Exokernel/Kernel call prototypes (placeholders)
static void* exo_alloc_mem_pages_placeholder(size_t bytes) {
    printf("RamdiskFS: Placeholder exo_alloc_mem_pages_placeholder for %zu bytes\n", bytes);
    void *mem = malloc(bytes);
    if(mem) {
        memset(mem, 0, bytes);
    }
    return mem;
}

static void exo_free_mem_pages_placeholder(void* ptr, size_t UNUSED(bytes)) {
    printf("RamdiskFS: Placeholder exo_free_mem_pages_placeholder for ptr %p\n", ptr);
    if (ptr) {
        free(ptr);
    }
}

// Placeholder permission bits if not included from cap_rights.h yet by build system
#ifndef RAMDISKFS_FILE_PERM_READ
#define RAMDISKFS_FILE_PERM_READ    (1 << 16)
#endif
#ifndef RAMDISKFS_FILE_PERM_WRITE
#define RAMDISKFS_FILE_PERM_WRITE   (1 << 17)
#endif


int handle_open(void) {
    char path_from_user[MAX_FILENAME_LEN];
    vir_bytes user_path_addr = (vir_bytes)m_in.M_RAMDISKFS_OPEN_PATH_ADDR;
    size_t path_len = m_in.M_RAMDISKFS_OPEN_PATH_LEN;
    int flags = m_in.M_RAMDISKFS_OPEN_FLAGS;
    mode_t mode = (mode_t)m_in.M_RAMDISKFS_OPEN_MODE;
    endpoint_t req_ep = m_in.M_RAMDISKFS_OPEN_REQ_ENDPT;
    int r;
    int free_slot = -1; // Initialize to invalid
    ramdisk_file_t *rf_to_open = NULL; // Initialize to NULL


    if (path_len == 0 || path_len > MAX_FILENAME_LEN) {
        printf("RamdiskFS: handle_open: Invalid path_len %zu\n", path_len);
        return EINVAL;
    }

    // Using sys_safecopyfrom: grant_id is effectively user_path_addr, offset is 0
    r = sys_safecopyfrom(req_ep, (cp_grant_id_t)user_path_addr, 0,
                         (vir_bytes)path_from_user, path_len);
    if (r != OK) {
        printf("RamdiskFS: sys_safecopyfrom for path failed: %d (src_ep %d, path_addr 0x%lx, len %zu)\n",
               r, req_ep, (unsigned long)user_path_addr, path_len);
        return EFAULT;
    }
    if (path_len == MAX_FILENAME_LEN) path_from_user[MAX_FILENAME_LEN-1] = '\0';


    printf("RamdiskFS: handle_open for path '%s', flags 0x%x, mode 0%o from EP %d\n",
           path_from_user, flags, mode, req_ep);

    int existing_slot = -1;
    for (int i = 0; i < MAX_RAMDISK_FILES; i++) {
        if (open_files[i].in_use) {
            if (strncmp(open_files[i].name, path_from_user, MAX_FILENAME_LEN) == 0) {
                existing_slot = i;
                break;
            }
        } else if (free_slot == -1) {
            free_slot = i;
        }
    }

    if (flags & O_CREAT) {
        if (existing_slot != -1) {
            if (flags & O_EXCL) {
                return EEXIST;
            }
            rf_to_open = &open_files[existing_slot];
            printf("RamdiskFS: File '%s' exists, opening in slot %d.\n", rf_to_open->name, existing_slot);
        } else {
            if (free_slot == -1) {
                return ENFILE;
            }
            existing_slot = free_slot;
            rf_to_open = &open_files[existing_slot];

            strncpy(rf_to_open->name, path_from_user, MAX_FILENAME_LEN -1);
            rf_to_open->name[MAX_FILENAME_LEN-1] = '\0';

            rf_to_open->data_ptr = (char*)exo_alloc_mem_pages_placeholder(DEFAULT_FILE_ALLOC_SIZE);
            if (rf_to_open->data_ptr == NULL) {
                printf("RamdiskFS: Failed to allocate memory for new file '%s'\n", rf_to_open->name);
                memset(rf_to_open->name, 0, MAX_FILENAME_LEN);
                return ENOMEM;
            }
            rf_to_open->allocated_size = DEFAULT_FILE_ALLOC_SIZE;
            rf_to_open->current_size = 0;
            rf_to_open->in_use = 1;
            printf("RamdiskFS: Created file '%s' in slot %d, data_ptr %p\n", rf_to_open->name, existing_slot, rf_to_open->data_ptr);
        }
    } else {
        if (existing_slot == -1) {
            return ENOENT;
        }
        rf_to_open = &open_files[existing_slot];
        printf("RamdiskFS: File '%s' exists, opening in slot %d.\n", rf_to_open->name, existing_slot);
    }

    if (rf_to_open == NULL) {
        return EINTERNAL;
    }

    uint32_t file_object_cap_idx;
    uint32_t permissions = 0;

    if ((flags & O_ACCMODE) == O_RDONLY) {
        permissions |= RAMDISKFS_FILE_PERM_READ;
    } else if ((flags & O_ACCMODE) == O_WRONLY) {
        permissions |= RAMDISKFS_FILE_PERM_WRITE;
    } else if ((flags & O_ACCMODE) == O_RDWR) {
        permissions |= RAMDISKFS_FILE_PERM_READ | RAMDISKFS_FILE_PERM_WRITE;
    }

    message m_cap_req;
    memset(&m_cap_req, 0, sizeof(m_cap_req));

    m_cap_req.m1_i1 = req_ep;
    m_cap_req.m1_i2 = permissions;
    m_cap_req.m1_i3 = (uintptr_t)existing_slot;
    m_cap_req.m2_i1 = RAMDISKFS_SERVICE_ID;
    m_cap_req.m2_i2 = ramdiskfs_current_epoch;

    printf("RamdiskFS: Calling KERNEL SYS_CREATE_USER_CAPABILITY for client EP %d, my SID %u, my epoch %u, perms 0x%x, obj_handle %d\n",
           req_ep, RAMDISKFS_SERVICE_ID, ramdiskfs_current_epoch, permissions, existing_slot);

    r = _kernel_call(SYS_CREATE_USER_CAPABILITY, &m_cap_req);

    if (r != OK) {
        printf("RamdiskFS: SYS_CREATE_USER_CAPABILITY failed with kernel error %d\n", r);
        if ((flags & O_CREAT) && (free_slot == existing_slot) && rf_to_open->data_ptr) {
            exo_free_mem_pages_placeholder(rf_to_open->data_ptr, rf_to_open->allocated_size);
            rf_to_open->data_ptr = NULL; // Important after freeing
            rf_to_open->in_use = 0;
            rf_to_open->name[0] = '\0';
        }
        return r;
    }

    file_object_cap_idx = (uint32_t)m_cap_req.m1_i1; // Kernel returns new cap_idx here

    m_out.M_RAMDISKFS_OPEN_REPLY_CAP_IDX = file_object_cap_idx;
    printf("RamdiskFS: Opened/Created '%s'. Kernel granted cap_idx %u to client EP %d. Returning handle %u to client.\n",
           rf_to_open->name, file_object_cap_idx, req_ep, file_object_cap_idx);

    return OK;
}
