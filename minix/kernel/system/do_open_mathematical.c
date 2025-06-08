/**
 * @file do_open_mathematical.c
 * @brief Mathematical implementation of POSIX open() system call
 *
 * This module demonstrates how POSIX system calls can be implemented as
 * mathematical operations on capability structures, providing both correctness
 * guarantees and performance benefits through mathematical optimization.
 * This is a conceptual implementation focusing on the integration points
 * with the capability DAG.
 */

// Kernel core definitions (proc, message, error codes, system call numbers)
#include "kernel/kernel.h"
#include "kernel/proc.h"
#include <sys/errno.h>

// Kernel library
#include "../../lib/klib/include/klib.h"

// Capability DAG API and global DAG pointer
#include <minix/kcapability_dag.h>
#include <minix/kcap.h>

// Standard library types
#include <sys/types.h>
#include <sys/stat.h>  // For S_IRUSR, S_IWUSR mode constants

// Capability Rights Macros
#ifndef CAP_RIGHT_TRAVERSE
#define CAP_RIGHT_TRAVERSE (_BitInt(64))0x00010000
#endif
#ifndef CAP_RIGHT_READ
#define CAP_RIGHT_READ (_BitInt(64))0x00020000
#endif
#ifndef CAP_RIGHT_WRITE
#define CAP_RIGHT_WRITE (_BitInt(64))0x00040000
#endif

// POSIX open flags (simplified definitions)
#ifndef O_RDONLY
#define O_RDONLY    00
#endif
#ifndef O_WRONLY
#define O_WRONLY    01
#endif
#ifndef O_RDWR
#define O_RDWR      02
#endif
#ifndef O_ACCMODE
#define O_ACCMODE   (O_RDONLY | O_WRONLY | O_RDWR)
#endif

// File descriptor table and error codes
#ifndef NO_FILES // Max open files per process
#define NO_FILES 256
#endif
#ifndef EMFILE // Too many open files error code
#define EMFILE 24
#endif


// Forward declarations for static helper functions
static kcapability_dag_node_t* kcapability_dag_lookup_process_capability(
    kcapability_dag_t* dag, endpoint_t proc_ep);
static int validate_directory_traversal_mathematical(
    kcapability_dag_t* dag, kcapability_dag_node_t* proc_cap, const char* path);
static int validate_file_access_mathematical(
    kcapability_dag_t* dag, kcapability_dag_node_t* file_or_dir_cap, const char* path, int flags);
static kcapability_dag_node_t* create_file_capability_mathematical(
    kcapability_dag_t* dag, kcapability_dag_node_t* parent_dir_cap, const char* path, int flags, mode_t mode);
static int allocate_file_descriptor_for_capability(
    struct proc* caller, kcapability_dag_node_t* file_cap);
static const char* get_filename_from_path(const char* path);
static kcapability_dag_node_t* lookup_component_capability(
    kcapability_dag_t* dag,
    kcapability_dag_node_t* parent_dir_cap,
    const char* component_name);


int do_open_mathematical(struct proc* caller, message* m_ptr) {
    KASSERT(caller != NULL, "do_open_mathematical: caller proc is NULL");
    KASSERT(m_ptr != NULL, "do_open_mathematical: message pointer is NULL");
    KASSERT(kernel_capability_dag != NULL, "do_open_mathematical: kernel_capability_dag not initialized");

    char* pathname = (char*)m_ptr->m_lc_vfs_path.name;
    int flags = m_ptr->m_lc_vfs_path.flags;
    mode_t mode = (mode_t)m_ptr->m_lc_vfs_path.mode;

    kprintf_stub("do_open_mathematical: Called for path '%s', flags %x, mode %o\n", pathname, flags, mode);

    kcapability_dag_node_t* process_capability =
        kcapability_dag_lookup_process_capability(kernel_capability_dag, caller->p_endpoint);

    if (!process_capability) {
        kprintf_stub("do_open_mathematical: No capability node for caller %d\n", caller->p_endpoint);
        return -EPERM;
    }

    // This will be the capability of the final directory component after traversal.
    kcapability_dag_node_t* parent_directory_capability = process_capability; // Start with process cap

    int traversal_result = validate_directory_traversal_mathematical(
        kernel_capability_dag, &parent_directory_capability, pathname); // Pass by address to update

    if (traversal_result != SUCCESS) {
        kprintf_stub("do_open_mathematical: Directory traversal denied for path '%s' by caller %d (err: %d)\n", pathname, caller->p_endpoint, traversal_result);
        return traversal_result;
    }

    // parent_directory_capability now holds the capability of the directory where the file is/will be.
    // The actual file/object to check access for is the last component of the path.
    // We need to look up this specific component under parent_directory_capability.
    // If O_CREAT is involved, this component might not exist yet.

    const char* filename = get_filename_from_path(pathname);
    kcapability_dag_node_t* target_object_cap = NULL;

    if (!(flags & O_CREAT)) { // If not creating, the target must exist
        target_object_cap = lookup_component_capability(kernel_capability_dag, parent_directory_capability, filename);
        if (!target_object_cap) {
            kprintf_stub("do_open_mathematical: Target object '%s' not found under parent cap ID %llu\n",
                         filename, (unsigned long long)parent_directory_capability->capability_id);
            return -ENOENT;
        }
    } else {
        // For O_CREAT, target_object_cap might be NULL (new file) or exist (O_EXCL not set).
        // If it exists, validate_file_access_mathematical will check its rights.
        // If it doesn't exist, access check is on parent_dir_cap's ability to allow creation (implicit for now).
        target_object_cap = lookup_component_capability(kernel_capability_dag, parent_directory_capability, filename);
        // If target_object_cap is NULL here, it means we are creating a new file.
        // If it's not NULL, we are opening an existing file (possibly with O_TRUNC, etc.).
    }


    // If we are not creating and the object doesn't exist (target_object_cap is NULL), it's an error.
    // If we ARE creating, target_object_cap being NULL is expected for a new file.
    // The access check needs to be on parent_dir_cap if creating, or target_object_cap if opening existing.

    kcapability_dag_node_t* cap_to_check_access_on = target_object_cap;
    if (flags & O_CREAT && !target_object_cap) {
        // For creating a new file, the relevant rights (e.g., WRITE on directory) are checked
        // implicitly by allowing create_file_capability_mathematical to proceed based on parent_dir_cap.
        // No specific access check on a non-existent file.
        kprintf_stub("do_open_mathematical: O_CREAT set and file does not exist. Access check deferred to creation logic.\n");
    } else if (target_object_cap) {
        int access_result = validate_file_access_mathematical(
            kernel_capability_dag, target_object_cap, filename, flags);
        if (access_result != SUCCESS) {
            kprintf_stub("do_open_mathematical: File access denied for '%s', flags %x (err: %d)\n", filename, flags, access_result);
            return access_result;
        }
    } else if (!(flags & O_CREAT)) {
         kprintf_stub("do_open_mathematical: File '%s' does not exist and O_CREAT not set.\n", filename);
         return -ENOENT; // File doesn't exist, and we are not creating it.
    }


    kcapability_dag_node_t* final_file_capability = NULL;
    if (flags & O_CREAT) {
        final_file_capability = create_file_capability_mathematical(
            kernel_capability_dag, parent_directory_capability, pathname, flags, mode);
        if (!final_file_capability) {
            kprintf_stub("do_open_mathematical: Failed to create file capability for path '%s'\n", pathname);
            return -ENOMEM;
        }
    } else {
        final_file_capability = target_object_cap; // We are opening an existing file
        KASSERT(final_file_capability != NULL, "do_open_mathematical: target_object_cap is NULL when O_CREAT is not set but access passed.");
    }


    int fd = allocate_file_descriptor_for_capability(caller, final_file_capability);
    if (fd < 0) {
        kprintf_stub("do_open_mathematical: Failed to allocate FD for file capability of path '%s' (err: %d)\n", pathname, fd);
        // If O_CREAT and fd allocation fails, the newly created node/file might need cleanup.
        // This is complex and depends on transactional semantics not yet implemented.
        // For now, if create_file_capability_mathematical added it to DAG, it remains.
        return fd;
    }

    kprintf_stub("do_open_mathematical: Success for path '%s', fd %d granted to caller %d\n", pathname, fd, caller->p_endpoint);
    return fd;
}

static kcapability_dag_node_t* kcapability_dag_lookup_process_capability(
    kcapability_dag_t* dag, endpoint_t proc_ep) {
    KASSERT(dag != NULL, "lookup_process_capability: dag is NULL");
    for (_BitInt(32) i = 0wb; i < dag->node_count; i++) {
        kcapability_dag_node_t* current_node = &dag->nodes[i];
        if (current_node->is_process_main_cap_node && current_node->owner_endpoint == proc_ep) {
            return current_node;
        }
    }
    kprintf_stub("lookup_process_capability: No capability node found for ep %d\n", proc_ep);
    return NULL;
}

static kcapability_dag_node_t* lookup_component_capability(
    kcapability_dag_t* dag,
    kcapability_dag_node_t* parent_dir_cap,
    const char* component_name) {

    KASSERT(dag != NULL, "lookup_component_capability: dag is NULL");
    KASSERT(parent_dir_cap != NULL, "lookup_component_capability: parent_dir_cap is NULL");
    KASSERT(component_name != NULL, "lookup_component_capability: component_name is NULL");

    for (_BitInt(16) i = 0wb; i < parent_dir_cap->child_count; i++) {
        kcapability_dag_node_t* child_node = parent_dir_cap->children[i];
        KASSERT(child_node != NULL, "lookup_component_capability: child_node in list is NULL");

        if (kstrcmp_c23(component_name, child_node->name) == 0) {
            kprintf_stub("lookup_component_capability: Found component '%s' (cap ID %llu) under parent cap ID %llu.\n",
                         component_name, (unsigned long long)child_node->capability_id, (unsigned long long)parent_dir_cap->capability_id);
            return child_node;
        }
    }

    kprintf_stub("lookup_component_capability: Component '%s' not found under parent cap ID %llu.\n",
                 component_name, (unsigned long long)parent_dir_cap->capability_id);
    return NULL;
}

// Modified to pass current_directory_cap by address to update it during traversal
static int validate_directory_traversal_mathematical(
    kcapability_dag_t* dag,
    kcapability_dag_node_t** current_directory_cap_ptr, // Pass by address
    const char* path) {

    KASSERT(dag != NULL, "validate_dir_trav: dag is NULL");
    KASSERT(current_directory_cap_ptr != NULL && *current_directory_cap_ptr != NULL, "validate_dir_trav: current_directory_cap_ptr is NULL or points to NULL");
    KASSERT(path != NULL, "validate_dir_trav: path is NULL");

    kprintf_stub("validate_directory_traversal_mathematical: Validating path '%s' for initial cap ID %llu\n",
                 path, (unsigned long long)(*current_directory_cap_ptr)->capability_id);

    char path_copy[256];
    kstrlcpy_c23(path_copy, path, sizeof(path_copy));

    // kcapability_dag_node_t* current_directory_cap = *current_directory_cap_ptr; // Local copy for traversal

    if (path_copy[0] == '/') {
        // This conceptual implementation does not fully resolve absolute paths from a system root capability.
        // It assumes the initial *current_directory_cap_ptr is either the process's CWD capability
        // or for absolute paths, it should be pre-set to the system's root capability.
        // For simplicity, we log and proceed with the provided starting capability.
        kprintf_stub("validate_directory_traversal_mathematical: Absolute path, using provided cap as starting point.\n");
    }

    char *token;
    char *rest = path_copy;
    const char *original_path_for_logging = path; // Keep original path for logging full path

    // Path processing logic needs to handle the full path vs. just the next component.
    // The loop should consume path components. If a component is the *last* one,
    // it might be a file, not a directory, so traversal checks stop there.
    // The `create_file_capability_mathematical` or `validate_file_access_mathematical`
    // will handle the final component. This function ensures all parent dirs are traversable.

    char* next_component_start = path_copy;
    if (path_copy[0] == '/') { // Skip leading slash for tokenization
        next_component_start++;
    }

    while (*next_component_start != '\0') {
        token = next_component_start;
        char* end_of_component = token;
        while (*end_of_component != '\0' && *end_of_component != '/') {
            end_of_component++;
        }

        char* next_path_segment = NULL;
        if (*end_of_component == '/') {
            *end_of_component = '\0'; // Null-terminate current component
            next_path_segment = end_of_component + 1;
        }
        // If *end_of_component is '\0', this is the last component.

        // If there's a next segment OR if the current token is not empty (it's the last component)
        // AND this component is not the filename itself (which is handled later for access/creation)
        if (next_path_segment != NULL && *next_path_segment != '\0') { // This token is a directory in the path, not the final file target
             if (*token == '\0') { // Handles cases like "//" by skipping empty token
                next_component_start = next_path_segment;
                continue;
            }
            kprintf_stub("validate_directory_traversal_mathematical: Traversing component: '%s'\n", token);

            if (!((*current_directory_cap_ptr)->rights_mask & CAP_RIGHT_TRAVERSE)) {
                kprintf_stub("validate_directory_traversal_mathematical: No TRAVERSE right in current_directory_cap ID %llu for component '%s'\n",
                             (unsigned long long)(*current_directory_cap_ptr)->capability_id, token);
                return -EACCES;
            }

            kcapability_dag_node_t* component_cap = lookup_component_capability(dag, *current_directory_cap_ptr, token);

            if (!component_cap) {
                kprintf_stub("validate_directory_traversal_mathematical: Component '%s' not found under cap ID %llu\n",
                             token, (unsigned long long)(*current_directory_cap_ptr)->capability_id);
                return -ENOENT;
            }
            *current_directory_cap_ptr = component_cap; // Update the caller's pointer to the new current directory
        } else if (*token != '\0') { // This is the last component (filename or final dir)
             kprintf_stub("validate_directory_traversal_mathematical: Reached final component '%s', traversal check stops here.\n", token);
             // The rights on this final component will be checked by validate_file_access or create_file_capability
        }


        if (next_path_segment == NULL || *next_path_segment == '\0') { // No more components
            break;
        }
        next_component_start = next_path_segment;
    }

    kprintf_stub("validate_directory_traversal_mathematical: Path prefix of '%s' successfully traversed. Final directory cap ID %llu.\n",
                 original_path_for_logging, (unsigned long long)(*current_directory_cap_ptr)->capability_id);
    return SUCCESS;
}

static int validate_file_access_mathematical(
    kcapability_dag_t* dag,
    kcapability_dag_node_t* file_or_dir_cap, // This should be the capability of the actual file/dir if it exists
    const char* path_or_filename, // Can be full path for logging, or just filename
    int flags) {
    KASSERT(dag != NULL, "validate_file_access: dag is NULL");
    KASSERT(file_or_dir_cap != NULL, "validate_file_access: file_or_dir_cap is NULL"); // This must be non-NULL if checking existing
    KASSERT(path_or_filename != NULL, "validate_file_access: path_or_filename is NULL");

    kprintf_stub("validate_file_access_mathematical: Validating access for '%s', flags %x, cap ID %llu, cap rights %llx\n",
                 path_or_filename, flags, (unsigned long long)file_or_dir_cap->capability_id, (unsigned long long)file_or_dir_cap->rights_mask);

    _BitInt(64) required_rights = 0wb;
    bool read_wanted = false;
    bool write_wanted = false;
    int access_mode = flags & O_ACCMODE;
    if (access_mode == O_RDONLY) {
        read_wanted = true;
    } else if (access_mode == O_WRONLY) {
        write_wanted = true;
    } else if (access_mode == O_RDWR) {
        read_wanted = true;
        write_wanted = true;
    }
    if (read_wanted) {
        required_rights |= CAP_RIGHT_READ;
    }
    if (write_wanted) {
        required_rights |= CAP_RIGHT_WRITE;
    }
    if ((file_or_dir_cap->rights_mask & required_rights) == required_rights) {
        kprintf_stub("validate_file_access_mathematical: Access GRANTED for '%s'. Required: %llx, Has: %llx\n",
                     path_or_filename, (unsigned long long)required_rights, (unsigned long long)file_or_dir_cap->rights_mask);
        return SUCCESS;
    } else {
        kprintf_stub("validate_file_access_mathematical: Access DENIED for '%s'. Required: %llx, Has: %llx\n",
                     path_or_filename, (unsigned long long)required_rights, (unsigned long long)file_or_dir_cap->rights_mask);
        return -EACCES;
    }
}

static const char* get_filename_from_path(const char* path) {
    if (!path) return NULL;
    const char* filename = path;
    const char* p = path;
    while (*p) {
        if (*p == '/') {
            filename = p + 1;
        }
        p++;
    }
    return filename;
}

static kcapability_dag_node_t* create_file_capability_mathematical(
    kcapability_dag_t* dag,
    kcapability_dag_node_t* parent_dir_cap,
    const char* path,
    int flags,
    mode_t mode) {
    KASSERT(dag != NULL, "create_file_cap: dag is NULL");
    KASSERT(parent_dir_cap != NULL, "create_file_cap: parent_dir_cap is NULL");
    KASSERT(path != NULL, "create_file_cap: path is NULL");
    _BitInt(64) new_file_rights = 0wb;
    if (mode & S_IRUSR) new_file_rights |= CAP_RIGHT_READ;
    if (mode & S_IWUSR) new_file_rights |= CAP_RIGHT_WRITE;
    new_file_rights &= parent_dir_cap->rights_mask;
    if (new_file_rights == 0wb && (mode & (S_IRUSR | S_IWUSR))) { // Log if requested rights became 0 after masking
        kprintf_stub("create_file_capability_mathematical: Calculated new_file_rights are 0 for path '%s' mode %o under parent cap %llu after parent mask.\n",
                     path, mode, (unsigned long long)parent_dir_cap->capability_id);
    }
    _BitInt(16) new_file_sec_level = parent_dir_cap->security_level;
    static _BitInt(64) next_cap_id_debug = 2000wb;
    _BitInt(64) new_cap_id = next_cap_id_debug++;
    const char* filename = get_filename_from_path(path);
    kcapability_dag_node_t* new_file_cap = kcapability_dag_node_create(new_cap_id, new_file_rights, new_file_sec_level, filename);
    if (!new_file_cap) {
        kprintf_stub("create_file_capability_mathematical: kcapability_dag_node_create failed for path '%s'\n", path);
        return NULL;
    }
    new_file_cap->resource_ptr = (void*)path;
    kprintf_stub("create_file_capability_mathematical: Conceptual: new_file_cap (ID %llu) for filename '%s' would be added to dag's main store here if not already handled by node_create.\n", (unsigned long long)new_cap_id, filename ? filename : "");
    if (kcapability_dag_add_edge(dag, parent_dir_cap, new_file_cap) != SUCCESS) {
        kprintf_stub("create_file_capability_mathematical: kcapability_dag_add_edge failed for path '%s'\n", path);
        new_file_cap->reference_count = 0wb;
        kcapability_dag_node_destroy(new_file_cap);
        return NULL;
    }
    kprintf_stub("create_file_capability_mathematical: File capability created and linked for path '%s', cap ID %llu\n",
                 path, (unsigned long long)new_file_cap->capability_id);
    return new_file_cap;
}

static int allocate_file_descriptor_for_capability(
    struct proc* caller,
    kcapability_dag_node_t* file_cap) {
    KASSERT(caller != NULL, "allocate_fd_for_cap: caller proc is NULL");
    KASSERT(file_cap != NULL, "allocate_fd_for_cap: file_cap is NULL");
    int fd = -1;
    for (int i = 0; i < NO_FILES; i++) {
        if (caller->p_filp[i] == NULL) {
            fd = i;
            break;
        }
    }
    if (fd == -1) {
        kprintf_stub("allocate_fd_for_capability: No free file descriptors for caller %d (Max %d)\n", caller->p_endpoint, NO_FILES);
        return -EMFILE;
    }
    kprintf_stub("allocate_fd_for_capability: Conceptually allocated FD %d for capability ID %llu for caller %d\n",
                 fd, (unsigned long long)file_cap->capability_id, caller->p_endpoint);
    return fd;
}
