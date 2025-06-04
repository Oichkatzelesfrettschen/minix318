#ifndef _MINIX_SYS_CDEFS_H
#define _MINIX_SYS_CDEFS_H

/* Include the system definitions first to obtain the standard macros. */
#include_next <sys/cdefs.h>

/* Provide minimal fallbacks when building the kernel. */
#ifndef __dead
#define __dead __attribute__((__noreturn__))
#endif
#ifndef __unused
#define __unused __attribute__((unused))
#endif
#ifndef __aligned
#define __aligned(x) __attribute__((aligned(x)))
#endif

#endif /* _MINIX_SYS_CDEFS_H */
