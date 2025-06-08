# Signal Handling Refinement and SMP Considerations

This document outlines the changes made to the MINIX kernel's signal handling mechanisms to improve robustness, reduce userspace dependencies, and note considerations for Symmetric Multiprocessing (SMP) safety.

## 1. Kernel-Specific Signal Set Operations

- **File:** `minix/kernel/include/minix/kernel_types.h`
- **Changes:**
    - Added the following macros for manipulating `k_sigset_t` (defined as `unsigned long`):
        - `k_sigemptyset(set)`
        - `k_sigaddset(set, signo)`
        - `k_sigdelset(set, signo)`
        - `k_sigismember(set, signo)`
        - `k_sigisempty(set)`
    - These macros utilize bitwise operations, assuming signal numbers are 1-based and fit within the `unsigned long` bitmask.
- **Rationale:** Replaces previous `FIXME` comments indicating reliance on userspace `sigsetops` which are not safe for kernel use. Provides a self-contained kernel way to manage signal sets.

## 2. Usage of New Signal Set Macros

- **File:** `minix/kernel/system.c`
  - **Function `send_sig()`:** Replaced `/* FIXME: sigaddset was here */` with `k_sigaddset(&priv(rp)->s_sig_pending, sig_nr);`.
  - **Function `cause_sig()`:**
    - Replaced `s = 0; /* FIXME: sigismember was here */` with `s = k_sigismember(&rp->p_pending, sig_nr);`.
    - Replaced `/* FIXME: sigaddset was here */` with `k_sigaddset(&rp->p_pending, sig_nr);`.
- **File:** `minix/kernel/proc.c` (within `BuildNotifyMessage` macro)
  - Replaced `/* FIXME: sigemptyset was here */` with `k_sigemptyset(&priv(dst_ptr)->s_sig_pending);`.
- **File:** `minix/kernel/system/do_getksig.c`
  - Replaced `/* FIXME: sigemptyset was here */` with `k_sigemptyset(&rp->p_pending);`.
- **Rationale:** Ensures that all manipulations of kernel signal sets (`p_pending`, `s_sig_pending`) use the newly defined kernel-specific operations.

## 3. Kernel-Specific Signal Structures for IPC

- **File:** `minix/kernel/ksignal.h` (New file created)
- **Definitions:**
    - **`k_sigmsg_t`**: Defined to hold parameters passed from PM for the `SYS_SIGSEND` call (signal number, mask, handler address, sigreturn address, stack pointer). This avoids the kernel directly using or needing the definition of the userspace `struct sigmsg`.
    - **`k_sigcontext_t`**: Defined as the kernel's internal representation of the signal context that is placed on and read from the user stack. It includes:
        - Architecture-specific register fields for i386 (e.g., `sc_gs`, `sc_eip`, `sc_eflags`, etc.) and ARM (e.g., `sc_spsr`, `sc_r0`, `sc_pc`, etc.).
        - `k_sigset_t sc_mask` for the signal mask to be restored.
        - `k_uint32_t sc_magic` with a `KSIGCONTEXT_MAGIC` (0x4B534358 /* KSCX */).
        - `k_uint32_t sc_fpu_flags` and `unsigned char sc_fpu_state[512]` for i386 FPU state.
    - **`K_MC_FPU_SAVED`**: A placeholder macro defined for FPU state flags (value `(1 << 0)`).
- **Rationale:** Decouples kernel signal handling logic from potentially complex or userspace-contaminated system header definitions of `struct sigmsg` and `struct sigcontext`. Provides clear, kernel-controlled structures.

## 4. Refinement of `SYS_SIGSEND` (`minix/kernel/system/do_sigsend.c`)

- Included `minix/kernel/ksignal.h`.
- Changed the local `smsg` variable to type `k_sigmsg_t ksm;`.
- **Data Copy:** Instead of a bulk copy of a userspace `struct sigmsg`, individual fields (signo, mask, sighandler, sigreturn) are copied from the userspace pointer (`m_ptr->m_sigcalls.sigctx`) into `ksm`.
    - **Note:** This still uses `K_OFFSETOF(struct sigmsg, ...)` which assumes knowledge of the userspace `struct sigmsg` layout. This is a known remaining dependency that ideally should be removed by changing the ABI (e.g., PM sending these fields directly).
- **Stack Frame Pointer:** The stack pointer for the signal frame (`frp`) is now determined by the kernel via `arch_get_sp(rp)` and this calculated value is stored in `ksm.sm_stkptr`.
- **Frame Population:** The on-stack signal frame (`fr.sf_sc`, assumed to be of type `k_sigcontext_t`) is populated using values from `rp->p_reg` (for general registers) and `ksm` (for `sc_mask`).
- `fr.sf_sc.sc_magic` is now set to `KSIGCONTEXT_MAGIC`.
- Corrected a typo in ARM register access (`rp_p_reg.r5` to `rp->p_reg.r5`).
- **Rationale:** Enhances safety by having the kernel work with its own `k_sigmsg_t` and by controlling the placement of the signal frame on the user stack. Reduces direct dependency on the full userspace `struct sigmsg` layout.

## 5. Refinement of `SYS_SIGRETURN` (`minix/kernel/system/do_sigreturn.c`)

- Included `minix/kernel/ksignal.h` and `minix/kernel/proc.h`.
- Changed the local `sc` variable to `k_sigcontext_t ksc;`.
- Data is now copied from the user stack into this kernel-defined `ksc` variable using `sizeof(k_sigcontext_t)`.
- Process registers (`rp->p_reg`) are restored from the fields of `ksc`.
- The check for a userspace `SC_MAGIC` was removed. The integrity is now based on the kernel copying a structure of type `k_sigcontext_t` which has its own `KSIGCONTEXT_MAGIC`.
- FPU state restoration (i386) now uses `ksc.sc_fpu_flags` and the kernel-defined `K_MC_FPU_SAVED`.
- **Rationale:** Ensures the kernel restores state based on a structure whose layout it controls (`k_sigcontext_t`), improving robustness against potentially malformed or incompatible userspace `sigcontext` structures.

## 6. SMP Locking Considerations (Comments Added)

- **File:** `minix/kernel/system.c`
  - **Function `send_sig()`:** Added `/* SMP_LOCK */` and `/* SMP_UNLOCK */` comments around the modification of `s_sig_pending` and the call to `mini_notify()`.
  - **Function `cause_sig()`:** Added `/* SMP_LOCK */` and `/* SMP_UNLOCK */` comments around the section that reads `p_pending`, potentially modifies it, and updates `RTS_SIGNALED` and `RTS_SIG_PENDING` flags.
- **File:** `minix/kernel/system/do_getksig.c`
  - **Function `do_getksig()`:** Added `/* SMP_LOCK */` and `/* SMP_UNLOCK */` comments around the section that reads `rp->p_pending`, clears it using `k_sigemptyset`, and unsets `RTS_SIGNALED`.
- **Rationale:** These comments mark critical sections where shared signal-related data structures (`p_pending`, `s_sig_pending`) and process flags (`RTS_SIGNALED`, `RTS_SIG_PENDING`) are accessed and modified. In an SMP environment, these sections would require protection (e.g., spinlocks) to prevent race conditions between CPUs or between CPUs and interrupt handlers. The `RTS_SET`/`RTS_UNSET` macros themselves would also need to be SMP-safe.

## Conclusion

These refinements address several `FIXME`s related to signal handling, introduce kernel-specific data structures and operations for better control and safety, and lay groundwork for future SMP considerations by identifying critical sections. The main remaining area for improvement regarding userspace dependencies is the method for copying `sigmsg` contents in `do_sigsend.c`.
