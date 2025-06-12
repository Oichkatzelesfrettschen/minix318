#pragma once
#include "exo.h"
#include "types.h"

/**
 * @file dag.h
 * @brief Interfaces for the weighted DAG scheduler.
 */

struct dag_node;

struct dag_node_list {
  struct dag_node *node;
  struct dag_node_list *next;
};

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

void dag_node_init(struct dag_node *n, exo_cap ctx);
void dag_node_set_priority(struct dag_node *n, int priority);
/**
 * @brief Set the scheduling weight of a node.
 *
 * Higher-weighted nodes are chosen before lower-weighted ones.
 */
void dag_node_set_weight(struct dag_node *n, int weight);
void dag_node_add_dep(struct dag_node *parent, struct dag_node *child);
void dag_sched_submit(struct dag_node *node);
void dag_sched_init(void);
