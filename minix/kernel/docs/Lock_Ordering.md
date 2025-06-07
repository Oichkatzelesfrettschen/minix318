# MINIX Kernel Lock Ordering

To prevent deadlocks when acquiring multiple locks, locks should generally be acquired in a consistent hierarchical order. This document outlines the known ordering for major kernel locks.

## Current Lock Hierarchy (Signal Subsystem Focus)

1.  **Per-Process Locks (e.g., `p_sig_lock` in `struct proc`):**
    *   These locks protect data specific to a single process.
    *   Example: `p_sig_lock` (type `spinlock_irq_t`) protects signal-related fields (`p_pending`, `s_sig_pending`, RTS flags) within a process structure. It is acquired using `spin_lock_irqsave()`, which disables local CPU interrupts.
    *   When locking data for a specific process, its `p_sig_lock` (if relevant to the data) should be taken.

*As more locks are introduced into the kernel (e.g., for global tables, device lists, scheduler queues), this document will be updated to reflect their place in the hierarchy.*

**General Rules:**
- Never attempt to acquire a lock that is earlier in the hierarchy (e.g., a global lock) while holding a lock that is later in the hierarchy (e.g., a per-process lock), if the operations could lead to a deadlock cycle.
- Interrupt-safe spinlocks (`spin_lock_irqsave`) disable local CPU interrupts. Operations performed while holding such locks must be brief, must not attempt to sleep, and must not re-enable interrupts before the lock is released via `spin_unlock_irqrestore()`.
