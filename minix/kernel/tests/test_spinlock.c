#include "kernel/k_spinlock.h" /* For simple_spinlock_t and functions */
#include "kernel/kernel.h" /* For KASSERT, kprintf_stub (via debug.h, klib) */
#include "kernel/proc.h" /* For struct proc, if needed for context, though not directly for these tests */
#include "kernel/protect.h" /* For disable_interrupts, restore_interrupts, KERNEL_CS */
                            /* If protect.h is too heavy or causes issues,
                               we can use arch_disable_interrupts_placeholder below. */

/*
 * Placeholder interrupt control functions if actual ones are problematic
 * for this standalone test context.
 * The actual kernel functions (disable_interrupts/restore_interrupts from
 * protect.h) are preferred if they can be used without issue.
 */
// static inline unsigned long arch_disable_interrupts_placeholder(void) {
//     kprintf_stub("arch_disable_interrupts_placeholder() called\n");
//     return 0; // Placeholder flags
// }
// static inline void arch_restore_interrupts_placeholder(unsigned long flags) {
//     kprintf_stub("arch_restore_interrupts_placeholder(0x%lx) called\n",
//     flags);
// }

/* Test 1: Basic lock/unlock functionality */
void test_basic_spinlock(void) {
  simple_spinlock_t lock;

  kprintf_stub("Starting test_basic_spinlock...\n");

  simple_spin_init(&lock);
  KASSERT(lock.locked == 0, "T1: spinlock should start unlocked");

  simple_spin_lock(&lock);
  KASSERT(lock.locked == 1, "T1: spinlock should be locked after acquisition");

  simple_spin_unlock(&lock);
  KASSERT(lock.locked == 0, "T1: spinlock should be unlocked after release");

  kprintf_stub("test_basic_spinlock: PASSED\n");
}

/*
 * Test 2: Lock exclusion (prevents recursive acquisition on the same CPU
 * context). This test relies on the spinlock being non-recursive. Attempting to
 * lock it again in the same thread of execution without an intervening unlock
 * would typically cause a deadlock if the lock were truly tested for
 * recursive locking by another thread trying to acquire it. Here, we just
 * verify it remains locked.
 */
void test_spinlock_exclusion(void) {
  simple_spinlock_t lock;
  // machine_t machine; /* For saved_eflags, if using real interrupt functions
  // */
  /* The type of saved_eflags depends on disable_interrupts() */
  /* Let's assume disable_interrupts() returns flags that can be stored in
   * unsigned long */
  // unsigned long saved_flags; // Changed to int based on disable_interrupts
  // return type
  int saved_eflags;

  kprintf_stub("Starting test_spinlock_exclusion...\n");

  simple_spin_init(&lock);

  /* Disable interrupts to simulate a critical section where preemption
     that could lead to re-entrant lock attempts (by the same thread if it
     were complexly scheduled, or by an IRQ handler) is prevented. */

  // Try to use actual kernel functions if available and suitable
  // save_flags_cli(&machine); // This is more typical for x86 to save eflags
  // and cli For MINIX, it's likely simpler: saved_flags = read_eflags(); //
  // Read current eflags - read_eflags might not be universally available or
  // correct type intr_disable(); // Disable interrupts (cli) - from protect.h?
  // or arch-specific? protect.h has enable_irq/disable_irq which are for
  // specific IRQs. Let's use the placeholder for now for broader compatibility
  // if actuals are complex. The user's snippet used `disable_interrupts()` and
  // `restore_interrupts(saved_eflags)` These are in `kernel/protect.h` and
  // `disable_interrupts` returns `eflags`.

  // Using functions from kernel/protect.h as suggested by user prompt
  saved_eflags = disable_interrupts();  // From kernel/protect.h, returns
                                        // original eflags (int on i386)

  simple_spin_lock(&lock);
  KASSERT(lock.locked == 1, "T2: lock should be held after first lock");

  /*
   * In a single-threaded test without true preemption or other CPUs,
   * calling simple_spin_lock(&lock) again here would deadlock, as it would
   * spin on lock.locked which is already 1. This demonstrates
   * non-recursiveness. We can't *execute* that deadlock in the test, so we just
   * assert the state.
   */

  simple_spin_unlock(&lock);
  restore_interrupts(saved_eflags);  // From kernel/protect.h

  kprintf_stub("test_spinlock_exclusion: PASSED (conceptual non-reentrancy)\n");
}

/*
 * It would be good to have a way to call these tests, e.g., from main.c
 * under a compile-time flag for testing purposes.
 * For now, these functions exist for compilation and conceptual validation.
 */
// void run_spinlock_tests(void) {
//     test_basic_spinlock();
//     test_spinlock_exclusion();
// }
