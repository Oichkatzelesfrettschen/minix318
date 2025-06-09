#include "kernel/kernel.h"
#include "kernel/proc.h"
#include "kernel/system.h"
#include <assert.h>
#include <minix/endpoint.h>
#include <minix/type.h>

/* Mock functions and external dependencies */
extern int do_clear(struct proc *caller, message *m_ptr);

/* Test helper functions */
static void setup_test_process(struct proc *rp, endpoint_t endpt) {
  rp->p_endpoint = endpt;
  rp->p_rts_flags = 0;
  rp->p_misc_flags = 0;
}

static void setup_test_message(message *m, endpoint_t endpt) {
  m->m_lsys_krn_sys_clear.endpt = endpt;
}

/* Test cases */
void test_do_clear_valid_endpoint() {
  struct proc caller;
  struct proc target;
  message msg;
  endpoint_t valid_endpt = 5;
  int result;

  setup_test_process(&caller, 1);
  setup_test_process(&target, valid_endpt);
  setup_test_message(&msg, valid_endpt);

  result = do_clear(&caller, &msg);
  assert(result == OK);
  printf("test_do_clear_valid_endpoint: PASSED\n");
}

void test_do_clear_invalid_endpoint() {
  struct proc caller;
  message msg;
  endpoint_t invalid_endpt = -999;
  int result;

  setup_test_process(&caller, 1);
  setup_test_message(&msg, invalid_endpt);

  result = do_clear(&caller, &msg);
  assert(result == EINVAL);
  printf("test_do_clear_invalid_endpoint: PASSED\n");
}

void test_do_clear_already_cleared_process() {
  struct proc caller;
  struct proc target;
  message msg;
  endpoint_t endpt = 10;
  int result;

  setup_test_process(&caller, 1);
  setup_test_process(&target, endpt);
  target.p_rts_flags = RTS_SLOT_FREE; /* Already cleared */
  setup_test_message(&msg, endpt);

  result = do_clear(&caller, &msg);
  assert(result == OK); /* Should be safe to call multiple times */
  printf("test_do_clear_already_cleared_process: PASSED\n");
}

void test_do_clear_system_process() {
  struct proc caller;
  struct proc target;
  message msg;
  endpoint_t sys_endpt = 2;
  int result;

  setup_test_process(&caller, 1);
  setup_test_process(&target, sys_endpt);
  target.p_priv = &priv[0]; /* System process with privileges */
  setup_test_message(&msg, sys_endpt);

  result = do_clear(&caller, &msg);
  assert(result == OK);
  printf("test_do_clear_system_process: PASSED\n");
}

void test_do_clear_null_caller() {
  message msg;
  endpoint_t endpt = 5;
  int result;

  setup_test_message(&msg, endpt);

  result = do_clear(NULL, &msg);
  assert(result == EINVAL);
  printf("test_do_clear_null_caller: PASSED\n");
}

void test_do_clear_null_message() {
  struct proc caller;
  int result;

  setup_test_process(&caller, 1);

  result = do_clear(&caller, NULL);
  assert(result == EINVAL);
  printf("test_do_clear_null_message: PASSED\n");
}

/* Main test runner */
int main() {
  printf("Running do_clear tests...\n");

  test_do_clear_valid_endpoint();
  test_do_clear_invalid_endpoint();
  test_do_clear_already_cleared_process();
  test_do_clear_system_process();
  test_do_clear_null_caller();
  test_do_clear_null_message();

  printf("All tests completed successfully!\n");
  return 0;
}