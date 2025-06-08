#ifndef PROC_H
#define PROC_H

#include <minix/const.h>       /* MINIX constants */
#ifndef __ASSEMBLY__

/** @file proc.h
 *  @brief Process table definition, runtime flags, and IPC primitives.
 *
 *  The process table contains per-process state:
 *    - CPU registers, segment descriptors
 *    - Scheduling parameters and accounting
 *    - IPC (message passing) buffers and flags
 *    - Signal handling state
 *    - VM request state
 *
 *  Offsets of struct proc fields used in assembly are defined in sconst.h.
 *  Update sconst.h when modifying this file.
 */

#include <minix/com.h>          /* message and endpoint types */
#include <minix/portio.h>       /* phys_bytes, port I/O types */
#include "const.h"              /* local kernel constants */
#include "priv.h"               /* privilege structure */

#include <minix/kernel_types.h> /* k_clock_t, k_sigset_t */
#include <klib/include/kmemory.h>
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include "kernel/k_spinlock_irq.h" /* spinlock_irq_t */

#ifdef CONFIG_SMP
#include <minix/clhlock.h> /* For clh_proc_state_t and clhlock_t */
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** Process table entry. */
struct proc {
    struct stackframe_s   p_reg;           /**< Saved user registers */
    struct segframe       p_seg;           /**< Segment descriptors */
    proc_nr_t             p_nr;            /**< Numeric process slot */
    struct priv          *p_priv;          /**< Privilege pointer */
    volatile u32_t        p_rts_flags;     /**< RTS flags (runnable iff zero) */
    volatile u32_t        p_misc_flags;    /**< Miscellaneous flags */

    char                  p_priority;      /**< Dynamic scheduling priority */
    u64_t                 p_cpu_time_left; /**< CPU ticks remaining */
    unsigned              p_quantum_size_ms; /**< Time quantum in ms */
    struct proc          *p_scheduler;     /**< Kernel scheduler endpoint */
    unsigned              p_cpu;           /**< Current CPU ID */

#ifdef CONFIG_SMP
    clhlock_t             p_lock;          /**< Process lock (CLH, MCS, etc. chosen at runtime) */
    clh_proc_state_t      p_clh_state;     /**< CLH lock state for p_lock if CLH is active */
    bitchunk_t            p_cpu_mask[BITMAP_CHUNKS(CONFIG_MAX_CPUS)];
    bitchunk_t            p_stale_tlb[BITMAP_CHUNKS(CONFIG_MAX_CPUS)];

#endif

    /** Scheduling/accounting statistics. */
    struct {
        u64_t enter_queue;     /**< TSC when enqueued */
        u64_t time_in_queue;   /**< Cycles spent waiting */
        unsigned long dequeues;/**< Number of times dequeued */
        unsigned long ipc_sync;/**< Synchronous IPC count */
        unsigned long ipc_async;/**< Asynchronous IPC count */
        unsigned long preempted;/**< Preemption count */
    } p_accounting;

    k_clock_t             p_dequeued;      /**< Time when last dequeued */
    k_clock_t             p_user_time;     /**< User-mode tick count */
    k_clock_t             p_sys_time;      /**< Kernel-mode tick count */
    k_clock_t             p_virt_left;     /**< Virtual timer ticks left */
    k_clock_t             p_prof_left;     /**< Profile timer ticks left */

    u64_t                 p_cycles;        /**< Total CPU cycles used */
    u64_t                 p_kcall_cycles;  /**< Cycles in kernel calls */
    u64_t                 p_kipc_cycles;   /**< Cycles in IPC routines */
    u64_t                 p_tick_cycles;   /**< Accumulated cycles per tick */
    struct cpuavg         p_cpuavg;        /**< Running CPU usage average */

    struct proc          *p_nextready;     /**< Next in ready queue */
    struct proc          *p_caller_q;      /**< Head of senders queue */
    struct proc          *p_q_link;        /**< Next sender in queue */
    endpoint_t            p_getfrom_e;     /**< Expected sender endpoint */
    endpoint_t            p_sendto_e;      /**< Destination endpoint */

    k_sigset_t            p_pending;       /**< Pending kernel signals */
    spinlock_irq_t        p_sig_lock;      /**< IRQ-safe signal lock */

    char                  p_name[PROC_NAME_LEN]; /**< Process name */
    endpoint_t            p_endpoint;      /**< Endpoint identifier */

    message               p_sendmsg;       /**< Outgoing message buffer */
    message               p_delivermsg;    /**< Incoming message buffer */
    vir_bytes             p_delivermsg_vir;/**< User addr for delivery */

    /** VM request state for fault handling. */
    struct {
        struct proc *nextrestart;    /**< Next in vmrestart chain */
        struct proc *nextrequestor;  /**< Next in vmrequest chain */
        int          type;           /**< VMSTYPE_* */
        union {
            message   reqmsg;        /**< Saved syscall request */
        } saved;
        int          req_type;       /**< VM parameter type */
        endpoint_t   target;         /**< Target endpoint */
        union {
            struct {
                vir_bytes start, length;
                uint8_t    writeflag;
            } check;                 /**< Range check parameters */
        } params;
        int          vmresult;       /**< VM result code */
    } p_vmrequest;

    /** Deferred syscall state for tracing. */
    struct {
        reg_t r1, r2, r3;           /**< Saved syscall registers */
    } p_defer;

#if DEBUG_TRACE
    int                   p_schedules;     /**< Dispatch count */
#endif
} __attribute__((aligned(CACHELINE_SIZE)));

/*---------------------------------------------------------------------------*
 *  Runtime flags (p_rts_flags)                                              *
 *---------------------------------------------------------------------------*/
/** Process slot is free (not in use) */
#define RTS_SLOT_FREE      0x0001
/** Process has been stopped (e.g., by tracer) */
#define RTS_PROC_STOP      0x0002
/** Blocked sending message */
#define RTS_SENDING        0x0004
/** Blocked receiving message */
#define RTS_RECEIVING      0x0008
/** New kernel signal arrived */
#define RTS_SIGNALED       0x0010
/** Signal delivery pending */
#define RTS_SIG_PENDING    0x0020
/** Process is being traced */
#define RTS_P_STOP         0x0040
/** System process not allowed to run */
#define RTS_NO_PRIV        0x0080
/** No IPC endpoints allowed */
#define RTS_NO_ENDPOINT    0x0100
/** Inhibit until VM sets pagetable */
#define RTS_VMINHIBIT      0x0200
/** Page fault pending */
#define RTS_PAGEFAULT      0x0400
/** Originator of VM memory request */
#define RTS_VMREQUEST      0x0800
/** Target of VM memory request */
#define RTS_VMREQTARGET    0x1000
/** Preempted by higher-priority process */
#define RTS_PREEMPTED      0x2000
/** Ran out of quantum */
#define RTS_NO_QUANTUM     0x4000
/** Not ready until VM completes boot */
#define RTS_BOOTINHIBIT    0x8000

/** Runnable if and only if p_rts_flags == 0. */
#define proc_is_runnable(p)    ((p)->p_rts_flags == 0)

/** Retrieve blocking endpoint: send or receive. */
#define P_BLOCKEDON(p) \
    (((p)->p_rts_flags & RTS_SENDING) ? (p)->p_sendto_e \
     : (((p)->p_rts_flags & RTS_RECEIVING) ? (p)->p_getfrom_e : NONE))

/** Test if specific RTS flags are set. */
#define RTS_ISSET(rp, f)  (((rp)->p_rts_flags & (f)) == (f))

/** Set RTS flags and dequeue if becoming non-runnable. */
#define RTS_SET(rp, f)                                \
    do {                                              \
        u32_t _old = (rp)->p_rts_flags;               \
        (rp)->p_rts_flags |= (f);                     \
        if ((_old) == 0 && (rp)->p_rts_flags != 0)    \
            dequeue(rp);                              \
    } while (0)

/** Clear RTS flags and enqueue if becoming runnable. */
#define RTS_UNSET(rp, f)                              \
    do {                                              \
        u32_t _old = (rp)->p_rts_flags;               \
        (rp)->p_rts_flags &= ~(f);                    \
        if (_old != 0 && (rp)->p_rts_flags == 0)      \
            enqueue(rp);                              \
    } while (0)

/** Replace RTS flags wholesale. */
#define RTS_SETFLAGS(rp, f)                           \
    do {                                              \
        if ((rp)->p_rts_flags == 0 && (f) != 0)       \
            dequeue(rp);                              \
        (rp)->p_rts_flags = (f);                      \
    } while (0)

/*---------------------------------------------------------------------------*
 *  Miscellaneous flags (p_misc_flags)                                       *
 *---------------------------------------------------------------------------*/
#define MF_REPLY_PEND      0x0001  /**< Reply to SENDREC pending */
#define MF_VIRT_TIMER      0x0002  /**< Virtual timer is running */
#define MF_PROF_TIMER      0x0004  /**< Profile timer is running */
#define MF_KCALL_RESUME    0x0008  /**< Resume interrupted kernel call */
#define MF_DELIVERMSG      0x0010  /**< Deliver pending message */
#define MF_SIG_DELAY       0x0020  /**< Delay signal until IPC done */
#define MF_SC_ACTIVE       0x0040  /**< In a traced system call */
#define MF_SC_DEFER        0x0080  /**< Deferred syscall entry */
#define MF_SC_TRACE        0x0100  /**< Syscall trace event */
#define MF_FPU_INITIALIZED 0x0200  /**< FPU state initialized */
#define MF_SENDING_FROM_KERNEL 0x0400 /**< Send originated in kernel */
#define MF_CONTEXT_SET     0x0800  /**< Context manually set */
#define MF_SPROF_SEEN      0x1000  /**< Profile seen this proc */
#define MF_FLUSH_TLB       0x2000  /**< TLB flush required */
#define MF_SENDA_VM_MISS   0x4000  /**< Async send deferred by VM */
#define MF_STEP            0x8000  /**< Single-step */
#define MF_MSGFAILED      0x10000  /**< Delivery failed */
#define MF_NICED          0x20000  /**< Lowered max priority */

/*---------------------------------------------------------------------------*
 *  Process table declarations                                                *
 *---------------------------------------------------------------------------*/
#define BEG_PROC_ADDR (&proc[0])
#define BEG_USER_ADDR (&proc[NR_TASKS])
#define END_PROC_ADDR (&proc[NR_TASKS + NR_PROCS])

#define proc_addr(n)   (&proc[NR_TASKS + (n)])
#define proc_nr(p)     ((p)->p_nr)

#define isokprocn(n)   ((unsigned)((n) + NR_TASKS) < NR_TASKS + NR_PROCS)
#define isemptyp(p)    ((p)->p_rts_flags == RTS_SLOT_FREE)
#define iskernelp(p)   ((p) < BEG_USER_ADDR)
#define iskerneln(n)   ((n) < 0)
#define isuserp(p)     ((p) >= BEG_USER_ADDR)
#define isusern(n)     ((n) >= 0)

extern struct proc proc[NR_TASKS + NR_PROCS]; /**< Process table */

/** Kernel IPC primitives */
int mini_send(struct proc *caller, endpoint_t dst, message *m_ptr, int flags);
int mini_receive(struct proc *caller, endpoint_t src, message *m_ptr, int flags);
int mini_notify(const struct proc *caller, endpoint_t dst);
int mini_senda(struct proc *caller, asynmsg_t *table, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLY__ */
#endif /* PROC_H */
