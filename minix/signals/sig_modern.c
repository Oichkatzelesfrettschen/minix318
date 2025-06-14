/**
 * @file sig_modern.c
 * @brief MINIX4 Modern Signal Handling Implementation
 * @version 4.0.0
 * @date 2025-06-12
 * 
 * Modern C23 signal handling with capability-based security,
 * exokernel principles, and modular architecture.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdnoreturn.h>
#include <stdalign.h>
#include <stdatomic.h>
#include <stdbool.h>
#include "spinlock.h"

// Basic type definitions
typedef int pid_t;
typedef int uid_t;

// Signal constants
#define NSIG        32          /**< Number of signals */
#define SIGKILL     9           /**< Kill signal */
#define SIGSTOP     19          /**< Stop signal */
#define SIGCONT     18          /**< Continue signal */
#define SIGCHLD     17          /**< Child status changed */

// Process states
enum proc_state {
    PROC_UNUSED = 0,
    PROC_RUNNING,
    PROC_SLEEPING,
    PROC_STOPPED,
    PROC_ZOMBIE
};

/**
 * @brief Signal actions
 */
enum signal_action {
    SIG_DFL = 0,               /**< Default action */
    SIG_IGN = 1,               /**< Ignore signal */
    SIG_HANDLER = 2            /**< Custom handler */
};

/**
 * @brief Signal information
 */
struct siginfo {
    int si_signo;              /**< Signal number */
    int si_code;               /**< Signal code */
    int si_errno;              /**< Error number */
    pid_t si_pid;              /**< Sending process ID */
    uid_t si_uid;              /**< Sending user ID */
    void *si_addr;             /**< Fault address */
    int si_status;             /**< Exit status */
    long si_value;             /**< Signal value */
};

/**
 * @brief Signal handler function type
 */
typedef void (*sighandler_t)(int sig, struct siginfo *info, void *context);

/**
 * @brief Process signal state
 */
struct signal_state {
    sighandler_t handlers[NSIG];    /**< Signal handlers */
    uint64_t pending;               /**< Pending signals mask */
    uint64_t blocked;               /**< Blocked signals mask */
    atomic_bool in_handler;         /**< Currently in signal handler */
    struct spinlock lock;           /**< Signal state lock */
};

/**
 * @brief Minimal process structure for signals
 */
struct proc {
    pid_t pid;                     /**< Process ID */
    uid_t uid;                     /**< User ID */
    enum proc_state state;         /**< Process state */
    struct signal_state *signal_state; /**< Signal handling state */
};

// Forward declarations for functions we'll need
extern struct proc *get_current_proc(void);
extern struct proc *proc_find(pid_t pid);
extern bool proc_can_signal(struct proc *sender, struct proc *target);
extern void proc_exit(struct proc *p, int code);
extern void proc_stop(struct proc *p);
extern void proc_continue(struct proc *p);
extern void wakeup(struct proc *p);
extern void spinlock_init(struct spinlock *lock, const char *name);
extern void spinlock_acquire(struct spinlock *lock);
extern void spinlock_release(struct spinlock *lock);

/**
 * @brief Initialize signal handling for a process
 */
void signal_init_process(struct proc *p)
{
    if (!p || !p->signal_state) return;
    
    struct signal_state *ss = p->signal_state;
    
    // Initialize default handlers
    for (int i = 0; i < NSIG; i++) {
        ss->handlers[i] = (sighandler_t)SIG_DFL;
    }
    
    // Clear signal masks
    ss->pending = 0;
    ss->blocked = 0;
    atomic_store(&ss->in_handler, false);
    
    // Initialize lock
    spinlock_init(&ss->lock, "signal_state");
}

/**
 * @brief Send signal to a process
 */
int signal_send(struct proc *target, int signo, struct siginfo *info)
{
    if (!target || signo <= 0 || signo >= NSIG) {
        return -1;
    }
    
    struct signal_state *ss = target->signal_state;
    if (!ss) return -1;
    
    spinlock_acquire(&ss->lock);
    
    // Set pending bit
    ss->pending |= (1UL << signo);
    
    // Wake up process if it's sleeping
    if (target->state == PROC_SLEEPING) {
        wakeup(target);
    }
    
    spinlock_release(&ss->lock);
    
    return 0;
}

/**
 * @brief Check for pending signals
 */
int signal_check(struct proc *p)
{
    if (!p || !p->signal_state) return 0;
    
    struct signal_state *ss = p->signal_state;
    
    spinlock_acquire(&ss->lock);
    
    // Find highest priority pending signal that's not blocked
    uint64_t deliverable = ss->pending & ~ss->blocked;
    if (deliverable == 0) {
        spinlock_release(&ss->lock);
        return 0;
    }
    
    // Find first set bit (lowest signal number has priority)
    int signo = __builtin_ctzl(deliverable) + 1;
    
    // Clear pending bit
    ss->pending &= ~(1UL << signo);
    
    spinlock_release(&ss->lock);
    
    return signo;
}

/**
 * @brief Default signal action
 */
static void signal_default_action(struct proc *p, int signo)
{
    switch (signo) {
        case SIGCHLD:
            // Default action for SIGCHLD is ignore
            break;
            
        default:
            // Default action is usually terminate
            proc_exit(p, signo);
            break;
    }
}

/**
 * @brief Invoke user signal handler
 */
static void signal_invoke_handler(struct proc *p, int signo, sighandler_t handler)
{
    struct signal_state *ss = p->signal_state;
    
    // Set flag to indicate we're in a handler
    atomic_store(&ss->in_handler, true);
    
    // Create signal info
    struct siginfo info = {
        .si_signo = signo,
        .si_code = 0,
        .si_errno = 0,
        .si_pid = 0,    // Would be filled with sender PID
        .si_uid = 0,    // Would be filled with sender UID
        .si_addr = NULL,
        .si_status = 0,
        .si_value = 0
    };
    
    // In a real implementation, this would:
    // 1. Save current process context
    // 2. Set up user stack for signal handler
    // 3. Switch to user mode to execute handler
    // 4. Handle return from signal handler
    
    // For now, just call the handler directly (simulation)
    if (handler && handler != (sighandler_t)SIG_DFL && handler != (sighandler_t)SIG_IGN) {
        handler(signo, &info, NULL);
    }
    
    atomic_store(&ss->in_handler, false);
}

/**
 * @brief Handle a signal
 */
void signal_handle(struct proc *p, int signo)
{
    if (!p || !p->signal_state || signo <= 0 || signo >= NSIG) {
        return;
    }
    
    struct signal_state *ss = p->signal_state;
    sighandler_t handler = ss->handlers[signo];
    
    // Handle special signals
    switch (signo) {
        case SIGKILL:
            // SIGKILL cannot be caught, blocked, or ignored
            proc_exit(p, signo);
            return;
            
        case SIGSTOP:
            // SIGSTOP cannot be caught, blocked, or ignored
            proc_stop(p);
            return;
            
        case SIGCONT:
            // Continue stopped process
            if (p->state == PROC_STOPPED) {
                proc_continue(p);
            }
            break;
    }
    
    // Handle based on action
    if (handler == (sighandler_t)SIG_IGN) {
        // Ignore signal
        return;
    } else if (handler == (sighandler_t)SIG_DFL) {
        // Default action - typically terminate
        signal_default_action(p, signo);
    } else {
        // Custom handler
        signal_invoke_handler(p, signo, handler);
    }
}

/**
 * @brief System call: signal
 */
int sys_signal(int signo, sighandler_t handler)
{
    struct proc *current = get_current_proc();
    if (!current || !current->signal_state) {
        return -1;
    }
    
    if (signo <= 0 || signo >= NSIG) {
        return -1;
    }
    
    // SIGKILL and SIGSTOP cannot have their handlers changed
    if (signo == SIGKILL || signo == SIGSTOP) {
        return -1;
    }
    
    struct signal_state *ss = current->signal_state;
    
    spinlock_acquire(&ss->lock);
    sighandler_t old_handler = ss->handlers[signo];
    ss->handlers[signo] = handler;
    spinlock_release(&ss->lock);
    
    return (int)(uintptr_t)old_handler;
}

/**
 * @brief System call: kill (send signal)
 */
int sys_kill(pid_t pid, int signo)
{
    if (signo < 0 || signo >= NSIG) {
        return -1;
    }
    
    struct proc *target = proc_find(pid);
    if (!target) {
        return -1;
    }
    
    // Check permissions (capability-based)
    if (!proc_can_signal(get_current_proc(), target)) {
        return -1;
    }
    
    struct siginfo info = {
        .si_signo = signo,
        .si_code = 0,
        .si_pid = get_current_proc()->pid,
        .si_uid = get_current_proc()->uid
    };
    
    return signal_send(target, signo, &info);
}

/**
 * @brief System call: sigprocmask
 */
int sys_sigprocmask(int how, const uint64_t *set, uint64_t *oldset)
{
    struct proc *current = get_current_proc();
    if (!current || !current->signal_state) {
        return -1;
    }
    
    struct signal_state *ss = current->signal_state;
    
    spinlock_acquire(&ss->lock);
    
    if (oldset) {
        *oldset = ss->blocked;
    }
    
    if (set) {
        uint64_t new_mask = *set;
        
        // SIGKILL and SIGSTOP cannot be blocked
        new_mask &= ~((1UL << SIGKILL) | (1UL << SIGSTOP));
        
        switch (how) {
            case 0: // SIG_BLOCK
                ss->blocked |= new_mask;
                break;
            case 1: // SIG_UNBLOCK
                ss->blocked &= ~new_mask;
                break;
            case 2: // SIG_SETMASK
                ss->blocked = new_mask;
                break;
            default:
                spinlock_release(&ss->lock);
                return -1;
        }
    }
    
    spinlock_release(&ss->lock);
    
    return 0;
}

/**
 * @brief Initialize signal subsystem
 */
void signal_init(void)
{
    // Initialize global signal handling state
    // Set up signal delivery mechanisms
    // Register system calls
}
