/**
 * @file syscall_unified.c
 * @brief Unified system calls
 * @details This file is a synthesized/unified implementation combining multiple
 *          legacy MINIX implementations into a single, modern, C23-compliant
 *          implementation for MINIX4.
 *
 * Original sources consolidated:
 * - minix4\libos\lib_legacy\libc\sys\syscall.c
 * - minix4\exokernel\kernel_legacy\syscall.c
 * - minix4\exokernel\kernel_legacy\uts\sparc\os\syscall.c
 * - minix4\exokernel\kernel_legacy\uts\intel\os\syscall.c

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
 * This file contains 4 very different implementations that
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
 * SOURCE 1/4: minix4\libos\lib_legacy\libc\sys\syscall.c
 * Size: 504 bytes, Lines: 26
 * Hash: f78c281b5b2d...
 * ============================================================================ */

#include <sys/cdefs.h>
#include <lib.h>
#include "namespace.h"

#ifdef __weak_alias
__weak_alias(syscall, _syscall)
#endif

int _syscall(endpoint_t who, int syscallnr, message *msgptr)
{
  int status;

  msgptr->m_type = syscallnr;
  status = ipc_sendrec(who, msgptr);
  if (status != 0) {
	/* 'ipc_sendrec' itself failed. */
	/* XXX - strerror doesn't know all the codes */
	msgptr->m_type = status;
  }
  if (msgptr->m_type < 0) {
	errno = -msgptr->m_type;
	return(-1);
  }
  return(msgptr->m_type);
}



/* ============================================================================
 * SOURCE 2/4: minix4\exokernel\kernel_legacy\syscall.c
 * Size: 6,095 bytes, Lines: 238
 * Hash: 580ecd7865b9...
 * ============================================================================ */


// clang-format off
#include "types.h"
#include "syscall.h"
#include "defs.h"
#include "memlayout.h"
#include "mmu.h"
#include "param.h"
#include "proc.h"
#include "x86.h"
// clang-format on


// User code makes a system call with INT T_SYSCALL.
// System call number in %eax.
// Arguments on the stack, from the user call to the C
// library system call function. The saved user %esp points
// to a saved program counter, and then the first argument.

// Fetch the int at addr from the current process.
int fetchint(uintptr_t addr, int *ip) {
  struct proc *curproc = myproc();

  if (addr >= curproc->sz || addr + 4 > curproc->sz)
    return -1;
  *ip = *(int *)(addr);
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int fetchstr(uintptr_t addr, char **pp) {
  char *s, *ep;
  struct proc *curproc = myproc();

  if (addr >= curproc->sz)
    return -1;
  *pp = (char *)addr;
  ep = (char *)curproc->sz;
  for (s = *pp; s < ep; s++) {
    if (*s == 0)
      return s - *pp;
  }
  return -1;
}

// Fetch the nth 32-bit system call argument.
int argint(int n, int *ip) {
#ifndef __x86_64__
  return fetchint((myproc()->tf->esp) + 4 + 4 * n, ip);
#else
  uint64_t val;
  struct trapframe *tf = myproc()->tf;
  switch (n) {
  case 0:
    val = tf->rdi;
    break;
  case 1:
    val = tf->rsi;
    break;
  case 2:
    val = tf->rdx;
    break;
  case 3:
    val = tf->r10;
    break;
  case 4:
    val = tf->r8;
    break;
  case 5:
    val = tf->r9;
    break;
  default:
    return -1;
  }
  *ip = val;
  return 0;
#endif
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size bytes.  Check that the pointer
// lies within the process address space.
int
argptr(int n, char **pp, size_t size)
{
  struct proc *curproc = myproc();
#ifndef __x86_64__
  int i;
  if (argint(n, &i) < 0)
    return -1;
  if (size < 0 || (uint32_t)i >= curproc->sz || (uint32_t)i + size > curproc->sz)
    return -1;
  *pp = (char *)i;
  return 0;
#else
  uint64_t addr;
  if (argint(n, (int *)&addr) < 0)
    return -1;
  if (size < 0 || addr >= curproc->sz || addr + size > curproc->sz)
    return -1;
  *pp = (char *)addr;
  return 0;
#endif
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int argstr(int n, char **pp) {
#ifndef __x86_64__
  int addr;
  if (argint(n, &addr) < 0)
    return -1;
  return fetchstr(addr, pp);
#else
  uint64_t addr;
  if (argint(n, (int *)&addr) < 0)
    return -1;
  return fetchstr(addr, pp);
#endif
}

extern int sys_chdir(void);
extern int sys_close(void);
extern int sys_dup(void);
extern int sys_exec(void);
extern int sys_exit(void);
extern int sys_fork(void);
extern int sys_getpid(void);
extern int sys_kill(void);
extern int sys_pipe(void);
extern int sys_sbrk(void);
extern int sys_sleep(void);
extern int sys_wait(void);
extern int sys_uptime(void);
extern int sys_mappte(void);
extern int sys_set_timer_upcall(void);
extern int sys_exo_alloc_page(void);
extern int sys_exo_unbind_page(void);
extern int sys_exo_alloc_block(void);
extern int sys_exo_bind_block(void);
extern int sys_exo_yield_to(void);
extern int sys_exo_read_disk(void);
extern int sys_exo_write_disk(void);
extern int sys_exo_send(void);
extern int sys_exo_recv(void);
extern int sys_exo_recv_timed(void);
extern int sys_exo_alloc_ioport(void);
extern int sys_exo_bind_irq(void);
extern int sys_exo_alloc_dma(void);
extern int sys_endpoint_send(void);
extern int sys_endpoint_recv(void);
extern int sys_proc_alloc(void);
extern int sys_set_gas(void);
extern int sys_get_gas(void);
extern int sys_set_numa_node(void);
extern int sys_ipc_fast(void);
extern int sys_fcntl(void);
extern int sys_sigsend(void);
extern int sys_sigcheck(void);
extern int sys_cap_inc(void);
extern int sys_cap_dec(void);

static int (*syscalls[])(void) = {
    [SYS_fork] sys_fork,
    [SYS_exit] sys_exit,
    [SYS_wait] sys_wait,
    [SYS_pipe] sys_pipe,
    [SYS_kill] sys_kill,
    [SYS_exec] sys_exec,
    [SYS_getpid] sys_getpid,
    [SYS_sbrk] sys_sbrk,
    [SYS_sleep] sys_sleep,
    [SYS_uptime] sys_uptime,
    [SYS_mappte] sys_mappte,
    [SYS_set_timer_upcall] sys_set_timer_upcall,
    [SYS_exo_alloc_page] sys_exo_alloc_page,
    [SYS_exo_unbind_page] sys_exo_unbind_page,
    [SYS_exo_alloc_block] sys_exo_alloc_block,
    [SYS_exo_bind_block] sys_exo_bind_block,
    [SYS_exo_yield_to] sys_exo_yield_to,
    [SYS_exo_read_disk] sys_exo_read_disk,
    [SYS_exo_write_disk] sys_exo_write_disk,
    [SYS_exo_alloc_ioport] sys_exo_alloc_ioport,
    [SYS_exo_bind_irq] sys_exo_bind_irq,
    [SYS_exo_alloc_dma] sys_exo_alloc_dma,
    [SYS_exo_send] sys_exo_send,
    [SYS_exo_recv] sys_exo_recv,
    [SYS_exo_recv_timed] sys_exo_recv_timed,
    [SYS_endpoint_send] sys_endpoint_send,
    [SYS_endpoint_recv] sys_endpoint_recv,
    [SYS_proc_alloc] sys_proc_alloc,
    [SYS_set_gas] sys_set_gas,
    [SYS_get_gas] sys_get_gas,
    [SYS_set_numa_node] sys_set_numa_node,
    [SYS_fcntl] sys_fcntl,
    [SYS_sigsend] sys_sigsend,
    [SYS_sigcheck] sys_sigcheck,
    [SYS_cap_inc] sys_cap_inc,
    [SYS_cap_dec] sys_cap_dec,
    [SYS_ipc_fast] sys_ipc_fast,
};

void syscall(void) {
  int num;
  struct proc *curproc = myproc();
#ifndef __x86_64__
  num = curproc->tf->eax;
#else
  num = curproc->tf->rax;
#endif
  if(num == 0x30){
#ifdef __x86_64__
    curproc->tf->rax = sys_ipc_fast();
#else
    curproc->tf->eax = sys_ipc_fast();
#endif
    return;
  }
  if (num > 0 && num < NELEM(syscalls) && syscalls[num]) {
#ifndef __x86_64__
    curproc->tf->eax = syscalls[num]();
#else
    curproc->tf->rax = syscalls[num]();
#endif
  } else {
    cprintf("%d %s: unknown sys call %d\n", curproc->pid, curproc->name, num);
#ifndef __x86_64__
    curproc->tf->eax = -1;
#else
    curproc->tf->rax = -1;
#endif
  }
}



/* ============================================================================
 * SOURCE 3/4: minix4\exokernel\kernel_legacy\uts\sparc\os\syscall.c
 * Size: 30,506 bytes, Lines: 1,216
 * Hash: bae9fdcc412b...
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
 * Copyright (c) 1991, 2010, Oracle and/or its affiliates. All rights reserved.
 * Copyright 2019 Joyent, Inc.
 */

#include <sys/param.h>
#include <sys/vmparam.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <sys/systm.h>
#include <sys/cmn_err.h>
#include <sys/signal.h>
#include <sys/stack.h>
#include <sys/cred.h>
#include <sys/user.h>
#include <sys/debug.h>
#include <sys/errno.h>
#include <sys/proc.h>
#include <sys/var.h>
#include <sys/inline.h>
#include <sys/syscall.h>
#include <sys/ucontext.h>
#include <sys/cpuvar.h>
#include <sys/siginfo.h>
#include <sys/trap.h>
#include <sys/machtrap.h>
#include <sys/sysinfo.h>
#include <sys/procfs.h>
#include <sys/prsystm.h>
#include <sys/fpu/fpusystm.h>
#include <sys/modctl.h>
#include <sys/aio_impl.h>
#include <c2/audit.h>
#include <sys/machpcb.h>
#include <sys/privregs.h>
#include <sys/copyops.h>
#include <sys/timer.h>
#include <sys/priv.h>
#include <sys/msacct.h>

int syscalltrace = 0;
#ifdef SYSCALLTRACE
static kmutex_t	systrace_lock;		/* syscall tracing lock */
#endif /* SYSCALLTRACE */

static krwlock_t *lock_syscall(struct sysent *, uint_t);

#ifdef _SYSCALL32_IMPL
static struct sysent *
lwp_getsysent(klwp_t *lwp)
{
	if (lwp_getdatamodel(lwp) == DATAMODEL_NATIVE)
		return (sysent);
	return (sysent32);
}
#define	LWP_GETSYSENT(lwp)	(lwp_getsysent(lwp))
#else
#define	LWP_GETSYSENT(lwp)	(sysent)
#endif

/*
 * Called to restore the lwp's register window just before
 * returning to user level (only if the registers have been
 * fetched or modified through /proc).
 */
/*ARGSUSED1*/
void
xregrestore(klwp_t *lwp, int shared)
{
	/*
	 * If locals+ins were modified by /proc copy them out.
	 * Also copy to the shared window, if necessary.
	 */
	if (lwp->lwp_pcb.pcb_xregstat == XREGMODIFIED) {
		struct machpcb *mpcb = lwptompcb(lwp);
		caddr_t sp = (caddr_t)lwptoregs(lwp)->r_sp;

		size_t rwinsize;
		caddr_t rwp;
		int is64;

		if (lwp_getdatamodel(lwp) == DATAMODEL_LP64) {
			rwinsize = sizeof (struct rwindow);
			rwp = sp + STACK_BIAS;
			is64 = 1;
		} else {
			rwinsize = sizeof (struct rwindow32);
			sp = (caddr_t)(uintptr_t)(caddr32_t)(uintptr_t)sp;
			rwp = sp;
			is64 = 0;
		}

		if (is64)
			(void) copyout_nowatch(&lwp->lwp_pcb.pcb_xregs,
			    rwp, rwinsize);
		else {
			struct rwindow32 rwindow32;
			int watched;

			watched = watch_disable_addr(rwp, rwinsize, S_WRITE);
			rwindow_nto32(&lwp->lwp_pcb.pcb_xregs, &rwindow32);
			(void) copyout(&rwindow32, rwp, rwinsize);
			if (watched)
				watch_enable_addr(rwp, rwinsize, S_WRITE);
		}

		/* also copy to the user return window */
		mpcb->mpcb_rsp[0] = sp;
		mpcb->mpcb_rsp[1] = NULL;
		bcopy(&lwp->lwp_pcb.pcb_xregs, &mpcb->mpcb_rwin[0],
		    sizeof (lwp->lwp_pcb.pcb_xregs));
	}
	lwp->lwp_pcb.pcb_xregstat = XREGNONE;
}


/*
 * Get the arguments to the current system call.
 *	lwp->lwp_ap normally points to the out regs in the reg structure.
 *	If the user is going to change the out registers and might want to
 *	get the args (for /proc tracing), it must copy the args elsewhere
 *	via save_syscall_args().
 */
uint_t
get_syscall_args(klwp_t *lwp, long *argp, int *nargsp)
{
	kthread_t	*t = lwptot(lwp);
	uint_t	code = t->t_sysnum;
	long	mask;
	long	*ap;
	int	nargs;

	if (lwptoproc(lwp)->p_model == DATAMODEL_ILP32)
		mask = (uint32_t)0xffffffffU;
	else
		mask = 0xffffffffffffffff;

	if (code != 0 && code < NSYSCALL) {

		nargs = LWP_GETSYSENT(lwp)[code].sy_narg;

		ASSERT(nargs <= MAXSYSARGS);

		*nargsp = nargs;
		ap = lwp->lwp_ap;
		while (nargs-- > 0)
			*argp++ = *ap++ & mask;
	} else {
		*nargsp = 0;
	}
	return (code);
}

#ifdef _SYSCALL32_IMPL
/*
 * Get the arguments to the current 32-bit system call.
 */
uint_t
get_syscall32_args(klwp_t *lwp, int *argp, int *nargsp)
{
	long args[MAXSYSARGS];
	uint_t i, code;

	code = get_syscall_args(lwp, args, nargsp);
	for (i = 0; i != *nargsp; i++)
		*argp++ = (int)args[i];
	return (code);
}
#endif

/*
 *	Save the system call arguments in a safe place.
 *	lwp->lwp_ap normally points to the out regs in the reg structure.
 *	If the user is going to change the out registers, g1, or the stack,
 *	and might want to get the args (for /proc tracing), it must copy
 *	the args elsewhere via save_syscall_args().
 *
 *	This may be called from stop() even when we're not in a system call.
 *	Since there's no easy way to tell, this must be safe (not panic).
 *	If the copyins get data faults, return non-zero.
 */
int
save_syscall_args()
{
	kthread_t	*t = curthread;
	klwp_t		*lwp = ttolwp(t);
	struct regs	*rp = lwptoregs(lwp);
	uint_t		code = t->t_sysnum;
	uint_t		nargs;
	int		i;
	caddr_t		ua;
	model_t		datamodel;

	if (lwp->lwp_argsaved || code == 0)
		return (0);		/* args already saved or not needed */

	if (code >= NSYSCALL) {
		nargs = 0;		/* illegal syscall */
	} else {
		struct sysent *se = LWP_GETSYSENT(lwp);
		struct sysent *callp = se + code;

		nargs = callp->sy_narg;
		if (LOADABLE_SYSCALL(callp) && nargs == 0) {
			krwlock_t	*module_lock;

			/*
			 * Find out how many arguments the system
			 * call uses.
			 *
			 * We have the property that loaded syscalls
			 * never change the number of arguments they
			 * use after they've been loaded once.  This
			 * allows us to stop for /proc tracing without
			 * holding the module lock.
			 * /proc is assured that sy_narg is valid.
			 */
			module_lock = lock_syscall(se, code);
			nargs = callp->sy_narg;
			rw_exit(module_lock);
		}
	}

	/*
	 * Fetch the system call arguments.
	 */
	if (nargs == 0)
		goto out;


	ASSERT(nargs <= MAXSYSARGS);

	if ((datamodel = lwp_getdatamodel(lwp)) == DATAMODEL_ILP32) {

		if (rp->r_g1 == 0) {	/* indirect syscall */

			lwp->lwp_arg[0] = (uint32_t)rp->r_o1;
			lwp->lwp_arg[1] = (uint32_t)rp->r_o2;
			lwp->lwp_arg[2] = (uint32_t)rp->r_o3;
			lwp->lwp_arg[3] = (uint32_t)rp->r_o4;
			lwp->lwp_arg[4] = (uint32_t)rp->r_o5;
			if (nargs > 5) {
				ua = (caddr_t)(uintptr_t)(caddr32_t)(uintptr_t)
				    (rp->r_sp + MINFRAME32);
				for (i = 5; i < nargs; i++) {
					uint32_t a;
					if (fuword32(ua, &a) != 0)
						return (-1);
					lwp->lwp_arg[i] = a;
					ua += sizeof (a);
				}
			}
		} else {
			lwp->lwp_arg[0] = (uint32_t)rp->r_o0;
			lwp->lwp_arg[1] = (uint32_t)rp->r_o1;
			lwp->lwp_arg[2] = (uint32_t)rp->r_o2;
			lwp->lwp_arg[3] = (uint32_t)rp->r_o3;
			lwp->lwp_arg[4] = (uint32_t)rp->r_o4;
			lwp->lwp_arg[5] = (uint32_t)rp->r_o5;
			if (nargs > 6) {
				ua = (caddr_t)(uintptr_t)(caddr32_t)(uintptr_t)
				    (rp->r_sp + MINFRAME32);
				for (i = 6; i < nargs; i++) {
					uint32_t a;
					if (fuword32(ua, &a) != 0)
						return (-1);
					lwp->lwp_arg[i] = a;
					ua += sizeof (a);
				}
			}
		}
	} else {
		ASSERT(datamodel == DATAMODEL_LP64);
		lwp->lwp_arg[0] = rp->r_o0;
		lwp->lwp_arg[1] = rp->r_o1;
		lwp->lwp_arg[2] = rp->r_o2;
		lwp->lwp_arg[3] = rp->r_o3;
		lwp->lwp_arg[4] = rp->r_o4;
		lwp->lwp_arg[5] = rp->r_o5;
		if (nargs > 6) {
			ua = (caddr_t)rp->r_sp + MINFRAME + STACK_BIAS;
			for (i = 6; i < nargs; i++) {
				unsigned long a;
				if (fulword(ua, &a) != 0)
					return (-1);
				lwp->lwp_arg[i] = a;
				ua += sizeof (a);
			}
		}
	}

out:
	lwp->lwp_ap = lwp->lwp_arg;
	lwp->lwp_argsaved = 1;
	t->t_post_sys = 1;	/* so lwp_ap will be reset */
	return (0);
}

void
reset_syscall_args(void)
{
	klwp_t *lwp = ttolwp(curthread);

	lwp->lwp_ap = (long *)&lwptoregs(lwp)->r_o0;
	lwp->lwp_argsaved = 0;
}

/*
 * nonexistent system call-- signal lwp (may want to handle it)
 * flag error if lwp won't see signal immediately
 * This works for old or new calling sequence.
 */
int64_t
nosys(void)
{
	tsignal(curthread, SIGSYS);
	return ((int64_t)set_errno(ENOSYS));
}

int
nosys32(void)
{
	return (nosys());
}

/*
 * Perform pre-system-call processing, including stopping for tracing,
 * auditing, microstate-accounting, etc.
 *
 * This routine is called only if the t_pre_sys flag is set.  Any condition
 * requiring pre-syscall handling must set the t_pre_sys flag.  If the
 * condition is persistent, this routine will repost t_pre_sys.
 */
int
pre_syscall(int arg0)
{
	unsigned int code;
	kthread_t *t = curthread;
	proc_t *p = ttoproc(t);
	klwp_t *lwp = ttolwp(t);
	struct regs *rp = lwptoregs(lwp);
	int	repost;

	t->t_pre_sys = repost = 0;	/* clear pre-syscall processing flag */

	ASSERT(t->t_schedflag & TS_DONT_SWAP);

	syscall_mstate(LMS_USER, LMS_SYSTEM);

	/*
	 * The syscall arguments in the out registers should be pointed to
	 * by lwp_ap.  If the args need to be copied so that the outs can
	 * be changed without losing the ability to get the args for /proc,
	 * they can be saved by save_syscall_args(), and lwp_ap will be
	 * restored by post_syscall().
	 */
	ASSERT(lwp->lwp_ap == (long *)&rp->r_o0);

	/*
	 * Make sure the thread is holding the latest credentials for the
	 * process.  The credentials in the process right now apply to this
	 * thread for the entire system call.
	 */
	if (t->t_cred != p->p_cred) {
		cred_t *oldcred = t->t_cred;
		/*
		 * DTrace accesses t_cred in probe context.  t_cred must
		 * always be either NULL, or point to a valid, allocated cred
		 * structure.
		 */
		t->t_cred = crgetcred();
		crfree(oldcred);
	}

	/*
	 * Undo special arrangements to single-step the lwp
	 * so that a debugger will see valid register contents.
	 * Also so that the pc is valid for syncfpu().
	 * Also so that a syscall like exec() can be stepped.
	 */
	if (lwp->lwp_pcb.pcb_step != STEP_NONE) {
		(void) prundostep();
		repost = 1;
	}

	/*
	 * Check for indirect system call in case we stop for tracing.
	 * Don't allow multiple indirection.
	 */
	code = t->t_sysnum;
	if (code == 0 && arg0 != 0) {		/* indirect syscall */
		code = arg0;
		t->t_sysnum = arg0;
	}

	/*
	 * From the proc(5) manual page:
	 * When entry to a system call is being traced, the traced process
	 * stops after having begun the call to the system but before the
	 * system call arguments have been fetched from the process.
	 * If proc changes the args we must refetch them after starting.
	 */
	if (PTOU(p)->u_systrap) {
		if (prismember(&PTOU(p)->u_entrymask, code)) {
			/*
			 * Recheck stop condition, now that lock is held.
			 */
			mutex_enter(&p->p_lock);
			if (PTOU(p)->u_systrap &&
			    prismember(&PTOU(p)->u_entrymask, code)) {
				stop(PR_SYSENTRY, code);
				/*
				 * Must refetch args since they were
				 * possibly modified by /proc.  Indicate
				 * that the valid copy is in the
				 * registers.
				 */
				lwp->lwp_argsaved = 0;
				lwp->lwp_ap = (long *)&rp->r_o0;
			}
			mutex_exit(&p->p_lock);
		}
		repost = 1;
	}

	if (lwp->lwp_sysabort) {
		/*
		 * lwp_sysabort may have been set via /proc while the process
		 * was stopped on PR_SYSENTRY.  If so, abort the system call.
		 * Override any error from the copyin() of the arguments.
		 */
		lwp->lwp_sysabort = 0;
		(void) set_errno(EINTR); /* sets post-sys processing */
		t->t_pre_sys = 1;	/* repost anyway */
		return (1);		/* don't do system call, return EINTR */
	}

	/* begin auditing for this syscall */
	if (audit_active == C2AUDIT_LOADED) {
		uint32_t auditing = au_zone_getstate(NULL);

		if (auditing & AU_AUDIT_MASK) {
			int error;
			if (error = audit_start(T_SYSCALL, code, auditing, \
			    0, lwp)) {
				t->t_pre_sys = 1;	/* repost anyway */
				lwp->lwp_error = 0;	/* for old drivers */
				return (error);
			}
			repost = 1;
		}
	}

#ifdef SYSCALLTRACE
	if (syscalltrace) {
		int i;
		long *ap;
		char *cp;
		char *sysname;
		struct sysent *callp;

		if (code >= NSYSCALL)
			callp = &nosys_ent;	/* nosys has no args */
		else
			callp = LWP_GETSYSENT(lwp) + code;
		(void) save_syscall_args();
		mutex_enter(&systrace_lock);
		printf("%d: ", p->p_pid);
		if (code >= NSYSCALL)
			printf("0x%x", code);
		else {
			sysname = mod_getsysname(code);
			printf("%s[0x%x]", sysname == NULL ? "NULL" :
			    sysname, code);
		}
		cp = "(";
		for (i = 0, ap = lwp->lwp_ap; i < callp->sy_narg; i++, ap++) {
			printf("%s%lx", cp, *ap);
			cp = ", ";
		}
		if (i)
			printf(")");
		printf(" %s id=0x%p\n", PTOU(p)->u_comm, curthread);
		mutex_exit(&systrace_lock);
	}
#endif /* SYSCALLTRACE */

	/*
	 * If there was a continuing reason for pre-syscall processing,
	 * set the t_pre_sys flag for the next system call.
	 */
	if (repost)
		t->t_pre_sys = 1;
	lwp->lwp_error = 0;	/* for old drivers */
	lwp->lwp_badpriv = PRIV_NONE;	/* for privilege tracing */
	return (0);
}

/*
 * Post-syscall processing.  Perform abnormal system call completion
 * actions such as /proc tracing, profiling, signals, preemption, etc.
 *
 * This routine is called only if t_post_sys, t_sig_check, or t_astflag is set.
 * Any condition requiring pre-syscall handling must set one of these.
 * If the condition is persistent, this routine will repost t_post_sys.
 */
void
post_syscall(long rval1, long rval2)
{
	kthread_t	*t = curthread;
	proc_t	*p = curproc;
	klwp_t	*lwp = ttolwp(t);
	struct regs *rp = lwptoregs(lwp);
	uint_t	error;
	int	code = t->t_sysnum;
	int	repost = 0;
	int	proc_stop = 0;		/* non-zero if stopping for /proc */
	int	sigprof = 0;		/* non-zero if sending SIGPROF */

	t->t_post_sys = 0;

	error = lwp->lwp_errno;

	/*
	 * Code can be zero if this is a new LWP returning after a forkall(),
	 * other than the one which matches the one in the parent which called
	 * forkall().  In these LWPs, skip most of post-syscall activity.
	 */
	if (code == 0)
		goto sig_check;

	/* put out audit record for this syscall */
	if (AU_AUDITING()) {
		rval_t	rval;	/* fix audit_finish() someday */

		/* XX64 -- truncation of 64-bit return values? */
		rval.r_val1 = (int)rval1;
		rval.r_val2 = (int)rval2;
		audit_finish(T_SYSCALL, code, error, &rval);
		repost = 1;
	}

	if (curthread->t_pdmsg != NULL) {
		char *m = curthread->t_pdmsg;

		uprintf("%s", m);
		kmem_free(m, strlen(m) + 1);
		curthread->t_pdmsg = NULL;
	}

	/*
	 * If we're going to stop for /proc tracing, set the flag and
	 * save the arguments so that the return values don't smash them.
	 */
	if (PTOU(p)->u_systrap) {
		if (prismember(&PTOU(p)->u_exitmask, code)) {
			proc_stop = 1;
			(void) save_syscall_args();
		}
		repost = 1;
	}

	/*
	 * Similarly check to see if SIGPROF might be sent.
	 */
	if (curthread->t_rprof != NULL &&
	    curthread->t_rprof->rp_anystate != 0) {
		(void) save_syscall_args();
		sigprof = 1;
	}

	if (lwp->lwp_eosys == NORMALRETURN) {
		if (error == 0) {
#ifdef SYSCALLTRACE
			if (syscalltrace) {
				mutex_enter(&systrace_lock);
				printf(
				    "%d: r_val1=0x%lx, r_val2=0x%lx, id 0x%p\n",
				    p->p_pid, rval1, rval2, curthread);
				mutex_exit(&systrace_lock);
			}
#endif /* SYSCALLTRACE */
			rp->r_tstate &= ~TSTATE_IC;
			rp->r_o0 = rval1;
			rp->r_o1 = rval2;
		} else {
			int sig;

#ifdef SYSCALLTRACE
			if (syscalltrace) {
				mutex_enter(&systrace_lock);
				printf("%d: error=%d, id 0x%p\n",
				    p->p_pid, error, curthread);
				mutex_exit(&systrace_lock);
			}
#endif /* SYSCALLTRACE */
			if (error == EINTR && t->t_activefd.a_stale)
				error = EBADF;
			if (error == EINTR &&
			    (sig = lwp->lwp_cursig) != 0 &&
			    sigismember(&PTOU(p)->u_sigrestart, sig) &&
			    PTOU(p)->u_signal[sig - 1] != SIG_DFL &&
			    PTOU(p)->u_signal[sig - 1] != SIG_IGN)
				error = ERESTART;
			rp->r_o0 = error;
			rp->r_tstate |= TSTATE_IC;
		}
		/*
		 * The default action is to redo the trap instruction.
		 * We increment the pc and npc past it for NORMALRETURN.
		 * JUSTRETURN has set up a new pc and npc already.
		 * If we are a cloned thread of forkall(), don't
		 * adjust here because we have already inherited
		 * the adjusted values from our clone.
		 */
		if (!(t->t_flag & T_FORKALL)) {
			rp->r_pc = rp->r_npc;
			rp->r_npc += 4;
		}
	}

	/*
	 * From the proc(5) manual page:
	 * When exit from a system call is being traced, the traced process
	 * stops on completion of the system call just prior to checking for
	 * signals and returning to user level.  At this point all return
	 * values have been stored into the traced process's saved registers.
	 */
	if (proc_stop) {
		mutex_enter(&p->p_lock);
		if (PTOU(p)->u_systrap &&
		    prismember(&PTOU(p)->u_exitmask, code))
			stop(PR_SYSEXIT, code);
		mutex_exit(&p->p_lock);
	}

	/*
	 * If we are the parent returning from a successful
	 * vfork, wait for the child to exec or exit.
	 * This code must be here and not in the bowels of the system
	 * so that /proc can intercept exit from vfork in a timely way.
	 */
	if (t->t_flag & T_VFPARENT) {
		ASSERT(code == SYS_vfork || code == SYS_forksys);
		ASSERT(rp->r_o1 == 0 && error == 0);
		vfwait((pid_t)rval1);
		t->t_flag &= ~T_VFPARENT;
	}

	/*
	 * If profiling is active, bill the current PC in user-land
	 * and keep reposting until profiling is disabled.
	 */
	if (p->p_prof.pr_scale) {
		if (lwp->lwp_oweupc)
			profil_tick(rp->r_pc);
		repost = 1;
	}

sig_check:
	/*
	 * Reset flag for next time.
	 * We must do this after stopping on PR_SYSEXIT
	 * because /proc uses the information in lwp_eosys.
	 */
	lwp->lwp_eosys = NORMALRETURN;
	clear_stale_fd();
	t->t_flag &= ~T_FORKALL;

	if (t->t_astflag | t->t_sig_check) {
		/*
		 * Turn off the AST flag before checking all the conditions that
		 * may have caused an AST.  This flag is on whenever a signal or
		 * unusual condition should be handled after the next trap or
		 * syscall.
		 */
		astoff(t);
		t->t_sig_check = 0;

		/*
		 * The following check is legal for the following reasons:
		 *	1) The thread we are checking, is ourselves, so there is
		 *	   no way the proc can go away.
		 *	2) The only time we need to be protected by the
		 *	   lock is if the binding is changed.
		 *
		 *	Note we will still take the lock and check the binding
		 *	if the condition was true without the lock held.  This
		 *	prevents lock contention among threads owned by the
		 *	same proc.
		 */

		if (curthread->t_proc_flag & TP_CHANGEBIND) {
			mutex_enter(&p->p_lock);
			if (curthread->t_proc_flag & TP_CHANGEBIND) {
				timer_lwpbind();
				curthread->t_proc_flag &= ~TP_CHANGEBIND;
			}
			mutex_exit(&p->p_lock);
		}

		/*
		 * for kaio requests on the special kaio poll queue,
		 * copyout their results to user memory.
		 */
		if (p->p_aio)
			aio_cleanup(0);

		/*
		 * If this LWP was asked to hold, call holdlwp(), which will
		 * stop.  holdlwps() sets this up and calls pokelwps() which
		 * sets the AST flag.
		 *
		 * Also check TP_EXITLWP, since this is used by fresh new LWPs
		 * through lwp_rtt().  That flag is set if the lwp_create(2)
		 * syscall failed after creating the LWP.
		 */
		if (ISHOLD(p) || (t->t_proc_flag & TP_EXITLWP))
			holdlwp();

		/*
		 * All code that sets signals and makes ISSIG_PENDING
		 * evaluate true must set t_sig_check afterwards.
		 */
		if (ISSIG_PENDING(t, lwp, p)) {
			if (issig(FORREAL))
				psig();
			t->t_sig_check = 1;	/* recheck next time */
		}

		if (sigprof) {
			int nargs = (code > 0 && code < NSYSCALL)?
			    LWP_GETSYSENT(lwp)[code].sy_narg : 0;
			realsigprof(code, nargs, error);
			t->t_sig_check = 1;	/* recheck next time */
		}

		/*
		 * If a performance counter overflow interrupt was
		 * delivered *during* the syscall, then re-enable the
		 * AST so that we take a trip through trap() to cause
		 * the SIGEMT to be delivered.
		 */
		if (lwp->lwp_pcb.pcb_flags & CPC_OVERFLOW)
			aston(t);

		/*
		 * If an asynchronous hardware error is pending, turn AST flag
		 * back on.  AST will be checked again before we return to user
		 * mode and we'll come back through trap() to handle the error.
		 */
		if (lwp->lwp_pcb.pcb_flags & ASYNC_HWERR)
			aston(t);
	}

	/*
	 * Restore register window if a debugger modified it.
	 * Set up to perform a single-step if a debugger requested it.
	 */
	if (lwp->lwp_pcb.pcb_xregstat != XREGNONE)
		xregrestore(lwp, 1);

	lwp->lwp_errno = 0;		/* clear error for next time */

	/*
	 * Set state to LWP_USER here so preempt won't give us a kernel
	 * priority if it occurs after this point.  Call CL_TRAPRET() to
	 * restore the user-level priority.
	 *
	 * It is important that no locks (other than spinlocks) be entered
	 * after this point before returning to user mode (unless lwp_state
	 * is set back to LWP_SYS).
	 *
	 * Sampled times past this point are charged to the user.
	 */
	lwp->lwp_state = LWP_USER;

	if (t->t_trapret) {
		t->t_trapret = 0;
		thread_lock(t);
		CL_TRAPRET(t);
		thread_unlock(t);
	}
	if (CPU->cpu_runrun || t->t_schedflag & TS_ANYWAITQ)
		preempt();
	prunstop();

	/*
	 * t_post_sys will be set if pcb_step is active.
	 */
	if (lwp->lwp_pcb.pcb_step != STEP_NONE) {
		prdostep();
		repost = 1;
	}

	t->t_sysnum = 0;	/* no longer in a system call */

	/*
	 * In case the args were copied to the lwp, reset the
	 * pointer so the next syscall will have the right lwp_ap pointer.
	 */
	lwp->lwp_ap = (long *)&rp->r_o0;
	lwp->lwp_argsaved = 0;

	/*
	 * If there was a continuing reason for post-syscall processing,
	 * set the t_post_sys flag for the next system call.
	 */
	if (repost)
		t->t_post_sys = 1;

	/*
	 * If there is a ustack registered for this lwp, and the stack rlimit
	 * has been altered, read in the ustack. If the saved stack rlimit
	 * matches the bounds of the ustack, update the ustack to reflect
	 * the new rlimit. If the new stack rlimit is RLIM_INFINITY, disable
	 * stack checking by setting the size to 0.
	 */
	if (lwp->lwp_ustack != 0 && lwp->lwp_old_stk_ctl != 0) {
		rlim64_t new_size;
		model_t model;
		caddr_t top;
		struct rlimit64 rl;

		mutex_enter(&p->p_lock);
		new_size = p->p_stk_ctl;
		model = p->p_model;
		top = p->p_usrstack;
		(void) rctl_rlimit_get(rctlproc_legacy[RLIMIT_STACK], p, &rl);
		mutex_exit(&p->p_lock);

		if (rl.rlim_cur == RLIM64_INFINITY)
			new_size = 0;

		if (model == DATAMODEL_NATIVE) {
			stack_t stk;

			if (copyin((stack_t *)lwp->lwp_ustack, &stk,
			    sizeof (stack_t)) == 0 &&
			    (stk.ss_size == lwp->lwp_old_stk_ctl ||
			    stk.ss_size == 0) &&
			    stk.ss_sp == top - stk.ss_size) {
				stk.ss_sp = (void *)((uintptr_t)stk.ss_sp +
				    stk.ss_size - new_size);
				stk.ss_size = new_size;

				(void) copyout(&stk,
				    (stack_t *)lwp->lwp_ustack,
				    sizeof (stack_t));
			}
		} else {
			stack32_t stk32;

			if (copyin((stack32_t *)lwp->lwp_ustack, &stk32,
			    sizeof (stack32_t)) == 0 &&
			    (stk32.ss_size == lwp->lwp_old_stk_ctl ||
			    stk32.ss_size == 0) &&
			    stk32.ss_sp ==
			    (caddr32_t)(uintptr_t)(top - stk32.ss_size)) {
				stk32.ss_sp += stk32.ss_size - new_size;
				stk32.ss_size = new_size;

				(void) copyout(&stk32,
				    (stack32_t *)lwp->lwp_ustack,
				    sizeof (stack32_t));
			}
		}

		lwp->lwp_old_stk_ctl = 0;
	}

	syscall_mstate(LMS_SYSTEM, LMS_USER);
}

/*
 * Call a system call which takes a pointer to the user args struct and
 * a pointer to the return values.  This is a bit slower than the standard
 * C arg-passing method in some cases.
 */
int64_t
syscall_ap()
{
	uint_t	error;
	struct sysent *callp;
	rval_t	rval;
	klwp_t	*lwp = ttolwp(curthread);
	struct regs *rp = lwptoregs(lwp);

	callp = LWP_GETSYSENT(lwp) + curthread->t_sysnum;

	/*
	 * If the arguments don't fit in registers %o0 - o5, make sure they
	 * have been copied to the lwp_arg array.
	 */
	if (callp->sy_narg > 6 && save_syscall_args())
		return ((int64_t)set_errno(EFAULT));

	rval.r_val1 = 0;
	rval.r_val2 = (int)rp->r_o1;
	lwp->lwp_error = 0;	/* for old drivers */
	error = (*(callp->sy_call))(lwp->lwp_ap, &rval);
	if (error)
		return ((int64_t)set_errno(error));
	return (rval.r_vals);
}

/*
 * Load system call module.
 *	Returns with pointer to held read lock for module.
 */
static krwlock_t *
lock_syscall(struct sysent *table, uint_t code)
{
	krwlock_t	*module_lock;
	struct modctl	*modp;
	int		id;
	struct sysent   *callp;

	module_lock = table[code].sy_lock;
	callp = &table[code];

	/*
	 * Optimization to only call modload if we don't have a loaded
	 * syscall.
	 */
	rw_enter(module_lock, RW_READER);
	if (LOADED_SYSCALL(callp))
		return (module_lock);
	rw_exit(module_lock);

	for (;;) {
		if ((id = modload("sys", syscallnames[code])) == -1)
			break;

		/*
		 * If we loaded successfully at least once, the modctl
		 * will still be valid, so we try to grab it by filename.
		 * If this call fails, it's because the mod_filename
		 * was changed after the call to modload() (mod_hold_by_name()
		 * is the likely culprit).  We can safely just take
		 * another lap if this is the case;  the modload() will
		 * change the mod_filename back to one by which we can
		 * find the modctl.
		 */
		modp = mod_find_by_filename("sys", syscallnames[code]);

		if (modp == NULL)
			continue;

		mutex_enter(&mod_lock);

		if (!modp->mod_installed) {
			mutex_exit(&mod_lock);
			continue;
		}
		break;
	}

	rw_enter(module_lock, RW_READER);

	if (id != -1)
		mutex_exit(&mod_lock);

	return (module_lock);
}

/*
 * Loadable syscall support.
 *	If needed, load the module, then reserve it by holding a read
 *	lock for the duration of the call.
 *	Later, if the syscall is not unloadable, it could patch the vector.
 */
/*ARGSUSED*/
int64_t
loadable_syscall(
    long a0, long a1, long a2, long a3,
    long a4, long a5, long a6, long a7)
{
	int64_t		rval;
	struct sysent	*callp;
	struct sysent	*se = LWP_GETSYSENT(ttolwp(curthread));
	krwlock_t	*module_lock;
	int		code;

	code = curthread->t_sysnum;
	callp = se + code;

	/*
	 * Try to autoload the system call if necessary.
	 */
	module_lock = lock_syscall(se, code);

	/*
	 * we've locked either the loaded syscall or nosys
	 */
	if (callp->sy_flags & SE_ARGC) {
		int64_t (*sy_call)();

		sy_call = (int64_t (*)())callp->sy_call;
		rval = (*sy_call)(a0, a1, a2, a3, a4, a5);
	} else {
		rval = syscall_ap();
	}

	rw_exit(module_lock);
	return (rval);
}

/*
 * Handle indirect system calls.
 *	This interface should be deprecated.  The library can handle
 *	this more efficiently, but keep this implementation for old binaries.
 *
 * XX64	Needs some work.
 */
int64_t
indir(int code, long a0, long a1, long a2, long a3, long a4)
{
	klwp_t		*lwp = ttolwp(curthread);
	struct sysent	*callp;

	if (code <= 0 || code >= NSYSCALL)
		return (nosys());

	ASSERT(lwp->lwp_ap != NULL);

	curthread->t_sysnum = code;
	callp = LWP_GETSYSENT(lwp) + code;

	/*
	 * Handle argument setup, unless already done in pre_syscall().
	 */
	if (callp->sy_narg > 5) {
		if (save_syscall_args())	/* move args to LWP array */
			return ((int64_t)set_errno(EFAULT));
	} else if (!lwp->lwp_argsaved) {
		long *ap;

		ap = lwp->lwp_ap;		/* args haven't been saved */
		lwp->lwp_ap = ap + 1;		/* advance arg pointer */
		curthread->t_post_sys = 1;	/* so lwp_ap will be reset */
	}
	return ((*callp->sy_callc)(a0, a1, a2, a3, a4, lwp->lwp_arg[5]));
}

/*
 * set_errno - set an error return from the current system call.
 *	This could be a macro.
 *	This returns the value it is passed, so that the caller can
 *	use tail-recursion-elimination and do return (set_errno(ERRNO));
 */
uint_t
set_errno(uint_t error)
{
	ASSERT(error != 0);		/* must not be used to clear errno */

	curthread->t_post_sys = 1;	/* have post_syscall do error return */
	return (ttolwp(curthread)->lwp_errno = error);
}

/*
 * set_proc_pre_sys - Set pre-syscall processing for entire process.
 */
void
set_proc_pre_sys(proc_t *p)
{
	kthread_t	*t;
	kthread_t	*first;

	ASSERT(MUTEX_HELD(&p->p_lock));

	t = first = p->p_tlist;
	do {
		t->t_pre_sys = 1;
	} while ((t = t->t_forw) != first);
}

/*
 * set_proc_post_sys - Set post-syscall processing for entire process.
 */
void
set_proc_post_sys(proc_t *p)
{
	kthread_t	*t;
	kthread_t	*first;

	ASSERT(MUTEX_HELD(&p->p_lock));

	t = first = p->p_tlist;
	do {
		t->t_post_sys = 1;
	} while ((t = t->t_forw) != first);
}

/*
 * set_proc_sys - Set pre- and post-syscall processing for entire process.
 */
void
set_proc_sys(proc_t *p)
{
	kthread_t	*t;
	kthread_t	*first;

	ASSERT(MUTEX_HELD(&p->p_lock));

	t = first = p->p_tlist;
	do {
		t->t_pre_sys = 1;
		t->t_post_sys = 1;
	} while ((t = t->t_forw) != first);
}

/*
 * set_all_proc_sys - set pre- and post-syscall processing flags for all
 * user processes.
 *
 * This is needed when auditing, tracing, or other facilities which affect
 * all processes are turned on.
 */
void
set_all_proc_sys()
{
	kthread_t	*t;
	kthread_t	*first;

	mutex_enter(&pidlock);
	t = first = curthread;
	do {
		t->t_pre_sys = 1;
		t->t_post_sys = 1;
	} while ((t = t->t_next) != first);
	mutex_exit(&pidlock);
}

/*
 * set_all_zone_usr_proc_sys - set pre- and post-syscall processing flags for
 * all user processes running in the zone of the current process
 *
 * This is needed when auditing is turned on.
 */
void
set_all_zone_usr_proc_sys(zoneid_t zoneid)
{
	proc_t	    *p;
	kthread_t   *t;

	mutex_enter(&pidlock);
	for (p = practive; p != NULL; p = p->p_next) {
		/* skip kernel processes */
		if (p->p_exec == NULLVP || p->p_as == &kas ||
		    p->p_stat == SIDL || p->p_stat == SZOMB ||
		    (p->p_flag & (SSYS | SEXITING | SEXITLWPS)))
			continue;
		/*
		 * Only processes in the given zone (eventually in
		 * all zones) are taken into account
		 */
		if (zoneid == ALL_ZONES || p->p_zone->zone_id == zoneid) {
			mutex_enter(&p->p_lock);
			if ((t = p->p_tlist) == NULL) {
				mutex_exit(&p->p_lock);
				continue;
			}
			/*
			 * Set pre- and post-syscall processing flags
			 * for all threads of the process
			 */
			do {
				t->t_pre_sys = 1;
				t->t_post_sys = 1;
			} while (p->p_tlist != (t = t->t_forw));
			mutex_exit(&p->p_lock);
		}
	}
	mutex_exit(&pidlock);
}

/*
 * set_proc_ast - Set asynchronous service trap (AST) flag for all
 * threads in process.
 */
void
set_proc_ast(proc_t *p)
{
	kthread_t	*t;
	kthread_t	*first;

	ASSERT(MUTEX_HELD(&p->p_lock));

	t = first = p->p_tlist;
	do {
		aston(t);
	} while ((t = t->t_forw) != first);
}



/* ============================================================================
 * SOURCE 4/4: minix4\exokernel\kernel_legacy\uts\intel\os\syscall.c
 * Size: 35,274 bytes, Lines: 1,375
 * Hash: 742f7affaf4a...
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
 * Copyright (c) 1992, 2010, Oracle and/or its affiliates. All rights reserved.
 * Copyright 2019 Joyent, Inc.
 */

#include <sys/param.h>
#include <sys/vmparam.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/stack.h>
#include <sys/cred.h>
#include <sys/cmn_err.h>
#include <sys/user.h>
#include <sys/privregs.h>
#include <sys/psw.h>
#include <sys/debug.h>
#include <sys/errno.h>
#include <sys/proc.h>
#include <sys/modctl.h>
#include <sys/var.h>
#include <sys/inline.h>
#include <sys/syscall.h>
#include <sys/ucontext.h>
#include <sys/cpuvar.h>
#include <sys/siginfo.h>
#include <sys/trap.h>
#include <sys/vtrace.h>
#include <sys/sysinfo.h>
#include <sys/procfs.h>
#include <sys/prsystm.h>
#include <c2/audit.h>
#include <sys/modctl.h>
#include <sys/aio_impl.h>
#include <sys/copyops.h>
#include <sys/priv.h>
#include <sys/msacct.h>

int syscalltrace = 0;
#ifdef SYSCALLTRACE
static kmutex_t systrace_lock;		/* syscall tracing lock */
#else
#define	syscalltrace 0
#endif /* SYSCALLTRACE */

typedef	int64_t (*llfcn_t)();	/* function returning long long */

int pre_syscall(void);
void post_syscall(long rval1, long rval2);
static krwlock_t *lock_syscall(struct sysent *, uint_t);
void deferred_singlestep_trap(caddr_t);

#ifdef _SYSCALL32_IMPL
#define	LWP_GETSYSENT(lwp)	\
	(lwp_getdatamodel(lwp) == DATAMODEL_NATIVE ? sysent : sysent32)
#else
#define	LWP_GETSYSENT(lwp)	(sysent)
#endif

/*
 * If watchpoints are active, don't make copying in of
 * system call arguments take a read watchpoint trap.
 */
static int
copyin_args(struct regs *rp, long *ap, uint_t nargs)
{
	greg_t *sp = 1 + (greg_t *)rp->r_sp;		/* skip ret addr */

	ASSERT(nargs <= MAXSYSARGS);

	return (copyin_nowatch(sp, ap, nargs * sizeof (*sp)));
}

#if defined(_SYSCALL32_IMPL)
static int
copyin_args32(struct regs *rp, long *ap, uint_t nargs)
{
	greg32_t *sp = 1 + (greg32_t *)rp->r_sp;	/* skip ret addr */
	uint32_t a32[MAXSYSARGS];
	int rc;

	ASSERT(nargs <= MAXSYSARGS);

	if ((rc = copyin_nowatch(sp, a32, nargs * sizeof (*sp))) == 0) {
		uint32_t *a32p = &a32[0];

		while (nargs--)
			*ap++ = (ulong_t)*a32p++;
	}
	return (rc);
}
#define	COPYIN_ARGS32	copyin_args32
#else
#define	COPYIN_ARGS32	copyin_args
#endif

/*
 * Error handler for system calls where arg copy gets fault.
 */
static longlong_t
syscall_err()
{
	return (0);
}

/*
 * Corresponding sysent entry to allow syscall_entry caller
 * to invoke syscall_err.
 */
static struct sysent sysent_err =  {
	0, SE_32RVAL1, NULL, NULL, (llfcn_t)syscall_err
};

/*
 * Called from syscall() when a non-trivial 32-bit system call occurs.
 *	Sets up the args and returns a pointer to the handler.
 */
struct sysent *
syscall_entry(kthread_t *t, long *argp)
{
	klwp_t *lwp = ttolwp(t);
	struct regs *rp = lwptoregs(lwp);
	unsigned int code;
	struct sysent *callp;
	struct sysent *se = LWP_GETSYSENT(lwp);
	int error = 0;
	uint_t nargs;

	ASSERT(t == curthread && curthread->t_schedflag & TS_DONT_SWAP);

	lwp->lwp_ru.sysc++;
	lwp->lwp_eosys = NORMALRETURN;	/* assume this will be normal */

	/*
	 * Set lwp_ap to point to the args, even if none are needed for this
	 * system call.  This is for the loadable-syscall case where the
	 * number of args won't be known until the system call is loaded, and
	 * also maintains a non-NULL lwp_ap setup for get_syscall_args(). Note
	 * that lwp_ap MUST be set to a non-NULL value _BEFORE_ t_sysnum is
	 * set to non-zero; otherwise get_syscall_args(), seeing a non-zero
	 * t_sysnum for this thread, will charge ahead and dereference lwp_ap.
	 */
	lwp->lwp_ap = argp;		/* for get_syscall_args */

	code = rp->r_r0;
	t->t_sysnum = (short)code;
	callp = code >= NSYSCALL ? &nosys_ent : se + code;

	if ((t->t_pre_sys | syscalltrace) != 0) {
		error = pre_syscall();

		/*
		 * pre_syscall() has taken care so that lwp_ap is current;
		 * it either points to syscall-entry-saved amd64 regs,
		 * or it points to lwp_arg[], which has been re-copied from
		 * the ia32 ustack, but either way, it's a current copy after
		 * /proc has possibly mucked with the syscall args.
		 */

		if (error)
			return (&sysent_err);	/* use dummy handler */
	}

	/*
	 * Fetch the system call arguments to the kernel stack copy used
	 * for syscall handling.
	 * Note: for loadable system calls the number of arguments required
	 * may not be known at this point, and will be zero if the system call
	 * was never loaded.  Once the system call has been loaded, the number
	 * of args is not allowed to be changed.
	 */
	if ((nargs = (uint_t)callp->sy_narg) != 0 &&
	    COPYIN_ARGS32(rp, argp, nargs)) {
		(void) set_errno(EFAULT);
		return (&sysent_err);	/* use dummy handler */
	}

	return (callp);		/* return sysent entry for caller */
}

void
syscall_exit(kthread_t *t, long rval1, long rval2)
{
	/*
	 * Handle signals and other post-call events if necessary.
	 */
	if ((t->t_post_sys_ast | syscalltrace) == 0) {
		klwp_t *lwp = ttolwp(t);
		struct regs *rp = lwptoregs(lwp);

		/*
		 * Normal return.
		 * Clear error indication and set return values.
		 */
		rp->r_ps &= ~PS_C;	/* reset carry bit */
		rp->r_r0 = rval1;
		rp->r_r1 = rval2;
		lwp->lwp_state = LWP_USER;
	} else {
		post_syscall(rval1, rval2);
	}
	t->t_sysnum = 0;		/* invalidate args */
}

/*
 * Perform pre-system-call processing, including stopping for tracing,
 * auditing, etc.
 *
 * This routine is called only if the t_pre_sys flag is set. Any condition
 * requiring pre-syscall handling must set the t_pre_sys flag. If the
 * condition is persistent, this routine will repost t_pre_sys.
 */
int
pre_syscall()
{
	kthread_t *t = curthread;
	unsigned code = t->t_sysnum;
	klwp_t *lwp = ttolwp(t);
	proc_t *p = ttoproc(t);
	int	repost;

	t->t_pre_sys = repost = 0;	/* clear pre-syscall processing flag */

	ASSERT(t->t_schedflag & TS_DONT_SWAP);

#if defined(DEBUG)
	/*
	 * On the i386 kernel, lwp_ap points at the piece of the thread
	 * stack that we copy the users arguments into.
	 *
	 * On the amd64 kernel, the syscall arguments in the rdi..r9
	 * registers should be pointed at by lwp_ap.  If the args need to
	 * be copied so that those registers can be changed without losing
	 * the ability to get the args for /proc, they can be saved by
	 * save_syscall_args(), and lwp_ap will be restored by post_syscall().
	 */
	if (lwp_getdatamodel(lwp) == DATAMODEL_NATIVE) {
#if defined(_LP64)
		ASSERT(lwp->lwp_ap == (long *)&lwptoregs(lwp)->r_rdi);
	} else {
#endif
		ASSERT((caddr_t)lwp->lwp_ap > t->t_stkbase &&
		    (caddr_t)lwp->lwp_ap < t->t_stk);
	}
#endif	/* DEBUG */

	/*
	 * Make sure the thread is holding the latest credentials for the
	 * process.  The credentials in the process right now apply to this
	 * thread for the entire system call.
	 */
	if (t->t_cred != p->p_cred) {
		cred_t *oldcred = t->t_cred;
		/*
		 * DTrace accesses t_cred in probe context.  t_cred must
		 * always be either NULL, or point to a valid, allocated cred
		 * structure.
		 */
		t->t_cred = crgetcred();
		crfree(oldcred);
	}

	/*
	 * From the proc(5) manual page:
	 * When entry to a system call is being traced, the traced process
	 * stops after having begun the call to the system but before the
	 * system call arguments have been fetched from the process.
	 */
	if (PTOU(p)->u_systrap) {
		if (prismember(&PTOU(p)->u_entrymask, code)) {
			mutex_enter(&p->p_lock);
			/*
			 * Recheck stop condition, now that lock is held.
			 */
			if (PTOU(p)->u_systrap &&
			    prismember(&PTOU(p)->u_entrymask, code)) {
				stop(PR_SYSENTRY, code);

				/*
				 * /proc may have modified syscall args,
				 * either in regs for amd64 or on ustack
				 * for ia32.  Either way, arrange to
				 * copy them again, both for the syscall
				 * handler and for other consumers in
				 * post_syscall (like audit).  Here, we
				 * only do amd64, and just set lwp_ap
				 * back to the kernel-entry stack copy;
				 * the syscall ml code redoes
				 * move-from-regs to set up for the
				 * syscall handler after we return.  For
				 * ia32, save_syscall_args() below makes
				 * an lwp_ap-accessible copy.
				 */
#if defined(_LP64)
				if (lwp_getdatamodel(lwp) == DATAMODEL_NATIVE) {
					lwp->lwp_argsaved = 0;
					lwp->lwp_ap =
					    (long *)&lwptoregs(lwp)->r_rdi;
				}
#endif
			}
			mutex_exit(&p->p_lock);
		}
		repost = 1;
	}

	/*
	 * ia32 kernel, or ia32 proc on amd64 kernel: keep args in
	 * lwp_arg for post-syscall processing, regardless of whether
	 * they might have been changed in /proc above.
	 */
#if defined(_LP64)
	if (lwp_getdatamodel(lwp) != DATAMODEL_NATIVE)
#endif
		(void) save_syscall_args();

	if (lwp->lwp_sysabort) {
		/*
		 * lwp_sysabort may have been set via /proc while the process
		 * was stopped on PR_SYSENTRY.  If so, abort the system call.
		 * Override any error from the copyin() of the arguments.
		 */
		lwp->lwp_sysabort = 0;
		(void) set_errno(EINTR);	/* forces post_sys */
		t->t_pre_sys = 1;	/* repost anyway */
		return (1);		/* don't do system call, return EINTR */
	}

	/*
	 * begin auditing for this syscall if the c2audit module is loaded
	 * and auditing is enabled
	 */
	if (audit_active == C2AUDIT_LOADED) {
		uint32_t auditing = au_zone_getstate(NULL);

		if (auditing & AU_AUDIT_MASK) {
			int error;
			if (error = audit_start(T_SYSCALL, code, auditing, \
			    0, lwp)) {
				t->t_pre_sys = 1;	/* repost anyway */
				(void) set_errno(error);
				return (1);
			}
			repost = 1;
		}
	}

#ifdef SYSCALLTRACE
	if (syscalltrace) {
		int i;
		long *ap;
		char *cp;
		char *sysname;
		struct sysent *callp;

		if (code >= NSYSCALL)
			callp = &nosys_ent;	/* nosys has no args */
		else
			callp = LWP_GETSYSENT(lwp) + code;
		(void) save_syscall_args();
		mutex_enter(&systrace_lock);
		printf("%d: ", p->p_pid);
		if (code >= NSYSCALL) {
			printf("0x%x", code);
		} else {
			sysname = mod_getsysname(code);
			printf("%s[0x%x/0x%p]", sysname == NULL ? "NULL" :
			    sysname, code, callp->sy_callc);
		}
		cp = "(";
		for (i = 0, ap = lwp->lwp_ap; i < callp->sy_narg; i++, ap++) {
			printf("%s%lx", cp, *ap);
			cp = ", ";
		}
		if (i)
			printf(")");
		printf(" %s id=0x%p\n", PTOU(p)->u_comm, curthread);
		mutex_exit(&systrace_lock);
	}
#endif /* SYSCALLTRACE */

	/*
	 * If there was a continuing reason for pre-syscall processing,
	 * set the t_pre_sys flag for the next system call.
	 */
	if (repost)
		t->t_pre_sys = 1;
	lwp->lwp_error = 0;	/* for old drivers */
	lwp->lwp_badpriv = PRIV_NONE;
	return (0);
}


/*
 * Post-syscall processing.  Perform abnormal system call completion
 * actions such as /proc tracing, profiling, signals, preemption, etc.
 *
 * This routine is called only if t_post_sys, t_sig_check, or t_astflag is set.
 * Any condition requiring pre-syscall handling must set one of these.
 * If the condition is persistent, this routine will repost t_post_sys.
 */
void
post_syscall(long rval1, long rval2)
{
	kthread_t *t = curthread;
	klwp_t *lwp = ttolwp(t);
	proc_t *p = ttoproc(t);
	struct regs *rp = lwptoregs(lwp);
	uint_t	error;
	uint_t	code = t->t_sysnum;
	int	repost = 0;
	int	proc_stop = 0;		/* non-zero if stopping */
	int	sigprof = 0;		/* non-zero if sending SIGPROF */

	t->t_post_sys = 0;

	error = lwp->lwp_errno;

	/*
	 * Code can be zero if this is a new LWP returning after a forkall(),
	 * other than the one which matches the one in the parent which called
	 * forkall().  In these LWPs, skip most of post-syscall activity.
	 */
	if (code == 0)
		goto sig_check;
	/*
	 * If the trace flag is set, mark the lwp to take a single-step trap
	 * on return to user level (below). The x86 lcall interface and
	 * sysenter has already done this, and turned off the flag, but
	 * amd64 syscall interface has not.
	 */
	if (rp->r_ps & PS_T) {
		lwp->lwp_pcb.pcb_flags |= DEBUG_PENDING;
		rp->r_ps &= ~PS_T;
		aston(curthread);
	}

	/* put out audit record for this syscall */
	if (AU_AUDITING()) {
		rval_t	rval;

		/* XX64 -- truncation of 64-bit return values? */
		rval.r_val1 = (int)rval1;
		rval.r_val2 = (int)rval2;
		audit_finish(T_SYSCALL, code, error, &rval);
		repost = 1;
	}

	if (curthread->t_pdmsg != NULL) {
		char *m = curthread->t_pdmsg;

		uprintf("%s", m);
		kmem_free(m, strlen(m) + 1);
		curthread->t_pdmsg = NULL;
	}

	/*
	 * If we're going to stop for /proc tracing, set the flag and
	 * save the arguments so that the return values don't smash them.
	 */
	if (PTOU(p)->u_systrap) {
		if (prismember(&PTOU(p)->u_exitmask, code)) {
			if (lwp_getdatamodel(lwp) == DATAMODEL_LP64)
				(void) save_syscall_args();
			proc_stop = 1;
		}
		repost = 1;
	}

	/*
	 * Similarly check to see if SIGPROF might be sent.
	 */
	if (curthread->t_rprof != NULL &&
	    curthread->t_rprof->rp_anystate != 0) {
		if (lwp_getdatamodel(lwp) == DATAMODEL_LP64)
			(void) save_syscall_args();
		sigprof = 1;
	}

	if (lwp->lwp_eosys == NORMALRETURN) {
		if (error == 0) {
#ifdef SYSCALLTRACE
			if (syscalltrace) {
				mutex_enter(&systrace_lock);
				printf(
				    "%d: r_val1=0x%lx, r_val2=0x%lx, id 0x%p\n",
				    p->p_pid, rval1, rval2, curthread);
				mutex_exit(&systrace_lock);
			}
#endif /* SYSCALLTRACE */
			rp->r_ps &= ~PS_C;
			rp->r_r0 = rval1;
			rp->r_r1 = rval2;
		} else {
			int sig;
#ifdef SYSCALLTRACE
			if (syscalltrace) {
				mutex_enter(&systrace_lock);
				printf("%d: error=%d, id 0x%p\n",
				    p->p_pid, error, curthread);
				mutex_exit(&systrace_lock);
			}
#endif /* SYSCALLTRACE */
			if (error == EINTR && t->t_activefd.a_stale)
				error = EBADF;
			if (error == EINTR &&
			    (sig = lwp->lwp_cursig) != 0 &&
			    sigismember(&PTOU(p)->u_sigrestart, sig) &&
			    PTOU(p)->u_signal[sig - 1] != SIG_DFL &&
			    PTOU(p)->u_signal[sig - 1] != SIG_IGN)
				error = ERESTART;
			rp->r_r0 = error;
			rp->r_ps |= PS_C;
		}
	}

	/*
	 * From the proc(5) manual page:
	 * When exit from a system call is being traced, the traced process
	 * stops on completion of the system call just prior to checking for
	 * signals and returning to user level.  At this point all return
	 * values have been stored into the traced process's saved registers.
	 */
	if (proc_stop) {
		mutex_enter(&p->p_lock);
		if (PTOU(p)->u_systrap &&
		    prismember(&PTOU(p)->u_exitmask, code))
			stop(PR_SYSEXIT, code);
		mutex_exit(&p->p_lock);
	}

	/*
	 * If we are the parent returning from a successful
	 * vfork, wait for the child to exec or exit.
	 * This code must be here and not in the bowels of the system
	 * so that /proc can intercept exit from vfork in a timely way.
	 */
	if (t->t_flag & T_VFPARENT) {
		ASSERT(code == SYS_vfork || code == SYS_forksys);
		ASSERT(rp->r_r1 == 0 && error == 0);
		vfwait((pid_t)rval1);
		t->t_flag &= ~T_VFPARENT;
	}

	/*
	 * If profiling is active, bill the current PC in user-land
	 * and keep reposting until profiling is disabled.
	 */
	if (p->p_prof.pr_scale) {
		if (lwp->lwp_oweupc)
			profil_tick(rp->r_pc);
		repost = 1;
	}

sig_check:
	/*
	 * Reset flag for next time.
	 * We must do this after stopping on PR_SYSEXIT
	 * because /proc uses the information in lwp_eosys.
	 */
	lwp->lwp_eosys = NORMALRETURN;
	clear_stale_fd();
	t->t_flag &= ~T_FORKALL;

	if (t->t_astflag | t->t_sig_check) {
		/*
		 * Turn off the AST flag before checking all the conditions that
		 * may have caused an AST.  This flag is on whenever a signal or
		 * unusual condition should be handled after the next trap or
		 * syscall.
		 */
		astoff(t);
		/*
		 * If a single-step trap occurred on a syscall (see trap())
		 * recognize it now.  Do this before checking for signals
		 * because deferred_singlestep_trap() may generate a SIGTRAP to
		 * the LWP or may otherwise mark the LWP to call issig(FORREAL).
		 */
		if (lwp->lwp_pcb.pcb_flags & DEBUG_PENDING)
			deferred_singlestep_trap((caddr_t)rp->r_pc);

		t->t_sig_check = 0;

		/*
		 * The following check is legal for the following reasons:
		 *	1) The thread we are checking, is ourselves, so there is
		 *	   no way the proc can go away.
		 *	2) The only time we need to be protected by the
		 *	   lock is if the binding is changed.
		 *
		 *	Note we will still take the lock and check the binding
		 *	if the condition was true without the lock held.  This
		 *	prevents lock contention among threads owned by the
		 *	same proc.
		 */

		if (curthread->t_proc_flag & TP_CHANGEBIND) {
			mutex_enter(&p->p_lock);
			if (curthread->t_proc_flag & TP_CHANGEBIND) {
				timer_lwpbind();
				curthread->t_proc_flag &= ~TP_CHANGEBIND;
			}
			mutex_exit(&p->p_lock);
		}

		/*
		 * for kaio requests on the special kaio poll queue,
		 * copyout their results to user memory.
		 */
		if (p->p_aio)
			aio_cleanup(0);
		/*
		 * If this LWP was asked to hold, call holdlwp(), which will
		 * stop.  holdlwps() sets this up and calls pokelwps() which
		 * sets the AST flag.
		 *
		 * Also check TP_EXITLWP, since this is used by fresh new LWPs
		 * through lwp_rtt().  That flag is set if the lwp_create(2)
		 * syscall failed after creating the LWP.
		 */
		if (ISHOLD(p) || (t->t_proc_flag & TP_EXITLWP))
			holdlwp();

		/*
		 * All code that sets signals and makes ISSIG_PENDING
		 * evaluate true must set t_sig_check afterwards.
		 */
		if (ISSIG_PENDING(t, lwp, p)) {
			if (issig(FORREAL))
				psig();
			t->t_sig_check = 1;	/* recheck next time */
		}

		if (sigprof) {
			int nargs = (code > 0 && code < NSYSCALL)?
			    LWP_GETSYSENT(lwp)[code].sy_narg : 0;
			realsigprof(code, nargs, error);
			t->t_sig_check = 1;	/* recheck next time */
		}

		/*
		 * If a performance counter overflow interrupt was
		 * delivered *during* the syscall, then re-enable the
		 * AST so that we take a trip through trap() to cause
		 * the SIGEMT to be delivered.
		 */
		if (lwp->lwp_pcb.pcb_flags & CPC_OVERFLOW)
			aston(t);

		/*
		 * /proc can't enable/disable the trace bit itself
		 * because that could race with the call gate used by
		 * system calls via "lcall". If that happened, an
		 * invalid EFLAGS would result. prstep()/prnostep()
		 * therefore schedule an AST for the purpose.
		 */
		if (lwp->lwp_pcb.pcb_flags & REQUEST_STEP) {
			lwp->lwp_pcb.pcb_flags &= ~REQUEST_STEP;
			rp->r_ps |= PS_T;
		}
		if (lwp->lwp_pcb.pcb_flags & REQUEST_NOSTEP) {
			lwp->lwp_pcb.pcb_flags &= ~REQUEST_NOSTEP;
			rp->r_ps &= ~PS_T;
		}
	}

	lwp->lwp_errno = 0;		/* clear error for next time */

	/*
	 * Set state to LWP_USER here so preempt won't give us a kernel
	 * priority if it occurs after this point.  Call CL_TRAPRET() to
	 * restore the user-level priority.
	 *
	 * It is important that no locks (other than spinlocks) be entered
	 * after this point before returning to user mode (unless lwp_state
	 * is set back to LWP_SYS).
	 *
	 * XXX Sampled times past this point are charged to the user.
	 */
	lwp->lwp_state = LWP_USER;

	if (t->t_trapret) {
		t->t_trapret = 0;
		thread_lock(t);
		CL_TRAPRET(t);
		thread_unlock(t);
	}
	if (CPU->cpu_runrun || t->t_schedflag & TS_ANYWAITQ)
		preempt();
	prunstop();

	lwp->lwp_errno = 0;		/* clear error for next time */

	/*
	 * The thread lock must be held in order to clear sysnum and reset
	 * lwp_ap atomically with respect to other threads in the system that
	 * may be looking at the args via lwp_ap from get_syscall_args().
	 */

	thread_lock(t);
	t->t_sysnum = 0;		/* no longer in a system call */

	if (lwp_getdatamodel(lwp) == DATAMODEL_NATIVE) {
#if defined(_LP64)
		/*
		 * In case the args were copied to the lwp, reset the
		 * pointer so the next syscall will have the right
		 * lwp_ap pointer.
		 */
		lwp->lwp_ap = (long *)&rp->r_rdi;
	} else {
#endif
		lwp->lwp_ap = NULL;	/* reset on every syscall entry */
	}
	thread_unlock(t);

	lwp->lwp_argsaved = 0;

	/*
	 * If there was a continuing reason for post-syscall processing,
	 * set the t_post_sys flag for the next system call.
	 */
	if (repost)
		t->t_post_sys = 1;

	/*
	 * If there is a ustack registered for this lwp, and the stack rlimit
	 * has been altered, read in the ustack. If the saved stack rlimit
	 * matches the bounds of the ustack, update the ustack to reflect
	 * the new rlimit. If the new stack rlimit is RLIM_INFINITY, disable
	 * stack checking by setting the size to 0.
	 */
	if (lwp->lwp_ustack != 0 && lwp->lwp_old_stk_ctl != 0) {
		rlim64_t new_size;
		caddr_t top;
		stack_t stk;
		struct rlimit64 rl;

		mutex_enter(&p->p_lock);
		new_size = p->p_stk_ctl;
		top = p->p_usrstack;
		(void) rctl_rlimit_get(rctlproc_legacy[RLIMIT_STACK], p, &rl);
		mutex_exit(&p->p_lock);

		if (rl.rlim_cur == RLIM64_INFINITY)
			new_size = 0;

		if (copyin((stack_t *)lwp->lwp_ustack, &stk,
		    sizeof (stack_t)) == 0 &&
		    (stk.ss_size == lwp->lwp_old_stk_ctl ||
		    stk.ss_size == 0) &&
		    stk.ss_sp == top - stk.ss_size) {
			stk.ss_sp = (void *)((uintptr_t)stk.ss_sp +
			    stk.ss_size - (uintptr_t)new_size);
			stk.ss_size = new_size;

			(void) copyout(&stk, (stack_t *)lwp->lwp_ustack,
			    sizeof (stack_t));
		}

		lwp->lwp_old_stk_ctl = 0;
	}
}

/*
 * Called from post_syscall() when a deferred singlestep is to be taken.
 */
void
deferred_singlestep_trap(caddr_t pc)
{
	proc_t *p = ttoproc(curthread);
	klwp_t *lwp = ttolwp(curthread);
	pcb_t *pcb = &lwp->lwp_pcb;
	uint_t fault = 0;
	k_siginfo_t siginfo;

	bzero(&siginfo, sizeof (siginfo));

	/*
	 * If both NORMAL_STEP and WATCH_STEP are in
	 * effect, give precedence to WATCH_STEP.
	 * If neither is set, user must have set the
	 * PS_T bit in %efl; treat this as NORMAL_STEP.
	 */
	if ((fault = undo_watch_step(&siginfo)) == 0 &&
	    ((pcb->pcb_flags & NORMAL_STEP) ||
	    !(pcb->pcb_flags & WATCH_STEP))) {
		siginfo.si_signo = SIGTRAP;
		siginfo.si_code = TRAP_TRACE;
		siginfo.si_addr  = pc;
		fault = FLTTRACE;
	}
	pcb->pcb_flags &= ~(DEBUG_PENDING|NORMAL_STEP|WATCH_STEP);

	if (fault) {
		/*
		 * Remember the fault and fault adddress
		 * for real-time (SIGPROF) profiling.
		 */
		lwp->lwp_lastfault = fault;
		lwp->lwp_lastfaddr = siginfo.si_addr;
		/*
		 * If a debugger has declared this fault to be an
		 * event of interest, stop the lwp.  Otherwise just
		 * deliver the associated signal.
		 */
		if (prismember(&p->p_fltmask, fault) &&
		    stop_on_fault(fault, &siginfo) == 0)
			siginfo.si_signo = 0;
	}

	if (siginfo.si_signo)
		trapsig(&siginfo, 1);
}

/*
 * nonexistent system call-- signal lwp (may want to handle it)
 * flag error if lwp won't see signal immediately
 */
int64_t
nosys(void)
{
	tsignal(curthread, SIGSYS);
	return (set_errno(ENOSYS));
}

int
nosys32(void)
{
	return (nosys());
}

/*
 * Execute a 32-bit system call on behalf of the current thread.
 */
void
dosyscall(void)
{
	/*
	 * Need space on the stack to store syscall arguments.
	 */
	long		syscall_args[MAXSYSARGS];
	struct sysent	*se;
	int64_t		ret;

	syscall_mstate(LMS_TRAP, LMS_SYSTEM);

	ASSERT(curproc->p_model == DATAMODEL_ILP32);

	CPU_STATS_ENTER_K();
	CPU_STATS_ADDQ(CPU, sys, syscall, 1);
	CPU_STATS_EXIT_K();

	se = syscall_entry(curthread, syscall_args);

	/*
	 * syscall_entry() copied all 8 arguments into syscall_args.
	 */
	ret = se->sy_callc(syscall_args[0], syscall_args[1], syscall_args[2],
	    syscall_args[3], syscall_args[4], syscall_args[5], syscall_args[6],
	    syscall_args[7]);

	syscall_exit(curthread, (int)ret & 0xffffffffu, (int)(ret >> 32));
	syscall_mstate(LMS_SYSTEM, LMS_TRAP);
}

/*
 * Get the arguments to the current system call. See comment atop
 * save_syscall_args() regarding lwp_ap usage.
 */

uint_t
get_syscall_args(klwp_t *lwp, long *argp, int *nargsp)
{
	kthread_t	*t = lwptot(lwp);
	ulong_t	mask = 0xfffffffful;
	uint_t	code;
	long	*ap;
	int	nargs;

#if defined(_LP64)
	if (lwp_getdatamodel(lwp) == DATAMODEL_LP64)
		mask = 0xfffffffffffffffful;
#endif

	/*
	 * The thread lock must be held while looking at the arguments to ensure
	 * they don't go away via post_syscall().
	 * get_syscall_args() is the only routine to read them which is callable
	 * outside the LWP in question and hence the only one that must be
	 * synchronized in this manner.
	 */
	thread_lock(t);

	code = t->t_sysnum;
	ap = lwp->lwp_ap;

	thread_unlock(t);

	if (code != 0 && code < NSYSCALL) {
		nargs = LWP_GETSYSENT(lwp)[code].sy_narg;

		ASSERT(nargs <= MAXSYSARGS);

		*nargsp = nargs;
		while (nargs-- > 0)
			*argp++ = *ap++ & mask;
	} else {
		*nargsp = 0;
	}

	return (code);
}

#ifdef _SYSCALL32_IMPL
/*
 * Get the arguments to the current 32-bit system call.
 */
uint_t
get_syscall32_args(klwp_t *lwp, int *argp, int *nargsp)
{
	long args[MAXSYSARGS];
	uint_t i, code;

	code = get_syscall_args(lwp, args, nargsp);

	for (i = 0; i != *nargsp; i++)
		*argp++ = (int)args[i];
	return (code);
}
#endif

/*
 * Save the system call arguments in a safe place.
 *
 * On the i386 kernel:
 *
 *	Copy the users args prior to changing the stack or stack pointer.
 *	This is so /proc will be able to get a valid copy of the
 *	args from the user stack even after the user stack has been changed.
 *	Note that the kernel stack copy of the args may also have been
 *	changed by a system call handler which takes C-style arguments.
 *
 *	Note that this may be called by stop() from trap().  In that case
 *	t_sysnum will be zero (syscall_exit clears it), so no args will be
 *	copied.
 *
 * On the amd64 kernel:
 *
 *	For 64-bit applications, lwp->lwp_ap normally points to %rdi..%r9
 *	in the reg structure. If the user is going to change the argument
 *	registers, rax, or the stack and might want to get the args (for
 *	/proc tracing), it must copy the args elsewhere via save_syscall_args().
 *
 *	For 32-bit applications, lwp->lwp_ap normally points to a copy of
 *	the system call arguments on the kernel stack made from the user
 *	stack.  Copy the args prior to change the stack or stack pointer.
 *	This is so /proc will be able to get a valid copy of the args
 *	from the user stack even after that stack has been changed.
 *
 *	This may be called from stop() even when we're not in a system call.
 *	Since there's no easy way to tell, this must be safe (not panic).
 *	If the copyins get data faults, return non-zero.
 */
int
save_syscall_args()
{
	kthread_t	*t = curthread;
	klwp_t		*lwp = ttolwp(t);
	uint_t		code = t->t_sysnum;
	uint_t		nargs;

	if (lwp->lwp_argsaved || code == 0)
		return (0);		/* args already saved or not needed */

	if (code >= NSYSCALL) {
		nargs = 0;		/* illegal syscall */
	} else {
		struct sysent *se = LWP_GETSYSENT(lwp);
		struct sysent *callp = se + code;

		nargs = callp->sy_narg;
		if (LOADABLE_SYSCALL(callp) && nargs == 0) {
			krwlock_t	*module_lock;

			/*
			 * Find out how many arguments the system
			 * call uses.
			 *
			 * We have the property that loaded syscalls
			 * never change the number of arguments they
			 * use after they've been loaded once.  This
			 * allows us to stop for /proc tracing without
			 * holding the module lock.
			 * /proc is assured that sy_narg is valid.
			 */
			module_lock = lock_syscall(se, code);
			nargs = callp->sy_narg;
			rw_exit(module_lock);
		}
	}

	/*
	 * Fetch the system call arguments.
	 */
	if (nargs == 0)
		goto out;

	ASSERT(nargs <= MAXSYSARGS);

	if (lwp_getdatamodel(lwp) == DATAMODEL_NATIVE) {
#if defined(_LP64)
		struct regs *rp = lwptoregs(lwp);

		lwp->lwp_arg[0] = rp->r_rdi;
		lwp->lwp_arg[1] = rp->r_rsi;
		lwp->lwp_arg[2] = rp->r_rdx;
		lwp->lwp_arg[3] = rp->r_rcx;
		lwp->lwp_arg[4] = rp->r_r8;
		lwp->lwp_arg[5] = rp->r_r9;
		if (nargs > 6 && copyin_args(rp, &lwp->lwp_arg[6], nargs - 6))
			return (-1);
	} else {
#endif
		if (COPYIN_ARGS32(lwptoregs(lwp), lwp->lwp_arg, nargs))
			return (-1);
	}
out:
	lwp->lwp_ap = lwp->lwp_arg;
	lwp->lwp_argsaved = 1;
	t->t_post_sys = 1;	/* so lwp_ap will be reset */
	return (0);
}

void
reset_syscall_args(void)
{
	ttolwp(curthread)->lwp_argsaved = 0;
}

/*
 * Call a system call which takes a pointer to the user args struct and
 * a pointer to the return values.  This is a bit slower than the standard
 * C arg-passing method in some cases.
 */
int64_t
syscall_ap(void)
{
	uint_t	error;
	struct sysent *callp;
	rval_t	rval;
	kthread_t *t = curthread;
	klwp_t	*lwp = ttolwp(t);
	struct regs *rp = lwptoregs(lwp);

	callp = LWP_GETSYSENT(lwp) + t->t_sysnum;

	/*
	 * If the arguments don't fit in registers %rdi-%r9, make sure they
	 * have been copied to the lwp_arg array.
	 */
	if (callp->sy_narg > 6 && save_syscall_args())
		return ((int64_t)set_errno(EFAULT));

	rval.r_val1 = 0;
	rval.r_val2 = rp->r_r1;
	lwp->lwp_error = 0;	/* for old drivers */
	error = (*(callp->sy_call))(lwp->lwp_ap, &rval);
	if (error)
		return ((longlong_t)set_errno(error));
	return (rval.r_vals);
}

/*
 * Load system call module.
 *	Returns with pointer to held read lock for module.
 */
static krwlock_t *
lock_syscall(struct sysent *table, uint_t code)
{
	krwlock_t	*module_lock;
	struct modctl	*modp;
	int		id;
	struct sysent   *callp;

	callp = table + code;
	module_lock = callp->sy_lock;

	/*
	 * Optimization to only call modload if we don't have a loaded
	 * syscall.
	 */
	rw_enter(module_lock, RW_READER);
	if (LOADED_SYSCALL(callp))
		return (module_lock);
	rw_exit(module_lock);

	for (;;) {
		if ((id = modload("sys", syscallnames[code])) == -1)
			break;

		/*
		 * If we loaded successfully at least once, the modctl
		 * will still be valid, so we try to grab it by filename.
		 * If this call fails, it's because the mod_filename
		 * was changed after the call to modload() (mod_hold_by_name()
		 * is the likely culprit).  We can safely just take
		 * another lap if this is the case;  the modload() will
		 * change the mod_filename back to one by which we can
		 * find the modctl.
		 */
		modp = mod_find_by_filename("sys", syscallnames[code]);

		if (modp == NULL)
			continue;

		mutex_enter(&mod_lock);

		if (!modp->mod_installed) {
			mutex_exit(&mod_lock);
			continue;
		}
		break;
	}
	rw_enter(module_lock, RW_READER);

	if (id != -1)
		mutex_exit(&mod_lock);

	return (module_lock);
}

/*
 * Loadable syscall support.
 *	If needed, load the module, then reserve it by holding a read
 *	lock for the duration of the call.
 *	Later, if the syscall is not unloadable, it could patch the vector.
 */
/*ARGSUSED*/
int64_t
loadable_syscall(
    long a0, long a1, long a2, long a3,
    long a4, long a5, long a6, long a7)
{
	klwp_t *lwp = ttolwp(curthread);
	int64_t	rval;
	struct sysent *callp;
	struct sysent *se = LWP_GETSYSENT(lwp);
	krwlock_t *module_lock;
	int code, error = 0;

	code = curthread->t_sysnum;
	callp = se + code;

	/*
	 * Try to autoload the system call if necessary
	 */
	module_lock = lock_syscall(se, code);

	/*
	 * we've locked either the loaded syscall or nosys
	 */

	if (lwp_getdatamodel(lwp) == DATAMODEL_NATIVE) {
#if defined(_LP64)
		if (callp->sy_flags & SE_ARGC) {
			rval = (int64_t)(*callp->sy_call)(a0, a1, a2, a3,
			    a4, a5);
		} else {
			rval = syscall_ap();
		}
	} else {
#endif
		/*
		 * Now that it's loaded, make sure enough args were copied.
		 */
		if (COPYIN_ARGS32(lwptoregs(lwp), lwp->lwp_ap, callp->sy_narg))
			error = EFAULT;
		if (error) {
			rval = set_errno(error);
		} else if (callp->sy_flags & SE_ARGC) {
			rval = (int64_t)(*callp->sy_call)(lwp->lwp_ap[0],
			    lwp->lwp_ap[1], lwp->lwp_ap[2], lwp->lwp_ap[3],
			    lwp->lwp_ap[4], lwp->lwp_ap[5]);
		} else {
			rval = syscall_ap();
		}
	}

	rw_exit(module_lock);
	return (rval);
}

/*
 * Indirect syscall handled in libc on x86 architectures
 */
int64_t
indir()
{
	return (nosys());
}

/*
 * set_errno - set an error return from the current system call.
 *	This could be a macro.
 *	This returns the value it is passed, so that the caller can
 *	use tail-recursion-elimination and do return (set_errno(ERRNO));
 */
uint_t
set_errno(uint_t error)
{
	ASSERT(error != 0);		/* must not be used to clear errno */

	curthread->t_post_sys = 1;	/* have post_syscall do error return */
	return (ttolwp(curthread)->lwp_errno = error);
}

/*
 * set_proc_pre_sys - Set pre-syscall processing for entire process.
 */
void
set_proc_pre_sys(proc_t *p)
{
	kthread_t	*t;
	kthread_t	*first;

	ASSERT(MUTEX_HELD(&p->p_lock));

	t = first = p->p_tlist;
	do {
		t->t_pre_sys = 1;
	} while ((t = t->t_forw) != first);
}

/*
 * set_proc_post_sys - Set post-syscall processing for entire process.
 */
void
set_proc_post_sys(proc_t *p)
{
	kthread_t	*t;
	kthread_t	*first;

	ASSERT(MUTEX_HELD(&p->p_lock));

	t = first = p->p_tlist;
	do {
		t->t_post_sys = 1;
	} while ((t = t->t_forw) != first);
}

/*
 * set_proc_sys - Set pre- and post-syscall processing for entire process.
 */
void
set_proc_sys(proc_t *p)
{
	kthread_t	*t;
	kthread_t	*first;

	ASSERT(MUTEX_HELD(&p->p_lock));

	t = first = p->p_tlist;
	do {
		t->t_pre_sys = 1;
		t->t_post_sys = 1;
	} while ((t = t->t_forw) != first);
}

/*
 * set_all_proc_sys - set pre- and post-syscall processing flags for all
 * user processes.
 *
 * This is needed when auditing, tracing, or other facilities which affect
 * all processes are turned on.
 */
void
set_all_proc_sys()
{
	kthread_t	*t;
	kthread_t	*first;

	mutex_enter(&pidlock);
	t = first = curthread;
	do {
		t->t_pre_sys = 1;
		t->t_post_sys = 1;
	} while ((t = t->t_next) != first);
	mutex_exit(&pidlock);
}

/*
 * set_all_zone_usr_proc_sys - set pre- and post-syscall processing flags for
 * all user processes running in the zone of the current process
 *
 * This is needed when auditing, tracing, or other facilities which affect
 * all processes are turned on.
 */
void
set_all_zone_usr_proc_sys(zoneid_t zoneid)
{
	proc_t	    *p;
	kthread_t   *t;

	mutex_enter(&pidlock);
	for (p = practive; p != NULL; p = p->p_next) {
		/* skip kernel and incomplete processes */
		if (p->p_exec == NULLVP || p->p_as == &kas ||
		    p->p_stat == SIDL || p->p_stat == SZOMB ||
		    (p->p_flag & (SSYS | SEXITING | SEXITLWPS)))
			continue;
		/*
		 * Only processes in the given zone (eventually in
		 * all zones) are taken into account
		 */
		if (zoneid == ALL_ZONES || p->p_zone->zone_id == zoneid) {
			mutex_enter(&p->p_lock);
			if ((t = p->p_tlist) == NULL) {
				mutex_exit(&p->p_lock);
				continue;
			}
			/*
			 * Set pre- and post-syscall processing flags
			 * for all threads of the process
			 */
			do {
				t->t_pre_sys = 1;
				t->t_post_sys = 1;
			} while (p->p_tlist != (t = t->t_forw));
			mutex_exit(&p->p_lock);
		}
	}
	mutex_exit(&pidlock);
}

/*
 * set_proc_ast - Set asynchronous service trap (AST) flag for all
 * threads in process.
 */
void
set_proc_ast(proc_t *p)
{
	kthread_t	*t;
	kthread_t	*first;

	ASSERT(MUTEX_HELD(&p->p_lock));

	t = first = p->p_tlist;
	do {
		aston(t);
	} while ((t = t->t_forw) != first);
}

