#ifndef _SYS_SIGTYPES_H
#define _SYS_SIGTYPES_H


/*
 * The kernel does not need the full glibc signal definitions.  Provide a
 * simple sigset_t that keeps IPC message structures compact.  The system
 * headers would normally define a large structure for sigset_t which breaks
 * the compile-time size checks in <minix/ipc.h>.
 */
#ifndef __sigset_t_defined
#define __sigset_t_defined
typedef unsigned long sigset_t;
#endif

#ifndef _SIGSET_T_DECLARED
#define _SIGSET_T_DECLARED
#endif

#endif /* _SYS_SIGTYPES_H */
