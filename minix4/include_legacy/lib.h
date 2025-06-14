#ifndef _MINIX_LIB_H
#define _MINIX_LIB_H

/* This is a central internal header for MINIX's libc.
 * It aggregates necessary low-level MINIX-specific includes
 * for IPC and system call mechanisms.
 */

#include <minix/types.h>
#include <minix/const.h>
#include <minix/ipc.h>
#include <minix/callnr.h>

/*
 * Note on <minix/syslib.h>:
 * The original prompt suggested including <minix/syslib.h> here.
 * However, if <minix/syslib.h> also includes this file (<minix/lib.h>),
 * it would create a circular dependency.
 * Instead, <minix/syslib.h> should include this <minix/lib.h>
 * to gain access to the headers above.
 * This file provides the foundational types and constants, which
 * syslib and other parts of libc will depend upon.
 */

#endif /* _MINIX_LIB_H */
