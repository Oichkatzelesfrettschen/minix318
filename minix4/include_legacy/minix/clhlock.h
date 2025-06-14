/* minix/include/minix/clhlock.h */
#ifndef _MINIX_CLHLOCK_H
#define _MINIX_CLHLOCK_H

#include <stdatomic.h>
#include <stdbool.h>
// Potentially include minix/config.h or kernel/config.h for MAX_CPUS if not hardcoded
// For kernel code, kernel/kernel.h or kernel/config.h is more appropriate for MAX_CPUS
// #include <minix/sys_config.h> // This is more for user-space config

// Forward declare if needed, or ensure kernel.h or similar is included by .c files
// void kprintf_stub(const char *fmt, ...); // Example if needed

typedef struct clh_node {
    _Atomic(bool) locked;
    /* Pad to cache line size to prevent false sharing between nodes of different CPUs,
     * especially if per_cpu_qnodes array elements are contiguous.
     * Assuming CACHE_LINE_SIZE is defined, e.g., 64.
     */
    char padding[64 - sizeof(_Atomic(bool))];
} clh_node_t;

typedef struct clhlock {
    _Atomic(clh_node_t *) tail;
    volatile unsigned owner; /* Optional: for debugging, MINIX-specific BKL ownership tracking */
#ifdef LOCK_STATS
    unsigned long acquisitions;
    unsigned long contended_acquisitions;
#endif
} clhlock_t;

/*
 * clh_proc_state_t (previously clh_thread_state_t) for per-process/thread CLH lock usage
 * is not strictly needed for the global BKL itself, as the BKL uses a global
 * per-CPU array of qnodes. This structure is more for when individual processes
 * each have their own CLH lock or use CLH locks for other purposes.
 * For the BKL, each CPU will get its qnode from a global per-CPU array.
 */

/* Function prototypes for core CLH operations (expected to be in clhlock.c or similar) */
/* These would operate on a clhlock_t and a specific clh_node_t */
/* void clh_acquire_core(clhlock_t *lock, clh_node_t *my_node); */
/* void clh_release_core(clhlock_t *lock, clh_node_t *my_node); */
/* int clh_tryacquire_core(clhlock_t *lock, clh_node_t *my_node); */

#endif /* _MINIX_CLHLOCK_H */
