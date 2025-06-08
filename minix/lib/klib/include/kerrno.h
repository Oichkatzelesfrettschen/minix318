/**
 * @file kerrno.h
 * @brief Kernel-internal definitions for common POSIX-style error numbers.
 *
 * This file provides a consistent set of error number definitions for use
 * within the kernel and klib, avoiding direct reliance on userspace <errno.h>.
 * A 'K_' prefix is used to prevent potential clashes if userspace headers
 * are ever inadvertently included in the same compilation unit or during
 * certain types of testing.
 */

#ifndef _KLIB_KERRNO_H
#define _KLIB_KERRNO_H

/* Standard POSIX-style error numbers for kernel use */
#define K_EPERM   1   /**< Operation not permitted */
#define K_ENOENT  2   /**< No such file or directory */
#define K_ESRCH   3   /**< No such process */
#define K_EINTR   4   /**< Interrupted system call */
#define K_EIO     5   /**< I/O error */
#define K_ENXIO   6   /**< No such device or address */
#define K_E2BIG   7   /**< Argument list too long */
#define K_ENOEXEC 8   /**< Exec format error */
#define K_EBADF   9   /**< Bad file number */
#define K_ECHILD  10  /**< No child processes */
#define K_EAGAIN  11  /**< Try again (may be EWOULDBLOCK) */
#define K_ENOMEM  12  /**< Out of memory */
#define K_EACCES  13  /**< Permission denied */
#define K_EFAULT  14  /**< Bad address */
#define K_ENOTBLK 15  /**< Block device required */
#define K_EBUSY   16  /**< Device or resource busy */
#define K_EEXIST  17  /**< File exists */
#define K_EXDEV   18  /**< Cross-device link */
#define K_ENODEV  19  /**< No such device */
#define K_ENOTDIR 20  /**< Not a directory */
#define K_EISDIR  21  /**< Is a directory */
#define K_EINVAL  22  /**< Invalid argument */
#define K_ENFILE  23  /**< File table overflow */
#define K_EMFILE  24  /**< Too many open files */
#define K_ENOTTY  25  /**< Not a typewriter (inappropriate ioctl) */
#define K_ETXTBSY 26  /**< Text file busy */
#define K_EFBIG   27  /**< File too large */
#define K_ENOSPC  28  /**< No space left on device */
#define K_ESPIPE  29  /**< Illegal seek */
#define K_EROFS   30  /**< Read-only file system */
#define K_EMLINK  31  /**< Too many links */
#define K_EPIPE   32  /**< Broken pipe */
#define K_EDOM    33  /**< Math argument out of domain of func */
#define K_ERANGE  34  /**< Math result not representable */
#define K_EDEADLK 35  /**< Resource deadlock would occur */
/* #define K_EDEADLOCK K_EDEADLK */ /**< Alias for EDEADLK */

#define K_ENAMETOOLONG 36 /**< File name too long */
#define K_ENOLCK       37 /**< No record locks available */
#define K_ENOSYS       38 /**< Function not implemented / System call not available */
#define K_ENOTEMPTY    39 /**< Directory not empty */
#define K_ELOOP        40 /**< Too many symbolic links encountered */
#define K_EWOULDBLOCK  K_EAGAIN /**< Operation would block (alias for EAGAIN) */

/* Additional common error numbers */
#define K_ENOMSG   42  /**< No message of desired type */
#define K_EIDRM    43  /**< Identifier removed */
#define K_EINPROGRESS  115 /**< Operation now in progress (from Linux errno.h) */
#define K_EALREADY     114 /**< Operation already in progress (from Linux errno.h) */
#define K_ENOTSOCK     88  /**< Socket operation on non-socket (from Linux errno.h) */
#define K_EDESTADDRREQ 89  /**< Destination address required (from Linux errno.h) */
#define K_EMSGSIZE     90  /**< Message too long (from Linux errno.h) */
#define K_EPROTOTYPE   91  /**< Protocol wrong type for socket (from Linux errno.h) */
#define K_ENOPROTOOPT  92  /**< Protocol not available (from Linux errno.h) */
#define K_EPROTONOSUPPORT 93 /**< Protocol not supported (from Linux errno.h) */
#define K_ESOCKTNOSUPPORT 94 /**< Socket type not supported (from Linux errno.h) */
#define K_EOPNOTSUPP   95  /**< Operation not supported on transport endpoint (from Linux errno.h) */
#define K_EPFNOSUPPORT 96  /**< Protocol family not supported (from Linux errno.h) */
#define K_EAFNOSUPPORT 97  /**< Address family not supported by protocol (from Linux errno.h) */
#define K_EADDRINUSE   98  /**< Address already in use (from Linux errno.h) */
#define K_EADDRNOTAVAIL 99 /**< Cannot assign requested address (from Linux errno.h) */
#define K_ENETDOWN     100 /**< Network is down (from Linux errno.h) */
#define K_ENETUNREACH  101 /**< Network is unreachable (from Linux errno.h) */
#define K_ENETRESET    102 /**< Network dropped connection because of reset (from Linux errno.h) */
#define K_ECONNABORTED 103 /**< Software caused connection abort (from Linux errno.h) */
#define K_ECONNRESET   104 /**< Connection reset by peer (from Linux errno.h) */
#define K_ENOBUFS      105 /**< No buffer space available (from Linux errno.h) */
#define K_EISCONN      106 /**< Transport endpoint is already connected (from Linux errno.h) */
#define K_ENOTCONN     107 /**< Transport endpoint is not connected (from Linux errno.h) */
#define K_ESHUTDOWN    108 /**< Cannot send after transport endpoint shutdown (from Linux errno.h) */
#define K_ETIMEDOUT    110 /**< Connection timed out (from Linux errno.h) */
#define K_ECONNREFUSED 111 /**< Connection refused (from Linux errno.h) */
#define K_EHOSTDOWN    112 /**< Host is down (from Linux errno.h) */
#define K_EHOSTUNREACH 113 /**< No route to host (from Linux errno.h) */

#define K_ECANCELED    125 /**< Operation Canceled (from Linux errno.h) */

#endif /* _KLIB_KERRNO_H */
