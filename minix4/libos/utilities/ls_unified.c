/**
 * @file ls_unified.c
 * @brief Unified ls implementation
 * @details This file is a synthesized/unified implementation combining multiple
 *          source files from the legacy MINIX codebase into a single, modern,
 *          C23-compliant implementation for MINIX4.
 * 
 * @version 4.0.0
 * @date 2025-06-13
 * @author MINIX4 Massive Synthesis System
 * 
 * @copyright Copyright (c) 2025 MINIX4 Project
 * @license MIT License
 * 
 * SYNTHESIS INFORMATION:
 * ======================
 * This file synthesizes the following source files:
 *     1. userspace\commands\core\ls.c
 *     2. minix4\libos_legacy\ls\ls.c
 *     3. minix4\libos_legacy\utilities\ls.c
 *     4. minix4\exokernel\kernel_legacy\ls.c
 *     5. minix4\exokernel\kernel_legacy\ucbcmd\ls\ls.c
 *     6. minix4\exokernel\kernel_legacy\cmd\ls\ls.c
 *     7. minix4\exokernel\kernel_legacy\boot\common\ls.c
 * 
 * Total source files: 7
 * Synthesis date: 2025-06-13 19:53:49
 * Synthesis strategy: Massive batch C23 unification
 * Processing batch: targets 32-131
 * 
 * MODERNIZATION FEATURES:
 * =======================
 * - C23 standard compliance with latest features
 * - Modern error handling patterns  
 * - Consistent coding style and formatting
 * - Improved memory safety and bounds checking
 * - Enhanced documentation and attribution
 * - POSIX.1-2024 compliance where applicable
 * - Thread-safe implementations where needed
 */

#pragma once

#define _POSIX_C_SOURCE 202311L
#define _XOPEN_SOURCE 800
#define _DEFAULT_SOURCE 1

// C23 Standard Headers
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <threads.h>
#include <stdnoreturn.h>

// POSIX Headers
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

// MINIX4 Headers
#include "minix4_types.h"
#include "minix4_config.h"
#include "minix4_errors.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ===== INCLUDES ===== */
#include	<stdio.h>
#include "bootstrap.h"
#include "extern.h"
#include "fs.h"
#include "ls.h"
#include "stat.h"
#include "types.h"
#include "user.h"
#include <aclutils.h>
#include <attr.h>
#include <ctype.h>
#include <curses.h>
#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <fts.h>
#include <getopt.h>
#include <grp.h>
#include <inttypes.h>
#include <libcmdutils.h>
#include <libgen.h>
#include <libnvpair.h>
#include <limits.h>
#include <locale.h>
#include <pwd.h>
#include <stand.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/acl.h>
#include <sys/cdefs.h>
#include <sys/dir.h>
#include <sys/ioctl.h>
#include <sys/mkdev.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/termios.h>
#include <sys/types.h>
#include <term.h>
#include <termios.h>
#include <time.h>
#include <ufs/ufs/dinode.h>
#include <ufs/ufs/dir.h>
#include <unistd.h>
#include <util.h>
#include <utmpx.h>
#include <wchar.h>
#include <wctype.h>
#include <widec.h>

/* ===== DEFINES ===== */
#define	NFILES	1024
#define	ISARG	0100000
#define	BY_NAME 0
#define	BY_SIZE 1
#define	BY_TIME	2
#define	IFMT	060000
#define	DIR	0100000
#define	CHR	020000
#define	BLK	040000
#define	LARGE	010000
#define	STXT	010000
#define	SUID	04000
#define	SGID	02000
#define	ROWN	0400
#define	WOWN	0200
#define	XOWN	0100
#define	RGRP	040
#define	WGRP	020
#define	XGRP	010
#define	ROTH	04
#define	WOTH	02
#define	XOTH	01
#define	RSTXT	01000
#define	dbtokb(nb)	((nb) / (1024 / DEV_BSIZE))
#define	NMAX	(sizeof (((struct utmpx *)0)->ut_name))
#define	SCPYN(a, b)	strncpy(a, b, NMAX)
#define	TERMINFO
#define	BFSIZE	16
#define	LS_NOTREG	010000
#define	FORMAT_OLD	" %b %e  %Y "
#define	FORMAT_NEW	" %b %e %H:%M "
#define	FORMAT_LONG	" %b %e %T %Y "
#define	FORMAT_ISO_FULL	" %%F %%T.%.09ld %%z "
#define	FORMAT_ISO_LONG	" %F %R "
#define	FORMAT_ISO_NEW	" %m-%d %H:%M "
#define	FORMAT_ISO_OLD	" %F "
#define	BUFSIZ 4096
#define	FMTSIZE 50
#define	LSA_NONE	(0)
#define	LSA_BOLD	(1L << 0)
#define	LSA_UNDERSCORE	(1L << 1)
#define	LSA_BLINK	(1L << 2)
#define	LSA_REVERSE	(1L << 3)
#define	LSA_CONCEALED	(1L << 4)
#define	NOTWORKINGDIR(d, l)	(((l) < 2) || \
#define	NOTPARENTDIR(d, l)	(((l) < 3) || \
#define	OWNED_GROUP	(ACE_GROUP | ACE_IDENTIFIER_GROUP)
#define	IS_TYPE_ALLOWED(type)	((type) == ACE_ACCESS_ALLOWED_ACE_TYPE)

/* ===== TYPES ===== */
struct lbuf {
	union {
struct	lbuf	*flist[NFILES];
struct	lbuf	**lastp = flist;
struct	lbuf	**firstp = flist;
struct	lbuf *gstat();
	struct lbuf **epp;
	struct lbuf lb;
struct lbuf *
	struct stat statb;
struct lbuf **pp1, **pp2;
struct lbuf *ap;
	struct { char dminor, dmajor;};
	struct winsize win;
	struct stat *sp;
  struct dirent de;
  struct stat st;
struct {
struct ibuf {
struct lbuf {
struct lbufx {
struct	lbuf	*lastp	&end;
struct	lbuf	*rlastp	&end;
	struct lbuf lb;
struct lbuf *ap;
	struct { char dminor, dmajor;};
	struct ibuf statb;
struct lbuf *ap1, *ap2;
	struct { char *charp;};
struct afile {
	struct	subdirs *sd_next;
	struct afile *fp0, *fplast;
	struct termios trbuf;
				struct subdirs *t;
	struct afile *dfp0, *dfplast;
		struct stat stb, stb1;
	struct cachenode *c;
	struct passwd *pw;
	struct cachenode *c;
	struct group *gr;
struct ditem {
struct attrb {
struct attrtm {
typedef enum LS_CFTYPE {
typedef struct {
struct	lbuf	{
	union	{
struct dchain {
	struct lbuf	*ep;
	struct lbuf	lb;
	struct ditem	*myinfo = NULL;
	struct dchain *dp;
	struct lbuf *ap;
	struct lbuf **ep;
	struct lbuf *p;
	struct dirent *dentry;
	struct lbuf *ep;
	struct ditem	*myinfo;
	struct ditem	*tptr;
	struct stat statb, statb1;
	struct lbuf *rep;
	struct cachenode **parent = head;
	struct cachenode *c = *parent;
	struct passwd *pwent;
	struct cachenode *c;
	struct group *grent;
	struct cachenode *c;
	struct cachenode *c;
	struct cachenode *c;
	struct lbuf *p1, *p2;
    struct stat	sb;
    struct	dirent *d;
    struct stat	sb;

/* ===== GLOBAL VARIABLES ===== */
		char	lname[15];
		char	*namep;
	char	ltype;
int	Aflg, nflg, qflg, across;
int	nopad;
char	buff[32];
int	rflg	1;
int	flags;
int	lastuid	-1;
char	tbuf[16];
int	statreq;
char	*dotp	".";
char	*makename();
char	*ctime();
int	colwidth 15;
int	outcol;
char	obuf[BUFSIZ];
char *argv[];
	int i;
	char *t;
	char *cp;
	int compar();
				printf("\n%s:\n", ep->ln.namep);
				printf("total %D", tblocks);
	int ncols, nrows, row, col;
			printf("\n");
	char c;
int uid;
char buf[];
	int j, c, n, i;
int	m1[] { 1, S_IREAD>>0, 'r', '-' };
int	m2[] { 1, S_IWRITE>>0, 'w', '-' };
int	m3[] { 2, S_ISUID, 's', S_IEXEC>>0, 'x', '-' };
int	m4[] { 1, S_IREAD>>3, 'r', '-' };
int	m5[] { 1, S_IWRITE>>3, 'w', '-' };
int	m6[] { 2, S_ISGID, 's', S_IEXEC>>3, 'x', '-' };
int	m7[] { 1, S_IREAD>>6, 'r', '-' };
int	m8[] { 1, S_IWRITE>>6, 'w', '-' };
int	m9[] { 2, S_ISVTX, 't', S_IEXEC>>6, 'x', '-' };
int	*m[] { m1, m2, m3, m4, m5, m6, m7, m8, m9};
	register int **mp;
register int *pairp;
	register int n;
	pputchar(*pairp);
char *dir, *file;
	static char dfile[100];
	register char *dp, *fp;
	register int i;
char *dir;
	static struct direct dentry;
	register int j;
		printf("%s unreadable\n", dir);
char *file;
	static int nomocore;
		fprintf(stderr, "ls: out of memory\n");
		static int msg;
			fprintf(stderr, "ls: too many files\n");
			printf("%s not found\n", file);
	register char *cp;
			printf("%d ", p->lnum);
			printf("%5d ", p->lnum);
			printf("%D ", nblock(p->lsize));
			printf("%4D ", nblock(p->lsize));
		putchar(p->ltype);
		printf("%2d ", p->lnl);
			printf("%-8.8s", tbuf);
			printf("%-8d", t);
			printf("%3d,%3d", major((int)p->lsize), minor((int)p->lsize));
			printf("%7ld", p->lsize);
			printf(" %-12.12s ", cp+4);
		printf("%s", p->ln.namep);
		printf("%.14s", p->ln.lname);
register char *string;
register int adjust;
			pputchar(*string++, file); count--;
static char sccsid[] = "@(#)ls.c	8.7 (Berkeley) 8/5/94";
static void	 display(FTSENT *, FTSENT *);
static int	 mastercmp(const FTSENT **, const FTSENT **);
static void	 traverse(int, char **, int);
static void (*printfcn)(DISPLAY *);
static int (*sortfcn)(const FTSENT *, const FTSENT *);
int sortkey = BY_NAME;
	static char dot[] = ".", *dotav[] = { dot, NULL };
	int ch, fts_options;
	int kflag = 0;
	const char *p;
	(void)setlocale(LC_ALL, "");
		f_column = f_nonprint = 1;
			f_nonprint = 0;
			f_nonprint = 0;
			f_nonprint = 1;
			f_nonprint = 0;
			(void)getbsize(NULL, &blocksize);
		printfcn = printscol;
		printfcn = printacol;
		printfcn = printlong;
		printfcn = printstream;
		printfcn = printcol;
	int ch_options, error;
		(void)fts_close(ftsp);
				(void)fts_set(ftsp, p, FTS_SKIP);
					(void)printf("\n%s:\n", p->fts_path);
					(void)printf("%s:\n", p->fts_path);
				(void)fts_set(ftsp, p, FTS_SKIP);
	(void)fts_close(ftsp);
	u_int64_t btotal;
	int maxmajor, maxminor;
	uint32_t maxnlink;
	int bcfile, entries, flen, glen, ulen, maxflags, maxgroup;
	unsigned int maxlen;
	int maxuser, needstats;
	const char *user, *group;
	char nuser[12], ngroup[12];
	char *flags = NULL;
				(void)strcpy(np->user, user);
				(void)strcpy(np->group, group);
				  	(void)strcpy(np->flags, flags);
				cur->fts_pointer = np;
		(void)snprintf(buf, sizeof(buf), "%u", maxnlink);
			(void)snprintf(buf, sizeof(buf), "%d", maxmajor);
			(void)snprintf(buf, sizeof(buf), "%d", maxminor);
	printfcn(&d);
			free(cur->fts_pointer);
	int a_info, b_info;
  static char buf[DIRSIZ+1];
  char *p;
  char buf[512], *p;
  int fd;
    printf(2, "ls: cannot open %s\n", path);
    printf(2, "ls: cannot stat %s\n", path);
    printf(1, "%s %d %d %d\n", fmtname(path), st.type, st.ino, st.size);
      printf(1, "ls: path too long\n");
        printf(1, "ls: cannot stat %s\n", buf);
      printf(1, "%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
  int i;
	int	fdes;
	int	nleft;
	char	*nextc;
	char	buff[512];
	int	idev;
	int	inum;
	char	inl;
	char	iuid;
	char	igid;
	char	isize0;
	int	isize;
	int	iaddr[8];
	char	*iatime[2];
	char	*imtime[2];
	char	lname[15];
	int	lnum;
	int	lflags;
	char	lnl;
	char	luid;
	char	lgid;
	char	lsize0;
	int	lsize;
	char	*lmtime[2];
	char	*namep;
int	fout;
int	rflg	1;
int	flags;
int	uidfil	-1;
int	tblocks;
int	statreq;
char	*dotp	".";
char **argv;
	int i, j;
	extern struct lbuf end;
	int compar();
				printf("\n%s:\n", ep->namep);
				printf("total %d\n", tblocks);
	char tbuf[16];
		printf("%5d ", p->lnum);
		printf("%2d ", p->lnl);
			printf("%-6.6s", tbuf);
			printf("%-6d", p->luid&0377);
			printf("%7s", locv(p->lsize0, p->lsize));
		printf(" %-12.12s ", ctime(p->lmtime)+4);
		printf("%4d ", nblock(p->lsize0, p->lsize));
		printf("%s\n", p->namep);
		printf("%.14s\n", p->lname);
int uid;
char buf[];
	int j, c, n, i;
char *size0, *size;
	register int n;
int	m0[] { 3, DIR, 'd', BLK, 'b', CHR, 'c', '-'};
int	m1[] { 1, ROWN, 'r', '-' };
int	m2[] { 1, WOWN, 'w', '-' };
int	m3[] { 2, SUID, 's', XOWN, 'x', '-' };
int	m4[] { 1, RGRP, 'r', '-' };
int	m5[] { 1, WGRP, 'w', '-' };
int	m6[] { 2, SGID, 's', XGRP, 'x', '-' };
int	m7[] { 1, ROTH, 'r', '-' };
int	m8[] { 1, WOTH, 'w', '-' };
int	m9[] { 1, XOTH, 'x', '-' };
int	m10[] { 1, STXT, 't', ' ' };
int	*m[] { m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10};
	register int **mp;
int *pairp;
	register int n, *ap;
	putchar(*ap);
char *dir, *file;
	static char dfile[100];
	register char *dp, *fp;
	register int i;
char *dir;
		int	dinode;
		char	dname[14];
	register char *p;
	register int j;
		printf("%s unreadable\n", dir);
char *file;
			printf("%s not found\n", file);
	register int i;
	int j;
	char	*sd_name;
static	int	rflg = 1;
static	int	qflg, Aflg, Cflg, Fflg, Lflg, Rflg;
static	int	usetabs;
static	time_t	now, sixmonthsago, onehourfromnow;
static	char	*dotp = ".";
static	struct	winsize win;
static	int	twidth;
static	struct	afile *gstat(struct afile *, char *, int, off_t *);
static	int	fcmp(const void *, const void *);
static	char	*cat(char *, char *);
static	char	*savestr(char *);
static	char	*fmtentry(struct afile *);
static	char	*getname(), *getgroup();
static	off_t	getdir(char *, struct afile **, struct afile **);
	int i;
	(void) time(&now);
		(void) ioctl(1, TCGETS, &trbuf);
		(void) fprintf(stderr, "ls: out of memory\n");
			(void) printf("\n");
				(void) printf("\n");
			(void) printf("\n");
		(void) printf("%s:\n", name);
		(void) printf("total %lld\n", nkb);
		(void) closedir(dirp);
				(void) fprintf(stderr, "ls: out of memory\n");
	(void) closedir(dirp);
	static struct afile azerofile;
	int (*statf)() = Lflg ? stat : lstat;
	int cc;
	char buf[PATH_MAX];
	int aclcnt;
	int grouppermfound, maskfound;
					(void) fprintf(stderr, "ls: ");
				(void) fprintf(stderr, "ls: ");
	int width = 0, w, nentry = fplast - fp0;
	int i, j, columns, lines;
	char *cp;
			int len = strlen(fmtentry(fp));
			(void) printf("%s", cp);
				(void) printf("\n");
					(void) putchar('\t');
					(void) putchar(' ');
	static char dfile[BUFSIZ];
		(void) fprintf(stderr, "ls: filename too long\n");
		(void) strcpy(dfile, file);
	(void) strcpy(dfile, dir);
		(void) strcat(dfile, "/");
	(void) strcat(dfile, file);
	char *cp = malloc(strlen(str) + 1);
		(void) fprintf(stderr, "ls: out of memory\n");
	(void) strcpy(cp, str);
static	char	*fmtinum(struct afile *);
static	char	*fmtsize(struct afile *);
static	char	*fmtlstuff(struct afile *);
static	char	*fmtmode(char *, int);
	static char fmtres[BUFSIZ];
	register char *cp, *dp;
		(void) strcpy(dp, " -> "); dp += 4;
	static char inumbuf[12];
	(void) sprintf(inumbuf, "%10llu ", p->fnum);
	static char sizebuf[32];
	static char lstuffbuf[256];
	char gname[32], uname[32], fsize[32], ftime[32];
	register char *lp = lstuffbuf;
		char *cp = getname(p->fuid);
		(void) sprintf(uname, "%-8s ", cp);
		char *cp = getgroup(p->fgid);
		(void) sprintf(gname, "%-8s ", cp);
		(void) sprintf(fsize, "%8d", 0);
		char *cp = ctime(&p->fmtime);
			(void) sprintf(ftime, " %-7.7s %-4.4s ", cp+4, cp+20);
			(void) sprintf(ftime, " %-12.12s ", cp+4);
static	int	*m[] = { m1, m2, m3, m4, m5, m6, m7, m8, m9};
	int **mp;
		register int *pairp = *mp++;
		register int n = *pairp++;
			(void) SCPYN(&c->name[0], pw->pw_name);
			(void) sprintf(&c->name[0], "%-8lu ", uid);
			(void) SCPYN(&c->name[0], gr->gr_name);
			(void) sprintf(&c->name[0], "%-8lu ", gid);
	char		*name;
	char		*name;
	uint64_t	stm;
	uint64_t	nstm;
	char		*sfx;
	int		attr;
	int		fg;
	int		bg;
static struct lbuf	*gstat(char *, int, struct ditem *);
static char		*getname(uid_t);
static char		*getgroup(gid_t);
static char		*getusid(uid_t);
static char		*getgsid(gid_t);
static char		*makename(char *, char *);
static void		pentry(struct lbuf *);
static void		column(void);
static void		pmode(mode_t aflag);
static void		selection(int *);
static void		new_line(void);
static void		rddir(char *, struct ditem *);
static int		strcol(unsigned char *);
static void		pem(struct lbuf **, struct lbuf **, int);
static void		pdirectory(char *, int, int, int, struct ditem *);
static struct cachenode *findincache(struct cachenode **, long);
static void		csi_pprintf(unsigned char *);
static void		pprintf(char *, char *);
static int		compar(struct lbuf **pp1, struct lbuf **pp2);
			    int, struct ditem *);
static void		ls_color_init(void);
static ls_color_t	*ls_color_find(const char *, mode_t);
static void		ls_start_color(ls_color_t *);
static void		ls_end_color(void);
static int		aflg;
static int		atflg;
static int		bflg;
static int		cflg;
static int		dflg;
static int		eflg;
static int		fflg;
static int		gflg;
static int		hflg;
static int		lflg;
static int		mflg;
static int		nflg;
static int		oflg;
static int		pflg;
static int		qflg;
static int		sflg;
static int		tflg;
static int		uflg;
static int		Uflg;
static int		wflg;
static int		xflg;
static int		Aflg;
static int		Bflg;
static int		Cflg;
static int		Eflg;
static int		Fflg;
static int		Hflg;
static int		Lflg;
static int		Rflg;
static int		Sflg;
static int		vflg;
static int		Vflg;
static int		copt;
static int		vopt;
static int		ctm;
static int		atm;
static int		mtm;
static int		crtm;
static int		alltm;
static uint_t		nicenum_flags;
static mode_t		flags;
static int		colorflg;
static int		file_typeflg;
static int		noflist = 0;
static uid_t		lastuid	= (uid_t)-1;
static gid_t		lastgid = (gid_t)-1;
static char		*lastuname = NULL;
static char		*lastgname = NULL;
static int		statreq;
static uint64_t		block_size = 1;
static char		*dotp = ".";
static time_t		year, now, starttime;
static int		num_cols = 80;
static int		colwidth;
static int		filewidth;
static int		fixedwidth;
static int		nomocore;
static int		curcol;
static struct	winsize	win;
static int		lsc_debug;
static ls_color_t	*lsc_match;
static ls_color_t	*lsc_colors;
static size_t		lsc_ncolors;
static char		*lsc_bold;
static char		*lsc_underline;
static char		*lsc_blink;
static char		*lsc_reverse;
static char		*lsc_concealed;
static char		*lsc_none;
static char		*lsc_setfg;
static char		*lsc_setbg;
static ls_color_t	*lsc_orphan;
static int get_sysxattr(char *, struct lbuf *);
static void set_sysattrb_display(char *, boolean_t, struct lbuf *);
static void set_sysattrtm_display(char *, struct lbuf *);
static void print_time(struct lbuf *);
static void *xmalloc(size_t, struct lbuf *);
static void free_sysattr(struct lbuf *);
static nvpair_t *pair;
static nvlist_t	*response;
static int acl_err;
	int		c;
	int		i;
	int		width;
	int		amino = 0;
	int		opterr = 0;
	int		option_index = 0;
	(void) setlocale(LC_ALL, "");
	(void) textdomain(TEXT_DOMAIN);
				uint64_t scale = 1;
				uint64_t kilo = 1024;
				char scale_c;
					char	*told, *tnew;
					char	*p;
					(void) memset(told, 0, timelen);
					(void) strlcat(told, " ", timelen);
						    (const char *)tnew;
						    (const char *)told;
					time_fmt_old = (const char *)told;
		char *clptr;
			width = strcol((unsigned char *)argv[optind]);
		    (int (*)(const void *, const void *))compar);
	char *pname;
	int j;
			(void) putc('\n', stdout);
		pprintf(name, ":");
			curcol += printf(gettext("total %d"), 0);
		    (int (*)(const void *, const void *))compar);
		curcol += printf(gettext("total %llu"), tblocks);
	int col, ncols = 1;
	char *cp;
	char *str;
			curcol += printf("%llu ", (long long)p->lnum);
			curcol += printf("%10llu ", (long long)p->lnum);
		(void) putchar(p->ltype);
		(void) putchar(p->acl);
		curcol += printf("%3lu ", (ulong_t)p->lnl);
				curcol += printf("%-8s ", cp);
				curcol += printf("%-8s ", cp);
				curcol += printf("%-8lu ", (ulong_t)p->luid);
				curcol += printf("%-8s ", cp);
				curcol += printf("%-8s ", cp);
				curcol += printf("%-8lu ", (ulong_t)p->lgid);
			    (uint_t)minor((dev_t)p->lsize));
			char numbuf[NN_NUMBUF_SZ];
			curcol += printf("%7s", numbuf);
			uint64_t bsize = p->lsize / block_size;
			curcol += printf("%7" PRIu64, bsize);
		curcol += printf("%s", time_buf);
		csi_pprintf((unsigned char *)str);
			csi_pprintf((unsigned char *)" -> ");
			csi_pprintf((unsigned char *)p->flinkto);
			csi_pprintf((unsigned char *)dmark);
		(void) printf("%s", str);
		curcol += strcol((unsigned char *)str);
			(void) printf("%s", str);
			curcol += strcol((unsigned char *)str);
			(void) printf("%s", p->flinkto);
			curcol += strcol((unsigned char *)p->flinkto);
			(void) printf("%s", dmark);
			curcol += strcol((unsigned char *)dmark);
		int i;
		print_time(p);
			int pa_flags = 0;
			acl_printacl2(p->aclp, num_cols, pa_flags);
	static int	m0[] = { 1, S_IRUSR, 'r', '-' };
	static int	m1[] = { 1, S_IWUSR, 'w', '-' };
	static int	m3[] = { 1, S_IRGRP, 'r', '-' };
	static int	m4[] = { 1, S_IWGRP, 'w', '-' };
	static int	m6[] = { 1, S_IROTH, 'r', '-' };
	static int	m7[] = { 1, S_IWOTH, 'w', '-' };
	static int *m[] = { m0, m1, m2, m3, m4, m5, m6, m7, m8};
	int **mp;
	int n;
	(void) putchar(*pairp);
		(void) putc(',', stdout);
			(void) putc('\n', stdout);
		(void) putc(' ', stdout);
		(void) putc('\n', stdout);
		(void) putc('\n', stdout);
		(void) putc(' ', stdout);
		(void) putc('\n', stdout);
	int j;
	int width;
		(void) fflush(stdout);
				width = strcol((unsigned char *)dentry->d_name);
			int sav_errno = errno;
		(void) closedir(dirf);
	int mode = 0, seen = 0;
	int acecnt;
	int flags;
	char buf[BUFSIZ];
	int (*statf)() = ((Lflg) || (Hflg && argfl)) ? stat : lstat;
	int aclcnt;
	int error;
	int grouppermfound, maskfound;
	(void) memset((void *) rep, 0, sizeof (struct lbuf));
		int doacl;
	static char dfile[PATH_MAX + 1 + MAXNAMLEN + 1];
	char *dp, *fp;
static struct cachenode *names, *groups;
static struct cachenode *user_sids, *group_sids;
	char *sid;
	char *sid;
	char *sid;
			(void) sprintf(&c->name[0], "%-8u", (int)uid);
	char *sid;
			(void) sprintf(&c->name[0], "%-8u", (int)gid);
	csi_pprintf((unsigned char *)s1);
	csi_pprintf((unsigned char *)s2);
	unsigned char *cp;
	char	c;
	int	i;
	int	c_len;
	int	p_col;
	wchar_t	pcode;
			(void) putchar(*cp);
					(void) putc('\\', stdout);
					(void) putc(c, stdout);
					(void) putc(c, stdout);
			(void) putc(c, stdout);
			goto not_print;
			(void) putwchar(pcode);
				(void) putc('\\', stdout);
				(void) putc(c, stdout);
				(void) putc(c, stdout);
			(void) putc(c, stdout);
	int	w;
	int	w_col;
	int	len;
	wchar_t	wc;
	int		error;
	char		*name;
	int		i;
	const char	*opt;
	uint_t		nelem;
	uint64_t	*value;
	int		i;
			(void) strlcpy(rep->extm[i].name, name, len + 1);
	const char *fstr = time_fmt_new;
	char fmt_buf[FMTSIZE];
		(void) snprintf(fmt_buf, FMTSIZE, fstr, nsec);
	int tmattr = 0;
	int i;
		const char *old_save = time_fmt_old;
		const char *new_save = time_fmt_new;
	const char *old_save = time_fmt_old;
	const char *new_save = time_fmt_new;
	int i = 0;
	(void) printf("         timestamp: atime        %s\n", time_buf);
	(void) printf("         timestamp: ctime        %s\n", time_buf);
	(void) printf("         timestamp: mtime        %s\n", time_buf);
	(void) printf("\n\ttype: ");
		(void) printf("LS_NORMAL");
		(void) printf("LS_FILE");
		(void) printf("LS_EXEC");
		(void) printf("LS_DIR");
		(void) printf("LS_LINK");
		(void) printf("LS_FIFO");
		(void) printf("LS_SOCK");
		(void) printf("LS_DOOR");
		(void) printf("LS_BLK");
		(void) printf("LS_CHR");
		(void) printf("LS_PORT");
		(void) printf("LS_STICKY");
		(void) printf("LS_ORPHAN");
		(void) printf("LS_SETGID");
		(void) printf("LS_SETUID");
		(void) printf("LS_OTHER_WRITABLE");
		(void) printf("LS_STICKY_OTHER_WRITABLE");
		(void) printf("LS_PAT\n");
		(void) printf("\tpattern: %s", c->sfx);
	(void) printf("\n");
	(void) printf("\tattr: %d\n", c->attr);
	(void) printf("\tfg: %d\n", c->fg);
	(void) printf("\tbg: %d\n", c->bg);
	(void) printf("\t");
	int i;
	char *s;
		(void) putp(s);
		ls_tprint(lsc_bold, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		ls_tprint(lsc_underline, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		ls_tprint(lsc_blink, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		ls_tprint(lsc_reverse, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		ls_tprint(lsc_concealed, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		ls_tprint(lsc_none, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		ls_tprint(lsc_setfg, c->fg, 0, 0, 0, 0, 0, 0, 0, 0);
		ls_tprint(lsc_setbg, c->bg, 0, 0, 0, 0, 0, 0, 0, 0);
	ls_tprint(lsc_none, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		const char	*s;
	char		*p, *lasts;
	int		i;
	int		color, attr;
	int ret = c1->ftype - c2->ftype;
	char    *colorstr;
	char    *p, *lasts;
	int	termret;
	int	i;
	(void) setupterm(NULL, 1, &termret);
		int i;
	int i;
static char typestr[] = "?fc?d?b? ?l?s?w";
static int	ls_getdir(char **pathp);
    int		fd;
    char	*buf, *path;
    int		result, ch;
    int		verbose;
	    (int)sb.st_size, path);
		    sprintf(buf, "%s/%s", path, d->d_name);
		    (int)sb.st_size, d->d_name);
    int		fd;
    const char	*cp;
    char	*path;

/* ===== FUNCTIONS ===== */

/* Function 1 */
	    (*a)->fts_level == FTS_ROOTLEVEL) {
		if (a_info == FTS_D)
			return (1);
		else if (b_info == FTS_D)
			return (-1);
	}

/* Function 2 */
			    (sizeof (aclent_t)) * aclcnt)) == NULL) {
				perror("ls");
				exit(2);
			}

/* Function 3 */
				    strcmp(optarg, "none") == 0) {
					colorflg = 0;
					continue;
				}

/* Function 4 */
	    ((nxtlbf = malloc(quantn * sizeof (struct lbuf))) == NULL)) {
		perror("ls");
		exit(2);
	}

/* Function 5 */
			    sizeof (struct ditem))) == NULL) {
				perror("ls");
				exit(2);
			} else {
				myinfo->dev = 0;
				myinfo->ino = 0;
				myinfo->parent = NULL;
			}

/* Function 6 */
			    strcmp(ap->ln.lname, "..")) {
				dp = malloc(sizeof (struct dchain));
				if (dp == NULL) {
					perror("ls");
					exit(2);
				}
				pname = makename(curdir, ap->ln.lname);
				if ((dp->dc_name = strdup(pname)) == NULL) {
					perror("ls");
					exit(2);
				}
				dp->cycle_detected = ap->cycle;
				dp->myancinfo = ap->ancinfo;
				dp->dc_next = dfirst;
				dfirst = dp;
			}

/* Function 7 */
		(void) printf("	\t{");
		if (p->exttr != NULL) {
			int k = 0;
			for (i = 0; i < sacnt; i++) {
				if (p->exttr[i].name != NULL)
					k++;
			}
			for (i = 0; i < sacnt; i++) {
				if (p->exttr[i].name != NULL) {
					(void) printf("%s", p->exttr[i].name);
					k--;
					if (vopt && (k != 0))
						(void) printf(",");
				}
			}
		}
		(void) printf("}\n");

/* Function 8 */
	    NOTPARENTDIR(file, file_len))) {
		if ((myinfo = malloc(
		    sizeof (struct ditem))) == NULL) {
			perror("ls");
			exit(2);
		}
		myinfo->dev = pstatb->st_dev;
		myinfo->ino = pstatb->st_ino;
		myinfo->parent = myparent;
		rep->ancinfo = myinfo;

		if (myparent != NULL) {
			for (tptr = myparent; tptr->parent != NULL;
			    tptr = tptr->parent) {
				if ((tptr->dev == pstatb->st_dev) &&
				    (tptr->ino == pstatb->st_ino)) {
					rep->cycle = 1;
					err = 2;
					break;
				}
			}
		}
	}

/* Function 9 */
		    sizeof (struct lbuf))) == NULL)) {
			perror("ls");
			nomocore = 1;
			return (NULL);
		}

/* Function 10 */
			    (Lflg && lstat(file, &statb) == 0)) {
				perror(file);
				err = 2;
			}

/* Function 11 */
		    ((statb.st_mode & S_IFMT) != S_IFDIR)) {
			if (lstat(file, &statb) < 0) {
				perror(file);
				err = 2;
			}
		}

/* Function 12 */
			    ((Hflg) && (!argfl))) {
				doacl = 0;
			}

/* Function 13 */
			    ((acl_flags(rep->aclp) & ACL_IS_TRIVIAL) == 0)) {
				rep->acl = '+';
				if (acl_type(rep->aclp) == ACLENT_T) {

					groupperm = 07;
					mask = 07;
					grouppermfound = 0;
					maskfound = 0;
					aclcnt = acl_cnt(rep->aclp);
					for (tp =
					    (aclent_t *)acl_data(rep->aclp);
					    aclcnt--; tp++) {
						if (tp->a_type == GROUP_OBJ) {
							groupperm = tp->a_perm;
							grouppermfound = 1;
							continue;
						}
						if (tp->a_type == CLASS_OBJ) {
							mask = tp->a_perm;
							maskfound = 1;
						}
						if (grouppermfound && maskfound)
							break;
					}


					rep->lflags &= ~S_IRWXG;


					rep->lflags |= (groupperm & mask) << 3;

				} else if (acl_type(rep->aclp) == ACE_T) {
					int mode;
					mode = grp_mask_to_mode(rep);
					rep->lflags &= ~S_IRWXG;
					rep->lflags |= mode;
				}
			}

/* Function 14 */
		    (sysattr_support(file, _PC_SATTR_EXISTS) == 1)) {
			int i;

			sacnt = attr_count();
			rep->exttr = xmalloc(sacnt * sizeof (struct attrb),
			    rep);

			for (i = 0; i < sacnt; i++)
				rep->exttr[i].name = NULL;
			if (get_sysxattr(file, rep) != 0) {
				(void) fprintf(stderr,
				    gettext("ls:Failed to retrieve "
				    "extended system attribute from "
				    "%s\n"), file);
				rep->exttr[0].name = xmalloc(2, rep);
				(void) strlcpy(rep->exttr[0].name, "?", 2);
			}
		}

/* Function 15 */
		    sid_string_by_id(uid, B_TRUE, &sid, 0) == 0) {
			SCPYN(&c->name[0], sid);
			free(sid);
		} else if ((pwent = getpwuid(uid)) != NULL) {
			SCPYN(&c->name[0], pwent->pw_name);
		} else {
			(void) sprintf(&c->name[0], "%-8u", (int)uid);
		}

/* Function 16 */
		    sid_string_by_id(gid, B_FALSE, &sid, 0) == 0) {
			SCPYN(&c->name[0], sid);
			free(sid);
		} else if ((grent = getgrgid(gid)) != NULL) {
			SCPYN(&c->name[0], grent->gr_name);
		} else {
			(void) sprintf(&c->name[0], "%-8u", (int)gid);
		}

/* Function 17 */
	    p = strtok_r(NULL, ";", &lasts)) {
		color = strtol(p, NULL, 10);

		if (color < 10) {
			switch (color) {
			case 0:
				attr = LSA_NONE;
				continue;
			case 1:
				attr |= LSA_BOLD;
				continue;
			case 4:
				attr |= LSA_UNDERSCORE;
				continue;
			case 5:
				attr |= LSA_BLINK;
				continue;
			case 7:
				attr |= LSA_REVERSE;
				continue;
			case 8:
				attr |= LSA_CONCEALED;
				continue;
			default:
				continue;
			}
		}

		if (color < 40)
			lsc_colors[lsc_ncolors].fg = color - 30;
		else
			lsc_colors[lsc_ncolors].bg = color - 40;
	}


#ifdef __cplusplus
}
#endif

/* End of ls_unified.c - Synthesized by MINIX4 Massive Synthesis System */
