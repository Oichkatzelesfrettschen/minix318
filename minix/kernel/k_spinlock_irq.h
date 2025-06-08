/**
 * @file k_spinlock_irq.h
 * @brief Defines interrupt-safe spinlock primitives.
 *
 * This header provides spinlock functions that also handle disabling and
 * restoring local CPU interrupts. This is crucial for preventing deadlocks
 * when a spinlock might be acquired by both normal kernel code and an
 * interrupt handler on the same CPU. These locks wrap the basic
 * `simple_spinlock_t`.
 */
#ifndef K_SPINLOCK_IRQ_H
#define K_SPINLOCK_IRQ_H

#include "kernel/k_spinlock.h" /* For simple_spinlock_t and its operations */
#include "kernel/protect.h" /* For disable_interrupts() and restore_interrupts() */

/**
 * @brief Structure representing an interrupt-safe spinlock.
 *
 * This structure embeds a `simple_spinlock_t` and is intended to be used
 * with the `spin_lock_irqsave` and `spin_unlock_irqrestore` functions,
 * which manage interrupt state along with lock acquisition/release.
 */
typedef struct {
  /** @brief The underlying simple spinlock instance. */
  simple_spinlock_t lock;
  /* Future extensions could include debug info, owner CPU, etc. for more
   * advanced debugging or lock tracking, but are not part of this simple
   * implementation.
   */
} spinlock_irq_t;

/**
 * @brief Initializes an interrupt-safe spinlock.
 * @param irq_lock Pointer to the spinlock_irq_t to initialize.
 *
 * This function initializes the underlying simple spinlock to an unlocked
 * state. It must be called before the spinlock_irq_t is used for the first
 * time.
 */
static inline void spin_lock_irq_init(spinlock_irq_t *irq_lock) {
  // Initialize the embedded simple_spinlock.
  simple_spin_init(&irq_lock->lock);
}

/**
 * @brief Acquires an interrupt-safe spinlock and disables local CPU interrupts.
 * @param irq_lock Pointer to the spinlock_irq_t to acquire.
 * @return int The previous interrupt state (e.g., EFLAGS on x86) to be used
 * with spin_unlock_irqrestore.
 *
 * This function first disables interrupts on the current CPU and saves their
 * previous state. Then, it acquires the spinlock using a busy-wait.
 * Disabling interrupts before attempting to acquire the lock prevents deadlocks
 * that could occur if an interrupt handler on the same CPU tries to take the
 * same lock that the interrupted code already holds or is attempting to
 * acquire.
 */
static inline int spin_lock_irqsave(spinlock_irq_t *irq_lock) {
  int flags;
  // Disable local CPU interrupts and save the previous interrupt state (e.g.,
  // EFLAGS). This is critical to prevent an interrupt handler from trying to
  // acquire the same lock, which would lead to a deadlock on a single CPU.
  flags = disable_interrupts();
  // Acquire the underlying simple spinlock.
  simple_spin_lock(&irq_lock->lock);
  // Return the saved interrupt flags to the caller.
  return flags;
}

/**
 * @brief Releases an interrupt-safe spinlock and restores local CPU interrupts.
 * @param irq_lock Pointer to the spinlock_irq_t to release.
 * @param flags    The interrupt state (e.g., EFLAGS on x86) previously returned
 *                 by spin_lock_irqsave.
 *
 * This function first releases the underlying simple spinlock. Then, it
 * restores the interrupt state of the local CPU to what it was before
 * `spin_lock_irqsave` was called. It is crucial to release the lock *before*
 * restoring interrupts to minimize the time interrupts are disabled and to
 * ensure the critical section is fully exited before other interrupt code can
 * run.
 */
static inline void spin_unlock_irqrestore(spinlock_irq_t *irq_lock, int flags) {
  // Release the underlying simple spinlock first.
  simple_spin_unlock(&irq_lock->lock);
  // Restore the previous interrupt state (e.g., EFLAGS), which may re-enable
  // interrupts if they were enabled before calling spin_lock_irqsave.
  restore_interrupts(flags);
}

#endif /* K_SPINLOCK_IRQ_H */
