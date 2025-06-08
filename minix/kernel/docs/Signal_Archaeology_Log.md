# Signal Archaeology Log

## Purpose
Document findings from examining MINIX signal handling code to understand
existing patterns, potential issues, and implicit invariants before implementing
improvements, as per the user's pedagogical guidance.

## Methodology
Examined files: `minix/kernel/system.c`, `minix/kernel/proc.h`, `minix/kernel/system/do_kill.c`, `minix/kernel/system/do_sigsend.c`, `minix/kernel/system/do_sigreturn.c`, `minix/kernel/system/do_getksig.c`, `minix/kernel/system/do_endksig.c`, `minix/kernel/ksignal.h`, `minix/kernel/include/minix/kernel_types.h`
Focus: Signal state modifications (especially `p_pending` in `struct proc` and `s_sig_pending` in `struct priv`), FIXMEs, potential race conditions, implicit assumptions, and required invariants.

## Findings

---

### Location: `minix/kernel/proc.h` (Fields in `struct proc` and `struct priv`)
**Code Context:**
```c
// In struct proc:
  k_sigset_t p_pending;         /* bit map for pending kernel signals */
  volatile u32_t p_rts_flags;   /* process is runnable only if zero */

// In struct priv (via priv(rp)->s_sig_pending):
  k_sigset_t s_sig_pending;     /* pending signals for this process */
```
Relevant flags in `p_rts_flags`: `RTS_SIGNALED`, `RTS_SIG_PENDING`.

**What's Happening:**
- `p_pending` in `struct proc`: Stores a bitmask of signals raised for a process, to be handled by its designated (external) signal manager (usually PM).
- `s_sig_pending` in `struct priv`: Stores a bitmask of signals for a system process that manages its own signals (e.g., when it receives `SIGKSIGSM`).
- `p_rts_flags`: Contains runtime status flags. `RTS_SIGNALED` indicates that a signal has been recorded and the manager needs to be (or has been) notified. `RTS_SIG_PENDING` indicates that the signal manager is currently processing signals for this process, and the process itself should not run.

**Implicit Assumptions:**
- `k_sigset_t` (defined as `unsigned long`) is wide enough for all signal numbers. Standard MINIX signals (1-31) fit.
- The macros `RTS_SET`, `RTS_UNSET`, `RTS_ISSET` provide a safe way to manage `p_rts_flags`.
- The distinction between `p_pending` (for external managers) and `s_sig_pending` (for self-managing system services) is consistently maintained.

**Potential Issues (Race Conditions, Violated Assumptions, etc.):**
- **Concurrent Access to Signal Sets:** In an SMP environment, `p_pending` and `s_sig_pending` could be accessed concurrently by different CPUs (e.g., one CPU delivering a new signal via `cause_sig`, another CPU handling `do_getksig` for PM). The bitmask operations (now `k_sigaddset`, `k_sigismember`, `k_sigemptyset`) are not inherently atomic across CPUs without explicit locking or atomic CPU instructions.
- **Concurrent Access to `p_rts_flags`:** Similarly, `p_rts_flags` can be modified by scheduling functions and signal handling functions. The `RTS_SET`/`RTS_UNSET` macros must ensure atomicity of read-modify-write sequences on these flags in SMP.

**Required Invariants (What must always be true here for correctness?):**
- Modifications to `p_pending` and `s_sig_pending` must be atomic or protected by a lock specific to the process structure.
- Modifications to `p_rts_flags` (especially combinations like setting `RTS_SIGNALED | RTS_SIG_PENDING`) must be atomic or protected.
- If `RTS_SIGNALED` is set, `p_pending` (or `s_sig_pending`) should contain the signal(s) that led to this state, until cleared by the signal manager via `do_getksig`.

**Why the FIXME? (If applicable):**
- No explicit FIXMEs on these field definitions themselves, but FIXMEs in the code that uses them (e.g., for `sigaddset`) were present and have been addressed by kernel-specific macros in `kernel_types.h`. The SMP comments added in a previous subtask highlight the need for locking around their use.

---

### Location: `minix/kernel/system.c` (Function `send_sig`)
**Code Context:**
```c
int send_sig(endpoint_t ep, int sig_nr)
{
  // ...
  rp = proc_addr(proc_nr);
  priv = priv(rp);
  if(!priv) return ENOENT;

  /* SMP_LOCK (atomicity for s_sig_pending and mini_notify) */
  /* This critical section should be protected by a spinlock in SMP environments,
   * or by disabling interrupts in a uniprocessor environment if mini_notify
   * could cause a reschedule or if send_sig can be called from an interrupt.
   */
  k_sigaddset(&priv->s_sig_pending, sig_nr);
  mini_notify(proc_addr(SYSTEM), rp->p_endpoint);
  /* SMP_UNLOCK */

  return OK;
}
```

**What's Happening:**
`send_sig` is used to send a kernel-internal signal (`sig_nr`, e.g., `SIGKSIGSM`, `SIGKMEM`) to a system process `rp`. It adds the signal to the `s_sig_pending` set of the target process's privilege structure and then sends a generic notification from `SYSTEM` to `rp`.

**Implicit Assumptions:**
- The target `rp` is a system process that handles its own signals recorded in `s_sig_pending`.
- `k_sigaddset` correctly and safely (atomically or within a lock) sets the signal bit.
- `mini_notify` is a safe way to alert the target process.
- `sig_nr` is a valid signal number (checked by KASSERT in previous step).

**Potential Issues (Race Conditions, Violated Assumptions, etc.):**
- **Atomicity:** The sequence of `k_sigaddset` and `mini_notify` should appear atomic to the receiving process. If `rp` checks `s_sig_pending` upon receiving the notification, the signal bit must already be set. The added `SMP_LOCK` comments address this for SMP.

**Required Invariants (What must always be true here for correctness?):**
- The signal bit in `s_sig_pending` must be set *before* `mini_notify` can cause the target process to inspect this set.
- Operations on `s_sig_pending` must be serialized for a given process.

**Why the FIXME? (If applicable):**
- The original `sigaddset` FIXME was replaced by `k_sigaddset`. The SMP comments were added to note the need for explicit locking.

---

### Location: `minix/kernel/system.c` (Function `cause_sig`)
**Code Context (general path):**
```c
  /* SMP_LOCK (atomicity for p_pending and RTS flags) */
  /* This section reads and potentially modifies p_pending and p_rts_flags.
   * It needs protection against concurrent access in SMP.
   */
  s = k_sigismember(&rp->p_pending, sig_nr);
  /* Check if the signal is already pending. Process it otherwise. */
  if (!s) {
      k_sigaddset(&rp->p_pending, sig_nr);
      if (! (RTS_ISSET(rp, RTS_SIGNALED))) {		/* other pending */
	  /* The RTS_SET macro itself should be SMP-safe or be called
	   * while holding the appropriate lock.
	   */
	  RTS_SET(rp, RTS_SIGNALED | RTS_SIG_PENDING);
          if(OK != send_sig(sig_mgr, SIGKSIG)) // SIGKSIG may be undefined
		panic("send_sig failed");
      }
  }
  /* SMP_UNLOCK */
```
**Code Context (self-management path):**
```c
// if(rp->p_endpoint == sig_mgr) {
//   if(0 /* FIXME: SIGS_IS_LETHAL(sig_nr) was here */) {
//       /* ... logic for lethal signals, backup manager ... */
//   }
//   k_sigaddset(&priv(rp)->s_sig_pending, sig_nr);
//   if(OK != send_sig(rp->p_endpoint, SIGKSIGSM))
//       panic("send_sig failed");
//   return;
// }
```

**What's Happening:**
`cause_sig` raises a signal `sig_nr` for process `rp`.
- **External Manager Path:** It checks if the signal is in `rp->p_pending`. If not, it adds it. If `rp` wasn't already `RTS_SIGNALED`, it sets `RTS_SIGNALED` and `RTS_SIG_PENDING`, then notifies the manager `sig_mgr` using `send_sig` (with `SIGKSIG`).
- **Self-Manager Path:** If `rp` is its own manager, it adds the signal to `priv(rp)->s_sig_pending` and notifies itself using `send_sig` (with `SIGKSIGSM`). The lethal signal handling is commented out.

**Implicit Assumptions:**
- The sequence of checks and modifications is atomic or race-free.
- `k_sigismember`, `k_sigaddset`, `RTS_ISSET`, `RTS_SET` are correct and safe.
- `SIGKSIG` and `SIGKSIGSM` are correctly defined and handled.

**Potential Issues (Race Conditions, Violated Assumptions, etc.):**
- **Atomicity (External Manager Path):** The check-then-act on `rp->p_pending` and `RTS_SIGNALED` needs to be atomic. Concurrent calls could lead to lost signals (if `k_sigaddset` isn't internally atomic for the bitmask operation and external locking isn't used) or redundant notifications. SMP comments now highlight this.
- **Missing Lethal Signal Logic (Self-Manager Path):** Critical for process termination.
- **Signal Number Validation:** `_NSIG` check was added in a previous task.

**Required Invariants (What must always be true here for correctness?):**
- A signal added to `p_pending` (or `s_sig_pending`) must lead to `RTS_SIGNALED` being set (if not already) and appropriate manager notification.
- State of `p_pending` / `s_sig_pending` and related RTS flags must be consistent.
- The entire operation block for a given process `rp` should be effectively atomic.

**Why the FIXME? (If applicable):**
- `/* FIXME: SIGS_IS_LETHAL(sig_nr) was here */`: Indicates missing critical logic.
- Original FIXMEs for `sigismember`/`sigaddset` were addressed. SMP comments note remaining sync needs.

---

### Location: `minix/kernel/system/do_getksig.c` (Function `do_getksig`)
**Code Context:**
```c
  // if (RTS_ISSET(rp, RTS_SIGNALED)) {
  //     if (caller->p_endpoint != priv(rp)->s_sig_mgr) continue;
  //     /* SMP_LOCK (...) */
  //     m_ptr->m_sigcalls.map = rp->p_pending;
  //     k_sigemptyset(&rp->p_pending);
  //     RTS_UNSET(rp, RTS_SIGNALED); /* remove signaled flag */
  //     /* SMP_UNLOCK */
  //     return(OK);
  // }
```

**What's Happening:**
The signal manager (PM) calls `SYS_GETKSIG`. If process `rp` is `RTS_SIGNALED` and caller is the manager:
1. `rp->p_pending` is copied to the reply message.
2. `rp->p_pending` is cleared with `k_sigemptyset`.
3. `RTS_SIGNALED` is cleared. (`RTS_SIG_PENDING` is cleared later by `do_endksig`).

**Implicit Assumptions:**
- The read of `rp->p_pending`, its clearing, and clearing `RTS_SIGNALED` is atomic with respect to new signals from `cause_sig`.

**Potential Issues (Race Conditions, Violated Assumptions, etc.):**
- **Race with `cause_sig`:** A new signal could be added to `rp->p_pending` by `cause_sig` *after* `do_getksig` copies `p_pending` but *before* `k_sigemptyset` clears it. This new signal would be wiped from `p_pending` without being delivered in the current call. If `cause_sig` didn't re-notify PM (e.g., because `RTS_SIGNALED` was still perceived as set by `cause_sig` due to timing), the signal could be lost. The added SMP comments indicate this critical section.

**Required Invariants (What must always be true here for correctness?):**
- The set of signals returned must be exactly those that caused `RTS_SIGNALED`.
- No signals should be lost. If a new signal arrives during processing, it must either be included or remain pending for a subsequent call.
- After this, `rp->p_pending` should be empty (for the retrieved signals) and `RTS_SIGNALED` clear (until a new signal fully processed by `cause_sig` sets it again).

---

### Location: `minix/kernel/system/do_sigsend.c` & `minix/kernel/system/do_sigreturn.c`
**Code Context:** These functions manage the user-level signal context frame. `do_sigsend` previously used `K_OFFSETOF(struct sigmsg, ...)` and both dealt with structures like `struct sigframe_sigcontext` and `struct sigcontext` (now `k_sigcontext_t`).

**What's Happening (Post-Refinement):**
- `do_sigsend` uses `k_sigmsg_t` to receive parameters from PM. It copies fields from userspace `sigmsg` pointer using `K_OFFSETOF(struct sigmsg, ...)`. It builds a `struct sigframe_sigcontext` on user stack, where `sf_sc` is `k_sigcontext_t` and includes `KSIGCONTEXT_MAGIC`.
- `do_sigreturn` copies `k_sigcontext_t` from user stack and restores registers.

**Implicit Assumptions:**
- `struct sigframe_sigcontext` layout (from `<machine/frame.h>`) correctly embeds `k_sigcontext_t` as `sf_sc`.
- `k_sigcontext_t` accurately represents all user-restorable state.
- The `K_OFFSETOF(struct sigmsg, ...)` approach for `do_sigsend` correctly extracts data.

**Potential Issues (Race Conditions, Violated Assumptions, etc.):**
- **`K_OFFSETOF(struct sigmsg, ...)` in `do_sigsend`**: If userspace `struct sigmsg` layout changes, kernel copies incorrect data. This is an ABI fragility.
- **`struct sigframe_sigcontext` Definition**: If kernel's view of this frame (especially `sf_sc` as `k_sigcontext_t`) doesn't match reality, corruption occurs. This structure is defined outside `minix/kernel/` and was not modifiable.
- **Completeness of `k_sigcontext_t`**: Any missed user-restorable register will cause bugs.
- **Missing `mcontext_t`**: The original `FIXME`s related to `mcontext_t`. As `<machine/mcontext.h>` was not found, full reconciliation wasn't possible. `k_sigcontext_t` is the kernel's attempt at a safe replacement.

**Required Invariants (What must always be true here for correctness?):**
- Signal frame structure pushed by `do_sigsend` must match what `do_sigreturn` expects.
- All necessary user-mode CPU state must be saved/restored.
- `SYS_SIGSEND` parameter passing from PM must be robust.

**Why the FIXME? (If applicable):**
- The `K_OFFSETOF(struct sigmsg, ...)` in `do_sigsend.c` remains an implicit FIXME due to its reliance on userspace struct layout.
- The `FIXME` comments in the original `do_sigsend.c` and `do_sigreturn.c` about `struct sigcontext` and `struct sigmsg` being problematic were partially addressed by introducing `k_sigcontext_t` and `k_sigmsg_t`.

---

### Location: `minix/kernel/system/do_endksig.c` (Function `do_endksig`)
**Code Context:**
```c
// if (!RTS_ISSET(rp, RTS_SIGNALED)) 		/* new signal arrived */
//	RTS_UNSET(rp, RTS_SIG_PENDING);	/* remove pending flag */
```

**What's Happening:**
PM calls `SYS_ENDKSIG` after processing signals for `rp`. This function clears `RTS_SIG_PENDING` for `rp` *only if no new signals have arrived* (i.e., if `RTS_SIGNALED` is not set). If new signals did arrive, `RTS_SIG_PENDING` remains set, and PM is expected to call `SYS_GETKSIG` again.

**Implicit Assumptions:**
- `RTS_SIGNALED` reliably indicates new signals since `SYS_GETKSIG` was last called by PM for this process.
- The interaction of clearing `RTS_SIG_PENDING` here and `cause_sig` setting `RTS_SIGNALED | RTS_SIG_PENDING` is race-free.

**Potential Issues (Race Conditions, Violated Assumptions, etc.):**
- **Atomicity of Flag Checks/Changes:** The check `!RTS_ISSET(rp, RTS_SIGNALED)` and the conditional `RTS_UNSET(rp, RTS_SIG_PENDING)` must be atomic with respect to `cause_sig` setting flags.
    - If `cause_sig` sets `RTS_SIGNALED` and `RTS_SIG_PENDING` between the check and the unset in `do_endksig`, `RTS_SIG_PENDING` might be incorrectly cleared, potentially causing PM to miss the newest signal notification if it relies on `RTS_SIG_PENDING` also being set. However, `cause_sig` would have sent a new `SIGKSIG` if it found `RTS_SIGNALED` clear, or if `RTS_SIGNALED` was already set, PM would still know to call `do_getksig`. The main risk is a brief inconsistency or reliance on PM re-checking due to `SIGKSIG` even if `RTS_SIG_PENDING` was momentarily cleared.

**Required Invariants (What must always be true here for correctness?):**
- If `RTS_SIGNALED` is set (indicating more signals or new signals arrived), `RTS_SIG_PENDING` should remain set to ensure PM calls `do_getksig` again.
- If `RTS_SIGNALED` is clear, then `RTS_SIG_PENDING` can be safely cleared.
- All `p_rts_flags` manipulations need to be SMP-safe.

---
This concludes the signal archaeology log.
