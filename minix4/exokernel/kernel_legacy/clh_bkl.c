#include <minix/clhlock.h>
#include "kernel/kernel.h" // For MAX_CPUS, kprintf, panic, get_cpulocal_var (for cpu_id)
#include "arch_proto.h"  // For arch_get_cpu_id() if distinct, or arch_pause() later

// The global CLH Big Kernel Lock instance
clhlock_t global_bkl_clh_instance;

// Per-CPU QNodes for the global BKL
// Ensure MAX_CPUS is defined, typically in kernel/config.h or by kernel/kernel.h
#ifndef MAX_CPUS
#error "MAX_CPUS not defined. Required for per_cpu_bkl_qnodes."
// Define a fallback for environments where it might be missing, though kernel.h should have it.
// #define MAX_CPUS 1
#endif

#if MAX_CPUS > 1 && !defined(CONFIG_SMP)
#warning "MAX_CPUS > 1 but CONFIG_SMP not defined. CLH BKL is primarily for SMP."
// This is a compile-time warning, not an error.
#endif

// Align to cache line size if CACHE_LINE_SIZE is known and qnodes are per-CPU.
// For an array, this aligns the start of the array. Each element's padding handles internal alignment.
clh_node_t per_cpu_bkl_qnodes[MAX_CPUS] __attribute__((aligned(64))); // Assuming 64-byte cache line

// The dummy node for CLH initialization, representing the "lock free" state.
// It must be globally unique for this lock instance.
static clh_node_t clh_bkl_dummy_node = { .locked = ATOMIC_VAR_INIT(false) }; // C11 static init

// Function to get the current CPU's qnode for the BKL
clh_node_t *get_this_cpu_bkl_qnode(void) {
    unsigned int cpu_id;
#if defined(CONFIG_SMP)
    // In MINIX, cpuid is often obtained via get_cpulocal_var or arch_get_cpu_id()
    // Assuming get_cpulocal_var(cpu_id) is the correct way for the kernel.
    cpu_id = get_cpulocal_var(cpu_id);
#else
    cpu_id = 0; // On non-SMP, always CPU 0.
#endif

    if (cpu_id >= MAX_CPUS) {
        // This should not happen in a correctly configured and running system.
        panic("get_this_cpu_bkl_qnode: cpu_id (%u) out of bounds (MAX_CPUS: %d)", cpu_id, MAX_CPUS);
    }
    return &per_cpu_bkl_qnodes[cpu_id];
}

// Initialization function for the global CLH BKL
void init_global_clh_bkl(void) {
    // Initialize the global CLH lock's tail to point to the dummy node.
    // The dummy_node's 'locked' field is already false due to static initialization.
    atomic_init(&global_bkl_clh_instance.tail, &clh_bkl_dummy_node);
    global_bkl_clh_instance.owner = (unsigned)-1; // Indicates no owner (e.g., an invalid CPU ID)

#ifdef LOCK_STATS
    global_bkl_clh_instance.acquisitions = 0;
    global_bkl_clh_instance.contended_acquisitions = 0;
#endif

    // Initialize all per-CPU qnodes for the BKL.
    // Their 'locked' status should be false, indicating they are not currently in use.
    for (int i = 0; i < MAX_CPUS; ++i) {
        atomic_init(&per_cpu_bkl_qnodes[i].locked, false);
    }

    // kprintf may not be safe if called before console is up, or if BKL protects kprintf.
    // For early init, a simpler direct print or no print might be better.
    // Assuming kprintf is safe enough for this stage or this init is called after console.
    kprintf("Global CLH BKL initialized (MAX_CPUS: %d).\n", MAX_CPUS);
}
