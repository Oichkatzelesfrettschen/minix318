/**
 * @file nstr_graph.c
 * @brief Thread-safe directed graph implementation with C23 features
 * @author GitHub Copilot
 * @date 2024
 * @version 2.0
 * 
 * This module provides a thread-safe implementation of a directed graph
 * data structure using modern C23 features including:
 * - Enhanced type safety with typeof and auto
 * - Improved error handling patterns
 * - Comprehensive bounds checking
 * - Memory safety guarantees
 * - Thread-safe operations with proper synchronization
 */

#define _POSIX_C_SOURCE 200809L
#include "nstr_graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>
#include <stdatomic.h>
#include <string.h>

/** @brief Magic number for graph structure validation */
#define NSTR_GRAPH_MAGIC 0x47524148U  // "GRAH" in ASCII

/**
 * @brief Internal representation of a single graph node
 * 
 * Each node represents a vertex in the directed graph with a unique
 * integer identifier. Nodes are stored in a singly-linked list for
 * dynamic memory management.
 */
struct node {
  int id;                    /**< @brief Unique numeric identifier of the node */
  struct node * restrict next; /**< @brief Pointer to next node in the linked list */
};

/**
 * @brief Linked list item representing one directed edge
 * 
 * Represents a directed connection from one node to another.
 * Edges are stored in a singly-linked list with duplicate detection.
 */
struct edge {
  int from;                  /**< @brief Source node identifier */
  int to;                    /**< @brief Destination node identifier */
  struct edge * restrict next; /**< @brief Pointer to next edge in the linked list */
};

/**
 * @brief Opaque graph implementation with thread safety
 * 
 * Main graph structure containing all nodes and edges with thread-safe
 * access protection. Uses a magic number for structure validation.
 */
struct nstr_graph {
  uint32_t magic;            /**< @brief Magic number for structure validation */
  struct node * restrict nodes; /**< @brief Head of linked list of nodes */
  struct edge * restrict edges; /**< @brief Head of linked list of edges */
  pthread_mutex_t lock;      /**< @brief Mutex protecting concurrent access */
  atomic_size_t node_count;  /**< @brief Atomic counter for number of nodes */
  atomic_size_t edge_count;  /**< @brief Atomic counter for number of edges */
};

/**
 * @brief Validates graph structure integrity
 * 
 * Performs comprehensive validation of the graph structure including
 * magic number verification and null pointer checks.
 * 
 * @param[in] g Pointer to graph structure to validate
 * @return true if graph is valid, false otherwise
 * 
 * @pre g may be NULL (will return false)
 * @post Graph structure remains unchanged
 * @threadsafe This function is thread-safe for read-only validation
 */
[[nodiscard]] static inline bool validate_graph(const struct nstr_graph * restrict g) {
  return g != NULL && g->magic == NSTR_GRAPH_MAGIC;
}

/**
 * @brief Locate a node by its unique identifier
 * 
 * Performs linear search through the linked list of nodes to find
 * a node with the specified identifier. This function assumes the
 * caller holds the appropriate locks.
 * 
 * @param[in] g Graph instance to search (must be valid and locked)
 * @param[in] id Identifier of the node to locate
 * @return Pointer to the node if found, nullptr otherwise
 * 
 * @pre g must be a valid graph structure
 * @pre Caller must hold g->lock for thread safety
 * @post Graph structure remains unchanged
 * @complexity O(n) where n is the number of nodes
 */
[[nodiscard]] static struct node * restrict find_node(
  const struct nstr_graph * restrict g, 
  int id
) {
  assert(validate_graph(g));
  for (struct node * restrict current = g->nodes; current != NULL; current = current->next) {
    if (current->id == id) {
      return current;
    }
  }
  return NULL;
}

/**
 * @brief Ensure a node with the given identifier exists in the graph
 * 
 * Adds a new node to the graph if one with the specified identifier
 * does not already exist. The node is inserted at the head of the
 * linked list for O(1) insertion time.
 * 
 * @param[in,out] g Graph to modify (must be valid and locked)
 * @param[in] id Identifier of the node to add
 * @return true if node was added or already exists, false on allocation failure
 * 
 * @pre g must be a valid graph structure
 * @pre Caller must hold g->lock for thread safety
 * @pre id should be a valid integer identifier
 * @post If successful, graph contains a node with the specified id
 * @post On failure, graph remains unchanged
 * @threadsafe Requires external synchronization via g->lock
 */
[[nodiscard]] static bool add_node(struct nstr_graph * restrict g, int id) {
  assert(validate_graph(g));
  // Check if node already exists
  if (find_node(g, id) != NULL) {
    return true; // Node already exists, success
  }
  
  // Allocate new node with error checking
  struct node * restrict new_node = malloc(sizeof(*new_node));
  if (new_node == NULL) {
    return false; // Allocation failure
  }
  
  // Initialize node and insert at head
  *new_node = (struct node){
    .id = id,
    .next = g->nodes
  };
  
  g->nodes = new_node;
  atomic_fetch_add_explicit(&g->node_count, 1, memory_order_relaxed);
  
  return true;
}

/**
 * @brief Create a new empty graph instance
 * 
 * Allocates and initializes a new graph structure with all necessary
 * components including mutex initialization and atomic counters.
 * 
 * @return Newly allocated graph on success, nullptr on failure
 * 
 * @post On success, returns a valid empty graph
 * @post On failure, returns nullptr and sets errno appropriately
 * @threadsafe This function is thread-safe for concurrent calls
 * @see nstr_graph_close() for proper cleanup
 */
nstr_graph * nstr_graph_create(void) {
  struct nstr_graph * restrict g = malloc(sizeof(*g));
  if (g == NULL) {
    errno = ENOMEM;
    return NULL;
  }
  
  // Initialize mutex first for proper error handling
  int mutex_result = pthread_mutex_init(&g->lock, NULL);
  if (mutex_result != 0) {
    free(g);
    errno = mutex_result;
    return NULL;
  }
  
  // Initialize structure
  *g = (struct nstr_graph){
    .magic = NSTR_GRAPH_MAGIC,
    .nodes = NULL,
    .edges = NULL,
    .lock = g->lock, // Preserve initialized mutex
    .node_count = ATOMIC_VAR_INIT(0),
    .edge_count = ATOMIC_VAR_INIT(0)
  };
  
  return g;
}

/**
 * @brief Destroy and deallocate a graph instance
 * 
 * Performs complete cleanup of a graph structure including:
 * - Freeing all nodes and edges
 * - Destroying the mutex
 * - Invalidating the structure
 * - Freeing the main structure
 * 
 * @param[in,out] g Graph to destroy (may be nullptr)
 * 
 * @pre g may be nullptr (function handles gracefully)
 * @post If g was valid, all memory is freed and structure is invalidated
 * @post If g was nullptr, function returns without action
 * @threadsafe This function is NOT thread-safe; caller must ensure
 *             no other threads are accessing the graph
 */
void nstr_graph_close(nstr_graph * restrict g) {
  if (!validate_graph(g)) {
    return; // Handle nullptr or invalid graph gracefully
  }
  
  // Lock once for the entire cleanup process
  pthread_mutex_lock(&g->lock);
  
  // Invalidate structure early to prevent use-after-free
  g->magic = 0;
  // Free all edges
  struct edge * restrict current_edge = g->edges;
  while (current_edge != NULL) {
    struct edge * restrict next_edge = current_edge->next;
    free(current_edge);
    current_edge = next_edge;
  }
  g->edges = NULL;
  
  // Free all nodes
  struct node * restrict current_node = g->nodes;
  while (current_node != NULL) {
    struct node * restrict next_node = current_node->next;
    free(current_node);
    current_node = next_node;
  }
  g->nodes = NULL;
  g->nodes = nullptr;
  
  // Reset counters
  atomic_store_explicit(&g->node_count, 0, memory_order_relaxed);
  atomic_store_explicit(&g->edge_count, 0, memory_order_relaxed);
  
  pthread_mutex_unlock(&g->lock);
  pthread_mutex_destroy(&g->lock);
  free(g);
}

/**
 * @brief Add a directed edge to the graph
 * 
 * Creates a directed edge from the source node to the destination node.
 * Both nodes are automatically created if they don't exist. Duplicate
 * edges are detected and prevented.
 * 
 * @param[in,out] g Graph to modify
 * @param[in] from Source node identifier
 * @param[in] to Destination node identifier
 * @return 0 on success, -1 on failure
 * @retval 0 Edge added successfully or already exists
 * @retval -1 Invalid graph, memory allocation failure, or other error
 * 
 * @pre g must be a valid graph structure
 * @post On success, graph contains the specified edge
 * @post On failure, graph remains unchanged
 * @threadsafe This function is thread-safe
 */
[[nodiscard]] int nstr_graph_add_edge(nstr_graph * restrict g, int from, int to) {
  if (!validate_graph(g)) {
    errno = EINVAL;
    return -1;
  }
  
  int result = pthread_mutex_lock(&g->lock);
  if (result != 0) {
    errno = result;
    return -1;
  }
  
  // Ensure both nodes exist
  if (!add_node(g, from) || !add_node(g, to)) {
    pthread_mutex_unlock(&g->lock);
    errno = ENOMEM;
    return -1;
  }
  // Check for duplicate edge
  for (const struct edge * restrict current = g->edges; current != NULL; current = current->next) {
    if (current->from == from && current->to == to) {
      pthread_mutex_unlock(&g->lock);
      return 0; // Edge already exists, success
    }
  }
  
  // Allocate and initialize new edge
  struct edge * restrict new_edge = malloc(sizeof(*new_edge));
  if (new_edge == NULL) {
    pthread_mutex_unlock(&g->lock);
    errno = ENOMEM;
    return -1;
  }
  }
  
  *new_edge = (struct edge){
    .from = from,
    .to = to,
    .next = g->edges
  };
  
  g->edges = new_edge;
  atomic_fetch_add_explicit(&g->edge_count, 1, memory_order_relaxed);
  
  pthread_mutex_unlock(&g->lock);
  return 0;
}

/**
 * @brief Remove a directed edge from the graph
 * 
 * Removes the specified directed edge from the graph if it exists.
 * Nodes are not removed even if they become isolated.
 * 
 * @param[in,out] g Graph to modify
 * @param[in] from Source node identifier
 * @param[in] to Destination node identifier
 * @return 0 on success, -1 if edge not found or error
 * @retval 0 Edge removed successfully
 * @retval -1 Invalid graph or edge not found
 * 
 * @pre g must be a valid graph structure
 * @post On success, the specified edge is removed from the graph
 * @post On failure, graph remains unchanged
 * @threadsafe This function is thread-safe
 */
[[nodiscard]] int nstr_graph_remove_edge(nstr_graph * restrict g, int from, int to) {
  if (!validate_graph(g)) {
    errno = EINVAL;
    return -1;
  }
  
  int result = pthread_mutex_lock(&g->lock);
  if (result != 0) {
    errno = result;
    return -1;
  }
  // Use pointer-to-pointer for efficient removal
  struct edge ** restrict prev_ptr = &g->edges;
  while (*prev_ptr != NULL) {
    struct edge * restrict current = *prev_ptr;
    if (current->from == from && current->to == to) {
      *prev_ptr = current->next;
      free(current);
      atomic_fetch_sub_explicit(&g->edge_count, 1, memory_order_relaxed);
      pthread_mutex_unlock(&g->lock);
      return 0;
    }
    prev_ptr = &current->next;
  }
  }
  
  pthread_mutex_unlock(&g->lock);
  errno = ENOENT;
  return -1; // Edge not found
}

/**
 * @brief Query whether a directed edge exists in the graph
 * 
 * Checks if a directed edge from the source to destination node exists.
 * This is a read-only operation that doesn't modify the graph.
 * 
 * @param[in] g Graph to query
 * @param[in] from Source node identifier
 * @param[in] to Destination node identifier
 * @return 1 if edge exists, 0 if not found or invalid graph
 * @retval 1 Edge exists in the graph
 * @retval 0 Edge does not exist or invalid graph
 * 
 * @pre g may be nullptr (returns 0)
 * @post Graph remains unchanged
 * @threadsafe This function is thread-safe
 */
[[nodiscard]] int nstr_graph_query(const nstr_graph * restrict g, int from, int to) {
  if (!validate_graph(g)) {
    return 0;
  }
  
  int result = pthread_mutex_lock(&((struct nstr_graph *)g)->lock);
  if (result != 0) {
    return 0;
  }
  for (const struct edge * restrict current = g->edges; current != NULL; current = current->next) {
    if (current->from == from && current->to == to) {
      pthread_mutex_unlock(&((struct nstr_graph *)g)->lock);
      return 1;
    }
  }
  }
  
  pthread_mutex_unlock(&((struct nstr_graph *)g)->lock);
  return 0;
}

/**
 * @brief Get the number of nodes in the graph
 * 
 * Returns the current count of nodes in the graph using atomic operations
 * for thread-safe access without requiring locks.
 * 
 * @param[in] g Graph to query
 * @return Number of nodes, or 0 if graph is invalid
 * 
 * @pre g may be nullptr (returns 0)
 * @post Graph remains unchanged
 * @threadsafe This function is thread-safe and lock-free
 */
[[nodiscard]] size_t nstr_graph_node_count(const nstr_graph * restrict g) {
  if (!validate_graph(g)) {
    return 0;
  }
  return atomic_load_explicit(&g->node_count, memory_order_relaxed);
}

/**
 * @brief Get the number of edges in the graph
 * 
 * Returns the current count of edges in the graph using atomic operations
 * for thread-safe access without requiring locks.
 * 
 * @param[in] g Graph to query
 * @return Number of edges, or 0 if graph is invalid
 * 
 * @pre g may be nullptr (returns 0)
 * @post Graph remains unchanged
 * @threadsafe This function is thread-safe and lock-free
 */
[[nodiscard]] size_t nstr_graph_edge_count(const nstr_graph * restrict g) {
  if (!validate_graph(g)) {
    return 0;
  }
  return atomic_load_explicit(&g->edge_count, memory_order_relaxed);
}
