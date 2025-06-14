/**
 * @file ksignal.h
 * @brief Kernel-internal definitions for common POSIX-style signal numbers.
 *
 * This file provides a consistent set of signal number definitions for use
 * within the kernel and klib, prefixed with 'K_' to avoid clashes with
 * userspace signal definitions if headers are inadvertently mixed.
 */

#ifndef _KLIB_KSIGNAL_H
#define _KLIB_KSIGNAL_H

/* Standard POSIX-like signal numbers for kernel use */
#define K_SIGHUP    1  /**< Hangup */
#define K_SIGINT    2  /**< Interrupt */
#define K_SIGQUIT   3  /**< Quit */
#define K_SIGILL    4  /**< Illegal instruction */
#define K_SIGTRAP   5  /**< Trace/breakpoint trap */
#define K_SIGABRT   6  /**< Aborted */
#define K_SIGIOT    K_SIGABRT /**< IOT trap. Alias for K_SIGABRT */
#define K_SIGBUS    7  /**< Bus error */
#define K_SIGFPE    8  /**< Floating point exception */
#define K_SIGKILL   9  /**< Killed */
#define K_SIGUSR1   10 /**< User defined signal 1 */
#define K_SIGSEGV   11 /**< Segmentation fault */
#define K_SIGUSR2   12 /**< User defined signal 2 */
#define K_SIGPIPE   13 /**< Broken pipe */
#define K_SIGALRM   14 /**< Alarm clock */
#define K_SIGTERM   15 /**< Terminated */
#define K_SIGSTKFLT 16 /**< Stack fault (obsolete in Linux, but sometimes defined) */
#define K_SIGCHLD   17 /**< Child process terminated, stopped, or continued */
#define K_SIGCLD    K_SIGCHLD /**< Alias for K_SIGCHLD */
#define K_SIGCONT   18 /**< Continue executing, if stopped */
#define K_SIGSTOP   19 /**< Stopped (signal) */
#define K_SIGTSTP   20 /**< Stopped (user) */
#define K_SIGTTIN   21 /**< Stopped (tty input) */
#define K_SIGTTOU   22 /**< Stopped (tty output) */
#define K_SIGURG    23 /**< Urgent I/O condition */
#define K_SIGXCPU   24 /**< CPU time limit exceeded */
#define K_SIGXFSZ   25 /**< File size limit exceeded */
#define K_SIGVTALRM 26 /**< Virtual timer expired */
#define K_SIGPROF   27 /**< Profiling timer expired */
#define K_SIGWINCH  28 /**< Window changed */
#define K_SIGIO     29 /**< I/O possible */
#define K_SIGPOLL   K_SIGIO /**< Pollable event. Alias for K_SIGIO */
#define K_SIGPWR    30 /**< Power failure */
#define K_SIGSYS    31 /**< Bad system call */
#define K_SIGUNUSED K_SIGSYS /**< Alias for K_SIGSYS */

/**
 * @brief Total number of K_ prefixed signals defined.
 * This should be one greater than the highest defined K_SIG* value
 * if signals are numbered 1 to K_SIGRTMAX (or equivalent).
 * If K_SIGSYS is 31, then K_NSIG should be 32.
 */
#define K_NSIG      32

#endif /* _KLIB_KSIGNAL_H */
