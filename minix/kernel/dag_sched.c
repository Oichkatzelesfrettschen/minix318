#include "types.h"
#include "defs.h"
#include "spinlock.h"
#include "dag.h"
#include "exo_stream.h"
#include "exo_cpu.h"
#include <string.h>

static struct spinlock dag_lock;
static struct dag_node *ready_head;

static struct exo_sched_ops dag_ops;
static struct exo_stream dag_stream;

/**
 * @brief Get the effective weight of a node.
 */
static inline int node_weight(struct dag_node *n) { return n->priority; }

/** Implementation of ::dag_node_init */
void dag_node_init(struct dag_node *n, exo_cap ctx) {
  memset(n, 0, sizeof(*n));
  n->ctx = ctx;
}

/** Implementation of ::dag_node_set_priority */
void dag_node_set_priority(struct dag_node *n, int priority) {
  n->priority = priority;
}

/** Implementation of ::dag_node_set_weight */
void dag_node_set_weight(struct dag_node *n, int weight) {
  n->weight = weight;
}

/** Implementation of ::dag_node_add_dep */
void dag_node_add_dep(struct dag_node *parent, struct dag_node *child) {
  struct dag_node_list *l = (struct dag_node_list *)kalloc();
  if (!l)
    return;
  l->node = child;
  l->next = parent->children;
  parent->children = l;
  child->pending++;
  if (child->deps == 0)
    child->deps = (struct dag_node **)kalloc();
  if (child->deps)
    child->deps[child->ndeps++] = parent;
}

/**
 * @brief Insert a node into the ready queue by weight.
 */
static void enqueue_ready(struct dag_node *n) {
  int w = node_weight(n);
  struct dag_node **pp = &ready_head;
  while (*pp && node_weight(*pp) >= w)

    pp = &(*pp)->next;
  n->next = *pp;
  *pp = n;
}

/** Implementation of ::dag_sched_submit */
void dag_sched_submit(struct dag_node *n) {
  acquire(&dag_lock);

  if (n->pending == 0 && !n->done)

    enqueue_ready(n);
  release(&dag_lock);
}

/**
 * @brief Remove the highest priority node from the ready queue.
 *
 * @return Node ready for execution or NULL if none exist.
 */
static struct dag_node *dequeue_ready(void) {
  struct dag_node *n = ready_head;
  if (n)
    ready_head = n->next;
  return n;
}

/**
 * @brief Mark a node as finished and update dependents.
 *
 * @param n Node that has completed execution.
 */
static void dag_mark_done(struct dag_node *n) {
  struct dag_node_list *l;
  n->done = 1;
  for (l = n->children; l; l = l->next) {

    struct dag_node *child = l->node;
    if (--child->pending == 0)
      enqueue_ready(child);
  }
}

/**
 * @brief Yield execution to the next ready node.
 */
static void dag_yield(void) {
  struct dag_node *n;

  acquire(&dag_lock);
  n = dequeue_ready();
  release(&dag_lock);

  if (!n)
    return;

  exo_yield_to(n->ctx);

  acquire(&dag_lock);
  dag_mark_done(n);
  release(&dag_lock);
}

/**
 * @brief Halt the scheduler (no-op).
 */
static void dag_halt(void) {
  /* nothing */
}

/**
 * @brief Set up the DAG scheduler structures and register with Exokernel.
 */
void dag_sched_init(void) {
  initlock(&dag_lock, "dag");
  dag_ops.halt = dag_halt;
  dag_ops.yield = dag_yield;
  dag_ops.next = 0;
  dag_stream.head = &dag_ops;
  exo_stream_register(&dag_stream);
}
