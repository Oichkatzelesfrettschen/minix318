#pragma once
#include "types.h"
#include "exo.h"

struct dag_node;

struct dag_node_list {
  struct dag_node *node;
  exo_cap cap;
  struct dag_node_list *next;
};

struct dag_node {
  exo_cap ctx;
  int pending;
  int priority;
  struct dag_node_list *children;
  struct dag_node *next;
  /* parents this node depends on */
  struct dag_node **deps;
  /* number of valid entries in deps */
  int ndeps;
  /* set once the node has run */
  int done;

};

void dag_node_init(struct dag_node *n, exo_cap ctx);
void dag_node_set_priority(struct dag_node *n, int priority);
void dag_node_add_dep(struct dag_node *parent, struct dag_node *child);
void dag_sched_submit(struct dag_node *node);
void dag_sched_init(void);

