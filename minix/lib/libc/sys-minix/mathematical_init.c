// minix/lib/libc/sys-minix/mathematical_init.c
#include <stdlib.h>     // For getenv, calloc, atoi
#include <string.h>     // For strcmp, strcasecmp, memset
#include <unistd.h>     // For geteuid, getprogname (POSIX)
#include <errno.h>      // For errno, ENOSYS, EINVAL, EPERM
#include <stdio.h>      // For dprintf (if used for debug logging, needs fd 2)

// Internal libc header
#include "mathematical_internal.h"

// Minix headers for _syscall and message passing
#include <minix/ipc.h>
#include <minix/com.h>      // For SYSTEM endpoint (assuming this is PM_PROC_NR or similar for kernel calls)
#include <minix/callnr.h>   // For actual syscall numbers (SYS_CAPABILITY_QUERY to be added)
#include <lib.h>          // Assumed to provide _syscall and message utilities

// Placeholder for the new system call number for querying capability policy
// This will be formally defined in callnr.h in a later step.
// SYS_CAPABILITY_QUERY is now defined in <minix/callnr.h>
// M_POLICY_FIELD (m1_i1) is used directly in the code.


/* Global state for mathematical verification - definition */
struct math_state __math_state = {
    .initialized = 0,
    .mode = MATH_MODE_DISABLED, // Default to disabled
    .kernel_support = 0,        // Assume no support until proven
    .policy = 0,
    .cache_enabled = 0,
    .cache = NULL,
    .cache_size = 0,
    // .cache_lock = PTHREAD_MUTEX_INITIALIZER, // Static initialization if possible, else in init_cache
    .verifications_attempted = 0,
    .verifications_succeeded = 0,
    .cache_hits = 0,
    .cache_misses = 0
};

static void parse_math_env(const char *value) {
    if (!value || !*value) return;

    if (strcmp(value, "0") == 0) {
        __math_state.mode = MATH_MODE_DISABLED;
    } else if (strcmp(value, "1") == 0) {
        __math_state.mode = MATH_MODE_ENABLED;
    } else if (strcasecmp(value, "off") == 0 || strcasecmp(value, "disabled") == 0) {
        __math_state.mode = MATH_MODE_DISABLED;
    } else if (strcasecmp(value, "on") == 0 || strcasecmp(value, "enabled") == 0) {
        __math_state.mode = MATH_MODE_ENABLED;
    } else if (strcasecmp(value, "required") == 0 || strcasecmp(value, "mandatory") == 0) {
        __math_state.mode = MATH_MODE_MANDATORY;
    } else if (strcasecmp(value, "auto") == 0) {
        __math_state.mode = (geteuid() == 0) ? MATH_MODE_ENABLED : MATH_MODE_DISABLED;
    }
    // Else: unknown value, mode remains as previously set (e.g. default MATH_MODE_DISABLED)
}

static void query_kernel_policy(void) {
    message m;
    int r;

    memset(&m, 0, sizeof(m));
    m.m_type = SYS_CAPABILITY_QUERY; // Placeholder syscall number

    // Assuming SYSTEM is the endpoint for such kernel queries
    r = _syscall(SYSTEM, SYS_CAPABILITY_QUERY, &m);

    if (r == 0) { // Success
        // Assuming kernel returns policy flags in a specific message field
        // For example, if policy is in m.m_lsys_krn_sys_policy (as per feedback example)
        // or m.m1_i1 (a generic field). Let's use M_POLICY_FIELD.
        __math_state.policy = m.M_POLICY_FIELD;

        if (__math_state.policy & POLICY_AVAILABLE) {
            __math_state.kernel_support = 1;
        } else {
            __math_state.kernel_support = 0;
        }

        if ((__math_state.policy & POLICY_MANDATORY_ALL) && __math_state.kernel_support) {
            // If kernel mandates for all and supports it, override current mode.
            __math_state.mode = MATH_MODE_MANDATORY;
        } else if ((__math_state.policy & POLICY_MANDATORY_PRIV) && geteuid() == 0 && __math_state.kernel_support) {
            // If kernel mandates for privileged users and this is root and kernel supports it.
             __math_state.mode = MATH_MODE_MANDATORY;
        }

    } else if (errno == ENOSYS || r == -ENOSYS) { // Check for ENOSYS from _syscall
        // Kernel doesn't support the SYS_CAPABILITY_QUERY call or the feature.
        __math_state.kernel_support = 0;
        __math_state.policy = 0;
        // If user requested mandatory mode via ENV, operations should fail later
        // if __should_use_mathematical checks kernel_support for mandatory mode.
    } else {
        // Other error from _syscall
        __math_state.kernel_support = 0;
        __math_state.policy = 0;
        // Optionally log this error if a debug mechanism is available
        #ifdef DEBUG_MATHEMATICAL
        dprintf(2, "MATHEMATICAL_POSIX: query_kernel_policy syscall failed, error %d\n", errno);
        #endif
    }
}

static void init_cache(void) {
    const char *cache_env = getenv("MINIX_CAPABILITY_CACHE");
    const char *cache_size_env = getenv("MINIX_CAPABILITY_CACHE_SIZE");
    int desired_cache_size = 16;  // Default size

    // Initialize mutex first, regardless of cache allocation
    // PTHREAD_MUTEX_INITIALIZER can be used for static __math_state, but if __math_state
    // is not file-scope static, then pthread_mutex_init is better.
    if (pthread_mutex_init(&__math_state.cache_lock, NULL) != 0) {
        // Failed to initialize mutex, cannot enable cache.
        __math_state.cache_enabled = 0;
        __math_state.cache = NULL;
        __math_state.cache_size = 0;
        #ifdef DEBUG_MATHEMATICAL
        dprintf(2, "MATHEMATICAL_POSIX: Cache mutex init failed.\n");
        #endif
        return;
    }

    if (!cache_env || strcmp(cache_env, "0") == 0 || strcmp(cache_env, "off") == 0) {
        __math_state.cache_enabled = 0; // Cache explicitly disabled
        return;
    }

    if (cache_size_env) {
        int size = atoi(cache_size_env);
        if (size > 0 && size <= 1024) { // Reasonable bounds
            desired_cache_size = size;
        }
    }

    __math_state.cache = calloc(desired_cache_size, sizeof(struct cap_cache_entry));
    if (__math_state.cache) {
        __math_state.cache_size = desired_cache_size;
        __math_state.cache_enabled = 1;
    } else {
        // calloc failed
        __math_state.cache_enabled = 0;
        __math_state.cache_size = 0;
        #ifdef DEBUG_MATHEMATICAL
        dprintf(2, "MATHEMATICAL_POSIX: Cache allocation failed for size %d.\n", desired_cache_size);
        #endif
    }
}

__attribute__((constructor(101)))
void __mathematical_init(void) {
    const char *env_value;
    const char *prog_name;

    if (__math_state.initialized) return; // Prevent double initialization

    // Initialize default state, especially if __math_state wasn't statically initialized fully.
    // (It was, but being explicit here can be good if static init of mutex is an issue)
    // pthread_mutex_init(&__math_state.cache_lock, NULL); // Moved to init_cache for safety

    // Step 1: Check environment variable for initial mode setting
    env_value = getenv("MINIX_MATHEMATICAL_POSIX");
    if (env_value) {
        parse_math_env(env_value);
    }

    // Step 2: Query kernel for system policy and capability support
    query_kernel_policy();

    // Step 3: Initialize cache if requested via environment (and kernel supports feature if cache needs it)
    // Cache can be initialized even if kernel doesn't support the main feature,
    // as it might be a user-space only cache for some aspects.
    init_cache();

    // Step 4: Special handling for security-critical programs
    // This overrides env var and potentially kernel policy if kernel doesn't mandate it already.
    prog_name = getprogname(); // Standard POSIX way to get program name
    if (prog_name) {
        if (strcmp(prog_name, "login") == 0 ||
            strcmp(prog_name, "su") == 0 ||
            strcmp(prog_name, "passwd") == 0) {
            // Only upgrade to mandatory if kernel supports the feature.
            if (__math_state.kernel_support) {
                __math_state.mode = MATH_MODE_MANDATORY;
            } else {
                // If kernel doesn't support it, these critical programs might need to fail
                // or log a serious warning. For now, they'll run without math mode if not supported.
                // Or, if MATH_MODE_MANDATORY implies failure on no-support, that check happens later.
            }
        }
    }

    __math_state.initialized = 1;

    #ifdef DEBUG_MATHEMATICAL
    // dprintf usually writes to stderr (fd 2). Ensure it's available or use a kernel log if in early init.
    // For libc init, dprintf(2, ...) should be fine.
    dprintf(2, "MATHEMATICAL_POSIX: Libc init complete. Mode: %d, Kernel Support: %d, Cache Enabled: %d, Cache Size: %zu\n",
            __math_state.mode, __math_state.kernel_support, __math_state.cache_enabled, __math_state.cache_size);
    #endif
}

/* Allows runtime changes to configuration (e.g., by the application itself) */
int mathematical_set_mode(int mode) {
    if (!__math_state.initialized) {
        __mathematical_init(); // Ensure initialized before changing mode
    }

    if (mode < MATH_MODE_DISABLED || mode > MATH_MODE_MANDATORY) {
        errno = EINVAL;
        return -1;
    }

    // If kernel doesn't support the feature, don't allow enabling.
    if (mode != MATH_MODE_DISABLED && !__math_state.kernel_support) {
        errno = ENOSYS; // Feature not supported by kernel
        return -1;
    }

    // Can't downgrade from mandatory if it was set by kernel policy or critical program logic
    // This check might need to be more nuanced based on the source of the MANDATORY setting.
    // For now, if current mode is MANDATORY, assume it's fixed.
    if (__math_state.mode == MATH_MODE_MANDATORY && mode != MATH_MODE_MANDATORY) {
        // Check if kernel policy itself mandated it
        if (__math_state.policy & POLICY_MANDATORY_ALL ||
           (__math_state.policy & POLICY_MANDATORY_PRIV && geteuid() == 0) ) {
             errno = EPERM; // Cannot change mode if kernel policy mandated it.
             return -1;
        }
        // Allow if mandatory was set by env var or programmatically, but not kernel policy
    }

    __math_state.mode = mode;

    #ifdef DEBUG_MATHEMATICAL
    dprintf(2, "MATHEMATICAL_POSIX: Mode changed to %d\n", __math_state.mode);
    #endif
    return 0;
}

// Implementation for __should_use_mathematical (moved from plan step 5 to here for cohesion)
int __should_use_mathematical(int flags __attribute__((unused))) {
    if (!__math_state.initialized) {
        __mathematical_init();
    }

    if (__math_state.mode == MATH_MODE_DISABLED) {
        return 0; // False
    }
    if (__math_state.mode == MATH_MODE_ENABLED || __math_state.mode == MATH_MODE_MANDATORY) {
        // For MANDATORY mode, if kernel doesn't support, operations should ideally fail.
        // This check is usually done at the point of syscall in mathematical_posix.c
        // or open() could return error here.
        if (!__math_state.kernel_support && __math_state.mode == MATH_MODE_MANDATORY) {
            // This state implies an issue, as MANDATORY should only be set if kernel_support=1
            // or by user overriding env var, which is risky.
            // Let individual syscall wrappers handle ENOSYS based on kernel_support for MANDATORY.
            return 1; // True, attempt it, let it fail if kernel doesn't support
        }
        return 1; // True
    }
    return 0; // False by default
}

// Stubs for cache functions (moved from plan step 5 to here for cohesion)
struct cap_cache_entry *__find_cached_capability(const char *path, int flags) {
    (void)path; (void)flags;
    if (!__math_state.cache_enabled || !__math_state.cache) return NULL;

    pthread_mutex_lock(&__math_state.cache_lock);
    // TODO: Implement cache lookup logic
    // For now, always miss.
    pthread_mutex_unlock(&__math_state.cache_lock);
    __math_state.cache_misses++;
    return NULL;
}

void __cache_capability(const char *path, int flags, capability_t cap, time_t expiry_time) {
    (void)path; (void)flags; (void)cap; (void)expiry_time;
    if (!__math_state.cache_enabled || !__math_state.cache) return;

    pthread_mutex_lock(&__math_state.cache_lock);
    // TODO: Implement cache insertion/update logic (e.g. LRU)
    pthread_mutex_unlock(&__math_state.cache_lock);
}

void __invalidate_cached_capability(const char *path, int flags) {
    (void)path; (void)flags;
     if (!__math_state.cache_enabled || !__math_state.cache) return;
    pthread_mutex_lock(&__math_state.cache_lock);
    // TODO: Implement cache invalidation
    pthread_mutex_unlock(&__math_state.cache_lock);
}

void __init_capability_cache_subsystem(void) {
    // Actual cache init is in init_cache() called by __mathematical_init()
    // This could be used for more complex setup if needed.
}
void __destroy_capability_cache_subsystem(void) {
    if (__math_state.cache_enabled && __math_state.cache) {
        pthread_mutex_lock(&__math_state.cache_lock);
        free(__math_state.cache);
        __math_state.cache = NULL;
        __math_state.cache_enabled = 0;
        __math_state.cache_size = 0;
        pthread_mutex_unlock(&__math_state.cache_lock);
        pthread_mutex_destroy(&__math_state.cache_lock);
    }
}
// TODO: Add __attribute__((destructor)) for __destroy_capability_cache_subsystem if needed.
