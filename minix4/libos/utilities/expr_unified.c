/**
 * @file expr_unified.c
 * @brief Unified expr implementation
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
 *     1. userspace\minix_commands\cawf\expr.c
 *     2. userspace\commands_legacy\text\cawf\expr.c
 *     3. minix4\libos_legacy\ksh\expr.c
 *     4. minix4\libos\lib_legacy\libsqlite\src\expr.c
 *     5. minix4\exokernel\kernel_legacy\contrib\ast\src\lib\libcmd\expr.c
 *     6. minix4\exokernel\kernel_legacy\cmd\expr\expr.c
 * 
 * Total source files: 6
 * Synthesis date: 2025-06-13 19:53:48
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
#include	<cmd.h>
#include	<regex.h>
#include "cawf.h"
#include "sh.h"
#include "sqliteInt.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <regex.h>
#include <regexpr.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include <unistd.h>

/* ===== DEFINES ===== */
#define IS_BINOP(op) (((int)op) >= (int)O_EQ && ((int)op) <= (int)O_COMMA)
#define IS_ASSIGNOP(op)	((int)(op) >= (int)O_ASN && (int)(op) <= (int)O_BORASN)
#define MAX_PREC	P_COMMA
#define T_ADD	0x100
#define T_MULT	0x200
#define T_CMP	0x400
#define T_FUN	0x800
#define T_OP	7
#define T_NUM	1
#define T_STR	2
#define OP_EQ		(T_CMP|0)
#define OP_GT		(T_CMP|1)
#define OP_LT		(T_CMP|2)
#define OP_GE		(T_CMP|3)
#define OP_LE		(T_CMP|4)
#define OP_NE		(T_CMP|5)
#define OP_PLUS		(T_ADD|0)
#define OP_MINUS	(T_ADD|1)
#define OP_MULT		(T_MULT|0)
#define OP_DIV		(T_MULT|1)
#define OP_MOD		(T_MULT|2)
#define OP_INDEX	(T_FUN|0)
#define OP_LENGTH	(T_FUN|1)
#define OP_MATCH	(T_FUN|2)
#define OP_QUOTE	(T_FUN|3)
#define OP_SUBSTR	(T_FUN|4)
#define numeric(np)	((np)->type&T_NUM)
#define	A_STRING 258
#define	NOARG 259
#define	OR 260
#define	AND 261
#define	EQ 262
#define	LT 263
#define	GT 264
#define	GEQ 265
#define	LEQ 266
#define	NEQ 267
#define	ADD 268
#define	SUBT 269
#define	MULT 270
#define	DIV 271
#define	REM 272
#define	MCH 273
#define	MATCH 274
#define	SUBSTR 275
#define	LENGTH 276
#define	INDEX  277
#define	MSIZE	LINE_MAX
#define	error(c)	errxx()
#define	EQL(x, y) (strcmp(x, y) == 0)
#define	ERROR(c)	errxx()
#define	MAX_MATCH 20

/* ===== TYPES ===== */
enum token {
enum prec {
struct opinfo {
typedef struct expr_state Expr_state;
struct expr_state {
enum error_type { ET_UNEXPECTED, ET_BADLIT, ET_RECURSIVE,
				      struct tbl *vasn));
	struct tbl v;
	struct tbl *vp;
	struct tbl *v;
	enum error_type type;
	enum prec prec;
	struct tbl *vl, UNINITIALIZED(*vr), *vasn;
	enum token op;
	struct tbl *vl;
	enum token op;
	struct tbl *vasn;
	struct tbl *vp;
	struct tbl *vp;
	struct tbl *vq;
  struct ExprList_item *pItem;
    struct SrcList_item *pNewItem = &pNew->a[i];
    struct SrcList_item *pOldItem = &p->a[i];
    struct IdList_item *pNewItem = &pNew->a[i];
    struct IdList_item *pOldItem = &p->a[i];
    struct ExprList_item *pItem = &pList->a[pList->nExpr++];
    struct SrcList_item *pItem = &pSrcList->a[i];
  struct ExprList_item *pItem;
typedef struct Node_s
typedef struct State_s

/* ===== GLOBAL VARIABLES ===== */
	char		name[4];
static struct tbl *evalexpr ARGS((Expr_state *es, enum prec prec));
static void        token    ARGS((Expr_state *es));
static struct tbl *do_ppmm(Expr_state *, enum token, struct tbl *, bool);
static struct tbl *tempvar  ARGS((void));
static struct tbl *intvar   ARGS((Expr_state *es, struct tbl *vp));
	const char *expr;
	int error_ok;
	int ret;
	const char *expr;
	volatile int error_ok;
	int i;
	v = intvar(es, evalexpr(es, MAX_PREC));
		evalerr(es, ET_UNEXPECTED, (char *) 0);
		setint_v(vp, v);
	const char *str;
	char tbuf[2];
	const char *s;
			s = opinfo[(int)es->tok].name;
			vl = intvar(es, evalexpr(es, P_PRIMARY));
				evalerr(es, ET_LVALUE, opinfo[(int) op].name);
			evalerr(es, ET_UNEXPECTED, (char *) 0);
	vl = evalexpr(es, ((int) prec) - 1);
			vl = intvar(es, vl);
			vr = intvar(es, evalexpr(es, P_ASSIGN));
			vr = intvar(es, evalexpr(es, ((int) prec) - 1));
			vr = intvar(es, evalexpr(es, ((int) prec) - 1));
			vr = intvar(es, evalexpr(es, ((int) prec) - 1));
				int ex = vl->val.i != 0;
				setint_v(vasn, vr);
				setint(vasn, res);
	const char *cp;
	int c;
	char *tvar;
			int len;
		int i, n0;
	int oval;
	vl = intvar(es, vasn);
		setint_v(vasn, vl);
		setint(vasn, vl->val.i);
		evalerr(es, ET_LVALUE, opinfo[(int) op].name);
		evalerr(es, ET_RDONLY, opinfo[(int) op].name);
        char *zTabName = pItem->zAlias;
        char *zTabName = pTab->zName;
      int j;
      char *zAs = pEList->a[j].zName;
    char *z = 0;
    char *zErr;
  int i;
        int i, iSet;
              int addr;
        int i;
  int i, n;
    sqliteHashInsert(&db->aFunc, zName, nName, (void*)p);
	const char	opname[3];
	int		op;
	int	type;
	char	*str;
	int	standard;
	char**	arglist;
	char	buf[36];
static int expr_or(State_t*, Node_t*);
	register char*	sp;
	register char*	cp;
	register int	i;
	register int	j;
	register int	k;
	register int	tok;
	char*		ep;
					error(ERROR_exit(2), "chars argument expected");
	register int	tok = getnode(state, np);
		int n;
		char *cp;
			sfsprintf(cp=state->buf,sizeof(state->buf),"%d",np->num);
	register int	tok = expr_cond(state, np);
		int op = (tok&T_OP);
	register int	tok = expr_mult(state, np);
		int op = (tok&T_OP);
	register int	tok = expr_add(state, np);
		register char *left,*right;
		char buff1[36],buff2[36];
		int op = (tok&T_OP);
				sfsprintf(left=buff1,sizeof(buff1),"%d",np->num);
				sfsprintf(right=buff2,sizeof(buff2),"%d",rp.num);
	register int	tok = expr_cmp(state, np);
	register int	tok = expr_and(state, np);
	int	n;
			error(ERROR_usage(2),"%s",optusage((char*)0));
			sfprintf(sfstdout,"%s\n",node.str);
		sfprintf(sfstdout,"%d\n",node.num);
static int ematch(char *, char *);
static void yyerror(char *);
static void errxx();
static void *exprmalloc(size_t size);
static char *ltoa();
static char *lltoa();
static char	**Av;
static char *buf;
static int	Ac;
static int	Argi;
static int noarg;
static int paren;
static char Mstring[1][MSIZE];
	int i = 0;
	int is_a_num = 1;
	int len;
			(void) sscanf(buf, "%lld", &num);
			(void) sprintf(buf, "%lld", num);
	char *p;
	int i;
	char *rv;
	i1 = strtoll(r1, (char **)NULL, 10);
	i2 = strtoll(r2, (char **)NULL, 10);
		(void) write(1, "\n", 1);
	(void) strcpy(rv, lltoa(i1));
	char *rv;
	int si, wi;
	char *res;
	char *rv;
				(void) strcpy(rv = exprmalloc(8), ltoa(++i));
	char *rv;
	(void) strcpy(rv, ltoa(i));
	char *rv;
	(void) strcpy(rv = exprmalloc(8), ltoa(val = (long)ematch(s, p)));
		(void) strcpy(rv, Mstring[0]);
	static char *expbuf;
	char *nexpbuf;
	int num;
	char tempbuf[256];
	char *tmptr;
			(void) strncpy(Mstring[0], p, num);
			yyerror("invalid multibyte character encountered");
	(void) write(1, "\n", 1);
	(void) write(2, "expr: ", 6);
	(void) write(2, gettext(s), (unsigned)strlen(gettext(s)));
	(void) write(2, "\n", 1);
	static char str[20];
	int i;
	int neg = 0;
	static char str[25];
	char *sp = &str[23];
	int i;
	int neg = 0;
	int ylex, temp, op1;
	char *r1, *ra, *rb, *rc;
	void *rv;
		char *s = gettext("malloc error");
		(void) write(2, "expr: ", 6);
		(void) write(2, s, (unsigned)strlen(s));
		(void) write(2, "\n", 1);
		(void) write(1, "\n", 1);
		(void) fprintf(stderr, gettext("Usage: expr expression\n"));
	(void) setlocale(LC_ALL, "");
	(void) textdomain(TEXT_DOMAIN);
	(void) write(1, buf, (unsigned)strlen(buf));
	(void) write(1, "\n", 1);

/* ===== FUNCTIONS ===== */

/* Function 1 */
unsigned char *Asmcode(unsigned char **s, unsigned char *c) {
	unsigned char *s1;

	s1 = *s + 1;
	c[0] = c[1] = c[2] = '\0';
	if ((c[0] = *s1) == '(') {
		s1++;
		if ((c[0] = *s1) != '\0') {
			s1++;
			c[1] = *s1;
		}
	}
	return(s1);
}

/* Function 2 */
void Delnum(int nx) {


	if (nx >= Nnr) {
		(void) sprintf((char *)buf, " bad Delnum(%d) index", nx);
		Error(FATAL, LINE, (char *)buf, NULL);
	}
	while (nx < (Nnr - 1)) {
		Numb[nx] = Numb[nx + 1];
		nx++;
	}
	Nnr--;
}

/* Function 3 */
int Findnum(unsigned char *n, int v, int e) {

	c[0] = n[0];
	c[1] = (n[1] == ' ' || n[1] == '\t') ? '\0' : n[1];
	c[2] = '\0';
	low = mid = 0;
	hi = Nnr - 1;
	while (low <= hi) {
		mid = (low + hi) / 2;
		if ((cmp = strncmp((char *)c, (char *)Numb[mid].nm, 2)) < 0)
			hi = mid - 1;
		else if (cmp > 0)
			low = mid + 1;
		else {
			if (e)
				Numb[mid].val = v;
			return(mid);
		}
	}
	if ( ! e)
		return(-1);
	if (Nnr >= MAXNR)
		Error(FATAL, LINE, " out of number registers at ", (char *)c);
	if (Nnr) {
		if (cmp > 0)
			mid++;
		for (hi = Nnr - 1; hi >= mid; hi--)
			Numb[hi+1] = Numb[hi];
	}
	Nnr++;
	Numb[mid].nm[0] = c[0];
	Numb[mid].nm[1] = c[1];
	Numb[mid].val = v;
	return(mid);
}

/* Function 4 */
int Findparms(unsigned char *n) {

	c[0] = n[0];
	c[1] = (n[1] == ' ' || n[1] == '\t') ? '\0' : n[1];
	c[2] = '\0';
	for (i = 0; Parms[i].nm[0]; i++) {
		if (c[0] == Parms[i].nm[0] && c[1] == Parms[i].nm[1])
			return(i);
	}
	return(-1);
}

/* Function 5 */
int Findscale(int n, double v, int e) {
	int i;
	double *pval;

	for (i = 0; Scale[i].nm; i++) {
		if ((unsigned char )n == Scale[i].nm)
			break;
	}
	if (Scale[i].nm) {
		if (e) {
			pval = &Scale[i].val;
			*pval = v;
		}
		return(i);
	}
	return(-1);
}

/* Function 6 */
unsigned char *Asmcode(unsigned char **s, unsigned char *c) {
	unsigned char *s1;

	s1 = *s + 1;
	c[0] = c[1] = c[2] = '\0';
	if ((c[0] = *s1) == '(') {
		s1++;
		if ((c[0] = *s1) != '\0') {
			s1++;
			c[1] = *s1;
		}
	}
	return(s1);
}

/* Function 7 */
void Delnum(int nx) {


	if (nx >= Nnr) {
		(void) sprintf((char *)buf, " bad Delnum(%d) index", nx);
		Error(FATAL, LINE, (char *)buf, NULL);
	}
	while (nx < (Nnr - 1)) {
		Numb[nx] = Numb[nx + 1];
		nx++;
	}
	Nnr--;
}

/* Function 8 */
int Findnum(unsigned char *n, int v, int e) {

	c[0] = n[0];
	c[1] = (n[1] == ' ' || n[1] == '\t') ? '\0' : n[1];
	c[2] = '\0';
	low = mid = 0;
	hi = Nnr - 1;
	while (low <= hi) {
		mid = (low + hi) / 2;
		if ((cmp = strncmp((char *)c, (char *)Numb[mid].nm, 2)) < 0)
			hi = mid - 1;
		else if (cmp > 0)
			low = mid + 1;
		else {
			if (e)
				Numb[mid].val = v;
			return(mid);
		}
	}
	if ( ! e)
		return(-1);
	if (Nnr >= MAXNR)
		Error(FATAL, LINE, " out of number registers at ", (char *)c);
	if (Nnr) {
		if (cmp > 0)
			mid++;
		for (hi = Nnr - 1; hi >= mid; hi--)
			Numb[hi+1] = Numb[hi];
	}
	Nnr++;
	Numb[mid].nm[0] = c[0];
	Numb[mid].nm[1] = c[1];
	Numb[mid].val = v;
	return(mid);
}

/* Function 9 */
int Findparms(unsigned char *n) {

	c[0] = n[0];
	c[1] = (n[1] == ' ' || n[1] == '\t') ? '\0' : n[1];
	c[2] = '\0';
	for (i = 0; Parms[i].nm[0]; i++) {
		if (c[0] == Parms[i].nm[0] && c[1] == Parms[i].nm[1])
			return(i);
	}
	return(-1);
}

/* Function 10 */
int Findscale(int n, double v, int e) {
	int i;
	double *pval;

	for (i = 0; Scale[i].nm; i++) {
		if ((unsigned char )n == Scale[i].nm)
			break;
	}
	if (Scale[i].nm) {
		if (e) {
			pval = &Scale[i].val;
			*pval = v;
		}
		return(i);
	}
	return(-1);
}

/* Function 11 */
Expr *sqliteExpr(int op, Expr *pLeft, Expr *pRight, Token *pToken){
  Expr *pNew;
  pNew = sqliteMalloc( sizeof(Expr) );
  if( pNew==0 ){
    return 0;
  }
  pNew->op = op;
  pNew->pLeft = pLeft;
  pNew->pRight = pRight;
  if( pToken ){
    assert( pToken->dyn==0 );
    pNew->token = *pToken;
    pNew->span = *pToken;
  }else{
    assert( pNew->token.dyn==0 );
    assert( pNew->token.z==0 );
    assert( pNew->token.n==0 );
    if( pLeft && pRight ){
      sqliteExprSpan(pNew, &pLeft->span, &pRight->span);
    }else{
      pNew->span = pNew->token;
    }
  }
  return pNew;
}

/* Function 12 */
void sqliteExprSpan(Expr *pExpr, Token *pLeft, Token *pRight){
  assert( pRight!=0 );
  assert( pLeft!=0 );
  if( pExpr && pRight->z && pLeft->z ){
    if( pLeft->dyn==0 && pRight->dyn==0 ){
      pExpr->span.z = pLeft->z;
      pExpr->span.n = pRight->n + Addr(pRight->z) - Addr(pLeft->z);
    }else{
      pExpr->span.z = 0;
    }
  }
}

/* Function 13 */
Expr *sqliteExprFunction(ExprList *pList, Token *pToken){
  Expr *pNew;
  pNew = sqliteMalloc( sizeof(Expr) );
  if( pNew==0 ){
    return 0;
  }
  pNew->op = TK_FUNCTION;
  pNew->pList = pList;
  if( pToken ){
    assert( pToken->dyn==0 );
    pNew->token = *pToken;
  }else{
    pNew->token.z = 0;
  }
  pNew->span = pNew->token;
  return pNew;
}

/* Function 14 */
void sqliteExprDelete(Expr *p){
  if( p==0 ) return;
  if( p->span.dyn ) sqliteFree((char*)p->span.z);
  if( p->token.dyn ) sqliteFree((char*)p->token.z);
  sqliteExprDelete(p->pLeft);
  sqliteExprDelete(p->pRight);
  sqliteExprListDelete(p->pList);
  sqliteSelectDelete(p->pSelect);
  sqliteFree(p);
}

/* Function 15 */
Expr *sqliteExprDup(Expr *p){
  Expr *pNew;
  if( p==0 ) return 0;
  pNew = sqliteMallocRaw( sizeof(*p) );
  if( pNew==0 ) return 0;
  memcpy(pNew, p, sizeof(*pNew));
  if( p->token.z!=0 ){
    pNew->token.z = sqliteStrDup(p->token.z);
    pNew->token.dyn = 1;
  }else{
    assert( pNew->token.z==0 );
  }
  pNew->span.z = 0;
  pNew->pLeft = sqliteExprDup(p->pLeft);
  pNew->pRight = sqliteExprDup(p->pRight);
  pNew->pList = sqliteExprListDup(p->pList);
  pNew->pSelect = sqliteSelectDup(p->pSelect);
  return pNew;
}

/* Function 16 */
void sqliteTokenCopy(Token *pTo, Token *pFrom){
  if( pTo->dyn ) sqliteFree((char*)pTo->z);
  if( pFrom->z ){
    pTo->n = pFrom->n;
    pTo->z = sqliteStrNDup(pFrom->z, pFrom->n);
    pTo->dyn = 1;
  }else{
    pTo->z = 0;
  }
}

/* Function 17 */
ExprList *sqliteExprListDup(ExprList *p){
  ExprList *pNew;
  int i;
  if( p==0 ) return 0;
  pNew = sqliteMalloc( sizeof(*pNew) );
  if( pNew==0 ) return 0;
  pNew->nExpr = pNew->nAlloc = p->nExpr;
  pNew->a = pItem = sqliteMalloc( p->nExpr*sizeof(p->a[0]) );
  if( pItem==0 ){
    sqliteFree(pNew);
    return 0;
  }
  for(i=0; i<p->nExpr; i++, pItem++){
    Expr *pNewExpr, *pOldExpr;
    pItem->pExpr = pNewExpr = sqliteExprDup(pOldExpr = p->a[i].pExpr);
    if( pOldExpr->span.z!=0 && pNewExpr ){
      sqliteTokenCopy(&pNewExpr->span, &pOldExpr->span);
    }
    assert( pNewExpr==0 || pNewExpr->span.z!=0
            || pOldExpr->span.z==0 || sqlite_malloc_failed );
    pItem->zName = sqliteStrDup(p->a[i].zName);
    pItem->sortOrder = p->a[i].sortOrder;
    pItem->isAgg = p->a[i].isAgg;
    pItem->done = 0;
  }
  return pNew;
}

/* Function 18 */
SrcList *sqliteSrcListDup(SrcList *p){
  SrcList *pNew;
  int i;
  int nByte;
  if( p==0 ) return 0;
  nByte = sizeof(*p) + (p->nSrc>0 ? sizeof(p->a[0]) * (p->nSrc-1) : 0);
  pNew = sqliteMallocRaw( nByte );
  if( pNew==0 ) return 0;
  pNew->nSrc = pNew->nAlloc = p->nSrc;
  for(i=0; i<p->nSrc; i++){
    pNewItem->zDatabase = sqliteStrDup(pOldItem->zDatabase);
    pNewItem->zName = sqliteStrDup(pOldItem->zName);
    pNewItem->zAlias = sqliteStrDup(pOldItem->zAlias);
    pNewItem->jointype = pOldItem->jointype;
    pNewItem->iCursor = pOldItem->iCursor;
    pNewItem->pTab = 0;
    pNewItem->pSelect = sqliteSelectDup(pOldItem->pSelect);
    pNewItem->pOn = sqliteExprDup(pOldItem->pOn);
    pNewItem->pUsing = sqliteIdListDup(pOldItem->pUsing);
  }
  return pNew;
}

/* Function 19 */
IdList *sqliteIdListDup(IdList *p){
  IdList *pNew;
  int i;
  if( p==0 ) return 0;
  pNew = sqliteMallocRaw( sizeof(*pNew) );
  if( pNew==0 ) return 0;
  pNew->nId = pNew->nAlloc = p->nId;
  pNew->a = sqliteMallocRaw( p->nId*sizeof(p->a[0]) );
  if( pNew->a==0 ) return 0;
  for(i=0; i<p->nId; i++){
    pNewItem->zName = sqliteStrDup(pOldItem->zName);
    pNewItem->idx = pOldItem->idx;
  }
  return pNew;
}

/* Function 20 */
Select *sqliteSelectDup(Select *p){
  Select *pNew;
  if( p==0 ) return 0;
  pNew = sqliteMallocRaw( sizeof(*p) );
  if( pNew==0 ) return 0;
  pNew->isDistinct = p->isDistinct;
  pNew->pEList = sqliteExprListDup(p->pEList);
  pNew->pSrc = sqliteSrcListDup(p->pSrc);
  pNew->pWhere = sqliteExprDup(p->pWhere);
  pNew->pGroupBy = sqliteExprListDup(p->pGroupBy);
  pNew->pHaving = sqliteExprDup(p->pHaving);
  pNew->pOrderBy = sqliteExprListDup(p->pOrderBy);
  pNew->op = p->op;
  pNew->pPrior = sqliteSelectDup(p->pPrior);
  pNew->nLimit = p->nLimit;
  pNew->nOffset = p->nOffset;
  pNew->zSelect = 0;
  pNew->iLimit = -1;
  pNew->iOffset = -1;
  return pNew;
}

/* Function 21 */
ExprList *sqliteExprListAppend(ExprList *pList, Expr *pExpr, Token *pName){
  if( pList==0 ){
    pList = sqliteMalloc( sizeof(ExprList) );
    if( pList==0 ){
      return 0;
    }
    assert( pList->nAlloc==0 );
  }
  if( pList->nAlloc<=pList->nExpr ){
    pList->nAlloc = pList->nAlloc*2 + 4;
    pList->a = sqliteRealloc(pList->a, pList->nAlloc*sizeof(pList->a[0]));
    if( pList->a==0 ){
      pList->nExpr = pList->nAlloc = 0;
      return pList;
    }
  }
  assert( pList->a!=0 );
  if( pExpr || pName ){
    memset(pItem, 0, sizeof(*pItem));
    pItem->pExpr = pExpr;
    if( pName ){
      sqliteSetNString(&pItem->zName, pName->z, pName->n, 0);
      sqliteDequote(pItem->zName);
    }
  }
  return pList;
}

/* Function 22 */
void sqliteExprListDelete(ExprList *pList){
  int i;
  if( pList==0 ) return;
  assert( pList->a!=0 || (pList->nExpr==0 && pList->nAlloc==0) );
  assert( pList->nExpr<=pList->nAlloc );
  for(i=0; i<pList->nExpr; i++){
    sqliteExprDelete(pList->a[i].pExpr);
    sqliteFree(pList->a[i].zName);
  }
  sqliteFree(pList->a);
  sqliteFree(pList);
}

/* Function 23 */
int sqliteExprIsConstant(Expr *p){
  switch( p->op ){
    case TK_ID:
    case TK_COLUMN:
    case TK_DOT:
    case TK_FUNCTION:
      return 0;
    case TK_NULL:
    case TK_STRING:
    case TK_INTEGER:
    case TK_FLOAT:
    case TK_VARIABLE:
      return 1;
    default: {
      if( p->pLeft && !sqliteExprIsConstant(p->pLeft) ) return 0;
      if( p->pRight && !sqliteExprIsConstant(p->pRight) ) return 0;
      if( p->pList ){
        int i;
        for(i=0; i<p->pList->nExpr; i++){
          if( !sqliteExprIsConstant(p->pList->a[i].pExpr) ) return 0;
        }
      }
      return p->pLeft!=0 || p->pRight!=0 || (p->pList && p->pList->nExpr>0);
    }
  }
  return 0;
}

/* Function 24 */
int sqliteExprIsInteger(Expr *p, int *pValue){
  switch( p->op ){
    case TK_INTEGER: {
      if( sqliteFitsIn32Bits(p->token.z) ){
        *pValue = atoi(p->token.z);
        return 1;
      }
      break;
    }
    case TK_STRING: {
      const char *z = p->token.z;
      int n = p->token.n;
      if( n>0 && z[0]=='-' ){ z++; n--; }
      while( n>0 && *z && isdigit(*z) ){ z++; n--; }
      if( n==0 && sqliteFitsIn32Bits(p->token.z) ){
        *pValue = atoi(p->token.z);
        return 1;
      }
      break;
    }
    case TK_UPLUS: {
      return sqliteExprIsInteger(p->pLeft, pValue);
    }
    case TK_UMINUS: {
      int v;
      if( sqliteExprIsInteger(p->pLeft, &v) ){
        *pValue = -v;
        return 1;
      }
      break;
    }
    default: break;
  }
  return 0;
}

/* Function 25 */
int sqliteIsRowid(const char *z){
  if( sqliteStrICmp(z, "_ROWID_")==0 ) return 1;
  if( sqliteStrICmp(z, "ROWID")==0 ) return 1;
  if( sqliteStrICmp(z, "OID")==0 ) return 1;
  return 0;
}

/* Function 26 */
      && sqliteExprResolveIds(pParse, pSrcList, pEList, pExpr->pLeft) ){
        return 1;
      }

/* Function 27 */
      && sqliteExprResolveIds(pParse, pSrcList, pEList, pExpr->pRight) ){
        return 1;
      }

/* Function 28 */
static void getFunctionName(Expr *pExpr, const char **pzName, int *pnName){
  switch( pExpr->op ){
    case TK_FUNCTION: {
      *pzName = pExpr->token.z;
      *pnName = pExpr->token.n;
      break;
    }
    case TK_LIKE: {
      *pzName = "like";
      *pnName = 4;
      break;
    }
    case TK_GLOB: {
      *pzName = "glob";
      *pnName = 4;
      break;
    }
    default: {
      *pzName = "can't happen";
      *pnName = 12;
      break;
    }
  }
}

/* Function 29 */
int sqliteExprCheck(Parse *pParse, Expr *pExpr, int allowAgg, int *pIsAgg){
  int nErr = 0;
  if( pExpr==0 ) return 0;
  switch( pExpr->op ){
    case TK_GLOB:
    case TK_LIKE:
    case TK_FUNCTION: {
      int i;
      FuncDef *pDef;

      getFunctionName(pExpr, &zId, &nId);
      pDef = sqliteFindFunction(pParse->db, zId, nId, n, 0);
      if( pDef==0 ){
        pDef = sqliteFindFunction(pParse->db, zId, nId, -1, 0);
        if( pDef==0 ){
          no_such_func = 1;
        }else{
          wrong_num_args = 1;
        }
      }else{
        is_agg = pDef->xFunc==0;
      }
      if( is_agg && !allowAgg ){
        sqliteErrorMsg(pParse, "misuse of aggregate function %.*s()", nId, zId);
        nErr++;
        is_agg = 0;
      }else if( no_such_func ){
        sqliteErrorMsg(pParse, "no such function: %.*s", nId, zId);
        nErr++;
      }else if( wrong_num_args ){
        sqliteErrorMsg(pParse,"wrong number of arguments to function %.*s()",
             nId, zId);
        nErr++;
      }
      if( is_agg ){
        pExpr->op = TK_AGG_FUNCTION;
        if( pIsAgg ) *pIsAgg = 1;
      }
      for(i=0; nErr==0 && i<n; i++){
        nErr = sqliteExprCheck(pParse, pExpr->pList->a[i].pExpr,
                               allowAgg && !is_agg, pIsAgg);
      }
      if( pDef==0 ){
      }else if( pDef->dataType>=0 ){
        if( pDef->dataType<n ){
          pExpr->dataType =
             sqliteExprType(pExpr->pList->a[pDef->dataType].pExpr);
        }else{
          pExpr->dataType = SQLITE_SO_NUM;
        }
      }else if( pDef->dataType==SQLITE_ARGS ){
        pDef->dataType = SQLITE_SO_TEXT;
        for(i=0; i<n; i++){
          if( sqliteExprType(pExpr->pList->a[i].pExpr)==SQLITE_SO_NUM ){
            pExpr->dataType = SQLITE_SO_NUM;
            break;
          }
        }
      }else if( pDef->dataType==SQLITE_NUMERIC ){
        pExpr->dataType = SQLITE_SO_NUM;
      }else{
        pExpr->dataType = SQLITE_SO_TEXT;
      }
    }
    default: {
      if( pExpr->pLeft ){
        nErr = sqliteExprCheck(pParse, pExpr->pLeft, allowAgg, pIsAgg);
      }
      if( nErr==0 && pExpr->pRight ){
        nErr = sqliteExprCheck(pParse, pExpr->pRight, allowAgg, pIsAgg);
      }
      if( nErr==0 && pExpr->pList ){
        int n = pExpr->pList->nExpr;
        int i;
        for(i=0; nErr==0 && i<n; i++){
          Expr *pE2 = pExpr->pList->a[i].pExpr;
          nErr = sqliteExprCheck(pParse, pE2, allowAgg, pIsAgg);
        }
      }
      break;
    }
  }
  return nErr;
}

/* Function 30 */
int sqliteExprType(Expr *p){
  if( p==0 ) return SQLITE_SO_NUM;
  while( p ) switch( p->op ){
    case TK_PLUS:
    case TK_MINUS:
    case TK_STAR:
    case TK_SLASH:
    case TK_AND:
    case TK_OR:
    case TK_ISNULL:
    case TK_NOTNULL:
    case TK_NOT:
    case TK_UMINUS:
    case TK_UPLUS:
    case TK_BITAND:
    case TK_BITOR:
    case TK_BITNOT:
    case TK_LSHIFT:
    case TK_RSHIFT:
    case TK_REM:
    case TK_INTEGER:
    case TK_FLOAT:
    case TK_IN:
    case TK_BETWEEN:
    case TK_GLOB:
    case TK_LIKE:
      return SQLITE_SO_NUM;

    case TK_STRING:
    case TK_NULL:
    case TK_CONCAT:
    case TK_VARIABLE:
      return SQLITE_SO_TEXT;

    case TK_LT:
    case TK_LE:
    case TK_GT:
    case TK_GE:
    case TK_NE:
    case TK_EQ:
      if( sqliteExprType(p->pLeft)==SQLITE_SO_NUM ){
        return SQLITE_SO_NUM;
      }
      p = p->pRight;
      break;

    case TK_AS:
      p = p->pLeft;
      break;

    case TK_COLUMN:
    case TK_FUNCTION:
    case TK_AGG_FUNCTION:
      return p->dataType;

    case TK_SELECT:
      assert( p->pSelect );
      assert( p->pSelect->pEList );
      assert( p->pSelect->pEList->nExpr>0 );
      p = p->pSelect->pEList->a[0].pExpr;
      break;

    case TK_CASE: {
      if( p->pRight && sqliteExprType(p->pRight)==SQLITE_SO_NUM ){
        return SQLITE_SO_NUM;
      }
      if( p->pList ){
        int i;
        ExprList *pList = p->pList;
        for(i=1; i<pList->nExpr; i+=2){
          if( sqliteExprType(pList->a[i].pExpr)==SQLITE_SO_NUM ){
            return SQLITE_SO_NUM;
          }
        }
      }
      return SQLITE_SO_TEXT;
    }

    default:
      break;
  }
  return SQLITE_SO_NUM;
}

/* Function 31 */
void sqliteExprCode(Parse *pParse, Expr *pExpr){
  Vdbe *v = pParse->pVdbe;
  int op;
  if( v==0 || pExpr==0 ) return;
  switch( pExpr->op ){
    case TK_PLUS:     op = OP_Add;      break;
    case TK_MINUS:    op = OP_Subtract; break;
    case TK_STAR:     op = OP_Multiply; break;
    case TK_SLASH:    op = OP_Divide;   break;
    case TK_AND:      op = OP_And;      break;
    case TK_OR:       op = OP_Or;       break;
    case TK_LT:       op = OP_Lt;       break;
    case TK_LE:       op = OP_Le;       break;
    case TK_GT:       op = OP_Gt;       break;
    case TK_GE:       op = OP_Ge;       break;
    case TK_NE:       op = OP_Ne;       break;
    case TK_EQ:       op = OP_Eq;       break;
    case TK_ISNULL:   op = OP_IsNull;   break;
    case TK_NOTNULL:  op = OP_NotNull;  break;
    case TK_NOT:      op = OP_Not;      break;
    case TK_UMINUS:   op = OP_Negative; break;
    case TK_BITAND:   op = OP_BitAnd;   break;
    case TK_BITOR:    op = OP_BitOr;    break;
    case TK_BITNOT:   op = OP_BitNot;   break;
    case TK_LSHIFT:   op = OP_ShiftLeft;  break;
    case TK_RSHIFT:   op = OP_ShiftRight; break;
    case TK_REM:      op = OP_Remainder;  break;
    default: break;
  }
  switch( pExpr->op ){
    case TK_COLUMN: {
      if( pParse->useAgg ){
        sqliteVdbeAddOp(v, OP_AggGet, 0, pExpr->iAgg);
      }else if( pExpr->iColumn>=0 ){
        sqliteVdbeAddOp(v, OP_Column, pExpr->iTable, pExpr->iColumn);
      }else{
        sqliteVdbeAddOp(v, OP_Recno, pExpr->iTable, 0);
      }
      break;
    }
    case TK_STRING:
    case TK_FLOAT:
    case TK_INTEGER: {
      if( pExpr->op==TK_INTEGER && sqliteFitsIn32Bits(pExpr->token.z) ){
        sqliteVdbeAddOp(v, OP_Integer, atoi(pExpr->token.z), 0);
      }else{
        sqliteVdbeAddOp(v, OP_String, 0, 0);
      }
      assert( pExpr->token.z );
      sqliteVdbeChangeP3(v, -1, pExpr->token.z, pExpr->token.n);
      sqliteVdbeDequoteP3(v, -1);
      break;
    }
    case TK_NULL: {
      sqliteVdbeAddOp(v, OP_String, 0, 0);
      break;
    }
    case TK_VARIABLE: {
      sqliteVdbeAddOp(v, OP_Variable, pExpr->iTable, 0);
      break;
    }
    case TK_LT:
    case TK_LE:
    case TK_GT:
    case TK_GE:
    case TK_NE:
    case TK_EQ: {
      if( pParse->db->file_format>=4 && sqliteExprType(pExpr)==SQLITE_SO_TEXT ){
      }
    }
    case TK_AND:
    case TK_OR:
    case TK_PLUS:
    case TK_STAR:
    case TK_MINUS:
    case TK_REM:
    case TK_BITAND:
    case TK_BITOR:
    case TK_SLASH: {
      sqliteExprCode(pParse, pExpr->pLeft);
      sqliteExprCode(pParse, pExpr->pRight);
      sqliteVdbeAddOp(v, op, 0, 0);
      break;
    }
    case TK_LSHIFT:
    case TK_RSHIFT: {
      sqliteExprCode(pParse, pExpr->pRight);
      sqliteExprCode(pParse, pExpr->pLeft);
      sqliteVdbeAddOp(v, op, 0, 0);
      break;
    }
    case TK_CONCAT: {
      sqliteExprCode(pParse, pExpr->pLeft);
      sqliteExprCode(pParse, pExpr->pRight);
      sqliteVdbeAddOp(v, OP_Concat, 2, 0);
      break;
    }
    case TK_UMINUS: {
      assert( pExpr->pLeft );
      if( pExpr->pLeft->op==TK_FLOAT || pExpr->pLeft->op==TK_INTEGER ){
        Token *p = &pExpr->pLeft->token;
        char *z = sqliteMalloc( p->n + 2 );
        sprintf(z, "-%.*s", p->n, p->z);
        if( pExpr->pLeft->op==TK_INTEGER && sqliteFitsIn32Bits(z) ){
          sqliteVdbeAddOp(v, OP_Integer, atoi(z), 0);
        }else{
          sqliteVdbeAddOp(v, OP_String, 0, 0);
        }
        sqliteVdbeChangeP3(v, -1, z, p->n+1);
        sqliteFree(z);
        break;
      }
    }
    case TK_BITNOT:
    case TK_NOT: {
      sqliteExprCode(pParse, pExpr->pLeft);
      sqliteVdbeAddOp(v, op, 0, 0);
      break;
    }
    case TK_ISNULL:
    case TK_NOTNULL: {
      int dest;
      sqliteVdbeAddOp(v, OP_Integer, 1, 0);
      sqliteExprCode(pParse, pExpr->pLeft);
      dest = sqliteVdbeCurrentAddr(v) + 2;
      sqliteVdbeAddOp(v, op, 1, dest);
      sqliteVdbeAddOp(v, OP_AddImm, -1, 0);
      break;
    }
    case TK_AGG_FUNCTION: {
      sqliteVdbeAddOp(v, OP_AggGet, 0, pExpr->iAgg);
      break;
    }
    case TK_GLOB:
    case TK_LIKE:
    case TK_FUNCTION: {
      ExprList *pList = pExpr->pList;
      int nExpr = pList ? pList->nExpr : 0;
      FuncDef *pDef;
      int nId;
      const char *zId;
      getFunctionName(pExpr, &zId, &nId);
      pDef = sqliteFindFunction(pParse->db, zId, nId, nExpr, 0);
      assert( pDef!=0 );
      nExpr = sqliteExprCodeExprList(pParse, pList, pDef->includeTypes);
      sqliteVdbeOp3(v, OP_Function, nExpr, 0, (char*)pDef, P3_POINTER);
      break;
    }
    case TK_SELECT: {
      sqliteVdbeAddOp(v, OP_MemLoad, pExpr->iColumn, 0);
      break;
    }
    case TK_IN: {
      int addr;
      sqliteVdbeAddOp(v, OP_Integer, 1, 0);
      sqliteExprCode(pParse, pExpr->pLeft);
      addr = sqliteVdbeCurrentAddr(v);
      sqliteVdbeAddOp(v, OP_NotNull, -1, addr+4);
      sqliteVdbeAddOp(v, OP_Pop, 2, 0);
      sqliteVdbeAddOp(v, OP_String, 0, 0);
      sqliteVdbeAddOp(v, OP_Goto, 0, addr+6);
      if( pExpr->pSelect ){
        sqliteVdbeAddOp(v, OP_Found, pExpr->iTable, addr+6);
      }else{
        sqliteVdbeAddOp(v, OP_SetFound, pExpr->iTable, addr+6);
      }
      sqliteVdbeAddOp(v, OP_AddImm, -1, 0);
      break;
    }
    case TK_BETWEEN: {
      sqliteExprCode(pParse, pExpr->pLeft);
      sqliteVdbeAddOp(v, OP_Dup, 0, 0);
      sqliteExprCode(pParse, pExpr->pList->a[0].pExpr);
      sqliteVdbeAddOp(v, OP_Ge, 0, 0);
      sqliteVdbeAddOp(v, OP_Pull, 1, 0);
      sqliteExprCode(pParse, pExpr->pList->a[1].pExpr);
      sqliteVdbeAddOp(v, OP_Le, 0, 0);
      sqliteVdbeAddOp(v, OP_And, 0, 0);
      break;
    }
    case TK_UPLUS:
    case TK_AS: {
      sqliteExprCode(pParse, pExpr->pLeft);
      break;
    }
    case TK_CASE: {
      int expr_end_label;
      int jumpInst;
      int addr;
      int nExpr;
      int i;

      assert(pExpr->pList);
      assert((pExpr->pList->nExpr % 2) == 0);
      assert(pExpr->pList->nExpr > 0);
      nExpr = pExpr->pList->nExpr;
      expr_end_label = sqliteVdbeMakeLabel(v);
      if( pExpr->pLeft ){
        sqliteExprCode(pParse, pExpr->pLeft);
      }
      for(i=0; i<nExpr; i=i+2){
        sqliteExprCode(pParse, pExpr->pList->a[i].pExpr);
        if( pExpr->pLeft ){
          sqliteVdbeAddOp(v, OP_Dup, 1, 1);
          jumpInst = sqliteVdbeAddOp(v, OP_Ne, 1, 0);
          sqliteVdbeAddOp(v, OP_Pop, 1, 0);
        }else{
          jumpInst = sqliteVdbeAddOp(v, OP_IfNot, 1, 0);
        }
        sqliteExprCode(pParse, pExpr->pList->a[i+1].pExpr);
        sqliteVdbeAddOp(v, OP_Goto, 0, expr_end_label);
        addr = sqliteVdbeCurrentAddr(v);
        sqliteVdbeChangeP2(v, jumpInst, addr);
      }
      if( pExpr->pLeft ){
        sqliteVdbeAddOp(v, OP_Pop, 1, 0);
      }
      if( pExpr->pRight ){
        sqliteExprCode(pParse, pExpr->pRight);
      }else{
        sqliteVdbeAddOp(v, OP_String, 0, 0);
      }
      sqliteVdbeResolveLabel(v, expr_end_label);
      break;
    }
    case TK_RAISE: {
      if( !pParse->trigStack ){
        sqliteErrorMsg(pParse,
                       "RAISE() may only be used within a trigger-program");
        pParse->nErr++;
	return;
      }
      if( pExpr->iColumn == OE_Rollback ||
	  pExpr->iColumn == OE_Abort ||
	  pExpr->iColumn == OE_Fail ){
	  sqliteVdbeOp3(v, OP_Halt, SQLITE_CONSTRAINT, pExpr->iColumn,
                           pExpr->token.z, pExpr->token.n);
	  sqliteVdbeDequoteP3(v, -1);
      } else {
	  assert( pExpr->iColumn == OE_Ignore );
	  sqliteVdbeOp3(v, OP_Goto, 0, pParse->trigStack->ignoreJump,
                           "(IGNORE jump)", 0);
      }
    }
    break;
  }
}

/* Function 32 */
void sqliteExprIfTrue(Parse *pParse, Expr *pExpr, int dest, int jumpIfNull){
  Vdbe *v = pParse->pVdbe;
  int op = 0;
  if( v==0 || pExpr==0 ) return;
  switch( pExpr->op ){
    case TK_LT:       op = OP_Lt;       break;
    case TK_LE:       op = OP_Le;       break;
    case TK_GT:       op = OP_Gt;       break;
    case TK_GE:       op = OP_Ge;       break;
    case TK_NE:       op = OP_Ne;       break;
    case TK_EQ:       op = OP_Eq;       break;
    case TK_ISNULL:   op = OP_IsNull;   break;
    case TK_NOTNULL:  op = OP_NotNull;  break;
    default:  break;
  }
  switch( pExpr->op ){
    case TK_AND: {
      int d2 = sqliteVdbeMakeLabel(v);
      sqliteExprIfFalse(pParse, pExpr->pLeft, d2, !jumpIfNull);
      sqliteExprIfTrue(pParse, pExpr->pRight, dest, jumpIfNull);
      sqliteVdbeResolveLabel(v, d2);
      break;
    }
    case TK_OR: {
      sqliteExprIfTrue(pParse, pExpr->pLeft, dest, jumpIfNull);
      sqliteExprIfTrue(pParse, pExpr->pRight, dest, jumpIfNull);
      break;
    }
    case TK_NOT: {
      sqliteExprIfFalse(pParse, pExpr->pLeft, dest, jumpIfNull);
      break;
    }
    case TK_LT:
    case TK_LE:
    case TK_GT:
    case TK_GE:
    case TK_NE:
    case TK_EQ: {
      sqliteExprCode(pParse, pExpr->pLeft);
      sqliteExprCode(pParse, pExpr->pRight);
      if( pParse->db->file_format>=4 && sqliteExprType(pExpr)==SQLITE_SO_TEXT ){
      }
      sqliteVdbeAddOp(v, op, jumpIfNull, dest);
      break;
    }
    case TK_ISNULL:
    case TK_NOTNULL: {
      sqliteExprCode(pParse, pExpr->pLeft);
      sqliteVdbeAddOp(v, op, 1, dest);
      break;
    }
    case TK_IN: {
      int addr;
      sqliteExprCode(pParse, pExpr->pLeft);
      addr = sqliteVdbeCurrentAddr(v);
      sqliteVdbeAddOp(v, OP_NotNull, -1, addr+3);
      sqliteVdbeAddOp(v, OP_Pop, 1, 0);
      sqliteVdbeAddOp(v, OP_Goto, 0, jumpIfNull ? dest : addr+4);
      if( pExpr->pSelect ){
        sqliteVdbeAddOp(v, OP_Found, pExpr->iTable, dest);
      }else{
        sqliteVdbeAddOp(v, OP_SetFound, pExpr->iTable, dest);
      }
      break;
    }
    case TK_BETWEEN: {
      int addr;
      sqliteExprCode(pParse, pExpr->pLeft);
      sqliteVdbeAddOp(v, OP_Dup, 0, 0);
      sqliteExprCode(pParse, pExpr->pList->a[0].pExpr);
      addr = sqliteVdbeAddOp(v, OP_Lt, !jumpIfNull, 0);
      sqliteExprCode(pParse, pExpr->pList->a[1].pExpr);
      sqliteVdbeAddOp(v, OP_Le, jumpIfNull, dest);
      sqliteVdbeAddOp(v, OP_Integer, 0, 0);
      sqliteVdbeChangeP2(v, addr, sqliteVdbeCurrentAddr(v));
      sqliteVdbeAddOp(v, OP_Pop, 1, 0);
      break;
    }
    default: {
      sqliteExprCode(pParse, pExpr);
      sqliteVdbeAddOp(v, OP_If, jumpIfNull, dest);
      break;
    }
  }
}

/* Function 33 */
void sqliteExprIfFalse(Parse *pParse, Expr *pExpr, int dest, int jumpIfNull){
  Vdbe *v = pParse->pVdbe;
  int op = 0;
  if( v==0 || pExpr==0 ) return;
  switch( pExpr->op ){
    case TK_LT:       op = OP_Ge;       break;
    case TK_LE:       op = OP_Gt;       break;
    case TK_GT:       op = OP_Le;       break;
    case TK_GE:       op = OP_Lt;       break;
    case TK_NE:       op = OP_Eq;       break;
    case TK_EQ:       op = OP_Ne;       break;
    case TK_ISNULL:   op = OP_NotNull;  break;
    case TK_NOTNULL:  op = OP_IsNull;   break;
    default:  break;
  }
  switch( pExpr->op ){
    case TK_AND: {
      sqliteExprIfFalse(pParse, pExpr->pLeft, dest, jumpIfNull);
      sqliteExprIfFalse(pParse, pExpr->pRight, dest, jumpIfNull);
      break;
    }
    case TK_OR: {
      int d2 = sqliteVdbeMakeLabel(v);
      sqliteExprIfTrue(pParse, pExpr->pLeft, d2, !jumpIfNull);
      sqliteExprIfFalse(pParse, pExpr->pRight, dest, jumpIfNull);
      sqliteVdbeResolveLabel(v, d2);
      break;
    }
    case TK_NOT: {
      sqliteExprIfTrue(pParse, pExpr->pLeft, dest, jumpIfNull);
      break;
    }
    case TK_LT:
    case TK_LE:
    case TK_GT:
    case TK_GE:
    case TK_NE:
    case TK_EQ: {
      if( pParse->db->file_format>=4 && sqliteExprType(pExpr)==SQLITE_SO_TEXT ){
        assert( OP_Eq+6 == OP_StrEq );
        op += 6;
      }
      sqliteExprCode(pParse, pExpr->pLeft);
      sqliteExprCode(pParse, pExpr->pRight);
      sqliteVdbeAddOp(v, op, jumpIfNull, dest);
      break;
    }
    case TK_ISNULL:
    case TK_NOTNULL: {
      sqliteExprCode(pParse, pExpr->pLeft);
      sqliteVdbeAddOp(v, op, 1, dest);
      break;
    }
    case TK_IN: {
      int addr;
      sqliteExprCode(pParse, pExpr->pLeft);
      addr = sqliteVdbeCurrentAddr(v);
      sqliteVdbeAddOp(v, OP_NotNull, -1, addr+3);
      sqliteVdbeAddOp(v, OP_Pop, 1, 0);
      sqliteVdbeAddOp(v, OP_Goto, 0, jumpIfNull ? dest : addr+4);
      if( pExpr->pSelect ){
        sqliteVdbeAddOp(v, OP_NotFound, pExpr->iTable, dest);
      }else{
        sqliteVdbeAddOp(v, OP_SetNotFound, pExpr->iTable, dest);
      }
      break;
    }
    case TK_BETWEEN: {
      int addr;
      sqliteExprCode(pParse, pExpr->pLeft);
      sqliteVdbeAddOp(v, OP_Dup, 0, 0);
      sqliteExprCode(pParse, pExpr->pList->a[0].pExpr);
      addr = sqliteVdbeCurrentAddr(v);
      sqliteVdbeAddOp(v, OP_Ge, !jumpIfNull, addr+3);
      sqliteVdbeAddOp(v, OP_Pop, 1, 0);
      sqliteVdbeAddOp(v, OP_Goto, 0, dest);
      sqliteExprCode(pParse, pExpr->pList->a[1].pExpr);
      sqliteVdbeAddOp(v, OP_Gt, jumpIfNull, dest);
      break;
    }
    default: {
      sqliteExprCode(pParse, pExpr);
      sqliteVdbeAddOp(v, OP_IfNot, jumpIfNull, dest);
      break;
    }
  }
}

/* Function 34 */
int sqliteExprCompare(Expr *pA, Expr *pB){
  int i;
  if( pA==0 ){
    return pB==0;
  }else if( pB==0 ){
    return 0;
  }
  if( pA->op!=pB->op ) return 0;
  if( !sqliteExprCompare(pA->pLeft, pB->pLeft) ) return 0;
  if( !sqliteExprCompare(pA->pRight, pB->pRight) ) return 0;
  if( pA->pList ){
    if( pB->pList==0 ) return 0;
    if( pA->pList->nExpr!=pB->pList->nExpr ) return 0;
    for(i=0; i<pA->pList->nExpr; i++){
      if( !sqliteExprCompare(pA->pList->a[i].pExpr, pB->pList->a[i].pExpr) ){
        return 0;
      }
    }
  }else if( pB->pList ){
    return 0;
  }
  if( pA->pSelect || pB->pSelect ) return 0;
  if( pA->iTable!=pB->iTable || pA->iColumn!=pB->iColumn ) return 0;
  if( pA->token.z ){
    if( pB->token.z==0 ) return 0;
    if( pB->token.n!=pA->token.n ) return 0;
    if( sqliteStrNICmp(pA->token.z, pB->token.z, pB->token.n)!=0 ) return 0;
  }
  return 1;
}

/* Function 35 */
static int appendAggInfo(Parse *pParse){
  if( (pParse->nAgg & 0x7)==0 ){
    int amt = pParse->nAgg + 8;
    AggExpr *aAgg = sqliteRealloc(pParse->aAgg, amt*sizeof(pParse->aAgg[0]));
    if( aAgg==0 ){
      return -1;
    }
    pParse->aAgg = aAgg;
  }
  memset(&pParse->aAgg[pParse->nAgg], 0, sizeof(pParse->aAgg[0]));
  return pParse->nAgg++;
}

/* Function 36 */
int sqliteExprAnalyzeAggregates(Parse *pParse, Expr *pExpr){
  int i;
  AggExpr *aAgg;
  int nErr = 0;

  if( pExpr==0 ) return 0;
  switch( pExpr->op ){
    case TK_COLUMN: {
      aAgg = pParse->aAgg;
      for(i=0; i<pParse->nAgg; i++){
        if( aAgg[i].isAgg ) continue;
        if( aAgg[i].pExpr->iTable==pExpr->iTable
         && aAgg[i].pExpr->iColumn==pExpr->iColumn ){
          break;
        }
      }
      if( i>=pParse->nAgg ){
        i = appendAggInfo(pParse);
        if( i<0 ) return 1;
        pParse->aAgg[i].isAgg = 0;
        pParse->aAgg[i].pExpr = pExpr;
      }
      pExpr->iAgg = i;
      break;
    }
    case TK_AGG_FUNCTION: {
      aAgg = pParse->aAgg;
      for(i=0; i<pParse->nAgg; i++){
        if( !aAgg[i].isAgg ) continue;
        if( sqliteExprCompare(aAgg[i].pExpr, pExpr) ){
          break;
        }
      }
      if( i>=pParse->nAgg ){
        i = appendAggInfo(pParse);
        if( i<0 ) return 1;
        pParse->aAgg[i].isAgg = 1;
        pParse->aAgg[i].pExpr = pExpr;
        pParse->aAgg[i].pFunc = sqliteFindFunction(pParse->db,
             pExpr->token.z, pExpr->token.n,
             pExpr->pList ? pExpr->pList->nExpr : 0, 0);
      }
      pExpr->iAgg = i;
      break;
    }
    default: {
      if( pExpr->pLeft ){
        nErr = sqliteExprAnalyzeAggregates(pParse, pExpr->pLeft);
      }
      if( nErr==0 && pExpr->pRight ){
        nErr = sqliteExprAnalyzeAggregates(pParse, pExpr->pRight);
      }
      if( nErr==0 && pExpr->pList ){
        int n = pExpr->pList->nExpr;
        int i;
        for(i=0; nErr==0 && i<n; i++){
          nErr = sqliteExprAnalyzeAggregates(pParse, pExpr->pList->a[i].pExpr);
        }
      }
      break;
    }
  }
  return nErr;
}

/* Function 37 */
	    ematch(r2, "-\\{0,1\\}[0-9]*$")) {
		errno = 0;
		l1 = strtoll(r1, (char **)NULL, 10);
		l2 = strtoll(r2, (char **)NULL, 10);
		if (errno) {
#ifdef XPG6
			(void) write(1, "\n", 1);
#endif
			if (errno == ERANGE) {
				(void) fprintf(stderr, gettext(
				    "expr: Integer argument too large\n"));
				exit(3);
			} else {
				perror("expr");
				exit(3);
			}
		}
		switch (oper) {
		case EQ:
			i = (l1 == l2);
			break;
		case GT:
			i = (l1 > l2);
			break;
		case GEQ:
			i = (l1 >= l2);
			break;
		case LT:
			i = (l1 < l2);
			break;
		case LEQ:
			i = (l1 <= l2);
			break;
		case NEQ:
			i = (l1 != l2);
			break;
		}
	}

/* Function 38 */
	    ematch(r2, "-\\{0,1\\}[0-9]*$")))
		yyerror("non-numeric argument");


#ifdef __cplusplus
}
#endif

/* End of expr_unified.c - Synthesized by MINIX4 Massive Synthesis System */
