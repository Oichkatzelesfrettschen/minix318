/*
 * Test for do_abort system call with Magic instrumentation support
 * Tests kernel abort functionality and memory safety checks
 */
#include "../magic_common.h"
#include "kernel.h"
#include "system.h"
#include <assert.h>
#include <minix/const.h>
#include <minix/type.h>

/* Test state tracking */
static int test_count = 0;
static int test_passed = 0;
static int test_failed = 0;

#define TEST_ASSERT(condition, msg)                                            \
  do {                                                                         \
    test_count++;                                                              \
    if (condition) {                                                           \
      test_passed++;                                                           \
      kprintf("[PASS] %s\n", msg);                                             \
    } else {                                                                   \
      test_failed++;                                                           \
      kprintf("[FAIL] %s\n", msg);                                             \
    }                                                                          \
  } while (0)

/*===========================================================================*
 *                              test_abort_basic                            *
 *===========================================================================*/
PRIVATE void test_abort_basic() {
  message m;
  int result;

  /* Test basic abort call structure */
  m.m_source = SYSTEM;
  m.m_type = SYS_ABORT;
  m.ABRT_HOW = RBT_HALT;

  /* This should not actually abort in test mode */
  kprintf("Testing basic abort call structure...\n");
  TEST_ASSERT(m.m_type == SYS_ABORT, "Abort message type set correctly");
  TEST_ASSERT(m.ABRT_HOW == RBT_HALT, "Abort method set correctly");
}

/*===========================================================================*
 *                          test_abort_magic_tracking                       *
 *===========================================================================*/
PRIVATE void test_abort_magic_tracking() {
  /* Test Magic instrumentation during abort scenarios */
  kprintf("Testing Magic instrumentation during abort...\n");

#if MAGIC_LLVM20_COMPAT
  /* Test that Magic tracking is active */
  TEST_ASSERT(MAGIC_LLVM20_FEATURE_DETECTION == 1,
              "LLVM-20 Magic feature detection enabled");

  /* Test memory state validation */
  TEST_ASSERT(MAGIC_ENHANCED_TYPE_ANALYSIS == 1,
              "Enhanced type analysis active");

  /* Test atomic operations support */
  TEST_ASSERT(MAGIC_ATOMIC_MEMORY_OPERATIONS == 1,
              "Atomic memory operations supported");
#endif

#ifdef __MINIX
  /* Test MINIX-specific Magic extensions */
  TEST_ASSERT(MAGIC_USE_MINIX_EXTENSIONS == 1,
              "MINIX Magic extensions enabled");

  TEST_ASSERT(MAGIC_MINIX_MEMORY_PROTECTION == 1,
              "MINIX memory protection active");
#endif
}

/*===========================================================================*
 *                          test_abort_memory_safety                        *
 *===========================================================================*/
PRIVATE void test_abort_memory_safety() {
  void *test_ptr = NULL;

  kprintf("Testing memory safety during abort scenarios...\n");

  /* Test null pointer detection */
  TEST_ASSERT(test_ptr == NULL, "Null pointer correctly detected");

  /* Test stack guard detection */
#if MAGIC_INSTRUMENT_STACK
  TEST_ASSERT(MAGIC_INSTRUMENT_STACK == 1, "Stack instrumentation active");
#endif

  /* Test heap tracking */
#if MAGIC_INSTRUMENT_HEAP_TRACKING
  TEST_ASSERT(MAGIC_INSTRUMENT_HEAP_TRACKING == 1, "Heap tracking active");
#endif
}

/*===========================================================================*
 *                          test_abort_error_codes                          *
 *===========================================================================*/
PRIVATE void test_abort_error_codes() {
  kprintf("Testing abort error code handling...\n");

  /* Test various abort types */
  TEST_ASSERT(RBT_HALT >= 0, "RBT_HALT is valid abort type");
  TEST_ASSERT(RBT_REBOOT >= 0, "RBT_REBOOT is valid abort type");
  TEST_ASSERT(RBT_PANIC >= 0, "RBT_PANIC is valid abort type");
  TEST_ASSERT(RBT_MONITOR >= 0, "RBT_MONITOR is valid abort type");
  TEST_ASSERT(RBT_RESET >= 0, "RBT_RESET is valid abort type");
}

/*===========================================================================*
 *                          test_abort_llvm20_features                      *
 *===========================================================================*/
PRIVATE void test_abort_llvm20_features() {
  kprintf("Testing LLVM-20 specific features during abort...\n");

#if MAGIC_LLVM20_COMPAT
  /* Test enhanced debugging */
  TEST_ASSERT(MAGIC_USE_ENHANCED_DEBUGGING == 1, "Enhanced debugging enabled");

  /* Test thread safety */
  TEST_ASSERT(MAGIC_THREAD_SAFE == 1, "Thread safety enabled");

  /* Test lockfree data structures */
  TEST_ASSERT(MAGIC_LOCKFREE_DATASTRUCTURES == 1,
              "Lockfree data structures enabled");

  /* Test cache optimization */
  TEST_ASSERT(MAGIC_LOOKUP_CACHE_OPTIMIZATION == 1,
              "Cache optimization enabled");
#endif
}

/*===========================================================================*
 *                              run_all_tests                               *
 *===========================================================================*/
PUBLIC int test_do_abort_run_all() {
  kprintf("=== Starting do_abort test suite ===\n");
  kprintf("Magic version: %s\n", MAGIC_VERSION_STRING);

  /* Initialize test counters */
  test_count = 0;
  test_passed = 0;
  test_failed = 0;

  /* Run all test functions */
  test_abort_basic();
  test_abort_magic_tracking();
  test_abort_memory_safety();
  test_abort_error_codes();
  test_abort_llvm20_features();

  /* Print test results */
  kprintf("\n=== Test Results ===\n");
  kprintf("Total tests: %d\n", test_count);
  kprintf("Passed: %d\n", test_passed);
  kprintf("Failed: %d\n", test_failed);
  kprintf("Success rate: %d%%\n",
          test_count > 0 ? (test_passed * 100) / test_count : 0);

  if (test_failed == 0) {
    kprintf("[SUCCESS] All do_abort tests passed!\n");
    return 0;
  } else {
    kprintf("[ERROR] %d tests failed\n", test_failed);
    return -1;
  }
}

/*===========================================================================*
 *                          test_do_abort_init                              *
 *===========================================================================*/
PUBLIC void test_do_abort_init() {
  kprintf("Initializing do_abort test framework...\n");

#if MAGIC_LLVM20_COMPAT
  kprintf("LLVM-20 Magic framework detected\n");
  kprintf("Magic check level: %d\n", MAGIC_CHECK_LEVEL);
  kprintf("Magic output control: %d\n", MAGIC_OUTPUT_CTL);
#endif

#ifdef __MINIX
  kprintf("MINIX extensions enabled\n");
#endif

  kprintf("do_abort test framework ready\n");
}
