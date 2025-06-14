/**
 * @file proc_unified.c
 * @brief Unified process management
 * @details This file is a synthesized/unified implementation combining multiple
 *          legacy MINIX implementations into a single, modern, C23-compliant
 *          implementation for MINIX4.
 *
 * Original sources consolidated:
 * - userspace\usr_bin_legacy\trace\proc.c
 * - minix4\libos_legacy\csh\proc.c
 * - minix4\exokernel\kernel_legacy\proc.c
 * - minix4\exokernel\kernel_legacy\uts\common\os\proc.c
 * - minix4\exokernel\kernel_legacy\cmd\auditreduce\proc.c
 * - minix4\exokernel\kernel_legacy\cmd\svc\startd\proc.c
 * - minix\process\proc.c
 * - archive\minix_legacy\proc.c
 * - archive\legacy\pascal\proc.c

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
 * This file contains 9 very different implementations that
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
 * SOURCE 1/9: userspace\usr_bin_legacy\trace\proc.c
 * Size: 1,836 bytes, Lines: 98
 * Hash: 06ed7372e72b...
 * ============================================================================ */


#include "inc.h"

static TAILQ_HEAD(, trace_proc) proc_root;
static unsigned int nr_procs;

/*
 * Initialize the list of traced processes.
 */
void
proc_init(void)
{

	TAILQ_INIT(&proc_root);
	nr_procs = 0;
}

/*
 * Add a new process to the list of traced processes, allocating memory for it
 * first.  Return the new process structure with its PID assigned and the rest
 * zeroed out, or NULL upon allocation failure (with errno set appropriately).
 */
struct trace_proc *
proc_add(pid_t pid)
{
	struct trace_proc *proc;

	proc = (struct trace_proc *)malloc(sizeof(struct trace_proc));

	if (proc == NULL)
		return NULL;

	memset(proc, 0, sizeof(*proc));

	proc->pid = pid;

	TAILQ_INSERT_TAIL(&proc_root, proc, next);
	nr_procs++;

	return proc;
}

/*
 * Retrieve the data structure for a traced process based on its PID.  Return
 * a pointer to the structure, or NULL if no structure exists for this process.
 */
struct trace_proc *
proc_get(pid_t pid)
{
	struct trace_proc *proc;

	/* Linear search for now; se we can easily add a hashtable later.. */
	TAILQ_FOREACH(proc, &proc_root, next) {
		if (proc->pid == pid)
			return proc;
	}

	return NULL;
}

/*
 * Remove a process from the list of traced processes.
 */
void
proc_del(struct trace_proc * proc)
{

	TAILQ_REMOVE(&proc_root, proc, next);
	nr_procs--;

	free(proc);
}

/*
 * Iterator for the list of traced processes.  If a NULL pointer is given,
 * return the first process in the list; otherwise, return the next process in
 * the list.  Not stable with respect to list modifications.
 */
struct trace_proc *
proc_next(struct trace_proc * proc)
{

	if (proc == NULL)
		return TAILQ_FIRST(&proc_root);
	else
		return TAILQ_NEXT(proc, next);
}

/*
 * Return the number of processes in the list of traced processes.
 */
unsigned int
proc_count(void)
{

	return nr_procs;
}



/* ============================================================================
 * SOURCE 2/9: minix4\libos_legacy\csh\proc.c
 * Size: 32,789 bytes, Lines: 1,380
 * Hash: 38b0e7743878...
 * ============================================================================ */

/* $NetBSD: proc.c,v 1.43 2025/03/20 15:02:00 buhrow Exp $ */

/*-
 * Copyright (c) 1980, 1991, 1993
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
static char sccsid[] = "@(#)proc.c	8.1 (Berkeley) 5/31/93";
#else
__RCSID("$NetBSD: proc.c,v 1.43 2025/03/20 15:02:00 buhrow Exp $");
#endif
#endif /* not lint */

#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "csh.h"
#include "dir.h"
#include "extern.h"
#include "proc.h"

#define BIGINDEX 9 /* largest desirable job index */

extern int insource;

struct process proclist;
int pnoprocesses;

struct process *pholdjob;

struct process *pcurrjob;
struct process *pcurrent;
struct process *pprevious;

int pmaxindex;

static void pflushall(void);
static void pflush(struct process *);
static void pclrcurr(struct process *);
static void padd(struct command *);
static int pprint(struct process *, int);
static void ptprint(struct process *);
static void pads(Char *);
static void pkill(Char **v, int);
static struct process *pgetcurr(struct process *);
static void okpcntl(void);

/*
 * pchild - called at interrupt level by the SIGCHLD signal
 *	indicating that at least one child has terminated or stopped
 *	thus at least one wait system call will definitely return a
 *	childs status.  Top level routines (like pwait) must be sure
 *	to mask interrupts when playing with the proclist data structures!
 */
/* ARGSUSED */
void
pchild(int notused)
{
    struct rusage ru;
    struct process *fp, *pp;
    int jobflags, pid, w;

loop:
    errno = 0;			/* reset, just in case */
    pid = wait3(&w,
       (setintr && (intty || insource) ? WNOHANG | WUNTRACED : WNOHANG), &ru);

    if (pid <= 0) {
	if (errno == EINTR) {
	    errno = 0;
	    goto loop;
	}
	pnoprocesses = pid == -1;
	return;
    }
    for (pp = proclist.p_next; pp != NULL; pp = pp->p_next)
	if (pid == pp->p_pid)
	    goto found;
    goto loop;
found:
    if (pid == atoi(short2str(value(STRchild))))
	unsetv(STRchild);
    pp->p_flags &= ~(PRUNNING | PSTOPPED | PREPORTED);
    if (WIFSTOPPED(w)) {
	pp->p_flags |= PSTOPPED;
	pp->p_reason = WSTOPSIG(w);
    }
    else {
	if (pp->p_flags & (PTIME | PPTIME) || adrof(STRtime))
	    (void)clock_gettime(CLOCK_MONOTONIC, &pp->p_etime);

	pp->p_rusage = ru;
	if (WIFSIGNALED(w)) {
	    if (WTERMSIG(w) == SIGINT)
		pp->p_flags |= PINTERRUPTED;
	    else
		pp->p_flags |= PSIGNALED;
	    if (WCOREDUMP(w))
		pp->p_flags |= PDUMPED;
	    pp->p_reason = WTERMSIG(w);
	}
	else {
	    pp->p_reason = WEXITSTATUS(w);
	    if (pp->p_reason != 0)
		pp->p_flags |= PAEXITED;
	    else
		pp->p_flags |= PNEXITED;
	}
    }
    jobflags = 0;
    fp = pp;
    do {
	if ((fp->p_flags & (PPTIME | PRUNNING | PSTOPPED)) == 0 &&
	    !child && adrof(STRtime) &&
	    fp->p_rusage.ru_utime.tv_sec + fp->p_rusage.ru_stime.tv_sec
	    >= atoi(short2str(value(STRtime))))
	    fp->p_flags |= PTIME;
	jobflags |= fp->p_flags;
    } while ((fp = fp->p_friends) != pp);
    pp->p_flags &= ~PFOREGND;
    if (pp == pp->p_friends && (pp->p_flags & PPTIME)) {
	pp->p_flags &= ~PPTIME;
	pp->p_flags |= PTIME;
    }
    if ((jobflags & (PRUNNING | PREPORTED)) == 0) {
	fp = pp;
	do {
	    if (fp->p_flags & PSTOPPED)
		fp->p_flags |= PREPORTED;
	} while ((fp = fp->p_friends) != pp);
	while (fp->p_pid != fp->p_jobid)
	    fp = fp->p_friends;
	if (jobflags & PSTOPPED) {
	    if (pcurrent && pcurrent != fp)
		pprevious = pcurrent;
	    pcurrent = fp;
	}
	else
	    pclrcurr(fp);
	if (jobflags & PFOREGND) {
	    if (jobflags & (PSIGNALED | PSTOPPED | PPTIME) ||
#ifdef IIASA
		jobflags & PAEXITED ||
#endif
		!eq(dcwd->di_name, fp->p_cwd->di_name)) {
		;		/* print in pjwait */
	    }
	    /* PWP: print a newline after ^C */
	    else if (jobflags & PINTERRUPTED) {
		(void)vis_fputc('\r' | QUOTE, cshout);
		(void)fputc('\n', cshout);
	    }
	}
	else {
	    if (jobflags & PNOTIFY || adrof(STRnotify)) {
		(void)vis_fputc('\r' | QUOTE, cshout);
		(void)fputc('\n', cshout);
		(void)pprint(pp, NUMBER | NAME | REASON);
		if ((jobflags & PSTOPPED) == 0)
		    pflush(pp);
	    }
	    else {
		fp->p_flags |= PNEEDNOTE;
		neednote++;
	    }
	}
    }
    goto loop;
}

void
pnote(void)
{
    struct process *pp;
    sigset_t osigset, nsigset;
    int flags;

    neednote = 0;
    sigemptyset(&nsigset);
    (void)sigaddset(&nsigset, SIGCHLD);
    for (pp = proclist.p_next; pp != NULL; pp = pp->p_next) {
	if (pp->p_flags & PNEEDNOTE) {
	    (void)sigprocmask(SIG_BLOCK, &nsigset, &osigset);
	    pp->p_flags &= ~PNEEDNOTE;
	    flags = pprint(pp, NUMBER | NAME | REASON);
	    if ((flags & (PRUNNING | PSTOPPED)) == 0)
		pflush(pp);
	    (void)sigprocmask(SIG_SETMASK, &osigset, NULL);
	}
    }
}

/*
 * pwait - wait for current job to terminate, maintaining integrity
 *	of current and previous job indicators.
 */
void
pwait(void)
{
    struct process *fp, *pp;
    sigset_t osigset, nsigset;

    /*
     * Here's where dead procs get flushed.
     */
    sigemptyset(&nsigset);
    (void)sigaddset(&nsigset, SIGCHLD);
    (void)sigprocmask(SIG_BLOCK, &nsigset, &osigset);
    for (pp = (fp = &proclist)->p_next; pp != NULL; pp = (fp = pp)->p_next)
	if (pp->p_pid == 0) {
	    fp->p_next = pp->p_next;
	    free(pp->p_command);
	    if (pp->p_cwd && --pp->p_cwd->di_count == 0)
		if (pp->p_cwd->di_next == 0)
		    dfree(pp->p_cwd);
	    free(pp);
	    pp = fp;
	}
    (void)sigprocmask(SIG_SETMASK, &osigset, NULL);
    pjwait(pcurrjob);
}


/*
 * pjwait - wait for a job to finish or become stopped
 *	It is assumed to be in the foreground state (PFOREGND)
 */
void
pjwait(struct process *pp)
{
    struct process *fp;
    sigset_t osigset, nsigset;
    int jobflags, reason;

    while (pp->p_pid != pp->p_jobid)
	pp = pp->p_friends;
    fp = pp;

    do {
	if ((fp->p_flags & (PFOREGND | PRUNNING)) == PRUNNING)
	    (void)fprintf(csherr, "BUG: waiting for background job!\n");
    } while ((fp = fp->p_friends) != pp);
    /*
     * Now keep pausing as long as we are not interrupted (SIGINT), and the
     * target process, or any of its friends, are running
     */
    fp = pp;
    sigemptyset(&nsigset);
    (void)sigaddset(&nsigset, SIGCHLD);
    (void)sigprocmask(SIG_BLOCK, &nsigset, &osigset);
    for (;;) {
	sigemptyset(&nsigset);
	(void)sigaddset(&nsigset, SIGCHLD);
	(void)sigprocmask(SIG_BLOCK, &nsigset, NULL);
	jobflags = 0;
	do
	    jobflags |= fp->p_flags;
	while ((fp = (fp->p_friends)) != pp);
	if ((jobflags & PRUNNING) == 0)
	    break;
#ifdef JOBDEBUG
	(void)fprintf(csherr, "starting to sigsuspend for  SIGCHLD on %d\n",
		       fp->p_pid);
#endif				/* JOBDEBUG */
	nsigset = osigset;
	(void)sigdelset(&nsigset, SIGCHLD);
	(void)sigsuspend(&nsigset);
    }
    (void)sigprocmask(SIG_SETMASK, &osigset, NULL);
    if (tpgrp > 0)		/* get tty back */
	(void)tcsetpgrp(FSHTTY, tpgrp);
    if ((jobflags & (PSIGNALED | PSTOPPED | PTIME)) ||
	!eq(dcwd->di_name, fp->p_cwd->di_name)) {
	if (jobflags & PSTOPPED) {
	    (void) fputc('\n', cshout);
	    if (adrof(STRlistjobs)) {
		Char *jobcommand[3];

		jobcommand[0] = STRjobs;
		if (eq(value(STRlistjobs), STRlong))
		    jobcommand[1] = STRml;
		else
		    jobcommand[1] = NULL;
		jobcommand[2] = NULL;

		dojobs(jobcommand, NULL);
		(void)pprint(pp, SHELLDIR);
	    }
	    else
		(void)pprint(pp, AREASON | SHELLDIR);
	}
	else
	    (void)pprint(pp, AREASON | SHELLDIR);
    }
    if ((jobflags & (PINTERRUPTED | PSTOPPED)) && setintr &&
	(!gointr || !eq(gointr, STRminus))) {
	if ((jobflags & PSTOPPED) == 0)
	    pflush(pp);
	pintr1(0);
    }
    reason = 0;
    fp = pp;
    do {
	if (fp->p_reason)
	    reason = fp->p_flags & (PSIGNALED | PINTERRUPTED) ?
		fp->p_reason | META : fp->p_reason;
    } while ((fp = fp->p_friends) != pp);
    if ((reason != 0) && (adrof(STRprintexitvalue))) {
	(void)fprintf(cshout, "Exit %d\n", reason);
    }
    set(STRstatus, putn(reason));
    if (reason && exiterr)
	exitstat();
    pflush(pp);
}

/*
 * dowait - wait for all processes to finish
 */
void
/*ARGSUSED*/
dowait(Char **v, struct command *t)
{
    struct process *pp;
    sigset_t osigset, nsigset;

    pjobs++;
    sigemptyset(&nsigset);
    (void)sigaddset(&nsigset, SIGCHLD);
    (void)sigprocmask(SIG_BLOCK, &nsigset, &osigset);
loop:
    for (pp = proclist.p_next; pp; pp = pp->p_next)
	if (pp->p_pid &&	/* pp->p_pid == pp->p_jobid && */
	    pp->p_flags & PRUNNING) {
	    sigemptyset(&nsigset);
	    (void)sigsuspend(&nsigset);
	    goto loop;
	}
    (void)sigprocmask(SIG_SETMASK, &osigset, NULL);
    pjobs = 0;
}

/*
 * pflushall - flush all jobs from list (e.g. at fork())
 */
static void
pflushall(void)
{
    struct process *pp;

    for (pp = proclist.p_next; pp != NULL; pp = pp->p_next)
	if (pp->p_pid)
	    pflush(pp);
}

/*
 * pflush - flag all process structures in the same job as the
 *	the argument process for deletion.  The actual free of the
 *	space is not done here since pflush is called at interrupt level.
 */
static void
pflush(struct process *pp)
{
    struct process *np;
    int idx;

    if (pp->p_pid == 0) {
	(void)fprintf(csherr, "BUG: process flushed twice");
	return;
    }
    while (pp->p_pid != pp->p_jobid)
	pp = pp->p_friends;
    pclrcurr(pp);
    if (pp == pcurrjob)
	pcurrjob = 0;
    idx = pp->p_index;
    np = pp;
    do {
	np->p_index = np->p_pid = 0;
	np->p_flags &= ~PNEEDNOTE;
    } while ((np = np->p_friends) != pp);
    if (idx == pmaxindex) {
	for (np = proclist.p_next, idx = 0; np; np = np->p_next)
	    if (np->p_index > idx)
		idx = np->p_index;
	pmaxindex = idx;
    }
}

/*
 * pclrcurr - make sure the given job is not the current or previous job;
 *	pp MUST be the job leader
 */
static void
pclrcurr(struct process *pp)
{
    if (pp == pcurrent) {
	if (pprevious != NULL) {
	    pcurrent = pprevious;
	    pprevious = pgetcurr(pp);
	}
	else {
	    pcurrent = pgetcurr(pp);
	    pprevious = pgetcurr(pp);
	}
    } else if (pp == pprevious)
	pprevious = pgetcurr(pp);
}

/* +4 here is 1 for '\0', 1 ea for << >& >> */
static Char command[PMAXLEN + 4];
static size_t cmdlen;
static Char *cmdp;

/*
 * palloc - allocate a process structure and fill it up.
 *	an important assumption is made that the process is running.
 */
void
palloc(int pid, struct command *t)
{
    struct process *pp;
    int i;

    pp = xcalloc(1, sizeof(*pp));
    pp->p_pid = pid;
    pp->p_flags = t->t_dflg & F_AMPERSAND ? PRUNNING : PRUNNING | PFOREGND;
    if (t->t_dflg & F_TIME)
	pp->p_flags |= PPTIME;
    cmdp = command;
    cmdlen = 0;
    padd(t);
    *cmdp++ = 0;
    if (t->t_dflg & F_PIPEOUT) {
	pp->p_flags |= PPOU;
	if (t->t_dflg & F_STDERR)
	    pp->p_flags |= PERR;
    }
    pp->p_command = Strsave(command);
    if (pcurrjob) {
	struct process *fp;

	/* careful here with interrupt level */
	pp->p_cwd = 0;
	pp->p_index = pcurrjob->p_index;
	pp->p_friends = pcurrjob;
	pp->p_jobid = pcurrjob->p_pid;
	for (fp = pcurrjob; fp->p_friends != pcurrjob; fp = fp->p_friends)
	    continue;
	fp->p_friends = pp;
    }
    else {
	pcurrjob = pp;
	pp->p_jobid = pid;
	pp->p_friends = pp;
	pp->p_cwd = dcwd;
	dcwd->di_count++;
	if (pmaxindex < BIGINDEX)
	    pp->p_index = ++pmaxindex;
	else {
	    struct process *np;

	    for (i = 1;; i++) {
		for (np = proclist.p_next; np; np = np->p_next)
		    if (np->p_index == i)
			goto tryagain;
		pp->p_index = i;
		if (i > pmaxindex)
		    pmaxindex = i;
		break;
	tryagain:;
	    }
	}
	if (pcurrent == NULL)
	    pcurrent = pp;
	else if (pprevious == NULL)
	    pprevious = pp;
    }
    pp->p_next = proclist.p_next;
    proclist.p_next = pp;
    (void)clock_gettime(CLOCK_MONOTONIC, &pp->p_btime);
}

static void
padd(struct command *t)
{
    Char **argp;

    if (t == 0)
	return;
    switch (t->t_dtyp) {
    case NODE_PAREN:
	pads(STRLparensp);
	padd(t->t_dspr);
	pads(STRspRparen);
	break;
    case NODE_COMMAND:
	for (argp = t->t_dcom; *argp; argp++) {
	    pads(*argp);
	    if (argp[1])
		pads(STRspace);
	}
	break;
    case NODE_OR:
    case NODE_AND:
    case NODE_PIPE:
    case NODE_LIST:
	padd(t->t_dcar);
	switch (t->t_dtyp) {
	case NODE_OR:
	    pads(STRspor2sp);
	    break;
	case NODE_AND:
	    pads(STRspand2sp);
	    break;
	case NODE_PIPE:
	    pads(STRsporsp);
	    break;
	case NODE_LIST:
	    pads(STRsemisp);
	    break;
	}
	padd(t->t_dcdr);
	return;
    }
    if ((t->t_dflg & F_PIPEIN) == 0 && t->t_dlef) {
	pads((t->t_dflg & F_READ) ? STRspLarrow2sp : STRspLarrowsp);
	pads(t->t_dlef);
    }
    if ((t->t_dflg & F_PIPEOUT) == 0 && t->t_drit) {
	pads((t->t_dflg & F_APPEND) ? STRspRarrow2 : STRspRarrow);
	if (t->t_dflg & F_STDERR)
	    pads(STRand);
	pads(STRspace);
	pads(t->t_drit);
    }
}

static void
pads(Char *cp)
{
    size_t i;

    /*
     * Avoid the Quoted Space alias hack! Reported by:
     * sam@john-bigboote.ICS.UCI.EDU (Sam Horrocks)
     */
    if (cp[0] == STRQNULL[0])
	cp++;

    i = Strlen(cp);

    if (cmdlen >= PMAXLEN)
	return;
    if (cmdlen + i >= PMAXLEN) {
	(void)Strcpy(cmdp, STRsp3dots);
	cmdlen = PMAXLEN;
	cmdp += 4;
	return;
    }
    (void)Strcpy(cmdp, cp);
    cmdp += i;
    cmdlen += i;
}

/*
 * psavejob - temporarily save the current job on a one level stack
 *	so another job can be created.  Used for { } in exp6
 *	and `` in globbing.
 */
void
psavejob(void)
{
    pholdjob = pcurrjob;
    pcurrjob = NULL;
}

/*
 * prestjob - opposite of psavejob.  This may be missed if we are interrupted
 *	somewhere, but pendjob cleans up anyway.
 */
void
prestjob(void)
{
    pcurrjob = pholdjob;
    pholdjob = NULL;
}

/*
 * pendjob - indicate that a job (set of commands) has been completed
 *	or is about to begin.
 */
void
pendjob(void)
{
    struct process *pp, *tp;

    if (pcurrjob && (pcurrjob->p_flags & (PFOREGND | PSTOPPED)) == 0) {
	pp = pcurrjob;
	while (pp->p_pid != pp->p_jobid)
	    pp = pp->p_friends;
	(void)fprintf(cshout, "[%d]", pp->p_index);
	tp = pp;
	do {
	    (void)fprintf(cshout, " %ld", (long)pp->p_pid);
	    pp = pp->p_friends;
	} while (pp != tp);
	(void)fputc('\n', cshout);
    }
    pholdjob = pcurrjob = 0;
}

/*
 * pprint - print a job
 */
static int
pprint(struct process *pp, int flag)
{
    static struct rusage zru;
    struct process *tp;
    const char *format;
    int jobflags, pstatus, reason, status;
    int hadnl;

    hadnl = 1; /* did we just have a newline */
    (void)fpurge(cshout);

    while (pp->p_pid != pp->p_jobid)
	pp = pp->p_friends;
    if (pp == pp->p_friends && (pp->p_flags & PPTIME)) {
	pp->p_flags &= ~PPTIME;
	pp->p_flags |= PTIME;
    }
    tp = pp;
    status = reason = -1;
    jobflags = 0;
    do {
	jobflags |= pp->p_flags;
	pstatus = pp->p_flags & PALLSTATES;
	if (tp != pp && !hadnl && !(flag & FANCY) &&
	    ((pstatus == status && pp->p_reason == reason) ||
	     !(flag & REASON))) {
	    (void)fputc(' ', cshout);
	    hadnl = 0;
	}
	else {
	    if (tp != pp && !hadnl) {
		(void)fputc('\n', cshout);
		hadnl = 1;
	    }
	    if (flag & NUMBER) {
		if (pp == tp)
		    (void)fprintf(cshout, "[%d]%s %c ", pp->p_index,
			    pp->p_index < 10 ? " " : "",
			    pp == pcurrent ? '+' :
			    (pp == pprevious ? '-' : ' '));
		else
		    (void)fprintf(cshout, "       ");
		hadnl = 0;
	    }
	    if (flag & FANCY) {
		(void)fprintf(cshout, "%5ld ", (long)pp->p_pid);
		hadnl = 0;
	    }
	    if (flag & (REASON | AREASON)) {
		if (flag & NAME)
		    format = "%-23s";
		else
		    format = "%s";
		if (pstatus == status) {
		    if (pp->p_reason == reason) {
			(void)fprintf(cshout, format, "");
			hadnl = 0;
			goto prcomd;
		    }
		    else
			reason = pp->p_reason;
		} else {
		    status = pstatus;
		    reason = pp->p_reason;
		}
		switch (status) {
		case PRUNNING:
		    (void)fprintf(cshout, format, "Running ");
		    hadnl = 0;
		    break;
		case PINTERRUPTED:
		case PSTOPPED:
		case PSIGNALED:
                    /*
                     * tell what happened to the background job
                     * From: Michael Schroeder
                     * <mlschroe@immd4.informatik.uni-erlangen.de>
                     */
                    if ((flag & REASON)
                        || ((flag & AREASON)
                            && reason != SIGINT
                            && (reason != SIGPIPE
                                || (pp->p_flags & PPOU) == 0))) {
			(void)fprintf(cshout, format,
				       sys_siglist[(unsigned char)
						   pp->p_reason]);
			hadnl = 0;
		    }
		    break;
		case PNEXITED:
		case PAEXITED:
		    if (flag & REASON) {
			if (pp->p_reason)
			    (void)fprintf(cshout, "Exit %-18d", pp->p_reason);
			else
			    (void)fprintf(cshout, format, "Done");
			hadnl = 0;
		    }
		    break;
		default:
		    (void)fprintf(csherr, "BUG: status=%-9o", status);
		}
	    }
	}
prcomd:
	if (flag & NAME) {
	    (void)fprintf(cshout, "%s", vis_str(pp->p_command));
	    if (pp->p_flags & PPOU)
		(void)fprintf(cshout, " |");
	    if (pp->p_flags & PERR)
		(void)fputc('&', cshout);
	    hadnl = 0;
	}
	if (flag & (REASON | AREASON) && pp->p_flags & PDUMPED) {
	    (void)fprintf(cshout, " (core dumped)");
	    hadnl = 0;
	}
	if (tp == pp->p_friends) {
	    if (flag & AMPERSAND) {
		(void)fprintf(cshout, " &");
		hadnl = 0;
	    }
	    if (flag & JOBDIR &&
		!eq(tp->p_cwd->di_name, dcwd->di_name)) {
		(void)fprintf(cshout, " (wd: ");
		dtildepr(value(STRhome), tp->p_cwd->di_name);
		(void)fputc(')', cshout);
		hadnl = 0;
	    }
	}
	if (pp->p_flags & PPTIME && !(status & (PSTOPPED | PRUNNING))) {
	    if (!hadnl)
		(void)fprintf(cshout, "\n\t");
	    prusage(cshout, &zru, &pp->p_rusage, &pp->p_etime,
		    &pp->p_btime);
	    hadnl = 1;
	}
	if (tp == pp->p_friends) {
	    if (!hadnl) {
		(void)fputc('\n', cshout);
		hadnl = 1;
	    }
	    if (flag & SHELLDIR && !eq(tp->p_cwd->di_name, dcwd->di_name)) {
		(void)fprintf(cshout, "(wd now: ");
		dtildepr(value(STRhome), dcwd->di_name);
		(void)fprintf(cshout, ")\n");
		hadnl = 1;
	    }
	}
    } while ((pp = pp->p_friends) != tp);
    if (jobflags & PTIME && (jobflags & (PSTOPPED | PRUNNING)) == 0) {
	if (jobflags & NUMBER)
	    (void)fprintf(cshout, "       ");
	ptprint(tp);
	hadnl = 1;
    }
    (void)fflush(cshout);
    return (jobflags);
}

static void
ptprint(struct process *tp)
{
    static struct rusage zru;
    static struct timespec ztime;
    struct rusage ru;
    struct timespec tetime, diff;
    struct process *pp;

    pp = tp;
    ru = zru;
    tetime = ztime;
    do {
	ruadd(&ru, &pp->p_rusage);
	timespecsub(&pp->p_etime, &pp->p_btime, &diff);
	if (timespeccmp(&diff, &tetime, >))
	    tetime = diff;
    } while ((pp = pp->p_friends) != tp);
    prusage(cshout, &zru, &ru, &tetime, &ztime);
}

/*
 * dojobs - print all jobs
 */
void
/*ARGSUSED*/
dojobs(Char **v, struct command *t)
{
    struct process *pp;
    int flag, i;

    flag = NUMBER | NAME | REASON;
    if (chkstop)
	chkstop = 2;
    if (*++v) {
	    if (eq(*v, STRml) && !v[1]) {
		flag |= FANCY | JOBDIR;
	    } else if (eq(*v, STRmZ)) {
		if (v[1] && v[1][0]) {
		    setproctitle("%s", short2str(v[1]));
		} else {
		    setproctitle(NULL);
		}
		return;
	    } else {
		stderror(ERR_JOBS);
	    }
    }
    for (i = 1; i <= pmaxindex; i++)
	for (pp = proclist.p_next; pp; pp = pp->p_next)
	    if (pp->p_index == i && pp->p_pid == pp->p_jobid) {
		pp->p_flags &= ~PNEEDNOTE;
		if (!(pprint(pp, flag) & (PRUNNING | PSTOPPED)))
		    pflush(pp);
		break;
	    }
}

/*
 * dofg - builtin - put the job into the foreground
 */
void
/*ARGSUSED*/
dofg(Char **v, struct command *t)
{
    struct process *pp;

    okpcntl();
    ++v;
    do {
	pp = pfind(*v);
	pstart(pp, 1);
	pjwait(pp);
    } while (*v && *++v);
}

/*
 * %... - builtin - put the job into the foreground
 */
void
/*ARGSUSED*/
dofg1(Char **v, struct command *t)
{
    struct process *pp;

    okpcntl();
    pp = pfind(v[0]);
    pstart(pp, 1);
    pjwait(pp);
}

/*
 * dobg - builtin - put the job into the background
 */
void
/*ARGSUSED*/
dobg(Char **v, struct command *t)
{
    struct process *pp;

    okpcntl();
    ++v;
    do {
	pp = pfind(*v);
	pstart(pp, 0);
    } while (*v && *++v);
}

/*
 * %... & - builtin - put the job into the background
 */
void
/*ARGSUSED*/
dobg1(Char **v, struct command *t)
{
    struct process *pp;

    pp = pfind(v[0]);
    pstart(pp, 0);
}

/*
 * dostop - builtin - stop the job
 */
void
/*ARGSUSED*/
dostop(Char **v, struct command *t)
{
    pkill(++v, SIGSTOP);
}

/*
 * dokill - builtin - superset of kill (1)
 */
void
/*ARGSUSED*/
dokill(Char **v, struct command *t)
{
    Char *signame;
    char *name;
    long signum;
    char *ep;

    signum = SIGTERM;
    v++;
    if (v[0] && v[0][0] == '-') {
	if (v[0][1] == 'l') {
	    if (v[1]) {
		if (!Isdigit(v[1][0]))
		    stderror(ERR_NAME | ERR_BADSIG);

		signum = strtol(short2str(v[1]), &ep, 10);
		if (signum < 0 || signum >= NSIG)
		    stderror(ERR_NAME | ERR_BADSIG);
		else if (signum == 0)
		    (void)fputc('0', cshout); /* 0's symbolic name is '0' */
		else
		    (void)fprintf(cshout, "%s ", sys_signame[signum]);
	    } else {
		for (signum = 1; signum < NSIG; signum++) {
		    (void)fprintf(cshout, "%s ", sys_signame[signum]);
		    if (signum == NSIG / 2)
			(void)fputc('\n', cshout);
	    	}
	    }
	    (void)fputc('\n', cshout);
	    return;
	}
	if (Isdigit(v[0][1])) {
	    signum = strtol(short2str(v[0] + 1), &ep, 10);
	    if (signum < 0 || signum >= NSIG || *ep)
		stderror(ERR_NAME | ERR_BADSIG);
	}
	else {
	    if (v[0][1] == 's' && v[0][2] == '\0')
		signame = *(++v);
	    else
		signame = &v[0][1];

	    if (signame == NULL || v[1] == NULL)
		stderror(ERR_NAME | ERR_TOOFEW);

	    name = short2str(signame);
	    for (signum = 1; signum < NSIG; signum++)
		if (!strcasecmp(sys_signame[signum], name) ||
		    (!strncasecmp("SIG", name, 3) &&	/* skip "SIG" prefix */
		     !strcasecmp(sys_signame[signum], name + 3)))
		    break;

	    if (signum == NSIG) {
		if (signame[0] == '0')
		    signum = 0;
		else {
		    setname(vis_str(signame));
		    stderror(ERR_NAME | ERR_UNKSIG);
		}
	    }
	}
	v++;
    }
    pkill(v, (int)signum);
}

static void
pkill(Char **v, int signum)
{
    struct process *pp, *np;
    Char *cp;
    sigset_t nsigset;
    int err1, jobflags, pid;
    char *ep;

    jobflags = 0;
    err1 = 0;    
    sigemptyset(&nsigset);
    (void)sigaddset(&nsigset, SIGCHLD);
    if (setintr)
	(void)sigaddset(&nsigset, SIGINT);
    (void)sigprocmask(SIG_BLOCK, &nsigset, NULL);
    gflag = 0, tglob(v);
    if (gflag) {
	v = globall(v);
	if (v == 0)
	    stderror(ERR_NAME | ERR_NOMATCH);
    }
    else {
	v = gargv = saveblk(v);
	trim(v);
    }

    while (v && (cp = *v)) {
	if (*cp == '%') {
	    np = pp = pfind(cp);
	    do
		jobflags |= np->p_flags;
	    while ((np = np->p_friends) != pp);
	    switch (signum) {
	    case SIGSTOP:
	    case SIGTSTP:
	    case SIGTTIN:
	    case SIGTTOU:
		if ((jobflags & PRUNNING) == 0) {
		    (void)fprintf(csherr, "%s: Already suspended\n",
				   vis_str(cp));
		    err1++;
		    goto cont;
		}
		break;
		/*
		 * suspend a process, kill -CONT %, then type jobs; the shell
		 * says it is suspended, but it is running; thanks jaap..
		 */
	    case SIGCONT:
		pstart(pp, 0);
		goto cont;
	    }
	    if (kill(-pp->p_jobid, signum) < 0) {
		(void)fprintf(csherr, "%s: %s\n", vis_str(cp),
			       strerror(errno));
		err1++;
	    }
	    if (signum == SIGTERM || signum == SIGHUP)
		(void)kill(-pp->p_jobid, SIGCONT);
	}
	else if (!(Isdigit(*cp) || *cp == '-'))
	    stderror(ERR_NAME | ERR_JOBARGS);
	else {
	    pid = (pid_t)strtoul(short2str(cp), &ep, 0);
	    if (*ep) {
		(void)fprintf(csherr, "%s: Badly formed number\n",
		    short2str(cp));
		err1++;
		goto cont;
	    } else if (kill(pid, signum) < 0) {
		(void)fprintf(csherr, "%d: %s\n", pid, strerror(errno));
		err1++;
		goto cont;
	    }
	    if (signum == SIGTERM || signum == SIGHUP)
		(void)kill((pid_t) pid, SIGCONT);
	}
cont:
	v++;
    }
    if (gargv)
	blkfree(gargv), gargv = 0;
    (void)sigprocmask(SIG_UNBLOCK, &nsigset, NULL);
    if (err1)
	stderror(ERR_SILENT);
}

/*
 * pstart - start the job in foreground/background
 */
void
pstart(struct process *pp, int foregnd)
{
    struct process *np;
    sigset_t osigset, nsigset;
    long jobflags;

    jobflags = 0;
    sigemptyset(&nsigset);
    (void)sigaddset(&nsigset, SIGCHLD);
    (void)sigprocmask(SIG_BLOCK, &nsigset, &osigset);
    np = pp;
    do {
	jobflags |= np->p_flags;
	if (np->p_flags & (PRUNNING | PSTOPPED)) {
	    np->p_flags |= PRUNNING;
	    np->p_flags &= ~PSTOPPED;
	    if (foregnd)
		np->p_flags |= PFOREGND;
	    else
		np->p_flags &= ~PFOREGND;
	}
    } while ((np = np->p_friends) != pp);
    if (!foregnd)
	pclrcurr(pp);
    (void)pprint(pp, foregnd ? NAME | JOBDIR : NUMBER | NAME | AMPERSAND);
    if (foregnd)
	(void)tcsetpgrp(FSHTTY, pp->p_jobid);
    if (jobflags & PSTOPPED)
	(void)kill(-pp->p_jobid, SIGCONT);
    (void)sigprocmask(SIG_SETMASK, &osigset, NULL);
}

void
panystop(int neednl)
{
    struct process *pp;

    chkstop = 2;
    for (pp = proclist.p_next; pp; pp = pp->p_next)
	if (pp->p_flags & PSTOPPED)
	    stderror(ERR_STOPPED, neednl ? "\n" : "");
}

struct process *
pfind(Char *cp)
{
    struct process *pp, *np;

    if (cp == 0 || cp[1] == 0 || eq(cp, STRcent2) || eq(cp, STRcentplus)) {
	if (pcurrent == NULL)
	    stderror(ERR_NAME | ERR_JOBCUR);
	return (pcurrent);
    }
    if (eq(cp, STRcentminus) || eq(cp, STRcenthash)) {
	if (pprevious == NULL)
	    stderror(ERR_NAME | ERR_JOBPREV);
	return (pprevious);
    }
    if (Isdigit(cp[1])) {
	int     idx = atoi(short2str(cp + 1));

	for (pp = proclist.p_next; pp; pp = pp->p_next)
	    if (pp->p_index == idx && pp->p_pid == pp->p_jobid)
		return (pp);
	stderror(ERR_NAME | ERR_NOSUCHJOB);
    }
    np = NULL;
    for (pp = proclist.p_next; pp; pp = pp->p_next)
	if (pp->p_pid == pp->p_jobid) {
	    if (cp[1] == '?') {
		Char *dp;

		for (dp = pp->p_command; *dp; dp++) {
		    if (*dp != cp[2])
			continue;
		    if (prefix(cp + 2, dp))
			goto match;
		}
	    }
	    else if (prefix(cp + 1, pp->p_command)) {
	match:
		if (np)
		    stderror(ERR_NAME | ERR_AMBIG);
		np = pp;
	    }
	}
    if (np)
	return (np);
    stderror(ERR_NAME | (cp[1] == '?' ? ERR_JOBPAT : ERR_NOSUCHJOB));
    /* NOTREACHED */
}

/*
 * pgetcurr - find most recent job that is not pp, preferably stopped
 */
static struct process *
pgetcurr(struct process *pp)
{
    struct process *np, *xp;

    xp = NULL;
    for (np = proclist.p_next; np; np = np->p_next)
	if (np != pcurrent && np != pp && np->p_pid &&
	    np->p_pid == np->p_jobid) {
	    if (np->p_flags & PSTOPPED)
		return (np);
	    if (xp == NULL)
		xp = np;
	}
    return (xp);
}

/*
 * donotify - flag the job so as to report termination asynchronously
 */
void
/*ARGSUSED*/
donotify(Char **v, struct command *t)
{
    struct process *pp;

    pp = pfind(*++v);
    pp->p_flags |= PNOTIFY;
}

/*
 * Do the fork and whatever should be done in the child side that
 * should not be done if we are not forking at all (like for simple builtin's)
 * Also do everything that needs any signals fiddled with in the parent side
 *
 * Wanttty tells whether process and/or tty pgrps are to be manipulated:
 *	-1:	leave tty alone; inherit pgrp from parent
 *	 0:	already have tty; manipulate process pgrps only
 *	 1:	want to claim tty; manipulate process and tty pgrps
 * It is usually just the value of tpgrp.
 */

int
pfork(struct command *t /* command we are forking for */, int wanttty)
{
    int pgrp, pid;
    sigset_t osigset, nsigset;
    int ignint;

    ignint = 0;
    /*
     * A child will be uninterruptible only under very special conditions.
     * Remember that the semantics of '&' is implemented by disconnecting the
     * process from the tty so signals do not need to ignored just for '&'.
     * Thus signals are set to default action for children unless: we have had
     * an "onintr -" (then specifically ignored) we are not playing with
     * signals (inherit action)
     */
    if (setintr)
	ignint = (tpgrp == -1 && (t->t_dflg & F_NOINTERRUPT))
	    || (gointr && eq(gointr, STRminus));
    /*
     * Check for maximum nesting of 16 processes to avoid Forking loops
     */
    if (child == 16)
	stderror(ERR_NESTING, 16);
    /*
     * Hold SIGCHLD until we have the process installed in our table.
     */
    sigemptyset(&nsigset);
    (void)sigaddset(&nsigset, SIGCHLD);
    (void)sigprocmask(SIG_BLOCK, &nsigset, &osigset);
    while ((pid = fork()) < 0)
	if (setintr == 0)
	    (void)sleep(FORKSLEEP);
	else {
	    (void)sigprocmask(SIG_SETMASK, &osigset, NULL);
	    stderror(ERR_NOPROC);
	}
    if (pid == 0) {
	settimes();
	pgrp = pcurrjob ? pcurrjob->p_jobid : getpid();
	pflushall();
	pcurrjob = NULL;
	child++;
	if (setintr) {
	    setintr = 0;	/* until I think otherwise */
	    /*
	     * Children just get blown away on SIGINT, SIGQUIT unless "onintr
	     * -" seen.
	     */
	    (void)signal(SIGINT, ignint ? SIG_IGN : SIG_DFL);
	    (void)signal(SIGQUIT, ignint ? SIG_IGN : SIG_DFL);
	    if (wanttty >= 0) {
		/* make stoppable */
		(void)signal(SIGTSTP, SIG_DFL);
		(void)signal(SIGTTIN, SIG_DFL);
		(void)signal(SIGTTOU, SIG_DFL);
	    }
	    (void)signal(SIGTERM, parterm);
	}
	else if (tpgrp == -1 && (t->t_dflg & F_NOINTERRUPT)) {
	    (void)signal(SIGINT, SIG_IGN);
	    (void)signal(SIGQUIT, SIG_IGN);
	}
	pgetty(wanttty, pgrp);
	/*
	 * Nohup and nice apply only to NODE_COMMAND's but it would be nice
	 * (?!?) if you could say "nohup (foo;bar)" Then the parser would have
	 * to know about nice/nohup/time
	 */
	if (t->t_dflg & F_NOHUP)
	    (void)signal(SIGHUP, SIG_IGN);
	if (t->t_dflg & F_NICE)
	    (void)setpriority(PRIO_PROCESS, 0, t->t_nice);
    }
    else {
	if (wanttty >= 0)
	    (void)setpgid(pid, pcurrjob ? pcurrjob->p_jobid : pid);
	palloc(pid, t);
	(void)sigprocmask(SIG_SETMASK, &osigset, NULL);
    }

    return (pid);
}

static void
okpcntl(void)
{
    if (tpgrp == -1)
	stderror(ERR_JOBCONTROL);
    if (tpgrp == 0)
	stderror(ERR_JOBCTRLSUB);
    /* NOTREACHED */
}

/*
 * if we don't have vfork(), things can still go in the wrong order
 * resulting in the famous 'Stopped (tty output)'. But some systems
 * don't permit the setpgid() call, (these are more recent secure
 * systems such as ibm's aix). Then we'd rather print an error message
 * than hang the shell!
 * I am open to suggestions how to fix that.
 */
void
pgetty(int wanttty, int pgrp)
{
    sigset_t osigset, nsigset;

    /*
     * christos: I am blocking the tty signals till I've set things
     * correctly....
     */
    if (wanttty > 0) {
	sigemptyset(&nsigset);
	(void)sigaddset(&nsigset, SIGTSTP);
	(void)sigaddset(&nsigset, SIGTTIN);
	(void)sigaddset(&nsigset, SIGTTOU);
	(void)sigprocmask(SIG_BLOCK, &nsigset, &osigset);
    }
    /*
     * From: Michael Schroeder <mlschroe@immd4.informatik.uni-erlangen.de>
     * Don't check for tpgrp >= 0 so even non-interactive shells give
     * background jobs process groups Same for the comparison in the other part
     * of the #ifdef
     */
    if (wanttty >= 0)
	if (setpgid(0, pgrp) == -1) {
	    (void)fprintf(csherr, "csh: setpgid error.\n");
	    xexit(0);
	}

    if (wanttty > 0) {
	(void)tcsetpgrp(FSHTTY, pgrp);
	(void)sigprocmask(SIG_SETMASK, &osigset, NULL);
    }

    if (tpgrp > 0)
	tpgrp = 0;		/* gave tty away */
}



/* ============================================================================
 * SOURCE 3/9: minix4\exokernel\kernel_legacy\proc.c
 * Size: 29,452 bytes, Lines: 736
 * Hash: 4c0aad9724db...
 * ============================================================================ */

/*
 * kernel_ipc.c
 *
 * Core process scheduling and IPC (message-passing) implementation
 * for the MINIX kernel. Together with mpx.s, this forms the lowest
 * layer of the operating system.
 *
 * Entry point:
 *   sys_call()   -- invoked on software interrupt (INT), dispatches
 *                   IPC-related system calls.
 *
 * History of major rewrites:
 *   2004-10-28  nonblocking send/receive          (J. Herder)
 *   2005-05-24  notification syscall              (J. Herder)
 *   2005-05-26  message passing functions         (J. Herder)
 *   2005-07-25  syscall handling                  (J. Herder)
 *   2005-08-19  scheduling code                   (J. Herder)
 *
 * This file is performance-critical.  Many list operations use pointer-
 * pointers to avoid head/tail special cases.
 */

#define _POSIX_C_SOURCE 202311L /* for C23 features */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Kernel abstractions */
#include <minix/kernel_types.h>
#include <sys/kassert.h>

#include "arch_proto.h"
#include "clock.h"
#include "spinlock.h"
#include "vm.h"

/* KLIB: memory, string, and printing utilities */
#include <klib/include/kmemory.h>
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>

#include "cap_table.h"
#include <minix/com.h>    // For PM_PROC_NR, needed for pre-populating PM cap
#include <minix/syslib.h> // For _ENDPOINT_P, needed for PM_SERVICE_ID_FOR_EPOCH

#include "cap_table.h"
#include <minix/com.h>    // For PM_PROC_NR, needed for pre-populating PM cap
#include <minix/syslib.h> // For _ENDPOINT_P, needed for PM_SERVICE_ID_FOR_EPOCH

/*---------------------------------------------------------------------------*
 *  Forward declarations                                                   *
 *---------------------------------------------------------------------------*/
static void idle(void);
static void switch_address_space_idle(void);

static int do_sync_ipc(struct proc *caller, int call_nr, endpoint_t src_dst,
                       message *m_ptr);
static int mini_send(struct proc *caller, endpoint_t dst, message *m_ptr,
                     int flags);
static int mini_receive(struct proc *caller, endpoint_t src, message *m_ptr,
                        int flags);
// Modifying mini_notify to accept user_m_ptr for badge extraction
static int mini_notify(const struct proc *caller, endpoint_t dst,
                       message *user_m_ptr);
static int mini_senda(struct proc *caller, asynmsg_t *table, size_t size);
static int try_async(struct proc *caller);
static int try_one(endpoint_t receive_e, struct proc *src, struct proc *dst);
static int try_deliver_senda(struct proc *caller, asynmsg_t *table,
                             size_t size);
static int cancel_async(struct proc *src, struct proc *dst);

static int deadlock(int function, struct proc *caller, endpoint_t peer);
static int has_pending(sys_map_t *map, int src_id, bool async);
static void unset_notify_pending(struct proc *caller, int src_id);

/* Scheduling queues */
static struct priv idle_priv; /* shared idle-CPU priv struct */

/*---------------------------------------------------------------------------*
 *  Initialization                                                          *
 *---------------------------------------------------------------------------*/

/**
 * Initialize process and privilege tables; set up idle tasks for each CPU.
 */
void proc_init(void) {
  struct proc *rp;
  struct priv *sp;
  int i;

  /* Clear process table */
  for (rp = BEG_PROC_ADDR, i = -NR_TASKS; rp < END_PROC_ADDR; ++rp, ++i) {
    rp->p_rts_flags = RTS_SLOT_FREE;
    rp->p_magic = PMAGIC;
    rp->p_nr = i;
    rp->p_endpoint = _ENDPOINT(0, i);
    rp->p_scheduler = NULL;
    rp->p_priority = 0;
    rp->p_quantum_size_ms = 0;

    /* init per-proc spinlock for signals */
    spin_lock_irq_init(&rp->p_sig_lock);
    arch_proc_reset(rp);
  }

  /* Clear privilege table */
  for (sp = BEG_PRIV_ADDR, i = 0; sp < END_PRIV_ADDR; ++sp, ++i) {
    sp->s_proc_nr = NONE;
    sp->s_id = (sys_id_t)i;
    ppriv_addr[i] = sp;
    sp->s_sig_mgr = NONE;
    sp->s_bak_sig_mgr = NONE;

    /* Initialize new pending notification store */
    for (int j = 0; j < MAX_PENDING_NOTIFICATIONS; ++j) {
      sp->s_pending_notifications[j].pn_in_use = 0;
      sp->s_pending_notifications[j].pn_source = NONE; /* Endpoint NONE */
      sp->s_pending_notifications[j].pn_badge = 0;
    }
  }

  /* Idle task for each CPU */
  idle_priv.s_flags = IDL_F;
  for (i = 0; i < CONFIG_MAX_CPUS; i++) {
    struct proc *ip = get_cpu_var_ptr(i, idle_proc);
    ip->p_endpoint = IDLE;
    ip->p_priv = &idle_priv;
    ip->p_rts_flags |= RTS_PROC_STOP;
    snprintf(ip->p_name, sizeof(ip->p_name), "idle%d", i);
  }
}

/**
 * Idle loop: invoked when no runnable processes; halts CPU and accounts time.
 */
static void idle(void) {
  struct proc *p = get_cpulocal_var_ptr(idle_proc);
  get_cpulocal_var(proc_ptr) = p;
  if (priv(p)->s_flags & BILLABLE)
    get_cpulocal_var(bill_ptr) = p;

  switch_address_space_idle();

#ifdef CONFIG_SMP
  get_cpulocal_var(cpu_is_idle) = 1;
  if (cpuid != bsp_cpu_id)
    stop_local_timer();
  else
#endif
    restart_local_timer();

  context_stop(proc_addr(KERNEL));

#if !SPROFILE
  halt_cpu();
#else
  if (!sprofiling)
    halt_cpu();
  else {
    volatile int *v = get_cpulocal_var_ptr(idle_interrupted);
    interrupts_enable();
    while (!*v)
      arch_pause();
    interrupts_disable();
    *v = 0;
  }
#endif
}

/**
 * Switch address space to VM's for idle.
 */
static void switch_address_space_idle(void) {
#ifdef CONFIG_SMP
  switch_address_space(proc_addr(VM_PROC_NR));
#endif
}

/*---------------------------------------------------------------------------*
 *  System call dispatch                                                    *
 *---------------------------------------------------------------------------*/

/**
 * Entry point: invoked on software trap. Dispatch IPC-related calls.
 */
int do_ipc(reg_t r1, reg_t r2, reg_t r3) {
  struct proc *caller = get_cpulocal_var(proc_ptr);
  int call_nr = (int)r1;

  KASSERT(!RTS_ISSET(caller, RTS_SLOT_FREE));
  kbill_ipc = caller;

  /* handle syscall tracing/defer flags */
  if (caller->p_misc_flags & (MF_SC_TRACE | MF_SC_DEFER)) {
    if ((caller->p_misc_flags & (MF_SC_TRACE | MF_SC_DEFER)) == MF_SC_TRACE) {
      caller->p_misc_flags &= ~MF_SC_TRACE;
      caller->p_misc_flags |= MF_SC_DEFER;
      caller->p_defer.r1 = r1;
      caller->p_defer.r2 = r2;
      caller->p_defer.r3 = r3;
      cause_sig(proc_nr(caller), SIGTRAP);
      return caller->p_reg.retreg;
    }
    caller->p_misc_flags &= ~MF_SC_DEFER;
    caller->p_misc_flags |= MF_SC_ACTIVE;
  }

  if (caller->p_misc_flags & MF_DELIVERMSG) {
    panic("do_ipc: pending delivermsg for %s/%d", caller->p_name,
          caller->p_endpoint);
  }

  switch (call_nr) {
  case SENDREC:
  case SEND:
  case RECEIVE:
  case NOTIFY:
  case SENDNB:
    caller->p_accounting.ipc_sync++;
    return do_sync_ipc(caller, call_nr, (endpoint_t)r2, (message *)r3);

  case SENDA: {
    size_t msg_size = (size_t)r2;
    caller->p_accounting.ipc_async++;
    if (msg_size > 16 * (NR_TASKS + NR_PROCS))
      return EDOM;
    return mini_senda(caller, (asynmsg_t *)r3, msg_size);
  }

  case MINIX_KERNINFO:
    if (!minix_kerninfo_user)
      return EBADCALL;
    arch_set_secondary_ipc_return(caller, minix_kerninfo_user);
    return OK;

  case SYS_IPC_SEND_CAP: /* New capability-based send syscall */
    // r1 is call_nr (SYS_IPC_SEND_CAP)
    // r2 is cap_idx (int)
    // r3 is user_msg_ptr (message *)
    // caller is proc_ptr (struct proc *)
    return sys_ipc_send_cap(caller, (int)r2, (message *)r3);

  default:
    return EBADCALL;
  }
}

/**
 * Handler for synchronous IPC calls (SEND, RECEIVE, SENDREC, NOTIFY).
 */
static int do_sync_ipc(struct proc *caller, int call_nr, endpoint_t src_dst,
                       message *m_ptr) {
  int result, target_slot;
  const char *callname = ipc_call_names[call_nr];

  /* validate call number and permissions... */
  KASSERT(call_nr >= 0 && call_nr < IPCNO_HIGHEST && ipc_call_names[call_nr]);

  /* map endpoint to slot */
  if (src_dst != ANY) {
    if (!isokendpt(src_dst, &target_slot))
      return EDEADSRCDST;
    if (!may_send_to(caller, target_slot) && call_nr != RECEIVE)
      return ECALLDENIED;
  } else if (call_nr != RECEIVE) {
    return EINVAL;
  } else {
    target_slot = ANY;
  }

  /* check trap mask */
  if (!(priv(caller)->s_trap_mask & (1 << call_nr)))
    return ETRAPDENIED;

  switch (call_nr) {
  case SENDREC:
    caller->p_misc_flags |= MF_REPLY_PEND;
    /* fall through */
  case SEND:
    result = mini_send(caller, src_dst, m_ptr, 0);
    if (call_nr == SEND || result != OK)
      break;
    /* fall through for SENDREC */
  case RECEIVE:
    if (call_nr == RECEIVE) {
      caller->p_misc_flags &= ~MF_REPLY_PEND;
      IPC_STATUS_CLEAR(caller);
    }
    result = mini_receive(caller, src_dst, m_ptr, 0);
    break;

  case NOTIFY:
    result = mini_notify(caller, src_dst);
    break;
  case NOTIFY:
    // Pass m_ptr (user's message) to mini_notify for badge extraction
    result = mini_notify(caller, src_dst, m_ptr);
    break;

  case SENDNB:
    result = mini_send(caller, src_dst, m_ptr, NON_BLOCKING);
    break;

  default:
    result = EBADCALL;
  }

  return result;
}

/*---------------------------------------------------------------------------*
 *  deadlock detection                                                       *
 *---------------------------------------------------------------------------*/
static int deadlock(int function, struct proc *caller, endpoint_t peer_e) {
  struct proc *xp;
  int group_size = 1;
  endpoint_t next = peer_e;

  while (next != ANY) {
    int peer_slot;
    okendpt(next, &peer_slot);
    xp = proc_addr(peer_slot);
    KASSERT(proc_ptr_ok(xp));

    /* no dependency → no cycle */
    next = P_BLOCKEDON(xp);
    if (next == NONE)
      return 0;

    group_size++;
    if (next == caller->p_endpoint) {
      /* two-party case: SEND & RECEIVE pairing is OK */
      if (group_size == 2 &&
          ((xp->p_rts_flags ^ (function << 2)) & RTS_SENDING)) {
        return 0;
      }
      /* otherwise → deadlock */
      return group_size;
    }
  }

  return 0;
}

/*---------------------------------------------------------------------------*
 *  Notification macros                                                      *
 *---------------------------------------------------------------------------*/
/** Zero and setup a notification message for dst_proc from src_id. */
#define BuildNotifyMessage(msg, src_id, dst_proc)                              \
  do {                                                                         \
    kmemset((msg), 0, sizeof(*(msg)));                                         \
    (msg)->m_type = NOTIFY_MESSAGE;                                            \
    (msg)->m_notify.timestamp = get_monotonic();                               \
    if ((src_id) == HARDWARE) {                                                \
      (msg)->m_notify.interrupts = priv(dst_proc)->s_int_pending;              \
      priv(dst_proc)->s_int_pending = 0;                                       \
    } else {                                                                   \
      kmemcpy(&(msg)->m_notify.sigset, &priv(dst_proc)->s_sig_pending,         \
              sizeof(k_sigset_t));                                             \
      k_sigemptyset(&priv(dst_proc)->s_sig_pending);                           \
    }                                                                          \
  } while (0)
/** Zero and setup a notification message for dst_proc from src_proc_nr,
 * including badge. */
#define BuildNotifyMessageWithBadge(msg, src_proc_nr_or_hw, dst_proc_ptr,      \
                                    badge_val)                                 \
  do {                                                                         \
    kmemset((msg), 0, sizeof(*(msg)));                                         \
    (msg)->m_type = NOTIFY_MESSAGE;                                            \
    /* m_source will be set by the caller of this macro (e.g. mini_receive     \
     * from pending) */                                                        \
    (msg)->m_notify.timestamp = get_monotonic();                               \
    (msg)->m_notify.badge = (badge_val);                                       \
    if ((src_proc_nr_or_hw) == HARDWARE) {                                     \
      (msg)->m_notify.interrupts = priv(dst_proc_ptr)->s_int_pending;          \
      priv(dst_proc_ptr)->s_int_pending = 0;                                   \
      (msg)->m_notify.sigset = 0; /* Clear */                                  \
    } else if ((src_proc_nr_or_hw) == SYSTEM) {                                \
      kmemcpy(&(msg)->m_notify.sigset, &priv(dst_proc_ptr)->s_sig_pending,     \
              sizeof(k_sigset_t));                                             \
      k_sigemptyset(&priv(dst_proc_ptr)->s_sig_pending);                       \
      (msg)->m_notify.interrupts = 0; /* Clear */                              \
    } else { /* General notification from a process */                         \
      (msg)->m_notify.interrupts = 0;                                          \
      (msg)->m_notify.sigset = 0;                                              \
    }                                                                          \
  } while (0)

/*---------------------------------------------------------------------------*
 *  Notification delivery                                                   *
 *---------------------------------------------------------------------------*/
/**
 * Send or queue asynchronous notification.
 */
static int mini_notify(const struct proc *caller, endpoint_t dst_e) {
  int dst_slot;
  if (!isokendpt(dst_e, &dst_slot))
    return EDEADSRCDST;
  static int mini_notify(const struct proc *caller, endpoint_t dst_e,
                         message *user_m_ptr) {
    int dst_slot;
    u32_t badge_value = 0;

    if (!isokendpt(dst_e, &dst_slot))
      return EDEADSRCDST;

    struct proc *dst = proc_addr(dst_slot);
    message m; /* temp buffer */

    if (WILLRECEIVE(caller->p_endpoint, dst, 0, &m) &&
        !(dst->p_misc_flags & MF_REPLY_PEND)) {
      BuildNotifyMessage(&dst->p_delivermsg, proc_nr(caller), dst);
      dst->p_delivermsg.m_source = caller->p_endpoint;
      dst->p_misc_flags |= MF_DELIVERMSG;
      IPC_STATUS_ADD_CALL(dst, NOTIFY);
      RTS_UNSET(dst, RTS_RECEIVING);
      return OK;
    }
    struct proc *dst_ptr =
        proc_addr(dst_slot); // Renamed to dst_ptr for clarity

    // Extract badge from user_m_ptr. Assuming user sets m_notify.badge.
    // If kernel internal call (user_m_ptr is NULL), badge remains 0.
    if (user_m_ptr != NULL) {
      badge_value = user_m_ptr->m_notify.badge;
    }

    // Attempt immediate delivery
    if (WILLRECEIVE(
            caller->p_endpoint, dst_ptr, 0,
            NULL) && // Pass NULL for message, not needed for WILLRECEIVE check
        !(dst_ptr->p_misc_flags &
          MF_REPLY_PEND)) { // Not busy with SENDREC reply object

      // Build message directly into dst_ptr->p_delivermsg
      BuildNotifyMessageWithBadge(
          &dst_ptr->p_delivermsg,
          proc_nr(caller), /* Use proc_nr for src_id type calls */
          dst_ptr, badge_value);
      dst_ptr->p_delivermsg.m_source =
          caller->p_endpoint; // Set the true source endpoint
      dst_ptr->p_misc_flags |= MF_DELIVERMSG;
      IPC_STATUS_ADD_CALL(dst_ptr, NOTIFY);
      RTS_UNSET(dst_ptr, RTS_RECEIVING); // Unblock receiver
      return OK;
    }

    /* queue for later delivery */
    set_sys_bit(priv(dst)->s_notify_pending, priv(caller)->s_id);
    return OK;
    // Immediate delivery failed, try to queue using new s_pending_notifications
    // array
    struct priv *dst_priv = priv(dst_ptr);
    int i;
    int stored = 0;
    for (i = 0; i < MAX_PENDING_NOTIFICATIONS; ++i) {
      if (!dst_priv->s_pending_notifications[i].pn_in_use) {
        dst_priv->s_pending_notifications[i].pn_source = caller->p_endpoint;
        dst_priv->s_pending_notifications[i].pn_badge = badge_value;
        dst_priv->s_pending_notifications[i].pn_in_use = 1;
        stored = 1;
        break;
      }
    }

    if (!stored) {
      // Notification queue is full, drop the notification.
      // Optional: kprintf warning about dropped notification.
      kprintf("mini_notify: Pending notification queue for %d full, dropping "
              "notification from %d with badge %u\n",
              dst_e, caller->p_endpoint, badge_value);
    }

    // NOTIFY is best-effort, so return OK even if dropped or if it replaced
    // s_notify_pending logic. The old set_sys_bit for s_notify_pending is now
    // fully replaced.
    return OK;
  }

  /*---------------------------------------------------------------------------*
   *  Pending check helpers                                                    *
   *---------------------------------------------------------------------------*/
  static int has_pending(sys_map_t * map, int src_id, bool async) {
    if (src_id != ANY) {
      if (get_sys_bit(*map, nr_to_id(src_id)))
        return src_id;
      return NULL_PRIV_ID;
    }
    for (int i = 0; i < NR_SYS_PROCS; ++i) {
      if (get_sys_bit(*map, i)) {
        struct proc *p = proc_addr(id_to_nr(i));
        if (async && RTS_ISSET(p, RTS_VMINHIBIT)) {
          p->p_misc_flags |= MF_SENDA_VM_MISS;
          continue;
        }
        return i;
      }
    }
    return NULL_PRIV_ID;
    static int has_pending(sys_map_t * map, int src_id, bool async) {
      // This function was for the old sys_map_t s_notify_pending and
      // s_asyn_pending. It might need to be adapted or is partially deprecated
      // if s_notify_pending is fully removed. For now, keep its logic for
      // s_asyn_pending if that's still in use. If only s_notify_pending is
      // changed, then this function's use for notifications will be replaced by
      // iterating the new s_pending_notifications array.
      if (src_id != ANY) {
        // Assuming 'map' could be s_asyn_pending. If it was s_notify_pending,
        // this path is dead for notifications.
        if (get_sys_bit(*map, nr_to_id(src_id)))
          return src_id;
        return NULL_PRIV_ID;
      }
      for (int i = 0; i < NR_SYS_PROCS; ++i) {
        if (get_sys_bit(*map, i)) {
          struct proc *p = proc_addr(id_to_nr(i));
          if (async &&
              RTS_ISSET(p, RTS_VMINHIBIT)) { // async seems to relate to SENDA /
                                             // s_asyn_pending
            p->p_misc_flags |= MF_SENDA_VM_MISS;
            continue;
          }
          return i;
        }
      }
      return NULL_PRIV_ID;
    }
    static void unset_notify_pending(struct proc * caller, int src_id) {
      unset_sys_bit(priv(caller)->s_notify_pending, src_id);
      static void unset_notify_pending(struct proc * caller, int src_id) {
        // This function was for the old s_notify_pending.
        // It should be removed or adapted if a similar "clear one specific
        // source" is needed for the new s_pending_notifications array (which
        // would involve finding it and clearing pn_in_use). For now, commenting
        // out its body as direct bit manipulation is no longer correct for
        // notifications.
        /* unset_sys_bit(priv(caller)->s_notify_pending, src_id); */
        // New logic: Iterate s_pending_notifications, find matching src_id, and
        // clear it. This function might not be directly called anymore if
        // mini_receive clears slots directly.
        struct priv *caller_priv = priv(caller);
        for (int i = 0; i < MAX_PENDING_NOTIFICATIONS; ++i) {
          if (caller_priv->s_pending_notifications[i].pn_in_use &&
              caller_priv->s_pending_notifications[i].pn_source ==
                  _ENDPOINT(0, src_id)) { // Assuming src_id is proc_nr
            caller_priv->s_pending_notifications[i].pn_in_use = 0;
            caller_priv->s_pending_notifications[i].pn_source = NONE;
            caller_priv->s_pending_notifications[i].pn_badge = 0;
            // break; // Assuming only one match, or clear all from this source?
            // The old unset_sys_bit only cleared one bit.
          }
        }
      }

/*---------------------------------------------------------------------------*
 *  Asynchronous send (mini_senda)                                           *
 *---------------------------------------------------------------------------*/
/** Complain macro when asynmsg operations fail */
#define ASCOMPLAIN(caller, entry, field)                                       \
  kprintf_stub("kernel:%s:%d: asyn failed for %s in %s (%zu/%zu)\n", __FILE__, \
               __LINE__, field, (caller)->p_name, (size_t)entry,               \
               (size_t)(caller)->p_priv->s_asynsize)
#define ASCOMPLAIN(caller, entry, field)                                       \
  kprintf("kernel:%s:%d: asyn failed for %s in %s (%zu/%zu)\n", __FILE__,      \
          __LINE__, field, (caller)->p_name, (size_t)entry,                    \
          (size_t)(caller)->p_priv->s_asynsize)

/* ... Implementation of try_deliver_senda, try_async, try_one,
   cancel_async, mini_senda follows the same pattern: retrieve table,
   iterate entries, handle copy, deliver or queue, set/clear flags,
   notify ASYNCM when done. Omitted here for brevity. ... */

/*---------------------------------------------------------------------------*
 *  Scheduling queue operations                                              *
 *---------------------------------------------------------------------------*/
/* enqueue, dequeue, pick_proc and related functions omitted for brevity */

/*---------------------------------------------------------------------------*
 *  Context switch: switch_to_user                                           *
 *---------------------------------------------------------------------------*/
/* switch_to_user and FPU exception handler omitted for brevity */

// Placeholder for mini_receive modification - this is complex and needs full
// context The diff below will only focus on mini_notify and related helpers for
// now. The actual modification of mini_receive to iterate
// s_pending_notifications will be done in the next diff block.

/*===========================================================================*
 * MDLM and Capability PoC Functions                                         *
 *===========================================================================*/
#include <minix/capability.h>
#include <minix/ipc_mdlm.h> // For ipc_state_t (if used directly by proc) and CONFIG_MDLM
#include <minix/syslib.h> // For data_copy
#ifdef CONFIG_MDLM
#include <minix/mdlm_cap_dag.h> // For mdlm_cap_dag_access_check
#endif
#include <minix/profile.h> // For Profiling Macros

      // Note: The old static pm_capability_instance and the cap_lookup stub
      // that were here are now removed. The actual cap_lookup is in
      // capability.c and uses per-process tables. init_proc_capabilities (in
      // capability.c) sets up initial capabilities.

      // The old mdlm_access_check stub is removed.
      // Calls will now go to mdlm_cap_dag_access_check from mdlm_cap_dag.c

#ifdef CONFIG_MDLM
// No local stub for mdlm_access_check anymore.
#endif

      // Core IPC fast send logic (skeleton)
      static int ipc_fast_send(struct proc * caller_ptr, capability_t * cap,
                               message * msg) {
        if (!cap) {
          return EINVAL; // No capability provided
        }

        // Target endpoint from capability
        endpoint_t target_ep = cap->target;
        int target_proc_nr;

        // Validate target endpoint and get process number
        // isokendpt also fills in the process number (slot)
        if (!isokendpt(target_ep, &target_proc_nr)) {
          return EDEADSRCDST;
        }

        // For this PoC, we are simplifying. A real fast path might have its own
        // streamlined checks and delivery logic, possibly bypassing some parts
        // of mini_send. Here, we'll just print and conceptually call mini_send.
        // kprintf("IPC: ipc_fast_send from %d to %d (proc %d), type %d\n",
        //         caller_ptr->p_endpoint, target_ep, target_proc_nr,
        //         msg->m_type);

        // For the getpid() PoC, the target is PM.
        // The actual message delivery for this PoC would be to make PM handle a
        // GETPID request. This simplified ipc_fast_send will just use
        // mini_send. A true "fast path" might avoid some overhead of mini_send
        // if certain conditions are met. (e.g. if receiver is already waiting,
        // copy directly, no full queue/state changes) This is a placeholder for
        // where that optimized logic would go. For now, it demonstrates the
        // capability check and then calls the existing path.
        return mini_send(caller_ptr, target_ep, msg, 0 /* flags */);
      }

      // New syscall handler for capability-based send
      // This would be registered or called from do_ipc if it were a real
      // syscall. Parameters: cap_idx (from user), user_msg_ptr (pointer to user
      // message) Caller is implicit (current_proc from get_cpulocal_var).
      int sys_ipc_send_cap(struct proc * caller_ptr, int cap_idx,
                           message *user_msg_ptr) {
        capability_t *cap;
        message k_message; // Kernel copy of the message
        int result;

        PROF_START(PROF_ID_IPC_SYSCALL_ENTRY);

        PROF_START(PROF_ID_IPC_MSG_COPY);
        // 1. Copy message from user space to kernel space
        // In MINIX, data_copy is typically used. It's available via syslib.h
        // for user space, but in kernel, direct memory copy or specific
        // kernel_phys_copy might be used if dealing with physical addresses, or
        // direct struct copy if virtual addresses are fine. Let's assume
        // data_copy or equivalent is available/adapted for kernel->kernel or
        // user->kernel. For copying from user caller_ptr to kernel's k_message:
        result =
            data_copy(caller_ptr->p_endpoint, (vir_bytes)user_msg_ptr, KERNEL,
                      (vir_bytes)&k_message, (phys_bytes)sizeof(message));
        if (result != OK) {
          kprintf(
              "sys_ipc_send_cap: data_copy failed from user %d with error %d\n",
              caller_ptr->p_endpoint, result);
          PROF_END(PROF_ID_IPC_MSG_COPY);
          PROF_END(PROF_ID_IPC_SYSCALL_ENTRY);
          return result; // Propagate EFAULT or other error from data_copy
        }
        PROF_END(PROF_ID_IPC_MSG_COPY);
        k_message.m_source =
            caller_ptr
                ->p_endpoint; // Set source endpoint in kernel message copy

        PROF_START(PROF_ID_IPC_CAP_LOOKUP);
        cap = cap_lookup(caller_ptr->p_endpoint, cap_idx);
        PROF_END(PROF_ID_IPC_CAP_LOOKUP);

        if (!cap) {
          PROF_END(PROF_ID_IPC_SYSCALL_ENTRY);
          return EINVAL; // Invalid capability index or lookup failed
        }

#ifdef CONFIG_MDLM
        PROF_START(PROF_ID_IPC_MDLM_CHECK);
        if (!mdlm_cap_dag_access_check(caller_ptr, cap,
                                       OP_SEND)) { // Updated call
          PROF_END(PROF_ID_IPC_MDLM_CHECK);
          PROF_END(PROF_ID_IPC_SYSCALL_ENTRY);
          return EACCES; // MDLM permission denied
        }
        PROF_END(PROF_ID_IPC_MDLM_CHECK);
#endif

        PROF_START(PROF_ID_IPC_FAST_SEND);
        result = ipc_fast_send(caller_ptr, cap, &k_message);
        PROF_END(PROF_ID_IPC_FAST_SEND);

        PROF_END(PROF_ID_IPC_SYSCALL_ENTRY);
        return result;
      }

      // Skeleton for ipc_fast_reply
      // This function is intended to be called by a syscall handler for replies
      // in the new IPC mechanism. For the getpid() PoC, the app calls
      // SYS_IPC_SEND_CAP, then a regular RECEIVE. PM calls a regular send to
      // reply. So this ipc_fast_reply might not be directly used by PM in this
      // specific PoC flow, but is part of the conceptual design. If the app
      // were to use a new SYS_IPC_RECEIVE_CAP, that new syscall might
      // internally note that it's expecting a reply via this path, and a
      // corresponding SYS_IPC_REPLY_CAP from PM would then perhaps call
      // ipc_fast_reply. For now, it's a conceptual counterpart to
      // ipc_fast_send.
      static int ipc_fast_reply(struct proc * replier_ptr,
                                endpoint_t original_caller_ep,
                                message * reply_msg) {
        // kprintf("IPC: ipc_fast_reply from %d to %d, type %d\n",
        //         replier_ptr->p_endpoint, original_caller_ep,
        //         reply_msg->m_type);

        // Ensure the reply message has its source set correctly.
        reply_msg->m_source = replier_ptr->p_endpoint;

        // For this PoC, just use mini_send to send the reply.
        // A more optimized version might have different logic.
        return mini_send(replier_ptr, original_caller_ep, reply_msg,
                         0 /* flags for blocking send */);
      }



/* ============================================================================
 * SOURCE 4/9: minix4\exokernel\kernel_legacy\uts\common\os\proc.c
 * Size: 4,128 bytes, Lines: 174
 * Hash: a8ae36d728f7...
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
 * Copyright 2006 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 * Copyright (c) 2013, Joyent, Inc.  All rights reserved.
 */

#include <sys/proc.h>
#include <sys/cpuvar.h>
#include <sys/disp.h>

/*
 * Install process context ops for the current process.
 */
void
installpctx(
	proc_t *p,
	void	*arg,
	void	(*save)(void *),
	void	(*restore)(void *),
	void	(*fork)(void *, void *),
	void	(*exit)(void *),
	void	(*free)(void *, int))
{
	struct pctxop *pctx;

	pctx = kmem_alloc(sizeof (struct pctxop), KM_SLEEP);
	pctx->save_op = save;
	pctx->restore_op = restore;
	pctx->fork_op = fork;
	pctx->exit_op = exit;
	pctx->free_op = free;
	pctx->arg = arg;
	pctx->next = p->p_pctx;
	p->p_pctx = pctx;
}

/*
 * Remove a process context ops from the current process.
 */
int
removepctx(
	proc_t *p,
	void	*arg,
	void	(*save)(void *),
	void	(*restore)(void *),
	void	(*fork)(void *, void *),
	void	(*exit)(void *),
	void	(*free)(void *, int))
{
	struct pctxop *pctx, *prev_pctx;

	prev_pctx = NULL;
	kpreempt_disable();
	for (pctx = p->p_pctx; pctx != NULL; pctx = pctx->next) {
		if (pctx->save_op == save && pctx->restore_op == restore &&
		    pctx->fork_op == fork &&
		    pctx->exit_op == exit && pctx->free_op == free &&
		    pctx->arg == arg) {
			if (prev_pctx)
				prev_pctx->next = pctx->next;
			else
				p->p_pctx = pctx->next;
			if (pctx->free_op != NULL)
				(pctx->free_op)(pctx->arg, 0);
			kmem_free(pctx, sizeof (struct pctxop));
			kpreempt_enable();
			return (1);
		}
		prev_pctx = pctx;
	}
	kpreempt_enable();
	return (0);
}

void
savepctx(proc_t *p)
{
	struct pctxop *pctx;

	ASSERT(p == curthread->t_procp);
	for (pctx = p->p_pctx; pctx != 0; pctx = pctx->next)
		if (pctx->save_op != NULL)
			(pctx->save_op)(pctx->arg);
}

void
restorepctx(proc_t *p)
{
	struct pctxop *pctx;

	ASSERT(p == curthread->t_procp);
	for (pctx = p->p_pctx; pctx != 0; pctx = pctx->next)
		if (pctx->restore_op != NULL)
			(pctx->restore_op)(pctx->arg);
}

void
forkpctx(proc_t *p, proc_t *cp)
{
	struct pctxop *pctx;

	for (pctx = p->p_pctx; pctx != NULL; pctx = pctx->next)
		if (pctx->fork_op != NULL)
			(pctx->fork_op)(p, cp);
}

/*
 * exitpctx is called during thread/lwp exit to perform any actions
 * needed when an LWP in the process leaves the processor for the last
 * time. This routine is not intended to deal with freeing memory; freepctx()
 * is used for that purpose during proc_exit(). This routine is provided to
 * allow for clean-up that can't wait until thread_free().
 */
void
exitpctx(proc_t *p)
{
	struct pctxop *pctx;

	for (pctx = p->p_pctx; pctx != NULL; pctx = pctx->next)
		if (pctx->exit_op != NULL)
			(pctx->exit_op)(p);
}

/*
 * freepctx is called from proc_exit() to get rid of the actual context ops.
 */
void
freepctx(proc_t *p, int isexec)
{
	struct pctxop *pctx;

	kpreempt_disable();
	while ((pctx = p->p_pctx) != NULL) {
		p->p_pctx = pctx->next;
		if (pctx->free_op != NULL)
			(pctx->free_op)(pctx->arg, isexec);
		kmem_free(pctx, sizeof (struct pctxop));
	}
	kpreempt_enable();
}

boolean_t
secflag_enabled(proc_t *p, secflag_t flag)
{
	return (secflag_isset(p->p_secflags.psf_effective, flag));
}

void
secflags_promote(proc_t *p)
{
	secflags_copy(&p->p_secflags.psf_effective, &p->p_secflags.psf_inherit);
}



/* ============================================================================
 * SOURCE 5/9: minix4\exokernel\kernel_legacy\cmd\auditreduce\proc.c
 * Size: 27,790 bytes, Lines: 1,048
 * Hash: 83e3c19aa528...
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

/*
 * Main processor for auditreduce.
 * Mproc() is the entry point for this module. It is the only visible
 * function in this module.
 */

#include <sys/types.h>
#include <locale.h>
#include <bsm/libbsm.h>
#include <bsm/audit.h>
#include "auditr.h"

extern int	write_header();
extern int	token_processing();

static void	asort();
static audit_pcb_t *aget();
static int	get_file();
static int	write_recs();
static int	get_recs();
static int	check_rec();
static void	check_order();
static int	check_header();
static int	get_record();

static char	empty_file_token[] = {
#ifdef _LP64
		AUT_OTHER_FILE64, /* token id */
		0, 0, 0, 0, 0, 0, 0, 0, /* seconds of time */
		0, 0, 0, 0, 0, 0, 0, 0, /* microseconds of time */
#else
		AUT_OTHER_FILE32, /* token id */
		0, 0, 0, 0, /* seconds of time */
		0, 0, 0, 0, /* microseconds of time */
#endif
		0, 0, /* length of path name */
};


/*
 * .func	mproc - main processor.
 * .desc	Mproc controls a single process's actions.
 *	First one record is retreived from each pcb. As they are retreived
 *	they are placed into a linked list sorted with oldest first. Then
 *	the first one from the list is written out and another record
 *	read in to replace it. The new record is placed into the list.
 *	This continues until the list is empty.
 * .call	ret = mproc(pcbr).
 * .arg	pcbr	- ptr to pcb for this process.
 * .ret	0	- no errors in processing.
 * .ret	-1	- errors in processing (message already printed).
 */
int
mproc(pcbr)
register audit_pcb_t *pcbr;
{
	int	i, ret, junk;
	int	nrecs = 0;		/* number of records read from stream */
	int	nprecs = 0;		/* number of records put to stream */
	register audit_pcb_t *pcb;
	audit_pcb_t *aget();
	void	asort();

#if AUDIT_PROC_TRACE
	(void) fprintf(stderr, "mproc: count %d lo %d hi %d\n",
	    pcbr->pcb_count, pcbr->pcb_lo, pcbr->pcb_hi);
#endif

	/*
	 * First load up a record from each input group.
	 */
	for (i = pcbr->pcb_lo; i <= pcbr->pcb_hi; i++) {
		pcb = &(pcbr->pcb_below[i]); /* get next PCB */
		while (pcb->pcb_time < 0) { /* while no active record ... */
			if ((ret = get_file(pcb)) == -1)
				break;		/*  no files - finished PCB */
			if (ret == -2)
				return (-1);	/* quit processing - failed */
			if (get_recs(pcb, &nrecs) == 0)
				asort(pcb);	/* got a rec - put in list */
		}
	}
	/*
	 * Now process all of the records.
	 */
	while ((pcb = aget()) != NULL) {	/* get oldest record */
		if (write_recs(pcbr, pcb, &nprecs))
			return (-1);
		while (pcb->pcb_time < 0) {	/* while we don't have a rec */
			if (pcb->pcb_fpr == NULL) {	/* no active file ... */
				if ((ret = get_file(pcb)) == -1)
					break;	/* no files - finished pcb */
				else if (ret == -2)
					return (-1);	/* quit - failed */
			}
			if (get_recs(pcb, &nrecs) == 0)
				asort(pcb);		/* put record in list */
		}
	}
	/*
	 * For root: write outfile header if no records were encountered.
	 * For non-root: write trailer to pipe and close pipe.
	 */
	if (pcbr->pcb_flags & PF_ROOT) {
		if (nprecs == 0) {
			if (write_header())	/* write header if no records */
				return (-1);
		}
	} else {
		pcb = &(pcbr->pcb_below[0]);	/* any old PCB will do */
		pcb->pcb_rec = empty_file_token;
		if (write_recs(pcbr, pcb, &junk))
			return (-1);
		if (fclose(pcbr->pcb_fpw) == EOF) {
			if (!f_quiet)
				(void) fprintf(stderr,
				    gettext("%s couldn't close pipe.\n"), ar);
		}
	}
	/*
	 * For root process tell how many records were written.
	 */
	if (f_verbose && (pcbr->pcb_flags & PF_ROOT)) {
		(void) fprintf(stderr,
		    gettext("%s %d record(s) total were written out.\n"),
			ar, nprecs);
	}
	return (0);
}


/*
 * Head of linked-list of pcbs - sorted by time - oldest first.
 */
static audit_pcb_t		*pcbls = NULL;

/*
 * .func	asort - audit sort.
 * .desc	Place a pcb in the list sorted by time - oldest first.
 * .call	asort(pcb);
 * .arg	pcb	- ptr to pcb to install in list.
 * .ret	void.
 */
static void
asort(pcb)
register audit_pcb_t *pcb;
{
	register audit_pcb_t *pcbc, *pcbp;
	extern audit_pcb_t *pcbls;	/* ptr to start of list */

	pcb->pcb_next = NULL;
	if (pcbls == NULL) {
		pcbls = pcb;		/* empty list */
		return;
	}
	pcbc = pcbls;			/* current pcb */
	pcbp = pcbls;			/* previous pcb */
	while (pcbc != NULL) {
		if (pcb->pcb_time < pcbc->pcb_time) {
			if (pcbp == pcbc) {
				pcb->pcb_next = pcbls;	/* new -> 1st in list */
				pcbls = pcb;
				return;
			}
			pcbp->pcb_next = pcb;
			pcb->pcb_next = pcbc;		/* new in the inside */
			return;
		}
		pcbp = pcbc;
		pcbc = pcbc->pcb_next;
	}
	pcbp->pcb_next = pcb;				/* new -> last */
}


/*
 * .func	aget - audit get.
 * .desc	Get the first pcb from the list. Pcb is removed from list, too.
 * .call	pcb = aget().
 * .arg	none.
 * .ret	pcb	- ptr to pcb that was the first.
 */
static audit_pcb_t *
aget()
{
	audit_pcb_t *pcbret;
	extern audit_pcb_t *pcbls;	/* ptr to start of list */

	if (pcbls == NULL)
		return (pcbls);		/* empty list */
	pcbret = pcbls;
	pcbls = pcbls->pcb_next;	/* 2nd becomes 1st */
	return (pcbret);
}


/*
 * .func	get_file - get a new file.
 * .desc	Get the next file from the pcb's list. Check the header to see
 *	if the file really is an audit file. If there are no more then
 *	quit. If a file open (fopen) fails because the system file table
 *	is full or the process file table is full then quit processing
 *	altogether.
 * .call	ret = get_file(pcb).
 * .arg	pcb	- pcb holding the fcb's (files).
 * .ret	0	- new file opened for processing.
 * .ret	-1	- no more files - pcb finished.
 * .ret	-2	- fatal error - quit processing.
 */
static int
get_file(pcb)
register audit_pcb_t *pcb;
{
	FILE *fp;
	audit_fcb_t *fcb;

	/*
	 * Process file list until a good one if found or empty.
	 */
	while (pcb->pcb_fpr == NULL) {
		if ((fcb = pcb->pcb_first) == NULL) {
			pcb->pcb_time = -1;
			return (-1);	/* pcb is all done */
		} else {
		/*
		 * If we are reading from files then open the next one.
		 */
			if (!f_stdin) {
				if ((fp = fopen(fcb->fcb_file, "r")) == NULL) {
					if (!f_quiet) {
						(void) sprintf(errbuf, gettext(
						"%s couldn't open:\n  %s"),
						ar, fcb->fcb_file);
						perror(errbuf);
					}
					/*
					 * See if file space is depleted.
					 * If it is then we quit.
					 */
					if (errno == ENFILE || errno == EMFILE)
					{
						return (-2);
					}
					pcb->pcb_first = fcb->fcb_next;
					continue;	/* try another file */
				}
			} else {
				/*
				 * Read from standard input.
				 */
				fp = stdin;
			}
			/*
			 * Check header of audit file.
			 */
			if (check_header(fp, fcb->fcb_name)) {
				if (!f_quiet) {
					(void) fprintf(stderr,
					    "%s %s:\n  %s.\n",
					    ar, error_str, fcb->fcb_file);
				}
				if (fclose(fp) == EOF) {
					if (!f_quiet) {
						(void) fprintf(stderr, gettext(
						"%s couldn't close %s.\n"),
						ar, fcb->fcb_file);
					}
				}
				pcb->pcb_first = fcb->fcb_next;
				continue;		/* try another file */
			}
			/*
			 * Found a good audit file.
			 * Initalize pcb for processing.
			 */
			pcb->pcb_first = fcb->fcb_next;
			pcb->pcb_cur = fcb;
			pcb->pcb_fpr = fp;
			pcb->pcb_nrecs = 0;
			pcb->pcb_nprecs = 0;
			pcb->pcb_otime = -1;
		}
	}
	return (0);
}


/*
 * .func	write_recs - write records.
 * .desc	Write record from a buffer to output stream. Keep an eye out
 *	for the first and last records of the root's output stream.
 * .call	ret = write_recs(pcbr, pcb, nprecs).
 * .arg	pcbr	- ptr to node pcb.
 * .arg	pcb		- ptr to pcb holding the stream.
 * .arg	nprecs	- ptr to the number of put records. Updated here.
 * .ret	0	- no errors detected.
 * .ret	-1	- error in writing. Quit processing.
 */
static int
write_recs(pcbr, pcb, nprecs)
register audit_pcb_t *pcbr, *pcb;
int	*nprecs;
{
	adr_t adr;
	char	id;
	int32_t	size;

	adrm_start(&adr, pcb->pcb_rec);
	(void) adrm_char(&adr, &id, 1);
	(void) adrm_int32(&adr, &size, 1);

	/*
	 * Scan for first record to be written to outfile.
	 * When we find it then write the header and
	 * save the time for the outfile name.
	 */
	if ((*nprecs)++ == 0) {
		if (pcbr->pcb_flags & PF_ROOT) {
			f_start = pcb->pcb_time;	/* save start time */
			if (write_header())
				return (-1);
		}
	}
	f_end = pcb->pcb_time;			/* find last record's time */
	pcb->pcb_time = -1;			/* disable just written rec */

	if ((fwrite(pcb->pcb_rec, sizeof (char), size, pcbr->pcb_fpw)) !=
			size) {
		if (pcbr->pcb_flags & PF_ROOT) {
			(void) sprintf(errbuf, gettext(
				"%s write failed to %s"),
				ar, f_outfile ? f_outfile : gettext("stdout"));
			perror(errbuf);
		} else {
			perror(gettext("auditreduce: write failed to pipe"));
		}
		return (-1);
	}
	free(pcb->pcb_rec);
	return (0);
}

/*
 * .func get_recs - get records.
 * .desc Get records from a stream until one passing the current selection
 *	criteria is found or the stream is emptied.
 * .call	ret = get_recs(pcb, nr).
 * .arg	pcb	- ptr to pcb that holds this stream.
 * .arg	nr	- ptr to number of records read. Updated by this routine.
 * .ret	0	- got a record.
 * .ret	-1	- stream is finished.
 */
static int
get_recs(pcb, nr)
register audit_pcb_t *pcb;
int	*nr;
{
	adr_t adr;
	time_t secs;
	int	tmp;
	int	ret, ret2;
	int	nrecs = 0;	/* count how many records read this call */
	int	getrec = TRUE;
	int	alldone = FALSE;
	char	header_type;
	short	e;
	char	*str;
#if AUDIT_FILE
	static void	get_trace();
#endif

	while (getrec) {
		ret = get_record(pcb->pcb_fpr, &pcb->pcb_rec,
			pcb->pcb_cur->fcb_name);
		if (ret > 0) {
			adrm_start(&adr, pcb->pcb_rec);

			/* get token id */
			(void) adrm_char(&adr, (char *)&header_type, 1);
			/* skip over byte count */
			(void) adrm_int32(&adr, (int32_t *)&tmp, 1);
			/* skip over version # */
			(void) adrm_char(&adr, (char *)&tmp, 1);
			/* skip over event id */
			(void) adrm_short(&adr, (short *)&e, 1);
			/* skip over event id modifier */
			(void) adrm_short(&adr, (short *)&tmp, 1);

			if (header_type == AUT_HEADER32) {
			    int32_t s, m;

			    /* get seconds */
			    (void) adrm_int32(&adr, (int32_t *)&s, 1);
			    /* get microseconds */
			    (void) adrm_int32(&adr, (int32_t *)&m, 1);
			    secs = (time_t)s;
			} else if (header_type == AUT_HEADER32_EX) {
			    int32_t s, m;
			    int32_t t, junk[4];	/* at_type + at_addr[4] */

			    /* skip type and ip address field */
			    (void) adrm_int32(&adr, (int32_t *)&t, 1);
			    (void) adrm_int32(&adr, (int32_t *)&junk[0], t/4);

			    /* get seconds */
			    (void) adrm_int32(&adr, (int32_t *)&s, 1);
			    /* get microseconds */
			    (void) adrm_int32(&adr, (int32_t *)&m, 1);
			    secs = (time_t)s;
			} else if (header_type == AUT_HEADER64) {
			    int64_t s, m;

			    /* get seconds */
			    (void) adrm_int64(&adr, (int64_t *)&s, 1);
			    /* get microseconds */
			    (void) adrm_int64(&adr, (int64_t *)&m, 1);
#if ((!defined(_LP64)) || defined(_SYSCALL32))
			    if (s < (time_t)INT32_MIN ||
				s > (time_t)INT32_MAX)
					secs = 0;
			    else
					secs = (time_t)s;
#else
			    secs = (time_t)s;
#endif
			} else if (header_type == AUT_HEADER64_EX) {
			    int64_t s, m;
			    int32_t t, junk[4];

			    /* skip type and ip address field */
			    (void) adrm_int32(&adr, (int32_t *)&t, 1);
			    (void) adrm_int32(&adr, (int32_t *)&junk[0], t/4);

			    /* get seconds */
			    (void) adrm_int64(&adr, (int64_t *)&s, 1);
			    /* get microseconds */
			    (void) adrm_int64(&adr, (int64_t *)&m, 1);
#if ((!defined(_LP64)) || defined(_SYSCALL32))
			    if (s < (time_t)INT32_MIN ||
				s > (time_t)INT32_MAX)
					secs = 0;
			    else
					secs = (time_t)s;
#else
			    secs = (time_t)s;
#endif
			}
		}

#if AUDIT_REC
		(void) fprintf(stderr, "get_recs: %d ret %d recno %d\n",
			pcb->pcb_procno, ret, pcb->pcb_nrecs + 1);
#endif
		/*
		 * See if entire file is after the time window specified.
		 * Must be check here because the start time of the file name
		 * may be after the first record(s).
		 */
		if (pcb->pcb_nrecs == 0 && (pcb->pcb_flags & PF_USEFILE)) {
			/*
			 * If the first record read failed then use the time
			 * that was in the filename to judge.
			 */
			if (ret > 0)
				(pcb->pcb_cur)->fcb_start = secs;
			if (!f_all && (m_before <= (pcb->pcb_cur)->fcb_start)) {
				(void) fclose(pcb->pcb_fpr); /* ignore file */
				pcb->pcb_fpr = NULL;
				pcb->pcb_time = -1;
				return (-1);
			} else {
				/* Give belated announcement of file opening. */
				if (f_verbose) {
					(void) fprintf(stderr,
						gettext("%s opened:\n  %s.\n"),
						ar, (pcb->pcb_cur)->fcb_file);
				}
			}
		}
		/* Succesful acquisition of a record.  */
		if (ret > 0) {
			pcb->pcb_time = secs;	/* time of record */
			pcb->pcb_nrecs++;	/* # of read recs from stream */
			nrecs++;		/* # of recs read this call */
			/* Only check record if at bottom of process tree. */
			if (pcb->pcb_flags & PF_USEFILE) {
				check_order(pcb); /* check time sequence */
				if ((ret2 = check_rec(pcb)) == 0) {
					pcb->pcb_nprecs++;
					getrec = FALSE;
				} else if (ret2 == -2) {
					/* error */
					getrec = FALSE;	/* get no more recs */
					alldone = TRUE;	/* quit this file */
					free(pcb->pcb_rec);
				} else {
					/* -1: record not interesting */
					free(pcb->pcb_rec);
				}
			} else {
				pcb->pcb_nprecs++;
				getrec = FALSE;
			}
		} else {
			/* Error with record read or all done with stream. */
			getrec = FALSE;
			alldone = TRUE;
		}
	}
	if (alldone == TRUE) {
#if AUDIT_FILE
		get_trace(pcb);
#endif
		/* Error in record read. Display messages. */
		if (ret < 0 || ret2 == -2) {
			pcb->pcb_nrecs++;	/* # of read records */
			if (!f_quiet) {
				if (pcb->pcb_flags & PF_USEFILE) {
					/* Ignore if this is not_terminated. */
					if (!strstr((pcb->pcb_cur)->fcb_file,
							"not_terminated")) {
(void) fprintf(stderr, gettext("%s read error in %s at record %d.\n"), ar,
	(pcb->pcb_cur)->fcb_file, pcb->pcb_nrecs);
					}
				} else {
(void) fprintf(stderr, gettext("%s read error in pipe at record %d.\n"), ar,
	pcb->pcb_nrecs);
				}
			}
		} else {
			/*
			 * Only mark infile for deleting if we have succesfully
			 * processed all of it.
			 */
			if (pcb->pcb_flags & PF_USEFILE)
				(pcb->pcb_cur)->fcb_flags |= FF_DELETE;
		}
		if (fclose(pcb->pcb_fpr) == EOF) {
			if (!f_quiet) {
				if (pcb->pcb_flags & PF_USEFILE) {
					str = (pcb->pcb_cur)->fcb_file;
				} else {
					str = "pipe";
				}
				(void) fprintf(stderr,
					gettext("%s couldn't close %s.\n"),
					ar, str);
			}
		}
		pcb->pcb_fpr = NULL;
		pcb->pcb_time = -1;
		*nr += nrecs;
		return (-1);
	}
	*nr += nrecs;
	return (0);
}


#if AUDIT_FILE
/*
 * .func get_trace - get trace.
 * .desc If we are tracing file action (AUDIT_FILE is on) then print out
 *	a message when the file is closed regarding how many records
 *	were handled.
 * .call	get_trace(pcb).
 * .arg	pcb	- ptr to pcb holding file/pipe.
 * .ret	void.
 */
static void
get_trace(pcb)
audit_pcb_t *pcb;
{
	/*
	 * For file give filename, too.
	 */
	if (pcb->pcb_flags & PF_USEFILE) {
	(void) fprintf(stderr, "%s closed %s: %d records read recs: \
		%d record written.\n", ar, (pcb->pcb_cur)->fcb_file,
		pcb->pcb_nrecs, pcb->pcb_nprecs);
	} else {
		(void) fprintf(stderr, "%s closed pipe: %d records read: \
			%d records written .\n", ar, pcb->pcb_nrecs,
			pcb->pcb_nprecs);
	}
}

#endif

/*
 * .func	check_rec - check a record.
 * .desc	Check a record against the user's selection criteria.
 * .call	ret = check_rec(pcb).
 * .arg	pcb	- ptr to pcb holding the record.
 * .ret	0	- record accepted.
 * .ret	-1	- record rejected - continue processing file.
 * .ret	-2	- record rejected - quit processing file.
 */
static int
check_rec(pcb)
register audit_pcb_t *pcb;
{
	adr_t adr;
	struct timeval tv;
	uint_t	bytes;
	au_emod_t id_modifier;
	char	version;
	au_event_t event_type;
	char	tokenid;
	int	rc;	 /* return code */

	adrm_start(&adr, pcb->pcb_rec);
	(void) adrm_char(&adr, &tokenid, 1);

	/*
	 * checkflags will be my data structure for determining if
	 * a record has met ALL the selection criteria.  Once
	 * checkflags == flags, we have seen all we need to of the
	 * record, and can go to the next one.  If when we finish
	 * processing the record we still have stuff to see,
	 * checkflags != flags, and thus we should return a -1
	 * from this function meaning reject this record.
	 */

	checkflags = 0;

	/* must be header token -- sanity check */
	if (tokenid != AUT_HEADER32 && tokenid != AUT_HEADER64 &&
	    tokenid != AUT_HEADER32_EX && tokenid != AUT_HEADER64_EX) {
#if AUDIT_REC
		(void) fprintf(stderr,
		    "check_rec: %d recno %d no header %d found\n",
		    pcb->pcb_procno, pcb->pcb_nrecs, tokenid);
#endif
		return (-2);
	}

	/*
	 * The header token is:
	 *	attribute id:		char
	 *	byte count:		int
	 *	version #:		char
	 *	event ID:		short
	 *	ID modifier:		short
	 *	seconds (date):		int
	 *	time (microsecs):	int
	 */
	(void) adrm_u_int32(&adr, (uint32_t *)&bytes, 1);
	(void) adrm_char(&adr, &version, 1);
	(void) adrm_u_short(&adr, &event_type, 1);

	/*
	 * Used by s5_IPC_token to set the ipc_type so
	 * s5_IPC_perm_token can test.
	 */
	ipc_type = (char)0;

	if (flags & M_TYPE) {
		checkflags |= M_TYPE;
		if (m_type != event_type)
			return (-1);
	}
	if (flags & M_CLASS) {
		au_event_ent_t *ev = NULL;

		checkflags |= M_CLASS;
		if (cacheauevent(&ev, event_type) <= 0) {
		    (void) fprintf(stderr, gettext(
			"Warning: invalid event no %d in audit trail."),
			event_type);
		    return (-1);
		}
		global_class = ev->ae_class;
		if (!(flags & M_SORF) && !(mask.am_success & global_class))
			return (-1);
	}

	(void) adrm_u_short(&adr, &id_modifier, 1);

	/*
	 * Check record against time criteria.
	 * If the 'A' option was used then no time checking is done.
	 * The 'a' parameter is inclusive and the 'b' exclusive.
	 */
	if (tokenid == AUT_HEADER32) {
	    int32_t secs, msecs;
	    (void) adrm_int32(&adr, (int32_t *)&secs, 1);
	    (void) adrm_int32(&adr, (int32_t *)&msecs, 1);
	    tv.tv_sec = (time_t)secs;
	    tv.tv_usec = (suseconds_t)msecs;
	} else if (tokenid == AUT_HEADER32_EX) {
	    int32_t secs, msecs;
	    int32_t t, junk[5];	/* at_type + at_addr[4] */
	    /* skip type and ip address field */
	    (void) adrm_int32(&adr, (int32_t *)&t, 1);
	    (void) adrm_int32(&adr, (int32_t *)&junk[0], t/4);
	    /* get time */
	    (void) adrm_int32(&adr, (int32_t *)&secs, 1);
	    (void) adrm_int32(&adr, (int32_t *)&msecs, 1);
	    tv.tv_sec = (time_t)secs;
	    tv.tv_usec = (suseconds_t)msecs;
	} else if (tokenid == AUT_HEADER64) {
	    int64_t secs, msecs;
	    (void) adrm_int64(&adr, (int64_t *)&secs, 1);
	    (void) adrm_int64(&adr, (int64_t *)&msecs, 1);
#if ((!defined(_LP64)) || defined(_SYSCALL32))
	    if (secs < (time_t)INT32_MIN ||
		secs > (time_t)INT32_MAX)
			tv.tv_sec = 0;
	    else
			tv.tv_sec = (time_t)secs;
	    if (msecs < (suseconds_t)INT32_MIN ||
		msecs > (suseconds_t)INT32_MAX)
			tv.tv_usec = 0;
	    else
			tv.tv_usec = (suseconds_t)msecs;
#else
	    tv.tv_sec = (time_t)secs;
	    tv.tv_usec = (suseconds_t)msecs;
#endif
	} else if (tokenid == AUT_HEADER64_EX) {
	    int64_t secs, msecs;
	    int32_t t, junk[4];	/* at_type + at_addr[4] */
	    /* skip type and ip address field */
	    (void) adrm_int32(&adr, (int32_t *)&t, 1);
	    (void) adrm_int32(&adr, (int32_t *)&junk[0], t/4);
	    /* get time */
	    (void) adrm_int64(&adr, (int64_t *)&secs, 1);
	    (void) adrm_int64(&adr, (int64_t *)&msecs, 1);
#if ((!defined(_LP64)) || defined(_SYSCALL32))
	    if (secs < (time_t)INT32_MIN ||
		secs > (time_t)INT32_MAX)
			tv.tv_sec = 0;
	    else
			tv.tv_sec = (time_t)secs;
	    if (msecs < (suseconds_t)INT32_MIN ||
		msecs > (suseconds_t)INT32_MAX)
			tv.tv_usec = 0;
	    else
			tv.tv_usec = (suseconds_t)msecs;
#else
	    tv.tv_sec = (time_t)secs;
	    tv.tv_usec = (suseconds_t)msecs;
#endif
	}
	pcb->pcb_otime = pcb->pcb_time;
	if (!f_all) {
		if (m_after > tv.tv_sec)
			return (-1);
		if (m_before <= tv.tv_sec)
			return (-1);
	}

	/* if no selection flags were passed, select everything */
	if (!flags)
		return (0);

	/*
	 * If all information can be found in header,
	 * there is no need to continue processing the tokens.
	 */
	if (flags == checkflags)
		return (0);

	/*
	 * Process tokens until we hit the end of the record
	 */
	while ((uint_t)(adr.adr_now - adr.adr_stream) < bytes) {
		adrm_char(&adr, &tokenid, 1);
		rc = token_processing(&adr, tokenid);

		/* Any Problems? */
		if (rc == -2) {
			(void) fprintf(stderr,
			    gettext("auditreduce: bad token %u, terminating "
			    "file %s\n"), tokenid, (pcb->pcb_cur)->fcb_file);
			return (-2);
		}

		/* Are we finished? */
		if (flags == checkflags)
			return (0);
	}

	/*
	 * So, we haven't seen all that we need to see.  Reject record.
	 */

	return (-1);
}


/*
 * .func check_order - Check temporal sequence.
 * .call check_order(pcb).
 * .arg	 pcb - ptr to audit_pcb_t.
 * .desc	Check to see if the records are out of temporal sequence, ie,
 *	a record has a time stamp older than its predecessor.
 *	Also check to see if the current record is within the bounds of
 *	the file itself.
 *	This routine prints a diagnostic message, unless the QUIET
 *	option was selected.
 * .call	check_order(pcb).
 * .arg	pcb	- ptr to pcb holding the records.
 * .ret	void.
 */
static void
check_order(pcb)
register audit_pcb_t *pcb;
{
	char	cptr1[28], cptr2[28];	/* for error reporting */

	/*
	 * If the record-past is not the oldest then say so.
	 */
	if (pcb->pcb_otime > pcb->pcb_time) {
		if (!f_quiet) {
			(void) memcpy((void *)cptr1,
				(void *)ctime(&pcb->pcb_otime), 26);
			cptr1[24] = ' ';
			(void) memcpy((void *)cptr2,
				(void *)ctime(&pcb->pcb_time), 26);
			cptr2[24] = ' ';
			(void) fprintf(stderr,
	gettext("%s %s had records out of order: %s was followed by %s.\n"),
				ar, (pcb->pcb_cur)->fcb_file, cptr1, cptr2);
		}
	}
}


/*
 * .func	check_header.
 * .desc	Read in and check the header for an audit file.
 *	The header must read-in properly and have the magic #.
 * .call	err = check_header(fp).
 * .arg	fp	- file stream.
 * .ret	0	no problems.
 * .ret	-1	problems.
 */
static int
check_header(fp, fn)
FILE *fp;
char	*fn;
{
	char	id;
	char	*fname;
	short	pathlength;
	adr_t	adr;
	adrf_t	adrf;

	adrf_start(&adrf, &adr, fp);

	if (adrf_char(&adrf, &id, 1)) {
		(void) sprintf(errbuf, gettext("%s is empty"), fn);
		error_str = errbuf;
		return (-1);
	}
	if (!(id == AUT_OTHER_FILE32 || id == AUT_OTHER_FILE64)) {
		(void) sprintf(errbuf, gettext("%s not an audit file "), fn);
		error_str = errbuf;
		return (-1);
	}

	if (id == AUT_OTHER_FILE32) {
	    int32_t secs, msecs;
	    (void) adrf_int32(&adrf, (int32_t *)&secs, 1);
	    (void) adrf_int32(&adrf, (int32_t *)&msecs, 1);
	} else {
	    int64_t secs, msecs;
	    (void) adrf_int64(&adrf, (int64_t *)&secs, 1);
	    (void) adrf_int64(&adrf, (int64_t *)&msecs, 1);
#if ((!defined(_LP64)) || defined(_SYSCALL32))
	    if (secs < (time_t)INT32_MIN ||
		secs > (time_t)INT32_MAX) {
		    error_str = gettext("bad time stamp in file header");
		    return (-1);
	    }
	    if (msecs < (suseconds_t)INT32_MIN ||
		msecs > (suseconds_t)INT32_MAX) {
		    error_str = gettext("bad time stamp in file header");
		    return (-1);
	    }
#endif
	}

	if (adrf_short(&adrf, &pathlength, 1)) {
		error_str = gettext("incomplete file header");
		return (-1);
	}

	if (pathlength != 0) {
		fname = (char *)a_calloc(1, (size_t)pathlength);
		if ((fread(fname, sizeof (char), pathlength, fp)) !=
				pathlength) {
			(void) sprintf(errbuf,
				gettext("error in header/filename read in %s"),
				fn);
			error_str = errbuf;
			return (-1);
		}
		free(fname);
	}
	return (0);
}


/*
 * .func	get_record - get a single record.
 * .desc	Read a single record from stream fp. If the record to be read
 *	is larger than the buffer given to hold it (as determined by
 *	cur_size) then free that buffer and allocate a new and bigger
 *	one, making sure to store its size.
 * .call	ret = get_record(fp, buf, cur_size, flags).
 * .arg	fp	- stream to read from.
 * .arg	buf	- ptr to ptr to buffer to place record in.
 * .arg	cur_size- ptr to the size of the buffer that *buf points to.
 * .arg	flags	- flags from fcb (to get FF_NOTTERM).
 * .ret	+number	- number of chars in the record.
 * .ret	0	- trailer seen - file done.
 * .ret	-1	- read error (error_str know what type).
 */
static int
get_record(fp, buf, fn)
FILE *fp;
char	**buf;
char	*fn;
{
	adr_t	adr;
	adrf_t	adrf;
	int	leadin;
	char	id;
	int	lsize;
	short	ssize;

	/*
	 * Get the token type. It will be either a header or a file
	 * token.
	 */
	(void) adrf_start(&adrf, &adr, fp);
	if (adrf_char(&adrf, &id, 1)) {
		(void) sprintf(errbuf, gettext(
			"record expected but not found in %s"),
			fn);
		error_str = errbuf;
		return (-1);
	}
	switch (id) {
	case AUT_HEADER32:
	case AUT_HEADER32_EX:
	case AUT_HEADER64:
	case AUT_HEADER64_EX:
		/*
		 * The header token is:
		 *	attribute id:		char
		 *	byte count:		int
		 *	version #:		char
		 *	event ID:		short
		 *	ID modifier:		short
		 *	IP address type		int	(_EX only)
		 *	IP address		1/4*int (_EX only)
		 *	seconds (date):		long
		 *	time (microsecs):	long
		 */
		leadin = sizeof (int32_t) + sizeof (char);
		(void) adrf_int32(&adrf, &lsize, 1);
		*buf = (char *)a_calloc(1, (size_t)(lsize + leadin));
		adr_start(&adr, *buf);
		adr_char(&adr, &id, 1);
		adr_int32(&adr, (int32_t *)&lsize, 1);
		if (fread(*buf + leadin, sizeof (char), lsize - leadin, fp) !=
			lsize - leadin) {
			(void) sprintf(errbuf,
				gettext("header token read failure in %s"), fn);
			error_str = errbuf;
			return (-1);
		}
		return (lsize + leadin);
	case AUT_OTHER_FILE32: {
		int32_t secs, msecs;
		leadin =  2 * sizeof (int32_t) +
				sizeof (short) + sizeof (char);
		(void) adrf_int32(&adrf, (int32_t *)&secs, 1);
		(void) adrf_int32(&adrf, (int32_t *)&msecs, 1);
		(void) adrf_short(&adrf, &ssize, 1);
		*buf = (char *)a_calloc(1, (size_t)(ssize + leadin));
		adr_start(&adr, *buf);
		adr_char(&adr, &id, 1);
		adr_int32(&adr, (int32_t *)&secs, 1);
		adr_int32(&adr, (int32_t *)&msecs, 1);
		adr_short(&adr, &ssize, 1);
		if (fread(*buf + leadin, sizeof (char), ssize, fp) != ssize) {
			error_str = gettext("file token read failure");
			return (-1);
		}
		return (0);		/* done! */
	}
	case AUT_OTHER_FILE64: {
		int64_t secs, msecs;
		leadin =  2 * sizeof (int64_t) +
				sizeof (short) + sizeof (char);
		(void) adrf_int64(&adrf, (int64_t *)&secs, 1);
		(void) adrf_int64(&adrf, (int64_t *)&msecs, 1);
		(void) adrf_short(&adrf, &ssize, 1);
		*buf = (char *)a_calloc(1, (size_t)(ssize + leadin));
		adr_start(&adr, *buf);
		adr_char(&adr, &id, 1);
		adr_int64(&adr, (int64_t *)&secs, 1);
		adr_int64(&adr, (int64_t *)&msecs, 1);
		adr_short(&adr, &ssize, 1);
		if (fread(*buf + leadin, sizeof (char), ssize, fp) != ssize) {
			error_str = gettext("file token read failure");
			return (-1);
		}
		return (0);		/* done! */
	}
	default:
		break;
	}
	error_str = gettext("record begins without proper token");
	return (-1);
}



/* ============================================================================
 * SOURCE 6/9: minix4\exokernel\kernel_legacy\cmd\svc\startd\proc.c
 * Size: 1,483 bytes, Lines: 59
 * Hash: a0abff30508f...
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
 * Copyright 2004 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*
 * This is in a separate file because procfs.h cannot be included if
 * _FILE_OFFSET_BITES=64 is defined.
 */

#include <procfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

ctid_t
proc_get_ctid()
{
	id_t ctid;
	int fd;
	ssize_t bytes;
	psinfo_t psinfo;

	fd = open("/proc/self/psinfo", O_RDONLY);
	if (fd < 0)
		return (-1);

	bytes = read(fd, &psinfo, sizeof (psinfo));
	if (bytes == sizeof (psinfo))
		ctid = psinfo.pr_contract;
	else
		ctid = -1;

	(void) close(fd);
	return (ctid);
}



/* ============================================================================
 * SOURCE 7/9: minix\process\proc.c
 * Size: 27,570 bytes, Lines: 663
 * Hash: 1fa44054bc0b...
 * ============================================================================ */

/**
 * @file proc.c
 * @brief MINIX process and message handling implementation
 *
 * This file contains the core process and message handling functionality for
 * the MINIX kernel. Together with "mpx.s", it forms the lowest layer of the
 * kernel, providing essential services for inter-process communication and
 * process scheduling.
 *
 * @section Entry Points
 * External entry point:
 * - sys_call(): Handle system calls (kernel trapped with INT instruction)
 *
 * Internal entry points (interrupt/task level):
 * - lock_notify(): Notify a process of a system event
 * - lock_send(): Send a message to a process
 * - lock_enqueue(): Add a process to scheduling queues
 * - lock_dequeue(): Remove a process from scheduling queues
 *
 * @section Implementation Details
 * The code uses pointer-to-pointer techniques throughout for efficient list
 * manipulation, eliminating special cases for head/tail operations in linked
 * lists. This approach improves both performance and code clarity.
 *
 * Example of pointer-to-pointer usage:
 * ```c
 * node_t **xpp = &queue;        // Get pointer to head of queue
 * while (*xpp != NULL)          // Find last pointer in list
 *     xpp = &(*xpp)->next;      // Get pointer to next pointer
 * *xpp = new_node;              // Replace end (NULL pointer)
 * new_node->next = NULL;        // Mark new end of list
 * ```
 *
 * @section Locking
 * All public functions use locking mechanisms to prevent race conditions by
 * temporarily disabling interrupts during critical operations.
 *
 * @section Message Types
 * Supports the following message operations:
 * - SEND: Blocking send until message delivered
 * - RECEIVE: Blocking receive until acceptable message arrives
 * - SENDREC: Combined send and receive in single system call
 * - NOTIFY: Non-blocking notification delivery
 * - ECHO: Non-blocking message echo back to sender
 *
 * @section Scheduling
 * Implements priority-based scheduling with quantum time slicing and
 * penalty system to prevent infinite loops in high-priority processes.
 *
 * @author Original MINIX team
 * @version Updated through 2005 with major rewrites by Jorrit N. Herder
 */
/* This file contains essentially all of the process and message handling.
 * Together with "mpx.s" it forms the lowest layer of the MINIX kernel.
 * There is one entry point from the outside:
 *
 *   sys_call: 	      a system call, i.e., the kernel is trapped with an INT
 *
 * As well as several entry points used from the interrupt and task level:
 *
 *   lock_notify:     notify a process of a system event
 *   lock_send:	      send a message to a process
 *   lock_enqueue:    put a process on one of the scheduling queues
 *   lock_dequeue:    remove a process from the scheduling queues
 *
 * Changes:
 *   Aug 19, 2005     rewrote scheduling code  (Jorrit N. Herder)
 *   Jul 25, 2005     rewrote system call handling  (Jorrit N. Herder)
 *   May 26, 2005     rewrote message passing functions  (Jorrit N. Herder)
 *   May 24, 2005     new notification system call  (Jorrit N. Herder)
 *   Oct 28, 2004     nonblocking send and receive calls  (Jorrit N. Herder)
 *
 * The code here is critical to make everything work and is important for the
 * overall performance of the system. A large fraction of the code deals with
 * list manipulation. To make this both easy to understand and fast to execute
 * pointer pointers are used throughout the code. Pointer pointers prevent
 * exceptions for the head or tail of a linked list.
 *
 *  node_t *queue, *new_node;	// assume these as global variables
 *  node_t **xpp = &queue; 	// get pointer pointer to head of queue
 *  while (*xpp != NULL) 	// find last pointer of the linked list
 *      xpp = &(*xpp)->next;	// get pointer to next pointer
 *  *xpp = new_node;		// now replace the end (the NULL pointer)
 *  new_node->next = NULL;	// and mark the new end of the list
 *
 * For example, when adding a new node to the end of the list, one normally
 * makes an exception for an empty list and looks up the end of the list for
 * nonempty lists. As shown above, this is not required with pointer pointers.
 */

#include "proc.h"
#include "kernel.h"
#include <minix/callnr.h>
#include <minix/com.h>

/* Scheduling and message passing functions. The functions are available to
 * other parts of the kernel through lock_...(). The lock temporarily disables
 * interrupts to prevent race conditions.
 */
FORWARD _PROTOTYPE(int mini_send, (struct proc * caller_ptr, int dst,
                                   message *m_ptr, unsigned flags));
FORWARD _PROTOTYPE(int mini_receive, (struct proc * caller_ptr, int src,
                                      message *m_ptr, unsigned flags));
FORWARD _PROTOTYPE(int mini_notify, (struct proc * caller_ptr, int dst));

FORWARD _PROTOTYPE(void enqueue, (struct proc * rp));
FORWARD _PROTOTYPE(void dequeue, (struct proc * rp));
FORWARD _PROTOTYPE(void sched, (struct proc * rp, int *queue, int *front));
FORWARD _PROTOTYPE(void pick_proc, (void));

#define BuildMess(m_ptr, src, dst_ptr)                                         \
  (m_ptr)->m_source = (src);                                                   \
  (m_ptr)->m_type = NOTIFY_FROM(src);                                          \
  (m_ptr)->NOTIFY_TIMESTAMP = get_uptime();                                    \
  switch (src) {                                                               \
  case HARDWARE:                                                               \
    (m_ptr)->NOTIFY_ARG = priv(dst_ptr)->s_int_pending;                        \
    priv(dst_ptr)->s_int_pending = 0;                                          \
    break;                                                                     \
  case SYSTEM:                                                                 \
    (m_ptr)->NOTIFY_ARG = priv(dst_ptr)->s_sig_pending;                        \
    priv(dst_ptr)->s_sig_pending = 0;                                          \
    break;                                                                     \
  }

#if (CHIP == INTEL)
#define CopyMess(s, sp, sm, dp, dm)                                            \
  cp_mess(s, (sp)->p_memmap[D].mem_phys, (vir_bytes)sm,                        \
          (dp)->p_memmap[D].mem_phys, (vir_bytes)dm)
#endif /* (CHIP == INTEL) */

#if (CHIP == M68000)
/* M68000 does not have cp_mess() in assembly like INTEL. Declare prototype
 * for cp_mess() here and define the function below. Also define CopyMess.
 */
#endif /* (CHIP == M68000) */

/*===========================================================================*
 *				sys_call				     *
 *===========================================================================*/
PUBLIC int sys_call(call_nr, src_dst, m_ptr)
int call_nr;    /* system call number and flags */
int src_dst;    /* src to receive from or dst to send to */
message *m_ptr; /* pointer to message in the caller's space */
{
  /* System calls are done by trapping to the kernel with an INT instruction.
   * The trap is caught and sys_call() is called to send or receive a message
   * (or both). The caller is always given by 'proc_ptr'.
   */
  register struct proc *caller_ptr = proc_ptr; /* get pointer to caller */
  int function = call_nr & SYSCALL_FUNC;       /* get system call function */
  unsigned flags = call_nr & SYSCALL_FLAGS;    /* get flags */
  int mask_entry;                              /* bit to check in send mask */
  int result;                                  /* the system call's result */
  vir_clicks vlo, vhi; /* virtual clicks containing message to send */

  /* Check if the process has privileges for the requested call. Calls to the
   * kernel may only be SENDREC, because tasks always reply and may not block
   * if the caller doesn't do receive().
   */
  if (!(priv(caller_ptr)->s_trap_mask & (1 << function)) ||
      (iskerneln(src_dst) && function != SENDREC && function != RECEIVE)) {
    kprintf("sys_call: trap %d not allowed, caller %d, src_dst %d\n", function,
            proc_nr(caller_ptr), src_dst);
    return (ECALLDENIED); /* trap denied by mask or kernel */
  }

  /* Require a valid source and/ or destination process, unless echoing. */
  if (!(isokprocn(src_dst) || src_dst == ANY || function == ECHO)) {
    kprintf("sys_call: invalid src_dst, src_dst %d, caller %d\n", src_dst,
            proc_nr(caller_ptr));
    return (EBADSRCDST); /* invalid process number */
  }

  /* If the call involves a message buffer, i.e., for SEND, RECEIVE, SENDREC,
   * or ECHO, check the message pointer. This check allows a message to be
   * anywhere in data or stack or gap. It will have to be made more elaborate
   * for machines which don't have the gap mapped.
   */
  if (function & CHECK_PTR) {
    vlo = (vir_bytes)m_ptr >> CLICK_SHIFT;
    vhi = ((vir_bytes)m_ptr + MESS_SIZE - 1) >> CLICK_SHIFT;
    if (vlo < caller_ptr->p_memmap[D].mem_vir || vlo > vhi ||
        vhi >=
            caller_ptr->p_memmap[S].mem_vir + caller_ptr->p_memmap[S].mem_len) {
      kprintf("sys_call: invalid message pointer, trap %d, caller %d\n",
              function, proc_nr(caller_ptr));
      return (EFAULT); /* invalid message pointer */
    }
  }

  /* If the call is to send to a process, i.e., for SEND, SENDREC or NOTIFY,
   * verify that the caller is allowed to send to the given destination and
   * that the destination is still alive.
   */
  if (function & CHECK_DST) {
    if (!get_sys_bit(priv(caller_ptr)->s_ipc_to, nr_to_id(src_dst))) {
      kprintf("sys_call: ipc mask denied %d sending to %d\n",
              proc_nr(caller_ptr), src_dst);
      return (ECALLDENIED); /* call denied by ipc mask */
    }

    if (isemptyn(src_dst) && !shutdown_started) {
      kprintf("sys_call: dead dest; %d, %d, %d\n", function,
              proc_nr(caller_ptr), src_dst);
      return (EDEADDST); /* cannot send to the dead */
    }
  }

  /* Now check if the call is known and try to perform the request. The only
   * system calls that exist in MINIX are sending and receiving messages.
   *   - SENDREC: combines SEND and RECEIVE in a single system call
   *   - SEND:    sender blocks until its message has been delivered
   *   - RECEIVE: receiver blocks until an acceptable message has arrived
   *   - NOTIFY:  nonblocking call; deliver notification or mark pending
   *   - ECHO:    nonblocking call; directly echo back the message
   */
  switch (function) {
  case SENDREC:
    /* A flag is set so that notifications cannot interrupt SENDREC. */
    priv(caller_ptr)->s_flags |= SENDREC_BUSY;
    /* fall through */
  case SEND:
    result = mini_send(caller_ptr, src_dst, m_ptr, flags);
    if (function == SEND || result != OK) {
      break; /* done, or SEND failed */
    } /* fall through for SENDREC */
  case RECEIVE:
    if (function == RECEIVE)
      priv(caller_ptr)->s_flags &= ~SENDREC_BUSY;
    result = mini_receive(caller_ptr, src_dst, m_ptr, flags);
    break;
  case NOTIFY:
    result = mini_notify(caller_ptr, src_dst);
    break;
  case ECHO:
    CopyMess(caller_ptr->p_nr, caller_ptr, m_ptr, caller_ptr, m_ptr);
    result = OK;
    break;
  default:
    result = EBADCALL; /* illegal system call */
  }

  /* Now, return the result of the system call to the caller. */
  return (result);
}

/*===========================================================================*
 *				mini_send				     *
 *===========================================================================*/
PRIVATE int mini_send(caller_ptr, dst, m_ptr, flags)
register struct proc *caller_ptr; /* who is trying to send a message? */
int dst;                          /* to whom is message being sent? */
message *m_ptr;                   /* pointer to message buffer */
unsigned flags;                   /* system call flags */
{
  /* Send a message from 'caller_ptr' to 'dst'. If 'dst' is blocked waiting
   * for this message, copy the message to it and unblock 'dst'. If 'dst' is
   * not waiting at all, or is waiting for another source, queue 'caller_ptr'.
   */
  register struct proc *dst_ptr = proc_addr(dst);
  register struct proc **xpp;
  register struct proc *xp;

  /* Check for deadlock by 'caller_ptr' and 'dst' sending to each other. */
  xp = dst_ptr;
  while (xp->p_rts_flags & SENDING) { /* check while sending */
    xp = proc_addr(xp->p_sendto);     /* get xp's destination */
    if (xp == caller_ptr)
      return (ELOCKED); /* deadlock if cyclic */
  }

  /* Check if 'dst' is blocked waiting for this message. The destination's
   * SENDING flag may be set when its SENDREC call blocked while sending.
   */
  if ((dst_ptr->p_rts_flags & (RECEIVING | SENDING)) == RECEIVING &&
      (dst_ptr->p_getfrom == ANY || dst_ptr->p_getfrom == caller_ptr->p_nr)) {
    /* Destination is indeed waiting for this message. */
    CopyMess(caller_ptr->p_nr, caller_ptr, m_ptr, dst_ptr, dst_ptr->p_messbuf);
    if ((dst_ptr->p_rts_flags &= ~RECEIVING) == 0)
      enqueue(dst_ptr);
  } else if (!(flags & NON_BLOCKING)) {
    /* Destination is not waiting.  Block and queue caller. */
    caller_ptr->p_messbuf = m_ptr;
    if (caller_ptr->p_rts_flags == 0)
      dequeue(caller_ptr);
    caller_ptr->p_rts_flags |= SENDING;
    caller_ptr->p_sendto = dst;

    /* Process is now blocked.  Put in on the destination's queue. */
    xpp = &dst_ptr->p_caller_q; /* find end of list */
    while (*xpp != NIL_PROC)
      xpp = &(*xpp)->p_q_link;
    *xpp = caller_ptr;               /* add caller to end */
    caller_ptr->p_q_link = NIL_PROC; /* mark new end of list */
  } else {
    return (ENOTREADY);
  }
  return (OK);
}

/*===========================================================================*
 *				mini_receive				     *
 *===========================================================================*/
PRIVATE int mini_receive(caller_ptr, src, m_ptr, flags)
register struct proc *caller_ptr; /* process trying to get message */
int src;                          /* which message source is wanted */
message *m_ptr;                   /* pointer to message buffer */
unsigned flags;                   /* system call flags */
{
  /* A process or task wants to get a message.  If a message is already queued,
   * acquire it and deblock the sender.  If no message from the desired source
   * is available block the caller, unless the flags don't allow blocking.
   */
  register struct proc **xpp;
  register struct notification **ntf_q_pp;
  message m;
  int bit_nr;
  sys_map_t *map;
  bitchunk_t *chunk;
  int i, src_id, src_proc_nr;

  /* Check to see if a message from desired source is already available.
   * The caller's SENDING flag may be set if SENDREC couldn't send. If it is
   * set, the process should be blocked.
   */
  if (!(caller_ptr->p_rts_flags & SENDING)) {

    /* Check if there are pending notifications, except for SENDREC. */
    if (!(priv(caller_ptr)->s_flags & SENDREC_BUSY)) {

      map = &priv(caller_ptr)->s_notify_pending;
      for (chunk = &map->chunk[0]; chunk < &map->chunk[NR_SYS_CHUNKS];
           chunk++) {

        /* Find a pending notification from the requested source. */
        if (!*chunk)
          continue; /* no bits in chunk */
        for (i = 0; !(*chunk & (1 << i)); ++i) {
        } /* look up the bit */
        src_id = (chunk - &map->chunk[0]) * BITCHUNK_BITS + i;
        if (src_id >= NR_SYS_PROCS)
          break;                        /* out of range */
        src_proc_nr = id_to_nr(src_id); /* get source proc */
        if (src != ANY && src != src_proc_nr)
          continue;          /* source not ok */
        *chunk &= ~(1 << i); /* no longer pending */

        /* Found a suitable source, deliver the notification message. */
        BuildMess(&m, src_proc_nr, caller_ptr); /* assemble message */
        CopyMess(src_proc_nr, proc_addr(HARDWARE), &m, caller_ptr, m_ptr);
        return (OK); /* report success */
      }
    }

    /* Check caller queue. Use pointer pointers to keep code simple. */
    xpp = &caller_ptr->p_caller_q;
    while (*xpp != NIL_PROC) {
      if (src == ANY || src == proc_nr(*xpp)) {
        /* Found acceptable message. Copy it and update status. */
        CopyMess((*xpp)->p_nr, *xpp, (*xpp)->p_messbuf, caller_ptr, m_ptr);
        if (((*xpp)->p_rts_flags &= ~SENDING) == 0)
          enqueue(*xpp);
        *xpp = (*xpp)->p_q_link; /* remove from queue */
        return (OK);             /* report success */
      }
      xpp = &(*xpp)->p_q_link; /* proceed to next */
    }
  }

  /* No suitable message is available or the caller couldn't send in SENDREC.
   * Block the process trying to receive, unless the flags tell otherwise.
   */
  if (!(flags & NON_BLOCKING)) {
    caller_ptr->p_getfrom = src;
    caller_ptr->p_messbuf = m_ptr;
    if (caller_ptr->p_rts_flags == 0)
      dequeue(caller_ptr);
    caller_ptr->p_rts_flags |= RECEIVING;
    return (OK);
  } else {
    return (ENOTREADY);
  }
}

/*===========================================================================*
 *				mini_notify				     *
 *===========================================================================*/
PRIVATE int mini_notify(caller_ptr, dst)
register struct proc *caller_ptr; /* sender of the notification */
int dst;                          /* which process to notify */
{
  register struct proc *dst_ptr = proc_addr(dst);
  int src_id; /* source id for late delivery */
  message m;  /* the notification message */

  /* Check to see if target is blocked waiting for this message. A process
   * can be both sending and receiving during a SENDREC system call.
   */
  if ((dst_ptr->p_rts_flags & (RECEIVING | SENDING)) == RECEIVING &&
      !(priv(dst_ptr)->s_flags & SENDREC_BUSY) &&
      (dst_ptr->p_getfrom == ANY || dst_ptr->p_getfrom == caller_ptr->p_nr)) {

    /* Destination is indeed waiting for a message. Assemble a notification
     * message and deliver it. Copy from pseudo-source HARDWARE, since the
     * message is in the kernel's address space.
     */
    BuildMess(&m, proc_nr(caller_ptr), dst_ptr);
    CopyMess(proc_nr(caller_ptr), proc_addr(HARDWARE), &m, dst_ptr,
             dst_ptr->p_messbuf);
    dst_ptr->p_rts_flags &= ~RECEIVING; /* deblock destination */
    if (dst_ptr->p_rts_flags == 0)
      enqueue(dst_ptr);
    return (OK);
  }

  /* Destination is not ready to receive the notification. Add it to the
   * bit map with pending notifications. Note the indirectness: the system id
   * instead of the process number is used in the pending bit map.
   */
  src_id = priv(caller_ptr)->s_id;
  set_sys_bit(priv(dst_ptr)->s_notify_pending, src_id);
  return (OK);
}

/*===========================================================================*
 *				lock_notify				     *
 *===========================================================================*/
PUBLIC int lock_notify(src, dst)
int src; /* sender of the notification */
int dst; /* who is to be notified */
{
  /* Safe gateway to mini_notify() for tasks and interrupt handlers. The sender
   * is explicitely given to prevent confusion where the call comes from. MINIX
   * kernel is not reentrant, which means to interrupts are disabled after
   * the first kernel entry (hardware interrupt, trap, or exception). Locking
   * is done by temporarily disabling interrupts.
   */
  int result;

  /* Exception or interrupt occurred, thus already locked. */
  if (k_reenter >= 0) {
    result = mini_notify(proc_addr(src), dst);
  }

  /* Call from task level, locking is required. */
  else {
    lock(0, "notify");
    result = mini_notify(proc_addr(src), dst);
    unlock(0);
  }
  return (result);
}

/*===========================================================================*
 *				enqueue					     *
 *===========================================================================*/
PRIVATE void
    enqueue(rp) register struct proc *rp; /* this process is now runnable */
{
  /* Add 'rp' to one of the queues of runnable processes.  This function is
   * responsible for inserting a process into one of the scheduling queues.
   * The mechanism is implemented here.   The actual scheduling policy is
   * defined in sched() and pick_proc().
   */
  int q;     /* scheduling queue to use */
  int front; /* add to front or back */

#if DEBUG_SCHED_CHECK
  check_runqueues("enqueue");
  if (rp->p_ready)
    kprintf("enqueue() already ready process\n");
#endif

  /* Determine where to insert to process. */
  sched(rp, &q, &front);

  /* Now add the process to the queue. */
  if (rdy_head[q] == NIL_PROC) {    /* add to empty queue */
    rdy_head[q] = rdy_tail[q] = rp; /* create a new queue */
    rp->p_nextready = NIL_PROC;     /* mark new end */
  } else if (front) {               /* add to head of queue */
    rp->p_nextready = rdy_head[q];  /* chain head of queue */
    rdy_head[q] = rp;               /* set new queue head */
  } else {                          /* add to tail of queue */
    rdy_tail[q]->p_nextready = rp;  /* chain tail of queue */
    rdy_tail[q] = rp;               /* set new queue tail */
    rp->p_nextready = NIL_PROC;     /* mark new end */
  }

  /* Now select the next process to run. */
  pick_proc();

#if DEBUG_SCHED_CHECK
  rp->p_ready = 1;
  check_runqueues("enqueue");
#endif
}

/*===========================================================================*
 *				dequeue					     *
 *===========================================================================*/
PRIVATE void dequeue(
    rp) register struct proc *rp; /* this process is no longer runnable */
{
  /* A process must be removed from the scheduling queues, for example, because
   * it has blocked.  If the currently active process is removed, a new process
   * is picked to run by calling pick_proc().
   */
  register int q = rp->p_priority; /* queue to use */
  register struct proc **xpp;      /* iterate over queue */
  register struct proc *prev_xp;

  /* Side-effect for kernel: check if the task's stack still is ok? */
  if (iskernelp(rp)) {
    if (*priv(rp)->s_stack_guard != STACK_GUARD)
      panic("stack overrun by task", proc_nr(rp));
  }

#if DEBUG_SCHED_CHECK
  check_runqueues("dequeue");
  if (!rp->p_ready)
    kprintf("dequeue() already unready process\n");
#endif

  /* Now make sure that the process is not in its ready queue. Remove the
   * process if it is found. A process can be made unready even if it is not
   * running by being sent a signal that kills it.
   */
  prev_xp = NIL_PROC;
  for (xpp = &rdy_head[q]; *xpp != NIL_PROC; xpp = &(*xpp)->p_nextready) {

    if (*xpp == rp) {                       /* found process to remove */
      *xpp = (*xpp)->p_nextready;           /* replace with next chain */
      if (rp == rdy_tail[q])                /* queue tail removed */
        rdy_tail[q] = prev_xp;              /* set new tail */
      if (rp == proc_ptr || rp == next_ptr) /* active process removed */
        pick_proc();                        /* pick new process to run */
      break;
    }
    prev_xp = *xpp; /* save previous in chain */
  }

#if DEBUG_SCHED_CHECK
  rp->p_ready = 0;
  check_runqueues("dequeue");
#endif
}

/*===========================================================================*
 *				sched					     *
 *===========================================================================*/
PRIVATE void
    sched(rp, queue,
          front) register struct proc *rp; /* process to be scheduled */
int *queue;                                /* return: queue to use */
int *front;                                /* return: front or back */
{
  /* This function determines the scheduling policy.  It is called whenever a
   * process must be added to one of the scheduling queues to decide where to
   * insert it.  As a side-effect the process' priority may be updated.
   */
  static struct proc *prev_ptr = NIL_PROC; /* previous without time */
  int time_left = (rp->p_ticks_left > 0);  /* quantum fully consumed */
  int penalty = 0;                         /* change in priority */

  /* Check whether the process has time left. Otherwise give a new quantum
   * and possibly raise the priority.  Processes using multiple quantums
   * in a row get a lower priority to catch infinite loops in high priority
   * processes (system servers and drivers).
   */
  if (!time_left) {                        /* quantum consumed ? */
    rp->p_ticks_left = rp->p_quantum_size; /* give new quantum */
    if (prev_ptr == rp)
      penalty++; /* catch infinite loops */
    else
      penalty--;   /* give slow way back */
    prev_ptr = rp; /* store ptr for next */
  }

  /* Determine the new priority of this process. The bounds are determined
   * by IDLE's queue and the maximum priority of this process. Kernel task
   * and the idle process are never changed in priority.
   */
  if (penalty != 0 && !iskernelp(rp)) {
    rp->p_priority += penalty;               /* update with penalty */
    if (rp->p_priority < rp->p_max_priority) /* check upper bound */
      rp->p_priority = rp->p_max_priority;
    else if (rp->p_priority > IDLE_Q - 1) /* check lower bound */
      rp->p_priority = IDLE_Q - 1;
  }

  /* If there is time left, the process is added to the front of its queue,
   * so that it can immediately run. The queue to use simply is always the
   * process' current priority.
   */
  *queue = rp->p_priority;
  *front = time_left;
}

/*===========================================================================*
 *				pick_proc				     *
 *===========================================================================*/
PRIVATE void pick_proc() {
  /* Decide who to run now.  A new process is selected by setting 'next_ptr'.
   * When a billable process is selected, record it in 'bill_ptr', so that the
   * clock task can tell who to bill for system time.
   */
  register struct proc *rp; /* process to run */
  int q;                    /* iterate over queues */

  /* Check each of the scheduling queues for ready processes. The number of
   * queues is defined in proc.h, and priorities are set in the image table.
   * The lowest queue contains IDLE, which is always ready.
   */
  for (q = 0; q < NR_SCHED_QUEUES; q++) {
    if ((rp = rdy_head[q]) != NIL_PROC) {
      next_ptr = rp; /* run process 'rp' next */
      if (priv(rp)->s_flags & BILLABLE)
        bill_ptr = rp; /* bill for system time */
      return;
    }
  }
}

/*===========================================================================*
 *				lock_send				     *
 *===========================================================================*/
PUBLIC int lock_send(dst, m_ptr)
int dst;        /* to whom is message being sent? */
message *m_ptr; /* pointer to message buffer */
{
  /* Safe gateway to mini_send() for tasks. */
  int result;
  lock(2, "send");
  result = mini_send(proc_ptr, dst, m_ptr, NON_BLOCKING);
  unlock(2);
  return (result);
}

/*===========================================================================*
 *				lock_enqueue				     *
 *===========================================================================*/
PUBLIC void lock_enqueue(rp) struct proc *rp; /* this process is now runnable */
{
  /* Safe gateway to enqueue() for tasks. */
  lock(3, "enqueue");
  enqueue(rp);
  unlock(3);
}

/*===========================================================================*
 *				lock_dequeue				     *
 *===========================================================================*/
PUBLIC void
    lock_dequeue(rp) struct proc *rp; /* this process is no longer runnable */
{
  /* Safe gateway to dequeue() for tasks. */
  lock(4, "dequeue");
  dequeue(rp);
  unlock(4);
}



/* ============================================================================
 * SOURCE 8/9: archive\minix_legacy\proc.c
 * Size: 16,897 bytes, Lines: 794
 * Hash: bbc6ce934551...
 * ============================================================================ */

/* Copyright (c) 1979 Regents of the University of California */
#
/*
 * pi - Pascal interpreter code translator
 *
 * Charles Haley, Bill Joy UCB
 * Version 1.2 January 1979
 */

#include "0.h"
#include "tree.h"
#include "opcode.h"

/*
 * The following arrays are used to determine which classes may be
 * read and written to/from text files.
 * They are indexed by the return types from classify.
 */
#define rdops(x) rdxxxx[(x)-(TFIRST)]
#define wrops(x) wrxxxx[(x)-(TFIRST)]

int rdxxxx[] {
	0,		/* -7  file types */
	0,		/* -6  record types */
	0,		/* -5  array types */
	0,		/* -4  scalar types */
	0,		/* -3  pointer types */
	0,		/* -2  set types */
	0,		/* -1  string types */
	0,		/*  0  nil - i.e. no type */
	0,		/*  1  booleans */
	O_READC,	/*  2  character */
	O_READ4,	/*  3  integer */
	O_READ8		/*  4  real */
};

int wrxxxx[] {
	0,		/* -7  file types */
	0,		/* -6  record types */
	0,		/* -5  array types */
	0,		/* -4  scalar types */
	0,		/* -3  pointer types */
	0,		/* -2  set types */
	O_WRITG,	/* -1  string types */
	0,		/*  0  nil - i.e. no type */
	O_WRITB,	/*  1  booleans */
	O_WRITC,	/*  2  character */
	O_WRIT4,	/*  3  integer */
	O_WRIT8,	/*  4  real */
};

/*
 * Proc handles procedure calls.
 * Non-builtin procedures are "buck-passed" to func (with a flag
 * indicating that they are actually procedures.
 * builtin procedures are handled here.
 */
proc(r)
	int *r;
{
	register struct nl *p;
	register int *al, op;
	struct nl *filetype, *ap;
	int argc, *argv, c, two, oct, hex, *file;
	int pu;
	int *pua, *pui, *puz;
	int i, j, k;

	/*
	 * Verify that the name is
	 * defined and is that of a
	 * procedure.
	 */
	p = lookup(r[2]);
	if (p == NIL) {
		rvlist(r[3]);
		return;
	}
	if (p->class != PROC) {
		error("Can't call %s, its %s not a procedure", p->symbol, classes[p->class]);
		rvlist(r[3]);
		return;
	}
	argv = r[3];

	/*
	 * Call handles user defined
	 * procedures and functions.
	 */
	if (bn != 0) {
		call(p, argv, PROC, bn);
		return;
	}

	/*
	 * Call to built-in procedure.
	 * Count the arguments.
	 */
	argc = 0;
	for (al = argv; al != NIL; al = al[2])
		argc++;

	/*
	 * Switch on the operator
	 * associated with the built-in
	 * procedure in the namelist
	 */
	op = p->value[0] &~ NSTAND;
	if (opt('s') && (p->value[0] & NSTAND)) {
		standard();
		error("%s is a nonstandard procedure", p->symbol);
	}
	switch (op) {

	case O_NULL:
		if (argc != 0)
			error("null takes no arguments");
		return;

	case O_FLUSH:
		if (argc == 0) {
			put1(O_MESSAGE);
			return;
		}
		if (argc != 1) {
			error("flush takes at most one argument");
			return;
		}
		ap = rvalue(argv[1], NIL);
		if (ap == NIL)
			return;
		if (ap->class != FILE) {
			error("flush's argument must be a file, not %s", nameof(ap));
			return;
		}
		put1(op);
		return;

	case O_MESSAGE:
	case O_WRIT2:
	case O_WRITLN:
		/*
		 * Set up default file "output"'s type
		 */
		file = NIL;
		filetype = nl+T1CHAR;
		/*
		 * Determine the file implied
		 * for the write and generate
		 * code to make it the active file.
		 */
		if (op == O_MESSAGE) {
			/*
			 * For message, all that matters
			 * is that the filetype is
			 * a character file.
			 * Thus "output" will suit us fine.
			 */
			put1(O_MESSAGE);
		} else if (argv != NIL && (al = argv[1])[0] != T_WEXP) {
			/*
			 * If there is a first argument which has
			 * no write widths, then it is potentially
			 * a file name.
			 */
			codeoff();
			ap = rvalue(argv[1], NIL);
			codeon();
			if (ap == NIL)
				argv = argv[2];
			if (ap != NIL && ap->class == FILE) {
				/*
				 * Got "write(f, ...", make
				 * f the active file, and save
				 * it and its type for use in
				 * processing the rest of the
				 * arguments to write.
				 */
				file = argv[1];
				filetype = ap->type;
				rvalue(argv[1], NIL);
				put1(O_UNIT);
				/*
				 * Skip over the first argument
				 */
				argv = argv[2];
				argc--;
			} else
				/*
				 * Set up for writing on 
				 * standard output.
				 */
				put1(O_UNITOUT);
		} else
			put1(O_UNITOUT);
		/*
		 * Loop and process each
		 * of the arguments.
		 */
		for (; argv != NIL; argv = argv[2]) {
			al = argv[1];
			if (al == NIL)
				continue;
			/*
			 * Op will be used to
			 * accumulate width information,
			 * and two records the fact
			 * that we saw two write widths
			 */
			op = 0;
			two = 0;
			oct = 0;
			hex = 0;
			if (al[0] == T_WEXP) {
				if (filetype != nl+T1CHAR) {
					error("Write widths allowed only with text files");
					continue;
				}
				/*
				 * Handle width expressions.
				 * The basic game here is that width
				 * expressions get evaluated and left
				 * on the stack and their width's get
				 * packed into the high byte of the
				 * affected opcode (subop).
				 */
				if (al[3] == OCT) 
					oct++;
				else if (al[3] == HEX)
					hex++;
				else if (al[3] != NIL) {
					two++;
					/*
					 * Arrange for the write
					 * opcode that takes two widths
					 */
					op =| O_WRIT82-O_WRIT8;
					ap = rvalue(al[3], NIL);
					if (ap == NIL)
						continue;
					if (isnta(ap, "i")) {
						error("Second write width must be integer, not %s", nameof(ap));
						continue;
					}
					op =| even(width(ap)) << 11;
				}
				if (al[2] != NIL) {
					ap = rvalue(al[2], NIL);
					if (ap == NIL)
						continue;
					if (isnta(ap, "i")) {
						error("First write width must be integer, not %s", nameof(ap));
						continue;
					}
					op =| even(width(ap)) << 8;
				}
				al = al[1];
				if (al == NIL)
					continue;
			}
			if (filetype != nl+T1CHAR) {
				if (oct || hex) {
					error("Oct/hex allowed only on text files");
					continue;
				}
				if (op) {
					error("Write widths allowed only on text files");
					continue;
				}
				/*
				 * Generalized write, i.e.
				 * to a non-textfile.
				 */
				rvalue(file, NIL);
				put1(O_FNIL);
				/*
				 * file^ := ...
				 */
				ap = rvalue(argv[1], NIL);
				if (ap == NIL)
					continue;
				if (incompat(ap, filetype, argv[1])) {
					cerror("Type mismatch in write to non-text file");
					continue;
				}
				convert(ap, filetype);
				put2(O_AS, width(filetype));
				/*
				 * put(file)
				 */
				put1(O_PUT);
				continue;
			}
			/*
			 * Write to a textfile
			 *
			 * Evaluate the expression
			 * to be written.
			 */
			ap = rvalue(al, NIL);
			if (ap == NIL)
				continue;
			c = classify(ap);
			if (two && c != TDOUBLE) {
				if (isnta(ap, "i")) {
					error("Only reals can have two write widths");
					continue;
				}
				convert(ap, nl+TDOUBLE);
				c = TDOUBLE;
			}
			if (oct || hex) {
				if (opt('s')) {
					standard();
					error("Oct and hex are non-standard");
				}
				switch (c) {
					case TREC:
					case TARY:
					case TFILE:
					case TSTR:
					case TSET:
					case TDOUBLE:
						error("Can't write %ss with oct/hex", clnames[c]);
						continue;
				}
				put1(op | (oct ? O_WROCT2 : O_WRHEX2) | (width(ap) >> 2));
				continue;
			}
			if (wrops(c) == NIL) {
				error("Can't write %ss to a text file", clnames[c]);
				continue;
			}
			if (c == TINT && width(ap) != 4)
				op =| O_WRIT2;
			else
				op =| wrops(c);
			if (c == TSTR)
				put2(op, width(ap));
			else
				put1(op);
		}
		/*
		 * Done with arguments.
		 * Handle writeln and
		 * insufficent number of args.
		 */
		switch (p->value[0] &~ NSTAND) {
			case O_WRIT2:
				if (argc == 0)
					error("Write requires an argument");
				break;
			case O_MESSAGE:
				if (argc == 0)
					error("Message requires an argument");
			case O_WRITLN:
				if (filetype != nl+T1CHAR)
					error("Can't 'writeln' a non text file");
				put1(O_WRITLN);
				break;
		}
		return;

	case O_READ4:
	case O_READLN:
		/*
		 * Set up default
		 * file "input".
		 */
		file = NIL;
		filetype = nl+T1CHAR;
		/*
		 * Determine the file implied
		 * for the read and generate
		 * code to make it the active file.
		 */
		if (argv != NIL) {
			codeoff();
			ap = rvalue(argv[1], NIL);
			codeon();
			if (ap == NIL)
				argv = argv[2];
			if (ap != NIL && ap->class == FILE) {
				/*
				 * Got "read(f, ...", make
				 * f the active file, and save
				 * it and its type for use in
				 * processing the rest of the
				 * arguments to read.
				 */
				file = argv[1];
				filetype = ap->type;
				rvalue(argv[1], NIL);
				put1(O_UNIT);
				argv = argv[2];
				argc--;
			} else {
				/*
				 * Default is read from
				 * standard input.
				 */
				put1(O_UNITINP);
				input->nl_flags =| NUSED;
			}
		} else {
			put1(O_UNITINP);
			input->nl_flags =| NUSED;
		}
		/*
		 * Loop and process each
		 * of the arguments.
		 */
		for (; argv != NIL; argv = argv[2]) {
			/*
			 * Get the address of the target
			 * on the stack.
			 */
			al = argv[1];
			if (al == NIL)
				continue;
			if (al[0] != T_VAR) {
				error("Arguments to %s must be variables, not expressions", p->symbol);
				continue;
			}
			ap = lvalue(al, MOD|ASGN|NOUSE);
			if (ap == NIL)
				continue;
			if (filetype != nl+T1CHAR) {
				/*
				 * Generalized read, i.e.
				 * from a non-textfile.
				 */
				if (incompat(filetype, ap, NIL)) {
					error("Type mismatch in read from non-text file");
					continue;
				}
				/*
				 * var := file ^;
				 */
				if (file != NIL)
					rvalue(file, NIL);
				else /* Magic */
					put2(O_RV2, input->value[0]);
				put1(O_FNIL);
				put2(O_IND, width(filetype));
				convert(filetype, ap);
				if (isa(ap, "bsci"))
					rangechk(ap, ap);
				put2(O_AS, width(ap));
				/*
				 * get(file);
				 */
				put1(O_GET);
				continue;
			}
			c = classify(ap);
			op = rdops(c);
			if (op == NIL) {
				error("Can't read %ss from a text file", clnames[c]);
				continue;
			}
			put1(op);
			/*
			 * Data read is on the stack.
			 * Assign it.
			 */
			if (op != O_READ8)
				rangechk(ap, op == O_READC ? ap : nl+T4INT);
			gen(O_AS2, O_AS2, width(ap),
				op == O_READ8 ? 8 : op == O_READ4 ? 4 : 2);
		}
		/*
		 * Done with arguments.
		 * Handle readln and
		 * insufficient number of args.
		 */
		if (p->value[0] == O_READLN) {
			if (filetype != nl+T1CHAR)
				error("Can't 'readln' a non text file");
			put1(O_READLN);
		}
		else if (argc == 0)
			error("read requires an argument");
		return;

	case O_GET:
	case O_PUT:
		if (argc != 1) {
			error("%s expects one argument", p->symbol);
			return;
		}
		ap = rvalue(argv[1], NIL);
		if (ap == NIL)
			return;
		if (ap->class != FILE) {
			error("Argument to %s must be a file, not %s", p->symbol, nameof(ap));
			return;
		}
		put1(O_UNIT);
		put1(op);
		return;

	case O_RESET:
	case O_REWRITE:
		if (argc == 0 || argc > 2) {
			error("%s expects one or two arguments", p->symbol);
			return;
		}
		if (opt('s') && argc == 2) {
			standard();
			error("Two argument forms of reset and rewrite are non-standard");
		}
		ap = lvalue(argv[1], MOD|NOUSE);
		if (ap == NIL)
			return;
		if (ap->class != FILE) {
			error("First argument to %s must be a file, not %s", p->symbol, nameof(ap));
			return;
		}
		if (argc == 2) {
			/*
			 * Optional second argument
			 * is a string name of a
			 * UNIX (R) file to be associated.
			 */
			al = argv[2];
			al = rvalue(al[1], NIL);
			if (al == NIL)
				return;
			if (classify(al) != TSTR) {
				error("Second argument to %s must be a string, not %s", p->symbol, nameof(al));
				return;
			}
			c = width(al);
		} else
			c = 0;
		if (c > 127) {
			error("File name too long");
			return;
		}
		put2(op | c << 8, text(ap) ? 0: width(ap->type));
		return;

	case O_NEW:
	case O_DISPOSE:
		if (argc == 0) {
			error("%s expects at least one argument", p->symbol);
			return;
		}
		ap = lvalue(argv[1], MOD|NOUSE);
		if (ap == NIL)
			return;
		if (ap->class != PTR) {
			error("(First) argument to %s must be a pointer, not %s", p->symbol, nameof(ap));
			return;
		}
		ap = ap->type;
		if (ap == NIL)
			return;
		argv = argv[2];
		if (argv != NIL) {
			if (ap->class != RECORD) {
				error("Record required when specifying variant tags");
				return;
			}
			for (; argv != NIL; argv = argv[2]) {
				if (ap->value[NL_VARNT] == NIL) {
					error("Too many tag fields");
					return;
				}
				if (!isconst(argv[1])) {
					error("Second and successive arguments to %s must be constants", p->symbol);
					return;
				}
				gconst(argv[1]);
				if (con.ctype == NIL)
					return;
				if (incompat(con.ctype, ap->value[NL_TAG]->type)) {
					cerror("Specified tag constant type clashed with variant case selector type");
					return;
				}
				for (ap = ap->value[NL_VARNT]; ap != NIL; ap = ap->chain)
					if (ap->range[0] == con.crval)
						break;
				if (ap == NIL) {
					error("No variant case label value equals specified constant value");
					return;
				}
				ap = ap->value[NL_VTOREC];
			}
		}
		put2(op, width(ap));
		return;

	case O_DATE:
	case O_TIME:
		if (argc != 1) {
			error("%s expects one argument", p->symbol);
			return;
		}
		ap = lvalue(argv[1], MOD|NOUSE);
		if (ap == NIL)
			return;
		if (classify(ap) != TSTR || width(ap) != 10) {
			error("Argument to %s must be a alfa, not %s", p->symbol, nameof(ap));
			return;
		}
		put1(op);
		return;

	case O_HALT:
		if (argc != 0) {
			error("halt takes no arguments");
			return;
		}
		put1(op);
		noreach = 1;
		return;

	case O_ARGV:
		if (argc != 2) {
			error("argv takes two arguments");
			return;
		}
		ap = rvalue(argv[1], NIL);
		if (ap == NIL)
			return;
		if (isnta(ap, "i")) {
			error("argv's first argument must be an integer, not %s", nameof(ap));
			return;
		}
		convert(ap, nl+T2INT);
		al = argv[2];
		ap = lvalue(al[1], MOD|NOUSE);
		if (ap == NIL)
			return;
		if (classify(ap) != TSTR) {
			error("argv's second argument must be a string, not %s", nameof(ap));
			return;
		}
		put2(op, width(ap));
		return;

	case O_STLIM:
		if (argc != 1) {
			error("stlimit requires one argument");
			return;
		}
		ap = rvalue(argv[1], NIL);
		if (ap == NIL)
			return;
		if (isnta(ap, "i")) {
			error("stlimit's argument must be an integer, not %s", nameof(ap));
			return;
		}
		if (width(ap) != 4)
			put1(O_STOI);
		put1(op);
		return;

	case O_REMOVE:
		if (argc != 1) {
			error("remove expects one argument");
			return;
		}
		ap = rvalue(argv[1], NIL);
		if (ap == NIL)
			return;
		if (classify(ap) != TSTR) {
			error("remove's argument must be a string, not %s", nameof(ap));
			return;
		}
		put2(op, width(ap));
		return;

	case O_LLIMIT:
		if (argc != 2) {
			error("linelimit expects two arguments");
			return;
		}
		ap = lvalue(argv[1], NOMOD|NOUSE);
		if (ap == NIL)
			return;
		if (!text(ap)) {
			error("linelimit's first argument must be a text file, not %s", nameof(ap));
			return;
		}
		al = argv[2];
		ap = rvalue(al[1], NIL);
		if (ap == NIL)
			return;
		if (isnta(ap, "i")) {
			error("linelimit's second argument must be an integer, not %s", nameof(ap));
			return;
		}
		convert(ap, nl+T2INT);
		put1(op);
		return;
	case O_PAGE:
		if (argc != 1) {
			error("page expects one argument");
			return;
		}
		ap = rvalue(argv[1], NIL);
		if (ap == NIL)
			return;
		if (!text(ap)) {
			error("Argument to page must be a text file, not %s", nameof(ap));
			return;
		}
		put1(O_UNIT);
		put1(op);
		return;

	case O_PACK:
		if (argc != 3) {
			error("pack expects three arguments");
			return;
		}
		pu = "pack(a,i,z)";
		pua = (al = argv)[1];
		pui = (al = al[2])[1];
		puz = (al = al[2])[1];
		goto packunp;
	case O_UNPACK:
		if (argc != 3) {
			error("unpack expects three arguments");
			return;
		}
		pu = "unpack(z,a,i)";
		puz = (al = argv)[1];
		pua = (al = al[2])[1];
		pui = (al = al[2])[1];
packunp:
		ap = rvalue(pui, NIL);
		if (ap == NIL)
			return;
		if (width(ap) == 4)
			put1(O_ITOS);
		ap = lvalue(pua, op == O_PACK ? NOMOD : MOD|NOUSE);
		if (ap == NIL)
			return;
		if (ap->class != ARRAY) {
			error("%s requires a to be an unpacked array, not %s", pu, nameof(ap));
			return;
		}
		al = lvalue(puz, op == O_UNPACK ? NOMOD : MOD|NOUSE);
		if (al->class != ARRAY) {
			error("%s requires z to be a packed array, not %s", pu, nameof(ap));
			return;
		}
		if (al->type == NIL || ap->type == NIL)
			return;
		if (al->type != ap->type) {
			error("%s requires a and z to be arrays of the same type", pu, nameof(ap));
			return;
		}
		k = width(al);
		ap = ap->chain;
		al = al->chain;
		if (ap->chain != NIL || al->chain != NIL) {
			error("%s requires a and z to be single dimension arrays", pu);
			return;
		}
		if (ap == NIL || al == NIL)
			return;
		/*
		 * al is the range for z i.e. u..v
		 * ap is the range for a i.e. m..n
		 * i will be n-m+1
		 * j will be v-u+1
		 */
		i = ap->range[1] - ap->range[0] + 1;
		j = al->range[1] - al->range[0] + 1;
		if (i < j) {
			error("%s cannot have more elements in a (%d) than in z (%d)", pu, j, i);
			return;
		}
		/*
		 * get n-m-(v-u) and m for the interpreter
		 */
		i =- j;
		j = ap->range[0];
		put(5, op, width(ap), j, i, k);
		return;
	case 0:
		error("%s is an unimplemented 6400 extension", p->symbol);
		return;

	default:
		panic("proc case");
	}
}



/* ============================================================================
 * SOURCE 9/9: archive\legacy\pascal\proc.c
 * Size: 16,897 bytes, Lines: 794
 * Hash: bbc6ce934551...
 * ============================================================================ */

/* Copyright (c) 1979 Regents of the University of California */
#
/*
 * pi - Pascal interpreter code translator
 *
 * Charles Haley, Bill Joy UCB
 * Version 1.2 January 1979
 */

#include "0.h"
#include "tree.h"
#include "opcode.h"

/*
 * The following arrays are used to determine which classes may be
 * read and written to/from text files.
 * They are indexed by the return types from classify.
 */
#define rdops(x) rdxxxx[(x)-(TFIRST)]
#define wrops(x) wrxxxx[(x)-(TFIRST)]

int rdxxxx[] {
	0,		/* -7  file types */
	0,		/* -6  record types */
	0,		/* -5  array types */
	0,		/* -4  scalar types */
	0,		/* -3  pointer types */
	0,		/* -2  set types */
	0,		/* -1  string types */
	0,		/*  0  nil - i.e. no type */
	0,		/*  1  booleans */
	O_READC,	/*  2  character */
	O_READ4,	/*  3  integer */
	O_READ8		/*  4  real */
};

int wrxxxx[] {
	0,		/* -7  file types */
	0,		/* -6  record types */
	0,		/* -5  array types */
	0,		/* -4  scalar types */
	0,		/* -3  pointer types */
	0,		/* -2  set types */
	O_WRITG,	/* -1  string types */
	0,		/*  0  nil - i.e. no type */
	O_WRITB,	/*  1  booleans */
	O_WRITC,	/*  2  character */
	O_WRIT4,	/*  3  integer */
	O_WRIT8,	/*  4  real */
};

/*
 * Proc handles procedure calls.
 * Non-builtin procedures are "buck-passed" to func (with a flag
 * indicating that they are actually procedures.
 * builtin procedures are handled here.
 */
proc(r)
	int *r;
{
	register struct nl *p;
	register int *al, op;
	struct nl *filetype, *ap;
	int argc, *argv, c, two, oct, hex, *file;
	int pu;
	int *pua, *pui, *puz;
	int i, j, k;

	/*
	 * Verify that the name is
	 * defined and is that of a
	 * procedure.
	 */
	p = lookup(r[2]);
	if (p == NIL) {
		rvlist(r[3]);
		return;
	}
	if (p->class != PROC) {
		error("Can't call %s, its %s not a procedure", p->symbol, classes[p->class]);
		rvlist(r[3]);
		return;
	}
	argv = r[3];

	/*
	 * Call handles user defined
	 * procedures and functions.
	 */
	if (bn != 0) {
		call(p, argv, PROC, bn);
		return;
	}

	/*
	 * Call to built-in procedure.
	 * Count the arguments.
	 */
	argc = 0;
	for (al = argv; al != NIL; al = al[2])
		argc++;

	/*
	 * Switch on the operator
	 * associated with the built-in
	 * procedure in the namelist
	 */
	op = p->value[0] &~ NSTAND;
	if (opt('s') && (p->value[0] & NSTAND)) {
		standard();
		error("%s is a nonstandard procedure", p->symbol);
	}
	switch (op) {

	case O_NULL:
		if (argc != 0)
			error("null takes no arguments");
		return;

	case O_FLUSH:
		if (argc == 0) {
			put1(O_MESSAGE);
			return;
		}
		if (argc != 1) {
			error("flush takes at most one argument");
			return;
		}
		ap = rvalue(argv[1], NIL);
		if (ap == NIL)
			return;
		if (ap->class != FILE) {
			error("flush's argument must be a file, not %s", nameof(ap));
			return;
		}
		put1(op);
		return;

	case O_MESSAGE:
	case O_WRIT2:
	case O_WRITLN:
		/*
		 * Set up default file "output"'s type
		 */
		file = NIL;
		filetype = nl+T1CHAR;
		/*
		 * Determine the file implied
		 * for the write and generate
		 * code to make it the active file.
		 */
		if (op == O_MESSAGE) {
			/*
			 * For message, all that matters
			 * is that the filetype is
			 * a character file.
			 * Thus "output" will suit us fine.
			 */
			put1(O_MESSAGE);
		} else if (argv != NIL && (al = argv[1])[0] != T_WEXP) {
			/*
			 * If there is a first argument which has
			 * no write widths, then it is potentially
			 * a file name.
			 */
			codeoff();
			ap = rvalue(argv[1], NIL);
			codeon();
			if (ap == NIL)
				argv = argv[2];
			if (ap != NIL && ap->class == FILE) {
				/*
				 * Got "write(f, ...", make
				 * f the active file, and save
				 * it and its type for use in
				 * processing the rest of the
				 * arguments to write.
				 */
				file = argv[1];
				filetype = ap->type;
				rvalue(argv[1], NIL);
				put1(O_UNIT);
				/*
				 * Skip over the first argument
				 */
				argv = argv[2];
				argc--;
			} else
				/*
				 * Set up for writing on 
				 * standard output.
				 */
				put1(O_UNITOUT);
		} else
			put1(O_UNITOUT);
		/*
		 * Loop and process each
		 * of the arguments.
		 */
		for (; argv != NIL; argv = argv[2]) {
			al = argv[1];
			if (al == NIL)
				continue;
			/*
			 * Op will be used to
			 * accumulate width information,
			 * and two records the fact
			 * that we saw two write widths
			 */
			op = 0;
			two = 0;
			oct = 0;
			hex = 0;
			if (al[0] == T_WEXP) {
				if (filetype != nl+T1CHAR) {
					error("Write widths allowed only with text files");
					continue;
				}
				/*
				 * Handle width expressions.
				 * The basic game here is that width
				 * expressions get evaluated and left
				 * on the stack and their width's get
				 * packed into the high byte of the
				 * affected opcode (subop).
				 */
				if (al[3] == OCT) 
					oct++;
				else if (al[3] == HEX)
					hex++;
				else if (al[3] != NIL) {
					two++;
					/*
					 * Arrange for the write
					 * opcode that takes two widths
					 */
					op =| O_WRIT82-O_WRIT8;
					ap = rvalue(al[3], NIL);
					if (ap == NIL)
						continue;
					if (isnta(ap, "i")) {
						error("Second write width must be integer, not %s", nameof(ap));
						continue;
					}
					op =| even(width(ap)) << 11;
				}
				if (al[2] != NIL) {
					ap = rvalue(al[2], NIL);
					if (ap == NIL)
						continue;
					if (isnta(ap, "i")) {
						error("First write width must be integer, not %s", nameof(ap));
						continue;
					}
					op =| even(width(ap)) << 8;
				}
				al = al[1];
				if (al == NIL)
					continue;
			}
			if (filetype != nl+T1CHAR) {
				if (oct || hex) {
					error("Oct/hex allowed only on text files");
					continue;
				}
				if (op) {
					error("Write widths allowed only on text files");
					continue;
				}
				/*
				 * Generalized write, i.e.
				 * to a non-textfile.
				 */
				rvalue(file, NIL);
				put1(O_FNIL);
				/*
				 * file^ := ...
				 */
				ap = rvalue(argv[1], NIL);
				if (ap == NIL)
					continue;
				if (incompat(ap, filetype, argv[1])) {
					cerror("Type mismatch in write to non-text file");
					continue;
				}
				convert(ap, filetype);
				put2(O_AS, width(filetype));
				/*
				 * put(file)
				 */
				put1(O_PUT);
				continue;
			}
			/*
			 * Write to a textfile
			 *
			 * Evaluate the expression
			 * to be written.
			 */
			ap = rvalue(al, NIL);
			if (ap == NIL)
				continue;
			c = classify(ap);
			if (two && c != TDOUBLE) {
				if (isnta(ap, "i")) {
					error("Only reals can have two write widths");
					continue;
				}
				convert(ap, nl+TDOUBLE);
				c = TDOUBLE;
			}
			if (oct || hex) {
				if (opt('s')) {
					standard();
					error("Oct and hex are non-standard");
				}
				switch (c) {
					case TREC:
					case TARY:
					case TFILE:
					case TSTR:
					case TSET:
					case TDOUBLE:
						error("Can't write %ss with oct/hex", clnames[c]);
						continue;
				}
				put1(op | (oct ? O_WROCT2 : O_WRHEX2) | (width(ap) >> 2));
				continue;
			}
			if (wrops(c) == NIL) {
				error("Can't write %ss to a text file", clnames[c]);
				continue;
			}
			if (c == TINT && width(ap) != 4)
				op =| O_WRIT2;
			else
				op =| wrops(c);
			if (c == TSTR)
				put2(op, width(ap));
			else
				put1(op);
		}
		/*
		 * Done with arguments.
		 * Handle writeln and
		 * insufficent number of args.
		 */
		switch (p->value[0] &~ NSTAND) {
			case O_WRIT2:
				if (argc == 0)
					error("Write requires an argument");
				break;
			case O_MESSAGE:
				if (argc == 0)
					error("Message requires an argument");
			case O_WRITLN:
				if (filetype != nl+T1CHAR)
					error("Can't 'writeln' a non text file");
				put1(O_WRITLN);
				break;
		}
		return;

	case O_READ4:
	case O_READLN:
		/*
		 * Set up default
		 * file "input".
		 */
		file = NIL;
		filetype = nl+T1CHAR;
		/*
		 * Determine the file implied
		 * for the read and generate
		 * code to make it the active file.
		 */
		if (argv != NIL) {
			codeoff();
			ap = rvalue(argv[1], NIL);
			codeon();
			if (ap == NIL)
				argv = argv[2];
			if (ap != NIL && ap->class == FILE) {
				/*
				 * Got "read(f, ...", make
				 * f the active file, and save
				 * it and its type for use in
				 * processing the rest of the
				 * arguments to read.
				 */
				file = argv[1];
				filetype = ap->type;
				rvalue(argv[1], NIL);
				put1(O_UNIT);
				argv = argv[2];
				argc--;
			} else {
				/*
				 * Default is read from
				 * standard input.
				 */
				put1(O_UNITINP);
				input->nl_flags =| NUSED;
			}
		} else {
			put1(O_UNITINP);
			input->nl_flags =| NUSED;
		}
		/*
		 * Loop and process each
		 * of the arguments.
		 */
		for (; argv != NIL; argv = argv[2]) {
			/*
			 * Get the address of the target
			 * on the stack.
			 */
			al = argv[1];
			if (al == NIL)
				continue;
			if (al[0] != T_VAR) {
				error("Arguments to %s must be variables, not expressions", p->symbol);
				continue;
			}
			ap = lvalue(al, MOD|ASGN|NOUSE);
			if (ap == NIL)
				continue;
			if (filetype != nl+T1CHAR) {
				/*
				 * Generalized read, i.e.
				 * from a non-textfile.
				 */
				if (incompat(filetype, ap, NIL)) {
					error("Type mismatch in read from non-text file");
					continue;
				}
				/*
				 * var := file ^;
				 */
				if (file != NIL)
					rvalue(file, NIL);
				else /* Magic */
					put2(O_RV2, input->value[0]);
				put1(O_FNIL);
				put2(O_IND, width(filetype));
				convert(filetype, ap);
				if (isa(ap, "bsci"))
					rangechk(ap, ap);
				put2(O_AS, width(ap));
				/*
				 * get(file);
				 */
				put1(O_GET);
				continue;
			}
			c = classify(ap);
			op = rdops(c);
			if (op == NIL) {
				error("Can't read %ss from a text file", clnames[c]);
				continue;
			}
			put1(op);
			/*
			 * Data read is on the stack.
			 * Assign it.
			 */
			if (op != O_READ8)
				rangechk(ap, op == O_READC ? ap : nl+T4INT);
			gen(O_AS2, O_AS2, width(ap),
				op == O_READ8 ? 8 : op == O_READ4 ? 4 : 2);
		}
		/*
		 * Done with arguments.
		 * Handle readln and
		 * insufficient number of args.
		 */
		if (p->value[0] == O_READLN) {
			if (filetype != nl+T1CHAR)
				error("Can't 'readln' a non text file");
			put1(O_READLN);
		}
		else if (argc == 0)
			error("read requires an argument");
		return;

	case O_GET:
	case O_PUT:
		if (argc != 1) {
			error("%s expects one argument", p->symbol);
			return;
		}
		ap = rvalue(argv[1], NIL);
		if (ap == NIL)
			return;
		if (ap->class != FILE) {
			error("Argument to %s must be a file, not %s", p->symbol, nameof(ap));
			return;
		}
		put1(O_UNIT);
		put1(op);
		return;

	case O_RESET:
	case O_REWRITE:
		if (argc == 0 || argc > 2) {
			error("%s expects one or two arguments", p->symbol);
			return;
		}
		if (opt('s') && argc == 2) {
			standard();
			error("Two argument forms of reset and rewrite are non-standard");
		}
		ap = lvalue(argv[1], MOD|NOUSE);
		if (ap == NIL)
			return;
		if (ap->class != FILE) {
			error("First argument to %s must be a file, not %s", p->symbol, nameof(ap));
			return;
		}
		if (argc == 2) {
			/*
			 * Optional second argument
			 * is a string name of a
			 * UNIX (R) file to be associated.
			 */
			al = argv[2];
			al = rvalue(al[1], NIL);
			if (al == NIL)
				return;
			if (classify(al) != TSTR) {
				error("Second argument to %s must be a string, not %s", p->symbol, nameof(al));
				return;
			}
			c = width(al);
		} else
			c = 0;
		if (c > 127) {
			error("File name too long");
			return;
		}
		put2(op | c << 8, text(ap) ? 0: width(ap->type));
		return;

	case O_NEW:
	case O_DISPOSE:
		if (argc == 0) {
			error("%s expects at least one argument", p->symbol);
			return;
		}
		ap = lvalue(argv[1], MOD|NOUSE);
		if (ap == NIL)
			return;
		if (ap->class != PTR) {
			error("(First) argument to %s must be a pointer, not %s", p->symbol, nameof(ap));
			return;
		}
		ap = ap->type;
		if (ap == NIL)
			return;
		argv = argv[2];
		if (argv != NIL) {
			if (ap->class != RECORD) {
				error("Record required when specifying variant tags");
				return;
			}
			for (; argv != NIL; argv = argv[2]) {
				if (ap->value[NL_VARNT] == NIL) {
					error("Too many tag fields");
					return;
				}
				if (!isconst(argv[1])) {
					error("Second and successive arguments to %s must be constants", p->symbol);
					return;
				}
				gconst(argv[1]);
				if (con.ctype == NIL)
					return;
				if (incompat(con.ctype, ap->value[NL_TAG]->type)) {
					cerror("Specified tag constant type clashed with variant case selector type");
					return;
				}
				for (ap = ap->value[NL_VARNT]; ap != NIL; ap = ap->chain)
					if (ap->range[0] == con.crval)
						break;
				if (ap == NIL) {
					error("No variant case label value equals specified constant value");
					return;
				}
				ap = ap->value[NL_VTOREC];
			}
		}
		put2(op, width(ap));
		return;

	case O_DATE:
	case O_TIME:
		if (argc != 1) {
			error("%s expects one argument", p->symbol);
			return;
		}
		ap = lvalue(argv[1], MOD|NOUSE);
		if (ap == NIL)
			return;
		if (classify(ap) != TSTR || width(ap) != 10) {
			error("Argument to %s must be a alfa, not %s", p->symbol, nameof(ap));
			return;
		}
		put1(op);
		return;

	case O_HALT:
		if (argc != 0) {
			error("halt takes no arguments");
			return;
		}
		put1(op);
		noreach = 1;
		return;

	case O_ARGV:
		if (argc != 2) {
			error("argv takes two arguments");
			return;
		}
		ap = rvalue(argv[1], NIL);
		if (ap == NIL)
			return;
		if (isnta(ap, "i")) {
			error("argv's first argument must be an integer, not %s", nameof(ap));
			return;
		}
		convert(ap, nl+T2INT);
		al = argv[2];
		ap = lvalue(al[1], MOD|NOUSE);
		if (ap == NIL)
			return;
		if (classify(ap) != TSTR) {
			error("argv's second argument must be a string, not %s", nameof(ap));
			return;
		}
		put2(op, width(ap));
		return;

	case O_STLIM:
		if (argc != 1) {
			error("stlimit requires one argument");
			return;
		}
		ap = rvalue(argv[1], NIL);
		if (ap == NIL)
			return;
		if (isnta(ap, "i")) {
			error("stlimit's argument must be an integer, not %s", nameof(ap));
			return;
		}
		if (width(ap) != 4)
			put1(O_STOI);
		put1(op);
		return;

	case O_REMOVE:
		if (argc != 1) {
			error("remove expects one argument");
			return;
		}
		ap = rvalue(argv[1], NIL);
		if (ap == NIL)
			return;
		if (classify(ap) != TSTR) {
			error("remove's argument must be a string, not %s", nameof(ap));
			return;
		}
		put2(op, width(ap));
		return;

	case O_LLIMIT:
		if (argc != 2) {
			error("linelimit expects two arguments");
			return;
		}
		ap = lvalue(argv[1], NOMOD|NOUSE);
		if (ap == NIL)
			return;
		if (!text(ap)) {
			error("linelimit's first argument must be a text file, not %s", nameof(ap));
			return;
		}
		al = argv[2];
		ap = rvalue(al[1], NIL);
		if (ap == NIL)
			return;
		if (isnta(ap, "i")) {
			error("linelimit's second argument must be an integer, not %s", nameof(ap));
			return;
		}
		convert(ap, nl+T2INT);
		put1(op);
		return;
	case O_PAGE:
		if (argc != 1) {
			error("page expects one argument");
			return;
		}
		ap = rvalue(argv[1], NIL);
		if (ap == NIL)
			return;
		if (!text(ap)) {
			error("Argument to page must be a text file, not %s", nameof(ap));
			return;
		}
		put1(O_UNIT);
		put1(op);
		return;

	case O_PACK:
		if (argc != 3) {
			error("pack expects three arguments");
			return;
		}
		pu = "pack(a,i,z)";
		pua = (al = argv)[1];
		pui = (al = al[2])[1];
		puz = (al = al[2])[1];
		goto packunp;
	case O_UNPACK:
		if (argc != 3) {
			error("unpack expects three arguments");
			return;
		}
		pu = "unpack(z,a,i)";
		puz = (al = argv)[1];
		pua = (al = al[2])[1];
		pui = (al = al[2])[1];
packunp:
		ap = rvalue(pui, NIL);
		if (ap == NIL)
			return;
		if (width(ap) == 4)
			put1(O_ITOS);
		ap = lvalue(pua, op == O_PACK ? NOMOD : MOD|NOUSE);
		if (ap == NIL)
			return;
		if (ap->class != ARRAY) {
			error("%s requires a to be an unpacked array, not %s", pu, nameof(ap));
			return;
		}
		al = lvalue(puz, op == O_UNPACK ? NOMOD : MOD|NOUSE);
		if (al->class != ARRAY) {
			error("%s requires z to be a packed array, not %s", pu, nameof(ap));
			return;
		}
		if (al->type == NIL || ap->type == NIL)
			return;
		if (al->type != ap->type) {
			error("%s requires a and z to be arrays of the same type", pu, nameof(ap));
			return;
		}
		k = width(al);
		ap = ap->chain;
		al = al->chain;
		if (ap->chain != NIL || al->chain != NIL) {
			error("%s requires a and z to be single dimension arrays", pu);
			return;
		}
		if (ap == NIL || al == NIL)
			return;
		/*
		 * al is the range for z i.e. u..v
		 * ap is the range for a i.e. m..n
		 * i will be n-m+1
		 * j will be v-u+1
		 */
		i = ap->range[1] - ap->range[0] + 1;
		j = al->range[1] - al->range[0] + 1;
		if (i < j) {
			error("%s cannot have more elements in a (%d) than in z (%d)", pu, j, i);
			return;
		}
		/*
		 * get n-m-(v-u) and m for the interpreter
		 */
		i =- j;
		j = ap->range[0];
		put(5, op, width(ap), j, i, k);
		return;
	case 0:
		error("%s is an unimplemented 6400 extension", p->symbol);
		return;

	default:
		panic("proc case");
	}
}

