#include "ramdiskfs.h"
#include <minix/u64.h> // For make64, ex64lo, ex64hi
#include <minix/sysutil.h> // For cpf_grant_magic, cpf_revoke (if used as placeholder)

// FIXME TODO: These cpf_ functions are from libsys (specifically from sffs component of libsys).
// Ensure ramdiskfs links with a library that provides these or implement direct syscalls.
// For now, assuming they are available.
#ifndef CPF_READ
#define CPF_READ 1
#endif
#ifndef CPF_WRITE
#define CPF_WRITE 2
#endif
#ifndef GRANT_INVALID
#define GRANT_INVALID ((cp_grant_id_t)0)
#endif


// Conceptual Exokernel/Kernel call prototypes (placeholders)
// static void* exo_alloc_mem_pages_placeholder(size_t bytes); // From handle_open.c
static void exo_free_mem_pages_placeholder(void* ptr, size_t bytes) {
    // FIXME TODO: This is a placeholder. Real implementation uses Exokernel call.
    // For Phase 0.5 testing, if using malloc, this should be free.
    printf("RamdiskFS: Placeholder exo_free_mem_pages_placeholder for %p (%zu bytes)\n", ptr, bytes);
    if (ptr) free(ptr); // Placeholder for malloc based allocation
}

static void* exo_realloc_mem_pages_placeholder(void* old_ptr, size_t old_size, size_t new_size) {
    // FIXME TODO: Placeholder for a more sophisticated Exokernel realloc,
    // which might involve new alloc, copy, old free.
    // For Phase 0.5 testing with malloc, use realloc.
    // Note: realloc might move the memory block.
    printf("RamdiskFS: Placeholder exo_realloc_mem_pages_placeholder for %p from %zu to %zu bytes\n",
           old_ptr, old_size, new_size);

    if (new_size == 0) { // Standard realloc behavior: if new_size is 0, free memory
        if (old_ptr) exo_free_mem_pages_placeholder(old_ptr, old_size);
        return NULL;
    }
    if (old_ptr == NULL && old_size == 0) { // Allocation if old_ptr is NULL
         void* mem = malloc(new_size);
         if(mem) memset(mem, 0, new_size);
         return mem;
    }

    void* new_ptr = realloc(old_ptr, new_size);
    if (!new_ptr && new_size > 0) { // realloc failed but new_size was non-zero
        // Original block old_ptr is NOT freed by realloc on failure.
        // We should not free it here, as the caller might still have the old_ptr.
        // The caller (e.g. write logic) needs to handle this failure.
        printf("RamdiskFS: exo_realloc_mem_pages_placeholder: realloc returned NULL for new_size %zu\n", new_size);
        return NULL;
    }
    // If new_size > old_size, the extended part is uninitialized by realloc. Zero it.
    if (new_ptr && new_size > old_size) {
        memset((char*)new_ptr + old_size, 0, new_size - old_size);
    }
    return new_ptr;
}


int handle_readwrite(void) {
    uint32_t cap_idx;
    size_t nbytes;
    vir_bytes user_buf_addr;
    endpoint_t req_ep;
    off_t offset64;
    uint32_t offset_lo, offset_hi;
    int r;
    cp_grant_id_t grant_id = GRANT_INVALID; // Initialize to invalid

    if (call_nr == RAMDISKFS_READ) {
        cap_idx = m_in.M_RAMDISKFS_READ_CAP_IDX;
        nbytes = m_in.M_RAMDISKFS_READ_NBYTES;
        user_buf_addr = (vir_bytes)m_in.M_RAMDISKFS_READ_BUF_ADDR;
        req_ep = m_in.M_RAMDISKFS_READ_REQ_ENDPT;
        offset_lo = m_in.M_RAMDISKFS_READ_OFFSET_LO;
        offset_hi = m_in.M_RAMDISKFS_READ_OFFSET_HI;
    } else { // RAMDISKFS_WRITE
        cap_idx = m_in.M_RAMDISKFS_WRITE_CAP_IDX;
        nbytes = m_in.M_RAMDISKFS_WRITE_NBYTES;
        user_buf_addr = (vir_bytes)m_in.M_RAMDISKFS_WRITE_BUF_ADDR;
        req_ep = m_in.M_RAMDISKFS_WRITE_REQ_ENDPT;
        offset_lo = m_in.M_RAMDISKFS_WRITE_OFFSET_LO;
        offset_hi = m_in.M_RAMDISKFS_WRITE_OFFSET_HI;
    }
    offset64 = make64(offset_lo, offset_hi);

    printf("RamdiskFS: handle_readwrite: call %d, cap_idx %u, nbytes %zu, offset %lld, user_ep %d, user_buf 0x%lx\n",
           call_nr, cap_idx, nbytes, (long long)offset64, req_ep, (unsigned long)user_buf_addr);

    if (cap_idx >= MAX_RAMDISK_FILES || !open_files[cap_idx].in_use) {
        printf("RamdiskFS: handle_readwrite: invalid cap_idx %u (max %d, in_use %d)\n",
               cap_idx, MAX_RAMDISK_FILES, open_files[cap_idx].in_use);
        return EBADF; // Bad file descriptor (capability index)
    }

    ramdisk_file_t *rf = &open_files[cap_idx];

    // FIXME TODO: Check capability permissions here based on file_obj_cap from caller's table.
    // This would involve a kernel call like:
    // r = sys_cap_check_permissions(who_e, cap_idx_from_message, required_permissions_mask);
    // if (r != OK) return EPERM;
    // For Phase 0.5, we assume the open flags were checked by handle_open and permissions are okay
    // and that the cap_idx passed by the user is valid and refers to what was given at open.

    if (offset64 < 0) {
        printf("RamdiskFS: handle_readwrite: invalid negative offset %lld\n", (long long)offset64);
        return EINVAL;
    }

    if (call_nr == RAMDISKFS_READ) {
        if (nbytes == 0) return 0;
        if (user_buf_addr == 0) return EFAULT; // Null buffer from user
        if (offset64 >= rf->current_size) return 0; // Read at or after EOF

        size_t bytes_to_read = nbytes;
        if ((uint64_t)offset64 + nbytes > rf->current_size) {
            bytes_to_read = rf->current_size - offset64;
        }
        if (bytes_to_read == 0) return 0; // Nothing to read if calculated size is 0

        // Create grant for RamdiskFS to write to user's buffer
        grant_id = cpf_grant_magic(req_ep, SELF, user_buf_addr, bytes_to_read, CPF_WRITE);
        if (grant_id == GRANT_INVALID) {
            printf("RamdiskFS: READ - cpf_grant_magic to user_ep %d failed for user_buf 0x%lx, size %zu\n",
                   req_ep, (unsigned long)user_buf_addr, bytes_to_read);
            return EPERM; // Using EPERM as an indication of grant failure
        }

        r = sys_safecopyto(req_ep, grant_id, (vir_bytes)0,
                           (vir_bytes)(rf->data_ptr + offset64), bytes_to_read);
        cpf_revoke(grant_id);

        if (r != OK) {
            printf("RamdiskFS: READ - sys_safecopyto failed: %d\n", r);
            return EIO; // Generic I/O error if copy failed
        }
        printf("RamdiskFS: READ - Copied %zu bytes to user for cap_idx %u\n", bytes_to_read, cap_idx);
        return (int)bytes_to_read;

    } else { // RAMDISKFS_WRITE
        if (nbytes == 0) return 0;
        if (user_buf_addr == 0) return EFAULT; // Null buffer from user

        size_t new_required_size = offset64 + nbytes;
        if (new_required_size > rf->allocated_size) {
            printf("RamdiskFS: WRITE - Growing file %s from %zu to %zu for cap_idx %u\n",
                   rf->name, rf->allocated_size, new_required_size, cap_idx);
            char *new_data_ptr = (char*)exo_realloc_mem_pages_placeholder(rf->data_ptr, rf->allocated_size, new_required_size);
            if (new_data_ptr == NULL && new_required_size > 0) {
                printf("RamdiskFS: WRITE - exo_realloc_mem_pages_placeholder failed for cap_idx %u, new_size %zu\n",
                       cap_idx, new_required_size);
                return ENOMEM;
            }
            rf->data_ptr = new_data_ptr;
            // If new_data_ptr is NULL because new_required_size was 0, allocated_size should be 0.
            rf->allocated_size = (new_data_ptr == NULL) ? 0 : new_required_size;
        }

        // Create grant for RamdiskFS to read from user's buffer
        grant_id = cpf_grant_magic(req_ep, SELF, user_buf_addr, nbytes, CPF_READ);
        if (grant_id == GRANT_INVALID) {
            printf("RamdiskFS: WRITE - cpf_grant_magic from user_ep %d failed for user_buf 0x%lx, size %zu\n",
                   req_ep, (unsigned long)user_buf_addr, nbytes);
            return EPERM;
        }

        r = sys_safecopyfrom(req_ep, grant_id, (vir_bytes)0,
                             (vir_bytes)(rf->data_ptr + offset64), nbytes);
        cpf_revoke(grant_id);

        if (r != OK) {
            printf("RamdiskFS: WRITE - sys_safecopyfrom failed: %d\n", r);
            return EIO;
        }

        if (new_required_size > rf->current_size) {
            rf->current_size = new_required_size;
        }
        printf("RamdiskFS: WRITE - Wrote %zu bytes to file '%s' (cap_idx %u), new size %zu\n",
               nbytes, rf->name, cap_idx, rf->current_size);
        return (int)nbytes;
    }
}
