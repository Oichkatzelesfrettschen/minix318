#ifndef _MINIX_KCAPABILITY_DAG_H
#define _MINIX_KCAPABILITY_DAG_H

#include <klib.h>   // For k_size_t, kbool, KASSERT (indirectly via klib.h)
#include <stdint.h> // For exact width types if not covered by klib's includes

// Define _BitInt if not available (e.g. for compilers not fully C23 or for
// clarity) However, the task assumes C23, so _BitInt should be available.
// typedef unsigned long long _BitInt_64_fallback; // Fallback for non-C23, not
// ideal #define _BitInt(N) _BitInt_##N##_fallback // Example, use compiler's
// _BitInt

#ifndef KCAPABILITY_MAX_NODES
#define KCAPABILITY_MAX_NODES 64 // Default max nodes, can be configured
#endif

#ifndef KCAPABILITY_MAX_EDGES_PER_NODE
#define KCAPABILITY_MAX_EDGES_PER_NODE                                         \
  8 // Default max edges per node (renamed to children)
#endif

#ifndef KCAPABILITY_MAX_PARENTS_PER_NODE
#define KCAPABILITY_MAX_PARENTS_PER_NODE                                       \
  8 // Default max incoming edges (parents)
#endif

/**
 * @typedef kcapability_id_t
 * @brief Defines the type for unique capability identifiers.
 * Using C23 _BitInt for precise width.
 */
typedef _BitInt(64) kcapability_id_t;

/**
 * @typedef krights_mask_t
 * @brief Defines the type for rights associated with a capability.
 * Each bit can represent a specific right.
 */
typedef _BitInt(64) krights_mask_t;

/**
 * @typedef ksecurity_level_t
 * @brief Defines the type for security levels.
 * Higher values typically mean higher security.
 */
typedef _BitInt(16) ksecurity_level_t;

/**
 * @typedef k_timestamp_t
 * @brief Defines a type for timestamps, e.g., for cache validation or
 * generation counts. Using C23 _BitInt for precise width, representing e.g.,
 * nanoseconds or ticks.
 */
typedef _BitInt(64) k_timestamp_t;

/**
 * @enum kcap_validation_state_t
 * @brief Represents the cached validation state of a capability node.
 */
typedef enum kcap_validation_state_t {
  KCAP_VALIDATION_UNKNOWN =
      0, /**< Validation state not yet determined or cache stale. */
  KCAP_VALIDATION_VALID =
      1, /**< Node has been validated as part of a legitimate derivation. */
  KCAP_VALIDATION_INVALID = 2 /**< Node has been determined to be invalid. */
} kcap_validation_state_t;

/**
 * @enum kcapability_dag_status_t
 * @brief Status codes for DAG operations.
 */
typedef enum kcapability_dag_status_t {
  KCAP_SUCCESS = 0,
  KCAP_ALLOC_FAILURE = -1,
  KCAP_INVALID_PARAM = -2,
  KCAP_NODE_NOT_FOUND = -3,
  KCAP_EDGE_ALREADY_EXISTS = -4,
  KCAP_CYCLE_DETECTED = -5,
  KCAP_RIGHTS_VIOLATION = -6,   // Child rights not subset of parent
  KCAP_SECURITY_VIOLATION = -7, // Child security level lower than parent
  KCAP_MAX_NODES_REACHED = -8,
  KCAP_MAX_EDGES_REACHED = -9,
  KCAP_SELF_REFERENCE = -10, // Attempt to add edge from node to itself
  KCAP_NODE_ALREADY_EXISTS = -11,
} kcapability_dag_status_t;

// Forward declaration
struct kcapability_dag_node_s;

/**
 * @struct kcapability_dag_edge_t
 * @brief Represents a directed edge in the capability DAG.
 *
 * Connects a source node to a target node. In this model, edges might not
 * need to store complex data if capabilities are primarily node-centric.
 * For simplicity, we'll represent edges as connections in an adjacency list.
 */
typedef struct kcapability_dag_edge_t {
  struct kcapability_dag_node_s
      *target_node; /**< Pointer to the target node of the edge. */
  // Potentially, an edge could have its own rights mask if derivations can
  // restrict further. krights_mask_t edge_rights_modifier;
} kcapability_dag_edge_t;

/**
 * @struct kcapability_dag_node_s
 * @brief Represents a node in the capability DAG, C23 enhanced.
 *
 * Each node is a unique capability with an ID, associated rights,
 * a security level, generation count, linkage to parents and children,
 * and fields for caching validation results.
 * Aligned to cache line size for performance (K_CACHE_LINE_SIZE assumed defined
 * elsewhere).
 */
typedef struct kcapability_dag_node_s {
  kcapability_id_t id; /**< Unique identifier for this capability node. */
  krights_mask_t
      rights_mask; /**< Bitmask of rights this capability possesses. */
  ksecurity_level_t security_level; /**< Security level of this capability (use
                                       _BitInt(16)). */
  _BitInt(32) generation; /**< Generation count for ABA problem mitigation in
                             dynamic scenarios. */
  kbool is_active; /**< Flag to indicate if this node slot is in use (for static
                      array model). */

  // Children (outgoing edges) - Adjacency list
  struct kcapability_dag_edge_t children[KCAPABILITY_MAX_EDGES_PER_NODE];
  _BitInt(KCAPABILITY_MAX_EDGES_PER_NODE > 0
              ? (unsigned int)__builtin_clz(0) -
                    (unsigned int)__builtin_clz(KCAPABILITY_MAX_EDGES_PER_NODE -
                                                1)
              : 1) num_children;

  // Parents (incoming edges) - Reverse adjacency list
  struct kcapability_dag_node_s *parents[KCAPABILITY_MAX_PARENTS_PER_NODE];
  _BitInt(KCAPABILITY_MAX_PARENTS_PER_NODE > 0
              ? (unsigned int)__builtin_clz(0) -
                    (unsigned int)
                        __builtin_clz(KCAPABILITY_MAX_PARENTS_PER_NODE - 1)
              : 1) num_parents;

  // Validation cache
  kcap_validation_state_t
      cached_validation_result; /**< Cached result of a previous validation. */
  k_timestamp_t
      cache_timestamp; /**< Timestamp of when this result was cached. */

  // Temporary flags for DFS traversal (note: for SMP, pass context or use
  // generation counts)
  kbool visited_flag; /**< Temp flag for DFS: visited in current traversal. */
  kbool recursion_stack_flag; /**< Temp flag for DFS: currently in recursion
                                 stack. */
} __attribute__((
    aligned(64))) kcapability_dag_node_t; // Assuming K_CACHE_LINE_SIZE = 64

/**
 * @struct kcap_dag_perf_stats_t
 * @brief Performance statistics for DAG operations.
 */
typedef struct kcap_dag_perf_stats_t {
  _BitInt(64) operations_total; /**< Total operations performed on the DAG. */
  _BitInt(64)
      cache_hits; /**< Number of cache hits during lookups (conceptual). */
  _BitInt(64)
      cache_misses; /**< Number of cache misses during lookups (conceptual). */
  _BitInt(64) successful_derivations; /**< Number of successful derivation
                                         validations. */
  _BitInt(64)
      failed_derivations; /**< Number of failed derivation validations. */
} kcap_dag_perf_stats_t;

/**
 * @struct kcapability_dag_t
 * @brief Represents the Capability Directed Acyclic Graph (DAG).
 *
 * Contains all capability nodes and their relationships.
 * This implementation uses a statically sized array for nodes within the DAG
 * structure.
 */
typedef struct kcapability_dag_t {
  kcapability_dag_node_t
      nodes[KCAPABILITY_MAX_NODES]; /**< Static array of nodes. */
  // num_nodes uses a bit-width appropriately sized for KCAPABILITY_MAX_NODES
  _BitInt(KCAPABILITY_MAX_NODES > 0
              ? (unsigned int)__builtin_clz(0) -
                    (unsigned int)__builtin_clz(KCAPABILITY_MAX_NODES - 1)
              : 1) num_nodes; /**< Current number of active nodes. */
  _BitInt(32) edge_count;     /**< Current number of active edges in the DAG. */

  kcap_dag_perf_stats_t
      performance_stats; /**< Performance counters for DAG operations. */

  /**
   * @brief Function pointer to validate the DAG's internal invariants.
   * @param dag Pointer to the DAG instance to validate.
   */
  void (*validate_invariants)(struct kcapability_dag_t *dag);

  // kbool initialized; // Optional: Could be useful
} kcapability_dag_t;

// --- Function Prototypes ---

// Core DAG management
kcapability_dag_t *kcapability_dag_create(void);
void kcapability_dag_destroy(
    kcapability_dag_t *dag); // Frees DAG resources (if dynamically allocated)

// Node management
kcapability_dag_node_t *kcapability_dag_node_create(kcapability_dag_t *dag,
                                                    kcapability_id_t id,
                                                    krights_mask_t rights,
                                                    ksecurity_level_t level);
void kcapability_dag_node_destroy(
    kcapability_dag_t *dag,
    kcapability_dag_node_t *node); // Marks node as inactive, handles edges.
kcapability_dag_node_t *
kcapability_dag_lookup_node(kcapability_dag_t *dag,
                            kcapability_id_t capability_id);
_BitInt(KCAPABILITY_MAX_NODES > 0
            ? (unsigned int)__builtin_clz(0) -
                  (unsigned int)__builtin_clz(KCAPABILITY_MAX_NODES - 1)
            : 1) kcapability_dag_get_node_index(kcapability_dag_t *dag,
                                                kcapability_dag_node_t *node);

// Edge management
kcapability_dag_status_t
kcapability_dag_add_edge(kcapability_dag_t *dag, kcapability_dag_node_t *parent,
                         kcapability_dag_node_t *child);
kcapability_dag_status_t kcapability_dag_add_edge_unsafe(
    kcapability_dag_t *dag, kcapability_dag_node_t *parent,
    kcapability_dag_node_t *child); // Bypasses some checks

// Path and cycle detection
kbool kcapability_dag_would_create_cycle(kcapability_dag_t *dag,
                                         kcapability_dag_node_t *source,
                                         kcapability_dag_node_t *dest);
kbool kcapability_dag_dfs_path_exists(
    kcapability_dag_t *dag, kcapability_dag_node_t *current,
    kcapability_dag_node_t *target,
    kbool *visited_dfs); // Renamed visited param

// Validation
void kcapability_dag_validate_invariants_impl(
    kcapability_dag_t *dag); // Internal invariant checker
// DFS helper for derivation validation (newly specified)
kbool kcapability_dag_dfs_validate_path(kcapability_dag_node_t *current,
                                        kcapability_dag_node_t *target,
                                        kbool *visited,
                                        krights_mask_t current_rights_mask);
// Derivation validation (newly specified)
kbool kcapability_dag_validate_derivation(
    kcapability_dag_t *dag, kcapability_dag_node_t *source_capability,
    kcapability_dag_node_t *target_capability);

#endif /* _MINIX_KCAPABILITY_DAG_H */
// minix/lib/klib/include/kcapability_dag.h
#pragma once
#include <stddef.h> // For size_t if used
#include <stdint.h> // For uint64_t if used by kcapability_node_t

// Forward declaration or basic structure for kcapability_node_t
// This is a placeholder as the actual structure is not provided.
typedef struct kcapability_node {
  uint64_t id;
  uint64_t rights;
  uint64_t creation_time;
  uint64_t expiry_time; // Added based on do_math_read
  // Other fields would be necessary for a real DAG implementation
} kcapability_node_t;

// Placeholder for related functions if any were expected here
#endif // KCAPABILITY_DAG_H (some editors add this automatically)
// minix/lib/klib/include/kcapability_dag.h
#pragma once
#include <stddef.h> // For size_t if used
#include <stdint.h> // For uint64_t if used by kcapability_node_t

// Forward declaration or basic structure for kcapability_node_t
// This is a placeholder as the actual structure is not provided.
typedef struct kcapability_node {
  uint64_t id;
  uint64_t rights;
  uint64_t creation_time;
  uint64_t expiry_time; // Added based on do_math_read
  // Other fields would be necessary for a real DAG implementation
} kcapability_node_t;

// Placeholder for related functions if any were expected here
#endif // KCAPABILITY_DAG_H (some editors add this automatically)
// minix/lib/klib/include/kcapability_dag.h
#pragma once
#include <stddef.h> // For size_t if used
#include <stdint.h> // For uint64_t if used by kcapability_node_t

// Forward declaration or basic structure for kcapability_node_t
// This is a placeholder as the actual structure is not provided.
typedef struct kcapability_node {
  uint64_t id;
  uint64_t rights;
  uint64_t creation_time;
  uint64_t expiry_time; // Added based on do_math_read
  // Other fields would be necessary for a real DAG implementation
} kcapability_node_t;

// Placeholder for related functions if any were expected here
#endif // KCAPABILITY_DAG_H (some editors add this automatically)
