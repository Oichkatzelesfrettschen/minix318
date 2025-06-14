/**
 * @file signal_unified.c
 * @brief Unified signal handling
 * @details This file is a synthesized/unified implementation combining multiple
 *          legacy MINIX implementations into a single, modern, C23-compliant
 *          implementation for MINIX4.
 *
 * Original sources consolidated:
 * - minix4\microkernel\servers\lib\libc\port\sys\signal.c
 * - minix4\libos\lib_legacy\libucb\sparc\sys\signal.c
 * - minix4\libos\lib_legacy\libucb\i386\sys\signal.c
 * - minix4\libos\lib_legacy\brand\lx\lx_brand\common\signal.c
 * - minix4\exokernel\kernel_legacy\signal.c
 * - minix4\exokernel\kernel_legacy\contrib\ast\src\lib\libast\features\signal.c
 * - minix4\exokernel\kernel_legacy\contrib\ast\src\lib\libast\misc\signal.c
 * - minix4\exokernel\kernel_legacy\cmd\sendmail\libmilter\signal.c
 * - minix4\exokernel\kernel_legacy\cmd\sendmail\libsm\signal.c
 * - minix4\exokernel\kernel_legacy\cmd\csh\i386\signal.c
 * - ... and 1 more files
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
 * This file contains 11 very different implementations that
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
 * SOURCE 1/11: minix4\microkernel\servers\lib\libc\port\sys\signal.c
 * Size: 4,974 bytes, Lines: 231
 * Hash: caad30a66913...
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

#pragma weak _signal = signal
#pragma weak _sighold = sighold
#pragma weak _sigrelse = sigrelse
#pragma weak _sigignore = sigignore
#pragma weak _sigset = sigset

#include "lint.h"
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>

/*
 * Check for valid signal number as per SVID.
 */
#define	CHECK_SIG(s, code) \
	if ((s) <= 0 || (s) >= NSIG || (s) == SIGKILL || (s) == SIGSTOP) { \
		errno = EINVAL; \
		return (code); \
	}

/*
 * Equivalent to stopdefault set in the kernel implementation (sig.c).
 */
#define	STOPDEFAULT(s) \
	((s) == SIGSTOP || (s) == SIGTSTP || (s) == SIGTTOU || (s) == SIGTTIN)


/*
 * SVr3.x signal compatibility routines. They are now
 * implemented as library routines instead of system
 * calls.
 */

void(*
signal(int sig, void(*func)(int)))(int)
{
	struct sigaction nact;
	struct sigaction oact;

	CHECK_SIG(sig, SIG_ERR);

	nact.sa_handler = func;
	nact.sa_flags = SA_RESETHAND|SA_NODEFER;
	(void) sigemptyset(&nact.sa_mask);

	/*
	 * Pay special attention if sig is SIGCHLD and
	 * the disposition is SIG_IGN, per sysV signal man page.
	 */
	if (sig == SIGCHLD) {
		nact.sa_flags |= SA_NOCLDSTOP;
		if (func == SIG_IGN)
			nact.sa_flags |= SA_NOCLDWAIT;
	}

	if (STOPDEFAULT(sig))
		nact.sa_flags |= SA_RESTART;

	if (sigaction(sig, &nact, &oact) < 0)
		return (SIG_ERR);

	return (oact.sa_handler);
}

int
sighold(int sig)
{
	sigset_t set;

	CHECK_SIG(sig, -1);

	/*
	 * errno set on failure by either sigaddset or sigprocmask.
	 */
	(void) sigemptyset(&set);
	if (sigaddset(&set, sig) < 0)
		return (-1);
	return (sigprocmask(SIG_BLOCK, &set, (sigset_t *)0));
}

int
sigrelse(int sig)
{
	sigset_t set;

	CHECK_SIG(sig, -1);

	/*
	 * errno set on failure by either sigaddset or sigprocmask.
	 */
	(void) sigemptyset(&set);
	if (sigaddset(&set, sig) < 0)
		return (-1);
	return (sigprocmask(SIG_UNBLOCK, &set, (sigset_t *)0));
}

int
sigignore(int sig)
{
	struct sigaction act;
	sigset_t set;

	CHECK_SIG(sig, -1);

	act.sa_handler = SIG_IGN;
	act.sa_flags = 0;
	(void) sigemptyset(&act.sa_mask);

	/*
	 * Pay special attention if sig is SIGCHLD and
	 * the disposition is SIG_IGN, per sysV signal man page.
	 */
	if (sig == SIGCHLD) {
		act.sa_flags |= SA_NOCLDSTOP;
		act.sa_flags |= SA_NOCLDWAIT;
	}

	if (STOPDEFAULT(sig))
		act.sa_flags |= SA_RESTART;

	if (sigaction(sig, &act, (struct sigaction *)0) < 0)
		return (-1);

	(void) sigemptyset(&set);
	if (sigaddset(&set, sig) < 0)
		return (-1);
	return (sigprocmask(SIG_UNBLOCK, &set, (sigset_t *)0));
}

int
__sigpause(int sig)
{
	sigset_t set;
	int rval;

	CHECK_SIG(sig, -1);

	/*
	 * sigpause() is defined to unblock the signal
	 * and not block it again on return.
	 * sigsuspend() restores the original signal set,
	 * so we have to unblock sig overtly.
	 */
	(void) sigprocmask(0, (sigset_t *)0, &set);
	if (sigdelset(&set, sig) < 0)
		return (-1);
	rval = sigsuspend(&set);
	(void) sigrelse(sig);
	return (rval);
}

void(*
sigset(int sig, void(*func)(int)))(int)
{
	struct sigaction nact;
	struct sigaction oact;
	sigset_t nset;
	sigset_t oset;
	int code;

	CHECK_SIG(sig, SIG_ERR);

	(void) sigemptyset(&nset);
	if (sigaddset(&nset, sig) < 0)
		return (SIG_ERR);

	if (func == SIG_HOLD) {
		if (sigprocmask(SIG_BLOCK, &nset, &oset) < 0)
			return (SIG_ERR);
		if (sigaction(sig, (struct sigaction *)0, &oact) < 0)
			return (SIG_ERR);
	} else {
		nact.sa_handler = func;
		nact.sa_flags = 0;
		(void) sigemptyset(&nact.sa_mask);
		/*
		 * Pay special attention if sig is SIGCHLD and
		 * the disposition is SIG_IGN, per sysV signal man page.
		 */
		if (sig == SIGCHLD) {
			nact.sa_flags |= SA_NOCLDSTOP;
			if (func == SIG_IGN)
				nact.sa_flags |= SA_NOCLDWAIT;
		}

		if (STOPDEFAULT(sig))
			nact.sa_flags |= SA_RESTART;

		if (sigaction(sig, &nact, &oact) < 0)
			return (SIG_ERR);

		if (sigprocmask(SIG_UNBLOCK, &nset, &oset) < 0)
			return (SIG_ERR);
	}

	if ((code = sigismember(&oset, sig)) < 0)
		return (SIG_ERR);
	else if (code == 1)
		return (SIG_HOLD);

	return (oact.sa_handler);
}



/* ============================================================================
 * SOURCE 2/11: minix4\libos\lib_legacy\libucb\sparc\sys\signal.c
 * Size: 14,857 bytes, Lines: 613
 * Hash: 9d29eb131513...
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

/*	Copyright (c) 1983, 1984, 1985, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved	*/

/*
 * Portions of this source code were derived from Berkeley 4.3 BSD
 * under license from the Regents of the University of California.
 */

/*
 * 4.3BSD signal compatibility functions
 *
 * the implementation interprets signal masks equal to -1 as "all of the
 * signals in the signal set", thereby allowing signals with numbers
 * above 32 to be blocked when referenced in code such as:
 *
 *	for (i = 0; i < NSIG; i++)
 *		mask |= sigmask(i)
 */

#include <sys/types.h>
#include <ucontext.h>
#include <signal.h>
#include <errno.h>

#undef	BUS_OBJERR	/* namespace conflict */
#include <sys/siginfo.h>
#include "libc.h"

#pragma weak sigvechandler = _sigvechandler
#pragma weak sigsetmask = _sigsetmask
#pragma weak sigblock = _sigblock
#pragma weak sigpause = usigpause
#pragma weak sigvec = _sigvec
#pragma weak sigstack = _sigstack
#pragma weak signal = usignal
#pragma weak siginterrupt = _siginterrupt

/*
 * DO NOT remove the _ from these 3 functions or the subsequent
 * calls to them below.  The non _ versions of these functions
 * are the wrong functions to call.  This is BCP.  Extra
 * care should be taken when modifying this code.
 */
extern int _sigfillset(sigset_t *);
extern int _sigemptyset(sigset_t *);
extern int _sigprocmask(int, const sigset_t *, sigset_t *);

#define	set2mask(setp) ((setp)->__sigbits[0])
#define	mask2set(mask, setp) \
	((mask) == -1 ? _sigfillset(setp) : \
	    ((void) _sigemptyset(setp), (((setp)->__sigbits[0]) = (int)(mask))))

void (*_siguhandler[NSIG])() = { 0 };

/*
 * forward declarations
 */
int ucbsiginterrupt(int, int);
int ucbsigvec(int, struct sigvec *, struct sigvec *);
int ucbsigpause(int);
int ucbsigblock(int);
int ucbsigsetmask(int);
static void ucbsigvechandler(int, siginfo_t *, ucontext_t *);

/*
 * sigvechandler is the real signal handler installed for all
 * signals handled in the 4.3BSD compatibility interface - it translates
 * SVR4 signal hander arguments into 4.3BSD signal handler arguments
 * and then calls the real handler
 */

int
_sigvechandler(int sig, siginfo_t *sip, ucontext_t *ucp)
{
	ucbsigvechandler(sig, sip, ucp);
	return (0);	/* keep the same as the original prototype */
}

static void
ucbsigvechandler(int sig, siginfo_t *sip, ucontext_t *ucp)
{
	struct sigcontext sc;
	int code;
	char *addr;
#ifdef NEVER
	int gwinswitch = 0;
#endif

	sc.sc_onstack = ((ucp->uc_stack.ss_flags & SS_ONSTACK) != 0);
	sc.sc_mask = set2mask(&ucp->uc_sigmask);

#if defined(__sparc)
	if (sig == SIGFPE && sip != NULL && SI_FROMKERNEL(sip) &&
	    (sip->si_code == FPE_INTDIV || sip->si_code == FPE_INTOVF)) {
		/*
		 * Hack to emulate the 4.x kernel behavior of incrementing
		 * the PC on integer divide by zero and integer overflow
		 * on sparc machines.  (5.x does not increment the PC.)
		 */
		ucp->uc_mcontext.gregs[REG_PC] =
		    ucp->uc_mcontext.gregs[REG_nPC];
		ucp->uc_mcontext.gregs[REG_nPC] += 4;
	}
	sc.sc_sp = ucp->uc_mcontext.gregs[REG_SP];
	sc.sc_pc = ucp->uc_mcontext.gregs[REG_PC];
	sc.sc_npc = ucp->uc_mcontext.gregs[REG_nPC];

	/* XX64 There is no REG_PSR for sparcv9, we map in REG_CCR for now */
#if defined(__sparcv9)
	sc.sc_psr = ucp->uc_mcontext.gregs[REG_CCR];
#else
	sc.sc_psr = ucp->uc_mcontext.gregs[REG_PSR];
#endif

	sc.sc_g1 = ucp->uc_mcontext.gregs[REG_G1];
	sc.sc_o0 = ucp->uc_mcontext.gregs[REG_O0];

	/*
	 * XXX - What a kludge!
	 * Store a pointer to the original ucontext_t in the sigcontext
	 * so that it's available to the sigcleanup call that needs to
	 * return from the signal handler.  Otherwise, vital information
	 * (e.g., the "out" registers) that's only saved in the
	 * ucontext_t isn't available to sigcleanup.
	 */
	sc.sc_wbcnt = (int)(sizeof (*ucp));
	sc.sc_spbuf[0] = (char *)(uintptr_t)sig;
	sc.sc_spbuf[1] = (char *)ucp;
#ifdef NEVER
	/*
	 * XXX - Sorry, we can never pass the saved register windows
	 * on in the sigcontext because we use that space to save the
	 * ucontext_t.
	 */
	if (ucp->uc_mcontext.gwins != (gwindows_t *)0) {
		int i, j;

		gwinswitch = 1;
		sc.sc_wbcnt = ucp->uc_mcontext.gwins->wbcnt;
		/* XXX - should use bcopy to move this in bulk */
		for (i = 0; i < ucp->uc_mcontext.gwins; i++) {
			sc.sc_spbuf[i] = ucp->uc_mcontext.gwins->spbuf[i];
			for (j = 0; j < 8; j++)
				sc.sc_wbuf[i][j] =
				    ucp->uc_mcontext.gwins->wbuf[i].rw_local[j];
			for (j = 0; j < 8; j++)
				sc.sc_wbuf[i][j+8] =
				    ucp->uc_mcontext.gwins->wbuf[i].rw_in[j];
		}
	}
#endif
#endif

	/*
	 * Translate signal codes from new to old.
	 * /usr/include/sys/siginfo.h contains new codes.
	 * /usr/ucbinclude/sys/signal.h contains old codes.
	 */
	code = 0;
	addr = SIG_NOADDR;
	if (sip != NULL && SI_FROMKERNEL(sip)) {
		addr = sip->si_addr;

		switch (sig) {
		case SIGILL:
			switch (sip->si_code) {
			case ILL_PRVOPC:
				code = ILL_PRIVINSTR_FAULT;
				break;
			case ILL_BADSTK:
				code = ILL_STACK;
				break;
			case ILL_ILLTRP:
				code = ILL_TRAP_FAULT(sip->si_trapno);
				break;
			default:
				code = ILL_ILLINSTR_FAULT;
				break;
			}
			break;

		case SIGEMT:
			code = EMT_TAG;
			break;

		case SIGFPE:
			switch (sip->si_code) {
			case FPE_INTDIV:
				code = FPE_INTDIV_TRAP;
				break;
			case FPE_INTOVF:
				code = FPE_INTOVF_TRAP;
				break;
			case FPE_FLTDIV:
				code = FPE_FLTDIV_TRAP;
				break;
			case FPE_FLTOVF:
				code = FPE_FLTOVF_TRAP;
				break;
			case FPE_FLTUND:
				code = FPE_FLTUND_TRAP;
				break;
			case FPE_FLTRES:
				code = FPE_FLTINEX_TRAP;
				break;
			default:
				code = FPE_FLTOPERR_TRAP;
				break;
			}
			break;

		case SIGBUS:
			switch (sip->si_code) {
			case BUS_ADRALN:
				code = BUS_ALIGN;
				break;
			case BUS_ADRERR:
				code = BUS_HWERR;
				break;
			default:	/* BUS_OBJERR */
				code = FC_MAKE_ERR(sip->si_errno);
				break;
			}
			break;

		case SIGSEGV:
			switch (sip->si_code) {
			case SEGV_MAPERR:
				code = SEGV_NOMAP;
				break;
			case SEGV_ACCERR:
				code = SEGV_PROT;
				break;
			default:
				code = FC_MAKE_ERR(sip->si_errno);
				break;
			}
			break;

		default:
			addr = SIG_NOADDR;
			break;
		}
	}

	(*_siguhandler[sig])(sig, code, &sc, addr);

	if (sc.sc_onstack)
		ucp->uc_stack.ss_flags |= SS_ONSTACK;
	else
		ucp->uc_stack.ss_flags &= ~SS_ONSTACK;
	mask2set(sc.sc_mask, &ucp->uc_sigmask);

#if defined(__sparc)
	ucp->uc_mcontext.gregs[REG_SP] = sc.sc_sp;
	ucp->uc_mcontext.gregs[REG_PC] = sc.sc_pc;
	ucp->uc_mcontext.gregs[REG_nPC] = sc.sc_npc;
#if defined(__sparcv9)
	ucp->uc_mcontext.gregs[REG_CCR] = sc.sc_psr;
#else
	ucp->uc_mcontext.gregs[REG_PSR] = sc.sc_psr;
#endif
	ucp->uc_mcontext.gregs[REG_G1] = sc.sc_g1;
	ucp->uc_mcontext.gregs[REG_O0] = sc.sc_o0;
#ifdef NEVER
	if (gwinswitch == 1) {
		int i, j;

		ucp->uc_mcontext.gwins->wbcnt = sc.sc_wbcnt;
		/* XXX - should use bcopy to move this in bulk */
		for (i = 0; i < sc.sc_wbcnt; i++) {
			ucp->uc_mcontext.gwins->spbuf[i] = sc.sc_spbuf[i];
			for (j = 0; j < 8; j++)
				ucp->uc_mcontext.gwins->wbuf[i].rw_local[j] =
				    sc.sc_wbuf[i][j];
			for (j = 0; j < 8; j++)
				ucp->uc_mcontext.gwins->wbuf[i].rw_in[j] =
				    sc.sc_wbuf[i][j+8];
		}
	}
#endif

	if (sig == SIGFPE) {
		if (ucp->uc_mcontext.fpregs.fpu_qcnt > 0) {
			ucp->uc_mcontext.fpregs.fpu_qcnt--;
			ucp->uc_mcontext.fpregs.fpu_q++;
		}
	}
#endif

	(void) setcontext(ucp);
}

#if defined(__sparc)
/*
 * Emulate the special sigcleanup trap.
 * This is only used by statically linked 4.x applications
 * and thus is only called by the static BCP support.
 * It lives here because of its close relationship with
 * the ucbsigvechandler code above.
 *
 * It's used by 4.x applications to:
 *	1. return from a signal handler (in __sigtramp)
 *	2. [sig]longjmp
 *	3. context switch, in the old 4.x liblwp
 */

void
__sigcleanup(struct sigcontext *scp)
{
	ucontext_t uc, *ucp;
	int sig;

	/*
	 * If there's a pointer to a ucontext_t hiding in the sigcontext,
	 * we *must* use that to return, since it contains important data
	 * such as the original "out" registers when the signal occurred.
	 */
	if (scp->sc_wbcnt == sizeof (*ucp)) {
		sig = (int)(uintptr_t)scp->sc_spbuf[0];
		ucp = (ucontext_t *)scp->sc_spbuf[1];
	} else {
		/*
		 * Otherwise, use a local ucontext_t and
		 * initialize it with getcontext.
		 */
		sig = 0;
		ucp = &uc;
		(void) getcontext(ucp);
	}

	if (scp->sc_onstack) {
		ucp->uc_stack.ss_flags |= SS_ONSTACK;
	} else
		ucp->uc_stack.ss_flags &= ~SS_ONSTACK;
	mask2set(scp->sc_mask, &ucp->uc_sigmask);

	ucp->uc_mcontext.gregs[REG_SP] = scp->sc_sp;
	ucp->uc_mcontext.gregs[REG_PC] = scp->sc_pc;
	ucp->uc_mcontext.gregs[REG_nPC] = scp->sc_npc;
#if defined(__sparcv9)
	ucp->uc_mcontext.gregs[REG_CCR] = scp->sc_psr;
#else
	ucp->uc_mcontext.gregs[REG_PSR] = scp->sc_psr;
#endif
	ucp->uc_mcontext.gregs[REG_G1] = scp->sc_g1;
	ucp->uc_mcontext.gregs[REG_O0] = scp->sc_o0;

	if (sig == SIGFPE) {
		if (ucp->uc_mcontext.fpregs.fpu_qcnt > 0) {
			ucp->uc_mcontext.fpregs.fpu_qcnt--;
			ucp->uc_mcontext.fpregs.fpu_q++;
		}
	}
	(void) setcontext(ucp);
	/* NOTREACHED */
}
#endif

int
_sigsetmask(int mask)
{
	return (ucbsigsetmask(mask));
}

int
ucbsigsetmask(int mask)
{
	sigset_t oset;
	sigset_t nset;

	(void) _sigprocmask(0, (sigset_t *)0, &nset);
	mask2set(mask, &nset);
	(void) _sigprocmask(SIG_SETMASK, &nset, &oset);
	return (set2mask(&oset));
}

int
_sigblock(int mask)
{
	return (ucbsigblock(mask));
}

int
ucbsigblock(int mask)
{
	sigset_t oset;
	sigset_t nset;

	(void) _sigprocmask(0, (sigset_t *)0, &nset);
	mask2set(mask, &nset);
	(void) _sigprocmask(SIG_BLOCK, &nset, &oset);
	return (set2mask(&oset));
}

int
usigpause(int mask)
{
	return (ucbsigpause(mask));
}

int
ucbsigpause(int mask)
{
	sigset_t set, oset;
	int ret;

	(void) _sigprocmask(0, (sigset_t *)0, &set);
	oset = set;
	mask2set(mask, &set);
	ret = sigsuspend(&set);
	(void) _sigprocmask(SIG_SETMASK, &oset, (sigset_t *)0);
	return (ret);
}

int
_sigvec(int sig, struct sigvec *nvec, struct sigvec *ovec)
{
	return (ucbsigvec(sig, nvec, ovec));
}

int
ucbsigvec(int sig, struct sigvec *nvec, struct sigvec *ovec)
{
	struct sigaction nact;
	struct sigaction oact;
	struct sigaction *nactp;
	void (*ohandler)(int, int, struct sigcontext *, char *);
	void (*nhandler)(int, int, struct sigcontext *, char *);

	if (sig <= 0 || sig >= NSIG) {
		errno = EINVAL;
		return (-1);
	}

	if ((long)ovec == -1L || (long)nvec == -1L) {
		errno = EFAULT;
		return (-1);
	}

	ohandler = _siguhandler[sig];

	if (nvec) {
		(void) _sigaction(sig, (struct sigaction *)0, &nact);
		nhandler = nvec->sv_handler;
		/*
		 * To be compatible with the behavior of SunOS 4.x:
		 * If the new signal handler is SIG_IGN or SIG_DFL,
		 * do not change the signal's entry in the handler array.
		 * This allows a child of vfork(2) to set signal handlers
		 * to SIG_IGN or SIG_DFL without affecting the parent.
		 */
		if ((void (*)(int))(uintptr_t)nhandler != SIG_DFL &&
		    (void (*)(int))(uintptr_t)nhandler != SIG_IGN) {
			_siguhandler[sig] = nhandler;
			nact.sa_handler =
			    (void (*)(int))(uintptr_t)ucbsigvechandler;
		} else {
			nact.sa_handler = (void (*)(int))(uintptr_t)nhandler;
		}
		mask2set(nvec->sv_mask, &nact.sa_mask);
		if (sig == SIGKILL || sig == SIGSTOP)
			nact.sa_handler = SIG_DFL;
		nact.sa_flags = SA_SIGINFO;
		if (!(nvec->sv_flags & SV_INTERRUPT))
			nact.sa_flags |= SA_RESTART;
		if (nvec->sv_flags & SV_RESETHAND)
			nact.sa_flags |= SA_RESETHAND;
		if (nvec->sv_flags & SV_ONSTACK)
			nact.sa_flags |= SA_ONSTACK;
		nactp = &nact;
	} else
		nactp = (struct sigaction *)0;

	if (_sigaction(sig, nactp, &oact) < 0) {
		_siguhandler[sig] = ohandler;
		return (-1);
	}

	if (ovec) {
		if (oact.sa_handler == SIG_DFL || oact.sa_handler == SIG_IGN)
			ovec->sv_handler =
			    (void (*) (int, int, struct sigcontext *, char *))
			    oact.sa_handler;
		else
			ovec->sv_handler = ohandler;
		ovec->sv_mask = set2mask(&oact.sa_mask);
		ovec->sv_flags = 0;
		if (oact.sa_flags & SA_ONSTACK)
			ovec->sv_flags |= SV_ONSTACK;
		if (oact.sa_flags & SA_RESETHAND)
			ovec->sv_flags |= SV_RESETHAND;
		if (!(oact.sa_flags & SA_RESTART))
			ovec->sv_flags |= SV_INTERRUPT;
	}

	return (0);
}

int
_sigstack(struct sigstack *nss, struct sigstack *oss)
{
	struct sigaltstack nalt;
	struct sigaltstack oalt;
	struct sigaltstack *naltp;

	if (nss) {
		/*
		 * XXX: assumes stack growth is down (like sparc)
		 */
		nalt.ss_sp = nss->ss_sp - SIGSTKSZ;
		nalt.ss_size = SIGSTKSZ;
		nalt.ss_flags = 0;
		naltp = &nalt;
	} else
		naltp = (struct sigaltstack *)0;

	if (sigaltstack(naltp, &oalt) < 0)
		return (-1);

	if (oss) {
		/*
		 * XXX: assumes stack growth is down (like sparc)
		 */
		oss->ss_sp = oalt.ss_sp + oalt.ss_size;
		oss->ss_onstack = ((oalt.ss_flags & SS_ONSTACK) != 0);
	}

	return (0);
}

void (*
ucbsignal(int s, void (*a)(int)))(int)
{
	struct sigvec osv;
	struct sigvec nsv;
	static int mask[NSIG];
	static int flags[NSIG];

	nsv.sv_handler =
	    (void (*) (int, int, struct sigcontext *, char *))(uintptr_t)a;
	nsv.sv_mask = mask[s];
	nsv.sv_flags = flags[s];
	if (ucbsigvec(s, &nsv, &osv) < 0)
		return (SIG_ERR);
	if (nsv.sv_mask != osv.sv_mask || nsv.sv_flags != osv.sv_flags) {
		mask[s] = nsv.sv_mask = osv.sv_mask;
		flags[s] = nsv.sv_flags =
		    osv.sv_flags & ~(SV_RESETHAND|SV_INTERRUPT);
		if (ucbsigvec(s, &nsv, (struct sigvec *)0) < 0)
			return (SIG_ERR);
	}
	return ((void (*) (int)) osv.sv_handler);
}

void (*
usignal(int s, void (*a) (int)))(int)
{
	return (ucbsignal(s, a));
}

/*
 * Set signal state to prevent restart of system calls
 * after an instance of the indicated signal.
 */

int
_siginterrupt(int sig, int flag)
{
	return (ucbsiginterrupt(sig, flag));
}

int
ucbsiginterrupt(int sig, int flag)
{
	struct sigvec sv;
	int ret;

	if ((ret = ucbsigvec(sig, 0, &sv)) < 0)
		return (ret);
	if (flag)
		sv.sv_flags |= SV_INTERRUPT;
	else
		sv.sv_flags &= ~SV_INTERRUPT;
	return (ucbsigvec(sig, &sv, 0));
}



/* ============================================================================
 * SOURCE 3/11: minix4\libos\lib_legacy\libucb\i386\sys\signal.c
 * Size: 9,601 bytes, Lines: 413
 * Hash: 7767369c2a49...
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

/*	Copyright (c) 1983, 1984, 1985, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*
 * Portions of this source code were derived from Berkeley 4.3 BSD
 * under license from the Regents of the University of California.
 */

/*
 * 4.3BSD signal compatibility functions
 *
 * the implementation interprets signal masks equal to -1 as "all of the
 * signals in the signal set", thereby allowing signals with numbers
 * above 32 to be blocked when referenced in code such as:
 *
 *	for (i = 0; i < NSIG; i++)
 *		mask |= sigmask(i)
 */

#include <sys/types.h>
#include <ucontext.h>
#include <signal.h>
#include <errno.h>

#undef	BUS_OBJERR	/* namespace conflict */
#include <sys/siginfo.h>
#include "libc.h"

#pragma weak sigvechandler = _sigvechandler
#pragma weak sigsetmask = _sigsetmask
#pragma weak sigblock = _sigblock
#pragma weak sigpause = usigpause
#pragma weak sigvec = _sigvec
#pragma weak sigstack = _sigstack
#pragma weak signal = usignal
#pragma weak siginterrupt = _siginterrupt

#define	set2mask(setp) ((setp)->__sigbits[0])
#define	mask2set(mask, setp) \
	((mask) == -1 ? sigfillset(setp) : \
	    (sigemptyset(setp), (((setp)->__sigbits[0]) = (int)(mask))))

void (*_siguhandler[NSIG])() = { 0 };

/* forward declarations */
int ucbsigsetmask(int);
int ucbsigblock(int);
int ucbsigvec(int, struct sigvec *, struct sigvec *);
int ucbsigpause(int);
int ucbsiginterrupt(int, int);

/*
 * sigvechandler is the real signal handler installed for all
 * signals handled in the 4.3BSD compatibility interface - it translates
 * SVR4 signal hander arguments into 4.3BSD signal handler arguments
 * and then calls the real handler
 */

static void ucbsigvechandler();
void
_sigvechandler(int sig, siginfo_t *sip, ucontext_t *ucp)
{

	ucbsigvechandler(sig, sip, ucp);
}

static void
ucbsigvechandler(int sig, siginfo_t *sip, ucontext_t *ucp)
{
	struct sigcontext sc;
	int code;
	char *addr;
	int i, j;
	int gwinswitch = 0;

	sc.sc_onstack = ((ucp->uc_stack.ss_flags & SS_ONSTACK) != 0);
	sc.sc_mask = set2mask(&ucp->uc_sigmask);

#if defined(__amd64)
	sc.sc_sp = (long)ucp->uc_mcontext.gregs[REG_RSP];
	sc.sc_pc = (long)ucp->uc_mcontext.gregs[REG_RIP];
	sc.sc_ps = (long)ucp->uc_mcontext.gregs[REG_RFL];
	sc.sc_r0 = (long)ucp->uc_mcontext.gregs[REG_RAX];
	sc.sc_r1 = (long)ucp->uc_mcontext.gregs[REG_RDX];
#else
	sc.sc_sp = (int)ucp->uc_mcontext.gregs[UESP];
	sc.sc_pc = (int)ucp->uc_mcontext.gregs[EIP];
	sc.sc_ps = (int)ucp->uc_mcontext.gregs[EFL];
	sc.sc_r0 = (int)ucp->uc_mcontext.gregs[EAX];
	sc.sc_r1 = (int)ucp->uc_mcontext.gregs[EDX];
#endif

	/*
	 * Translate signal codes from new to old.
	 * /usr/include/sys/siginfo.h contains new codes.
	 * /usr/ucbinclude/sys/signal.h contains old codes.
	 */
	code = 0;
	addr = SIG_NOADDR;
	if (sip != NULL && SI_FROMKERNEL(sip)) {
		addr = sip->si_addr;

		switch (sig) {
		case SIGILL:
		case SIGFPE:
			code = ILL_ILLINSTR_FAULT;
			break;

		case SIGBUS:
			switch (sip->si_code) {
			case BUS_ADRALN:
				code = BUS_ALIGN;
				break;
			case BUS_ADRERR:
				code = BUS_HWERR;
				break;
			default:	/* BUS_OBJERR */
				code = FC_MAKE_ERR(sip->si_errno);
				break;
			}
			break;

		case SIGSEGV:
			switch (sip->si_code) {
			case SEGV_MAPERR:
				code = SEGV_NOMAP;
				break;
			case SEGV_ACCERR:
				code = SEGV_PROT;
				break;
			default:
				code = FC_MAKE_ERR(sip->si_errno);
				break;
			}
			break;

		default:
			addr = SIG_NOADDR;
			break;
		}
	}

	(*_siguhandler[sig])(sig, code, &sc, addr);

	if (sc.sc_onstack)
		ucp->uc_stack.ss_flags |= SS_ONSTACK;
	else
		ucp->uc_stack.ss_flags &= ~SS_ONSTACK;
	mask2set(sc.sc_mask, &ucp->uc_sigmask);

#if defined(__amd64)
	ucp->uc_mcontext.gregs[REG_RSP] = (long)sc.sc_sp;
	ucp->uc_mcontext.gregs[REG_RIP] = (long)sc.sc_pc;
	ucp->uc_mcontext.gregs[REG_RFL] = (long)sc.sc_ps;
	ucp->uc_mcontext.gregs[REG_RAX] = (long)sc.sc_r0;
	ucp->uc_mcontext.gregs[REG_RDX] = (long)sc.sc_r1;
#else
	ucp->uc_mcontext.gregs[UESP] = (int)sc.sc_sp;
	ucp->uc_mcontext.gregs[EIP] = (int)sc.sc_pc;
	ucp->uc_mcontext.gregs[EFL] = (int)sc.sc_ps;
	ucp->uc_mcontext.gregs[EAX] = (int)sc.sc_r0;
	ucp->uc_mcontext.gregs[EDX] = (int)sc.sc_r1;
#endif

	setcontext(ucp);
}

int
_sigsetmask(int mask)
{
	return (ucbsigsetmask(mask));
}

int
ucbsigsetmask(int mask)
{
	sigset_t oset;
	sigset_t nset;

	(void) sigprocmask(0, (sigset_t *)0, &nset);
	mask2set(mask, &nset);
	(void) sigprocmask(SIG_SETMASK, &nset, &oset);
	return (set2mask(&oset));
}

int
_sigblock(int mask)
{
	return (ucbsigblock(mask));
}

int
ucbsigblock(int mask)
{
	sigset_t oset;
	sigset_t nset;

	(void) sigprocmask(0, (sigset_t *)0, &nset);
	mask2set(mask, &nset);
	(void) sigprocmask(SIG_BLOCK, &nset, &oset);
	return (set2mask(&oset));
}

int
usigpause(int mask)
{
	return (ucbsigpause(mask));
}

int
ucbsigpause(int mask)
{
	sigset_t set, oset;
	int ret;

	(void) sigprocmask(0, (sigset_t *)0, &set);
	oset = set;
	mask2set(mask, &set);
	ret = sigsuspend(&set);
	(void) sigprocmask(SIG_SETMASK, &oset, (sigset_t *)0);
	return (ret);
}

int
_sigvec(int sig, struct sigvec *nvec, struct sigvec *ovec)
{
	return (ucbsigvec(sig, nvec, ovec));
}

int
ucbsigvec(int sig, struct sigvec *nvec, struct sigvec *ovec)
{
	struct sigaction nact;
	struct sigaction oact;
	struct sigaction *nactp;
	void (*ohandler)(), (*nhandler)();

	if (sig <= 0 || sig >= NSIG) {
		errno = EINVAL;
		return (-1);
	}

	if ((intptr_t)ovec == -1 || (intptr_t)nvec == -1) {
		errno = EFAULT;
		return (-1);
	}

	ohandler = _siguhandler[sig];

	if (nvec) {
		_sigaction(sig, (struct sigaction *)0, &nact);
		nhandler = nvec->sv_handler;
		/*
		 * To be compatible with the behavior of SunOS 4.x:
		 * If the new signal handler is SIG_IGN or SIG_DFL,
		 * do not change the signal's entry in the handler array.
		 * This allows a child of vfork(2) to set signal handlers
		 * to SIG_IGN or SIG_DFL without affecting the parent.
		 */
		if (nhandler != SIG_DFL && nhandler != SIG_IGN) {
			_siguhandler[sig] = nhandler;
			nact.sa_handler = (void (*)())ucbsigvechandler;
		} else {
			nact.sa_handler = nhandler;
		}
		mask2set(nvec->sv_mask, &nact.sa_mask);
		if (sig == SIGKILL || sig == SIGSTOP)
			nact.sa_handler = SIG_DFL;
		nact.sa_flags = SA_SIGINFO;
		if (!(nvec->sv_flags & SV_INTERRUPT))
			nact.sa_flags |= SA_RESTART;
		if (nvec->sv_flags & SV_RESETHAND)
			nact.sa_flags |= SA_RESETHAND;
		if (nvec->sv_flags & SV_ONSTACK)
			nact.sa_flags |= SA_ONSTACK;
		nactp = &nact;
	} else
		nactp = (struct sigaction *)0;

	if (_sigaction(sig, nactp, &oact) < 0) {
		_siguhandler[sig] = ohandler;
		return (-1);
	}

	if (ovec) {
		if (oact.sa_handler == SIG_DFL || oact.sa_handler == SIG_IGN)
			ovec->sv_handler = oact.sa_handler;
		else
			ovec->sv_handler = ohandler;
		ovec->sv_mask = set2mask(&oact.sa_mask);
		ovec->sv_flags = 0;
		if (oact.sa_flags & SA_ONSTACK)
			ovec->sv_flags |= SV_ONSTACK;
		if (oact.sa_flags & SA_RESETHAND)
			ovec->sv_flags |= SV_RESETHAND;
		if (!(oact.sa_flags & SA_RESTART))
			ovec->sv_flags |= SV_INTERRUPT;
	}

	return (0);
}

int
_sigstack(struct sigstack *nss, struct sigstack *oss)
{
	struct sigaltstack nalt;
	struct sigaltstack oalt;
	struct sigaltstack *naltp;

	if (nss) {
		/*
		 * assumes stack growth is down (like sparc and x86)
		 */
		nalt.ss_sp = nss->ss_sp - SIGSTKSZ;
		nalt.ss_size = SIGSTKSZ;
		nalt.ss_flags = 0;
		naltp = &nalt;
	} else
		naltp = (struct sigaltstack *)0;

	if (sigaltstack(naltp, &oalt) < 0)
		return (-1);

	if (oss) {
		/*
		 * assumes stack growth is down (like sparc and x86)
		 */
		oss->ss_sp = oalt.ss_sp + oalt.ss_size;
		oss->ss_onstack = ((oalt.ss_flags & SS_ONSTACK) != 0);
	}

	return (0);
}

void (*
ucbsignal(int s, void (*a)()))()
{
	struct sigvec osv;
	struct sigvec nsv;
	static int mask[NSIG];
	static int flags[NSIG];

	nsv.sv_handler = a;
	nsv.sv_mask = mask[s];
	nsv.sv_flags = flags[s];
	if (ucbsigvec(s, &nsv, &osv) < 0)
		return (SIG_ERR);
	if (nsv.sv_mask != osv.sv_mask || nsv.sv_flags != osv.sv_flags) {
		mask[s] = nsv.sv_mask = osv.sv_mask;
		flags[s] = nsv.sv_flags =
		    osv.sv_flags & ~(SV_RESETHAND|SV_INTERRUPT);
		if (ucbsigvec(s, &nsv, (struct sigvec *)0) < 0)
			return (SIG_ERR);
	}
	return (osv.sv_handler);
}

void (*
usignal(int s, void (*a)()))()
{
	return (ucbsignal(s, a));
}

/*
 * Set signal state to prevent restart of system calls
 * after an instance of the indicated signal.
 */
int
_siginterrupt(int sig, int flag)
{
	return (ucbsiginterrupt(sig, flag));
}

int
ucbsiginterrupt(int sig, int flag)
{
	struct sigvec sv;
	int ret;

	if ((ret = ucbsigvec(sig, 0, &sv)) < 0)
		return (ret);
	if (flag)
		sv.sv_flags |= SV_INTERRUPT;
	else
		sv.sv_flags &= ~SV_INTERRUPT;
	return (ucbsigvec(sig, &sv, 0));
}



/* ============================================================================
 * SOURCE 4/11: minix4\libos\lib_legacy\brand\lx\lx_brand\common\signal.c
 * Size: 74,109 bytes, Lines: 2,432
 * Hash: 163ae6d7aab7...
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
 */

/*
 * Copyright 2019 Joyent, Inc.
 * Copyright 2020 OmniOS Community Edition (OmniOSce) Association.
 */

#include <sys/types.h>
#include <sys/param.h>
#include <sys/segments.h>
#include <sys/lx_types.h>
#include <sys/lx_brand.h>
#include <sys/lx_misc.h>
#include <sys/lx_debug.h>
#include <sys/lx_poll.h>
#include <sys/lx_signal.h>
#include <sys/lx_sigstack.h>
#include <sys/lx_syscall.h>
#include <sys/lx_thread.h>
#include <sys/lx_userhz.h>
#include <sys/syscall.h>
#include <lx_provider_impl.h>
#include <sys/stack.h>
#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <rctl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <thread.h>
#include <ucontext.h>
#include <unistd.h>
#include <stdio.h>
#include <libintl.h>
#include <ieeefp.h>
#include <sys/signalfd.h>

#if defined(_ILP32)
extern int pselect_large_fdset(int nfds, fd_set *in0, fd_set *out0, fd_set *ex0,
	const timespec_t *tsp, const sigset_t *sp);
#endif

#define	MIN(a, b)	((a) < (b) ? (a) : (b))

/*
 * Delivering signals to a Linux process is complicated by differences in
 * signal numbering, stack structure and contents, and the action taken when a
 * signal handler exits.  In addition, many signal-related structures, such as
 * sigset_ts, vary between Illumos and Linux.
 *
 * To support user-level signal handlers, the brand uses a double layer of
 * indirection to process and deliver signals to branded threads.
 *
 * When a Linux process sends a signal using the kill(2) system call, we must
 * translate the signal into the Illumos equivalent before handing control off
 * to the standard signalling mechanism.  When a signal is delivered to a Linux
 * process, we translate the signal number from Illumos to back to Linux.
 * Translating signals both at generation and delivery time ensures both that
 * Illumos signals are sent properly to Linux applications and that signals'
 * default behavior works as expected.
 *
 * In a normal Illumos process, signal delivery is interposed on for any thread
 * registering a signal handler by libc. Libc needs to do various bits of magic
 * to provide thread-safe critical regions, so it registers its own handler,
 * named sigacthandler(), using the sigaction(2) system call. When a signal is
 * received, sigacthandler() is called, and after some processing, libc turns
 * around and calls the user's signal handler via a routine named
 * call_user_handler().
 *
 * Adding a Linux branded thread to the mix complicates things somewhat.
 *
 * First, when a thread receives a signal, it may either be running in an
 * emulated Linux context or a native illumos context.  In either case, the
 * in-kernel brand module is responsible for preserving the register state
 * from the interrupted context, regardless of whether emulated or native
 * software was running at the time.  The kernel is also responsible for
 * ensuring that the illumos native sigacthandler() is called with register
 * values appropriate for native code.  Of particular note is the %gs segment
 * selector for 32-bit code, and the %fsbase segment base register for 64-bit
 * code; these are used by libc to locate per-thread data structures.
 *
 * Second, the signal number translation referenced above must take place.
 * Finally, when we hand control to the Linux signal handler we must do so
 * on the brand stack, and with registers configured appropriately for the
 * Linux application.
 *
 * This need to translate signal numbers (and manipulate the signal handling
 * context) means that with standard Illumos libc, following a signal from
 * generation to delivery looks something like:
 *
 * 	kernel ->
 *	    sigacthandler() ->
 *		call_user_handler() ->
 *		    user signal handler
 *
 * but for the brand's Linux threads, this would look like:
 *
 *	kernel ->
 *	    sigacthandler() ->
 *		call_user_handler() ->
 *		    lx_call_user_handler() ->
 *			lx_sigdeliver() ->
 *			    syscall(B_JUMP_TO_LINUX, ...) ->
 *				Linux user signal handler
 *
 * The new addtions are:
 *
 * 	lx_call_user_handler
 *	====================
 *	This routine is responsible for translating Illumos signal numbers to
 *	their Linux equivalents, building a Linux signal stack based on the
 * 	information Illumos has provided, and passing the stack to the
 *	registered Linux signal handler. It is, in effect, the Linux thread
 *	equivalent to libc's call_user_handler().
 *
 * 	lx_sigdeliver
 *	=============
 *
 * Note that none of this interposition is necessary unless a Linux thread
 * registers a user signal handler, as the default action for all signals is the
 * same between Illumos and Linux save for one signal, SIGPWR.  For this reason,
 * the brand ALWAYS installs its own internal signal handler for SIGPWR that
 * translates the action to the Linux default, to terminate the process.
 * (Illumos' default action is to ignore SIGPWR.)
 *
 * A notable behavior of lx_sigdeliver is that it must replace the stack
 * pointer in the context that will be handed to the Linux signal handler.
 * There is at least one application (mono) which inspects the SP in the
 * context it receives and which fails when the SP is not within the thread's
 * stack range. There is not much else within the context that a signal
 * handler could depend on, so we only ensure that the SP is from the Linux
 * stack and not the alternate stack. lx_sigdeliver will restore the correct
 * SP when setcontext returns into this function as part of returning from
 * the signal handler.
 *
 * It is also important to note that when signals are not translated, the brand
 * relies upon code interposing upon the wait(2) system call to translate
 * signals to their proper values for any Linux threads retrieving the status
 * of others.  So while the Illumos signal number for a particular signal is set
 * in a process' data structures (and would be returned as the result of say,
 * WTERMSIG()), the brand's interposiiton upon wait(2) is responsible for
 * translating the value WTERMSIG() would return from a Illumos signal number
 * to the appropriate Linux value.
 *
 * lx_call_user_handler() calls lx_sigdeliver() with a helper function
 * (typically lx_build_signal_frame) which builds a stack frame for the 32-bit
 * Linux signal handler, or populates a local (on the stack) structure for the
 * 64-bit Linux signal handler. The stack at that time looks like this:
 *
 * 	=========================================================
 * |	| lx_sigdeliver_frame_t -- includes LX_SIGRT_MAGIC and	|
 * |	| a return context for the eventual sigreturn(2) call	|
 * | 	=========================================================
 * |	| Linux signal frame (32-bit) or local data		|
 * V	| (64-bit) built by stack_builder()			|
 * 	=========================================================
 *
 * The process of returning to an interrupted thread of execution from a user
 * signal handler is entirely different between Illumos and Linux.  While
 * Illumos generally expects to set the context to the interrupted one on a
 * normal return from a signal handler, in the normal case Linux instead calls
 * code that calls a specific Linux system call, rt_sigreturn(2) (or it also
 * can call sigreturn(2) in 32-bit code).  Thus when a Linux signal handler
 * completes execution, instead of returning through what would in libc be a
 * call to setcontext(2), the rt_sigreturn(2) Linux system call is responsible
 * for accomplishing much the same thing. It's for this reason that the stack
 * frame we build has the lx_(rt_)sigreturn_tramp code on the top of the
 * stack.  The code looks like this:
 *
 *	32-bit					64-bit
 *	--------------------------------	-----------------------------
 *	mov LX_SYS_rt_sigreturn, %eax		movq LX_SYS_rt_sigreturn, %rax
 *	int $0x80				syscall
 *
 * We also use these same functions (lx_rt_sigreturn_tramp or
 * lx_sigreturn_tramp) to actually return from the signal handler.
 *
 * (Note that this trampoline code actually lives in a proper executable segment
 * and not on the stack, but gdb checks for the exact code sequence of the
 * trampoline code on the stack to determine whether it is in a signal stack
 * frame or not.  Really.)
 *
 * When the 32-bit Linux user signal handler is eventually called, the brand
 * stack frame looks like this (in the case of a "modern" signal stack; see
 * the lx_sigstack structure definition):
 *
 *	=========================================================
 * |	| lx_sigdeliver_frame_t					|
 * |	=========================================================
 * |	| Trampoline code (marker for gdb, not really executed)	|
 * |	=========================================================
 * |	| Linux struct _fpstate					|
 * |	=========================================================
 * V	| Linux ucontext_t					| <--+
 *	=========================================================    |
 *	| Linux siginfo_t					| <--|-----+
 *	=========================================================    |     |
 *	| Pointer to Linux ucontext_t (or NULL)	(sigaction arg2)| ---+     |
 *	=========================================================          |
 *	| Pointer to Linux siginfo_t (or NULL)  (sigaction arg1)| ---------+
 *	=========================================================
 *	| Linux signal number                   (sigaction arg0)|
 *	=========================================================
 *	| Pointer to signal return code (trampoline code)	|
 *	=========================================================
 *
 * The 64-bit stack-local data looks like this:
 *
 *	=========================================================
 * |	| lx_sigdeliver_frame_t					|
 * |	=========================================================
 * |	| Trampoline code (marker for gdb, not really executed)	|
 * |	=========================================================
 * |	| Linux struct _fpstate					|
 * |	=========================================================
 * V	| Linux ucontext_t					| %rdx arg2
 *	=========================================================
 *	| Linux siginfo_t					| %rsi arg1
 *	=========================================================
 *	| Pointer to signal return code (trampoline code)	|
 *	=========================================================
 *
 * As usual in 64-bit code, %rdi is arg0 which is the signal number.
 *
 * The *sigreturn(2) family of emulated system call handlers locates the
 * "lx_sigdeliver_frame_t" struct on the Linux stack as part of processing
 * the system call.  This object contains a guard value (LX_SIGRT_MAGIC) to
 * detect stack smashing or an incorrect stack pointer.  It also contains a
 * "return" context, which we use to get back to the "lx_sigdeliver()" frame
 * on the native stack that originally dispatched to the Linux signal
 * handler.  The lx_sigdeliver() function is then able to return to the
 * native libc signal handler in the usual way.  This results in a further
 * setcontext() back to whatever was running when we took the signal.
 *
 * There are some edge cases where the "return" context cannot be located
 * by inspection of the Linux stack; e.g. if the guard value has been
 * corrupted, or the emulated program has relocated parts of the signal
 * delivery stack frame.  If this case is detected, a fallback mechanism is
 * used to attempt to find the return context.  A chain of "lx_sigbackup_t"
 * objects is maintained in signal interposer call frames, with the current
 * head stored in the thread-specific "lx_tsd_t".  This mechanism is
 * similar in principle to the "lwp_oldcontext" member of the "klwp_t" used
 * by the native signal handling infrastructure.  This backup chain is used
 * by the sigreturn(2) family of emulated system calls in the event that
 * the Linux stack did not correctly reference a return context.
 */

typedef struct lx_sigdeliver_frame {
	uintptr_t lxsdf_magic;
	ucontext_t *lxsdf_retucp;
	ucontext_t *lxsdf_sigucp;
	lx_sigbackup_t *lxsdf_sigbackup;
} lx_sigdeliver_frame_t;

struct lx_oldsigstack {
	void (*retaddr)();	/* address of real lx_sigreturn code */
	int sig;		/* signal number */
	lx_sigcontext_t sigc;	/* saved user context */
	lx_fpstate_t fpstate;	/* saved FP state */
	int sig_extra;		/* signal mask for signals [32 .. NSIG - 1] */
	char trampoline[8];	/* code for trampoline to lx_sigreturn() */
};

/*
 * The lx_sighandlers structure needs to be a global due to the semantics of
 * clone().
 *
 * If CLONE_SIGHAND is set, the calling process and child share signal
 * handlers, and if either calls sigaction(2) it should change the behavior
 * in the other thread.  Each thread does, however, have its own signal mask
 * and set of pending signals.
 *
 * If CLONE_SIGHAND is not set, the child process should inherit a copy of
 * the signal handlers at the time of the clone() but later calls to
 * sigaction(2) should only affect the individual thread calling it.
 *
 * This maps perfectly to a thr_create(3C) thread semantic in the first
 * case and a fork(2)-type semantic in the second case.  By making
 * lx_sighandlers global, we automatically get the correct behavior.
 */
static lx_sighandlers_t lx_sighandlers;

static void lx_sigdeliver(int, siginfo_t *, ucontext_t *, size_t, void (*)(),
    void (*)(), struct lx_sigaction *);

/*
 * stol_stack() and ltos_stack() convert between Illumos and Linux stack_t
 * structures.
 *
 * These routines are needed because although the two structures have the same
 * contents, their contents are declared in a different order, so the content
 * of the structures cannot be copied with a simple bcopy().
 */
static void
stol_stack(stack_t *fr, lx_stack_t *to)
{
	to->ss_sp = fr->ss_sp;
	to->ss_flags = fr->ss_flags;
	to->ss_size = fr->ss_size;
}

static void
ltos_stack(lx_stack_t *fr, stack_t *to)
{
	to->ss_sp = fr->ss_sp;
	to->ss_flags = fr->ss_flags;
	to->ss_size = fr->ss_size;
}

static int
ltos_sigset(lx_sigset_t *lx_sigsetp, sigset_t *s_sigsetp)
{
	lx_sigset_t l;
	int lx_sig, sig;

	if (uucopy(lx_sigsetp, &l, sizeof (lx_sigset_t)) != 0)
		return (-errno);

	(void) sigemptyset(s_sigsetp);

	for (lx_sig = 1; lx_sig <= LX_NSIG; lx_sig++) {
		if (lx_sigismember(&l, lx_sig) &&
		    ((sig = ltos_signo[lx_sig]) > 0))
			(void) sigaddset(s_sigsetp, sig);
	}

	return (0);
}

static int
stol_sigset(sigset_t *s_sigsetp, lx_sigset_t *lx_sigsetp)
{
	lx_sigset_t l;
	int sig, lx_sig;

	bzero(&l, sizeof (lx_sigset_t));

	for (sig = 1; sig < NSIG; sig++) {
		if (sigismember(s_sigsetp, sig) &&
		    ((lx_sig = stol_signo[sig]) > 0))
			lx_sigaddset(&l, lx_sig);
	}

	return ((uucopy(&l, lx_sigsetp, sizeof (lx_sigset_t)) != 0)
	    ? -errno : 0);
}

#if defined(_ILP32)
static int
ltos_osigset(lx_osigset_t *lx_osigsetp, sigset_t *s_sigsetp)
{
	lx_osigset_t lo;
	int lx_sig, sig;

	if (uucopy(lx_osigsetp, &lo, sizeof (lx_osigset_t)) != 0)
		return (-errno);

	(void) sigemptyset(s_sigsetp);

	for (lx_sig = 1; lx_sig <= OSIGSET_NBITS; lx_sig++)
		if ((lo & OSIGSET_BITSET(lx_sig)) &&
		    ((sig = ltos_signo[lx_sig]) > 0))
			(void) sigaddset(s_sigsetp, sig);

	return (0);
}

static int
stol_osigset(sigset_t *s_sigsetp, lx_osigset_t *lx_osigsetp)
{
	lx_osigset_t lo = 0;
	int lx_sig, sig;

	/*
	 * Note that an lx_osigset_t can only represent the signals from
	 * [1 .. OSIGSET_NBITS], so even though a signal may be present in the
	 * Illumos sigset_t, it may not be representable as a bit in the
	 * lx_osigset_t.
	 */
	for (sig = 1; sig < NSIG; sig++)
		if (sigismember(s_sigsetp, sig) &&
		    ((lx_sig = stol_signo[sig]) > 0) &&
		    (lx_sig <= OSIGSET_NBITS))
			lo |= OSIGSET_BITSET(lx_sig);

	return ((uucopy(&lo, lx_osigsetp, sizeof (lx_osigset_t)) != 0)
	    ? -errno : 0);
}
#endif

static int
ltos_sigcode(int si_code)
{
	switch (si_code) {
		case LX_SI_USER:
			return (SI_USER);
		case LX_SI_TKILL:
			return (SI_LWP);
		case LX_SI_QUEUE:
			return (SI_QUEUE);
		case LX_SI_TIMER:
			return (SI_TIMER);
		case LX_SI_ASYNCIO:
			return (SI_ASYNCIO);
		case LX_SI_MESGQ:
			return (SI_MESGQ);
		default:
			return (LX_SI_CODE_NOT_EXIST);
	}
}

int
stol_siginfo(siginfo_t *siginfop, lx_siginfo_t *lx_siginfop)
{
	int ret = 0;
	lx_siginfo_t lx_siginfo;

	bzero(&lx_siginfo, sizeof (*lx_siginfop));

	if ((lx_siginfo.lsi_signo = stol_signo[siginfop->si_signo]) <= 0) {
		/*
		 * Depending on the caller we may still need to get a usable
		 * converted siginfo struct.
		 */
		lx_siginfo.lsi_signo = LX_SIGKILL;
		errno = EINVAL;
		ret = -1;
	}

	lx_siginfo.lsi_code = lx_stol_sigcode(siginfop->si_code);
	lx_siginfo.lsi_errno = siginfop->si_errno;

	switch (lx_siginfo.lsi_signo) {
		/*
		 * Semantics ARE defined for SIGKILL, but since
		 * we can't catch it, we can't translate it. :-(
		 */
		case LX_SIGPOLL:
			lx_siginfo.lsi_band = siginfop->si_band;
			lx_siginfo.lsi_fd = siginfop->si_fd;
			break;

		case LX_SIGCHLD:
			lx_siginfo.lsi_pid = siginfop->si_pid;
			if (siginfop->si_code <= 0 || siginfop->si_code ==
			    CLD_EXITED) {
				lx_siginfo.lsi_status = siginfop->si_status;
			} else {
				lx_siginfo.lsi_status = lx_stol_status(
				    siginfop->si_status, -1);
			}
			lx_siginfo.lsi_utime =
			    HZ_TO_LX_USERHZ(siginfop->si_utime);
			lx_siginfo.lsi_stime =
			    HZ_TO_LX_USERHZ(siginfop->si_stime);
			break;

		case LX_SIGILL:
		case LX_SIGBUS:
		case LX_SIGFPE:
		case LX_SIGSEGV:
			lx_siginfo.lsi_addr = siginfop->si_addr;
			break;

		default:
			lx_siginfo.lsi_pid = siginfop->si_pid;
			lx_siginfo.lsi_uid =
			    LX_UID32_TO_UID16(siginfop->si_uid);
			lx_siginfo.lsi_value = siginfop->si_value;
			break;
	}

	if (uucopy(&lx_siginfo, lx_siginfop, sizeof (lx_siginfo_t)) != 0)
		return (-errno);
	return ((ret != 0) ? -errno : 0);
}

static void
stol_fpstate(fpregset_t *fpr, lx_fpstate_t *lfpr)
{
	size_t copy_len;

#if defined(_LP64)
	/*
	 * The 64-bit Illumos struct fpregset_t and lx_fpstate_t are identical
	 * so just bcopy() those entries (see usr/src/uts/intel/sys/regset.h
	 * for __amd64's struct fpu).
	 */
	copy_len = sizeof (fpr->fp_reg_set.fpchip_state);
	bcopy(fpr, lfpr, copy_len);

#else /* is _ILP32 */
	struct _fpstate *fpsp = (struct _fpstate *)fpr;

	/*
	 * The Illumos struct _fpstate and lx_fpstate_t are identical from the
	 * beginning of the structure to the lx_fpstate_t "magic" field, so
	 * just bcopy() those entries.
	 */
	copy_len = (size_t)&(((lx_fpstate_t *)0)->magic);
	bcopy(fpsp, lfpr, copy_len);

	/*
	 * These fields are all only significant for the first 16 bits.
	 */
	lfpr->cw &= 0xffff;		/* x87 control word */
	lfpr->tag &= 0xffff;		/* x87 tag word */
	lfpr->cssel &= 0xffff;		/* cs selector */
	lfpr->datasel &= 0xffff;	/* ds selector */

	/*
	 * Linux wants the x87 status word field to contain the value of the
	 * x87 saved exception status word.
	 */
	lfpr->sw = lfpr->status & 0xffff;	/* x87 status word */

	lfpr->mxcsr = fpsp->mxcsr;

	if (fpsp->mxcsr != 0) {
		/*
		 * Linux uses the "magic" field to denote whether the XMM
		 * registers contain legal data or not.  Since we can't get to
		 * %cr4 from userland to check the status of the OSFXSR bit,
		 * check the mxcsr field to see if it's 0, which it should
		 * never be on a system with the OXFXSR bit enabled.
		 */
		lfpr->magic = LX_X86_FXSR_MAGIC;
		bcopy(fpsp->xmm, lfpr->_xmm, sizeof (lfpr->_xmm));
	} else {
		lfpr->magic = LX_X86_FXSR_NONE;
	}
#endif
}

static void
ltos_fpstate(lx_fpstate_t *lfpr, fpregset_t *fpr)
{
	size_t copy_len;

#if defined(_LP64)
	/*
	 * The 64-bit Illumos struct fpregset_t and lx_fpstate_t are identical
	 * so just bcopy() those entries (see usr/src/uts/intel/sys/regset.h
	 * for __amd64's struct fpu).
	 */
	copy_len = sizeof (fpr->fp_reg_set.fpchip_state);
	bcopy(lfpr, fpr, copy_len);

#else /* is _ILP32 */
	struct _fpstate *fpsp = (struct _fpstate *)fpr;

	/*
	 * The lx_fpstate_t and Illumos struct _fpstate are identical from the
	 * beginning of the structure to the struct _fpstate "mxcsr" field, so
	 * just bcopy() those entries.
	 *
	 * Note that we do NOT have to propogate changes the user may have made
	 * to the "status" word back to the "sw" word, unlike the way we have
	 * to deal with processing the ESP and UESP register values on return
	 * from a signal handler.
	 */
	copy_len = (size_t)&(((struct _fpstate *)0)->mxcsr);
	bcopy(lfpr, fpsp, copy_len);

	/*
	 * These fields are all only significant for the first 16 bits.
	 */
	fpsp->cw &= 0xffff;		/* x87 control word */
	fpsp->sw &= 0xffff;		/* x87 status word */
	fpsp->tag &= 0xffff;		/* x87 tag word */
	fpsp->cssel &= 0xffff;		/* cs selector */
	fpsp->datasel &= 0xffff;	/* ds selector */
	fpsp->status &= 0xffff;		/* saved status */

	fpsp->mxcsr = lfpr->mxcsr;

	if (lfpr->magic == LX_X86_FXSR_MAGIC)
		bcopy(lfpr->_xmm, fpsp->xmm, sizeof (fpsp->xmm));
#endif
}

/*
 * We do not use the system sigaltstack() infrastructure as that would conflict
 * with our handling of both system call emulation and native signals on the
 * native stack.  Instead, we track the Linux stack structure in our
 * thread-specific data.  This function is modeled on the behaviour of the
 * native sigaltstack system call handler.
 */
long
lx_sigaltstack(uintptr_t ssp, uintptr_t oss)
{
	lx_tsd_t *lxtsd = lx_get_tsd();
	lx_stack_t ss;

	if (ssp != (uintptr_t)NULL) {
		if (lxtsd->lxtsd_sigaltstack.ss_flags & LX_SS_ONSTACK) {
			/*
			 * If we are currently using the installed alternate
			 * stack for signal handling, the user may not modify
			 * the stack for this thread.
			 */
			return (-EPERM);
		}

		if (uucopy((void *)ssp, &ss, sizeof (ss)) != 0) {
			return (-EFAULT);
		}

		if (ss.ss_flags & ~LX_SS_DISABLE) {
			/*
			 * The user may not specify a value for flags other
			 * than 0 or SS_DISABLE.
			 */
			return (-EINVAL);
		}

		if (!(ss.ss_flags & LX_SS_DISABLE) && ss.ss_size <
		    LX_MINSIGSTKSZ) {
			return (-ENOMEM);
		}

		if ((ss.ss_flags & LX_SS_DISABLE) != 0) {
			ss.ss_sp = NULL;
			ss.ss_size = 0;
		}
	}

	if (oss != (uintptr_t)NULL) {
		/*
		 * User provided old and new stack_t pointers may point to
		 * the same location.  Copy out before we modify.
		 */
		if (uucopy(&lxtsd->lxtsd_sigaltstack, (void *)oss,
		    sizeof (lxtsd->lxtsd_sigaltstack)) != 0) {
			return (-EFAULT);
		}
	}

	if (ssp != (uintptr_t)NULL) {
		lxtsd->lxtsd_sigaltstack = ss;
	}

	return (0);
}

#if defined(_ILP32)
/*
 * The following routines are needed because sigset_ts and siginfo_ts are
 * different in format between Linux and Illumos.
 *
 * Note that there are two different lx_sigset structures, lx_sigset_ts and
 * lx_osigset_ts:
 *
 *    + An lx_sigset_t is the equivalent of a Illumos sigset_t and supports
 *	more than 32 signals.
 *
 *    + An lx_osigset_t is simply a uint32_t, so it by definition only supports
 *	32 signals.
 *
 * When there are two versions of a routine, one prefixed with lx_rt_ and
 * one prefixed with lx_ alone, in GENERAL the lx_rt_ routines deal with
 * lx_sigset_ts while the lx_ routines deal with lx_osigset_ts.  Unfortunately,
 * this is not always the case (e.g. lx_sigreturn() vs. lx_rt_sigreturn())
 */
long
lx_sigpending(uintptr_t sigpend)
{
	sigset_t sigpendset;

	if (sigpending(&sigpendset) != 0)
		return (-errno);

	return (stol_osigset(&sigpendset, (lx_osigset_t *)sigpend));
}
#endif

long
lx_rt_sigpending(uintptr_t sigpend, uintptr_t setsize)
{
	sigset_t sigpendset;

	if ((size_t)setsize != sizeof (lx_sigset_t))
		return (-EINVAL);

	if (sigpending(&sigpendset) != 0)
		return (-errno);

	return (stol_sigset(&sigpendset, (lx_sigset_t *)sigpend));
}

/*
 * Create a common routine to encapsulate all of the sigprocmask code,
 * as the only difference between lx_sigprocmask() and lx_rt_sigprocmask()
 * is the usage of lx_osigset_ts vs. lx_sigset_ts, as toggled in the code by
 * the setting of the "sigset_type" flag.
 */
static int
lx_sigprocmask_common(uintptr_t how, uintptr_t l_setp, uintptr_t l_osetp,
    uintptr_t sigset_type)
{
	int err = 0;
	sigset_t set, oset;
	sigset_t *s_setp = NULL;
	sigset_t *s_osetp;

	if (l_setp) {
		switch (how) {
			case LX_SIG_BLOCK:
				how = SIG_BLOCK;
				break;

			case LX_SIG_UNBLOCK:
				how = SIG_UNBLOCK;
				break;

			case LX_SIG_SETMASK:
				how = SIG_SETMASK;
				break;

			default:
				return (-EINVAL);
		}

		s_setp = &set;

		/* Only 32-bit code passes other than USE_SIGSET */
		if (sigset_type == USE_SIGSET)
			err = ltos_sigset((lx_sigset_t *)l_setp, s_setp);
#if defined(_ILP32)
		else
			err = ltos_osigset((lx_osigset_t *)l_setp, s_setp);
#endif

		if (err != 0)
			return (err);

	}

	s_osetp = (l_osetp ? &oset : NULL);

	/*
	 * In a multithreaded environment, a call to sigprocmask(2) should
	 * only affect the current thread's signal mask so we don't need to
	 * explicitly call thr_sigsetmask(3C) here.
	 */
	if (sigprocmask(how, s_setp, s_osetp) != 0)
		return (-errno);

	if (l_osetp) {
		if (sigset_type == USE_SIGSET)
			err = stol_sigset(s_osetp, (lx_sigset_t *)l_osetp);
#if defined(_ILP32)
		else
			err = stol_osigset(s_osetp, (lx_osigset_t *)l_osetp);
#endif

		if (err != 0) {
			/*
			 * Encountered a fault while writing to the old signal
			 * mask buffer, so unwind the signal mask change made
			 * above.
			 */
			(void) sigprocmask(how, s_osetp, (sigset_t *)NULL);
			return (err);
		}
	}

	return (0);
}

#if defined(_ILP32)
long
lx_sigprocmask(uintptr_t how, uintptr_t setp, uintptr_t osetp)
{
	return (lx_sigprocmask_common(how, setp, osetp, USE_OSIGSET));
}
#endif

long
lx_rt_sigprocmask(uintptr_t how, uintptr_t setp, uintptr_t osetp,
    uintptr_t setsize)
{
	if ((size_t)setsize != sizeof (lx_sigset_t))
		return (-EINVAL);

	return (lx_sigprocmask_common(how, setp, osetp, USE_SIGSET));
}

#if defined(_ILP32)
long
lx_sigsuspend(uintptr_t set)
{
	sigset_t s_set;

	if (ltos_osigset((lx_osigset_t *)set, &s_set) != 0)
		return (-errno);

	return ((sigsuspend(&s_set) == -1) ? -errno : 0);
}
#endif

long
lx_rt_sigsuspend(uintptr_t set, uintptr_t setsize)
{
	sigset_t s_set;

	if ((size_t)setsize != sizeof (lx_sigset_t))
		return (-EINVAL);

	if (ltos_sigset((lx_sigset_t *)set, &s_set) != 0)
		return (-errno);

	return ((sigsuspend(&s_set) == -1) ? -errno : 0);
}

long
lx_rt_sigwaitinfo(uintptr_t set, uintptr_t sinfo, uintptr_t setsize)
{
	sigset_t s_set;
	siginfo_t s_sinfo, *s_sinfop;
	int rc;

	lx_sigset_t *setp = (lx_sigset_t *)set;
	lx_siginfo_t *sinfop = (lx_siginfo_t *)sinfo;

	if ((size_t)setsize != sizeof (lx_sigset_t))
		return (-EINVAL);

	if (ltos_sigset(setp, &s_set) != 0)
		return (-errno);

	s_sinfop = (sinfop == NULL) ? NULL : &s_sinfo;

	if ((rc = sigwaitinfo(&s_set, s_sinfop)) == -1)
		return (-errno);

	if (s_sinfop == NULL)
		return (stol_signo[rc]);

	return ((stol_siginfo(s_sinfop, sinfop) != 0)
	    ? -errno : stol_signo[rc]);
}

long
lx_rt_sigtimedwait(uintptr_t set, uintptr_t sinfo, uintptr_t toutp,
    uintptr_t setsize)
{
	sigset_t s_set;
	siginfo_t s_sinfo, *s_sinfop;
	int rc;

	lx_sigset_t *setp = (lx_sigset_t *)set;
	lx_siginfo_t *sinfop = (lx_siginfo_t *)sinfo;

	if ((size_t)setsize != sizeof (lx_sigset_t))
		return (-EINVAL);

	if (ltos_sigset(setp, &s_set) != 0)
		return (-errno);

	s_sinfop = (sinfop == NULL) ? NULL : &s_sinfo;

	/*
	 * "If timeout is the NULL pointer, the behavior is unspecified."
	 * Match what LTP expects.
	 */
	if ((rc = sigtimedwait(&s_set, s_sinfop,
	    (struct timespec *)toutp)) == -1)
		return (toutp == (uintptr_t)NULL ? -EINTR : -errno);

	if (s_sinfop == NULL)
		return (stol_signo[rc]);

	return ((stol_siginfo(s_sinfop, sinfop) != 0)
	    ? -errno : stol_signo[rc]);
}

static void
lx_sigreturn_find_native_context(const char *caller, ucontext_t **sigucp,
    ucontext_t **retucp, uintptr_t sp)
{
	lx_tsd_t *lxtsd = lx_get_tsd();
	lx_sigdeliver_frame_t *lxsdfp = (lx_sigdeliver_frame_t *)sp;
	lx_sigdeliver_frame_t lxsdf;
	boolean_t copy_ok;

	lx_debug("%s: reading lx_sigdeliver_frame_t @ %p\n", caller, lxsdfp);
	if (uucopy(lxsdfp, &lxsdf, sizeof (lxsdf)) != 0) {
		lx_debug("%s: failed to read lx_sigdeliver_frame_t @ %p\n",
		    lxsdfp);

		copy_ok = B_FALSE;
	} else {
		lx_debug("%s: lxsdf: magic %p retucp %p sigucp %p\n", caller,
		    lxsdf.lxsdf_magic, lxsdf.lxsdf_retucp, lxsdf.lxsdf_sigucp);

		copy_ok = B_TRUE;
	}

	/*
	 * lx_sigdeliver() pushes a lx_sigdeliver_frame_t onto the stack
	 * before it creates the struct lx_oldsigstack.
	 */
	if (copy_ok && lxsdf.lxsdf_magic == LX_SIGRT_MAGIC) {
		LX_SIGNAL_DELIVERY_FRAME_FOUND(lxsdfp);

		/*
		 * The guard value is intact; use the context pointers stored
		 * in the signal delivery frame:
		 */
		*sigucp = lxsdf.lxsdf_sigucp;
		*retucp = lxsdf.lxsdf_retucp;

		/*
		 * Ensure that the backup signal delivery chain is in sync with
		 * the frame we are returning via:
		 */
		lxtsd->lxtsd_sigbackup = lxsdf.lxsdf_sigbackup;
	} else {
		/*
		 * The guard value was not intact.  Either the program smashed
		 * the stack unintentionally, or worse: intentionally moved
		 * some parts of the signal delivery frame we constructed to
		 * another location before calling rt_sigreturn(2).
		 */
		LX_SIGNAL_DELIVERY_FRAME_CORRUPT(lxsdfp);

		if (lxtsd->lxtsd_sigbackup == NULL) {
			/*
			 * There was no backup context to use, so we must
			 * kill the process.
			 */
			if (copy_ok) {
				lx_err_fatal("%s: sp 0x%p, expected 0x%x, "
				    "found 0x%lx!", caller, (void *)sp,
				    LX_SIGRT_MAGIC,
				    (unsigned long)lxsdf.lxsdf_magic);
			} else {
				lx_err_fatal("%s: sp 0x%p, could not read "
				    "magic", caller, (void *)sp);
			}
		}

		/*
		 * Attempt to recover by using the backup signal delivery
		 * chain:
		 */
		lx_debug("%s: SIGRT_MAGIC not found @ sp %p; using backup "
		    "@ %p\n", caller, (void *)sp, lxtsd->lxtsd_sigbackup);
		*sigucp = lxtsd->lxtsd_sigbackup->lxsb_sigucp;
		*retucp = lxtsd->lxtsd_sigbackup->lxsb_retucp;
	}
}

#if defined(_ILP32)
/*
 * Intercept the Linux sigreturn() syscall to turn it into the return through
 * the libc call stack that Illumos expects.
 *
 * When control returns to libc's call_user_handler() routine, a setcontext(2)
 * will be done that returns thread execution to the point originally
 * interrupted by receipt of the signal.
 *
 * This is only used by 32-bit code.
 */
long
lx_sigreturn(void)
{
	struct lx_oldsigstack *lx_ossp;
	lx_sigset_t lx_sigset;
	ucontext_t *ucp;
	ucontext_t *sigucp;
	ucontext_t *retucp;
	uintptr_t sp;

	ucp = lx_syscall_regs();

	/*
	 * NOTE:  The sp saved in the context is eight bytes off of where we
	 *	  need it to be (either due to trampoline or the copying of
	 *	  sp = uesp, not clear which).
	 */
	sp = LX_REG(ucp, REG_SP) - 8;

	/*
	 * At this point, the stack pointer should point to the struct
	 * lx_oldsigstack that lx_build_old_signal_frame() constructed and
	 * placed on the stack.  We need to reference it a bit later, so
	 * save a pointer to it before incrementing our copy of the sp.
	 */
	lx_ossp = (struct lx_oldsigstack *)sp;
	sp += SA(sizeof (struct lx_oldsigstack));

	lx_sigreturn_find_native_context(__func__, &sigucp, &retucp, sp);

	/*
	 * We need to copy machine registers the Linux signal handler may have
	 * modified back to the Illumos ucontext_t.
	 *
	 * General registers copy across as-is, except Linux expects that
	 * changes made to uc_mcontext.gregs[ESP] will be reflected when the
	 * interrupted thread resumes execution after the signal handler. To
	 * emulate this behavior, we must modify uc_mcontext.gregs[UESP] to
	 * match uc_mcontext.gregs[ESP] as Illumos will restore the UESP
	 * value to ESP.
	 */
	lx_ossp->sigc.sc_esp_at_signal = lx_ossp->sigc.sc_esp;
	bcopy(&lx_ossp->sigc, &sigucp->uc_mcontext, sizeof (gregset_t));

	LX_SIGRETURN(NULL, sigucp, sp);

	/* copy back FP regs if present */
	if (lx_ossp->sigc.sc_fpstate != NULL)
		ltos_fpstate(&lx_ossp->fpstate, &sigucp->uc_mcontext.fpregs);

	/* convert Linux signal mask back to its Illumos equivalent */
	bzero(&lx_sigset, sizeof (lx_sigset_t));
	lx_sigset.__bits[0] = lx_ossp->sigc.sc_mask;
	lx_sigset.__bits[1] = lx_ossp->sig_extra;
	(void) ltos_sigset(&lx_sigset, &sigucp->uc_sigmask);

	/*
	 * For signal mask handling to be done properly, this call needs to
	 * return to the libc routine that originally called the signal handler
	 * rather than directly set the context back to the place the signal
	 * interrupted execution as the original Linux code would do.
	 */
	lx_debug("lx_sigreturn: calling setcontext; retucp %p flags %lx "
	    "link %p\n", retucp, retucp->uc_flags, retucp->uc_link);
	(void) setcontext(retucp);
	assert(0);

	/*NOTREACHED*/
	return (0);
}
#endif

/*
 * This signal return syscall is used by both 32-bit and 64-bit code.
 */
long
lx_rt_sigreturn(void)
{
	struct lx_sigstack *lx_ssp;
	lx_ucontext_t *lx_ucp;
	ucontext_t *ucp;
	ucontext_t *sigucp;
	ucontext_t *retucp;
	uintptr_t sp;

	/*
	 * Since we don't take the normal return path from this syscall, we
	 * inform the kernel that we're returning, for the sake of ptrace.
	 */
	(void) syscall(SYS_brand, B_PTRACE_SIG_RETURN);

	/* Get the registers at the emulated Linux rt_sigreturn syscall */
	ucp = lx_syscall_regs();

#if defined(_ILP32)
	lx_debug("lx_rt_sigreturn: ESP %p UESP %p\n", LX_REG(ucp, ESP),
	    LX_REG(ucp, UESP));
	/*
	 * For 32-bit
	 *
	 * NOTE:  Because of the silly compatibility measures done in the
	 *	  signal trampoline code to make sure the stack holds the
	 *	   _exact same_  instruction sequence Linux does, we have to
	 *	  manually "pop" some extra instructions off the stack here
	 *	  before passing the stack address to the syscall because the
	 *	  trampoline code isn't allowed to do it due to the gdb
	 *	  compatability issues.
	 *
	 *	  No, I'm not kidding.
	 *
	 *	  The sp saved in the context is eight bytes off of where we
	 *	  need it to be (either due to trampoline or the copying of
	 *	  sp = uesp, not clear which but looks like the uesp case), so
	 *	  the need to pop the extra four byte instruction means we need
	 *	  to subtract  a net four bytes from the sp before "popping" the
	 *	  struct lx_sigstack off the stack.
	 *
	 *	  This will yield the value the stack pointer had before
	 *	  lx_sigdeliver() created the stack frame for the Linux signal
	 *	  handler.
	 */
	sp = (uintptr_t)LX_REG(ucp, REG_SP) - 4;
#else
	/*
	 * We need to make an adjustment for 64-bit code as well. Since 64-bit
	 * does not use the trampoline, it's probably for the same reason as
	 * alluded to above.
	 */
	sp = (uintptr_t)LX_REG(ucp, REG_SP) - 8;
#endif

	/*
	 * At this point, the stack pointer should point to the struct
	 * lx_sigstack that lx_build_signal_frame() constructed and
	 * placed on the stack.  We need to reference it a bit later, so
	 * save a pointer to it before incrementing our copy of the sp.
	 */
	lx_ssp = (struct lx_sigstack *)sp;
	sp += SA(sizeof (struct lx_sigstack));

#if defined(_LP64)
	/*
	 * The 64-bit lx_sigdeliver() inserts 8 bytes of padding between
	 * the lx_sigstack_t and the delivery frame to maintain ABI stack
	 * alignment.
	 */
	sp += 8;
#endif

	lx_sigreturn_find_native_context(__func__, &sigucp, &retucp, sp);

	/*
	 * We need to copy machine registers the Linux signal handler may have
	 * modified back to the Illumos version.
	 */
#if defined(_LP64)
	lx_ucp = &lx_ssp->uc;

	/*
	 * General register layout is completely different.
	 */
	LX_REG(sigucp, REG_R15) = lx_ucp->uc_sigcontext.sc_r15;
	LX_REG(sigucp, REG_R14) = lx_ucp->uc_sigcontext.sc_r14;
	LX_REG(sigucp, REG_R13) = lx_ucp->uc_sigcontext.sc_r13;
	LX_REG(sigucp, REG_R12) = lx_ucp->uc_sigcontext.sc_r12;
	LX_REG(sigucp, REG_R11) = lx_ucp->uc_sigcontext.sc_r11;
	LX_REG(sigucp, REG_R10) = lx_ucp->uc_sigcontext.sc_r10;
	LX_REG(sigucp, REG_R9) = lx_ucp->uc_sigcontext.sc_r9;
	LX_REG(sigucp, REG_R8) = lx_ucp->uc_sigcontext.sc_r8;
	LX_REG(sigucp, REG_RDI) = lx_ucp->uc_sigcontext.sc_rdi;
	LX_REG(sigucp, REG_RSI) = lx_ucp->uc_sigcontext.sc_rsi;
	LX_REG(sigucp, REG_RBP) = lx_ucp->uc_sigcontext.sc_rbp;
	LX_REG(sigucp, REG_RBX) = lx_ucp->uc_sigcontext.sc_rbx;
	LX_REG(sigucp, REG_RDX) = lx_ucp->uc_sigcontext.sc_rdx;
	LX_REG(sigucp, REG_RCX) = lx_ucp->uc_sigcontext.sc_rcx;
	LX_REG(sigucp, REG_RAX) = lx_ucp->uc_sigcontext.sc_rax;
	LX_REG(sigucp, REG_TRAPNO) = lx_ucp->uc_sigcontext.sc_trapno;
	LX_REG(sigucp, REG_ERR) = lx_ucp->uc_sigcontext.sc_err;
	LX_REG(sigucp, REG_RIP) = lx_ucp->uc_sigcontext.sc_rip;
	LX_REG(sigucp, REG_CS) = lx_ucp->uc_sigcontext.sc_cs;
	LX_REG(sigucp, REG_RFL) = lx_ucp->uc_sigcontext.sc_eflags;
	LX_REG(sigucp, REG_RSP) = lx_ucp->uc_sigcontext.sc_rsp;
	LX_REG(sigucp, REG_SS) = lx_ucp->uc_sigcontext.sc_pad0;
	LX_REG(sigucp, REG_FS) = lx_ucp->uc_sigcontext.sc_fs;
	LX_REG(sigucp, REG_GS) = lx_ucp->uc_sigcontext.sc_gs;

#else /* is _ILP32 */
	lx_ucp = &lx_ssp->uc;

	/*
	 * Illumos and Linux both follow the SysV i386 ABI layout for the
	 * mcontext.
	 *
	 * General registers copy across as-is, except Linux expects that
	 * changes made to uc_mcontext.gregs[ESP] will be reflected when the
	 * interrupted thread resumes execution after the signal handler. To
	 * emulate this behavior, we must modify uc_mcontext.gregs[UESP] to
	 * match uc_mcontext.gregs[ESP] as Illumos will restore the UESP value
	 * to ESP.
	 */
	lx_ucp->uc_sigcontext.sc_esp_at_signal = lx_ucp->uc_sigcontext.sc_esp;

	bcopy(&lx_ucp->uc_sigcontext, &sigucp->uc_mcontext.gregs,
	    sizeof (gregset_t));
#endif

	LX_SIGRETURN(lx_ucp, sigucp, sp);

	if (lx_ucp->uc_sigcontext.sc_fpstate != NULL) {
		ltos_fpstate(lx_ucp->uc_sigcontext.sc_fpstate,
		    &sigucp->uc_mcontext.fpregs);
	}

	/*
	 * Convert the Linux signal mask and stack back to their
	 * Illumos equivalents.
	 */
	(void) ltos_sigset(&lx_ucp->uc_sigmask, &sigucp->uc_sigmask);
	ltos_stack(&lx_ucp->uc_stack, &sigucp->uc_stack);

	/*
	 * For signal mask handling to be done properly, this call needs to
	 * return to the libc routine that originally called the signal handler
	 * rather than directly set the context back to the place the signal
	 * interrupted execution as the original Linux code would do.
	 */
	lx_debug("lx_rt_sigreturn: calling setcontext; retucp %p\n", retucp);
	(void) setcontext(retucp);
	assert(0);

	/*NOTREACHED*/
	return (0);
}


#if defined(_ILP32)
/*
 * Build signal frame for processing for "old" (legacy) Linux signals
 * This stack-builder function is only used by 32-bit code.
 */
/* ARGSUSED4 */
static void
lx_build_old_signal_frame(int lx_sig, siginfo_t *sip, void *p, void *sp,
    uintptr_t *hargs)
{
	extern void lx_sigreturn_tramp();

	lx_sigset_t lx_sigset;
	ucontext_t *ucp = (ucontext_t *)p;
	struct lx_sigaction *lxsap;
	struct lx_oldsigstack *lx_ossp = sp;

	lx_debug("building old signal frame for lx sig %d at 0x%p", lx_sig, sp);

	lx_ossp->sig = lx_sig;
	lxsap = &lx_sighandlers.lx_sa[lx_sig];
	lx_debug("lxsap @ 0x%p", lxsap);

	if (lxsap && (lxsap->lxsa_flags & LX_SA_RESTORER) &&
	    lxsap->lxsa_restorer) {
		lx_ossp->retaddr = lxsap->lxsa_restorer;
		lx_debug("lxsa_restorer exists @ 0x%p", lx_ossp->retaddr);
	} else {
		lx_ossp->retaddr = lx_sigreturn_tramp;
		lx_debug("lx_ossp->retaddr set to 0x%p", lx_sigreturn_tramp);
	}

	lx_debug("osf retaddr = 0x%p", lx_ossp->retaddr);

	/* convert Illumos signal mask and stack to their Linux equivalents */
	(void) stol_sigset(&ucp->uc_sigmask, &lx_sigset);
	lx_ossp->sigc.sc_mask = lx_sigset.__bits[0];
	lx_ossp->sig_extra = lx_sigset.__bits[1];

	/*
	 * General registers copy across as-is, except Linux expects that
	 * uc_mcontext.gregs[ESP] == uc_mcontext.gregs[UESP] on receipt of a
	 * signal.
	 */
	bcopy(&ucp->uc_mcontext, &lx_ossp->sigc, sizeof (gregset_t));
	lx_ossp->sigc.sc_esp = lx_ossp->sigc.sc_esp_at_signal;

	/*
	 * cr2 contains the faulting address, and Linux only sets cr2 for a
	 * a segmentation fault.
	 */
	lx_ossp->sigc.sc_cr2 = (((lx_sig == LX_SIGSEGV) && (sip)) ?
	    (uintptr_t)sip->si_addr : 0);

	/* convert FP regs if present */
	if (ucp->uc_flags & UC_FPU) {
		stol_fpstate(&ucp->uc_mcontext.fpregs, &lx_ossp->fpstate);
		lx_ossp->sigc.sc_fpstate = &lx_ossp->fpstate;
	} else {
		lx_ossp->sigc.sc_fpstate = NULL;
	}

	/*
	 * Believe it or not, gdb wants to SEE the trampoline code on the
	 * bottom of the stack to determine whether the stack frame belongs to
	 * a signal handler, even though this code is no longer actually
	 * called.
	 *
	 * You can't make this stuff up.
	 */
	bcopy((void *)lx_sigreturn_tramp, lx_ossp->trampoline,
	    sizeof (lx_ossp->trampoline));
}
#endif

/*
 * Build stack frame (32-bit) or stack local data (64-bit) for processing for
 * modern Linux signals. This is the only stack-builder function for 64-bit
 * code (32-bit code also calls this when using "modern" signals).
 */
/* ARGSUSED4 */
static void
lx_build_signal_frame(int lx_sig, siginfo_t *sip, void *p, void *sp,
    uintptr_t *hargs)
{
	extern void lx_rt_sigreturn_tramp();

	lx_ucontext_t *lx_ucp;
	ucontext_t *ucp = (ucontext_t *)p;
	struct lx_sigstack *lx_ssp = sp;
	struct lx_sigaction *lxsap;

	lx_debug("building signal frame for lx sig %d at 0x%p", lx_sig, sp);

	lx_ucp = &lx_ssp->uc;
#if defined(_ILP32)
	/*
	 * Arguments are passed to the 32-bit signal handler on the stack.
	 */
	lx_ssp->ucp = lx_ucp;
	lx_ssp->sip = sip != NULL ? &lx_ssp->si : NULL;
	lx_ssp->sig = lx_sig;
#else
	/*
	 * Arguments to the 64-bit signal handler are passed in registers:
	 *   hdlr(int sig, siginfo_t *sip, void *ucp);
	 */
	hargs[0] = lx_sig;
	hargs[1] = sip != NULL ? (uintptr_t)&lx_ssp->si : (uintptr_t)NULL;
	hargs[2] = (uintptr_t)lx_ucp;
#endif

	lxsap = &lx_sighandlers.lx_sa[lx_sig];
	lx_debug("lxsap @ 0x%p", lxsap);

	if (lxsap && (lxsap->lxsa_flags & LX_SA_RESTORER) &&
	    lxsap->lxsa_restorer) {
		/*
		 * lxsa_restorer is explicitly set by sigaction in 32-bit code
		 * but it can also be implicitly set for both 32 and 64 bit
		 * code via lx_sigaction_common when we bcopy the user-supplied
		 * lx_sigaction element into the proper slot in the sighandler
		 * array.
		 */
		lx_ssp->retaddr = lxsap->lxsa_restorer;
		lx_debug("lxsa_restorer exists @ 0x%p", lx_ssp->retaddr);
	} else {
		lx_ssp->retaddr = lx_rt_sigreturn_tramp;
		lx_debug("lx_ssp->retaddr set to 0x%p", lx_rt_sigreturn_tramp);
	}

	/* Linux has these fields but always clears them to 0 */
	lx_ucp->uc_flags = 0;
	lx_ucp->uc_link = NULL;

	/* convert Illumos signal mask and stack to their Linux equivalents */
	(void) stol_sigset(&ucp->uc_sigmask, &lx_ucp->uc_sigmask);
	stol_stack(&ucp->uc_stack, &lx_ucp->uc_stack);

#if defined(_LP64)
	/*
	 * General register layout is completely different.
	 */
	lx_ucp->uc_sigcontext.sc_r8 = LX_REG(ucp, REG_R8);
	lx_ucp->uc_sigcontext.sc_r9 = LX_REG(ucp, REG_R9);
	lx_ucp->uc_sigcontext.sc_r10 = LX_REG(ucp, REG_R10);
	lx_ucp->uc_sigcontext.sc_r11 = LX_REG(ucp, REG_R11);
	lx_ucp->uc_sigcontext.sc_r12 = LX_REG(ucp, REG_R12);
	lx_ucp->uc_sigcontext.sc_r13 = LX_REG(ucp, REG_R13);
	lx_ucp->uc_sigcontext.sc_r14 = LX_REG(ucp, REG_R14);
	lx_ucp->uc_sigcontext.sc_r15 = LX_REG(ucp, REG_R15);
	lx_ucp->uc_sigcontext.sc_rdi = LX_REG(ucp, REG_RDI);
	lx_ucp->uc_sigcontext.sc_rsi = LX_REG(ucp, REG_RSI);
	lx_ucp->uc_sigcontext.sc_rbp = LX_REG(ucp, REG_RBP);
	lx_ucp->uc_sigcontext.sc_rbx = LX_REG(ucp, REG_RBX);
	lx_ucp->uc_sigcontext.sc_rdx = LX_REG(ucp, REG_RDX);
	lx_ucp->uc_sigcontext.sc_rax = LX_REG(ucp, REG_RAX);
	lx_ucp->uc_sigcontext.sc_rcx = LX_REG(ucp, REG_RCX);
	lx_ucp->uc_sigcontext.sc_rsp = LX_REG(ucp, REG_RSP);
	lx_ucp->uc_sigcontext.sc_rip = LX_REG(ucp, REG_RIP);
	lx_ucp->uc_sigcontext.sc_eflags = LX_REG(ucp, REG_RFL);
	lx_ucp->uc_sigcontext.sc_cs = LX_REG(ucp, REG_CS);
	lx_ucp->uc_sigcontext.sc_gs = LX_REG(ucp, REG_GS);
	lx_ucp->uc_sigcontext.sc_fs = LX_REG(ucp, REG_FS);
	lx_ucp->uc_sigcontext.sc_pad0 = LX_REG(ucp, REG_SS);
	lx_ucp->uc_sigcontext.sc_err = LX_REG(ucp, REG_ERR);
	lx_ucp->uc_sigcontext.sc_trapno = LX_REG(ucp, REG_TRAPNO);

#else /* is _ILP32 */
	/*
	 * General registers copy across as-is, except Linux expects that
	 * uc_mcontext.gregs[ESP] == uc_mcontext.gregs[UESP] on receipt of a
	 * signal.
	 */
	bcopy(&ucp->uc_mcontext, &lx_ucp->uc_sigcontext, sizeof (gregset_t));
	lx_ucp->uc_sigcontext.sc_esp = lx_ucp->uc_sigcontext.sc_esp_at_signal;
#endif

	/*
	 * cr2 contains the faulting address, which Linux only sets for a
	 * a segmentation fault.
	 */
	lx_ucp->uc_sigcontext.sc_cr2 = ((lx_sig == LX_SIGSEGV) && (sip)) ?
	    (uintptr_t)sip->si_addr : 0;

	/*
	 * This should only return an error if the signum is invalid but that
	 * also gets converted into a LX_SIGKILL by this function.
	 */
	if (sip != NULL)
		(void) stol_siginfo(sip, &lx_ssp->si);
	else
		bzero(&lx_ssp->si, sizeof (lx_siginfo_t));

	/* convert FP regs if present */
	if (ucp->uc_flags & UC_FPU) {
		/*
		 * Copy FP regs to the appropriate place in the the lx_sigstack
		 * structure.
		 */
		stol_fpstate(&ucp->uc_mcontext.fpregs, &lx_ssp->fpstate);
		lx_ucp->uc_sigcontext.sc_fpstate = &lx_ssp->fpstate;
	} else {
		lx_ucp->uc_sigcontext.sc_fpstate = NULL;
	}

#if defined(_ILP32)
	/*
	 * Believe it or not, gdb wants to SEE the sigreturn code on the
	 * top of the stack to determine whether the stack frame belongs to
	 * a signal handler, even though this code is not actually called.
	 *
	 * You can't make this stuff up.
	 */
	bcopy((void *)lx_rt_sigreturn_tramp, lx_ssp->trampoline,
	    sizeof (lx_ssp->trampoline));
#endif
}

/*
 * This is the interposition handler for Linux signals.
 */
static void
lx_call_user_handler(int sig, siginfo_t *sip, void *p)
{
	void (*user_handler)();
	void (*stk_builder)();
	volatile struct lx_sigaction *lxsap;
	ucontext_t *ucp = (ucontext_t *)p;
	size_t stksize;
	int lx_sig;

	/*
	 * If Illumos signal has no Linux equivalent, effectively ignore it.
	 */
	if ((lx_sig = stol_signo[sig]) == -1) {
		lx_unsupported("caught Illumos signal %d, no Linux equivalent",
		    sig);
		return;
	}

	lx_debug("interpose caught Illumos signal %d, translating to Linux "
	    "signal %d", sig, lx_sig);

	lxsap = &lx_sighandlers.lx_sa[lx_sig];
	lx_debug("lxsap @ 0x%p", lxsap);

	if ((sig == SIGPWR) && (lxsap->lxsa_handler == SIG_DFL)) {
		/*
		 * Linux SIG_DFL for SIGPWR is to terminate. The lx wait
		 * emulation will translate SIGPWR to LX_SIGPWR.
		 */
		(void) syscall(SYS_brand, B_EXIT_AS_SIG, SIGPWR);
		/* This should never return */
		assert(0);
	}

	while (lxsap->lxsa_handler == SIG_DFL ||
	    lxsap->lxsa_handler == SIG_IGN) {
		/*
		 * This normally shouldn't be possible, but there is a window
		 * in which a vfork()'d process can have its signal disposition
		 * corrupted by its child.  While this window is narrowed by
		 * blocking all signals in the brand, that leaves a (smaller)
		 * window whereby a signal in flight is delivered before the
		 * brand has blocked them.  To detect this case, we will spin
		 * if our signal disposition is impossible and all signals are
		 * blocked due to vfork() activity:  we know that the vfork()'d
		 * child will eventually restore the signal disposition before
		 * it unblocks signals, allowing us to proceed.
		 */
		if (lx_all_signals_blocked())
			continue;

		if (lxsap->lxsa_handler != SIG_DFL &&
		    lxsap->lxsa_handler != SIG_IGN)
			break;

		lx_err_fatal("lxsa_handler set to %s?  How?!?!?",
		    (lxsap->lxsa_handler == SIG_DFL) ? "SIG_DFL" : "SIG_IGN");
	}

#if defined(_LP64)
	stksize = sizeof (struct lx_sigstack);
	stk_builder = lx_build_signal_frame;
#else
	if (lxsap->lxsa_flags & LX_SA_SIGINFO) {
		stksize = sizeof (struct lx_sigstack);
		stk_builder = lx_build_signal_frame;
	} else  {
		stksize = sizeof (struct lx_oldsigstack);
		stk_builder = lx_build_old_signal_frame;
	}
#endif

	user_handler = lxsap->lxsa_handler;

	lx_debug("delivering %d (lx %d) to handler at 0x%p", sig, lx_sig,
	    lxsap->lxsa_handler);

	if (lxsap->lxsa_flags & LX_SA_RESETHAND)
		lxsap->lxsa_handler = SIG_DFL;

	lx_sigdeliver(lx_sig, sip, ucp, stksize, stk_builder, user_handler,
	    (struct lx_sigaction *)lxsap);

	/*
	 * We need to handle restarting system calls if requested by the
	 * program for this signal type:
	 */
	if (lxsap->lxsa_flags & LX_SA_RESTART) {
		uintptr_t flags = (uintptr_t)ucp->uc_brand_data[0];
		long ret = (long)LX_REG(ucp, REG_R0);
		boolean_t interrupted = (ret == -lx_errno(EINTR, -1));

		/*
		 * If the system call returned EINTR, and the system
		 * call handler set "br_syscall_restart" when returning,
		 * we modify the context to try the system call again
		 * when we return from this signal handler.
		 */
		if ((flags & LX_UC_RESTART_SYSCALL) && interrupted) {
			int syscall_num = (int)(uintptr_t)ucp->uc_brand_data[2];

			lx_debug("restarting interrupted system call %d",
			    syscall_num);

			/*
			 * Both the "int 0x80" and the "syscall" instruction
			 * are two bytes long.  Wind the program counter back
			 * to the start of this instruction.
			 *
			 * The system call we interrupted is preserved in the
			 * brand-specific data in the ucontext_t when the
			 * LX_UC_RESTART_SYSCALL flag is set.  This is
			 * analogous to the "orig_[er]ax" field in the Linux
			 * "user_regs_struct".
			 */
			LX_REG(ucp, REG_PC) -= 2;
			LX_REG(ucp, REG_R0) = syscall_num;
		}
	}
}

/*
 * The "lx_sigdeliver()" function is responsible for constructing the emulated
 * signal delivery frame on the brand stack for this LWP.  A context is saved
 * on the stack which will be used by the "sigreturn(2)" family of emulated
 * system calls to get us back here after the Linux signal handler returns.
 * This function is modelled on the in-kernel "sendsig()" signal delivery
 * mechanism.
 */
void
lx_sigdeliver(int lx_sig, siginfo_t *sip, ucontext_t *ucp, size_t stacksz,
    void (*stack_builder)(), void (*user_handler)(),
    struct lx_sigaction *lxsap)
{
	lx_sigbackup_t sigbackup;
	ucontext_t uc;
	lx_tsd_t *lxtsd = lx_get_tsd();
	int totsz = 0;
	uintptr_t flags;
	uintptr_t hargs[3];
	uintptr_t orig_sp = 0;

	/*
	 * These variables must be "volatile", as they are modified after the
	 * getcontext() stores the register state:
	 */
	volatile boolean_t signal_delivered = B_FALSE;
	volatile boolean_t sp_modified = B_FALSE;
	volatile uintptr_t lxfp = 0;
	volatile uintptr_t old_tsd_sp = 0;
	volatile int newstack = 0;

	/*
	 * This function involves modifying the Linux process stack for this
	 * thread.  To do so without corruption requires us to exclude other
	 * signal handlers (or emulated system calls called from within those
	 * handlers) from running while we reserve space on that stack.  We
	 * defer the execution of further instances of lx_call_user_handler()
	 * until we have completed this operation.
	 */
	_sigoff();

	/*
	 * Clear register arguments vector.
	 */
	bzero(hargs, sizeof (hargs));

	/* Save our SP so we can restore it after coming back in. */
	orig_sp = LX_REG(ucp, REG_SP);

	/*
	 * We save a context here so that we can be returned later to complete
	 * handling the signal.
	 */
	lx_debug("lx_sigdeliver: STORING RETURN CONTEXT @ %p\n", &uc);
	assert(getcontext(&uc) == 0);
	lx_debug("lx_sigdeliver: RETURN CONTEXT %p LINK %p FLAGS %lx\n",
	    &uc, uc.uc_link, uc.uc_flags);
	if (signal_delivered) {
		/*
		 * If the "signal_delivered" flag is set, we are returned here
		 * via setcontext() as called by the emulated Linux signal
		 * return system call.
		 */
		lx_debug("lx_sigdeliver: WE ARE BACK, VIA UC @ %p!\n", &uc);

		if (sp_modified) {
			/*
			 * Restore the original stack pointer, which we saved
			 * on our alt. stack, back into the context.
			 */
			LX_REG(ucp, REG_SP) = orig_sp;
		}

		goto after_signal_handler;
	}
	signal_delivered = B_TRUE;

	/*
	 * Preserve the current tsd value of the Linux process stack pointer,
	 * even if it is zero.  We will restore it when we are returned here
	 * via setcontext() after the Linux process has completed execution of
	 * its signal handler.
	 */
	old_tsd_sp = lxtsd->lxtsd_lx_sp;

	/*
	 * Figure out whether we will be handling this signal on an alternate
	 * stack specified by the user.
	 */
	newstack = (lxsap->lxsa_flags & LX_SA_ONSTACK) &&
	    !(lxtsd->lxtsd_sigaltstack.ss_flags & (LX_SS_ONSTACK |
	    LX_SS_DISABLE));

	/*
	 * Find the first unused region of the Linux process stack, where
	 * we will assemble our signal delivery frame.
	 */
	flags = (uintptr_t)ucp->uc_brand_data[0];
	if (newstack) {
		/*
		 * We are moving to the user-provided alternate signal
		 * stack.
		 */
		lxfp = SA((uintptr_t)lxtsd->lxtsd_sigaltstack.ss_sp) +
		    SA(lxtsd->lxtsd_sigaltstack.ss_size) - STACK_ALIGN;
		lx_debug("lx_sigdeliver: moving to ALTSTACK sp %p\n", lxfp);
		LX_SIGNAL_ALTSTACK_ENABLE(lxfp);
	} else if (flags & LX_UC_STACK_BRAND) {
		/*
		 * We interrupted the Linux process to take this signal.  The
		 * stack pointer is the one saved in this context.
		 */
		lxfp = LX_REG(ucp, REG_SP);
	} else {
		/*
		 * We interrupted a native (emulation) routine, so we must get
		 * the current stack pointer from either the tsd (if one is
		 * stored there) or via the context chain.
		 *
		 */
		lxfp = lx_find_brand_sp();
		if (lxtsd->lxtsd_lx_sp != 0) {
			/*
			 * We must also make room for the possibility of nested
			 * signal delivery -- we may be pre-empting the
			 * in-progress handling of another signal.
			 *
			 * Note that if we were already on the alternate stack,
			 * any emulated Linux system calls would be betwixt
			 * that original signal frame and this new one on the
			 * one contiguous stack, so this logic holds either
			 * way:
			 */
			lxfp = MIN(lxtsd->lxtsd_lx_sp, lxfp);
		}

		/* Replace the context SP with the one from the Linux context */
		LX_REG(ucp, REG_SP) = lxfp;
		sp_modified = B_TRUE;
	}

	/*
	 * Account for a reserved stack region (for amd64, this is 128 bytes),
	 * and align the stack:
	 */
	lxfp -= STACK_RESERVE;
	lxfp &= ~(STACK_ALIGN - 1);

	/*
	 * Allocate space on the Linux process stack for our delivery frame,
	 * including:
	 *
	 *   ----------------------------------------------------- old %sp
	 *   - lx_sigdeliver_frame_t
	 *   - (ucontext_t pointers and stack magic)
	 *   -----------------------------------------------------
	 *   - (amd64-only 8-byte alignment gap)
	 *   -----------------------------------------------------
	 *   - frame of size "stacksz" from the stack builder
	 *   ----------------------------------------------------- new %sp
	 */
#if defined(_LP64)
	/*
	 * The AMD64 ABI requires us to align the stack such that when the
	 * called function pushes the base pointer, the stack is 16 byte
	 * aligned.  The stack must, therefore, be 8- but _not_ 16-byte
	 * aligned.
	 */
#if (STACK_ALIGN != 16) || (STACK_ENTRY_ALIGN != 8)
#error "lx_sigdeliver() did not find expected stack alignment"
#endif
	totsz = SA(sizeof (lx_sigdeliver_frame_t)) + SA(stacksz) + 8;
	assert((totsz & (STACK_ENTRY_ALIGN - 1)) == 0);
	assert((totsz & (STACK_ALIGN - 1)) == 8);
#else
	totsz = SA(sizeof (lx_sigdeliver_frame_t)) + SA(stacksz);
	assert((totsz & (STACK_ALIGN - 1)) == 0);
#endif

	/*
	 * Copy our return frame into place:
	 */
	lxfp -= SA(sizeof (lx_sigdeliver_frame_t));
	lx_debug("lx_sigdeliver: lx_sigdeliver_frame_t @ %p\n", lxfp);
	{
		lx_sigdeliver_frame_t frm;

		frm.lxsdf_magic = LX_SIGRT_MAGIC;
		frm.lxsdf_retucp = &uc;
		frm.lxsdf_sigucp = ucp;
		frm.lxsdf_sigbackup = &sigbackup;

		lx_debug("lx_sigdeliver: retucp %p sigucp %p\n",
		    frm.lxsdf_retucp, frm.lxsdf_sigucp);

		if (uucopy(&frm, (void *)lxfp, sizeof (frm)) != 0) {
			/*
			 * We could not modify the stack of the emulated Linux
			 * program.  Act like the kernel and terminate the
			 * program with a segmentation violation.
			 */
			(void) syscall(SYS_brand, B_EXIT_AS_SIG, SIGSEGV);
		}

		LX_SIGNAL_DELIVERY_FRAME_CREATE((void *)lxfp);

		/*
		 * Populate a backup copy of signal linkage to use in case
		 * the Linux program completely destroys (or relocates) the
		 * delivery frame.
		 *
		 * This is necessary for programs that have flown so far off
		 * the architectural rails that they believe it is
		 * acceptable to make assumptions about the precise size and
		 * layout of the signal handling frame assembled by the
		 * kernel.
		 */
		sigbackup.lxsb_retucp = frm.lxsdf_retucp;
		sigbackup.lxsb_sigucp = frm.lxsdf_sigucp;
		sigbackup.lxsb_sigdeliver_frame = lxfp;
		sigbackup.lxsb_previous = lxtsd->lxtsd_sigbackup;
		lxtsd->lxtsd_sigbackup = &sigbackup;

		lx_debug("lx_sigdeliver: installed sigbackup %p; prev %p\n",
		    &sigbackup, sigbackup.lxsb_previous);
	}

	/*
	 * Build the Linux signal handling frame:
	 */
#if defined(_LP64)
	lxfp -= SA(stacksz) + 8;
#else
	lxfp -= SA(stacksz);
#endif
	lx_debug("lx_sigdeliver: Linux sig frame @ %p\n", lxfp);
	stack_builder(lx_sig, sip, ucp, lxfp, hargs);

	/*
	 * Record our reservation so that any nested signal handlers
	 * can see it.
	 */
	lx_debug("lx_sigdeliver: Linux tsd sp %p -> %p\n", lxtsd->lxtsd_lx_sp,
	    lxfp);
	lxtsd->lxtsd_lx_sp = lxfp;

	if (newstack) {
		lxtsd->lxtsd_sigaltstack.ss_flags |= LX_SS_ONSTACK;
	}

	LX_SIGDELIVER(lx_sig, lxsap, (void *)lxfp);

	/*
	 * Re-enable signal delivery.  If a signal was queued while we were
	 * in the critical section, it will be delivered immediately.
	 */
	_sigon();

	/*
	 * Pass control to the Linux signal handler:
	 */
	lx_debug("lx_sigdeliver: JUMPING TO LINUX (sig %d sp %p eip %p)\n",
	    lx_sig, lxfp, user_handler);
	{
		ucontext_t jump_uc;

		bcopy(lx_find_brand_uc(), &jump_uc, sizeof (jump_uc));

		/*
		 * We want to load the general registers from this context, and
		 * switch to the BRAND stack.  We do _not_ want to restore the
		 * uc_link value from this synthetic context, as that would
		 * break the signal handling context chain.
		 */
		jump_uc.uc_flags = UC_CPU;
		jump_uc.uc_brand_data[0] = (void *)(LX_UC_STACK_BRAND |
		    LX_UC_IGNORE_LINK);

		LX_REG(&jump_uc, REG_FP) = 0;
		LX_REG(&jump_uc, REG_SP) = lxfp;
		LX_REG(&jump_uc, REG_PC) = (uintptr_t)user_handler;

#if defined(_LP64)
		/*
		 * Pass signal handler arguments by registers on AMD64.
		 */
		LX_REG(&jump_uc, REG_RDI) = hargs[0];
		LX_REG(&jump_uc, REG_RSI) = hargs[1];
		LX_REG(&jump_uc, REG_RDX) = hargs[2];
#endif

		lx_jump_to_linux(&jump_uc);
	}

	assert(0);
	abort();

after_signal_handler:
	/*
	 * Ensure all nested signal handlers have completed correctly
	 * and then remove our stack reservation.
	 */
	_sigoff();
	LX_SIGNAL_POST_HANDLER(lxfp, old_tsd_sp);
	assert(lxtsd->lxtsd_lx_sp == lxfp);
	lx_debug("lx_sigdeliver: after; Linux tsd sp %p -> %p\n", lxfp,
	    old_tsd_sp);
	lxtsd->lxtsd_lx_sp = old_tsd_sp;
	if (newstack) {
		LX_SIGNAL_ALTSTACK_DISABLE();
		lx_debug("lx_sigdeliver: disabling ALTSTACK sp %p\n", lxfp);
		lxtsd->lxtsd_sigaltstack.ss_flags &= ~LX_SS_ONSTACK;
	}
	/*
	 * Restore backup signal tracking chain pointer to previous value:
	 */
	if (lxtsd->lxtsd_sigbackup != NULL) {
		lx_sigbackup_t *bprev = lxtsd->lxtsd_sigbackup->lxsb_previous;

		lx_debug("lx_sigdeliver: restoring sigbackup %p to %p\n",
		    lxtsd->lxtsd_sigbackup, bprev);

		lxtsd->lxtsd_sigbackup = bprev;
	}
	_sigon();

	/*
	 * Here we return to libc so that it may clean up and restore the
	 * context originally interrupted by this signal.
	 */
}

/*
 * Common routine to modify sigaction characteristics of a thread.
 *
 * We shouldn't need any special locking code here as we actually use our copy
 * of libc's sigaction() to do all the real work, so its thread locking should
 * take care of any issues for us.
 */
static int
lx_sigaction_common(int lx_sig, struct lx_sigaction *lxsp,
    struct lx_sigaction *olxsp)
{
	struct lx_sigaction *lxsap;
	struct sigaction sa;

	if (lx_sig <= 0 || lx_sig > LX_NSIG)
		return (-EINVAL);

	lxsap = &lx_sighandlers.lx_sa[lx_sig];
	lx_debug("&lx_sighandlers.lx_sa[%d] = 0x%p", lx_sig, lxsap);

	if ((olxsp != NULL) &&
	    ((uucopy(lxsap, olxsp, sizeof (struct lx_sigaction))) != 0))
		return (-errno);

	if (lxsp != NULL) {
		int err, sig;
		struct lx_sigaction lxsa;
		sigset_t new_set, oset;

		if (uucopy(lxsp, &lxsa, sizeof (struct lx_sigaction)) != 0)
			return (-errno);

		if ((sig = ltos_signo[lx_sig]) != -1) {
			if (lx_no_abort_handler) {
				/*
				 * If LX_NO_ABORT_HANDLER has been set, we will
				 * not allow the emulated program to do
				 * anything hamfisted with SIGSEGV or SIGABRT
				 * signals.
				 */
				if (sig == SIGSEGV || sig == SIGABRT) {
					return (0);
				}
			}

			/*
			 * Block this signal while messing with its dispostion
			 */
			(void) sigemptyset(&new_set);
			(void) sigaddset(&new_set, sig);

			if (sigprocmask(SIG_BLOCK, &new_set, &oset) < 0) {
				err = errno;
				lx_debug("unable to block signal %d: %d",
				    sig, err);
				return (-err);
			}

			/*
			 * We don't really need the old signal disposition at
			 * this point, but this weeds out signals that would
			 * cause sigaction() to return an error before we change
			 * anything other than the current signal mask.
			 */
			if (sigaction(sig, NULL, &sa) < 0) {
				err = errno;
				lx_debug("sigaction() to get old "
				    "disposition for signal %d failed: %d",
				    sig, err);
				(void) sigprocmask(SIG_SETMASK, &oset, NULL);
				return (-err);
			}

			if ((lxsa.lxsa_handler != SIG_DFL) &&
			    (lxsa.lxsa_handler != SIG_IGN)) {
				sa.sa_sigaction = lx_call_user_handler;

				/*
				 * The interposition signal handler needs the
				 * information provided via the SA_SIGINFO flag.
				 */
				sa.sa_flags = SA_SIGINFO;

				/*
				 * When translating from Linux to illumos
				 * sigaction(2) flags, we explicitly do not
				 * pass SA_ONSTACK to the kernel.  The
				 * alternate stack for Linux signal handling is
				 * handled entirely by the emulation code.
				 */
				if (lxsa.lxsa_flags & LX_SA_NOCLDSTOP)
					sa.sa_flags |= SA_NOCLDSTOP;
				if (lxsa.lxsa_flags & LX_SA_NOCLDWAIT)
					sa.sa_flags |= SA_NOCLDWAIT;
				if (lxsa.lxsa_flags & LX_SA_RESTART)
					sa.sa_flags |= SA_RESTART;
				if (lxsa.lxsa_flags & LX_SA_NODEFER)
					sa.sa_flags |= SA_NODEFER;

				/*
				 * RESETHAND cannot be used be passed through
				 * for SIGPWR due to different default actions
				 * between Linux and Illumos.
				 */
				if ((sig != SIGPWR) &&
				    (lxsa.lxsa_flags & LX_SA_RESETHAND))
					sa.sa_flags |= SA_RESETHAND;

				if (ltos_sigset(&lxsa.lxsa_mask,
				    &sa.sa_mask) != 0) {
					err = errno;
					(void) sigprocmask(SIG_SETMASK, &oset,
					    NULL);
					return (-err);
				}

				lx_debug("interposing handler @ 0x%p for "
				    "signal %d (lx %d), flags 0x%x",
				    lxsa.lxsa_handler, sig, lx_sig,
				    lxsa.lxsa_flags);

				if (sigaction(sig, &sa, NULL) < 0) {
					err = errno;
					lx_debug("sigaction() to set new "
					    "disposition for signal %d failed: "
					    "%d", sig, err);
					(void) sigprocmask(SIG_SETMASK, &oset,
					    NULL);
					return (-err);
				}
			} else if ((sig != SIGPWR) ||
			    ((sig == SIGPWR) &&
			    (lxsa.lxsa_handler == SIG_IGN))) {
				/*
				 * There's no need to interpose for SIG_DFL or
				 * SIG_IGN so just call our copy of libc's
				 * sigaction(), but don't allow SIG_DFL for
				 * SIGPWR due to differing default actions
				 * between Linux and Illumos.
				 *
				 * Get the previous disposition first so things
				 * like sa_mask and sa_flags are preserved over
				 * a transition to SIG_DFL or SIG_IGN, which is
				 * what Linux expects.
				 */

				sa.sa_handler = lxsa.lxsa_handler;

				if (sigaction(sig, &sa, NULL) < 0) {
					err = errno;
					lx_debug("sigaction(%d, %s) failed: %d",
					    sig, ((sa.sa_handler == SIG_DFL) ?
					    "SIG_DFL" : "SIG_IGN"), err);
					(void) sigprocmask(SIG_SETMASK, &oset,
					    NULL);
					return (-err);
				}
			}
		} else {
			lx_debug("Linux signal with no kill support "
			    "specified: %d", lx_sig);
		}

		/*
		 * Save the new disposition for the signal in the global
		 * lx_sighandlers structure.
		 */
		bcopy(&lxsa, lxsap, sizeof (struct lx_sigaction));

		/*
		 * Reset the signal mask to what we came in with if
		 * we were modifying a kill-supported signal.
		 */
		if (sig != -1)
			(void) sigprocmask(SIG_SETMASK, &oset, NULL);
	}

	return (0);
}

#if defined(_ILP32)
/*
 * sigaction is only used in 32-bit code.
 */
long
lx_sigaction(uintptr_t lx_sig, uintptr_t actp, uintptr_t oactp)
{
	int val;
	struct lx_sigaction sa, osa;
	struct lx_sigaction *sap, *osap;
	struct lx_osigaction *osp;

	sap = (actp ? &sa : NULL);
	osap = (oactp ? &osa : NULL);

	/*
	 * If we have a source pointer, convert source lxsa_mask from
	 * lx_osigset_t to lx_sigset_t format.
	 */
	if (sap) {
		osp = (struct lx_osigaction *)actp;
		sap->lxsa_handler = osp->lxsa_handler;

		bzero(&sap->lxsa_mask, sizeof (lx_sigset_t));

		for (val = 1; val <= OSIGSET_NBITS; val++)
			if (osp->lxsa_mask & OSIGSET_BITSET(val))
				(void) lx_sigaddset(&sap->lxsa_mask, val);

		sap->lxsa_flags = osp->lxsa_flags;
		sap->lxsa_restorer = osp->lxsa_restorer;
	}

	if ((val = lx_sigaction_common(lx_sig, sap, osap)))
		return (val);

	/*
	 * If we have a save pointer, convert the old lxsa_mask from
	 * lx_sigset_t to lx_osigset_t format.
	 */
	if (osap) {
		osp = (struct lx_osigaction *)oactp;

		osp->lxsa_handler = osap->lxsa_handler;

		bzero(&osp->lxsa_mask, sizeof (osp->lxsa_mask));
		for (val = 1; val <= OSIGSET_NBITS; val++)
			if (lx_sigismember(&osap->lxsa_mask, val))
				osp->lxsa_mask |= OSIGSET_BITSET(val);

		osp->lxsa_flags = osap->lxsa_flags;
		osp->lxsa_restorer = osap->lxsa_restorer;
	}

	return (0);
}
#endif

long
lx_rt_sigaction(uintptr_t lx_sig, uintptr_t actp, uintptr_t oactp,
    uintptr_t setsize)
{
	/*
	 * The "new" rt_sigaction call checks the setsize
	 * parameter.
	 */
	if ((size_t)setsize != sizeof (lx_sigset_t))
		return (-EINVAL);

	return (lx_sigaction_common(lx_sig, (struct lx_sigaction *)actp,
	    (struct lx_sigaction *)oactp));
}

#if defined(_ILP32)
/*
 * Convert signal syscall to a call to the lx_sigaction() syscall
 * Only used in 32-bit code.
 */
long
lx_signal(uintptr_t lx_sig, uintptr_t handler)
{
	struct sigaction act;
	struct sigaction oact;
	int rc;

	/*
	 * Use sigaction to mimic SYSV signal() behavior; glibc will
	 * actually call sigaction(2) itself, so we're really reaching
	 * back for signal(2) semantics here.
	 */
	bzero(&act, sizeof (act));
	act.sa_handler = (void (*)())handler;
	act.sa_flags = SA_RESETHAND | SA_NODEFER;

	rc = lx_sigaction(lx_sig, (uintptr_t)&act, (uintptr_t)&oact);
	return ((rc == 0) ? ((ssize_t)oact.sa_handler) : rc);
}
#endif

void
lx_sighandlers_save(lx_sighandlers_t *saved)
{
	bcopy(&lx_sighandlers, saved, sizeof (lx_sighandlers_t));
}

void
lx_sighandlers_restore(lx_sighandlers_t *saved)
{
	bcopy(saved, &lx_sighandlers, sizeof (lx_sighandlers_t));
}

int
lx_siginit(void)
{
	extern void set_setcontext_enforcement(int);
	extern void set_escaped_context_cleanup(int);

	struct sigaction sa;
	sigset_t new_set, oset;
	int lx_sig, sig;

	/*
	 * Block all signals possible while setting up the signal imposition
	 * mechanism.
	 */
	(void) sigfillset(&new_set);

	if (sigprocmask(SIG_BLOCK, &new_set, &oset) < 0)
		lx_err_fatal("unable to block signals while setting up "
		    "imposition mechanism: %s", strerror(errno));

	/*
	 * Ignore any signals that have no Linux analog so that those
	 * signals cannot be sent to Linux processes from the global zone
	 */
	for (sig = 1; sig < NSIG; sig++)
		if (stol_signo[sig] < 0)
			(void) sigignore(sig);

	/*
	 * Mark any signals that are ignored as ignored in our interposition
	 * handler array
	 */
	for (lx_sig = 1; lx_sig <= LX_NSIG; lx_sig++) {
		if (((sig = ltos_signo[lx_sig]) != -1) &&
		    (sigaction(sig, NULL, &sa) < 0))
			lx_err_fatal("unable to determine previous disposition "
			    "for signal %d: %s", sig, strerror(errno));

		if (sa.sa_handler == SIG_IGN) {
			lx_debug("marking signal %d (lx %d) as SIG_IGN",
			    sig, lx_sig);
			lx_sighandlers.lx_sa[lx_sig].lxsa_handler = SIG_IGN;
		}
	}

	/*
	 * Have our interposition handler handle SIGPWR to start with,
	 * as it has a default action of terminating the process in Linux
	 * but its default is to be ignored in Illumos.
	 */
	(void) sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = lx_call_user_handler;
	sa.sa_flags = SA_SIGINFO;

	if (sigaction(SIGPWR, &sa, NULL) < 0)
		lx_err_fatal("sigaction(SIGPWR) failed: %s", strerror(errno));

	/*
	 * Illumos' libc forces certain register values in the ucontext_t
	 * used to restore a post-signal user context to be those Illumos
	 * expects; however that is not what we want to happen if the signal
	 * was taken while branded code was executing, so we must disable
	 * that behavior.
	 */
	set_setcontext_enforcement(0);

	/*
	 * The illumos libc attempts to clean up dangling uc_link pointers in
	 * signal handling contexts when libc believes us to have escaped a
	 * signal handler incorrectly in the past.  We want to disable this
	 * behaviour, so that the system call emulation context saved by the
	 * kernel brand module for lx_emulate() may be part of the context
	 * chain without itself being used for signal handling.
	 */
	set_escaped_context_cleanup(0);

	/*
	 * Reset the signal mask to what we came in with.
	 */
	(void) sigprocmask(SIG_SETMASK, &oset, NULL);

	lx_debug("interposition handler setup for SIGPWR");
	return (0);
}

/*
 * The first argument is the pid (Linux tgid) to send the signal to, second
 * argument is the signal to send (an lx signal), and third is the siginfo_t
 * with extra information. We translate the code and signal only from the
 * siginfo_t, and leave everything else the same as it gets passed through the
 * signalling system. This is enough to get sigqueue working. See Linux man
 * page rt_sigqueueinfo(2).
 */
long
lx_rt_sigqueueinfo(uintptr_t p1, uintptr_t p2, uintptr_t p3)
{
	pid_t tgid = (pid_t)p1;
	int lx_sig = (int)p2;
	int sig;
	lx_siginfo_t lx_siginfo;
	siginfo_t siginfo;
	int s_code;
	pid_t s_pid;

	if (uucopy((void *)p3, &lx_siginfo, sizeof (lx_siginfo_t)) != 0)
		return (-EFAULT);
	s_code = ltos_sigcode(lx_siginfo.lsi_code);
	if (s_code == LX_SI_CODE_NOT_EXIST)
		return (-EINVAL);
	if (lx_sig < 0 || lx_sig > LX_NSIG || (sig = ltos_signo[lx_sig]) < 0) {
		return (-EINVAL);
	}
	/*
	 * This case (when trying to kill pid 0) just has a different errno
	 * returned in illumos than in Linux.
	 */
	if (tgid == 0)
		return (-ESRCH);
	if (lx_lpid_to_spid(tgid, &s_pid) != 0)
		return (-ESRCH);
	if (SI_CANQUEUE(s_code)) {
		return ((syscall(SYS_sigqueue, s_pid, sig,
		    lx_siginfo.lsi_value, s_code, 0) == -1) ?
		    (-errno): 0);
	} else {
		/*
		 * This case is unlikely, as the main entry point is through
		 * sigqueue, which always has a queuable si_code.
		 */
		siginfo.si_signo = sig;
		siginfo.si_code = s_code;
		siginfo.si_pid = lx_siginfo.lsi_pid;
		siginfo.si_value = lx_siginfo.lsi_value;
		siginfo.si_uid = lx_siginfo.lsi_uid;
		return ((syscall(SYS_brand, B_HELPER_SIGQUEUE,
		    tgid, sig, &siginfo)) ? (-errno) : 0);
	}
}

/*
 * Adds an additional argument for which thread within a thread group to send
 * the signal to (added as the second argument).
 */
long
lx_rt_tgsigqueueinfo(uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4)
{
	pid_t tgid = (pid_t)p1;
	pid_t tid = (pid_t)p2;
	int lx_sig = (int)p3;
	int sig;
	lx_siginfo_t lx_siginfo;
	siginfo_t siginfo;
	int si_code;

	if (uucopy((void *)p4, &lx_siginfo, sizeof (lx_siginfo_t)) != 0)
		return (-EFAULT);
	if (lx_sig < 0 || lx_sig > LX_NSIG || (sig = ltos_signo[lx_sig]) < 0) {
		return (-EINVAL);
	}
	si_code = ltos_sigcode(lx_siginfo.lsi_code);
	if (si_code == LX_SI_CODE_NOT_EXIST)
		return (-EINVAL);
	/*
	 * Check for invalid tgid and tids. That appears to be only negatives
	 * and 0 values. Everything else that doesn't exist is instead ESRCH.
	 */
	if (tgid <= 0 || tid <= 0)
		return (-EINVAL);
	siginfo.si_signo = sig;
	siginfo.si_code = si_code;
	siginfo.si_pid = lx_siginfo.lsi_pid;
	siginfo.si_value = lx_siginfo.lsi_value;
	siginfo.si_uid = lx_siginfo.lsi_uid;

	return ((syscall(SYS_brand, B_HELPER_TGSIGQUEUE, tgid, tid, sig,
	    &siginfo)) ? (-errno) : 0);
}

long
lx_signalfd(int fd, uintptr_t mask, size_t msize)
{
	return (lx_signalfd4(fd, mask, msize, 0));
}

long
lx_signalfd4(int fd, uintptr_t mask, size_t msize, int flags)
{
	sigset_t s_set;
	int r;

	if (msize != sizeof (int64_t))
		return (-EINVAL);

	if (ltos_sigset((lx_sigset_t *)mask, &s_set) != 0)
		return (-errno);

	r = signalfd(fd, &s_set, flags);

	/*
	 * signalfd(3C) may fail with ENOENT if /dev/signalfd is not available.
	 * It is less jarring to Linux programs to tell them that internal
	 * allocation failed than to report an error number they are not
	 * expecting.
	 */
	if (r == -1 && errno == ENOENT)
		return (-ENODEV);

	return (r == -1 ? -errno : r);
}

/*
 * Since this brackets vfork, we also use it as a synchronisation point to
 * prevent multiple vfork() calls occuring in parallel. This is necessary
 * because vfork() on illumos is not MT-safe whereas it is on Linux (with
 * caveats).
 *
 * Some real-world applications (java in particular) can run multiple vfork()
 * calls in parallel across different threads and they need to be serialised
 * in the lx brand.
 */
void
lx_block_all_signals()
{
	while (syscall(SYS_brand, B_BLOCK_ALL_SIGS) != 0 && errno == EAGAIN)
		yield();
}

void
lx_unblock_all_signals()
{
	(void) syscall(SYS_brand, B_UNBLOCK_ALL_SIGS);
}

int
lx_all_signals_blocked()
{
	return (syscall(SYS_brand, B_ALL_SIGS_BLOCKED));
}



/* ============================================================================
 * SOURCE 5/11: minix4\exokernel\kernel_legacy\signal.c
 * Size: 4,974 bytes, Lines: 231
 * Hash: caad30a66913...
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

#pragma weak _signal = signal
#pragma weak _sighold = sighold
#pragma weak _sigrelse = sigrelse
#pragma weak _sigignore = sigignore
#pragma weak _sigset = sigset

#include "lint.h"
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>

/*
 * Check for valid signal number as per SVID.
 */
#define	CHECK_SIG(s, code) \
	if ((s) <= 0 || (s) >= NSIG || (s) == SIGKILL || (s) == SIGSTOP) { \
		errno = EINVAL; \
		return (code); \
	}

/*
 * Equivalent to stopdefault set in the kernel implementation (sig.c).
 */
#define	STOPDEFAULT(s) \
	((s) == SIGSTOP || (s) == SIGTSTP || (s) == SIGTTOU || (s) == SIGTTIN)


/*
 * SVr3.x signal compatibility routines. They are now
 * implemented as library routines instead of system
 * calls.
 */

void(*
signal(int sig, void(*func)(int)))(int)
{
	struct sigaction nact;
	struct sigaction oact;

	CHECK_SIG(sig, SIG_ERR);

	nact.sa_handler = func;
	nact.sa_flags = SA_RESETHAND|SA_NODEFER;
	(void) sigemptyset(&nact.sa_mask);

	/*
	 * Pay special attention if sig is SIGCHLD and
	 * the disposition is SIG_IGN, per sysV signal man page.
	 */
	if (sig == SIGCHLD) {
		nact.sa_flags |= SA_NOCLDSTOP;
		if (func == SIG_IGN)
			nact.sa_flags |= SA_NOCLDWAIT;
	}

	if (STOPDEFAULT(sig))
		nact.sa_flags |= SA_RESTART;

	if (sigaction(sig, &nact, &oact) < 0)
		return (SIG_ERR);

	return (oact.sa_handler);
}

int
sighold(int sig)
{
	sigset_t set;

	CHECK_SIG(sig, -1);

	/*
	 * errno set on failure by either sigaddset or sigprocmask.
	 */
	(void) sigemptyset(&set);
	if (sigaddset(&set, sig) < 0)
		return (-1);
	return (sigprocmask(SIG_BLOCK, &set, (sigset_t *)0));
}

int
sigrelse(int sig)
{
	sigset_t set;

	CHECK_SIG(sig, -1);

	/*
	 * errno set on failure by either sigaddset or sigprocmask.
	 */
	(void) sigemptyset(&set);
	if (sigaddset(&set, sig) < 0)
		return (-1);
	return (sigprocmask(SIG_UNBLOCK, &set, (sigset_t *)0));
}

int
sigignore(int sig)
{
	struct sigaction act;
	sigset_t set;

	CHECK_SIG(sig, -1);

	act.sa_handler = SIG_IGN;
	act.sa_flags = 0;
	(void) sigemptyset(&act.sa_mask);

	/*
	 * Pay special attention if sig is SIGCHLD and
	 * the disposition is SIG_IGN, per sysV signal man page.
	 */
	if (sig == SIGCHLD) {
		act.sa_flags |= SA_NOCLDSTOP;
		act.sa_flags |= SA_NOCLDWAIT;
	}

	if (STOPDEFAULT(sig))
		act.sa_flags |= SA_RESTART;

	if (sigaction(sig, &act, (struct sigaction *)0) < 0)
		return (-1);

	(void) sigemptyset(&set);
	if (sigaddset(&set, sig) < 0)
		return (-1);
	return (sigprocmask(SIG_UNBLOCK, &set, (sigset_t *)0));
}

int
__sigpause(int sig)
{
	sigset_t set;
	int rval;

	CHECK_SIG(sig, -1);

	/*
	 * sigpause() is defined to unblock the signal
	 * and not block it again on return.
	 * sigsuspend() restores the original signal set,
	 * so we have to unblock sig overtly.
	 */
	(void) sigprocmask(0, (sigset_t *)0, &set);
	if (sigdelset(&set, sig) < 0)
		return (-1);
	rval = sigsuspend(&set);
	(void) sigrelse(sig);
	return (rval);
}

void(*
sigset(int sig, void(*func)(int)))(int)
{
	struct sigaction nact;
	struct sigaction oact;
	sigset_t nset;
	sigset_t oset;
	int code;

	CHECK_SIG(sig, SIG_ERR);

	(void) sigemptyset(&nset);
	if (sigaddset(&nset, sig) < 0)
		return (SIG_ERR);

	if (func == SIG_HOLD) {
		if (sigprocmask(SIG_BLOCK, &nset, &oset) < 0)
			return (SIG_ERR);
		if (sigaction(sig, (struct sigaction *)0, &oact) < 0)
			return (SIG_ERR);
	} else {
		nact.sa_handler = func;
		nact.sa_flags = 0;
		(void) sigemptyset(&nact.sa_mask);
		/*
		 * Pay special attention if sig is SIGCHLD and
		 * the disposition is SIG_IGN, per sysV signal man page.
		 */
		if (sig == SIGCHLD) {
			nact.sa_flags |= SA_NOCLDSTOP;
			if (func == SIG_IGN)
				nact.sa_flags |= SA_NOCLDWAIT;
		}

		if (STOPDEFAULT(sig))
			nact.sa_flags |= SA_RESTART;

		if (sigaction(sig, &nact, &oact) < 0)
			return (SIG_ERR);

		if (sigprocmask(SIG_UNBLOCK, &nset, &oset) < 0)
			return (SIG_ERR);
	}

	if ((code = sigismember(&oset, sig)) < 0)
		return (SIG_ERR);
	else if (code == 1)
		return (SIG_HOLD);

	return (oact.sa_handler);
}



/* ============================================================================
 * SOURCE 6/11: minix4\exokernel\kernel_legacy\contrib\ast\src\lib\libast\features\signal.c
 * Size: 8,448 bytes, Lines: 376
 * Hash: d31cf068ee96...
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
#pragma prototyped
/*
 * Glenn Fowler
 * AT&T Research
 *
 * generate signal features
 */

#include "FEATURE/standards"

#define strsignal	______strsignal

#include <signal.h>

#undef	strsignal

struct _m_
{
	char*		text;
	char*		name;
	int		value;
};

#define elementsof(x)	(sizeof(x)/sizeof(x[0]))

static struct _m_ map[] =
{
#ifdef SIGABRT
#define HAD_SIGABRT	1
"Abort",			"ABRT",		SIGABRT,
#endif
#ifdef SIGAIO
#define HAD_SIGAIO	1
"Asynchronous I/O",		"AIO",		SIGAIO,
#endif
#ifdef SIGALRM
#define HAD_SIGALRM	1
"Alarm call",			"ALRM",		SIGALRM,
#endif
#ifdef SIGAPOLLO
#define HAD_SIGAPOLLO	1
"Apollo",			"APOLLO",	SIGAPOLLO,
#endif
#ifdef SIGBUS
#define HAD_SIGBUS	1
"Bus error",			"BUS",		SIGBUS,
#endif
#ifdef SIGCHLD
#define HAD_SIGCHLD	1
"Child status change",		"CHLD",		SIGCHLD,
#endif
#ifdef SIGCLD
#define HAD_SIGCLD	1
"Death of child", 		"CLD",		SIGCLD,
#endif
#ifdef SIGCONT
#define HAD_SIGCONT	1
"Stopped process continued",	"CONT",		SIGCONT,
#endif
#ifdef SIGDANGER
#define HAD_SIGDANGER	1
"System crash soon",		"DANGER",	SIGDANGER,
#endif
#ifdef SIGDEBUG
#define HAD_SIGDEBUG	1
"Debug trap",			"DEBUG",	SIGDEBUG,
#endif
#ifdef SIGDIL
#define HAD_SIGDIL	1
"DIL trap",			"DIL",		SIGDIL,
#endif
#ifdef SIGEMT
#define HAD_SIGEMT	1
"EMT trap",			"EMT",		SIGEMT,
#endif
#ifdef SIGERR
#define HAD_SIGERR	1
"ERR trap",			"ERR",		SIGERR,
#endif
#ifdef SIGEXIT
#define HAD_SIGEXIT	1
"Exit",				"EXIT",		SIGEXIT,
#endif
#ifdef SIGFPE
#define HAD_SIGFPE	1
"Floating exception",		"FPE",		SIGFPE,
#endif
#ifdef SIGFREEZE
#define HAD_SIGFREEZE	1
"CPR freeze",			"FREEZE",	SIGFREEZE,
#endif
#ifdef SIGHUP
#define HAD_SIGHUP	1
"Hangup",			"HUP",		SIGHUP,
#endif
#ifdef SIGILL
#define HAD_SIGILL	1
"Illegal instruction",		"ILL",		SIGILL,
#endif
#ifdef SIGINT
#define HAD_SIGINT	1
"Interrupt",			"INT",		SIGINT,
#endif
#ifdef SIGIO
#define HAD_SIGIO	1
"IO possible",			"IO",		SIGIO,
#endif
#ifdef SIGIOT
#define HAD_SIGIOT	1
"IOT trap",			"IOT",		SIGIOT,
#endif
#ifdef SIGKILL
#define HAD_SIGKILL	1
"Killed",			"KILL",		SIGKILL,
#endif
#ifdef SIGLAB
#define HAD_SIGLAB	1
"Security label changed",	"LAB",		SIGLAB,
#endif
#ifdef SIGLOST
#define HAD_SIGLOST	1
"Resources lost",		"LOST",		SIGLOST,
#endif
#ifdef SIGLWP
#define HAD_SIGLWP	1
"Thread event",			"LWP",		SIGLWP,
#endif
#ifdef SIGMIGRATE
#define HAD_SIGMIGRATE	1
"Migrate process",		"MIGRATE",	SIGMIGRATE,
#endif
#ifdef SIGPHONE
#define HAD_SIGPHONE	1
"Phone status change",		"PHONE",	SIGPHONE,
#endif
#ifdef SIGPIPE
#define HAD_SIGPIPE	1
"Broken pipe",			"PIPE",		SIGPIPE,
#endif
#ifdef SIGPOLL
#define HAD_SIGPOLL	1
"Poll event",			"POLL",		SIGPOLL,
#endif
#ifdef SIGPROF
#define HAD_SIGPROF	1
"Profile timer alarm",		"PROF",		SIGPROF,
#endif
#ifdef SIGPWR
#define HAD_SIGPWR	1
"Power fail",			"PWR",		SIGPWR,
#endif
#ifdef SIGQUIT
#define HAD_SIGQUIT	1
"Quit",				"QUIT",		SIGQUIT,
#endif
#ifdef SIGSEGV
#define HAD_SIGSEGV	1
"Memory fault",			"SEGV",		SIGSEGV,
#endif
#ifdef SIGSOUND
#define HAD_SIGSOUND	1
"Sound completed",		"SOUND",	SIGSOUND,
#endif
#ifdef SIGSSTOP
#define HAD_SIGSSTOP	1
"Sendable stop",		"SSTOP",	SIGSSTOP,
#endif
#ifdef SIGSTKFLT
#define HAD_SIGSTKFLT	1
"Stack fault",			"STKFLT",	SIGSTKFLT,
#endif
#ifdef gould
#define HAD_gould	1
"Stack overflow",		"STKOV",	28,
#endif
#ifdef SIGSTOP
#define HAD_SIGSTOP	1
"Stopped (signal)",		"STOP",		SIGSTOP,
#endif
#ifdef SIGSYS
#define HAD_SIGSYS	1
"Bad system call", 		"SYS",		SIGSYS,
#endif
#ifdef SIGTERM
#define HAD_SIGTERM	1
"Terminated",			"TERM",		SIGTERM,
#endif
#ifdef SIGTHAW
#define HAD_SIGTHAW	1
"CPR thaw",			"THAW",		SIGTHAW,
#endif
#ifdef SIGTINT
#define HAD_SIGTINT	1
"Interrupt (terminal)",		"TINT",		SIGTINT,
#endif
#ifdef SIGTRAP
#define HAD_SIGTRAP	1
"Trace trap",			"TRAP",		SIGTRAP,
#endif
#ifdef SIGTSTP
#define HAD_SIGTSTP	1
"Stopped",			"TSTP",		SIGTSTP,
#endif
#ifdef SIGTTIN
#define HAD_SIGTTIN	1
"Stopped (tty input)",		"TTIN",		SIGTTIN,
#endif
#ifdef SIGTTOU
#define HAD_SIGTTOU	1
"Stopped (tty output)",		"TTOU",		SIGTTOU,
#endif
#ifdef SIGURG
#define HAD_SIGURG	1
"Urgent IO",			"URG",		SIGURG,
#endif
#ifdef SIGUSR1
#define HAD_SIGUSR1	1
"User signal 1",		"USR1",		SIGUSR1,
#endif
#ifdef SIGUSR2
#define HAD_SIGUSR2	1
"User signal 2",		"USR2",		SIGUSR2,
#endif
#ifdef SIGVTALRM
#define HAD_SIGVTALRM	1
"Virtual timer alarm",		"VTALRM",	SIGVTALRM,
#endif
#ifdef SIGWAITING
#define HAD_SIGWAITING	1
"All threads blocked",		"WAITING",	SIGWAITING,
#endif
#ifdef SIGWINCH
#define HAD_SIGWINCH	1
"Window change", 		"WINCH",	SIGWINCH,
#endif
#ifdef SIGWIND
#define HAD_SIGWIND	1
"Window change",		"WIND",		SIGWIND,
#endif
#ifdef SIGWINDOW
#define HAD_SIGWINDOW	1
"Window change",		"WINDOW",	SIGWINDOW,
#endif
#ifdef SIGXCPU
#define HAD_SIGXCPU	1
"CPU time limit",		"XCPU",		SIGXCPU,
#endif
#ifdef SIGXFSZ
#define HAD_SIGXFSZ	1
"File size limit",		"XFSZ",		SIGXFSZ,
#endif
#include "FEATURE/siglist"
0
};

#define RANGE_MIN	(1<<14)
#define RANGE_MAX	(1<<13)
#define RANGE_RT	(1<<12)

#define RANGE_SIG	(~(RANGE_MIN|RANGE_MAX|RANGE_RT))

static int		mapindex[1024];

#if _lib_strsignal
extern char*		strsignal(int);
#endif

int
main()
{
	register int	i;
	register int	j;
	register int	k;
	int		m;
	int		n;
#if _lib_strsignal
	char*		s;
#endif

	k = 0;
	for (i = 0; map[i].name; i++)
		if ((j = map[i].value) > 0 && j < elementsof(mapindex) && !mapindex[j])
		{
			if (j > k)
				k = j;
			mapindex[j] = i;
		}
#ifdef SIGRTMIN
	i = _SIGRTMIN;
#ifdef SIGRTMAX
	j = _SIGRTMAX;
#else
	j = i;
#endif
	if (j >= elementsof(mapindex))
		j = elementsof(mapindex) - 1;
	if (i <= j && i > 0 && i < elementsof(mapindex) && j > 0 && j < elementsof(mapindex))
	{
		if (j > k)
			k = j;
		mapindex[i] = RANGE_MIN | RANGE_RT;
		n = 1;
		while (++i < j)
			mapindex[i] = RANGE_RT | n++;
		mapindex[j] = RANGE_MAX | RANGE_RT | n;
	}
#endif
	printf("#pragma prototyped\n");
	printf("#define SIG_MAX	%d\n", k);
	printf("\n");
	printf("static const char* const	sig_name[] =\n");
	printf("{\n");
	for (i = 0; i <= k; i++)
		if (!(j = mapindex[i]))
			printf("	\"%d\",\n", i);
		else if (j & RANGE_RT)
		{
			if (j & RANGE_MIN)
				printf("	\"RTMIN\",\n");
			else if (j & RANGE_MAX)
				printf("	\"RTMAX\",\n");
			else
			{
				m = j & RANGE_SIG;
				if (m > n / 2)
					printf("	\"RTMAX-%d\",\n", n - m);
				else
					printf("	\"RTMIN+%d\",\n", m);
			}
		}
		else
			printf("	\"%s\",\n", map[j].name);
	printf("	0\n");
	printf("};\n");
	printf("\n");
	printf("static const char* const	sig_text[] =\n");
	printf("{\n");
	for (i = 0; i <= k; i++)
		if (!(j = mapindex[i]))
			printf("	\"Signal %d\",\n", i);
		else if (j & RANGE_RT)
			printf("	\"Realtime priority %d%s\",\n", j & RANGE_SIG, (j & RANGE_MIN) ? " (lo)" : (j & RANGE_MAX) ? " (hi)" : "");
		else if (map[j].text)
			printf("	\"%s\",\n", map[j].text);
#if _lib_strsignal
		else if (s = strsignal(i))
			printf("	\"%s\",\n", s);
#endif
		else
			printf("	\"Signal %d\",\n", i);
	printf("	0\n");
	printf("};\n");
	return 0;
}



/* ============================================================================
 * SOURCE 7/11: minix4\exokernel\kernel_legacy\contrib\ast\src\lib\libast\misc\signal.c
 * Size: 3,475 bytes, Lines: 137
 * Hash: aae5f3d206cd...
 * ============================================================================ */

/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*          Copyright (c) 1985-2011 AT&T Intellectual Property          *
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
#pragma prototyped

/*
 * signal that disables syscall restart on interrupt with clear signal mask
 * fun==SIG_DFL also unblocks signal
 */

#if !_UWIN

#undef	signal
#define signal		______signal

#endif

#include <ast.h>
#include <sig.h>

#if !_UWIN

#undef	signal

#undef	_def_map_ast
#include <ast_map.h>

#if defined(__EXPORT__)
#define extern	__EXPORT__
#endif

#endif

#if defined(SV_ABORT)                                         
#undef	SV_INTERRUPT
#define SV_INTERRUPT	SV_ABORT
#endif

#if !_std_signal && (_lib_sigaction && defined(SA_NOCLDSTOP) || _lib_sigvec && defined(SV_INTERRUPT))

#if !defined(SA_NOCLDSTOP) || !defined(SA_INTERRUPT) && defined(SV_INTERRUPT)
#undef	SA_INTERRUPT
#define SA_INTERRUPT	SV_INTERRUPT
#undef	sigaction
#define sigaction	sigvec
#undef	sigemptyset
#define sigemptyset(p)	(*(p)=0)
#undef	sa_flags
#define sa_flags	sv_flags
#undef	sa_handler
#define sa_handler	sv_handler
#undef	sa_mask
#define	sa_mask		sv_mask
#endif

extern Sig_handler_t
signal(int sig, Sig_handler_t fun)
{
	struct sigaction	na;
	struct sigaction	oa;
	int			unblock;
#ifdef SIGNO_MASK
	unsigned int		flags;
#endif

	if (sig < 0)
	{
		sig = -sig;
		unblock = 0;
	}
	else
		unblock = fun == SIG_DFL;
#ifdef SIGNO_MASK
	flags = sig & ~SIGNO_MASK;
	sig &= SIGNO_MASK;
#endif
	memzero(&na, sizeof(na));
	na.sa_handler = fun;
#if defined(SA_INTERRUPT) || defined(SA_RESTART)
	switch (sig)
	{
#if defined(SIGIO) || defined(SIGTSTP) || defined(SIGTTIN) || defined(SIGTTOU)
#if defined(SIGIO)
	case SIGIO:
#endif
#if defined(SIGTSTP)
	case SIGTSTP:
#endif
#if defined(SIGTTIN)
	case SIGTTIN:
#endif
#if defined(SIGTTOU)
	case SIGTTOU:
#endif
#if defined(SA_RESTART)
		na.sa_flags = SA_RESTART;
#endif
		break;
#endif
	default:
#if defined(SA_INTERRUPT)
		na.sa_flags = SA_INTERRUPT;
#endif
		break;
	}
#endif
	if (sigaction(sig, &na, &oa))
		return 0;
	if (unblock)
		sigunblock(sig);
	return oa.sa_handler;
}

#else

NoN(signal)

#endif



/* ============================================================================
 * SOURCE 8/11: minix4\exokernel\kernel_legacy\cmd\sendmail\libmilter\signal.c
 * Size: 4,054 bytes, Lines: 226
 * Hash: 9655d06510ac...
 * ============================================================================ */

/*
 *  Copyright (c) 1999-2004, 2006 Sendmail, Inc. and its suppliers.
 *	All rights reserved.
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the sendmail distribution.
 *
 */

#include <sm/gen.h>
SM_RCSID("@(#)$Id: signal.c,v 8.44 2006/03/03 03:42:04 ca Exp $")

#include "libmilter.h"

/*
**  thread to handle signals
*/

static smutex_t M_Mutex;

static int MilterStop = MILTER_CONT;

static void	*mi_signal_thread __P((void *));
static int	 mi_spawn_signal_thread __P((char *));

/*
**  MI_STOP -- return value of MilterStop
**
**	Parameters:
**		none.
**
**	Returns:
**		value of MilterStop
*/

int
mi_stop()
{
	return MilterStop;
}
/*
**  MI_STOP_MILTERS -- set value of MilterStop
**
**	Parameters:
**		v -- new value for MilterStop.
**
**	Returns:
**		none.
*/

void
mi_stop_milters(v)
	int v;
{
	(void) smutex_lock(&M_Mutex);
	if (MilterStop < v)
		MilterStop = v;

	/* close listen socket */
	mi_closener();
	(void) smutex_unlock(&M_Mutex);
}
/*
**  MI_CLEAN_SIGNALS -- clean up signal handler thread
**
**	Parameters:
**		none.
**
**	Returns:
**		none.
*/

void
mi_clean_signals()
{
	(void) smutex_destroy(&M_Mutex);
}
/*
**  MI_SIGNAL_THREAD -- thread to deal with signals
**
**	Parameters:
**		name -- name of milter
**
**	Returns:
**		NULL
*/

static void *
mi_signal_thread(name)
	void *name;
{
	int sig, errs, sigerr;
	sigset_t set;

	(void) sigemptyset(&set);
	(void) sigaddset(&set, SIGHUP);
	(void) sigaddset(&set, SIGTERM);

	/* Handle Ctrl-C gracefully for debugging */
	(void) sigaddset(&set, SIGINT);
	errs = 0;

	for (;;)
	{
		sigerr = sig = 0;
#if defined(SOLARIS) || defined(__svr5__)
		if ((sig = sigwait(&set)) < 0)
#else /* defined(SOLARIS) || defined(__svr5__) */
		if ((sigerr = sigwait(&set, &sig)) != 0)
#endif /* defined(SOLARIS) || defined(__svr5__) */
		{
			/* some OS return -1 and set errno: copy it */
			if (sigerr <= 0)
				sigerr = errno;

			/* this can happen on OSF/1 (at least) */
			if (sigerr == EINTR)
				continue;
			smi_log(SMI_LOG_ERR,
				"%s: sigwait returned error: %d",
				(char *)name, sigerr);
			if (++errs > MAX_FAILS_T)
			{
				mi_stop_milters(MILTER_ABRT);
				return NULL;
			}
			continue;
		}
		errs = 0;

		switch (sig)
		{
		  case SIGHUP:
		  case SIGTERM:
			mi_stop_milters(MILTER_STOP);
			return NULL;
		  case SIGINT:
			mi_stop_milters(MILTER_ABRT);
			return NULL;
		  default:
			smi_log(SMI_LOG_ERR,
				"%s: sigwait returned unmasked signal: %d",
				(char *)name, sig);
			break;
		}
	}
	/* NOTREACHED */
}
/*
**  MI_SPAWN_SIGNAL_THREAD -- spawn thread to handle signals
**
**	Parameters:
**		name -- name of milter
**
**	Returns:
**		MI_SUCCESS/MI_FAILURE
*/

static int
mi_spawn_signal_thread(name)
	char *name;
{
	sthread_t tid;
	int r;
	sigset_t set;

	/* Mask HUP and KILL signals */
	(void) sigemptyset(&set);
	(void) sigaddset(&set, SIGHUP);
	(void) sigaddset(&set, SIGTERM);
	(void) sigaddset(&set, SIGINT);

	if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0)
	{
		smi_log(SMI_LOG_ERR,
			"%s: Couldn't mask HUP and KILL signals", name);
		return MI_FAILURE;
	}
	r = thread_create(&tid, mi_signal_thread, (void *)name);
	if (r != 0)
	{
		smi_log(SMI_LOG_ERR,
			"%s: Couldn't start signal thread: %d",
			name, r);
		return MI_FAILURE;
	}
	return MI_SUCCESS;
}
/*
**  MI_CONTROL_STARTUP -- startup for thread to handle signals
**
**	Parameters:
**		name -- name of milter
**
**	Returns:
**		MI_SUCCESS/MI_FAILURE
*/

int
mi_control_startup(name)
	char *name;
{

	if (!smutex_init(&M_Mutex))
	{
		smi_log(SMI_LOG_ERR,
			"%s: Couldn't initialize control pipe mutex", name);
		return MI_FAILURE;
	}

	/*
	**  spawn_signal_thread must happen before other threads are spawned
	**  off so that it can mask the right signals and other threads
	**  will inherit that mask.
	*/
	if (mi_spawn_signal_thread(name) == MI_FAILURE)
	{
		smi_log(SMI_LOG_ERR,
			"%s: Couldn't spawn signal thread", name);
		(void) smutex_destroy(&M_Mutex);
		return MI_FAILURE;
	}
	return MI_SUCCESS;
}



/* ============================================================================
 * SOURCE 9/11: minix4\exokernel\kernel_legacy\cmd\sendmail\libsm\signal.c
 * Size: 7,148 bytes, Lines: 341
 * Hash: b6ea4ced3bb9...
 * ============================================================================ */

/*
 * Copyright (c) 2000-2001 Sendmail, Inc. and its suppliers.
 *      All rights reserved.
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the sendmail distribution.
 */

#include <sm/gen.h>
SM_RCSID("@(#)$Id: signal.c,v 1.17 2005/06/14 23:07:20 ca Exp $")

#if SM_CONF_SETITIMER
# include <sm/time.h>
#endif /* SM_CONF_SETITIMER */
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sm/clock.h>
#include <sm/signal.h>
#include <signal.h>
#include <sm/string.h>

unsigned int	volatile InCriticalSection; /* >0 if inside critical section */
int		volatile PendingSignal;	/* pending signal to resend */

/*
**  SM_SIGNAL -- set a signal handler
**
**	This is essentially old BSD "signal(3)".
**
**	NOTE:	THIS CAN BE CALLED FROM A SIGNAL HANDLER.  DO NOT ADD
**		ANYTHING TO THIS ROUTINE UNLESS YOU KNOW WHAT YOU ARE
**		DOING.
*/

sigfunc_t
sm_signal(sig, handler)
	int sig;
	sigfunc_t handler;
{
# if defined(SA_RESTART) || (!defined(SYS5SIGNALS) && !defined(BSD4_3))
	struct sigaction n, o;
# endif /* defined(SA_RESTART) || (!defined(SYS5SIGNALS) && !defined(BSD4_3)) */

	/*
	**  First, try for modern signal calls
	**  and restartable syscalls
	*/

# ifdef SA_RESTART
	(void) memset(&n, '\0', sizeof n);
#  if USE_SA_SIGACTION
	n.sa_sigaction = (void(*)(int, siginfo_t *, void *))(uintptr_t) handler;
	n.sa_flags = SA_RESTART|SA_SIGINFO;
#  else /* USE_SA_SIGACTION */
	n.sa_handler = handler;
	n.sa_flags = SA_RESTART;
#  endif /* USE_SA_SIGACTION */
	if (sigaction(sig, &n, &o) < 0)
		return SIG_ERR;
	return o.sa_handler;
# else /* SA_RESTART */

	/*
	**  Else check for SYS5SIGNALS or
	**  BSD4_3 signals
	*/

#  if defined(SYS5SIGNALS) || defined(BSD4_3)
#   ifdef BSD4_3
	return signal(sig, handler);
#   else /* BSD4_3 */
	return sigset(sig, handler);
#   endif /* BSD4_3 */
#  else /* defined(SYS5SIGNALS) || defined(BSD4_3) */

	/*
	**  Finally, if nothing else is available,
	**  go for a default
	*/

	(void) memset(&n, '\0', sizeof n);
	n.sa_handler = handler;
	if (sigaction(sig, &n, &o) < 0)
		return SIG_ERR;
	return o.sa_handler;
#  endif /* defined(SYS5SIGNALS) || defined(BSD4_3) */
# endif /* SA_RESTART */
}
/*
**  SM_BLOCKSIGNAL -- hold a signal to prevent delivery
**
**	Parameters:
**		sig -- the signal to block.
**
**	Returns:
**		1 signal was previously blocked
**		0 signal was not previously blocked
**		-1 on failure.
*/

int
sm_blocksignal(sig)
	int sig;
{
# ifdef BSD4_3
#  ifndef sigmask
#   define sigmask(s)	(1 << ((s) - 1))
#  endif /* ! sigmask */
	return (sigblock(sigmask(sig)) & sigmask(sig)) != 0;
# else /* BSD4_3 */
#  ifdef ALTOS_SYSTEM_V
	sigfunc_t handler;

	handler = sigset(sig, SIG_HOLD);
	if (handler == SIG_ERR)
		return -1;
	else
		return handler == SIG_HOLD;
#  else /* ALTOS_SYSTEM_V */
	sigset_t sset, oset;

	(void) sigemptyset(&sset);
	(void) sigaddset(&sset, sig);
	if (sigprocmask(SIG_BLOCK, &sset, &oset) < 0)
		return -1;
	else
		return sigismember(&oset, sig);
#  endif /* ALTOS_SYSTEM_V */
# endif /* BSD4_3 */
}
/*
**  SM_RELEASESIGNAL -- release a held signal
**
**	Parameters:
**		sig -- the signal to release.
**
**	Returns:
**		1 signal was previously blocked
**		0 signal was not previously blocked
**		-1 on failure.
*/

int
sm_releasesignal(sig)
	int sig;
{
# ifdef BSD4_3
	return (sigsetmask(sigblock(0) & ~sigmask(sig)) & sigmask(sig)) != 0;
# else /* BSD4_3 */
#  ifdef ALTOS_SYSTEM_V
	sigfunc_t handler;

	handler = sigset(sig, SIG_HOLD);
	if (sigrelse(sig) < 0)
		return -1;
	else
		return handler == SIG_HOLD;
#  else /* ALTOS_SYSTEM_V */
	sigset_t sset, oset;

	(void) sigemptyset(&sset);
	(void) sigaddset(&sset, sig);
	if (sigprocmask(SIG_UNBLOCK, &sset, &oset) < 0)
		return -1;
	else
		return sigismember(&oset, sig);
#  endif /* ALTOS_SYSTEM_V */
# endif /* BSD4_3 */
}
/*
**  PEND_SIGNAL -- Add a signal to the pending signal list
**
**	Parameters:
**		sig -- signal to add
**
**	Returns:
**		none.
**
**	NOTE:	THIS CAN BE CALLED FROM A SIGNAL HANDLER.  DO NOT ADD
**		ANYTHING TO THIS ROUTINE UNLESS YOU KNOW WHAT YOU ARE
**		DOING.
*/

void
pend_signal(sig)
	int sig;
{
	int sigbit;
	int save_errno = errno;
#if SM_CONF_SETITIMER
	struct itimerval clr;
#endif /* SM_CONF_SETITIMER */

	/*
	**  Don't want to interrupt something critical, hence delay
	**  the alarm for one second.  Hopefully, by then we
	**  will be out of the critical section.  If not, then
	**  we will just delay again.  The events to be run will
	**  still all be run, maybe just a little bit late.
	*/

	switch (sig)
	{
	  case SIGHUP:
		sigbit = PEND_SIGHUP;
		break;

	  case SIGINT:
		sigbit = PEND_SIGINT;
		break;

	  case SIGTERM:
		sigbit = PEND_SIGTERM;
		break;

	  case SIGUSR1:
		sigbit = PEND_SIGUSR1;
		break;

	  case SIGALRM:
		/* don't have to pend these */
		sigbit = 0;
		break;

	  default:
		/* If we get here, we are in trouble */
		abort();

		/* NOTREACHED */
		/* shut up stupid compiler warning on HP-UX 11 */
		sigbit = 0;
		break;
	}

	if (sigbit != 0)
		PendingSignal |= sigbit;
	(void) sm_signal(SIGALRM, sm_tick);
#if SM_CONF_SETITIMER
	clr.it_interval.tv_sec = 0;
	clr.it_interval.tv_usec = 0;
	clr.it_value.tv_sec = 1;
	clr.it_value.tv_usec = 0;
	(void) setitimer(ITIMER_REAL, &clr, NULL);
#else /* SM_CONF_SETITIMER */
	(void) alarm(1);
#endif /* SM_CONF_SETITIMER */
	errno = save_errno;
}
/*
**  SM_ALLSIGNALS -- act on all signals
**
**	Parameters:
**		block -- whether to block or release all signals.
**
**	Returns:
**		none.
*/

void
sm_allsignals(block)
	bool block;
{
# ifdef BSD4_3
#  ifndef sigmask
#   define sigmask(s)	(1 << ((s) - 1))
#  endif /* ! sigmask */
	if (block)
	{
		int mask = 0;

		mask |= sigmask(SIGALRM);
		mask |= sigmask(SIGCHLD);
		mask |= sigmask(SIGHUP);
		mask |= sigmask(SIGINT);
		mask |= sigmask(SIGTERM);
		mask |= sigmask(SIGUSR1);

		(void) sigblock(mask);
	}
	else
		sigsetmask(0);
# else /* BSD4_3 */
#  ifdef ALTOS_SYSTEM_V
	if (block)
	{
		(void) sigset(SIGALRM, SIG_HOLD);
		(void) sigset(SIGCHLD, SIG_HOLD);
		(void) sigset(SIGHUP, SIG_HOLD);
		(void) sigset(SIGINT, SIG_HOLD);
		(void) sigset(SIGTERM, SIG_HOLD);
		(void) sigset(SIGUSR1, SIG_HOLD);
	}
	else
	{
		(void) sigset(SIGALRM, SIG_DFL);
		(void) sigset(SIGCHLD, SIG_DFL);
		(void) sigset(SIGHUP, SIG_DFL);
		(void) sigset(SIGINT, SIG_DFL);
		(void) sigset(SIGTERM, SIG_DFL);
		(void) sigset(SIGUSR1, SIG_DFL);
	}
#  else /* ALTOS_SYSTEM_V */
	sigset_t sset;

	(void) sigemptyset(&sset);
	(void) sigaddset(&sset, SIGALRM);
	(void) sigaddset(&sset, SIGCHLD);
	(void) sigaddset(&sset, SIGHUP);
	(void) sigaddset(&sset, SIGINT);
	(void) sigaddset(&sset, SIGTERM);
	(void) sigaddset(&sset, SIGUSR1);
	(void) sigprocmask(block ? SIG_BLOCK : SIG_UNBLOCK, &sset, NULL);
#  endif /* ALTOS_SYSTEM_V */
# endif /* BSD4_3 */
}
/*
**  SM_SIGNAL_NOOP -- A signal no-op function
**
**	Parameters:
**		sig -- signal received
**
**	Returns:
**		SIGFUNC_RETURN
*/

/* ARGSUSED */
SIGFUNC_DECL
sm_signal_noop(sig)
	int sig;
{
	int save_errno = errno;

	FIX_SYSV_SIGNAL(sig, sm_signal_noop);
	errno = save_errno;
	return SIGFUNC_RETURN;
}




/* ============================================================================
 * SOURCE 10/11: minix4\exokernel\kernel_legacy\cmd\csh\i386\signal.c
 * Size: 6,230 bytes, Lines: 239
 * Hash: 32d21e66b3ec...
 * ============================================================================ */

/*
 * Copyright 2005 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1983, 1984, 1985, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved. The Berkeley Software License Agreement
 * specifies the terms and conditions for redistribution.
 */

/*
 * 4.3BSD signal compatibility functions
 *
 * the implementation interprets signal masks equal to -1 as "all of the
 * signals in the signal set", thereby allowing signals with numbers
 * above 32 to be blocked when referenced in code such as:
 *
 *	for (i = 0; i < NSIG; i++)
 *		mask |= sigmask(i)
 */

#include <sys/types.h>
#include <sys/siginfo.h>
#include <ucontext.h>
#include <signal.h>
#include "signal.h"
#include <errno.h>
#include <stdio.h>

#define set2mask(setp) ((setp)->__sigbits[0])
#define mask2set(mask, setp) \
	((mask) == -1 ? sigfillset(setp) : sigemptyset(setp), (((setp)->__sigbits[0]) = (mask)))

void (*_siguhandler[NSIG])() = { 0 };

/*
 * sigstack is emulated with sigaltstack by guessing an appropriate
 * value for the stack size - on machines that have stacks that grow
 * upwards, the ss_sp arguments for both functions mean the same thing,
 * (the initial stack pointer sigstack() is also the stack base
 * sigaltstack()), so a "very large" value should be chosen for the
 * stack size - on machines that have stacks that grow downwards, the
 * ss_sp arguments mean opposite things, so 0 should be used (hopefully
 * these machines don't have hardware stack bounds registers that pay
 * attention to sigaltstack()'s size argument.
 */

#ifdef sun
#define SIGSTACKSIZE	0
#endif


/*
 * sigvechandler is the real signal handler installed for all
 * signals handled in the 4.3BSD compatibility interface - it translates
 * SVR4 signal hander arguments into 4.3BSD signal handler arguments
 * and then calls the real handler
 */

static void
sigvechandler(int sig, siginfo_t *sip, ucontext_t *ucp)
{
	struct sigcontext sc;
	int code;
	char *addr;
	int i, j;
	int gwinswitch = 0;

	sc.sc_onstack = ((ucp->uc_stack.ss_flags & SS_ONSTACK) != 0);
	sc.sc_mask = set2mask(&ucp->uc_sigmask);

	/*
	 * Machine dependent code begins
	 */
	sc.sc_sp = (int) ucp->uc_mcontext.gregs[UESP];
	sc.sc_pc = (int) ucp->uc_mcontext.gregs[EIP];
	sc.sc_ps = (int) ucp->uc_mcontext.gregs[EFL];
	sc.sc_eax = (int) ucp->uc_mcontext.gregs[EAX];
	sc.sc_edx = (int) ucp->uc_mcontext.gregs[EDX];

	/*
	 * Machine dependent code ends
	 */

	if (sip != NULL)
		if ((code = sip->si_code) == BUS_OBJERR)
			code = SEGV_MAKE_ERR(sip->si_errno);

	if (sig == SIGILL || sig == SIGFPE || sig == SIGSEGV || sig == SIGBUS)
		if (sip != NULL)
			addr = (char *)sip->si_addr;
	else
		addr = SIG_NOADDR;

	(*_siguhandler[sig])(sig, code, &sc, addr);

	if (sc.sc_onstack)
		ucp->uc_stack.ss_flags |= SS_ONSTACK;
	else
		ucp->uc_stack.ss_flags &= ~SS_ONSTACK;
	mask2set(sc.sc_mask, &ucp->uc_sigmask);

	/*
	 * Machine dependent code begins
	 */
	ucp->uc_mcontext.gregs[UESP] = (int) sc.sc_sp;
	ucp->uc_mcontext.gregs[EIP] = (int) sc.sc_pc;
	ucp->uc_mcontext.gregs[EFL] = (int) sc.sc_ps;
	ucp->uc_mcontext.gregs[EAX] = (int) sc.sc_eax;
	ucp->uc_mcontext.gregs[EDX] = (int) sc.sc_edx;
	/*
	 * Machine dependent code ends
	 */

	setcontext (ucp);
}

int
sigsetmask(int mask)
{
	sigset_t oset;
	sigset_t nset;

	(void) sigprocmask(0, (sigset_t *)0, &nset);
	mask2set(mask, &nset);
	(void) sigprocmask(SIG_SETMASK, &nset, &oset);
	return set2mask(&oset);
}

int
sigblock(int mask)
{
	sigset_t oset;
	sigset_t nset;

	(void) sigprocmask(0, (sigset_t *)0, &nset);
	mask2set(mask, &nset);
	(void) sigprocmask(SIG_BLOCK, &nset, &oset);
	return set2mask(&oset);
}

int
sigpause(int mask)
{
	sigset_t set;

	(void) sigprocmask(0, (sigset_t *)0, &set);
	mask2set(mask, &set);
	return (sigsuspend(&set));
}

int
sigvec(int sig, struct sigvec *nvec, struct sigvec *ovec)
{
        struct sigaction nact;
        struct sigaction oact;
        struct sigaction *nactp;
        void (*ohandler)(), (*nhandler)();

        if (sig <= 0 || sig >= NSIG) {
                errno = EINVAL;
                return -1;
        }

        ohandler = _siguhandler[sig];

        if (nvec) {
		_sigaction(sig, (struct sigaction *)0, &nact);
                nhandler = nvec->sv_handler;
                _siguhandler[sig] = nhandler;
                if (nhandler != SIG_DFL && nhandler != SIG_IGN)
                        nact.sa_handler = (void (*)())sigvechandler;
		else
			nact.sa_handler = nhandler;
		mask2set(nvec->sv_mask, &nact.sa_mask);
		/*
		if ( sig == SIGTSTP || sig == SIGSTOP )
			nact.sa_handler = SIG_DFL; 	*/
		nact.sa_flags = SA_SIGINFO;
		if (!(nvec->sv_flags & SV_INTERRUPT))
			nact.sa_flags |= SA_RESTART;
		if (nvec->sv_flags & SV_RESETHAND)
			nact.sa_flags |= SA_RESETHAND;
		if (nvec->sv_flags & SV_ONSTACK)
			nact.sa_flags |= SA_ONSTACK;
		nactp = &nact;
        } else
		nactp = (struct sigaction *)0;

        if (_sigaction(sig, nactp, &oact) < 0) {
                _siguhandler[sig] = ohandler;
                return -1;
        }

        if (ovec) {
		if (oact.sa_handler == SIG_DFL || oact.sa_handler == SIG_IGN)
			ovec->sv_handler = oact.sa_handler;
		else
			ovec->sv_handler = ohandler;
		ovec->sv_mask = set2mask(&oact.sa_mask);
		ovec->sv_flags = 0;
		if (oact.sa_flags & SA_ONSTACK)
			ovec->sv_flags |= SV_ONSTACK;
		if (oact.sa_flags & SA_RESETHAND)
			ovec->sv_flags |= SV_RESETHAND;
		if (!(oact.sa_flags & SA_RESTART))
			ovec->sv_flags |= SV_INTERRUPT;
	}

        return 0;
}


void (*
signal(int s, void (*a)()))()
{
        struct sigvec osv;
	struct sigvec nsv;
        static int mask[NSIG];
        static int flags[NSIG];

	nsv.sv_handler = a;
	nsv.sv_mask = mask[s];
	nsv.sv_flags = flags[s];
        if (sigvec(s, &nsv, &osv) < 0)
                return (SIG_ERR);
        if (nsv.sv_mask != osv.sv_mask || nsv.sv_flags != osv.sv_flags) {
                mask[s] = nsv.sv_mask = osv.sv_mask;
                flags[s] = nsv.sv_flags = osv.sv_flags & ~SV_RESETHAND;
                if (sigvec(s, &nsv, (struct sigvec *)0) < 0)
                        return (SIG_ERR);
        }
        return (osv.sv_handler);
}



/* ============================================================================
 * SOURCE 11/11: minix4\exokernel\kernel_legacy\cmd\csh\sparc\signal.c
 * Size: 7,126 bytes, Lines: 263
 * Hash: b5a1e895360a...
 * ============================================================================ */

/*
 * Copyright 2005 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1983, 1984, 1985, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved. The Berkeley Software License Agreement
 * specifies the terms and conditions for redistribution.
 */

/*
 * 4.3BSD signal compatibility functions
 *
 * the implementation interprets signal masks equal to -1 as "all of the
 * signals in the signal set", thereby allowing signals with numbers
 * above 32 to be blocked when referenced in code such as:
 *
 *	for (i = 0; i < NSIG; i++)
 *		mask |= sigmask(i)
 */

#include <sys/types.h>
#include <sys/siginfo.h>
#include <ucontext.h>
#include <signal.h>
#include "signal.h"
#include <errno.h>
#include <stdio.h>

#define set2mask(setp) ((setp)->__sigbits[0])
#define mask2set(mask, setp) \
	((mask) == -1 ? sigfillset(setp) : sigemptyset(setp), (((setp)->__sigbits[0]) = (mask)))

void (*_siguhandler[NSIG])() = { 0 };

/*
 * sigstack is emulated with sigaltstack by guessing an appropriate
 * value for the stack size - on machines that have stacks that grow
 * upwards, the ss_sp arguments for both functions mean the same thing,
 * (the initial stack pointer sigstack() is also the stack base
 * sigaltstack()), so a "very large" value should be chosen for the
 * stack size - on machines that have stacks that grow downwards, the
 * ss_sp arguments mean opposite things, so 0 should be used (hopefully
 * these machines don't have hardware stack bounds registers that pay
 * attention to sigaltstack()'s size argument.
 */

#ifdef sun
#define SIGSTACKSIZE	0
#endif


/*
 * sigvechandler is the real signal handler installed for all
 * signals handled in the 4.3BSD compatibility interface - it translates
 * SVR4 signal hander arguments into 4.3BSD signal handler arguments
 * and then calls the real handler
 */

static void
sigvechandler(int sig, siginfo_t *sip, ucontext_t *ucp)
{
	struct sigcontext sc;
	int code;
	char *addr;
	int i, j;
	int gwinswitch = 0;

	sc.sc_onstack = ((ucp->uc_stack.ss_flags & SS_ONSTACK) != 0);
	sc.sc_mask = set2mask(&ucp->uc_sigmask);

	/*
	 * Machine dependent code begins
	 */
	sc.sc_sp = ucp->uc_mcontext.gregs[REG_O6];
	sc.sc_pc = ucp->uc_mcontext.gregs[REG_PC];
	sc.sc_npc = ucp->uc_mcontext.gregs[REG_nPC];
	sc.sc_psr = ucp->uc_mcontext.gregs[REG_PSR];
	sc.sc_g1 = ucp->uc_mcontext.gregs[REG_G1];
	sc.sc_o0 = ucp->uc_mcontext.gregs[REG_O0];
	if (ucp->uc_mcontext.gwins != (gwindows_t *)0) {
		gwinswitch = 1;
		sc.sc_wbcnt = ucp->uc_mcontext.gwins->wbcnt;
		for (i = 0; i < MAXWINDOW; i++) {
			for (j = 0; j < 16; j++)
				sc.sc_spbuf[i][j] = (int)ucp->uc_mcontext.gwins->spbuf[j];
			for (j = 0; j < 8; j++)
				sc.sc_wbuf[i][j] = ucp->uc_mcontext.gwins->wbuf[i].rw_local[j];
			for (j = 0; j < 8; j++)
				sc.sc_wbuf[i][j+8] = ucp->uc_mcontext.gwins->wbuf[i].rw_in[j];
		}
	}
	/*
	 * Machine dependent code ends
	 */

	if (sip != NULL)
		if ((code = sip->si_code) == BUS_OBJERR)
			code = SEGV_MAKE_ERR(sip->si_errno);

	if (sig == SIGILL || sig == SIGFPE || sig == SIGSEGV || sig == SIGBUS)
		if (sip != NULL)
			addr = (char *)sip->si_addr;
	else
		addr = SIG_NOADDR;

	(*_siguhandler[sig])(sig, code, &sc, addr);

	if (sc.sc_onstack)
		ucp->uc_stack.ss_flags |= SS_ONSTACK;
	else
		ucp->uc_stack.ss_flags &= ~SS_ONSTACK;
	mask2set(sc.sc_mask, &ucp->uc_sigmask);

	/*
	 * Machine dependent code begins
	 */
	ucp->uc_mcontext.gregs[REG_O6] = sc.sc_sp;
	ucp->uc_mcontext.gregs[REG_PC] = sc.sc_pc;
	ucp->uc_mcontext.gregs[REG_nPC] = sc.sc_npc;
	ucp->uc_mcontext.gregs[REG_PSR] = sc.sc_psr;
	ucp->uc_mcontext.gregs[REG_G1] = sc.sc_g1;
	ucp->uc_mcontext.gregs[REG_O0] = sc.sc_o0;
	if (gwinswitch == 1) {
		ucp->uc_mcontext.gwins->wbcnt = sc.sc_wbcnt;
		for (i = 0; i < MAXWINDOW; i++) {
			for (j = 0; j < 16; j++)
				ucp->uc_mcontext.gwins->spbuf[j] = (greg_t *)sc.sc_spbuf[i][j];
			for (j = 0; j < 8; j++)
				ucp->uc_mcontext.gwins->wbuf[i].rw_local[j] = sc.sc_wbuf[i][j];
			for (j = 0; j < 8; j++)
				ucp->uc_mcontext.gwins->wbuf[i].rw_in[j] = sc.sc_wbuf[i][j+8];
		}
	}
	/*
	 * Machine dependent code ends
	 */

	setcontext (ucp);
}

int
sigsetmask(int mask)
{
	sigset_t oset;
	sigset_t nset;

	(void) sigprocmask(0, (sigset_t *)0, &nset);
	mask2set(mask, &nset);
	(void) sigprocmask(SIG_SETMASK, &nset, &oset);
	return set2mask(&oset);
}

int
sigblock(int mask)
{
	sigset_t oset;
	sigset_t nset;

	(void) sigprocmask(0, (sigset_t *)0, &nset);
	mask2set(mask, &nset);
	(void) sigprocmask(SIG_BLOCK, &nset, &oset);
	return set2mask(&oset);
}

int
sigpause(int mask)
{
	sigset_t set;

	(void) sigprocmask(0, (sigset_t *)0, &set);
	mask2set(mask, &set);
	return (sigsuspend(&set));
}

int
sigvec(int sig, struct sigvec *nvec, struct sigvec *ovec)
{
        struct sigaction nact;
        struct sigaction oact;
        struct sigaction *nactp;
        void (*ohandler)(), (*nhandler)();

        if (sig <= 0 || sig >= NSIG) {
                errno = EINVAL;
                return -1;
        }

        ohandler = _siguhandler[sig];

        if (nvec) {
		_sigaction(sig, (struct sigaction *)0, &nact);
                nhandler = nvec->sv_handler;
                _siguhandler[sig] = nhandler;
                if (nhandler != SIG_DFL && nhandler != SIG_IGN)
                        nact.sa_handler = (void (*)())sigvechandler;
		else
			nact.sa_handler = nhandler;
		mask2set(nvec->sv_mask, &nact.sa_mask);
		/*
		if ( sig == SIGTSTP || sig == SIGSTOP )
			nact.sa_handler = SIG_DFL; 	*/
		nact.sa_flags = SA_SIGINFO;
		if (!(nvec->sv_flags & SV_INTERRUPT))
			nact.sa_flags |= SA_RESTART;
		if (nvec->sv_flags & SV_RESETHAND)
			nact.sa_flags |= SA_RESETHAND;
		if (nvec->sv_flags & SV_ONSTACK)
			nact.sa_flags |= SA_ONSTACK;
		nactp = &nact;
        } else
		nactp = (struct sigaction *)0;

        if (_sigaction(sig, nactp, &oact) < 0) {
                _siguhandler[sig] = ohandler;
                return -1;
        }

        if (ovec) {
		if (oact.sa_handler == SIG_DFL || oact.sa_handler == SIG_IGN)
			ovec->sv_handler = oact.sa_handler;
		else
			ovec->sv_handler = ohandler;
		ovec->sv_mask = set2mask(&oact.sa_mask);
		ovec->sv_flags = 0;
		if (oact.sa_flags & SA_ONSTACK)
			ovec->sv_flags |= SV_ONSTACK;
		if (oact.sa_flags & SA_RESETHAND)
			ovec->sv_flags |= SV_RESETHAND;
		if (!(oact.sa_flags & SA_RESTART))
			ovec->sv_flags |= SV_INTERRUPT;
	}

        return 0;
}


void (*
signal(int s, void (*a)()))()
{
        struct sigvec osv;
	struct sigvec nsv;
        static int mask[NSIG];
        static int flags[NSIG];

	nsv.sv_handler = a;
	nsv.sv_mask = mask[s];
	nsv.sv_flags = flags[s];
        if (sigvec(s, &nsv, &osv) < 0)
                return (SIG_ERR);
        if (nsv.sv_mask != osv.sv_mask || nsv.sv_flags != osv.sv_flags) {
                mask[s] = nsv.sv_mask = osv.sv_mask;
                flags[s] = nsv.sv_flags = osv.sv_flags & ~SV_RESETHAND;
                if (sigvec(s, &nsv, (struct sigvec *)0) < 0)
                        return (SIG_ERR);
        }
        return (osv.sv_handler);
}

