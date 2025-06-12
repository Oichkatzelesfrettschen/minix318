#include "dag.h"
#include "defs.h"
#include "exo_cpu.h"
#include "exo_stream.h"
#include "spinlock.h"
#include "types.h"
#include <string.h>

static struct spinlock dag_lock;
static struct dag_node *ready_head;

static struct exo_sched_ops dag_ops;
static struct exo_stream dag_stream;

/**
 * @brief Return the priority weight of @p n.
 */
static inline int node_weight(struct dag_node *n) { return n->priority; }

/**
 * @brief Initialize a DAG node instance.
 */
void dag_node_init(struct dag_node *n, exo_cap ctx) {
  memset(n, 0, sizeof(*n));
  n->ctx = ctx;
}

/**
 * @brief Set the priority value for @p n.
 */
void dag_node_set_priority(struct dag_node *n, int priority) {
  n->priority = priority;
}

/**
 * @brief Set the weight value for @p n.
 */
void dag_node_set_weight(struct dag_node *n, int weight) { n->weight = weight; }

/**
 * @brief Register that @p child depends on @p parent.
 */
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

/** Place @p n into the ready queue ordered by weight. */
static void enqueue_ready(struct dag_node *n) {
  int w = node_weight(n);
  struct dag_node **pp = &ready_head;
  while (*pp && node_weight(*pp) >= w)

    pp = &(*pp)->next;
  n->next = *pp;
  *pp = n;
}

/**
 * @brief Submit a node to the scheduler.
 */
void dag_sched_submit(struct dag_node *n) {
  acquire(&dag_lock);

  if (n->pending == 0 && !n->done)

    enqueue_ready(n);
  release(&dag_lock);
}

/** Remove and return the next ready node. */
static struct dag_node *dequeue_ready(void) {
  struct dag_node *n = ready_head;
  if (n)
    ready_head = n->next;
  return n;
}

/** Mark @p n as complete and update dependents. */
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

/** Nothing to do when the scheduler halts. */
static void dag_halt(void) {}

/** Initialize the DAG scheduler and register it with Exo. */
void dag_sched_init(void) {
  initlock(&dag_lock, "dag");
  dag_ops.halt = dag_halt;
  dag_ops.yield = dag_yield;
  dag_ops.next = 0;
  dag_stream.head = &dag_ops;
  exo_stream_register(&dag_stream);
}
