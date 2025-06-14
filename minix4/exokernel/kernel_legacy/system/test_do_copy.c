#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Mock definitions for testing
#define USE_VIRCOPY 1
#define USE_PHYSCOPY 1
#define _SRC_ 0
#define _DST_ 1
#define SELF -1
#define NONE -2
#define VFS_PROC_NR 100
#define CP_FLAG_TRY 0x01
#define EINVAL 22
#define E2BIG 7
#define EFAULT 14
#define EFAULT_SRC 1001
#define EFAULT_DST 1002

typedef unsigned long phys_bytes;
typedef unsigned int vir_bytes;
typedef int endpoint_t;

struct vir_addr {
  endpoint_t proc_nr_e;
  vir_bytes offset;
};

struct proc {
  endpoint_t p_endpoint;
};

typedef struct {
  struct {
    endpoint_t src_endpt;
    endpoint_t dst_endpt;
    vir_bytes src_addr;
    vir_bytes dst_addr;
    phys_bytes nr_bytes;
    int flags;
  } m_lsys_krn_sys_copy;
} message;

// Mock functions
int isokendpt_result = 1;
int isokendpt(endpoint_t ep, int *p) {
  if (p)
    *p = ep;
  return isokendpt_result;
}

int virtual_copy_result = 0;
int virtual_copy(struct vir_addr *src, struct vir_addr *dst, phys_bytes bytes) {
  return virtual_copy_result;
}

int virtual_copy_vmcheck_result = 0;
int virtual_copy_vmcheck(struct proc *caller, struct vir_addr *src,
                         struct vir_addr *dst, phys_bytes bytes) {
  return virtual_copy_vmcheck_result;
}

void kprintf_stub(const char *fmt, ...) {
  // Mock printf for testing
}

#define KASSERT(expr) assert(expr)

// Include the function under test
int do_copy(struct proc *caller, message *m_ptr) {
  struct vir_addr vir_addr[2];
  phys_bytes bytes;
  int i;

  vir_addr[_SRC_].proc_nr_e = m_ptr->m_lsys_krn_sys_copy.src_endpt;
  vir_addr[_DST_].proc_nr_e = m_ptr->m_lsys_krn_sys_copy.dst_endpt;
  vir_addr[_SRC_].offset = m_ptr->m_lsys_krn_sys_copy.src_addr;
  vir_addr[_DST_].offset = m_ptr->m_lsys_krn_sys_copy.dst_addr;
  bytes = m_ptr->m_lsys_krn_sys_copy.nr_bytes;

  for (i = _SRC_; i <= _DST_; i++) {
    int p;
    if (vir_addr[i].proc_nr_e == SELF)
      vir_addr[i].proc_nr_e = caller->p_endpoint;
    if (vir_addr[i].proc_nr_e != NONE) {
      if (!isokendpt(vir_addr[i].proc_nr_e, &p)) {
        return EINVAL;
      }
    }
  }

  if (bytes != (phys_bytes)(vir_bytes)bytes)
    return E2BIG;

  if (m_ptr->m_lsys_krn_sys_copy.flags & CP_FLAG_TRY) {
    int r;
    assert(caller->p_endpoint == VFS_PROC_NR);
    r = virtual_copy(&vir_addr[_SRC_], &vir_addr[_DST_], bytes);
    if (r == EFAULT_SRC || r == EFAULT_DST)
      return EFAULT;
    return r;
  } else {
    return virtual_copy_vmcheck(caller, &vir_addr[_SRC_], &vir_addr[_DST_],
                                bytes);
  }
}

// Test functions
void test_successful_copy() {
  printf("Running test_successful_copy...\n");

  struct proc caller = {.p_endpoint = 123};
  message msg = {0};
  msg.m_lsys_krn_sys_copy.src_endpt = 100;
  msg.m_lsys_krn_sys_copy.dst_endpt = 200;
  msg.m_lsys_krn_sys_copy.src_addr = 0x1000;
  msg.m_lsys_krn_sys_copy.dst_addr = 0x2000;
  msg.m_lsys_krn_sys_copy.nr_bytes = 1024;
  msg.m_lsys_krn_sys_copy.flags = 0;

  isokendpt_result = 1;
  virtual_copy_vmcheck_result = 0;

  int result = do_copy(&caller, &msg);
  assert(result == 0);
  printf("PASSED\n");
}

void test_self_endpoint_resolution() {
  printf("Running test_self_endpoint_resolution...\n");

  struct proc caller = {.p_endpoint = 123};
  message msg = {0};
  msg.m_lsys_krn_sys_copy.src_endpt = SELF;
  msg.m_lsys_krn_sys_copy.dst_endpt = 200;
  msg.m_lsys_krn_sys_copy.src_addr = 0x1000;
  msg.m_lsys_krn_sys_copy.dst_addr = 0x2000;
  msg.m_lsys_krn_sys_copy.nr_bytes = 1024;
  msg.m_lsys_krn_sys_copy.flags = 0;

  isokendpt_result = 1;
  virtual_copy_vmcheck_result = 0;

  int result = do_copy(&caller, &msg);
  assert(result == 0);
  printf("PASSED\n");
}

void test_invalid_endpoint() {
  printf("Running test_invalid_endpoint...\n");

  struct proc caller = {.p_endpoint = 123};
  message msg = {0};
  msg.m_lsys_krn_sys_copy.src_endpt = 100;
  msg.m_lsys_krn_sys_copy.dst_endpt = 200;
  msg.m_lsys_krn_sys_copy.src_addr = 0x1000;
  msg.m_lsys_krn_sys_copy.dst_addr = 0x2000;
  msg.m_lsys_krn_sys_copy.nr_bytes = 1024;
  msg.m_lsys_krn_sys_copy.flags = 0;

  isokendpt_result = 0; // Invalid endpoint

  int result = do_copy(&caller, &msg);
  assert(result == EINVAL);
  printf("PASSED\n");
}

void test_byte_overflow() {
  printf("Running test_byte_overflow...\n");

  struct proc caller = {.p_endpoint = 123};
  message msg = {0};
  msg.m_lsys_krn_sys_copy.src_endpt = 100;
  msg.m_lsys_krn_sys_copy.dst_endpt = 200;
  msg.m_lsys_krn_sys_copy.src_addr = 0x1000;
  msg.m_lsys_krn_sys_copy.dst_addr = 0x2000;
  msg.m_lsys_krn_sys_copy.nr_bytes =
      (phys_bytes)-1; // Large value causing overflow
  msg.m_lsys_krn_sys_copy.flags = 0;

  isokendpt_result = 1;

  int result = do_copy(&caller, &msg);
  assert(result == E2BIG);
  printf("PASSED\n");
}

void test_cp_flag_try_success() {
  printf("Running test_cp_flag_try_success...\n");

  struct proc caller = {.p_endpoint = VFS_PROC_NR};
  message msg = {0};
  msg.m_lsys_krn_sys_copy.src_endpt = 100;
  msg.m_lsys_krn_sys_copy.dst_endpt = 200;
  msg.m_lsys_krn_sys_copy.src_addr = 0x1000;
  msg.m_lsys_krn_sys_copy.dst_addr = 0x2000;
  msg.m_lsys_krn_sys_copy.nr_bytes = 1024;
  msg.m_lsys_krn_sys_copy.flags = CP_FLAG_TRY;

  isokendpt_result = 1;
  virtual_copy_result = 0;

  int result = do_copy(&caller, &msg);
  assert(result == 0);
  printf("PASSED\n");
}

void test_cp_flag_try_fault() {
  printf("Running test_cp_flag_try_fault...\n");

  struct proc caller = {.p_endpoint = VFS_PROC_NR};
  message msg = {0};
  msg.m_lsys_krn_sys_copy.src_endpt = 100;
  msg.m_lsys_krn_sys_copy.dst_endpt = 200;
  msg.m_lsys_krn_sys_copy.src_addr = 0x1000;
  msg.m_lsys_krn_sys_copy.dst_addr = 0x2000;
  msg.m_lsys_krn_sys_copy.nr_bytes = 1024;
  msg.m_lsys_krn_sys_copy.flags = CP_FLAG_TRY;

  isokendpt_result = 1;
  virtual_copy_result = EFAULT_SRC;

  int result = do_copy(&caller, &msg);
  assert(result == EFAULT);
  printf("PASSED\n");
}

void test_none_endpoint() {
  printf("Running test_none_endpoint...\n");

  struct proc caller = {.p_endpoint = 123};
  message msg = {0};
  msg.m_lsys_krn_sys_copy.src_endpt = NONE;
  msg.m_lsys_krn_sys_copy.dst_endpt = NONE;
  msg.m_lsys_krn_sys_copy.src_addr = 0x1000;
  msg.m_lsys_krn_sys_copy.dst_addr = 0x2000;
  msg.m_lsys_krn_sys_copy.nr_bytes = 1024;
  msg.m_lsys_krn_sys_copy.flags = 0;

  virtual_copy_vmcheck_result = 0;

  int result = do_copy(&caller, &msg);
  assert(result == 0);
  printf("PASSED\n");
}

void test_virtual_copy_vmcheck_error() {
  printf("Running test_virtual_copy_vmcheck_error...\n");

  struct proc caller = {.p_endpoint = 123};
  message msg = {0};
  msg.m_lsys_krn_sys_copy.src_endpt = 100;
  msg.m_lsys_krn_sys_copy.dst_endpt = 200;
  msg.m_lsys_krn_sys_copy.src_addr = 0x1000;
  msg.m_lsys_krn_sys_copy.dst_addr = 0x2000;
  msg.m_lsys_krn_sys_copy.nr_bytes = 1024;
  msg.m_lsys_krn_sys_copy.flags = 0;

  isokendpt_result = 1;
  virtual_copy_vmcheck_result = -1; // Error code

  int result = do_copy(&caller, &msg);
  assert(result == -1);
  printf("PASSED\n");
}

int main() {
  printf("Starting do_copy tests...\n\n");

  test_successful_copy();
  test_self_endpoint_resolution();
  test_invalid_endpoint();
  test_byte_overflow();
  test_cp_flag_try_success();
  test_cp_flag_try_fault();
  test_none_endpoint();
  test_virtual_copy_vmcheck_error();

  printf("\nAll tests passed!\n");
  return 0;
}