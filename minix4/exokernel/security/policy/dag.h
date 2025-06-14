#pragma once
#include "exo.h"
#include "types.h"

/**
 * @file dag.h
 * @brief Weighted DAG scheduler public interface.
 */

struct dag_node;

/**
 * @brief Linked list element used for a node's child list.
 */
struct dag_node_list {
  struct dag_node *node;      /**< Child node in the list. */
  exo_cap cap;                /**< Reserved for future capability hooks. */
  struct dag_node_list *next; /**< Next element. */
};

/**
 * @brief Single vertex in a dependency graph.
 */
struct dag_node {
  exo_cap ctx;                    /**< Execution context capability. */
  int pending;                    /**< Number of unfinished dependencies. */
  int priority;                   /**< Historical priority value. */
  int weight;                     /**< Scheduling weight. */
  struct dag_node_list *children; /**< Linked list of dependents. */
  struct dag_node *next;          /**< Internal scheduler linkage. */
  struct dag_node **deps;         /**< Array of parent dependencies. */
  int ndeps;                      /**< Length of the deps array. */
  int done;                       /**< Non-zero once the node has executed. */
};

/**
 * @brief Initialize a DAG node with an execution context.
 * @param n   Node to initialize.
 * @param ctx Execution context capability.
 */
void dag_node_init(struct dag_node *n, exo_cap ctx);

/** Set a node's informational priority value. */
void dag_node_set_priority(struct dag_node *n, int priority);

/** Set the weight used when ordering nodes in the ready queue. */
void dag_node_set_weight(struct dag_node *n, int weight);

/** Record that @p child depends on @p parent. */
void dag_node_add_dep(struct dag_node *parent, struct dag_node *child);

/** Submit a node to the scheduler. */
void dag_sched_submit(struct dag_node *node);

/** Initialize the scheduler subsystem. */
void dag_sched_init(void);
