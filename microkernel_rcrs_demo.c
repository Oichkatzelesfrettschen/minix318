#include <stdio.h>
#include <stdlib.h>

/** Simplified capability representation used by the demo. */
typedef struct {
  int id; /**< Identifier for the capability. */
} exo_cap;

/** Register with the microkernel (stub implementation). */
static inline void microkernel_register(void) {
  puts("microkernel registered");
}

/** Allocate a page capability (stub implementation). */
static inline exo_cap mk_cap_alloc_page(void) {
  puts("allocate page");
  return (exo_cap){.id = 1};
}

/** Release a page capability (stub implementation). */
static inline void mk_cap_free_page(exo_cap cap) {
  (void)cap;
  puts("free page");
}

/** Type of a lambda step function. */
typedef int (*lambda_step_t)(void *);

/** Lambda capability used to invoke callbacks. */
typedef struct {
  lambda_step_t step; /**< Function to execute. */
  void *env;          /**< Environment pointer. */
} lambda_cap_t;

/** Create a lambda capability. */
static inline lambda_cap_t *mk_lambda_cap_create(lambda_step_t step, int flags,
                                                 exo_cap cap) {
  (void)flags;
  (void)cap;
  lambda_cap_t *lc = malloc(sizeof(*lc));
  if (lc != NULL) {
    lc->step = step;
    lc->env = NULL;
  }
  return lc;
}

/** Invoke a lambda capability a number of times. */
static inline void mk_lambda_cap_use(lambda_cap_t *lc, int times) {
  for (int i = 0; i < times; ++i) {
    lc->step(lc->env);
  }
}

/** Destroy a lambda capability. */
static inline void mk_lambda_cap_destroy(lambda_cap_t *lc) { free(lc); }

/** Route a message to a destination capability (stub). */
static inline void mk_route_message(exo_cap dest, const char *msg, size_t len) {
  (void)dest;
  fwrite(msg, 1, len, stdout);
  fputc('\n', stdout);
}

static int print_step(void *env) {
  (void)env;
  puts("demo lambda executed");
  return 1;
}

int main(void) {
  microkernel_register();

  exo_cap page = mk_cap_alloc_page();
  mk_cap_free_page(page);

  lambda_cap_t *lc = mk_lambda_cap_create(print_step, 0, (exo_cap){0});
  mk_lambda_cap_use(lc, 1);
  mk_lambda_cap_destroy(lc);

  const char msg[] = "hello";
  mk_route_message((exo_cap){0}, msg, sizeof(msg));

  return 0;
}
