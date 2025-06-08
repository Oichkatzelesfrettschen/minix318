/**
 * @file kcapability_dag.c
 * @brief Implementation of Capability Directed Acyclic Graph for MINIX kernel
 *
 * This module implements the mathematical foundation for capability-based
 * security through directed acyclic graph operations. Every function here
 * represents a mathematical operation on graph structures, with runtime
 * verification of mathematical invariants.
 */

#include "../include/klib.h" // For KASSERT, kmalloc, kfree, kpanic
#include <minix/kcapability_dag.h> // For kcapability_dag_t, kcapability_dag_node_t, etc. (from minix/kernel/include/minix/)
#include <stdbit.h>   // For C23 _BitInt support
#include <stdckdint.h> // For C23 checked integer arithmetic
#include <stddef.h> // For size_t
#include <stdbool.h> // For bool type

// Forward declaration for the validation function pointer
static bool kcapability_dag_validate_invariants_impl(const kcapability_dag_t* dag);

// Forward declarations for static helper functions
static bool kcapability_dag_would_create_cycle(kcapability_dag_t* dag, kcapability_dag_node_t* parent, kcapability_dag_node_t* child);
static int kcapability_dag_add_edge_unsafe(kcapability_dag_t* dag, kcapability_dag_node_t* parent_node, kcapability_dag_node_t* child_node);
static size_t kcapability_dag_get_node_index(const kcapability_dag_t* dag, const kcapability_dag_node_t* node);
static bool kcapability_dag_dfs_path_exists(const kcapability_dag_t* dag, kcapability_dag_node_t* current, kcapability_dag_node_t* target, bool* visited);
static bool kcapability_dag_dfs_validate_path(const kcapability_dag_t* dag, kcapability_dag_node_t* current, kcapability_dag_node_t* target, bool* visited);


// Define MAX_CAPABILITY_NODES if not defined elsewhere (e.g., in kcapability_dag.h or a config file)
// This is a placeholder value; a real system would have this configured.
#ifndef MAX_CAPABILITY_NODES
#define MAX_CAPABILITY_NODES 1024
#endif

// Define helper status codes if not defined elsewhere
// These should ideally be in kcapability_dag.h or a general klib_status.h
#ifndef SUCCESS
#define SUCCESS 0
#endif
#ifndef CAPABILITY_RIGHTS_VIOLATION
#define CAPABILITY_RIGHTS_VIOLATION -1 // Example error code
#endif
#ifndef CAPABILITY_SECURITY_VIOLATION
#define CAPABILITY_SECURITY_VIOLATION -2 // Example error code
#endif
#ifndef CAPABILITY_CYCLE_DETECTED
#define CAPABILITY_CYCLE_DETECTED -3 // Example error code
#endif
#ifndef ERROR_OUT_OF_MEMORY
#define ERROR_OUT_OF_MEMORY -4 // Example error code
#endif
#ifndef INITIAL_CHILD_PARENT_CAPACITY
#define INITIAL_CHILD_PARENT_CAPACITY 4 // Initial capacity for children/parents arrays
#endif
// #define ERROR_OUT_OF_MEMORY -4 // Defined in kcapability_dag_add_edge_unsafe if needed

/**
 * @brief Create a new capability DAG with specified initial capacity
 *
 * This function creates the mathematical foundation for capability management.
 * Think of this like establishing the axioms for a mathematical system -
 * everything else builds upon this foundation.
 */
kcapability_dag_t* kcapability_dag_create(size_t initial_capacity) {
    /* Validate input parameters using kernel assertions */
    KASSERT(initial_capacity > 0, "kcapability_dag_create: initial_capacity must be > 0");
    KASSERT(initial_capacity <= MAX_CAPABILITY_NODES, "kcapability_dag_create: initial_capacity exceeds MAX_CAPABILITY_NODES");

    /* Allocate memory for the DAG structure itself */
    kcapability_dag_t* dag = (kcapability_dag_t*)kmalloc(sizeof(kcapability_dag_t));
    if (!dag) {
        // In a real kernel, kpanic might not return.
        // If it could, we might return NULL, but the user's spec implies panic.
        kpanic("kcapability_dag_create: failed to allocate DAG structure");
        return NULL; // Should be unreachable if kpanic halts
    }

    /*
     * Initialize the mathematical structure. Notice how we use C23's
     * exact-width integers to ensure precise mathematical properties.
     * The user's spec uses `dag->nodes` to store an array of nodes.
     */
    dag->nodes = (kcapability_dag_node_t*)kmalloc(initial_capacity * sizeof(kcapability_dag_node_t));
    if (!dag->nodes) {
        kfree(dag);
        kpanic("kcapability_dag_create: failed to allocate node array");
        return NULL; // Should be unreachable
    }

    /* Initialize all fields using C23 precise integer types */
    dag->node_count = 0wb;        /* C23 _BitInt literal syntax */
    dag->node_capacity = (_BitInt(32))initial_capacity;
    dag->edge_count = 0wb;

    /* Initialize performance tracking for mathematical analysis */
    dag->performance_stats.operations_total = 0wb;
    dag->performance_stats.cache_hits = 0wb;
    dag->performance_stats.cache_misses = 0wb;
    dag->performance_stats.traversals = 0wb;
    dag->performance_stats.successful_derivations = 0wb;
    dag->performance_stats.failed_derivations = 0wb;


    /*
     * Initialize the mathematical validation function pointer.
     * This enables runtime verification of DAG mathematical properties.
     */
    dag->validate_invariants = kcapability_dag_validate_invariants_impl;

    return dag;
}

/**
 * @brief Validates mathematical invariants of the DAG.
 *
 * This function checks fundamental mathematical properties that must always
 * hold for a consistent DAG structure. For example, node_count should not
 * exceed node_capacity. More sophisticated checks (e.g., full acyclicity check,
 * consistency of parent/child counts) can be added here.
 * This function is crucial for runtime verification of the DAG's integrity.
 *
 * @param dag Pointer to the capability DAG to validate.
 * @return True if all checked invariants hold, false otherwise.
 *
 * @pre dag must not be NULL.
 */
static bool kcapability_dag_validate_invariants_impl(const kcapability_dag_t* dag) {
    KASSERT(dag != NULL, "validate_invariants_impl: dag pointer is NULL");

    // Basic invariant: node_count must not exceed node_capacity.
    KASSERT(dag->node_count <= dag->node_capacity, "validate_invariants_impl: node_count exceeds node_capacity");

    // Basic invariant: if node_capacity is 0, nodes pointer should ideally be NULL,
    // and node_count must be 0. If node_capacity > 0, nodes should not be NULL.
    if (dag->node_capacity == 0) {
        KASSERT(dag->nodes == NULL, "validate_invariants_impl: nodes pointer should be NULL for 0 capacity");
        KASSERT(dag->node_count == 0, "validate_invariants_impl: node_count should be 0 for 0 capacity");
    } else {
        KASSERT(dag->nodes != NULL, "validate_invariants_impl: nodes pointer is NULL for non-zero capacity");
    }

    // Basic invariant: edge_count should not be negative (it's unsigned _BitInt, so check against large values if it wraps)
    // For _BitInt, it won't be negative. A check for consistency (e.g. sum of all child_count == edge_count) is more advanced.
    // KASSERT(dag->edge_count >= 0wb); // Implicit for unsigned _BitInt

    // TODO: Add more sophisticated checks:
    // 1. Full acyclicity check of the entire DAG (can be expensive).
    // 2. Consistency of node_count with actual nodes in the store.
    // 3. Consistency of edge_count with sum of parent_node->child_count.
    // 4. Validate all node indices if a flat array model is strictly used.
    // 5. Check integrity of individual nodes (e.g., parent/child pointers valid).

    return true; // For now, basic checks pass.
}


// Define helper constants if not defined elsewhere
// These should ideally be in kcapability_dag.h or a config file.
#ifndef INVALID_CAPABILITY_ID
#define INVALID_CAPABILITY_ID 0wb // Assuming 0 is an invalid ID for _BitInt(64)
#endif
#ifndef MIN_SECURITY_LEVEL
#define MIN_SECURITY_LEVEL 0wb
#endif
#ifndef MAX_SECURITY_LEVEL
#define MAX_SECURITY_LEVEL 255wb // Example for _BitInt(16)
#endif
#ifndef VALIDATION_UNKNOWN // For cached_validation_result
#define VALIDATION_UNKNOWN 0 // Assuming an enum or defined constants
#endif

// Forward declaration for get_kernel_timestamp (assuming it exists or will be stubbed)
// uint64_t get_kernel_timestamp(void); // Or appropriate type

/**
 * @brief Create a new capability node with mathematical validation.
 *
 * This function creates a capability node that represents a mathematical
 * entity in your capability space. Each node embodies specific mathematical
 * properties about permissions and relationships. The node is initialized
 * with no parent or child relationships.
 *
 * @param capability_id The unique identifier for the new capability node.
 *                      Must not be INVALID_CAPABILITY_ID.
 * @param rights_mask A bitmask representing the permissions this capability grants.
 *                    Must not be zero.
 * @param security_level The security level associated with this capability.
 *                       Must be within [MIN_SECURITY_LEVEL, MAX_SECURITY_LEVEL].
 * @return Pointer to the newly created kcapability_dag_node_t, or triggers a
 *         kernel panic if allocation fails.
 *
 * @pre capability_id != INVALID_CAPABILITY_ID
 * @pre rights_mask != 0
 * @pre security_level >= MIN_SECURITY_LEVEL && security_level <= MAX_SECURITY_LEVEL
 * @post A valid kcapability_dag_node_t is returned with all mathematical properties initialized.
 * @post node->parent_count == 0 && node->child_count == 0
 * @post node->parents == NULL && node->children == NULL
 * @post node->generation == 1
 */
kcapability_dag_node_t* kcapability_dag_node_create(_BitInt(64) capability_id,
                                                   _BitInt(64) rights_mask,
                                                   _BitInt(16) security_level) {
    /* Validate mathematical preconditions - Axiom Check for Node Properties */
    KASSERT(capability_id != INVALID_CAPABILITY_ID, "kcapability_dag_node_create: capability_id is invalid");
    KASSERT(rights_mask != 0wb, "kcapability_dag_node_create: rights_mask must not be empty");
    KASSERT(security_level >= MIN_SECURITY_LEVEL, "kcapability_dag_node_create: security_level is below minimum");
    KASSERT(security_level <= MAX_SECURITY_LEVEL, "kcapability_dag_node_create: security_level is above maximum");

    /* Allocate memory for the node structure - Creating a Mathematical Entity */
    kcapability_dag_node_t* node = (kcapability_dag_node_t*)kmalloc(sizeof(kcapability_dag_node_t));
    if (!node) {
        kpanic("kcapability_dag_node_create: allocation failed");
        // kpanic typically does not return
        return NULL;
    }

    /* Initialize the mathematical properties of this capability */
    node->capability_id = capability_id;
    node->rights_mask = rights_mask;
    node->security_level = security_level;
    node->generation = 1wb;  /* Start with generation 1 for ABA protection */

    /*
     * Initialize graph structure pointers. These represent the mathematical
     * relationships between this capability and others in the system.
     * A newly created node has no relationships yet.
     */
    node->parent_count = 0wb;
    node->child_count = 0wb;
    node->parents = NULL;  // Dynamically allocated when edges are added
    node->children = NULL; // Dynamically allocated when edges are added
    node->parents_capacity = 0wb; // Initialize capacity
    node->children_capacity = 0wb; // Initialize capacity


    /* Initialize performance optimization structures */
    node->cached_validation_result = VALIDATION_UNKNOWN;
    // node->cache_timestamp = get_kernel_timestamp(); // Assuming get_kernel_timestamp() exists
    node->cache_timestamp = 0wb; // Placeholder if get_kernel_timestamp() is not yet available

    // resource_ptr is passed to kcapability_dag_node_create in the header, but not in this C code.
    // The header was: kcapability_dag_node_create(_BitInt(64) capability_id, _BitInt(64) rights_mask, _BitInt(16) security_level);
    // The original template had: kcapability_dag_node_create(_BitInt(64) id, _BitInt(32) granted_permissions, void *resource_ptr);
    // The C code here does not take resource_ptr. Let's assume it's set separately or not needed for this version.
    // For consistency with the current header, I will assume resource_ptr is not set here.
    // If resource_ptr was part of the function signature for this create call, it would be:
    // node->resource_ptr = resource_ptr;
    // However, the feedback's C code for this function does not include resource_ptr as a parameter.
    // The header was changed to match this C code's parameters.
    // The kcapability_dag_node_t struct *does* have resource_ptr. It should be initialized.
    node->resource_ptr = NULL; // Initialize to NULL as it's not passed in this version of create.


    return node;
}


/**
 * @brief Add an edge between two capability nodes with mathematical verification.
 *
 * This function implements the mathematical operation of edge addition in
 * a directed acyclic graph, representing a derivation or dependency.
 * The edge parent_node -> child_node is added.
 * Mathematical constraints such as rights subset, security level ordering,
 * and acyclicity are enforced.
 *
 * @param dag Pointer to the capability DAG.
 * @param parent_node Pointer to the parent capability node.
 * @param child_node Pointer to the child capability node.
 * @return SUCCESS (0) if the edge was added successfully.
 * @return CAPABILITY_RIGHTS_VIOLATION if the child's rights are not a subset of the parent's.
 * @return CAPABILITY_SECURITY_VIOLATION if the child's security level is less than the parent's.
 * @return CAPABILITY_CYCLE_DETECTED if adding this edge would create a cycle.
 * @return Other negative error codes on internal failure (e.g., memory allocation).
 *
 * @pre dag, parent_node, and child_node must not be NULL.
 * @pre parent_node and child_node must be different nodes.
 * @post If successful, an edge from parent_node to child_node is added to the DAG.
 * @post dag->edge_count is incremented.
 * @post dag->performance_stats.operations_total is incremented.
 * @post The DAG invariants hold true.
 */
int kcapability_dag_add_edge(kcapability_dag_t* dag,
                            kcapability_dag_node_t* parent_node,
                            kcapability_dag_node_t* child_node) {
    /* Validate mathematical preconditions - Axiom Check for Edge Properties */
    KASSERT(dag != NULL, "kcapability_dag_add_edge: dag is NULL");
    KASSERT(parent_node != NULL, "kcapability_dag_add_edge: parent_node is NULL");
    KASSERT(child_node != NULL, "kcapability_dag_add_edge: child_node is NULL");
    KASSERT(parent_node != child_node, "kcapability_dag_add_edge: parent_node and child_node cannot be the same (self-edges would create cycles)");

    /*
     * Mathematical constraint verification: The child's rights must be
     * a subset of the parent's rights. This enforces the capability
     * monotonicity theorem at runtime.
     * (parent_rights | child_rights) == parent_rights  <=> child_rights is subset of parent_rights
     */
    if ((child_node->rights_mask & parent_node->rights_mask) != child_node->rights_mask) {
        return CAPABILITY_RIGHTS_VIOLATION;
    }

    /*
     * Mathematical constraint verification: The child's security level
     * must be greater than or equal to the parent's level. This enforces
     * the security lattice ordering relationship.
     */
    if (child_node->security_level < parent_node->security_level) {
        return CAPABILITY_SECURITY_VIOLATION;
    }

    /*
     * Before adding the edge, we must verify that it won't create a cycle.
     * This is the fundamental mathematical constraint that makes this a DAG
     * rather than just a directed graph. This is a proof of acyclicity.
     */
    if (kcapability_dag_would_create_cycle(dag, parent_node, child_node)) {
        return CAPABILITY_CYCLE_DETECTED;
    }

    /*
     * Now we can safely add the edge. This involves updating the mathematical
     * relationship structures in both nodes. This is the constructive step.
     */
    int result = kcapability_dag_add_edge_unsafe(dag, parent_node, child_node);
    if (result != SUCCESS) {
        return result; // Propagate error from unsafe helper (e.g., out of memory)
    }

    /* Update DAG-level statistics for mathematical analysis */
    dag->edge_count++; // Assumes edge_count is _BitInt, direct ++ might need casting or helper if not.
                       // For _BitInt, direct increment is fine.
    dag->performance_stats.operations_total++;

    /* Verify that our mathematical invariants still hold - Post-condition Check */
    if (!dag->validate_invariants(dag)) {
        // This signifies a critical internal error or flaw in logic if invariants fail.
        kpanic("DAG mathematical invariants violated after edge addition");
    }

    return SUCCESS;
}


// Forward declaration for the new DFS helper, if not already present from previous steps
// (kcapability_dag_dfs_path_exists was for cycle detection, this one is for validation path)
// Based on user feedback, this is a new DFS specifically for validation.
// static bool kcapability_dag_dfs_validate_path(const kcapability_dag_t* dag, kcapability_dag_node_t* current, kcapability_dag_node_t* target, bool* visited);
// This forward declaration is now added above with other static helpers.

/**
 * @brief Validate capability derivation path with mathematical proof construction.
 *
 * This function constructs a mathematical proof that one capability (target_capability)
 * can be validly derived from another (source_capability) through the
 * relationships encoded in the DAG. This involves checking for a valid path
 * and ensuring that all intermediate capability derivation rules (e.g., rights
 * subset, security level monotonicity) are maintained along that path.
 *
 * @param dag Pointer to the capability DAG.
 * @param source_capability The starting capability node in the potential derivation chain.
 * @param target_capability The target capability node whose derivation is being validated.
 * @return True if a valid derivation path exists, false otherwise.
 *
 * @pre dag, source_capability, and target_capability must not be NULL.
 * @pre The DAG's mathematical invariants must hold.
 * @post Performance statistics (successful_derivations/failed_derivations) are updated.
 */
bool kcapability_dag_validate_derivation(kcapability_dag_t* dag,
                                        kcapability_dag_node_t* source_capability,
                                        kcapability_dag_node_t* target_capability) {
    KASSERT(dag != NULL, "kcapability_dag_validate_derivation: dag is NULL");
    KASSERT(source_capability != NULL, "kcapability_dag_validate_derivation: source_capability is NULL");
    KASSERT(target_capability != NULL, "kcapability_dag_validate_derivation: target_capability is NULL");

    // Ensure DAG invariants hold before complex operations
    KASSERT(dag->validate_invariants(dag), "kcapability_dag_validate_derivation: DAG invariants failed");

    /*
     * Mathematical proof construction: Use depth-first search to construct
     * a derivation path from source to target. If such a path exists and
     * all intermediate steps preserve the mathematical constraints of
     * capability theory (handled by kcapability_dag_dfs_validate_path),
     * then the derivation is mathematically valid.
     */

    /* Allocate visited tracking for proof construction.
     * kcalloc initializes to false (0).
     */
    if (dag->node_capacity == 0 && source_capability != target_capability) {
        // No capacity for nodes, so no path can exist unless source IS target.
        // If source == target, it's a valid zero-length derivation.
        if (source_capability == target_capability) {
             dag->performance_stats.successful_derivations++;
             return true;
        }
        dag->performance_stats.failed_derivations++;
        return false;
    }
    if (dag->node_capacity == 0 && source_capability == target_capability) {
        // This case is covered above.
        dag->performance_stats.successful_derivations++;
        return true;
    }
    // If source_capability == target_capability and node_capacity > 0, dfs_validate_path will handle it.


    bool* visited = (bool*)kcalloc((size_t)dag->node_capacity, sizeof(bool));
    if (!visited && dag->node_capacity > 0) { // Check if kcalloc failed for non-zero capacity
        kpanic("kcapability_dag_validate_derivation: failed to allocate visited array");
        // Defensive return, kpanic might not return
        return false;
    }
    // If dag->node_capacity is 0, visited will be NULL from kcalloc(0, size).
    // dfs_validate_path must handle a NULL visited array if node_capacity is 0.
    // However, kcapability_dag_dfs_validate_path will likely use get_node_index, which assumes nodes exist.
    // The check for node_capacity == 0 above should handle this.

    /* Construct mathematical proof of derivation validity */
    // Pass dag to dfs_validate_path as it's needed for get_node_index
    bool derivation_valid = kcapability_dag_dfs_validate_path(dag, source_capability, target_capability, visited);

    /* Clean up proof construction workspace */
    if (visited) { // Only free if allocated
        kfree(visited);
    }

    /* Update performance statistics for mathematical analysis */
    if (derivation_valid) {
        dag->performance_stats.successful_derivations++;
    } else {
        dag->performance_stats.failed_derivations++;
    }

    return derivation_valid;
}


/**
 * @brief Lookup a capability node by ID with mathematical validation.
 *
 * This function searches for a capability node within the DAG given its unique
 * capability ID. The current implementation uses a linear search through the
 * DAG's node array. It ensures that mathematical invariants of the DAG are
 * checked before and potentially after the operation (though lookup is read-only).
 * Performance statistics for cache hits/misses (conceptually) are updated.
 *
 * @param dag Pointer to the capability DAG.
 * @param capability_id The unique ID of the capability node to find.
 * @return Pointer to the found kcapability_dag_node_t if it exists, otherwise NULL.
 *
 * @pre dag != NULL.
 * @pre capability_id != INVALID_CAPABILITY_ID.
 * @pre The DAG's mathematical invariants must hold before this operation.
 * @post If a node with the given ID exists, a pointer to it is returned.
 * @post If no such node exists, NULL is returned.
 * @post Performance statistics (cache_hits/cache_misses) are updated.
 */
kcapability_dag_node_t* kcapability_dag_lookup_node(kcapability_dag_t* dag,
                                                   _BitInt(64) capability_id) {
    KASSERT(dag != NULL, "kcapability_dag_lookup_node: dag is NULL");
    // INVALID_CAPABILITY_ID should be defined (e.g., in kcapability_dag.c or kcapability_dag.h)
    KASSERT(capability_id != INVALID_CAPABILITY_ID, "kcapability_dag_lookup_node: capability_id is invalid");

    // It's good practice to ensure invariants hold before operations.
    // The user's spec included this KASSERT.
    KASSERT(dag->validate_invariants(dag), "kcapability_dag_lookup_node: DAG invariants failed before lookup");

    /*
     * Mathematical approach: Linear search for now, as specified.
     * This assumes dag->nodes is an array of kcapability_dag_node_t structures.
     * The mathematical structure (e.g., if nodes were in a hash table based on ID)
     * would enable more sophisticated indexing strategies for O(1) average case.
     * This will be implemented in future optimization phases.
     */
    for (_BitInt(32) i = 0wb; i < dag->node_count; i++) {
        // Ensure we are not accessing beyond allocated memory if node_count is somehow corrupt.
        // This should ideally be caught by validate_invariants checking node_count <= node_capacity.
        // KASSERT(i < dag->node_capacity, "Lookup iterating beyond capacity");
        if (dag->nodes[i].capability_id == capability_id) {
            /* Update performance statistics for mathematical analysis */
            dag->performance_stats.cache_hits++; // Conceptual cache hit
            return &dag->nodes[i];
        }
    }

    /* Capability not found - update performance statistics */
    dag->performance_stats.cache_misses++; // Conceptual cache miss
    return NULL;
}


/**
 * @brief Destroys a single capability DAG node, freeing associated resources.
 *
 * This function frees the memory allocated for the node itself and any
 * dynamically allocated internal structures, such as arrays for parent
 * and child pointers. It should be called when a node is no longer
 * referenced or when the entire DAG is being destroyed.
 *
 * @param node Pointer to the kcapability_dag_node_t to destroy.
 *             If NULL, the function does nothing.
 *
 * @pre The node should ideally have its relationships (edges) removed from
 *      other nodes before being destroyed to maintain DAG consistency,
 *      unless the entire DAG is being torn down.
 * @post All memory directly owned by the node (e.g., children/parents arrays) is freed.
 * @post The memory for the node structure itself is freed.
 */
void kcapability_dag_node_destroy(kcapability_dag_node_t* node) {
    if (!node) {
        return;
    }

    KASSERT(node->reference_count == 0wb, "kcapability_dag_node_destroy: Destroying node with non-zero reference count.");
    // ^ This KASSERT is important if ref counting is used for shared nodes.
    // If nodes are exclusively owned by the DAG's flat array and not shared,
    // this might be less critical during full DAG destruction but good for individual node removal.
    // For now, let's assume it's a good check.

    // Free dynamically allocated children and parents arrays
    if (node->children) {
        kfree(node->children);
        node->children = NULL;
    }
    if (node->parents) {
        kfree(node->parents);
        node->parents = NULL;
    }

    // Clear sensitive data for security (optional, but good practice)
    // Note: _BitInt types are essentially integers, direct overwrite is fine.
    node->capability_id = INVALID_CAPABILITY_ID; // Mark as invalid
    node->rights_mask = 0wb;
    node->security_level = 0wb;
    // node->resource_ptr = NULL; // If it points to something the node owns, it should be freed too.
                               // If it's just a reference, NULLing it out is fine.

    // Finally, free the node structure itself
    kfree(node);
}

/**
 * @brief Destroys a capability DAG, freeing all associated resources.
 *
 * This function iterates through all nodes in the DAG (if the DAG owns them
 * in a flat array structure) and calls kcapability_dag_node_destroy on each.
 * It then frees the main node array and the DAG structure itself.
 *
 * @param dag Pointer to the kcapability_dag_t to destroy.
 *            If NULL, the function does nothing.
 *
 * @pre The DAG should no longer be in use.
 * @post All memory allocated for the DAG and its nodes (if owned by the DAG's
 *       flat array) is freed.
 */
void kcapability_dag_destroy(kcapability_dag_t* dag) {
    if (!dag) {
        return;
    }

    KASSERT(dag != NULL, "kcapability_dag_destroy: dag pointer is NULL");

    // If the DAG nodes array (`dag->nodes`) stores the actual node data
    // (not just pointers to separately allocated nodes)
    // and kcapability_dag_node_destroy frees internal arrays of a node,
    // then we need to iterate and clean up those internal arrays first.
    // The current model from kcapability_dag_create is that dag->nodes is an array
    // of kcapability_dag_node_t structures themselves.
    if (dag->nodes) {
        for (_BitInt(32) i = 0wb; i < dag->node_count; i++) {
            // For each node, free its dynamically allocated children/parents arrays.
            // The node itself is part of the larger dag->nodes allocation.
            kcapability_dag_node_t* current_node = &dag->nodes[i];
            if (current_node->children) {
                kfree(current_node->children);
                current_node->children = NULL;
            }
            if (current_node->parents) {
                kfree(current_node->parents);
                current_node->parents = NULL;
            }
            // No kfree(current_node) here as it's part of the dag->nodes block
        }
        // Free the array of nodes
        kfree(dag->nodes);
        dag->nodes = NULL;
    }

    // If node_store_handle is different from dag->nodes and manages other allocations,
    // it should be cleaned up here too. Current assumption is dag->nodes is the main store.
    // dag->node_store_handle = NULL; // This was removed from kcapability_dag_t in a previous step.


    // Finally, free the DAG structure itself
    kfree(dag);
}


/**
 * @brief Helper function to get the index of a node in the DAG's node array.
 *
 * This function assumes that the DAG stores its nodes in a contiguous array
 * (`dag->nodes`) and that the node pointer is a valid pointer within this array.
 * The index is calculated based on pointer arithmetic.
 *
 * @param dag Pointer to the capability DAG.
 * @param node Pointer to the node whose index is required.
 * @return The index of the node in the dag's node array.
 * @warning Behavior is undefined if node is not part of dag->nodes.
 *          Panics if node pointer is less than dag->nodes base pointer.
 */
static size_t kcapability_dag_get_node_index(const kcapability_dag_t* dag, const kcapability_dag_node_t* node) {
    KASSERT(dag != NULL, "kcapability_dag_get_node_index: dag is NULL");
    KASSERT(node != NULL, "kcapability_dag_get_node_index: node is NULL");
    KASSERT(dag->nodes != NULL, "kcapability_dag_get_node_index: dag->nodes is NULL");
    // Ensure node is within the allocated array bounds for safety,
    // though pointer arithmetic itself doesn't check upper bound here.
    KASSERT(node >= dag->nodes, "kcapability_dag_get_node_index: node pointer is before dag->nodes base");

    ptrdiff_t diff = node - dag->nodes;
    // KASSERT(diff >= 0 && (size_t)diff < dag->node_capacity, "Node out of bounds or not in this DAG's primary node array");
    // The above KASSERT is more robust but requires node_capacity to be accurate for the dag->nodes array.
    // For now, rely on the fact that nodes are from this array.
    return (size_t)diff;
}


/**
 * @brief Depth-first search to validate a derivation path between two capability nodes.
 *
 * This function recursively traverses the DAG starting from the 'current' node,
 * attempting to find a path to the 'target' node. Unlike a simple path existence
 * check, this function would also incorporate validation of capability theory rules
 * at each step of the path if needed (e.g., ensuring rights are non-increasing,
 * security levels are non-decreasing along the path from target to source, or
 * source to target depending on derivation direction).
 *
 * For this implementation, as per the user's direct specification for
 * `kcapability_dag_validate_derivation` which calls this, this DFS will primarily
 * focus on path existence from `source_capability` (initial `current`) to
 * `target_capability`. The specific capability theory constraints (rights subset,
 * security level) are checked in `kcapability_dag_add_edge` when forming the DAG,
 * so a simple path existence here implies a valid derivation chain if the DAG
 * is correctly constructed.
 *
 * @param dag Pointer to the DAG (used for get_node_index).
 * @param current The current node in the DFS traversal.
 * @param target The target node we are trying to find a path to.
 * @param visited An array marking visited nodes to prevent infinite loops
 *                and redundant computations during this specific DFS traversal.
 * @return True if a valid path exists from current to target, false otherwise.
 *
 * @pre dag, current, target, and visited must not be NULL (unless dag->node_capacity is 0, then visited can be NULL).
 * @pre visited array must be at least dag->node_capacity in size if dag->node_capacity > 0.
 */
static bool kcapability_dag_dfs_validate_path(const kcapability_dag_t* dag,
                                             kcapability_dag_node_t* current,
                                             kcapability_dag_node_t* target,
                                             bool* visited) {
    KASSERT(dag != NULL, "dfs_validate_path: dag is NULL");
    KASSERT(current != NULL, "dfs_validate_path: current node is NULL");
    KASSERT(target != NULL, "dfs_validate_path: target node is NULL");
    // Visited can be NULL if dag->node_capacity is 0, kcalloc(0, size) returns NULL.
    // kcapability_dag_validate_derivation handles the node_capacity == 0 case before calling this.
    // So, if called, and node_capacity > 0, visited should not be NULL.
    if (dag->node_capacity > 0) {
        KASSERT(visited != NULL, "dfs_validate_path: visited array is NULL for non-zero capacity DAG");
    }


    /* Base case: we found the target, path is valid */
    if (current == target) {
        return true;
    }

    /* Mark current node as visited in our proof construction */
    // This requires dag->nodes to be the flat array store and dag->node_capacity to be correct.
    // If dag->node_capacity is 0, this part will not be reached due to checks in caller.
    size_t current_index = kcapability_dag_get_node_index(dag, current);
    // KASSERT(current_index < dag->node_capacity, "dfs_validate_path: current_index out of bounds");

    // If already visited in this specific DFS call, we've been here before down this path;
    // this check is crucial to prevent infinite loops if, hypothetically, the graph had a cycle
    // (though kcapability_dag_add_edge tries to prevent this).
    if (visited[current_index]) {
        return false; // Already visited this node in the current path search from the initial source.
    }
    visited[current_index] = true;

    /*
     * Recursive case: explore all children to extend our proof construction.
     * This implements the mathematical induction step in our proof.
     * Here, we are checking for a path from 'current' (which starts as 'source_capability')
     * down to 'target_capability'.
     */
    for (_BitInt(16) i = 0wb; i < current->child_count; i++) {
        kcapability_dag_node_t* child_node = current->children[i];
        KASSERT(child_node != NULL, "dfs_validate_path: child_node in children array is NULL");

        // No need to check visited[child_index] before recursive call,
        // the recursive call's own check `if (visited[child_index]) return false;` handles it.
        if (kcapability_dag_dfs_validate_path(dag, child_node, target, visited)) {
            return true;  /* Path found through this child */
        }
    }

    // Backtrack: Unmark current node as visited if we want to allow it to be found via other paths
    // in more complex search algorithms. For simple path existence from A to B,
    // once a path is exhausted without finding B, nodes on that path don't need unmarking
    // for *this specific call* to dfs_validate_path from its direct parent.
    // The `visited` array is for one top-level call to `kcapability_dag_validate_derivation`.
    // So, no explicit unmarking `visited[current_index] = false;` here.

    /* No path found through this node's children */
    return false;
}

/**
 * @brief Depth-first search for mathematical path existence proof.
 *
 * This function constructs a mathematical proof of path existence between
 * two nodes in the capability DAG by recursively exploring children.
 * It uses a 'visited' array to avoid getting stuck in cycles during the
 * traversal (though for DAG cycle *detection*, this DFS is initiated on
 * a graph that *should* be a DAG before adding the prospective edge).
 *
 * @param dag Pointer to the DAG (used for get_node_index).
 * @param current The current node in the DFS traversal.
 * @param target The target node we are trying to find a path to.
 * @param visited An array marking visited nodes to prevent infinite loops
 *                and redundant computations during this specific DFS traversal.
 *                The caller is responsible for allocating and freeing this array.
 *                Indices correspond to node indices from kcapability_dag_get_node_index.
 * @return True if a path exists from current to target, false otherwise.
 *
 * @pre current, target, and visited must not be NULL.
 * @pre visited array must be at least dag->node_capacity in size.
 */
static bool kcapability_dag_dfs_path_exists(const kcapability_dag_t* dag,
                                           kcapability_dag_node_t* current,
                                           kcapability_dag_node_t* target,
                                           bool* visited) {
    KASSERT(dag != NULL, "dfs_path_exists: dag is NULL");
    KASSERT(current != NULL, "dfs_path_exists: current node is NULL");
    KASSERT(target != NULL, "dfs_path_exists: target node is NULL");
    KASSERT(visited != NULL, "dfs_path_exists: visited array is NULL");

    /* Base case: we found the target, proof complete */
    if (current == target) {
        return true;
    }

    /* Mark current node as visited in our proof construction */
    // Note: The user's original code for kcapability_dag_would_create_cycle
    // implies dag->node_capacity for the size of the visited array.
    // This assumes nodes are indexed 0 to node_capacity-1.
    // If dag->nodes is a flat array, get_node_index works.
    size_t current_index = kcapability_dag_get_node_index(dag, current);
    // KASSERT(current_index < dag->node_capacity, "dfs_path_exists: current_index out of bounds for visited array");
    visited[current_index] = true;

    /*
     * Recursive case: explore all children to extend our proof construction.
     * This implements the mathematical induction step in our proof.
     */
    for (_BitInt(16) i = 0; i < current->child_count; i++) {
        kcapability_dag_node_t* child_node = current->children[i]; // Renamed for clarity
        KASSERT(child_node != NULL, "dfs_path_exists: child_node in children array is NULL");
        size_t child_index = kcapability_dag_get_node_index(dag, child_node);
        // KASSERT(child_index < dag->node_capacity, "dfs_path_exists: child_index out of bounds for visited array");


        /* Avoid infinite recursion in proof construction if already visited in *this specific DFS path search* */
        if (!visited[child_index]) {
            if (kcapability_dag_dfs_path_exists(dag, child_node, target, visited)) {
                return true;  /* Proof found through this path */
            }
        }
    }

    /* No proof found through this node for this DFS path */
    return false;
}


/**
 * @brief Determine if adding an edge would create a cycle using mathematical proof.
 *
 * This function implements mathematical cycle detection through depth-first
 * traversal. The mathematical approach ensures that we can prove the absence
 * of cycles rather than just testing for their presence before adding an edge.
 * An edge parent -> child would create a cycle if there is already a path
 * from child -> parent.
 *
 * @param dag Pointer to the capability DAG.
 * @param parent The proposed parent node of the new edge.
 * @param child The proposed child node of the new edge.
 * @return True if adding the edge parent->child would create a cycle, false otherwise.
 *
 * @pre dag, parent, and child must not be NULL.
 * @pre The current graph (before adding parent->child) is assumed to be a DAG.
 */
static bool kcapability_dag_would_create_cycle(kcapability_dag_t* dag,
                                              kcapability_dag_node_t* parent,
                                              kcapability_dag_node_t* child) {
    KASSERT(dag != NULL, "would_create_cycle: dag is NULL");
    KASSERT(parent != NULL, "would_create_cycle: parent node is NULL");
    KASSERT(child != NULL, "would_create_cycle: child node is NULL");
    /*
     * Mathematical insight: Adding edge parent->child creates a cycle if and
     * only if there already exists a path from child to parent. We use
     * depth-first search to construct a mathematical proof of path existence.
     */

    /* Allocate visited tracking for mathematical proof construction.
     * Size based on node_capacity as indices are derived from the base dag->nodes array.
     * kcalloc initializes to false (0).
     */
    if (dag->node_capacity == 0) { // Should not happen if dag is properly initialized with capacity
        return false; // Or panic, as a DAG with 0 capacity for nodes is problematic for indexing
    }
    bool* visited = (bool*)kcalloc((size_t)dag->node_capacity, sizeof(bool));
    if (!visited) {
        // This is a critical failure. In a real kernel, might try to recover or log.
        // For now, per user spec for allocation failures:
        kpanic("kcapability_dag_would_create_cycle: failed to allocate visited array");
        return true; // Defensive: assume cycle if we can't check
    }

    /*
     * Perform mathematical proof by construction: try to find a path from
     * child to parent using depth-first traversal.
     */
    bool cycle_exists = kcapability_dag_dfs_path_exists(dag, child, parent, visited);

    /* Clean up proof construction workspace */
    kfree(visited);

    return cycle_exists;
}


/**
 * @brief Unsafely adds a directed edge from parent_node to child_node.
 *
 * This function performs the actual pointer manipulations to establish the
 * parent-child relationship by updating their respective children and parents
 * arrays. It handles dynamic resizing of these arrays if necessary.
 * This function does NOT perform cycle checks or other high-level DAG
 * validation; it assumes callers have already done so.
 *
 * @param dag Pointer to the capability DAG (currently unused here, but good for context).
 * @param parent_node Pointer to the parent node.
 * @param child_node Pointer to the child node.
 * @return SUCCESS (0) if the edge was added.
 * @return ERROR_OUT_OF_MEMORY if memory allocation for resizing arrays fails.
 *
 * @note This is a low-level helper. Prefer kcapability_dag_add_edge for general use.
 * @pre parent_node and child_node are valid, non-NULL pointers.
 * @pre parent_node != child_node.
 * @pre Adding this edge does not create a cycle (caller responsibility).
 * @post parent_node will list child_node as a child.
 * @post child_node will list parent_node as a parent.
 * @post parent_node->child_count and child_node->parent_count are incremented.
 */
static int kcapability_dag_add_edge_unsafe(kcapability_dag_t* dag,
                                           kcapability_dag_node_t* parent_node,
                                           kcapability_dag_node_t* child_node) {
    // Add child to parent's children list
    if (parent_node->child_count >= parent_node->children_capacity) {
        // Resize children array
        _BitInt(16) new_capacity_children = parent_node->children_capacity == 0 ?
                                            (_BitInt(16))INITIAL_CHILD_PARENT_CAPACITY :
                                            parent_node->children_capacity * 2;
        // Ensure kcapability_dag_node_t** is the correct type for krealloc
        kcapability_dag_node_t** new_children_array =
            (kcapability_dag_node_t**)krealloc(parent_node->children,
                                      (size_t)new_capacity_children * sizeof(kcapability_dag_node_t*));
        if (!new_children_array) {
            return ERROR_OUT_OF_MEMORY;
        }
        parent_node->children = new_children_array;
        parent_node->children_capacity = new_capacity_children;
    }
    parent_node->children[parent_node->child_count] = child_node;
    parent_node->child_count++;

    // Add parent to child's parents list
    if (child_node->parent_count >= child_node->parents_capacity) {
        // Resize parents array
        _BitInt(16) new_capacity_parents = child_node->parents_capacity == 0 ?
                                           (_BitInt(16))INITIAL_CHILD_PARENT_CAPACITY :
                                           child_node->parents_capacity * 2;
        kcapability_dag_node_t** new_parents_array =
            (kcapability_dag_node_t**)krealloc(child_node->parents,
                                      (size_t)new_capacity_parents * sizeof(kcapability_dag_node_t*));
        if (!new_parents_array) {
            // Attempt to revert the addition to parent's children list for consistency,
            // though this is tricky if krealloc modified the original block in place before failing.
            // A more robust strategy might be needed for transactional semantics.
            // For now, simple revert:
            parent_node->child_count--;
            return ERROR_OUT_OF_MEMORY;
        }
        child_node->parents = new_parents_array;
        child_node->parents_capacity = new_capacity_parents;
    }
    child_node->parents[child_node->parent_count] = parent_node;
    child_node->parent_count++;

    (void)dag; // dag parameter currently unused but kept for API consistency or future use

    return SUCCESS;
}
