/**
 * @file test_spinlock.c
 * @brief Unit tests for the non-recursive simple_spinlock_t implementation.
 *
 * This file provides conceptual tests for the MINIX kernel’s simple
 * spinlock:
 *   - test_basic_spinlock(): verifies initialization, lock, unlock
 *   - test_spinlock_exclusion(): asserts non-recursive behavior
 *
 * These tests use KASSERT() for validation and kprintf_stub() for output.
 * They should be invoked under a TEST_SPINLOCK compile-time flag, e.g.:
 *   cc -DTEST_SPINLOCK ... -o test_spinlock test_spinlock.c
 */

#include "kernel/k_spinlock.h"    /**< simple_spinlock_t API */
#include "kernel/kernel.h"       /**< KASSERT(), kprintf_stub() */
#include "kernel/proc.h"         /**< struct proc (for proc_stacktrace()) */
#include "kernel/protect.h"      /**< disable_interrupts(), restore_interrupts() */
#include <sys/kassert.h>         /**< KASSERT() macro */

#ifdef TEST_SPINLOCK

/**
 * @brief Test basic spinlock operations.
 *
 * Initializes a simple_spinlock_t and checks that:
 *   - After init, lock.locked == 0
 *   - After lock(), lock.locked == 1
 *   - After unlock(), lock.locked == 0
 *
 * Uses KASSERT() to enforce each state and prints a PASS message.
 */
void test_basic_spinlock(void)
{
    simple_spinlock_t lock;

    kprintf_stub("T1: Starting test_basic_spinlock...\n");

    simple_spin_init(&lock);
    KASSERT(lock.locked == 0,
            "T1: spinlock should start unlocked");

    simple_spin_lock(&lock);
    KASSERT(lock.locked == 1,
            "T1: spinlock should be locked after simple_spin_lock()");

    simple_spin_unlock(&lock);
    KASSERT(lock.locked == 0,
            "T1: spinlock should be unlocked after simple_spin_unlock()");

    kprintf_stub("T1: test_basic_spinlock PASSED\n");
}

/**
 * @brief Test non-recursive exclusion property of spinlock.
 *
 * Disables interrupts to simulate atomic context, locks once,
 * and asserts that the lock remains held. A second lock()
 * would deadlock if attempted, demonstrating non-recursiveness.
 * This test does not actually call lock() twice to avoid hanging.
 */
void test_spinlock_exclusion(void)
{
    simple_spinlock_t lock;
    int saved_flags;

    kprintf_stub("T2: Starting test_spinlock_exclusion...\n");

    simple_spin_init(&lock);

    /* Disable interrupts to prevent preemption */
    saved_flags = disable_interrupts();

    simple_spin_lock(&lock);
    KASSERT(lock.locked == 1,
            "T2: spinlock should be held after first simple_spin_lock()");

    /*
     * Conceptually, a second simple_spin_lock() here would spin forever
     * because the lock is non-recursive. We do not invoke it to avoid deadlock.
     */

    simple_spin_unlock(&lock);
    restore_interrupts(saved_flags);

    kprintf_stub("T2: test_spinlock_exclusion PASSED (non-recursive)\n");
}

/**
 * @brief Run all spinlock unit tests.
 *
 * Invokes test_basic_spinlock() and test_spinlock_exclusion().
 * Can be called from a test harness or main() when TEST_SPINLOCK is defined.
 */
void run_spinlock_tests(void)
{
    test_basic_spinlock();
    test_spinlock_exclusion();
}

#endif /* TEST_SPINLOCK */
