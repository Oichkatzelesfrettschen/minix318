/**
 * @file malloc_unified.c
 * @brief Unified malloc implementation
 * @details This file is a synthesized/unified implementation combining multiple
 *          legacy MINIX implementations into a single, modern, C23-compliant
 *          implementation for MINIX4.
 *
 * Original sources consolidated:
 * - minix4\microkernel\servers\pxk\malloc.c
 * - minix4\microkernel\servers\netiso\xebec\malloc.c
 * - minix4\microkernel\servers\lib\libc\port\gen\malloc.c
 * - minix4\libos\posix\libc\stdlib\malloc.c
 * - minix4\libos\lib_legacy\watchmalloc\common\malloc.c
 * - minix4\libos\lib_legacy\openssl\lib\posix\netiso\xebec\malloc.c
 * - minix4\libos\lib_legacy\libumem\common\malloc.c
 * - minix4\libos\lib_legacy\libmalloc\common\malloc.c
 * - minix4\libos\lib_legacy\i386\boot\malloc.c
 * - minix4\exokernel\kernel_legacy\malloc.c
 * - ... and 3 more files
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
 * This file contains 13 very different implementations that
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
 * SOURCE 1/13: minix4\microkernel\servers\pxk\malloc.c
 * Size: 10,860 bytes, Lines: 472
 * Hash: dc8bc5b78746...
 * ============================================================================ */

/* 
 * Mach Operating System
 * Copyright (c) 1992,1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: malloc.c,v $
 * Revision 2.9.1.1  1993/12/03  17:25:52  menze
 * [ho]: Uses "bucket stealing" and attempts to deallocate large memory
 * pieces.
 *
 * Revision 2.9  93/05/14  15:10:32  rvb
 * 	#include string.h->strings.h; kill stdlib.h
 * 	size_t -> vm_size_t. 
 * 	[93/05/14            rvb]
 * 
 * Revision 2.8  93/01/14  18:05:32  danner
 * 	Fixed comments on #else/#endif lines.
 * 	[92/12/18            pds]
 * 	Fixed typo.
 * 	[92/12/15            pds]
 * 	Converted file to ANSI C.  Fixed realloc behavior when passed
 * 	NULL/0 arguments to conform to the ANSI standard.
 * 	[92/12/13            pds]
 * 
 * 	Fixed arguments and return type of malloc and realloc to any_t so
 * 	that they will be correctly typed if __STDC__ is defined.
 * 	Defined free as a void function.
 * 	Added includes of stdlib.h and string.h.
 * 	[92/06/13            pds]
 * 
 * Revision 2.7  91/05/14  17:57:34  mrt
 * 	Correcting copyright
 * 
 * Revision 2.6  91/02/14  14:20:26  mrt
 * 	Added new Mach copyright
 * 	[91/02/13  12:41:21  mrt]
 * 
 * Revision 2.5  90/11/05  14:37:33  rpd
 * 	Added malloc_fork* code.
 * 	[90/11/02            rwd]
 * 
 * 	Add spin_lock_t.
 * 	[90/10/31            rwd]
 * 
 * Revision 2.4  90/08/07  14:31:28  rpd
 * 	Removed RCS keyword nonsense.
 * 
 * Revision 2.3  90/06/02  15:14:00  rpd
 * 	Converted to new IPC.
 * 	[90/03/20  20:56:57  rpd]
 * 
 * Revision 2.2  89/12/08  19:53:59  rwd
 * 	Removed conditionals.
 * 	[89/10/23            rwd]
 * 
 * Revision 2.1  89/08/03  17:09:46  rwd
 * Created.
 * 
 *
 * 13-Sep-88  Eric Cooper (ecc) at Carnegie Mellon University
 *	Changed realloc() to copy min(old size, new size) bytes.
 *	Bug found by Mike Kupfer at Olivetti.
 */
/*
 * 	File: 	malloc.c
 *	Author: Eric Cooper, Carnegie Mellon University
 *	Date:	July, 1988
 *
 * 	Memory allocator for use with multiple threads.
 */


#include <strings.h>
#include <cthreads.h>
#include "cthread_internals.h"

/*
 * Structure of memory block header.
 * When free, next points to next block on free list.
 * When allocated, fl points to free list.
 * Size of header is 4 bytes, so minimum usable block size is 8 bytes.
 */
typedef union header {
	union header *next;
	struct free_list *fl;
} *header_t;

#define MIN_SIZE	8	/* minimum block size */

#ifdef XK_DEBUG
#define DEBUG
#endif XK_DEBUG

typedef struct free_list {
	spin_lock_t lock;	/* spin lock for mutual exclusion */
	header_t head;		/* head of free list for this size */
#if	defined(DEBUG)
	int in_use;		/* # mallocs - # frees */
#endif	/* defined(DEBUG) */
} *free_list_t;

/*
 * Free list with index i contains blocks of size 2^(i+3) including header.
 * Smallest block size is 8, with 4 bytes available to user.
 * Size argument to malloc is a signed integer for sanity checking,
 * so largest block size is 2^31.
 */
#define NBUCKETS	29

static struct free_list malloc_free_list[NBUCKETS];

static totalmallocdmem = 0;
#define MEMSOFTLIMIT 8*1024*1024
#define LARGEST_SMALL_CHUNK 5
#define REUSE_DELTA 2

static void
more_memory(int size, register free_list_t fl)
{
	register int amount;
	register int n;
	vm_address_t where;
	register header_t h;
	kern_return_t r;

	if (size <= vm_page_size) {
		amount = vm_page_size;
		n = vm_page_size / size;
		/*
		 * We lose vm_page_size - n*size bytes here.
		 */
	} else {
		amount = size;
		totalmallocdmem += amount;
		n = 1;
	}
	MACH_CALL(vm_allocate(mach_task_self(), &where, (vm_size_t) amount, TRUE), r);
	h = (header_t) where;
	do {
		h->next = fl->head;
		fl->head = h;
		h = (header_t) ((char *) h + size);
	} while (--n != 0);
}

static void
reuse_memory(size, fl, where, where_size)
	int size;
	register free_list_t fl;
        vm_address_t where;
        int where_size;
{
	register int amount;
	register int n;
	register header_t h;
#ifdef XK_DEBUG
	static waitabit = 0;
#endif

	if (size <= where_size) {
		amount = where_size;
		n = where_size / size;
		/*
		 * We lose where_size - n*size bytes here.
		 */
	} else {
		amount = size;
		n = 1;
	}
#ifdef XK_DEBUG
	if (waitabit++ > 5)
	  printf("more_memory reuse: %d\n", amount);
#endif
	h = (header_t) where;
	do {
		h->next = fl->head;
		fl->head = h;
		h = (header_t) ((char *) h + size);
	} while (--n != 0);
}

static char *
find_bigger_bucket(flsmall, size, bucnum)
free_list_t  flsmall;
int *size, bucnum;
{
     int m = *size;
     free_list_t  fl;
     header_t hbigger = 0;

     spin_unlock(&flsmall->lock);
     while(++bucnum<NBUCKETS) {
       m <<= 1;
       fl = &malloc_free_list[bucnum];
       spin_lock(&fl->lock);
       if (hbigger=fl->head) {
	 fl->head = hbigger->next;
	 spin_unlock(&fl->lock);
	 hbigger = (header_t) ((char *) hbigger + sizeof(union header));
	 break;
       }
       spin_unlock(&fl->lock);
     }
     *size = m;
     spin_lock(&flsmall->lock);
     return hbigger;
   }

void *
malloc(register vm_size_t size)
{
	register int i, n;
	register free_list_t fl;
	register header_t h;

	if (size == 0)		/* sanity check */
		return 0;

	size += sizeof(union header);
	/*
	 * Find smallest power-of-two block size
	 * big enough to hold requested size plus header.
	 */
	i = 0;
	n = MIN_SIZE;
	while (n < size) {
		i += 1;
		n <<= 1;
	}
	ASSERT(i < NBUCKETS);
	fl = &malloc_free_list[i];
	spin_lock(&fl->lock);
	h = fl->head;
	if (h == 0) {
	  int m = n;
	  header_t hbigger;
		/*
		 * Free list is empty;
		 * allocate more blocks.
		 * First, look for a bigger bucket to steal from
		 */
	  if (i>LARGEST_SMALL_CHUNK
	      && (hbigger = find_bigger_bucket(fl, &m, i+REUSE_DELTA)))
 	        reuse_memory(n, fl, (char *)hbigger+ sizeof(union header),
			     m - sizeof(union header));
	  else
		/*
		 * allocate more blocks.
		 */
		more_memory(n, fl);
		h = fl->head;
		if (h == 0) {
			/*
			 * Allocation failed.
			 */
			spin_unlock(&fl->lock);
			return 0;
		}
	}
	/*
	 * Pop block from free list.
	 */
	fl->head = h->next;
#if	defined(DEBUG)
	fl->in_use += 1;
#endif	/* defined(DEBUG) */
	spin_unlock(&fl->lock);
	/*
	 * Store free list pointer in block header
	 * so we can figure out where it goes
	 * at free() time.
	 */
	h->fl = fl;
	/*
	 * Return pointer past the block header.
	 */
	return ((char *) h) + sizeof(union header);
}

void
free(void *base)
{
	register header_t h;
	register free_list_t fl;
	register int i;
	kern_return_t r = ~KERN_SUCCESS;

	if (base == 0)
		return;
	/*
	 * Find free list for block.
	 */
	h = (header_t) ((char *) base - sizeof(union header));
	fl = h->fl;
	i = fl - malloc_free_list;
	/*
	 * Sanity checks.
	 */
	if (i < 0 || i >= NBUCKETS) {
		ASSERT(0 <= i && i < NBUCKETS);
		return;
	}
	if (fl != &malloc_free_list[i]) {
		ASSERT(fl == &malloc_free_list[i]);
		return;
	}
	if (totalmallocdmem > MEMSOFTLIMIT && i>LARGEST_SMALL_CHUNK) {
	  int  base_size=MIN_SIZE;

	  MACH_CALL(vm_deallocate(mach_task_self(), base, base_size), r);
#ifdef XK_DEBUG
	  printf("trying to deallocate %d bytes result %x\n", base_size, r);
#endif
	  if (r==KERN_SUCCESS) totalmallocdmem -= base_size;
	}
	if (!(totalmallocdmem > MEMSOFTLIMIT && i>LARGEST_SMALL_CHUNK)
	    || r != KERN_SUCCESS)
	  {
	    /*
	     * Push block on free list.
	     */
	    spin_lock(&fl->lock);
	    h->next = fl->head;
	    fl->head = h;
#if	defined(DEBUG)
	    fl->in_use -= 1;
#endif	/* defined(DEBUG) */
	    spin_unlock(&fl->lock);
	  }
	return;
}

void *
realloc(void *old_base, vm_size_t new_size)
{
	register header_t h;
	register free_list_t fl;
	register int i;
	vm_size_t old_size;
	void *new_base;

	if (old_base == 0)
		return malloc(new_size);

	if (new_size == 0) {
		free(old_base);
		return 0;
	}

	/*
	 * Find size of old block.
	 */
	h = (header_t) ((char *) old_base - sizeof(union header));
	fl = h->fl;
	i = fl - malloc_free_list;
	/*
	 * Sanity checks.
	 */
	if (i < 0 || i >= NBUCKETS) {
		ASSERT(0 <= i && i < NBUCKETS);
		return 0;
	}
	if (fl != &malloc_free_list[i]) {
		ASSERT(fl == &malloc_free_list[i]);
		return 0;
	}
	/*
	 * Free list with index i contains blocks of size 2^(i+3) including header.
	 */
	old_size = (1 << (i+3)) - sizeof(union header);
	/*
	 * Allocate new block, copy old bytes, and free old block.
	 */
	new_base = malloc(new_size);
	if (new_base != 0) {
		memcpy(new_base, old_base,
		      (old_size < new_size ? old_size : new_size));
		free(old_base);
	}
	return new_base;
}

#if	defined(DEBUG)
void
print_malloc_free_list(void)
{
  	register int i, size;
	register free_list_t fl;
	register int n;
  	register header_t h;
	int total_used = 0;
	int total_free = 0;

	fprintf(stderr, "      Size     In Use       Free      Total\n");
  	for (i = 0, size = MIN_SIZE, fl = malloc_free_list;
	     i < NBUCKETS;
	     i += 1, size <<= 1, fl += 1) {
		spin_lock(&fl->lock);
		if (fl->in_use != 0 || fl->head != 0) {
			total_used += fl->in_use * size;
			for (n = 0, h = fl->head; h != 0; h = h->next, n += 1)
				;
			total_free += n * size;
			fprintf(stderr, "%10d %10d %10d %10d\n",
				size, fl->in_use, n, fl->in_use + n);
		}
		spin_unlock(&fl->lock);
  	}
  	fprintf(stderr, " all sizes %10d %10d %10d\n",
		total_used, total_free, total_used + total_free);
}
#endif	/* defined(DEBUG) */

void malloc_fork_prepare(void)
/*
 * Prepare the malloc module for a fork by insuring that no thread is in a
 * malloc critical section.
 */
{
    register int i;
    
    for (i = 0; i < NBUCKETS; i++) {
	spin_lock(&malloc_free_list[i].lock);
    }
}

void malloc_fork_parent(void)
/*
 * Called in the parent process after a fork() to resume normal operation.
 */
{
    register int i;

    for (i = NBUCKETS-1; i >= 0; i--) {
	spin_unlock(&malloc_free_list[i].lock);
    }
}

void malloc_fork_child(void)
/*
 * Called in the child process after a fork() to resume normal operation.
 */
{
    register int i;

    for (i = NBUCKETS-1; i >= 0; i--) {
	spin_unlock(&malloc_free_list[i].lock);
    }
}



/* ============================================================================
 * SOURCE 2/13: minix4\microkernel\servers\netiso\xebec\malloc.c
 * Size: 2,980 bytes, Lines: 137
 * Hash: f003fd0706ce...
 * ============================================================================ */

/* $Header: /n/fast/usr/lsrc/mach/CVS/lites/server/netiso/xebec/malloc.c,v 1.1.1.1 1995/03/02 21:49:57 mike Exp $ */
/* $Source: /n/fast/usr/lsrc/mach/CVS/lites/server/netiso/xebec/malloc.c,v $ */
/*
 * This code is such a kludge that I don't want to put my name on it.
 * It was a ridiculously fast hack and needs rewriting.
 * However it does work...
 */

/* 
 * a simple malloc
 * it might be brain-damaged but for the purposes of xebec
 * it's a whole lot faster than the c library malloc 
 */

#include <stdio.h>
#include "malloc.h"
#include "debug.h"
#define CHUNKSIZE 4096*2

static char *hiwat, *highend;
int bytesmalloced=0;
int byteswasted = 0;


init_alloc()
{
#ifdef LINT
	hiwat = 0;
	highend = 0;
#else LINT
	extern char *sbrk();

	hiwat = (char *) sbrk(0);
	hiwat = (char *)((unsigned)(hiwat + 3) & ~0x3);
	highend = hiwat;
#endif LINT
}

HIWAT(s)
char *s;
{
	IFDEBUG(M)
		fprintf(stdout, "HIWAT 0x%x  %s\n", hiwat,s);
		fflush(stdout);
	ENDDEBUG
}

#define MIN(x,y) ((x<y)?x:y)

char *Malloc(x)
int x;
{
	char *c;
	extern char *sbrk();
	static int firsttime=1;
	int total = x;
	int first_iter = 1;
	char *returnvalue;

	IFDEBUG(N)
		fprintf(stdout, "Malloc 0x%x, %d, bytesmalloced %d\n",
			total,total, bytesmalloced);
		fflush(stdout);
	ENDDEBUG
	IFDEBUG(M)
		fprintf(stdout, "Malloc 0x%x, %d, hiwat 0x%x\n",
			total,total, hiwat);
		fflush(stdout);
	ENDDEBUG
	if(firsttime) {
		hiwat = sbrk(0);
		if(((unsigned)(hiwat) & 0x3)) {
			bytesmalloced = 4 - (int) ((unsigned)(hiwat) & 0x3);
			hiwat = sbrk( bytesmalloced );
		} else 
			bytesmalloced = 0;
		firsttime = 0;
		highend = hiwat;
	}
	while( total ) {
		x = MIN(CHUNKSIZE, total);
		if(total != x)  {
			IFDEBUG(N)
				fprintf(stdout, "BIG Malloc tot %d, x %d, left %d net %d\n",
					total,x, total-x, bytesmalloced);
				fflush(stdout);
			ENDDEBUG
		}
		if ( (hiwat + x) > highend) {
			c = sbrk(CHUNKSIZE);
			IFDEBUG(M)
				fprintf(stdout, "hiwat 0x%x, x 0x%x, highend 0x%x, c 0x%x\n",
						hiwat, x, highend, c);
				fflush(stdout);
			ENDDEBUG
			if( c == (char *) -1 ) {
				fprintf(stderr, "Ran out of memory!\n");
				Exit(-1);
			}
			if(first_iter) {
				returnvalue = c;
				first_iter = 0;
			}
			bytesmalloced +=  CHUNKSIZE;
			IFDEBUG(m)
				if (highend != c) {
					fprintf(OUT, "warning: %d wasted bytes!\n", highend - hiwat);
				fprintf(OUT, " chunksize 0x%x,  x 0x%x \n", CHUNKSIZE, x);
				}
			ENDDEBUG
			highend = c + CHUNKSIZE;
			hiwat = c;
		}
		c = hiwat;
		if(first_iter) {
			returnvalue = c;
			first_iter = 0;
		}
		hiwat += x;
		total -= x;
	}
	if((unsigned)hiwat & 0x3) {
		byteswasted += (int)((unsigned)(hiwat) & 0x3);
		hiwat = (char *)((unsigned)(hiwat + 3) & ~0x3);
	}
	IFDEBUG(M)
		fprintf(stdout, "Malloc = 0x%x, bytesm 0x%x, wasted 0x%x, hiwat 0x%x\n",
			returnvalue, bytesmalloced, byteswasted, hiwat);
	ENDDEBUG
	IFDEBUG(N)
		fprintf(stdout, "Malloc returns 0x%x, sbrk(0) 0x%x\n", returnvalue, sbrk(0));
		fflush(stdout);
	ENDDEBUG
	return(returnvalue);
}




/* ============================================================================
 * SOURCE 3/13: minix4\microkernel\servers\lib\libc\port\gen\malloc.c
 * Size: 21,325 bytes, Lines: 922
 * Hash: bcb094783039...
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
 * Copyright 2008 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/

/*
 *	Memory management: malloc(), realloc(), free().
 *
 *	The following #-parameters may be redefined:
 *	SEGMENTED: if defined, memory requests are assumed to be
 *		non-contiguous across calls of GETCORE's.
 *	GETCORE: a function to get more core memory. If not SEGMENTED,
 *		GETCORE(0) is assumed to return the next available
 *		address. Default is 'sbrk'.
 *	ERRCORE: the error code as returned by GETCORE.
 *		Default is (char *)(-1).
 *	CORESIZE: a desired unit (measured in bytes) to be used
 *		with GETCORE. Default is (1024*ALIGN).
 *
 *	This algorithm is based on a  best fit strategy with lists of
 *	free elts maintained in a self-adjusting binary tree. Each list
 *	contains all elts of the same size. The tree is ordered by size.
 *	For results on self-adjusting trees, see the paper:
 *		Self-Adjusting Binary Trees,
 *		DD Sleator & RE Tarjan, JACM 1985.
 *
 *	The header of a block contains the size of the data part in bytes.
 *	Since the size of a block is 0%4, the low two bits of the header
 *	are free and used as follows:
 *
 *		BIT0:	1 for busy (block is in use), 0 for free.
 *		BIT1:	if the block is busy, this bit is 1 if the
 *			preceding block in contiguous memory is free.
 *			Otherwise, it is always 0.
 */

#include "lint.h"
#include "mallint.h"
#include "mtlib.h"

/*
 * Some abusers of the system (notably java1.2) acquire __malloc_lock
 * in order to prevent threads from holding it while they are being
 * suspended via thr_suspend() or thr_suspend_allmutators().
 * This never worked when alternate malloc() libraries were used
 * because they don't use __malloc_lock for their locking strategy.
 * We leave __malloc_lock as an external variable to satisfy these
 * old programs, but we define a new lock, private to libc, to do the
 * real locking: libc_malloc_lock.  This puts libc's malloc() package
 * on the same footing as all other malloc packages.
 */
mutex_t __malloc_lock = DEFAULTMUTEX;
mutex_t libc_malloc_lock = DEFAULTMUTEX;

static TREE	*Root,		/* root of the free tree */
		*Bottom,	/* the last free chunk in the arena */
		*_morecore(size_t);	/* function to get more core */

static char	*Baddr;		/* current high address of the arena */
static char	*Lfree;		/* last freed block with data intact */

static void	t_delete(TREE *);
static void	t_splay(TREE *);
static void	realfree(void *);
static void	cleanfree(void *);
static void	*_malloc_unlocked(size_t);

#define	FREESIZE (1<<5) /* size for preserving free blocks until next malloc */
#define	FREEMASK FREESIZE-1

static void *flist[FREESIZE];	/* list of blocks to be freed on next malloc */
static int freeidx;		/* index of free blocks in flist % FREESIZE */

/*
 * Interfaces used only by atfork_init() functions.
 */
void
malloc_locks(void)
{
	(void) mutex_lock(&libc_malloc_lock);
}

void
malloc_unlocks(void)
{
	(void) mutex_unlock(&libc_malloc_lock);
}

/*
 *	Allocation of small blocks
 */
static TREE	*List[MINSIZE/WORDSIZE-1]; /* lists of small blocks */

static void *
_smalloc(size_t size)
{
	TREE	*tp;
	size_t	i;

	ASSERT(size % WORDSIZE == 0);
	/* want to return a unique pointer on malloc(0) */
	if (size == 0)
		size = WORDSIZE;

	/* list to use */
	i = size / WORDSIZE - 1;

	if (List[i] == NULL) {
		TREE *np;
		int n;
		/* number of blocks to get at one time */
#define	NPS (WORDSIZE*8)
		ASSERT((size + WORDSIZE) * NPS >= MINSIZE);

		/* get NPS of these block types */
		if ((List[i] = _malloc_unlocked((size + WORDSIZE) * NPS)) == 0)
			return (0);

		/* make them into a link list */
		for (n = 0, np = List[i]; n < NPS; ++n) {
			tp = np;
			SIZE(tp) = size;
			np = NEXT(tp);
			AFTER(tp) = np;
		}
		AFTER(tp) = NULL;
	}

	/* allocate from the head of the queue */
	tp = List[i];
	List[i] = AFTER(tp);
	SETBIT0(SIZE(tp));
	return (DATA(tp));
}

void *
malloc(size_t size)
{
	void *ret;

	if (!primary_link_map) {
		errno = ENOTSUP;
		return (NULL);
	}
	assert_no_libc_locks_held();
	(void) mutex_lock(&libc_malloc_lock);
	ret = _malloc_unlocked(size);
	(void) mutex_unlock(&libc_malloc_lock);
	return (ret);
}

static void *
_malloc_unlocked(size_t size)
{
	size_t	n;
	TREE	*tp, *sp;
	size_t	o_bit1;

	COUNT(nmalloc);
	ASSERT(WORDSIZE == ALIGN);

	/* check for size that could overflow calculations */
	if (size > MAX_MALLOC) {
		errno = ENOMEM;
		return (NULL);
	}

	/* make sure that size is 0 mod ALIGN */
	ROUND(size);

	/* see if the last free block can be used */
	if (Lfree) {
		sp = BLOCK(Lfree);
		n = SIZE(sp);
		CLRBITS01(n);
		if (n == size) {
			/*
			 * exact match, use it as is
			 */
			freeidx = (freeidx + FREESIZE - 1) &
			    FREEMASK; /* 1 back */
			flist[freeidx] = Lfree = NULL;
			return (DATA(sp));
		} else if (size >= MINSIZE && n > size) {
			/*
			 * got a big enough piece
			 */
			freeidx = (freeidx + FREESIZE - 1) &
			    FREEMASK; /* 1 back */
			flist[freeidx] = Lfree = NULL;
			o_bit1 = SIZE(sp) & BIT1;
			SIZE(sp) = n;
			goto leftover;
		}
	}
	o_bit1 = 0;

	/* perform free's of space since last malloc */
	cleanfree(NULL);

	/* small blocks */
	if (size < MINSIZE)
		return (_smalloc(size));

	/* search for an elt of the right size */
	sp = NULL;
	n  = 0;
	if (Root) {
		tp = Root;
		for (;;) {
			/* branch left */
			if (SIZE(tp) >= size) {
				if (n == 0 || n >= SIZE(tp)) {
					sp = tp;
					n = SIZE(tp);
				}
				if (LEFT(tp))
					tp = LEFT(tp);
				else
					break;
			} else { /* branch right */
				if (RIGHT(tp))
					tp = RIGHT(tp);
				else
					break;
			}
		}

		if (sp) {
			t_delete(sp);
		} else if (tp != Root) {
			/* make the searched-to element the root */
			t_splay(tp);
			Root = tp;
		}
	}

	/* if found none fitted in the tree */
	if (!sp) {
		if (Bottom && size <= SIZE(Bottom)) {
			sp = Bottom;
			CLRBITS01(SIZE(sp));
		} else if ((sp = _morecore(size)) == NULL) /* no more memory */
			return (NULL);
	}

	/* tell the forward neighbor that we're busy */
	CLRBIT1(SIZE(NEXT(sp)));

	ASSERT(ISBIT0(SIZE(NEXT(sp))));

leftover:
	/* if the leftover is enough for a new free piece */
	if ((n = (SIZE(sp) - size)) >= MINSIZE + WORDSIZE) {
		n -= WORDSIZE;
		SIZE(sp) = size;
		tp = NEXT(sp);
		SIZE(tp) = n|BIT0;
		realfree(DATA(tp));
	} else if (BOTTOM(sp))
		Bottom = NULL;

	/* return the allocated space */
	SIZE(sp) |= BIT0 | o_bit1;
	return (DATA(sp));
}


/*
 * realloc().
 *
 * If the block size is increasing, we try forward merging first.
 * This is not best-fit but it avoids some data recopying.
 */
void *
realloc(void *old, size_t size)
{
	TREE	*tp, *np;
	size_t	ts;
	char	*new;

	if (!primary_link_map) {
		errno = ENOTSUP;
		return (NULL);
	}

	assert_no_libc_locks_held();
	COUNT(nrealloc);

	/* check for size that could overflow calculations */
	if (size > MAX_MALLOC) {
		errno = ENOMEM;
		return (NULL);
	}

	/* pointer to the block */
	(void) mutex_lock(&libc_malloc_lock);
	if (old == NULL) {
		new = _malloc_unlocked(size);
		(void) mutex_unlock(&libc_malloc_lock);
		return (new);
	}

	/* perform free's of space since last malloc */
	cleanfree(old);

	/* make sure that size is 0 mod ALIGN */
	ROUND(size);

	tp = BLOCK(old);
	ts = SIZE(tp);

	/* if the block was freed, data has been destroyed. */
	if (!ISBIT0(ts)) {
		(void) mutex_unlock(&libc_malloc_lock);
		return (NULL);
	}

	/* nothing to do */
	CLRBITS01(SIZE(tp));
	if (size == SIZE(tp)) {
		SIZE(tp) = ts;
		(void) mutex_unlock(&libc_malloc_lock);
		return (old);
	}

	/* special cases involving small blocks */
	if (size < MINSIZE || SIZE(tp) < MINSIZE) {
		/* free is size is zero */
		if (size == 0) {
			SETOLD01(SIZE(tp), ts);
			_free_unlocked(old);
			(void) mutex_unlock(&libc_malloc_lock);
			return (NULL);
		} else {
			goto call_malloc;
		}
	}

	/* block is increasing in size, try merging the next block */
	if (size > SIZE(tp)) {
		np = NEXT(tp);
		if (!ISBIT0(SIZE(np))) {
			ASSERT(SIZE(np) >= MINSIZE);
			ASSERT(!ISBIT1(SIZE(np)));
			SIZE(tp) += SIZE(np) + WORDSIZE;
			if (np != Bottom)
				t_delete(np);
			else
				Bottom = NULL;
			CLRBIT1(SIZE(NEXT(np)));
		}

#ifndef SEGMENTED
		/* not enough & at TRUE end of memory, try extending core */
		if (size > SIZE(tp) && BOTTOM(tp) && GETCORE(0) == Baddr) {
			Bottom = tp;
			if ((tp = _morecore(size)) == NULL) {
				tp = Bottom;
				Bottom = NULL;
				}
		}
#endif
	}

	/* got enough space to use */
	if (size <= SIZE(tp)) {
		size_t n;

chop_big:
		if ((n = (SIZE(tp) - size)) >= MINSIZE + WORDSIZE) {
			n -= WORDSIZE;
			SIZE(tp) = size;
			np = NEXT(tp);
			SIZE(np) = n|BIT0;
			realfree(DATA(np));
		} else if (BOTTOM(tp))
			Bottom = NULL;

		/* the previous block may be free */
		SETOLD01(SIZE(tp), ts);
		(void) mutex_unlock(&libc_malloc_lock);
		return (old);
	}

	/* call malloc to get a new block */
call_malloc:
	SETOLD01(SIZE(tp), ts);
	if ((new = _malloc_unlocked(size)) != NULL) {
		CLRBITS01(ts);
		if (ts > size)
			ts = size;
		MEMCOPY(new, old, ts);
		_free_unlocked(old);
		(void) mutex_unlock(&libc_malloc_lock);
		return (new);
	}

	/*
	 * Attempt special case recovery allocations since malloc() failed:
	 *
	 * 1. size <= SIZE(tp) < MINSIZE
	 *	Simply return the existing block
	 * 2. SIZE(tp) < size < MINSIZE
	 *	malloc() may have failed to allocate the chunk of
	 *	small blocks. Try asking for MINSIZE bytes.
	 * 3. size < MINSIZE <= SIZE(tp)
	 *	malloc() may have failed as with 2.  Change to
	 *	MINSIZE allocation which is taken from the beginning
	 *	of the current block.
	 * 4. MINSIZE <= SIZE(tp) < size
	 *	If the previous block is free and the combination of
	 *	these two blocks has at least size bytes, then merge
	 *	the two blocks copying the existing contents backwards.
	 */
	CLRBITS01(SIZE(tp));
	if (SIZE(tp) < MINSIZE) {
		if (size < SIZE(tp)) {			/* case 1. */
			SETOLD01(SIZE(tp), ts);
			(void) mutex_unlock(&libc_malloc_lock);
			return (old);
		} else if (size < MINSIZE) {		/* case 2. */
			size = MINSIZE;
			goto call_malloc;
		}
	} else if (size < MINSIZE) {			/* case 3. */
		size = MINSIZE;
		goto chop_big;
	} else if (ISBIT1(ts) &&
	    (SIZE(np = LAST(tp)) + SIZE(tp) + WORDSIZE) >= size) {
		ASSERT(!ISBIT0(SIZE(np)));
		t_delete(np);
		SIZE(np) += SIZE(tp) + WORDSIZE;
		/*
		 * Since the copy may overlap, use memmove() if available.
		 * Otherwise, copy by hand.
		 */
		(void) memmove(DATA(np), old, SIZE(tp));
		old = DATA(np);
		tp = np;
		CLRBIT1(ts);
		goto chop_big;
	}
	SETOLD01(SIZE(tp), ts);
	(void) mutex_unlock(&libc_malloc_lock);
	return (NULL);
}


/*
 * realfree().
 *
 * Coalescing of adjacent free blocks is done first.
 * Then, the new free block is leaf-inserted into the free tree
 * without splaying. This strategy does not guarantee the amortized
 * O(nlogn) behaviour for the insert/delete/find set of operations
 * on the tree. In practice, however, free is much more infrequent
 * than malloc/realloc and the tree searches performed by these
 * functions adequately keep the tree in balance.
 */
static void
realfree(void *old)
{
	TREE	*tp, *sp, *np;
	size_t	ts, size;

	COUNT(nfree);

	/* pointer to the block */
	tp = BLOCK(old);
	ts = SIZE(tp);
	if (!ISBIT0(ts))
		return;
	CLRBITS01(SIZE(tp));

	/* small block, put it in the right linked list */
	if (SIZE(tp) < MINSIZE) {
		ASSERT(SIZE(tp) / WORDSIZE >= 1);
		ts = SIZE(tp) / WORDSIZE - 1;
		AFTER(tp) = List[ts];
		List[ts] = tp;
		return;
	}

	/* see if coalescing with next block is warranted */
	np = NEXT(tp);
	if (!ISBIT0(SIZE(np))) {
		if (np != Bottom)
			t_delete(np);
		SIZE(tp) += SIZE(np) + WORDSIZE;
	}

	/* the same with the preceding block */
	if (ISBIT1(ts)) {
		np = LAST(tp);
		ASSERT(!ISBIT0(SIZE(np)));
		ASSERT(np != Bottom);
		t_delete(np);
		SIZE(np) += SIZE(tp) + WORDSIZE;
		tp = np;
	}

	/* initialize tree info */
	PARENT(tp) = LEFT(tp) = RIGHT(tp) = LINKFOR(tp) = NULL;

	/* the last word of the block contains self's address */
	*(SELFP(tp)) = tp;

	/* set bottom block, or insert in the free tree */
	if (BOTTOM(tp))
		Bottom = tp;
	else {
		/* search for the place to insert */
		if (Root) {
			size = SIZE(tp);
			np = Root;
			for (;;) {
				if (SIZE(np) > size) {
					if (LEFT(np))
						np = LEFT(np);
					else {
						LEFT(np) = tp;
						PARENT(tp) = np;
						break;
					}
				} else if (SIZE(np) < size) {
					if (RIGHT(np))
						np = RIGHT(np);
					else {
						RIGHT(np) = tp;
						PARENT(tp) = np;
						break;
					}
				} else {
					if ((sp = PARENT(np)) != NULL) {
						if (np == LEFT(sp))
							LEFT(sp) = tp;
						else
							RIGHT(sp) = tp;
						PARENT(tp) = sp;
					} else
						Root = tp;

					/* insert to head of list */
					if ((sp = LEFT(np)) != NULL)
						PARENT(sp) = tp;
					LEFT(tp) = sp;

					if ((sp = RIGHT(np)) != NULL)
						PARENT(sp) = tp;
					RIGHT(tp) = sp;

					/* doubly link list */
					LINKFOR(tp) = np;
					LINKBAK(np) = tp;
					SETNOTREE(np);

					break;
				}
			}
		} else
			Root = tp;
	}

	/* tell next block that this one is free */
	SETBIT1(SIZE(NEXT(tp)));

	ASSERT(ISBIT0(SIZE(NEXT(tp))));
}

/*
 * Get more core. Gaps in memory are noted as busy blocks.
 */
static TREE *
_morecore(size_t size)
{
	TREE	*tp;
	ssize_t	n, offset;
	char	*addr;
	ssize_t	nsize;

	/* compute new amount of memory to get */
	tp = Bottom;
	n = (ssize_t)size + 2 * WORDSIZE;
	addr = GETCORE(0);

	if (addr == ERRCORE)
		return (NULL);

	/* need to pad size out so that addr is aligned */
	if ((((uintptr_t)addr) % ALIGN) != 0)
		offset = ALIGN - (uintptr_t)addr % ALIGN;
	else
		offset = 0;

#ifndef SEGMENTED
	/* if not segmented memory, what we need may be smaller */
	if (addr == Baddr) {
		n -= WORDSIZE;
		if (tp != NULL)
			n -= SIZE(tp);
	}
#endif

	/* get a multiple of CORESIZE */
	n = ((n - 1) / CORESIZE + 1) * CORESIZE;
	nsize = n + offset;

	/* check if nsize request could overflow in GETCORE */
	if ((size_t)nsize > MAX_MALLOC - (uintptr_t)addr) {
		errno = ENOMEM;
		return (NULL);
	}

	if ((size_t)nsize <= MAX_GETCORE) {
		if (GETCORE(nsize) == ERRCORE)
			return (NULL);
	} else {
		intptr_t	delta;
		/*
		 * the value required is too big for GETCORE() to deal with
		 * in one go, so use GETCORE() at most 2 times instead.
		 * Argument to GETCORE() must be multiple of ALIGN.
		 * If not, GETCORE(-MAX_GETCORE) will not return brk point
		 * to previous value, but will be ALIGN more.
		 * This would leave a small hole.
		 */
		delta = MAX_GETCORE;
		while (delta > 0) {
			if (GETCORE(delta) == ERRCORE) {
				if (addr != GETCORE(0))
					(void) GETCORE(-MAX_GETCORE);
				return (NULL);
			}
			nsize -= MAX_GETCORE;
			delta = nsize;
		}
	}

	/* contiguous memory */
	if (addr == Baddr) {
		ASSERT(offset == 0);
		if (tp) {
			addr = (char *)tp;
			n += SIZE(tp) + 2 * WORDSIZE;
		} else {
			addr = Baddr - WORDSIZE;
			n += WORDSIZE;
		}
	} else
		addr += offset;

	/* new bottom address */
	Baddr = addr + n;

	/* new bottom block */
	tp = (TREE *)(uintptr_t)addr;
	SIZE(tp) = n - 2 * WORDSIZE;
	ASSERT((SIZE(tp) % ALIGN) == 0);

	/* reserved the last word to head any noncontiguous memory */
	SETBIT0(SIZE(NEXT(tp)));

	/* non-contiguous memory, free old bottom block */
	if (Bottom && Bottom != tp) {
		SETBIT0(SIZE(Bottom));
		realfree(DATA(Bottom));
	}

	return (tp);
}


/*
 * Tree rotation functions (BU: bottom-up, TD: top-down)
 */

#define	LEFT1(x, y)		\
			if ((RIGHT(x) = LEFT(y)) != NULL) PARENT(RIGHT(x)) = x;\
			if ((PARENT(y) = PARENT(x)) != NULL)\
				if (LEFT(PARENT(x)) == x) LEFT(PARENT(y)) = y;\
				else RIGHT(PARENT(y)) = y;\
			LEFT(y) = x; PARENT(x) = y

#define	RIGHT1(x, y)		\
			if ((LEFT(x) = RIGHT(y)) != NULL) PARENT(LEFT(x)) = x;\
			if ((PARENT(y) = PARENT(x)) != NULL)\
				if (LEFT(PARENT(x)) == x) LEFT(PARENT(y)) = y;\
				else RIGHT(PARENT(y)) = y;\
			RIGHT(y) = x; PARENT(x) = y

#define	BULEFT2(x, y, z)	\
			if ((RIGHT(x) = LEFT(y)) != NULL) PARENT(RIGHT(x)) = x;\
			if ((RIGHT(y) = LEFT(z)) != NULL) PARENT(RIGHT(y)) = y;\
			if ((PARENT(z) = PARENT(x)) != NULL)\
				if (LEFT(PARENT(x)) == x) LEFT(PARENT(z)) = z;\
				else RIGHT(PARENT(z)) = z;\
			LEFT(z) = y; PARENT(y) = z; LEFT(y) = x; PARENT(x) = y

#define	BURIGHT2(x, y, z)	\
			if ((LEFT(x) = RIGHT(y)) != NULL) PARENT(LEFT(x)) = x;\
			if ((LEFT(y) = RIGHT(z)) != NULL) PARENT(LEFT(y)) = y;\
			if ((PARENT(z) = PARENT(x)) != NULL)\
				if (LEFT(PARENT(x)) == x) LEFT(PARENT(z)) = z;\
				else RIGHT(PARENT(z)) = z;\
			RIGHT(z) = y; PARENT(y) = z; RIGHT(y) = x; PARENT(x) = y

#define	TDLEFT2(x, y, z)	\
			if ((RIGHT(y) = LEFT(z)) != NULL) PARENT(RIGHT(y)) = y;\
			if ((PARENT(z) = PARENT(x)) != NULL)\
				if (LEFT(PARENT(x)) == x) LEFT(PARENT(z)) = z;\
				else RIGHT(PARENT(z)) = z;\
			PARENT(x) = z; LEFT(z) = x;

#define	TDRIGHT2(x, y, z)	\
			if ((LEFT(y) = RIGHT(z)) != NULL) PARENT(LEFT(y)) = y;\
			if ((PARENT(z) = PARENT(x)) != NULL)\
				if (LEFT(PARENT(x)) == x) LEFT(PARENT(z)) = z;\
				else RIGHT(PARENT(z)) = z;\
			PARENT(x) = z; RIGHT(z) = x;

/*
 * Delete a tree element
 */
static void
t_delete(TREE *op)
{
	TREE	*tp, *sp, *gp;

	/* if this is a non-tree node */
	if (ISNOTREE(op)) {
		tp = LINKBAK(op);
		if ((sp = LINKFOR(op)) != NULL)
			LINKBAK(sp) = tp;
		LINKFOR(tp) = sp;
		return;
	}

	/* make op the root of the tree */
	if (PARENT(op))
		t_splay(op);

	/* if this is the start of a list */
	if ((tp = LINKFOR(op)) != NULL) {
		PARENT(tp) = NULL;
		if ((sp = LEFT(op)) != NULL)
			PARENT(sp) = tp;
		LEFT(tp) = sp;

		if ((sp = RIGHT(op)) != NULL)
			PARENT(sp) = tp;
		RIGHT(tp) = sp;

		Root = tp;
		return;
	}

	/* if op has a non-null left subtree */
	if ((tp = LEFT(op)) != NULL) {
		PARENT(tp) = NULL;

		if (RIGHT(op)) {
			/* make the right-end of the left subtree its root */
			while ((sp = RIGHT(tp)) != NULL) {
				if ((gp = RIGHT(sp)) != NULL) {
					TDLEFT2(tp, sp, gp);
					tp = gp;
				} else {
					LEFT1(tp, sp);
					tp = sp;
				}
			}

			/* hook the right subtree of op to the above elt */
			RIGHT(tp) = RIGHT(op);
			PARENT(RIGHT(tp)) = tp;
		}
	} else if ((tp = RIGHT(op)) != NULL)	/* no left subtree */
		PARENT(tp) = NULL;

	Root = tp;
}

/*
 * Bottom up splaying (simple version).
 * The basic idea is to roughly cut in half the
 * path from Root to tp and make tp the new root.
 */
static void
t_splay(TREE *tp)
{
	TREE	*pp, *gp;

	/* iterate until tp is the root */
	while ((pp = PARENT(tp)) != NULL) {
		/* grandparent of tp */
		gp = PARENT(pp);

		/* x is a left child */
		if (LEFT(pp) == tp) {
			if (gp && LEFT(gp) == pp) {
				BURIGHT2(gp, pp, tp);
			} else {
				RIGHT1(pp, tp);
			}
		} else {
			ASSERT(RIGHT(pp) == tp);
			if (gp && RIGHT(gp) == pp) {
				BULEFT2(gp, pp, tp);
			} else {
				LEFT1(pp, tp);
			}
		}
	}
}


/*
 *	free().
 *	Performs a delayed free of the block pointed to
 *	by old. The pointer to old is saved on a list, flist,
 *	until the next malloc or realloc. At that time, all the
 *	blocks pointed to in flist are actually freed via
 *	realfree(). This allows the contents of free blocks to
 *	remain undisturbed until the next malloc or realloc.
 */
void
free(void *old)
{
	if (!primary_link_map) {
		errno = ENOTSUP;
		return;
	}
	assert_no_libc_locks_held();
	(void) mutex_lock(&libc_malloc_lock);
	_free_unlocked(old);
	(void) mutex_unlock(&libc_malloc_lock);
}


void
_free_unlocked(void *old)
{
	int	i;

	if (old == NULL)
		return;

	/*
	 * Make sure the same data block is not freed twice.
	 * 3 cases are checked.  It returns immediately if either
	 * one of the conditions is true.
	 *	1. Last freed.
	 *	2. Not in use or freed already.
	 *	3. In the free list.
	 */
	if (old == Lfree)
		return;
	if (!ISBIT0(SIZE(BLOCK(old))))
		return;
	for (i = 0; i < freeidx; i++)
		if (old == flist[i])
			return;

	if (flist[freeidx] != NULL)
		realfree(flist[freeidx]);
	flist[freeidx] = Lfree = old;
	freeidx = (freeidx + 1) & FREEMASK; /* one forward */
}

/*
 * cleanfree() frees all the blocks pointed to be flist.
 *
 * realloc() should work if it is called with a pointer
 * to a block that was freed since the last call to malloc() or
 * realloc(). If cleanfree() is called from realloc(), ptr
 * is set to the old block and that block should not be
 * freed since it is actually being reallocated.
 */
static void
cleanfree(void *ptr)
{
	char	**flp;

	flp = (char **)&(flist[freeidx]);
	for (;;) {
		if (flp == (char **)&(flist[0]))
			flp = (char **)&(flist[FREESIZE]);
		if (*--flp == NULL)
			break;
		if (*flp != ptr)
			realfree(*flp);
		*flp = NULL;
	}
	freeidx = 0;
	Lfree = NULL;
}



/* ============================================================================
 * SOURCE 4/13: minix4\libos\posix\libc\stdlib\malloc.c
 * Size: 4,820 bytes, Lines: 207
 * Hash: f0b15416a24d...
 * ============================================================================ */

/**
 * @file malloc.c
 * @brief Legacy memory allocator now forwarded to kalloc.
 *
 * The original implementation is preserved below and disabled.
 */
#include <kalloc.h>
#if 0
/* Copyright (c) 1979 Regents of the University of California */
#ifdef debug
#define ASSERT(p)                                                              \
  if (!(p))                                                                    \
    botch("p");                                                                \
  else
botch(s)
char *s;
{
	printf("assertion botched: %s\n",s);
	abort();
}
#else
#define ASSERT(p)
#endif

/*	avoid break bug */
#ifdef pdp11
#define GRANULE 64
#else
#define GRANULE 0
#endif
/*	C storage allocator
 *	circular first-fit strategy
 *	works with noncontiguous, but monotonically linked, arena
 *	each block is preceded by a ptr to the (pointer of) 
 *	the next following block
 *	blocks are exact number of words long 
 *	aligned to the data type requirements of ALIGN
 *	pointers to blocks must have BUSY bit 0
 *	bit in ptr is 1 for busy, 0 for idle
 *	gaps in arena are merely noted as busy blocks
 *	last block of arena (pointed to by alloct) is empty and
 *	has a pointer to first
 *	idle blocks are coalesced during space search
 *
 *	a different implementation may need to redefine
 *	ALIGN, NALIGN, BLOCK, BUSY, INT
 *	where INT is integer type to which a pointer can be cast
*/
#define INT int
#define ALIGN int
#define NALIGN 1
#define WORD sizeof(union store)
#define BLOCK 1024 /* a multiple of WORD*/
#define BUSY 1
#define NULL 0
#define testbusy(p) ((INT)(p) & BUSY)
#define setbusy(p) (union store *)((INT)(p) | BUSY)
#define clearbusy(p) (union store *)((INT)(p) & ~BUSY)

union store { union store *ptr;
	      ALIGN dummy[NALIGN];
	      int calloc;	/*calloc clears an array of integers*/
};

static	union store allocs[2];	/*initial arena*/
static	union store *allocp;	/*search ptr*/
static	union store *alloct;	/*arena top*/
static	union store *allocx;	/*for benefit of realloc*/
char	*sbrk();

char *
malloc(nbytes)
unsigned nbytes;
{
	register union store *p, *q;
	register nw;
	static temp;	/*coroutines assume no auto*/

	if(allocs[0].ptr==0) {	/*first time*/
		allocs[0].ptr = setbusy(&allocs[1]);
		allocs[1].ptr = setbusy(&allocs[0]);
		alloct = &allocs[1];
		allocp = &allocs[0];
	}
	nw = (nbytes+WORD+WORD-1)/WORD;
	ASSERT(allocp>=allocs && allocp<=alloct);
	ASSERT(allock());
	for(p=allocp; ; ) {
		for(temp=0; ; ) {
			if(!testbusy(p->ptr)) {
				while(!testbusy((q=p->ptr)->ptr)) {
					ASSERT(q>p&&q<alloct);
					p->ptr = q->ptr;
				}
				if(q>=p+nw && p+nw>=p)
					goto found;
			}
			q = p;
			p = clearbusy(p->ptr);
			if(p>q)
				ASSERT(p<=alloct);
			else if(q!=alloct || p!=allocs) {
				ASSERT(q==alloct&&p==allocs);
				return(NULL);
			} else if(++temp>1)
				break;
		}
		temp = ((nw+BLOCK/WORD)/(BLOCK/WORD))*(BLOCK/WORD);
		q = (union store *)sbrk(0);
		if(q+temp+GRANULE < q) {
			return(NULL);
		}
		q = (union store *)sbrk(temp*WORD);
		if((INT)q == -1) {
			return(NULL);
		}
		ASSERT(q>alloct);
		alloct->ptr = q;
		if(q!=alloct+1)
			alloct->ptr = setbusy(alloct->ptr);
		alloct = q->ptr = q+temp-1;
		alloct->ptr = setbusy(allocs);
	}
found:
	allocp = p + nw;
	ASSERT(allocp<=alloct);
	if(q>allocp) {
		allocx = allocp->ptr;
		allocp->ptr = p->ptr;
	}
	p->ptr = setbusy(allocp);
	return((char *)(p+1));
}

/*	freeing strategy tuned for LIFO allocation
*/
free(ap)
register char *ap;
{
	register union store *p = (union store *)ap;

	ASSERT(p>clearbusy(allocs[1].ptr)&&p<=alloct);
	ASSERT(allock());
	allocp = --p;
	ASSERT(testbusy(p->ptr));
	p->ptr = clearbusy(p->ptr);
	ASSERT(p->ptr > allocp && p->ptr <= alloct);
}

/*	realloc(p, nbytes) reallocates a block obtained from malloc()
 *	and freed since last call of malloc()
 *	to have new size nbytes, and old content
 *	returns new location, or 0 on failure
*/

char *
realloc(p, nbytes)
register union store *p;
unsigned nbytes;
{
	register union store *q;
	union store *s, *t;
	register unsigned nw;
	unsigned onw;

	if(testbusy(p[-1].ptr))
		free((char *)p);
	onw = p[-1].ptr - p;
	q = (union store *)malloc(nbytes);
	if(q==NULL || q==p)
		return((char *)q);
	s = p;
	t = q;
	nw = (nbytes+WORD-1)/WORD;
	if(nw<onw)
		onw = nw;
	while(onw--!=0)
		*t++ = *s++;
	if(q<p && q+nw>=p)
		(q+(q+nw-p))->ptr = allocx;
	return((char *)q);
}

#ifdef debug
allock()
{
#ifdef longdebug
	register union store *p;
	int x;
	x = 0;
	for(p= &allocs[0]; clearbusy(p->ptr) > p; p=clearbusy(p->ptr)) {
		if(p==allocp)
			x++;
	}
	ASSERT(p==alloct);
	return(x==1|p==allocp);
#else
	return(1);
#endif
}
#endif
#endif /* end legacy implementation */

char *malloc(unsigned nbytes) { return kalloc(nbytes); }
void free(char *ap) { kfree(ap); }
char *realloc(char *ptr, unsigned nbytes) { return krealloc(ptr, nbytes); }



/* ============================================================================
 * SOURCE 5/13: minix4\libos\lib_legacy\watchmalloc\common\malloc.c
 * Size: 31,268 bytes, Lines: 1,460
 * Hash: adce9a97c9c4...
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
 * Copyright 2008 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved	*/

/*
 *	Memory management: malloc(), realloc(), free(), memalign().
 *
 *	The following #-parameters may be redefined:
 *	GETCORE: a function to get more core memory.
 *		GETCORE(0) is assumed to return the next available
 *		address. Default is 'sbrk'.
 *	ERRCORE: the error code as returned by GETCORE.
 *		Default is ((char *)(-1)).
 *	CORESIZE: a desired unit (measured in bytes) to be used
 *		with GETCORE. Default is (1024*ALIGN).
 *
 *	This algorithm is based on a best fit strategy with lists of
 *	free elts maintained in a self-adjusting binary tree. Each list
 *	contains all elts of the same size. The tree is ordered by size.
 *	For results on self-adjusting trees, see the paper:
 *		Self-Adjusting Binary Trees,
 *		DD Sleator & RE Tarjan, JACM 1985.
 *
 *	The header of a block contains the size of the data part in bytes.
 *	Since the size of a block is 0%4, the low two bits of the header
 *	are free and used as follows:
 *
 *		BIT0:	1 for busy (block is in use), 0 for free.
 *		BIT1:	if the block is busy, this bit is 1 if the
 *			preceding block in contiguous memory is free.
 *			Otherwise, it is always 0.
 */

#include "mallint.h"

static	mutex_t	__watch_malloc_lock = DEFAULTMUTEX;

static	TREE	*Root;		/* root of the free tree */
static	TREE	*Bottom;	/* the last free chunk in the arena */
static	char	*Baddr;		/* current high address of the arena */

static	void	t_delete(TREE *);
static	void	t_splay(TREE *);
static	void	realfree(void *);
static	void	*malloc_unlocked(size_t);
static	void	free_unlocked(void *);
static	TREE	*morecore(size_t);

static	void	protect(TREE *);
static	void	unprotect(TREE *);

#define	FREEPAT	0
#define	LIVEPAT	1

/*
 * Patterns to be copied into freed blocks and allocated blocks.
 * 0xfeedbeef and 0xfeedface are invalid pointer values in all programs.
 */
static	uint64_t	patterns[2] = {
	0xdeadbeefdeadbeefULL,	/* pattern in a freed block */
	0xbaddcafebaddcafeULL	/* pattern in an allocated block */
};

static void
copy_pattern(int pat, TREE *tp)
{
	uint64_t pattern = patterns[pat];
	size_t sz = SIZE(tp) / sizeof (uint64_t);
	/* LINTED improper alignment */
	uint64_t *datap = (uint64_t *)DATA(tp);

	while (sz--)
		*datap++ = pattern;
}

/*
 * Keep lists of small blocks, LIFO order.
 */
static	TREE	*List[MINSIZE/WORDSIZE-1];
static	TREE	*Last[MINSIZE/WORDSIZE-1];

/* number of blocks to get at one time */
#define	NPS	(WORDSIZE*8)

static void *
smalloc(size_t size)
{
	TREE	*tp;
	size_t	i;

	ASSERT(size % WORDSIZE == 0);
	/* want to return a unique pointer on malloc(0) */
	if (size == 0)
		size = WORDSIZE;

	/* list to use */
	i = size / WORDSIZE - 1;

	if (List[i] == NULL) {
		TREE	*np;
		int	n;
		ASSERT((size + WORDSIZE) * NPS >= MINSIZE);

		/* get NPS of these block types */
		if ((np = malloc_unlocked((size + WORDSIZE)*NPS)) == NULL)
			return (NULL);

		/* make them into a link list */
		for (n = 0, List[i] = np; n < NPS; ++n) {
			tp = np;
			SIZE(tp) = size;
			copy_pattern(FREEPAT, tp);
			if (n == NPS - 1) {
				Last[i] = tp;
				np = NULL;
			} else {
				/* LINTED improper alignment */
				np = NEXT(tp);
			}
			AFTER(tp) = np;
			protect(tp);
		}
	}

	/* allocate from the head of the queue */
	tp = List[i];
	unprotect(tp);
	if ((List[i] = AFTER(tp)) == NULL)
		Last[i] = NULL;
	copy_pattern(LIVEPAT, tp);
	SETBIT0(SIZE(tp));
	protect(tp);
	return (DATA(tp));
}

void *
malloc(size_t size)
{
	void	*ret;
	(void) mutex_lock(&__watch_malloc_lock);
	ret = malloc_unlocked(size);
	(void) mutex_unlock(&__watch_malloc_lock);
	return (ret);
}

static void *
malloc_unlocked(size_t size)
{
	size_t	n;
	TREE	*tp, *sp, *tmp;

	COUNT(nmalloc);
	ASSERT(WORDSIZE == ALIGN);

	/* check for size that could overflow calculations */
	if (size > MAX_MALLOC) {
		errno = ENOMEM;
		return (NULL);
	}
	/* make sure that size is 0 mod ALIGN */
	ROUND(size);

	/* small blocks */
	if (size < MINSIZE)
		return (smalloc(size));

	/* search for an elt of the right size */
	sp = NULL;
	n = 0;
	if (Root) {
		tp = Root;
		for (;;) {
			unprotect(tp);
			if (SIZE(tp) >= size) {	/* branch left */
				if (n == 0 || n >= SIZE(tp)) {
					sp = tp;
					n = SIZE(tp);
				}
				if ((tmp = LEFT(tp)) != NULL) {
					protect(tp);
					tp = tmp;
				} else {
					protect(tp);
					break;
				}
			} else {		/* branch right */
				if ((tmp = RIGHT(tp)) != NULL) {
					protect(tp);
					tp = tmp;
				} else {
					protect(tp);
					break;
				}
			}
		}

		if (sp) {
			unprotect(sp);
			t_delete(sp);
		} else if (tp != Root) {
			/* make the searched-to element the root */
			unprotect(tp);
			t_splay(tp);
			protect(tp);
			Root = tp;
		}
	}

	/* if found none fitted in the tree */
	if (sp == NULL) {
		if (Bottom) {
			unprotect(Bottom);
			if (size <= SIZE(Bottom)) {
				sp = Bottom;
				CLRBITS01(SIZE(sp));
			} else {
				protect(Bottom);
				if ((sp = morecore(size)) == NULL)
					return (NULL);
			}
		} else {
			if ((sp = morecore(size)) == NULL)
				return (NULL);
		}
	}

	/* tell the forward neighbor that we're busy */
	/* LINTED improper alignment */
	tmp = NEXT(sp);
	unprotect(tmp);
	CLRBIT1(SIZE(tmp));
	ASSERT(ISBIT0(SIZE(tmp)));
	protect(tmp);

leftover:
	/* if the leftover is enough for a new free piece */
	if ((n = (SIZE(sp) - size)) >= MINSIZE + WORDSIZE) {
		n -= WORDSIZE;
		SIZE(sp) = size;
		/* LINTED improper alignment */
		tp = NEXT(sp);
		SIZE(tp) = n | BIT0;
		realfree(DATA(tp));
	} else if (BOTTOM(sp))
		Bottom = NULL;

	/* return the allocated space */
	copy_pattern(LIVEPAT, sp);
	SIZE(sp) |= BIT0;
	protect(sp);
	return (DATA(sp));
}

/*
 *	realloc().
 *	If the block size is increasing, we try forward merging first.
 *	This is not best-fit but it avoids some data recopying.
 */
void *
realloc(void *old, size_t size)
{
	TREE	*tp, *np;
	size_t	ts;
	char	*new;

	COUNT(nrealloc);

	/* check for size that could overflow calculations */
	if (size > MAX_MALLOC) {
		errno = ENOMEM;
		return (NULL);
	}

	/* pointer to the block */
	(void) mutex_lock(&__watch_malloc_lock);
	if (old == NULL) {
		new = malloc_unlocked(size);
		(void) mutex_unlock(&__watch_malloc_lock);
		return (new);
	}

	/* make sure that size is 0 mod ALIGN */
	ROUND(size);

	/* LINTED improper alignment */
	tp = BLOCK(old);
	unprotect(tp);
	ts = SIZE(tp);

	/* if the block was freed, data has been destroyed. */
	if (!ISBIT0(ts)) {
		/* XXX; complain here! */
		protect(tp);
		(void) mutex_unlock(&__watch_malloc_lock);
		errno = EINVAL;
		return (NULL);
	}

	CLRBITS01(SIZE(tp));
	if (size == SIZE(tp)) {	/* nothing to do */
		SIZE(tp) = ts;
		protect(tp);
		(void) mutex_unlock(&__watch_malloc_lock);
		return (old);
	}

	/* special cases involving small blocks */
	if (size < MINSIZE || SIZE(tp) < MINSIZE) {
		if (size == 0) {
			SETOLD01(SIZE(tp), ts);
			free_unlocked(old);
			(void) mutex_unlock(&__watch_malloc_lock);
			return (NULL);
		}
		goto call_malloc;
	}

	/* block is increasing in size, try merging the next block */
	if (size > SIZE(tp)) {
		/* LINTED improper alignment */
		np = NEXT(tp);
		unprotect(np);
		if (ISBIT0(SIZE(np)))
			protect(np);
		else {
			TREE *tmp;
			ASSERT(SIZE(np) >= MINSIZE);
			ASSERT(!ISBIT1(SIZE(np)));
			SIZE(tp) += SIZE(np) + WORDSIZE;
			if (np != Bottom)
				t_delete(np);
			else
				Bottom = NULL;
			/* LINTED improper alignment */
			tmp = NEXT(np);
			unprotect(tmp);
			CLRBIT1(SIZE(tmp));
			protect(tmp);
		}

		/* not enough & at TRUE end of memory, try extending core */
		if (size > SIZE(tp) && BOTTOM(tp) && GETCORE(0) == Baddr) {
			Bottom = tp;
			protect(Bottom);
			if ((tp = morecore(size)) == NULL) {
				tp = Bottom;
				Bottom = NULL;
				unprotect(tp);
			}
		}
	}

	/* got enough space to use */
	if (size <= SIZE(tp)) {
		size_t n;
chop_big:
		if ((n = (SIZE(tp) - size)) >= MINSIZE + WORDSIZE) {
			n -= WORDSIZE;
			SIZE(tp) = size;
			/* LINTED improper alignment */
			np = NEXT(tp);
			SIZE(np) = n | BIT0;
			realfree(DATA(np));
		} else if (BOTTOM(tp))
			Bottom = NULL;

		/* the previous block may be free */
		SETOLD01(SIZE(tp), ts);
		protect(tp);
		(void) mutex_unlock(&__watch_malloc_lock);
		return (old);
	}

call_malloc:	/* call malloc to get a new block */
	SETOLD01(SIZE(tp), ts);
	if ((new = malloc_unlocked(size)) != NULL) {
		CLRBITS01(ts);
		if (ts > size)
			ts = size;
		(void) memcpy(new, old, ts);
		free_unlocked(old);
		(void) mutex_unlock(&__watch_malloc_lock);
		return (new);
	}

	/*
	 * Attempt special case recovery allocations since malloc() failed:
	 *
	 * 1. size <= SIZE(tp) < MINSIZE
	 *	Simply return the existing block
	 * 2. SIZE(tp) < size < MINSIZE
	 *	malloc() may have failed to allocate the chunk of
	 *	small blocks. Try asking for MINSIZE bytes.
	 * 3. size < MINSIZE <= SIZE(tp)
	 *	malloc() may have failed as with 2.  Change to
	 *	MINSIZE allocation which is taken from the beginning
	 *	of the current block.
	 * 4. MINSIZE <= SIZE(tp) < size
	 *	If the previous block is free and the combination of
	 *	these two blocks has at least size bytes, then merge
	 *	the two blocks copying the existing contents backwards.
	 */
	CLRBITS01(SIZE(tp));
	if (SIZE(tp) < MINSIZE) {
		if (size < SIZE(tp))		/* case 1. */ {
			SETOLD01(SIZE(tp), ts);
			protect(tp);
			(void) mutex_unlock(&__watch_malloc_lock);
			return (old);
		} else if (size < MINSIZE)	/* case 2. */ {
			size = MINSIZE;
			goto call_malloc;
		}
	} else if (size < MINSIZE)		/* case 3. */ {
		size = MINSIZE;
		goto chop_big;
	} else if (ISBIT1(ts)) {
		np = LAST(tp);
		unprotect(np);
		if ((SIZE(np) + SIZE(tp) + WORDSIZE) >= size) {
			ASSERT(!ISBIT0(SIZE(np)));
			t_delete(np);
			SIZE(np) += SIZE(tp) + WORDSIZE;
			/*
			 * Since the copy may overlap, use memmove().
			 */
			(void) memmove(DATA(np), old, SIZE(tp));
			old = DATA(np);
			tp = np;
			CLRBIT1(ts);
			goto chop_big;
		}
		protect(np);
	}
	SETOLD01(SIZE(tp), ts);
	protect(tp);
	(void) mutex_unlock(&__watch_malloc_lock);
	/* malloc() sets errno */
	return (NULL);
}

/*
 *	realfree().
 *	Coalescing of adjacent free blocks is done first.
 *	Then, the new free block is leaf-inserted into the free tree
 *	without splaying. This strategy does not guarantee the amortized
 *	O(nlogn) behaviour for the insert/delete/find set of operations
 *	on the tree. In practice, however, free is much more infrequent
 *	than malloc/realloc and the tree searches performed by these
 *	functions adequately keep the tree in balance.
 */
static void
realfree(void *old)
{
	TREE	*tp, *sp, *np, *tmp;
	size_t	ts, size;

	COUNT(nfree);

	/* pointer to the block */
	/* LINTED improper alignment */
	tp = BLOCK(old);
	unprotect(tp);
	ts = SIZE(tp);
	if (!ISBIT0(ts)) {	/* block is not busy; previously freed? */
		protect(tp);	/* force a watchpoint trap */
		CLRBIT0(SIZE(tp));
		return;
	}
	CLRBITS01(SIZE(tp));
	copy_pattern(FREEPAT, tp);

	/* small block, return it to the tail of its queue */
	if (SIZE(tp) < MINSIZE) {
		ASSERT(SIZE(tp) / WORDSIZE >= 1);
		ts = SIZE(tp) / WORDSIZE - 1;
		AFTER(tp) = NULL;
		protect(tp);
		if (List[ts] == NULL) {
			List[ts] = tp;
			Last[ts] = tp;
		} else {
			sp = Last[ts];
			unprotect(sp);
			AFTER(sp) = tp;
			protect(sp);
			Last[ts] = tp;
		}
		return;
	}

	/* see if coalescing with next block is warranted */
	/* LINTED improper alignment */
	np = NEXT(tp);
	unprotect(np);
	if (ISBIT0(SIZE(np)))
		protect(np);
	else {
		if (np != Bottom)
			t_delete(np);
		SIZE(tp) += SIZE(np) + WORDSIZE;
	}

	/* the same with the preceding block */
	if (ISBIT1(ts)) {
		np = LAST(tp);
		unprotect(np);
		ASSERT(!ISBIT0(SIZE(np)));
		ASSERT(np != Bottom);
		t_delete(np);
		SIZE(np) += SIZE(tp) + WORDSIZE;
		tp = np;
	}

	/* initialize tree info */
	PARENT(tp) = LEFT(tp) = RIGHT(tp) = LINKFOR(tp) = NULL;

	/* set bottom block, or insert in the free tree */
	if (BOTTOM(tp))
		Bottom = tp;
	else {
		/* search for the place to insert */
		if (Root) {
			size = SIZE(tp);
			np = Root;
			for (;;) {
				unprotect(np);
				if (SIZE(np) > size) {
					if ((tmp = LEFT(np)) != NULL) {
						protect(np);
						np = tmp;
					} else {
						LEFT(np) = tp;
						PARENT(tp) = np;
						protect(np);
						break;
					}
				} else if (SIZE(np) < size) {
					if ((tmp = RIGHT(np)) != NULL) {
						protect(np);
						np = tmp;
					} else {
						RIGHT(np) = tp;
						PARENT(tp) = np;
						protect(np);
						break;
					}
				} else {
					if ((sp = PARENT(np)) != NULL) {
						unprotect(sp);
						if (np == LEFT(sp))
							LEFT(sp) = tp;
						else
							RIGHT(sp) = tp;
						PARENT(tp) = sp;
						protect(sp);
					} else
						Root = tp;

					/* insert to head of list */
					if ((sp = LEFT(np)) != NULL) {
						unprotect(sp);
						PARENT(sp) = tp;
						protect(sp);
					}
					LEFT(tp) = sp;

					if ((sp = RIGHT(np)) != NULL) {
						unprotect(sp);
						PARENT(sp) = tp;
						protect(sp);
					}
					RIGHT(tp) = sp;

					/* doubly link list */
					LINKFOR(tp) = np;
					LINKBAK(np) = tp;
					SETNOTREE(np);
					protect(np);

					break;
				}
			}
		} else {
			Root = tp;
		}
	}

	/*
	 * Tell next block that this one is free.
	 * The first WORD of the next block contains self's address.
	 */
	/* LINTED improper alignment */
	tmp = NEXT(tp);
	unprotect(tmp);
	/* LINTED improper alignment */
	*(SELFP(tp)) = tp;
	SETBIT1(SIZE(tmp));
	ASSERT(ISBIT0(SIZE(tmp)));
	protect(tmp);
	protect(tp);
}

/*
 * Get more core. Gaps in memory are noted as busy blocks.
 */
static TREE *
morecore(size_t size)
{
	TREE	*tp;
	size_t	n, offset, requestsize;
	char	*addr;

	/* compute new amount of memory to get */
	tp = Bottom;
	n = size + 2 * WORDSIZE;
	addr = GETCORE(0);

	if (addr == ERRCORE)
		/* errno set by GETCORE sbrk */
		return (NULL);

	/* need to pad size out so that addr is aligned */
	if ((((size_t)addr) % ALIGN) != 0)
		offset = ALIGN - (size_t)addr % ALIGN;
	else
		offset = 0;

	if (tp)
		unprotect(tp);

	/* if not segmented memory, what we need may be smaller */
	if (addr == Baddr) {
		n -= WORDSIZE;
		if (tp != NULL)
			n -= SIZE(tp);
	}

	/* get a multiple of CORESIZE */
	n = ((n - 1) / CORESIZE + 1) * CORESIZE;
	requestsize = n + offset;

	/* check if nsize request could overflow in GETCORE */
	if (requestsize > MAX_MALLOC - (size_t)addr) {
		if (tp)
			protect(tp);
		errno = ENOMEM;
		return (NULL);
	}

	if (requestsize > MAX_GETCORE) {
		intptr_t	delta;
		/*
		 * the value required is too big for GETCORE() to deal with
		 * in one go, so use GETCORE() at most 2 times instead.
		 * Argument to GETCORE() must be multiple of ALIGN.
		 * If not, GETCORE(-MAX_GETCORE) will not return brk point
		 * to previous value, but will be ALIGN more.
		 * This would leave a small hole.
		 */
		delta = MAX_GETCORE;
		while (delta > 0) {
			if (GETCORE(delta) == ERRCORE) {
				if (tp)
					protect(tp);
				if (addr != GETCORE(0))
					(void) GETCORE(-MAX_GETCORE);
				return (NULL);
			}
			requestsize -= MAX_GETCORE;
			delta = requestsize;
		}
	} else if (GETCORE(requestsize) == ERRCORE) {
		if (tp)
			protect(tp);
		return (NULL);
	}

	/* contiguous memory */
	if (addr == Baddr) {
		ASSERT(offset == 0);
		if (tp) {
			addr = ((char *)tp);
			n += SIZE(tp) + 2 * WORDSIZE;
		} else {
			addr = Baddr - WORDSIZE;
			n += WORDSIZE;
		}
	} else {
		addr += offset;
	}

	/* new bottom address */
	Baddr = addr + n;

	/* new bottom block */
	/* LINTED improper alignment */
	tp = ((TREE *)addr);
	SIZE(tp) = n - 2 * WORDSIZE;
	ASSERT((SIZE(tp) % ALIGN) == 0);

	/* reserved the last word to head any noncontiguous memory */
	/* LINTED improper alignment */
	SETBIT0(SIZE(NEXT(tp)));

	/* non-contiguous memory, free old bottom block */
	if (Bottom && Bottom != tp) {
		SETBIT0(SIZE(Bottom));
		realfree(DATA(Bottom));
	}

	return (tp);
}

/*
 * Utility function to avoid protecting a tree node twice.
 * Return true if tp is in the NULL-terminated array of tree nodes.
 */
static int
in_list(TREE *tp, TREE **npp)
{
	TREE *sp;

	while ((sp = *npp++) != NULL)
		if (tp == sp)
			return (1);
	return (0);
}

/*
 * Tree rotation functions (BU: bottom-up, TD: top-down).
 * All functions are entered with the arguments unprotected.
 * They must return in the same condition, with all other elements
 * that have been unprotected during the operation re-protected.
 */
static void
LEFT1(TREE *x, TREE *y)
{
	TREE *node[3];
	TREE **npp = node;
	TREE *tp;

	if ((RIGHT(x) = LEFT(y)) != NULL) {
		unprotect(*npp++ = RIGHT(x));
		PARENT(RIGHT(x)) = x;
	}
	if ((PARENT(y) = PARENT(x)) != NULL) {
		unprotect(*npp++ = PARENT(x));
		if (LEFT(PARENT(x)) == x)
			LEFT(PARENT(y)) = y;
		else
			RIGHT(PARENT(y)) = y;
	}
	LEFT(y) = x;
	PARENT(x) = y;

	*npp = NULL;
	npp = node;
	while ((tp = *npp++) != NULL)
		if (tp != x && tp != y && !in_list(tp, npp))
			protect(tp);
}

static void
RIGHT1(TREE *x, TREE *y)
{
	TREE *node[3];
	TREE **npp = node;
	TREE *tp;

	if ((LEFT(x) = RIGHT(y)) != NULL) {
		unprotect(*npp++ = LEFT(x));
		PARENT(LEFT(x)) = x;
	}
	if ((PARENT(y) = PARENT(x)) != NULL) {
		unprotect(*npp++ = PARENT(x));
		if (LEFT(PARENT(x)) == x)
			LEFT(PARENT(y)) = y;
		else
			RIGHT(PARENT(y)) = y;
	}
	RIGHT(y) = x;
	PARENT(x) = y;

	*npp = NULL;
	npp = node;
	while ((tp = *npp++) != NULL)
		if (tp != x && tp != y && !in_list(tp, npp))
			protect(tp);
}

static void
BULEFT2(TREE *x, TREE *y, TREE *z)
{
	TREE *node[4];
	TREE **npp = node;
	TREE *tp;

	if ((RIGHT(x) = LEFT(y)) != NULL) {
		unprotect(*npp++ = RIGHT(x));
		PARENT(RIGHT(x)) = x;
	}
	if ((RIGHT(y) = LEFT(z)) != NULL) {
		unprotect(*npp++ = RIGHT(y));
		PARENT(RIGHT(y)) = y;
	}
	if ((PARENT(z) = PARENT(x)) != NULL) {
		unprotect(*npp++ = PARENT(x));
		if (LEFT(PARENT(x)) == x)
			LEFT(PARENT(z)) = z;
		else
			RIGHT(PARENT(z)) = z;
	}
	LEFT(z) = y;
	PARENT(y) = z;
	LEFT(y) = x;
	PARENT(x) = y;

	*npp = NULL;
	npp = node;
	while ((tp = *npp++) != NULL)
		if (tp != x && tp != y && tp != z && !in_list(tp, npp))
			protect(tp);
}

static void
BURIGHT2(TREE *x, TREE *y, TREE *z)
{
	TREE *node[4];
	TREE **npp = node;
	TREE *tp;

	if ((LEFT(x) = RIGHT(y)) != NULL) {
		unprotect(*npp++ = LEFT(x));
		PARENT(LEFT(x)) = x;
	}
	if ((LEFT(y) = RIGHT(z)) != NULL) {
		unprotect(*npp++ = LEFT(y));
		PARENT(LEFT(y)) = y;
	}
	if ((PARENT(z) = PARENT(x)) != NULL) {
		unprotect(*npp++ = PARENT(x));
		if (LEFT(PARENT(x)) == x)
			LEFT(PARENT(z)) = z;
		else
			RIGHT(PARENT(z)) = z;
	}
	RIGHT(z) = y;
	PARENT(y) = z;
	RIGHT(y) = x;
	PARENT(x) = y;

	*npp = NULL;
	npp = node;
	while ((tp = *npp++) != NULL)
		if (tp != x && tp != y && tp != z && !in_list(tp, npp))
			protect(tp);
}

static void
TDLEFT2(TREE *x, TREE *y, TREE *z)
{
	TREE *node[3];
	TREE **npp = node;
	TREE *tp;

	if ((RIGHT(y) = LEFT(z)) != NULL) {
		unprotect(*npp++ = RIGHT(y));
		PARENT(RIGHT(y)) = y;
	}
	if ((PARENT(z) = PARENT(x)) != NULL) {
		unprotect(*npp++ = PARENT(x));
		if (LEFT(PARENT(x)) == x)
			LEFT(PARENT(z)) = z;
		else
			RIGHT(PARENT(z)) = z;
	}
	PARENT(x) = z;
	LEFT(z) = x;

	*npp = NULL;
	npp = node;
	while ((tp = *npp++) != NULL)
		if (tp != x && tp != y && tp != z && !in_list(tp, npp))
			protect(tp);
}

#if 0	/* Not used, for now */
static void
TDRIGHT2(TREE *x, TREE *y, TREE *z)
{
	TREE *node[3];
	TREE **npp = node;
	TREE *tp;

	if ((LEFT(y) = RIGHT(z)) != NULL) {
		unprotect(*npp++ = LEFT(y));
		PARENT(LEFT(y)) = y;
	}
	if ((PARENT(z) = PARENT(x)) != NULL) {
		unprotect(*npp++ = PARENT(x));
		if (LEFT(PARENT(x)) == x)
			LEFT(PARENT(z)) = z;
		else
			RIGHT(PARENT(z)) = z;
	}
	PARENT(x) = z;
	RIGHT(z) = x;

	*npp = NULL;
	npp = node;
	while ((tp = *npp++) != NULL)
		if (tp != x && tp != y && tp != z && !in_list(tp, npp))
			protect(tp);
}
#endif

/*
 *	Delete a tree element
 */
static void
t_delete(TREE *op)
{
	TREE *tp, *sp, *gp;

	/* if this is a non-tree node */
	if (ISNOTREE(op)) {
		tp = LINKBAK(op);
		unprotect(tp);
		if ((sp = LINKFOR(op)) != NULL) {
			unprotect(sp);
			LINKBAK(sp) = tp;
			protect(sp);
		}
		LINKFOR(tp) = sp;
		protect(tp);
		return;
	}

	/* make op the root of the tree */
	if (PARENT(op))
		t_splay(op);

	/* if this is the start of a list */
	if ((tp = LINKFOR(op)) != NULL) {
		unprotect(tp);
		PARENT(tp) = NULL;
		if ((sp = LEFT(op)) != NULL) {
			unprotect(sp);
			PARENT(sp) = tp;
			protect(sp);
		}
		LEFT(tp) = sp;

		if ((sp = RIGHT(op)) != NULL) {
			unprotect(sp);
			PARENT(sp) = tp;
			protect(sp);
		}
		RIGHT(tp) = sp;

		Root = tp;
		protect(tp);
		return;
	}

	/* if op has a non-null left subtree */
	if ((tp = LEFT(op)) != NULL) {
		unprotect(tp);
		PARENT(tp) = NULL;
		if (RIGHT(op)) {
			/* make the right-end of the left subtree its root */
			while ((sp = RIGHT(tp)) != NULL) {
				unprotect(sp);
				if ((gp = RIGHT(sp)) != NULL) {
					unprotect(gp);
					TDLEFT2(tp, sp, gp);
					protect(sp);
					protect(tp);
					tp = gp;
				} else {
					LEFT1(tp, sp);
					protect(tp);
					tp = sp;
				}
			}

			/* hook the right subtree of op to the above elt */
			RIGHT(tp) = sp = RIGHT(op);
			unprotect(sp);
			PARENT(sp) = tp;
			protect(sp);
		}
		protect(tp);
	} else if ((tp = RIGHT(op)) != NULL) {	/* no left subtree */
		unprotect(tp);
		PARENT(tp) = NULL;
		protect(tp);
	}

	Root = tp;
}

/*
 *	Bottom up splaying (simple version).
 *	The basic idea is to roughly cut in half the
 *	path from Root to tp and make tp the new root.
 */
static void
t_splay(TREE *tp)
{
	TREE *pp, *gp;

	/* iterate until tp is the root */
	while ((pp = PARENT(tp)) != NULL) {
		unprotect(pp);
		/* grandparent of tp */
		gp = PARENT(pp);
		if (gp)
			unprotect(gp);

		/* x is a left child */
		if (LEFT(pp) == tp) {
			if (gp && LEFT(gp) == pp) {
				BURIGHT2(gp, pp, tp);
				protect(gp);
			} else {
				if (gp)
					protect(gp);
				RIGHT1(pp, tp);
			}
		} else {
			ASSERT(RIGHT(pp) == tp);
			if (gp && RIGHT(gp) == pp) {
				BULEFT2(gp, pp, tp);
				protect(gp);
			} else {
				if (gp)
					protect(gp);
				LEFT1(pp, tp);
			}
		}
		protect(pp);
		unprotect(tp);	/* just in case */
	}
}

void
free(void *old)
{
	(void) mutex_lock(&__watch_malloc_lock);
	free_unlocked(old);
	(void) mutex_unlock(&__watch_malloc_lock);
}


static void
free_unlocked(void *old)
{
	if (old != NULL)
		realfree(old);
}


/*
 * memalign(align,nbytes)
 *
 * Description:
 *	Returns a block of specified size on a specified alignment boundary.
 *
 * Algorithm:
 *	Malloc enough to ensure that a block can be aligned correctly.
 *	Find the alignment point and return the fragments
 *	before and after the block.
 *
 * Errors:
 *	Returns NULL and sets errno as follows:
 *	[EINVAL]
 *		if nbytes = 0,
 *		or if alignment is misaligned,
 *		or if the heap has been detectably corrupted.
 *	[ENOMEM]
 *		if the requested memory could not be allocated.
 */

#define	misaligned(p)		((unsigned)(p) & 3)
		/* 4-byte "word" alignment is considered ok in LP64 */
#define	nextblk(p, size)	((TREE *)((char *)(p) + (size)))

void *
memalign(size_t align, size_t nbytes)
{
	size_t	reqsize;	/* Num of bytes to get from malloc() */
	TREE	*p;		/* Ptr returned from malloc() */
	TREE	*blk;		/* For addressing fragment blocks */
	size_t	blksize;	/* Current (shrinking) block size */
	TREE	*alignedp;	/* Ptr to properly aligned boundary */
	TREE	*aligned_blk;	/* The block to be returned */
	size_t	frag_size;	/* size of fragments fore and aft */
	size_t	x;

	/*
	 * check for valid size and alignment parameters
	 * MAX_ALIGN check prevents overflow in later calculation.
	 */
	if (nbytes == 0 || misaligned(align) || align == 0 ||
	    align > MAX_ALIGN) {
		errno = EINVAL;
		return (NULL);
	}

	/*
	 * Malloc enough memory to guarantee that the result can be
	 * aligned correctly. The worst case is when malloc returns
	 * a block so close to the next alignment boundary that a
	 * fragment of minimum size cannot be created.  In order to
	 * make sure we can handle this, we need to force the
	 * alignment to be at least as large as the minimum frag size
	 * (MINSIZE + WORDSIZE).
	 */

	/* check for size that could overflow ROUND calculation */
	if (nbytes > MAX_MALLOC) {
		errno = ENOMEM;
		return (NULL);
	}
	ROUND(nbytes);
	if (nbytes < MINSIZE)
		nbytes = MINSIZE;
	ROUND(align);
	while (align < MINSIZE + WORDSIZE)
		align <<= 1;
	reqsize = nbytes + align + (MINSIZE + WORDSIZE);
	/* check for overflow */
	if (reqsize < nbytes) {
		errno = ENOMEM;
		return (NULL);
	}
	p = (TREE *) malloc(reqsize);
	if (p == (TREE *) NULL) {
		/* malloc sets errno */
		return (NULL);
	}
	(void) mutex_lock(&__watch_malloc_lock);

	/*
	 * get size of the entire block (overhead and all)
	 */
	/* LINTED improper alignment */
	blk = BLOCK(p);			/* back up to get length word */
	unprotect(blk);
	blksize = SIZE(blk);
	CLRBITS01(blksize);

	/*
	 * locate the proper alignment boundary within the block.
	 */
	x = (size_t)p;
	if (x % align != 0)
		x += align - (x % align);
	alignedp = (TREE *)x;
	/* LINTED improper alignment */
	aligned_blk = BLOCK(alignedp);

	/*
	 * Check out the space to the left of the alignment
	 * boundary, and split off a fragment if necessary.
	 */
	frag_size = (size_t)aligned_blk - (size_t)blk;
	if (frag_size != 0) {
		/*
		 * Create a fragment to the left of the aligned block.
		 */
		if (frag_size < MINSIZE + WORDSIZE) {
			/*
			 * Not enough space. So make the split
			 * at the other end of the alignment unit.
			 * We know this yields enough space, because
			 * we forced align >= MINSIZE + WORDSIZE above.
			 */
			frag_size += align;
			/* LINTED improper alignment */
			aligned_blk = nextblk(aligned_blk, align);
		}
		blksize -= frag_size;
		SIZE(aligned_blk) = blksize | BIT0;
		frag_size -= WORDSIZE;
		SIZE(blk) = frag_size | BIT0 | ISBIT1(SIZE(blk));
		free_unlocked(DATA(blk));
		/*
		 * free_unlocked(DATA(blk)) has the side-effect of calling
		 * protect() on the block following blk, that is, aligned_blk.
		 * We recover from this by unprotect()ing it here.
		 */
		unprotect(aligned_blk);
	}

	/*
	 * Is there a (sufficiently large) fragment to the
	 * right of the aligned block?
	 */
	frag_size = blksize - nbytes;
	if (frag_size >= MINSIZE + WORDSIZE) {
		/*
		 * split and free a fragment on the right
		 */
		blksize = SIZE(aligned_blk);
		SIZE(aligned_blk) = nbytes;
		/* LINTED improper alignment */
		blk = NEXT(aligned_blk);
		SETOLD01(SIZE(aligned_blk), blksize);
		frag_size -= WORDSIZE;
		SIZE(blk) = frag_size | BIT0;
		free_unlocked(DATA(blk));
	}
	copy_pattern(LIVEPAT, aligned_blk);
	protect(aligned_blk);
	(void) mutex_unlock(&__watch_malloc_lock);
	return (DATA(aligned_blk));
}

void *
valloc(size_t size)
{
	static unsigned pagesize;
	if (!pagesize)
		pagesize = _sysconf(_SC_PAGESIZE);
	return (memalign(pagesize, size));
}

void *
calloc(size_t num, size_t size)
{
	void *mp;
	size_t total;

	total = num * size;

	/* check for overflow */
	if (num != 0 && total / num != size) {
		errno = ENOMEM;
		return (NULL);
	}
	if ((mp = malloc(total)) != NULL)
		(void) memset(mp, 0, total);
	return (mp);
}

/* ARGSUSED1 */
void
cfree(void *p, size_t num, size_t size)
{
	free(p);
}

typedef struct {
	long cmd;
	prwatch_t prwatch;
} ctl_t;

static pid_t my_pid = 0;	/* to check for whether we fork()d */
static int dont_watch = 0;
static int do_stop = 0;
static int ctlfd = -1;
struct stat ctlstatb;
static int wflags = WA_WRITE;

static void
init_watch()
{
	char str[80];
	char *s;

	my_pid = getpid();

	dont_watch = 1;

	if ((s = getenv("MALLOC_DEBUG")) == NULL)
		return;

	s = strncpy(str, s, sizeof (str));
	while (s != NULL) {
		char *e = strchr(s, ',');
		if (e)
			*e++ = '\0';
		if (strcmp(s, "STOP") == 0)
			do_stop = 1;
		else if (strcmp(s, "WATCH") == 0)
			dont_watch = 0;
		else if (strcmp(s, "RW") == 0) {
			dont_watch = 0;
			wflags = WA_READ|WA_WRITE;
		}
		s = e;
	}

	if (dont_watch)
		return;

	if ((ctlfd = open("/proc/self/ctl", O_WRONLY)) < 0 ||
	    fstat(ctlfd, &ctlstatb) != 0) {
		if (ctlfd >= 0)
			(void) close(ctlfd);
		ctlfd = -1;
		dont_watch = 1;
		return;
	}
	/* close-on-exec */
	(void) fcntl(ctlfd, F_SETFD, 1);

	if (do_stop) {
		int pfd;
		pstatus_t pstatus;
		struct {
			long cmd;
			fltset_t fltset;
		} ctl;

		/*
		 * Play together with some /proc controller
		 * that has set other stop-on-fault flags.
		 */
		premptyset(&ctl.fltset);
		if ((pfd = open("/proc/self/status", O_RDONLY)) >= 0) {
			if (read(pfd, &pstatus, sizeof (pstatus))
			    == sizeof (pstatus))
				ctl.fltset = pstatus.pr_flttrace;
			(void) close(pfd);
		}
		praddset(&ctl.fltset, FLTWATCH);
		ctl.cmd = PCSFAULT;
		(void) write(ctlfd, &ctl, sizeof (ctl));
	}
}

static int
nowatch()
{
	struct stat statb;

	if (dont_watch)
		return (1);
	if (ctlfd < 0)	/* first time */
		init_watch();
	else if (fstat(ctlfd, &statb) != 0 ||
	    statb.st_dev != ctlstatb.st_dev ||
	    statb.st_ino != ctlstatb.st_ino) {
		/*
		 * Someone closed our file descriptor.
		 * Just open another one.
		 */
		if ((ctlfd = open("/proc/self/ctl", O_WRONLY)) < 0 ||
		    fstat(ctlfd, &ctlstatb) != 0) {
			if (ctlfd >= 0)
				(void) close(ctlfd);
			ctlfd = -1;
			dont_watch = 1;
			return (1);
		}
		/* close-on-exec */
		(void) fcntl(ctlfd, F_SETFD, 1);
	}
	if (my_pid != getpid()) {
		/*
		 * We fork()d since the last call to the allocator.
		 * watchpoints are not inherited across fork().
		 * XXX: how to recover from this ???
		 */
		dont_watch = 1;
		(void) close(ctlfd);
		ctlfd = -1;
	}
	return (dont_watch);
}

static void
protect(TREE *tp)
{
	ctl_t ctl;
	size_t size, sz;

	if (nowatch())
		return;
	if (tp == NULL || DATA(tp) == Baddr)
		return;

	sz = size = SIZE(tp);
	CLRBITS01(size);
	if (size == 0)
		return;
	if (ISBIT0(sz))		/* block is busy, protect only the head */
		size = 0;
	ctl.cmd = PCWATCH;
	ctl.prwatch.pr_vaddr = (uintptr_t)tp;
	ctl.prwatch.pr_size = size + WORDSIZE;
	ctl.prwatch.pr_wflags = wflags;
	(void) write(ctlfd, &ctl, sizeof (ctl));
}

static void
unprotect(TREE *tp)
{
	ctl_t ctl;

	if (nowatch())
		return;
	if (tp == NULL || DATA(tp) == Baddr)
		return;

	ctl.cmd = PCWATCH;
	ctl.prwatch.pr_vaddr = (uintptr_t)tp;
	ctl.prwatch.pr_size = WORDSIZE;		/* size is arbitrary */
	ctl.prwatch.pr_wflags = 0;		/* clear the watched area */
	(void) write(ctlfd, &ctl, sizeof (ctl));
}

static void
malloc_prepare()
{
	(void) mutex_lock(&__watch_malloc_lock);
}

static void
malloc_release()
{
	(void) mutex_unlock(&__watch_malloc_lock);
}

#pragma init(malloc_init)
static void
malloc_init(void)
{
	(void) pthread_atfork(malloc_prepare, malloc_release, malloc_release);
}



/* ============================================================================
 * SOURCE 6/13: minix4\libos\lib_legacy\openssl\lib\posix\netiso\xebec\malloc.c
 * Size: 2,980 bytes, Lines: 137
 * Hash: f003fd0706ce...
 * ============================================================================ */

/* $Header: /n/fast/usr/lsrc/mach/CVS/lites/server/netiso/xebec/malloc.c,v 1.1.1.1 1995/03/02 21:49:57 mike Exp $ */
/* $Source: /n/fast/usr/lsrc/mach/CVS/lites/server/netiso/xebec/malloc.c,v $ */
/*
 * This code is such a kludge that I don't want to put my name on it.
 * It was a ridiculously fast hack and needs rewriting.
 * However it does work...
 */

/* 
 * a simple malloc
 * it might be brain-damaged but for the purposes of xebec
 * it's a whole lot faster than the c library malloc 
 */

#include <stdio.h>
#include "malloc.h"
#include "debug.h"
#define CHUNKSIZE 4096*2

static char *hiwat, *highend;
int bytesmalloced=0;
int byteswasted = 0;


init_alloc()
{
#ifdef LINT
	hiwat = 0;
	highend = 0;
#else LINT
	extern char *sbrk();

	hiwat = (char *) sbrk(0);
	hiwat = (char *)((unsigned)(hiwat + 3) & ~0x3);
	highend = hiwat;
#endif LINT
}

HIWAT(s)
char *s;
{
	IFDEBUG(M)
		fprintf(stdout, "HIWAT 0x%x  %s\n", hiwat,s);
		fflush(stdout);
	ENDDEBUG
}

#define MIN(x,y) ((x<y)?x:y)

char *Malloc(x)
int x;
{
	char *c;
	extern char *sbrk();
	static int firsttime=1;
	int total = x;
	int first_iter = 1;
	char *returnvalue;

	IFDEBUG(N)
		fprintf(stdout, "Malloc 0x%x, %d, bytesmalloced %d\n",
			total,total, bytesmalloced);
		fflush(stdout);
	ENDDEBUG
	IFDEBUG(M)
		fprintf(stdout, "Malloc 0x%x, %d, hiwat 0x%x\n",
			total,total, hiwat);
		fflush(stdout);
	ENDDEBUG
	if(firsttime) {
		hiwat = sbrk(0);
		if(((unsigned)(hiwat) & 0x3)) {
			bytesmalloced = 4 - (int) ((unsigned)(hiwat) & 0x3);
			hiwat = sbrk( bytesmalloced );
		} else 
			bytesmalloced = 0;
		firsttime = 0;
		highend = hiwat;
	}
	while( total ) {
		x = MIN(CHUNKSIZE, total);
		if(total != x)  {
			IFDEBUG(N)
				fprintf(stdout, "BIG Malloc tot %d, x %d, left %d net %d\n",
					total,x, total-x, bytesmalloced);
				fflush(stdout);
			ENDDEBUG
		}
		if ( (hiwat + x) > highend) {
			c = sbrk(CHUNKSIZE);
			IFDEBUG(M)
				fprintf(stdout, "hiwat 0x%x, x 0x%x, highend 0x%x, c 0x%x\n",
						hiwat, x, highend, c);
				fflush(stdout);
			ENDDEBUG
			if( c == (char *) -1 ) {
				fprintf(stderr, "Ran out of memory!\n");
				Exit(-1);
			}
			if(first_iter) {
				returnvalue = c;
				first_iter = 0;
			}
			bytesmalloced +=  CHUNKSIZE;
			IFDEBUG(m)
				if (highend != c) {
					fprintf(OUT, "warning: %d wasted bytes!\n", highend - hiwat);
				fprintf(OUT, " chunksize 0x%x,  x 0x%x \n", CHUNKSIZE, x);
				}
			ENDDEBUG
			highend = c + CHUNKSIZE;
			hiwat = c;
		}
		c = hiwat;
		if(first_iter) {
			returnvalue = c;
			first_iter = 0;
		}
		hiwat += x;
		total -= x;
	}
	if((unsigned)hiwat & 0x3) {
		byteswasted += (int)((unsigned)(hiwat) & 0x3);
		hiwat = (char *)((unsigned)(hiwat + 3) & ~0x3);
	}
	IFDEBUG(M)
		fprintf(stdout, "Malloc = 0x%x, bytesm 0x%x, wasted 0x%x, hiwat 0x%x\n",
			returnvalue, bytesmalloced, byteswasted, hiwat);
	ENDDEBUG
	IFDEBUG(N)
		fprintf(stdout, "Malloc returns 0x%x, sbrk(0) 0x%x\n", returnvalue, sbrk(0));
		fflush(stdout);
	ENDDEBUG
	return(returnvalue);
}




/* ============================================================================
 * SOURCE 7/13: minix4\libos\lib_legacy\libumem\common\malloc.c
 * Size: 8,765 bytes, Lines: 423
 * Hash: 244b08738313...
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
 * Copyright 2008 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/sysmacros.h>
#include "umem_base.h"
#include "misc.h"

/*
 * malloc_data_t is an 8-byte structure which is located "before" the pointer
 * returned from {m,c,re}alloc and memalign.  The first four bytes give
 * information about the buffer, and the second four bytes are a status byte.
 *
 * See umem_impl.h for the various magic numbers used, and the size
 * encode/decode macros.
 *
 * The 'size' of the buffer includes the tags.  That is, we encode the
 * argument to umem_alloc(), not the argument to malloc().
 */

typedef struct malloc_data {
	uint32_t malloc_size;
	uint32_t malloc_stat; /* = UMEM_MALLOC_ENCODE(state, malloc_size) */
} malloc_data_t;

/*
 * Because we do not support ptcumem on non-x86 today, we have to create these
 * weak aliases.
 */
#ifndef _x86
#pragma weak malloc = umem_malloc
#pragma weak free = umem_malloc_free
#endif /* !_x86 */

void *
umem_malloc(size_t size_arg)
{
#ifdef _LP64
	uint32_t high_size = 0;
#endif
	size_t size;

	malloc_data_t *ret;
	size = size_arg + sizeof (malloc_data_t);

#ifdef _LP64
	if (size > UMEM_SECOND_ALIGN) {
		size += sizeof (malloc_data_t);
		high_size = (size >> 32);
	}
#endif
	if (size < size_arg) {
		errno = ENOMEM;			/* overflow */
		return (NULL);
	}
	ret = (malloc_data_t *)_umem_alloc(size, UMEM_DEFAULT);
	if (ret == NULL) {
		if (size <= UMEM_MAXBUF)
			errno = EAGAIN;
		else
			errno = ENOMEM;
		return (NULL);
#ifdef _LP64
	} else if (high_size > 0) {
		uint32_t low_size = (uint32_t)size;

		/*
		 * uses different magic numbers to make it harder to
		 * undetectably corrupt
		 */
		ret->malloc_size = high_size;
		ret->malloc_stat = UMEM_MALLOC_ENCODE(MALLOC_MAGIC, high_size);
		ret++;

		ret->malloc_size = low_size;
		ret->malloc_stat = UMEM_MALLOC_ENCODE(MALLOC_OVERSIZE_MAGIC,
		    low_size);
		ret++;
	} else if (size > UMEM_SECOND_ALIGN) {
		uint32_t low_size = (uint32_t)size;

		ret++; /* leave the first 8 bytes alone */

		ret->malloc_size = low_size;
		ret->malloc_stat = UMEM_MALLOC_ENCODE(MALLOC_SECOND_MAGIC,
		    low_size);
		ret++;
#endif
	} else {
		ret->malloc_size = size;
		ret->malloc_stat = UMEM_MALLOC_ENCODE(MALLOC_MAGIC, size);
		ret++;
	}
	return ((void *)ret);
}

void *
calloc(size_t nelem, size_t elsize)
{
	size_t size = nelem * elsize;
	void *retval;

	if (nelem > 0 && elsize > 0 && size/nelem != elsize) {
		errno = ENOMEM;				/* overflow */
		return (NULL);
	}

	retval = malloc(size);
	if (retval == NULL)
		return (NULL);

	(void) memset(retval, 0, size);
	return (retval);
}

/*
 * memalign uses vmem_xalloc to do its work.
 *
 * in 64-bit, the memaligned buffer always has two tags.  This simplifies the
 * code.
 */

void *
memalign(size_t align, size_t size_arg)
{
	size_t size;
	uintptr_t phase;

	void *buf;
	malloc_data_t *ret;

	size_t overhead;

	if (size_arg == 0 || align == 0 || (align & (align - 1)) != 0) {
		errno = EINVAL;
		return (NULL);
	}

	/*
	 * if malloc provides the required alignment, use it.
	 */
	if (align <= UMEM_ALIGN ||
	    (align <= UMEM_SECOND_ALIGN && size_arg >= UMEM_SECOND_ALIGN))
		return (malloc(size_arg));

#ifdef _LP64
	overhead = 2 * sizeof (malloc_data_t);
#else
	overhead = sizeof (malloc_data_t);
#endif

	ASSERT(overhead <= align);

	size = size_arg + overhead;
	phase = align - overhead;

	if (umem_memalign_arena == NULL && umem_init() == 0) {
		errno = ENOMEM;
		return (NULL);
	}

	if (size < size_arg) {
		errno = ENOMEM;			/* overflow */
		return (NULL);
	}

	buf = vmem_xalloc(umem_memalign_arena, size, align, phase,
	    0, NULL, NULL, VM_NOSLEEP);

	if (buf == NULL) {
		if ((size_arg + align) <= UMEM_MAXBUF)
			errno = EAGAIN;
		else
			errno = ENOMEM;

		return (NULL);
	}

	ret = (malloc_data_t *)buf;
	{
		uint32_t low_size = (uint32_t)size;

#ifdef _LP64
		uint32_t high_size = (uint32_t)(size >> 32);

		ret->malloc_size = high_size;
		ret->malloc_stat = UMEM_MALLOC_ENCODE(MEMALIGN_MAGIC,
		    high_size);
		ret++;
#endif

		ret->malloc_size = low_size;
		ret->malloc_stat = UMEM_MALLOC_ENCODE(MEMALIGN_MAGIC, low_size);
		ret++;
	}

	ASSERT(P2PHASE((uintptr_t)ret, align) == 0);
	ASSERT((void *)((uintptr_t)ret - overhead) == buf);

	return ((void *)ret);
}

void *
valloc(size_t size)
{
	return (memalign(pagesize, size));
}

/*
 * process_free:
 *
 * Pulls information out of a buffer pointer, and optionally free it.
 * This is used by free() and realloc() to process buffers.
 *
 * On failure, calls umem_err_recoverable() with an appropriate message
 * On success, returns the data size through *data_size_arg, if (!is_free).
 *
 * Preserves errno, since free()'s semantics require it.
 */

static int
process_free(void *buf_arg,
    int do_free,		/* free the buffer, or just get its size? */
    size_t *data_size_arg)	/* output: bytes of data in buf_arg */
{
	malloc_data_t *buf;

	void *base;
	size_t size;
	size_t data_size;

	const char *message;
	int old_errno = errno;

	buf = (malloc_data_t *)buf_arg;

	buf--;
	size = buf->malloc_size;

	switch (UMEM_MALLOC_DECODE(buf->malloc_stat, size)) {

	case MALLOC_MAGIC:
		base = (void *)buf;
		data_size = size - sizeof (malloc_data_t);

		if (do_free)
			buf->malloc_stat = UMEM_FREE_PATTERN_32;

		goto process_malloc;

#ifdef _LP64
	case MALLOC_SECOND_MAGIC:
		base = (void *)(buf - 1);
		data_size = size - 2 * sizeof (malloc_data_t);

		if (do_free)
			buf->malloc_stat = UMEM_FREE_PATTERN_32;

		goto process_malloc;

	case MALLOC_OVERSIZE_MAGIC: {
		size_t high_size;

		buf--;
		high_size = buf->malloc_size;

		if (UMEM_MALLOC_DECODE(buf->malloc_stat, high_size) !=
		    MALLOC_MAGIC) {
			message = "invalid or corrupted buffer";
			break;
		}

		size += high_size << 32;

		base = (void *)buf;
		data_size = size - 2 * sizeof (malloc_data_t);

		if (do_free) {
			buf->malloc_stat = UMEM_FREE_PATTERN_32;
			(buf + 1)->malloc_stat = UMEM_FREE_PATTERN_32;
		}

		goto process_malloc;
	}
#endif

	case MEMALIGN_MAGIC: {
		size_t overhead = sizeof (malloc_data_t);

#ifdef _LP64
		size_t high_size;

		overhead += sizeof (malloc_data_t);

		buf--;
		high_size = buf->malloc_size;

		if (UMEM_MALLOC_DECODE(buf->malloc_stat, high_size) !=
		    MEMALIGN_MAGIC) {
			message = "invalid or corrupted buffer";
			break;
		}
		size += high_size << 32;

		/*
		 * destroy the main tag's malloc_stat
		 */
		if (do_free)
			(buf + 1)->malloc_stat = UMEM_FREE_PATTERN_32;
#endif

		base = (void *)buf;
		data_size = size - overhead;

		if (do_free)
			buf->malloc_stat = UMEM_FREE_PATTERN_32;

		goto process_memalign;
	}
	default:
		if (buf->malloc_stat == UMEM_FREE_PATTERN_32)
			message = "double-free or invalid buffer";
		else
			message = "invalid or corrupted buffer";
		break;
	}

	umem_err_recoverable("%s(%p): %s\n",
	    do_free? "free" : "realloc", buf_arg, message);

	errno = old_errno;
	return (0);

process_malloc:
	if (do_free)
		_umem_free(base, size);
	else
		*data_size_arg = data_size;

	errno = old_errno;
	return (1);

process_memalign:
	if (do_free)
		vmem_xfree(umem_memalign_arena, base, size);
	else
		*data_size_arg = data_size;

	errno = old_errno;
	return (1);
}

void
umem_malloc_free(void *buf)
{
	if (buf == NULL)
		return;

	/*
	 * Process buf, freeing it if it is not corrupt.
	 */
	(void) process_free(buf, 1, NULL);
}

void *
realloc(void *buf_arg, size_t newsize)
{
	size_t oldsize;
	void *buf;

	if (buf_arg == NULL)
		return (malloc(newsize));

	if (newsize == 0) {
		free(buf_arg);
		return (NULL);
	}

	/*
	 * get the old data size without freeing the buffer
	 */
	if (process_free(buf_arg, 0, &oldsize) == 0) {
		errno = EINVAL;
		return (NULL);
	}

	if (newsize == oldsize)		/* size didn't change */
		return (buf_arg);

	buf = malloc(newsize);
	if (buf == NULL)
		return (NULL);

	(void) memcpy(buf, buf_arg, MIN(newsize, oldsize));
	free(buf_arg);
	return (buf);
}



/* ============================================================================
 * SOURCE 8/13: minix4\libos\lib_legacy\libmalloc\common\malloc.c
 * Size: 31,834 bytes, Lines: 1,203
 * Hash: 06a4c6ed3315...
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
 * Copyright 2008 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/

#include <sys/types.h>

#ifndef debug
#define	NDEBUG
#endif

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "assert.h"
#include "malloc.h"
#include "mallint.h"
#include <thread.h>
#include <pthread.h>
#include <synch.h>
#include <unistd.h>
#include <limits.h>

static mutex_t mlock = DEFAULTMUTEX;
static ssize_t freespace(struct holdblk *);
static void *malloc_unlocked(size_t, int);
static void *realloc_unlocked(void *, size_t);
static void free_unlocked(void *);
static void *morecore(size_t);

/*
 * use level memory allocater (malloc, free, realloc)
 *
 *	-malloc, free, realloc and mallopt form a memory allocator
 *	similar to malloc, free, and realloc.  The routines
 *	here are much faster than the original, with slightly worse
 *	space usage (a few percent difference on most input).  They
 *	do not have the property that data in freed blocks is left
 *	untouched until the space is reallocated.
 *
 *	-Memory is kept in the "arena", a singly linked list of blocks.
 *	These blocks are of 3 types.
 *		1. A free block.  This is a block not in use by the
 *		   user.  It has a 3 word header. (See description
 *		   of the free queue.)
 *		2. An allocated block.  This is a block the user has
 *		   requested.  It has only a 1 word header, pointing
 *		   to the next block of any sort.
 *		3. A permanently allocated block.  This covers space
 *		   aquired by the user directly through sbrk().  It
 *		   has a 1 word header, as does 2.
 *	Blocks of type 1 have the lower bit of the pointer to the
 *	nextblock = 0.  Blocks of type 2 and 3 have that bit set,
 *	to mark them busy.
 *
 *	-Unallocated blocks are kept on an unsorted doubly linked
 *	free list.
 *
 *	-Memory is allocated in blocks, with sizes specified by the
 *	user.  A circular first-fit startegy is used, with a roving
 *	head of the free queue, which prevents bunching of small
 *	blocks at the head of the queue.
 *
 *	-Compaction is performed at free time of any blocks immediately
 *	following the freed block.  The freed block will be combined
 *	with a preceding block during the search phase of malloc.
 *	Since a freed block is added at the front of the free queue,
 *	which is moved to the end of the queue if considered and
 *	rejected during the search, fragmentation only occurs if
 *	a block with a contiguious preceding block that is free is
 *	freed and reallocated on the next call to malloc.  The
 *	time savings of this strategy is judged to be worth the
 *	occasional waste of memory.
 *
 *	-Small blocks (of size < MAXSIZE)  are not allocated directly.
 *	A large "holding" block is allocated via a recursive call to
 *	malloc.  This block contains a header and ?????? small blocks.
 *	Holding blocks for a given size of small block (rounded to the
 *	nearest ALIGNSZ bytes) are kept on a queue with the property that any
 *	holding block with an unused small block is in front of any without.
 *	A list of free blocks is kept within the holding block.
 */

/*
 *	description of arena, free queue, holding blocks etc.
 *
 * New compiler and linker does not guarentee order of initialized data.
 * Define freeptr as arena[2-3] to guarentee it follows arena in memory.
 * Later code depends on this order.
 */

static struct header arena[4] = {
	    {0, 0, 0},
	    {0, 0, 0},
	    {0, 0, 0},
	    {0, 0, 0}
	};
				/*
				 * the second word is a minimal block to
				 * start the arena. The first is a busy
				 * block to be pointed to by the last block.
				 */

#define	freeptr (arena + 2)
				/* first and last entry in free list */
static struct header *arenaend;	/* ptr to block marking high end of arena */
static struct header *lastblk;	/* the highest block in the arena */
static struct holdblk **holdhead;   /* pointer to array of head pointers */
				    /* to holding block chains */
/*
 * In order to save time calculating indices, the array is 1 too
 * large, and the first element is unused
 *
 * Variables controlling algorithm, esp. how holding blocs are used
 */
static int numlblks = NUMLBLKS;
static int minhead = MINHEAD;
static int change = 0;	/* != 0, once param changes are no longer allowed */
static int fastct = FASTCT;
static unsigned int maxfast = MAXFAST;
/* number of small block sizes to map to one size */

static int grain = ALIGNSZ;

#ifdef debug
static int case1count = 0;

static void
checkq(void)
{
	register struct header *p;

	p = &freeptr[0];

	/* check forward */
	/*CSTYLED*/
	while (p != &freeptr[1]) {
		p = p->nextfree;
		assert(p->prevfree->nextfree == p);
	}

	/* check backward */
	/*CSTYLED*/
	while (p != &freeptr[0]) {
		p = p->prevfree;
		assert(p->nextfree->prevfree == p);
	}
}
#endif


/*
 * malloc(nbytes) - give a user nbytes to use
 */

void *
malloc(size_t nbytes)
{
	void *ret;

	(void) mutex_lock(&mlock);
	ret = malloc_unlocked(nbytes, 0);
	(void) mutex_unlock(&mlock);
	return (ret);
}

/*
 * Use malloc_unlocked() to get the address to start with; Given this
 * address, find out the closest address that aligns with the request
 * and return that address after doing some house keeping (refer to the
 * ascii art below).
 */
void *
memalign(size_t alignment, size_t size)
{
	void *alloc_buf;
	struct header *hd;
	size_t alloc_size;
	uintptr_t fr;
	static int realloc;

	if (size == 0 || alignment == 0 ||
	    (alignment & (alignment - 1)) != 0) {
		return (NULL);
	}
	if (alignment <= ALIGNSZ)
		return (malloc(size));

	alloc_size = size + alignment;
	if (alloc_size < size) { /* overflow */
		return (NULL);
	}

	(void) mutex_lock(&mlock);
	alloc_buf = malloc_unlocked(alloc_size, 1);
	(void) mutex_unlock(&mlock);

	if (alloc_buf == NULL)
		return (NULL);
	fr = (uintptr_t)alloc_buf;

	fr = (fr + alignment - 1) / alignment * alignment;

	if (fr == (uintptr_t)alloc_buf)
		return (alloc_buf);

	if ((fr - (uintptr_t)alloc_buf) <= HEADSZ) {
		/*
		 * we hit an edge case, where the space ahead of aligned
		 * address is not sufficient to hold 'header' and hence we
		 * can't free it. So double the allocation request.
		 */
		realloc++;
		free(alloc_buf);
		alloc_size = size + alignment*2;
		if (alloc_size < size) {
			return (NULL);
		}

		(void) mutex_lock(&mlock);
		alloc_buf = malloc_unlocked(alloc_size, 1);
		(void) mutex_unlock(&mlock);

		if (alloc_buf == NULL)
			return (NULL);
		fr = (uintptr_t)alloc_buf;

		fr = (fr + alignment - 1) / alignment * alignment;
		if (fr == (uintptr_t)alloc_buf)
			return (alloc_buf);
		if ((fr - (uintptr_t)alloc_buf) <= HEADSZ) {
			fr = fr + alignment;
		}
	}

	/*
	 *	+-------+		+-------+
	 *  +---| <a>   |		| <a>   |--+
	 *  |   +-------+<--alloc_buf-->+-------+  |
	 *  |   |	|		|	|  |
	 *  |   |	|		|	|  |
	 *  |   |	|		|	|  |
	 *  |   |	|	 hd-->  +-------+  |
	 *  |   |	|	    +---|  <b>  |<-+
	 *  |   |	|	    |   +-------+<--- fr
	 *  |   |	|	    |   |	|
	 *  |   |	|	    |   |	|
	 *  |   |	|	    |   |	|
	 *  |   |	|	    |   |	|
	 *  |   |	|	    |   |	|
	 *  |   |	|	    |   |	|
	 *  |   +-------+	    |   +-------+
	 *  +-->|  next |	    +-->|  next |
	 *	+-------+		+-------+
	 *
	 */
	hd = (struct header *)((char *)fr - minhead);
	(void) mutex_lock(&mlock);
	hd->nextblk = ((struct header *)((char *)alloc_buf - minhead))->nextblk;
	((struct header *)((char *)alloc_buf - minhead))->nextblk = SETBUSY(hd);
	(void) mutex_unlock(&mlock);
	free(alloc_buf);
	CHECKQ
	return ((void *)fr);
}

void *
valloc(size_t size)
{
	static unsigned pagesize;
	if (size == 0)
		return (NULL);

	if (!pagesize)
		pagesize = sysconf(_SC_PAGESIZE);

	return (memalign(pagesize, size));
}

/*
 * malloc_unlocked(nbytes, nosmall) - Do the real work for malloc
 */

static void *
malloc_unlocked(size_t nbytes, int nosmall)
{
	struct header *blk;
	size_t nb;	/* size of entire block we need */

	/* on first call, initialize */
	if (freeptr[0].nextfree == GROUND) {
		/* initialize arena */
		arena[1].nextblk = (struct header *)BUSY;
		arena[0].nextblk = (struct header *)BUSY;
		lastblk = arenaend = &(arena[1]);
		/* initialize free queue */
		freeptr[0].nextfree = &(freeptr[1]);
		freeptr[1].nextblk = &(arena[0]);
		freeptr[1].prevfree = &(freeptr[0]);
		/* mark that small blocks not init yet */
	}
	if (nbytes == 0)
		return (NULL);

	if (nbytes <= maxfast && !nosmall) {
		/*
		 * We can allocate out of a holding block
		 */
		struct holdblk *holdblk; /* head of right sized queue */
		struct lblk *lblk;	 /* pointer to a little block */
		struct holdblk *newhold;

		if (!change) {
			int i;
			/*
			 * This allocates space for hold block
			 * pointers by calling malloc recursively.
			 * Maxfast is temporarily set to 0, to
			 * avoid infinite recursion.  allocate
			 * space for an extra ptr so that an index
			 * is just ->blksz/grain, with the first
			 * ptr unused.
			 */
			change = 1;	/* change to algorithm params */
					/* no longer allowed */
			/*
			 * temporarily alter maxfast, to avoid
			 * infinite recursion
			 */
			maxfast = 0;
			holdhead = (struct holdblk **)
			    malloc_unlocked(sizeof (struct holdblk *) *
			    (fastct + 1), 0);
			if (holdhead == NULL)
				return (malloc_unlocked(nbytes, 0));
			for (i = 1; i <= fastct; i++) {
				holdhead[i] = HGROUND;
			}
			maxfast = fastct * grain;
		}
		/*
		 * Note that this uses the absolute min header size (MINHEAD)
		 * unlike the large block case which uses minhead
		 *
		 * round up to nearest multiple of grain
		 * code assumes grain is a multiple of MINHEAD
		 */
		/* round up to grain */
		nb = (nbytes + grain - 1) / grain * grain;
		holdblk = holdhead[nb / grain];
		nb = nb + MINHEAD;
		/*
		 * look for space in the holding block.  Blocks with
		 * space will be in front of those without
		 */
		if ((holdblk != HGROUND) && (holdblk->lfreeq != LGROUND))  {
			/* there is space */
			lblk = holdblk->lfreeq;

			/*
			 * Now make lfreeq point to a free block.
			 * If lblk has been previously allocated and
			 * freed, it has a valid pointer to use.
			 * Otherwise, lblk is at the beginning of
			 * the unallocated blocks at the end of
			 * the holding block, so, if there is room, take
			 * the next space.  If not, mark holdblk full,
			 * and move holdblk to the end of the queue
			 */
			if (lblk < holdblk->unused) {
				/* move to next holdblk, if this one full */
				if ((holdblk->lfreeq =
				    CLRSMAL(lblk->header.nextfree)) ==
				    LGROUND) {
					holdhead[(nb-MINHEAD) / grain] =
					    holdblk->nexthblk;
				}
			} else if (((char *)holdblk->unused + nb) <
			    ((char *)holdblk + HOLDSZ(nb))) {
				holdblk->unused = (struct lblk *)
				    ((char *)holdblk->unused+nb);
				holdblk->lfreeq = holdblk->unused;
			} else {
				holdblk->unused = (struct lblk *)
				    ((char *)holdblk->unused+nb);
				holdblk->lfreeq = LGROUND;
				holdhead[(nb-MINHEAD)/grain] =
				    holdblk->nexthblk;
			}
			/* mark as busy and small */
			lblk->header.holder = (struct holdblk *)SETALL(holdblk);
		} else {
			/* we need a new holding block */
			newhold = (struct holdblk *)
			    malloc_unlocked(HOLDSZ(nb), 0);
			if ((char *)newhold == NULL) {
				return (NULL);
			}
			/* add to head of free queue */
			if (holdblk != HGROUND) {
				newhold->nexthblk = holdblk;
				newhold->prevhblk = holdblk->prevhblk;
				holdblk->prevhblk = newhold;
				newhold->prevhblk->nexthblk = newhold;
			} else {
				newhold->nexthblk = newhold->prevhblk = newhold;
			}
			holdhead[(nb-MINHEAD)/grain] = newhold;
			/* set up newhold */
			lblk = (struct lblk *)(newhold->space);
			newhold->lfreeq = newhold->unused =
			    (struct lblk *)((char *)newhold->space+nb);
			lblk->header.holder = (struct holdblk *)SETALL(newhold);
			newhold->blksz = nb-MINHEAD;
		}
#ifdef debug
		assert(((struct holdblk *)CLRALL(lblk->header.holder))->blksz >=
		    nbytes);
#endif /* debug */
		return ((char *)lblk + MINHEAD);
	} else {
		/*
		 * We need an ordinary block
		 */
		struct header *newblk;	/* used for creating a block */

		/* get number of bytes we need */
		nb = nbytes + minhead;
		nb = (nb + ALIGNSZ - 1) / ALIGNSZ * ALIGNSZ;	/* align */
		nb = (nb > MINBLKSZ) ? nb : MINBLKSZ;
		/*
		 * see if there is a big enough block
		 * If none exists, you will get to freeptr[1].
		 * freeptr[1].next = &arena[0], so when you do the test,
		 * the result is a large positive number, since arena[0]
		 * comes before all blocks.  Arena[0] is marked busy so
		 * that it will not be compacted.  This kludge is for the
		 * sake of the almighty efficiency.
		 */
		/* check that a very large request won't cause an inf. loop */

		if ((freeptr[1].nextblk-&(freeptr[1])) < nb) {
			return (NULL);
		} else {
			struct header *next;		/* following block */
			struct header *nextnext;	/* block after next */

			blk = freeptr;
			do {
				blk = blk->nextfree;
				/* see if we can compact */
				next = blk->nextblk;
				if (!TESTBUSY(nextnext = next->nextblk)) {
					do {
						DELFREEQ(next);
						next = nextnext;
						nextnext = next->nextblk;
					} while (!TESTBUSY(nextnext));
					/*
					 * next will be at most == to lastblk,
					 * but I think the >= test is faster
					 */
					if (next >= arenaend)
						lastblk = blk;
					blk->nextblk = next;
				}
			} while (((char *)(next) - (char *)blk) < nb);
		}
		/*
		 * if we didn't find a block, get more memory
		 */
		if (blk == &(freeptr[1])) {
			/*
			 * careful coding could likely replace
			 * newend with arenaend
			 */
			struct header *newend;	/* new end of arena */
			ssize_t nget;	/* number of words to get */

			/*
			 * Three cases - 1. There is space between arenaend
			 *		    and the break value that will become
			 *		    a permanently allocated block.
			 *		 2. Case 1 is not true, and the last
			 *		    block is allocated.
			 *		 3. Case 1 is not true, and the last
			 *		    block is free
			 */
			if ((newblk = (struct header *)sbrk(0)) !=
			    (struct header *)((char *)arenaend + HEADSZ)) {
				/* case 1 */
#ifdef debug
				if (case1count++ > 0)
				    (void) write(2, "Case 1 hit more that once."
					" brk or sbrk?\n", 41);
#endif
				/* get size to fetch */
				nget = nb + HEADSZ;
				/* round up to a block */
				nget = (nget + BLOCKSZ - 1)/BLOCKSZ * BLOCKSZ;
				assert((uintptr_t)newblk % ALIGNSZ == 0);
				/* get memory */
				if (morecore(nget) == (void *)-1)
					return (NULL);
				/* add to arena */
				newend = (struct header *)((char *)newblk + nget
				    - HEADSZ);
				assert((uintptr_t)newblk % ALIGNSZ == 0);
				newend->nextblk = SETBUSY(&(arena[1]));
/* ???  newblk ?? */
				newblk->nextblk = newend;

				/*
				 * space becomes a permanently allocated block.
				 * This is likely not mt-safe as lock is not
				 * shared with brk or sbrk
				 */
				arenaend->nextblk = SETBUSY(newblk);
				/* adjust other pointers */
				arenaend = newend;
				lastblk = newblk;
				blk = newblk;
			} else if (TESTBUSY(lastblk->nextblk)) {
				/* case 2 */
				nget = (nb + BLOCKSZ - 1) / BLOCKSZ * BLOCKSZ;
				if (morecore(nget) == (void *)-1)
					return (NULL);
				/* block must be word aligned */
				assert(((uintptr_t)newblk%ALIGNSZ) == 0);
				/*
				 * stub at old arenaend becomes first word
				 * in blk
				 */
/* ???  	newblk = arenaend; */

				newend =
				    (struct header *)((char *)arenaend+nget);
				newend->nextblk = SETBUSY(&(arena[1]));
				arenaend->nextblk = newend;
				lastblk = blk = arenaend;
				arenaend = newend;
			} else {
				/* case 3 */
				/*
				 * last block in arena is at end of memory and
				 * is free
				 */
				/* 1.7 had this backward without cast */
				nget = nb -
				    ((char *)arenaend - (char *)lastblk);
				nget = (nget + (BLOCKSZ - 1)) /
				    BLOCKSZ * BLOCKSZ;
				assert(((uintptr_t)newblk % ALIGNSZ) == 0);
				if (morecore(nget) == (void *)-1)
					return (NULL);
				/* combine with last block, put in arena */
				newend = (struct header *)
				    ((char *)arenaend + nget);
				arenaend = lastblk->nextblk = newend;
				newend->nextblk = SETBUSY(&(arena[1]));
				/* set which block to use */
				blk = lastblk;
				DELFREEQ(blk);
			}
		} else {
			struct header *nblk;	/* next block */

			/* take block found of free queue */
			DELFREEQ(blk);
			/*
			 * make head of free queue immediately follow blk,
			 * unless blk was at the end of the queue
			 */
			nblk = blk->nextfree;
			if (nblk != &(freeptr[1])) {
				MOVEHEAD(nblk);
			}
		}
		/* blk now points to an adequate block */
		if (((char *)blk->nextblk - (char *)blk) - nb >= MINBLKSZ) {
			/* carve out the right size block */
			/* newblk will be the remainder */
			newblk = (struct header *)((char *)blk + nb);
			newblk->nextblk = blk->nextblk;
			/* mark the block busy */
			blk->nextblk = SETBUSY(newblk);
			ADDFREEQ(newblk);
			/* if blk was lastblk, make newblk lastblk */
			if (blk == lastblk)
				lastblk = newblk;
		} else {
			/* just mark the block busy */
			blk->nextblk = SETBUSY(blk->nextblk);
		}
	}
	CHECKQ
	assert((char *)CLRALL(blk->nextblk) -
	    ((char *)blk + minhead) >= nbytes);
	assert((char *)CLRALL(blk->nextblk) -
	    ((char *)blk + minhead) < nbytes + MINBLKSZ);
	return ((char *)blk + minhead);
}

/*
 * free(ptr) - free block that user thinks starts at ptr
 *
 *	input - ptr-1 contains the block header.
 *		If the header points forward, we have a normal
 *			block pointing to the next block
 *		if the header points backward, we have a small
 *			block from a holding block.
 *		In both cases, the busy bit must be set
 */

void
free(void *ptr)
{
	(void) mutex_lock(&mlock);
	free_unlocked(ptr);
	(void) mutex_unlock(&mlock);
}

/*
 * free_unlocked(ptr) - Do the real work for free()
 */

void
free_unlocked(void *ptr)
{
	struct holdblk *holdblk;	/* block holding blk */
	struct holdblk *oldhead;	/* former head of the hold block */
					/* queue containing blk's holder */

	if (ptr == NULL)
		return;
	if (TESTSMAL(((struct header *)((char *)ptr - MINHEAD))->nextblk)) {
		struct lblk	*lblk;	/* pointer to freed block */
		ssize_t		offset;	/* choice of header lists */

		lblk = (struct lblk *)CLRBUSY((char *)ptr - MINHEAD);
		assert((struct header *)lblk < arenaend);
		assert((struct header *)lblk > arena);
		/* allow twits (e.g. awk) to free a block twice */
		holdblk = lblk->header.holder;
		if (!TESTBUSY(holdblk))
			return;
		holdblk = (struct holdblk *)CLRALL(holdblk);
		/* put lblk on its hold block's free list */
		lblk->header.nextfree = SETSMAL(holdblk->lfreeq);
		holdblk->lfreeq = lblk;
		/* move holdblk to head of queue, if its not already there */
		offset = holdblk->blksz / grain;
		oldhead = holdhead[offset];
		if (oldhead != holdblk) {
			/* first take out of current spot */
			holdhead[offset] = holdblk;
			holdblk->nexthblk->prevhblk = holdblk->prevhblk;
			holdblk->prevhblk->nexthblk = holdblk->nexthblk;
			/* now add at front */
			holdblk->nexthblk = oldhead;
			holdblk->prevhblk = oldhead->prevhblk;
			oldhead->prevhblk = holdblk;
			holdblk->prevhblk->nexthblk = holdblk;
		}
	} else {
		struct header *blk;	/* real start of block */
		struct header *next;	/* next = blk->nextblk */
		struct header *nextnext;	/* block after next */

		blk = (struct header *)((char *)ptr - minhead);
		next = blk->nextblk;
		/* take care of twits (e.g. awk) who return blocks twice */
		if (!TESTBUSY(next))
			return;
		blk->nextblk = next = CLRBUSY(next);
		ADDFREEQ(blk);
		/* see if we can compact */
		if (!TESTBUSY(nextnext = next->nextblk)) {
			do {
				DELFREEQ(next);
				next = nextnext;
			} while (!TESTBUSY(nextnext = next->nextblk));
			if (next == arenaend) lastblk = blk;
			blk->nextblk = next;
		}
	}
	CHECKQ
}


/*
 * realloc(ptr, size) - give the user a block of size "size", with
 *			    the contents pointed to by ptr.  Free ptr.
 */

void *
realloc(void *ptr, size_t size)
{
	void	*retval;

	(void) mutex_lock(&mlock);
	retval = realloc_unlocked(ptr, size);
	(void) mutex_unlock(&mlock);
	return (retval);
}


/*
 * realloc_unlocked(ptr) - Do the real work for realloc()
 */

static void *
realloc_unlocked(void *ptr, size_t size)
{
	struct header *blk;	/* block ptr is contained in */
	size_t trusize;	/* block size as allocater sees it */
	char *newptr;			/* pointer to user's new block */
	size_t cpysize;	/* amount to copy */
	struct header *next;	/* block after blk */

	if (ptr == NULL)
		return (malloc_unlocked(size, 0));

	if (size == 0) {
		free_unlocked(ptr);
		return (NULL);
	}

	if (TESTSMAL(((struct lblk *)((char *)ptr - MINHEAD))->
	    header.holder)) {
		/*
		 * we have a special small block which can't be expanded
		 *
		 * This makes the assumption that even if the user is
		 * reallocating a free block, malloc doesn't alter the contents
		 * of small blocks
		 */
		newptr = malloc_unlocked(size, 0);
		if (newptr == NULL)
			return (NULL);
		/* this isn't to save time--its to protect the twits */
		if ((char *)ptr != newptr) {
			struct lblk *lblk;
			lblk = (struct lblk *)((char *)ptr - MINHEAD);
			cpysize = ((struct holdblk *)
			    CLRALL(lblk->header.holder))->blksz;
			cpysize = (size > cpysize) ? cpysize : size;
			(void) memcpy(newptr, ptr, cpysize);
			free_unlocked(ptr);
		}
	} else {
		blk = (struct header *)((char *)ptr - minhead);
		next = blk->nextblk;
		/*
		 * deal with twits who reallocate free blocks
		 *
		 * if they haven't reset minblk via getopt, that's
		 * their problem
		 */
		if (!TESTBUSY(next)) {
			DELFREEQ(blk);
			blk->nextblk = SETBUSY(next);
		}
		next = CLRBUSY(next);
		/* make blk as big as possible */
		if (!TESTBUSY(next->nextblk)) {
			do {
				DELFREEQ(next);
				next = next->nextblk;
			} while (!TESTBUSY(next->nextblk));
			blk->nextblk = SETBUSY(next);
			if (next >= arenaend) lastblk = blk;
		}
		/* get size we really need */
		trusize = size+minhead;
		trusize = (trusize + ALIGNSZ - 1)/ALIGNSZ*ALIGNSZ;
		trusize = (trusize >= MINBLKSZ) ? trusize : MINBLKSZ;
		/* see if we have enough */
		/* this isn't really the copy size, but I need a register */
		cpysize = (char *)next - (char *)blk;
		if (cpysize >= trusize) {
			/* carve out the size we need */
			struct header *newblk;	/* remainder */

			if (cpysize - trusize >= MINBLKSZ) {
				/*
				 * carve out the right size block
				 * newblk will be the remainder
				 */
				newblk = (struct header *)((char *)blk +
				    trusize);
				newblk->nextblk = next;
				blk->nextblk = SETBUSY(newblk);
				/* at this point, next is invalid */
				ADDFREEQ(newblk);
				/* if blk was lastblk, make newblk lastblk */
				if (blk == lastblk)
					lastblk = newblk;
			}
			newptr = ptr;
		} else {
			/* bite the bullet, and call malloc */
			cpysize = (size > cpysize) ? cpysize : size;
			newptr = malloc_unlocked(size, 0);
			if (newptr == NULL)
				return (NULL);
			(void) memcpy(newptr, ptr, cpysize);
			free_unlocked(ptr);
		}
	}
	return (newptr);
}


/*
 * calloc - allocate and clear memory block
 */

void *
calloc(size_t num, size_t size)
{
	char *mp;
	size_t total;

	if (num == 0 || size == 0) {
		total = 0;
	} else {
		total = num * size;

		/* check for overflow */
		if ((total / num) != size) {
			errno = ENOMEM;
			return (NULL);
		}
	}

	mp = malloc(total);
	if (mp == NULL)
		return (NULL);
	(void) memset(mp, 0, total);
	return (mp);
}


/*
 * Mallopt - set options for allocation
 *
 *	Mallopt provides for control over the allocation algorithm.
 *	The cmds available are:
 *
 *	M_MXFAST Set maxfast to value.  Maxfast is the size of the
 *		 largest small, quickly allocated block.  Maxfast
 *		 may be set to 0 to disable fast allocation entirely.
 *
 *	M_NLBLKS Set numlblks to value.  Numlblks is the number of
 *		 small blocks per holding block.  Value must be
 *		 greater than 0.
 *
 *	M_GRAIN  Set grain to value.  The sizes of all blocks
 *		 smaller than maxfast are considered to be rounded
 *		 up to the nearest multiple of grain. The default
 *		 value of grain is the smallest number of bytes
 *		 which will allow alignment of any data type.    Grain
 *		 will be rounded up to a multiple of its default,
 *		 and maxsize will be rounded up to a multiple of
 *		 grain.  Value must be greater than 0.
 *
 *	M_KEEP   Retain data in freed block until the next malloc,
 *		 realloc, or calloc.  Value is ignored.
 *		 This option is provided only for compatibility with
 *		 the old version of malloc, and is not recommended.
 *
 *	returns - 0, upon successful completion
 *		 1, if malloc has previously been called or
 *		    if value or cmd have illegal values
 */

int
mallopt(int cmd, int value)
{
	/* disallow changes once a small block is allocated */
	(void) mutex_lock(&mlock);
	if (change) {
		(void) mutex_unlock(&mlock);
		return (1);
	}
	switch (cmd) {
	case M_MXFAST:
		if (value < 0) {
			(void) mutex_unlock(&mlock);
			return (1);
		}
		fastct = (value + grain - 1) / grain;
		maxfast = grain*fastct;
		break;
	case M_NLBLKS:
		if (value <= 1) {
			(void) mutex_unlock(&mlock);
			return (1);
		}
		numlblks = value;
		break;
	case M_GRAIN:
		if (value <= 0) {
			(void) mutex_unlock(&mlock);
			return (1);
		}

		/* round grain up to a multiple of ALIGNSZ */
		grain = (value + ALIGNSZ - 1)/ALIGNSZ*ALIGNSZ;

		/* reduce fastct appropriately */
		fastct = (maxfast + grain - 1) / grain;
		maxfast = grain * fastct;
		break;
	case M_KEEP:
		if (change && holdhead != NULL) {
			(void) mutex_unlock(&mlock);
			return (1);
		}
		minhead = HEADSZ;
		break;
	default:
		(void) mutex_unlock(&mlock);
		return (1);
	}
	(void) mutex_unlock(&mlock);
	return (0);
}

/*
 * mallinfo-provide information about space usage
 *
 *	input - max; mallinfo will return the size of the
 *		largest block < max.
 *
 *	output - a structure containing a description of
 *		 of space usage, defined in malloc.h
 */

struct mallinfo
mallinfo(void)
{
	struct header *blk, *next;	/* ptr to ordinary blocks */
	struct holdblk *hblk;		/* ptr to holding blocks */
	struct mallinfo inf;		/* return value */
	int	i;			/* the ubiquitous counter */
	ssize_t size;			/* size of a block */
	ssize_t fsp;			/* free space in 1 hold block */

	(void) mutex_lock(&mlock);
	(void) memset(&inf, 0, sizeof (struct mallinfo));
	if (freeptr[0].nextfree == GROUND) {
		(void) mutex_unlock(&mlock);
		return (inf);
	}
	blk = CLRBUSY(arena[1].nextblk);
	/* return total space used */
	inf.arena = (char *)arenaend - (char *)blk;

	/*
	 * loop through arena, counting # of blocks, and
	 * and space used by blocks
	 */
	next = CLRBUSY(blk->nextblk);
	while (next != &(arena[1])) {
		inf.ordblks++;
		size = (char *)next - (char *)blk;
		if (TESTBUSY(blk->nextblk)) {
			inf.uordblks += size;
			inf.keepcost += HEADSZ-MINHEAD;
		} else {
			inf.fordblks += size;
		}
		blk = next;
		next = CLRBUSY(blk->nextblk);
	}

	/*
	 * if any holding block have been allocated
	 * then examine space in holding blks
	 */
	if (change && holdhead != NULL) {
		for (i = fastct; i > 0; i--) {	/* loop thru ea. chain */
			hblk = holdhead[i];
			/* do only if chain not empty */
			if (hblk != HGROUND) {
				size = hblk->blksz +
				    sizeof (struct lblk) - sizeof (int);
				do {	/* loop thru 1 hold blk chain */
					inf.hblks++;
					fsp = freespace(hblk);
					inf.fsmblks += fsp;
					inf.usmblks += numlblks*size - fsp;
					inf.smblks += numlblks;
					hblk = hblk->nexthblk;
				} while (hblk != holdhead[i]);
			}
		}
	}
	inf.hblkhd = (inf.smblks / numlblks) * sizeof (struct holdblk);
	/* holding block were counted in ordblks, so subtract off */
	inf.ordblks -= inf.hblks;
	inf.uordblks -= inf.hblkhd + inf.usmblks + inf.fsmblks;
	inf.keepcost -= inf.hblks*(HEADSZ - MINHEAD);
	(void) mutex_unlock(&mlock);
	return (inf);
}


/*
 * freespace - calc. how much space is used in the free
 *		    small blocks in a given holding block
 *
 *	input - hblk = given holding block
 *
 *	returns space used in free small blocks of hblk
 */

static ssize_t
freespace(struct holdblk *holdblk)
{
	struct lblk *lblk;
	ssize_t space = 0;
	ssize_t size;
	struct lblk *unused;

	lblk = CLRSMAL(holdblk->lfreeq);
	size = holdblk->blksz + sizeof (struct lblk) - sizeof (int);
	unused = CLRSMAL(holdblk->unused);
	/* follow free chain */
	while ((lblk != LGROUND) && (lblk != unused)) {
		space += size;
		lblk = CLRSMAL(lblk->header.nextfree);
	}
	space += ((char *)holdblk + HOLDSZ(size)) - (char *)unused;
	return (space);
}

static void *
morecore(size_t bytes)
{
	void * ret;

	if (bytes > LONG_MAX) {
		intptr_t wad;
		/*
		 * The request size is too big. We need to do this in
		 * chunks. Sbrk only takes an int for an arg.
		 */
		if (bytes == ULONG_MAX)
			return ((void *)-1);

		ret = sbrk(0);
		wad = LONG_MAX;
		while (wad > 0) {
			if (sbrk(wad) == (void *)-1) {
				if (ret != sbrk(0))
					(void) sbrk(-LONG_MAX);
				return ((void *)-1);
			}
			bytes -= LONG_MAX;
			wad = bytes;
		}
	} else
		ret = sbrk(bytes);

	return (ret);
}

#ifdef debug
int
check_arena(void)
{
	struct header *blk, *prev, *next;	/* ptr to ordinary blocks */

	(void) mutex_lock(&mlock);
	if (freeptr[0].nextfree == GROUND) {
		(void) mutex_unlock(&mlock);
		return (-1);
	}
	blk = arena + 1;

	/* loop through arena, checking */
	blk = (struct header *)CLRALL(blk->nextblk);
	next = (struct header *)CLRALL(blk->nextblk);
	while (next != arena + 1) {
		assert(blk >= arena + 1);
		assert(blk <= lastblk);
		assert(next >= blk + 1);
		assert(((uintptr_t)((struct header *)blk->nextblk) &
		    (4 | SMAL)) == 0);

		if (TESTBUSY(blk->nextblk) == 0) {
			assert(blk->nextfree >= freeptr);
			assert(blk->prevfree >= freeptr);
			assert(blk->nextfree <= lastblk);
			assert(blk->prevfree <= lastblk);
			assert(((uintptr_t)((struct header *)blk->nextfree) &
			    7) == 0);
			assert(((uintptr_t)((struct header *)blk->prevfree) &
			    7) == 0 || blk->prevfree == freeptr);
		}
		blk = next;
		next = CLRBUSY(blk->nextblk);
	}
	(void) mutex_unlock(&mlock);
	return (0);
}

#define	RSTALLOC	1
#endif

#ifdef RSTALLOC
/*
 * rstalloc - reset alloc routines
 *
 *	description -	return allocated memory and reset
 *			allocation pointers.
 *
 *	Warning - This is for debugging purposes only.
 *		  It will return all memory allocated after
 *		  the first call to malloc, even if some
 *		  of it was fetched by a user's sbrk().
 */

void
rstalloc(void)
{
	(void) mutex_lock(&mlock);
	minhead = MINHEAD;
	grain = ALIGNSZ;
	numlblks = NUMLBLKS;
	fastct = FASTCT;
	maxfast = MAXFAST;
	change = 0;
	if (freeptr[0].nextfree == GROUND) {
		(void) mutex_unlock(&mlock);
		return;
	}
	brk(CLRBUSY(arena[1].nextblk));
	freeptr[0].nextfree = GROUND;
#ifdef debug
	case1count = 0;
#endif
	(void) mutex_unlock(&mlock);
}
#endif	/* RSTALLOC */

/*
 * cfree is an undocumented, obsolete function
 */

/* ARGSUSED1 */
void
cfree(void *p, size_t num, size_t size)
{
	free(p);
}

static void
malloc_prepare()
{
	(void) mutex_lock(&mlock);
}

static void
malloc_release()
{
	(void) mutex_unlock(&mlock);
}

#pragma init(malloc_init)
static void
malloc_init(void)
{
	(void) pthread_atfork(malloc_prepare, malloc_release, malloc_release);
}



/* ============================================================================
 * SOURCE 9/13: minix4\libos\lib_legacy\i386\boot\malloc.c
 * Size: 1,206 bytes, Lines: 50
 * Hash: 8aac94a1fdbb...
 * ============================================================================ */


#include <mach/machine/vm_types.h>
#include <mach/lmm.h>
#include <malloc.h>
#include <assert.h>

/* Remove any memory in the specified range from the free memory list.  */
void malloc_reserve(vm_offset_t rstart, vm_offset_t rend)
{
	/*printf("malloc_reserve %08x-%08x\n", rstart, rend);*/
	while (rstart < rend)
	{
		vm_offset_t size;
		lmm_flags_t flags;
		void *ptr;

		/*printf("lmm_find_free %08x ", rstart);*/
		lmm_find_free(&malloc_lmm, &rstart, &size, &flags);
		/*printf(" returned %08x-%08x (size %08x) flags %08x\n",
			rstart, rstart+size, size, flags);*/
		if ((size == 0) || (rstart >= rend))
			break;
		if (rstart + size > rend)
			size = rend - rstart;
		/*printf("reserving %08x-%08x\n", rstart, rstart+size);*/
		ptr = lmm_alloc_gen(&malloc_lmm, size, flags, 0, 0,
				    rstart, size);
		assert((vm_offset_t)ptr == rstart);
	}
}

void *mustmalloc(vm_size_t size)
{
	void *buf;
	if (!(buf = malloc(size)))
		die("out of memory");
	/*printf("malloc returning %08x-%08x\n", buf, buf+size);*/
	return buf;
}

void *mustcalloc(vm_size_t size)
{
	void *buf;
	if (!(buf = calloc(size, 1)))
		die("out of memory");
	/*printf("calloc returning %08x-%08x\n", buf, buf+size);*/
	return buf;
}




/* ============================================================================
 * SOURCE 10/13: minix4\exokernel\kernel_legacy\malloc.c
 * Size: 1,649 bytes, Lines: 88
 * Hash: 22b2c02d60ae...
 * ============================================================================ */

#
/*
 */

/*
 * Structure of the coremap and swapmap
 * arrays. Consists of non-zero count
 * and base address of that many
 * contiguous units.
 * (The coremap unit is 64 bytes,
 * the swapmap unit is 512 bytes)
 * The addresses are increasing and
 * the list is terminated with the
 * first zero count.
 */
struct map
{
	char *m_size;
	char *m_addr;
};

/*
 * Allocate size units from the given
 * map. Return the base of the allocated
 * space.
 * Algorithm is first fit.
 */
malloc(mp, size)
struct map *mp;
{
	register int a;
	register struct map *bp;

	for (bp = mp; bp->m_size; bp++) {
		if (bp->m_size >= size) {
			a = bp->m_addr;
			bp->m_addr =+ size;
			if ((bp->m_size =- size) == 0)
				do {
					bp++;
					(bp-1)->m_addr = bp->m_addr;
				} while ((bp-1)->m_size = bp->m_size);
			return(a);
		}
	}
	return(0);
}

/*
 * Free the previously allocated space aa
 * of size units into the specified map.
 * Sort aa into map and combine on
 * one or both ends if possible.
 */
mfree(mp, size, aa)
struct map *mp;
{
	register struct map *bp;
	register int t;
	register int a;

	a = aa;
	for (bp = mp; bp->m_addr<=a && bp->m_size!=0; bp++);
	if (bp>mp && (bp-1)->m_addr+(bp-1)->m_size == a) {
		(bp-1)->m_size =+ size;
		if (a+size == bp->m_addr) {
			(bp-1)->m_size =+ bp->m_size;
			while (bp->m_size) {
				bp++;
				(bp-1)->m_addr = bp->m_addr;
				(bp-1)->m_size = bp->m_size;
			}
		}
	} else {
		if (a+size == bp->m_addr && bp->m_size) {
			bp->m_addr =- size;
			bp->m_size =+ size;
		} else if (size) do {
			t = bp->m_addr;
			bp->m_addr = a;
			a = t;
			t = bp->m_size;
			bp->m_size = size;
			bp++;
		} while (size = t);
	}
}



/* ============================================================================
 * SOURCE 11/13: minix4\exokernel\kernel_legacy\stand\lib\sa\malloc.c
 * Size: 1,901 bytes, Lines: 90
 * Hash: f04dfe65175d...
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
/*
 * Copyright 2002-2003 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#include <sys/salib.h>

/*
 * For documentation on these functions, see malloc(3C).
 */

void *
malloc(size_t size)
{
	size_t *iaddr;

	iaddr = (size_t *)bkmem_alloc(size + sizeof (size_t));
	if (iaddr == NULL) {
		errno = ENOMEM;
		return (NULL);
	}

	iaddr[0] = size;
	return (&iaddr[1]);
}

void *
calloc(size_t number, size_t size)
{
	void *addr;

	addr = malloc(number * size);
	if (addr == NULL)
		return (NULL);

	return (memset(addr, 0, number * size));
}

void *
realloc(void *oldaddr, size_t size)
{
	void *addr;
	size_t oldsize;

	addr = malloc(size);
	if (oldaddr != NULL) {
		oldsize = ((size_t *)oldaddr)[-1];
		if (addr != NULL) {
			bcopy(oldaddr, addr, (oldsize > size ? oldsize : size));
			free(oldaddr);
		}
	}

	return (addr);
}

void
free(void *addr)
{
	size_t *lenloc;

	if (addr == NULL)
		return;
	lenloc = (size_t *)addr - 1;
	bkmem_free((caddr_t)lenloc, *lenloc + sizeof (size_t));
}



/* ============================================================================
 * SOURCE 12/13: minix4\exokernel\kernel_legacy\contrib\ast\src\lib\libast\vmalloc\malloc.c
 * Size: 31,386 bytes, Lines: 1,477
 * Hash: 168548c93654...
 * ============================================================================ */

/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*          Copyright (c) 1985-2012 AT&T Intellectual Property          *
*                      and is licensed under the                       *
*                 Eclipse Public License, Version 1.0                  *
*                    by AT&T Intellectual Property                     *
*                                                                      *
*                A copy of the License is available at                 *
*          http://www.eclipse.org/org/documents/epl-v10.html           *
*         (with md5 checksum b35adb5213ca9657e911e9befb180842)         *
*                                                                      *
*              Information and Software Systems Research               *
*                            AT&T Research                             *
*                           Florham Park NJ                            *
*                                                                      *
*                 Glenn Fowler <gsf@research.att.com>                  *
*                  David Korn <dgk@research.att.com>                   *
*                   Phong Vo <kpv@research.att.com>                    *
*                                                                      *
***********************************************************************/
#if defined(_UWIN) && defined(_BLD_ast)

void _STUB_malloc(){}

#else

#if _UWIN

#define calloc		______calloc
#define _ast_free	______free
#define malloc		______malloc
#define mallinfo	______mallinfo
#define mallopt		______mallopt
#define mstats		______mstats
#define realloc		______realloc

#define _STDLIB_H_	1

extern int		atexit(void(*)(void));
extern char*		getenv(const char*);

#endif

#include	"vmhdr.h"
#include	<errno.h>

#if _UWIN

#include	<malloc.h>

#define _map_malloc	1
#define _mal_alloca	1

#undef	calloc
#define calloc		_ast_calloc
#undef	_ast_free
#define free		_ast_free
#undef	malloc
#define malloc		_ast_malloc
#undef	mallinfo
typedef struct ______mallinfo Mallinfo_t;
#undef	mallopt
#undef	mstats
typedef struct ______mstats Mstats_t;
#undef	realloc
#define realloc		_ast_realloc

#endif

#if __STD_C
#define F0(f,t0)		f(t0)
#define F1(f,t1,a1)		f(t1 a1)
#define F2(f,t1,a1,t2,a2)	f(t1 a1, t2 a2)
#else
#define F0(f,t0)		f()
#define F1(f,t1,a1)		f(a1) t1 a1;
#define F2(f,t1,a1,t2,a2)	f(a1, a2) t1 a1; t2 a2;
#endif

/*
 * define _AST_std_malloc=1 to force the standard malloc
 * if _map_malloc is also defined then _ast_malloc etc.
 * will simply call malloc etc.
 */

#if !defined(_AST_std_malloc) && __CYGWIN__
#define _AST_std_malloc	1
#endif

/*	malloc compatibility functions
**
**	These are aware of debugging/profiling and are driven by the
**	VMALLOC_OPTIONS environment variable which is a comma or space
**	separated list of [no]name[=value] options:
**
**	    abort	if Vmregion==Vmdebug then VM_DBABORT is set,
**			otherwise _BLD_DEBUG enabled assertions abort()
**			on failure
**	    break	try sbrk() block allocator first
**	    check	if Vmregion==Vmbest then the region is checked every op
**	    free	disable addfreelist()
**	    keep	disable free -- if code works with this enabled then it
**	    		probably accesses free'd data
**	    method=m	sets Vmregion=m if not defined, m (Vm prefix optional)
**			may be one of { best debug last profile }
**	    mmap	try mmap() block allocator first
**	    period=n	sets Vmregion=Vmdebug if not defined, if
**			Vmregion==Vmdebug the region is checked every n ops
**	    profile=f	sets Vmregion=Vmprofile if not set, if
**			Vmregion==Vmprofile then profile info printed to file f
**	    start=n	sets Vmregion=Vmdebug if not defined, if
**			Vmregion==Vmdebug region checking starts after n ops
**	    trace=f	enables tracing to file f
**	    warn=f	sets Vmregion=Vmdebug if not defined, if
**			Vmregion==Vmdebug then warnings printed to file f
**	    watch=a	sets Vmregion=Vmdebug if not defined, if
**			Vmregion==Vmdebug then address a is watched
**
**	Output files are created if they don't exist. &n and /dev/fd/n name
**	the file descriptor n which must be open for writing. The pattern %p
**	in a file name is replaced by the process ID.
**
**	VMALLOC_OPTIONS combines the features of these previously used env vars:
**	    { VMCHECK VMDEBUG VMETHOD VMPROFILE VMTRACE }
**
**	Written by Kiem-Phong Vo, kpv@research.att.com, 01/16/94.
*/

#if _sys_stat
#include	<sys/stat.h>
#endif
#include	<fcntl.h>

#ifdef S_IRUSR
#define CREAT_MODE	(S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
#else
#define CREAT_MODE	0644
#endif

static Vmulong_t	_Vmdbstart = 0;
static Vmulong_t	_Vmdbcheck = 0;
static Vmulong_t	_Vmdbtime = 0;
static int		_Vmpffd = -1;

#if ( !_std_malloc || !_BLD_ast ) && !_AST_std_malloc

#if !_map_malloc
#undef calloc
#undef cfree
#undef free
#undef mallinfo
#undef malloc
#undef mallopt
#undef memalign
#undef mstats
#undef realloc
#undef valloc

#if _malloc_hook

#include <malloc.h>

#undef	calloc
#undef	cfree
#undef	free
#undef	malloc
#undef	memalign
#undef	realloc

#define calloc		_ast_calloc
#define cfree		_ast_cfree
#define free		_ast_free
#define malloc		_ast_malloc
#define memalign	_ast_memalign
#define realloc		_ast_realloc

#endif

#endif

#if _WINIX

#include <ast_windows.h>

#if _UWIN

#define VMRECORD(p)	_vmrecord(p)
#define VMBLOCK		{ int _vmblock = _sigblock();
#define VMUNBLOCK	_sigunblock(_vmblock); }

extern int		_sigblock(void);
extern void		_sigunblock(int);
extern unsigned long	_record[2048];

__inline Void_t* _vmrecord(Void_t* p)
{
	register unsigned long	v = ((unsigned long)p)>>16; 

	_record[v>>5] |= 1<<((v&0x1f));
	return p;
}

#else

#define getenv(s)	lcl_getenv(s)

static char*
lcl_getenv(const char* s)
{
	int		n;
	static char	buf[512];

	if (!(n = GetEnvironmentVariable(s, buf, sizeof(buf))) || n > sizeof(buf))
		return 0;
	return buf;
}

#endif /* _UWIN */

#endif /* _WINIX */

#ifndef VMRECORD
#define VMRECORD(p)	(p)
#define VMBLOCK
#define VMUNBLOCK
#endif

#if defined(__EXPORT__)
#define extern		extern __EXPORT__
#endif

static int		_Vmflinit = 0;
#define VMFLINIT() \
	{ if(!_Vmflinit)	vmflinit(); \
	  if(_Vmdbcheck) \
	  { if(_Vmdbtime < _Vmdbstart) _Vmdbtime += 1; \
	    else if((_Vmdbtime += 1) < _Vmdbstart) _Vmdbtime = _Vmdbstart; \
	    if(_Vmdbtime >= _Vmdbstart && (_Vmdbtime % _Vmdbcheck) == 0 && \
	       Vmregion->meth.meth == VM_MTDEBUG) \
		vmdbcheck(Vmregion); \
	  } \
	}

#if __STD_C
static int vmflinit(void)
#else
static int vmflinit()
#endif
{
	char*		file;
	int		line;
	Void_t*		func;

	/* this must be done now to avoid any inadvertent recursion (more below) */
	_Vmflinit = 1;
	VMFLF(Vmregion,file,line,func);

	/* if getenv() calls malloc(), the options may not affect the eventual region */
	VMOPTIONS();

	/* reset file and line number to correct values for the call */
	Vmregion->file = file;
	Vmregion->line = line;
	Vmregion->func = func;

	return 0;
}

/* use multiple regions to reduce blocking by concurrent threads  */
#if _mem_mmap_anon || _mem_mmap_zero
static Vmalloc_t	*Region[64];	/* list of concurrent regions	*/
static unsigned int	Regmax = 64;	/* max number of regions	*/
#else
static Vmalloc_t*	Region[1];	/* list of concurrent regions	*/
static unsigned int	Regmax = 0;
#endif
static unsigned int	Regnum = 0; 	/* current #concurrent regions	*/

/* statistics */
static unsigned int	Regopen = 0; 	/* #allocation calls opened	*/
static unsigned int	Reglock = 0; 	/* #allocation calls locked	*/
static unsigned int	Regprobe = 0; 	/* #probes to find a region	*/

int setregmax(int regmax)
{
	int	oldmax = Regmax;

	if(regmax >= Regnum && regmax <= sizeof(Region)/sizeof(Region[0]))
		Regmax = regmax;

	return oldmax;
}

/* return statistics */
int _mallocstat(Vmstat_t* st)
{
	Vmstat_t	vmst;
	int		k;

	if(vmstat(Vmregion, st) < 0) /* add up all stats */
		return -1;
	for(k = 0; k < Regnum; ++k)
	{	if(!Region[k])
			continue;
		if(vmstat(Region[k], &vmst) < 0 )
			return -1;
		st->n_busy += vmst.n_busy;
		st->n_free += vmst.n_free;
		st->s_busy += vmst.s_busy;
		st->s_free += vmst.s_free;
		st->m_busy += vmst.m_busy;
		st->m_free += vmst.m_free;
		st->n_seg  += vmst.n_seg;
		st->extent += vmst.extent;
	}

	st->n_region = Regnum+1;
	st->n_open = Regopen;
	st->n_lock = Reglock;
	st->n_probe = Regprobe;

	return 0;
}

/* find the region that a block was allocated from */
static Vmalloc_t* regionof(Void_t* addr)
{
	int	k;

#if USE_NATIVE
#define CAUTIOUS	1
#else
#define CAUTIOUS	0
#endif
	if(CAUTIOUS || Vmregion->meth.meth != VM_MTBEST )
	{	/* addr will not be dereferenced here */
		if(vmaddr(Vmregion,addr) == 0 )
			return Vmregion;
		for(k = 0; k < Regnum; ++k)
			if(Region[k] && vmaddr(Region[k], addr) == 0 )
				return Region[k];
		return NIL(Vmalloc_t*);
	}
	else
	{	/* fast, but susceptible to bad data */
		Vmdata_t *vd = SEG(BLOCK(addr))->vmdt;
		if(Vmregion->data == vd )
			return Vmregion;
		for(k = 0; k < Regnum; ++k)
			if(Region[k] && Region[k]->data == vd)
				return Region[k];
		return NIL(Vmalloc_t*);
	}
}

/* manage a cache of free objects */
typedef struct _regfree_s
{	struct _regfree_s*	next;
} Regfree_t;
static Regfree_t	*Regfree;

static void addfreelist(Regfree_t* data)
{
	unsigned int	k;
	Regfree_t	*head;

	for(k = 0;; ASOLOOP(k) )
	{	data->next = head = Regfree;
		if(asocasptr(&Regfree, head, data) == (Void_t*)head )
			return;
	}
}

static void clrfreelist()
{
	Regfree_t	*list, *next;
	Vmalloc_t	*vm;

	if(!(list = Regfree) )
		return; /* nothing to do */

	if(asocasptr(&Regfree, list, NIL(Regfree_t*)) != list )
		return; /* somebody else is doing it */

	for(; list; list = next)
	{	next = list->next;
		if(vm = regionof((Void_t*)list))
		{	if(asocasint(&vm->data->lock, 0, 1) == 0) /* can free this now */
			{	(void)(*vm->meth.freef)(vm, (Void_t*)list, 1);
				vm->data->lock = 0;
			}
			else	addfreelist(list); /* ah well, back in the queue */
		}
	}
}

/* get a suitable region to allocate from */
typedef struct _regdisc_s
{	Vmdisc_t	disc;
	char		slop[64]; /* to absorb any extra data in Vmdcsystem */
} Regdisc_t;

static int regexcept(Vmalloc_t* vm, int type, Void_t* data, Vmdisc_t* disc)
{
	if(type == VM_OPEN)
	{	if(data) /* make vmopen allocate all memory using discipline */
			*(Void_t**)data = data; /* just make it non-NULL */
		return 0;
	}
	return 0;
}

static Vmalloc_t* getregion(int* local)
{
	Vmalloc_t		*vm;
	int			p, pos;

	static unsigned int	Rand = 0xdeadbeef; /* a cheap prng */
#define RAND()			(Rand = Rand*16777617 + 3)

	clrfreelist();

	if(Regmax <= 0 )
	{	/* uni-process/thread */
		*local = 1;
		Vmregion->data->lock = 1;
		return Vmregion;
	}
	else if(asocasint(&Vmregion->data->lock, 0, 1) == 0 )
	{	/* Vmregion is open, so use it */
		*local = 1;
		asoincint(&Regopen);
		return Vmregion;
	}

	asoincint(&Regprobe); /* probe Region[] to find an open region */
	if(Regnum == 0)
		pos = 0;
	else for(pos = p = RAND()%Regnum;; )
	{	if(Region[p] && asocasint(&Region[p]->data->lock, 0, 1) == 0 )
		{	*local = 1;
			asoincint(&Regopen);
			return Region[p];
		}
		if((p = (p+1)%Regnum) == pos )
			break;
	}

	/* grab the next open slot for a new region */
	while((p = Regnum) < Regmax)
		if(asocasint(&Regnum, p, p+1) == p )
			break;
	if(p < Regmax) /* this slot is now ours */
	{	static Regdisc_t	Regdisc;
		if(!Regdisc.disc.exceptf) /* one time initialization */
		{	GETPAGESIZE(_Vmpagesize);
			memcpy(&Regdisc, Vmdcsystem, Vmdcsystem->size);
			Regdisc.disc.round = ROUND(_Vmpagesize, 64*1024);
			Regdisc.disc.exceptf = regexcept;
		}

		/**/ASSERT(Region[p] == NIL(Vmalloc_t*));
		if((vm = vmopen(&Regdisc.disc, Vmbest, VM_SHARE)) != NIL(Vmalloc_t*) )
		{	vm->data->lock = 1; /* lock new region now */
			*local = 1;
			asoincint(&Regopen);
			return (Region[p] = vm);
		}
		else	Region[p] = Vmregion; /* better than nothing */
	}

	/* must return something */
	vm = Region[pos] ? Region[pos] : Vmregion;
	if(asocasint(&vm->data->lock, 0, 1) == 0)
	{	*local = 1;
		asoincint(&Regopen);
	}
	else
	{	*local = 0;
		asoincint(&Reglock);
	}
	return vm;
}

#if __STD_C
extern Void_t* calloc(reg size_t n_obj, reg size_t s_obj)
#else
extern Void_t* calloc(n_obj, s_obj)
reg size_t	n_obj;
reg size_t	s_obj;
#endif
{
	Void_t		*addr;
	Vmalloc_t	*vm;
	int		local = 0;
	VMFLINIT();

	vm = getregion(&local);
	addr = (*vm->meth.resizef)(vm, NIL(Void_t*), n_obj*s_obj, VM_RSZERO, local);
	if(local)
	{	/**/ASSERT(vm->data->lock == 1);
		vm->data->lock = 0;
	}
	return VMRECORD(addr);
}

#if __STD_C
extern Void_t* malloc(reg size_t size)
#else
extern Void_t* malloc(size)
reg size_t	size;
#endif
{
	Void_t		*addr;
	Vmalloc_t	*vm;
	int		local = 0;
	VMFLINIT();

	vm = getregion(&local);
	addr = (*vm->meth.allocf)(vm, size, local);
	if(local)
	{	/**/ASSERT(vm->data->lock == 1);
		vm->data->lock = 0;
	}
	return VMRECORD(addr);
}

#if __STD_C
extern Void_t* realloc(reg Void_t* data, reg size_t size)
#else
extern Void_t* realloc(data,size)
reg Void_t*	data;	/* block to be reallocated	*/
reg size_t	size;	/* new size			*/
#endif
{
	ssize_t		copy;
	Void_t		*addr;
	Vmalloc_t	*vm;
	VMFLINIT();

	if(!data)
		return malloc(size);
	else if((vm = regionof(data)) )
	{	if(vm == Vmregion && vm != Vmheap) /* no multiple region usage here */
		{	addr = (*vm->meth.resizef)(vm, data, size, VM_RSCOPY|VM_RSMOVE, 0);
			return VMRECORD(addr);
		}
		if(asocasint(&vm->data->lock, 0, 1) == 0 ) /* region is open */
		{	addr = (*vm->meth.resizef)(vm, data, size, VM_RSCOPY|VM_RSMOVE, 1);
			vm->data->lock = 0;
			return VMRECORD(addr);
		}
		else if(Regmax > 0 && Vmregion == Vmheap && (addr = malloc(size)) )
		{	if((copy = SIZE(BLOCK(data))&~BITS) > size )
				copy = size;	
			memcpy(addr, data, copy);
			addfreelist((Regfree_t*)data);
			return VMRECORD(addr);
		}
		else /* this may block but it is the best that we can do now */
		{	addr = (*vm->meth.resizef)(vm, data, size, VM_RSCOPY|VM_RSMOVE, 0);
			return VMRECORD(addr);
		}
	}
	else /* not our data */
	{
#if USE_NATIVE
#undef	realloc /* let the native realloc() take care of it */
#if __STD_C
		extern Void_t*	realloc(Void_t*, size_t);
#else
		extern Void_t*	realloc();
#endif
		return realloc(data, size);
#else 
		return NIL(Void_t*);
#endif
	}
}

#if __STD_C
extern void free(reg Void_t* data)
#else
extern void free(data)
reg Void_t*	data;
#endif
{
	Vmalloc_t	*vm;
	VMFLINIT();

	if(!data || (_Vmassert & VM_keep))
		return;
	else if((vm = regionof(data)) )
	{	
		if(vm == Vmregion && Vmregion != Vmheap || (_Vmassert & VM_free))
			(void)(*vm->meth.freef)(vm, data, 0);
		else	addfreelist((Regfree_t*)data);
		return;
	}
	else /* not our data */
	{
#if USE_NATIVE
#undef	free /* let the native free() take care of it */
#if __STD_C
		extern void	free(Void_t*);
#else
		extern void	free();
#endif
		free(data);
#endif
		return;
	}
}

#if __STD_C
extern void cfree(reg Void_t* data)
#else
extern void cfree(data)
reg Void_t*	data;
#endif
{
	free(data);
}

#if __STD_C
extern Void_t* memalign(reg size_t align, reg size_t size)
#else
extern Void_t* memalign(align, size)
reg size_t	align;
reg size_t	size;
#endif
{
	Void_t		*addr;
	Vmalloc_t	*vm;
	int		local = 0;
	VMFLINIT();

	vm = getregion(&local);
	VMBLOCK
	addr = (*vm->meth.alignf)(vm, size, align, local);
	if(local)
	{	/**/ASSERT(vm->data->lock == 1);
		vm->data->lock = 0;
	}
	VMUNBLOCK
	return VMRECORD(addr);
}

#if __STD_C
extern int posix_memalign(reg Void_t **memptr, reg size_t align, reg size_t size)
#else
extern int posix_memalign(memptr, align, size)
reg Void_t**	memptr;
reg size_t	align;
reg size_t	size;
#endif
{
	Void_t	*mem;

	if(align == 0 || (align%sizeof(Void_t*)) != 0 || ((align-1)&align) != 0 )
		return EINVAL;

	if(!(mem = memalign(align, size)) )
		return ENOMEM;

	*memptr = mem;
	return 0;
}

#if __STD_C
extern Void_t* valloc(reg size_t size)
#else
extern Void_t* valloc(size)
reg size_t	size;
#endif
{
	VMFLINIT();

	GETPAGESIZE(_Vmpagesize);
	return VMRECORD(memalign(_Vmpagesize, size));
}

#if __STD_C
extern Void_t* pvalloc(reg size_t size)
#else
extern Void_t* pvalloc(size)
reg size_t	size;
#endif
{
	VMFLINIT();

	GETPAGESIZE(_Vmpagesize);
	return VMRECORD(memalign(_Vmpagesize, ROUND(size,_Vmpagesize)) );
}

#if !_PACKAGE_ast
#if __STD_C
char* strdup(const char* s)
#else
char* strdup(s)
char*	s;
#endif
{
	char	*ns;
	size_t	n;

	if(!s)
		return NIL(char*);
	else
	{	n = strlen(s);
		if((ns = malloc(n+1)) )
			memcpy(ns,s,n+1);
		return ns;
	}
}
#endif /* _PACKAGE_ast */

#if !_lib_alloca || _mal_alloca
#ifndef _stk_down
#define _stk_down	0
#endif
typedef struct _alloca_s	Alloca_t;
union _alloca_u
{	struct
	{	char*		addr;
		Alloca_t*	next;
	} head;
	char	array[ALIGN];
};
struct _alloca_s
{	union _alloca_u	head;
	Vmuchar_t	data[1];
};

#if __STD_C
extern Void_t* alloca(size_t size)
#else
extern Void_t* alloca(size)
size_t	size;
#endif
{	char		array[ALIGN];
	char*		file;
	int		line;
	Void_t*		func;
	Alloca_t*	f;
	Vmalloc_t	*vm;
	static Alloca_t* Frame;

	VMFLINIT();

	VMFLF(Vmregion,file,line,func); /* save info before freeing frames */

	while(Frame) /* free unused frames */
	{	if(( _stk_down && &array[0] > Frame->head.head.addr) ||
		   (!_stk_down && &array[0] < Frame->head.head.addr) )
		{	f = Frame; Frame = f->head.head.next;
			if((vm = regionof(f)) )
				(void)(*vm->meth.freef)(vm, f, 0);
			/* else: something bad happened. just keep going */
		}
		else	break;
	}

	Vmregion->file = file; /* restore file/line info before allocation */
	Vmregion->line = line;
	Vmregion->func = func;

	f = (Alloca_t*)(*Vmregion->meth.allocf)(Vmregion, size+sizeof(Alloca_t)-1, 0);

	/* if f is NULL, this mimics a stack overflow with a memory error! */
	f->head.head.addr = &array[0];
	f->head.head.next = Frame;
	Frame = f;

	return (Void_t*)f->data;
}
#endif /*!_lib_alloca || _mal_alloca*/

#if _map_malloc

/* not sure of all the implications -- 0 is conservative for now */
#define USE_NATIVE	0	/* native free/realloc on non-vmalloc ptrs */

#else

#if _malloc_hook

static void vm_free_hook(void* ptr, const void* caller)
{
	free(ptr);
}

static void* vm_malloc_hook(size_t size, const void* caller)
{
	void*	r;

	r = malloc(size);
	return r;
}

static void* vm_memalign_hook(size_t align, size_t size, const void* caller)
{
	void*	r;

	r = memalign(align, size);
	return r;
}

static void* vm_realloc_hook(void* ptr, size_t size, const void* caller)
{
	void*	r;

	r = realloc(ptr, size);
	return r;
}

static void vm_initialize_hook(void)
{
	__free_hook = vm_free_hook;
	__malloc_hook = vm_malloc_hook;
	__memalign_hook = vm_memalign_hook;
	__realloc_hook = vm_realloc_hook;
}

void	(*__malloc_initialize_hook)(void) = vm_initialize_hook;

#if 0 /* 2012-02-29 this may be needed to cover shared libs */

void __attribute__ ((constructor)) vm_initialize_initialize_hook(void)
{
	vm_initialize_hook();
	__malloc_initialize_hook = vm_initialize_hook;
}

#endif

#else

/* intercept _* __* __libc_* variants */

#if __lib__malloc
extern Void_t*	F2(_calloc, size_t,n, size_t,m) { return calloc(n, m); }
extern Void_t	F1(_cfree, Void_t*,p) { free(p); }
extern Void_t	F1(_free, Void_t*,p) { free(p); }
extern Void_t*	F1(_malloc, size_t,n) { return malloc(n); }
#if _lib_memalign
extern Void_t*	F2(_memalign, size_t,a, size_t,n) { return memalign(a, n); }
#endif
#if _lib_pvalloc
extern Void_t*	F1(_pvalloc, size_t,n) { return pvalloc(n); }
#endif
extern Void_t*	F2(_realloc, Void_t*,p, size_t,n) { return realloc(p, n); }
#if _lib_valloc
extern Void_t*	F1(_valloc, size_t,n) { return valloc(n); }
#endif
#endif

#if _lib___malloc
extern Void_t*	F2(__calloc, size_t,n, size_t,m) { return calloc(n, m); }
extern Void_t	F1(__cfree, Void_t*,p) { free(p); }
extern Void_t	F1(__free, Void_t*,p) { free(p); }
extern Void_t*	F1(__malloc, size_t,n) { return malloc(n); }
#if _lib_memalign
extern Void_t*	F2(__memalign, size_t,a, size_t,n) { return memalign(a, n); }
#endif
#if _lib_pvalloc
extern Void_t*	F1(__pvalloc, size_t,n) { return pvalloc(n); }
#endif
extern Void_t*	F2(__realloc, Void_t*,p, size_t,n) { return realloc(p, n); }
#if _lib_valloc
extern Void_t*	F1(__valloc, size_t,n) { return valloc(n); }
#endif
#endif

#if _lib___libc_malloc
extern Void_t*	F2(__libc_calloc, size_t,n, size_t,m) { return calloc(n, m); }
extern Void_t	F1(__libc_cfree, Void_t*,p) { free(p); }
extern Void_t	F1(__libc_free, Void_t*,p) { free(p); }
extern Void_t*	F1(__libc_malloc, size_t,n) { return malloc(n); }
#if _lib_memalign
extern Void_t*	F2(__libc_memalign, size_t,a, size_t,n) { return memalign(a, n); }
#endif
#if _lib_pvalloc
extern Void_t*	F1(__libc_pvalloc, size_t,n) { return pvalloc(n); }
#endif
extern Void_t*	F2(__libc_realloc, Void_t*,p, size_t,n) { return realloc(p, n); }
#if _lib_valloc
extern Void_t*	F1(__libc_valloc, size_t,n) { return valloc(n); }
#endif
#endif

#endif /* _malloc_hook */

#endif /* _map_malloc */

#undef	extern

#if _hdr_malloc /* need the mallint interface for statistics, etc. */

#undef	calloc
#define calloc		______calloc
#undef	cfree
#define cfree		______cfree
#undef	free
#define free		______free
#undef	malloc
#define malloc		______malloc
#undef	pvalloc
#define pvalloc		______pvalloc
#undef	realloc
#define realloc		______realloc
#undef	valloc
#define valloc		______valloc

#if !_UWIN

#include	<malloc.h>

typedef struct mallinfo Mallinfo_t;
typedef struct mstats Mstats_t;

#endif

#if defined(__EXPORT__)
#define extern		__EXPORT__
#endif

#if _lib_mallopt
#if __STD_C
extern int mallopt(int cmd, int value)
#else
extern int mallopt(cmd, value)
int	cmd;
int	value;
#endif
{
	VMFLINIT();
	return 0;
}
#endif /*_lib_mallopt*/

#if _lib_mallinfo && _mem_arena_mallinfo
#if __STD_C
extern Mallinfo_t mallinfo(void)
#else
extern Mallinfo_t mallinfo()
#endif
{
	Vmstat_t	sb;
	Mallinfo_t	mi;

	VMFLINIT();
	memset(&mi,0,sizeof(mi));
	if(vmstat(Vmregion,&sb) >= 0)
	{	mi.arena = sb.extent;
		mi.ordblks = sb.n_busy+sb.n_free;
		mi.uordblks = sb.s_busy;
		mi.fordblks = sb.s_free;
	}
	return mi;
}
#endif /* _lib_mallinfo */

#if _lib_mstats && _mem_bytes_total_mstats
#if __STD_C
extern Mstats_t mstats(void)
#else
extern Mstats_t mstats()
#endif
{
	Vmstat_t	sb;
	Mstats_t	ms;

	VMFLINIT();
	memset(&ms,0,sizeof(ms));
	if(vmstat(Vmregion,&sb) >= 0)
	{	ms.bytes_total = sb.extent;
		ms.chunks_used = sb.n_busy;
		ms.bytes_used = sb.s_busy;
		ms.chunks_free = sb.n_free;
		ms.bytes_free = sb.s_free;
	}
	return ms;
}
#endif /*_lib_mstats*/

#undef	extern

#endif/*_hdr_malloc*/

#else

/*
 * even though there is no malloc override, still provide
 * _ast_* counterparts for object compatibility
 */

#define setregmax(n)

#undef	calloc
extern Void_t*	calloc _ARG_((size_t, size_t));

#undef	cfree
extern void	cfree _ARG_((Void_t*));

#undef	free
extern void	free _ARG_((Void_t*));

#undef	malloc
extern Void_t*	malloc _ARG_((size_t));

#if _lib_memalign
#undef	memalign
extern Void_t*	memalign _ARG_((size_t, size_t));
#endif

#if _lib_pvalloc
#undef	pvalloc
extern Void_t*	pvalloc _ARG_((size_t));
#endif

#undef	realloc
extern Void_t*	realloc _ARG_((Void_t*, size_t));

#if _lib_valloc
#undef	valloc
extern Void_t*	valloc _ARG_((size_t));
#endif

#if defined(__EXPORT__)
#define extern		__EXPORT__
#endif

#if !_malloc_hook

extern Void_t	F1(_ast_free, Void_t*,p) { free(p); }
extern Void_t*	F1(_ast_malloc, size_t,n) { return malloc(n); }
#if _lib_memalign
extern Void_t*	F2(_ast_memalign, size_t,a, size_t,n) { return memalign(a, n); }
#endif
extern Void_t*	F2(_ast_realloc, Void_t*,p, size_t,n) { return realloc(p, n); }

#endif

extern Void_t*	F2(_ast_calloc, size_t,n, size_t,m) { return calloc(n, m); }
extern Void_t	F1(_ast_cfree, Void_t*,p) { free(p); }
#if _lib_pvalloc
extern Void_t*	F1(_ast_pvalloc, size_t,n) { return pvalloc(n); }
#endif
#if _lib_valloc
extern Void_t*	F1(_ast_valloc, size_t,n) { return valloc(n); }
#endif

#undef	extern

#if _hdr_malloc

#undef	mallinfo
#undef	mallopt
#undef	mstats

#define calloc		______calloc
#define cfree		______cfree
#define free		______free
#define malloc		______malloc
#define pvalloc		______pvalloc
#define realloc		______realloc
#define valloc		______valloc

#if !_UWIN

#if !_malloc_hook

#include	<malloc.h>

#endif

typedef struct mallinfo Mallinfo_t;
typedef struct mstats Mstats_t;

#endif

#if defined(__EXPORT__)
#define extern		__EXPORT__
#endif

#if _lib_mallopt
extern int	F2(_ast_mallopt, int,cmd, int,value) { return mallopt(cmd, value); }
#endif

#if _lib_mallinfo && _mem_arena_mallinfo
extern Mallinfo_t	F0(_ast_mallinfo, void) { return mallinfo(); }
#endif

#if _lib_mstats && _mem_bytes_total_mstats
extern Mstats_t		F0(_ast_mstats, void) { return mstats(); }
#endif

#undef	extern

#endif /*_hdr_malloc*/

#endif /*!_std_malloc*/

#if __STD_C
static Vmulong_t atou(char** sp)
#else
static Vmulong_t atou(sp)
char**	sp;
#endif
{
	char*		s = *sp;
	Vmulong_t	v = 0;

	if(s[0] == '0' && (s[1] == 'x' || s[1] == 'X') )
	{	for(s += 2; *s; ++s)
		{	if(*s >= '0' && *s <= '9')
				v = (v << 4) + (*s - '0');
			else if(*s >= 'a' && *s <= 'f')
				v = (v << 4) + (*s - 'a') + 10;
			else if(*s >= 'A' && *s <= 'F')
				v = (v << 4) + (*s - 'A') + 10;
			else break;
		}
	}
	else
	{	for(; *s; ++s)
		{	if(*s >= '0' && *s <= '9')
				v = v*10 + (*s - '0');
			else break;
		}
	}

	*sp = s;
	return v;
}

#if __STD_C
static char* insertpid(char* begs, char* ends)
#else
static char* insertpid(begs,ends)
char*	begs;
char*	ends;
#endif
{	int	pid;
	char*	s;

	if((pid = getpid()) < 0)
		return NIL(char*);

	s = ends;
	do
	{	if(s == begs)
			return NIL(char*);
		*--s = '0' + pid%10;
	} while((pid /= 10) > 0);
	while(s < ends)
		*begs++ = *s++;

	return begs;
}

#define FD_PRIVATE	(3*OPEN_MAX/4)

#if __STD_C
int _vmfd(int fd)
#else
int _vmfd(fd)
int	fd;
#endif
{
	int	pd;

	if (fd >= 0)
	{
		if (fd < FD_PRIVATE && (pd = fcntl(fd, F_DUPFD, FD_PRIVATE)) >= 0)
		{
			close(fd);
			fd = pd;
		}
#ifdef FD_CLOEXEC
		fcntl(fd,  F_SETFD, FD_CLOEXEC);
#endif
	}
	return fd;
}

#if __STD_C
static int createfile(char* file)
#else
static int createfile(file)
char*	file;
#endif
{
	char	buf[1024];
	char	*next, *endb;
	int	fd;

	next = buf;
	endb = buf + sizeof(buf);
	while(*file)
	{	if(*file == '%')
		{	switch(file[1])
			{
			case 'p' :
				if(!(next = insertpid(next,endb)) )
					return -1;
				file += 2;
				break;
			default :
				goto copy;
			}
		}
		else
		{ copy:
			*next++ = *file++;
		}

		if(next >= endb)
			return -1;
	}

	*next = '\0';
	file = buf;
	if (*file == '&' && *(file += 1) || strncmp(file, "/dev/fd/", 8) == 0 && *(file += 8))
		fd = dup((int)atou(&file));
	else if (*file)
	{
#if _PACKAGE_ast
		fd = open(file, O_WRONLY|O_CREAT|O_TRUNC, CREAT_MODE);
#else
		fd = creat(file, CREAT_MODE);
#endif
		fd = _vmfd(fd);
	}
	else
		return -1;
#if _PACKAGE_ast
#ifdef FD_CLOEXEC
	if (fd >= 0)
		fcntl(fd, F_SETFD, FD_CLOEXEC);
#endif
#endif
	return fd;
}

#if __STD_C
static void pfprint(void)
#else
static void pfprint()
#endif
{
	if(Vmregion->meth.meth == VM_MTPROFILE)
		vmprofile(Vmregion,_Vmpffd);
}

/*
 * initialize runtime options from the VMALLOC_OPTIONS env var
 */

#define COPY(t,e,f)	while ((*t = *f++) && t < e) t++

#if __STD_C
void _vmoptions(void)
#else
void _vmoptions()
#endif
{
	Vmalloc_t*	vm = 0;
	char*		trace = 0;
	char*		s;
	char*		t;
	char*		v;
	Vmulong_t	n;
	int		fd;
	char		buf[1024];

	_Vmoptions = 1;
	t = buf;
	v = &buf[sizeof(buf)-1];
	if (s = getenv("VMALLOC_OPTIONS"))
		COPY(t, v, s);
	if (t > buf)
	{
		*t = 0;
		s = buf;
		for (;;)
		{
			while (*s == ',' || *s == ' ' || *s == '\t' || *s == '\r' || *s == '\n')
				s++;
			if (!*(t = s))
				break;
			v = 0;
			while (*s)
				if (*s == ',' || *s == ' ' || *s == '\t' || *s == '\r' || *s == '\n')
				{
					*s++ = 0;
					break;
				}
				else if (!v && *s == '=')
				{
					*s++ = 0;
					if (!*(v = s))
						v = 0;
				}
				else
					s++;
			if (t[0] == 'n' && t[1] == 'o')
				continue;
			switch (t[0])
			{
			case 'a':		/* abort */
				if (!vm)
					vm = vmopen(Vmdcsystem, Vmdebug, 0);
				if (vm && vm->meth.meth == VM_MTDEBUG)
					vmset(vm, VM_DBABORT, 1);
				else
					_Vmassert |= VM_abort;
				break;
			case 'b':		/* break */
				_Vmassert |= VM_break;
				break;
			case 'c':		/* check */
				_Vmassert |= VM_check;
				break;
			case 'f':		/* free */
				_Vmassert |= VM_free;
				break;
			case 'k':		/* keep */
				_Vmassert |= VM_keep;
				break;
			case 'm':
				if (v)
					switch (t[1])
					{
					case 'e': /* method=METHOD */
						if (!vm)
						{
							if ((v[0] == 'V' || v[0] == 'v') && (v[1] == 'M' || v[1] == 'm'))
								v += 2;
							if (strcmp(v, "debug") == 0)
								vm = vmopen(Vmdcsystem, Vmdebug, 0);
							else if (strcmp(v, "profile") == 0)
								vm = vmopen(Vmdcsystem, Vmprofile, 0);
							else if (strcmp(v, "last") == 0)
								vm = vmopen(Vmdcsystem, Vmlast, 0);
							else if (strcmp(v, "best") == 0)
								vm = Vmheap;
						}
						break;
					case 'm': /* mmap */
						_Vmassert |= VM_mmap;
						break;
					}
				break;
			case 'p':
				if (v)
					switch (t[1])
					{
					case 'e':	/* period=<count> */
						if (!vm)
							vm = vmopen(Vmdcsystem, Vmdebug, 0);
						if (vm && vm->meth.meth == VM_MTDEBUG)
							_Vmdbcheck = atou(&v);
						break;
					case 'r':	/* profile=<path> */
						if (!vm)
							vm = vmopen(Vmdcsystem, Vmprofile, 0);
						if (v && vm && vm->meth.meth == VM_MTPROFILE)
							_Vmpffd = createfile(v);
						break;
					}
				break;
			case 's':		/* start=<count> */
				if (!vm)
					vm = vmopen(Vmdcsystem, Vmdebug, 0);
				if (v && vm && vm->meth.meth == VM_MTDEBUG)
					_Vmdbstart = atou(&v);
				break;
			case 't':		/* trace=<path> */
				trace = v;
				break;
			case 'w':
				if (t[1] == 'a')
					switch (t[2])
					{
					case 'r':	/* warn=<path> */
						if (!vm)
							vm = vmopen(Vmdcsystem, Vmdebug, 0);
						if (v && vm && vm->meth.meth == VM_MTDEBUG && (fd = createfile(v)) >= 0)
							vmdebug(fd);
						break;
					case 't':	/* watch=<addr> */
						if (!vm)
							vm = vmopen(Vmdcsystem, Vmdebug, 0);
						if (v && vm && vm->meth.meth == VM_MTDEBUG && (n = atou(&v)) >= 0)
							vmdbwatch((Void_t*)n);
						break;
					}
				break;
			}
		}
	}

	/* slip in the new region now so that malloc() will work fine */

	if (vm)
	{
		if (vm->meth.meth == VM_MTDEBUG)
			_Vmdbcheck = 1;
		Vmregion = vm;
	}

	/* enable tracing -- this currently disables multiple regions */

	if (trace)
	{
		setregmax(0);
		if ((fd = createfile(trace)) >= 0)
		{
			vmset(Vmregion, VM_TRACE, 1);
			vmtrace(fd);
		}
	}
	else if (Vmregion != Vmheap || asometh(0, 0)->type == ASO_SIGNAL)
		setregmax(0);

	/* make sure that profile data is output upon exiting */

	if (vm && vm->meth.meth == VM_MTPROFILE)
	{	
		if (_Vmpffd < 0)
			_Vmpffd = 2;
		/* this may wind up calling malloc(), but region is ok now */
		atexit(pfprint);
	}
	else if (_Vmpffd >= 0)
	{	
		close(_Vmpffd);
		_Vmpffd = -1;
	}
}

/*
 * ast semi-private workaround for system functions
 * that misbehave by passing bogus addresses to free()
 *
 * not prototyped in any header to keep it ast semi-private
 *
 * to keep malloc() data by disabling free()
 *	extern _vmkeep(int);
 *	int r = _vmkeep(1);
 * and to restore to the previous state
 *	(void)_vmkeep(r);
 */

int
#if __STD_C
_vmkeep(int v)
#else
_vmkeep(v)
int	v;
#endif
{
	int	r;

	r = !!(_Vmassert & VM_keep);
	if (v)
		_Vmassert |= VM_keep;
	else
		_Vmassert &= ~VM_keep;
	return r;
}

#endif /*_UWIN*/



/* ============================================================================
 * SOURCE 13/13: minix4\exokernel\kernel_legacy\cmd\sgs\rtld\common\malloc.c
 * Size: 7,550 bytes, Lines: 334
 * Hash: 65dd0521f758...
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
 * Copyright 2009 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*
 *	Copyright (c) 1988 AT&T
 *	  All Rights Reserved
 */

/*
 * Simplified version of malloc(), calloc() and free(), to be linked with
 * utilities that use [s]brk() and do not define their own version of the
 * routines.
 * The algorithm maps /dev/zero to get extra memory space.
 * Each call to mmap() creates a page. The pages are linked in a list.
 * Each page is divided in blocks. There is at least one block in a page.
 * New memory chunks are allocated on a first-fit basis.
 * Freed blocks are joined in larger blocks. Free pages are unmapped.
 */

#include	<stdlib.h>
#include	<sys/types.h>
#include	<sys/mman.h>
#include	<sys/debug.h>
#include	<memory.h>
#include	"_rtld.h"
#include	"msg.h"

struct block {
	size_t		size;		/* Space available for user */
	struct page	*page;		/* Backwards reference to page */
	int		status;
	struct block	*next;
	void *		memstart[1];
};

struct page {
	size_t		size;		/* Total page size (incl. header) */
	struct page	*next;
	struct block	block[1];
};

#define	FREE	0
#define	BUSY	1

#define	HDR_BLOCK	(sizeof (struct block) - sizeof (void *))
#define	HDR_PAGE	(sizeof (struct page) - sizeof (void *))

static struct page	*memstart;

#if	DEBUG
/*
 * When built for debugging, scribble a pattern over newly allocated and
 * freed memory.
 */
#define	NEWMEM		0
#define	FREMEM		1

/* LINTED */
const ulong_t	patterns[] = {
	(ulong_t)0xbaddcafebaddcafeULL, (ulong_t)0xdeadbeefdeadbeefULL
};

static void
scribble(ulong_t *membgn, int pattern, size_t size)
{
	size_t	memsize = size / sizeof (ulong_t);

	while (memsize--) {
		if (pattern == FREMEM)
			ASSERT(*membgn != patterns[pattern]);
		*membgn++ = patterns[pattern];
	}
}
#endif

/*
 * Defragmentation
 */
void
defrag()
{
	struct page	*page;
	Aliste		idx;

	for (APLIST_TRAVERSE(free_alp, idx, page)) {
		struct block	*block;

		for (block = page->block; block; block = block->next) {
			struct block	*block2;

			if (block->status == BUSY)
				continue;
			for (block2 = block->next; block2 &&
			    block2->status == FREE; block2 = block2->next) {
				block->next = block2->next;
				block->size += block2->size + HDR_BLOCK;
			}
		}

		/*
		 * If a page becomes free, leave it, and save the unmapping
		 * expense, as we'll probably come back and reclaim the page
		 * for later malloc activity.
		 *
		 * Free the defrag index.
		 */
		aplist_delete(free_alp, &idx);
	}
}

static void
split(struct block *block, size_t size)
{
	if (block->size > size + sizeof (struct block)) {
		struct block	*newblock;
		/* LINTED */
		newblock = (struct block *)
		    ((char *)block + HDR_BLOCK + size);
		newblock->next = block->next;
		block->next = newblock;
		newblock->status = FREE;
		newblock->page = block->page;
		newblock->size = block->size - size - HDR_BLOCK;
		block->size = size;
	}
}

#include <stdio.h>

/*
 * Replace both malloc() and lmalloc() (libc's private memory allocator).
 * They are both private here.
 */
#pragma weak lmalloc = malloc
void *
malloc(size_t size)
{
	struct block	*block;
	struct page	*page;

	size = S_DROUND(size);

	/*
	 * Try to locate necessary space
	 */
	for (page = memstart; page; page = page->next) {
		for (block = page->block; block; block = block->next) {
			if ((block->status == FREE) && (block->size >= size))
				goto found;
		}
	}
found:
	/*
	 * Need to allocate a new page
	 */
	if (!page) {
		size_t	totsize = size + HDR_PAGE;
		size_t	totpage = S_ROUND(totsize, syspagsz);

		if ((page = dz_map(0, 0, totpage,
		    PROT_READ | PROT_WRITE | PROT_EXEC,
		    MAP_PRIVATE)) == MAP_FAILED)
			return (0);

		page->next = memstart;
		memstart = page;
		page->size = totpage;
		block = page->block;
		block->next = 0;
		block->status = FREE;
		block->size = totpage - HDR_PAGE;
		block->page = page;
	}

	split(block, size);
#if	DEBUG
	scribble((ulong_t *)&block->memstart, NEWMEM, block->size);
#endif
	block->status = BUSY;
	return (&block->memstart);
}

void *
calloc(size_t num, size_t size)
{
	void *	mp;
	size_t	total;

	if (num == 0 || size == 0) {
		total = 0;
	} else {
		total = num * size;

		/* check for overflow */
		if ((total / num) != size) {
			errno = ENOMEM;
			return (NULL);
		}
	}

	if ((mp = malloc(total)) == NULL)
		return (NULL);
	(void) memset(mp, 0, total);
	return (mp);
}

void *
realloc(void *ptr, size_t size)
{
	struct block	*block;
	size_t		osize;
	void *		newptr;

	if (ptr == NULL)
		return (malloc(size));

	/* LINTED */
	block = (struct block *)((char *)ptr - HDR_BLOCK);
	size = S_DROUND(size);
	osize = block->size;

	/*
	 * Join block with next one if it is free
	 */
	if (block->next && block->next->status == FREE) {
		block->size += block->next->size + HDR_BLOCK;
		block->next = block->next->next;
	}

	if (size <= block->size) {
		split(block, size);
#if	DEBUG
		if (block->size > osize)
			scribble((ulong_t *)((char *)ptr + osize), NEWMEM,
			    (block->size - osize));
#endif
		return (ptr);
	}

	if ((newptr = malloc(size)) == NULL)
		return (NULL);
	(void) memcpy(newptr, ptr, osize);
	block->status = FREE;

	/*
	 * Add the free block to the free APlist for later defragmentation.
	 * However, this addition can only be achieved if there is room on the
	 * free APlist.  The APlist can't be allowed to grow, as the growth
	 * requires a realloc(), which would recurse back here, resulting in an
	 * infinite loop.  If the free APlist is full, defrag() now.  This
	 * defragmentation might not be able to collapse any free space, but
	 * the free APlist will be cleared as part of the processing, ensuring
	 * room for the addition.
	 */
	if (free_alp && (aplist_nitems(free_alp) >= aplist_arritems(free_alp)))
		defrag();
	(void) aplist_test(&free_alp, block->page, AL_CNT_FREELIST);
	return (newptr);
}

/*
 * Replace both free() and lfree() (libc's private memory allocator).
 * They are both private here.
 */
void
free(void *ptr)
{
	struct block	*block;

	if (ptr == NULL)
		return;

	/* LINTED */
	block = (struct block *)((char *)ptr - HDR_BLOCK);
	block->status = FREE;
#if	DEBUG
	scribble((ulong_t *)&block->memstart, FREMEM, block->size);
#endif
	(void) aplist_test(&free_alp, block->page, AL_CNT_FREELIST);
}

/* ARGSUSED1 */
void
lfree(void *ptr, size_t size)
{
	free(ptr);
}

/*
 * We can use any memory after ld.so.1's .bss up until the next page boundary
 * as allocatable memory.
 */
void
addfree(void *ptr, size_t bytes)
{
	struct block	*block;
	struct page	*page;

	if (bytes <= sizeof (struct page))
		return;
	page = ptr;
	page->next = memstart;
	memstart = page;
	page->size = bytes;
	block = page->block;
	block->next = 0;
	block->status = FREE;
	block->size = bytes - HDR_PAGE;
	block->page = page;
}

