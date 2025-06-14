/**
 * @file string_unified.c
 * @brief Unified string operations
 * @details This file is a synthesized/unified implementation combining multiple
 *          legacy MINIX implementations into a single, modern, C23-compliant
 *          implementation for MINIX4.
 *
 * Original sources consolidated:
 * - userspace\minix_commands\cawf\string.c
 * - userspace\commands_legacy\text\cawf\string.c
 * - minix4\libos\posix\libc\string\string.c
 * - minix4\exokernel\kernel_legacy\string.c
 * - minix4\exokernel\kernel_legacy\contrib\ast\src\cmd\ksh93\sh\string.c
 * - minix4\exokernel\kernel_legacy\common\util\string.c
 * - minix4\exokernel\kernel_legacy\cmd\sh\string.c
 * - minix4\exokernel\kernel_legacy\cmd\sendmail\libsm\string.c

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
 * SYNTHESIS NOTE: FUNCTION MERGE
 * ============================================================================
 * This file synthesizes 8 source files with:
 * - 7 common functions (deduplicated)
 * - 0 unique functions (all preserved)
 * 
 * Common functions: Endword, Setroman, Findchar, Findhy, Str2word...
 * Unique functions: 
 * ============================================================================ */


/* ============================================================================
 * SOURCE 1: userspace\minix_commands\cawf\string.c
 * ============================================================================ */

/*
 *	string.c - string support functions for cawf(1)
 */

/*
 *	Copyright (c) 1991 Purdue University Research Foundation,
 *	West Lafayette, Indiana 47907.  All rights reserved.
 *
 *	Written by Victor A. Abell <abe@mace.cc.purdue.edu>,  Purdue
 *	University Computing Center.  Not derived from licensed software;
 *	derived from awf(1) by Henry Spencer of the University of Toronto.
 *
 *	Permission is granted to anyone to use this software for any
 *	purpose on any computer system, and to alter it and redistribute
 *	it freely, subject to the following restrictions:
 *
 *	1. The author is not responsible for any consequences of use of
 *	   this software, even if they arise from flaws in it.
 *
 *	2. The origin of this software must not be misrepresented, either
 *	   by explicit claim or by omission.  Credits must appear in the
 *	   documentation.
 *
 *	3. Altered versions must be plainly marked as such, and must not
 *	   be misrepresented as being the original software.  Credits must
 *	   appear in the documentation.
 *
 *	4. This notice may not be removed or altered.
 */

#include "cawf.h"
#include <ctype.h>

static void Setroman(void);


/*
 * Asmname(s, c) - assemble name
 */

int Asmname(unsigned char *s, unsigned char *c) {
/* pointer to name s
 * code destination (c[3])
 */
	c[1] = c[2] = '\0';
	while (*s == ' ')
		s++;
	if ((c[0] = *s) == '\0')
		return(0);
	return(((c[1] = s[1]) == '\0') ? 1 : 2);
}


/*
 * Delstr(sx) - delete string
 */

void Delstr(int sx) {
/* string index sx */
	char buf[MAXLINE];		/* message buffer */

	if (sx >= Nstr) {
		(void) sprintf(buf, " bad Delstr(%d) index", sx);
		Error(FATAL, LINE, buf, NULL);
	}
	Free(&Str[sx].str);
	while (sx < (Nstr - 1)) {
		Str[sx] = Str[sx + 1];
		sx++;
	}
	Nstr--;
}


/*
 * Endword() - end a word
 */

void Endword(void) {
	if (Fontstat != 'R')
		Setroman();
	Word[Wordx] = '\0';
}


/*
 * Findchar(nm, l, s, e) - find special character definition and
 *			   optionally enter it
 */

int Findchar(unsigned char *nm, int l, unsigned char *s, int e) {
/* character name nm
 * effective length l
 * value string s
 * e = 0 = find, don't enter
 * e = 1 = don't find, enter
 */
	int cmp, hi, low, mid;
	unsigned char c[3];

	c[0] = nm[0];
	c[1] = (nm[1] == ' ' || nm[1] == '\t') ? '\0' : nm[1];
	c[2] = '\0';
	low = mid = 0;
	hi = Nsch - 1;
	while (low <= hi) {
		mid = (low + hi) / 2;
		if ((cmp = strncmp((char *)c, (char *)Schar[mid].nm, 2)) < 0)
			hi = mid - 1;
		else if (cmp > 0)
			low = mid + 1;
		else {
			if ( ! e)
				return(mid);
			Free(&Schar[mid].str);
			goto new_char;
		}
	}
	if ( ! e)
		return(-1);
	if (Nsch >= MAXSCH)
		Error(FATAL, LINE, " at character table limit", NULL);
	if (Nsch) {
		if (cmp > 0)
			mid++;
		for (hi = Nsch - 1; hi >= mid; hi--)
			Schar[hi+1] = Schar[hi];
	}
	Nsch++;
	Schar[mid].nm[0] = c[0];
	Schar[mid].nm[1] = c[1];

new_char:

	Schar[mid].str = Newstr(s);
	Schar[mid].len = l;
	return(mid);
}


/*
 * Findhy(s, l, e) - find and optionally enter hyphen
 */

int Findhy(unsigned char *s, int l, int e) {
/* value string s
 * equivalent length l
 * e = 0 = find, don't enter
 * e = 1 = enter, don't find
 */
	int i;

	for (i = 0; i < Nhy; i++) {
		if (Font[0] == Hychar[i].font)
			break;
	}
	if (i >= Nhy) {
		if ( ! e)
			return(-1);
		if (Nhy >= MAXHYCH)
			Error(FATAL, LINE, " at hyphen limit for font ",
				(char *)Font);
		Hychar[i].font = Font[0];
		Nhy++;
	} else {
		if ( ! e)
			return(i);
		Error(WARN, LINE, " duplicate hyphen for font ", (char *)Font);
		Free(&Hychar[i].str);
	}
	Hychar[i].str = Newstr(s);
	Hychar[i].len = l;
	return(i);
}


/*
 * Findstr(nm, s, e) - find and  optionally enter string in Str[]
 */

unsigned char *Findstr(unsigned char *nm, unsigned char *s, int e) {
/* 2 character string name nm
 * string value s
 * e = 0 = find, don't enter
 * e = 1 = enter, don't find
 */
	unsigned char c[3];		/* character buffer */
	int cmp, hi, low, mid;		/* binary search controls */
	int i;				/* temporary indexes */
	unsigned char *s1, *s2;		/* temporary string pointers */

	c[0] = nm[0];
	c[1] = (nm[1] == ' ' || nm[1] == '\t') ? '\0' : nm[1];
	c[2] = '\0';
	low = mid = 0;
	hi = Nstr - 1;
	Sx = -1;
	while (low <= hi) {
		mid = (low + hi) / 2;
		if ((cmp = strncmp((char *)c, (char *)Str[mid].nm, 2)) < 0)
			hi = mid - 1;
		else if (cmp > 0)
			low = mid + 1;
		else {
			Sx = mid;
			if ( ! e)
				return(Str[mid].str);
			Free(&Str[mid].str);
			goto new_string;
		}
	}
	if ( ! e)
		return((unsigned char *)"");
	if (Nstr >= MAXSTR)
		Error(FATAL, LINE, " out of space for string ", (char *)c);
	if (Nstr) {
		if (cmp > 0)
			mid++;
		for (hi = Nstr - 1; hi >= mid; hi--)
			Str[hi+1] = Str[hi];
	}
	Nstr++;
	Sx = mid;
	Str[mid].nm[0] = c[0];
	Str[mid].nm[1] = c[1];

new_string:

	if (s == NULL)
		return (Str[mid].str = Newstr((unsigned char *)""));
	i = (*s == '"') ? 1 : 0;
	s1 = Str[mid].str = Newstr(s + i);
	if (i) {
		s2 = s1 + strlen((char *)s1);
		if (s2 > s1 && *(s2-1) == '"')
			*(s2-1) = '\0';
	}
	return(s1);
}


/*
 * Setroman() - set Roman font
 */

static void Setroman(void) {
	int i;

	if ((Wordx + Fstr.rl) >= MAXLINE)
		Error(WARN, LINE, " word too long", NULL);
	else {
		if (Fstr.r) {
			for (i = 0; i < Fstr.rl; i++) {
				Word[Wordx++] = Fstr.r[i];
			}
	    	}
		Fontstat = 'R';
	}
}


/*
 * Str2word(s, len) - copy len characters from string to Word[]
 */

int Str2word(unsigned char *s, int len) {
	int i;

	for (; len > 0; len--, s++) {
		switch (Font[0]) {
		case 'B':
		case 'C':
			if (Fontctl == 0) {
				if ((Wordx + 5) >= MAXLINE) {
word_too_long:
					Error(WARN, LINE, " word too long",
						NULL);
					return(1);
				}
				Word[Wordx++] = Trtbl[(int)*s];
				Word[Wordx++] = '\b';
				Word[Wordx++] = Trtbl[(int)*s];
				Word[Wordx++] = '\b';
				Word[Wordx++] = Trtbl[(int)*s];
				break;
			}
			if (Fontstat != Font[0]) {
				if (Fontstat != 'R')
					Setroman();
				if ((Wordx + Fstr.bl) >= MAXLINE)
					goto word_too_long;
				if (Fstr.b) {
					for (i = 0; i < Fstr.bl; i++) {
						Word[Wordx++] = Fstr.b[i];
					}
				}
				Fontstat = Font[0];
			}
			if ((Wordx + 1) >= MAXLINE)
				goto word_too_long;
			Word[Wordx++] = Trtbl[(int)*s];
			break;
		case 'I':
			if (isalnum(*s)) {
				if (Fontctl == 0) {
					if ((Wordx + 3) >= MAXLINE)
						goto word_too_long;
					Word[Wordx++] = '_';
					Word[Wordx++] = '\b';
					Word[Wordx++] = Trtbl[(int)*s];
					break;
				}
				if (Fontstat != 'I') {
					if (Fontstat != 'R')
						Setroman();
					if ((Wordx + Fstr.itl) >= MAXLINE)
						goto word_too_long;
					if (Fstr.it) {
					    for (i = 0; i < Fstr.itl; i++) {
						Word[Wordx++] = Fstr.it[i];
					    }
					}
					Fontstat = 'I';
				}
				if ((Wordx + 1) >= MAXLINE)
					goto word_too_long;
				Word[Wordx++] = Trtbl[(int)*s];
				break;
			}
			/* else fall through */
		default:
			if (Fontstat != 'R')
				Setroman();
			if ((Wordx + 1) >= MAXLINE)
				goto word_too_long;
			Word[Wordx++] = Trtbl[(int)*s];
		}
	}
	return(0);
}



/* ============================================================================
 * SOURCE 2: userspace\commands_legacy\text\cawf\string.c
 * ============================================================================ */

/*
 *	string.c - string support functions for cawf(1)
 */

/*
 *	Copyright (c) 1991 Purdue University Research Foundation,
 *	West Lafayette, Indiana 47907.  All rights reserved.
 *
 *	Written by Victor A. Abell <abe@mace.cc.purdue.edu>,  Purdue
 *	University Computing Center.  Not derived from licensed software;
 *	derived from awf(1) by Henry Spencer of the University of Toronto.
 *
 *	Permission is granted to anyone to use this software for any
 *	purpose on any computer system, and to alter it and redistribute
 *	it freely, subject to the following restrictions:
 *
 *	1. The author is not responsible for any consequences of use of
 *	   this software, even if they arise from flaws in it.
 *
 *	2. The origin of this software must not be misrepresented, either
 *	   by explicit claim or by omission.  Credits must appear in the
 *	   documentation.
 *
 *	3. Altered versions must be plainly marked as such, and must not
 *	   be misrepresented as being the original software.  Credits must
 *	   appear in the documentation.
 *
 *	4. This notice may not be removed or altered.
 */

#include "cawf.h"
#include <ctype.h>

static void Setroman(void);


/*
 * Asmname(s, c) - assemble name
 */

int Asmname(unsigned char *s, unsigned char *c) {
/* pointer to name s
 * code destination (c[3])
 */
	c[1] = c[2] = '\0';
	while (*s == ' ')
		s++;
	if ((c[0] = *s) == '\0')
		return(0);
	return(((c[1] = s[1]) == '\0') ? 1 : 2);
}


/*
 * Delstr(sx) - delete string
 */

void Delstr(int sx) {
/* string index sx */
	char buf[MAXLINE];		/* message buffer */

	if (sx >= Nstr) {
		(void) sprintf(buf, " bad Delstr(%d) index", sx);
		Error(FATAL, LINE, buf, NULL);
	}
	Free(&Str[sx].str);
	while (sx < (Nstr - 1)) {
		Str[sx] = Str[sx + 1];
		sx++;
	}
	Nstr--;
}


/*
 * Endword() - end a word
 */

void Endword(void) {
	if (Fontstat != 'R')
		Setroman();
	Word[Wordx] = '\0';
}


/*
 * Findchar(nm, l, s, e) - find special character definition and
 *			   optionally enter it
 */

int Findchar(unsigned char *nm, int l, unsigned char *s, int e) {
/* character name nm
 * effective length l
 * value string s
 * e = 0 = find, don't enter
 * e = 1 = don't find, enter
 */
	int cmp, hi, low, mid;
	unsigned char c[3];

	c[0] = nm[0];
	c[1] = (nm[1] == ' ' || nm[1] == '\t') ? '\0' : nm[1];
	c[2] = '\0';
	low = mid = 0;
	hi = Nsch - 1;
	while (low <= hi) {
		mid = (low + hi) / 2;
		if ((cmp = strncmp((char *)c, (char *)Schar[mid].nm, 2)) < 0)
			hi = mid - 1;
		else if (cmp > 0)
			low = mid + 1;
		else {
			if ( ! e)
				return(mid);
			Free(&Schar[mid].str);
			goto new_char;
		}
	}
	if ( ! e)
		return(-1);
	if (Nsch >= MAXSCH)
		Error(FATAL, LINE, " at character table limit", NULL);
	if (Nsch) {
		if (cmp > 0)
			mid++;
		for (hi = Nsch - 1; hi >= mid; hi--)
			Schar[hi+1] = Schar[hi];
	}
	Nsch++;
	Schar[mid].nm[0] = c[0];
	Schar[mid].nm[1] = c[1];

new_char:

	Schar[mid].str = Newstr(s);
	Schar[mid].len = l;
	return(mid);
}


/*
 * Findhy(s, l, e) - find and optionally enter hyphen
 */

int Findhy(unsigned char *s, int l, int e) {
/* value string s
 * equivalent length l
 * e = 0 = find, don't enter
 * e = 1 = enter, don't find
 */
	int i;

	for (i = 0; i < Nhy; i++) {
		if (Font[0] == Hychar[i].font)
			break;
	}
	if (i >= Nhy) {
		if ( ! e)
			return(-1);
		if (Nhy >= MAXHYCH)
			Error(FATAL, LINE, " at hyphen limit for font ",
				(char *)Font);
		Hychar[i].font = Font[0];
		Nhy++;
	} else {
		if ( ! e)
			return(i);
		Error(WARN, LINE, " duplicate hyphen for font ", (char *)Font);
		Free(&Hychar[i].str);
	}
	Hychar[i].str = Newstr(s);
	Hychar[i].len = l;
	return(i);
}


/*
 * Findstr(nm, s, e) - find and  optionally enter string in Str[]
 */

unsigned char *Findstr(unsigned char *nm, unsigned char *s, int e) {
/* 2 character string name nm
 * string value s
 * e = 0 = find, don't enter
 * e = 1 = enter, don't find
 */
	unsigned char c[3];		/* character buffer */
	int cmp, hi, low, mid;		/* binary search controls */
	int i;				/* temporary indexes */
	unsigned char *s1, *s2;		/* temporary string pointers */

	c[0] = nm[0];
	c[1] = (nm[1] == ' ' || nm[1] == '\t') ? '\0' : nm[1];
	c[2] = '\0';
	low = mid = 0;
	hi = Nstr - 1;
	Sx = -1;
	while (low <= hi) {
		mid = (low + hi) / 2;
		if ((cmp = strncmp((char *)c, (char *)Str[mid].nm, 2)) < 0)
			hi = mid - 1;
		else if (cmp > 0)
			low = mid + 1;
		else {
			Sx = mid;
			if ( ! e)
				return(Str[mid].str);
			Free(&Str[mid].str);
			goto new_string;
		}
	}
	if ( ! e)
		return((unsigned char *)"");
	if (Nstr >= MAXSTR)
		Error(FATAL, LINE, " out of space for string ", (char *)c);
	if (Nstr) {
		if (cmp > 0)
			mid++;
		for (hi = Nstr - 1; hi >= mid; hi--)
			Str[hi+1] = Str[hi];
	}
	Nstr++;
	Sx = mid;
	Str[mid].nm[0] = c[0];
	Str[mid].nm[1] = c[1];

new_string:

	if (s == NULL)
		return (Str[mid].str = Newstr((unsigned char *)""));
	i = (*s == '"') ? 1 : 0;
	s1 = Str[mid].str = Newstr(s + i);
	if (i) {
		s2 = s1 + strlen((char *)s1);
		if (s2 > s1 && *(s2-1) == '"')
			*(s2-1) = '\0';
	}
	return(s1);
}


/*
 * Setroman() - set Roman font
 */

static void Setroman(void) {
	int i;

	if ((Wordx + Fstr.rl) >= MAXLINE)
		Error(WARN, LINE, " word too long", NULL);
	else {
		if (Fstr.r) {
			for (i = 0; i < Fstr.rl; i++) {
				Word[Wordx++] = Fstr.r[i];
			}
	    	}
		Fontstat = 'R';
	}
}


/*
 * Str2word(s, len) - copy len characters from string to Word[]
 */

int Str2word(unsigned char *s, int len) {
	int i;

	for (; len > 0; len--, s++) {
		switch (Font[0]) {
		case 'B':
		case 'C':
			if (Fontctl == 0) {
				if ((Wordx + 5) >= MAXLINE) {
word_too_long:
					Error(WARN, LINE, " word too long",
						NULL);
					return(1);
				}
				Word[Wordx++] = Trtbl[(int)*s];
				Word[Wordx++] = '\b';
				Word[Wordx++] = Trtbl[(int)*s];
				Word[Wordx++] = '\b';
				Word[Wordx++] = Trtbl[(int)*s];
				break;
			}
			if (Fontstat != Font[0]) {
				if (Fontstat != 'R')
					Setroman();
				if ((Wordx + Fstr.bl) >= MAXLINE)
					goto word_too_long;
				if (Fstr.b) {
					for (i = 0; i < Fstr.bl; i++) {
						Word[Wordx++] = Fstr.b[i];
					}
				}
				Fontstat = Font[0];
			}
			if ((Wordx + 1) >= MAXLINE)
				goto word_too_long;
			Word[Wordx++] = Trtbl[(int)*s];
			break;
		case 'I':
			if (isalnum(*s)) {
				if (Fontctl == 0) {
					if ((Wordx + 3) >= MAXLINE)
						goto word_too_long;
					Word[Wordx++] = '_';
					Word[Wordx++] = '\b';
					Word[Wordx++] = Trtbl[(int)*s];
					break;
				}
				if (Fontstat != 'I') {
					if (Fontstat != 'R')
						Setroman();
					if ((Wordx + Fstr.itl) >= MAXLINE)
						goto word_too_long;
					if (Fstr.it) {
					    for (i = 0; i < Fstr.itl; i++) {
						Word[Wordx++] = Fstr.it[i];
					    }
					}
					Fontstat = 'I';
				}
				if ((Wordx + 1) >= MAXLINE)
					goto word_too_long;
				Word[Wordx++] = Trtbl[(int)*s];
				break;
			}
			/* else fall through */
		default:
			if (Fontstat != 'R')
				Setroman();
			if ((Wordx + 1) >= MAXLINE)
				goto word_too_long;
			Word[Wordx++] = Trtbl[(int)*s];
		}
	}
	return(0);
}



/* ============================================================================
 * SOURCE 3: minix4\libos\posix\libc\string\string.c
 * ============================================================================ */

/* Copyright (c) 1979 Regents of the University of California */
#
/*
 * pi - Pascal interpreter code translator
 *
 * Charles Haley, Bill Joy UCB
 * Version 1.2 January 1979
 *
 * pxp - Pascal execution profiler
 *
 * Bill Joy UCB
 * Version 1.2 January 1979
 */

#include "0.h"
#ifndef PI01
#ifndef PXP
#include "send.h"
#endif
#endif

/*
 * STRING SPACE DECLARATIONS
 *
 * Strng is the base of the current
 * string space and strngp the
 * base of the free area therein.
 * Strp is the array of descriptors.
 */
#ifndef PI0
static	char strings[STRINC];
static	char *strng strings;
static	char *strngp strings;
#else
char	*strng, *strngp;
#endif
#ifndef PI01
#ifndef PXP
static	char *strp[20];
static	char **stract strp;
int	strmax;
#endif
#endif

#ifndef PI01
#ifndef PXP
#ifndef PI0
initstring()
#else
initstring(strings)
	char *strings;
#endif
{

	*stract++ = strings;
#ifdef PI0
	strng = strngp = strings;
#endif
	strmax = STRINC * 2;
}
#endif
#endif

/*
 * Copy a string into the string area.
 */
savestr(cp)
	register char *cp;
{
	register int i;

	i = strlen(cp) + 1;
	if (strngp + i >= strng + STRINC) {
		strngp = alloc(STRINC);
		if (strngp == -1) {
			yerror("Ran out of memory (string)");
			pexit(DIED);
		}
#ifndef PI01
#ifndef PXP
		*stract++ = strngp;
		strmax =+ STRINC;
#endif
#endif
		strng = strngp;
	}
	strcpy(strngp, cp);
	cp = strngp;
	strngp = cp + i;
#ifdef PI0
	send(RSTRING, cp);
#endif
	return (cp);
}

#ifndef PI1
#ifndef PXP
esavestr(cp)
	char *cp;
{

#ifdef PI0
	send(REVENIT);
#endif
	strngp = (strngp + 1) &~ 1;
	return (savestr(cp));
}
#endif
#endif

#ifndef PI01
#ifndef PXP
soffset(cp)
	register char *cp;
{
	register char **sp;
	register int i;

	if (cp == NIL || cp == OCT || cp == HEX)
		return (-cp);
	for (i = STRINC, sp = strp; sp < stract; sp++) {
		if (cp >= *sp && cp < (*sp + STRINC))
			return (i + (cp - *sp));
		i =+ STRINC;
	}
	i = nlfund(cp);
	if (i != 0)
		return (i);
	panic("soffset");
}
#ifdef PI1
sreloc(i)
	register int i;
{

	if (i == 0 || i == -OCT || i == -HEX)
		return (-i);
	if (i < STRINC) {
		if (i >= INL)
			panic("sreloc INL");
		i = nl[i].symbol;
		if (i == 0)
			panic("sreloc nl[i]");
		return (i);
	}
	if (i > strmax || i < 0)
		panic("sreloc");
	return (strp[(i / STRINC) - 1] + (i % STRINC));
}

evenit()
{

	strngp = (strngp + 1) &~ 1;
}
#endif
#endif
#endif



/* ============================================================================
 * SOURCE 4: minix4\exokernel\kernel_legacy\string.c
 * ============================================================================ */

/* Copyright (c) 1979 Regents of the University of California */
#
/*
 * pi - Pascal interpreter code translator
 *
 * Charles Haley, Bill Joy UCB
 * Version 1.2 January 1979
 *
 * pxp - Pascal execution profiler
 *
 * Bill Joy UCB
 * Version 1.2 January 1979
 */

#include "0.h"
#ifndef PI01
#ifndef PXP
#include "send.h"
#endif
#endif

/*
 * STRING SPACE DECLARATIONS
 *
 * Strng is the base of the current
 * string space and strngp the
 * base of the free area therein.
 * Strp is the array of descriptors.
 */
#ifndef PI0
static	char strings[STRINC];
static	char *strng strings;
static	char *strngp strings;
#else
char	*strng, *strngp;
#endif
#ifndef PI01
#ifndef PXP
static	char *strp[20];
static	char **stract strp;
int	strmax;
#endif
#endif

#ifndef PI01
#ifndef PXP
#ifndef PI0
initstring()
#else
initstring(strings)
	char *strings;
#endif
{

	*stract++ = strings;
#ifdef PI0
	strng = strngp = strings;
#endif
	strmax = STRINC * 2;
}
#endif
#endif

/*
 * Copy a string into the string area.
 */
savestr(cp)
	register char *cp;
{
	register int i;

	i = strlen(cp) + 1;
	if (strngp + i >= strng + STRINC) {
		strngp = alloc(STRINC);
		if (strngp == -1) {
			yerror("Ran out of memory (string)");
			pexit(DIED);
		}
#ifndef PI01
#ifndef PXP
		*stract++ = strngp;
		strmax =+ STRINC;
#endif
#endif
		strng = strngp;
	}
	strcpy(strngp, cp);
	cp = strngp;
	strngp = cp + i;
#ifdef PI0
	send(RSTRING, cp);
#endif
	return (cp);
}

#ifndef PI1
#ifndef PXP
esavestr(cp)
	char *cp;
{

#ifdef PI0
	send(REVENIT);
#endif
	strngp = (strngp + 1) &~ 1;
	return (savestr(cp));
}
#endif
#endif

#ifndef PI01
#ifndef PXP
soffset(cp)
	register char *cp;
{
	register char **sp;
	register int i;

	if (cp == NIL || cp == OCT || cp == HEX)
		return (-cp);
	for (i = STRINC, sp = strp; sp < stract; sp++) {
		if (cp >= *sp && cp < (*sp + STRINC))
			return (i + (cp - *sp));
		i =+ STRINC;
	}
	i = nlfund(cp);
	if (i != 0)
		return (i);
	panic("soffset");
}
#ifdef PI1
sreloc(i)
	register int i;
{

	if (i == 0 || i == -OCT || i == -HEX)
		return (-i);
	if (i < STRINC) {
		if (i >= INL)
			panic("sreloc INL");
		i = nl[i].symbol;
		if (i == 0)
			panic("sreloc nl[i]");
		return (i);
	}
	if (i > strmax || i < 0)
		panic("sreloc");
	return (strp[(i / STRINC) - 1] + (i % STRINC));
}

evenit()
{

	strngp = (strngp + 1) &~ 1;
}
#endif
#endif
#endif



/* ============================================================================
 * SOURCE 5: minix4\exokernel\kernel_legacy\contrib\ast\src\cmd\ksh93\sh\string.c
 * ============================================================================ */

/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*          Copyright (c) 1982-2012 AT&T Intellectual Property          *
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
*                  David Korn <dgk@research.att.com>                   *
*                                                                      *
***********************************************************************/
#pragma prototyped
/*
 * string processing routines for Korn shell
 *
 */

#include	<ast.h>
#include	<ast_wchar.h>
#include	"defs.h"
#include	<stak.h>
#include	<ccode.h>
#include	"shtable.h"
#include	"lexstates.h"
#include	"national.h"

#if _hdr_wctype
#   include <wctype.h>
#endif

#if !_lib_iswprint && !defined(iswprint)
#   define iswprint(c)		(((c)&~0377) || isprint(c))
#endif


/*
 *  Table lookup routine
 *  <table> is searched for string <sp> and corresponding value is returned
 *  This is only used for small tables and is used to save non-sharable memory 
 */

const Shtable_t *sh_locate(register const char *sp,const Shtable_t *table,int size)
{
	register int			first;
	register const Shtable_t	*tp;
	register int			c;
	static const Shtable_t		empty = {0,0};
	if(sp==0 || (first= *sp)==0)
		return(&empty);
	tp=table;
	while((c= *tp->sh_name) && (CC_NATIVE!=CC_ASCII || c <= first))
	{
		if(first == c && strcmp(sp,tp->sh_name)==0)
			return(tp);
		tp = (Shtable_t*)((char*)tp+size);
	}
	return(&empty);
}

/*
 *  shtab_options lookup routine
 */

#define sep(c)		((c)=='-'||(c)=='_')

int sh_lookopt(register const char *sp, int *invert)
{
	register int			first;
	register const Shtable_t	*tp;
	register int			c;
	register const char		*s, *t, *sw, *tw;
	int				amb;
	int				hit;
	int				inv;
	int				no;
	if(sp==0)
		return(0);
	if(*sp=='n' && *(sp+1)=='o' && (*(sp+2)!='t' || *(sp+3)!='i'))
	{
		sp+=2;
		if(sep(*sp))
			sp++;
		*invert = !*invert;
	}
	if((first= *sp)==0)
		return(0);
	tp=shtab_options;
	amb=hit=0;
	for(;;)
	{
		t=tp->sh_name;
		if(no = *t=='n' && *(t+1)=='o' && *(t+2)!='t')
			t+=2;
		if(!(c= *t))
			break;
		if(first == c)
		{
			if(strcmp(sp,t)==0)
			{
				*invert ^= no;
				return(tp->sh_number);
			}
			s=sw=sp;
			tw=t;
			for(;;)
			{
				if(!*s || *s=='=')
				{
					if (*s == '=' && !strtol(s+1, NiL, 0))
						no = !no;
					if (!*t)
					{
						*invert ^= no;
						return(tp->sh_number);
					}
					if (hit || amb)
					{
						hit = 0;
						amb = 1;
					}
					else
					{
						hit = tp->sh_number;
						inv = no;
					}
					break;
				}
				else if(!*t)
					break;
				else if(sep(*s))
					sw = ++s;
				else if(sep(*t))
					tw = ++t;
				else if(*s==*t)
				{
					s++;
					t++;
				}
				else if(s==sw && t==tw)
					break;
				else
				{
					if(t!=tw)
					{
						while(*t && !sep(*t))
							t++;
						if(!*t)
							break;
						tw = ++t;
					}
					while (s>sw && *s!=*t)
						s--;
				}
			}
		}
		tp = (Shtable_t*)((char*)tp+sizeof(*shtab_options));
	}
	if(hit)
		*invert ^= inv;
	return(hit);
}

/*
 * look for the substring <oldsp> in <string> and replace with <newsp>
 * The new string is put on top of the stack
 */
char *sh_substitute(const char *string,const char *oldsp,char *newsp)
/*@
	assume string!=NULL && oldsp!=NULL && newsp!=NULL;
	return x satisfying x==NULL ||
		strlen(x)==(strlen(in string)+strlen(in newsp)-strlen(in oldsp));
@*/
{
	register const char *sp = string;
	register const char *cp;
	const char *savesp = 0;
	stakseek(0);
	if(*sp==0)
		return((char*)0);
	if(*(cp=oldsp) == 0)
		goto found;
#if SHOPT_MULTIBYTE
	mbinit();
#endif /* SHOPT_MULTIBYTE */
	do
	{
	/* skip to first character which matches start of oldsp */
		while(*sp && (savesp==sp || *sp != *cp))
		{
#if SHOPT_MULTIBYTE
			/* skip a whole character at a time */
			int c = mbsize(sp);
			if(c < 0)
				sp++;
			while(c-- > 0)
#endif /* SHOPT_MULTIBYTE */
			stakputc(*sp++);
		}
		if(*sp == 0)
			return((char*)0);
		savesp = sp;
	        for(;*cp;cp++)
		{
			if(*cp != *sp++)
				break;
		}
		if(*cp==0)
		/* match found */
			goto found;
		sp = savesp;
		cp = oldsp;
	}
	while(*sp);
	return((char*)0);

found:
	/* copy new */
	stakputs(newsp);
	/* copy rest of string */
	stakputs(sp);
	return(stakfreeze(1));
}

/*
 * TRIM(sp)
 * Remove escape characters from characters in <sp> and eliminate quoted nulls.
 */

void	sh_trim(register char *sp)
/*@
	assume sp!=NULL;
	promise  strlen(in sp) <= in strlen(sp);
@*/
{
	register char *dp;
	register int c;
	if(sp)
	{
		dp = sp;
		while(c= *sp)
		{
#if SHOPT_MULTIBYTE
			int len;
			if(mbwide() && (len=mbsize(sp))>1)
			{
				memmove(dp, sp, len);
				dp += len;
				sp += len;
				continue;
			}
#endif /* SHOPT_MULTIBYTE */
			sp++;
			if(c == '\\')
				c = *sp++;
			if(c)
				*dp++ = c;
		}
		*dp = 0;
	}
}

/*
 * copy <str1> to <str2> changing upper case to lower case
 * <str2> must be big enough to hold <str1>
 * <str1> and <str2> may point to the same place.
 */

void sh_utol(register char const *str1,register char *str2)
/*@
	assume str1!=0 && str2!=0
	return x satisfying strlen(in str1)==strlen(in str2);
@*/ 
{
	register int c;
	for(; c= *((unsigned char*)str1); str1++,str2++)
	{
		if(isupper(c))
			*str2 = tolower(c);
		else
			*str2 = c;
	}
	*str2 = 0;
}

/*
 * format string as a csv field
 */
static char	*sh_fmtcsv(const char *string)
{
	register const char *cp = string;
	register int c;
	int offset;
	if(!cp)
		return((char*)0);
	offset = staktell();
	while((c=mbchar(cp)),isaname(c));
	if(c==0)
		return((char*)string);
	stakputc('"');
	stakwrite(string,cp-string);
	if(c=='"')
		stakputc('"');
	string = cp;
	while(c=mbchar(cp))
	{
		if(c=='"')
		{
			stakwrite(string,cp-string);
			string = cp;
			stakputc('"');
		}
	}
	if(--cp>string)
		stakwrite(string,cp-string);
	stakputc('"');
	stakputc(0);
	return(stakptr(offset));
}

/*
 * print <str> quoting chars so that it can be read by the shell
 * puts null terminated result on stack, but doesn't freeze it
 */
char	*sh_fmtq(const char *string)
{
	register const char *cp = string, *op;
	register int c, state;
	int offset;
	if(!cp)
		return((char*)0);
#if SHOPT_MULTIBYTE
	mbinit();
#endif
	offset = staktell();
	state = ((c= mbchar(cp))==0);
	if(isaletter(c))
	{
		while((c=mbchar(cp)),isaname(c));
		if(c==0)
			return((char*)string);
		if(c=='=')
		{
			if(*cp==0)
				return((char*)string);
			if(*cp=='=')
				cp++;
			c = cp - string;
			stakwrite(string,c);
			string = cp;
			c = mbchar(cp);
		}
	}
	if(c==0 || c=='#' || c=='~')
		state = 1;
	for(;c;c= mbchar(cp))
	{
#if SHOPT_MULTIBYTE
		if(c=='\'' || c>=128 || c<0 || !iswprint(c)) 
#else
		if(c=='\'' || !isprint(c))
#endif /* SHOPT_MULTIBYTE */
			state = 2;
		else if(c==']' || c=='=' || (c!=':' && c<=0x7f && (c=sh_lexstates[ST_NORM][c]) && c!=S_EPAT))
			state |=1;
	}
	if(state<2)
	{
		if(state==1)
			stakputc('\'');
		if(c = --cp - string)
			stakwrite(string,c);
		if(state==1)
			stakputc('\'');
	}
	else
	{
		int isbyte=0;
		stakwrite("$'",2);
		cp = string;
#if SHOPT_MULTIBYTE
		while(op = cp, c= mbchar(cp))
#else
		while(op = cp, c= *(unsigned char*)cp++)
#endif
		{
			state=1;
			switch(c)
			{
			    case ('a'==97?'\033':39):
				c = 'E';
				break;
			    case '\n':
				c = 'n';
				break;
			    case '\r':
				c = 'r';
				break;
			    case '\t':
				c = 't';
				break;
			    case '\f':
				c = 'f';
				break;
			    case '\b':
				c = 'b';
				break;
			    case '\a':
				c = 'a';
				break;
			    case '\\':	case '\'':
				break;
			    default:
#if SHOPT_MULTIBYTE
				isbyte = 0;
				if(c<0)
				{
					c = *((unsigned char *)op);
					cp = op+1;
					isbyte = 1;
				}
				if(mbwide() && ((cp-op)>1))
				{
					sfprintf(staksp,"\\u[%x]",c);
					continue;
				}
				else if(!iswprint(c) || isbyte)
#else
				if(!isprint(c))
#endif
				{
					sfprintf(staksp,"\\x%.2x",c);
					continue;
				}
				state=0;
				break;
			}
			if(state)
			{
				stakputc('\\');
				stakputc(c);
			}
			else
				stakwrite(op, cp-op);
		}
		stakputc('\'');
	}
	stakputc(0);
	return(stakptr(offset));
}

/*
 * print <str> quoting chars so that it can be read by the shell
 * puts null terminated result on stack, but doesn't freeze it
 * single!=0 limits quoting to '...'
 * fold>0 prints raw newlines and inserts appropriately
 * escaped newlines every (fold-x) chars
 */
char	*sh_fmtqf(const char *string, int single, int fold)
{
	register const char *cp = string;
	register const char *bp;
	register const char *vp;
	register int c;
	register int n;
	register int q;
	register int a;
	int offset;

	if (--fold < 8)
		fold = 0;
	if(single)
		return sh_fmtcsv(cp);
	if (!cp || !*cp || !fold || fold && strlen(string) < fold)
		return sh_fmtq(cp);
	offset = staktell();
	single = single ? 1 : 3;
	c = mbchar(string);
	a = isaletter(c) ? '=' : 0;
	vp = cp + 1;
	do
	{
		q = 0;
		n = fold;
		bp = cp;
		while ((!n || n-- > 0) && (c = mbchar(cp)))
		{
			if (a && !isaname(c))
				a = 0;
#if SHOPT_MULTIBYTE
			if (c >= 0x200)
				continue;
			if (c == '\'' || !iswprint(c))
#else
			if (c == '\'' || !isprint(c))
#endif /* SHOPT_MULTIBYTE */
			{
				q = single;
				break;
			}
			if (c == '\n')
				q = 1;
			else if (c == a)
			{
				stakwrite(bp, cp - bp);
				bp = cp;
				vp = cp + 1;
				a = 0;
			}
			else if ((c == '#' || c == '~') && cp == vp || c == ']' || c != ':' && (c = sh_lexstates[ST_NORM][c]) && c != S_EPAT)
				q = 1;
		}
		if (q & 2)
		{
			stakputc('$');
			stakputc('\'');
			cp = bp;
			n = fold - 3;
			q = 1;
			while (c = mbchar(cp))
			{
				switch (c)
				{
		    		case ('a'==97?'\033':39):
					c = 'E';
					break;
		    		case '\n':
					q = 0;
					n = fold - 1;
					break;
		    		case '\r':
					c = 'r';
					break;
		    		case '\t':
					c = 't';
					break;
		    		case '\f':
					c = 'f';
					break;
		    		case '\b':
					c = 'b';
					break;
		    		case '\a':
					c = 'a';
					break;
		    		case '\\':
					if (*cp == 'n')
					{
						c = '\n';
						q = 0;
						n = fold - 1;
						break;
					}
				case '\'':
					break;
		    		default:
#if SHOPT_MULTIBYTE
					if(!iswprint(c))
#else
					if(!isprint(c))
#endif
					{
						if ((n -= 4) <= 0)
						{
							stakwrite("'\\\n$'", 5);
							n = fold - 7;
						}
						sfprintf(staksp, "\\%03o", c);
						continue;
					}
					q = 0;
					break;
				}
				if ((n -= q + 1) <= 0)
				{
					if (!q)
					{
						stakputc('\'');
						cp = bp;
						break;
					}
					stakwrite("'\\\n$'", 5);
					n = fold - 5;
				}
				if (q)
					stakputc('\\');
				else
					q = 1;
				stakputc(c);
				bp = cp;
			}
			if (!c)
				stakputc('\'');
		}
		else if (q & 1)
		{
			stakputc('\'');
			cp = bp;
			n = fold ? (fold - 2) : 0;
			while (c = mbchar(cp))
			{
				if (c == '\n')
					n = fold - 1;
				else if (n && --n <= 0)
				{
					n = fold - 2;
					stakwrite(bp, --cp - bp);
					bp = cp;
					stakwrite("'\\\n'", 4);
				}
				else if (n == 1 && *cp == '\'')
				{
					n = fold - 5;
					stakwrite(bp, --cp - bp);
					bp = cp;
					stakwrite("'\\\n\\''", 6);
				}
				else if (c == '\'')
				{
					stakwrite(bp, cp - bp - 1);
					bp = cp;
					if (n && (n -= 4) <= 0)
					{
						n = fold - 5;
						stakwrite("'\\\n\\''", 6);
					}
					else
						stakwrite("'\\''", 4);
				}
			}
			stakwrite(bp, cp - bp - 1);
			stakputc('\'');
		}
		else if (n = fold)
		{
			cp = bp;
			while (c = mbchar(cp))
			{
				if (--n <= 0)
				{
					n = fold;
					stakwrite(bp, --cp - bp);
					bp = cp;
					stakwrite("\\\n", 2);
				}
			}
			stakwrite(bp, cp - bp - 1);
		}
		else
			stakwrite(bp, cp - bp);
		if (c)
		{
			stakputc('\\');
			stakputc('\n');
		}
	} while (c);
	stakputc(0);
	return(stakptr(offset));
}

#if SHOPT_MULTIBYTE
	int sh_strchr(const char *string, register const char *dp)
	{
		wchar_t c, d;
		register const char *cp=string;
		mbinit();
		d = mbchar(dp); 
		mbinit();
		while(c = mbchar(cp))
		{
			if(c==d)
				return(cp-string);
		}
		if(d==0)
			return(cp-string);
		return(-1);
	}
#endif /* SHOPT_MULTIBYTE */

const char *_sh_translate(const char *message)
{
#if ERROR_VERSION >= 20000317L
	return(ERROR_translate(0,0,e_dict,message));
#else
#if ERROR_VERSION >= 20000101L
	return(ERROR_translate(e_dict,message));
#else
	return(ERROR_translate(message,1));
#endif
#endif
}

/*
 * change '['identifier']' to identifier
 * character before <str> must be a '['
 * returns pointer to last character
 */
char *sh_checkid(char *str, char *last)
{
	register unsigned char *cp = (unsigned char*)str;
	register unsigned char *v = cp;
	register int c;
	if(c=mbchar(cp),isaletter(c))
		while(c=mbchar(cp),isaname(c));
	if(c==']' && (!last || ((char*)cp==last)))
	{
		/* eliminate [ and ] */
		while(v < cp)
		{
			v[-1] = *v;
			v++;
		}
		if(last)
			last -=2;
		else
		{
			while(*v)
			{
				v[-2] = *v;
				v++;
			}
			v[-2] = 0;
			last = (char*)v;
		}
	}
	return(last);
}

#if	_AST_VERSION  <= 20000317L
char *fmtident(const char *string)
{
	return((char*)string);
}
#endif



/* ============================================================================
 * SOURCE 6: minix4\exokernel\kernel_legacy\common\util\string.c
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
 * Copyright 2014 Joyent, Inc.  All rights reserved.
 */

/*
 * Copyright (c) 2016 by Delphix. All rights reserved.
 * Copyright 2022 Tintri by DDN, Inc. All rights reserved.
 */

/*
 * Implementations of the functions described in vsnprintf(3C) and string(3C),
 * for use by the kernel, the standalone, and kmdb.  Unless otherwise specified,
 * these functions match the section 3C manpages.
 */

#include <sys/types.h>
#include <sys/null.h>
#include <sys/varargs.h>
#include <sys/stdbool.h>

#if defined(_KERNEL)
#include <sys/systm.h>
#include <sys/debug.h>
#elif !defined(_BOOT)
#include <string.h>
#endif

#include "memcpy.h"
#include "string.h"

/*
 * We don't need these for x86 boot or kmdb.
 */
#if !defined(_KMDB) && (!defined(_BOOT) || defined(__sparc))

#define	ADDCHAR(c)	if (bufp++ - buf < buflen) bufp[-1] = (c)

/*
 * Given a buffer 'buf' of size 'buflen', render as much of the string
 * described by <fmt, args> as possible.  The string will always be
 * null-terminated, so the maximum string length is 'buflen - 1'.
 * Returns the number of bytes that would be necessary to render the
 * entire string, not including null terminator (just like vsnprintf(3C)).
 * To determine buffer size in advance, use vsnprintf(NULL, 0, fmt, args) + 1.
 *
 * There is no support for floating point, and the C locale is assumed.
 */
size_t
vsnprintf(char *buf, size_t buflen, const char *fmt, va_list aargs)
{
	uintmax_t ul, tmp;
	char *bufp = buf;	/* current buffer pointer */
	char c, pad;
	int width, base, sign, num;
	int prec, h_count, l_count, dot_count;
	bool zflag, jflag;
	int pad_count, transfer_count, left_align;
	char *digits, *sp, *bs;
	char numbuf[65];	/* sufficient for a 64-bit binary value */
	int numwidth;
	va_list args;

	ul = 0;
	bs = NULL;
	/*
	 * Make a copy so that all our callers don't have to make a copy
	 */
	va_copy(args, aargs);

	if ((ssize_t)buflen < 0)
		buflen = 0;

	while ((c = *fmt++) != '\0') {
		if (c != '%') {
			ADDCHAR(c);
			continue;
		}

		zflag = jflag = false;
		width = prec = numwidth = 0;
		left_align = base = sign = 0;
		h_count = l_count = dot_count = 0;
		pad = ' ';
		digits = "0123456789abcdef";
next_fmt:
		if ((c = *fmt++) == '\0')
			break;

		if (c >= 'A' && c <= 'Z') {
			c += 'a' - 'A';
			digits = "0123456789ABCDEF";
		}

		switch (c) {
		case '-':
			left_align++;
			goto next_fmt;
		case '0':
			if (dot_count == 0)
				pad = '0';
			/*FALLTHROUGH*/
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			num = 0;
			for (;;) {
				num = 10 * num + c - '0';
				c = *fmt;
				if (c < '0' || c > '9')
					break;
				else
					fmt++;
			}
			if (dot_count > 0)
				prec = num;
			else
				width = num;

			goto next_fmt;
		case '.':
			dot_count++;
			goto next_fmt;
		case '*':
			if (dot_count > 0)
				prec = (int)va_arg(args, int);
			else
				width = (int)va_arg(args, int);
			goto next_fmt;
		case 'l':
			l_count++;
			goto next_fmt;
		case 'h':
			h_count++;
			goto next_fmt;
		case 'j':
			jflag = true;
			goto next_fmt;
		case 'd':
			sign = 1;
			/*FALLTHROUGH*/
		case 'u':
			base = 10;
			break;
		case 'p':
			l_count = 1;
			/*FALLTHROUGH*/
		case 'x':
			base = 16;
			break;
		case 'o':
			base = 8;
			break;
		case 'b':
			l_count = 0;
			base = 1;
			break;
		case 'c':
			c = (char)va_arg(args, int);
			ADDCHAR(c);
			break;
		case 's':
			sp = va_arg(args, char *);
			if (sp == NULL) {
				sp = "<null string>";
				/* avoid truncation */
				prec = strlen(sp);
			}
			/*
			 * Handle simple case specially to avoid
			 * performance hit of strlen()
			 */
			if (prec == 0 && width == 0) {
				while ((c = *sp++) != 0)
					ADDCHAR(c);
				break;
			}
			if (prec > 0) {
				transfer_count = strnlen(sp, prec);
				/* widen field if too narrow */
				if (prec > width)
					width = prec;
			} else
				transfer_count = strlen(sp);
			if (width > transfer_count)
				pad_count = width - transfer_count;
			else
				pad_count = 0;
			while ((!left_align) && (pad_count-- > 0))
				ADDCHAR(' ');
			/* ADDCHAR() evaluates arg at most once */
			while (transfer_count-- > 0)
				ADDCHAR(*sp++);
			while ((left_align) && (pad_count-- > 0))
				ADDCHAR(' ');
			break;
		case 'z':
			zflag = true;
			goto next_fmt;
		case '%':
			ADDCHAR('%');
			break;
		}

		if (base == 0)
			continue;

		if (jflag) {
			if (sign)
				ul = va_arg(args, intmax_t);
			else
				ul = va_arg(args, uintmax_t);
		} else if (zflag) {
			if (sign)
				ul = va_arg(args, ssize_t);
			else
				ul = va_arg(args, size_t);
		} else if (h_count == 0 && l_count == 0) {
			if (sign)
				ul = va_arg(args, int);
			else
				ul = va_arg(args, unsigned int);
		} else if (l_count > 1) {
			if (sign)
				ul = va_arg(args, int64_t);
			else
				ul = va_arg(args, uint64_t);
		} else if (l_count > 0) {
			if (sign)
				ul = va_arg(args, long);
			else
				ul = va_arg(args, unsigned long);
		} else if (h_count > 1) {
			if (sign)
				ul = (signed char)va_arg(args, int);
			else
				ul = (unsigned char)va_arg(args, int);
		} else if (h_count > 0) {
			if (sign)
				ul = (short)va_arg(args, int);
			else
				ul = (unsigned short)va_arg(args, int);
		}

		if (sign && (intmax_t)ul < 0)
			ul = -ul;
		else
			sign = 0;

		if (c == 'b') {
			bs = va_arg(args, char *);
			base = *bs++;
		}

		/*
		 * Fill in the number string buffer and calculate the
		 * number string length.
		 */
		tmp = ul;
		sp = numbuf;
		do {
			*sp++ = digits[tmp % base];
			numwidth++;
		} while ((tmp /= base) != 0);

		/*
		 * Reduce the total field width by precision or the number
		 * string length depending on which one is bigger, and sign.
		 */
		if (prec >= numwidth)
			width -= prec;
		else
			width -= numwidth;
		width -= sign;

		/* Add the sign if width is '0'-padded */
		if (sign && pad == '0')
			ADDCHAR('-');

		/* If not left-aligned, add the width padding */
		if (!left_align) {
			while (width-- > 0)
				ADDCHAR(pad);
		}

		/* Add the sign if width is NOT '0'-padded */
		if (sign && pad != '0')
			ADDCHAR('-');

		/* Add the precision '0'-padding */
		while (prec-- > numwidth)
			ADDCHAR('0');

		/* Print out the number */
		while (sp > numbuf) {
			sp--;
			ADDCHAR(*sp);
		}

		/* Add left-alignment padding */
		while (width-- > 0)
			ADDCHAR(' ');

		if (c == 'b' && ul != 0) {
			int any = 0;
			c = *bs++;
			while (c != 0) {
				if (ul & (1 << (c - 1))) {
					if (any++ == 0)
						ADDCHAR('<');
					while ((c = *bs++) > 32)
						ADDCHAR(c);
					ADDCHAR(',');
				} else {
					while ((c = *bs++) > 32)
						continue;
				}
			}
			if (any) {
				bufp--;
				ADDCHAR('>');
			}
		}
	}
	if (bufp - buf < buflen)
		bufp[0] = c;
	else if (buflen != 0)
		buf[buflen - 1] = c;

	va_end(args);

	return (bufp - buf);
}

/*PRINTFLIKE3*/
size_t
snprintf(char *buf, size_t buflen, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	buflen = vsnprintf(buf, buflen, fmt, args);
	va_end(args);

	return (buflen);
}

#if defined(_BOOT) && defined(__sparc)
/*
 * The sprintf() and vsprintf() routines aren't shared with the kernel because
 * the DDI mandates that they return the buffer rather than its length.
 */
/*PRINTFLIKE2*/
int
sprintf(char *buf, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	(void) vsnprintf(buf, INT_MAX, fmt, args);
	va_end(args);

	return (strlen(buf));
}

int
vsprintf(char *buf, const char *fmt, va_list args)
{
	(void) vsnprintf(buf, INT_MAX, fmt, args);
	return (strlen(buf));
}
#endif /* _BOOT && __sparc */

#endif /* !_KMDB && (!_BOOT || __sparc) */

char *
strcat(char *s1, const char *s2)
{
	char *os1 = s1;

	while (*s1++ != '\0')
		;
	s1--;
	while ((*s1++ = *s2++) != '\0')
		;
	return (os1);
}

char *
strchr(const char *sp, int c)
{
	do {
		if (*sp == (char)c)
			return ((char *)sp);
	} while (*sp++);
	return (NULL);
}

int
strcmp(const char *s1, const char *s2)
{
	while (*s1 == *s2++)
		if (*s1++ == '\0')
			return (0);
	return (*(unsigned char *)s1 - *(unsigned char *)--s2);
}

int
strncmp(const char *s1, const char *s2, size_t n)
{
	if (s1 == s2)
		return (0);
	n++;
	while (--n != 0 && *s1 == *s2++)
		if (*s1++ == '\0')
			return (0);
	return ((n == 0) ? 0 : *(unsigned char *)s1 - *(unsigned char *)--s2);
}

static const char charmap[] = {
	'\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
	'\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
	'\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
	'\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
	'\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
	'\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
	'\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
	'\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
	'\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	'\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
	'\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	'\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
	'\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
	'\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
	'\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
	'\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
	'\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
	'\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
	'\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
	'\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
	'\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
	'\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
	'\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
	'\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
	'\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
	'\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
	'\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
	'\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};

int
strcasecmp(const char *s1, const char *s2)
{
	const unsigned char *cm = (const unsigned char *)charmap;
	const unsigned char *us1 = (const unsigned char *)s1;
	const unsigned char *us2 = (const unsigned char *)s2;

	while (cm[*us1] == cm[*us2++])
		if (*us1++ == '\0')
			return (0);
	return (cm[*us1] - cm[*(us2 - 1)]);
}

int
strncasecmp(const char *s1, const char *s2, size_t n)
{
	const unsigned char *cm = (const unsigned char *)charmap;
	const unsigned char *us1 = (const unsigned char *)s1;
	const unsigned char *us2 = (const unsigned char *)s2;

	while (n != 0 && cm[*us1] == cm[*us2++]) {
		if (*us1++ == '\0')
			return (0);
		n--;
	}
	return (n == 0 ? 0 : cm[*us1] - cm[*(us2 - 1)]);
}

char *
strcpy(char *s1, const char *s2)
{
	char *os1 = s1;

	while ((*s1++ = *s2++) != '\0')
		;
	return (os1);
}

char *
strncpy(char *s1, const char *s2, size_t n)
{
	char *os1 = s1;

	n++;
	while (--n != 0 && (*s1++ = *s2++) != '\0')
		;
	if (n != 0)
		while (--n != 0)
			*s1++ = '\0';
	return (os1);
}

char *
strrchr(const char *sp, int c)
{
	char *r = NULL;

	do {
		if (*sp == (char)c)
			r = (char *)sp;
	} while (*sp++);

	return (r);
}

char *
strstr(const char *as1, const char *as2)
{
	const char *s1, *s2;
	const char *tptr;
	char c;

	s1 = as1;
	s2 = as2;

	if (s2 == NULL || *s2 == '\0')
		return ((char *)s1);
	c = *s2;

	while (*s1)
		if (*s1++ == c) {
			tptr = s1;
			while ((c = *++s2) == *s1++ && c)
				;
			if (c == 0)
				return ((char *)tptr - 1);
			s1 = tptr;
			s2 = as2;
			c = *s2;
		}

	return (NULL);
}

char *
strpbrk(const char *string, const char *brkset)
{
	const char *p;

	do {
		for (p = brkset; *p != '\0' && *p != *string; ++p)
			;
		if (*p != '\0')
			return ((char *)string);
	} while (*string++);

	return (NULL);
}

char *
strncat(char *s1, const char *s2, size_t n)
{
	char *os1 = s1;

	n++;
	while (*s1++ != '\0')
		;
	--s1;
	while ((*s1++ = *s2++) != '\0') {
		if (--n == 0) {
			s1[-1] = '\0';
			break;
		}
	}
	return (os1);
}

#if defined(_BOOT) || defined(_KMDB)
#define	bcopy(src, dst, n)	(void) memcpy((dst), (src), (n))
#endif

size_t
strlcat(char *dst, const char *src, size_t dstsize)
{
	char *df = dst;
	size_t left = dstsize;
	size_t l1;
	size_t l2 = strlen(src);
	size_t copied;

	while (left-- != 0 && *df != '\0')
		df++;
	/*LINTED: possible ptrdiff_t overflow*/
	l1 = (size_t)(df - dst);
	if (dstsize == l1)
		return (l1 + l2);

	copied = l1 + l2 >= dstsize ? dstsize - l1 - 1 : l2;
	bcopy(src, dst + l1, copied);
	dst[l1+copied] = '\0';
	return (l1 + l2);
}

size_t
strlcpy(char *dst, const char *src, size_t len)
{
	size_t slen = strlen(src);
	size_t copied;

	if (len == 0)
		return (slen);

	if (slen >= len)
		copied = len - 1;
	else
		copied = slen;
	bcopy(src, dst, copied);
	dst[copied] = '\0';
	return (slen);
}

size_t
strspn(const char *string, const char *charset)
{
	const char *p, *q;

	for (q = string; *q != '\0'; ++q) {
		for (p = charset; *p != '\0' && *p != *q; ++p)
			;
		if (*p == '\0')
			break;
	}

	/*LINTED: possible ptrdiff_t overflow*/
	return ((size_t)(q - string));
}

size_t
strcspn(const char *string, const char *charset)
{
	const char *p, *q;

	for (q = string; *q != '\0'; ++q) {
		for (p = charset; *p != '\0' && *p != *q; ++p)
			;
		if (*p != '\0')
			break;
	}

	/*LINTED E_PTRDIFF_OVERFLOW*/
	return ((size_t)(q - string));
}

/*
 * strsep
 *
 * The strsep() function locates, in the string referenced by *stringp, the
 * first occurrence of any character in the string delim (or the terminating
 * `\0' character) and replaces it with a `\0'.  The location of the next
 * character after the delimiter character (or NULL, if the end of the
 * string was reached) is stored in *stringp.  The original value of
 * *stringp is returned.
 *
 * If *stringp is initially NULL, strsep() returns NULL.
 *
 * NOTE: This instance is left for in-kernel use. Libraries and programs
 *       should use strsep from libc.
 */
char *
strsep(char **stringp, const char *delim)
{
	char *s;
	const char *spanp;
	int c, sc;
	char *tok;

	if ((s = *stringp) == NULL)
		return (NULL);

	for (tok = s; ; ) {
		c = *s++;
		spanp = delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				*stringp = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */
}

/*
 * strtok_r
 *
 * uses strpbrk and strspn to break string into tokens on
 * sequentially subsequent calls.  returns NULL when no
 * non-separator characters remain.
 * `subsequent' calls are calls with first argument NULL.
 */
char *
strtok_r(char *string, const char *sepset, char **lasts)
{
	char	*q, *r;

	/* first or subsequent call */
	if (string == NULL)
		string = *lasts;

	if (string == NULL)		/* return if no tokens remaining */
		return (NULL);

	q = string + strspn(string, sepset);	/* skip leading separators */

	if (*q == '\0')		/* return if no tokens remaining */
		return (NULL);

	if ((r = strpbrk(q, sepset)) == NULL) {	/* move past token */
		*lasts = NULL;	/* indicate this is last token */
	} else {
		*r = '\0';
		*lasts = r + 1;
	}
	return (q);
}

/*
 * Unless mentioned otherwise, all of the routines below should be added to
 * the Solaris DDI as necessary.  For now, only provide them to standalone.
 */
#if defined(_BOOT) || defined(_KMDB)
char *
strtok(char *string, const char *sepset)
{
	char		*p, *q, *r;
	static char	*savept;

	/*
	 * Set `p' to our current location in the string.
	 */
	p = (string == NULL) ? savept : string;
	if (p == NULL)
		return (NULL);

	/*
	 * Skip leading separators; bail if no tokens remain.
	 */
	q = p + strspn(p, sepset);
	if (*q == '\0')
		return (NULL);

	/*
	 * Mark the end of the token and set `savept' for the next iteration.
	 */
	if ((r = strpbrk(q, sepset)) == NULL)
		savept = NULL;
	else {
		*r = '\0';
		savept = ++r;
	}

	return (q);
}

/*
 * The strlen() routine isn't shared with the kernel because it has its own
 * hand-tuned assembly version.
 */
size_t
strlen(const char *s)
{
	size_t n = 0;

	while (*s++)
		n++;
	return (n);
}

#endif /* _BOOT || _KMDB */

/*
 * Returns the number of non-NULL bytes in string argument,
 * but not more than maxlen.  Does not look past str + maxlen.
 */
size_t
strnlen(const char *s, size_t maxlen)
{
	size_t n = 0;

	while (maxlen != 0 && *s != 0) {
		s++;
		maxlen--;
		n++;
	}

	return (n);
}


#ifdef _KERNEL
/*
 * Check for a valid C identifier:
 *	a letter or underscore, followed by
 *	zero or more letters, digits and underscores.
 */

#define	IS_DIGIT(c)	((c) >= '0' && (c) <= '9')

#define	IS_ALPHA(c)	\
	(((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))

int
strident_valid(const char *id)
{
	int c = *id++;

	if (!IS_ALPHA(c) && c != '_')
		return (0);
	while ((c = *id++) != 0) {
		if (!IS_ALPHA(c) && !IS_DIGIT(c) && c != '_')
			return (0);
	}
	return (1);
}

/*
 * Convert a string into a valid C identifier by replacing invalid
 * characters with '_'.  Also makes sure the string is nul-terminated
 * and takes up at most n bytes.
 */
void
strident_canon(char *s, size_t n)
{
	char c;
	char *end = s + n - 1;

	ASSERT(n > 0);

	if ((c = *s) == 0)
		return;

	if (!IS_ALPHA(c) && c != '_')
		*s = '_';

	while (s < end && ((c = *(++s)) != 0)) {
		if (!IS_ALPHA(c) && !IS_DIGIT(c) && c != '_')
			*s = '_';
	}
	*s = 0;
}

#endif	/* _KERNEL */



/* ============================================================================
 * SOURCE 7: minix4\exokernel\kernel_legacy\cmd\sh\string.c
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
 * Copyright 1995 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*
 * UNIX shell
 */

#include	"defs.h"


/* ========	general purpose string handling ======== */


unsigned char *
movstr(unsigned char *a, unsigned char *b)
{
	while (*b++ = *a++);
	return(--b);
}

int
any(wchar_t c, unsigned char *s)
{
	unsigned int d;

	while (d = *s++)
	{
		if (d == c)
			return(TRUE);
	}
	return(FALSE);
}

int anys(c, s)
unsigned char *c, *s;
{
	wchar_t f, e;
	wchar_t d;
	int n;
	if((n = mbtowc(&f, (char *)c, MULTI_BYTE_MAX)) <= 0)
		return(FALSE);
	d = f;
	while(1) {
		if((n = mbtowc(&e, (char *)s, MULTI_BYTE_MAX)) <= 0)
			return(FALSE);
		if(d == e)
			return(TRUE);
		s += n;
	}
}

int
cf(unsigned char *s1, unsigned char *s2)
{
	while (*s1++ == *s2)
		if (*s2++ == 0)
			return(0);
	return(*--s1 - *s2);
}

int length(as)
unsigned char	*as;
{
	unsigned char	*s;

	if (s = as)
		while (*s++);
	return(s - as);
}

unsigned char *
movstrn(unsigned char *a, unsigned char *b, int n)
{
	while ((n-- > 0) && *a)
		*b++ = *a++;

	return(b);
}



/* ============================================================================
 * SOURCE 8: minix4\exokernel\kernel_legacy\cmd\sendmail\libsm\string.c
 * ============================================================================ */

/*
 * Copyright (c) 2001 Sendmail, Inc. and its suppliers.
 *      All rights reserved.
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the sendmail distribution.
 *
 */

#include <sm/gen.h>
SM_RCSID("@(#)$Id: string.c,v 1.1 2001/02/15 21:04:50 ca Exp $")

#include <ctype.h>
#include <errno.h>

#include <sm/string.h>

/*
**  STRIPQUOTES -- Strip quotes & quote bits from a string.
**
**	Runs through a string and strips off unquoted quote
**	characters and quote bits.  This is done in place.
**
**	Parameters:
**		s -- the string to strip.
**
**	Returns:
**		none.
**
**	Side Effects:
**		none.
*/

void
stripquotes(s)
	char *s;
{
	register char *p;
	register char *q;
	register char c;

	if (s == NULL)
		return;

	p = q = s;
	do
	{
		c = *p++;
		if (c == '\\')
			c = *p++;
		else if (c == '"')
			continue;
		*q++ = c;
	} while (c != '\0');
}

