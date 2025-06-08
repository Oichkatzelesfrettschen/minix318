# Signal Handling KASSERT Verification and Code Snippets

This document provides code snippets from key signal handling routines after the introduction of P1, P2, P3, and P4 KASSERTs, as well as other refinements like the usage of `k_sig*` macros and kernel-specific signal context structures.

## Methodology
Files examined:
- `minix/kernel/system/do_getksig.c`
- `minix/kernel/system/do_endksig.c`
- (Implicitly, `minix/kernel/system.c` for `cause_sig` and `send_sig` which were modified in prior subtasks with P1/P2 KASSERTs, and `minix/kernel/include/minix/kernel_types.h` for signal set macros).

Focus: Verification of KASSERT placements and extraction of relevant code logic related to signal state management (`p_pending`, `RTS_SIGNALED`, `RTS_SIG_PENDING`).

## Findings and Code Snippets

---

### File: `minix/kernel/system/do_getksig.c`
**Function:** `do_getksig(struct proc * caller, message * m_ptr)`

**Core Logic Snippet (Post-KASSERT additions):**
```c
/* Find the next process with pending signals. */
for (rp = BEG_USER_ADDR; rp < END_PROC_ADDR; rp++) {
    if (isemptyp(rp)) continue; /* Skip empty slots */
    if (RTS_ISSET(rp, RTS_SIGNALED)) {
        /* Verify caller is the designated signal manager for this process */
        KASSERT(caller->p_endpoint == priv(rp)->s_sig_mgr,
                "do_getksig: caller %d is not signal manager %d for target %d",
                caller->p_endpoint, priv(rp)->s_sig_mgr, rp->p_endpoint);
        if (caller->p_endpoint != priv(rp)->s_sig_mgr) continue;

  /* SMP_LOCK (atomicity for p_pending and RTS_SIGNALED flag) */
  /* This section reads and modifies p_pending and p_rts_flags (RTS_SIGNALED).
   * It needs protection against concurrent access in SMP, particularly from
   * cause_sig().
   */
  /* store signaled process' endpoint */
        m_ptr->m_sigcalls.endpt = rp->p_endpoint;
        m_ptr->m_sigcalls.map = rp->p_pending;	/* pending signals map */
        k_sigemptyset(&rp->p_pending); 	/* clear map in the kernel */
        KASSERT(k_sigisempty(&rp->p_pending),
                "do_getksig: signal set p_pending for proc %d (ep %d) not properly cleared by k_sigemptyset",
                proc_nr(rp), rp->p_endpoint);
  /* The RTS_UNSET macro itself should be SMP-safe or be called
   * while holding the appropriate lock.
   */
  RTS_UNSET(rp, RTS_SIGNALED);		/* remove signaled flag */
  /* SMP_UNLOCK */
        return(OK);
    }
}

/* No process with pending signals was found. */
m_ptr->m_sigcalls.endpt = NONE;
```

**Analysis of Snippet:**
- **Target Process (`rp`):** Determined by iterating from `BEG_USER_ADDR` to `END_PROC_ADDR`.
- **`rp->p_pending` Access:**
    - Read: `m_ptr->m_sigcalls.map = rp->p_pending;`
    - Cleared: `k_sigemptyset(&rp->p_pending);`
- **`RTS_SIG_PENDING` Manipulation:** Not directly manipulated in this function. `RTS_SIG_PENDING` is primarily managed by `cause_sig` (set) and `do_endksig` (conditionally unset).
- **`RTS_SIGNALED` Manipulation:**
    - Checked: `if (RTS_ISSET(rp, RTS_SIGNALED))`
    - Unset: `RTS_UNSET(rp, RTS_SIGNALED);`
- **P3 KASSERT (Manager Verification):** `KASSERT(caller->p_endpoint == priv(rp)->s_sig_mgr, ...)` is present and correctly placed to verify that the process calling `do_getksig` (the signal manager, `caller`) is authorized to manage signals for the target process `rp`.
- **P4 KASSERT (Signal Set Integrity):** `KASSERT(k_sigisempty(&rp->p_pending), ...)` is present immediately after `k_sigemptyset`, correctly verifying that the pending signal set was cleared.

---

### File: `minix/kernel/system/do_endksig.c`
**Function:** `do_endksig(struct proc * caller, message * m_ptr)`

**Core Logic Snippet (Post-KASSERT additions):**
```c
  /* Get process pointer and verify that it had signals pending. If the
   * process is already dead its flags will be reset.
   */
  if(!isokendpt(m_ptr->m_sigcalls.endpt, &proc_nr))
	return EINVAL;

  rp = proc_addr(proc_nr);

  /* P2 KASSERTs for process and privilege structure validity */
  KASSERT(rp != NULL, "do_endksig: null process pointer for proc_nr %d", proc_nr);
  KASSERT(rp->p_magic == PMAGIC, "do_endksig: corrupted process structure for proc_nr %d, endpoint %d", proc_nr, rp->p_endpoint);
  KASSERT(priv(rp) != NULL, "do_endksig: null privilege structure for proc_nr %d, endpoint %d", proc_nr, rp->p_endpoint);

  /* P3 KASSERTs for state consistency */
  KASSERT(caller->p_endpoint == priv(rp)->s_sig_mgr,
          "do_endksig: caller %d is not signal manager %d for target process %d",
          caller->p_endpoint, priv(rp)->s_sig_mgr, rp->p_endpoint);
  KASSERT(RTS_ISSET(rp, RTS_SIG_PENDING),
          "do_endksig: RTS_SIG_PENDING is not set for proc %d, endpoint %d",
          proc_nr, rp->p_endpoint);

  /* Existing checks for runtime enforcement (kept for non-KASSERT builds) */
  if (caller->p_endpoint != priv(rp)->s_sig_mgr) return(EPERM);
  if (!RTS_ISSET(rp, RTS_SIG_PENDING)) return(EINVAL);

  /* The signal manager has finished one kernel signal. Is the process ready? */
  /* If no new signal has arrived (RTS_SIGNALED is clear), then clear SIG_PENDING. */
  /* SMP_LOCK: вокруг проверки RTS_SIGNALED и RTS_UNSET(RTS_SIG_PENDING) */
  if (!RTS_ISSET(rp, RTS_SIGNALED))
	RTS_UNSET(rp, RTS_SIG_PENDING);	/* remove pending flag */
  /* SMP_UNLOCK */
```

**Analysis of Snippet:**
- **Target Process (`rp`):** Determined from `m_ptr->m_sigcalls.endpt`.
- **`RTS_SIG_PENDING` Manipulation:**
    - Checked: `KASSERT(RTS_ISSET(rp, RTS_SIG_PENDING), ...)` and the subsequent `if (!RTS_ISSET(rp, RTS_SIG_PENDING)) return(EINVAL);`.
    - Unset: Conditionally unset by `RTS_UNSET(rp, RTS_SIG_PENDING);` if `RTS_SIGNALED` is not set.
- **`RTS_SIGNALED` Manipulation:**
    - Checked: `if (!RTS_ISSET(rp, RTS_SIGNALED))`
    - Not directly set or unset by `do_endksig` itself; its state reflects whether new signals have arrived since `do_getksig` cleared it.
- **P3 KASSERTs (State Consistency):**
    - `KASSERT(caller->p_endpoint == priv(rp)->s_sig_mgr, ...)`: Verifies the caller is the authorized signal manager.
    - `KASSERT(RTS_ISSET(rp, RTS_SIG_PENDING), ...)`: Verifies that the process was indeed marked as having a signal being processed. This is the primary state `do_endksig` acts upon.
    - The user-requested `KASSERT(RTS_ISSET(rp, RTS_SIGNALED), ...)` was determined to be logically incorrect for this specific location, as `RTS_SIGNALED` being set at this point means *new* signals have arrived, not a condition for ending the *current* signal processing. The key is that `RTS_SIG_PENDING` is set.

---

This concludes the extraction and documentation of the specified code snippets.
 appending KASSERT summary marker.

## KASSERT Implementation for Signal Handling Robustness

As part of the signal handling refactoring and verification, several KASSERTs were introduced to enforce critical invariants and preconditions. These assertions primarily target `minix/kernel/system.c`, `minix/kernel/system/do_getksig.c`, and `minix/kernel/system/do_endksig.c`.

### 1. Signal Number Validation (P1)
- **Location:** `cause_sig()`, `send_sig()` in `minix/kernel/system.c`
- **Assertion:** `KASSERT(sig_nr > 0 && sig_nr < _NSIG, "function_name: invalid signal number %d", sig_nr);`
- **Purpose:** Ensures that signal numbers passed to these core functions are within the valid range (1 to `_NSIG`-1). Protects against memory corruption from invalid bitmask operations and enforces ABI contracts. `_NSIG` is sourced from `<minix/signal.h>`.

### 2. Process Structure Validity (P2)
- **Location:** `cause_sig()`, `send_sig()` in `minix/kernel/system.c`; also added defensively in `do_endksig.c`.
- **Assertions:**
    - `KASSERT(rp != NULL, "function_name: null process pointer ...");`
    - `KASSERT(rp->p_magic == PMAGIC, "function_name: corrupted process structure ...");`
    - `KASSERT(priv(rp) != NULL, "function_name: null privilege structure ...");`
- **Purpose:** Verifies that the target process pointer (`rp`) is non-null, the process structure is not corrupted (using `PMAGIC` from `proc.h`), and its associated privilege structure is valid before signal operations proceed.

### 3. State Consistency Checks (P3)
- **Location:** `do_getksig()` in `minix/kernel/system/do_getksig.c`
  - **Assertion:** `KASSERT(caller->p_endpoint == priv(rp)->s_sig_mgr, "do_getksig: caller %d is not registered signal manager %d for target process %d", ...);`
  - **Purpose:** Ensures that only the officially registered signal manager for a process can retrieve its pending signals.

- **Location:** `do_endksig()` in `minix/kernel/system/do_endksig.c`
  - **Assertion:** `KASSERT(RTS_ISSET(rp, RTS_SIG_PENDING), "do_endksig: RTS_SIG_PENDING is not set for proc %d, endpoint %d", ...);`
  - **Purpose:** Based on the clarified signal state protocol, this asserts that `do_endksig` is called only when the kernel believes the signal manager was actively processing signals for the process (`RTS_SIG_PENDING` is set). This catches protocol violations.

### 4. Signal Set Integrity (P4)
- **Location:** `do_getksig()` in `minix/kernel/system/do_getksig.c`
- **Assertion:** `KASSERT(k_sigisempty(&rp->p_pending), "do_getksig: signal set p_pending for proc %d (ep %d) not properly cleared by k_sigemptyset", ...);`
- **Purpose:** After `p_pending` is cleared using `k_sigemptyset()`, this verifies that the set is indeed empty, ensuring correctness of the signal clearing operation. Requires `k_sigisempty()` macro from `kernel_types.h`.

These KASSERTs serve as executable documentation, enforce critical invariants, and aid in early bug detection during development and debugging.


## Spinlock Synchronization for Signal Handling

To address race conditions identified in the signal handling subsystem (primarily between `cause_sig` generating signals and `do_getksig` retrieving them), an interrupt-safe spinlock mechanism has been introduced.

### 1. Spinlock Definition (`minix/kernel/k_spinlock.h` and `minix/kernel/k_spinlock_irq.h`)
<<<<<<< HEAD
- The basic spinlock `simple_spinlock_t` is defined in `minix/kernel/k_spinlock.h`.
- A new header file `minix/kernel/k_spinlock_irq.h` defines `spinlock_irq_t`. This wraps `simple_spinlock_t` and integrates local CPU interrupt disabling/enabling using `disable_interrupts()` and `restore_interrupts()` from `kernel/protect.h`.
- It provides inline functions:
    - `spin_lock_irq_init()`: Initializes the spinlock.
    - `spin_lock_irqsave()`: Disables local CPU interrupts (saving prior state) and then acquires the spinlock.
    - `spin_unlock_irqrestore()`: Releases the spinlock and then restores the saved interrupt state.
- The underlying `simple_spinlock_t` relies on GCC's atomic builtins for its core operations. Its `simple_spin_lock` function was further enhanced to include a call to `arch_pause()` (defined in architecture-specific headers like `arch/i386/include/arch_cpu.h` for x86 via `asm volatile("pause");`) within its busy-wait loop. This reduces CPU power consumption and bus contention during lock contention on supported architectures, providing a form of adaptive spinning. These features ensure atomicity, appropriate memory barriers for SMP safety, and prevent deadlocks with interrupt handlers on the same CPU.
=======
- The basic spinlock `simple_spinlock_t` is defined in `minix/kernel/k_spinlock.h`. It now includes members for basic statistics: `acquisitions` (number of times the lock was acquired) and `contentions` (number of times a thread tried to acquire the lock but found it already held).
- `simple_spin_init()` initializes these statistics to zero.
- `simple_spin_lock()` updates these statistics. It was also enhanced to include a call to `arch_pause()` (defined in architecture-specific headers like `arch/i386/include/arch_cpu.h` for x86 via `asm volatile("pause");`) within its busy-wait loop. If spinning exceeds `MAX_SPIN_THRESHOLD`, a `kernel_yield()` stub (currently also calling `arch_pause()`) is invoked to prevent CPU monopolization and allow other threads to run. This provides adaptive spinning and basic contention monitoring.
- A new header file `minix/kernel/k_spinlock_irq.h` defines `spinlock_irq_t`. This wraps `simple_spinlock_t` and integrates local CPU interrupt disabling/enabling using `disable_interrupts()` and `restore_interrupts()` from `kernel/protect.h`.
- It provides inline functions:
    - `spin_lock_irq_init()`: Initializes the underlying simple spinlock (including its statistics).
    - `spin_lock_irqsave()`: Disables local CPU interrupts (saving prior state) and then acquires the spinlock.
    - `spin_unlock_irqrestore()`: Releases the spinlock and then restores the saved interrupt state.
- These features ensure atomicity, appropriate memory barriers for SMP safety, and prevent deadlocks with interrupt handlers on the same CPU, while also providing insights into lock behavior.
>>>>>>> acfb8ad15 (feat: Dev tools, advanced spinlocks, IPC KASSERTs, docs & quality)

### 2. Integration with Process Structure
- `struct proc` (in `minix/kernel/proc.h`) now includes a `spinlock_irq_t p_sig_lock;` member.
- This lock is initialized for each process during `proc_init()` in `minix/kernel/proc.c` using `spin_lock_irq_init()`.

### 3. Usage in Signal Handling
- **`cause_sig()` (`minix/kernel/system.c`):**
    - `rp->p_sig_lock` is acquired using `spin_lock_irqsave()` before any modification to `rp->p_pending` (for external managers) or `priv(rp)->s_sig_pending` (for self-managed processes) and associated `RTS_SIGNALED`/`RTS_SIG_PENDING` flags.
    - The lock is released using `spin_unlock_irqrestore()` after these critical operations, including the call to `send_sig()`.
- **`do_getksig()` (`minix/kernel/system/do_getksig.c`):**
    - `rp->p_sig_lock` is acquired using `spin_lock_irqsave()` before reading `rp->p_pending`, clearing `rp->p_pending` (with `k_sigemptyset`), and clearing the `RTS_SIGNALED` flag.
    - The lock is released using `spin_unlock_irqrestore()` after these operations.
- **`do_endksig()` (`minix/kernel/system/do_endksig.c`):**
    - `rp->p_sig_lock` is acquired using `spin_lock_irqsave()` after `rp` is validated and the caller (signal manager) is authorized.
    - The KASSERT checking `RTS_ISSET(rp, RTS_SIG_PENDING)` (asserting PM is ending an active signal processing session) is performed while holding the lock.
    - The core logic that inspects `RTS_SIGNALED` to decide whether to clear `RTS_SIG_PENDING` (if no new signal arrived after `do_getksig` cleared `RTS_SIGNALED`) is protected by the lock.
    - The lock is released using `spin_unlock_irqrestore()` before returning.
- **`send_sig()` (`minix/kernel/system.c`):**
    - This function was refactored to no longer modify `priv(rp)->s_sig_pending`. The modification is now handled by its caller (`cause_sig`) under lock. `send_sig` is now purely for notification via `mini_notify` in this context.
- **`do_kill()` (`minix/kernel/system/do_kill.c`):**
    - This function was analyzed and determined not to require direct locking. It validates parameters and then calls `cause_sig`, which internally handles the necessary locking for signal state modifications.
- **Signal Action Handling (e.g. `SYS_SIGACTION`):**
    - An analysis for a kernel handler for `SYS_SIGACTION` (e.g., in a `do_sigaction.c` file) was performed. Such a file was not found. Standard MINIX architecture suggests that detailed signal action (disposition, `sa_mask`, `sa_flags`) management is primarily handled by the Process Manager (PM) in userspace. The kernel's direct involvement is mainly in the mechanics of signal delivery and managing pending/blocked states. Therefore, no `p_sig_lock` applications were made in this specific context.

This per-process IRQ-safe spinlock (`p_sig_lock`) ensures that updates and reads of a process's signal state (pending sets and RTS flags) are atomic, preventing lost signals and inconsistent state due to concurrent execution in an SMP environment or preemption by interrupt handlers.
