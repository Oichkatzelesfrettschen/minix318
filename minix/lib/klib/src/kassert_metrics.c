/**
 * @file kassert_metrics.c
 * @brief Kernel Assertion Metrics Collection (Stub)
 *
 * This file will contain functions for collecting and reporting metrics
 * on kernel assertions, as part of an assertion effectiveness framework
 * (see Synthesis Document, Sec 7.2).
 * Currently, these are stubs.
 *
 * @note This klib module is intended for debugging and kernel analysis builds.
 */

#include <klib.h> // For kprintf_stub, basic types like k_size_t (though not directly used yet)
#include <sys/kassert.h> // For KASSERT itself (though not directly used by these stubs)
          // and potentially for klib-specific types if KASSERT expands to use them.
#include "../../../kernel/include/k_spinlock.h" // simple_spinlock_t for thread safety
#include <kassert_metrics.h> // Declarations for kassert_metrics_record_hit and kassert_metrics_report

/** Structure storing a single assertion site's metrics. */
typedef struct kassert_site_info {
  const char *file;             /**< Source file of the assertion. */
  int line;                     /**< Line number where the assertion resides. */
  const char *condition_string; /**< The assertion condition as a string. */
  unsigned long hit_count;      /**< Number of times this site was hit. */
} kassert_site_info_t;

/** Maximum number of unique assertion sites tracked. */
#define MAX_ASSERT_SITES 128

/** Static table of assertion site metrics. */
static kassert_site_info_t assertion_sites[MAX_ASSERT_SITES];

/** Current number of registered assertion sites. */
static int registered_assertion_sites = 0;

/** Spinlock protecting updates to the metrics table. */
static simple_spinlock_t metrics_lock = {0};

/**
 * @brief Record that an assertion has been encountered.
 *
 * The function keeps track of how many times each unique assertion site has
 * been triggered. It is intended to be called from an enhanced KASSERT macro.
 *
 * @param file             Source file containing the assertion.
 * @param line             Line number of the assertion.
 * @param condition_string The textual form of the asserted condition.
 */
void kassert_metrics_record_hit(const char *file, int line,
                                const char *condition_string) {
  if (!file || !condition_string) {
    return; /* Invalid parameters; ignore the hit. */
  }

  simple_spin_lock(&metrics_lock);

  for (int i = 0; i < registered_assertion_sites; ++i) {
    if (assertion_sites[i].line == line &&
        kstrcmp(assertion_sites[i].file, file) == 0 &&
        kstrcmp(assertion_sites[i].condition_string, condition_string) == 0) {
      assertion_sites[i].hit_count++;
      simple_spin_unlock(&metrics_lock);
      return;
    }
  }

  if (registered_assertion_sites < MAX_ASSERT_SITES) {
    kassert_site_info_t *new_site =
        &assertion_sites[registered_assertion_sites];
    new_site->file = file;
    new_site->line = line;
    new_site->condition_string = condition_string;
    new_site->hit_count = 1;
    registered_assertion_sites++;
  }

  simple_spin_unlock(&metrics_lock);
}

/**
 * @brief Reports collected assertion metrics. (STUB)
 *
 * This function would iterate through collected assertion data and print
 * a summary, potentially including hit counts for each registered assertion
 * site. This data could contribute to calculating assertion effectiveness E(a).
 */
/**
 * @brief Print a summary of collected assertion metrics.
 *
 * Each registered assertion site and its hit count are printed via
 * kprintf_stub(). The operation is protected by a spinlock so that metrics
 * may be recorded concurrently while a report is generated.
 */
void kassert_metrics_report(void) {
  simple_spin_lock(&metrics_lock);

  kprintf_stub("--- KASSERT metrics ---\n");
  for (int i = 0; i < registered_assertion_sites; ++i) {
    kprintf_stub("%s:%d (%s): %lu hits\n", assertion_sites[i].file,
                 assertion_sites[i].line, assertion_sites[i].condition_string,
                 assertion_sites[i].hit_count);
  }

  simple_spin_unlock(&metrics_lock);
}
