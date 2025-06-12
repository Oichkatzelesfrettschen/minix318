#pragma once
#include "exo.h"
#include "types.h"

struct dag_node;

struct dag_node_list {
  struct dag_node *node;
  struct dag_node_list *next;
};

/**
 * @brief Representation of a task scheduled by the DAG scheduler.
 *
 * The structure stores dependency and scheduling metadata for a
 * single execution context. All fields originate from the legacy
 * scheduler implementation and are preserved verbatim.
 */
struct dag_node {
  /** execution context capability */
  exo_cap ctx;
  /** unresolved dependencies */
  int pending;
  /** priority used for ordering */
  int priority;
  /** weight used by weighted schedulers */
  int weight;
  /** list of children depending on this node */
  struct dag_node_list *children;
  /** next pointer used for ready queue */
  struct dag_node *next;
  /** array of parent dependencies */
  struct dag_node **deps;
  /** number of dependency entries */
  int ndeps;
  /** non-zero once execution completes */
  int done;
};

/**
 * @brief Initialize a DAG node.
 *
 * All fields are cleared and the execution context is set to @p ctx.
 */
void dag_node_init(struct dag_node *n, exo_cap ctx);

/** Set the scheduling priority for @p n. */
void dag_node_set_priority(struct dag_node *n, int priority);

/** Assign a weight used by weighted schedulers. */
void dag_node_set_weight(struct dag_node *n, int weight);

/**
 * @brief Record that @p child depends on @p parent.
 */
void dag_node_add_dep(struct dag_node *parent, struct dag_node *child);

/** Queue @p node if it has no pending dependencies. */
void dag_sched_submit(struct dag_node *node);

/** Initialize internal state of the DAG scheduler. */
void dag_sched_init(void);
