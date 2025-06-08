// minix/lib/libc/sys-minix/mathematical_init.c

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>

#include "mathematical_internal.h"

#include <minix/ipc.h>
#include <minix/com.h>
#include <minix/callnr.h>
#include <lib.h>

/* Global state for mathematical verification */
struct math_state __math_state = {
    .initialized         = 0,
    .mode                = MATH_MODE_DISABLED,
    .kernel_support      = 0,
    .policy              = 0,
    .cache_enabled       = 0,
    .cache               = NULL,
    .cache_size          = 0,
    .verifications_attempted  = 0,
    .verifications_succeeded  = 0,
    .cache_hits          = 0,
    .cache_misses        = 0
};

/* Parse MINIX_MATHEMATICAL_POSIX environment variable */
static void parse_math_env(const char *value) {
    if (!value || !*value) return;
    if (strcmp(value, "0") == 0 ||
        strcasecmp(value, "off") == 0 ||
        strcasecmp(value, "disabled") == 0) {
        __math_state.mode = MATH_MODE_DISABLED;
    } else if (strcmp(value, "1") == 0 ||
               strcasecmp(value, "on") == 0 ||
               strcasecmp(value, "enabled") == 0) {
        __math_state.mode = MATH_MODE_ENABLED;
    } else if (strcasecmp(value, "required") == 0 ||
               strcasecmp(value, "mandatory") == 0) {
        __math_state.mode = MATH_MODE_MANDATORY;
    } else if (strcasecmp(value, "audit") == 0) {
        __math_state.mode = MATH_MODE_AUDIT;
    } else if (strcasecmp(value, "auto") == 0) {
        __math_state.mode = (geteuid() == 0)
            ? MATH_MODE_ENABLED
            : MATH_MODE_DISABLED;
    }
}

/* Query kernel for global capability policy */
static void query_kernel_policy(void) {
    message m;
    int r;

    memset(&m, 0, sizeof(m));
    m.m_type = SYS_CAPABILITY_QUERY;

    r = _syscall(SYSTEM, SYS_CAPABILITY_QUERY, &m);
    if (r == 0) {
        __math_state.policy = m.m1_i1;
        __math_state.kernel_support =
            (__math_state.policy & POLICY_AVAILABLE) ? 1 : 0;
        if ((__math_state.policy & POLICY_MANDATORY_ALL) && __math_state.kernel_support) {
            __math_state.mode = MATH_MODE_MANDATORY;
        } else if ((__math_state.policy & POLICY_MANDATORY_PRIV) &&
                   geteuid() == 0 && __math_state.kernel_support) {
            __math_state.mode = MATH_MODE_MANDATORY;
        }
    } else if (errno == ENOSYS) {
        __math_state.kernel_support = 0;
        __math_state.policy         = 0;
    } else {
        __math_state.kernel_support = 0;
        __math_state.policy         = 0;
    }
}

/* Initialize or disable the capability cache */
static void init_cache(void) {
    const char *env = getenv("MINIX_CAPABILITY_CACHE_SIZE");
    int size = 16;

    pthread_mutex_init(&__math_state.cache_lock, NULL);
    if (env) {
        int v = atoi(env);
        if (v > 0) size = v;
    }

    __math_state.cache = calloc(size, sizeof(struct cap_cache_entry));
    if (__math_state.cache) {
        __math_state.cache_size    = size;
        __math_state.cache_enabled = 1;
    } else {
        __math_state.cache_enabled = 0;
    }
}

/* Tear down and free the capability cache */
static void destroy_cache(void) {
    if (__math_state.cache) {
        pthread_mutex_lock(&__math_state.cache_lock);
        free(__math_state.cache);
        __math_state.cache       = NULL;
        __math_state.cache_size  = 0;
        __math_state.cache_enabled = 0;
        pthread_mutex_unlock(&__math_state.cache_lock);
        pthread_mutex_destroy(&__math_state.cache_lock);
    }
}

/* Auto‐constructor: runs before main() */
__attribute__((constructor))
static void __mathematical_init(void) {
    if (__math_state.initialized) return;

    if (const char *env = getenv("MINIX_MATHEMATICAL_POSIX"))
        parse_math_env(env);

    query_kernel_policy();
    init_cache();

    if (const char *p = getprogname()) {
        if (!strcmp(p, "login") || !strcmp(p, "su") || !strcmp(p, "passwd")) {
            if (__math_state.kernel_support)
                __math_state.mode = MATH_MODE_MANDATORY;
        }
    }

    __math_state.initialized = 1;
}

/* Auto‐destructor: runs after exit() */
__attribute__((destructor))
static void __mathematical_fini(void) {
    if (!__math_state.initialized) return;
    destroy_cache();
    __math_state.initialized = 0;
}

/* Allow applications to change the math mode at runtime */
int mathematical_set_mode(int mode) {
    if (!__math_state.initialized) __mathematical_init();

    if (mode < MATH_MODE_DISABLED || mode > MATH_MODE_AUDIT) {
        errno = EINVAL; return -1;
    }
    if (mode != MATH_MODE_DISABLED && !__math_state.kernel_support) {
        errno = ENOSYS; return -1;
    }
    if (__math_state.mode == MATH_MODE_MANDATORY && mode != MATH_MODE_MANDATORY) {
        if ((__math_state.policy & POLICY_MANDATORY_ALL) ||
            ((__math_state.policy & POLICY_MANDATORY_PRIV) && geteuid() == 0)) {
            errno = EPERM; return -1;
        }
    }

    __math_state.mode = mode;
    return 0;
}

/* Internal helper: should we apply mathematical verification? */
int __should_use_mathematical(int flags __attribute__((unused))) {
    if (!__math_state.initialized) __mathematical_init();
    return __math_state.mode != MATH_MODE_DISABLED;
}
