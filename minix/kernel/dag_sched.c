#include "dag.h"
#include "defs.h"
#include "exo_cpu.h"
#include "exo_stream.h"
#include "spinlock.h"
#include "types.h"
#include <string.h>

/**
 * @file dag_sched.c
 * @brief Weighted DAG scheduler implementation.
 *
 * Ready nodes are stored in a binary max-heap keyed by the node's
 * scheduling weight.  The scheduler always executes the node with the
 * highest weight whose dependencies are satisfied.
 */

static struct spinlock dag_lock;
#define DAG_HEAP_CAPACITY (PGSIZE / sizeof(struct dag_node *))
static struct dag_node *ready_heap[DAG_HEAP_CAPACITY];
static int ready_size;

static struct exo_sched_ops dag_ops;
static struct exo_stream dag_stream;

static inline int node_weight(struct dag_node *n) { return n->weight; }

void dag_node_init(struct dag_node *n, exo_cap ctx) {
  memset(n, 0, sizeof(*n));
  n->ctx = ctx;
}

void dag_node_set_priority(struct dag_node *n, int priority) {
  n->priority = priority;
}

/** Set the scheduling weight for a DAG node. */
void dag_node_set_weight(struct dag_node *n, int weight) { n->weight = weight; }

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

static void heap_push(struct dag_node *n) {
  if (ready_size >= DAG_HEAP_CAPACITY)
    return;
  int i = ready_size++;
  while (i > 0) {
    int p = (i - 1) / 2;
    if (ready_heap[p]->weight >= n->weight)
      break;
    ready_heap[i] = ready_heap[p];
    i = p;
  }
  ready_heap[i] = n;
}

void dag_sched_submit(struct dag_node *n) {
  acquire(&dag_lock);

  if (n->pending == 0 && !n->done)
    heap_push(n);
  release(&dag_lock);
}

static struct dag_node *heap_pop(void) {
  if (ready_size == 0)
    return 0;
  struct dag_node *n = ready_heap[0];
  struct dag_node *last = ready_heap[--ready_size];
  int i = 0;
  while (1) {
    int l = 2 * i + 1;
    int r = l + 1;
    if (l >= ready_size)
      break;
    int c = l;
    if (r < ready_size && ready_heap[r]->weight > ready_heap[l]->weight)
      c = r;
    if (last->weight >= ready_heap[c]->weight)
      break;
    ready_heap[i] = ready_heap[c];
    i = c;
  }
  ready_heap[i] = last;
  return n;
}

static void dag_mark_done(struct dag_node *n) {
  struct dag_node_list *l;
  n->done = 1;
  for (l = n->children; l; l = l->next) {

    struct dag_node *child = l->node;
    if (--child->pending == 0)
      heap_push(child);
  }
}

static void dag_yield(void) {
  struct dag_node *n;

  acquire(&dag_lock);
  n = heap_pop();
  release(&dag_lock);

  if (!n)
    return;

  exo_yield_to(n->ctx);

  acquire(&dag_lock);
  dag_mark_done(n);
  release(&dag_lock);
}

static void dag_halt(void) {
  // nothing
}

void dag_sched_init(void) {
  initlock(&dag_lock, "dag");
  ready_size = 0;
  dag_ops.halt = dag_halt;
  dag_ops.yield = dag_yield;
  dag_ops.next = 0;
  dag_stream.head = &dag_ops;
  exo_stream_register(&dag_stream);
}
