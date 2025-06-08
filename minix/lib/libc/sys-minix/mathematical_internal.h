/**
 * @file mathematical_internal.h
 * @brief Internal implementation header for MINIX mathematical POSIX capabilities.
 *
 * This header defines internal data structures, modes, flags, and function
 * prototypes used by the libc implementation of mathematical verification.
 * It must not be included by user programs; they should use <minix/capability.h>
 * for the public API.
 */

#ifndef MATHEMATICAL_INTERNAL_H
#define MATHEMATICAL_INTERNAL_H

/* Standard headers */
#include <sys/types.h>   /* uid_t, gid_t, pid_t, size_t */
#include <time.h>        /* time_t */
#include <limits.h>      /* PATH_MAX */
#include <pthread.h>     /* pthread_mutex_t, pthread_key_t */
#include <stdio.h>       /* FILE */

/* MINIX public APIs */
#include <minix/capability.h>  /* capability_t, CAP_* rights */
#include <minix/ipc.h>         /* message, endpoint_t */
#include <minix/const.h>       /* NONE, etc. */
#include <minix/callnr.h>      /* SYS_CAPABILITY_QUERY */

/** @brief Mathematical verification modes. */
enum math_mode {
    MATH_MODE_DISABLED  = 0, /**< No verification */
    MATH_MODE_ENABLED   = 1, /**< Verify when enabled by user or env */
    MATH_MODE_MANDATORY = 2, /**< Always verify; fail if not possible */
    MATH_MODE_AUDIT     = 3  /**< Verify and log all operations */
};

/** @brief Kernel policy flags returned by SYS_CAPABILITY_QUERY. */
#define POLICY_AVAILABLE      0x0001 /**< Kernel supports capability system */
#define POLICY_MANDATORY_ALL  0x0002 /**< Kernel mandates verification for all */
#define POLICY_MANDATORY_PRIV 0x0004 /**< Kernel mandates verification for root */
#define POLICY_CACHE_KERNEL   0x0008 /**< Kernel supports its own caching */
#define POLICY_AUDIT_ALL      0x0010 /**< Kernel requests audit logging */

/**
 * @struct internal_capability
 * @brief Extended, opaque representation of a capability in userspace.
 *
 * This structure holds both the public-facing capability_t and additional
 * bookkeeping, cache metadata, and proof data.
 */
struct internal_capability {
    capability_t     public_cap;              /**< User-visible identifier */
    uint64_t         kernel_handle;           /**< Kernel reference handle */
    time_t           cached_at;               /**< Timestamp when cached */
    int              cache_hits;              /**< Hit count for this entry */
    pid_t            owning_process;          /**< PID that owns this cache entry */
    unsigned char    proof_hash[32];          /**< SHA-256 of proof (if any) */
    int              validation_flags;        /**< Internal validation state */
    time_t           proof_generation_timestamp; /**< When proof was generated */
};

/**
 * @struct cap_cache_entry
 * @brief Single entry in the capability cache hash table and LRU list.
 */
struct cap_cache_entry {
    char                      path[PATH_MAX]; /**< Lookup key: filesystem path */
    int                       open_flags;     /**< Flags passed to open() */
    uid_t                     uid;            /**< Effective UID at cache time */
    gid_t                     gid;            /**< Effective GID at cache time */
    struct internal_capability capability;     /**< Cached capability data */
    time_t                    expiry;         /**< Expiration time (absolute) */
    struct cap_cache_entry  * hash_next;      /**< Next in hash bucket chain */
    struct cap_cache_entry  * lru_prev, *lru_next; /**< LRU doubly‐linked list */
    unsigned long             hit_count;      /**< Times this entry was hit */
    time_t                    last_used;      /**< Last access timestamp */
    int                       valid;          /**< Nonzero if entry is valid */
};

/**
 * @struct thread_math_state
 * @brief Per-thread overrides and caches for mathematical mode.
 */
struct thread_math_state {
    enum math_mode            thread_mode;       /**< Thread-specific mode */
    int                       mode_is_set;       /**< Has thread_mode been set? */
    int                       mode_locked;       /**< Prevent downgrading once set */
    struct internal_capability recent_caps[8];   /**< Tiny LRU of recent caps */
    int                       recent_caps_count; /**< Number of valid entries */
    int                       recent_caps_lru_idx; /**< Next victim index */
    int                       last_math_errno;   /**< Last internal error code */
    char                      last_error_path[PATH_MAX]; /**< Path of last error */
};

/**
 * @struct math_state
 * @brief Global state for mathematical POSIX support within libc.
 */
struct math_state {
    int                       initialized;      /**< Has constructor run? */
    enum math_mode            mode;             /**< Current global mode */
    int                       kernel_support;   /**< POLICY_AVAILABLE? */
    unsigned int              policy;           /**< Raw policy flags */

    /* capability cache */
    int                       cache_enabled;    /**< Nonzero if cache active */
    struct cap_cache_entry ** cache_hash_table; /**< Hash table array */
    size_t                    cache_hash_size;  /**< Number of buckets */
    struct cap_cache_entry  * cache_lru_head;   /**< LRU list head */
    struct cap_cache_entry  * cache_lru_tail;   /**< LRU list tail */
    size_t                    cache_current_entries; /**< Current size */
    size_t                    cache_max_entries; /**< Maximum entries allowed */
    pthread_mutex_t           cache_lock;       /**< Protects cache structures */

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
    pthread_key_t             thread_state_key;        /**< TLS key */
    int                       thread_state_key_initialized; /**< Key created? */

    /* notifications */
    int                       kernel_notify_fd;  /**< FD for kernel events */

    /* debugging / auditing */
    int                       debug_level;      /**< Verbosity */
    FILE *                    audit_log_file;   /**< Audit log stream */
};

/** @brief Global instance, defined in mathematical_init.c */
extern struct math_state __math_state;

/*=== Internal function prototypes ===*/

/* Initialization and teardown */
void __mathematical_init(void) __attribute__((constructor(101)));
void __mathematical_fini(void) __attribute__((destructor(101)));
int  __mathematical_reinit(void);
void __mathematical_atfork_child(void);

/* Mode and TLS */
enum math_mode            __get_effective_mode(void);
int                       __should_use_mathematical(int flags);
struct thread_math_state * __get_thread_state(int create_if_missing);

/* Cache management */
void                      __init_capability_cache_subsystem(void);
void                      __destroy_capability_cache_subsystem(void);
struct cap_cache_entry  * __find_cached_capability(const char *path, int flags,
                                                   uid_t uid, gid_t gid);
int                       __cache_capability(const char *path, int flags,
                                             uid_t uid, gid_t gid,
                                             const struct internal_capability *cap,
                                             time_t expiry);
void                      __evict_expired_capabilities(void);
void                      __flush_capability_cache(void);
void                      __invalidate_path_capabilities(const char *path);

/* Kernel communication */
int                       __query_kernel_policy(unsigned int *policy_out);
int                       __notify_kernel_verification(const char *path,
                                                       int result_code);
int                       __register_kernel_notifications(void);

/* Proof construction & verification */
struct capability_proof * __construct_capability_proof_libc(endpoint_t proc_ep,
                                                             const char *path,
                                                             int rights);
int                       __verify_capability_proof_libc(const struct capability_proof *proof);
void                      __free_capability_proof_libc(struct capability_proof *proof);

/* Debugging and diagnostics */
void                      __log_capability_operation(const char *op,
                                                     const char *path,
                                                     int result,
                                                     const char *detail);
void                      __dump_capability_stats(FILE *stream);
void                      __enable_capability_debugging(int level);
void                      __set_audit_log_file(FILE *log_file);

/* Security checks */
int                       __validate_capability_transition(const capability_t *from,
                                                           const capability_t *to,
                                                           int op);
int                       __check_capability_tampering(const struct internal_capability *cap);

/**
 * @brief Quick check to see if an open() call might need math support.
 * @param flags  open() flags
 * @return nonzero if math logic should run
 */
static inline int __quick_capability_check(int flags) {
    if (!(flags & O_MATHEMATICAL)) return 0;
    if (!__math_state.initialized)    return 0;
    if (__math_state.mode == MATH_MODE_DISABLED &&
        !(__math_state.policy & (POLICY_MANDATORY_ALL|POLICY_MANDATORY_PRIV)))
        return 0;
    return 1;
}

/* Internal error codes (negative values) */
#define MATH_ERR_BASE              -10000
#define MATH_ERR_NOT_INITIALIZED   (MATH_ERR_BASE - 0)
#define MATH_ERR_CACHE_FULL        (MATH_ERR_BASE - 1)
#define MATH_ERR_KERNEL_COMM       (MATH_ERR_BASE - 2)
#define MATH_ERR_PROOF_EXPIRED     (MATH_ERR_BASE - 3)
#define MATH_ERR_TAMPERING         (MATH_ERR_BASE - 4)
#define MATH_ERR_NO_KERNEL_SUPPORT (MATH_ERR_BASE - 5)
#define MATH_ERR_INVALID_ARG       (MATH_ERR_BASE - 6)
#define MATH_ERR_TLS_FAILURE       (MATH_ERR_BASE - 7)
#define MATH_ERR_MUTEX_FAILURE     (MATH_ERR_BASE - 8)

#endif /* MATHEMATICAL_INTERNAL_H */
