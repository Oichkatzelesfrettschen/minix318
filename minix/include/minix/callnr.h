/* This header file defines the calls to PM and VFS. */
#ifndef _MINIX_CALLNR_H
#define _MINIX_CALLNR_H

/*===========================================================================*
 *				Calls to PM				     *
 *===========================================================================*/

#define PM_BASE 0x000

#define IS_PM_CALL(type) (((type) & ~0xff) == PM_BASE)

/* Message type 0 is traditionally reserved. */
#define PM_EXIT (PM_BASE + 1)
#define PM_FORK (PM_BASE + 2)
#define PM_WAIT4 (PM_BASE + 3)
#define PM_GETPID (PM_BASE + 4)
#define PM_SETUID (PM_BASE + 5)
#define PM_GETUID (PM_BASE + 6)
#define PM_STIME (PM_BASE + 7)
#define PM_PTRACE (PM_BASE + 8)
#define PM_SETGROUPS (PM_BASE + 9)
#define PM_GETGROUPS (PM_BASE + 10)
#define PM_KILL (PM_BASE + 11)
#define PM_SETGID (PM_BASE + 12)
#define PM_GETGID (PM_BASE + 13)
#define PM_EXEC (PM_BASE + 14)
#define PM_SETSID (PM_BASE + 15)
#define PM_GETPGRP (PM_BASE + 16)
#define PM_ITIMER (PM_BASE + 17)
#define PM_GETMCONTEXT (PM_BASE + 18)
#define PM_SETMCONTEXT (PM_BASE + 19)
#define PM_SIGACTION (PM_BASE + 20)
#define PM_SIGSUSPEND (PM_BASE + 21)
#define PM_SIGPENDING (PM_BASE + 22)
#define PM_SIGPROCMASK (PM_BASE + 23)
#define PM_SIGRETURN (PM_BASE + 24)
#define PM_SYSUNAME (PM_BASE + 25) /* obsolete */
#define PM_GETPRIORITY (PM_BASE + 26)
#define PM_SETPRIORITY (PM_BASE + 27)
#define PM_GETTIMEOFDAY (PM_BASE + 28)
#define PM_SETEUID (PM_BASE + 29)
#define PM_SETEGID (PM_BASE + 30)
#define PM_ISSETUGID (PM_BASE + 31)
#define PM_GETSID (PM_BASE + 32)
#define PM_CLOCK_GETRES (PM_BASE + 33)
#define PM_CLOCK_GETTIME (PM_BASE + 34)
#define PM_CLOCK_SETTIME (PM_BASE + 35)
#define PM_GETRUSAGE (PM_BASE + 36)
#define PM_REBOOT (PM_BASE + 37)
#define PM_SVRCTL (PM_BASE + 38)
#define PM_SPROF (PM_BASE + 39)
#define PM_PROCEVENTMASK (PM_BASE + 40)
#define PM_SRV_FORK (PM_BASE + 41)
#define PM_SRV_KILL (PM_BASE + 42)
#define PM_EXEC_NEW (PM_BASE + 43)
#define PM_EXEC_RESTART (PM_BASE + 44)
#define PM_GETEPINFO (PM_BASE + 45)
#define PM_GETPROCNR (PM_BASE + 46)
#define PM_GETSYSINFO (PM_BASE + 47)

#define NR_PM_CALLS 48 /* highest number from base plus one */

/*===========================================================================*
 *				Calls to VFS				     *
 *===========================================================================*/

#define VFS_BASE 0x100

#define IS_VFS_CALL(type) (((type) & ~0xff) == VFS_BASE)

#define VFS_READ (VFS_BASE + 0)
#define VFS_WRITE (VFS_BASE + 1)
#define VFS_LSEEK (VFS_BASE + 2)
#define VFS_OPEN (VFS_BASE + 3)
#define VFS_CREAT (VFS_BASE + 4)
#define VFS_CLOSE (VFS_BASE + 5)
#define VFS_LINK (VFS_BASE + 6)
#define VFS_UNLINK (VFS_BASE + 7)
#define VFS_CHDIR (VFS_BASE + 8)
#define VFS_MKDIR (VFS_BASE + 9)
#define VFS_MKNOD (VFS_BASE + 10)
#define VFS_CHMOD (VFS_BASE + 11)
#define VFS_CHOWN (VFS_BASE + 12)
#define VFS_MOUNT (VFS_BASE + 13)
#define VFS_UMOUNT (VFS_BASE + 14)
#define VFS_ACCESS (VFS_BASE + 15)
#define VFS_SYNC (VFS_BASE + 16)
#define VFS_RENAME (VFS_BASE + 17)
#define VFS_RMDIR (VFS_BASE + 18)
#define VFS_SYMLINK (VFS_BASE + 19)
#define VFS_READLINK (VFS_BASE + 20)
#define VFS_STAT (VFS_BASE + 21)
#define VFS_FSTAT (VFS_BASE + 22)
#define VFS_LSTAT (VFS_BASE + 23)
#define VFS_IOCTL (VFS_BASE + 24)
#define VFS_FCNTL (VFS_BASE + 25)
#define VFS_PIPE2 (VFS_BASE + 26)
#define VFS_UMASK (VFS_BASE + 27)
#define VFS_CHROOT (VFS_BASE + 28)
#define VFS_GETDENTS (VFS_BASE + 29)
#define VFS_SELECT (VFS_BASE + 30)
#define VFS_FCHDIR (VFS_BASE + 31)
#define VFS_FSYNC (VFS_BASE + 32)
#define VFS_TRUNCATE (VFS_BASE + 33)
#define VFS_FTRUNCATE (VFS_BASE + 34)
#define VFS_FCHMOD (VFS_BASE + 35)
#define VFS_FCHOWN (VFS_BASE + 36)
#define VFS_UTIMENS (VFS_BASE + 37)
#define VFS_VMCALL (VFS_BASE + 38)
#define VFS_GETVFSSTAT (VFS_BASE + 39)
#define VFS_STATVFS1 (VFS_BASE + 40)
#define VFS_FSTATVFS1 (VFS_BASE + 41)
#define VFS_GETRUSAGE (VFS_BASE + 42) /* obsolete */
#define VFS_SVRCTL (VFS_BASE + 43)
#define VFS_GCOV_FLUSH (VFS_BASE + 44)
#define VFS_MAPDRIVER (VFS_BASE + 45)
#define VFS_COPYFD (VFS_BASE + 46)
#define VFS_SOCKETPATH (VFS_BASE + 47)
#define VFS_GETSYSINFO (VFS_BASE + 48)
#define VFS_SOCKET (VFS_BASE + 49)
#define VFS_SOCKETPAIR (VFS_BASE + 50)
#define VFS_BIND (VFS_BASE + 51)
#define VFS_CONNECT (VFS_BASE + 52)
#define VFS_LISTEN (VFS_BASE + 53)
#define VFS_ACCEPT (VFS_BASE + 54)
#define VFS_SENDTO (VFS_BASE + 55)
#define VFS_SENDMSG (VFS_BASE + 56)
#define VFS_RECVFROM (VFS_BASE + 57)
#define VFS_RECVMSG (VFS_BASE + 58)
#define VFS_SETSOCKOPT (VFS_BASE + 59)
#define VFS_GETSOCKOPT (VFS_BASE + 60)
#define VFS_GETSOCKNAME (VFS_BASE + 61)
#define VFS_GETPEERNAME (VFS_BASE + 62)
#define VFS_SHUTDOWN (VFS_BASE + 63)

#define NR_VFS_CALLS 64 /* highest number from base plus one */

/* Existing MINIX system calls - Assume PM and VFS calls contribute to a total.
   The feedback implies a flat numbering scheme starting from a generic
   NR_SYSCALLS. Let's define a starting point for general kernel system calls if
   not already present, or append to an existing general list. If PM_BASE is
   0x000 and VFS_BASE is 0x100, these are not contiguous. The feedback's
   NR_SYSCALLS = 128 seems like an arbitrary starting point for new calls if no
   such global counter exists. Let's use the provided numbers directly as they
   might be known to avoid collision or are part of a new scheme.
*/

/* Define a base for new system calls if the table is indeed flat from 0.
   The feedback uses (NR_SYSCALLS + X). Let's assume NR_SYSCALLS is the total
   count of *all* existing calls that should precede these new ones in a unified
   table. Given NR_PM_CALLS = 48 and NR_VFS_CALLS = 64, total ~112. The
   feedback's starting NR_SYSCALLS = 128 is a reasonable point after these.
*/

#define MINIX_NR_SYSCALLS                                                      \
  128 /* Base for new system calls as per feedback's initial value */

/* Mathematical POSIX system calls - placed after existing calls */
#define SYS_MATH_OPEN                                                          \
  (MINIX_NR_SYSCALLS + 0) // Adjusted to start from 0 for clarity with base
#define SYS_MATH_READ (MINIX_NR_SYSCALLS + 1)
#define SYS_MATH_WRITE (MINIX_NR_SYSCALLS + 2)
#define SYS_MATH_CLOSE (MINIX_NR_SYSCALLS + 3)
#define SYS_MATH_OPEN_CACHED                                                   \
  (MINIX_NR_SYSCALLS + 4) // Was +5, adjusted for 0-indexed from base
#define SYS_MATH_OPEN_BATCH (MINIX_NR_SYSCALLS + 5) // Was +6

/* Capability management system calls */
#define SYS_CAP_DERIVE (MINIX_NR_SYSCALLS + 6)
#define SYS_CAP_RESTRICT (MINIX_NR_SYSCALLS + 7)
#define SYS_CAP_REVOKE (MINIX_NR_SYSCALLS + 8)
#define SYS_CAP_DELEGATE (MINIX_NR_SYSCALLS + 9)

/* New system call for querying capability policy */
#define SYS_CAPABILITY_QUERY (MINIX_NR_SYSCALLS + 10)

/* Total number of system calls including new ones */
/* The new total will be MINIX_NR_SYSCALLS + 11.
 */
#define NR_SYSCALLS (MINIX_NR_SYSCALLS + 11)

#endif /* !_MINIX_CALLNR_H */
