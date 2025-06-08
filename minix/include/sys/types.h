#ifndef _MINIX_SYS_TYPES_H
#define _MINIX_SYS_TYPES_H

/*
 * Provide a minimal sigset_t before pulling in the system headers.  This
 * prevents glibc from defining its much larger version which causes our IPC
 * message size assertions to fail during compilation.
 */
#ifndef __sigset_t_defined
#define __sigset_t_defined
typedef unsigned long __sigset_t;
typedef __sigset_t sigset_t;
#endif

#ifndef _SIGSET_T_DECLARED
#define _SIGSET_T_DECLARED
#endif

/* Pull in the system definitions next. */
#include_next <sys/types.h>

/* MINIX specific extensions used by the kernel. */
typedef unsigned int devmajor_t;
typedef unsigned int devminor_t;
typedef int key_t;

#endif /* _MINIX_SYS_TYPES_H */
