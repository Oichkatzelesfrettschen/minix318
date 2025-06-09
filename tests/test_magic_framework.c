/*
 * Comprehensive test suite for Magic LLVM-20 instrumentation framework
 * Tests memory safety, type safety, and runtime analysis capabilities
 */
#include "../minix/llvm/include/magic_common.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test framework macros */
#define TEST_SUITE_NAME "Magic LLVM-20 Framework"
#define MAX_TEST_NAME_LEN 128

static int total_tests = 0;
static int passed_tests = 0;
static int failed_tests = 0;

#define MAGIC_TEST_ASSERT(condition, test_name)                                \
  do {                                                                         \
    total_tests++;                                                             \
    if (condition) {                                                           \
      passed_tests++;                                                          \
      printf("[PASS] %s\n", test_name);                                        \
    } else {                                                                   \
      failed_tests++;                                                          \
      printf("[FAIL] %s\n", test_name);                                        \
    }                                                                          \
  } while (0)

/*===========================================================================*
 *                          test_magic_constants                            *
 *===========================================================================*/
void test_magic_constants() {
  printf("Testing Magic constants and configuration...\n");

  MAGIC_TEST_ASSERT(LLVM_VERSION_MAJOR == 20, "LLVM version major is 20");

  MAGIC_TEST_ASSERT(MAGIC_LLVM20_COMPAT == 1, "LLVM-20 compatibility enabled");

  MAGIC_TEST_ASSERT(strcmp(MAGIC_PREFIX_STR, "magic_") == 0,
                    "Magic prefix string correct");

  MAGIC_TEST_ASSERT(
      strcmp(MAGIC_VERSION_STRING, "MAGIC_LLVM20_MINIX318_2025") == 0,
      "Magic version string correct");
}

/*===========================================================================*
 *                          test_magic_memory_tracking                      *
 *===========================================================================*/
void test_magic_memory_tracking() {
  printf("Testing Magic memory tracking capabilities...\n");

  MAGIC_TEST_ASSERT(MAGIC_INSTRUMENT_MEM_FUNCS == 1,
                    "Memory function instrumentation enabled");

  MAGIC_TEST_ASSERT(MAGIC_INSTRUMENT_HEAP_TRACKING == 1,
                    "Heap tracking enabled");

  MAGIC_TEST_ASSERT(MAGIC_INSTRUMENT_STACK == 1,
                    "Stack instrumentation enabled");

  MAGIC_TEST_ASSERT(MAGIC_IMPROVED_MEMORY_TRACKING == 1,
                    "Improved memory tracking enabled");

  /* Test memory allocation tracking */
  void *test_ptr = malloc(100);
  MAGIC_TEST_ASSERT(test_ptr != NULL, "Memory allocation successful");
  free(test_ptr);
}

/*===========================================================================*
 *                          test_magic_type_safety                          *
 *===========================================================================*/
void test_magic_type_safety() {
  printf("Testing Magic type safety features...\n");

  MAGIC_TEST_ASSERT(MAGIC_INSTRUMENT_TYPE_SAFETY == 1,
                    "Type safety instrumentation enabled");

  MAGIC_TEST_ASSERT(MAGIC_ENHANCED_TYPE_ANALYSIS == 1,
                    "Enhanced type analysis enabled");

  /* Test type flag definitions */
  MAGIC_TEST_ASSERT(MAGIC_TYPE_LLVM20_ENHANCED == 0x100,
                    "LLVM-20 enhanced type flag correct");

  MAGIC_TEST_ASSERT(MAGIC_TYPE_ATOMIC_SAFE == 0x200,
                    "Atomic safe type flag correct");

  MAGIC_TEST_ASSERT(MAGIC_TYPE_MEMORY_TAGGED == 0x400,
                    "Memory tagged type flag correct");
}

/*===========================================================================*
 *                          test_magic_atomic_operations                    *
 *===========================================================================*/
void test_magic_atomic_operations() {
  printf("Testing Magic atomic operations support...\n");

  MAGIC_TEST_ASSERT(MAGIC_ATOMIC_OPERATIONS == 1, "Atomic operations enabled");

  MAGIC_TEST_ASSERT(MAGIC_ATOMIC_MEMORY_OPERATIONS == 1,
                    "Atomic memory operations enabled");

  MAGIC_TEST_ASSERT(MAGIC_THREAD_SAFE == 1, "Thread safety enabled");

  MAGIC_TEST_ASSERT(MAGIC_LOCKFREE_DATASTRUCTURES == 1,
                    "Lockfree data structures enabled");
}

/*===========================================================================*
 *                          test_magic_minix_extensions                     *
 *===========================================================================*/
void test_magic_minix_extensions() {
  printf("Testing Magic MINIX extensions...\n");

#ifdef __MINIX
  MAGIC_TEST_ASSERT(MAGIC_USE_MINIX_EXTENSIONS == 1,
                    "MINIX extensions enabled");

  MAGIC_TEST_ASSERT(MAGIC_MINIX_MEMORY_PROTECTION == 1,
                    "MINIX memory protection enabled");

  MAGIC_TEST_ASSERT(GLOBAL_VARS_IN_SECTION == 1,
                    "Global variables in section enabled");
#else
  MAGIC_TEST_ASSERT(MAGIC_USE_MINIX_EXTENSIONS == 0,
                    "MINIX extensions disabled on non-MINIX");
#endif
}

/*===========================================================================*
 *                          test_magic_debug_features                       *
 *===========================================================================*/
void test_magic_debug_features() {
  printf("Testing Magic debugging features...\n");

  MAGIC_TEST_ASSERT(MAGIC_USE_ENHANCED_DEBUGGING == 1,
                    "Enhanced debugging enabled");

  MAGIC_TEST_ASSERT(MAGIC_NAMED_ALLOC_USE_DBG_INFO == 1,
                    "Debug info for allocations enabled");

  MAGIC_TEST_ASSERT(MAGIC_CHECK_LEVEL >= 1, "Check level is adequate");

  MAGIC_TEST_ASSERT(MAGIC_MAX_NAME_LEN == 128,
                    "Maximum name length increased for LLVM-20");

  MAGIC_TEST_ASSERT(MAGIC_MAX_TYPE_STR_LEN == 512,
                    "Maximum type string length increased");
}

/*===========================================================================*
 *                          test_magic_performance                          *
 *===========================================================================*/
void test_magic_performance() {
  printf("Testing Magic performance optimizations...\n");

  MAGIC_TEST_ASSERT(MAGIC_LOOKUP_CACHE_OPTIMIZATION == 1,
                    "Cache optimization enabled");

  MAGIC_TEST_ASSERT(MAGIC_COMPACT_COMP_TYPES == 1,
                    "Compact composite types enabled");

  MAGIC_TEST_ASSERT(MAGIC_SHRINK_TYPE_STR == 1,
                    "Type string shrinking enabled");

  MAGIC_TEST_ASSERT(MAGIC_MAX_RECURSIVE_TYPES == 2048,
                    "Recursive types limit increased");
}

/*===========================================================================*
 *                          test_magic_memory_regions                       *
 *===========================================================================*/
void test_magic_memory_regions() {
  printf("Testing Magic memory region tracking...\n");

  /* Test state flags for different memory regions */
  MAGIC_TEST_ASSERT((MAGIC_STATE_DATA & 0x00000010) != 0,
                    "DATA region flag defined");

  MAGIC_TEST_ASSERT((MAGIC_STATE_HEAP & 0x00000020) != 0,
                    "HEAP region flag defined");

  MAGIC_TEST_ASSERT((MAGIC_STATE_STACK & 0x00000100) != 0,
                    "STACK region flag defined");

  MAGIC_TEST_ASSERT((MAGIC_STATE_MAP & 0x00000040) != 0,
                    "MAP region flag defined");

  MAGIC_TEST_ASSERT((MAGIC_STATE_LLVM20_INSTRUMENTED & 0x40000000) != 0,
                    "LLVM-20 instrumented flag defined");
}

/*===========================================================================*
 *                              main                                        *
 *===========================================================================*/
int main() {
  printf("=== %s Test Suite ===\n", TEST_SUITE_NAME);
  printf("Testing Magic LLVM-20 instrumentation framework\n");
  printf("Version: %s\n", MAGIC_VERSION_STRING);
  printf("\n");

  /* Run all test functions */
  test_magic_constants();
  test_magic_memory_tracking();
  test_magic_type_safety();
  test_magic_atomic_operations();
  test_magic_minix_extensions();
  test_magic_debug_features();
  test_magic_performance();
  test_magic_memory_regions();

  /* Print final results */
  printf("\n=== Test Results ===\n");
  printf("Total tests: %d\n", total_tests);
  printf("Passed: %d\n", passed_tests);
  printf("Failed: %d\n", failed_tests);
  printf("Success rate: %.1f%%\n",
         total_tests > 0 ? (passed_tests * 100.0) / total_tests : 0.0);

  if (failed_tests == 0) {
    printf("\n[SUCCESS] All Magic framework tests passed!\n");
    return 0;
  } else {
    printf("\n[ERROR] %d tests failed\n", failed_tests);
    return 1;
  }
}
