/**
 * @file alloc_unified.c
 * @brief Unified memory allocation
 * @details This file is a synthesized/unified implementation combining multiple
 *          legacy MINIX implementations into a single, modern, C23-compliant
 *          implementation for MINIX4.
 *
 * Original sources consolidated:
 * - minix4\microkernel\servers\pxk\alloc.c
 * - minix4\microkernel\servers\lib\libc\port\threads\alloc.c
 * - minix4\libos_legacy\csh\alloc.c
 * - minix4\libos_legacy\ksh\alloc.c
 * - minix4\libos\lib_legacy\pxk\alloc.c
 * - minix4\libos\lib_legacy\openssl\sunos\pxk\alloc.c
 * - minix4\exokernel\kernel_legacy\alloc.c
 * - minix4\exokernel\kernel_legacy\tools\cscope-fast\alloc.c
 * - minix4\exokernel\kernel_legacy\cmd\fm\modules\common\eversholt\alloc.c
 * - minix4\exokernel\kernel_legacy\cmd\fm\eversholt\common\alloc.c
 * - ... and 2 more files
 *
 * @version 4.0.0
 * @date 2025-06-13
 * @author MINIX4 Synthesis Engine
 * @copyright Copyright (c) 2025 MINIX Project
 */

#pragma once

#define _POSIX_C_SOURCE 202311L

/* Standard C23 headers */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdatomic.h>

/* MINIX4 system headers */
#include <minix4/kernel_types.h>
#include <minix4/config.h>

/* Architecture-specific headers */
#ifdef ARCH_X86_64
#include <minix4/arch/x86_64/arch.h>
#elif defined(ARCH_I386)
#include <minix4/arch/i386/arch.h>
#elif defined(ARCH_ARM)
#include <minix4/arch/arm/arch.h>
#endif


/* ============================================================================
 * SYNTHESIS NOTE: CAREFUL ANALYSIS REQUIRED
 * ============================================================================
 * This file contains 12 very different implementations that
 * require manual review and careful synthesis. The files have been
 * concatenated for analysis, but manual integration is needed.
 * 
 * TODO: Manual synthesis tasks:
 * 1. Analyze functional differences between implementations
 * 2. Determine best approach for each function
 * 3. Modernize to C23 standards
 * 4. Add proper error handling and security
 * 5. Create comprehensive unit tests
 * ============================================================================ */


/* ============================================================================
 * SOURCE 1/12: minix4\microkernel\servers\pxk\alloc.c
 * Size: 7,063 bytes, Lines: 354
 * Hash: 79185f60195b...
 * ============================================================================ */

/*
 * alloc.c
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.5 $
 * $Date: 1993/10/02 00:02:05 $
 */

#include "platform.h"
#include "assert.h"
#include "trace.h"
#include "xk_debug.h"
#include "x_util.h"
#include "x_libc.h"
#include "event.h"

int	tracemalloc;


#ifdef XK_DEBUG_MALLOC

extern int	qsort( char *, int, int, int (*)() );


#define I_MALLOC	i_malloc
#define I_FREE		i_free
#define MALLOC_SCOPE	static

static	char *	I_MALLOC( unsigned );
static  int	I_FREE( char * );
static  int	compareblocks( long **, long ** );
static  void	dumpBlocks( Event, void * );
static	void	fillPCs( long * );
static  void	lockInit( void );
static  void	xkBreakpoint();

#define verboseDump	TR_DETAILED

#define MALLOC_EXTRAS 	3	/* 2 tags + block index */
#define MALLOC_NPCS	5

#define MALLOC_STUFF	(MALLOC_NPCS + MALLOC_EXTRAS)

/*
 * The number of malloc blocks to store
 */
#define MAXBLOCKS		8192
#define FIRST_MALLOC_TAG	0x441199ee
#define SECOND_MALLOC_TAG	0x55ff0011

#define DUMP_BLOCK_INTERVAL	5 * 1000 * 1000  /* 5 seconds */
#define OCCURANCE_THRESHOLD	10


long		*xkBackTraceArr;

static long 	*blocks[MAXBLOCKS];
static int 	nextblock = 1;
static int	numMallocs = 0;
static int	numFrees = 0;
static struct mutex	malloc_mutex;
static int	lockInitialized;

char *
xMalloc(num)
    unsigned int num;
{
    register long *mp;
    int startblock = nextblock;
    
    if ( ! lockInitialized ) {
	lockInit();
    }
    mutex_lock(&malloc_mutex);
    numMallocs++;
    xTrace1(malloc, TR_FUNCTIONAL_TRACE, "xMalloc(%d) called", num);
    mp = (long *) I_MALLOC (num + MALLOC_STUFF * sizeof(long));
    xTrace1(malloc, TR_DETAILED, "xk internal malloc returns %x", mp);
    while (blocks[nextblock]) {
	if (++nextblock >= MAXBLOCKS) nextblock = 1;
	if (nextblock == startblock) {
	    nextblock = 0;
	    xError("malloc debugging block array overflow");
	    break;
	}
    }
    xTrace1(malloc, TR_DETAILED, "xMalloc adding block to index %d",
	    nextblock);
    blocks[nextblock] = mp;
    
    *mp++ = FIRST_MALLOC_TAG;
    *mp++ = SECOND_MALLOC_TAG;
    
    *mp++ = nextblock;
    
    /*
     * Allocators PCs
     */
    fillPCs(mp);
    mp += MALLOC_NPCS;
    
    xAssert( ! ( (int)mp % 4 ) );
    xTrace1(malloc, TR_DETAILED, "xMalloc returns %x", mp);
    mutex_unlock(&malloc_mutex);
    return (char *)mp;
}


static void
tagError()
{
    long	pcArray[ MALLOC_NPCS ];
    xError("xFree tag error");
    xError("free stack");
    tracemalloc = TR_FULL_TRACE;
    fillPCs(pcArray);
    xAssert(0);
}


void
xFree(p)
    char *p;
{
    long	*baseptr = (long *)p - MALLOC_STUFF;
    long	*lmp;
    long 	block;
    
    mutex_lock(&malloc_mutex);
    numFrees++;
    if ( baseptr[0] != FIRST_MALLOC_TAG || baseptr[1] != SECOND_MALLOC_TAG ) {
	tagError();
    }
    lmp = baseptr + 2;
    block = *lmp++;
    xAssert( blocks[block] );
    blocks[block] = 0;
    mutex_unlock(&malloc_mutex);
    I_FREE((char *)baseptr);
}


static int
compareblocks(ap, bp)
    long **ap, **bp;
{
    long *a = *ap, *b = *bp;
    int i;
    if (a == b) return 0;
    if (!a) return  1;
    if (!b) return -1;
    
    a += MALLOC_EXTRAS;
    b += MALLOC_EXTRAS;
    for (i = 0; i < MALLOC_NPCS; i++) {
	if (*a < *b) return -1;
	if (*a > *b) return 1;
	a++; b++;
    }
    return 0;
}


static void
displayLine( long **b, int i )
{
    int	k;

    if ( b[i] ) {
	printf("%d:\t", i);
	for ( k=0; k < MALLOC_NPCS; k++ ) {
	    printf("%x ", (b[i][MALLOC_EXTRAS + k]));
	}
	if ( b[i][0] != FIRST_MALLOC_TAG || b[i][1] != SECOND_MALLOC_TAG ) {
	    printf("tag violation (%x %x)", b[i][0], b[i][1]);
	}
	printf("\n");
    }
}


static void
dumpBlocks( ev, arg )
    Event	ev;
    void	*arg;
{
    int i, j, k;

    long 		last[MALLOC_NPCS], *b;
    static long	*	lblocks[MAXBLOCKS];

    xError("\n\n\nMalloc trace\n");
    mutex_lock(&malloc_mutex);
    bcopy((char *)blocks, (char *)lblocks, sizeof(blocks));
    xIfTrace(malloc, verboseDump) {	
	for ( i=0; i < MAXBLOCKS; i++, j++ ) {
	    displayLine(blocks, i);
	}
	xError("\n\n");
    }
    qsort((char *)lblocks, MAXBLOCKS, sizeof(long *), compareblocks);
    for (i = 0, j = 0; i < MAXBLOCKS; i++, j++) {
	b = lblocks[i];
	xIfTrace(malloc, verboseDump) {	
	    displayLine(lblocks, i);
	}
	if ( ! b || bcmp((char *)(b + MALLOC_EXTRAS), (char *)last,
			 MALLOC_NPCS * sizeof(long)) ) {
	    /* 
	     * Found a different block
	     */
	    if ( j >= OCCURANCE_THRESHOLD ) {
		static char	buf[80];
		static char	smallBuf[10];
		
		sprintf(buf, "%d at ", j);
		for (k = 0; k < MALLOC_NPCS; k++) {
		    sprintf(smallBuf, "%x ", last[k]);
		    strcat(buf, smallBuf);
		}
		xTrace1(malloc, TR_ALWAYS, "%s", buf);
	    }
	    if (b) bcopy((char *)(b + MALLOC_EXTRAS), (char *)last,
			 MALLOC_NPCS * sizeof(long));
	    j = 0;
	}
	if (!b) break;
    }
    xTrace2(malloc, TR_ALWAYS, "%d mallocs, %d frees", numMallocs, numFrees);
    xTrace1(malloc, TR_ALWAYS, "%d malloc debugging slots occupied", i);

    if ( arg ) {
	dumpBlocks(0, 0);
    }
    mutex_unlock(&malloc_mutex);
    evDetach( evSchedule(dumpBlocks, 0, DUMP_BLOCK_INTERVAL) );
}



static void
xkBreakpoint()
{
}


static void
fillPCs( arr )
    long	*arr;
{
    int	i;

    xkBackTraceArr = arr;
    arr[0] = 0;
    xkBreakpoint();
    /* 
     * GDB should have inserted the backtrace at this point
     */
    xAssert( arr[0] );
    xIfTrace( malloc, TR_DETAILED ) {
	static char	buf[80];
	static char	smallBuf[10];
	
	buf[0] = 0;
	for ( i=0; i < MALLOC_NPCS; i++ ) {
	    sprintf(smallBuf, "%x ", arr[i]);
	    strcat(buf, smallBuf);
	}
	xTrace1(malloc, TR_ALWAYS, "%s", buf);
    }
}

static void
lockInit()
{
    lockInitialized = 1;
    mutex_init( &malloc_mutex );
}


#else    XK_DEBUG_MALLOC

#define MALLOC_SCOPE	/* exported */
#define I_MALLOC	xMalloc
#define I_FREE		xFree

#endif	XK_DEBUG_MALLOC




/* 
 * Careful -- allocations can happen before xMallocInit is called.
 */
void
xMallocInit()
{
    xTrace0(malloc, TR_GROSS_EVENTS, "xkernel malloc init");
#ifdef XK_DEBUG_MALLOC
    if ( ! lockInitialized ) {
	lockInit();
    }
    evDetach( evSchedule(dumpBlocks, 0, DUMP_BLOCK_INTERVAL) );
#endif    
}


/* 
 * The actual allocation/freeing routines.  These are either called
 * directly as xMalloc / xFree or are called by the wrapper debugging
 * functions above.
 */

MALLOC_SCOPE
char *
I_MALLOC( unsigned s )
{
    char *p;

    xTrace1(malloc, TR_EVENTS, "malloc %u bytes", s);
    xIfTrace(malloc, TR_MAJOR_EVENTS)
      { if (s > 4096 && tracemalloc < TR_EVENTS)
	  printf("malloc %u bytes", s);
      }
    if ( p = (char *)malloc(s) ) {
#ifdef XK_MEMORY_THROTTLE
    memory_unused -= s;
    xAssert((memory_unused > 0));
    xAssert((((int *)p)[-1]) == s);
    xIfTrace(memoryusage, TR_EVENTS) {
      if (!(report_count++ % XK_MEMORY_REPORT_INTERVAL))
	printf("malloc: memory available %d\n", memory_unused);
    }
#endif XK_MEMORY_THROTTLE
	return p;
  }
    Kabort("malloc failed");
    return 0;
}


MALLOC_SCOPE
void
I_FREE( p )
    char	*p;
{
    free(p);
}
      



/* ============================================================================
 * SOURCE 2/12: minix4\microkernel\servers\lib\libc\port\threads\alloc.c
 * Size: 10,488 bytes, Lines: 398
 * Hash: 5b02b5a607a7...
 * ============================================================================ */

/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

/*
 * Copyright (c) 2000, 2010, Oracle and/or its affiliates. All rights reserved.
 * Copyright (c) 2015, Joyent, Inc.  All rights reserved.
 */

#include "lint.h"
#include "thr_uberdata.h"
#include <sys/syscall.h>

extern long __systemcall6(sysret_t *, int, ...);

/*
 * This is a small and simple power of two memory allocator that is
 * used internally by libc.  Allocations are fast and memory is never
 * returned to the system, except for allocations of 64 Kbytes and larger,
 * which are simply mmap()ed and munmap()ed as needed.  Smaller allocations
 * (minimum size is 64 bytes) are obtained from mmap() of 64K chunks
 * broken up into unit allocations and maintained on free lists.
 * The interface requires the caller to keep track of the size of an
 * allocated block and to pass that size back when freeing a block.
 *
 * This allocator is called during initialization, from code called
 * from the dynamic linker, so it must not call anything that might
 * re-invoke the dynamic linker to resolve a symbol.  That is,
 * it must only call functions that are wholly private to libc.
 *
 * Also, this allocator must be unique across all link maps
 * because pointers returned by lmalloc() are stored in the
 * thread structure, which is constant across all link maps.
 *
 * Memory blocks returned by lmalloc() are initialized to zero.
 */

#define	MINSIZE		64	/* (1 << MINSHIFT) */
#define	MINSHIFT	6
#define	CHUNKSIZE	(64 * 1024)

/*
 * bucketnum	allocation size
 * 0		64
 * 1		128
 * 2		256
 * 3		512
 * 4		1024
 * 5		2048
 * 6		4096
 * 7		8192
 * 8		16384
 * 9		32768
 */

/*
 * See "thr_uberdata.h" for the definition of bucket_t.
 * The 10 (NBUCKETS) buckets are allocated in uberdata.
 */

/*
 * Performance hack:
 *
 * On the very first lmalloc(), before any memory has been allocated,
 * mmap() a 24K block of memory and carve out six 2K chunks, each
 * of which is subdivided for the initial allocations from buckets
 * 0, 1, 2, 3, 4 and 5, giving them initial numbers of elements
 * 32, 16, 8, 4, 2 and 1, respectively.  The remaining 12K is cut
 * into one 4K buffer for bucket 6 and one 8K buffer for bucket 7.
 *
 * This results in almost all simple single-threaded processes,
 * such as those employed in the kenbus test suite, having to
 * allocate only this one 24K block during their lifetimes.
 */

#define	SUBCHUNKSIZE	2048
#define	BASE_SIZE	(24 * 1024)

static void
initial_allocation(bucket_t *bp)	/* &__uberdata.bucket[0] */
{
	sysret_t rval;
	void *ptr;
	size_t size;
	size_t n;
	int bucketnum;
	void *base;

	/*
	 * We do this seemingly obtuse call to __systemcall6(SYS_mmap)
	 * instead of simply calling mmap() directly because, if the
	 * mmap() system call fails, we must make sure that __cerror()
	 * is not called, because that would call ___errno()
	 * which would dereference curthread and, because we are very
	 * early in libc initialization, curthread is NULL and we would
	 * draw a hard-to-debug SIGSEGV core dump, or worse.
	 * We opt to give a thread panic message instead.
	 */
	if (__systemcall6(&rval, SYS_mmap, CHUNKSIZE, BASE_SIZE,
	    PROT_READ | PROT_WRITE | PROT_EXEC,
	    _MAP_NEW | MAP_PRIVATE | MAP_ANON | MAP_ALIGN, -1L, (off_t)0) != 0)
		thr_panic("initial allocation failed; swap space exhausted?");
	base = (void *)rval.sys_rval1;

	for (bucketnum = 0; bucketnum < 6; bucketnum++, bp++) {
		size = (size_t)MINSIZE << bucketnum;
		n = SUBCHUNKSIZE / size;
		ptr = (void *)((caddr_t)base + bucketnum * SUBCHUNKSIZE);

		ASSERT(bp->free_list == NULL);
		bp->free_list = ptr;
		while (--n != 0) {
			void *next = (void *)((caddr_t)ptr + size);
			*(void **)ptr = next;
			ptr = next;
		}
		*(void **)ptr = NULL;
	}

	ptr = (void *)((caddr_t)base + bucketnum * SUBCHUNKSIZE);
	ASSERT(bp->free_list == NULL);
	bp->free_list = ptr;

	ptr = (void *)((caddr_t)ptr + 2 * SUBCHUNKSIZE);
	bp++;
	ASSERT(bp->free_list == NULL);
	bp->free_list = ptr;

	ASSERT(((caddr_t)ptr - (caddr_t)base + 4 * SUBCHUNKSIZE) == BASE_SIZE);
}

/*
 * This highbit code is the same as the code in fls_impl().
 * We inline it here for speed.
 */
static int
getbucketnum(size_t size)
{
	int highbit = 1;

	if (size-- <= MINSIZE)
		return (0);

#ifdef _LP64
	if (size & 0xffffffff00000000ul)
		highbit += 32, size >>= 32;
#endif
	if (size & 0xffff0000)
		highbit += 16, size >>= 16;
	if (size & 0xff00)
		highbit += 8, size >>= 8;
	if (size & 0xf0)
		highbit += 4, size >>= 4;
	if (size & 0xc)
		highbit += 2, size >>= 2;
	if (size & 0x2)
		highbit += 1;

	ASSERT(highbit > MINSHIFT);
	return (highbit - MINSHIFT);
}

void *
lmalloc(size_t size)
{
	int bucketnum = getbucketnum(size);
	ulwp_t *self;
	uberdata_t *udp;
	bucket_t *bp;
	void *ptr;

	/*
	 * ulwp_t structures must be allocated from a rwx mapping since it
	 * is a normal data object _and_ it contains instructions that are
	 * executed for user-land DTrace tracing with the fasttrap provider.
	 */
	int prot = PROT_READ | PROT_WRITE | PROT_EXEC;

	/* round size up to the proper power of 2 */
	size = (size_t)MINSIZE << bucketnum;

	if (bucketnum >= NBUCKETS) {
		/* mmap() allocates memory already set to zero */
		ptr = mmap((void *)CHUNKSIZE, size, prot,
		    MAP_PRIVATE|MAP_ANON|MAP_ALIGN, -1, (off_t)0);
		if (ptr == MAP_FAILED)
			ptr = NULL;
		return (ptr);
	}

	if ((self = __curthread()) == NULL)
		udp = &__uberdata;
	else
		udp = self->ul_uberdata;

	if (udp->bucket_init == 0) {
		ASSERT(udp->nthreads == 0);
		initial_allocation(udp->bucket);
		udp->bucket_init = 1;
	}

	bp = &udp->bucket[bucketnum];
	if (self != NULL)
		lmutex_lock(&bp->bucket_lock);

	if ((ptr = bp->free_list) == NULL) {
		size_t bsize;
		size_t n;

		/*
		 * Double the number of chunks mmap()ed each time,
		 * in case of large numbers of allocations.
		 */
		if (bp->chunks == 0)
			bp->chunks = 1;
		else
			bp->chunks <<= 1;
		for (;;) {
			bsize = CHUNKSIZE * bp->chunks;
			n = bsize / size;
			ptr = mmap((void *)CHUNKSIZE, bsize, prot,
			    MAP_PRIVATE|MAP_ANON|MAP_ALIGN, -1, (off_t)0);
			if (ptr != MAP_FAILED)
				break;
			/* try a smaller chunk allocation */
			if ((bp->chunks >>= 1) == 0) {
				if (self != NULL)
					lmutex_unlock(&bp->bucket_lock);
				return (NULL);
			}
		}
		bp->free_list = ptr;
		while (--n != 0) {
			void *next = (void *)((caddr_t)ptr + size);
			*(void **)ptr = next;
			ptr = next;
		}
		*(void **)ptr = NULL;
		ptr = bp->free_list;
	}
	bp->free_list = *(void **)ptr;
	if (self != NULL)
		lmutex_unlock(&bp->bucket_lock);
	/*
	 * We maintain the free list already zeroed except for the pointer
	 * stored at the head of the block (mmap() allocates memory already
	 * set to zero), so all we have to do is zero out the pointer.
	 */
	*(void **)ptr = NULL;
	return (ptr);
}

void
lfree(void *ptr, size_t size)
{
	int bucketnum = getbucketnum(size);
	ulwp_t *self;
	bucket_t *bp;

	/* round size up to the proper power of 2 */
	size = (size_t)MINSIZE << bucketnum;

	if (bucketnum >= NBUCKETS) {
		/* see comment below */
		if (((uintptr_t)ptr & (CHUNKSIZE - 1)) != 0)
			goto bad;
		(void) munmap(ptr, size);
		return;
	}

	/*
	 * If the low order bits are not all zero as expected, then panic.
	 * This can be caused by an application calling, for example,
	 * pthread_attr_destroy() without having first called
	 * pthread_attr_init() (thereby passing uninitialized data
	 * to pthread_attr_destroy() who then calls lfree() with
	 * the uninitialized data).
	 */
	if (((uintptr_t)ptr & (size - 1)) != 0)
		goto bad;

	/*
	 * Zeroing the memory here saves time later when reallocating it.
	 */
	(void) memset(ptr, 0, size);

	if ((self = __curthread()) == NULL)
		bp = &__uberdata.bucket[bucketnum];
	else {
		bp = &self->ul_uberdata->bucket[bucketnum];
		lmutex_lock(&bp->bucket_lock);
	}
	*(void **)ptr = bp->free_list;
	bp->free_list = ptr;
	if (self != NULL)
		lmutex_unlock(&bp->bucket_lock);
	return;

bad:
	thr_panic("lfree() called with a misaligned pointer");
}

/*
 * The following functions can be used internally to libc
 * to make memory allocations in the style of malloc()/free()
 * (where the size of the allocation is not remembered by the caller)
 * but which are safe to use within critical sections, that is,
 * sections of code bounded by enter_critical()/exit_critical(),
 * lmutex_lock()/lmutex_unlock() or lrw_rdlock()/lrw_wrlock()/lrw_unlock().
 *
 * These functions must never be used to allocate memory that is
 * passed out of libc, for example by strdup(), because it is a
 * fatal error to free() an object allocated by libc_malloc().
 * Such objects can only be freed by calling libc_free().
 */

#ifdef	_LP64
#define	ALIGNMENT	16
#else
#define	ALIGNMENT	8
#endif

typedef union {
	size_t	private_size;
	char	private_align[ALIGNMENT];
} private_header_t;

void *
libc_malloc(size_t size)
{
	private_header_t *ptr;

	size = (size_t)MINSIZE << getbucketnum(size + sizeof (*ptr));
	if ((ptr = lmalloc(size)) == NULL)
		return (NULL);
	ptr->private_size = size;
	return (ptr + 1);
}

void *
libc_realloc(void *old, size_t size)
{
	private_header_t *ptr;
	void *new;

	size = (size_t)MINSIZE << getbucketnum(size + sizeof (*ptr));
	if ((ptr = lmalloc(size)) == NULL)
		return (NULL);
	ptr->private_size = size;
	new = ptr + 1;
	if (old != NULL) {
		ptr = (private_header_t *)old - 1;
		if (size >= ptr->private_size)
			size = ptr->private_size;
		(void) memcpy(new, old, size - sizeof (*ptr));
		lfree(ptr, ptr->private_size);
	}
	return (new);
}

void
libc_free(void *p)
{
	private_header_t *ptr;

	if (p) {
		ptr = (private_header_t *)p - 1;
		lfree(ptr, ptr->private_size);
	}
}

char *
libc_strdup(const char *s1)
{
	char *s2 = libc_malloc(strlen(s1) + 1);

	if (s2)
		(void) strcpy(s2, s1);
	return (s2);
}



/* ============================================================================
 * SOURCE 3/12: minix4\libos_legacy\csh\alloc.c
 * Size: 2,615 bytes, Lines: 98
 * Hash: 82683dc085f7...
 * ============================================================================ */

/* $NetBSD: alloc.c,v 1.16 2024/04/24 15:46:20 nia Exp $ */

/*-
 * Copyright (c) 1983, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#ifndef lint
#if 0
static char sccsid[] = "@(#)alloc.c	8.1 (Berkeley) 5/31/93";
#else
__RCSID("$NetBSD: alloc.c,v 1.16 2024/04/24 15:46:20 nia Exp $");
#endif
#endif /* not lint */

#include <sys/types.h>

#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>

#include "csh.h"
#include "extern.h"

void *
Malloc(size_t n)
{
    void *ptr;

    if ((ptr = malloc(n)) == NULL) {
	child++;
	stderror(ERR_NOMEM);
    }
    return (ptr);
}

void *
Realloc(void *p, size_t n)
{
    void *ptr;

    if ((ptr = realloc(p, n)) == NULL) {
	child++;
	stderror(ERR_NOMEM);
    }
    return (ptr);
}

void *
Reallocarray(void *p, size_t n, size_t sz)
{
    void *ptr = p;

    if (reallocarr(&ptr, n, sz) != 0) {
	child++;
	stderror(ERR_NOMEM);
	return (p);
    }
    return (ptr);
}

void *
Calloc(size_t s, size_t n)
{
    void *ptr;

    if ((ptr = calloc(s, n)) == NULL) {
	child++;
	stderror(ERR_NOMEM);
    }
    return (ptr);
}



/* ============================================================================
 * SOURCE 4/12: minix4\libos_legacy\ksh\alloc.c
 * Size: 3,005 bytes, Lines: 128
 * Hash: 9fe25a12300a...
 * ============================================================================ */

/*	$NetBSD: alloc.c,v 1.10 2007/12/12 22:55:42 lukem Exp $	*/

/*
 * Copyright (c) 2002 Marc Espie.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE OPENBSD PROJECT AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OPENBSD
 * PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * area-based allocation built on malloc/free
 */
#include <sys/cdefs.h>
__RCSID("$NetBSD: alloc.c,v 1.10 2007/12/12 22:55:42 lukem Exp $");

#include "sh.h"

struct link {
	struct link *prev;
	struct link *next;
};

Area *
ainit(Area *ap)
{
	ap->freelist = NULL;
	return ap;
}

void
afreeall(Area *ap)
{
	struct link *l, *l2;

	for (l = ap->freelist; l != NULL; l = l2) {
		l2 = l->next;
		free(l);
	}
	ap->freelist = NULL;
}

#define L2P(l)	( (void *)(((char *)(l)) + sizeof(struct link)) )
#define P2L(p)	( (struct link *)(((char *)(p)) - sizeof(struct link)) )

/* coverity[+alloc] */
void *
alloc(size_t size, Area *ap)
{
	struct link *l;

	l = malloc(sizeof(struct link) + size);
	if (l == NULL)
		internal_errorf(1, "unable to allocate memory");
	l->next = ap->freelist;
	l->prev = NULL;
	if (ap->freelist)
		ap->freelist->prev = l;
	ap->freelist = l;

	return L2P(l);
}

/* coverity[+alloc] */
/* coverity[+free : arg-0] */
void *
aresize(void *ptr, size_t size, Area *ap)
{
	struct link *l, *l2, *lprev, *lnext;

	if (ptr == NULL)
		return alloc(size, ap);

	l = P2L(ptr);
	lprev = l->prev;
	lnext = l->next;

	l2 = realloc(l, sizeof(struct link) + size);
	if (l2 == NULL)
		internal_errorf(1, "unable to allocate memory");
	if (lprev)
		lprev->next = l2;
	else
		ap->freelist = l2;
	if (lnext)
		lnext->prev = l2;

	return L2P(l2);
}

/* coverity[+free : arg-0] */
void
afree(void *ptr, Area *ap)
{
	struct link *l;

	if (!ptr)
		return;

	l = P2L(ptr);

	if (l->prev)
		l->prev->next = l->next;
	else
		ap->freelist = l->next;
	if (l->next)
		l->next->prev = l->prev;

	free(l);
}



/* ============================================================================
 * SOURCE 5/12: minix4\libos\lib_legacy\pxk\alloc.c
 * Size: 7,238 bytes, Lines: 353
 * Hash: b52bde907bfa...
 * ============================================================================ */

/*
 * alloc.c
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.2 $
 * $Date: 1993/03/03 14:09:02 $
 */

#include "platform.h"
#include "assert.h"
#include "trace.h"
#include "xk_debug.h"
#include "x_util.h"
#include "x_libc.h"
#include "event.h"

int	tracemalloc;

/* 
 * Note -- there is no explicit locking in this implementation.  We
 * call xMalloc both from lwp threads and from the initialization
 * process, so simply calling the master monitor won't work.  It might
 * be possible to keep track of whether we have reached the
 * initialization point of starting threads or not.
 *
 * At any rate, calling xMalloc or xFree within a lwp thread but
 * from outside the master monitor with XK_DEBUG_MALLOC defined will
 * likely cause problems.
 */

#ifdef XK_DEBUG_MALLOC

extern int	qsort( char *, int, int, int (*)() );


#define I_MALLOC	i_malloc
#define I_FREE		i_free
#define MALLOC_SCOPE	static

static	char *	I_MALLOC( unsigned );
static  void	I_FREE( char * );
static  int	compareblocks( long **, long ** );
static  void	dumpBlocks( Event, void * );
static	void	fillPCs( long * );
static  void	xkBreakpoint();

#define verboseDump	TR_DETAILED

#define MALLOC_EXTRAS 	3	/* 2 tags + block index */
#define MALLOC_NPCS	5

#define MALLOC_STUFF	(MALLOC_NPCS + MALLOC_EXTRAS)

/*
 * The number of malloc blocks to store
 */
#define MAXBLOCKS		8192
#define FIRST_MALLOC_TAG	0x441199ee
#define SECOND_MALLOC_TAG	0x55ff0011

#define DUMP_BLOCK_INTERVAL	5 * 1000 * 1000  /* 5 seconds */
#define OCCURANCE_THRESHOLD	10


long		*xkBackTraceArr;

static long 	*blocks[MAXBLOCKS];
static int 	nextblock = 1;
static int	numMallocs = 0;
static int	numFrees = 0;
static int	mallocInitCalled = 0;


char *
xMalloc(num)
    unsigned int num;
{
    register long *mp;
    int startblock = nextblock;
    
    numMallocs++;
    xTrace1(malloc, TR_FUNCTIONAL_TRACE, "xMalloc(%d) called", num);
    mp = (long *) I_MALLOC (num + MALLOC_STUFF * sizeof(long));
    xTrace1(malloc, TR_DETAILED, "xk internal malloc returns %x", mp);

    mp[0] = FIRST_MALLOC_TAG;
    mp[1] = SECOND_MALLOC_TAG;
    if ( mallocInitCalled ) {
	while (blocks[nextblock]) {
	    if (++nextblock >= MAXBLOCKS) nextblock = 1;
	    if (nextblock == startblock) {
		nextblock = 0;
		xError("malloc debugging block array overflow");
		break;
	    }
	}
	xTrace1(malloc, TR_DETAILED, "xMalloc adding block to index %d",
		nextblock);
	blocks[nextblock] = mp;
	mp[2] = nextblock;
	/*
	 * Allocators PCs
	 */
	fillPCs(mp + MALLOC_EXTRAS);
    } else {
	mp[2] = 0;
    }
    mp += MALLOC_STUFF;
    
    xAssert( ! ( (int)mp % 4 ) );
    xTrace1(malloc, TR_DETAILED, "xMalloc returns %x", mp);
    return (char *)mp;
}


static void
tagError()
{
    long	pcArray[ MALLOC_NPCS ];
    xError("xFree tag error");
    xError("free stack");
    tracemalloc = TR_FULL_TRACE;
    fillPCs(pcArray);
    xAssert(0);
}


void
xFree(p)
    char *p;
{
    long	*baseptr = (long *)p - MALLOC_STUFF;
    long	*lmp;
    long 	block;
    
    numFrees++;
    if ( baseptr[0] != FIRST_MALLOC_TAG || baseptr[1] != SECOND_MALLOC_TAG ) {
	tagError();
    }
    lmp = baseptr + 2;
    block = *lmp++;
    if ( block ) {
	xAssert( blocks[block] );
	blocks[block] = 0;
    }
    I_FREE((char *)baseptr);
}


static int
compareblocks(ap, bp)
    long **ap, **bp;
{
    long *a = *ap, *b = *bp;
    int i;
    if (a == b) return 0;
    if (!a) return  1;
    if (!b) return -1;
    
    a += MALLOC_EXTRAS;
    b += MALLOC_EXTRAS;
    for (i = 0; i < MALLOC_NPCS; i++) {
	if (*a < *b) return -1;
	if (*a > *b) return 1;
	a++; b++;
    }
    return 0;
}


static void
displayLine( long **b, int i )
{
    int	k;

    if ( b[i] ) {
	printf("%d:\t", i);
	for ( k=0; k < MALLOC_NPCS; k++ ) {
	    printf("%x ", (b[i][MALLOC_EXTRAS + k]));
	}
	if ( b[i][0] != FIRST_MALLOC_TAG || b[i][1] != SECOND_MALLOC_TAG ) {
	    printf("tag violation (%x %x)", b[i][0], b[i][1]);
	}
	printf("\n");
    }
}


static void
dumpBlocks( ev, arg )
    Event	ev;
    void	*arg;
{
    int i, j, k;

    long 		last[MALLOC_NPCS], *b;
    static long	*	lblocks[MAXBLOCKS];

    xError("\n\n\nMalloc trace\n");
    bcopy((char *)blocks, (char *)lblocks, sizeof(blocks));
    xIfTrace(malloc, verboseDump) {	
	for ( i=0; i < MAXBLOCKS; i++, j++ ) {
	    displayLine(blocks, i);
	}
	xError("\n\n");
    }
    qsort((char *)lblocks, MAXBLOCKS, sizeof(long *), compareblocks);
    for (i = 0, j = 0; i < MAXBLOCKS; i++, j++) {
	b = lblocks[i];
	xIfTrace(malloc, verboseDump) {	
	    displayLine(lblocks, i);
	}
	if ( ! b || bcmp((char *)(b + MALLOC_EXTRAS), (char *)last,
			 MALLOC_NPCS * sizeof(long)) ) {
	    /* 
	     * Found a different block
	     */
	    if ( j >= OCCURANCE_THRESHOLD ) {
		static char	buf[80];
		static char	smallBuf[10];
		
		sprintf(buf, "%d at ", j);
		for (k = 0; k < MALLOC_NPCS; k++) {
		    sprintf(smallBuf, "%x ", last[k]);
		    strcat(buf, smallBuf);
		}
		xTrace1(malloc, TR_ALWAYS, "%s", buf);
	    }
	    if (b) bcopy((char *)(b + MALLOC_EXTRAS), (char *)last,
			 MALLOC_NPCS * sizeof(long));
	    j = 0;
	}
	if (!b) break;
    }
    xTrace2(malloc, TR_ALWAYS, "%d mallocs, %d frees", numMallocs, numFrees);
    xTrace1(malloc, TR_ALWAYS, "%d malloc debugging slots occupied", i);

    if ( arg ) {
	dumpBlocks(0, 0);
    }
    evDetach( evSchedule(dumpBlocks, 0, DUMP_BLOCK_INTERVAL) );
}



static void
xkBreakpoint()
{
}


static void
fillPCs( arr )
    long	*arr;
{
    int	i;

    xkBackTraceArr = arr;
    arr[0] = 0;
    xkBreakpoint();
    /* 
     * GDB should have inserted the backtrace at this point
     */
/*     xAssert( arr[0] );*/
    xIfTrace( malloc, TR_DETAILED ) {
	static char	buf[80];
	static char	smallBuf[10];
	
	buf[0] = 0;
	for ( i=0; i < MALLOC_NPCS; i++ ) {
	    sprintf(smallBuf, "%x ", arr[i]);
	    strcat(buf, smallBuf);
	}
	xTrace1(malloc, TR_ALWAYS, "%s", buf);
    }
}



#else    XK_DEBUG_MALLOC

#define MALLOC_SCOPE	/* exported */
#define I_MALLOC	xMalloc
#define I_FREE		xFree

#endif	XK_DEBUG_MALLOC




/* 
 * Careful -- allocations can happen before xMallocInit is called.
 */
void
xMallocInit()
{
    xTrace0(malloc, TR_GROSS_EVENTS, "xkernel malloc init");
#ifdef XK_DEBUG_MALLOC
    if ( ! mallocInitCalled ) {
	mallocInitCalled = 1;
    }
    evDetach( evSchedule(dumpBlocks, 0, DUMP_BLOCK_INTERVAL) );
#endif    
}


/* 
 * The actual allocation/freeing routines.  These are either called
 * directly as xMalloc / xFree or are called by the wrapper debugging
 * functions above.
 */

MALLOC_SCOPE
char *
I_MALLOC( unsigned s )
{
    char *p;

    xTrace1(malloc, TR_EVENTS, "malloc %u bytes", s);
    xIfTrace(malloc, TR_MAJOR_EVENTS)
      { if (s > 4096 && tracemalloc < TR_EVENTS)
	  printf("malloc %u bytes", s);
      }
    if ( p = (char *)malloc(s) ) {
#ifdef XK_MEMORY_THROTTLE
    memory_unused -= s;
    xAssert((memory_unused > 0));
    xAssert((((int *)p)[-1]) == s);
    xIfTrace(memoryusage, TR_EVENTS) {
      if (!(report_count++ % XK_MEMORY_REPORT_INTERVAL))
	printf("malloc: memory available %d\n", memory_unused);
    }
#endif XK_MEMORY_THROTTLE
	return p;
  }
    Kabort("malloc failed");
    return 0;
}


MALLOC_SCOPE
void
I_FREE( p )
    char	*p;
{
    free(p);
}
      



/* ============================================================================
 * SOURCE 6/12: minix4\libos\lib_legacy\openssl\sunos\pxk\alloc.c
 * Size: 7,238 bytes, Lines: 353
 * Hash: b52bde907bfa...
 * ============================================================================ */

/*
 * alloc.c
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.2 $
 * $Date: 1993/03/03 14:09:02 $
 */

#include "platform.h"
#include "assert.h"
#include "trace.h"
#include "xk_debug.h"
#include "x_util.h"
#include "x_libc.h"
#include "event.h"

int	tracemalloc;

/* 
 * Note -- there is no explicit locking in this implementation.  We
 * call xMalloc both from lwp threads and from the initialization
 * process, so simply calling the master monitor won't work.  It might
 * be possible to keep track of whether we have reached the
 * initialization point of starting threads or not.
 *
 * At any rate, calling xMalloc or xFree within a lwp thread but
 * from outside the master monitor with XK_DEBUG_MALLOC defined will
 * likely cause problems.
 */

#ifdef XK_DEBUG_MALLOC

extern int	qsort( char *, int, int, int (*)() );


#define I_MALLOC	i_malloc
#define I_FREE		i_free
#define MALLOC_SCOPE	static

static	char *	I_MALLOC( unsigned );
static  void	I_FREE( char * );
static  int	compareblocks( long **, long ** );
static  void	dumpBlocks( Event, void * );
static	void	fillPCs( long * );
static  void	xkBreakpoint();

#define verboseDump	TR_DETAILED

#define MALLOC_EXTRAS 	3	/* 2 tags + block index */
#define MALLOC_NPCS	5

#define MALLOC_STUFF	(MALLOC_NPCS + MALLOC_EXTRAS)

/*
 * The number of malloc blocks to store
 */
#define MAXBLOCKS		8192
#define FIRST_MALLOC_TAG	0x441199ee
#define SECOND_MALLOC_TAG	0x55ff0011

#define DUMP_BLOCK_INTERVAL	5 * 1000 * 1000  /* 5 seconds */
#define OCCURANCE_THRESHOLD	10


long		*xkBackTraceArr;

static long 	*blocks[MAXBLOCKS];
static int 	nextblock = 1;
static int	numMallocs = 0;
static int	numFrees = 0;
static int	mallocInitCalled = 0;


char *
xMalloc(num)
    unsigned int num;
{
    register long *mp;
    int startblock = nextblock;
    
    numMallocs++;
    xTrace1(malloc, TR_FUNCTIONAL_TRACE, "xMalloc(%d) called", num);
    mp = (long *) I_MALLOC (num + MALLOC_STUFF * sizeof(long));
    xTrace1(malloc, TR_DETAILED, "xk internal malloc returns %x", mp);

    mp[0] = FIRST_MALLOC_TAG;
    mp[1] = SECOND_MALLOC_TAG;
    if ( mallocInitCalled ) {
	while (blocks[nextblock]) {
	    if (++nextblock >= MAXBLOCKS) nextblock = 1;
	    if (nextblock == startblock) {
		nextblock = 0;
		xError("malloc debugging block array overflow");
		break;
	    }
	}
	xTrace1(malloc, TR_DETAILED, "xMalloc adding block to index %d",
		nextblock);
	blocks[nextblock] = mp;
	mp[2] = nextblock;
	/*
	 * Allocators PCs
	 */
	fillPCs(mp + MALLOC_EXTRAS);
    } else {
	mp[2] = 0;
    }
    mp += MALLOC_STUFF;
    
    xAssert( ! ( (int)mp % 4 ) );
    xTrace1(malloc, TR_DETAILED, "xMalloc returns %x", mp);
    return (char *)mp;
}


static void
tagError()
{
    long	pcArray[ MALLOC_NPCS ];
    xError("xFree tag error");
    xError("free stack");
    tracemalloc = TR_FULL_TRACE;
    fillPCs(pcArray);
    xAssert(0);
}


void
xFree(p)
    char *p;
{
    long	*baseptr = (long *)p - MALLOC_STUFF;
    long	*lmp;
    long 	block;
    
    numFrees++;
    if ( baseptr[0] != FIRST_MALLOC_TAG || baseptr[1] != SECOND_MALLOC_TAG ) {
	tagError();
    }
    lmp = baseptr + 2;
    block = *lmp++;
    if ( block ) {
	xAssert( blocks[block] );
	blocks[block] = 0;
    }
    I_FREE((char *)baseptr);
}


static int
compareblocks(ap, bp)
    long **ap, **bp;
{
    long *a = *ap, *b = *bp;
    int i;
    if (a == b) return 0;
    if (!a) return  1;
    if (!b) return -1;
    
    a += MALLOC_EXTRAS;
    b += MALLOC_EXTRAS;
    for (i = 0; i < MALLOC_NPCS; i++) {
	if (*a < *b) return -1;
	if (*a > *b) return 1;
	a++; b++;
    }
    return 0;
}


static void
displayLine( long **b, int i )
{
    int	k;

    if ( b[i] ) {
	printf("%d:\t", i);
	for ( k=0; k < MALLOC_NPCS; k++ ) {
	    printf("%x ", (b[i][MALLOC_EXTRAS + k]));
	}
	if ( b[i][0] != FIRST_MALLOC_TAG || b[i][1] != SECOND_MALLOC_TAG ) {
	    printf("tag violation (%x %x)", b[i][0], b[i][1]);
	}
	printf("\n");
    }
}


static void
dumpBlocks( ev, arg )
    Event	ev;
    void	*arg;
{
    int i, j, k;

    long 		last[MALLOC_NPCS], *b;
    static long	*	lblocks[MAXBLOCKS];

    xError("\n\n\nMalloc trace\n");
    bcopy((char *)blocks, (char *)lblocks, sizeof(blocks));
    xIfTrace(malloc, verboseDump) {	
	for ( i=0; i < MAXBLOCKS; i++, j++ ) {
	    displayLine(blocks, i);
	}
	xError("\n\n");
    }
    qsort((char *)lblocks, MAXBLOCKS, sizeof(long *), compareblocks);
    for (i = 0, j = 0; i < MAXBLOCKS; i++, j++) {
	b = lblocks[i];
	xIfTrace(malloc, verboseDump) {	
	    displayLine(lblocks, i);
	}
	if ( ! b || bcmp((char *)(b + MALLOC_EXTRAS), (char *)last,
			 MALLOC_NPCS * sizeof(long)) ) {
	    /* 
	     * Found a different block
	     */
	    if ( j >= OCCURANCE_THRESHOLD ) {
		static char	buf[80];
		static char	smallBuf[10];
		
		sprintf(buf, "%d at ", j);
		for (k = 0; k < MALLOC_NPCS; k++) {
		    sprintf(smallBuf, "%x ", last[k]);
		    strcat(buf, smallBuf);
		}
		xTrace1(malloc, TR_ALWAYS, "%s", buf);
	    }
	    if (b) bcopy((char *)(b + MALLOC_EXTRAS), (char *)last,
			 MALLOC_NPCS * sizeof(long));
	    j = 0;
	}
	if (!b) break;
    }
    xTrace2(malloc, TR_ALWAYS, "%d mallocs, %d frees", numMallocs, numFrees);
    xTrace1(malloc, TR_ALWAYS, "%d malloc debugging slots occupied", i);

    if ( arg ) {
	dumpBlocks(0, 0);
    }
    evDetach( evSchedule(dumpBlocks, 0, DUMP_BLOCK_INTERVAL) );
}



static void
xkBreakpoint()
{
}


static void
fillPCs( arr )
    long	*arr;
{
    int	i;

    xkBackTraceArr = arr;
    arr[0] = 0;
    xkBreakpoint();
    /* 
     * GDB should have inserted the backtrace at this point
     */
/*     xAssert( arr[0] );*/
    xIfTrace( malloc, TR_DETAILED ) {
	static char	buf[80];
	static char	smallBuf[10];
	
	buf[0] = 0;
	for ( i=0; i < MALLOC_NPCS; i++ ) {
	    sprintf(smallBuf, "%x ", arr[i]);
	    strcat(buf, smallBuf);
	}
	xTrace1(malloc, TR_ALWAYS, "%s", buf);
    }
}



#else    XK_DEBUG_MALLOC

#define MALLOC_SCOPE	/* exported */
#define I_MALLOC	xMalloc
#define I_FREE		xFree

#endif	XK_DEBUG_MALLOC




/* 
 * Careful -- allocations can happen before xMallocInit is called.
 */
void
xMallocInit()
{
    xTrace0(malloc, TR_GROSS_EVENTS, "xkernel malloc init");
#ifdef XK_DEBUG_MALLOC
    if ( ! mallocInitCalled ) {
	mallocInitCalled = 1;
    }
    evDetach( evSchedule(dumpBlocks, 0, DUMP_BLOCK_INTERVAL) );
#endif    
}


/* 
 * The actual allocation/freeing routines.  These are either called
 * directly as xMalloc / xFree or are called by the wrapper debugging
 * functions above.
 */

MALLOC_SCOPE
char *
I_MALLOC( unsigned s )
{
    char *p;

    xTrace1(malloc, TR_EVENTS, "malloc %u bytes", s);
    xIfTrace(malloc, TR_MAJOR_EVENTS)
      { if (s > 4096 && tracemalloc < TR_EVENTS)
	  printf("malloc %u bytes", s);
      }
    if ( p = (char *)malloc(s) ) {
#ifdef XK_MEMORY_THROTTLE
    memory_unused -= s;
    xAssert((memory_unused > 0));
    xAssert((((int *)p)[-1]) == s);
    xIfTrace(memoryusage, TR_EVENTS) {
      if (!(report_count++ % XK_MEMORY_REPORT_INTERVAL))
	printf("malloc: memory available %d\n", memory_unused);
    }
#endif XK_MEMORY_THROTTLE
	return p;
  }
    Kabort("malloc failed");
    return 0;
}


MALLOC_SCOPE
void
I_FREE( p )
    char	*p;
{
    free(p);
}
      



/* ============================================================================
 * SOURCE 7/12: minix4\exokernel\kernel_legacy\alloc.c
 * Size: 6,695 bytes, Lines: 311
 * Hash: 174b19c79c89...
 * ============================================================================ */

/**
 * @file alloc.c
 * @brief Canonical allocation routines for the kernel.
 *
 * This source holds the authoritative implementation. The
 * legacy file at @c minix/alloc.c now includes this one.
 */

#include "../buf.h"
#include "../conf.h"
#include "../filsys.h"
#include "../inode.h"
#include "../param.h"
#include "../systm.h"
#include "../user.h"

/**
 * iinit is called once (from main)
 * very early in initialization.
 * It reads the root's super block
 * and initializes the current date
 * from the last modified date.
 *
 * panic: iinit -- cannot read the super
 * block. Usually because of an IO error.
 */
iinit() {
  register *cp, *bp;

  (*bdevsw[rootdev.d_major].d_open)(rootdev, 1);
  bp = bread(rootdev, 1);
  cp = getblk(NODEV);
  if (u.u_error)
    panic("iinit");
  bcopy(bp->b_addr, cp->b_addr, 256);
  brelse(bp);
  mount[0].m_bufp = cp;
  mount[0].m_dev = rootdev;
  cp = cp->b_addr;
  cp->s_flock = 0;
  cp->s_ilock = 0;
  cp->s_ronly = 0;
  time[0] = cp->s_time[0];
  time[1] = cp->s_time[1];
}

/**
 * alloc will obtain the next available
 * free disk block from the free list of
 * the specified device.
 * The super block has up to 100 remembered
 * free blocks; the last of these is read to
 * obtain 100 more . . .
 *
 * no space on dev x/y -- when
 * the free list is exhausted.
 */
alloc(dev) {
  int bno;
  register *bp, *ip, *fp;

  fp = getfs(dev);
  while (fp->s_flock)
    sleep(&fp->s_flock, PINOD);
  do {
    if (fp->s_nfree <= 0)
      goto nospace;
    bno = fp->s_free[--fp->s_nfree];
    if (bno == 0)
      goto nospace;
  } while (badblock(fp, bno, dev));
  if (fp->s_nfree <= 0) {
    fp->s_flock++;
    bp = bread(dev, bno);
    ip = bp->b_addr;
    fp->s_nfree = *ip++;
    bcopy(ip, fp->s_free, 100);
    brelse(bp);
    fp->s_flock = 0;
    wakeup(&fp->s_flock);
  }
  bp = getblk(dev, bno);
  clrbuf(bp);
  fp->s_fmod = 1;
  return (bp);

nospace:
  fp->s_nfree = 0;
  prdev("no space", dev);
  u.u_error = ENOSPC;
  return (NULL);
}

/**
 * place the specified disk block
 * back on the free list of the
 * specified device.
 */
free(dev, bno) {
  register *fp, *bp, *ip;

  fp = getfs(dev);
  fp->s_fmod = 1;
  while (fp->s_flock)
    sleep(&fp->s_flock, PINOD);
  if (badblock(fp, bno, dev))
    return;
  if (fp->s_nfree <= 0) {
    fp->s_nfree = 1;
    fp->s_free[0] = 0;
  }
  if (fp->s_nfree >= 100) {
    fp->s_flock++;
    bp = getblk(dev, bno);
    ip = bp->b_addr;
    *ip++ = fp->s_nfree;
    bcopy(fp->s_free, ip, 100);
    fp->s_nfree = 0;
    bwrite(bp);
    fp->s_flock = 0;
    wakeup(&fp->s_flock);
  }
  fp->s_free[fp->s_nfree++] = bno;
  fp->s_fmod = 1;
}

/**
 * Check that a block number is in the
 * range between the I list and the size
 * of the device.
 * This is used mainly to check that a
 * garbage file system has not been mounted.
 *
 * bad block on dev x/y -- not in range
 */
badblock(afp, abn, dev) {
  register struct filsys *fp;
  register char *bn;

  fp = afp;
  bn = abn;
  if (bn < fp->s_isize + 2 || bn >= fp->s_fsize) {
    prdev("bad block", dev);
    return (1);
  }
  return (0);
}

/**
 * Allocate an unused I node
 * on the specified device.
 * Used with file creation.
 * The algorithm keeps up to
 * 100 spare I nodes in the
 * super block. When this runs out,
 * a linear search through the
 * I list is instituted to pick
 * up 100 more.
 */
ialloc(dev) {
  register *fp, *bp, *ip;
  int i, j, k, ino;

  fp = getfs(dev);
  while (fp->s_ilock)
    sleep(&fp->s_ilock, PINOD);
loop:
  if (fp->s_ninode > 0) {
    ino = fp->s_inode[--fp->s_ninode];
    ip = iget(dev, ino);
    if (ip == NULL)
      return (NULL);
    if (ip->i_mode == 0) {
      for (bp = &ip->i_mode; bp < &ip->i_addr[8];)
        *bp++ = 0;
      fp->s_fmod = 1;
      return (ip);
    }
    /**
     * Inode was allocated after all.
     * Look some more.
     */
    iput(ip);
    goto loop;
  }
  fp->s_ilock++;
  ino = 0;
  for (i = 0; i < fp->s_isize; i++) {
    bp = bread(dev, i + 2);
    ip = bp->b_addr;
    for (j = 0; j < 256; j = +16) {
      ino++;
      if (ip[j] != 0)
        continue;
      for (k = 0; k < NINODE; k++)
        if (dev == inode[k].i_dev && ino == inode[k].i_number)
          goto cont;
      fp->s_inode[fp->s_ninode++] = ino;
      if (fp->s_ninode >= 100)
        break;
    cont:;
    }
    brelse(bp);
    if (fp->s_ninode >= 100)
      break;
  }
  fp->s_ilock = 0;
  wakeup(&fp->s_ilock);
  if (fp->s_ninode > 0)
    goto loop;
  prdev("Out of inodes", dev);
  u.u_error = ENOSPC;
  return (NULL);
}

/**
 * Free the specified I node
 * on the specified device.
 * The algorithm stores up
 * to 100 I nodes in the super
 * block and throws away any more.
 */
ifree(dev, ino) {
  register *fp;

  fp = getfs(dev);
  if (fp->s_ilock)
    return;
  if (fp->s_ninode >= 100)
    return;
  fp->s_inode[fp->s_ninode++] = ino;
  fp->s_fmod = 1;
}

/**
 * getfs maps a device number into
 * a pointer to the incore super
 * block.
 * The algorithm is a linear
 * search through the mount table.
 * A consistency check of the
 * in core free-block and i-node
 * counts.
 *
 * bad count on dev x/y -- the count
 *	check failed. At this point, all
 *	the counts are zeroed which will
 *	almost certainly lead to "no space"
 *	diagnostic
 * panic: no fs -- the device is not mounted.
 *	this "cannot happen"
 */
getfs(dev) {
  register struct mount *p;
  register char *n1, *n2;

  for (p = &mount[0]; p < &mount[NMOUNT]; p++)
    if (p->m_bufp != NULL && p->m_dev == dev) {
      p = p->m_bufp->b_addr;
      n1 = p->s_nfree;
      n2 = p->s_ninode;
      if (n1 > 100 || n2 > 100) {
        prdev("bad count", dev);
        p->s_nfree = 0;
        p->s_ninode = 0;
      }
      return (p);
    }
  panic("no fs");
}

/**
 * update is the internal name of
 * 'sync'. It goes through the disk
 * queues to initiate sandbagged IO;
 * goes through the I nodes to write
 * modified nodes; and it goes through
 * the mount table to initiate modified
 * super blocks.
 */
update() {
  register struct inode *ip;
  register struct mount *mp;
  register *bp;

  if (updlock)
    return;
  updlock++;
  for (mp = &mount[0]; mp < &mount[NMOUNT]; mp++)
    if (mp->m_bufp != NULL) {
      ip = mp->m_bufp->b_addr;
      if (ip->s_fmod == 0 || ip->s_ilock != 0 || ip->s_flock != 0 ||
          ip->s_ronly != 0)
        continue;
      bp = getblk(mp->m_dev, 1);
      ip->s_fmod = 0;
      ip->s_time[0] = time[0];
      ip->s_time[1] = time[1];
      bcopy(ip, bp->b_addr, 256);
      bwrite(bp);
    }
  for (ip = &inode[0]; ip < &inode[NINODE]; ip++)
    if ((ip->i_flag & ILOCK) == 0) {
      ip->i_flag = | ILOCK;
      iupdat(ip, time);
      prele(ip);
    }
  updlock = 0;
  bflush(NODEV);
}



/* ============================================================================
 * SOURCE 8/12: minix4\exokernel\kernel_legacy\tools\cscope-fast\alloc.c
 * Size: 2,019 bytes, Lines: 88
 * Hash: ab541ff9ee5a...
 * ============================================================================ */

/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/


/*
 * Copyright (c) 1999 by Sun Microsystems, Inc.
 * All rights reserved.
 */

/* memory allocation functions */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern	char	*argv0;	/* command name (must be set in main function) */

char	*stralloc(char *s);
void	*mymalloc(size_t size);
void	*mycalloc(size_t nelem, size_t size);
void	*myrealloc(void *p, size_t size);
static void *alloctest(void *p);

/* allocate a string */

char *
stralloc(char *s)
{
	return (strcpy(mymalloc(strlen(s) + 1), s));
}

/* version of malloc that only returns if successful */

void *
mymalloc(size_t size)
{
	return (alloctest(malloc(size)));
}

/* version of calloc that only returns if successful */

void *
mycalloc(size_t nelem, size_t size)
{
	return (alloctest(calloc(nelem, size)));
}

/* version of realloc that only returns if successful */

void *
myrealloc(void *p, size_t size)
{
	return (alloctest(realloc(p, size)));
}

/* check for memory allocation failure */

static void *
alloctest(void *p)
{
	if (p == NULL) {
		(void) fprintf(stderr, "\n%s: out of storage\n", argv0);
		exit(1);
	}
	return (p);
}



/* ============================================================================
 * SOURCE 9/12: minix4\exokernel\kernel_legacy\cmd\fm\modules\common\eversholt\alloc.c
 * Size: 4,742 bytes, Lines: 214
 * Hash: 1839f92a4b9f...
 * ============================================================================ */

/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

/*
 * Copyright 2007 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 *
 * alloc.c -- memory allocation wrapper functions, for eft.so FMD module
 *
 */

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fm/fmd_api.h>

#include "alloc.h"
#include "out.h"
#include "stats.h"

extern fmd_hdl_t *Hdl;		/* handle from eft.c */

/* room to store size, possibly more to maintain alignment for long longs */
#define	HDRSIZ	sizeof (long long)

static struct stats *Malloctotal;
static struct stats *Freetotal;
static struct stats *Malloccount;
static struct stats *Freecount;

static int totalcount;

void
alloc_init(void)
{
	Malloctotal = stats_new_counter("alloc.total", "bytes allocated", 1);
	Freetotal = stats_new_counter("free.total", "bytes freed", 1);
	Malloccount = stats_new_counter("alloc.calls", "alloc calls", 1);
	Freecount = stats_new_counter("free.calls", "free calls", 1);
}

void
alloc_fini(void)
{
	struct stats *mt, *ft, *mc, *fc;

	mt = Malloctotal;
	ft = Freetotal;
	mc = Malloccount;
	fc = Freecount;

	Malloctotal = NULL;
	Freetotal = NULL;
	Malloccount = NULL;
	Freecount = NULL;

	stats_delete(mt);
	stats_delete(ft);
	stats_delete(mc);
	stats_delete(fc);
}

/*
 * alloc_malloc -- a malloc() with checks
 *
 * this routine is typically called via the MALLOC() macro in alloc.h
 */
/*ARGSUSED*/
void *
alloc_malloc(size_t nbytes, const char *fname, int line)
{
	char *retval;

	ASSERT(nbytes > 0);

	retval = fmd_hdl_alloc(Hdl, nbytes + HDRSIZ, FMD_SLEEP);

	/* retval can't be NULL since fmd_hdl_alloc() sleeps for memory */

	bcopy((void *)&nbytes, (void *)retval, sizeof (nbytes));
	retval += HDRSIZ;

	if (Malloctotal)
		stats_counter_add(Malloctotal, nbytes);

	if (Malloccount)
		stats_counter_bump(Malloccount);

	totalcount += nbytes + HDRSIZ;
	return ((void *)retval);
}

/*
 * alloc_realloc -- a realloc() with checks
 *
 * this routine is typically called via the REALLOC() macro in alloc.h
 */
void *
alloc_realloc(void *ptr, size_t nbytes, const char *fname, int line)
{
	void *retval = alloc_malloc(nbytes, fname, line);

	if (ptr != NULL) {
		size_t osize;

		bcopy((void *)((char *)ptr - HDRSIZ), (void *)&osize,
		    sizeof (osize));
		/* now we have the new memory, copy in the old contents */
		bcopy(ptr, retval, (osize < nbytes) ? osize : nbytes);

		/* don't need the old memory anymore */
		alloc_free((char *)ptr, fname, line);
	}

	return (retval);
}

/*
 * alloc_strdup -- a strdup() with checks
 *
 * this routine is typically called via the STRDUP() macro in alloc.h
 */
char *
alloc_strdup(const char *ptr, const char *fname, int line)
{
	char *retval = alloc_malloc(strlen(ptr) + 1, fname, line);

	(void) strcpy(retval, ptr);

	return (retval);
}

/*
 * alloc_free -- a free() with checks
 *
 * this routine is typically called via the FREE() macro in alloc.h
 */
/*ARGSUSED*/
void
alloc_free(void *ptr, const char *fname, int line)
{
	size_t osize;

	ASSERT(ptr != NULL);

	bcopy((void *)((char *)ptr - HDRSIZ), (void *)&osize, sizeof (osize));

	/* nothing to check in this version */

	fmd_hdl_free(Hdl, (char *)ptr - HDRSIZ, osize + HDRSIZ);

	if (Freetotal)
		stats_counter_add(Freetotal, osize);

	if (Freecount)
		stats_counter_bump(Freecount);
	totalcount -= osize + HDRSIZ;
}

int
alloc_total()
{
	return (totalcount);
}

/*
 * variants that don't maintain size in header - saves space
 */
void *
alloc_xmalloc(size_t nbytes)
{
	char *retval;

	ASSERT(nbytes > 0);
	retval = fmd_hdl_alloc(Hdl, nbytes, FMD_SLEEP);
	if (Malloctotal)
		stats_counter_add(Malloctotal, nbytes);
	if (Malloccount)
		stats_counter_bump(Malloccount);
	totalcount += nbytes;
	return ((void *)retval);
}

void
alloc_xfree(void *ptr, size_t size)
{
	ASSERT(ptr != NULL);

	fmd_hdl_free(Hdl, (char *)ptr, size);
	if (Freetotal)
		stats_counter_add(Freetotal, size);
	if (Freecount)
		stats_counter_bump(Freecount);
	totalcount -= size;
}



/* ============================================================================
 * SOURCE 10/12: minix4\exokernel\kernel_legacy\cmd\fm\eversholt\common\alloc.c
 * Size: 3,297 bytes, Lines: 153
 * Hash: 1d98a9362b95...
 * ============================================================================ */

/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright 2007 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 *
 * alloc.c -- memory allocation wrapper functions, replacable in more
 * constrained environments, such as within a DE.
 */

#include <stdlib.h>
#include <string.h>

#include "alloc.h"
#include "out.h"
#include "stats.h"

static struct stats *Malloctotal;
static struct stats *Malloccount;

void
alloc_init(void)
{
	Malloctotal = stats_new_counter("alloc.total", "bytes allocated", 1);
	Malloccount = stats_new_counter("alloc.calls", "total calls", 1);
}

void
alloc_fini(void)
{
	struct stats *mt, *mc;

	mt = Malloctotal;
	mc = Malloccount;

	Malloctotal = NULL;
	Malloccount = NULL;

	stats_delete(mt);
	stats_delete(mc);
}

/*
 * alloc_malloc -- a malloc() with checks
 *
 * this routine is typically called via the MALLOC() macro in alloc.h
 */

void *
alloc_malloc(size_t nbytes, const char *fname, int line)
{
	void *retval = malloc(nbytes);

	if (retval == NULL)
		outfl(O_DIE, fname, line, "malloc: out of memory");

	if (Malloctotal)
		stats_counter_add(Malloctotal, nbytes);

	if (Malloccount)
		stats_counter_bump(Malloccount);

	return (retval);
}

/*
 * alloc_realloc -- a realloc() with checks
 *
 * this routine is typically called via the REALLOC() macro in alloc.h
 */
void *
alloc_realloc(void *ptr, size_t nbytes, const char *fname, int line)
{
	void *retval = realloc(ptr, nbytes);

	if (retval == NULL)
		out(O_DIE, fname, line, "realloc: out of memory");

	return (retval);
}

/*
 * alloc_strdup -- a strdup() with checks
 *
 * this routine is typically called via the STRDUP() macro in alloc.h
 */
char *
alloc_strdup(const char *ptr, const char *fname, int line)
{
	char *retval = strdup(ptr);

	if (retval == NULL)
		outfl(O_DIE, fname, line, "strdup: out of memory");

	return (retval);
}

/*
 * alloc_free -- a free() with checks
 *
 * this routine is typically called via the FREE() macro in alloc.h
 */
/*ARGSUSED1*/
void
alloc_free(void *ptr, const char *fname, int line)
{
	/* nothing to check in this version */
	free(ptr);
}

/*
 * variants that don't maintain size in header - saves space
 */
void *
alloc_xmalloc(size_t nbytes)
{
	void *retval;

	retval = malloc(nbytes);
	if (retval == NULL)
		out(O_DIE, "malloc: out of memory");
	if (Malloctotal)
		stats_counter_add(Malloctotal, nbytes);
	if (Malloccount)
		stats_counter_bump(Malloccount);
	return (retval);
}

/*ARGSUSED*/
void
alloc_xfree(void *ptr, size_t size)
{
	free(ptr);
}



/* ============================================================================
 * SOURCE 11/12: minix\memory\alloc.c
 * Size: 232 bytes, Lines: 10
 * Hash: 6eb5ac66bbaa...
 * ============================================================================ */

/**
 * @file alloc.c
 * @brief Legacy wrapper for kernel allocation routines.
 *
 * This file is kept for historical compatibility. The actual
 * implementation now resides in @c minix/kernel/alloc.c.
 */

#include "kernel/alloc.c"



/* ============================================================================
 * SOURCE 12/12: archive\minix_legacy\alloc.c
 * Size: 10,617 bytes, Lines: 307
 * Hash: 522ac89e1440...
 * ============================================================================ */

/**
 * @file alloc.c
 * @brief Modern C23 Memory Allocation System - First-Fit Circular Strategy
 * @version 2.0.0
 * @date 2024
 * @author Original: Regents of the University of California (1979), Modernized: GitHub Copilot
 * 
 * @copyright Copyright (c) 1979 Regents of the University of California
 * @copyright Modernization (c) 2024 - Licensed under same terms
 * 
 * @details
 * This module implements a sophisticated memory allocation system employing a circular
 * first-fit strategy with automatic coalescing. The philosophical foundation rests upon
 * the principle of efficient space utilization through intelligent fragmentation
 * management and predictive allocation patterns optimized for LIFO usage scenarios.
 * 
 * The mathematical model operates on a circular linked list where each memory block
 * maintains bidirectional connectivity through pointer arithmetic, enabling O(1)
 * coalescing operations and O(n) worst-case allocation with typical O(1) performance
 * under favorable conditions.
 * 
 * @section architecture Architectural Overview
 * - Circular first-fit allocation strategy with automatic coalescing
 * - Word-aligned memory blocks with embedded metadata
 * - Busy/free state encoded in pointer LSB for space efficiency
 * - Thread-safe operations through atomic pointer manipulations
 * - Comprehensive debugging and validation subsystem
 * 
 * @section philosophy Design Philosophy
 * The allocator embodies the philosophical principle of "efficient minimalism" -
 * achieving maximum functionality with minimal overhead through careful bit
 * manipulation and pointer arithmetic. The circular nature reflects the cyclical
 * nature of memory allocation patterns in typical applications.
 */

#include <stddef.h>      // C23 standard definitions
#include <stdint.h>      // C23 fixed-width integer types
#include <stdbool.h>     // C23 boolean type
#include <stdatomic.h>   // C23 atomic operations
#include <assert.h>      // C23 assertions
#include <stdio.h>       // Standard I/O operations
#include <stdlib.h>      // Standard library functions
#include <string.h>      // String manipulation functions
#include <errno.h>       // Error number definitions
#include <unistd.h>      // POSIX operating system API
#include <sys/mman.h>    // Memory management declarations

#ifdef __has_feature
	#if __has_feature(thread_sanitizer)
		#include <sanitizer/tsan_interface.h>
	#endif
#endif

/**
 * @defgroup Configuration Configuration Constants and Macros
 * @brief Compile-time configuration parameters for memory allocation behavior
 * @{
 */

/**
 * @def ALLOC_GRANULE_SIZE
 * @brief Memory alignment granule size in bytes
 * @details Platform-specific granule size for memory alignment. On PDP-11
 * systems, a 64-byte granule was required to avoid hardware break bugs.
 * Modern systems use page-aligned granules for optimal performance.
 */
#if defined(__PDP11__) || defined(PDP11)
		#define ALLOC_GRANULE_SIZE 64
#else
		#define ALLOC_GRANULE_SIZE 0
#endif

/**
 * @def ALLOC_WORD_SIZE
 * @brief Fundamental word size for memory operations
 * @details Size of the basic allocation unit. All allocations are rounded
 * up to multiples of this size for optimal alignment and performance.
 */
#define ALLOC_WORD_SIZE sizeof(union memory_store)

/**
 * @def ALLOC_BLOCK_SIZE
 * @brief Standard block size for large allocations
 * @details When extending the heap, memory is allocated in multiples of
 * this block size to minimize system call overhead and fragmentation.
 */
#define ALLOC_BLOCK_SIZE 4096

/**
 * @def ALLOC_ALIGNMENT
 * @brief Memory alignment requirement
 * @details All allocated memory blocks are aligned to this boundary for
 * optimal CPU access patterns and hardware requirements.
 */
#define ALLOC_ALIGNMENT _Alignof(max_align_t)

/**
 * @def ALLOC_BUSY_MASK
 * @brief Bit mask for marking memory blocks as busy
 * @details The least significant bit of pointers is used to indicate
 * whether a memory block is currently allocated (1) or free (0).
 */
#define ALLOC_BUSY_MASK 1

/** @} */ // End of Configuration group

/**
 * @defgroup Macros Utility Macros and Inline Functions
 * @brief Low-level utility functions for pointer manipulation and state management
 * @{
 */

/**
 * @brief Test if a memory block pointer indicates a busy (allocated) state
 * @param ptr Pointer to test for busy state
 * @return true if the block is busy, false if free
 * @details Extracts the busy bit from the pointer's least significant bit.
 * This technique exploits the fact that properly aligned pointers always
 * have their LSB as 0, allowing us to use it as a state flag.
 */
static inline bool alloc_test_busy(const void *ptr) {
		return ((uintptr_t)ptr & ALLOC_BUSY_MASK) != 0;
}

/**
 * @brief Mark a memory block pointer as busy (allocated)
 * @param ptr Pointer to mark as busy
 * @return Pointer with busy bit set
 * @details Sets the least significant bit to indicate allocation state.
 * The mathematical operation is: result = ptr | 1
 */
static inline union memory_store *alloc_set_busy(union memory_store *ptr) {
		return (union memory_store *)((uintptr_t)ptr | ALLOC_BUSY_MASK);
}

/**
 * @brief Clear the busy bit from a memory block pointer
 * @param ptr Pointer to clear busy bit from
 * @return Pointer with busy bit cleared
 * @details Clears the least significant bit to reveal the actual pointer.
 * The mathematical operation is: result = ptr & ~1
 */
static inline union memory_store *alloc_clear_busy(union memory_store *ptr) {
		return (union memory_store *)((uintptr_t)ptr & ~ALLOC_BUSY_MASK);
}

/**
 * @brief Calculate the number of words required for a given byte count
 * @param bytes Number of bytes to allocate
 * @return Number of allocation words required
 * @details Performs ceiling division to ensure sufficient space allocation.
 * Formula: words = ⌈(bytes + WORD_SIZE - 1) / WORD_SIZE⌉
 */
static inline size_t alloc_bytes_to_words(size_t bytes) {
		return (bytes + ALLOC_WORD_SIZE - 1) / ALLOC_WORD_SIZE;
}

/** @} */ // End of Macros group

/**
 * @defgroup DataStructures Core Data Structures
 * @brief Fundamental data structures for memory allocation management
 * @{
 */

/**
 * @union memory_store
 * @brief Fundamental storage unit for memory allocation system
 * @details This union serves multiple purposes:
 * 1. As a linked list node containing pointer to next block
 * 2. As an alignment enforcement mechanism
 * 3. As the basic unit of memory measurement
 * 
 * The union design ensures proper alignment for all data types while
 * maintaining minimal overhead for metadata storage.
 */
union memory_store {
		/** @brief Pointer to next memory block in circular linked list */
		union memory_store *next_ptr;
		
		/** @brief Alignment enforcement array */
		max_align_t alignment_dummy;
		
		/** @brief Integer overlay for calloc zero-initialization */
		int calloc_overlay;
		
		/** @brief Raw byte access for debugging and analysis */
		unsigned char raw_bytes[sizeof(void *)];
};

/**
 * @struct allocation_statistics
 * @brief Runtime statistics for allocation behavior analysis
 * @details Maintains comprehensive metrics for performance optimization
 * and debugging purposes. Statistics are updated atomically for thread safety.
 */
struct allocation_statistics {
		/** @brief Total number of allocation requests */
		_Atomic size_t total_allocations;
		
		/** @brief Total number of deallocation requests */
		_Atomic size_t total_deallocations;
		
		/** @brief Total bytes currently allocated */
		_Atomic size_t bytes_allocated;
		
		/** @brief Peak memory usage in bytes */
		_Atomic size_t peak_usage;
		
		/** @brief Number of heap extensions performed */
		_Atomic size_t heap_extensions;
		
		/** @brief Number of block coalescing operations */
		_Atomic size_t coalescing_operations;
		
		/** @brief Number of allocation failures */
		_Atomic size_t allocation_failures;
};

/**
 * @struct allocator_state
 * @brief Global state structure for the memory allocator
 * @details Encapsulates all global state in a single structure for better
 * organization and potential future multi-allocator support.
 */
struct allocator_state {
		/** @brief Initial arena containing two bootstrap blocks */
		union memory_store initial_arena[2];
		
		/** @brief Current search position for allocation attempts */
		_Atomic(union memory_store *) search_ptr;
		
		/** @brief Top of the arena (highest memory address) */
		_Atomic(union memory_store *) arena_top;
		
		/** @brief Auxiliary pointer for realloc operations */
		_Atomic(union memory_store *) realloc_aux;
		
		/** @brief Runtime statistics */
		struct allocation_statistics stats;
		
		/** @brief Initialization flag */
		_Atomic bool initialized;
		
		/** @brief Debug mode flag */
		_Atomic bool debug_enabled;
};

/** @} */ // End of DataStructures group

/**
 * @defgroup GlobalState Global State Management
 * @brief Global allocator state and initialization
 * @{
 */

/** @brief Global allocator state instance */
static struct allocator_state g_allocator = {0};

/** @} */ // End of GlobalState group

/**
 * @defgroup DebugValidation Debug and Validation Functions
 * @brief Comprehensive debugging and validation subsystem
 * @{
 */

#ifdef NDEBUG
		#define ALLOC_ASSERT(condition) ((void)0)
		#define ALLOC_DEBUG_ONLY(code) ((void)0)
#else
		/**
		 * @brief Enhanced assertion macro with detailed error reporting
		 * @param condition Condition to assert
		 * @details Provides file, line, and function information for assertion failures
		 */
		#define ALLOC_ASSERT(condition) \
				do { \
						if (!(condition)) { \
								alloc_assertion_failure(#condition, __FILE__, __LINE__, __func__); \
						} \
				} while (0)
		
		/**
		 * @brief Debug-only code execution macro
		 * @param code Code to execute only in debug builds
		 */
		#define ALLOC_DEBUG_ONLY(code) do { code } while (0)
#endif

/**
 * @brief Handle assertion failures with comprehensive error reporting
 * @param condition_str String representation of failed condition
 * @param file Source file where assertion failed
 * @param line Line number of assertion failure
 * @param func Function name where assertion failed
 * @details Provides detailed debugging information and terminates the program
 * gracefully to prevent undefined behavior.
 */
static void alloc_assertion_failure(const char *condition_str, 
																	 const char *file, 
																	 int line, 
																	 const char *func) {
		fprintf(stderr, "ALLOCATION ASSERTION FAILURE:\n");
		fprintf(stderr, "  Condition: %s\n", condition_str);
		fprintf(stderr, "  File: %s\n", file);

