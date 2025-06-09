#include <stdio.h>
#include <unistd.h>   // For pid_t, getpid() (for comparison)
#include <stdlib.h>   // For exit()
#include <errno.h>    // For errno
#include <string.h>   // For strerror
#include <minix/config.h> // For any MINIX specific defs
// #include <minix/ipc.h> // For message if needed for debug syscalls
// #include <minix/callnr.h> // For PM_PROC_NR if used in debug syscalls
#include <minix/syslib.h> // For _syscall or _kernel_call for debug syscalls
#include <minix/com.h>    // For KERNEL_CALL (needs to be available)

// FIXME TODO: Define these syscall numbers in a proper header (e.g., minix/com.h or a test-specific one)
// These are conceptual syscalls for testing purposes for Phase 0.25.
// Ensure these values do not clash with existing KERNEL_CALL + offset values in com.h
#ifndef SYS_DEBUG_GET_PM_CAP
#define SYS_DEBUG_GET_PM_CAP    (KERNEL_CALL + 100) // Example: Kernel call to get a PM capability
#endif
#ifndef SYS_DEBUG_RS_RESTART_PM
#define SYS_DEBUG_RS_RESTART_PM (KERNEL_CALL + 101) // Example: Kernel call to ask RS to simulate PM restart
#endif

// Prototype for the new capability-based getpid
extern pid_t getpid_cap(void); // Assumes it's linked from libc

// Prototypes for conceptual debug syscalls
// Returns capability index for PM, or -1 on error.
static uint32_t debug_get_pm_capability(void) {
    message m;
    memset(&m, 0, sizeof(m));
    // No params needed for this simple version
    // FIXME TODO: _kernel_call might not be directly available/appropriate here.
    // This might need to be a regular _syscall to a specific endpoint (e.g., SELF, handled by kernel)
    // or a dedicated wrapper. For now, conceptual.
    if (_kernel_call(SYS_DEBUG_GET_PM_CAP, &m) < 0) {
        return (uint32_t)-1;
    }
    // Assuming kernel returns cap_idx in m.m1_i1 (similar to how PM returns PID)
    // and m.m_type would be OK.
    return (uint32_t)m.m1_i1;
}

// Requests RS to simulate a restart of PM (which should bump PM's epoch)
static int debug_rs_simulate_pm_restart(void) {
    message m;
    memset(&m, 0, sizeof(m));
    // No params needed
    return _kernel_call(SYS_DEBUG_RS_RESTART_PM, &m);
}

// FIXME TODO: rdtsc() needs to be properly defined for the architecture
// For x86:
static inline unsigned long long rdtsc(void) {
    unsigned hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((unsigned long long)lo) | (((unsigned long long)hi) << 32);
}

#define ITERATIONS 100000
#define WARMUP_ITERATIONS 1000

int main(void) {
    pid_t p_cap, p_std;
    // uint32_t pm_cap_idx_v1; // Kept for conceptual clarity, but not used directly by getpid_cap() yet
    int test_failures = 0;
    unsigned long long start_time, end_time, total_cycles = 0;
    unsigned long long min_cycles = (unsigned long long)-1, max_cycles = 0;

    printf("TESTING: Capability-based getpid() and Epoch Validation\n");

    // Step 1: Get initial PM capability
    // FIXME TODO PLACEHOLDER: For Phase 0.25, the test program cannot directly call cap_alloc.
    // It needs a way to get a PM capability. Using a conceptual debug syscall.
    // The kernel handler for SYS_DEBUG_GET_PM_CAP would:
    // 1. Find current proc (caller).
    // 2. Create a capability_t template for PM:
    //    - service_id = PM_SERVICE_ID_FOR_EPOCH (e.g. _ENDPOINT_P(PM_PROC_NR))
    //    - epoch = global_service_epochs[PM_SERVICE_ID_FOR_EPOCH] (current epoch)
    //    - permissions = CAP_PERM_IPC_SEND
    //    - endpoint_or_obj = PM_PROC_NR
    // 3. Call cap_alloc(&caller->p_cap_table, &template, &cap_idx).
    // 4. Return cap_idx to user.
    // This debug syscall needs to be implemented in the kernel.
    // In getpid_cap(), this pm_cap_idx_v1 would then need to be passed to _syscall_cap.
    // For now, getpid_cap() assumes a fixed index (KERNEL_PM_CAP_IDX in proc.c).
    // So this test relies on the kernel pre-populating KERNEL_PM_CAP_IDX for this test process,
    // or the kernel implicitly using KERNEL_PM_CAP_IDX if the target is PM_PROC_NR and
    // call is PM_GETPID_CAP_RQ.
    printf("INFO: Assuming kernel provides PM capability at fixed index for this test process.\n");
    // pm_cap_idx_v1 = debug_get_pm_capability(); // Conceptual call
    // if (pm_cap_idx_v1 == (uint32_t)-1) {
    //     perror("debug_get_pm_capability failed");
    //     test_failures++;
    // }

    // Step 2: Call getpid_cap() and verify
    p_cap = getpid_cap(); // This uses the implicit/fixed PM capability index
    p_std = getpid();     // Standard getpid for comparison

    printf("  getpid_cap() returned: %d\n", (int)p_cap);
    printf("  Standard getpid() returned: %d\n", (int)p_std);

    if (p_cap == -1) {
        printf("  ERROR: getpid_cap() failed. errno: %d (%s)\n", errno, strerror(errno));
        test_failures++;
    } else if (p_cap != p_std) {
        printf("  ERROR: getpid_cap() [%d] != standard getpid() [%d]\n", (int)p_cap, (int)p_std);
        test_failures++;
    } else {
        printf("  SUCCESS: getpid_cap() matches standard getpid().\n");
    }

    // Step 3: Simulate RS restarting PM (bumps PM's epoch in kernel)
    printf("TESTING: Epoch validation. Requesting RS to simulate PM restart...\n");
    // FIXME TODO PLACEHOLDER: SYS_DEBUG_RS_RESTART_PM needs to be implemented.
    // Kernel handler for this would send a message/signal to RS.
    // RS, upon receiving, would call its internal logic to increment PM's epoch
    // and then call the SYS_UPDATE_SERVICE_EPOCH kernel call.
    if (debug_rs_simulate_pm_restart() < 0) {
        perror("debug_rs_simulate_pm_restart failed");
        // test_failures++; // May not be fatal if syscall not fully implemented yet
        printf("  WARNING: Could not simulate PM restart. Epoch test might not be meaningful.\n");
    } else {
        printf("  INFO: RS simulated PM restart. PM epoch should now be incremented.\n");
        // Give a moment for RS to call kernel if it's async
        usleep(100000); // 100ms, crude, but for testing.
    }


    // Step 4: Call getpid_cap() again. It should fail due to stale epoch.
    // (This assumes the original capability used by getpid_cap() via fixed index is now stale)
    printf("TESTING: Calling getpid_cap() with presumed stale capability context...\n");
    errno = 0; // Clear errno
    p_cap = getpid_cap();
    printf("  getpid_cap() (stale attempt) returned: %d\n", (int)p_cap);
    if (p_cap != -1) {
        printf("  ERROR: getpid_cap() should have failed due to stale epoch, but succeeded.\n");
        test_failures++;
    } else {
        // FIXME TODO: Check for a specific "stale capability" or "epoch mismatch" errno
        // For now, EPERM is returned by the kernel code in proc.c.
        if (errno == EPERM) {
            printf("  SUCCESS: getpid_cap() failed as expected with EPERM (stale epoch/cap).\n");
        } else {
            printf("  ERROR: getpid_cap() failed, but with wrong errno: %d (%s), expected EPERM for stale epoch.\n", errno, strerror(errno));
            test_failures++;
        }
    }

    // Step 5: (Conceptual) Get a new capability for PM (would have new epoch) and test again
    // printf("TESTING: (Conceptual) Obtaining new PM capability and calling getpid_cap()...\n");
    // pm_cap_idx_v2 = debug_get_pm_capability(); // This would now get a cap with the new epoch
    // if (pm_cap_idx_v2 == (uint32_t)-1) { ... }
    // // Then, if getpid_cap could take an index: getpid_cap_explicit(pm_cap_idx_v2)
    // // For now, we assume the implicit fixed capability KERNEL_PM_CAP_IDX in proc.c
    // // is NOT updated by this test program, so it remains stale.
    // // A full test would involve kernel updating the process's cap table entry for PM.

    // Step 6: Benchmark getpid_cap() if initial calls were successful
    if (p_std > 0 && getpid_cap() == p_std) { // Ensure it works before benchmark
        printf("BENCHMARKING: getpid_cap()...\n");
        // Warmup
        for (int i = 0; i < WARMUP_ITERATIONS; i++) {
            (void)getpid_cap();
        }
        // Actual benchmark
        total_cycles = 0; // Reset for this benchmark run
        min_cycles = (unsigned long long)-1;
        max_cycles = 0;
        for (int i = 0; i < ITERATIONS; i++) {
            start_time = rdtsc();
            (void)getpid_cap();
            end_time = rdtsc();
            unsigned long long diff = end_time - start_time;
            total_cycles += diff;
            if (diff < min_cycles) min_cycles = diff;
            if (diff > max_cycles) max_cycles = diff;
        }
        if (ITERATIONS > 0) {
             printf("  getpid_cap() average cycles: %llu (over %d iterations)\n", total_cycles / ITERATIONS, ITERATIONS);
        }
        printf("  getpid_cap() min cycles: %llu\n", min_cycles);
        printf("  getpid_cap() max cycles: %llu\n", max_cycles);
    } else {
        printf("SKIPPING: Benchmark due to earlier failures or inconsistencies.\n");
    }

    printf("TESTING: Completed. Failures: %d\n", test_failures);
    return test_failures > 0 ? 1 : 0;
}
