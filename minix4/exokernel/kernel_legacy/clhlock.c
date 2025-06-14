/* minix/kernel/clhlock.c */
#include <minix/clhlock.h>
// #include <minix/cpulocals.h> // For cpuid and arch_pause. Will use kernel/kernel.h for now.
#include "kernel/kernel.h" // For cpuid via get_cpulocal_var(cpu_id) and arch_pause(), kprintf_stub
#include "arch_proto.h"    // For arch_pause() if not in kernel.h, and potentially arch_get_cpu_id()

// Define get_current_cpu_id() based on available kernel functions
#if defined(CONFIG_SMP)
// Assuming get_cpulocal_var(cpu_id) or arch_get_cpu_id() is the way.
// If a direct arch_get_cpu_id() is available and preferred:
// #define get_current_cpu_id() arch_get_cpu_id()
// Otherwise, using get_cpulocal_var as a placeholder for MINIX's mechanism:
#define get_current_cpu_id() get_cpulocal_var(cpu_id)
#else
#define get_current_cpu_id() 0 // Single processor system
#endif

/*
 * kprintf_stub is used by assert_clhlock_held in the header.
 * It should be a weak alias or a simple implementation if full kprintf is too heavy.
 * For now, assume it's available or defined elsewhere in kernel.
 * If not, this provides a minimal definition.
 */
#if !defined(kprintf_stub)
void kprintf_stub(const char *fmt, ...) {
    // A very simple stub if no proper one is linked.
    // In a real scenario, this should point to a minimal kernel printf or be handled by linking.
    // This might involve using a varargs list and a low-level char output function.
    // For the purpose of this file creation, a non-functional stub is included.
    (void)fmt; // Suppress unused parameter warning
}
#endif


/* Global dummy node for CLH locks, representing the "lock available" state */
static clh_node_t clh_global_dummy_node = { .locked = ATOMIC_VAR_INIT(false) };

void clhlock_init(clhlock_t *lock)
{
    // Initialize the tail to point to the global dummy node.
    // The dummy_node's 'locked' field is already false due to static initialization.
    atomic_store_explicit(&lock->tail, &clh_global_dummy_node, memory_order_relaxed);
    lock->owner = (unsigned)-1; // Initialize owner to an invalid CPU ID (e.g., -1 or MAX_CPUS)

#ifdef LOCK_STATS
    lock->acquisitions = 0;
    lock->contended_acquisitions = 0;
#endif
}

void clhlock_lock(clhlock_t *lock, clh_proc_state_t *state)
{
    // Select the node this thread will use for this acquisition attempt.
    clh_node_t *node_to_use = &state->nodes[state->current_node_idx];

    // Mark this node as "wanting the lock".
    atomic_store_explicit(&node_to_use->locked, true, memory_order_relaxed);

    // Atomically swap this node into the lock's tail, getting the predecessor.
    clh_node_t *predecessor_node = atomic_exchange_explicit(&lock->tail, node_to_use, memory_order_acq_rel);

    // Store the predecessor; this is the node whose 'locked' flag we will spin on.
    state->my_pred = predecessor_node;

#ifdef LOCK_STATS
    // If the predecessor was already locked, this acquisition was contended.
    // memory_order_acquire to ensure we see the correct state of predecessor->locked.
    if (atomic_load_explicit(&predecessor_node->locked, memory_order_acquire)) {
        lock->contended_acquisitions++;
    }
    lock->acquisitions++;
#endif

    // Spin until the predecessor releases the lock (its 'locked' flag becomes false).
    while (atomic_load_explicit(&predecessor_node->locked, memory_order_acquire)) {
        arch_pause(); // Yield hint to the CPU (e.g., REP NOP on x86)
    }

    // Lock acquired.
    lock->owner = get_current_cpu_id();

    // For the *next* lock operation by this thread, 'my_node' (the node it will release)
    // will be 'node_to_use'. This is set so unlock knows which node to operate on.
    state->my_node = node_to_use;

    // Toggle current_node_idx to use the *other* node in state->nodes[] for the *next*
    // lock acquisition attempt by this thread. This "recycles" the predecessor's slot
    // (conceptually, as we use one of our two nodes) for this thread's future use.
    state->current_node_idx = 1 - state->current_node_idx;
}

void clhlock_unlock(clhlock_t *lock, clh_proc_state_t *state)
{
#if defined(CONFIG_SMP) && defined(DEBUG_LOCK_CHECK)
    // In a real kernel, cpuid is obtained via a reliable kernel function.
    // The assert in the header might be preferred, or use a direct check here.
    unsigned current_cpu = get_current_cpu_id();
    if (lock->owner != current_cpu) {
        // Use kprintf from kernel/kernel.h if available, otherwise kprintf_stub
        kprintf("PANIC: CLH lock %p unlocked by cpu %u, but owned by %u\n",
                     (void*)lock, current_cpu, lock->owner);
        // panic("clh_unlock_owner_mismatch"); // Or appropriate MINIX panic
        return; // Avoid proceeding if panic doesn't halt immediately
    }
#endif

    lock->owner = (unsigned)-1; // Mark as no owner

    // Release the lock by setting the 'locked' flag of *our current node* (state->my_node) to false.
    // This allows the successor (if any, spinning on state->my_node->locked) to acquire the lock.
    // A store-release memory barrier is crucial here.
    atomic_store_explicit(&state->my_node->locked, false, memory_order_release);

    // The node that this thread will use for its *next* lock acquisition attempt
    // has already been determined by toggling state->current_node_idx in clhlock_lock.
}

int clhlock_trylock(clhlock_t *lock, clh_proc_state_t *state)
{
    clh_node_t *current_tail;

    // Load current tail, ensure we see its 'locked' state correctly.
    current_tail = atomic_load_explicit(&lock->tail, memory_order_acquire);

    // Check if the lock appears free (i.e., the current tail node is not locked).
    if (atomic_load_explicit(&current_tail->locked, memory_order_acquire)) {
        return 0; // Lock is held by someone (or current tail is marked locked).
    }

    // Lock appears free. Try to acquire it by CASing our node as the new tail.
    // Select our node for this attempt.
    clh_node_t *node_to_use = &state->nodes[state->current_node_idx];
    atomic_store_explicit(&node_to_use->locked, true, memory_order_relaxed); // We intend to hold it.

    // Attempt to swap 'node_to_use' in if 'lock->tail' is still 'current_tail' (which was unlocked).
    if (atomic_compare_exchange_strong_explicit(&lock->tail, &current_tail, node_to_use,
                                                memory_order_acq_rel, memory_order_relaxed)) {
        // CAS succeeded: we are now the tail, and the previous tail was 'current_tail'.
        // Since current_tail->locked was false, we have acquired the lock.
        lock->owner = get_current_cpu_id();

        // Setup state for this acquisition, similar to clhlock_lock:
        state->my_pred = current_tail; // Our predecessor was the unlocked tail.
        state->my_node = node_to_use;  // This is the node we now "own".
        state->current_node_idx = 1 - state->current_node_idx; // Prepare for next acquisition.

        return 1; // Acquired lock
    }

    // CAS failed, someone else intervened. Lock not acquired.
    // Revert our node's state as it wasn't successfully enqueued.
    atomic_store_explicit(&node_to_use->locked, false, memory_order_relaxed);
    return 0;
}
