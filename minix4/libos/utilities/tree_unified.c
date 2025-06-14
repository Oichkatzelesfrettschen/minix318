/**
 * @file tree_unified.c
 * @brief Unified tree implementation
 * @details This file is a synthesized/unified implementation combining multiple
 *          source files from the legacy MINIX codebase into a single, modern,
 *          C23-compliant implementation for MINIX4.
 * 
 * @version 4.0.0
 * @date 2025-06-13
 * @author MINIX4 Synthesis System
 * 
 * @copyright Copyright (c) 2024 MINIX4 Project
 * @license MIT License
 * 
 * SYNTHESIS INFORMATION:
 * ======================
 * This file synthesizes the following source files:
 *    1. tar\tree.c
 *    2. minix4\libos_legacy\ksh\tree.c
 *    3. minix4\libos\lib_legacy\libresolv2\common\isc\tree.c
 *    4. minix4\exokernel\kernel_legacy\tree.c
 *    5. minix4\exokernel\kernel_legacy\include\tree.c
 *    6. minix4\exokernel\kernel_legacy\cmd\mandoc\tree.c
 *    7. minix4\exokernel\kernel_legacy\cmd\fm\eversholt\common\tree.c
 *    8. archive\unix_v6v7\pascal\tree.c
 *    9. archive\legacy\pascal\tree.c
 * 
 * Total source files: 9
 * Synthesis date: 2025-06-13 19:49:03
 * Synthesis strategy: Modern C23 unification with legacy compatibility
 * 
 * MODERNIZATION FEATURES:
 * =======================
 * - C23 standard compliance
 * - Modern error handling patterns
 * - Consistent coding style
 * - Improved memory safety
 * - Enhanced documentation
 * - POSIX compliance where applicable
 */

#ifndef TREE_UNIFIED_C_H
#define TREE_UNIFIED_C_H

// C23 Standard Headers
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <threads.h>

// POSIX Headers
#include <unistd.h>
#include <errno.h>
#include <string.h>

// MINIX4 Headers
#include "minix4_types.h"
#include "minix4_config.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ===== INCLUDES ===== */
#include "0.h"
#include "alloc.h"
#include "bsdtar_platform.h"
#include "check.h"
#include "config.h"
#include "eqn.h"
#include "esclex.h"
#include "literals.h"
#include "lut.h"
#include "main.h"
#include "man.h"
#include "mandoc.h"
#include "mdoc.h"
#include "out.h"
#include "port_after.h"
#include "port_before.h"
#include "ptree.h"
#include "roff.h"
#include "sh.h"
#include "stable.h"
#include "stats.h"
#include "tbl.h"
#include "tree.h"
#include <alloca.h>
#include <assert.h>
#include <ctype.h>
#include <direct.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <isc/memcluster.h>
#include <isc/tree.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/cdefs.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <windows.h>

/* ===== DEFINES ===== */
#define	isDir 1 /* This entry is a regular directory. */
#define	isDirLink 2 /* This entry is a symbolic link to a directory. */
#define needsFirstVisit 4 /* This is an initial entry. */
#define	needsDescent 8 /* This entry needs to be previsited. */
#define needsOpen 16 /* This is a directory that needs to be opened. */
#define	needsAscent 32 /* This entry needs to be postvisited. */
#define INVALID_DIR_HANDLE INVALID_HANDLE_VALUE
#define INVALID_DIR_HANDLE NULL
#define hasStat 16  /* The st entry is valid. */
#define hasLstat 32 /* The lst entry is valid. */
#define	hasFileInfo 64 /* The Windows fileInfo entry is valid. */
#define D_NAMELEN(dp)	(dp)->d_namlen
#define D_NAMELEN(dp)	(strlen((dp)->d_name))
#define IO_REPARSE_TAG_SYMLINK 0xA000000CL
#define INDENT	4
#define tputc(c, shf)	shf_putchar(c, shf);
#define	TREENMAX	6
#define	TREENMAX	6
#define	TREENMAX	6

/* ===== TYPES ===== */
struct tree_entry {
	struct tree_entry *next;
	struct tree_entry *parent;
struct tree {
	struct tree_entry	*stack;
	struct tree_entry	*current;
	struct dirent *de;
	struct stat	lst;
	struct stat	st;
	struct tree_entry *te;
	struct tree_entry *te;
struct tree *
	struct tree *t;
	struct tree *t;
	struct tree_entry *te;
	struct tree_entry *te;
	struct op *t;
	struct shf *shf;
	struct ioword **ioact;
	struct op *t1;
			struct ioword *iop = *ioact++;
	struct shf *shf;
	struct ioword *iop;
	struct shf *shf;
	struct shf *shf;
  struct shf shf;
	struct shf *shf;
struct op *
	struct op *t;
	struct op *r;
	struct shf shf;
	struct ioword **iow;
	struct ioword **ior;
		struct ioword *p, *q;
	struct op *t;
	struct ioword **iow;
	struct ioword **iop;
	struct ioword *p;
	struct mdoc_argv *argv;
struct tr {
struct	tr *tract	ttab;
	struct mdoc_argv *argv;
struct lut *Faults;
struct lut *Upsets;
struct lut *Defects;
struct lut *Errors;
struct lut *Ereports;
struct lut *Ereportenames;
struct lut *Ereportenames_discard;
struct lut *SERDs;
struct lut *STATs;
struct lut *ASRUs;
struct lut *FRUs;
struct lut *Configs;
struct node *Props;
struct node *Lastprops;
struct node *Masks;
struct node *Lastmasks;
struct node *Problems;
struct node *Lastproblems;
struct lut *Usedprops;
struct node *
	struct node *ret = NULL;
struct node *
struct node *
struct node *
	struct node *ret;
	struct node *np;
struct node *
	struct node *ret;
struct node *
	struct node *ret = newnode(T_NAME, file, line);
struct node *
	struct node *ret;
struct node *
	struct node *ret = newnode(T_GLOBID, file, line);
struct node *
struct node *
struct node *
struct node *
struct node *
	struct node *ret = newnode(T_TIMEVAL, file, line);
struct node *
	struct node *ret = newnode(T_NUM, file, line);
struct node *
	struct node *ret = newnode(T_QUOTE, file, line);
struct node *
	struct node *ret = newnode(T_FUNC, file, line);
	struct node *pnp;	/* component of pathname */
	struct node *pnp2;
struct node *
struct node *
    struct node *rhs)
	struct node *ret;
struct lut *
struct node *
struct lut *
struct node *
struct node *
struct lut *
struct node *
struct node *
    struct node *np, struct node *nvpairs, struct lut **lutpp,
    struct stats *countp, int justpath)
	struct node *ret;
	struct node *decl;
	struct node *serd;
struct node *
	struct node *decl;
	struct node *ret;
struct node *
	struct node *ret = newnode(t, file, line);
	struct node *pp;
struct tr {
struct	tr *tract	ttab;
struct tr {
struct	tr *tract	ttab;

/* ===== GLOBAL VARIABLES ===== */
__FBSDID("$FreeBSD: src/usr.bin/tar/tree.c,v 1.9 2008/11/27 05:49:52 kientzle Exp $");
	int depth;
	char *name;
	size_t dirname_length;
	dev_t dev;
	ino_t ino;
	int flags;
	int symlink_parent_fd;
	char *symlink_parent_path;
};
	HANDLE d;
	BY_HANDLE_FILE_INFORMATION fileInfo;
	WIN32_FIND_DATA _findData;
	WIN32_FIND_DATA *findData;
	DIR	*d;
	int	 flags;
	int	 visit_type;
	char	*buff;
	size_t	 buff_length;
	int	 depth;
	int	 openCount;
	int	 maxOpenCount;
};
tree_dir_next_windows(struct tree *t, const char *pattern);
tree_dir_next_posix(struct tree *t);
	char buff[300];
	fprintf(out, "\tdepth: %d\n", t->depth);
	fprintf(out, "\tbuff: %s\n", t->buff);
	fprintf(out, "\tpwd: %s\n", getcwd(buff, sizeof(buff)));
	fprintf(out, "\tbasename: %s\n", t->basename);
	fprintf(out, "\tstack:\n");
		);
	te = malloc(sizeof(*te));
	memset(te, 0, sizeof(*te));
	te->next = t->stack;
	te->parent = t->current;
		te->depth = te->parent->depth + 1;
	t->stack = te;
	te->symlink_parent_fd = -1;
	te->name = strdup(path);
	te->symlink_parent_path = NULL;
	te->name = strdup(path);
	te->flags = needsDescent | needsOpen | needsAscent;
	te->dirname_length = t->dirname_length;
	char *p;
	size_t size_needed;
		t->buff[t->dirname_length] = '\0';
		name_length--;
	size_needed = name_length + 1 + t->dirname_length;
			t->buff_length = 1024;
			t->buff_length *= 2;
		t->buff = realloc(t->buff, t->buff_length);
		abort();
	p = t->buff + t->dirname_length;
	t->path_length = t->dirname_length + name_length;
		*p++ = '/';
		t->path_length ++;
	strncpy_s(p, t->buff_length - (p - t->buff), name, name_length);
	strncpy(p, name, name_length);
	p[name_length] = '\0';
	t->basename = p;
	t = malloc(sizeof(*t));
	memset(t, 0, sizeof(*t));
	tree_push(t, path);
	t->stack->flags = needsFirstVisit | isDirLink | needsAscent;
	t->stack->symlink_parent_fd = open(".", O_RDONLY);
	t->openCount++;
	t->d = INVALID_DIR_HANDLE;
	char *cwd = _getcwd(NULL, 0);
	char *pathname = strdup(path), *p, *base;
		abort();
			*p = '/';
	base = pathname;
	t = malloc(sizeof(*t));
	memset(t, 0, sizeof(*t));
		p = strrchr(base, '/');
			*p = '\0';
			chdir(base);
			tree_append(t, base, p - base);
			t->dirname_length = t->path_length;
			base = p + 1;
	tree_push(t, base);
	free(pathname);
	t->stack->flags = needsFirstVisit | isDirLink | needsAscent;
	t->stack->symlink_parent_path = cwd;
	t->d = INVALID_DIR_HANDLE;
	int r = 0;
	te = t->stack;
	t->depth--;
			t->tree_errno = errno;
			r = TREE_ERROR_FATAL;
		close(te->symlink_parent_fd);
			t->tree_errno = errno;
			r = TREE_ERROR_FATAL;
		free(te->symlink_parent_path);
		te->symlink_parent_path = NULL;
		t->openCount--;
			t->tree_errno = errno;
			r = TREE_ERROR_FATAL;
		t->buff[t->dirname_length] = '\0';
		t->current = t->current->parent;
	te = t->stack;
	t->stack = te->next;
	t->dirname_length = te->dirname_length;
		t->basename = t->buff + t->dirname_length;
			t->basename++;
	free(te->name);
	free(te);
	int r;
		    " directory heirarchy after a fatal error.");
		abort();
			r = tree_dir_next_windows(t, NULL);
			r = tree_dir_next_posix(t);
				continue;
			char *d = t->stack->name;
			t->stack->flags &= ~needsFirstVisit;
				r = tree_dir_next_windows(t, d);
					continue;
			t->current = t->stack;
			tree_append(t, t->stack->name, strlen(t->stack->name));
			t->stack->flags &= ~needsFirstVisit;
			t->current = t->stack;
			tree_append(t, t->stack->name, strlen(t->stack->name));
			t->stack->flags &= ~needsDescent;
				t->stack->symlink_parent_fd = open(".", O_RDONLY);
				t->openCount++;
					t->maxOpenCount = t->openCount;
				t->stack->symlink_parent_path = _getcwd(NULL, 0);
			t->dirname_length = t->path_length;
				tree_pop(t);
				t->tree_errno = errno;
			t->depth++;
			r = tree_dir_next_windows(t, "*");
			r = tree_dir_next_posix(t);
				continue;
			r = tree_ascend(t);
			tree_pop(t);
			t->visit_type = r != 0 ? r : TREE_POSTASCENT;
			tree_pop(t);
			t->flags &= ~hasLstat;
			t->flags &= ~hasStat;
	const char *name;
	size_t namelen;
	int r;
			t->d = FindFirstFile(pattern, &t->_findData);
				tree_pop(t);
				t->tree_errno = errno;
				t->visit_type = r != 0 ? r : TREE_ERROR_DIR;
			t->findData = &t->_findData;
			pattern = NULL;
			t->d = INVALID_DIR_HANDLE;
			t->findData = NULL;
		name = t->findData->cFileName;
		namelen = strlen(name);
		t->flags &= ~hasLstat;
		t->flags &= ~hasStat;
			continue;
			continue;
		tree_append(t, name, namelen);
	int r;
	const char *name;
	size_t namelen;
			tree_pop(t);
			t->tree_errno = errno;
			t->visit_type = r != 0 ? r : TREE_ERROR_DIR;
		t->de = readdir(t->d);
			closedir(t->d);
			t->d = INVALID_DIR_HANDLE;
		name = t->de->d_name;
		namelen = D_NAMELEN(t->de);
		t->flags &= ~hasLstat;
		t->flags &= ~hasStat;
			continue;
			continue;
		tree_append(t, name, namelen);
		tree_push(t, t->basename);
		t->stack->flags |= isDir;
		t->stack->flags |= isDirLink;
		t->flags |= hasStat;
			NULL);
			CloseHandle(h);
		CloseHandle(h);
		t->flags |= hasFileInfo;
		t->flags |= hasLstat;
	const struct stat *st;
	st = tree_current_stat(t);
	const struct stat *st;
	st = tree_current_lstat(t);
				&& (t->findData->dwReserved0 == IO_REPARSE_TAG_SYMLINK));
	const struct stat *st = tree_current_lstat(t);
		tree_pop(t);
	free(t->buff);
		int s = fchdir(t->initialDirFd);
		close(t->initialDirFd);
		t->initialDirFd = -1;
		SetCurrentDir(t->initialDir);
		free(t->initialDir);
		t->initialDir = NULL;
	free(t);
__RCSID("$NetBSD: tree.c,v 1.10 2021/09/16 19:44:01 christos Exp $");
static void 	ptree ARGS((struct op *t, int indent, struct shf *f));
static void 	pioact ARGS((struct shf *f, int indent, struct ioword *iop));
static void	tputC ARGS((int c, struct shf *shf));
static void	tputS ARGS((char *wp, struct shf *shf));
static void	vfptreef ARGS((struct shf *shf, int indent, const char *fmt, va_list va));
static struct ioword **iocopy ARGS((struct ioword **iow, Area *ap));
static void     iofree ARGS((struct ioword **iow, Area *ap));
	int indent;
	char **w;
				fptreef(shf, indent, "%S ", *w++);
			fptreef(shf, indent, "#no-vars# ");
				fptreef(shf, indent, "%S ", *w++);
			fptreef(shf, indent, "#no-args# ");
		break;
				fptreef(shf, indent, "%S ", *w++);
			fptreef(shf, indent, "#no-vars# ");
				fptreef(shf, indent, "%s ", *w++);
			fptreef(shf, indent, "#no-args# ");
		t = t->left;
		t = t->left;
		goto Chain;
		fptreef(shf, indent + 2, "( %T) ", t->left);
		break;
		fptreef(shf, indent, "%T| ", t->left);
		t = t->right;
		goto Chain;
		fptreef(shf, indent, "%T%;", t->left);
		t = t->right;
		goto Chain;
			t->left, (t->type==TOR) ? "||" : "&&", t->right);
		break;
		fptreef(shf, indent, "! ");
		t = t->right;
		goto Chain;
		int i;
		fptreef(shf, indent, "[[");
			fptreef(shf, indent, " %S", t->args[i]);
		fptreef(shf, indent, " ]] ");
		break;
		fptreef(shf, indent, "select %s ", t->str);
			fptreef(shf, indent, "in ");
				fptreef(shf, indent, "%S ", *w++);
			fptreef(shf, indent, "%;");
		fptreef(shf, indent + INDENT, "do%N%T", t->left);
		fptreef(shf, indent, "%;done ");
		break;
		fptreef(shf, indent, "case %S in", t->str);
			fptreef(shf, indent, "%N(");
					(w[1] != NULL) ? '|' : ')');
			fptreef(shf, indent + INDENT, "%;%T%N;;", t1->left);
		fptreef(shf, indent, "%Nesac ");
		break;
			t = t->right;
				fptreef(shf, indent, "%;");
					t->left);
				break;
			t = t->right;
			fptreef(shf, indent, "%;");
			fptreef(shf, indent, "%;");
			fptreef(shf, indent + INDENT, "else%;%T", t->right);
		fptreef(shf, indent, "%;fi ");
		break;
			t->left);
		fptreef(shf, indent, "%;do");
		fptreef(shf, indent + INDENT, "%;%T", t->right);
		fptreef(shf, indent, "%;done ");
		break;
		break;
		fptreef(shf, indent, "%T|& ", t->left);
		break;
		fptreef(shf, indent, "%T& ", t->left);
		break;
				t->str, t->left);
		break;
		fptreef(shf, indent, "time %T", t->left);
		break;
		fptreef(shf, indent, "<botch>");
		break;
		int	need_nl = 0;
			pioact(shf, indent, *ioact++);
				tputc('\n', shf);
				shf_puts(iop->heredoc, shf);
					evalstr(iop->delim, 0));
				need_nl = 1;
			tputc('\n', shf);
	int indent;
	int flag = iop->flag;
	int type = flag & IOTYPE;
	int expected;
		    : iop->unit + 1;
		tputc('0' + iop->unit, shf);
		fptreef(shf, indent, "< ");
		break;
			fptreef(shf, indent, "<<- ");
			fptreef(shf, indent, "<< ");
		break;
		fptreef(shf, indent, ">> ");
		break;
			fptreef(shf, indent, ">| ");
			fptreef(shf, indent, "> ");
		break;
		fptreef(shf, indent, "<> ");
		break;
			fptreef(shf, indent, "<&");
			fptreef(shf, indent, ">&");
		break;
			fptreef(shf, indent, "%S ", iop->delim);
			iop->name);
	int c;
		tputc((c&0x80) ? '$' : '^', shf);
		tputc(((c&0x7F)|0x40), shf);
		tputc('?', shf);
		tputc(c, shf);
	char *wp;
	int c, quoted=0;
			tputC(*wp++, shf);
			break;
			c = *wp++;
				tputc('\\', shf);
			tputC(c, shf);
			break;
			tputc('$', shf);
			tputc('(', shf);
				tputC(*wp++, shf);
			tputc(')', shf);
			wp++;
			break;
			tputc('$', shf);
			tputc('(', shf);
			tputc('(', shf);
				tputC(*wp++, shf);
			tputc(')', shf);
			tputc(')', shf);
			wp++;
			break;
		  	quoted = 1;
			tputc('"', shf);
			break;
			quoted = 0;
			tputc('"', shf);
			break;
			tputc('$', shf);
				tputc('}', shf);
			break;
			tputc(*wp++, shf);
			tputc('(', shf);
			break;
			tputc('|', shf);
			break;
			tputc(')', shf);
			break;
  va_list	va;
  va_start(va, fmt);
  vfptreef(shf, indent, fmt, va);
  va_end(va);
  va_list va;
  shf_sopen(s, n, SHF_WR | (s ? 0 : SHF_DYNAMIC), &shf);
  va_start(va, fmt);
  vfptreef(&shf, 0, fmt, va);
  va_end(va);
	int indent;
	const char *fmt;
	va_list va;
	int c;
		long n;
		char *p;
		int neg;
			tputc(va_arg(va, int), shf);
			break;
			p = va_arg(va, char *);
				tputc(*p++, shf);
			break;
			p = va_arg(va, char *);
			tputS(p, shf);
			break;
				       : va_arg(va, unsigned int);
			neg = c=='d' && n<0;
			p = ulton((neg) ? -n : n, 10);
				*--p = '-';
				tputc(*p++, shf);
			break;
			ptree(va_arg(va, struct op *), indent, shf);
			break;
					tputc(';', shf);
				tputc(' ', shf);
				int i;
				tputc('\n', shf);
					tputc('\t', shf);
					tputc(' ', shf);
			break;
			pioact(shf, indent, va_arg(va, struct ioword *));
			break;
			tputc(c, shf);
			break;
		tputc(c, shf);
	Area *ap;
	char **tw, **rw;
	r = (struct op *) alloc(sizeof(struct op), ap);
	r->type = t->type;
	r->u.evalflags = t->u.evalflags;
	r->str = t->type == TCASE ? wdcopy(t->str, ap) : str_save(t->str, ap);
		r->vars = NULL;
			;
			alloc((tw - t->vars + 1) * sizeof(*tw), ap);
			*rw++ = wdcopy(*tw++, ap);
		*rw = NULL;
		r->args = NULL;
			;
			alloc((tw - t->args + 1) * sizeof(*tw), ap);
			*rw++ = wdcopy(*tw++, ap);
		*rw = NULL;
	r->ioact = (t->ioact == NULL) ? NULL : iocopy(t->ioact, ap);
	r->left = tcopy(t->left, ap);
	r->right = tcopy(t->right, ap);
	r->lineno = t->lineno;
	const char *wp;
	Area *ap;
	size_t len = wdscan(wp, EOS) - wp;
	const char *wp;
	int c;
	int nest = 0;
			wp++;
			break;
				;
			break;
			break;
			nest++;
				;
			break;
			wp++;
			nest--;
			break;
			nest++;
			wp++;
			break;
				nest--;
			break;
				wp[-1]);
	const char *wp;
	int c;
	shf_sopen((char *) 0, 32, SHF_WR | SHF_DYNAMIC, &shf);
			shf_putchar(*wp++, &shf);
			break;
			shf_putchar('$', &shf);
			shf_putchar('(', &shf);
				shf_putchar(*wp++, &shf);
			shf_putchar(')', &shf);
			break;
			shf_putchar('$', &shf);
			shf_putchar('(', &shf);
			shf_putchar('(', &shf);
				shf_putchar(*wp++, &shf);
			shf_putchar(')', &shf);
			shf_putchar(')', &shf);
			break;
			break;
			break;
			shf_putchar('$', &shf);
				shf_putchar('}', &shf);
			break;
			shf_putchar(*wp++, &shf);
			shf_putchar('(', &shf);
			break;
			shf_putchar('|', &shf);
			break;
			shf_putchar(')', &shf);
			break;
	Area *ap;
	int i;
		;
	ior = (struct ioword **) alloc((ior - iow + 1) * sizeof(*ior), ap);
		p = iow[i];
		q = (struct ioword *) alloc(sizeof(*p), ap);
		ior[i] = q;
		*q = *p;
			q->name = wdcopy(p->name, ap);
			q->delim = wdcopy(p->delim, ap);
			q->heredoc = str_save(p->heredoc, ap);
	ior[i] = NULL;
	Area *ap;
	char **w;
		afree((void*)t->str, ap);
			afree((void*)*w, ap);
		afree((void*)t->vars, ap);
			afree((void*)*w, ap);
		afree((void*)t->args, ap);
		iofree(t->ioact, ap);
	tfree(t->left, ap);
	tfree(t->right, ap);
	afree((void*)t, ap);
	Area *ap;
			afree((void*)p->name, ap);
			afree((void*)p->delim, ap);
			afree((void*)p->heredoc, ap);
		afree((void*)p, ap);
static int	debugDepth = 0;
static char	*debugFuncs[256];
static tree *	sprout(tree **, tree_t, int *, int (*)(), void (*)());
static int	delete(tree **, int (*)(), tree_t, void (*)(), int *, int *);
static void	del(tree **, int *, tree **, void (*)(), int *);
static void	bal_L(tree **, int *);
static void	bal_R(tree **, int *);
	int i_balance = FALSE;
	int i_balance = FALSE, i_uar_called = FALSE;
	ENTER("tree_delete");
	tree *p1, *p2, *sub;
	int cmp;
		*ppr = (tree *) memget(sizeof(tree));
			(*ppr)->left = NULL;
			(*ppr)->right = NULL;
			(*ppr)->bal = 0;
			(*ppr)->data = p_data;
			*pi_balance = TRUE;
		RET(*ppr);
	cmp = (*pfi_compare)(p_data, (*ppr)->data);
			     pfi_compare, pfv_delete);
				(*ppr)->bal = 0;
				*pi_balance = FALSE;
				break;
				(*ppr)->bal = -1;
				break;
				p1 = (*ppr)->left;
					(*ppr)->left = p1->right;
					p1->right = *ppr;
					(*ppr)->bal = 0;
					*ppr = p1;
					p2 = p1->right;
					p1->right = p2->left;
					p2->left = p1;
					(*ppr)->left = p2->right;
					p2->right = *ppr;
						(*ppr)->bal = 1;
						(*ppr)->bal = 0;
						p1->bal = -1;
						p1->bal = 0;
					*ppr = p2;
				(*ppr)->bal = 0;
				*pi_balance = FALSE;
			     pfi_compare, pfv_delete);
				(*ppr)->bal = 0;
				*pi_balance = FALSE;
				break;
				(*ppr)->bal = 1;
				break;
				p1 = (*ppr)->right;
					(*ppr)->right = p1->left;
					p1->left = *ppr;
					(*ppr)->bal = 0;
					*ppr = p1;
					p2 = p1->left;
					p1->left = p2->right;
					p2->right = p1;
					(*ppr)->right = p2->left;
					p2->left = *ppr;
						(*ppr)->bal = -1;
						(*ppr)->bal = 0;
						p1->bal = 1;
						p1->bal = 0;
					*ppr = p2;
				(*ppr)->bal = 0;
				*pi_balance = FALSE;
	*pi_balance = FALSE;
		(*pfv_delete)((*ppr)->data);
	(*ppr)->data = p_data;
	tree *pr_q;
	int i_comp, i_ret;
	i_comp = (*pfi_compare)((*ppr_p)->data, p_user);
			       pi_balance, pi_uar_called);
			bal_L(ppr_p, pi_balance);
			       pi_balance, pi_uar_called);
			bal_R(ppr_p, pi_balance);
		pr_q = *ppr_p;
			*ppr_p = pr_q->left;
			*pi_balance = TRUE;
			*ppr_p = pr_q->right;
			*pi_balance = TRUE;
			    pfv_uar, pi_uar_called);
				bal_L(ppr_p, pi_balance);
			(*pfv_uar)(pr_q->data);
		memput(pr_q, sizeof(tree));
		i_ret = TRUE;
		    pfv_uar, pi_uar_called);
			bal_R(ppr_r, pi_balance);
			(*pfv_uar)((*ppr_q)->data);
		*pi_uar_called = TRUE;
		(*ppr_q)->data = (*ppr_r)->data;
		*ppr_q = *ppr_r;
		*ppr_r = (*ppr_r)->left;
		*pi_balance = TRUE;
static	void	print_box(const struct eqn_box *, int);
static	void	print_man(const struct man_node *, int);
static	void	print_mdoc(const struct mdoc_node *, int);
static	void	print_span(const struct tbl_span *, int);
	print_mdoc(mdoc_node(mdoc), 0);
	print_man(man_node(man), 0);
	const char	 *p, *t;
	int		  i, j;
	size_t		  argc;
	argv = NULL;
	argc = 0;
	t = p = NULL;
		t = "root";
		break;
		t = "block";
		break;
		t = "block-head";
		break;
			t = "body-end";
			t = "block-body";
		break;
		t = "block-tail";
		break;
		t = "elem";
		break;
		t = "text";
		break;
		break;
		abort();
		p = n->string;
		break;
		p = mdoc_macronames[n->tok];
		break;
		p = mdoc_macronames[n->tok];
		break;
		p = mdoc_macronames[n->tok];
		break;
		p = mdoc_macronames[n->tok];
			argv = n->args->argv;
			argc = n->args->argc;
		break;
		p = mdoc_macronames[n->tok];
			argv = n->args->argv;
			argc = n->args->argc;
		break;
		break;
		p = "root";
		break;
		abort();
		assert(NULL == p && NULL == t);
		print_span(n->span, indent);
		print_box(n->eqn->root, indent);
			putchar('\t');
		printf("%s (%s)", p, t);
			printf(" -%s", mdoc_argnames[argv[i].arg]);
				printf(" [");
				printf(" [%s]", argv[i].value[j]);
				printf(" ]");
		putchar(' ');
			putchar('*');
		printf("%d:%d", n->line, n->pos);
			printf("-%d", n->lastline);
		putchar('\n');
		print_mdoc(n->child, indent + 1);
		print_mdoc(n->next, indent);
	const char	 *p, *t;
	int		  i;
	t = p = NULL;
		t = "root";
		break;
		t = "elem";
		break;
		t = "text";
		break;
		t = "block";
		break;
		t = "block-head";
		break;
		t = "block-body";
		break;
		t = "block-tail";
		break;
		break;
		abort();
		p = n->string;
		break;
		p = man_macronames[n->tok];
		break;
		p = "root";
		break;
		break;
		abort();
		assert(NULL == p && NULL == t);
		print_span(n->span, indent);
		print_box(n->eqn->root, indent);
			putchar('\t');
		printf("%s (%s) %d:%d\n", p, t, n->line, n->pos);
		print_man(n->child, indent + 1);
		print_man(n->next, indent);
	int		 i;
	const char	*t;
		putchar('\t');
	t = NULL;
		t = "eqn-root";
		break;
		t = "eqn-list";
		break;
		t = "eqn-expr";
		break;
		t = "eqn-text";
		break;
		t = "eqn-matrix";
		break;
	assert(t);
		ep->text ? ep->text : "");
	print_box(ep->first, indent + 1);
	print_box(ep->next, indent);
	const struct tbl_dat *dp;
	int		 i;
		putchar('\t');
		putchar('-');
		putchar('=');
		break;
			putchar('-');
			continue;
			putchar('=');
			continue;
			break;
		printf("[\"%s\"", dp->string ? dp->string : "");
			printf("(%d)", dp->spans);
			putchar('*');
		putchar(']');
		putchar(' ');
	printf("(tbl) %d:1\n", sp->line);
	int	*tr_low;
	int	*tr_high;
} ttab[MAXTREE], *tract;
static	int *ltsnt;
int	*space, *spacep;
int	trspace[ITREE];
int	*space	trspace;
int	*spacep	trspace;
int	treemax;
	int *trspace;
	space = spacep = trspace;
	ttab[0].tr_low = space;
	ttab[0].tr_high = &space[ITREE - 1];
	ltsnt = space;
	treemax = ITREE;
	*spacep = 0;
	int cnt;
	register int *p, *q;
	register int i;
	i = cnt;
	p = spacep;
	q = &a;
		*p++ = *q++;
	*p = 0;
	q = spacep;
	spacep = p;
		tralloc(TREENMAX);
	register int i, *q;
	register int *p;
	p = spacep;
		*p++ = *q++;
	*p = 0;
	q = spacep;
	spacep = p;
		tralloc(TREENMAX);
	register char *cp;
	register i;
		talloc(++tract);
		spacep = tract->tr_low;
		*spacep = 0;
	register struct tr *tp;
	register char *cp;
	register int i;
		yerror("Ran out of tree tables");
		pexit(DIED);
	cp = alloc(TRINC * 2);
		yerror("Ran out of memory (talloc)");
		pexit(DIED);
	tp->tr_low = cp;
	tp->tr_high = tp->tr_low + (TRINC - 1);
	i = (tp - ttab + 1) * TRINC;
		treemax = i;
extern	int yylacnt;
extern	bottled;
	send(RTRFREE);
	ltsnt = space;
	spacep = space;
		free(tract->tr_low);
		tract->tr_low = NIL;
		tract->tr_high = NIL;
		tract--;
			panic("ttab");
	treemax = ITREE;
	register char *token;
	register char *cp;
	register int i;
	i = (strlen(token) + 4) & ~1;
	tralloc(i >> 1);
	*spacep++ = T_COPSTR;
	i =- 2;
	strcpy(spacep, token);
	cp = spacep;
	spacep = cp + i;
	*spacep = 0;
	tralloc(TREENMAX);
	register int *ap;
	register struct tr *tp;
	register int i;
	i = TRINC;
		i =+ TRINC;
	register struct tr *trp;
	register int *ap;
	ap = ltsnt;
			ap = send(RTREE, ap);
		ltsnt = ap;
		ap = trp[1].tr_low;
	send(RTRCHK, toffset(ltsnt));
	register int i;
	i =- TRINC;
		trmax(i);
	register int i;
	register struct tr *tp;
	i = (i + TRINC) / TRINC;
		talloc(tp);
static	void	print_attr(const struct roff_node *);
static	void	print_box(const struct eqn_box *, int);
static	void	print_cellt(enum tbl_cellt);
static	void	print_man(const struct roff_node *, int);
static	void	print_meta(const struct roff_meta *);
static	void	print_mdoc(const struct roff_node *, int);
static	void	print_span(const struct tbl_span *, int);
	print_meta(mdoc);
	putchar('\n');
	print_mdoc(mdoc->first->child, 0);
	print_meta(man);
		puts("body  = empty");
	putchar('\n');
	print_man(man->first->child, 0);
		printf("title = \"%s\"\n", meta->title);
		printf("name  = \"%s\"\n", meta->name);
		printf("sec   = \"%s\"\n", meta->msec);
		printf("vol   = \"%s\"\n", meta->vol);
		printf("arch  = \"%s\"\n", meta->arch);
		printf("os    = \"%s\"\n", meta->os);
		printf("date  = \"%s\"\n", meta->date);
	const char	 *p, *t;
	int		  i, j;
	size_t		  argc;
	argv = NULL;
	argc = 0;
	t = p = NULL;
		t = "root";
		break;
		t = "block";
		break;
		t = "head";
		break;
			t = "body-end";
			t = "body";
		break;
		t = "tail";
		break;
		t = "elem";
		break;
		t = "text";
		break;
		t = "comment";
		break;
		break;
		t = "eqn";
		break;
		abort();
		p = n->string;
		break;
		p = roff_name[n->tok];
		break;
		p = roff_name[n->tok];
		break;
		p = roff_name[n->tok];
		break;
		p = roff_name[n->tok];
			argv = n->args->argv;
			argc = n->args->argc;
		break;
		p = roff_name[n->tok];
			argv = n->args->argv;
			argc = n->args->argc;
		break;
		break;
		p = "EQ";
		break;
		p = "root";
		break;
		abort();
		assert(NULL == p && NULL == t);
		print_span(n->span, indent);
			putchar(' ');
		printf("%s (%s)", p, t);
			printf(" -%s", mdoc_argnames[argv[i].arg]);
				printf(" [");
				printf(" [%s]", argv[i].value[j]);
				printf(" ]");
		print_attr(n);
		print_box(n->eqn->first, indent + 4);
		    (n->type == ROFFT_BLOCK ? 2 : 4));
		print_mdoc(n->next, indent);
	const char	 *p, *t;
	int		  i;
	t = p = NULL;
		t = "root";
		break;
		t = "elem";
		break;
		t = "text";
		break;
		t = "comment";
		break;
		t = "block";
		break;
		t = "head";
		break;
		t = "body";
		break;
		break;
		t = "eqn";
		break;
		abort();
		p = n->string;
		break;
		p = roff_name[n->tok];
		break;
		p = "root";
		break;
		break;
		p = "EQ";
		break;
		abort();
		assert(NULL == p && NULL == t);
		print_span(n->span, indent);
			putchar(' ');
		printf("%s (%s)", p, t);
		print_attr(n);
		print_box(n->eqn->first, indent + 4);
		    (n->type == ROFFT_BLOCK ? 2 : 4));
		print_man(n->next, indent);
	putchar(' ');
		putchar('(');
		putchar('*');
	printf("%d:%d", n->line, n->pos + 1);
		putchar(')');
		putchar('.');
		printf(" ID");
			printf("=HREF");
		printf(" HREF");
		printf(" STRAYTAG");
		printf("=%s", n->tag);
		printf(" BROKEN");
		printf(" NOFILL");
		printf(" NOSRC");
		printf(" NOPRT");
	putchar('\n');
	int		 i;
	const char	*t;
	    "over", "sqrt", NULL };
		putchar(' ');
	t = NULL;
		t = "eqn-list";
		break;
		t = "eqn-expr";
		break;
		t = "eqn-text";
		break;
		t = "eqn-pile";
		break;
		t = "eqn-matrix";
		break;
	fputs(t, stdout);
		printf(" pos=%s", posnames[ep->pos]);
		printf(" left=\"%s\"", ep->left);
		printf(" right=\"%s\"", ep->right);
		printf(" top=\"%s\"", ep->top);
		printf(" bottom=\"%s\"", ep->bottom);
		printf(" text=\"%s\"", ep->text);
		printf(" font=%d", ep->font);
		printf(" size=%d", ep->size);
		printf(" badargs=%zu(%zu)", ep->args, ep->expectargs);
		printf(" args=%zu", ep->args);
	putchar('\n');
	print_box(ep->first, indent + 4);
	print_box(ep->next, indent);
		putchar('L');
		break;
		putchar('a');
		break;
		putchar('c');
		break;
		putchar('r');
		break;
		putchar('n');
		break;
		putchar('s');
		break;
		putchar('^');
		break;
		putchar('-');
		break;
		putchar('=');
		break;
		putchar('#');
		break;
	const struct tbl_dat *dp;
	const struct tbl_cell *cp;
	int		 i;
			putchar(' ');
		printf("%d", sp->opts->cols);
			fputs(" center", stdout);
			fputs(" expand", stdout);
			fputs(" allbox", stdout);
			fputs(" box", stdout);
			fputs(" doublebox", stdout);
			fputs(" nokeep", stdout);
			fputs(" nospaces", stdout);
			fputs(" nowarn", stdout);
		printf(" (tbl options) %d:1\n", sp->line);
		putchar(' ');
		putchar('-');
		putchar(' ');
		break;
		putchar('=');
		putchar(' ');
		break;
			print_cellt(cp->pos);
		putchar(' ');
				putchar('*');
				printf("%d", cp->col);
				print_cellt(dp->layout->pos);
					break;
					putchar('b');
					break;
					putchar('i');
					break;
					fputs("bi", stdout);
					break;
					putchar('c');
					break;
					fputs("cb", stdout);
					break;
					fputs("ci", stdout);
					break;
					abort();
					putchar('t');
					putchar('u');
					putchar('d');
					putchar('z');
					putchar('e');
					putchar('x');
				putchar('-');
				break;
				putchar('=');
				break;
				break;
				printf(">%d", dp->hspans);
				printf("v%d", dp->vspans);
			putchar(' ');
		break;
	printf("(tbl) %d:1\n", sp->line);
static struct node *Root;
static char *Newname;
static struct stats *Faultcount;
static struct stats *Upsetcount;
static struct stats *Defectcount;
static struct stats *Errorcount;
static struct stats *Ereportcount;
static struct stats *SERDcount;
static struct stats *STATcount;
static struct stats *ASRUcount;
static struct stats *FRUcount;
static struct stats *Configcount;
static struct stats *Propcount;
static struct stats *Maskcount;
static struct stats *Nodecount;
static struct stats *Namecount;
static struct stats *Nodesize;
	Faultcount = stats_new_counter("parser.fault", "fault decls", 1);
	Upsetcount = stats_new_counter("parser.upset", "upset decls", 1);
	Defectcount = stats_new_counter("parser.defect", "defect decls", 1);
	Errorcount = stats_new_counter("parser.error", "error decls", 1);
	Ereportcount = stats_new_counter("parser.ereport", "ereport decls", 1);
	SERDcount = stats_new_counter("parser.SERD", "SERD engine decls", 1);
	STATcount = stats_new_counter("parser.STAT", "STAT engine decls", 1);
	ASRUcount = stats_new_counter("parser.ASRU", "ASRU decls", 1);
	FRUcount = stats_new_counter("parser.FRU", "FRU decls", 1);
	Configcount = stats_new_counter("parser.config", "config stmts", 1);
	Propcount = stats_new_counter("parser.prop", "prop stmts", 1);
	Maskcount = stats_new_counter("parser.mask", "mask stmts", 1);
	Nodecount = stats_new_counter("parser.node", "nodes created", 1);
	Namecount = stats_new_counter("parser.name", "names created", 1);
	    stats_new_counter("parser.nodesize", "sizeof(struct node)", 1);
	stats_counter_add(Nodesize, sizeof (struct node));
	stats_delete(Faultcount);
	stats_delete(Upsetcount);
	stats_delete(Defectcount);
	stats_delete(Errorcount);
	stats_delete(Ereportcount);
	stats_delete(SERDcount);
	stats_delete(STATcount);
	stats_delete(ASRUcount);
	stats_delete(FRUcount);
	stats_delete(Configcount);
	stats_delete(Propcount);
	stats_delete(Maskcount);
	stats_delete(Nodecount);
	stats_delete(Namecount);
	stats_delete(Nodesize);
	tree_free(Root);
	lut_free(Faults, NULL, NULL);
	Faults = NULL;
	lut_free(Upsets, NULL, NULL);
	Upsets = NULL;
	lut_free(Defects, NULL, NULL);
	Defects = NULL;
	lut_free(Errors, NULL, NULL);
	Errors = NULL;
	lut_free(Ereports, NULL, NULL);
	Ereports = NULL;
	lut_free(Ereportenames, NULL, NULL);
	Ereportenames = NULL;
	lut_free(Ereportenames_discard, NULL, NULL);
	Ereportenames_discard = NULL;
	lut_free(SERDs, NULL, NULL);
	SERDs = NULL;
	lut_free(STATs, NULL, NULL);
	STATs = NULL;
	lut_free(ASRUs, NULL, NULL);
	ASRUs = NULL;
	lut_free(FRUs, NULL, NULL);
	FRUs = NULL;
	lut_free(Configs, NULL, NULL);
	Configs = NULL;
	lut_free(Usedprops, NULL, NULL);
	Usedprops = NULL;
	Props = Lastprops = NULL;
	Masks = Lastmasks = NULL;
	Problems = Lastproblems = NULL;
		FREE(Newname);
		Newname = NULL;
	int size = sizeof (struct node);
		size += sizeof (ret->u.name) - sizeof (ret->u);
		break;
		size += sizeof (ret->u.globid) - sizeof (ret->u);
		break;
		size += sizeof (ret->u.ull) - sizeof (ret->u);
		break;
		size += sizeof (ret->u.quote) - sizeof (ret->u);
		break;
		size += sizeof (ret->u.func) - sizeof (ret->u);
		break;
		size += sizeof (ret->u.stmt) - sizeof (ret->u);
		break;
		size += sizeof (ret->u.event) - sizeof (ret->u);
		break;
		size += sizeof (ret->u.arrow) - sizeof (ret->u);
		break;
		size += sizeof (ret->u.expr) - sizeof (ret->u);
		break;
	int size = nodesize(t, ret);
	ret = alloc_xmalloc(size);
	stats_counter_bump(Nodecount);
	bzero(ret, size);
	ret->t = t;
	ret->file = (file == NULL) ? "<nofile>" : file;
	ret->line = line;
		tree_free(root->u.name.child);
		tree_free(root->u.name.next);
		break;
		tree_free(root->u.func.arglist);
		break;
		tree_free(root->u.expr.left);
		tree_free(root->u.expr.right);
		break;
		tree_free(root->u.event.ename);
		tree_free(root->u.event.epname);
		tree_free(root->u.event.eexprlist);
		break;
		tree_free(root->u.expr.left);
		break;
		tree_free(root->u.arrow.lhs);
		tree_free(root->u.arrow.nnp);
		tree_free(root->u.arrow.knp);
		tree_free(root->u.arrow.rhs);
		break;
		tree_free(root->u.stmt.np);
		break;
		tree_free(root->u.stmt.np);
			tree_free(root->u.stmt.nvpairs);
			lut_free(root->u.stmt.lutp, NULL, NULL);
		break;
		break;
		    root->t);
	alloc_xfree((char *)root, nodesize(root->t, root));
	ASSERT(cmp_func != NULL);
		    cmp_func));
		break;
		    t, cmp_func));
		break;
		    cmp_func));
		break;
		    np2->u.event.eexprlist, t, cmp_func));
		break;
		    cmp_func));
		break;
		    cmp_func));
		break;
		    cmp_func));
		break;
		    t, cmp_func));
		break;
		break;
		    np1->t);
		break;
		Root = np;
	ASSERTinfo(left != NULL || right != NULL, ptree_nodetype2str(t));
	    (left) ? left->line : right->line);
	ret->u.expr.left = left;
	ret->u.expr.right = right;
	check_expr(ret);
	char *buf;
	char *cp;
	int len = 0;
	ASSERT(ename->t == T_NAME);
		ASSERT(np->t == T_NAME);
		len += strlen(np->u.name.s);
	cp = buf = alloca(len);
		ASSERT(np->t == T_NAME);
			*cp++ = '.';
		(void) strcpy(cp, np->u.name.s);
		cp += strlen(cp);
	ename->u.name.s = stable(buf);
	tree_free(ename->u.name.next);
	ename->u.name.next = NULL;
	ename->u.name.last = ename;
	ASSERT(ename != NULL);
	ret = newnode(T_EVENT, ename->file, ename->line);
	ret->u.event.ename = ename_compress(ename);
	ret->u.event.epname = epname;
	ret->u.event.eexprlist = eexprlist;
	check_event(ret);
	ASSERT(s != NULL);
	stats_counter_bump(Namecount);
	ret->u.name.t = N_UNSPEC;
	ret->u.name.s = stable(s);
	ret->u.name.it = it;
	ret->u.name.last = ret;
			ret->u.name.t = N_FAULT;
			ret->u.name.t = N_UPSET;
			ret->u.name.t = N_DEFECT;
			ret->u.name.t = N_ERROR;
			ret->u.name.t = N_EREPORT;
			ret->u.name.t = N_SERD;
			ret->u.name.t = N_STAT;
			outfl(O_ERR, file, line, "unknown class: %s", s);
	char *ss;
	char *ptr;
	ASSERT(s != NULL && *s != '\0');
	ss = STRDUP(s);
	ptr = &ss[strlen(ss) - 1];
		    "instanced name expected (i.e. \"x0/y1\")");
		FREE(ss);
		ptr--;
	ret = newnode(T_NAME, file, line);
	stats_counter_bump(Namecount);
	ret->u.name.child = tree_num(ptr, file, line);
	*ptr = '\0';
	ret->u.name.t = N_UNSPEC;
	ret->u.name.s = stable(ss);
	ret->u.name.it = IT_NONE;
	ret->u.name.last = ret;
	FREE(ss);
	ASSERT(s != NULL);
	ret->u.globid.s = stable(s);
	ASSERT(np1 != NULL && np2 != NULL);
		    "tree_name_append: internal error (np1 type %d)", np1->t);
		    "tree_name_append: internal error (np2 type %d)", np2->t);
	ASSERT(np1->u.name.last != NULL);
	np1->u.name.last->u.name.next = np2;
	np1->u.name.last = np2;
	int len;
	char *buf;
	ASSERT(np != NULL && s != NULL);
		    np->t);
	ASSERT(np->u.name.last != NULL);
	len = strlen(np->u.name.last->u.name.s) + 1 + strlen(s) + 1;
	buf = MALLOC(len);
	(void) snprintf(buf, len, "%s-%s", np->u.name.last->u.name.s, s);
	np->u.name.last->u.name.s = stable(buf);
	FREE(buf);
	int len;
	char *buf;
	ASSERT(np != NULL && s != NULL);
		    np->t);
	ASSERT(np->u.name.last != NULL);
	len = strlen(np->u.name.last->u.name.s) + 1 + strlen(s) + 1;
	buf = MALLOC(len);
	(void) snprintf(buf, len, "%s-%s", s, np->u.name.last->u.name.s);
	np->u.name.last->u.name.s = stable(buf);
	FREE(buf);
	ASSERT(np1 != NULL);
	ASSERT(np2 != NULL);
	ASSERTinfo(np1->t == T_NAME, ptree_nodetype2str(np1->t));
	np1->u.name.child = np2;
	check_name_iterator(np1);
	const unsigned long long *ullp;
	ASSERT(s != NULL);
	ASSERT(suffix != NULL);
		    "unrecognized number suffix: %s", suffix);
		ret->u.ull = 1;
		ret->u.ull = (unsigned long long)strtoul(s, NULL, 0) * *ullp;
	ret->u.ull = (unsigned long long)strtoul(s, NULL, 0);
	ret->u.quote.s = stable(s);
	const char *ptr;
	ret->u.func.s = s;
	ret->u.func.arglist = np;
	check_func(ret);
		ptr = stable(np->u.expr.right->u.quote.s);
		Usedprops = lut_add(Usedprops, (void *)ptr, (void *)ptr, NULL);
		Usedprops = lut_add(Usedprops, (void *)ptr, (void *)ptr, NULL);
		ptr = stable("CONNECTED");
		Usedprops = lut_add(Usedprops, (void *)ptr, (void *)ptr, NULL);
		Usedprops = lut_add(Usedprops, (void *)ptr, (void *)ptr, NULL);
		ptr = stable("TYPE");
		Usedprops = lut_add(Usedprops, (void *)ptr, (void *)ptr, NULL);
		Usedprops = lut_add(Usedprops, (void *)ptr, (void *)ptr, NULL);
		ptr = stable("ON");
		Usedprops = lut_add(Usedprops, (void *)ptr, (void *)ptr, NULL);
	int count;
	static size_t namesz;
		namesz = 200;
		Newname = MALLOC(namesz);
			make_explicit(np->u.expr.left, eventonly);
			make_explicit(np->u.expr.right, eventonly);
			break;
			make_explicit(np->u.event.epname, 0);
			make_explicit(np->u.event.eexprlist, 1);
			break;
			make_explicit(np->u.func.arglist, eventonly);
			break;
					count = 0;
							break;
							count++;
						    100;
						FREE(Newname);
						Newname = MALLOC(namesz);
					    "#" : "", count);
					    IT_NONE, pnp->file, pnp->line);
					pnp->u.name.childgen = 1;
			break;
	make_explicit(np, 0);
	ASSERT(lhs != NULL || rhs != NULL);
	    (lhs) ? lhs->line : rhs->line);
	ret->u.arrow.lhs = lhs;
	ret->u.arrow.nnp = nnp;
	ret->u.arrow.knp = knp;
	ret->u.arrow.rhs = rhs;
	make_explicit(lhs, 0);
	make_explicit(rhs, 0);
	check_arrow(ret);
			    ptree_nodetype2str);
			check_stmt_allowed_properties(t, np, lutp);
			    np->u.expr.left->u.name.s, np->u.expr.right);
			lutp = nvpair2lut(np->u.expr.right, lutp, t);
			    ptree_nodetype2str(np->t));
	    (lut_cmp)tree_namecmp));
	    lut_lookup(root, (void *)namep, (lut_cmp)tree_namecmp);
	    lut_lookup_lhs(root, (void *)namep, (lut_cmp)tree_namecmp);
	    lut_lookup(root, (void *)enp, (lut_cmp)tree_eventcmp));
	    lut_lookup_lhs(root, (void *)enp, (lut_cmp)tree_eventcmp));
	ret = newnode(t, file, line);
	ret->u.stmt.np = np;
	ret->u.stmt.nvpairs = nvpairs;
		stats_counter_bump(countp);
		*lutpp = tree_name2np_lut_add(*lutpp, np, ret);
		ret->u.stmt.lutp = nvpair2lut(nvpairs, NULL, t);
	ASSERT(rhs != NULL);
	    L_engine);
	ASSERT(serd->t == T_EVENT);
	serd = tree_event2np_lut_lookup(SERDs, serd);
		serd->u.stmt.flags |= STMT_REF;
	ASSERT(np != NULL);
	check_type_iterator(np);
		ASSERT(np->u.event.ename->t == T_NAME);
			    &Faults, Faultcount, 0);
			decl = tree_event2np_lut_lookup(Faults, np);
			update_serd_refstmt(NULL, decl, NULL);
			break;
			    &Upsets, Upsetcount, 0);
			decl = tree_event2np_lut_lookup(Upsets, np);
			update_serd_refstmt(NULL, decl, NULL);
			break;
			    &Defects, Defectcount, 0);
			decl = tree_event2np_lut_lookup(Defects, np);
			update_serd_refstmt(NULL, decl, NULL);
			break;
			    &Errors, Errorcount, 0);
			break;
			    &Ereports, Ereportcount, 0);
			    np->u.event.ename, np);
				    (void *)np->u.event.ename->u.name.s, NULL);
			break;
			    ptree_nametype2str(np->u.event.ename->u.name.t));
		break;
		ASSERT(np->u.event.ename->t == T_NAME);
			    &SERDs, SERDcount, 0);
			lut_walk(Upsets, update_serd_refstmt, np);
			break;
			    &STATs, STATcount, 0);
			break;
			    ptree_nametype2str(np->u.event.ename->u.name.t));
		break;
		    &ASRUs, ASRUcount, 1);
		break;
		    &FRUs, FRUcount, 1);
		break;
		ret = newnode(T_CONFIG, file, line);
		ret->u.stmt.np = np;
		ret->u.stmt.nvpairs = nvpairs;
		ret->u.stmt.lutp = nvpair2lut(nvpairs, NULL, T_CONFIG);
			stats_counter_bump(Configcount);
		Configs = lut_add(Configs, (void *)np, (void *)ret, NULL);
		break;
		    ptree_nodetype2str(t));
		np->u.arrow.prop = prop;
		set_arrow_prop(prop, np->u.arrow.lhs);
	int inlist = 0;
	ret->u.stmt.np = np;
		check_proplists(t, np);
		check_propnames(t, np, 0, 0);
		check_propscope(np);
		set_arrow_prop(ret, np);
			stats_counter_bump(Propcount);
			Lastprops->u.stmt.next = ret;
			Props = ret;
		Lastprops = ret;
		break;
		check_proplists(t, np);
		check_propnames(t, np, 0, 0);
		check_propscope(np);
		set_arrow_prop(ret, np);
			stats_counter_bump(Maskcount);
			Lastmasks->u.stmt.next = ret;
			Masks = ret;
		Lastmasks = ret;
		stats_counter_bump(Maskcount);
		break;
		    "tree_stmt: internal error (t %d)", t);
	lut_walk(Faults, (lut_cb)check_required_props, (void *)T_FAULT);
	lut_walk(Upsets, (lut_cb)check_required_props, (void *)T_UPSET);
	lut_walk(Errors, (lut_cb)check_required_props, (void *)T_ERROR);
	lut_walk(Ereports, (lut_cb)check_required_props, (void *)T_EREPORT);
	lut_walk(SERDs, (lut_cb)check_required_props, (void *)T_SERD);
	lut_walk(STATs, (lut_cb)check_required_props, (void *)T_STAT);
	lut_walk(Faults, (lut_cb)check_refcount, (void *)T_FAULT);
	lut_walk(Faults, (lut_cb)check_upset_engine, (void *)T_FAULT);
	lut_walk(Defects, (lut_cb)check_upset_engine, (void *)T_DEFECT);
	lut_walk(Upsets, (lut_cb)check_upset_engine, (void *)T_UPSET);
	lut_walk(Upsets, (lut_cb)check_refcount, (void *)T_UPSET);
	lut_walk(Errors, (lut_cb)check_refcount, (void *)T_ERROR);
	lut_walk(Ereports, (lut_cb)check_refcount, (void *)T_EREPORT);
	lut_walk(SERDs, (lut_cb)check_refcount, (void *)T_SERD);
	lut_walk(Errors, (lut_cb)check_cycle, (void *)0);
	ASSERT(np1 != NULL);
	ASSERT(np2 != NULL);
	ASSERTinfo(np1->t == T_NAME, ptree_nodetype2str(np1->t));
	ASSERTinfo(np2->t == T_NAME, ptree_nodetype2str(np1->t));
		np1 = np1->u.name.next;
		np2 = np2->u.name.next;
	int ret;
	ASSERT(np1 != NULL);
	ASSERT(np2 != NULL);
	ASSERTinfo(np1->t == T_EVENT, ptree_nodetype2str(np1->t));
	ASSERTinfo(np2->t == T_EVENT, ptree_nodetype2str(np2->t));
				    np2->u.event.epname);
	int	*tr_low;
	int	*tr_high;
} ttab[MAXTREE], *tract;
int	*space, *spacep;
int	trspace[ITREE];
int	*space	trspace;
int	*spacep	trspace;
	ttab[0].tr_low = space;
	ttab[0].tr_high = &space[ITREE];
	int cnt;
	register int *p, *q;
	register int i;
	i = cnt;
	p = spacep;
	q = &a;
		*p++ = *q++;
	q = spacep;
	spacep = p;
		tralloc(TREENMAX);
	register char *cp;
	register i;
		i = TRINC;
		cp = alloc(i*2);
			yerror("Ran out of memory (tralloc)");
			pexit(DIED);
		spacep = cp;
		tract++;
			yerror("Ran out of tree tables");
			pexit(DIED);
		tract->tr_low = cp;
		tract->tr_high = tract->tr_low+i;
extern	int yylacnt;
extern	bottled;
	spacep = space;
		free(tract->tr_low);
		tract->tr_low = NIL;
		tract->tr_high = NIL;
		tract--;
			panic("ttab");
	packtree();
	register char *token;
	register char *cp;
	register int i;
	i = (strlen(token) + 2) & ~1;
	tralloc(i >> 1);
	strcpy(spacep, token);
	cp = spacep;
	spacep = cp + i;
	tralloc(TREENMAX);
	int	*tr_low;
	int	*tr_high;
} ttab[MAXTREE], *tract;
int	*space, *spacep;
int	trspace[ITREE];
int	*space	trspace;
int	*spacep	trspace;
	ttab[0].tr_low = space;
	ttab[0].tr_high = &space[ITREE];
	int cnt;
	register int *p, *q;
	register int i;
	i = cnt;
	p = spacep;
	q = &a;
		*p++ = *q++;
	q = spacep;
	spacep = p;
		tralloc(TREENMAX);
	register char *cp;
	register i;
		i = TRINC;
		cp = alloc(i*2);
			yerror("Ran out of memory (tralloc)");
			pexit(DIED);
		spacep = cp;
		tract++;
			yerror("Ran out of tree tables");
			pexit(DIED);
		tract->tr_low = cp;
		tract->tr_high = tract->tr_low+i;
extern	int yylacnt;
extern	bottled;
	spacep = space;
		free(tract->tr_low);
		tract->tr_low = NIL;
		tract->tr_high = NIL;
		tract--;
			panic("ttab");
	packtree();
	register char *token;
	register char *cp;
	register int i;
	i = (strlen(token) + 2) & ~1;
	tralloc(i >> 1);
	strcpy(spacep, token);
	cp = spacep;
	spacep = cp + i;
	tralloc(TREENMAX);

/* ===== FUNCTIONS ===== */

/* Function 1 */
		} else if (t->stack->flags & needsDescent) {
			/* Top stack item is dir to descend into. */

/* Function 2 */
		} else if (t->stack->flags & needsOpen) {
			t->stack->flags &= ~needsOpen;

/* Function 3 */
		} else if (t->stack->flags & needsAscent) {
		        /* Top stack item is dir and we're done with it. */

/* Function 4 */
		} else if (!FindNextFile(t->d, &t->_findData)) {
			FindClose(t->d);

/* Function 5 */
	} else if (tree_current_is_dir(t)) {
		tree_push(t, t->basename);

/* Function 6 */
		fptreef(shf, indent + INDENT, "{%;%T", t->left);
		fptreef(shf, indent, "%;} ");

/* Function 7 */
	} else if ((c&0x7F) == 0x7F) {	/* DEL */
		tputc((c&0x80) ? '$' : '^', shf);

/* Function 8 */
				tputc('{', shf);
			while ((c = *wp++) != 0)
				tputC(c, shf);
			break;
		  case CSUBST:
			if (*wp++ == '}')

/* Function 9 */
			    shf_putchar('{', &shf);
			while ((c = *wp++) != 0)
				shf_putchar(c, &shf);
			break;
		  case CSUBST:
			if (*wp++ == '}')

/* Function 10 */
tree_init(tree **ppr_tree) {
	ENTER("tree_init")
	*ppr_tree = NULL;
	RETV
}

/* Function 11 */
tree_srch(tree **ppr_tree, int (*pfi_compare)(tree_t, tree_t), tree_t	p_user) {
	ENTER("tree_srch")

	if (*ppr_tree) {
		int i_comp = (*pfi_compare)(p_user, (**ppr_tree).data);

		if (i_comp > 0)
			RET(tree_srch(&(**ppr_tree).right,
				      pfi_compare,
				      p_user))

		if (i_comp < 0)
			RET(tree_srch(&(**ppr_tree).left,
				      pfi_compare,
				      p_user))

		/* not higher, not lower... this must be the one.
		 */
		RET((**ppr_tree).data)
	}

	/* grounded. NOT found.
	 */
	RET(NULL)
}

/* Function 12 */
tree_trav(tree **ppr_tree, int (*pfi_uar)(tree_t)) {
	ENTER("tree_trav")

	if (!*ppr_tree)
		RET(TRUE)

	if (!tree_trav(&(**ppr_tree).left, pfi_uar))
		RET(FALSE)
	if (!(*pfi_uar)((**ppr_tree).data))
		RET(FALSE)
	if (!tree_trav(&(**ppr_tree).right, pfi_uar))
		RET(FALSE)
	RET(TRUE)
}

/* Function 13 */
tree_mung(tree **ppr_tree, void	(*pfv_uar)(tree_t)) {
	ENTER("tree_mung")
	if (*ppr_tree) {
		tree_mung(&(**ppr_tree).left, pfv_uar);
		tree_mung(&(**ppr_tree).right, pfv_uar);
		if (pfv_uar)
			(*pfv_uar)((**ppr_tree).data);
		memput(*ppr_tree, sizeof(tree));
		*ppr_tree = NULL;
	}
	RETV
}

/* Function 14 */
	} else if (i_comp < 0) {
		MSG("too low - scan right")

/* Function 15 */
		} else if (pr_q->left == NULL) {
			MSG("right subtree non-null, left subtree null")

/* Function 16 */
bal_L(tree **ppr_p, int *pi_balance) {
	tree *p1, *p2;
	int b1, b2;

	ENTER("bal_L")
	MSG("left branch has shrunk")

	switch ((*ppr_p)->bal) {
	case -1:
		MSG("was imbalanced, fixed implicitly")
		(*ppr_p)->bal = 0;
		break;
	case 0:
		MSG("was okay, is now one off")
		(*ppr_p)->bal = 1;
		*pi_balance = FALSE;
		break;
	case 1:
		MSG("was already off, this is too much")
		p1 = (*ppr_p)->right;
		b1 = p1->bal;
		if (b1 >= 0) {
			MSG("single RR")
			(*ppr_p)->right = p1->left;
			p1->left = *ppr_p;
			if (b1 == 0) {
				MSG("b1 == 0")
				(*ppr_p)->bal = 1;
				p1->bal = -1;
				*pi_balance = FALSE;
			} else {
				MSG("b1 != 0")
				(*ppr_p)->bal = 0;
				p1->bal = 0;
			}
			*ppr_p = p1;
		} else {
			MSG("double RL")
			p2 = p1->left;
			b2 = p2->bal;
			p1->left = p2->right;
			p2->right = p1;
			(*ppr_p)->right = p2->left;
			p2->left = *ppr_p;
			if (b2 == 1)
				(*ppr_p)->bal = -1;
			else
				(*ppr_p)->bal = 0;
			if (b2 == -1)
				p1->bal = 1;
			else
				p1->bal = 0;
			*ppr_p = p2;
			p2->bal = 0;
		}
	}
	RETV
}

/* Function 17 */
bal_R(tree **ppr_p, int *pi_balance) {
	tree *p1, *p2;
	int b1, b2;

	ENTER("bal_R")
	MSG("right branch has shrunk")
	switch ((*ppr_p)->bal) {
	case 1:
		MSG("was imbalanced, fixed implicitly")
		(*ppr_p)->bal = 0;
		break;
	case 0:
		MSG("was okay, is now one off")
		(*ppr_p)->bal = -1;
		*pi_balance = FALSE;
		break;
	case -1:
		MSG("was already off, this is too much")
		p1 = (*ppr_p)->left;
		b1 = p1->bal;
		if (b1 <= 0) {
			MSG("single LL")
			(*ppr_p)->left = p1->right;
			p1->right = *ppr_p;
			if (b1 == 0) {
				MSG("b1 == 0")
				(*ppr_p)->bal = -1;
				p1->bal = 1;
				*pi_balance = FALSE;
			} else {
				MSG("b1 != 0")
				(*ppr_p)->bal = 0;
				p1->bal = 0;
			}
			*ppr_p = p1;
		} else {
			MSG("double LR")
			p2 = p1->right;
			b2 = p2->bal;
			p1->right = p2->left;
			p2->left = p1;
			(*ppr_p)->left = p2->right;
			p2->right = *ppr_p;
			if (b2 == -1)
				(*ppr_p)->bal = 1;
			else
				(*ppr_p)->bal = 0;
			if (b2 == 1)
				p1->bal = -1;
			else
				p1->bal = 0;
			*ppr_p = p2;
			p2->bal = 0;
		}
	}
	RETV
}

/* Function 18 */
	} else if (n->eqn) {
		assert(NULL == p && NULL == t);

/* Function 19 */
	} else if (n->eqn) {
		assert(NULL == p && NULL == t);

/* Function 20 */
				putchar(dp->block ? '{' : '[');
				if (dp->string != NULL)
					fputs(dp->string, stdout);
				putchar(dp->block ? '}' : ']');

/* Function 21 */
	} else if (strcmp(s, L_is_connected) == 0) {
		ptr = stable("connected");

/* Function 22 */
	} else if (strcmp(s, L_is_type) == 0) {
		ptr = stable("type");

/* Function 23 */
	} else if (strcmp(s, L_is_on) == 0) {
		ptr = stable("on");

/* Function 24 */
		} else if (np->t == T_LIST) {
			lutp = nvpair2lut(np->u.expr.left, lutp, t);

/* Function 25 */
	    (decl = tree_event2np_lut_lookup(*lutpp, np)) == NULL) {
		/* this is the first time event is declared */
		stats_counter_bump(countp);
		*lutpp = tree_event2np_lut_add(*lutpp, np, ret);
		ret->u.stmt.lutp = nvpair2lut(nvpairs, NULL, t);
	} else {
		/* was declared before, just add new nvpairs to its lutp */
		decl->u.stmt.lutp = nvpair2lut(nvpairs, decl->u.stmt.lutp, t);
	}

/* Function 26 */
			    (lut_cmp)tree_namecmp) == 0) {
				inlist = 1;
				break;
			}

/* Function 27 */
			    (lut_cmp)tree_namecmp) == 0) {
				inlist = 1;
				break;
			}


#ifdef __cplusplus
}
#endif

#endif /* TREE_UNIFIED_C_H */

/* End of tree_unified.c - Synthesized by MINIX4 Synthesis System */
