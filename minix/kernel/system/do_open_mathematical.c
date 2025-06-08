/**
 * @file do_open_mathematical.c
 * @brief Implements the mathematical capability-based open system call.
 *
 * This file contains the logic for the `do_open_mathematical` syscall handler,
 * which uses a capability DAG to enforce access control for opening files.
 * It includes stubs for various helper functions required for this process.
 */

#include "kernel/kernel.h"  // For struct proc, VFS_PROC_NR etc. (adjust as per actual kernel headers)
#include "kernel/system.h"  // For system call infrastructure if any particular macros are needed.
    // May need more specific headers like "kernel/proc.h"
#include <errno.h>      // For EPERM, EACCES, ENOMEM etc.
#include <klib.h>       // For kprintf_stub, KASSERT
#include <minix/com.h>  // For message types (e.g. m_lc_vfs_open) and VFS_PROC_NR
#include <minix/ipc.h>           // For message structure definitions
#include <minix/kcap_helpers.h>  // For declarations of these helper functions
#include <minix/kcapability_dag.h>  // For DAG structures and functions
#include <minix/vfsif.h>  // For VFS_OPEN message structure (usually defines m_lc_vfs_open)
#include <sys/kassert.h>  // For KASSERT

// Assume kernel_capability_dag is a global or accessible pointer to the main
// DAG This would typically be initialized in kmain or similar.
extern kcapability_dag_t*
    kernel_capability_dag;  // Ensure this is declared/defined elsewhere (e.g.
                            // kmain or klib itself)

// --- Stub Implementations for Helper Functions ---

/**
 * @brief (STUB) Looks up the root capability node associated with a process
 * endpoint.
 * @param dag The capability DAG.
 * @param proc_ep The endpoint of the process.
 * @return Pointer to the process's root capability node, or NULL.
 */
kcapability_dag_node_t* kcapability_dag_lookup_process_capability(
    kcapability_dag_t* dag, endpoint_t proc_ep) {
  KASSERT(dag != NULL);
  kprintf_stub(
      "kcapability_dag_lookup_process_capability: STUB for proc_ep %d\n",
      proc_ep);
  // For testing, we might return a dummy node or the first node if any.
  // if (dag->num_nodes > 0) return &dag->nodes[0];
  return NULL;  // Default stub behavior: process has no capabilities yet
}

/**
 * @brief (STUB) Validates directory traversal capabilities for a path.
 * @param dag The capability DAG.
 * @param proc_cap The root capability node for the calling process.
 * @param path The file path.
 * @return 0 (KCAP_SUCCESS) if allowed, or an error code.
 */
int validate_directory_traversal_mathematical(kcapability_dag_t* dag,
                                              kcapability_dag_node_t* proc_cap,
                                              const char* path) {
  KASSERT(dag != NULL);
  // KASSERT(proc_cap != NULL); // proc_cap can be NULL if lookup fails
  KASSERT(path != NULL);
  kprintf_stub(
      "validate_directory_traversal_mathematical: STUB for path '%s', "
      "proc_cap_id %llu\n",
      path, proc_cap ? proc_cap->id : INVALID_CAPABILITY_ID);
  return KCAP_SUCCESS;  // Assume success for stub
}

/**
 * @brief (STUB) Validates file access capabilities for a path and flags.
 * @param dag The capability DAG.
 * @param proc_cap The root capability node for the calling process.
 * @param path The file path.
 * @param flags Open flags.
 * @return 0 (KCAP_SUCCESS) if allowed, or an error code.
 */
int validate_file_access_mathematical(kcapability_dag_t* dag,
                                      kcapability_dag_node_t* proc_cap,
                                      const char* path, int flags) {
  KASSERT(dag != NULL);
  // KASSERT(proc_cap != NULL);
  KASSERT(path != NULL);
  kprintf_stub(
      "validate_file_access_mathematical: STUB for path '%s', flags %d, "
      "proc_cap_id %llu\n",
      path, flags, proc_cap ? proc_cap->id : INVALID_CAPABILITY_ID);
  return KCAP_SUCCESS;  // Assume success for stub
}

// Static counter for dummy capability IDs for file nodes
static kcapability_id_t next_file_cap_id =
    0x10000000F11E0001ULL;  // Arbitrary starting point

/**
 * @brief (STUB) Creates a new capability node for an opened file.
 * @param dag The capability DAG.
 * @param proc_cap The root capability node for the calling process.
 * @param path The file path.
 * @param flags Open flags.
 * @param mode File mode.
 * @return Pointer to the new file capability node, or NULL.
 */
kcapability_dag_node_t* create_file_capability_mathematical(
    kcapability_dag_t* dag, kcapability_dag_node_t* proc_cap, const char* path,
    int flags, mode_t mode) {
  KASSERT(dag != NULL);
  // proc_cap can be NULL.
  KASSERT(path != NULL);
  kprintf_stub(
      "create_file_capability_mathematical: STUB for path '%s', flags %d, mode "
      "%o, proc_cap_id %llu\n",
      path, flags, mode, proc_cap ? proc_cap->id : INVALID_CAPABILITY_ID);

  // Create a dummy node
  krights_mask_t file_rights =
      0x1FF;  // RWRWRW (owner, group, other full) - simplified
  ksecurity_level_t sec_level =
      proc_cap ? proc_cap->security_level : 0;  // Inherit or default

  kcapability_dag_node_t* new_file_node = kcapability_dag_node_create(
      dag, next_file_cap_id++, file_rights, sec_level);
  if (!new_file_node) {
    kprintf_stub(
        "create_file_capability_mathematical: STUB kcapability_dag_node_create "
        "failed\n");
    return NULL;
  }

  if (proc_cap) {  // If there's a process capability, try to link it
    kcapability_dag_status_t edge_status =
        kcapability_dag_add_edge_unsafe(dag, proc_cap, new_file_node);
    if (edge_status != KCAP_SUCCESS) {
      kprintf_stub(
          "create_file_capability_mathematical: STUB "
          "kcapability_dag_add_edge_unsafe failed (%d)\n",
          edge_status);
      // If edge fails, we might want to destroy the created node if it's not
      // automatically cleaned up For now, a real destroy is complex. We'll just
      // "leak" it in the DAG in this stub. kcapability_dag_node_destroy(dag,
      // new_file_node); // This would be ideal
      return NULL;
    }
  }
  kprintf_stub(
      "create_file_capability_mathematical: STUB created node ID %llu\n",
      new_file_node->id);
  return new_file_node;
}

/**
 * @brief (STUB) Allocates a file descriptor and associates it with a capability
 * node.
 * @param p Pointer to the process structure.
 * @param cap_node Pointer to the file capability node.
 * @return A dummy file descriptor (e.g., 3) or a negative error code.
 */
int allocate_file_descriptor_for_capability(struct proc* p,
                                            kcapability_dag_node_t* cap_node) {
  KASSERT(p != NULL);
  KASSERT(cap_node != NULL);  // Should have a valid capability node here
  kprintf_stub(
      "allocate_file_descriptor_for_capability: STUB for proc %d, cap_id "
      "%llu\n",
      p->p_endpoint, cap_node->id);
  return 3;  // Return a dummy fd (0,1,2 usually stdin,out,err)
}

// --- Main do_open_mathematical Implementation ---

/**
 * @brief Handles the mathematical capability-based open system call.
 *
 * This function is invoked when a process attempts to open a file. It uses the
 * capability DAG to perform access control checks based on the process's
 * capabilities and the requested file operation.
 *
 * @param caller Pointer to the calling process's proc structure.
 * @param m_ptr Pointer to the IPC message containing open parameters.
 * @return File descriptor on success, or a negative error code on failure.
 */
int do_open_mathematical(struct proc* caller, message* m_ptr) {
  KASSERT(caller != NULL);
  KASSERT(m_ptr != NULL);
  KASSERT(kernel_capability_dag != NULL);  // DAG should be initialized

  // 1. Extract parameters from the message
  //    The exact field names depend on how VFS_OPEN messages are structured.
  //    Commonly found in <minix/vfsif.h> or similar.
  //    Assuming m_lc_vfs_open structure based on typical MINIX IPC for VFS.

  char* pathname =
      (char*)m_ptr->m_lc_vfs_open
          .name;  // Pathname is usually passed by address in message
  int flags = m_ptr->m_lc_vfs_open.flags;
  mode_t mode =
      (mode_t)
          m_ptr->m_lc_vfs_open.mode;  // mode_t might be int or short in message
  // k_size_t path_len = m_ptr->m_lc_vfs_open.path_len; // Often length is also
  // passed

  // KASSERT(pathname != NULL); // Pathname validity should be checked
  // carefully. A userspace pointer needs proper handling (e.g.
  // fetch_user_string) if not already in kernel space. For now, assume pathname
  // is accessible. If it's a user pointer, this is a simplification. If the
  // path is embedded in the message or copied by generic IPC layer, it's fine.
  // Let's assume it's directly usable for the stub.
  if (pathname == NULL) return EINVAL;  // Basic check

  kprintf_stub(
      "do_open_mathematical: Caller %d, Path '%s', Flags %x, Mode %03o\n",
      caller->p_endpoint, pathname, flags, mode);

  // 2. Get process's capability node
  kcapability_dag_node_t* process_capability =
      kcapability_dag_lookup_process_capability(kernel_capability_dag,
                                                caller->p_endpoint);

  if (process_capability == NULL) {
    kprintf_stub("do_open_mathematical: No capability node for caller %d\n",
                 caller->p_endpoint);
    return EPERM;  // No capability context for this process
  }

  // 3. Validate directory traversal
  int traversal_status = validate_directory_traversal_mathematical(
      kernel_capability_dag, process_capability, pathname);
  if (traversal_status != KCAP_SUCCESS) {
    kprintf_stub(
        "do_open_mathematical: Directory traversal failed for %s (status %d)\n",
        pathname, traversal_status);
    return EACCES;
  }

  // 4. Validate file access
  int access_status = validate_file_access_mathematical(
      kernel_capability_dag, process_capability, pathname, flags);
  if (access_status != KCAP_SUCCESS) {
    kprintf_stub(
        "do_open_mathematical: File access validation failed for %s (status "
        "%d)\n",
        pathname, access_status);
    return EACCES;
  }

  // 5. Create file capability node
  kcapability_dag_node_t* file_capability = create_file_capability_mathematical(
      kernel_capability_dag, process_capability, pathname, flags, mode);

  if (file_capability == NULL) {
    kprintf_stub(
        "do_open_mathematical: Failed to create file capability for %s\n",
        pathname);
    return ENOMEM;  // Or another error like ENFILE if DAG is full
  }

  // 6. Allocate file descriptor
  int fd = allocate_file_descriptor_for_capability(caller, file_capability);
  if (fd < 0) {  // Assuming negative value indicates error
    kprintf_stub(
        "do_open_mathematical: Failed to allocate FD for %s (err %d)\n",
        pathname, fd);
    // Attempt to clean up the created file capability node
    // kcapability_dag_node_destroy(kernel_capability_dag, file_capability); //
    // Ideal For stub, this might be complex. The node might be "leaked" in the
    // DAG for now.
    return fd;  // Return the error from allocate_fd
  }

  kprintf_stub(
      "do_open_mathematical: Successfully opened '%s', fd %d, cap_id %llu\n",
      pathname, fd, file_capability->id);
  return fd;  // Success
}
