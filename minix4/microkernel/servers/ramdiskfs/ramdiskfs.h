#ifndef _RAMDISKFS_H
#define _RAMDISKFS_H

#include <minix/com.h>
#include <minix/config.h>
#include <minix/ipc.h>
#include <minix/syslib.h>    // For sys_safecopyfrom/to, cpf_grant_magic etc.
#include <minix/safecopies.h> // For cp_grant_id_t, GRANT_INVALID
#include <minix/ramdiskfsif.h> // Interface header
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>         // For malloc, free (if used for internal structs)
#include <stdio.h>          // For printf

// FIXME TODO: Define RAMDISKFS_PROC_NR properly in minix/com.h
#ifndef RAMDISKFS_PROC_NR
#define RAMDISKFS_PROC_NR ((endpoint_t) 12)
#endif

#define MAX_RAMDISK_FILES 32
#define MAX_FILENAME_LEN  60
#define DEFAULT_FILE_ALLOC_SIZE 4096 // Bytes, e.g., one page

// Permissions for capabilities (conceptual)
#define RAMDISK_FILE_PERM_READ  0x1
#define RAMDISK_FILE_PERM_WRITE 0x2

typedef struct {
    char name[MAX_FILENAME_LEN];
    char *data_ptr;         // Pointer to memory block (conceptual, from Exokernel)
    size_t current_size;
    size_t allocated_size;   // Actual size of exokernel-allocated memory block
    // FIXME TODO: Add mem_capability from exokernel for data_ptr
    // FIXME TODO: Add ref_count for multiple opens if needed
    int in_use;
} ramdisk_file_t;

extern ramdisk_file_t open_files[MAX_RAMDISK_FILES];
extern message m_in; // Incoming message
extern message m_out; // Outgoing message
extern endpoint_t who_e; // Caller endpoint
extern int call_nr;   // Call number

// Function prototypes
int handle_open(void);
int handle_readwrite(void); // Will differentiate READ/WRITE based on call_nr
int handle_close(void);

// Conceptual Exokernel calls (placeholders for now)
// void* exo_alloc_mem_pages_k(size_t bytes); // Kernel/Exo function to alloc
// void exo_free_mem_pages_k(void* ptr, size_t bytes);
// int sys_create_user_capability(endpoint_t user_ep, uint32_t *cap_idx_out, uint32_t service_id, uint32_t epoch, uint32_t permissions, uintptr_t object_handle);

#endif // _RAMDISKFS_H
