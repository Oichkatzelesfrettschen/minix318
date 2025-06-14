#ifndef _MINIX_KCAP_HELPERS_H
#define _MINIX_KCAP_HELPERS_H

#include <minix/kcapability_dag.h>  // For kcapability_dag_t, kcapability_dag_node_t
#include <minix/types.h>            // For endpoint_t, mode_t

// Forward declaration for struct proc if not fully included via other headers
struct proc;

/**
 * @brief Looks up the root capability node associated with a process endpoint.
 *
 * This function maps a process endpoint to its corresponding capability node
 * in the capability DAG. This node serves as the starting point for capability
 * checks related to this process.
 *
 * @param dag The capability DAG.
 * @param proc_ep The endpoint of the process.
 * @return Pointer to the process's root capability node, or NULL if not found
 *         or if the process does not have a capability context.
 */
kcapability_dag_node_t* kcapability_dag_lookup_process_capability(
    kcapability_dag_t* dag, endpoint_t proc_ep);

/**
 * @brief Validates if a process has the necessary directory traversal
 * capabilities for the given path.
 *
 * This function checks against the capability DAG to ensure that the process's
 * capabilities (starting from proc_cap) allow traversal of each component
 * of the specified path.
 *
 * @param dag The capability DAG.
 * @param proc_cap The root capability node for the calling process.
 * @param path The absolute file path to validate.
 * @return An integer status code (e.g., 0 for SUCCESS, or an error code like
 * EACCES).
 */
int validate_directory_traversal_mathematical(kcapability_dag_t* dag,
                                              kcapability_dag_node_t* proc_cap,
                                              const char* path);

/**
 * @brief Validates if a process has the necessary file access capabilities
 *        for the given path and access flags.
 *
 * This function checks if the process's capabilities (derived from proc_cap)
 * permit the requested type of access (e.g., read, write, execute) on the
 * final component of the path.
 *
 * @param dag The capability DAG.
 * @param proc_cap The root capability node for the calling process.
 * @param path The absolute file path to validate.
 * @param flags The access flags (e.g., O_RDONLY, O_WRONLY, O_RDWR).
 * @return An integer status code (e.g., 0 for SUCCESS, or an error code like
 * EACCES).
 */
int validate_file_access_mathematical(kcapability_dag_t* dag,
                                      kcapability_dag_node_t* proc_cap,
                                      const char* path,
                                      int flags  // Corresponds to open() flags
);

/**
 * @brief Creates a new capability node representing an opened file.
 *
 * This function is called after access validations have passed. It creates a
 * new node in the DAG for the opened file, deriving its properties (rights,
 * security level) from the parent process's capability and the requested access
 * mode. This new node is typically linked as a child of the process's
 * capability or a relevant directory capability.
 *
 * @param dag The capability DAG.
 * @param proc_cap The root capability node for the calling process.
 * @param path The absolute file path for which the capability is created.
 * @param flags The open flags used, which might influence derived rights.
 * @param mode The mode for file creation (if applicable), influencing initial
 * rights.
 * @return Pointer to the newly created file capability node, or NULL on
 * failure.
 */
kcapability_dag_node_t* create_file_capability_mathematical(
    kcapability_dag_t* dag, kcapability_dag_node_t* proc_cap, const char* path,
    int flags, mode_t mode);

/**
 * @brief Allocates a file descriptor in the process's file descriptor table
 *        and associates it with the given capability node.
 *
 * @param p Pointer to the process structure (`struct proc`).
 * @param cap_node Pointer to the file capability node.
 * @return The allocated file descriptor number, or a negative error code on
 * failure.
 */
int allocate_file_descriptor_for_capability(struct proc* p,
                                            kcapability_dag_node_t* cap_node);

// Define a dummy INVALID_CAPABILITY_ID for stub if not defined elsewhere
#ifndef INVALID_CAPABILITY_ID
#define INVALID_CAPABILITY_ID \
  ((kcapability_id_t)0)  // Or some other sentinel like ~0ULL
#endif

#endif /* _MINIX_KCAP_HELPERS_H */
