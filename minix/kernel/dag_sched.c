/**
 * @file dag_sched.c
 * @brief Weighted DAG scheduler implementation.
 *
 * Ready nodes are stored in a dynamically sized binary max-heap keyed
 * by each node's scheduling weight.  The scheduler always executes the
 * highest-weight node whose dependencies are satisfied.
 *
 * The heap grows on demand using the legacy @c kalloc allocator.  This
 * keeps the ready queue efficient even as large lattices or resurrection
 * DAGs submit many nodes for execution.
 */

#include "dag.h"
#include "defs.h"
#include "exo_cpu.h"
#include "exo_stream.h"
#include "spinlock.h"
#include "types.h"
#include <stdlib.h>

/* Legacy allocation APIs */
extern void *kalloc();
extern void kfree(void *, unsigned long);
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Initial heap capacity (in nodes). */
#define DAG_HEAP_INITIAL_CAPACITY 32

/** Spinlock protecting the heap and node state. */
static struct spinlock dag_lock;

/** Binary max-heap of ready nodes. Allocated dynamically. */
static struct dag_node **ready_heap;

/** Current maximum capacity of the heap. */
static int ready_capacity;

/** Current number of elements in the heap. */
static int ready_size;

/** Exokernel scheduler operations for this DAG scheduler. */
static struct exo_sched_ops dag_ops;

/** Exokernel stream used to register the scheduler. */
static struct exo_stream dag_stream;

/**
 * @brief Retrieve a node’s scheduling weight.
 * @param n  Pointer to a DAG node.
 * @return   The node’s weight (higher ⇒ higher priority).
 */
static inline int node_weight(const struct dag_node *n) { return n->weight; }

/**
 * @brief Initialize a DAG node.
 * @param n    The node to initialize.
 * @param ctx  Execution context/capability assigned to this node.
 *
 * Zeroes all fields and sets up the context handle.
 */
void dag_node_init(struct dag_node *n, exo_cap ctx) {
  memset(n, 0, sizeof(*n));
  n->ctx = ctx;
}

/**
 * @brief Set the priority (informational) of a DAG node.
 * @param n         Node to update.
 * @param priority  Arbitrary priority value.
 */
void dag_node_set_priority(struct dag_node *n, int priority) {
  n->priority = priority;
}

/**
 * @brief Set the scheduling weight of a DAG node.
 * @param n       Node to update.
 * @param weight  Weight used to order execution (higher ⇒ runs sooner).
 */
void dag_node_set_weight(struct dag_node *n, int weight) { n->weight = weight; }

/**
 * @brief Declare a dependency between two nodes.
 * @param parent  Node that must complete first.
 * @param child   Node that depends on @p parent.
 *
 * Increments @p child’s pending count and links it into @p parent’s
 * child list.  If memory allocation fails, the dependency is dropped.
 */
void dag_node_add_dep(struct dag_node *parent, struct dag_node *child) {
  struct dag_node_list *entry = (struct dag_node_list *)kalloc();
  if (!entry) {
    return;
  }
  entry->node = child;
  entry->next = parent->children;
  parent->children = entry;
  child->pending++;
}

/**
 * @brief Ensure the ready heap can store at least one more node.
 *
 * Called whenever a new node is inserted.  When the array is full,
 * it doubles the heap capacity using @c kalloc and copies existing
 * entries to the new storage.
 *
 * @return true on success, false on allocation failure.
 */
static bool heap_reserve(void) {
  if (ready_size < ready_capacity) {
    return true;
  }

  int new_cap = ready_capacity ? ready_capacity * 2 : DAG_HEAP_INITIAL_CAPACITY;
  struct dag_node **new_heap =
      (struct dag_node **)kalloc(new_cap * sizeof(struct dag_node *));
  if (!new_heap) {
    printf("dag_sched: heap allocation failed\n");
    return false;
  }
  if (ready_heap) {
    memcpy(new_heap, ready_heap, ready_size * sizeof(*ready_heap));
    kfree((void *)ready_heap, ready_capacity * sizeof(*ready_heap));
  }
  ready_heap = new_heap;
  ready_capacity = new_cap;
  return true;
}

/**
 * @brief Push a ready node into the max-heap.
 * @param n  Node with no remaining dependencies.
 *
 * Maintains the heap invariant: parent.weight ≥ child.weight.
 */
static void heap_push(struct dag_node *n) {
  if (!heap_reserve()) {
    return;
  }

  int i = ready_size++;
  while (i > 0) {
    int p = (i - 1) / 2;
    if (node_weight(ready_heap[p]) >= node_weight(n)) {
      break;
    }
    ready_heap[i] = ready_heap[p];
    i = p;
  }
  ready_heap[i] = n;
}

/**
 * @brief Pop the highest-weight node from the max-heap.
 * @return  The next node to execute, or NULL if none are ready.
 */
static struct dag_node *heap_pop(void) {
  if (ready_size == 0) {
    return NULL;
  }

  struct dag_node *top = ready_heap[0];
  struct dag_node *last = ready_heap[--ready_size];
  int i = 0;

  while (1) {
    int l = 2 * i + 1;
    if (l >= ready_size) {
      break;
    }
    int c = l;
    int r = l + 1;
    if (r < ready_size &&
        node_weight(ready_heap[r]) > node_weight(ready_heap[l])) {
      c = r;
    }
    if (node_weight(last) >= node_weight(ready_heap[c])) {
      break;
    }
    ready_heap[i] = ready_heap[c];
    i = c;
  }
  ready_heap[i] = last;
  return top;
}

/**
 * @brief Submit a node for scheduling.
 * @param n  Node to schedule.
 *
 * If the node has zero pending dependencies and isn’t already done,
 * it’s pushed into the ready-heap under lock.
 */
void dag_sched_submit(struct dag_node *n) {
  acquire(&dag_lock);
  if (n->pending == 0 && !n->done) {
    heap_push(n);
  }
  release(&dag_lock);
}

/**
 * @brief Mark a node as complete and wake dependents.
 * @param n  The completed node.
 *
 * Sets its done flag, decrements each child’s pending count, and
 * pushes any child with zero pending onto the ready-heap.
 */
static void dag_mark_done(struct dag_node *n) {
  n->done = 1;
  for (struct dag_node_list *l = n->children; l; l = l->next) {
    struct dag_node *child = l->node;
    if (--child->pending == 0) {
      heap_push(child);
    }
  }
}

/**
 * @brief Yield control to the next ready node.
 *
 * Pops the top node from the heap, context-switches to it, then
 * on its return marks it done (under lock).
 */
static void dag_yield(void) {
  struct dag_node *next;

  acquire(&dag_lock);
  next = heap_pop();
  release(&dag_lock);

  if (!next) {
    return;
  }

  exo_yield_to(next->ctx);

  acquire(&dag_lock);
  dag_mark_done(next);
  release(&dag_lock);
}

/**
 * @brief Halt the scheduler (no further work).
 */
static void dag_halt(void) { /* No-op */ }

/**
 * @brief Initialize and register the DAG scheduler with Exokernel.
 *
 * - Initializes the spinlock and heap size
 * - Sets up dag_ops (halt/yield)
 * - Registers dag_stream with the Exo framework
 * - Allocates the initial ready-heap storage
 */
void dag_sched_init(void) {
  initlock(&dag_lock, "dag");
  ready_size = 0;
  ready_capacity = 0;
  ready_heap = NULL;
  heap_reserve();

  dag_ops.halt = dag_halt;
  dag_ops.yield = dag_yield;
  dag_ops.next = NULL; /**< Not used in this scheduler */

  dag_stream.head = &dag_ops;
  exo_stream_register(&dag_stream);
}
