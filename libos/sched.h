#pragma once
#include <minix/dag.h>
/**
 * @brief Set the priority for a DAG node.
 *
 * @param n        Node to update.
 * @param priority New priority value.
 */
static inline void dag_node_set_priority(struct dag_node *n, int priority) {
  n->priority = priority;
}

/**
 * @brief Set the weight for a DAG node.
 *
 * @param n      Node to update.
 * @param weight New weight value.
 */
static inline void dag_node_set_weight(struct dag_node *n, int weight) {
  n->weight = weight;
}

void beatty_sched_set_tasks(exo_cap, exo_cap);
void beatty_sched_set_weights(int, int);
void libos_setup_beatty_dag(void);
