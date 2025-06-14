/**
 * @file test_do_diagctl.c
 * @brief Unit tests for the do_diagctl system call function
 *
 * This file contains comprehensive unit tests for the do_diagctl function,
 * which handles diagnostic control operations in the MINIX kernel. The tests
 * cover all four diagnostic control codes and various edge cases.
 *
 * @section test_coverage Test Coverage
 * The test suite covers:
 * - DIAGCTL_CODE_DIAG: Diagnostic output functionality
 * - DIAGCTL_CODE_STACKTRACE: Stack trace generation
 * - DIAGCTL_CODE_REGISTER: Process registration for diagnostics
 * - DIAGCTL_CODE_UNREGISTER: Process unregistration from diagnostics
 * - Invalid operation codes and parameter validation
 * - Permission checks for privileged operations
 * - Error handling for data copy failures and invalid endpoints
 *
 * @section mock_framework Mock Framework
 * The file implements a comprehensive mocking framework that includes:
 * - Mock data structures (proc, priv, message, kmessages, kinfo)
 * - Mock function implementations with configurable return values
 * - Test helper functions for setup and mock object creation
 * - Output capture mechanism for verification
 *
 * @section test_cases Test Cases
 * 1. test_diagctl_diag_success() - Valid diagnostic output
 * 2. test_diagctl_diag_invalid_length_too_small() - Buffer length validation
 * (too small)
 * 3. test_diagctl_diag_invalid_length_too_large() - Buffer length validation
 * (too large)
 * 4. test_diagctl_diag_copy_failure() - Data copy error handling
 * 5. test_diagctl_stacktrace_success() - Valid stack trace request
 * 6. test_diagctl_stacktrace_invalid_endpoint() - Invalid endpoint handling
 * 7. test_diagctl_register_success() - Successful process registration
 * 8. test_diagctl_register_no_permission() - Permission denied for registration
 * 9. test_diagctl_unregister_success() - Successful process unregistration
 * 10. test_diagctl_unregister_no_permission() - Permission denied for
 * unregistration
 * 11. test_diagctl_invalid_code() - Invalid operation code handling
 *
 * @section constants Constants and Definitions
 * - DIAG_BUFSIZE: Maximum diagnostic buffer size (256 bytes)
 * - DIAGCTL_CODE_*: Operation codes for different diagnostic functions
 * - Error codes: OK, EINVAL, EPERM for function return values
 * - Process flags: SYS_PROC for system process identification
 *
 * @author Test implementation for MINIX kernel diagnostic control
 * @version 1.0
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Mock definitions for testing
#define OK 0
#define EINVAL 22
#define EPERM 1
#define KERNEL 0
#define TRUE 1
#define FALSE 0
#define SYS_PROC 0x01
#define DIAG_BUFSIZE 256
#define END_OF_KMESS '\0'
#define SIGKMESS 29

#define DIAGCTL_CODE_DIAG 1
#define DIAGCTL_CODE_STACKTRACE 2
#define DIAGCTL_CODE_REGISTER 3
#define DIAGCTL_CODE_UNREGISTER 4

typedef unsigned long vir_bytes;

// Mock structures
struct proc {
  int p_endpoint;
  struct priv *p_priv;
};

struct priv {
  int s_flags;
  int s_diag_sig;
};

typedef struct {
  struct {
    int code;
    vir_bytes buf;
    vir_bytes len;
    int endpt;
  } m_lsys_krn_sys_diagctl;
} message;

struct kmessages {
  int km_size;
} kmess = {0};

struct kinfo {
  int do_serial_debug;
} kinfo = {0};

// Mock function implementations
static int mock_data_copy_result = OK;
static int mock_isokendpt_result = 1;
static char mock_output[512] = {0};
static int mock_output_pos = 0;

int data_copy_vmcheck(struct proc *caller, int src_ep, vir_bytes src,
                      int dst_ep, vir_bytes dst, vir_bytes len) {
  if (mock_data_copy_result == OK && len <= DIAG_BUFSIZE) {
    memcpy((void *)dst, "test data", len < 9 ? len : 9);
  }
  return mock_data_copy_result;
}

int isokendpt(int endpoint, int *proc_nr) {
  if (mock_isokendpt_result) {
    *proc_nr = 1;
    return 1;
  }
  return 0;
}

void kputc(char c) {
  if (mock_output_pos < sizeof(mock_output) - 1) {
    mock_output[mock_output_pos++] = c;
  }
}

void kprintf_stub(const char *fmt, ...) {
  // Mock printf for testing
}

void proc_stacktrace(struct proc *p) {
  // Mock stacktrace function
}

struct proc *proc_addr(int proc_nr) {
  static struct proc mock_proc = {0};
  return &mock_proc;
}

struct priv *priv(struct proc *p) { return p->p_priv; }

void send_sig(int endpoint, int sig) {
  // Mock signal sending
}

// Include the function under test
int do_diagctl(struct proc *caller, message *m_ptr);

// Test helper functions
void setup_test() {
  mock_data_copy_result = OK;
  mock_isokendpt_result = 1;
  mock_output_pos = 0;
  memset(mock_output, 0, sizeof(mock_output));
  kmess.km_size = 0;
  kinfo.do_serial_debug = 0;
}

struct proc *create_mock_proc(int endpoint, int flags) {
  static struct proc mock_proc;
  static struct priv mock_priv;
  mock_proc.p_endpoint = endpoint;
  mock_proc.p_priv = &mock_priv;
  mock_priv.s_flags = flags;
  mock_priv.s_diag_sig = FALSE;
  return &mock_proc;
}

// Test cases
void test_diagctl_diag_success() {
  setup_test();
  struct proc *caller = create_mock_proc(100, 0);
  message msg = {0};
  msg.m_lsys_krn_sys_diagctl.code = DIAGCTL_CODE_DIAG;
  msg.m_lsys_krn_sys_diagctl.buf = 0x1000;
  msg.m_lsys_krn_sys_diagctl.len = 5;

  int result = do_diagctl(caller, &msg);

  assert(result == OK);
  assert(mock_output[mock_output_pos - 1] == END_OF_KMESS);
  printf("PASS: test_diagctl_diag_success\n");
}

void test_diagctl_diag_invalid_length_too_small() {
  setup_test();
  struct proc *caller = create_mock_proc(100, 0);
  message msg = {0};
  msg.m_lsys_krn_sys_diagctl.code = DIAGCTL_CODE_DIAG;
  msg.m_lsys_krn_sys_diagctl.buf = 0x1000;
  msg.m_lsys_krn_sys_diagctl.len = 0;

  int result = do_diagctl(caller, &msg);

  assert(result == EINVAL);
  printf("PASS: test_diagctl_diag_invalid_length_too_small\n");
}

void test_diagctl_diag_invalid_length_too_large() {
  setup_test();
  struct proc *caller = create_mock_proc(100, 0);
  message msg = {0};
  msg.m_lsys_krn_sys_diagctl.code = DIAGCTL_CODE_DIAG;
  msg.m_lsys_krn_sys_diagctl.buf = 0x1000;
  msg.m_lsys_krn_sys_diagctl.len = DIAG_BUFSIZE + 1;

  int result = do_diagctl(caller, &msg);

  assert(result == EINVAL);
  printf("PASS: test_diagctl_diag_invalid_length_too_large\n");
}

void test_diagctl_diag_copy_failure() {
  setup_test();
  mock_data_copy_result = -1;
  struct proc *caller = create_mock_proc(100, 0);
  message msg = {0};
  msg.m_lsys_krn_sys_diagctl.code = DIAGCTL_CODE_DIAG;
  msg.m_lsys_krn_sys_diagctl.buf = 0x1000;
  msg.m_lsys_krn_sys_diagctl.len = 5;

  int result = do_diagctl(caller, &msg);

  assert(result == -1);
  printf("PASS: test_diagctl_diag_copy_failure\n");
}

void test_diagctl_stacktrace_success() {
  setup_test();
  struct proc *caller = create_mock_proc(100, 0);
  message msg = {0};
  msg.m_lsys_krn_sys_diagctl.code = DIAGCTL_CODE_STACKTRACE;
  msg.m_lsys_krn_sys_diagctl.endpt = 200;

  int result = do_diagctl(caller, &msg);

  assert(result == OK);
  printf("PASS: test_diagctl_stacktrace_success\n");
}

void test_diagctl_stacktrace_invalid_endpoint() {
  setup_test();
  mock_isokendpt_result = 0;
  struct proc *caller = create_mock_proc(100, 0);
  message msg = {0};
  msg.m_lsys_krn_sys_diagctl.code = DIAGCTL_CODE_STACKTRACE;
  msg.m_lsys_krn_sys_diagctl.endpt = -1;

  int result = do_diagctl(caller, &msg);

  assert(result == EINVAL);
  printf("PASS: test_diagctl_stacktrace_invalid_endpoint\n");
}

void test_diagctl_register_success() {
  setup_test();
  struct proc *caller = create_mock_proc(100, SYS_PROC);
  message msg = {0};
  msg.m_lsys_krn_sys_diagctl.code = DIAGCTL_CODE_REGISTER;

  int result = do_diagctl(caller, &msg);

  assert(result == OK);
  assert(priv(caller)->s_diag_sig == TRUE);
  printf("PASS: test_diagctl_register_success\n");
}

void test_diagctl_register_no_permission() {
  setup_test();
  struct proc *caller = create_mock_proc(100, 0); // No SYS_PROC flag
  message msg = {0};
  msg.m_lsys_krn_sys_diagctl.code = DIAGCTL_CODE_REGISTER;

  int result = do_diagctl(caller, &msg);

  assert(result == EPERM);
  printf("PASS: test_diagctl_register_no_permission\n");
}

void test_diagctl_unregister_success() {
  setup_test();
  struct proc *caller = create_mock_proc(100, SYS_PROC);
  priv(caller)->s_diag_sig = TRUE; // Initially registered
  message msg = {0};
  msg.m_lsys_krn_sys_diagctl.code = DIAGCTL_CODE_UNREGISTER;

  int result = do_diagctl(caller, &msg);

  assert(result == OK);
  assert(priv(caller)->s_diag_sig == FALSE);
  printf("PASS: test_diagctl_unregister_success\n");
}

void test_diagctl_unregister_no_permission() {
  setup_test();
  struct proc *caller = create_mock_proc(100, 0); // No SYS_PROC flag
  message msg = {0};
  msg.m_lsys_krn_sys_diagctl.code = DIAGCTL_CODE_UNREGISTER;

  int result = do_diagctl(caller, &msg);

  assert(result == EPERM);
  printf("PASS: test_diagctl_unregister_no_permission\n");
}

void test_diagctl_invalid_code() {
  setup_test();
  struct proc *caller = create_mock_proc(100, 0);
  message msg = {0};
  msg.m_lsys_krn_sys_diagctl.code = 999; // Invalid code

  int result = do_diagctl(caller, &msg);

  assert(result == EINVAL);
  printf("PASS: test_diagctl_invalid_code\n");
}

int main() {
  printf("Running do_diagctl tests...\n\n");

  test_diagctl_diag_success();
  test_diagctl_diag_invalid_length_too_small();
  test_diagctl_diag_invalid_length_too_large();
  test_diagctl_diag_copy_failure();
  test_diagctl_stacktrace_success();
  test_diagctl_stacktrace_invalid_endpoint();
  test_diagctl_register_success();
  test_diagctl_register_no_permission();
  test_diagctl_unregister_success();
  test_diagctl_unregister_no_permission();
  test_diagctl_invalid_code();

  printf("\nAll tests passed!\n");
  return 0;
}

// Implementation of the function under test (copy from original file)
int do_diagctl(struct proc *caller, message *m_ptr) {
  vir_bytes len, buf;
  static char mybuf[DIAG_BUFSIZE];
  int s, i, proc_nr;

  switch (m_ptr->m_lsys_krn_sys_diagctl.code) {
  case DIAGCTL_CODE_DIAG:
    buf = m_ptr->m_lsys_krn_sys_diagctl.buf;
    len = m_ptr->m_lsys_krn_sys_diagctl.len;
    if (len < 1 || len > DIAG_BUFSIZE) {
      kprintf_stub("do_diagctl: diag for %d: len %d out of range\n",
                   caller->p_endpoint, len);
      return EINVAL;
    }
    if ((s = data_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
                               (vir_bytes)mybuf, len)) != OK) {
      kprintf_stub("do_diagctl: diag for %d: len %d: copy failed: %d\n",
                   caller->p_endpoint, len, s);
      return s;
    }
    for (i = 0; i < len; i++)
      kputc(mybuf[i]);
    kputc(END_OF_KMESS);
    return OK;
  case DIAGCTL_CODE_STACKTRACE:
    if (!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
      return EINVAL;
    proc_stacktrace(proc_addr(proc_nr));
    return OK;
  case DIAGCTL_CODE_REGISTER:
    if (!(priv(caller)->s_flags & SYS_PROC))
      return EPERM;
    priv(caller)->s_diag_sig = TRUE;
    if (kmess.km_size > 0 && !kinfo.do_serial_debug)
      send_sig(caller->p_endpoint, SIGKMESS);
    return OK;
  case DIAGCTL_CODE_UNREGISTER:
    if (!(priv(caller)->s_flags & SYS_PROC))
      return EPERM;
    priv(caller)->s_diag_sig = FALSE;
    return OK;
  default:
    kprintf_stub("do_diagctl: invalid request %d\n",
                 m_ptr->m_lsys_krn_sys_diagctl.code);
    return (EINVAL);
  }
}