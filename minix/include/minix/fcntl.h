// minix/include/minix/fcntl.h
#ifndef _MINIX_FCNTL_H
#define _MINIX_FCNTL_H

// Standard POSIX open flags (values may vary slightly by system, these are common)
#define O_RDONLY        00000000
#define O_WRONLY        00000001
#define O_RDWR          00000002
#define O_ACCMODE       (O_RDONLY|O_WRONLY|O_RDWR) // Mask for access mode

#define O_CREAT         00000100  /* creat file if it doesn't exist */
#define O_EXCL          00000200  /* error if O_CREAT and file exists */
#define O_NOCTTY        00000400  /* don't assign a controlling terminal */
#define O_TRUNC         00001000  /* truncate size to 0 */
#define O_APPEND        00002000  /* append to end of file */
#define O_NONBLOCK      00004000  /* non-blocking I/O */
#define O_SYNC          00010000  /* sync I/O file integrity */
#define O_DSYNC         O_SYNC    /* FreeBSD, often same as O_SYNC for data */
#define O_RSYNC         O_SYNC    /* FreeBSD, often same as O_SYNC for read */

// Other common flags (some might be Minix specific or from later POSIX versions)
#define O_CLOEXEC       02000000  /* close on exec */
#define O_DIRECTORY     00200000  /* fail if not a directory */
#define O_NOFOLLOW      00400000  /* don't follow symlinks */
#define O_PATH          010000000 /* open for metadata only */


/* Mathematical verification flags for open() - values from user feedback */
#define O_MATHEMATICAL          0x10000000  /* Request capability verification */
#define O_MATHEMATICAL_REQUIRED 0x20000000  /* Fail if verification unavailable */
#define O_CACHED_CAPABILITY     0x40000000  /* Internal: VFS uses a cached capability from libc */
                                             /* Ensure this doesn't clash with O_LARGEFILE or others if Minix uses them */

// Standard fcntl commands (for fcntl syscall itself, not open flags)
// F_DUPFD, F_GETFD, F_SETFD, F_GETFL, F_SETFL, etc.
// These are not strictly needed for O_MATHEMATICAL but make the header more complete.
#define F_DUPFD         0   /* Duplicate file descriptor */
#define F_GETFD         1   /* Get file descriptor flags */
#define F_SETFD         2   /* Set file descriptor flags */
#define F_GETFL         3   /* Get file status flags */
#define F_SETFL         4   /* Set file status flags */
// Add F_GETLK, F_SETLK, F_SETLKW for file locking if needed

// File descriptor flags for F_GETFD and F_SETFD.
#define FD_CLOEXEC      1   /* Close on exec. */

#endif /* _MINIX_FCNTL_H */
