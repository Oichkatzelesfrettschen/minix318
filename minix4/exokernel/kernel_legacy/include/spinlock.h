#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

#include "kernel/kernel.h"

// Added kernel headers (precautionary for consistency)
#include <minix/kernel_types.h>
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <klib/include/kmemory.h>


typedef struct spinlock {
	atomic_t val;
} spinlock_t;

#ifndef CONFIG_SMP

#define SPINLOCK_DEFINE(name)
#define PRIVATE_SPINLOCK_DEFINE(name)
#define SPINLOCK_DECLARE(name)
#define spinlock_init(sl)
#define spinlock_lock(sl)
#define spinlock_unlock(sl)

#else

/* SMP */
#define SPINLOCK_DEFINE(name)	spinlock_t name;
#define PRIVATE_SPINLOCK_DEFINE(name)	PRIVATE SPINLOCK_DEFINE(name)
#define SPINLOCK_DECLARE(name)	extern SPINLOCK_DEFINE(name)
#define spinlock_init(sl) do { (sl)->val = 0; } while (0)

#if CONFIG_MAX_CPUS == 1
#define spinlock_lock(sl)
#define spinlock_unlock(sl)
#else
void arch_spinlock_lock(atomic_t * sl);
void arch_spinlock_unlock(atomic_t * sl);
#define spinlock_lock(sl)	arch_spinlock_lock((atomic_t*) sl)
#define spinlock_unlock(sl)	arch_spinlock_unlock((atomic_t*) sl)
#endif


#endif /* CONFIG_SMP */

// Make sure this doesn't conflict with other includes of kernel.h
// #include "kernel/kernel.h" // Already included at the top

#if defined(CONFIG_SMP)
// CLH BKL specific includes and declarations
#include <minix/clhlock.h> // Provides clhlock_t, clh_node_t

// External declarations for the global CLH BKL instance and qnode getter
extern clhlock_t global_bkl_clh_instance; // Defined in clh_bkl.c
extern clh_node_t *get_this_cpu_bkl_qnode(void); // Defined in clh_bkl.c

// External declarations for CLH core lock operations
// These are assumed to be implemented in a file like clhlock.c or clh_bkl.c
extern void clh_core_acquire(clhlock_t *lock, clh_node_t *my_node);
extern void clh_core_release(clhlock_t *lock, clh_node_t *my_node);

// Assume profiling functions and constants are declared elsewhere
// extern void ktzprofile_event(int event_type);
// enum KTRACE_EVENTS { KTRACE_BKL_TRY, KTRACE_BKL_ACQUIRE, KTRACE_BKL_RELEASE, ... };

#undef BKL_LOCK
#define BKL_LOCK() \
    do { \
        /* ktzprofile_event(KTRACE_BKL_TRY); // Profiling call */ \
        clh_core_acquire(&global_bkl_clh_instance, get_this_cpu_bkl_qnode()); \
        /* ktzprofile_event(KTRACE_BKL_ACQUIRE); // Profiling call */ \
    } while (0)

#undef BKL_UNLOCK
#define BKL_UNLOCK() \
    do { \
        /* ktzprofile_event(KTRACE_BKL_RELEASE); // Profiling call */ \
        clh_core_release(&global_bkl_clh_instance, get_this_cpu_bkl_qnode()); \
    } while (0)

#else /* !CONFIG_SMP */

// For non-SMP, BKL typically does nothing or very little (e.g., just disable interrupts)
// The original non-SMP BKL_LOCK/UNLOCK were no-ops because spinlock_lock/unlock were no-ops.
// We maintain that behavior.
#undef BKL_LOCK
#define BKL_LOCK()         do { /* ktzprofile_event(KTRACE_BKL_TRY); */ /* ktzprofile_event(KTRACE_BKL_ACQUIRE); */ } while (0)

#undef BKL_UNLOCK
#define BKL_UNLOCK()       do { /* ktzprofile_event(KTRACE_BKL_RELEASE); */ } while (0)

#endif /* CONFIG_SMP */

#endif /* __SPINLOCK_H__ */
