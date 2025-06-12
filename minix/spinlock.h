#pragma once
#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#include "config.h"

/** Forward declaration used for debugging ownership information. */
struct cpu;

/** Ticket-based mutual exclusion lock. */
struct ticketlock {
    _Atomic uint16_t head; /**< Next ticket number to serve. */
    _Atomic uint16_t tail; /**< Current ticket being served. */
};

/** Spin lock using a ticket lock for fairness. */
struct spinlock {
    struct ticketlock ticket; /**< Ticket lock implementation. */
    char *lock_name_ptr;      /**< Optional name for diagnostics. */
    uint32_t pcs[10];         /**< Call stack captured on acquire. */
    struct cpu *cpu;          /**< CPU that currently holds the lock. */
};

/** Size of a cache line in bytes, detected at runtime. */
extern size_t cache_line_size;

/** Detect the cache line size of the current system. */
void detect_cache_line_size(void);

/** Initialise a spin lock. */
void initlock(struct spinlock *lk, char *lock_name_ptr);

#if CONFIG_SMP && !defined(SPINLOCK_UNIPROCESSOR)
/** Acquire a spin lock. */
void acquire(struct spinlock *lk);

/** Release a spin lock. */
void release(struct spinlock *lk);
#endif

/**
 * Return the recommended alignment for instances of struct spinlock.
 * The value equals the runtime cache line size to avoid false sharing.
 */
static inline size_t spinlock_alignment(void) {
    if (cache_line_size == 0) {
        detect_cache_line_size();
    }
    return cache_line_size;
}
