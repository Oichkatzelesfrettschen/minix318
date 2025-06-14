/*	$NetBSD: print.c,v 1.138 2022/01/26 11:48:53 andvar Exp $	*/

/*
 * Copyright (c) 2000, 2007 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Simon Burge.
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
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copyright (c) 1990, 1993, 1994
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
static char sccsid[] = "@(#)print.c	8.6 (Berkeley) 4/16/94";
#else
__RCSID("$NetBSD: print.c,v 1.138 2022/01/26 11:48:53 andvar Exp $");
#endif
#endif /* not lint */

#include <sys/param.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/sysctl.h>
#include <sys/lwp.h>
#include <sys/proc.h>
#include <sys/stat.h>
#include <sys/ucred.h>
#include <sys/sysctl.h>
#include <sys/acct.h>
#include <sys/ktrace.h>

#include <err.h>
#include <grp.h>
#include <kvm.h>
#include <math.h>
#include <nlist.h>
#include <pwd.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <util.h>
#include <tzfile.h>
#include <unistd.h>
#include <signal.h>

#include "ps.h"

static char *cmdpart(char *);
static void  printval(void *, VAR *, enum mode);
static int   titlecmp(char *, char **);

static void  doubleprintorsetwidth(VAR *, double, int, enum mode);
static void  intprintorsetwidth(VAR *, int, enum mode);
static void  strprintorsetwidth(VAR *, const char *, enum mode);

static time_t now;

#define	min(a,b)	((a) <= (b) ? (a) : (b))

/* pre-NetBSD 5.x support. */
#ifndef LSDEAD
#define LSDEAD 6
#endif

static void __attribute__((__format__(__strftime__, 3, 0)))
safe_strftime(char *buf, size_t bufsiz, const char *fmt,
    const struct tm *tp)
{
	if (tp == NULL || strftime(buf, bufsiz, fmt, tp) == 0)
		strlcpy(buf, "-", sizeof(buf));
}

static int
iwidth(u_int64_t v)
{
	u_int64_t nlim, lim;
	int w = 1;

	for (lim = 10; v >= lim; lim = nlim) {
		nlim = lim * 10;
		w++;
		if (nlim < lim)
			break;
	}
	return w;
}

static char *
cmdpart(char *arg0)
{
	char *cp;

	return ((cp = strrchr(arg0, '/')) != NULL ? cp + 1 : arg0);
}

void
printheader(void)
{
	int len;
	VAR *v;
	struct varent *vent;
	static int firsttime = 1;
	static int noheader = 0;

	/*
	 * If all the columns have user-specified null headers,
	 * don't print the blank header line at all.
	 */
	if (firsttime) {
		SIMPLEQ_FOREACH(vent, &displaylist, next) {
			if (vent->var->header[0])
				break;
		}
		if (vent == NULL) {
			noheader = 1;
			firsttime = 0;
		}

	}
	if (noheader)
		return;

	SIMPLEQ_FOREACH(vent, &displaylist, next) {
		v = vent->var;
		if (firsttime) {
			len = strlen(v->header);
			if (len > v->width)
				v->width = len;
			totwidth += v->width + 1;	/* +1 for space */
		}
		if (v->flag & LJUST) {
			if (SIMPLEQ_NEXT(vent, next) == NULL)	/* last one */
				(void)printf("%s", v->header);
			else
				(void)printf("%-*s", v->width,
				    v->header);
		} else
			(void)printf("%*s", v->width, v->header);
		if (SIMPLEQ_NEXT(vent, next) != NULL)
			(void)putchar(' ');
	}
	(void)putchar('\n');
	if (firsttime) {
		firsttime = 0;
		totwidth--;	/* take off last space */
	}
}

/*
 * Return 1 if the command name in the argument vector (u-area) does
 * not match the command name (p_comm)
 */
static int
titlecmp(char *name, char **argv)
{
	char *title;
	int namelen;


	/* no argument vector == no match; system processes/threads do that */
	if (argv == 0 || argv[0] == 0)
		return (1);

	title = cmdpart(argv[0]);

	/* the basename matches */
	if (!strcmp(name, title))
		return (0);

	/* handle login shells, by skipping the leading - */
	if (title[0] == '-' && !strcmp(name, title + 1))
		return (0);

	namelen = strlen(name);

	/* handle daemons that report activity as daemonname: activity */
	if (argv[1] == 0 &&
	    !strncmp(name, title, namelen) &&
	    title[namelen + 0] == ':' &&
	    title[namelen + 1] == ' ')
		return (0);

	return (1);
}

static void
doubleprintorsetwidth(VAR *v, double val, int prec, enum mode mode)
{
	int fmtlen;

	if (mode == WIDTHMODE) {
		if (val < 0.0 && val < v->longestnd) {
			fmtlen = (int)log10(-val) + prec + 2;
			v->longestnd = val;
			if (fmtlen > v->width)
				v->width = fmtlen;
		} else if (val > 0.0 && val > v->longestpd) {
			fmtlen = (int)log10(val) + prec + 1;
			v->longestpd = val;
			if (fmtlen > v->width)
				v->width = fmtlen;
		}
	} else {
		(void)printf("%*.*f", v->width, prec, val);
	}
}

static void
intprintorsetwidth(VAR *v, int val, enum mode mode)
{
	int fmtlen;

	if (mode == WIDTHMODE) {
		if (val < 0 && val < v->longestn) {
			v->longestn = val;
			fmtlen = iwidth(-val) + 1;
			if (fmtlen > v->width)
				v->width = fmtlen;
		} else if (val > 0 && val > v->longestp) {
			v->longestp = val;
			fmtlen = iwidth(val);
			if (fmtlen > v->width)
				v->width = fmtlen;
		}
	} else
		(void)printf("%*d", v->width, val);
}

static void
strprintorsetwidth(VAR *v, const char *str, enum mode mode)
{
	int len;

	if (mode == WIDTHMODE) {
		len = strlen(str);
		if (len > v->width)
			v->width = len;
	} else {
		if (v->flag & LJUST)
			(void)printf("%-*.*s", v->width, v->width, str);
		else
			(void)printf("%*.*s", v->width, v->width, str);
	}
}

void
command(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *ki = pi->ki;
	VAR *v;
	int left;
	char **argv, **p, *name;

	if (mode == WIDTHMODE)
		return;

	v = ve->var;
	if (SIMPLEQ_NEXT(ve, next) != NULL || termwidth != UNLIMITED) {
		if (SIMPLEQ_NEXT(ve, next) == NULL) {
			left = termwidth - (totwidth - v->width);
			if (left < 1) /* already wrapped, just use std width */
				left = v->width;
		} else
			left = v->width;
	} else
		left = -1;
	if (needenv && kd) {
		argv = kvm_getenvv2(kd, ki, termwidth);
		if ((p = argv) != NULL) {
			while (*p) {
				fmt_puts(*p, &left);
				p++;
				fmt_putc(' ', &left);
			}
		}
	}
	if (needcomm) {
		if (pi->prefix)
			(void)fmt_puts(pi->prefix, &left);
		name = ki->p_comm;
		if (!commandonly) {
			argv = kvm_getargv2(kd, ki, termwidth);
			if ((p = argv) != NULL) {
				while (*p) {
					fmt_puts(*p, &left);
					p++;
					fmt_putc(' ', &left);
					if (v->flag & ARGV0)
						break;
				}
				if (!(v->flag & ARGV0) &&
				    titlecmp(name, argv)) {
					/*
					 * append the real command name within
					 * parentheses, if the command name
					 * does not match the one in the
					 * argument vector
					 */
					fmt_putc('(', &left);
					fmt_puts(name, &left);
					fmt_putc(')', &left);
				}
			} else {
				/*
				 * Commands that don't set an argv vector
				 * are printed with square brackets if they
				 * are system commands.  Otherwise they are
				 * printed within parentheses.
				 */
				if (ki->p_flag & P_SYSTEM) {
					fmt_putc('[', &left);
					fmt_puts(name, &left);
					fmt_putc(']', &left);
				} else {
					fmt_putc('(', &left);
					fmt_puts(name, &left);
					fmt_putc(')', &left);
				}
			}
		} else {
			fmt_puts(name, &left);
		}
	}
	if (SIMPLEQ_NEXT(ve, next) != NULL && left > 0)
		(void)printf("%*s", left, "");
}

void
groups(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *ki = pi->ki;
	VAR *v;
	int left, i;
	char buf[16], *p;

	if (mode == WIDTHMODE)
		return;

	v = ve->var;
	if (SIMPLEQ_NEXT(ve, next) != NULL || termwidth != UNLIMITED) {
		if (SIMPLEQ_NEXT(ve, next) == NULL) {
			left = termwidth - (totwidth - v->width);
			if (left < 1) /* already wrapped, just use std width */
				left = v->width;
		} else
			left = v->width;
	} else
		left = -1;

	if (ki->p_ngroups == 0)
		fmt_putc('-', &left);

	for (i = 0; i < ki->p_ngroups; i++) {
		(void)snprintf(buf, sizeof(buf), "%d", ki->p_groups[i]);
		if (i)
			fmt_putc(' ', &left);
		for (p = &buf[0]; *p; p++)
			fmt_putc(*p, &left);
	}

	if (SIMPLEQ_NEXT(ve, next) != NULL && left > 0)
		(void)printf("%*s", left, "");
}

void
groupnames(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *ki = pi->ki;
	VAR *v;
	int left, i;
	const char *p;

	if (mode == WIDTHMODE)
		return;

	v = ve->var;
	if (SIMPLEQ_NEXT(ve, next) != NULL || termwidth != UNLIMITED) {
		if (SIMPLEQ_NEXT(ve, next) == NULL) {
			left = termwidth - (totwidth - v->width);
			if (left < 1) /* already wrapped, just use std width */
				left = v->width;
		} else
			left = v->width;
	} else
		left = -1;

	if (ki->p_ngroups == 0)
		fmt_putc('-', &left);

	for (i = 0; i < ki->p_ngroups; i++) {
		if (i)
			fmt_putc(' ', &left);
		for (p = group_from_gid(ki->p_groups[i], 0); *p; p++)
			fmt_putc(*p, &left);
	}

	if (SIMPLEQ_NEXT(ve, next) != NULL && left > 0)
		(void)printf("%*s", left, "");
}

void
ucomm(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	char buf[MAXPATHLEN], *p;
	VAR *v;

	v = ve->var;
	if (pi->prefix)
		snprintf(p = buf, sizeof(buf), "%s%s", pi->prefix, k->p_comm);
	else
		p = k->p_comm;
	strprintorsetwidth(v, p, mode);
}

void
emul(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	VAR *v;

	v = ve->var;
	strprintorsetwidth(v, k->p_ename, mode);
}

void
logname(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	VAR *v;

	v = ve->var;
	strprintorsetwidth(v, k->p_login, mode);
}

void
state(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	int flag, is_zombie;
	char *cp;
	VAR *v;
	char buf[16];

	is_zombie = 0;
	v = ve->var;
	flag = k->p_flag;
	cp = buf;

	/*
	 * NOTE: There are historical letters, which are no longer used:
	 *
	 * - W: indicated that process is swapped out.
	 * - L: indicated non-zero l_holdcnt (i.e. that process was
	 *   prevented from swapping-out.
	 *
	 * These letters should not be used for new states to avoid
	 * conflicts with old applications which might depend on them.
	 */
	switch (k->p_stat) {

	case LSSTOP:
		*cp = 'T';
		break;

	case LSSLEEP:
		if (flag & L_SINTR)	/* interruptable (long) */
			*cp = (int)k->p_slptime >= maxslp ? 'I' : 'S';
		else
			*cp = 'D';
		break;

	case LSRUN:
	case LSIDL:
		*cp = 'R';
		break;

	case LSONPROC:
		*cp = 'O';
		break;

	case LSZOMB:
		*cp = 'Z';
		is_zombie = 1;
		break;

	case LSSUSPENDED:
		*cp = 'U';
		break;

	default:
		*cp = '?';
	}
	cp++;
	if (k->p_nice < NZERO)
		*cp++ = '<';
	else if (k->p_nice > NZERO)
		*cp++ = 'N';
	if (flag & P_TRACED)
		*cp++ = 'X';
	if (flag & P_WEXIT && !is_zombie)
		*cp++ = 'E';
	if (flag & P_PPWAIT)
		*cp++ = 'V';
	if (flag & P_SYSTEM)
		*cp++ = 'K';
	if (k->p_eflag & EPROC_SLEADER)
		*cp++ = 's';
	if (flag & P_SA)
		*cp++ = 'a';
	else if (k->p_nlwps > 1)
		*cp++ = 'l';
	if ((flag & P_CONTROLT) && k->p__pgid == k->p_tpgid)
		*cp++ = '+';
	*cp = '\0';
	strprintorsetwidth(v, buf, mode);
}

void
lstate(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_lwp *k = pi->li;
	int flag;
	char *cp;
	VAR *v;
	char buf[16];

	v = ve->var;
	flag = k->l_flag;
	cp = buf;

	switch (k->l_stat) {

	case LSSTOP:
		*cp = 'T';
		break;

	case LSSLEEP:
		if (flag & L_SINTR)	/* interruptible (long) */
			*cp = (int)k->l_slptime >= maxslp ? 'I' : 'S';
		else
			*cp = 'D';
		break;

	case LSRUN:
	case LSIDL:
		*cp = 'R';
		break;

	case LSONPROC:
		*cp = 'O';
		break;

	case LSZOMB:
	case LSDEAD:
		*cp = 'Z';
		break;

	case LSSUSPENDED:
		*cp = 'U';
		break;

	default:
		*cp = '?';
	}
	cp++;
	if (flag & L_SYSTEM)
		*cp++ = 'K';
	if (flag & L_SA)
		*cp++ = 'a';
	if (flag & L_DETACHED)
		*cp++ = '-';
	*cp = '\0';
	strprintorsetwidth(v, buf, mode);
}

void
pnice(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	VAR *v;

	v = ve->var;
	intprintorsetwidth(v, k->p_nice - NZERO, mode);
}

void
pri(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_lwp *l = pi->li;
	VAR *v;

	v = ve->var;
	intprintorsetwidth(v, l->l_priority, mode);
}

void
usrname(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	VAR *v;

	v = ve->var;
	strprintorsetwidth(v, user_from_uid(k->p_uid, 0), mode);
}

void
runame(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	VAR *v;

	v = ve->var;
	strprintorsetwidth(v, user_from_uid(k->p_ruid, 0), mode);
}

void
svuname(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	VAR *v;

	v = ve->var;
	strprintorsetwidth(v, user_from_uid(k->p_svuid, 0), mode);
}

void
gname(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	VAR *v;

	v = ve->var;
	strprintorsetwidth(v, group_from_gid(k->p_gid, 0), mode);
}

void
rgname(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	VAR *v;

	v = ve->var;
	strprintorsetwidth(v, group_from_gid(k->p_rgid, 0), mode);
}

void
svgname(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	VAR *v;

	v = ve->var;
	strprintorsetwidth(v, group_from_gid(k->p_svgid, 0), mode);
}

void
tdev(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	VAR *v;
	dev_t dev;
	char buff[16];

	v = ve->var;
	dev = k->p_tdev;
	if (dev == NODEV) {
		if (mode == PRINTMODE)
			(void)printf("%*s", v->width, "?");
		else
			if (v->width < 2)
				v->width = 2;
	} else {
		(void)snprintf(buff, sizeof(buff),
		    "%lld/%lld", (long long)major(dev), (long long)minor(dev));
		strprintorsetwidth(v, buff, mode);
	}
}

void
tname(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	VAR *v;
	dev_t dev;
	const char *ttname;
	int noctty;

	v = ve->var;
	dev = k->p_tdev;
	if (dev == NODEV || (ttname = devname(dev, S_IFCHR)) == NULL) {
		if (mode == PRINTMODE)
			(void)printf("%-*s", v->width, "?");
		else
			if (v->width < 2)
				v->width = 2;
	} else {
#ifdef __minix
		/* Actually shorten TTY names.   "console" is *really* long. */
		if (strcmp(ttname, "console") == 0)
			ttname = "co";
		else if (strncmp(ttname, "tty", 3) == 0 && ttname[3] != '\0')
			ttname += 3;
		else if (strncmp(ttname, "pts/", 4) == 0 && ttname[4] != '\0')
			ttname += 4; /* this is what FreeBSD does */
#endif /* __minix */
		noctty = !(k->p_eflag & EPROC_CTTY) ? 1 : 0;
		if (mode == WIDTHMODE) {
			int fmtlen;

			fmtlen = strlen(ttname) + noctty;
			if (v->width < fmtlen)
				v->width = fmtlen;
		} else {
			if (noctty)
				(void)printf("%-*s-", v->width - 1, ttname);
			else
				(void)printf("%-*s", v->width, ttname);
		}
	}
}

void
longtname(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	VAR *v;
	dev_t dev;
	const char *ttname;

	v = ve->var;
	dev = k->p_tdev;
	if (dev == NODEV || (ttname = devname(dev, S_IFCHR)) == NULL) {
		if (mode == PRINTMODE)
			(void)printf("%-*s", v->width, "?");
		else
			if (v->width < 2)
				v->width = 2;
	} else {
		strprintorsetwidth(v, ttname, mode);
	}
}

void
started(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	VAR *v;
	time_t startt;
	struct tm *tp;
	char buf[100], *cp;

	v = ve->var;
	if (!k->p_uvalid) {
		if (mode == PRINTMODE)
			(void)printf("%*s", v->width, "-");
		return;
	}

	startt = k->p_ustart_sec;
	tp = localtime(&startt);
	if (now == 0)
		(void)time(&now);
	if (now - k->p_ustart_sec < SECSPERDAY)
		safe_strftime(buf, sizeof(buf) - 1, "%l:%M%p", tp);
	else if (now - k->p_ustart_sec < DAYSPERWEEK * SECSPERDAY)
		safe_strftime(buf, sizeof(buf) - 1, "%a%I%p", tp);
	else
		safe_strftime(buf, sizeof(buf) - 1, "%e%b%y", tp);
	/* %e and %l can start with a space. */
	cp = buf;
	if (*cp == ' ')
		cp++;
	strprintorsetwidth(v, cp, mode);
}

void
lstarted(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	VAR *v;
	time_t startt;
	char buf[100];

	v = ve->var;
	startt = k->p_ustart_sec;

	if (mode == WIDTHMODE) {
		/*
		 * We only need to set the width once, as we assume
		 * that all times are the same length.  We do need to
		 * check against the header length as well, as "no
		 * header" mode for this variable will set the field
		 * width to the length of the header anyway (ref: the
		 * P1003.1-2004 comment in findvar()).
		 *
		 * XXX: The hardcoded "STARTED" string.  Better or
		 * worse than a "<= 7" or some other arbitrary number?
		 */
		if (v->width > (int)sizeof("STARTED") - 1) {
			return;
		}
	} else {
		if (!k->p_uvalid) {
			(void)printf("%*s", v->width, "-");
			return;
		}
	}
	safe_strftime(buf, sizeof(buf) - 1, "%c", localtime(&startt));
	strprintorsetwidth(v, buf, mode);
}

void
elapsed(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	VAR *v;
	int32_t origseconds, secs, mins, hours, days;
	int fmtlen, printed_something;

	v = ve->var;
	if (k->p_uvalid == 0) {
		origseconds = 0;
	} else {
		if (now == 0)
			(void)time(&now);
		origseconds = now - k->p_ustart_sec;
		if (origseconds < 0) {
			/*
			 * Don't try to be fancy if the machine's
			 * clock has been rewound to before the
			 * process "started".
			 */
			origseconds = 0;
		}
	}

	secs = origseconds;
	mins = secs / SECSPERMIN;
	secs %= SECSPERMIN;
	hours = mins / MINSPERHOUR;
	mins %= MINSPERHOUR;
	days = hours / HOURSPERDAY;
	hours %= HOURSPERDAY;

	if (mode == WIDTHMODE) {
		if (origseconds == 0)
			/* non-zero so fmtlen is calculated at least once */
			origseconds = 1;

		if (origseconds > v->longestp) {
			v->longestp = origseconds;

			if (days > 0) {
				/* +9 for "-hh:mm:ss" */
				fmtlen = iwidth(days) + 9;
			} else if (hours > 0) {
				/* +6 for "mm:ss" */
				fmtlen = iwidth(hours) + 6;
			} else {
				/* +3 for ":ss" */
				fmtlen = iwidth(mins) + 3;
			}

			if (fmtlen > v->width)
				v->width = fmtlen;
		}
	} else {
		printed_something = 0;
		fmtlen = v->width;

		if (days > 0) {
			(void)printf("%*d", fmtlen - 9, days);
			printed_something = 1;
		} else if (fmtlen > 9) {
			(void)printf("%*s", fmtlen - 9, "");
		}
		if (fmtlen > 9)
			fmtlen = 9;

		if (printed_something) {
			(void)printf("-%.*d", fmtlen - 7, hours);
			printed_something = 1;
		} else if (hours > 0) {
			(void)printf("%*d", fmtlen - 6, hours);
			printed_something = 1;
		} else if (fmtlen > 6) {
			(void)printf("%*s", fmtlen - 6, "");
		}
		if (fmtlen > 6)
			fmtlen = 6;

		/* Don't need to set fmtlen or printed_something any more... */
		if (printed_something) {
			(void)printf(":%.*d", fmtlen - 4, mins);
		} else if (mins > 0) {
			(void)printf("%*d", fmtlen - 3, mins);
		} else if (fmtlen > 3) {
			(void)printf("%*s", fmtlen - 3, "0");
		}

		(void)printf(":%.2d", secs);
	}
}

void
wchan(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_lwp *l = pi->li;
	VAR *v;

	v = ve->var;
	if (l->l_wmesg[0]) {
		strprintorsetwidth(v, l->l_wmesg, mode);
		v->width = min(v->width, KI_WMESGLEN);
	} else {
		if (mode == PRINTMODE)
			(void)printf("%-*s", v->width, "-");
	}
}

#define	pgtok(a)        (((a)*(size_t)getpagesize())/1024)

void
vsize(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	VAR *v;

	v = ve->var;
	intprintorsetwidth(v, pgtok(k->p_vm_msize), mode);
}

void
rssize(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	VAR *v;

	v = ve->var;
	/* XXX don't have info about shared */
	intprintorsetwidth(v, pgtok(k->p_vm_rssize), mode);
}

void
p_rssize(struct pinfo *pi, VARENT *ve, enum mode mode)	/* doesn't account for text */
{
	struct kinfo_proc2 *k = pi->ki;
	VAR *v;

	v = ve->var;
	intprintorsetwidth(v, pgtok(k->p_vm_rssize), mode);
}

void
cpuid(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_lwp *l = pi->li;
	VAR *v;

	v = ve->var;
	intprintorsetwidth(v, l->l_cpuid, mode);
}

static void
cputime1(int32_t secs, int32_t psecs, VAR *v, enum mode mode)
{
	int fmtlen;

	/*
	 * round and scale to 100's
	 */
	psecs = (psecs + 5000) / 10000;
	secs += psecs / 100;
	psecs = psecs % 100;

	if (mode == WIDTHMODE) {
		/*
		 * Ugg, this is the only field where a value of 0 is longer
		 * than the column title.
		 * Use SECSPERMIN, because secs is divided by that when
		 * passed to iwidth().
		 */
		if (secs == 0)
			secs = SECSPERMIN;

		if (secs > v->longestp) {
			v->longestp = secs;
			/* "+6" for the ":%02ld.%02ld" in the printf() below */
			fmtlen = iwidth(secs / SECSPERMIN) + 6;
			if (fmtlen > v->width)
				v->width = fmtlen;
		}
	} else {
		(void)printf("%*ld:%02ld.%02ld", v->width - 6,
		    (long)(secs / SECSPERMIN), (long)(secs % SECSPERMIN),
		    (long)psecs);
	}
}

void
cputime(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	VAR *v;
	int32_t secs;
	int32_t psecs;	/* "parts" of a second. first micro, then centi */

	v = ve->var;

	/*
	 * This counts time spent handling interrupts.  We could
	 * fix this, but it is not 100% trivial (and interrupt
	 * time fractions only work on the sparc anyway).	XXX
	 */
	secs = k->p_rtime_sec;
	psecs = k->p_rtime_usec;
	if (sumrusage) {
		secs += k->p_uctime_sec;
		psecs += k->p_uctime_usec;
	}

	cputime1(secs, psecs, v, mode);
}

void
lcputime(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_lwp *l = pi->li;
	VAR *v;
	int32_t secs;
	int32_t psecs;	/* "parts" of a second. first micro, then centi */

	v = ve->var;

	secs = l->l_rtime_sec;
	psecs = l->l_rtime_usec;

	cputime1(secs, psecs, v, mode);
}

void
pcpu(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	VAR *v;
	double dbl;

	v = ve->var;
	dbl = pi->pcpu;
	doubleprintorsetwidth(v, dbl, (dbl >= 99.95) ? 0 : 1, mode);
}

double
getpmem(const struct kinfo_proc2 *k)
{
	double fracmem;
	int szptudot;

	if (!nlistread)
		donlist();

	/* XXX want pmap ptpages, segtab, etc. (per architecture) */
	szptudot = uspace/getpagesize();
	/* XXX don't have info about shared */
	fracmem = ((float)k->p_vm_rssize + szptudot)/mempages;
	return (100.0 * fracmem);
}

void
pmem(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	VAR *v;

	v = ve->var;
	doubleprintorsetwidth(v, getpmem(k), 1, mode);
}

void
pagein(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	VAR *v;

	v = ve->var;
	intprintorsetwidth(v, k->p_uvalid ? k->p_uru_majflt : 0, mode);
}

void
maxrss(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	VAR *v;

	v = ve->var;
	/* No need to check width! */
	if (mode == PRINTMODE)
		(void)printf("%*s", v->width, "-");
}

void
tsize(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_proc2 *k = pi->ki;
	VAR *v;

	v = ve->var;
	intprintorsetwidth(v, pgtok(k->p_vm_tsize), mode);
}

static void
printsig(VAR *v, const sigset_t *s, enum mode mode)
{
#define	SIGSETSIZE	__arraycount(s->__bits)
	if ((v->flag & ALTPR) == 0) {
		char buf[SIGSETSIZE * 8 + 1];
		size_t i;

		for (i = 0; i < SIGSETSIZE; i++)
			(void)snprintf(&buf[i * 8], 9, "%.8x",
			    s->__bits[(SIGSETSIZE - 1) - i]);

		/* Skip leading zeroes */
		for (i = 0; buf[i] == '0'; i++)
			continue;

		if (buf[i] == '\0')
			i--;
		strprintorsetwidth(v, buf + i, mode);
	} else {
		size_t maxlen = 1024, len = 0;
		char *buf = emalloc(maxlen);
		*buf = '\0';
		for (size_t i = 0; i < SIGSETSIZE; i++) {
			uint32_t m = s->__bits[i];
			for (uint32_t j = 0; j < 32; j++) {
				if ((m & (1 << j)) == 0)
					continue;
				const char *n = signalname(j + 1);
				size_t sn = strlen(n);
				if (len)
					sn++;
				if (len + sn >= maxlen) {
					maxlen += 1024;
					buf = erealloc(buf, maxlen);
				}
				snprintf(buf + len, sn + 1, "%s%s",
				    len == 0 ? "" : ",", n);
				len += sn;
			}
		}
		strprintorsetwidth(v, buf, mode);
		free(buf);
#undef SIGSETSIZE
	}
}

static void
printflag(VAR *v, int flag, enum mode mode)
{
	char buf[1024];
	const char *fmt;

	switch (v->type) {
	case PROCFLAG:
		fmt = __SYSCTL_PROC_FLAG_BITS;
		break;
	case KTRACEFLAG:
		fmt = __KTRACE_FLAG_BITS;
		break;
	case PROCACFLAG:
		fmt = __ACCT_FLAG_BITS;
		break;
	default:
		err(EXIT_FAILURE, "Bad type %d", v->type);
	}

	snprintb(buf, sizeof(buf), fmt, (unsigned)flag);
	strprintorsetwidth(v, buf, mode);
}

/*
 * Generic output routines.  Print fields from various prototype
 * structures.
 */
static void
printval(void *bp, VAR *v, enum mode mode)
{
	static char ofmt[32] = "%";
	int width, vok, fmtlen;
	const char *fcp;
	char *cp;
	int64_t val;
	u_int64_t uval;

	val = 0;	/* XXXGCC -Wuninitialized [hpcarm] */
	uval = 0;	/* XXXGCC -Wuninitialized [hpcarm] */

	/*
	 * Note that the "INF127" check is nonsensical for types
	 * that are or can be signed.
	 */
#define	GET(type)		(*(type *)bp)
#define	CHK_INF127(n)		(((n) > 127) && (v->flag & INF127) ? 127 : (n))

#define	VSIGN	1
#define	VUNSIGN	2
#define	VPTR	3

	if (mode == WIDTHMODE) {
		vok = 0;
		switch (v->type) {
		case CHAR:
			val = GET(char);
			vok = VSIGN;
			break;
		case UCHAR:
			uval = CHK_INF127(GET(u_char));
			vok = VUNSIGN;
			break;
		case SHORT:
			val = GET(short);
			vok = VSIGN;
			break;
		case PROCACFLAG:
			if (v->flag & ALTPR)
				break;
			/*FALLTHROUGH*/
		case USHORT:
			uval = CHK_INF127(GET(u_short));
			vok = VUNSIGN;
			break;
		case INT32:
			val = GET(int32_t);
			vok = VSIGN;
			break;
		case KTRACEFLAG:
		case PROCFLAG:
			if (v->flag & ALTPR)
				break;
			/*FALLTHROUGH*/
		case INT:
			val = GET(int);
			vok = VSIGN;
			break;
		case UINT:
		case UINT32:
			uval = CHK_INF127(GET(u_int));
			vok = VUNSIGN;
			break;
		case LONG:
			val = GET(long);
			vok = VSIGN;
			break;
		case ULONG:
			uval = CHK_INF127(GET(u_long));
			vok = VUNSIGN;
			break;
		case KPTR:
			uval = GET(u_int64_t);
			vok = VPTR;
			break;
		case KPTR24:
			uval = GET(u_int64_t);
			uval &= 0xffffff;
			vok = VPTR;
			break;
		case INT64:
			val = GET(int64_t);
			vok = VSIGN;
			break;
		case UINT64:
			uval = CHK_INF127(GET(u_int64_t));
			vok = VUNSIGN;
			break;

		case SIGLIST:
		default:
			/* nothing... */;
		}
		switch (vok) {
		case VSIGN:
			if (val < 0 && val < v->longestn) {
				v->longestn = val;
				fmtlen = iwidth(-val) + 1;
				if (fmtlen > v->width)
					v->width = fmtlen;
			} else if (val > 0 && val > v->longestp) {
				v->longestp = val;
				fmtlen = iwidth(val);
				if (fmtlen > v->width)
					v->width = fmtlen;
			}
			return;
		case VUNSIGN:
			if (uval > v->longestu) {
				v->longestu = uval;
				v->width = iwidth(uval);
			}
			return;
		case VPTR:
			fmtlen = 0;
			while (uval > 0) {
				uval >>= 4;
				fmtlen++;
			}
			if (fmtlen > v->width)
				v->width = fmtlen;
			return;
		}
	}

	width = v->width;
	cp = ofmt + 1;
	fcp = v->fmt;
	if (v->flag & LJUST)
		*cp++ = '-';
	*cp++ = '*';
	while ((*cp++ = *fcp++) != '\0')
		continue;

	switch (v->type) {
	case CHAR:
		(void)printf(ofmt, width, GET(char));
		return;
	case UCHAR:
		(void)printf(ofmt, width, CHK_INF127(GET(u_char)));
		return;
	case SHORT:
		(void)printf(ofmt, width, GET(short));
		return;
	case PROCACFLAG:
		if (v->flag & ALTPR) {
			printflag(v, CHK_INF127(GET(u_short)), mode);
			return;
		}
		/*FALLTHROUGH*/
	case USHORT:
		(void)printf(ofmt, width, CHK_INF127(GET(u_short)));
		return;
	case KTRACEFLAG:
	case PROCFLAG:
		if (v->flag & ALTPR) {
			printflag(v, GET(int), mode);
			return;
		}
		/*FALLTHROUGH*/
	case INT:
		(void)printf(ofmt, width, GET(int));
		return;
	case UINT:
		(void)printf(ofmt, width, CHK_INF127(GET(u_int)));
		return;
	case LONG:
		(void)printf(ofmt, width, GET(long));
		return;
	case ULONG:
		(void)printf(ofmt, width, CHK_INF127(GET(u_long)));
		return;
	case KPTR:
		(void)printf(ofmt, width, GET(u_int64_t));
		return;
	case KPTR24:
		(void)printf(ofmt, width, GET(u_int64_t) & 0xffffff);
		return;
	case INT32:
		(void)printf(ofmt, width, GET(int32_t));
		return;
	case UINT32:
		(void)printf(ofmt, width, CHK_INF127(GET(u_int32_t)));
		return;
	case INT64:
		(void)printf(ofmt, width, GET(int64_t));
		return;
	case UINT64:
		(void)printf(ofmt, width, CHK_INF127(GET(u_int64_t)));
		return;
	case SIGLIST:
		printsig(v, (const sigset_t *)(void *)bp, mode);
		return;
	default:
		errx(EXIT_FAILURE, "unknown type %d", v->type);
	}
#undef GET
#undef CHK_INF127
}

void
pvar(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	VAR *v = ve->var;
	char *b = (v->flag & LWP) ? (char *)pi->li : (char *)pi->ki;

	if ((v->flag & UAREA) && !pi->ki->p_uvalid) {
		if (mode == PRINTMODE)
			(void)printf("%*s", v->width, "-");
		return;
	}

	(void)printval(b + v->off, v, mode);
}

void
putimeval(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	VAR *v = ve->var;
	struct kinfo_proc2 *k = pi->ki;
	char *b = (v->flag & LWP) ? (char *)pi->li : (char *)pi->ki;
	ulong secs = *(uint32_t *)(b + v->off);
	ulong usec = *(uint32_t *)(b + v->off + sizeof (uint32_t));
	int fmtlen;

	if (!k->p_uvalid) {
		if (mode == PRINTMODE)
			(void)printf("%*s", v->width, "-");
		return;
	}

	if (mode == WIDTHMODE) {
		if (secs == 0)
			/* non-zero so fmtlen is calculated at least once */
			secs = 1;
		if (secs > v->longestu) {
			v->longestu = secs;
			if (secs <= 999)
				/* sss.ssssss */
				fmtlen = iwidth(secs) + 6 + 1;
			else
				/* hh:mm:ss.ss */
				fmtlen = iwidth((secs + 1) / SECSPERHOUR)
					+ 2 + 1 + 2 + 1 + 2 + 1;
			if (fmtlen > v->width)
				v->width = fmtlen;
		}
		return;
	}

	if (secs < 999)
		(void)printf( "%*lu.%.6lu", v->width - 6 - 1, secs, usec);
	else {
		uint h, m;
		usec += 5000;
		if (usec >= 1000000) {
			usec -= 1000000;
			secs++;
		}
		m = secs / SECSPERMIN;
		secs -= m * SECSPERMIN;
		h = m / MINSPERHOUR;
		m -= h * MINSPERHOUR;
		(void)printf( "%*u:%.2u:%.2lu.%.2lu", v->width - 9, h, m, secs,
		    usec / 10000u );
	}
}

void
lname(struct pinfo *pi, VARENT *ve, enum mode mode)
{
	struct kinfo_lwp *l = pi->li;
	VAR *v;

	v = ve->var;
	if (l->l_name[0] != '\0') {
		strprintorsetwidth(v, l->l_name, mode);
		v->width = min(v->width, KI_LNAMELEN);
	} else {
		if (mode == PRINTMODE)
			(void)printf("%-*s", v->width, "-");
	}
}
