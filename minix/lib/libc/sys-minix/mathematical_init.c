// minix/lib/libc/sys-minix/mathematical_init.c
#define _GNU_SOURCE // For getprogname, pthread_atfork
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mathematical_internal.h" // Advanced version

#include <lib.h>          // Assumed for _syscall
#include <minix/callnr.h> // For SYS_CAPABILITY_QUERY
#include <minix/com.h>
#include <minix/ipc.h>

/* Global state for mathematical verification - definition */
struct math_state __math_state = {
    .initialized = 0,
    .mode = MATH_MODE_DISABLED,
    .kernel_support = 0,
    .policy = 0,
    .cache_enabled = 0,
    .cache_hash_table = NULL,
    .cache_hash_size = 64, // Default hash size, can be tuned
    .cache_lru_head = NULL,
    .cache_lru_tail = NULL,
    .cache_current_entries = 0,
    .cache_max_entries = 16, // Default max entries
    // .cache_lock = PTHREAD_MUTEX_INITIALIZER, // Initialize dynamically
    .stats = {0},
    // .thread_state_key = (pthread_key_t)0, // Will be created
    .thread_state_key_initialized = 0,
    .kernel_notify_fd = -1,
    .debug_level = 0,
    .audit_log_file = NULL};

// Forward declaration for TLS cleanup
static void thread_state_destroy(void *st);

// --- Static helper function implementations from previous version
// (parse_math_env) ---
static void parse_math_env(const char *value) {
  if (!value || !*value)
    return;
  // This sets an initial mode based on ENV, final mode determined after kernel
  // query & critical app check
  if (strcmp(value, "0") == 0) {
    __math_state.mode = MATH_MODE_DISABLED;
  } else if (strcmp(value, "1") == 0) {
    __math_state.mode = MATH_MODE_ENABLED;
  } else if (strcasecmp(value, "off") == 0 ||
             strcasecmp(value, "disabled") == 0) {
    __math_state.mode = MATH_MODE_DISABLED;
  } else if (strcasecmp(value, "on") == 0 ||
             strcasecmp(value, "enabled") == 0) {
    __math_state.mode = MATH_MODE_ENABLED;
  } else if (strcasecmp(value, "required") == 0 ||
             strcasecmp(value, "mandatory") == 0) {
    __math_state.mode = MATH_MODE_MANDATORY;
  } else if (strcasecmp(value, "audit") == 0) {
    __math_state.mode = MATH_MODE_AUDIT;
  } else if (strcasecmp(value, "auto") == 0) {
    __math_state.mode =
        (geteuid() == 0) ? MATH_MODE_ENABLED : MATH_MODE_DISABLED;
  }
}

// --- Implementation of functions from mathematical_internal.h ---

void __init_capability_cache_subsystem(void) {
  const char *cache_env = getenv("MINIX_CAPABILITY_CACHE");
  const char *cache_size_env = getenv("MINIX_CAPABILITY_CACHE_SIZE");
  size_t desired_cache_max_entries = 16; // Default

  if (pthread_mutex_init(&__math_state.cache_lock, NULL) != 0) {
    __math_state.cache_enabled = 0; // Cannot enable without lock
#ifdef DEBUG_MATHEMATICAL
    if (__math_state.debug_level > 0)
      dprintf(2, "MATHEMATICAL_POSIX: Cache mutex init failed.\n");
#endif
    return;
  }

  if (!cache_env || strcmp(cache_env, "0") == 0 ||
      strcasecmp(cache_env, "off") == 0) {
    __math_state.cache_enabled = 0;
    return; // Explicitly disabled
  }

  if (cache_size_env) {
    long size = atol(cache_size_env);
    if (size > 0 && size <= 10240) { // Max 10k entries
      desired_cache_max_entries = (size_t)size;
    }
  }
  __math_state.cache_max_entries = desired_cache_max_entries;

  // Default hash size, could also be configurable
  __math_state.cache_hash_size = 64;
  __math_state.cache_hash_table =
      calloc(__math_state.cache_hash_size, sizeof(struct cap_cache_entry *));
  if (!__math_state.cache_hash_table) {
    __math_state.cache_enabled = 0;
    pthread_mutex_destroy(&__math_state.cache_lock);
#ifdef DEBUG_MATHEMATICAL
    if (__math_state.debug_level > 0)
      dprintf(2, "MATHEMATICAL_POSIX: Cache hash table allocation failed.\n");
#endif
    return;
  }

  __math_state.cache_lru_head = NULL;
  __math_state.cache_lru_tail = NULL;
  __math_state.cache_current_entries = 0;
  __math_state.cache_enabled = 1;
}

void __destroy_capability_cache_subsystem(void) {
  if (__math_state.cache_hash_table) { // Check if hash table was allocated
    // Lock needed only if freeing entries, which should be done by
    // __flush_capability_cache
    if (__math_state.cache_enabled) { // If enabled, implies lock was init'd
      __flush_capability_cache();     // Clear entries first
    }
    free(__math_state.cache_hash_table);
    __math_state.cache_hash_table = NULL;
  }
  // Attempt to destroy mutex if it might have been initialized
  // It's generally safe to destroy an initialized mutex.
  // If init failed before mutex_init, this won't be called due to initialized
  // flag. If mutex_init failed, cache_enabled would be 0.
  pthread_mutex_destroy(&__math_state.cache_lock);

  __math_state.cache_enabled = 0;
  __math_state.cache_current_entries = 0;
  __math_state.cache_lru_head = NULL;
  __math_state.cache_lru_tail = NULL;
}

int __query_kernel_policy(unsigned int *policy_flags_from_kernel) {
  message m;
  int r;
  if (!policy_flags_from_kernel)
    return MATH_ERR_INVALID_ARG;

  memset(&m, 0, sizeof(m));
  m.m_type = SYS_CAPABILITY_QUERY;
  r = _syscall(SYSTEM, SYS_CAPABILITY_QUERY, &m);

  if (r == 0) {
    *policy_flags_from_kernel =
        (unsigned int)m.m1_i1; // Kernel returns policy in m1_i1
    return 0;                  // OK
  } else {
    *policy_flags_from_kernel = 0;
    return (r == -ENOSYS || errno == ENOSYS) ? MATH_ERR_NO_KERNEL_SUPPORT
                                             : MATH_ERR_KERNEL_COMM;
  }
}

int __register_kernel_notifications(void) {
  // TODO: Implement kernel notification registration (e.g., via a new syscall)
  __math_state.kernel_notify_fd = -1; // Mark as not implemented/not open
  return 0;                           // Success for stub
}

__attribute__((constructor(101))) void __mathematical_init(void) {
  const char *env_value;
  const char *prog_name;
  unsigned int kernel_policy_flags = 0;
  int query_result;

  if (__math_state.initialized)
    return;
  pthread_mutex_init(&__math_state.cache_lock,
                     NULL); // Initialize here, before any potential use by
                            // other functions if called early.

  if (!__math_state.thread_state_key_initialized) {
    if (pthread_key_create(&__math_state.thread_state_key,
                           thread_state_destroy) == 0) {
      __math_state.thread_state_key_initialized = 1;
    } else {
      __math_state.thread_state_key_initialized = 0;
#ifdef DEBUG_MATHEMATICAL
      if (__math_state.debug_level > 0)
        dprintf(2, "MATHEMATICAL_POSIX: pthread_key_create failed! Per-thread "
                   "state disabled.\n");
#endif
    }
  }

  env_value = getenv("MINIX_MATHEMATICAL_POSIX");
  if (env_value) {
    parse_math_env(env_value);
  }

  query_result = __query_kernel_policy(&kernel_policy_flags);
  __math_state.policy = kernel_policy_flags;

  if (query_result == 0 && (kernel_policy_flags & POLICY_AVAILABLE)) {
    __math_state.kernel_support = 1;
    if (kernel_policy_flags & POLICY_MANDATORY_ALL) {
      __math_state.mode = MATH_MODE_MANDATORY;
    } else if ((kernel_policy_flags & POLICY_MANDATORY_PRIV) &&
               (geteuid() == 0)) {
      __math_state.mode = MATH_MODE_MANDATORY;
    }
    // Note: AUDIT can be combined, not exclusive. If kernel suggests AUDIT,
    // libc might enable its audit logging.
    if (kernel_policy_flags & POLICY_AUDIT_ALL &&
        __math_state.mode != MATH_MODE_DISABLED) {
      if (__math_state.mode != MATH_MODE_MANDATORY)
        __math_state.mode = MATH_MODE_AUDIT; // Or combine with flags
    }
  } else {
    __math_state.kernel_support = 0;
    if (__math_state.mode != MATH_MODE_DISABLED) {
#ifdef DEBUG_MATHEMATICAL
      if (__math_state.debug_level > 0)
        dprintf(2,
                "MATHEMATICAL_POSIX: Kernel query failed or feature "
                "unavailable (res:%d). Forcing mode to DISABLED from %d.\n",
                query_result, __math_state.mode);
#endif
      __math_state.mode = MATH_MODE_DISABLED;
    }
  }

  __init_capability_cache_subsystem(); // Uses env vars, sets up cache_enabled
  __register_kernel_notifications();   // Stub

  prog_name = getprogname();
  if (prog_name) {
    if (strcmp(prog_name, "login") == 0 || strcmp(prog_name, "su") == 0 ||
        strcmp(prog_name, "passwd") == 0) {
      if (__math_state.kernel_support) {
        __math_state.mode = MATH_MODE_MANDATORY;
      } else { /* Critical program, no kernel support: mode remains DISABLED */
      }
    }
  }

  __math_state.initialized = 1;
  pthread_atfork(NULL, NULL, __mathematical_atfork_child);

#ifdef DEBUG_MATHEMATICAL
  if (__math_state.debug_level > 0) {
    dprintf(2,
            "MATHEMATICAL_POSIX: Libc init complete. Mode: %d, Kernel Support: "
            "%d, Policy: 0x%x, Cache: %s, Max Entries: %zu, Hash Size: %zu\n",
            __math_state.mode, __math_state.kernel_support, __math_state.policy,
            __math_state.cache_enabled ? "Enabled" : "Disabled",
            __math_state.cache_max_entries, __math_state.cache_hash_size);
  }
#endif
}

__attribute__((destructor(101))) void __mathematical_fini(void) {
  if (!__math_state.initialized)
    return;
  __destroy_capability_cache_subsystem();
  if (__math_state.kernel_notify_fd != -1) {
    close(__math_state.kernel_notify_fd);
    __math_state.kernel_notify_fd = -1;
  }
  if (__math_state.audit_log_file) {
    fclose(__math_state.audit_log_file);
    __math_state.audit_log_file = NULL;
  }
  if (__math_state.thread_state_key_initialized) {
    pthread_key_delete(__math_state.thread_state_key);
    __math_state.thread_state_key_initialized = 0;
  }
  __math_state.initialized = 0;
#ifdef DEBUG_MATHEMATICAL
  if (__math_state.debug_level > 0)
    dprintf(2, "MATHEMATICAL_POSIX: Libc fini complete.\n");
#endif
}

void __mathematical_atfork_child(void) {
  // Re-initialize locks for the child process
  if (__math_state
          .cache_enabled) { // Only re-init if it was successfully enabled.
    pthread_mutex_init(&__math_state.cache_lock, NULL);
  } else {
    // If cache wasn't enabled, but lock was attempted and failed, it still
    // needs init for destroy. This path is tricky. Simpler: cache_lock is
    // always init'd in __mathematical_init.
  }
  __flush_capability_cache();
  if (__math_state.kernel_notify_fd != -1) {
    close(__math_state.kernel_notify_fd);
    __register_kernel_notifications();
  }
  memset(&__math_state.stats, 0, sizeof(__math_state.stats));
  // Child does NOT re-run constructor. If TLS key needs re-init for child main
  // thread, it's complex. pthread_key_create is per-process. Child inherits the
  // key. Values are not inherited.
  // __get_thread_state will handle creating value for child's main thread when
  // first called.
}

int __mathematical_reinit(void) {
  if (!__math_state.initialized) {
    __mathematical_init();
    return 0;
  }
  unsigned int pflags = 0;
  __query_kernel_policy(&pflags);
  __math_state.policy = pflags;
  __math_state.kernel_support = (pflags & POLICY_AVAILABLE) ? 1 : 0;
  // Re-evaluate mode based on new policy, but don't touch env-var based
  // settings. This is mostly for refreshing kernel state.
  return 0;
}

struct thread_math_state *__get_thread_state(int create_if_missing) {
  if (!__math_state.thread_state_key_initialized)
    return NULL;
  void *st_ptr = pthread_getspecific(__math_state.thread_state_key);
  if (!st_ptr && create_if_missing) {
    st_ptr = calloc(1, sizeof(struct thread_math_state));
    if (st_ptr) {
      struct thread_math_state *ts = (struct thread_math_state *)st_ptr;
      ts->thread_mode = __math_state.mode; // Inherit global by default
      if (pthread_setspecific(__math_state.thread_state_key, st_ptr) != 0) {
        free(st_ptr);
        return NULL;
      }
    }
  }
  return (struct thread_math_state *)st_ptr;
}

static void thread_state_destroy(void *st) {
  if (st)
    free(st);
}

int mathematical_set_mode(int mode) {
  if (!__math_state.initialized) {
    __mathematical_init();
  }
  if (mode < MATH_MODE_DISABLED || mode > MATH_MODE_AUDIT) {
    errno = EINVAL;
    return -1;
  }
  if (mode != MATH_MODE_DISABLED && !__math_state.kernel_support) {
    errno = ENOSYS;
    return -1;
  }
  if (__math_state.mode == MATH_MODE_MANDATORY && mode != MATH_MODE_MANDATORY) {
    if ((__math_state.policy & POLICY_MANDATORY_ALL) ||
        ((__math_state.policy & POLICY_MANDATORY_PRIV) && geteuid() == 0)) {
      errno = EPERM;
      return -1;
    }
  }
  // Thread-specific lock check (simplified: assume for now global set_mode
  // affects global __math_state.mode) A per-thread set_mode would be: struct
  // thread_math_state *ts = __get_thread_state(1); if(!ts) return -1; ...
  // ts->thread_mode = mode;
  __math_state.mode = (enum math_mode)mode;
#ifdef DEBUG_MATHEMATICAL
  if (__math_state.debug_level > 0)
    dprintf(2, "MATHEMATICAL_POSIX: Global mode changed to %d\n",
            __math_state.mode);
#endif
  return 0;
}

// Stubs for remaining functions from mathematical_internal.h
// These will be moved to their own files or filled in later.

enum math_mode __get_effective_mode(void) {
  if (!__math_state.initialized)
    __mathematical_init();
  struct thread_math_state *ts = __get_thread_state(0);
  if (ts && ts->mode_is_set)
    return ts->thread_mode;
  return __math_state.mode;
}

// __should_use_mathematical was implemented based on __get_effective_mode and
// kernel_support in the previous version of this file. That logic is still
// largely correct.
int __should_use_mathematical(int flags __attribute__((unused))) {
  if (!__math_state.initialized)
    __mathematical_init();
  enum math_mode effective_mode = __get_effective_mode();
  if (effective_mode == MATH_MODE_DISABLED)
    return 0;
  if (!__math_state.kernel_support && (effective_mode == MATH_MODE_MANDATORY ||
                                       effective_mode == MATH_MODE_ENABLED ||
                                       effective_mode == MATH_MODE_AUDIT))
    return 0;
  return 1;
}

// Cache function stubs (basic implementations, real logic in
// mathematical_cache.c)
struct cap_cache_entry *__find_cached_capability(const char *path, int flags,
                                                 uid_t uid, gid_t gid) {
  (void)path;
  (void)flags;
  (void)uid;
  (void)gid;
  if (!__math_state.cache_enabled || !__math_state.cache_hash_table) {
    __math_state.stats.cache_misses++;
    return NULL;
  }
  pthread_mutex_lock(&__math_state.cache_lock);
  // Actual lookup logic placeholder
  pthread_mutex_unlock(&__math_state.cache_lock);
  __math_state.stats.cache_misses++; // Assume miss for stub
  return NULL;
}
int __cache_capability(const char *path, int flags, uid_t uid, gid_t gid,
                       const struct internal_capability *cap,
                       time_t expiry_abs) {
  (void)path;
  (void)flags;
  (void)uid;
  (void)gid;
  (void)cap;
  (void)expiry_abs;
  if (!__math_state.cache_enabled || !__math_state.cache_hash_table)
    return MATH_ERR_CACHE_FULL;
  pthread_mutex_lock(&__math_state.cache_lock);
  // Actual cache add logic placeholder
  pthread_mutex_unlock(&__math_state.cache_lock);
  return 0;
}
void __evict_expired_capabilities(void) { /* TODO */ }
void __flush_capability_cache(
    void) { /* TODO: Implemented in __destroy_capability_cache_subsystem, make
               it callable */
}
void __invalidate_path_capabilities(const char *path) { (void)path; /* TODO */ }

// Kernel communication stubs
int __notify_kernel_verification(const char *path, int result_code) {
  (void)path;
  (void)result_code;
  return 0;
}
// __register_kernel_notifications is above

// Proof function stubs
struct capability_proof *__construct_capability_proof_libc(endpoint_t proc_ep,
                                                           const char *path,
                                                           int rights) {
  (void)proc_ep;
  (void)path;
  (void)rights;
  errno = ENOSYS;
  return NULL;
}
int __verify_capability_proof_libc(const struct capability_proof *proof) {
  (void)proof;
  errno = ENOSYS;
  return MATH_ERR_NO_KERNEL_SUPPORT;
}
void __free_capability_proof_libc(struct capability_proof *proof) {
  (void)proof;
}

// Debugging and diagnostics stubs
void __log_capability_operation(const char *op, const char *path, int result,
                                const char *detail) {
  (void)op;
  (void)path;
  (void)result;
  (void)detail; /* TODO */
}
void __dump_capability_stats(FILE *stream) { (void)stream; /* TODO */ }
void __enable_capability_debugging(int level) {
  __math_state.debug_level = level;
}
void __set_audit_log_file(FILE *log_file) { /* TODO: handle old file if open */
  __math_state.audit_log_file = log_file;
}

// Security-sensitive op stubs
int __validate_capability_transition(const capability_t *from,
                                     const capability_t *to, int op) {
  (void)from;
  (void)to;
  (void)op;
  return 0;
}
int __check_capability_tampering(const struct internal_capability *cap) {
  (void)cap;
  return 0;
}
// minix/lib/libc/sys-minix/mathematical_init.c
#include <errno.h>  // For errno, ENOSYS, EINVAL, EPERM
#include <stdio.h>  // For dprintf (if used for debug logging, needs fd 2)
#include <stdlib.h> // For getenv, calloc, atoi
#include <string.h> // For strcmp, strcasecmp, memset
#include <unistd.h> // For geteuid, getprogname (POSIX)

// Internal libc header
#include "mathematical_internal.h"

// Minix headers for _syscall and message passing
#include <lib.h> // Assumed to provide _syscall and message utilities
#include <minix/callnr.h> // For actual syscall numbers (SYS_CAPABILITY_QUERY to be added)
#include <minix/com.h> // For SYSTEM endpoint (assuming this is PM_PROC_NR or similar for kernel calls)
#include <minix/ipc.h>

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
    // .cache_lock = PTHREAD_MUTEX_INITIALIZER, // Static initialization if
    // possible, else in init_cache
    .verifications_attempted = 0,
    .verifications_succeeded = 0,
    .cache_hits = 0,
    .cache_misses = 0};

static void parse_math_env(const char *value) {
  if (!value || !*value)
    return;

  if (strcmp(value, "0") == 0) {
    __math_state.mode = MATH_MODE_DISABLED;
  } else if (strcmp(value, "1") == 0) {
    __math_state.mode = MATH_MODE_ENABLED;
  } else if (strcasecmp(value, "off") == 0 ||
             strcasecmp(value, "disabled") == 0) {
    __math_state.mode = MATH_MODE_DISABLED;
  } else if (strcasecmp(value, "on") == 0 ||
             strcasecmp(value, "enabled") == 0) {
    __math_state.mode = MATH_MODE_ENABLED;
  } else if (strcasecmp(value, "required") == 0 ||
             strcasecmp(value, "mandatory") == 0) {
    __math_state.mode = MATH_MODE_MANDATORY;
  } else if (strcasecmp(value, "auto") == 0) {
    __math_state.mode =
        (geteuid() == 0) ? MATH_MODE_ENABLED : MATH_MODE_DISABLED;
  }
  // Else: unknown value, mode remains as previously set (e.g. default
  // MATH_MODE_DISABLED)
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
    // For example, if policy is in m.m_lsys_krn_sys_policy (as per feedback
    // example) or m.m1_i1 (a generic field). Let's use M_POLICY_FIELD.
    __math_state.policy = m.M_POLICY_FIELD;

    if (__math_state.policy & POLICY_AVAILABLE) {
      __math_state.kernel_support = 1;
    } else {
      __math_state.kernel_support = 0;
    }

    if ((__math_state.policy & POLICY_MANDATORY_ALL) &&
        __math_state.kernel_support) {
      // If kernel mandates for all and supports it, override current mode.
      __math_state.mode = MATH_MODE_MANDATORY;
    } else if ((__math_state.policy & POLICY_MANDATORY_PRIV) &&
               geteuid() == 0 && __math_state.kernel_support) {
      // If kernel mandates for privileged users and this is root and kernel
      // supports it.
      __math_state.mode = MATH_MODE_MANDATORY;
    }

  } else if (errno == ENOSYS ||
             r == -ENOSYS) { // Check for ENOSYS from _syscall
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
    dprintf(
        2, "MATHEMATICAL_POSIX: query_kernel_policy syscall failed, error %d\n",
        errno);
#endif
  }
}

static void init_cache(void) {
  const char *cache_env = getenv("MINIX_CAPABILITY_CACHE");
  const char *cache_size_env = getenv("MINIX_CAPABILITY_CACHE_SIZE");
  int desired_cache_size = 16; // Default size

  // Initialize mutex first, regardless of cache allocation
  // PTHREAD_MUTEX_INITIALIZER can be used for static __math_state, but if
  // __math_state is not file-scope static, then pthread_mutex_init is better.
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

  if (!cache_env || strcmp(cache_env, "0") == 0 ||
      strcmp(cache_env, "off") == 0) {
    __math_state.cache_enabled = 0; // Cache explicitly disabled
    return;
  }

  if (cache_size_env) {
    int size = atoi(cache_size_env);
    if (size > 0 && size <= 1024) { // Reasonable bounds
      desired_cache_size = size;
    }
  }

  __math_state.cache =
      calloc(desired_cache_size, sizeof(struct cap_cache_entry));
  if (__math_state.cache) {
    __math_state.cache_size = desired_cache_size;
    __math_state.cache_enabled = 1;
  } else {
    // calloc failed
    __math_state.cache_enabled = 0;
    __math_state.cache_size = 0;
#ifdef DEBUG_MATHEMATICAL
    dprintf(2, "MATHEMATICAL_POSIX: Cache allocation failed for size %d.\n",
            desired_cache_size);
#endif
  }
}

__attribute__((constructor(101))) void __mathematical_init(void) {
  const char *env_value;
  const char *prog_name;

  if (__math_state.initialized)
    return; // Prevent double initialization

  // Initialize default state, especially if __math_state wasn't statically
  // initialized fully. (It was, but being explicit here can be good if static
  // init of mutex is an issue) pthread_mutex_init(&__math_state.cache_lock,
  // NULL); // Moved to init_cache for safety

  // Step 1: Check environment variable for initial mode setting
  env_value = getenv("MINIX_MATHEMATICAL_POSIX");
  if (env_value) {
    parse_math_env(env_value);
  }

  // Step 2: Query kernel for system policy and capability support
  query_kernel_policy();

  // Step 3: Initialize cache if requested via environment (and kernel supports
  // feature if cache needs it) Cache can be initialized even if kernel doesn't
  // support the main feature, as it might be a user-space only cache for some
  // aspects.
  init_cache();

  // Step 4: Special handling for security-critical programs
  // This overrides env var and potentially kernel policy if kernel doesn't
  // mandate it already.
  prog_name = getprogname(); // Standard POSIX way to get program name
  if (prog_name) {
    if (strcmp(prog_name, "login") == 0 || strcmp(prog_name, "su") == 0 ||
        strcmp(prog_name, "passwd") == 0) {
      // Only upgrade to mandatory if kernel supports the feature.
      if (__math_state.kernel_support) {
        __math_state.mode = MATH_MODE_MANDATORY;
      } else {
        // If kernel doesn't support it, these critical programs might need to
        // fail or log a serious warning. For now, they'll run without math mode
        // if not supported. Or, if MATH_MODE_MANDATORY implies failure on
        // no-support, that check happens later.
      }
    }
  }

  __math_state.initialized = 1;

#ifdef DEBUG_MATHEMATICAL
  // dprintf usually writes to stderr (fd 2). Ensure it's available or use a
  // kernel log if in early init. For libc init, dprintf(2, ...) should be fine.
  dprintf(2,
          "MATHEMATICAL_POSIX: Libc init complete. Mode: %d, Kernel Support: "
          "%d, Cache Enabled: %d, Cache Size: %zu\n",
          __math_state.mode, __math_state.kernel_support,
          __math_state.cache_enabled, __math_state.cache_size);
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

  // Can't downgrade from mandatory if it was set by kernel policy or critical
  // program logic This check might need to be more nuanced based on the source
  // of the MANDATORY setting. For now, if current mode is MANDATORY, assume
  // it's fixed.
  if (__math_state.mode == MATH_MODE_MANDATORY && mode != MATH_MODE_MANDATORY) {
    // Check if kernel policy itself mandated it
    if (__math_state.policy & POLICY_MANDATORY_ALL ||
        (__math_state.policy & POLICY_MANDATORY_PRIV && geteuid() == 0)) {
      errno = EPERM; // Cannot change mode if kernel policy mandated it.
      return -1;
    }
    // Allow if mandatory was set by env var or programmatically, but not kernel
    // policy
  }

  __math_state.mode = mode;

#ifdef DEBUG_MATHEMATICAL
  dprintf(2, "MATHEMATICAL_POSIX: Mode changed to %d\n", __math_state.mode);
#endif
  return 0;
}

// Implementation for __should_use_mathematical (moved from plan step 5 to here
// for cohesion)
int __should_use_mathematical(int flags __attribute__((unused))) {
  if (!__math_state.initialized) {
    __mathematical_init();
  }

  if (__math_state.mode == MATH_MODE_DISABLED) {
    return 0; // False
  }
  if (__math_state.mode == MATH_MODE_ENABLED ||
      __math_state.mode == MATH_MODE_MANDATORY) {
    // For MANDATORY mode, if kernel doesn't support, operations should ideally
    // fail. This check is usually done at the point of syscall in
    // mathematical_posix.c or open() could return error here.
    if (!__math_state.kernel_support &&
        __math_state.mode == MATH_MODE_MANDATORY) {
      // This state implies an issue, as MANDATORY should only be set if
      // kernel_support=1 or by user overriding env var, which is risky. Let
      // individual syscall wrappers handle ENOSYS based on kernel_support for
      // MANDATORY.
      return 1; // True, attempt it, let it fail if kernel doesn't support
    }
    return 1; // True
  }
  return 0; // False by default
}

// Stubs for cache functions (moved from plan step 5 to here for cohesion)
struct cap_cache_entry *__find_cached_capability(const char *path, int flags) {
  (void)path;
  (void)flags;
  if (!__math_state.cache_enabled || !__math_state.cache)
    return NULL;

  pthread_mutex_lock(&__math_state.cache_lock);
  // TODO: Implement cache lookup logic
  // For now, always miss.
  pthread_mutex_unlock(&__math_state.cache_lock);
  __math_state.cache_misses++;
  return NULL;
}

void __cache_capability(const char *path, int flags, capability_t cap,
                        time_t expiry_time) {
  (void)path;
  (void)flags;
  (void)cap;
  (void)expiry_time;
  if (!__math_state.cache_enabled || !__math_state.cache)
    return;

  pthread_mutex_lock(&__math_state.cache_lock);
  // TODO: Implement cache insertion/update logic (e.g. LRU)
  pthread_mutex_unlock(&__math_state.cache_lock);
}

void __invalidate_cached_capability(const char *path, int flags) {
  (void)path;
  (void)flags;
  if (!__math_state.cache_enabled || !__math_state.cache)
    return;
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
// TODO: Add __attribute__((destructor)) for
// __destroy_capability_cache_subsystem if needed.
// minix/lib/libc/sys-minix/mathematical_init.c
#include <errno.h>  // For errno, ENOSYS, EINVAL, EPERM
#include <stdio.h>  // For dprintf (if used for debug logging, needs fd 2)
#include <stdlib.h> // For getenv, calloc, atoi
#include <string.h> // For strcmp, strcasecmp, memset
#include <unistd.h> // For geteuid, getprogname (POSIX)

// Internal libc header
#include "mathematical_internal.h"

// Minix headers for _syscall and message passing
#include <lib.h> // Assumed to provide _syscall and message utilities
#include <minix/callnr.h> // For actual syscall numbers (SYS_CAPABILITY_QUERY to be added)
#include <minix/com.h> // For SYSTEM endpoint (assuming this is PM_PROC_NR or similar for kernel calls)
#include <minix/ipc.h>

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
    // .cache_lock = PTHREAD_MUTEX_INITIALIZER, // Static initialization if
    // possible, else in init_cache
    .verifications_attempted = 0,
    .verifications_succeeded = 0,
    .cache_hits = 0,
    .cache_misses = 0};

static void parse_math_env(const char *value) {
  if (!value || !*value)
    return;

  if (strcmp(value, "0") == 0) {
    __math_state.mode = MATH_MODE_DISABLED;
  } else if (strcmp(value, "1") == 0) {
    __math_state.mode = MATH_MODE_ENABLED;
  } else if (strcasecmp(value, "off") == 0 ||
             strcasecmp(value, "disabled") == 0) {
    __math_state.mode = MATH_MODE_DISABLED;
  } else if (strcasecmp(value, "on") == 0 ||
             strcasecmp(value, "enabled") == 0) {
    __math_state.mode = MATH_MODE_ENABLED;
  } else if (strcasecmp(value, "required") == 0 ||
             strcasecmp(value, "mandatory") == 0) {
    __math_state.mode = MATH_MODE_MANDATORY;
  } else if (strcasecmp(value, "auto") == 0) {
    __math_state.mode =
        (geteuid() == 0) ? MATH_MODE_ENABLED : MATH_MODE_DISABLED;
  }
  // Else: unknown value, mode remains as previously set (e.g. default
  // MATH_MODE_DISABLED)
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
    // For example, if policy is in m.m_lsys_krn_sys_policy (as per feedback
    // example) or m.m1_i1 (a generic field). Let's use M_POLICY_FIELD.
    __math_state.policy = m.M_POLICY_FIELD;

    if (__math_state.policy & POLICY_AVAILABLE) {
      __math_state.kernel_support = 1;
    } else {
      __math_state.kernel_support = 0;
    }

    if ((__math_state.policy & POLICY_MANDATORY_ALL) &&
        __math_state.kernel_support) {
      // If kernel mandates for all and supports it, override current mode.
      __math_state.mode = MATH_MODE_MANDATORY;
    } else if ((__math_state.policy & POLICY_MANDATORY_PRIV) &&
               geteuid() == 0 && __math_state.kernel_support) {
      // If kernel mandates for privileged users and this is root and kernel
      // supports it.
      __math_state.mode = MATH_MODE_MANDATORY;
    }

  } else if (errno == ENOSYS ||
             r == -ENOSYS) { // Check for ENOSYS from _syscall
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
    dprintf(
        2, "MATHEMATICAL_POSIX: query_kernel_policy syscall failed, error %d\n",
        errno);
#endif
  }
}

static void init_cache(void) {
  const char *cache_env = getenv("MINIX_CAPABILITY_CACHE");
  const char *cache_size_env = getenv("MINIX_CAPABILITY_CACHE_SIZE");
  int desired_cache_size = 16; // Default size

  // Initialize mutex first, regardless of cache allocation
  // PTHREAD_MUTEX_INITIALIZER can be used for static __math_state, but if
  // __math_state is not file-scope static, then pthread_mutex_init is better.
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

  if (!cache_env || strcmp(cache_env, "0") == 0 ||
      strcmp(cache_env, "off") == 0) {
    __math_state.cache_enabled = 0; // Cache explicitly disabled
    return;
  }

  if (cache_size_env) {
    int size = atoi(cache_size_env);
    if (size > 0 && size <= 1024) { // Reasonable bounds
      desired_cache_size = size;
    }
  }

  __math_state.cache =
      calloc(desired_cache_size, sizeof(struct cap_cache_entry));
  if (__math_state.cache) {
    __math_state.cache_size = desired_cache_size;
    __math_state.cache_enabled = 1;
  } else {
    // calloc failed
    __math_state.cache_enabled = 0;
    __math_state.cache_size = 0;
#ifdef DEBUG_MATHEMATICAL
    dprintf(2, "MATHEMATICAL_POSIX: Cache allocation failed for size %d.\n",
            desired_cache_size);
#endif
  }
}

__attribute__((constructor(101))) void __mathematical_init(void) {
  const char *env_value;
  const char *prog_name;

  if (__math_state.initialized)
    return; // Prevent double initialization

  // Initialize default state, especially if __math_state wasn't statically
  // initialized fully. (It was, but being explicit here can be good if static
  // init of mutex is an issue) pthread_mutex_init(&__math_state.cache_lock,
  // NULL); // Moved to init_cache for safety

  // Step 1: Check environment variable for initial mode setting
  env_value = getenv("MINIX_MATHEMATICAL_POSIX");
  if (env_value) {
    parse_math_env(env_value);
  }

  // Step 2: Query kernel for system policy and capability support
  query_kernel_policy();

  // Step 3: Initialize cache if requested via environment (and kernel supports
  // feature if cache needs it) Cache can be initialized even if kernel doesn't
  // support the main feature, as it might be a user-space only cache for some
  // aspects.
  init_cache();

  // Step 4: Special handling for security-critical programs
  // This overrides env var and potentially kernel policy if kernel doesn't
  // mandate it already.
  prog_name = getprogname(); // Standard POSIX way to get program name
  if (prog_name) {
    if (strcmp(prog_name, "login") == 0 || strcmp(prog_name, "su") == 0 ||
        strcmp(prog_name, "passwd") == 0) {
      // Only upgrade to mandatory if kernel supports the feature.
      if (__math_state.kernel_support) {
        __math_state.mode = MATH_MODE_MANDATORY;
      } else {
        // If kernel doesn't support it, these critical programs might need to
        // fail or log a serious warning. For now, they'll run without math mode
        // if not supported. Or, if MATH_MODE_MANDATORY implies failure on
        // no-support, that check happens later.
      }
    }
  }

  __math_state.initialized = 1;

#ifdef DEBUG_MATHEMATICAL
  // dprintf usually writes to stderr (fd 2). Ensure it's available or use a
  // kernel log if in early init. For libc init, dprintf(2, ...) should be fine.
  dprintf(2,
          "MATHEMATICAL_POSIX: Libc init complete. Mode: %d, Kernel Support: "
          "%d, Cache Enabled: %d, Cache Size: %zu\n",
          __math_state.mode, __math_state.kernel_support,
          __math_state.cache_enabled, __math_state.cache_size);
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

  // Can't downgrade from mandatory if it was set by kernel policy or critical
  // program logic This check might need to be more nuanced based on the source
  // of the MANDATORY setting. For now, if current mode is MANDATORY, assume
  // it's fixed.
  if (__math_state.mode == MATH_MODE_MANDATORY && mode != MATH_MODE_MANDATORY) {
    // Check if kernel policy itself mandated it
    if (__math_state.policy & POLICY_MANDATORY_ALL ||
        (__math_state.policy & POLICY_MANDATORY_PRIV && geteuid() == 0)) {
      errno = EPERM; // Cannot change mode if kernel policy mandated it.
      return -1;
    }
    // Allow if mandatory was set by env var or programmatically, but not kernel
    // policy
  }

  __math_state.mode = mode;

#ifdef DEBUG_MATHEMATICAL
  dprintf(2, "MATHEMATICAL_POSIX: Mode changed to %d\n", __math_state.mode);
#endif
  return 0;
}

// Implementation for __should_use_mathematical (moved from plan step 5 to here
// for cohesion)
int __should_use_mathematical(int flags __attribute__((unused))) {
  if (!__math_state.initialized) {
    __mathematical_init();
  }

  if (__math_state.mode == MATH_MODE_DISABLED) {
    return 0; // False
  }
  if (__math_state.mode == MATH_MODE_ENABLED ||
      __math_state.mode == MATH_MODE_MANDATORY) {
    // For MANDATORY mode, if kernel doesn't support, operations should ideally
    // fail. This check is usually done at the point of syscall in
    // mathematical_posix.c or open() could return error here.
    if (!__math_state.kernel_support &&
        __math_state.mode == MATH_MODE_MANDATORY) {
      // This state implies an issue, as MANDATORY should only be set if
      // kernel_support=1 or by user overriding env var, which is risky. Let
      // individual syscall wrappers handle ENOSYS based on kernel_support for
      // MANDATORY.
      return 1; // True, attempt it, let it fail if kernel doesn't support
    }
    return 1; // True
  }
  return 0; // False by default
}

// Stubs for cache functions (moved from plan step 5 to here for cohesion)
struct cap_cache_entry *__find_cached_capability(const char *path, int flags) {
  (void)path;
  (void)flags;
  if (!__math_state.cache_enabled || !__math_state.cache)
    return NULL;

  pthread_mutex_lock(&__math_state.cache_lock);
  // TODO: Implement cache lookup logic
  // For now, always miss.
  pthread_mutex_unlock(&__math_state.cache_lock);
  __math_state.cache_misses++;
  return NULL;
}

void __cache_capability(const char *path, int flags, capability_t cap,
                        time_t expiry_time) {
  (void)path;
  (void)flags;
  (void)cap;
  (void)expiry_time;
  if (!__math_state.cache_enabled || !__math_state.cache)
    return;

  pthread_mutex_lock(&__math_state.cache_lock);
  // TODO: Implement cache insertion/update logic (e.g. LRU)
  pthread_mutex_unlock(&__math_state.cache_lock);
}

void __invalidate_cached_capability(const char *path, int flags) {
  (void)path;
  (void)flags;
  if (!__math_state.cache_enabled || !__math_state.cache)
    return;
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
// TODO: Add __attribute__((destructor)) for
// __destroy_capability_cache_subsystem if needed.
// minix/lib/libc/sys-minix/mathematical_init.c

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mathematical_internal.h"

#include <lib.h>
#include <minix/callnr.h>
#include <minix/com.h>
#include <minix/ipc.h>

/* Global state for mathematical verification */
struct math_state __math_state = {.initialized = 0,
                                  .mode = MATH_MODE_DISABLED,
                                  .kernel_support = 0,
                                  .policy = 0,
                                  .cache_enabled = 0,
                                  .cache = NULL,
                                  .cache_size = 0,
                                  .verifications_attempted = 0,
                                  .verifications_succeeded = 0,
                                  .cache_hits = 0,
                                  .cache_misses = 0};

/* Parse MINIX_MATHEMATICAL_POSIX environment variable */
static void parse_math_env(const char *value) {
  if (!value || !*value)
    return;
  if (strcmp(value, "0") == 0 || strcasecmp(value, "off") == 0 ||
      strcasecmp(value, "disabled") == 0) {
    __math_state.mode = MATH_MODE_DISABLED;
  } else if (strcmp(value, "1") == 0 || strcasecmp(value, "on") == 0 ||
             strcasecmp(value, "enabled") == 0) {
    __math_state.mode = MATH_MODE_ENABLED;
  } else if (strcasecmp(value, "required") == 0 ||
             strcasecmp(value, "mandatory") == 0) {
    __math_state.mode = MATH_MODE_MANDATORY;
  } else if (strcasecmp(value, "audit") == 0) {
    __math_state.mode = MATH_MODE_AUDIT;
  } else if (strcasecmp(value, "auto") == 0) {
    __math_state.mode =
        (geteuid() == 0) ? MATH_MODE_ENABLED : MATH_MODE_DISABLED;
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
    if ((__math_state.policy & POLICY_MANDATORY_ALL) &&
        __math_state.kernel_support) {
      __math_state.mode = MATH_MODE_MANDATORY;
    } else if ((__math_state.policy & POLICY_MANDATORY_PRIV) &&
               geteuid() == 0 && __math_state.kernel_support) {
      __math_state.mode = MATH_MODE_MANDATORY;
    }
  } else if (errno == ENOSYS) {
    __math_state.kernel_support = 0;
    __math_state.policy = 0;
  } else {
    __math_state.kernel_support = 0;
    __math_state.policy = 0;
  }
}

/* Initialize or disable the capability cache */
static void init_cache(void) {
  const char *env = getenv("MINIX_CAPABILITY_CACHE_SIZE");
  int size = 16;

  pthread_mutex_init(&__math_state.cache_lock, NULL);
  if (env) {
    int v = atoi(env);
    if (v > 0)
      size = v;
  }

  __math_state.cache = calloc(size, sizeof(struct cap_cache_entry));
  if (__math_state.cache) {
    __math_state.cache_size = size;
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
    __math_state.cache = NULL;
    __math_state.cache_size = 0;
    __math_state.cache_enabled = 0;
    pthread_mutex_unlock(&__math_state.cache_lock);
    pthread_mutex_destroy(&__math_state.cache_lock);
  }
}

/* Auto‐constructor: runs before main() */
__attribute__((constructor)) static void __mathematical_init(void) {
  if (__math_state.initialized)
    return;

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
__attribute__((destructor)) static void __mathematical_fini(void) {
  if (!__math_state.initialized)
    return;
  destroy_cache();
  __math_state.initialized = 0;
}

/* Allow applications to change the math mode at runtime */
int mathematical_set_mode(int mode) {
  if (!__math_state.initialized)
    __mathematical_init();

  if (mode < MATH_MODE_DISABLED || mode > MATH_MODE_AUDIT) {
    errno = EINVAL;
    return -1;
  }
  if (mode != MATH_MODE_DISABLED && !__math_state.kernel_support) {
    errno = ENOSYS;
    return -1;
  }
  if (__math_state.mode == MATH_MODE_MANDATORY && mode != MATH_MODE_MANDATORY) {
    if ((__math_state.policy & POLICY_MANDATORY_ALL) ||
        ((__math_state.policy & POLICY_MANDATORY_PRIV) && geteuid() == 0)) {
      errno = EPERM;
      return -1;
    }
  }

  __math_state.mode = mode;
  return 0;
}

/* Internal helper: should we apply mathematical verification? */
int __should_use_mathematical(int flags __attribute__((unused))) {
  if (!__math_state.initialized)
    __mathematical_init();
  return __math_state.mode != MATH_MODE_DISABLED;
}
// minix/lib/libc/sys-minix/mathematical_init.c

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mathematical_internal.h"

#include <lib.h>
#include <minix/callnr.h>
#include <minix/com.h>
#include <minix/ipc.h>

/* Global state for mathematical verification */
struct math_state __math_state = {.initialized = 0,
                                  .mode = MATH_MODE_DISABLED,
                                  .kernel_support = 0,
                                  .policy = 0,
                                  .cache_enabled = 0,
                                  .cache = NULL,
                                  .cache_size = 0,
                                  .verifications_attempted = 0,
                                  .verifications_succeeded = 0,
                                  .cache_hits = 0,
                                  .cache_misses = 0};

/* Parse MINIX_MATHEMATICAL_POSIX environment variable */
static void parse_math_env(const char *value) {
  if (!value || !*value)
    return;
  if (strcmp(value, "0") == 0 || strcasecmp(value, "off") == 0 ||
      strcasecmp(value, "disabled") == 0) {
    __math_state.mode = MATH_MODE_DISABLED;
  } else if (strcmp(value, "1") == 0 || strcasecmp(value, "on") == 0 ||
             strcasecmp(value, "enabled") == 0) {
    __math_state.mode = MATH_MODE_ENABLED;
  } else if (strcasecmp(value, "required") == 0 ||
             strcasecmp(value, "mandatory") == 0) {
    __math_state.mode = MATH_MODE_MANDATORY;
  } else if (strcasecmp(value, "audit") == 0) {
    __math_state.mode = MATH_MODE_AUDIT;
  } else if (strcasecmp(value, "auto") == 0) {
    __math_state.mode =
        (geteuid() == 0) ? MATH_MODE_ENABLED : MATH_MODE_DISABLED;
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
    if ((__math_state.policy & POLICY_MANDATORY_ALL) &&
        __math_state.kernel_support) {
      __math_state.mode = MATH_MODE_MANDATORY;
    } else if ((__math_state.policy & POLICY_MANDATORY_PRIV) &&
               geteuid() == 0 && __math_state.kernel_support) {
      __math_state.mode = MATH_MODE_MANDATORY;
    }
  } else if (errno == ENOSYS) {
    __math_state.kernel_support = 0;
    __math_state.policy = 0;
  } else {
    __math_state.kernel_support = 0;
    __math_state.policy = 0;
  }
}

/* Initialize or disable the capability cache */
static void init_cache(void) {
  const char *env = getenv("MINIX_CAPABILITY_CACHE_SIZE");
  int size = 16;

  pthread_mutex_init(&__math_state.cache_lock, NULL);
  if (env) {
    int v = atoi(env);
    if (v > 0)
      size = v;
  }

  __math_state.cache = calloc(size, sizeof(struct cap_cache_entry));
  if (__math_state.cache) {
    __math_state.cache_size = size;
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
    __math_state.cache = NULL;
    __math_state.cache_size = 0;
    __math_state.cache_enabled = 0;
    pthread_mutex_unlock(&__math_state.cache_lock);
    pthread_mutex_destroy(&__math_state.cache_lock);
  }
}

/* Auto‐constructor: runs before main() */
__attribute__((constructor)) static void __mathematical_init(void) {
  if (__math_state.initialized)
    return;

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
__attribute__((destructor)) static void __mathematical_fini(void) {
  if (!__math_state.initialized)
    return;
  destroy_cache();
  __math_state.initialized = 0;
}

/* Allow applications to change the math mode at runtime */
int mathematical_set_mode(int mode) {
  if (!__math_state.initialized)
    __mathematical_init();

  if (mode < MATH_MODE_DISABLED || mode > MATH_MODE_AUDIT) {
    errno = EINVAL;
    return -1;
  }
  if (mode != MATH_MODE_DISABLED && !__math_state.kernel_support) {
    errno = ENOSYS;
    return -1;
  }
  if (__math_state.mode == MATH_MODE_MANDATORY && mode != MATH_MODE_MANDATORY) {
    if ((__math_state.policy & POLICY_MANDATORY_ALL) ||
        ((__math_state.policy & POLICY_MANDATORY_PRIV) && geteuid() == 0)) {
      errno = EPERM;
      return -1;
    }
  }

  __math_state.mode = mode;
  return 0;
}

/* Internal helper: should we apply mathematical verification? */
int __should_use_mathematical(int flags __attribute__((unused))) {
  if (!__math_state.initialized)
    __mathematical_init();
  return __math_state.mode != MATH_MODE_DISABLED;
}
