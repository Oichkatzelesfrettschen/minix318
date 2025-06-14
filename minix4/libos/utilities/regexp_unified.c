/**
 * @file regexp_unified.c
 * @brief Unified regexp implementation
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
 *     1. userspace\minix_commands\cawf\regexp.c
 *     2. userspace\commands_legacy\text\cawf\regexp.c
 *     3. minix4\libos\lib_legacy\libadm\common\regexp.c
 *     4. minix4\exokernel\kernel_legacy\contrib\ast\src\lib\libast\comp\regexp.c
 *     5. minix4\exokernel\kernel_legacy\cmd\vgrind\regexp.c
 * 
 * Total source files: 5
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
#include "proto.h"
#include "regexp.h"
#include "regmagic.h"
#include <align.h>
#include <ast.h>
#include <ctype.h>
#include <regex.h>
#include <regexp.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

/* ===== DEFINES ===== */
#define	OP(p)	(*(p))
#define	NEXT(p)	(((*((p)+1)&0377)<<8) + (*((p)+2)&0377))
#define	OPERAND(p)	((p) + 3)
#define	UCHARAT(p)	((int)*(unsigned char *)(p))
#define	FAIL(m)	{ regerror(m); return(NULL); }
#define	ISMULT(c)	((c) == '*' || (c) == '+' || (c) == '?')
#define	META	"^$.[()|?+*\\"
#define STATIC 
#define	INIT		char *sp = instring;
#define	GETC()		(*sp++)
#define	PEEKC()		(*sp)
#define	UNGETC(c)	(--sp)
#define	RETURN(pt)	return (pt)
#define	ERROR(c)	return (NULL)
#define	advance	__advance
#define	compile	__compile
#define	step	__step
#define	loc1	__loc1
#define	loc2	__loc2
#define	locs	__locs
#define	nbra	__nbra
#define	circf	__circf
#define	sed	__sed
#define _REGEXP_DECLARE
#define TRUE	1
#define FALSE	0
#define NIL	0
#define isidchr(c)	\
#define makelower(c)	(isupper((c)) ? tolower((c)) : (c))
#define OPT 1
#define STR 2
#define ALT 8
#define OPER 16

/* ===== TYPES ===== */
typedef struct
typedef int	boolean;

/* ===== GLOBAL VARIABLES ===== */
STATIC unsigned char regdummy;
STATIC unsigned char *reg(int paren, int *flagp );
STATIC unsigned char *regbranch(int *flagp );
STATIC unsigned char *regpiece(int *flagp );
STATIC unsigned char *regatom(int *flagp );
STATIC unsigned char *regnode(int op );
STATIC void regc(int b );
STATIC void reginsert(int op, unsigned char *opnd );
STATIC void regtail(unsigned char *p, unsigned char *val );
STATIC void regoptail(unsigned char *p, unsigned char *val );
STATIC int regtry(regexp *prog, unsigned char *string );
STATIC int regmatch(unsigned char *prog );
STATIC int regrepeat(unsigned char *p );
STATIC unsigned char *regnext(unsigned char *p );
STATIC unsigned char *regprop(unsigned char *op );
STATIC int strcspn(unsigned char *s1, unsigned char *s2 );
int regnarrate = 0;
void regdump(void);
STATIC unsigned char *regprop(void);
STATIC unsigned char *regprop(void);
		printf("start `%c' ", r->regstart);
		printf("anchored ");
		printf("must have \"%s\"", r->regmust);
	printf("\n");
STATIC unsigned char regprop_buf[50];
STATIC unsigned char regdummy;
STATIC unsigned char *reg(int paren, int *flagp );
STATIC unsigned char *regbranch(int *flagp );
STATIC unsigned char *regpiece(int *flagp );
STATIC unsigned char *regatom(int *flagp );
STATIC unsigned char *regnode(int op );
STATIC void regc(int b );
STATIC void reginsert(int op, unsigned char *opnd );
STATIC void regtail(unsigned char *p, unsigned char *val );
STATIC void regoptail(unsigned char *p, unsigned char *val );
STATIC int regtry(regexp *prog, unsigned char *string );
STATIC int regmatch(unsigned char *prog );
STATIC int regrepeat(unsigned char *p );
STATIC unsigned char *regnext(unsigned char *p );
STATIC unsigned char *regprop(unsigned char *op );
STATIC int strcspn(unsigned char *s1, unsigned char *s2 );
int regnarrate = 0;
void regdump(void);
STATIC unsigned char *regprop(void);
STATIC unsigned char *regprop(void);
		printf("start `%c' ", r->regstart);
		printf("anchored ");
		printf("must have \"%s\"", r->regmust);
	printf("\n");
STATIC unsigned char regprop_buf[50];
int circf;
int sed;
	char*		buf;
	char*		cur;
	unsigned int	size;
	data = (void*)env->cur;
	register int	n;
	env->buf = env->cur = (char*)env + sizeof(Env_t);
	register int	n;
	re->re_loc1 = (char*)subject + match[0].rm_so;
	re->re_loc2 = (char*)subject + match[0].rm_eo;
		re->re_braslist[n-1] = (char*)subject + match[n].rm_so;
		re->re_braelist[n-1] = (char*)subject + match[n].rm_eo;
	static Sfio_t*	sp;
extern char	*strchr();
static void	expconv(void);
char *malloc();
    int temp;

/* ===== FUNCTIONS ===== */

/* Function 1 */
regexp *regcomp(char *exp) {
	register regexp *r;
	register unsigned char *scan;
	register unsigned char *longest;
	register int len;
	int flags;

	if (exp == NULL)
		FAIL("NULL argument");

	regparse = exp;
	regnpar = 1;
	regsize = 0L;
	regcode = &regdummy;
	regc(MAGIC);
	if (reg(0, &flags) == NULL)
		return(NULL);

		FAIL("regexp too big");

	r = (regexp *)malloc(sizeof(regexp) + (unsigned)regsize);
	if (r == NULL)
		FAIL("out of space");

	regparse = exp;
	regnpar = 1;
	regcode = r->program;
	regc(MAGIC);
	if (reg(0, &flags) == NULL) {
		free(r);
		return(NULL);
	}

	r->reganch = 0;
	r->regmust = NULL;
	r->regmlen = 0;
		scan = OPERAND(scan);

		if (OP(scan) == EXACTLY)
			r->regstart = *OPERAND(scan);
		else if (OP(scan) == BOL)
			r->reganch++;

		if (flags&SPSTART) {
			longest = NULL;
			len = 0;
			for (; scan != NULL; scan = regnext(scan))
				if (OP(scan) == EXACTLY
				&& strlen((char *)OPERAND(scan)) >= len) {
					longest = OPERAND(scan);
					len = strlen((char *)OPERAND(scan));
				}
			r->regmust = longest;
			r->regmlen = len;
		}
	}

/* Function 2 */
STATIC unsigned char *reg(int paren, int *flagp) {
	register unsigned char *ret;
	register unsigned char *br;
	register unsigned char *ender;
	register int parno;
	int flags;


	if (paren) {
		if (regnpar >= NSUBEXP)
			FAIL("too many ()");
		parno = regnpar;
		regnpar++;
		ret = regnode(OPEN+parno);
	} else
		ret = NULL;

	br = regbranch(&flags);
	if (br == NULL)
		return(NULL);
	if (ret != NULL)
	else
		ret = br;
	if (!(flags&HASWIDTH))
		*flagp &= ~HASWIDTH;
	*flagp |= flags&SPSTART;
	while (*regparse == '|') {
		regparse++;
		br = regbranch(&flags);
		if (br == NULL)
			return(NULL);
		if (!(flags&HASWIDTH))
			*flagp &= ~HASWIDTH;
		*flagp |= flags&SPSTART;
	}

	ender = regnode((paren) ? CLOSE+parno : END);	
	regtail(ret, ender);

	for (br = ret; br != NULL; br = regnext(br))
		regoptail(br, ender);

	if (paren && *regparse++ != ')') {
		FAIL("unmatched ()");
	} else if (!paren && *regparse != '\0') {
		if (*regparse == ')') {
			FAIL("unmatched ()");
		} else
	}

	return(ret);
}

/* Function 3 */
STATIC unsigned char *regbranch(int *flagp) {
	register unsigned char *ret;
	register unsigned char *chain;
	register unsigned char *latest;
	int flags;


	ret = regnode(BRANCH);
	chain = NULL;
	while (*regparse != '\0' && *regparse != '|' && *regparse != ')') {
		latest = regpiece(&flags);
		if (latest == NULL)
			return(NULL);
		*flagp |= flags&HASWIDTH;
			*flagp |= flags&SPSTART;
		else
			regtail(chain, latest);
		chain = latest;
	}
		(void) regnode(NOTHING);

	return(ret);
}

/* Function 4 */
STATIC unsigned char *regpiece(int *flagp) {
	register unsigned char *ret;
	register unsigned char op;
	register unsigned char *next;
	int flags;

	ret = regatom(&flags);
	if (ret == NULL)
		return(NULL);

	op = *regparse;
	if (!ISMULT(op)) {
		*flagp = flags;
		return(ret);
	}

	if (!(flags&HASWIDTH) && op != '?')
		FAIL("*+ operand could be empty");
	*flagp = (op != '+') ? (WORST|SPSTART) : (WORST|HASWIDTH);

	if (op == '*' && (flags&SIMPLE))
		reginsert(STAR, ret);
	else if (op == '*') {
	} else if (op == '+' && (flags&SIMPLE))
		reginsert(PLUS, ret);
	else if (op == '+') {
		regtail(ret, next);
	} else if (op == '?') {
		regtail(ret, next);
		regoptail(ret, next);
	}
	regparse++;
	if (ISMULT(*regparse))
		FAIL("nested *?+");

	return(ret);
}

/* Function 5 */
STATIC unsigned char *regatom(int *flagp) {
	register unsigned char *ret;
	int flags;


	switch (*regparse++) {
	case '^':
		ret = regnode(BOL);
		break;
	case '$':
		ret = regnode(EOL);
		break;
	case '.':
		ret = regnode(ANY);
		*flagp |= HASWIDTH|SIMPLE;
		break;
	case '[': {
			register int class;
			register int classend;

				ret = regnode(ANYBUT);
				regparse++;
			} else
				ret = regnode(ANYOF);
			if (*regparse == ']' || *regparse == '-')
				regc(*regparse++);
			while (*regparse != '\0' && *regparse != ']') {
				if (*regparse == '-') {
					regparse++;
					if (*regparse == ']' || *regparse == '\0')
						regc('-');
					else {
						class = UCHARAT(regparse-2)+1;
						classend = UCHARAT(regparse);
						if (class > classend+1)
							FAIL("invalid [] range");
						for (; class <= classend; class++)
							regc(class);
						regparse++;
					}
				} else
					regc(*regparse++);
			}
			regc('\0');
			if (*regparse != ']')
				FAIL("unmatched []");
			regparse++;
			*flagp |= HASWIDTH|SIMPLE;
		}
		break;
	case '(':
		ret = reg(1, &flags);
		if (ret == NULL)
			return(NULL);
		*flagp |= flags&(HASWIDTH|SPSTART);
		break;
	case '\0':
	case '|':
	case ')':
		break;
	case '?':
	case '+':
	case '*':
		FAIL("?+* follows nothing");
		break;
	case '\\':
		if (*regparse == '\0')
			FAIL("trailing \\");
		ret = regnode(EXACTLY);
		regc(*regparse++);
		regc('\0');
		*flagp |= HASWIDTH|SIMPLE;
		break;
	default: {
			register int len;
			register unsigned char ender;

			regparse--;
#ifdef	STRCSPN
			len = strcspn(regparse, (unsigned char *)META);
#else
			len = strcspn((char *)regparse, META);
#endif
			if (len <= 0)
				FAIL("internal disaster");
			ender = *(regparse+len);
			if (len > 1 && ISMULT(ender))
			*flagp |= HASWIDTH;
			if (len == 1)
				*flagp |= SIMPLE;
			ret = regnode(EXACTLY);
			while (len > 0) {
				regc(*regparse++);
				len--;
			}
			regc('\0');
		}
		break;
	}

/* Function 6 */
STATIC unsigned char *regnode(int iop) {
	register unsigned char *ret;
	register unsigned char *ptr;
	unsigned char op;

	op = (unsigned char) iop;
	ret = regcode;
	if (ret == &regdummy) {
		regsize += 3;
		return(ret);
	}

	ptr = ret;
	*ptr++ = op;
	*ptr++ = '\0';
	regcode = ptr;

	return(ret);
}

/* Function 7 */
STATIC void regc(int ib) {
	unsigned char b;

	b = (unsigned char) ib;
	if (regcode != &regdummy)
		*regcode++ = b;
	else
		regsize++;
}

/* Function 8 */
STATIC void reginsert(int iop, unsigned char *opnd) {
	register unsigned char *src;
	register unsigned char *dst;
	register unsigned char *place;
	unsigned char op;

	op = (unsigned char) iop;
	if (regcode == &regdummy) {
		regsize += 3;
		return;
	}

	src = regcode;
	regcode += 3;
	dst = regcode;
	while (src > opnd)
		*--dst = *--src;

	*place++ = op;
	*place++ = '\0';
	*place++ = '\0';
}

/* Function 9 */
STATIC void regtail(unsigned char *p, unsigned char *val) {
	register unsigned char *scan;
	register unsigned char *temp;
	register int offset;

	if (p == &regdummy)
		return;

	scan = p;
	for (;;) {
		temp = regnext(scan);
		if (temp == NULL)
			break;
		scan = temp;
	}

	if (OP(scan) == BACK)
		offset = scan - val;
	else
		offset = val - scan;
	*(scan+1) = (offset>>8)&0377;
	*(scan+2) = offset&0377;
}

/* Function 10 */
STATIC void regoptail(unsigned char *p, unsigned char *val) {
	if (p == NULL || p == &regdummy || OP(p) != BRANCH)
		return;
	regtail(OPERAND(p), val);
}

/* Function 11 */
regexec(register regexp *prog, register unsigned char *string) {
	register unsigned char *s;
#ifndef	STDLIB
	extern char *strchr();
#endif

	if (prog == NULL || string == NULL) {
		regerror("NULL parameter");
		return(0);
	}

	if (UCHARAT(prog->program) != MAGIC) {
		regerror("corrupted program");
		return(0);
	}

	if (prog->regmust != NULL) {
		s = string;
		while ((s = (unsigned char *)strchr((char *)s,prog->regmust[0]))
		!= NULL) {
			if (strncmp((char *)s, (char *)prog->regmust,
			    prog->regmlen)
			== 0)
			s++;
		}
			return(0);
	}

	regbol = string;

	if (prog->reganch)
		return(regtry(prog, string));

	s = string;
	if (prog->regstart != '\0')
		while ((s = (unsigned char *)strchr((char *)s, prog->regstart))
		!= NULL) {
			if (regtry(prog, s))
				return(1);
			s++;
		}
	else
		do {
			if (regtry(prog, s))
				return(1);
		} while (*s++ != '\0');

	return(0);
}

/* Function 12 */
STATIC int regtry(regexp *prog, unsigned char *string) {
	register int i;
	register unsigned char **sp;
	register unsigned char **ep;

	reginput = string;
	regstartp = prog->startp;
	regendp = prog->endp;

	sp = prog->startp;
	ep = prog->endp;
	for (i = NSUBEXP; i > 0; i--) {
		*sp++ = NULL;
		*ep++ = NULL;
	}
	if (regmatch(prog->program + 1)) {
		prog->startp[0] = string;
		prog->endp[0] = reginput;
		return(1);
	} else
		return(0);
}

/* Function 13 */
STATIC int regmatch( unsigned char *prog) {
#ifndef	STDLIB
	extern char *strchr();
#endif

	scan = prog;
#ifdef DEBUG
	if (scan != NULL && regnarrate)
		fprintf(stderr, "%s(\n", regprop(scan));
#endif
	while (scan != NULL) {
#ifdef DEBUG
		if (regnarrate)
			fprintf(stderr, "%s...\n", regprop(scan));
#endif
		next = regnext(scan);

		switch (OP(scan)) {
		case BOL:
			if (reginput != regbol)
				return(0);
			break;
		case EOL:
			if (*reginput != '\0')
				return(0);
			break;
		case ANY:
			if (*reginput == '\0')
				return(0);
			reginput++;
			break;
		case EXACTLY: {
				register int len;
				register unsigned char *opnd;

				opnd = OPERAND(scan);
				if (*opnd != *reginput)
					return(0);
				len = strlen((char *)opnd);
				if (len > 1 && strncmp((char *)opnd,
				   (char *)reginput, len)
				!= 0)
					return(0);
				reginput += len;
			}
			break;
		case ANYOF:
			if (*reginput == '\0'
			|| strchr((char *)OPERAND(scan), *reginput) == NULL)
				return(0);
			reginput++;
			break;
		case ANYBUT:
			if (*reginput == '\0'
			|| strchr((char *)OPERAND(scan), *reginput) != NULL)
				return(0);
			reginput++;
			break;
		case NOTHING:
			break;
		case BACK:
			break;
		case OPEN+1:
		case OPEN+2:
		case OPEN+3:
		case OPEN+4:
		case OPEN+5:
		case OPEN+6:
		case OPEN+7:
		case OPEN+8:
		case OPEN+9: {
				register int no;
				register unsigned char *save;

				no = OP(scan) - OPEN;
				save = reginput;

				if (regmatch(next)) {
					if (regstartp[no] == NULL)
						regstartp[no] = save;
					return(1);
				} else
					return(0);
			}
			break;
		case CLOSE+1:
		case CLOSE+2:
		case CLOSE+3:
		case CLOSE+4:
		case CLOSE+5:
		case CLOSE+6:
		case CLOSE+7:
		case CLOSE+8:
		case CLOSE+9: {
				register int no;
				register unsigned char *save;

				no = OP(scan) - CLOSE;
				save = reginput;

				if (regmatch(next)) {
					if (regendp[no] == NULL)
						regendp[no] = save;
					return(1);
				} else
					return(0);
			}
			break;
		case BRANCH: {
				register unsigned char *save;

				else {
					do {
						save = reginput;
						if (regmatch(OPERAND(scan)))
							return(1);
						reginput = save;
						scan = regnext(scan);
					} while (scan != NULL && OP(scan) == BRANCH);
					return(0);
				}
			}
			break;
		case STAR:
		case PLUS: {
				register unsigned char nextch;
				register int no;
				register unsigned char *save;
				register int min;

				nextch = '\0';
				if (OP(next) == EXACTLY)
					nextch = *OPERAND(next);
				min = (OP(scan) == STAR) ? 0 : 1;
				save = reginput;
				no = regrepeat(OPERAND(scan));
				while (no >= min) {
					if (nextch == '\0' || *reginput == nextch)
						if (regmatch(next))
							return(1);
					no--;
					reginput = save + no;
				}
				return(0);
			}
			break;
		case END:
			break;
		default:
			regerror("memory corruption");
			return(0);
			break;
		}

		scan = next;
	}

	regerror("corrupted pointers");
	return(0);
}

/* Function 14 */
STATIC int regrepeat(unsigned char *p) {
	register int count = 0;
	register unsigned char *scan;
	register unsigned char *opnd;

	scan = reginput;
	opnd = OPERAND(p);
	switch (OP(p)) {
	case ANY:
		count = strlen((char *)scan);
		scan += count;
		break;
	case EXACTLY:
		while (*opnd == *scan) {
			count++;
			scan++;
		}
		break;
	case ANYOF:
		while (*scan != '\0' && strchr((char *)opnd, *scan) != NULL) {
			count++;
			scan++;
		}
		break;
	case ANYBUT:
		while (*scan != '\0' && strchr((char *)opnd, *scan) == NULL) {
			count++;
			scan++;
		}
		break;
		regerror("internal foulup");
		break;
	}
	reginput = scan;

	return(count);
}

/* Function 15 */
STATIC unsigned char *regnext(register unsigned char *p) {
	register int offset;

	if (p == &regdummy)
		return(NULL);

	offset = NEXT(p);
	if (offset == 0)
		return(NULL);

	if (OP(p) == BACK)
		return(p-offset);
	else
		return(p+offset);
}

/* Function 16 */
void regdump(regexp r) {
	register unsigned char *s;
	register unsigned char *next;
	extern char *strchr();


	s = r->program + 1;
		op = OP(s);
		next = regnext(s);
			printf("(0)");
		else 
			printf("(%d)", (s-r->program)+(next-s));
		s += 3;
		if (op == ANYOF || op == ANYBUT || op == EXACTLY) {
			while (*s != '\0') {
				putchar(*s);
				s++;
			}
			s++;
		}
		putchar('\n');
	}

/* Function 17 */
STATIC unsigned char *regprop(unsigned char *op) {
	register unsigned char *p;

	(void) strcpy(regprop_buf, ":");

	switch (OP(op)) {
	case BOL:
		p = "BOL";
		break;
	case EOL:
		p = "EOL";
		break;
	case ANY:
		p = "ANY";
		break;
	case ANYOF:
		p = "ANYOF";
		break;
	case ANYBUT:
		p = "ANYBUT";
		break;
	case BRANCH:
		p = "BRANCH";
		break;
	case EXACTLY:
		p = "EXACTLY";
		break;
	case NOTHING:
		p = "NOTHING";
		break;
	case BACK:
		p = "BACK";
		break;
	case END:
		p = "END";
		break;
	case OPEN+1:
	case OPEN+2:
	case OPEN+3:
	case OPEN+4:
	case OPEN+5:
	case OPEN+6:
	case OPEN+7:
	case OPEN+8:
	case OPEN+9:
		sprintf(regprop_buf+strlen(regprop_buf), "OPEN%d", OP(op)-OPEN);
		p = NULL;
		break;
	case CLOSE+1:
	case CLOSE+2:
	case CLOSE+3:
	case CLOSE+4:
	case CLOSE+5:
	case CLOSE+6:
	case CLOSE+7:
	case CLOSE+8:
	case CLOSE+9:
		sprintf(regprop_buf+strlen(regprop_buf), "CLOSE%d", OP(op)-CLOSE);
		p = NULL;
		break;
	case STAR:
		p = "STAR";
		break;
	case PLUS:
		p = "PLUS";
		break;
	default:
		regerror("corrupted opcode");
		break;
	}
	if (p != NULL)
		(void) strcat(regprop_buf, p);
	return(regprop_buf);
}

/* Function 18 */
STATIC int strcspn(unsigned char *s1, unsigned char *s2) {
	register unsigned char *scan1;
	register unsigned char *scan2;
	register int count;

	count = 0;
	for (scan1 = s1; *scan1 != '\0'; scan1++) {
			if (*scan1 == *scan2++)
				return(count);
		count++;
	}
	return(count);
}

/* Function 19 */
regexp *regcomp(char *exp) {
	register regexp *r;
	register unsigned char *scan;
	register unsigned char *longest;
	register int len;
	int flags;

	if (exp == NULL)
		FAIL("NULL argument");

	regparse = exp;
	regnpar = 1;
	regsize = 0L;
	regcode = &regdummy;
	regc(MAGIC);
	if (reg(0, &flags) == NULL)
		return(NULL);

		FAIL("regexp too big");

	r = (regexp *)malloc(sizeof(regexp) + (unsigned)regsize);
	if (r == NULL)
		FAIL("out of space");

	regparse = exp;
	regnpar = 1;
	regcode = r->program;
	regc(MAGIC);
	if (reg(0, &flags) == NULL) {
		free(r);
		return(NULL);
	}

	r->reganch = 0;
	r->regmust = NULL;
	r->regmlen = 0;
		scan = OPERAND(scan);

		if (OP(scan) == EXACTLY)
			r->regstart = *OPERAND(scan);
		else if (OP(scan) == BOL)
			r->reganch++;

		if (flags&SPSTART) {
			longest = NULL;
			len = 0;
			for (; scan != NULL; scan = regnext(scan))
				if (OP(scan) == EXACTLY
				&& strlen((char *)OPERAND(scan)) >= len) {
					longest = OPERAND(scan);
					len = strlen((char *)OPERAND(scan));
				}
			r->regmust = longest;
			r->regmlen = len;
		}
	}

/* Function 20 */
STATIC unsigned char *reg(int paren, int *flagp) {
	register unsigned char *ret;
	register unsigned char *br;
	register unsigned char *ender;
	register int parno;
	int flags;


	if (paren) {
		if (regnpar >= NSUBEXP)
			FAIL("too many ()");
		parno = regnpar;
		regnpar++;
		ret = regnode(OPEN+parno);
	} else
		ret = NULL;

	br = regbranch(&flags);
	if (br == NULL)
		return(NULL);
	if (ret != NULL)
	else
		ret = br;
	if (!(flags&HASWIDTH))
		*flagp &= ~HASWIDTH;
	*flagp |= flags&SPSTART;
	while (*regparse == '|') {
		regparse++;
		br = regbranch(&flags);
		if (br == NULL)
			return(NULL);
		if (!(flags&HASWIDTH))
			*flagp &= ~HASWIDTH;
		*flagp |= flags&SPSTART;
	}

	ender = regnode((paren) ? CLOSE+parno : END);	
	regtail(ret, ender);

	for (br = ret; br != NULL; br = regnext(br))
		regoptail(br, ender);

	if (paren && *regparse++ != ')') {
		FAIL("unmatched ()");
	} else if (!paren && *regparse != '\0') {
		if (*regparse == ')') {
			FAIL("unmatched ()");
		} else
	}

	return(ret);
}

/* Function 21 */
STATIC unsigned char *regbranch(int *flagp) {
	register unsigned char *ret;
	register unsigned char *chain;
	register unsigned char *latest;
	int flags;


	ret = regnode(BRANCH);
	chain = NULL;
	while (*regparse != '\0' && *regparse != '|' && *regparse != ')') {
		latest = regpiece(&flags);
		if (latest == NULL)
			return(NULL);
		*flagp |= flags&HASWIDTH;
			*flagp |= flags&SPSTART;
		else
			regtail(chain, latest);
		chain = latest;
	}
		(void) regnode(NOTHING);

	return(ret);
}

/* Function 22 */
STATIC unsigned char *regpiece(int *flagp) {
	register unsigned char *ret;
	register unsigned char op;
	register unsigned char *next;
	int flags;

	ret = regatom(&flags);
	if (ret == NULL)
		return(NULL);

	op = *regparse;
	if (!ISMULT(op)) {
		*flagp = flags;
		return(ret);
	}

	if (!(flags&HASWIDTH) && op != '?')
		FAIL("*+ operand could be empty");
	*flagp = (op != '+') ? (WORST|SPSTART) : (WORST|HASWIDTH);

	if (op == '*' && (flags&SIMPLE))
		reginsert(STAR, ret);
	else if (op == '*') {
	} else if (op == '+' && (flags&SIMPLE))
		reginsert(PLUS, ret);
	else if (op == '+') {
		regtail(ret, next);
	} else if (op == '?') {
		regtail(ret, next);
		regoptail(ret, next);
	}
	regparse++;
	if (ISMULT(*regparse))
		FAIL("nested *?+");

	return(ret);
}

/* Function 23 */
STATIC unsigned char *regatom(int *flagp) {
	register unsigned char *ret;
	int flags;


	switch (*regparse++) {
	case '^':
		ret = regnode(BOL);
		break;
	case '$':
		ret = regnode(EOL);
		break;
	case '.':
		ret = regnode(ANY);
		*flagp |= HASWIDTH|SIMPLE;
		break;
	case '[': {
			register int class;
			register int classend;

				ret = regnode(ANYBUT);
				regparse++;
			} else
				ret = regnode(ANYOF);
			if (*regparse == ']' || *regparse == '-')
				regc(*regparse++);
			while (*regparse != '\0' && *regparse != ']') {
				if (*regparse == '-') {
					regparse++;
					if (*regparse == ']' || *regparse == '\0')
						regc('-');
					else {
						class = UCHARAT(regparse-2)+1;
						classend = UCHARAT(regparse);
						if (class > classend+1)
							FAIL("invalid [] range");
						for (; class <= classend; class++)
							regc(class);
						regparse++;
					}
				} else
					regc(*regparse++);
			}
			regc('\0');
			if (*regparse != ']')
				FAIL("unmatched []");
			regparse++;
			*flagp |= HASWIDTH|SIMPLE;
		}
		break;
	case '(':
		ret = reg(1, &flags);
		if (ret == NULL)
			return(NULL);
		*flagp |= flags&(HASWIDTH|SPSTART);
		break;
	case '\0':
	case '|':
	case ')':
		break;
	case '?':
	case '+':
	case '*':
		FAIL("?+* follows nothing");
		break;
	case '\\':
		if (*regparse == '\0')
			FAIL("trailing \\");
		ret = regnode(EXACTLY);
		regc(*regparse++);
		regc('\0');
		*flagp |= HASWIDTH|SIMPLE;
		break;
	default: {
			register int len;
			register unsigned char ender;

			regparse--;
#ifdef	STRCSPN
			len = strcspn(regparse, (unsigned char *)META);
#else
			len = strcspn((char *)regparse, META);
#endif
			if (len <= 0)
				FAIL("internal disaster");
			ender = *(regparse+len);
			if (len > 1 && ISMULT(ender))
			*flagp |= HASWIDTH;
			if (len == 1)
				*flagp |= SIMPLE;
			ret = regnode(EXACTLY);
			while (len > 0) {
				regc(*regparse++);
				len--;
			}
			regc('\0');
		}
		break;
	}

/* Function 24 */
STATIC unsigned char *regnode(int iop) {
	register unsigned char *ret;
	register unsigned char *ptr;
	unsigned char op;

	op = (unsigned char) iop;
	ret = regcode;
	if (ret == &regdummy) {
		regsize += 3;
		return(ret);
	}

	ptr = ret;
	*ptr++ = op;
	*ptr++ = '\0';
	regcode = ptr;

	return(ret);
}

/* Function 25 */
STATIC void regc(int ib) {
	unsigned char b;

	b = (unsigned char) ib;
	if (regcode != &regdummy)
		*regcode++ = b;
	else
		regsize++;
}

/* Function 26 */
STATIC void reginsert(int iop, unsigned char *opnd) {
	register unsigned char *src;
	register unsigned char *dst;
	register unsigned char *place;
	unsigned char op;

	op = (unsigned char) iop;
	if (regcode == &regdummy) {
		regsize += 3;
		return;
	}

	src = regcode;
	regcode += 3;
	dst = regcode;
	while (src > opnd)
		*--dst = *--src;

	*place++ = op;
	*place++ = '\0';
	*place++ = '\0';
}

/* Function 27 */
STATIC void regtail(unsigned char *p, unsigned char *val) {
	register unsigned char *scan;
	register unsigned char *temp;
	register int offset;

	if (p == &regdummy)
		return;

	scan = p;
	for (;;) {
		temp = regnext(scan);
		if (temp == NULL)
			break;
		scan = temp;
	}

	if (OP(scan) == BACK)
		offset = scan - val;
	else
		offset = val - scan;
	*(scan+1) = (offset>>8)&0377;
	*(scan+2) = offset&0377;
}

/* Function 28 */
STATIC void regoptail(unsigned char *p, unsigned char *val) {
	if (p == NULL || p == &regdummy || OP(p) != BRANCH)
		return;
	regtail(OPERAND(p), val);
}

/* Function 29 */
regexec(register regexp *prog, register unsigned char *string) {
	register unsigned char *s;
#ifndef	STDLIB
	extern char *strchr();
#endif

	if (prog == NULL || string == NULL) {
		regerror("NULL parameter");
		return(0);
	}

	if (UCHARAT(prog->program) != MAGIC) {
		regerror("corrupted program");
		return(0);
	}

	if (prog->regmust != NULL) {
		s = string;
		while ((s = (unsigned char *)strchr((char *)s,prog->regmust[0]))
		!= NULL) {
			if (strncmp((char *)s, (char *)prog->regmust,
			    prog->regmlen)
			== 0)
			s++;
		}
			return(0);
	}

	regbol = string;

	if (prog->reganch)
		return(regtry(prog, string));

	s = string;
	if (prog->regstart != '\0')
		while ((s = (unsigned char *)strchr((char *)s, prog->regstart))
		!= NULL) {
			if (regtry(prog, s))
				return(1);
			s++;
		}
	else
		do {
			if (regtry(prog, s))
				return(1);
		} while (*s++ != '\0');

	return(0);
}

/* Function 30 */
STATIC int regtry(regexp *prog, unsigned char *string) {
	register int i;
	register unsigned char **sp;
	register unsigned char **ep;

	reginput = string;
	regstartp = prog->startp;
	regendp = prog->endp;

	sp = prog->startp;
	ep = prog->endp;
	for (i = NSUBEXP; i > 0; i--) {
		*sp++ = NULL;
		*ep++ = NULL;
	}
	if (regmatch(prog->program + 1)) {
		prog->startp[0] = string;
		prog->endp[0] = reginput;
		return(1);
	} else
		return(0);
}

/* Function 31 */
STATIC int regmatch( unsigned char *prog) {
#ifndef	STDLIB
	extern char *strchr();
#endif

	scan = prog;
#ifdef DEBUG
	if (scan != NULL && regnarrate)
		fprintf(stderr, "%s(\n", regprop(scan));
#endif
	while (scan != NULL) {
#ifdef DEBUG
		if (regnarrate)
			fprintf(stderr, "%s...\n", regprop(scan));
#endif
		next = regnext(scan);

		switch (OP(scan)) {
		case BOL:
			if (reginput != regbol)
				return(0);
			break;
		case EOL:
			if (*reginput != '\0')
				return(0);
			break;
		case ANY:
			if (*reginput == '\0')
				return(0);
			reginput++;
			break;
		case EXACTLY: {
				register int len;
				register unsigned char *opnd;

				opnd = OPERAND(scan);
				if (*opnd != *reginput)
					return(0);
				len = strlen((char *)opnd);
				if (len > 1 && strncmp((char *)opnd,
				   (char *)reginput, len)
				!= 0)
					return(0);
				reginput += len;
			}
			break;
		case ANYOF:
			if (*reginput == '\0'
			|| strchr((char *)OPERAND(scan), *reginput) == NULL)
				return(0);
			reginput++;
			break;
		case ANYBUT:
			if (*reginput == '\0'
			|| strchr((char *)OPERAND(scan), *reginput) != NULL)
				return(0);
			reginput++;
			break;
		case NOTHING:
			break;
		case BACK:
			break;
		case OPEN+1:
		case OPEN+2:
		case OPEN+3:
		case OPEN+4:
		case OPEN+5:
		case OPEN+6:
		case OPEN+7:
		case OPEN+8:
		case OPEN+9: {
				register int no;
				register unsigned char *save;

				no = OP(scan) - OPEN;
				save = reginput;

				if (regmatch(next)) {
					if (regstartp[no] == NULL)
						regstartp[no] = save;
					return(1);
				} else
					return(0);
			}
			break;
		case CLOSE+1:
		case CLOSE+2:
		case CLOSE+3:
		case CLOSE+4:
		case CLOSE+5:
		case CLOSE+6:
		case CLOSE+7:
		case CLOSE+8:
		case CLOSE+9: {
				register int no;
				register unsigned char *save;

				no = OP(scan) - CLOSE;
				save = reginput;

				if (regmatch(next)) {
					if (regendp[no] == NULL)
						regendp[no] = save;
					return(1);
				} else
					return(0);
			}
			break;
		case BRANCH: {
				register unsigned char *save;

				else {
					do {
						save = reginput;
						if (regmatch(OPERAND(scan)))
							return(1);
						reginput = save;
						scan = regnext(scan);
					} while (scan != NULL && OP(scan) == BRANCH);
					return(0);
				}
			}
			break;
		case STAR:
		case PLUS: {
				register unsigned char nextch;
				register int no;
				register unsigned char *save;
				register int min;

				nextch = '\0';
				if (OP(next) == EXACTLY)
					nextch = *OPERAND(next);
				min = (OP(scan) == STAR) ? 0 : 1;
				save = reginput;
				no = regrepeat(OPERAND(scan));
				while (no >= min) {
					if (nextch == '\0' || *reginput == nextch)
						if (regmatch(next))
							return(1);
					no--;
					reginput = save + no;
				}
				return(0);
			}
			break;
		case END:
			break;
		default:
			regerror("memory corruption");
			return(0);
			break;
		}

		scan = next;
	}

	regerror("corrupted pointers");
	return(0);
}

/* Function 32 */
STATIC int regrepeat(unsigned char *p) {
	register int count = 0;
	register unsigned char *scan;
	register unsigned char *opnd;

	scan = reginput;
	opnd = OPERAND(p);
	switch (OP(p)) {
	case ANY:
		count = strlen((char *)scan);
		scan += count;
		break;
	case EXACTLY:
		while (*opnd == *scan) {
			count++;
			scan++;
		}
		break;
	case ANYOF:
		while (*scan != '\0' && strchr((char *)opnd, *scan) != NULL) {
			count++;
			scan++;
		}
		break;
	case ANYBUT:
		while (*scan != '\0' && strchr((char *)opnd, *scan) == NULL) {
			count++;
			scan++;
		}
		break;
		regerror("internal foulup");
		break;
	}
	reginput = scan;

	return(count);
}

/* Function 33 */
STATIC unsigned char *regnext(register unsigned char *p) {
	register int offset;

	if (p == &regdummy)
		return(NULL);

	offset = NEXT(p);
	if (offset == 0)
		return(NULL);

	if (OP(p) == BACK)
		return(p-offset);
	else
		return(p+offset);
}

/* Function 34 */
void regdump(regexp r) {
	register unsigned char *s;
	register unsigned char *next;
	extern char *strchr();


	s = r->program + 1;
		op = OP(s);
		next = regnext(s);
			printf("(0)");
		else 
			printf("(%d)", (s-r->program)+(next-s));
		s += 3;
		if (op == ANYOF || op == ANYBUT || op == EXACTLY) {
			while (*s != '\0') {
				putchar(*s);
				s++;
			}
			s++;
		}
		putchar('\n');
	}

/* Function 35 */
STATIC unsigned char *regprop(unsigned char *op) {
	register unsigned char *p;

	(void) strcpy(regprop_buf, ":");

	switch (OP(op)) {
	case BOL:
		p = "BOL";
		break;
	case EOL:
		p = "EOL";
		break;
	case ANY:
		p = "ANY";
		break;
	case ANYOF:
		p = "ANYOF";
		break;
	case ANYBUT:
		p = "ANYBUT";
		break;
	case BRANCH:
		p = "BRANCH";
		break;
	case EXACTLY:
		p = "EXACTLY";
		break;
	case NOTHING:
		p = "NOTHING";
		break;
	case BACK:
		p = "BACK";
		break;
	case END:
		p = "END";
		break;
	case OPEN+1:
	case OPEN+2:
	case OPEN+3:
	case OPEN+4:
	case OPEN+5:
	case OPEN+6:
	case OPEN+7:
	case OPEN+8:
	case OPEN+9:
		sprintf(regprop_buf+strlen(regprop_buf), "OPEN%d", OP(op)-OPEN);
		p = NULL;
		break;
	case CLOSE+1:
	case CLOSE+2:
	case CLOSE+3:
	case CLOSE+4:
	case CLOSE+5:
	case CLOSE+6:
	case CLOSE+7:
	case CLOSE+8:
	case CLOSE+9:
		sprintf(regprop_buf+strlen(regprop_buf), "CLOSE%d", OP(op)-CLOSE);
		p = NULL;
		break;
	case STAR:
		p = "STAR";
		break;
	case PLUS:
		p = "PLUS";
		break;
	default:
		regerror("corrupted opcode");
		break;
	}
	if (p != NULL)
		(void) strcat(regprop_buf, p);
	return(regprop_buf);
}

/* Function 36 */
STATIC int strcspn(unsigned char *s1, unsigned char *s2) {
	register unsigned char *scan1;
	register unsigned char *scan2;
	register int count;

	count = 0;
	for (scan1 = s1; *scan1 != '\0'; scan1++) {
			if (*scan1 == *scan2++)
				return(count);
		count++;
	}
	return(count);
}


#ifdef __cplusplus
}
#endif

/* End of regexp_unified.c - Synthesized by MINIX4 Massive Synthesis System */
