/**
 * @file k_spinlock.h
 * @brief Defines a simple, non-recursive spinlock using GCC atomic builtins.
 *
 * This header provides a basic spinlock implementation suitable for short
 * critical sections, particularly in contexts where sleeping is not permissible
 * (e.g., some interrupt handlers or core kernel code before schedulers are fully active).
 * It is designed with SMP considerations, relying on GCC's atomic builtins which
 * typically ensure full memory barriers for sequential consistency.
 * Includes adaptive spinning using `arch_pause()` for supported architectures.
 */
#ifndef K_SPINLOCK_H
#define K_SPINLOCK_H

#include <minix/sys_config.h> /* For potential CONFIG_SMP or other system configs */

/* Include arch-specific definitions, e.g., for arch_pause() */
#if defined(__i386__) || defined(__x86_64__)
#include "arch/i386/include/arch_cpu.h" // Provides arch_pause for x86
#else
/**
 * @brief Placeholder for arch_pause on non-x86 architectures.
 *
 * For architectures other than i386/x86_64, this function currently acts as a no-op.
 * It can be defined with architecture-specific pause/yield instructions if available
 * to improve spin-wait loop performance.
 */
static inline void arch_pause(void) { /* No-op */ }
#endif

/**
 * @brief Structure representing a simple spinlock.
 *
 * The spinlock's state is determined by the `locked` member.
 * It is crucial that operations on this structure use the provided
 * `simple_spin_*` functions to ensure atomicity and correct memory ordering.
 */
typedef struct {
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
}

/**
 * @brief Acquires a spinlock, busy-waiting if necessary.
 * @param lock Pointer to the simple_spinlock_t to acquire.
 *
 * This function attempts to acquire the lock. If the lock is already held,
 * it will spin (busy-wait) until the lock becomes available.
 * This function is non-recursive; a thread attempting to acquire a lock
 * it already holds will deadlock.
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
}

/**
 * @brief Releases a previously acquired spinlock.
 * @param lock Pointer to the simple_spinlock_t to release.
 *
 * This function releases the lock, allowing another thread to acquire it.
 * It must only be called by the thread that currently holds the lock.
 */
static inline void simple_spin_unlock(simple_spinlock_t *lock) {
    /* Atomically set lock->locked to 0 (unlocked).
     * __sync_lock_release provides a release memory barrier. This ensures that all
     * memory writes made by this thread within the critical section (before this
     * unlock) are visible to other CPUs before the lock is actually released.
     */
    __sync_lock_release(&lock->locked);
}

#endif /* K_SPINLOCK_H */
