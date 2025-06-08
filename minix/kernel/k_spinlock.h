/**
 * @file k_spinlock.h
 * @brief Defines a simple, non-recursive spinlock using GCC atomic builtins.
 *
 * This header provides a basic spinlock implementation suitable for short
 * critical sections, particularly in contexts where sleeping is not permissible
<<<<<<< HEAD
<<<<<<< HEAD
 * (e.g., some interrupt handlers or core kernel code before schedulers are fully active).
 * It is designed with SMP considerations, relying on GCC's atomic builtins which
 * typically ensure full memory barriers for sequential consistency.
 * Includes adaptive spinning using `arch_pause()` for supported architectures.
=======
=======
>>>>>>> acfb8ad15 (feat: Dev tools, advanced spinlocks, IPC KASSERTs, docs & quality)
 * (e.g., some interrupt handlers or core kernel code before schedulers are
 * fully active). It is designed with SMP considerations, relying on GCC's
 * atomic builtins which typically ensure full memory barriers for sequential
 * consistency. Includes adaptive spinning using `arch_pause()` for supported
 * architectures.
<<<<<<< HEAD
>>>>>>> acfb8ad15 (feat: Dev tools, advanced spinlocks, IPC KASSERTs, docs & quality)
=======
>>>>>>> acfb8ad15 (feat: Dev tools, advanced spinlocks, IPC KASSERTs, docs & quality)
 */
#ifndef K_SPINLOCK_H
#define K_SPINLOCK_H

#include <minix/sys_config.h> /* For potential CONFIG_SMP or other system configs */

/* Include arch-specific definitions, e.g., for arch_pause() */
#if defined(__i386__) || defined(__x86_64__)
<<<<<<< HEAD
<<<<<<< HEAD
#include "arch/i386/include/arch_cpu.h" // Provides arch_pause for x86
=======
#include "arch/i386/include/arch_cpu.h"  // Provides arch_pause for x86
>>>>>>> acfb8ad15 (feat: Dev tools, advanced spinlocks, IPC KASSERTs, docs & quality)
=======
#include "arch/i386/include/arch_cpu.h"  // Provides arch_pause for x86
>>>>>>> acfb8ad15 (feat: Dev tools, advanced spinlocks, IPC KASSERTs, docs & quality)
#else
/**
 * @brief Placeholder for arch_pause on non-x86 architectures.
 *
<<<<<<< HEAD
<<<<<<< HEAD
 * For architectures other than i386/x86_64, this function currently acts as a no-op.
 * It can be defined with architecture-specific pause/yield instructions if available
 * to improve spin-wait loop performance.
=======
 * For architectures other than i386/x86_64, this function currently acts as a
 * no-op. It can be defined with architecture-specific pause/yield instructions
 * if available to improve spin-wait loop performance.
>>>>>>> acfb8ad15 (feat: Dev tools, advanced spinlocks, IPC KASSERTs, docs & quality)
=======
 * For architectures other than i386/x86_64, this function currently acts as a
 * no-op. It can be defined with architecture-specific pause/yield instructions
 * if available to improve spin-wait loop performance.
>>>>>>> acfb8ad15 (feat: Dev tools, advanced spinlocks, IPC KASSERTs, docs & quality)
 */
static inline void arch_pause(void) { /* No-op */ }
#endif

/**
<<<<<<< HEAD
<<<<<<< HEAD
 * @brief Structure representing a simple spinlock.
 *
 * The spinlock's state is determined by the `locked` member.
=======
=======
>>>>>>> acfb8ad15 (feat: Dev tools, advanced spinlocks, IPC KASSERTs, docs & quality)
 * @brief Maximum number of spin iterations before attempting to yield.
 *
 * This threshold is used in `simple_spin_lock` to prevent a CPU from
 * monopolizing resources by spinning indefinitely on a highly contended lock.
 * After this many spins in the inner loop, `kernel_yield()` is called.
 * The value should be tuned based on system characteristics and expected
 * contention levels.
 */
#define MAX_SPIN_THRESHOLD 100000

#ifndef KERNEL_YIELD_DEFINED
#define KERNEL_YIELD_DEFINED
/**
 * @brief Yields the CPU, typically to the scheduler. (Stub Implementation)
 *
 * This function is called when a spinlock has been spinning for too long
 * (exceeding MAX_SPIN_THRESHOLD), as a mechanism to prevent CPU monopolization
 * and allow other threads/processes to run.
 *
 * @note This is a stub implementation. A full implementation would typically
 * involve interacting with the system scheduler to relinquish the CPU.
 * For now, it at least performs an `arch_pause()` to reduce contention.
 * A real implementation might call something like `sched_yield()` or `yield()`.
 */
static inline void kernel_yield(void) {
  /* Placeholder for actual yield. On x86, 'rep nop' is sometimes used
   * as a more potent pause than single 'pause', or actual scheduler yield.
   * For now, this can be a kprintf_stub for debugging or just a comment.
   * A true yield would involve scheduler interaction.
   */
  // kprintf_stub("kernel_yield() called (stub)\n"); // Uncomment for debugging
  // yield calls
  arch_pause();  // At least do an arch_pause if yielding fully is complex.
}
#endif

/**
 * @brief Structure representing a simple spinlock.
 *
 * The spinlock's state is determined by the `locked` member.
 * It also includes basic statistics for acquisitions and contentions.
<<<<<<< HEAD
>>>>>>> acfb8ad15 (feat: Dev tools, advanced spinlocks, IPC KASSERTs, docs & quality)
=======
>>>>>>> acfb8ad15 (feat: Dev tools, advanced spinlocks, IPC KASSERTs, docs & quality)
 * It is crucial that operations on this structure use the provided
 * `simple_spin_*` functions to ensure atomicity and correct memory ordering.
 */
typedef struct {
<<<<<<< HEAD
<<<<<<< HEAD
    /**
     * @brief The lock state. 0 for unlocked, 1 for locked.
     *
     * `volatile` ensures that the compiler does not optimize away reads of this
     * variable, as its value can change unexpectedly due to actions from other
     * CPUs or threads. The atomicity of lock operations is guaranteed by GCC's
     * `__sync_*` builtins, not by `volatile` itself.
     */
    volatile int locked;
} simple_spinlock_t;

/**
 * @brief Initializes a spinlock to the unlocked state.
 * @param lock Pointer to the simple_spinlock_t to initialize.
 *
 * This function must be called before the spinlock is used for the first time.
 * It sets the lock state to 0, indicating it is available.
 */
static inline void simple_spin_init(simple_spinlock_t *lock) {
    // Initialize the lock state to 0 (unlocked).
    lock->locked = 0;
=======
=======
>>>>>>> acfb8ad15 (feat: Dev tools, advanced spinlocks, IPC KASSERTs, docs & quality)
  /**
   * @brief The lock state. 0 for unlocked, 1 for locked.
   *
   * `volatile` ensures that the compiler does not optimize away reads of this
   * variable, as its value can change unexpectedly due to actions from other
   * CPUs or threads. The atomicity of lock operations is guaranteed by GCC's
   * `__sync_*` builtins, not by `volatile` itself.
   */
  volatile int locked;
  /** @brief Number of times the lock was successfully acquired. */
  unsigned long acquisitions;
  /** @brief Number of times a thread tried to acquire the lock but found it
   * already held, thus entering a spin-wait loop. This indicates contention. */
  unsigned long contentions;
  /* Future: unsigned long total_spin_cycles; // Could be added with more
   * advanced cycle counting */
} simple_spinlock_t;

/**
 * @brief Initializes a spinlock to the unlocked state and resets statistics.
 * @param lock Pointer to the simple_spinlock_t to initialize.
 *
 * This function must be called before the spinlock is used for the first time.
 * It sets the lock state to 0 (unlocked) and initializes statistics counters to
 * zero.
 */
static inline void simple_spin_init(simple_spinlock_t *lock) {
  // Initialize the lock state to 0 (unlocked).
  lock->locked = 0;
  // Initialize statistics.
  lock->acquisitions = 0;
  lock->contentions = 0;
<<<<<<< HEAD
>>>>>>> acfb8ad15 (feat: Dev tools, advanced spinlocks, IPC KASSERTs, docs & quality)
=======
>>>>>>> acfb8ad15 (feat: Dev tools, advanced spinlocks, IPC KASSERTs, docs & quality)
}

/**
 * @brief Acquires a spinlock, busy-waiting if necessary.
 * @param lock Pointer to the simple_spinlock_t to acquire.
 *
 * This function attempts to acquire the lock. If the lock is already held,
 * it will spin (busy-wait) until the lock becomes available.
 * This function is non-recursive; a thread attempting to acquire a lock
 * it already holds will deadlock.
<<<<<<< HEAD
<<<<<<< HEAD
 */
static inline void simple_spin_lock(simple_spinlock_t *lock) {
    /* Attempt to acquire the lock by atomically setting lock->locked to 1
     * and getting the *previous* value.
     * __sync_lock_test_and_set provides a full memory barrier, ensuring that
     * memory operations after acquiring the lock are not reordered before it,
     * and operations before are completed.
     * The loop continues as long as the previous value was not 0 (i.e., the lock
     * was already held by someone else or by this attempt if it just got set to 1).
     */
    while (__sync_lock_test_and_set(&lock->locked, 1) != 0) {
        /* Inner busy-wait loop: Spin while the lock is held by someone else.
         * This inner read loop (checking lock->locked directly) can be slightly
         * more efficient on some architectures than repeatedly executing the atomic
         * __sync_lock_test_and_set, as it might reduce bus contention.
         */
        while (lock->locked != 0) {
            /* arch_pause() provides a hint to the CPU that this is a spin-wait loop.
             * On x86, this is the "pause" instruction, which can improve performance
             * and reduce power consumption during the spin, especially on hyper-threaded
             * processors by yielding execution resources.
             */
            arch_pause();
        }
    }
    /* Upon successful acquisition (previous value was 0), the lock is now held,
     * and a full memory barrier is implied by __sync_lock_test_and_set.
     */
=======
=======
>>>>>>> acfb8ad15 (feat: Dev tools, advanced spinlocks, IPC KASSERTs, docs & quality)
 * Includes a spin counter and calls `kernel_yield()` if spinning excessively.
 * Also updates lock acquisition and contention statistics.
 */
static inline void simple_spin_lock(simple_spinlock_t *lock) {
  // Attempt to acquire the lock immediately using atomic test-and-set.
  // If __sync_lock_test_and_set returns 0, the lock was acquired successfully
  // (it was 0 and is now 1).
  if (__sync_lock_test_and_set(&lock->locked, 1) == 0) {
    lock->acquisitions++;  // Successfully acquired on the first try.
    return;                // Lock acquired, no contention.
  }

  // If the first attempt failed, the lock was already held. This is a
  // contention.
  lock->contentions++;
  int spin_count = 0;  // Initialize spin counter for this contention episode.

  // Loop indefinitely, spinning and re-attempting to acquire the lock.
  while (1) {
    // Inner busy-wait loop: Spin while the lock is held by someone else.
    // This inner read loop (checking lock->locked directly) can be slightly
    // more efficient on some architectures than repeatedly executing the atomic
    // __sync_lock_test_and_set, as it might reduce bus contention.
    while (lock->locked != 0) {
      /* arch_pause() provides a hint to the CPU that this is a spin-wait loop.
       * On x86, this is the "pause" instruction, which can improve performance
       * and reduce power consumption during the spin, especially on
       * hyper-threaded processors by yielding execution resources.
       */
      arch_pause();

      spin_count++;  // Increment spin counter.
      if (spin_count > MAX_SPIN_THRESHOLD) {
        /* If we've spun too many times, call kernel_yield().
         * This is to prevent CPU monopolization on highly contended locks
         * by allowing other threads/processes to run.
         * The actual behavior of kernel_yield() depends on its implementation
         * (e.g., true scheduler yield or just a more potent pause).
         */
        kernel_yield();
        spin_count = 0;  // Reset counter after yielding.
      }
    }

    // After observing lock->locked == 0 in the inner loop,
    // attempt to acquire the lock again using atomic test-and-set.
    if (__sync_lock_test_and_set(&lock->locked, 1) == 0) {
      lock->acquisitions++;  // Lock acquired after spinning.
      return;                // Exit the function, lock is now held.
    }
    // If __sync_lock_test_and_set still returned non-zero, it means another
    // CPU/thread acquired the lock between our read of lock->locked and our TAS
    // attempt (a race). In this case, the outer while(1) loop continues, and we
    // re-enter the inner spin.
  }
<<<<<<< HEAD
>>>>>>> acfb8ad15 (feat: Dev tools, advanced spinlocks, IPC KASSERTs, docs & quality)
=======
>>>>>>> acfb8ad15 (feat: Dev tools, advanced spinlocks, IPC KASSERTs, docs & quality)
}

/**
 * @brief Releases a previously acquired spinlock.
 * @param lock Pointer to the simple_spinlock_t to release.
 *
 * This function releases the lock, allowing another thread to acquire it.
 * It must only be called by the thread that currently holds the lock.
 */
static inline void simple_spin_unlock(simple_spinlock_t *lock) {
<<<<<<< HEAD
<<<<<<< HEAD
    /* Atomically set lock->locked to 0 (unlocked).
     * __sync_lock_release provides a release memory barrier. This ensures that all
     * memory writes made by this thread within the critical section (before this
     * unlock) are visible to other CPUs before the lock is actually released.
     */
    __sync_lock_release(&lock->locked);
=======
=======
>>>>>>> acfb8ad15 (feat: Dev tools, advanced spinlocks, IPC KASSERTs, docs & quality)
  /* Atomically set lock->locked to 0 (unlocked).
   * __sync_lock_release provides a release memory barrier. This ensures that
   * all memory writes made by this thread within the critical section (before
   * this unlock) are visible to other CPUs before the lock is actually
   * released.
   */
  __sync_lock_release(&lock->locked);
<<<<<<< HEAD
>>>>>>> acfb8ad15 (feat: Dev tools, advanced spinlocks, IPC KASSERTs, docs & quality)
=======
>>>>>>> acfb8ad15 (feat: Dev tools, advanced spinlocks, IPC KASSERTs, docs & quality)
}

#endif /* K_SPINLOCK_H */
