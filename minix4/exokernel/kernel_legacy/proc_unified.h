/**
 * @file proc_unified.h  
 * @brief MINIX4 Unified Process Management Header
 * @details Consolidated process structures and definitions for MINIX4
 * @version 4.0.0
 * @date 2025-06-12
 */

#pragma once

#include "kernel.h"

/*===========================================================================*
 *                           FORWARD DECLARATIONS                           *
 *===========================================================================*/

struct proc;
struct priv;
struct spinlock;

/*===========================================================================*
 *                          SPINLOCK STRUCTURE                              *
 *===========================================================================*/

/** @brief Simple spinlock for SMP synchronization */
struct spinlock {
    volatile int locked;        /**< Lock state (0=free, 1=locked) */
    int cpu;                   /**< CPU that acquired the lock */
    const char *name;          /**< Lock name for debugging */
};

/*===========================================================================*
 *                          PRIVILEGE STRUCTURE                             *
 *===========================================================================*/

/** @brief Process privilege and capability structure */
struct priv {
    uint32_t s_flags;          /**< Privilege flags */
    uint32_t s_trap_mask;      /**< Allowed traps/interrupts */
    uint64_t s_notify_pending; /**< Pending notification bitmap */
    uint64_t s_int_pending;    /**< Pending interrupt bitmap */
    uint64_t s_sig_pending;    /**< Pending signal bitmap */
    
    /* IPC permissions */
    uint64_t s_ipc_from;       /**< Allowed IPC sources bitmap */
    uint64_t s_ipc_to;         /**< Allowed IPC destinations bitmap */
    
    /* System call permissions */
    uint64_t s_call_mask;      /**< Allowed system calls bitmap */
};

/** @brief Privilege flags */
#define SYS_PROC        0x01   /**< System process */
#define ROOT_SYS_PROC   0x02   /**< Root system process */
#define BILLABLE        0x04   /**< Billable for CPU time */

/*===========================================================================*
 *                           PROCESS STRUCTURE                              *
 *===========================================================================*/

/** @brief Process control block - the heart of MINIX process management */
struct proc {
    /* === Process Identity === */
    int p_nr;                  /**< Process slot number (-NR_TASKS to NR_PROCS-1) */
    endpoint_t p_endpoint;     /**< Unique process endpoint identifier */
    pid_t p_pid;              /**< POSIX process ID */
    uint32_t p_magic;         /**< Magic number for structure validation */
    
    /* === Process State === */
    uint32_t p_rts_flags;     /**< Runtime status flags (RTS_*) */
    int p_priority;           /**< Current scheduling priority (0-15) */
    int p_max_priority;       /**< Maximum allowed priority */
    int p_quantum_size_ms;    /**< Time quantum in milliseconds */
    int p_ticks_left;         /**< Ticks remaining in current quantum */
    uint64_t p_user_time;     /**< CPU time spent in user mode */
    uint64_t p_sys_time;      /**< CPU time spent in system mode */
    
    /* === IPC State === */
    endpoint_t p_getfrom;     /**< Expected sender endpoint (or ANY/NONE) */
    endpoint_t p_sendto;      /**< Destination endpoint for blocked send */
    message p_delivermsg;     /**< Message buffer for IPC operations */
    
    /* === Process Queues and Links === */
    struct proc *p_caller_q;  /**< Queue of processes sending to this process */
    struct proc *p_q_link;    /**< Link pointer for various queues */
    struct proc *p_nextready; /**< Next process in ready queue */
    struct proc *p_prevready; /**< Previous process in ready queue */
    
    /* === Notification System === */
    uint64_t p_ntf_q[BITMAP_CHUNKS]; /**< Notification pending bitmap */
    
    /* === Memory Management === */
    virt_addr_t p_memmap_start; /**< Start of process memory mapping */
    size_t p_memmap_size;       /**< Size of memory mapping */
    phys_addr_t p_cr3_phys;     /**< Physical address of page directory */
    void *p_cr3_v;              /**< Virtual address of page directory */
    
    /* === Signal Handling === */
    uint32_t p_pending_signals; /**< Bitmask of pending signals */
    uint32_t p_signal_mask;     /**< Signal mask */
    virt_addr_t p_signal_handler[32]; /**< Signal handler addresses */
    
    /* === Architecture-Specific Context === */
    union {
        struct {
            /* x86/x86_64 specific state */
            uint64_t regs[16];   /**< General purpose registers */
            uint64_t rip;        /**< Instruction pointer */
            uint64_t rsp;        /**< Stack pointer */
            uint64_t rflags;     /**< Flags register */
            uint16_t cs, ds, es, fs, gs, ss; /**< Segment registers */
        } x86;
        
        struct {
            /* ARM/AArch64 specific state */
            uint64_t regs[31];   /**< General purpose registers */
            uint64_t sp;         /**< Stack pointer */
            uint64_t pc;         /**< Program counter */
            uint64_t pstate;     /**< Processor state */
        } arm;
    } p_reg;
    
    /* === Privileges and Security === */
    struct priv *p_priv;       /**< Privilege structure pointer */
    void *p_cap_context;       /**< Capability context */
    
    /* === Synchronization === */
    struct spinlock p_lock;    /**< Per-process lock for SMP safety */
    
    /* === Parent-Child Relationships === */
    struct proc *p_parent;     /**< Parent process */
    struct proc *p_child;      /**< First child process */
    struct proc *p_sibling;    /**< Next sibling process */
    
    /* === Scheduler Integration === */
    struct proc *p_scheduler;  /**< Scheduler process for this process */
    int p_nice;               /**< Nice value (-20 to 19) */
    uint64_t p_deadline;      /**< Deadline for real-time scheduling */
    
    /* === Debugging and Profiling === */
    const char *p_name;       /**< Process name for debugging */
    uint64_t p_cycles_start;  /**< Cycle counter at process start */
    uint32_t p_context_switches; /**< Number of context switches */
};

/*===========================================================================*
 *                            QUEUE OPERATIONS                              *
 *===========================================================================*/

/** @brief Queue manipulation macros using pointer-to-pointer technique */
#define QUEUE_INIT(head) do { *(head) = NULL; } while(0)

#define QUEUE_INSERT_HEAD(head, elem, field) do { \
    (elem)->field = *(head); \
    *(head) = (elem); \
} while(0)

#define QUEUE_INSERT_TAIL(head, elem, field) do { \
    struct proc **_pp = (head); \
    while (*_pp != NULL) _pp = &(*_pp)->field; \
    *_pp = (elem); \
    (elem)->field = NULL; \
} while(0)

#define QUEUE_REMOVE(head, elem, field) do { \
    struct proc **_pp = (head); \
    while (*_pp != NULL && *_pp != (elem)) { \
        _pp = &(*_pp)->field; \
    } \
    if (*_pp == (elem)) { \
        *_pp = (elem)->field; \
        (elem)->field = NULL; \
    } \
} while(0)

/*===========================================================================*
 *                         FUNCTION PROTOTYPES                              *
 *===========================================================================*/

/* Process management core functions */
int proc_init(void);
struct proc *proc_lookup(endpoint_t endpoint);
struct proc *proc_addr(int proc_nr);
int proc_nr(struct proc *rp);
endpoint_t proc_endpoint(struct proc *rp);

/* Process state management */
void proc_schedule(struct proc *rp);
void proc_unschedule(struct proc *rp);
int proc_is_runnable(struct proc *rp);
void proc_set_priority(struct proc *rp, int priority);

/* IPC functions */
int proc_send(struct proc *caller, endpoint_t dest, message *msg);
int proc_receive(struct proc *caller, endpoint_t src, message *msg);
int proc_notify(struct proc *caller, endpoint_t dest);

/* Queue management */
void enqueue_head(struct proc **queue, struct proc *rp);
void enqueue_tail(struct proc **queue, struct proc *rp);
struct proc *dequeue_head(struct proc **queue);
void dequeue_proc(struct proc **queue, struct proc *rp);

/* Spinlock functions */
void spinlock_init(struct spinlock *lock, const char *name);
void spinlock_acquire(struct spinlock *lock);
void spinlock_release(struct spinlock *lock);
int spinlock_holding(struct spinlock *lock);

/* Privilege and capability functions */
void priv_init(struct priv *priv);
int priv_check_call(struct proc *caller, int call_nr);
int priv_check_ipc(struct proc *src, struct proc *dest);

/* Capability system integration */
void capability_proc_init(struct proc *rp);
void capability_proc_cleanup(struct proc *rp);

/* Notification system */
void set_sys_bit(uint64_t *map, int bit_nr);
void unset_sys_bit(uint64_t *map, int bit_nr);
int get_sys_bit(uint64_t *map, int bit_nr);

/* Process debugging and diagnostics */
void proc_dump(struct proc *rp);
void proc_dump_all(void);

/*===========================================================================*
 *                             INLINE HELPERS                               *
 *===========================================================================*/

/** @brief Check if process slot is free */
static inline int proc_is_slot_free(struct proc *rp) {
    return (rp->p_rts_flags & RTS_SLOT_FREE) != 0;
}

/** @brief Check if process is system process */
static inline int proc_is_system(struct proc *rp) {
    return rp->p_priv != NULL && (rp->p_priv->s_flags & SYS_PROC) != 0;
}

/** @brief Check if process can send to destination */
static inline int proc_can_send_to(struct proc *src, endpoint_t dest) {
    return src->p_priv != NULL; /* Simplified check */
}

/** @brief Get process privilege flags */
static inline uint32_t proc_priv_flags(struct proc *rp) {
    return rp->p_priv ? rp->p_priv->s_flags : 0;
}
