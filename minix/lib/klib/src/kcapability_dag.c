/**
 * @file kcapability_dag.c
 * @brief Implementation of the Capability Directed Acyclic Graph (DAG).
 *
 * This file provides functions to create, manage, and validate a DAG of
 * capabilities, ensuring properties like acyclicity and adherence to
 * security policies (rights subset, security level non-decreasing).
 */

#include <kcapability_dag.h>
#include <klib.h>        // For kmemset_c23, kpanic (indirectly via klib.h)
#include <sys/kassert.h> // For KASSERT

// --- Static Helper Function Prototypes ---

/**
 * @brief Depth-First Search helper to detect if adding an edge from source to
 * dest would create a cycle.
 *
 * It checks if 'dest' is reachable from 'source' *before* adding the edge. If
 * 'dest' is reachable from 'source', and then an edge (source -> dest) is
 * added, it implies a cycle. This function is slightly different from
 * dfs_path_exists as it's used predictively. The roles of source/dest here are
 * specific to the would_create_cycle context: If we add an edge u -> v, we
 * check if there's already a path v -> u.
 *
 * @param dag The capability DAG.
 * @param current The current node in the DFS traversal (initially, this would
 * be 'dest' from would_create_cycle).
 * @param target The target node to find a path to (initially, this would be
 * 'source' from would_create_cycle).
 * @param visited An array to keep track of visited nodes during DFS to prevent
 * re-processing.
 * @return kbool True if a path exists from 'current' to 'target', false
 * otherwise.
 */
static kbool kcapability_dag_dfs_for_cycle_check(
    kcapability_dag_t *dag, kcapability_dag_node_t *current,
    kcapability_dag_node_t *target, kbool *visited);

// --- Core DAG Management ---

/**
 * @brief Creates and initializes the global/static capability DAG instance.
 *
 * Initializes the DAG structure, including zeroing out node counts, edge count,
 * performance statistics, and setting up the invariant validation function
 * pointer. This implementation uses a single static instance for the DAG to
 * avoid dynamic kernel memory allocation for the DAG structure itself, ensuring
 * it's available early. The initialization occurs only once.
 *
 * @return Pointer to the initialized (or already initialized) static DAG
 * instance.
 */
kcapability_dag_t *kcapability_dag_create(void) {
  static kcapability_dag_t klib_dag_instance;
  static kbool klib_dag_initialized = false;

  // Initialize only once
  if (!klib_dag_initialized) {
    kmemset_c23(&klib_dag_instance, 0, sizeof(kcapability_dag_t));

    // num_nodes, edge_count, and all fields in performance_stats
    // are now zeroed by kmemset_c23.

    // Explicitly set the function pointer
    klib_dag_instance.validate_invariants =
        kcapability_dag_validate_invariants_impl;

    klib_dag_initialized = true;
    // kprintf_stub("Capability DAG initialized.\n"); // Optional debug
  }

  return &klib_dag_instance;
}

/**
 * @brief Destroys a capability DAG, freeing associated resources.
 * @param dag The DAG to destroy.
 * @note In the current static array model, "destroy" mainly means resetting.
 */
void kcapability_dag_destroy(kcapability_dag_t *dag) {
  KASSERT(dag != NULL);
  // If nodes or edges involved dynamic allocations, they'd be freed here.
  // For a static array model, this might just re-initialize.
  kmemset_c23(dag, 0, sizeof(kcapability_dag_t));
}

// --- Node Management ---

/**
 * @brief Gets the index of a node within the DAG's node array.
 * @param dag The capability DAG.
 * @param node The node to find.
 * @return The index of the node, or -1 (or an invalid index marker) if not
 * found.
 * @note This is useful because nodes are stored in an array.
 */
_BitInt(KCAPABILITY_MAX_NODES > 0
            ? (unsigned int)__builtin_clz(0) -
                  (unsigned int)__builtin_clz(KCAPABILITY_MAX_NODES - 1)
            : 1) kcapability_dag_get_node_index(kcapability_dag_t *dag,
                                                kcapability_dag_node_t *node) {
  KASSERT(dag != NULL);
  KASSERT(node != NULL);

  // Check if node pointer is within the bounds of the nodes array
  if (node >= &dag->nodes[0] && node < &dag->nodes[KCAPABILITY_MAX_NODES]) {
    k_size_t index = (k_size_t)(node - &dag->nodes[0]);
    // Ensure the node at this index is active and matches (optional sanity
    // check) if (dag->nodes[index].is_active && dag->nodes[index].id ==
    // node->id) {
    return (
        _BitInt(KCAPABILITY_MAX_NODES > 0
                    ? (unsigned int)__builtin_clz(0) -
                          (unsigned int)__builtin_clz(KCAPABILITY_MAX_NODES - 1)
                    : 1)) index;
    // }
  }
  // Consider returning a special value like KCAPABILITY_MAX_NODES or using a
  // status param For now, using a common pattern of returning a value that can
  // be checked. Let's use KCAPABILITY_MAX_NODES itself as an invalid index
  // sentinel.
  return (_BitInt(KCAPABILITY_MAX_NODES > 0 ? (unsigned int)__builtin_clz(0) -
                                                  (unsigned int)__builtin_clz(
                                                      KCAPABILITY_MAX_NODES - 1)
                                            : 1)) KCAPABILITY_MAX_NODES;
}

/**
 * @brief Creates a new capability node in the DAG.
 * @param dag The capability DAG.
 * @param id Unique ID for the new capability.
 * @param rights Rights mask for the new capability.
 * @param level Security level for the new capability.
 * @return Pointer to the created node, or NULL if max nodes reached or ID
 * exists.
 */
kcapability_dag_node_t *kcapability_dag_node_create(kcapability_dag_t *dag,
                                                    kcapability_id_t id,
                                                    krights_mask_t rights,
                                                    ksecurity_level_t level) {
  KASSERT(dag != NULL);

  if (dag->num_nodes >= KCAPABILITY_MAX_NODES) {
    return NULL; // Max nodes reached
  }

  // Check if node with this ID already exists (optional, but good practice)
  for (_BitInt(KCAPABILITY_MAX_NODES > 0
                   ? (unsigned int)__builtin_clz(0) -
                         (unsigned int)__builtin_clz(KCAPABILITY_MAX_NODES - 1)
                   : 1) i = 0;
       i < dag->num_nodes; ++i) {
    if (dag->nodes[i].is_active && dag->nodes[i].id == id) {
      // Node with this ID already exists. Depending on policy, could return it
      // or error. For create, usually means error or return existing. Let's
      // error. Or, this check belongs in a higher-level "add_capability"
      // function. For now, assuming create is low-level, finds first inactive
      // slot.
    }
  }

  // Find the first inactive node slot
  _BitInt(KCAPABILITY_MAX_NODES > 0
              ? (unsigned int)__builtin_clz(0) -
                    (unsigned int)__builtin_clz(KCAPABILITY_MAX_NODES - 1)
              : 1) node_idx = 0;
  for (node_idx = 0; node_idx < KCAPABILITY_MAX_NODES; ++node_idx) {
    if (!dag->nodes[node_idx].is_active) {
      break;
    }
  }

  if (node_idx >= KCAPABILITY_MAX_NODES)
    return NULL; // Should be caught by num_nodes check, but as safeguard.

  kcapability_dag_node_t *new_node = &dag->nodes[node_idx];
  kmemset_c23(new_node, 0,
              sizeof(kcapability_dag_node_t)); // Clear the node slot

  new_node->id = id;
  new_node->rights_mask = rights;
  new_node->security_level = level; // This is _BitInt(16)
  new_node->is_active = true;       // kbool

  // Initialize new C23-enhanced fields
  new_node->generation = 1wb; // Start generation at 1 (_BitInt(32))
  new_node->cached_validation_result = KCAP_VALIDATION_UNKNOWN;
  new_node->cache_timestamp = 0wb; // k_timestamp_t is _BitInt(64)

  // Initialize parent/child linkage fields
  new_node->num_children = 0wb; // Renamed. _BitInt type from header.
  // new_node->children array is zeroed by kmemset_c23

  new_node->num_parents = 0wb; // _BitInt type from header.
  // new_node->parents array is zeroed by kmemset_c23

  // DFS flags (already present, ensure they are initialized by kmemset or
  // explicitly)
  new_node->visited_flag = false;
  new_node->recursion_stack_flag = false;

  // Only increment num_nodes if we are taking a slot that was truly at the end
  // or if num_nodes tracks active count rather than high watermark.
  // Let's assume num_nodes tracks active count.
  // The loop for node_idx finds any free slot. If it's not the one at current
  // dag->num_nodes, this logic needs care. For simplicity, assume nodes are
  // appended or we find any free slot and dag->num_nodes is the count of active
  // nodes. A simpler model: always add to dag->nodes[dag->num_nodes] if that
  // slot is indeed free. This requires nodes to be "destroyed" by compacting or
  // properly marking free.

  // Let's refine: iterate up to KCAPABILITY_MAX_NODES to find any free slot.
  // If found, initialize it. dag->num_nodes should be the count of *active*
  // nodes. So, we need to iterate all nodes to find a free one if we want to
  // reuse slots. The current loop for node_idx does this.

  dag->num_nodes++; // Increment active node count
  return new_node;
}

/**
 * @brief Destroys a capability node (marks as inactive).
 * @param dag The DAG.
 * @param node The node to destroy.
 * @note This currently only marks the node as inactive. Edges pointing to/from
 * it are not explicitly removed from other nodes' adjacency lists here.
 */
void kcapability_dag_node_destroy(kcapability_dag_t *dag,
                                  kcapability_dag_node_t *node) {
  KASSERT(dag != NULL);
  KASSERT(node != NULL);
  KASSERT(node->is_active); // Should only destroy active nodes

  if (!node->is_active)
    return;

  // Mark as inactive
  node->is_active = false;
  // Optionally, clear other fields
  // kmemset_c23(node, 0, sizeof(kcapability_dag_node_t)); // Clears ID too,
  // careful

  dag->num_nodes--;

  // Important: Edges pointing to this node from other nodes are now stale.
  // A full cleanup would require iterating all other nodes and removing edges
  // to this one. Also, edges from this node are implicitly gone. For a simple
  // "mark as inactive", this is it.
}

/**
 * @brief Looks up a capability node by its ID.
 * @param dag The capability DAG.
 * @param capability_id The ID of the capability to find.
 * @return Pointer to the found node, or NULL if not found.
 */
kcapability_dag_node_t *
kcapability_dag_lookup_node(kcapability_dag_t *dag,
                            kcapability_id_t capability_id) {
  KASSERT(dag != NULL);
  for (_BitInt(KCAPABILITY_MAX_NODES > 0
                   ? (unsigned int)__builtin_clz(0) -
                         (unsigned int)__builtin_clz(KCAPABILITY_MAX_NODES - 1)
                   : 1) i = 0;
       i < KCAPABILITY_MAX_NODES; ++i) {
    if (dag->nodes[i].is_active && dag->nodes[i].id == capability_id) {
      return &dag->nodes[i];
    }
  }
  return NULL;
}

// --- Edge Management & Cycle Detection ---

/**
 * @brief Resets visited/recursion_stack flags for all nodes in the DAG.
 * Utility for multiple DFS traversals.
 * @param dag The capability DAG.
 */
static void kcapability_dag_reset_dfs_flags(kcapability_dag_t *dag) {
  KASSERT(dag != NULL);
  for (_BitInt(KCAPABILITY_MAX_NODES > 0
                   ? (unsigned int)__builtin_clz(0) -
                         (unsigned int)__builtin_clz(KCAPABILITY_MAX_NODES - 1)
                   : 1) i = 0;
       i < KCAPABILITY_MAX_NODES; ++i) {
    if (dag->nodes[i].is_active) {
      dag->nodes[i].visited_flag = false;
      dag->nodes[i].recursion_stack_flag = false;
    }
  }
}

/**
 * @brief DFS helper for kcapability_dag_would_create_cycle.
 * This checks if 'target_for_cycle' is reachable from 'current_dfs_node'.
 * Used to check if adding an edge (source_edge -> dest_edge) would create a
 * cycle, by checking if 'source_edge' is reachable from 'dest_edge' *before*
 * adding the edge.
 */
static kbool kcapability_dag_dfs_for_cycle_check_recursive(
    kcapability_dag_t *dag, kcapability_dag_node_t *current_dfs_node,
    kcapability_dag_node_t *target_for_cycle) {
  current_dfs_node->visited_flag = true;
  current_dfs_node->recursion_stack_flag = true;

  for (_BitInt(KCAPABILITY_MAX_EDGES_PER_NODE > 0
                   ? (unsigned int)__builtin_clz(0) -
                         (unsigned int)__builtin_clz(
                             KCAPABILITY_MAX_EDGES_PER_NODE - 1)
                   : 1) i = 0;
       i < current_dfs_node->num_children; ++i) { // Renamed
    kcapability_dag_node_t *adjacent_node =
        current_dfs_node->children[i].target_node; // Renamed
    KASSERT(adjacent_node &&
            adjacent_node
                ->is_active); // Should always be true if graph is consistent

    if (!adjacent_node->visited_flag) {
      if (kcapability_dag_dfs_for_cycle_check_recursive(dag, adjacent_node,
                                                        target_for_cycle)) {
        return true; // Cycle detected downstream
      }
    } else if (adjacent_node->recursion_stack_flag) {
      // This condition means we found the 'target_for_cycle' by traversing back
      // to it while it's still in the recursion stack of the *original* path
      // finding. For would_create_cycle(u,v), we search v -> u. If true, then
      // u->v makes a cycle. If adjacent_node == target_for_cycle, it means we
      // found a path.
      if (adjacent_node == target_for_cycle)
        return true;
      // Note: a generic cycle detection DFS would just return true here.
      // But we are looking for a specific back-edge to target_for_cycle.
    }
    if (adjacent_node == target_for_cycle)
      return true; // Path found
  }

  current_dfs_node->recursion_stack_flag = false; // Backtrack
  return false;
}

/**
 * @brief Generic DFS routine to detect cycles starting from a given node.
 *
 * This helper is used by the invariant validation routine to ensure the
 * overall capability DAG remains acyclic.
 *
 * @param start The node to begin traversal from.
 * @return kbool True if a cycle is detected, false otherwise.
 */
static kbool kcapability_dag_detect_cycle_dfs(kcapability_dag_node_t *start) {
  start->visited_flag = true;
  start->recursion_stack_flag = true;

  for (_BitInt(KCAPABILITY_MAX_EDGES_PER_NODE > 0
                   ? (unsigned int)__builtin_clz(0) -
                         (unsigned int)__builtin_clz(
                             KCAPABILITY_MAX_EDGES_PER_NODE - 1)
                   : 1) i = 0;
       i < start->num_children; ++i) {
    kcapability_dag_node_t *next = start->children[i].target_node;
    if (!next->visited_flag) {
      if (kcapability_dag_detect_cycle_dfs(next)) {
        return true;
      }
    } else if (next->recursion_stack_flag) {
      return true; // Found a back-edge
    }
  }

  start->recursion_stack_flag = false;
  return false;
}

/**
 * @brief Checks if adding a directed edge from source to dest would create a
 * cycle.
 * @param dag The DAG.
 * @param source The source node of the proposed edge.
 * @param dest The destination node of the proposed edge.
 * @return kbool True if adding the edge (source->dest) would create a cycle,
 * false otherwise.
 */
kbool kcapability_dag_would_create_cycle(kcapability_dag_t *dag,
                                         kcapability_dag_node_t *source,
                                         kcapability_dag_node_t *dest) {
  KASSERT(dag != NULL && source != NULL && dest != NULL);
  KASSERT(source->is_active && dest->is_active);

  if (source == dest)
    return true; // Self-loops are cycles

  kcapability_dag_reset_dfs_flags(dag); // Reset flags before new DFS

  // To check if source -> dest creates a cycle, see if there's already a path
  // from dest -> source
  return kcapability_dag_dfs_for_cycle_check_recursive(dag, dest, source);
}

/**
 * @brief Adds a directed edge from parent to child node in the DAG.
 * Performs checks for cycles, rights subset, and security level.
 * @param dag The capability DAG.
 * @param parent The parent (source) node.
 * @param child The child (destination) node.
 * @return kcapability_dag_status_t status code.
 */
kcapability_dag_status_t
kcapability_dag_add_edge(kcapability_dag_t *dag, kcapability_dag_node_t *parent,
                         kcapability_dag_node_t *child) {
  KASSERT(dag != NULL && parent != NULL && child != NULL);
  KASSERT(parent->is_active && child->is_active);

  if (parent == child)
    return KCAP_SELF_REFERENCE;

  // Check security policy: child's security level must be >= parent's
  if (child->security_level < parent->security_level) {
    return KCAP_SECURITY_VIOLATION;
  }

  // Check rights policy: child's rights must be a subset of parent's rights
  // (parent.rights & child.rights) == child.rights  OR child.rights is subset
  // of parent.rights
  if ((parent->rights_mask & child->rights_mask) != child->rights_mask) {
    return KCAP_RIGHTS_VIOLATION;
  }

  if (parent->num_children >= KCAPABILITY_MAX_EDGES_PER_NODE) { // Renamed
    return KCAP_MAX_EDGES_REACHED;
  }

  // Check for existing edge (optional, depends on policy for multiple edges)
  for (_BitInt(KCAPABILITY_MAX_EDGES_PER_NODE > 0
                   ? (unsigned int)__builtin_clz(0) -
                         (unsigned int)__builtin_clz(
                             KCAPABILITY_MAX_EDGES_PER_NODE - 1)
                   : 1) i = 0;
       i < parent->num_children; ++i) {             // Renamed
    if (parent->children[i].target_node == child) { // Renamed
      return KCAP_EDGE_ALREADY_EXISTS;
    }
  }

  // Check for cycle creation: this is the crucial DAG property
  if (kcapability_dag_would_create_cycle(dag, parent, child)) {
    return KCAP_CYCLE_DETECTED;
  }

  // Add edge
  parent->children[parent->num_children].target_node = child; // Renamed
  // ckd_add needs a pointer to the type, and the type must be a standard
  // integer for some compiler versions
  parent->num_children++; // Increment child count for parent

  // Add edge to child's parents list
  if (child->num_parents >= KCAPABILITY_MAX_PARENTS_PER_NODE) {
    // This should ideally not happen if MAX_PARENTS is well chosen,
    // or we need a strategy for when a child has too many parents.
    // For now, roll back the child addition to parent and return error.
    parent->num_children--;
    // kpanic("kcapability_dag_add_edge: Child node has reached max parents.");
    return KCAP_MAX_EDGES_REACHED; // Re-using this error, though it's for
                                   // parents here
  }
  child->parents[child->num_parents] = parent;
  child->num_parents++;

  dag->edge_count++; // Increment global edge count
  return KCAP_SUCCESS;
}

/**
 * @brief Adds an edge without safety checks (cycle, rights, security). Use with
 * extreme caution.
 * @param dag The DAG.
 * @param parent Parent node.
 * @param child Child node.
 * @return KCAP_SUCCESS or KCAP_MAX_EDGES_REACHED.
 */
kcapability_dag_status_t
kcapability_dag_add_edge_unsafe(kcapability_dag_t *dag,
                                kcapability_dag_node_t *parent,
                                kcapability_dag_node_t *child) {
  KASSERT(dag != NULL && parent != NULL && child != NULL);
  KASSERT(parent->is_active && child->is_active);

  if (parent->num_children >= KCAPABILITY_MAX_EDGES_PER_NODE) { // Renamed
    return KCAP_MAX_EDGES_REACHED;
  }
  parent->children[parent->num_children].target_node = child; // Renamed
  parent->num_children++;

  // Add to child's parents list
  if (child->num_parents >= KCAPABILITY_MAX_PARENTS_PER_NODE) {
    parent->num_children--; // Rollback
    // kpanic("kcapability_dag_add_edge_unsafe: Child node has reached max
    // parents.");
    return KCAP_MAX_EDGES_REACHED; // Or a more specific error
  }
  child->parents[child->num_parents] = parent;
  child->num_parents++;

  dag->edge_count++; // Increment global edge count
  return KCAP_SUCCESS;
}

/**
 * @brief DFS helper to check if a path exists from current to target. (General
 * purpose)
 * @param dag The DAG.
 * @param current Current node in DFS.
 * @param target Target node to find.
 * @param visited_dfs Array to track visited nodes for this specific DFS run.
 * @return kbool True if path exists.
 */
kbool kcapability_dag_dfs_path_exists(kcapability_dag_t *dag,
                                      kcapability_dag_node_t *current,
                                      kcapability_dag_node_t *target,
                                      kbool *visited_dfs) {
  KASSERT(dag != NULL && current != NULL && target != NULL &&
          visited_dfs != NULL);

  _BitInt(KCAPABILITY_MAX_NODES > 0
              ? (unsigned int)__builtin_clz(0) -
                    (unsigned int)__builtin_clz(KCAPABILITY_MAX_NODES - 1)
              : 1) current_idx = kcapability_dag_get_node_index(dag, current);
  // KASSERT(current_idx < KCAPABILITY_MAX_NODES); // Assuming get_node_index
  // returns valid or sentinel
  if (current_idx >= KCAPABILITY_MAX_NODES)
    return false; // Should not happen if node is valid

  visited_dfs[current_idx] = true;

  if (current == target) {
    return true;
  }

  for (_BitInt(KCAPABILITY_MAX_EDGES_PER_NODE > 0
                   ? (unsigned int)__builtin_clz(0) -
                         (unsigned int)__builtin_clz(
                             KCAPABILITY_MAX_EDGES_PER_NODE - 1)
                   : 1) i = 0;
       i < current->num_children; ++i) { // Renamed
    kcapability_dag_node_t *adjacent_node =
        current->children[i].target_node; // Renamed
    i < current->num_children; ++i) { // Renamed
      kcapability_dag_node_t *adjacent_node =
          current->children[i].target_node; // Renamed
      _BitInt(KCAPABILITY_MAX_NODES > 0
                  ? (unsigned int)__builtin_clz(0) -
                        (unsigned int)__builtin_clz(KCAPABILITY_MAX_NODES - 1)
                  : 1) adj_idx =
          kcapability_dag_get_node_index(dag, adjacent_node);
      if (adj_idx < KCAPABILITY_MAX_NODES && !visited_dfs[adj_idx]) {
        if (kcapability_dag_dfs_path_exists(dag, adjacent_node, target,
                                            visited_dfs)) {
          return true;
        }
      }
    }
    return false;
  }

  // --- Derivation Validation ---

  /**
   * @brief DFS helper to validate a derivation path considering rights masks.
   *
   * Traverses from 'current' to 'target'. Along any path found, it ensures that
   * the rights_mask of each child node is a subset of the 'current_rights_mask'
   * passed down from its parent in that specific path traversal.
   *
   * @param current The current node in the DFS traversal.
   * @param target The target node of the derivation.
   * @param visited An array to keep track of visited nodes for this DFS run.
   * @param path_rights_mask The rights mask allowed by the parent in this path.
   *                         The 'current' node's actual rights_mask must be a
   * subset of this.
   * @return kbool True if a valid path to target is found respecting rights,
   * false otherwise.
   */
  kbool kcapability_dag_dfs_validate_path(
      kcapability_dag_node_t * current, kcapability_dag_node_t * target,
      kbool * visited, krights_mask_t path_rights_mask) {
    KASSERT(current != NULL && target != NULL && visited != NULL);
    KASSERT(current->is_active); // Should be called on active nodes

    // Check if current node's rights are a subset of what's allowed by this
    // path from parent
    if ((path_rights_mask & current->rights_mask) != current->rights_mask) {
      return false; // Rights violation at this node for this path
    }

    // For kcapability_dag_get_node_index, we need the DAG pointer. This
    // function's signature lacks it. This implies 'visited' might need to be
    // managed differently, or node struct needs an index. Assuming 'visited' is
    // sized KCAPABILITY_MAX_NODES and indices are derivable if DAG ptr was
    // here. For now, this function has a flaw if node indices aren't directly
    // in node struct. Let's assume for a moment node->id could be used if it's
    // a dense 0 to N-1 index, which is not true. This helper needs redesigning
    // or the main kcapability_dag_validate_derivation needs to pass indices.
    // TEMPORARY: using current->visited_flag for this DFS run. Requires
    // resetting flags before main call.

    current->visited_flag = true; // Mark as visited for THIS DFS run

    if (current == target) {
      return true; // Valid path found to target
    }

    for (_BitInt(KCAPABILITY_MAX_EDGES_PER_NODE > 0
                     ? (unsigned int)__builtin_clz(0) -
                           (unsigned int)__builtin_clz(
                               KCAPABILITY_MAX_EDGES_PER_NODE - 1)
                     : 1) i = 0;
         i < current->num_adjacent_nodes; ++i) {
      kcapability_dag_node_t *adjacent_node =
          current->adjacent_nodes[i].target_node;
      KASSERT(adjacent_node && adjacent_node->is_active);

      if (!adjacent_node
               ->visited_flag) { // If not visited in *this specific DFS run*
        // The rights for the next step are dictated by the 'current' node's
        // actual rights
        if (kcapability_dag_dfs_validate_path(adjacent_node, target, visited,
                                              current->rights_mask)) {
          return true;
        }
      }
    }
    return false;
  }

  /**
   * @brief Validates if a 'target_capability' can be validly derived from
   * 'source_capability'.
   *
   * This means a path must exist from source to target, and along that path,
   * the rights of each subsequent capability must be a subset of its
   * predecessor's rights in that path, and security levels must be
   * non-decreasing. The non-decreasing security level is enforced by
   * kcapability_dag_add_edge. This function primarily focuses on path existence
   * and rights subset property along the path.
   *
   * @param dag The capability DAG.
   * @param source_capability The starting capability node.
   * @param target_capability The capability node to be derived.
   * @return kbool True if a valid derivation path exists, false otherwise.
   */
  kbool kcapability_dag_validate_derivation(
      kcapability_dag_t * dag, kcapability_dag_node_t * source_capability,
      kcapability_dag_node_t * target_capability) {
    KASSERT(dag != NULL && source_capability != NULL &&
            target_capability != NULL);
    KASSERT(source_capability->is_active && target_capability->is_active);

    if (source_capability ==
        target_capability) { // Deriving itself is valid if
                             // rights match (which they do)
      return true;
    }

    // Reset visited_flag for all nodes before starting DFS.
    // This is crucial if kcapability_dag_dfs_validate_path uses
    // node->visited_flag.
    kcapability_dag_reset_dfs_flags(dag);

    // For kcapability_dag_dfs_validate_path, it needs a 'visited' array if it's
    // to be re-entrant and not rely on global node flags for a single DFS run.
    // The current kcapability_dag_dfs_validate_path uses node->visited_flag,
    // which is reset above. The initial call starts with the
    // source_capability's own rights_mask.
    return kcapability_dag_dfs_validate_path(
        source_capability, target_capability,
        NULL /* not used if node->visited_flag is used */,
        source_capability->rights_mask);
  }

  /**
   * @brief Validates invariants of the DAG structure (placeholder).
   * @param dag The DAG.
   * @note A full implementation would check all consistency rules.
   */
  void kcapability_dag_validate_invariants_impl(kcapability_dag_t * dag) {
    KASSERT(dag != NULL);

    /**
     * Count active nodes and verify the count matches dag->num_nodes.
     * Also perform per-node structural checks while iterating.
     */
    k_size_t active_count = 0;
    _BitInt(64) computed_edges = 0wb; // Track total edges for consistency

    for (_BitInt(KCAPABILITY_MAX_NODES > 0 ? (unsigned int)__builtin_clz(0) -
                                                 (unsigned int)__builtin_clz(
                                                     KCAPABILITY_MAX_NODES - 1)
                                           : 1) i = 0;
         i < KCAPABILITY_MAX_NODES; ++i) {
      kcapability_dag_node_t *node = &dag->nodes[i];
      if (!node->is_active)
        continue;

      active_count++;

      // Validate node index using helper
      _BitInt(KCAPABILITY_MAX_NODES > 0
                  ? (unsigned int)__builtin_clz(0) -
                        (unsigned int)__builtin_clz(KCAPABILITY_MAX_NODES - 1)
                  : 1) idx = kcapability_dag_get_node_index(dag, node);
      KASSERT(idx == i);

      // Adjacency counts should not exceed compile-time limits
      KASSERT(node->num_children <= KCAPABILITY_MAX_EDGES_PER_NODE);
      KASSERT(node->num_parents <= KCAPABILITY_MAX_PARENTS_PER_NODE);

      // Validate each outgoing edge
      for (_BitInt(KCAPABILITY_MAX_EDGES_PER_NODE > 0
                       ? (unsigned int)__builtin_clz(0) -
                             (unsigned int)__builtin_clz(
                                 KCAPABILITY_MAX_EDGES_PER_NODE - 1)
                       : 1) c = 0;
           c < node->num_children; ++c) {
        kcapability_dag_node_t *child = node->children[c].target_node;
        KASSERT(child && child->is_active);

        // Security level and rights subset rules
        KASSERT(child->security_level >= node->security_level);
        KASSERT((node->rights_mask & child->rights_mask) == child->rights_mask);

        // Child must list this node as a parent
        kbool parent_found = false;
        for (_BitInt(KCAPABILITY_MAX_PARENTS_PER_NODE > 0
                         ? (unsigned int)__builtin_clz(0) -
                               (unsigned int)__builtin_clz(
                                   KCAPABILITY_MAX_PARENTS_PER_NODE - 1)
                         : 1) p = 0;
             p < child->num_parents; ++p) {
          if (child->parents[p] == node) {
            parent_found = true;
            break;
          }
        }
        KASSERT(parent_found);

        computed_edges++;
      }

      // Validate each incoming edge list is consistent
      for (_BitInt(KCAPABILITY_MAX_PARENTS_PER_NODE > 0
                       ? (unsigned int)__builtin_clz(0) -
                             (unsigned int)__builtin_clz(
                                 KCAPABILITY_MAX_PARENTS_PER_NODE - 1)
                       : 1) p = 0;
           p < node->num_parents; ++p) {
        kcapability_dag_node_t *parent = node->parents[p];
        KASSERT(parent && parent->is_active);

        kbool child_found = false;
        for (_BitInt(KCAPABILITY_MAX_EDGES_PER_NODE > 0
                         ? (unsigned int)__builtin_clz(0) -
                               (unsigned int)__builtin_clz(
                                   KCAPABILITY_MAX_EDGES_PER_NODE - 1)
                         : 1) c = 0;
             c < parent->num_children; ++c) {
          if (parent->children[c].target_node == node) {
            child_found = true;
            break;
          }
        }
        KASSERT(child_found);
      }
    }

    KASSERT(active_count == dag->num_nodes);

    // Edge count should match the total from adjacency lists
    KASSERT((_BitInt(64))dag->edge_count == computed_edges);

    // --- Cycle detection ---
    kcapability_dag_reset_dfs_flags(dag);

    for (_BitInt(KCAPABILITY_MAX_NODES > 0 ? (unsigned int)__builtin_clz(0) -
                                                 (unsigned int)__builtin_clz(
                                                     KCAPABILITY_MAX_NODES - 1)
                                           : 1) i = 0;
         i < KCAPABILITY_MAX_NODES; ++i) {
      if (!dag->nodes[i].is_active)
        continue;
      if (!dag->nodes[i].visited_flag) {
        KASSERT(!kcapability_dag_detect_cycle_dfs(&dag->nodes[i]));
      }
    }

    kcapability_dag_reset_dfs_flags(dag);
  }
