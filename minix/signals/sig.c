/**
 * @file sig.c
 * @brief Signal handling and process tracing implementation for MINIX
 * @details This module implements signal delivery, process tracing (ptrace),
 *          and core dump functionality. Originally based on early UNIX V6/V7
 *          signal handling mechanisms.
 * @author Original UNIX authors, modernized for MINIX
 * @version 3.1.8
 * @date 2024
 */

#include "../inode.h"
#include "../param.h"
#include "../proc.h"
#include "../systm.h"
#include "../user.h"
#include <reg.h>
#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @defgroup SignalHandling Signal Handling Subsystem
 * @brief Core signal delivery and handling mechanisms
 * @{
 */

/**
 * @def IPCPRI
 * @brief Priority level for inter-process communication during tracing
 * @details Negative priority ensures tracing operations get precedence
 */
#define IPCPRI (-1)

/**
 * @def SINCR
 * @brief Stack increment size for dynamic stack growth
 * @details Number of 64-byte blocks to grow stack by when needed
 */
#define SINCR 20

/**
 * @def MAX_CORE_SIZE
 * @brief Maximum size for core dump files (in blocks)
 */
#define MAX_CORE_SIZE 0x10000

/**
 * @struct inta_array
 * @brief Structure to access an array of integers for user space data
 * @details Used for accessing user process data during tracing operations
 */
struct inta_array {
    int inta[];  /**< Flexible array member for integer access */
};

/**
 * @struct ipc_data
 * @brief Inter-process communication data for ptrace operations
 * @details This structure maintains the state for parent-child communication
 *          during process tracing. The data is locked per user to prevent
 *          race conditions in multi-user environments.
 */
struct ipc_data {
    int ip_lock;    /**< Process ID that currently holds the lock (0 = unlocked) */
    int ip_req;     /**< Current trace request type (see ptrace command codes) */
    int ip_addr;    /**< Target address for trace operations */
    int ip_data;    /**< Data value for read/write trace operations */
} ipc;

/**
 * @brief Register location mapping for ptrace register access
 * @details Maps register indices to their locations in the saved register array
 */
static const int regloc[] = {
    R0, R1, R2, R3, R4, R5, R6, R7, RPS
};

/**
 * @brief Send signal to all processes controlled by specified TTY
 * @param tp Pointer to TTY structure
 * @param sig Signal number to send
 * @details Called by TTY driver for quit signals (SIGQUIT) and interrupts (SIGINT).
 *          Iterates through all processes and sends the signal to those whose
 *          controlling terminal matches the specified TTY.
 * @note This function does not validate signal numbers - caller must ensure validity
 * @see psignal()
 */
void signal(struct tty *tp, int sig)
{
    register struct proc *p;

    /* Validate inputs */
    if (tp == NULL || sig <= 0 || sig >= NSIG) {
        return;
    }

    /* Send signal to all processes using this TTY */
    for (p = &proc[0]; p < &proc[NPROC]; p++) {
        if (p->p_ttyp == tp) {
            psignal(p, sig);
        }
    }
}

/**
 * @brief Send specified signal to specified process
 * @param p Pointer to target process structure
 * @param sig Signal number to send (must be < NSIG)
 * @retval 0 Success
 * @retval -1 Invalid signal number
 * @details Posts a signal to the target process. If the process already has
 *          a pending signal that isn't SIGKILL, the new signal replaces it.
 *          Also adjusts process state to ensure signal will be processed.
 * @warning Does not validate process pointer - caller must ensure validity
 * @note SIGKILL cannot be overridden by other signals
 */
int psignal(struct proc *p, int sig)
{
    register struct proc *rp;

    /* Validate signal number */
    if (sig >= NSIG || sig <= 0) {
        return -1;
    }

    /* Validate process pointer */
    if (p == NULL) {
        return -1;
    }

    rp = p;
    
    /* Don't override SIGKILL with other signals */
    if (rp->p_sig != SIGKILL) {
        rp->p_sig = sig;
    }

    /* Ensure process will run to handle signal */
    if (rp->p_stat > PUSER) {
        rp->p_stat = PUSER;
    }

    /* Wake up sleeping process */
    if (rp->p_stat == SWAIT) {
        setrun(rp);
    }

    return 0;
}

/**
 * @brief Check if current process has pending signal to process
 * @retval >0 Signal number that needs processing
 * @retval 0 No signals pending
 * @details This function is called at least once each time a process enters
 *          the system. Handles traced processes specially by stopping them
 *          and notifying the parent. A signal doesn't directly affect a process;
 *          it sets a flag asking the process to handle it.
 * @note For traced processes, this may cause the process to stop
 * @see stop(), psig()
 */
int issig(void)
{
    register int n;
    register struct proc *p;

    p = u.u_procp;
    if (p == NULL) {
        return 0;
    }

    n = p->p_sig;
    if (n > 0) {
        /* Handle traced processes */
        if (p->p_flag & STRC) {
            stop();
            n = p->p_sig;
            if (n == 0) {
                return 0;
            }
        }

        /* Check if signal handler is installed */
        if (n < NSIG && (u.u_signal[n] & 1) == 0) {
            return n;
        }
    }

    return 0;
}

/**
 * @brief Enter tracing STOP state for debugged process
 * @details In this state, the parent process is informed and can send
 *          commands to control the child process. The process remains
 *          stopped until the parent continues it or it exits.
 * @note This function may not return if the process is killed while stopped
 * @see procxmt(), ptrace()
 */
void stop(void)
{
    register struct proc *pp, *cp;

loop:
    cp = u.u_procp;
    if (cp == NULL) {
        exit();
        return;
    }

    /* Don't trace init process */
    if (cp->p_ppid != 1) {
        /* Find parent process */
        for (pp = &proc[0]; pp < &proc[NPROC]; pp++) {
            if (pp->p_pid == cp->p_ppid) {
                wakeup(pp);              /* Notify parent */
                cp->p_stat = SSTOP;      /* Mark as stopped */
                swtch();                 /* Context switch */
                
                /* Check if still being traced */
                if ((cp->p_flag & STRC) == 0 || procxmt()) {
                    return;
                }
                goto loop;
            }
        }
    }
    
    /* Parent not found or is init - exit */
    exit();
}

/**
 * @brief Perform action specified by current signal
 * @details The usual sequence is: if(issig()) psig();
 *          This function either calls a user signal handler or performs
 *          the default action (usually termination with optional core dump).
 * @note This function handles stack manipulation for user signal handlers
 * @see issig(), core()
 */
void psig(void)
{
    register int n, p;
    register struct proc *rp;

    rp = u.u_procp;
    if (rp == NULL) {
        return;
    }

    n = rp->p_sig;
    rp->p_sig = 0;

    /* Check for user signal handler */
    p = u.u_signal[n];
    if (p != 0) {
        u.u_error = 0;
        
        /* Don't reset handler for SIGILL and SIGTRAP */
        if (n != SIGILL && n != SIGTRAP) {
            u.u_signal[n] = 0;
        }

        /* Set up user stack for signal handler */
        n = u.u_ar0[R6] - 4;
        if (grow((char *)n)) {
            suword(n + 2, u.u_ar0[RPS]);  /* Save processor status */
            suword(n, u.u_ar0[R7]);       /* Save return address */
            u.u_ar0[R6] = n;              /* Adjust stack pointer */
            u.u_ar0[RPS] &= ~TBIT;        /* Clear trace bit */
            u.u_ar0[R7] = p;              /* Jump to handler */
            return;
        }
    }

    /* Default signal actions */
    switch (n) {
    case SIGQUIT:
    case SIGILL:
    case SIGTRAP:
    case SIGIOT:
    case SIGEMT:
    case SIGFPE:
    case SIGBUS:
    case SIGSEGV:
    case SIGSYS:
        u.u_arg[0] = n;
        if (core()) {
            n += 0200;  /* Indicate core dumped */
        }
        break;
    }

    /* Exit with signal number */
    u.u_arg[0] = (u.u_ar0[R0] << 8) | n;
    exit();
}

/**
 * @brief Create core dump file for crashed process
 * @retval 1 Core dump successful
 * @retval 0 Core dump failed
 * @details Creates a core image in the file "core" containing the user area
 *          followed by the entire data+stack segments. This is useful for
 *          post-mortem debugging.
 * @warning Security implications exist when called by setuid programs
 * @note Core file format: USIZE blocks of user area + data/stack segments
 */
int core(void)
{
    register int s;
    register struct inode *ip;
    extern char schar;

    u.u_error = 0;
    u.u_dirp = "core";
    
    /* Try to create/open core file */
    ip = namei(&schar, 1);
    if (ip == NULL) {
        if (u.u_error) {
            return 0;
        }
        ip = maknode(0666);
        if (ip == NULL) {
            return 0;
        }
    }

    /* Security check: only write if we own it and have permission */
    if (!access(ip, IWRITE) && 
        (ip->i_mode & IFMT) == IFREG && 
        u.u_uid == u.u_ruid) {
        
        /* Truncate existing core file */
        itrunc(ip);
        
        /* Write user area */
        u.u_offset[0] = 0;
        u.u_offset[1] = 0;
        u.u_base = (char *)&u;
        u.u_count = USIZE * 64;
        u.u_segflg = 1;  /* Kernel space */
        writei(ip);

        /* Write data and stack segments */
        s = u.u_procp->p_size - USIZE;
        if (s > 0 && s < MAX_CORE_SIZE) {
            estabur(0, s, 0, 0);
            u.u_base = NULL;
            u.u_count = s * 64;
            u.u_segflg = 0;  /* User space */
            writei(ip);
        }
    }

    iput(ip);
    return (u.u_error == 0);
}

/**
 * @brief Grow process stack to include specified stack pointer
 * @param sp Stack pointer that must be included in stack
 * @retval 1 Stack growth successful
 * @retval 0 Stack growth failed
 * @details Dynamically expands the process stack downward to accommodate
 *          the specified stack pointer. This enables automatic stack growth
 *          when the process needs more stack space.
 * @note Stack grows downward (toward lower addresses)
 */
int grow(char *sp)
{
    register int a, si, i;
    intptr_t sp_addr = (intptr_t)sp;

    /* Check if SP is already within current stack */
    if (sp_addr >= -u.u_ssize * 64) {
        return 0;
    }

    /* Calculate required stack increment */
    si = (int)((-sp_addr) / 64) - u.u_ssize + SINCR;
    if (si <= 0) {
        return 0;
    }

    /* Attempt to establish new address space */
    if (estabur(u.u_tsize, u.u_dsize, u.u_ssize + si, u.u_sep)) {
        return 0;
    }

    /* Expand process size */
    expand(u.u_procp->p_size + si);

    /* Copy existing stack to new location */
    a = u.u_procp->p_addr + u.u_procp->p_size;
    for (i = u.u_ssize; i > 0; i--) {
        a--;
        copyseg(a - si, a);
    }

    /* Clear new stack space */
    for (i = si; i > 0; i--) {
        clearseg(--a);
    }

    u.u_ssize += si;
    return 1;
}

/**
 * @brief System call interface for process tracing (ptrace)
 * @details Implements the ptrace system call for debugging support.
 *          Allows parent processes to control and examine child processes.
 * @note Uses global IPC structure for parent-child communication
 * @see procxmt(), stop()
 */
void ptrace(void)
{
    register struct proc *p;

    /* Case 0: Enable tracing for current process */
    if (u.u_arg[2] <= 0) {
        u.u_procp->p_flag |= STRC;
        return;
    }

    /* Find target process */
    for (p = proc; p < &proc[NPROC]; p++) {
        if (p->p_stat == SSTOP && 
            p->p_pid == u.u_arg[0] && 
            p->p_ppid == u.u_procp->p_pid) {
            goto found;
        }
    }
    
    u.u_error = ESRCH;
    return;

found:
    /* Wait for IPC lock */
    while (ipc.ip_lock) {
        sleep(&ipc, IPCPRI);
    }

    /* Set up trace request */
    ipc.ip_lock = p->p_pid;
    ipc.ip_data = u.u_ar0[R0];
    ipc.ip_addr = u.u_arg[1] & ~01;  /* Word align */
    ipc.ip_req = u.u_arg[2];
    
    /* Wake up traced process */
    p->p_flag &= ~SWTED;
    setrun(p);

    /* Wait for completion */
    while (ipc.ip_req > 0) {
        sleep(&ipc, IPCPRI);
    }

    /* Return result */
    u.u_ar0[R0] = ipc.ip_data;
    if (ipc.ip_req < 0) {
        u.u_error = EIO;
    }

    /* Release lock */
    ipc.ip_lock = 0;
    wakeup(&ipc);
}

/**
 * @brief Execute tracing command from parent process
 * @retval 1 Continue execution
 * @retval 0 Remain stopped
 * @details Child process executes commands from parent during tracing.
 *          Handles memory access, register manipulation, and process control.
 * @note This function runs in the context of the traced process
 */
int procxmt(void)
{
    register int i;
    register int *p;

    /* Verify this process is being traced */
    if (ipc.ip_lock != u.u_procp->p_pid) {
        return 0;
    }

    i = ipc.ip_req;
    ipc.ip_req = 0;
    wakeup(&ipc);

    switch (i) {
    case 1: /* Read user instruction space */
        if (fuibyte((char *)ipc.ip_addr) == -1) {
            goto error;
        }
        ipc.ip_data = fuiword((char *)ipc.ip_addr);
        break;

    case 2: /* Read user data space */
        if (fubyte((char *)ipc.ip_addr) == -1) {
            goto error;
        }
        ipc.ip_data = fuword((char *)ipc.ip_addr);
        break;

    case 3: /* Read user area */
        i = ipc.ip_addr;
        if (i < 0 || i >= (USIZE << 6)) {
            goto error;
        }
        ipc.ip_data = ((int *)&u)[i >> 1];
        break;

    case 4: /* Write user instruction space */
        if (suiword((char *)ipc.ip_addr, 0) < 0) {
            goto error;
        }
        suiword((char *)ipc.ip_addr, ipc.ip_data);
        break;

    case 5: /* Write user data space */
        if (suword((char *)ipc.ip_addr, 0) < 0) {
            goto error;
        }
        suword((char *)ipc.ip_addr, ipc.ip_data);
        break;

    case 6: /* Write user area */
        p = &((int *)&u)[ipc.ip_addr >> 1];
        
        /* Check if address is in floating point save area */
        if (p >= u.u_fsav && p < &u.u_fsav[25]) {
            goto ok;
        }
        
        /* Check if address is a register */
        for (i = 0; i < 9; i++) {
            if (p == &u.u_ar0[regloc[i]]) {
                goto ok;
            }
        }
        goto error;

    ok:
        /* Special handling for processor status register */
        if (p == &u.u_ar0[RPS]) {
            ipc.ip_data |= 0170000;  /* Ensure user space */
            ipc.ip_data &= ~0340;    /* Set priority 0 */
        }
        *p = ipc.ip_data;
        break;

    case 7: /* Set signal and continue */
        if (ipc.ip_data >= 0 && ipc.ip_data < NSIG) {
            u.u_procp->p_sig = ipc.ip_data;
        }
        return 1;

    case 8: /* Force exit */
        exit();
        break;

    default:
    error:
        ipc.ip_req = -1;
    }
    
    return 0;
}

/** @} */ /* End of SignalHandling group */
