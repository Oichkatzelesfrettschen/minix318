/**
 * @file kcapability_dag.h
 * @brief Capability Directed Acyclic Graph implementation for MINIX kernel
 *
 * This module implements a Capability Directed Acyclic Graph data structure
 * optimized for kernel-space operations with C23 language enhancements.
 *
 * Mathematical Properties:
 * - Hierarchical capability representation
 * - Supports transitive privilege checks (implicitly)
 * - Designed for efficient lookup and modification
 *
 * Complexity Guarantees:
 * - Insert: O(V+E) in worst case (rebuilding lists), O(1) amortized if well-managed
 * - Lookup: O(depth) or O(V) depending on traversal strategy
 * - Delete: O(V+E) in worst case (rebuilding lists)
 *
 * @author MINIX C23 Development Team
 * @date 2025-06-08
 * @version 1.0
 */

#ifndef KCAPABILITY_DAG_H
#define KCAPABILITY_DAG_H

#include <minix/kernel_types.h> /* For k_atomic_t, size_t potentially, endpoint_t */
#include <stddef.h> /* For size_t if not in kernel_types.h */
#include <stdbool.h> /* For bool type */
/* TODO: Re-evaluate if a more specific header like kernel/klib.h is better */
/* Or if individual klib components should be included directly. */

// Define maximum length for a capability node's name component
#ifndef MAX_COMPONENT_NAME_LEN
#define MAX_COMPONENT_NAME_LEN 64
#endif

/* C23 standard headers for specific features */
#include <stdbit.h>    /* C23 bit manipulation functions */
/* #include <stdckdint.h> */ /* C23 checked integer arithmetic */

/**
 * @brief Represents a node in the Capability Directed Acyclic Graph.
 *
 * Each node typically represents a specific kernel object or resource and
 * the capabilities (permissions) it grants. The DAG structure allows
 * for hierarchical and inherited capabilities.
 */
typedef struct kcapability_dag_node {
    _BitInt(64) capability_id;         /**< Unique identifier for this capability node. */
    _BitInt(64) rights_mask;           /**< Bitmask of permissions/rights granted by this capability. */
    _BitInt(16) security_level;        /**< Security level associated with this capability. */
    _BitInt(32) generation;            /**< Generation count for ABA problem mitigation with IDs. */
    _BitInt(16) reference_count;       /**< Number of references to this node; aids in lifecycle management. */

    void *resource_ptr;              /**< Opaque pointer to the underlying kernel resource or object this capability pertains to. */

    struct kcapability_dag_node **parents;  /**< Dynamically allocated array of pointers to parent capability nodes. */
    _BitInt(16) parent_count;                /**< Current number of parent nodes. */
    _BitInt(16) parents_capacity;            /**< Allocated capacity of the parents array. */

    struct kcapability_dag_node **children; /**< Dynamically allocated array of pointers to child capability nodes. */
    _BitInt(16) child_count;                 /**< Current number of child nodes. */
    _BitInt(16) children_capacity;           /**< Allocated capacity of the children array. */

    /* Performance and validation cache fields */
    _BitInt(8) cached_validation_result; /**< Cached result of a previous validation (e.g., VALIDATION_UNKNOWN, VALID, INVALID). */
    _BitInt(64) cache_timestamp;          /**< Timestamp of when the cached_validation_result was stored. */

    /* Consider: __attribute__((aligned(64))) for cache line optimization if critical. */

    // Fields for identifying process-specific capability nodes
    _BitInt(1) is_process_main_cap_node; /**< Flag: True if this node represents a process's primary capability set. */
    endpoint_t owner_endpoint;         /**< If is_process_main_cap_node is true, this holds the process's endpoint. */

    char name[MAX_COMPONENT_NAME_LEN]; /**< Name of the component if this node represents a named entity like a file/directory. */
} kcapability_dag_node_t;

/**
 * @brief Represents the entire Capability Directed Acyclic Graph.
 *
 * This structure holds metadata for the DAG, such as the total number of nodes
 * and potentially pointers to root nodes or a central node store (details
 * depend on the chosen underlying storage and lookup mechanism for nodes).
 * It also includes performance statistics.
 */
typedef struct kcapability_dag {
    /* For managing all nodes in the DAG:
     * Option 1: A flat array or hash table (implementation detail not shown here).
     * Option 2: Pointers to root nodes (if the DAG has specific roots).
     * For this template, we'll keep it abstract. 'nodes' could represent
     * the primary access point to the node collection.
     */
    kcapability_dag_node_t* nodes;    /**< Pointer to an array of nodes, acting as the primary node store. */
    _BitInt(32) node_count;           /**< Current total number of active nodes in the DAG. */
    _BitInt(32) node_capacity;        /**< Current allocated capacity of the 'nodes' array. */
    _BitInt(32) edge_count;           /**< Current total number of edges in the DAG. */

    /* Optional: Function pointers for polymorphic behavior or strategy pattern. */
    bool (*validate_invariants)(const struct kcapability_dag* dag); /**< Pointer to a function that validates DAG invariants. */
    // int (*compare_nodes)(const kcapability_dag_node_t* a, const kcapability_dag_node_t* b);
    // void (*traverse_dag)(const struct kcapability_dag* dag, void (*visit_func)(kcapability_dag_node_t* node, void* context), void* context);

    /** Performance monitoring for DAG operations. */
    struct {
        _BitInt(64) operations_total;   /**< Total number of significant DAG operations (insert, delete, lookup). */
        _BitInt(64) cache_hits;         /**< Cache hits during node lookups (if applicable). */
        _BitInt(64) cache_misses;       /**< Cache misses during node lookups (if applicable). */
        _BitInt(64) traversals;         /**< Number of full or partial traversals performed. */
        _BitInt(64) successful_derivations; /**< Count of successful derivation validations. */
        _BitInt(64) failed_derivations;     /**< Count of failed derivation validations. */
    } performance_stats;

} kcapability_dag_t;

/* Function declarations */

/**
 * @brief Creates and initializes an empty Capability Directed Acyclic Graph (DAG).
 * @param initial_node_pool_capacity Hint for the initial capacity of any internally managed node storage.
 *                                   Actual allocation strategy is implementation-defined.
 * @return Pointer to the newly created kcapability_dag_t, or NULL on failure (e.g., memory allocation error).
 */
kcapability_dag_t* kcapability_dag_create(size_t initial_node_pool_capacity);

/**
 * @brief Creates a new, isolated capability DAG node.
 *
 * The node is initialized with the given parameters. Its generation is typically
 * set to an initial value (e.g., 0 or 1), and its reference_count might be
 * initialized to 1 (if the creator holds a reference) or 0.
 * The children array is typically initialized to NULL with zero capacity/count.
 *
 * @param id The unique identifier for the new node.
 * @param granted_permissions A bitmask of permissions granted by this capability node.
 * @param resource_ptr Pointer to the underlying kernel resource this capability governs.
 * @param name_param The name for this capability node (e.g. filename). Can be NULL.
 * @return Pointer to the newly created kcapability_dag_node_t, or NULL on failure.
 */
kcapability_dag_node_t* kcapability_dag_node_create(_BitInt(64) capability_id, _BitInt(64) rights_mask, _BitInt(16) security_level, const char* name_param);

/**
 * @brief Adds a pre-created node to the DAG's managed set.
 *
 * This function typically involves adding the node to a lookup structure
 * within the DAG (e.g., a hash table managed via `dag->node_store_handle`).
 * It does not establish edges; use `kcapability_dag_add_edge` for that.
 *
 * @param dag Pointer to the kcapability_dag_t structure.
 * @param node Pointer to the kcapability_dag_node_t to insert.
 * @return 0 on success, or an error code (e.g., -EINVAL if node is NULL,
 *         -EEXIST if a node with the same ID already exists, -ENOMEM if internal reallocation fails).
 */
int kcapability_dag_insert_node(kcapability_dag_t* dag, kcapability_dag_node_t* node);

/**
 * @brief Creates a directed edge from a parent node to a child node in the DAG.
 *
 * This function modifies the parent_node's children list to include child_node.
 * It may involve reallocating the children array in parent_node.
 * Implementations should typically check for:
 * - NULL pointers for dag, parent_node, or child_node.
 * - Whether parent_node and child_node are already part of the DAG (managed by node_store_handle).
 * - Potential cycles if strict DAG properties must be maintained dynamically (can be complex).
 *   Alternatively, cycle detection can be part of `kcapability_dag_validate_invariants`.
 *
 * @param dag Pointer to the kcapability_dag_t structure (used for context, e.g., memory allocators).
 * @param parent_node Pointer to the parent kcapability_dag_node_t.
 * @param child_node Pointer to the child kcapability_dag_node_t.
 * @return 0 on success, or an error code (e.g., -EINVAL, -ENOMEM, -ELOOP if cycle detected and disallowed).
 */
int kcapability_dag_add_edge(kcapability_dag_t* dag, kcapability_dag_node_t* parent_node, kcapability_dag_node_t* child_node);

/**
 * @brief Looks up a node by its ID in the capability DAG.
 * @param dag Pointer to the kcapability_dag_t structure.
 * @param id The ID of the node to find.
 * @return Pointer to the found kcapability_dag_node_t, or NULL if not found or if `dag` is NULL.
 */
kcapability_dag_node_t* kcapability_dag_lookup_node(const kcapability_dag_t* dag, _BitInt(64) id);

/**
 * @brief Removes a node and all its associated edges from the DAG.
 *
 * This involves:
 * 1. Removing all edges pointing from this node to its children.
 * 2. Removing all edges pointing from any parent to this node.
 * 3. Removing the node itself from the DAG's central store.
 * The node's memory itself is not freed here unless its reference count drops to zero
 * (if ref counting is the primary lifecycle mechanism). See `kcapability_dag_node_destroy`.
 *
 * @param dag Pointer to the kcapability_dag_t structure.
 * @param id The ID of the node to delete.
 * @return 0 on success, or an error code (e.g., -ENOENT if node not found).
 */
int kcapability_dag_delete_node(kcapability_dag_t* dag, _BitInt(64) id);

/**
 * @brief Removes a directed edge from a parent node to a child node.
 * @param dag Pointer to the kcapability_dag_t structure (for context).
 * @param parent_node Pointer to the parent kcapability_dag_node_t.
 * @param child_node Pointer to the child kcapability_dag_node_t whose edge from parent_node should be removed.
 * @return 0 on success, or an error code (e.g., -ENOENT if the edge doesn't exist).
 */
int kcapability_dag_remove_edge(kcapability_dag_t* dag, kcapability_dag_node_t* parent_node, kcapability_dag_node_t* child_node);

/**
 * @brief Validates the internal invariants of the capability DAG.
 *
 * This can include checks for:
 * - Structural integrity (e.g., valid pointers).
 * - Absence of cycles (essential for a DAG).
 * - Consistent reference counts (if applicable).
 * - Any other domain-specific rules for capabilities.
 *
 * @param dag Pointer to the kcapability_dag_t structure.
 * @return True if all invariants hold, false otherwise.
 */
bool kcapability_dag_validate_invariants(const kcapability_dag_t* dag);

/**
 * @brief Destroys a capability DAG structure, freeing all associated resources.
 *
 * This includes freeing all nodes managed by the DAG (if the DAG has ownership,
 * potentially by calling `kcapability_dag_node_destroy` on each) and the DAG structure itself.
 *
 * @param dag Pointer to the kcapability_dag_t to destroy.
 */
void kcapability_dag_destroy(kcapability_dag_t* dag);

/**
 * @brief Destroys an individual capability DAG node, freeing its resources.
 *
 * This function is responsible for freeing memory allocated for the node itself
 * and any members it owns (e.g., the `children` array).
 * It might be called by `kcapability_dag_destroy` for all nodes in a DAG,
 * or independently if nodes are managed with reference counting and a node's
 * count drops to zero.
 *
 * @param node Pointer to the kcapability_dag_node_t to destroy.
 */
void kcapability_dag_node_destroy(kcapability_dag_node_t* node);

#endif /* KCAPABILITY_DAG_H */
