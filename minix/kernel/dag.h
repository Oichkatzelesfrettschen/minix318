#pragma once
#include "exo.h"
#include "types.h"

/**
 * @file dag.h
 * @brief Interfaces for the weighted DAG scheduler.
 */

struct dag_node;

/**
 * @brief Linked list element for DAG nodes.
 */
struct dag_node_list {
  struct dag_node *node; /**< Child node in the list. */
  struct dag_node_list *next; /**< Next element in the list. */
};

/**

 * @brief Node representing a schedulable unit in the DAG scheduler.
 */
struct dag_node {
  exo_cap ctx;                    /**< Capability to resume the task. */
  int pending;                    /**< Number of unfinished dependencies. */
  int priority;                   /**< Historical priority field. */
  int weight;                     /**< Scheduling weight. */
  struct dag_node_list *children; /**< Child nodes depending on this node. */
  struct dag_node *next;          /**< Internal link used by the scheduler. */
  struct dag_node **deps;         /**< Array of parent dependencies. */
  int ndeps;                      /**< Number of entries in @c deps. */
  int done;                       /**< Set once the node has run. */

};

/**
 * @brief Initialize a DAG node with an execution context.
 *
 * @param n   Node to initialize.
 * @param ctx Execution context capability.
 */
void dag_node_init(struct dag_node *n, exo_cap ctx);

/**
 * @brief Set the scheduling priority for a node.
 *
 * @param n        Node to update.
 * @param priority Priority value.
 */
void dag_node_set_priority(struct dag_node *n, int priority);
/**
 * @brief Set the scheduling weight of a node.
 *
 * Higher-weighted nodes are chosen before lower-weighted ones.


/**
 * @brief Set the relative weight for a node.
 *
 * @param n      Node to update.
 * @param weight Weight used when ordering nodes.

 */
void dag_node_set_weight(struct dag_node *n, int weight);

/**
 * @brief Declare a dependency between two nodes.
 *
 * @param parent Node that must finish first.
 * @param child  Node that depends on @p parent.
 */
void dag_node_add_dep(struct dag_node *parent, struct dag_node *child);

/**
 * @brief Submit a node to the DAG scheduler.
 *
 * Nodes with no pending dependencies are added to the ready queue.
 *
 * @param n Node to submit.
 */
void dag_sched_submit(struct dag_node *n);

/**
 * @brief Initialize the DAG scheduler subsystem.
 */
void dag_sched_init(void);
