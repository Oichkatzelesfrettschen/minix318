// minix/lib/libc/sys-minix/mathematical_internal.h
#ifndef _MATHEMATICAL_INTERNAL_H
#define _MATHEMATICAL_INTERNAL_H

#include <pthread.h> // For pthread_mutex_t
#include <sys/types.h> // For time_t, size_t
#include <limits.h>  // For PATH_MAX (usually defined here or in unistd.h)

// Placeholder for capability_t until <minix/capability.h> is defined.
// This allows struct cap_cache_entry to be defined.
typedef void* capability_t;

/* Mathematical verification modes */
#define MATH_MODE_DISABLED   0  /* No verification */
#define MATH_MODE_ENABLED    1  /* Verification when beneficial (e.g., user opt-in) */
#define MATH_MODE_MANDATORY  2  /* Always verify, fail operations if not possible or proof fails */
// #define MATH_MODE_AUTO (This was in parse_math_env, might be an internal resolution, not a stored mode)

/* Kernel policy flags (communicated by kernel via SYS_CAPABILITY_QUERY) */
#define POLICY_AVAILABLE        0x01 // Kernel supports the capability system
#define POLICY_MANDATORY_ALL    0x02 // Kernel mandates verification for all processes
#define POLICY_MANDATORY_PRIV   0x04 // Kernel mandates for privileged processes
#define POLICY_CACHE_KERNEL     0x08 // Kernel suggests/supports kernel-side caching (info only)

/* Cache entry structure */
struct cap_cache_entry {
    char path[PATH_MAX]; // PATH_MAX needs to be defined, e.g. via <limits.h> or <unistd.h>
    int flags;           // Open flags associated with this capability/path
    capability_t capability; // The actual capability data/proof (opaque for now)
    time_t expiry;       // Expiry time for the cached entry
    int valid;           // Is this cache entry currently valid?
    // Potentially add usage counters for LRU eviction, etc.
};

/* Global state structure for mathematical POSIX features within libc */
struct math_state {
    int initialized;    // Has __mathematical_init() run?
    int mode;           // Current operational mode (MATH_MODE_DISABLED, etc.)

    int kernel_support; // Does the running kernel support these capabilities? (from POLICY_AVAILABLE)
    int policy;         // Policy flags received from the kernel

    /* Cache management */
    int cache_enabled;
    struct cap_cache_entry *cache; // Pointer to the allocated cache memory
    size_t cache_size;             // Number of entries in the cache
    pthread_mutex_t cache_lock;    // Mutex for thread-safe cache access

    /* Statistics for debugging/monitoring (optional) */
    unsigned long verifications_attempted;
    unsigned long verifications_succeeded;
    unsigned long cache_hits;
    unsigned long cache_misses; // Added for completeness
};

/* Global state instance - defined in mathematical_init.c */
extern struct math_state __math_state;

/* --- Internal function prototypes --- */

/* From mathematical_init.c */
void __mathematical_init(void); // Ensures the system is initialized
int mathematical_set_mode(int mode); // Allows changing the mode at runtime (declared in mathposix.h too)

/* From mathematical_posix.c (or wherever they are implemented) */
int __should_use_mathematical(int flags); // Decides if math verification should be used for an operation

/* Cache functions (to be implemented, likely in a new cache management C file or mathematical_init.c) */
struct cap_cache_entry *__find_cached_capability(const char *path, int flags);
void __cache_capability(const char *path, int flags, capability_t cap, time_t expiry_time);
void __invalidate_cached_capability(const char *path, int flags); // Example of another cache op
void __init_capability_cache_subsystem(void); // If cache init is complex
void __destroy_capability_cache_subsystem(void); // If cleanup needed

#endif /* _MATHEMATICAL_INTERNAL_H */
