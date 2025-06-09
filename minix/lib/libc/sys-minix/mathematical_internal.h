// minix/lib/libc/sys-minix/mathematical_internal.h
#ifndef _MATHEMATICAL_INTERNAL_H
#define _MATHEMATICAL_INTERNAL_H

/*
 * Internal implementation header for mathematical POSIX capabilities
 *
 * This header contains structures and functions that are used internally
 * by libc to implement mathematical verification. It should never be
 * included by user programs - they should use <minix/capability.h> instead.
 *
 * The separation between public and internal APIs allows us to change
 * implementation details without breaking user programs.
 */

#include <limits.h>    // For PATH_MAX
#include <pthread.h>   // For pthread_mutex_t, pthread_key_t
#include <stdio.h>     // For FILE (for audit_log)
#include <sys/types.h> // For uid_t, gid_t, pid_t, time_t, size_t
#include <time.h>      // For time_t (though often in sys/types.h too)

/* Include the public API to ensure consistency and get capability_t */
#include <minix/callnr.h>     // For syscall numbers if used in prototypes here
#include <minix/capability.h> // Provides capability_t, CAP_ rights etc.
#include <minix/const.h>      // For endpoint_t, etc.
#include <minix/ipc.h> // For message structure (used in kernel_capability_request if it sends one)

/* Internal capability representation
 * This extends the public capability_t with implementation details
 * that users don't need to know about.
 * Note: The public capability_t was:
 * typedef struct { unsigned long id; void *internal_details; } capability_t;
 * We'll assume internal_details can point to this struct or parts of it.
 * Or, this struct IS the opaque internal_details.
 * For now, let's assume public_cap is the user-facing part.
 */
struct internal_capability {
  capability_t
      public_cap; /* The user-visible part (could be an ID or small data) */

  /* Internal bookkeeping */
  uint64_t kernel_handle; /* Kernel's reference to this capability (if any) */
  time_t cached_at;       /* When this was cached */
  int cache_hits;         /* Performance monitoring for this specific entry */
  pid_t owning_process; /* Which process created/owns this libc-cached entry */

  /* Validation data from a kernel proof, if applicable */
  unsigned char proof_hash[32]; /* SHA-256 of the proof, if proofs are passed
                                   to/cached in userspace */
  int validation_flags; /* Internal validation state for this cached item */
  time_t proof_generation_timestamp; /* From kernel proof */
};

/* Mathematical verification modes - internal use only */
enum math_mode {
  MATH_MODE_DISABLED = 0,  /* No verification */
  MATH_MODE_ENABLED = 1,   /* Verify when beneficial (user/env opt-in) */
  MATH_MODE_MANDATORY = 2, /* Always verify; fail operations if verification not
                              possible or fails */
  MATH_MODE_AUDIT = 3,     /* Verify and log everything (could be combined with
                              ENABLED/MANDATORY) */
};

/* Kernel policy flags - these map to kernel-side definitions from
 * SYS_CAPABILITY_QUERY */
#define POLICY_AVAILABLE 0x0001 // Kernel supports the capability system
#define POLICY_MANDATORY_ALL                                                   \
  0x0002 // Kernel mandates verification for all processes
#define POLICY_MANDATORY_PRIV                                                  \
  0x0004 // Kernel mandates for privileged processes (e.g. root)
#define POLICY_CACHE_KERNEL                                                    \
  0x0008 // Kernel suggests/supports kernel-side caching (info only for libc)
#define POLICY_AUDIT_ALL 0x0010 // Kernel requests all operations be audited

/* Cache entry structure with full implementation details */
struct cap_cache_entry {
  /* Key for cache lookup */
  char path[PATH_MAX];
  int open_flags; // Flags used during open() for this entry
  uid_t uid;      // User ID at time of caching
  gid_t gid;      // Group ID at time of caching

  /* Cached capability data */
  struct internal_capability capability; // The actual cached capability info

  /* Cache management */
  time_t expiry;                     // When this entry expires (absolute time)
  struct cap_cache_entry *hash_next; // For hash collision chain
  struct cap_cache_entry *lru_prev, *lru_next; // For LRU list

  /* Statistics for this entry */
  unsigned int hit_count;
  time_t last_used;
  int valid; // Is this entry considered valid (e.g. not explicitly invalidated)
};

/* Global state structure - extended from what users might know about */
struct math_state {
  /* Basic state */
  int initialized;
  enum math_mode mode;
  int kernel_support;  // Derived from (policy & POLICY_AVAILABLE)
  unsigned int policy; // Raw policy flags from kernel

  /* Cache management */
  int cache_enabled;
  struct cap_cache_entry *
      *cache_hash_table;  // Hash table (array of pointers to list heads)
  size_t cache_hash_size; // Number of buckets in hash table
  struct cap_cache_entry *cache_lru_head; // LRU list head
  struct cap_cache_entry *cache_lru_tail; // LRU list tail
  size_t cache_current_entries;           // Current number of items in cache
  size_t cache_max_entries;               // Maximum allowed items in cache
  pthread_mutex_t cache_lock;

  /* Performance monitoring / Statistics */
  struct {
    unsigned long verifications_attempted;
    unsigned long verifications_succeeded;
    unsigned long verifications_failed;
    unsigned long cache_hits;
    unsigned long cache_misses;
    unsigned long cache_evictions;
    unsigned long capability_derives;   // Count of derive operations
    unsigned long capability_restricts; // Count of restrict operations
    unsigned long capability_revokes;   // Count of revoke operations
  } stats;

  /* Thread-local storage key for per-thread state */
  pthread_key_t thread_state_key;
  int thread_state_key_initialized;

  /* Communication with kernel (example, might not be fd based) */
  int kernel_notify_fd; /* File descriptor for kernel events (if using
                           select/poll) */

  /* Debugging and diagnostics */
  int debug_level;
  FILE *audit_log_file; // Changed from audit_log to avoid conflict with
                        // potential function
};

/* Per-thread state for mathematical operations */
struct thread_math_state {
  enum math_mode thread_mode; // Override global mode for this thread
  int mode_is_set;            // Has thread_mode been explicitly set?
  int mode_locked; /* Prevent downgrades if thread sets a higher bar */

  /* Thread-local capability cache for very frequent operations (e.g. last N
   * capabilities) */
  struct internal_capability recent_caps[8]; // Example size
  int recent_caps_count;
  int recent_caps_lru_idx; // Index for simple LRU replacement

  /* Error handling specific to this thread's last math operation */
  int last_math_errno; // Could store MATH_ERR_ codes or translated errno
  char last_error_path[PATH_MAX]; // Path related to last error
};

/* Global state - defined in mathematical_init.c */
extern struct math_state __math_state;

/* --- Internal function prototypes --- */

/* Initialization and configuration (from mathematical_init.c) */
void __mathematical_init(void) __attribute__((constructor(101)));
void __mathematical_fini(void) __attribute__((destructor(101)));
int __mathematical_reinit(void); /* For fork() handling (calls atfork_child) */
void __mathematical_atfork_child(void); /* Logic for child post-fork */

/* State management (from mathematical_init.c or math_control.c) */
enum math_mode
__get_effective_mode(void); // Gets current effective mode (global or thread)
int __should_use_mathematical(
    int flags); // Decides if math verification should be attempted for an op
struct thread_math_state *
__get_thread_state(int create_if_missing); // Manages TLS

/* Capability cache management (from mathematical_cache.c or similar) */
void __init_capability_cache_subsystem(
    void); // Initializes __math_state.cache_hash_table etc.
void __destroy_capability_cache_subsystem(void); // Frees cache resources
struct cap_cache_entry *__find_cached_capability(const char *path, int flags,
                                                 uid_t uid, gid_t gid);
int __cache_capability(const char *path, int flags, uid_t uid, gid_t gid,
                       const struct internal_capability *cap,
                       time_t expiry_abs);
void __evict_expired_capabilities(void);
void __flush_capability_cache(void);
void __invalidate_path_capabilities(
    const char *path); // Example for more specific invalidation

/* Kernel communication (from mathematical_kernel_comm.c or similar) */
int __query_kernel_policy(
    unsigned int *policy_flags_from_kernel); // Fills policy_flags_from_kernel
int __notify_kernel_verification(
    const char *path,
    int result_code); // Informs kernel about a verification result
int __register_kernel_notifications(void); // Sets up kernel_notify_fd if used

/* Proof construction and verification - internal versions (may call kernel via
 * syscalls) */
/* These are internal libc wrappers that might call SYS_CAP_CONSTRUCT etc. */
/* The capability_proof struct is from the kernel's kcapability_posix.h, assumed
 * opaque here if passed. */
struct capability_proof;
struct capability_proof *__construct_capability_proof_libc(endpoint_t proc_ep,
                                                           const char *path,
                                                           int rights);
int __verify_capability_proof_libc(const struct capability_proof *proof);
void __free_capability_proof_libc(struct capability_proof *proof);

/* Debugging and diagnostics */
void __log_capability_operation(const char *op, const char *path, int result,
                                const char *detail);
void __dump_capability_stats(FILE *stream);
void __enable_capability_debugging(int level); // Sets __math_state.debug_level
void __set_audit_log_file(FILE *log_file); // Sets __math_state.audit_log_file

/* Security-sensitive operations (internal checks) */
int __validate_capability_transition(const capability_t *from_cap,
                                     const capability_t *to_cap,
                                     int operation_type);
int __check_capability_tampering(
    const struct internal_capability *cap_internal);

/* Performance optimizations */
static inline int __quick_capability_check(int flags) {
  if (!(flags &
        (O_MATHEMATICAL | O_MATHEMATICAL_REQUIRED))) // O_flags from fcntl.h
    return 0; // Not a mathematical open request
  if (!__math_state.initialized)
    return 0; // Libc state not ready
  if (__math_state.mode == MATH_MODE_DISABLED &&
      !(__math_state.policy & (POLICY_MANDATORY_ALL | POLICY_MANDATORY_PRIV)))
    return 0; // Disabled and not forced by kernel policy
  return 1;   // Potentially needs math check
}

/* Internal Error codes for libc internal use, translated to errno for public
 * API */
#define MATH_ERR_BASE -10000 // Base for our internal errors
#define MATH_ERR_NOT_INITIALIZED (MATH_ERR_BASE - 0)
#define MATH_ERR_CACHE_FULL (MATH_ERR_BASE - 1)
#define MATH_ERR_KERNEL_COMM                                                   \
  (MATH_ERR_BASE - 2) // General kernel communication error
#define MATH_ERR_PROOF_EXPIRED                                                 \
  (MATH_ERR_BASE - 3) // If userspace caches and validates proofs
#define MATH_ERR_TAMPERING                                                     \
  (MATH_ERR_BASE - 4) // If a cached proof/cap looks tampered
#define MATH_ERR_NO_KERNEL_SUPPORT                                             \
  (MATH_ERR_BASE - 5) // Kernel doesn't support feature but was required
#define MATH_ERR_INVALID_ARG                                                   \
  (MATH_ERR_BASE - 6) // Bad arguments to internal functions
#define MATH_ERR_TLS_FAILURE (MATH_ERR_BASE - 7) // Thread-Local Storage failure
#define MATH_ERR_MUTEX_FAILURE (MATH_ERR_BASE - 8) // Mutex operation failure

/* Kernel communication request/event structures (as conceptualized in feedback)
 */
/* These define the payload for messages if specific structures are used for
 * kernel syscalls */
/* For SYS_CAPABILITY_QUERY, the kernel just returns flags in m1_i1 (current
 * plan) */
/* For proof construction, it might be more complex */

struct kernel_capability_request {
  int operation_type; // e.g., CONSTRUCT_PROOF, VALIDATE_PROOF_HANDLE
  char path[PATH_MAX];
  int rights_requested;
  uid_t effective_uid;
  gid_t effective_gid;
  // Space for kernel to return a handle or proof details
  uint64_t out_kernel_handle;
  unsigned char out_proof_hash[32];
};

struct kernel_capability_event {
  int event_type;              // e.g., CAPABILITY_REVOKED_BY_KERNEL
  pid_t affected_pid;          // Process whose capability is affected
  capability_t revoked_cap_id; // ID of the capability that was revoked
  char reason_string[128];
};

#endif /* _MATHEMATICAL_INTERNAL_H */
// minix/lib/libc/sys-minix/mathematical_internal.h
#ifndef _MATHEMATICAL_INTERNAL_H
#define _MATHEMATICAL_INTERNAL_H

#include <limits.h>    // For PATH_MAX (usually defined here or in unistd.h)
#include <pthread.h>   // For pthread_mutex_t
#include <sys/types.h> // For time_t, size_t

// Placeholder for capability_t until <minix/capability.h> is defined.
// This allows struct cap_cache_entry to be defined.
typedef void *capability_t;

/* Mathematical verification modes */
#define MATH_MODE_DISABLED 0 /* No verification */
#define MATH_MODE_ENABLED                                                      \
  1 /* Verification when beneficial (e.g., user opt-in) */
#define MATH_MODE_MANDATORY                                                    \
  2 /* Always verify, fail operations if not possible or proof fails */
// #define MATH_MODE_AUTO (This was in parse_math_env, might be an internal
// resolution, not a stored mode)

/* Kernel policy flags (communicated by kernel via SYS_CAPABILITY_QUERY) */
#define POLICY_AVAILABLE 0x01 // Kernel supports the capability system
#define POLICY_MANDATORY_ALL                                                   \
  0x02 // Kernel mandates verification for all processes
#define POLICY_MANDATORY_PRIV 0x04 // Kernel mandates for privileged processes
#define POLICY_CACHE_KERNEL                                                    \
  0x08 // Kernel suggests/supports kernel-side caching (info only)

/* Cache entry structure */
struct cap_cache_entry {
  char path[PATH_MAX]; // PATH_MAX needs to be defined, e.g. via <limits.h> or
                       // <unistd.h>
  int flags;           // Open flags associated with this capability/path
  capability_t capability; // The actual capability data/proof (opaque for now)
  time_t expiry;           // Expiry time for the cached entry
  int valid;               // Is this cache entry currently valid?
  // Potentially add usage counters for LRU eviction, etc.
};

/* Global state structure for mathematical POSIX features within libc */
struct math_state {
  int initialized; // Has __mathematical_init() run?
  int mode;        // Current operational mode (MATH_MODE_DISABLED, etc.)

  int kernel_support; // Does the running kernel support these capabilities?
                      // (from POLICY_AVAILABLE)
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
void __mathematical_init(void);      // Ensures the system is initialized
int mathematical_set_mode(int mode); // Allows changing the mode at runtime
                                     // (declared in mathposix.h too)

/* From mathematical_posix.c (or wherever they are implemented) */
int __should_use_mathematical(
    int flags); // Decides if math verification should be used for an operation

/* Cache functions (to be implemented, likely in a new cache management C file
 * or mathematical_init.c) */
struct cap_cache_entry *__find_cached_capability(const char *path, int flags);
void __cache_capability(const char *path, int flags, capability_t cap,
                        time_t expiry_time);
void __invalidate_cached_capability(const char *path,
                                    int flags);  // Example of another cache op
void __init_capability_cache_subsystem(void);    // If cache init is complex
void __destroy_capability_cache_subsystem(void); // If cleanup needed

#endif /* _MATHEMATICAL_INTERNAL_H */
/**
 * @file mathematical_internal.h
 * @brief Internal implementation header for MINIX mathematical POSIX
 * capabilities.
 *
 * This header defines internal data structures, modes, flags, and function
 * prototypes used by the libc implementation of mathematical verification.
 * It must not be included by user programs; they should use
 * <minix/capability.h> for the public API.
 */

#ifndef MATHEMATICAL_INTERNAL_H
#define MATHEMATICAL_INTERNAL_H

/* Standard headers */
#include <limits.h>    /* PATH_MAX */
#include <pthread.h>   /* pthread_mutex_t, pthread_key_t */
#include <stdio.h>     /* FILE */
#include <sys/types.h> /* uid_t, gid_t, pid_t, size_t */
#include <time.h>      /* time_t */

/* MINIX public APIs */
#include <minix/callnr.h>     /* SYS_CAPABILITY_QUERY */
#include <minix/capability.h> /* capability_t, CAP_* rights */
#include <minix/const.h>      /* NONE, etc. */
#include <minix/ipc.h>        /* message, endpoint_t */

/** @brief Mathematical verification modes. */
enum math_mode {
  MATH_MODE_DISABLED = 0,  /**< No verification */
  MATH_MODE_ENABLED = 1,   /**< Verify when enabled by user or env */
  MATH_MODE_MANDATORY = 2, /**< Always verify; fail if not possible */
  MATH_MODE_AUDIT = 3      /**< Verify and log all operations */
};

/** @brief Kernel policy flags returned by SYS_CAPABILITY_QUERY. */
#define POLICY_AVAILABLE 0x0001 /**< Kernel supports capability system */
#define POLICY_MANDATORY_ALL                                                   \
  0x0002 /**< Kernel mandates verification for all                             \
          */
#define POLICY_MANDATORY_PRIV                                                  \
  0x0004                           /**< Kernel mandates verification for root */
#define POLICY_CACHE_KERNEL 0x0008 /**< Kernel supports its own caching */
#define POLICY_AUDIT_ALL 0x0010    /**< Kernel requests audit logging */

/**
 * @struct internal_capability
 * @brief Extended, opaque representation of a capability in userspace.
 *
 * This structure holds both the public-facing capability_t and additional
 * bookkeeping, cache metadata, and proof data.
 */
struct internal_capability {
  capability_t public_cap;           /**< User-visible identifier */
  uint64_t kernel_handle;            /**< Kernel reference handle */
  time_t cached_at;                  /**< Timestamp when cached */
  int cache_hits;                    /**< Hit count for this entry */
  pid_t owning_process;              /**< PID that owns this cache entry */
  unsigned char proof_hash[32];      /**< SHA-256 of proof (if any) */
  int validation_flags;              /**< Internal validation state */
  time_t proof_generation_timestamp; /**< When proof was generated */
};

/**
 * @struct cap_cache_entry
 * @brief Single entry in the capability cache hash table and LRU list.
 */
struct cap_cache_entry {
  char path[PATH_MAX];                   /**< Lookup key: filesystem path */
  int open_flags;                        /**< Flags passed to open() */
  uid_t uid;                             /**< Effective UID at cache time */
  gid_t gid;                             /**< Effective GID at cache time */
  struct internal_capability capability; /**< Cached capability data */
  time_t expiry;                         /**< Expiration time (absolute) */
  struct cap_cache_entry *hash_next;     /**< Next in hash bucket chain */
  struct cap_cache_entry *lru_prev, *lru_next; /**< LRU doubly‐linked list */
  unsigned long hit_count;                     /**< Times this entry was hit */
  time_t last_used;                            /**< Last access timestamp */
  int valid;                                   /**< Nonzero if entry is valid */
};

/**
 * @struct thread_math_state
 * @brief Per-thread overrides and caches for mathematical mode.
 */
struct thread_math_state {
  enum math_mode thread_mode; /**< Thread-specific mode */
  int mode_is_set;            /**< Has thread_mode been set? */
  int mode_locked;            /**< Prevent downgrading once set */
  struct internal_capability recent_caps[8]; /**< Tiny LRU of recent caps */
  int recent_caps_count;                     /**< Number of valid entries */
  int recent_caps_lru_idx;                   /**< Next victim index */
  int last_math_errno;                       /**< Last internal error code */
  char last_error_path[PATH_MAX];            /**< Path of last error */
};

/**
 * @struct math_state
 * @brief Global state for mathematical POSIX support within libc.
 */
struct math_state {
  int initialized;     /**< Has constructor run? */
  enum math_mode mode; /**< Current global mode */
  int kernel_support;  /**< POLICY_AVAILABLE? */
  unsigned int policy; /**< Raw policy flags */

  /* capability cache */
  int cache_enabled;                         /**< Nonzero if cache active */
  struct cap_cache_entry **cache_hash_table; /**< Hash table array */
  size_t cache_hash_size;                    /**< Number of buckets */
  struct cap_cache_entry *cache_lru_head;    /**< LRU list head */
  struct cap_cache_entry *cache_lru_tail;    /**< LRU list tail */
  size_t cache_current_entries;              /**< Current size */
  size_t cache_max_entries;                  /**< Maximum entries allowed */
  pthread_mutex_t cache_lock;                /**< Protects cache structures */

  /* statistics */
  struct {
    unsigned long verifications_attempted;
    unsigned long verifications_succeeded;
    unsigned long verifications_failed;
    unsigned long cache_hits;
    unsigned long cache_misses;
    unsigned long cache_evictions;
    unsigned long capability_derives;
    unsigned long capability_restricts;
    unsigned long capability_revokes;
  } stats;

  /* per-thread state key */
  pthread_key_t thread_state_key;   /**< TLS key */
  int thread_state_key_initialized; /**< Key created? */

  /* notifications */
  int kernel_notify_fd; /**< FD for kernel events */

  /* debugging / auditing */
  int debug_level;      /**< Verbosity */
  FILE *audit_log_file; /**< Audit log stream */
};

/** @brief Global instance, defined in mathematical_init.c */
extern struct math_state __math_state;

/*=== Internal function prototypes ===*/

/* Initialization and teardown */
void __mathematical_init(void) __attribute__((constructor(101)));
void __mathematical_fini(void) __attribute__((destructor(101)));
int __mathematical_reinit(void);
void __mathematical_atfork_child(void);

/* Mode and TLS */
enum math_mode __get_effective_mode(void);
int __should_use_mathematical(int flags);
struct thread_math_state *__get_thread_state(int create_if_missing);

/* Cache management */
void __init_capability_cache_subsystem(void);
void __destroy_capability_cache_subsystem(void);
struct cap_cache_entry *__find_cached_capability(const char *path, int flags,
                                                 uid_t uid, gid_t gid);
int __cache_capability(const char *path, int flags, uid_t uid, gid_t gid,
                       const struct internal_capability *cap, time_t expiry);
void __evict_expired_capabilities(void);
void __flush_capability_cache(void);
void __invalidate_path_capabilities(const char *path);

/* Kernel communication */
int __query_kernel_policy(unsigned int *policy_out);
int __notify_kernel_verification(const char *path, int result_code);
int __register_kernel_notifications(void);

/* Proof construction & verification */
struct capability_proof *__construct_capability_proof_libc(endpoint_t proc_ep,
                                                           const char *path,
                                                           int rights);
int __verify_capability_proof_libc(const struct capability_proof *proof);
void __free_capability_proof_libc(struct capability_proof *proof);

/* Debugging and diagnostics */
void __log_capability_operation(const char *op, const char *path, int result,
                                const char *detail);
void __dump_capability_stats(FILE *stream);
void __enable_capability_debugging(int level);
void __set_audit_log_file(FILE *log_file);

/* Security checks */
int __validate_capability_transition(const capability_t *from,
                                     const capability_t *to, int op);
int __check_capability_tampering(const struct internal_capability *cap);

/**
 * @brief Quick check to see if an open() call might need math support.
 * @param flags  open() flags
 * @return nonzero if math logic should run
 */
static inline int __quick_capability_check(int flags) {
  if (!(flags & O_MATHEMATICAL))
    return 0;
  if (!__math_state.initialized)
    return 0;
  if (__math_state.mode == MATH_MODE_DISABLED &&
      !(__math_state.policy & (POLICY_MANDATORY_ALL | POLICY_MANDATORY_PRIV)))
    return 0;
  return 1;
}

/* Internal error codes (negative values) */
#define MATH_ERR_BASE -10000
#define MATH_ERR_NOT_INITIALIZED (MATH_ERR_BASE - 0)
#define MATH_ERR_CACHE_FULL (MATH_ERR_BASE - 1)
#define MATH_ERR_KERNEL_COMM (MATH_ERR_BASE - 2)
#define MATH_ERR_PROOF_EXPIRED (MATH_ERR_BASE - 3)
#define MATH_ERR_TAMPERING (MATH_ERR_BASE - 4)
#define MATH_ERR_NO_KERNEL_SUPPORT (MATH_ERR_BASE - 5)
#define MATH_ERR_INVALID_ARG (MATH_ERR_BASE - 6)
#define MATH_ERR_TLS_FAILURE (MATH_ERR_BASE - 7)
#define MATH_ERR_MUTEX_FAILURE (MATH_ERR_BASE - 8)

#endif /* MATHEMATICAL_INTERNAL_H */
/**
 * @file mathematical_internal.h
 * @brief Internal implementation header for MINIX mathematical POSIX
 * capabilities.
 *
 * This header defines internal data structures, modes, flags, and function
 * prototypes used by the libc implementation of mathematical verification.
 * It must not be included by user programs; they should use
 * <minix/capability.h> for the public API.
 */

#ifndef MATHEMATICAL_INTERNAL_H
#define MATHEMATICAL_INTERNAL_H

/* Standard headers */
#include <limits.h>    /* PATH_MAX */
#include <pthread.h>   /* pthread_mutex_t, pthread_key_t */
#include <stdio.h>     /* FILE */
#include <sys/types.h> /* uid_t, gid_t, pid_t, size_t */
#include <time.h>      /* time_t */

/* MINIX public APIs */
#include <minix/callnr.h>     /* SYS_CAPABILITY_QUERY */
#include <minix/capability.h> /* capability_t, CAP_* rights */
#include <minix/const.h>      /* NONE, etc. */
#include <minix/ipc.h>        /* message, endpoint_t */

/** @brief Mathematical verification modes. */
enum math_mode {
  MATH_MODE_DISABLED = 0,  /**< No verification */
  MATH_MODE_ENABLED = 1,   /**< Verify when enabled by user or env */
  MATH_MODE_MANDATORY = 2, /**< Always verify; fail if not possible */
  MATH_MODE_AUDIT = 3      /**< Verify and log all operations */
};

/** @brief Kernel policy flags returned by SYS_CAPABILITY_QUERY. */
#define POLICY_AVAILABLE 0x0001 /**< Kernel supports capability system */
#define POLICY_MANDATORY_ALL                                                   \
  0x0002 /**< Kernel mandates verification for all                             \
          */
#define POLICY_MANDATORY_PRIV                                                  \
  0x0004                           /**< Kernel mandates verification for root */
#define POLICY_CACHE_KERNEL 0x0008 /**< Kernel supports its own caching */
#define POLICY_AUDIT_ALL 0x0010    /**< Kernel requests audit logging */

/**
 * @struct internal_capability
 * @brief Extended, opaque representation of a capability in userspace.
 *
 * This structure holds both the public-facing capability_t and additional
 * bookkeeping, cache metadata, and proof data.
 */
struct internal_capability {
  capability_t public_cap;           /**< User-visible identifier */
  uint64_t kernel_handle;            /**< Kernel reference handle */
  time_t cached_at;                  /**< Timestamp when cached */
  int cache_hits;                    /**< Hit count for this entry */
  pid_t owning_process;              /**< PID that owns this cache entry */
  unsigned char proof_hash[32];      /**< SHA-256 of proof (if any) */
  int validation_flags;              /**< Internal validation state */
  time_t proof_generation_timestamp; /**< When proof was generated */
};

/**
 * @struct cap_cache_entry
 * @brief Single entry in the capability cache hash table and LRU list.
 */
struct cap_cache_entry {
  char path[PATH_MAX];                   /**< Lookup key: filesystem path */
  int open_flags;                        /**< Flags passed to open() */
  uid_t uid;                             /**< Effective UID at cache time */
  gid_t gid;                             /**< Effective GID at cache time */
  struct internal_capability capability; /**< Cached capability data */
  time_t expiry;                         /**< Expiration time (absolute) */
  struct cap_cache_entry *hash_next;     /**< Next in hash bucket chain */
  struct cap_cache_entry *lru_prev, *lru_next; /**< LRU doubly‐linked list */
  unsigned long hit_count;                     /**< Times this entry was hit */
  time_t last_used;                            /**< Last access timestamp */
  int valid;                                   /**< Nonzero if entry is valid */
};

/**
 * @struct thread_math_state
 * @brief Per-thread overrides and caches for mathematical mode.
 */
struct thread_math_state {
  enum math_mode thread_mode; /**< Thread-specific mode */
  int mode_is_set;            /**< Has thread_mode been set? */
  int mode_locked;            /**< Prevent downgrading once set */
  struct internal_capability recent_caps[8]; /**< Tiny LRU of recent caps */
  int recent_caps_count;                     /**< Number of valid entries */
  int recent_caps_lru_idx;                   /**< Next victim index */
  int last_math_errno;                       /**< Last internal error code */
  char last_error_path[PATH_MAX];            /**< Path of last error */
};

/**
 * @struct math_state
 * @brief Global state for mathematical POSIX support within libc.
 */
struct math_state {
  int initialized;     /**< Has constructor run? */
  enum math_mode mode; /**< Current global mode */
  int kernel_support;  /**< POLICY_AVAILABLE? */
  unsigned int policy; /**< Raw policy flags */

  /* capability cache */
  int cache_enabled;                         /**< Nonzero if cache active */
  struct cap_cache_entry **cache_hash_table; /**< Hash table array */
  size_t cache_hash_size;                    /**< Number of buckets */
  struct cap_cache_entry *cache_lru_head;    /**< LRU list head */
  struct cap_cache_entry *cache_lru_tail;    /**< LRU list tail */
  size_t cache_current_entries;              /**< Current size */
  size_t cache_max_entries;                  /**< Maximum entries allowed */
  pthread_mutex_t cache_lock;                /**< Protects cache structures */

  /* statistics */
  struct {
    unsigned long verifications_attempted;
    unsigned long verifications_succeeded;
    unsigned long verifications_failed;
    unsigned long cache_hits;
    unsigned long cache_misses;
    unsigned long cache_evictions;
    unsigned long capability_derives;
    unsigned long capability_restricts;
    unsigned long capability_revokes;
  } stats;

  /* per-thread state key */
  pthread_key_t thread_state_key;   /**< TLS key */
  int thread_state_key_initialized; /**< Key created? */

  /* notifications */
  int kernel_notify_fd; /**< FD for kernel events */

  /* debugging / auditing */
  int debug_level;      /**< Verbosity */
  FILE *audit_log_file; /**< Audit log stream */
};

/** @brief Global instance, defined in mathematical_init.c */
extern struct math_state __math_state;

/*=== Internal function prototypes ===*/

/* Initialization and teardown */
void __mathematical_init(void) __attribute__((constructor(101)));
void __mathematical_fini(void) __attribute__((destructor(101)));
int __mathematical_reinit(void);
void __mathematical_atfork_child(void);

/* Mode and TLS */
enum math_mode __get_effective_mode(void);
int __should_use_mathematical(int flags);
struct thread_math_state *__get_thread_state(int create_if_missing);

/* Cache management */
void __init_capability_cache_subsystem(void);
void __destroy_capability_cache_subsystem(void);
struct cap_cache_entry *__find_cached_capability(const char *path, int flags,
                                                 uid_t uid, gid_t gid);
int __cache_capability(const char *path, int flags, uid_t uid, gid_t gid,
                       const struct internal_capability *cap, time_t expiry);
void __evict_expired_capabilities(void);
void __flush_capability_cache(void);
void __invalidate_path_capabilities(const char *path);

/* Kernel communication */
int __query_kernel_policy(unsigned int *policy_out);
int __notify_kernel_verification(const char *path, int result_code);
int __register_kernel_notifications(void);

/* Proof construction & verification */
struct capability_proof *__construct_capability_proof_libc(endpoint_t proc_ep,
                                                           const char *path,
                                                           int rights);
int __verify_capability_proof_libc(const struct capability_proof *proof);
void __free_capability_proof_libc(struct capability_proof *proof);

/* Debugging and diagnostics */
void __log_capability_operation(const char *op, const char *path, int result,
                                const char *detail);
void __dump_capability_stats(FILE *stream);
void __enable_capability_debugging(int level);
void __set_audit_log_file(FILE *log_file);

/* Security checks */
int __validate_capability_transition(const capability_t *from,
                                     const capability_t *to, int op);
int __check_capability_tampering(const struct internal_capability *cap);

/**
 * @brief Quick check to see if an open() call might need math support.
 * @param flags  open() flags
 * @return nonzero if math logic should run
 */
static inline int __quick_capability_check(int flags) {
  if (!(flags & O_MATHEMATICAL))
    return 0;
  if (!__math_state.initialized)
    return 0;
  if (__math_state.mode == MATH_MODE_DISABLED &&
      !(__math_state.policy & (POLICY_MANDATORY_ALL | POLICY_MANDATORY_PRIV)))
    return 0;
  return 1;
}

/* Internal error codes (negative values) */
#define MATH_ERR_BASE -10000
#define MATH_ERR_NOT_INITIALIZED (MATH_ERR_BASE - 0)
#define MATH_ERR_CACHE_FULL (MATH_ERR_BASE - 1)
#define MATH_ERR_KERNEL_COMM (MATH_ERR_BASE - 2)
#define MATH_ERR_PROOF_EXPIRED (MATH_ERR_BASE - 3)
#define MATH_ERR_TAMPERING (MATH_ERR_BASE - 4)
#define MATH_ERR_NO_KERNEL_SUPPORT (MATH_ERR_BASE - 5)
#define MATH_ERR_INVALID_ARG (MATH_ERR_BASE - 6)
#define MATH_ERR_TLS_FAILURE (MATH_ERR_BASE - 7)
#define MATH_ERR_MUTEX_FAILURE (MATH_ERR_BASE - 8)

#endif /* MATHEMATICAL_INTERNAL_H */
