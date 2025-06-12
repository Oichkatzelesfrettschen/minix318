#pragma once
#include "exo.h"
#include "types.h"

/**
 * @struct dag_node_list
 * @brief Linked list of DAG child nodes.
 */
struct dag_node;
struct dag_node_list {
  struct dag_node *node;      /**< Child node pointer */
  struct dag_node_list *next; /**< Next child in list */
};

/**
 * @struct dag_node
 * @brief Schedulable DAG node with priority and weight.
 */
struct dag_node {
  exo_cap ctx;                    /**< Execution context capability */
  int pending;                    /**< Number of unfinished dependencies */
  int priority;                   /**< Scheduling priority */
  int weight;                     /**< Scheduling weight */
  struct dag_node_list *children; /**< Child nodes */
  struct dag_node *next;          /**< Next node in ready list */
  struct dag_node **deps;         /**< Array of parent nodes */
  int ndeps;                      /**< Number of parents */
  int done;                       /**< Completion flag */
};

void dag_node_init(struct dag_node *n, exo_cap ctx);
void dag_node_set_priority(struct dag_node *n, int priority);
void dag_node_set_weight(struct dag_node *n, int weight);
void dag_node_add_dep(struct dag_node *parent, struct dag_node *child);
void dag_sched_submit(struct dag_node *node);
void dag_sched_init(void);
