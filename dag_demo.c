#include "libos/sched.h"
#include "types.h"
#include "user.h"
#include <minix/dag.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static struct dag_node a, b, c;

static void setup(void) {
  exo_cap cap = {0};
  dag_node_init(&a, cap);
  dag_node_set_priority(&a, 10);
  dag_node_init(&b, cap);
  dag_node_set_priority(&b, 5);
  dag_node_init(&c, cap);
  dag_node_set_priority(&c, 1);
  dag_node_add_dep(&a, &c);
  dag_node_add_dep(&b, &c);
  dag_sched_submit(&a);
  dag_sched_submit(&b);
  dag_sched_submit(&c);
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  printf("DAG scheduler demo\n");
  setup();
  exo_stream_yield();
  exo_stream_yield();
  exo_stream_yield();
  return 0;
}
