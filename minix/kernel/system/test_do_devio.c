#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Mock definitions for testing
#define USE_DEVIO 1
#define _DIO_BYTE 0x01
#define _DIO_WORD 0x02
#define _DIO_LONG 0x04
#define _DIO_INPUT 0x10
#define _DIO_OUTPUT 0x20
#define _DIO_TYPEMASK 0x0F
#define _DIO_DIRMASK 0xF0
#define CHECK_IO_PORT 0x01
#define OK 0
#define EPERM 1
#define EINVAL 22

typedef unsigned short port_t;

struct io_range {
  port_t ior_base;
  port_t ior_limit;
};

struct priv {
  int s_flags;
  int s_nr_io_range;
  struct io_range *s_io_tab;
};

struct proc {
  struct priv *p_priv;
};

typedef struct {
  struct {
    int request;
    port_t port;
    unsigned long value;
  } m_lsys_krn_sys_devio;
  struct {
    unsigned long value;
  } m_krn_lsys_sys_devio;
} message;

// Mock functions
void kprintf_stub(const char *fmt, ...) { /* stub */ }
struct priv *priv(struct proc *p) { return p->p_priv; }
unsigned char inb(port_t port) { return 0x42; }
unsigned short inw(port_t port) { return 0x1234; }
unsigned long inl(port_t port) { return 0x12345678; }
void outb(port_t port, unsigned char value) { /* stub */ }
void outw(port_t port, unsigned short value) { /* stub */ }
void outl(port_t port, unsigned long value) { /* stub */ }

// Include the function under test
int do_devio(struct proc *caller, message *m_ptr);

// Test fixtures
static struct priv test_priv;
static struct proc test_proc;
static struct io_range test_io_ranges[2];
static message test_msg;

void setup_test() {
  memset(&test_priv, 0, sizeof(test_priv));
  memset(&test_proc, 0, sizeof(test_proc));
  memset(test_io_ranges, 0, sizeof(test_io_ranges));
  memset(&test_msg, 0, sizeof(test_msg));

  test_proc.p_priv = &test_priv;
  test_priv.s_flags = CHECK_IO_PORT;
  test_priv.s_nr_io_range = 2;
  test_priv.s_io_tab = test_io_ranges;

  // Setup allowed I/O ranges
  test_io_ranges[0].ior_base = 0x100;
  test_io_ranges[0].ior_limit = 0x1FF;
  test_io_ranges[1].ior_base = 0x300;
  test_io_ranges[1].ior_limit = 0x3FF;
}

void test_byte_input() {
  setup_test();
  test_msg.m_lsys_krn_sys_devio.request = _DIO_BYTE | _DIO_INPUT;
  test_msg.m_lsys_krn_sys_devio.port = 0x150;

  int result = do_devio(&test_proc, &test_msg);
  assert(result == OK);
  printf("PASS: test_byte_input\n");
}

void test_word_input() {
  setup_test();
  test_msg.m_lsys_krn_sys_devio.request = _DIO_WORD | _DIO_INPUT;
  test_msg.m_lsys_krn_sys_devio.port = 0x150;

  int result = do_devio(&test_proc, &test_msg);
  assert(result == OK);
  printf("PASS: test_word_input\n");
}

void test_long_input() {
  setup_test();
  test_msg.m_lsys_krn_sys_devio.request = _DIO_LONG | _DIO_INPUT;
  test_msg.m_lsys_krn_sys_devio.port = 0x150;

  int result = do_devio(&test_proc, &test_msg);
  assert(result == OK);
  printf("PASS: test_long_input\n");
}

void test_byte_output() {
  setup_test();
  test_msg.m_lsys_krn_sys_devio.request = _DIO_BYTE | _DIO_OUTPUT;
  test_msg.m_lsys_krn_sys_devio.port = 0x150;
  test_msg.m_lsys_krn_sys_devio.value = 0x42;

  int result = do_devio(&test_proc, &test_msg);
  assert(result == OK);
  printf("PASS: test_byte_output\n");
}

void test_word_output() {
  setup_test();
  test_msg.m_lsys_krn_sys_devio.request = _DIO_WORD | _DIO_OUTPUT;
  test_msg.m_lsys_krn_sys_devio.port = 0x150;
  test_msg.m_lsys_krn_sys_devio.value = 0x1234;

  int result = do_devio(&test_proc, &test_msg);
  assert(result == OK);
  printf("PASS: test_word_output\n");
}

void test_long_output() {
  setup_test();
  test_msg.m_lsys_krn_sys_devio.request = _DIO_LONG | _DIO_OUTPUT;
  test_msg.m_lsys_krn_sys_devio.port = 0x150;
  test_msg.m_lsys_krn_sys_devio.value = 0x12345678;

  int result = do_devio(&test_proc, &test_msg);
  assert(result == OK);
  printf("PASS: test_long_output\n");
}

void test_no_privileges() {
  setup_test();
  test_priv.s_flags = 0; // Remove CHECK_IO_PORT flag
  test_msg.m_lsys_krn_sys_devio.request = _DIO_BYTE | _DIO_INPUT;
  test_msg.m_lsys_krn_sys_devio.port = 0x150;

  int result = do_devio(&test_proc, &test_msg);
  assert(result == EPERM);
  printf("PASS: test_no_privileges\n");
}

void test_port_out_of_range() {
  setup_test();
  test_msg.m_lsys_krn_sys_devio.request = _DIO_BYTE | _DIO_INPUT;
  test_msg.m_lsys_krn_sys_devio.port = 0x500; // Outside allowed ranges

  int result = do_devio(&test_proc, &test_msg);
  assert(result == EPERM);
  printf("PASS: test_port_out_of_range\n");
}

void test_invalid_io_type() {
  setup_test();
  test_msg.m_lsys_krn_sys_devio.request = 0xFF | _DIO_INPUT; // Invalid type
  test_msg.m_lsys_krn_sys_devio.port = 0x150;

  int result = do_devio(&test_proc, &test_msg);
  assert(result == EINVAL);
  printf("PASS: test_invalid_io_type\n");
}

void test_null_priv() {
  setup_test();
  test_proc.p_priv = NULL;
  test_msg.m_lsys_krn_sys_devio.request = _DIO_BYTE | _DIO_INPUT;
  test_msg.m_lsys_krn_sys_devio.port = 0x150;

  int result = do_devio(&test_proc, &test_msg);
  assert(result == EPERM);
  printf("PASS: test_null_priv\n");
}

int main() {
  printf("Running do_devio tests...\n");

  test_byte_input();
  test_word_input();
  test_long_input();
  test_byte_output();
  test_word_output();
  test_long_output();
  test_no_privileges();
  test_port_out_of_range();
  test_invalid_io_type();
  test_null_priv();

  printf("All tests passed!\n");
  return 0;
}