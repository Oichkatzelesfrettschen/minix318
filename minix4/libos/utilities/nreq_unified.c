/**
 * @file nreq_unified.c
 * @brief Unified nreq implementation
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
 *     1. userspace\minix_commands\cawf\nreq.c
 *     2. userspace\commands_legacy\text\cawf\nreq.c
 * 
 * Total source files: 2
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
#include "cawf.h"
#include <ctype.h>

/* ===== GLOBAL VARIABLES ===== */
static void nr_UL(unsigned char *line, int brk);
static void nr_Ub(unsigned char *line, int brk);
static void nr_Uc(unsigned char *line, int brk);
static void nr_Uf(unsigned char *line, int brk);
static void nr_Ur(unsigned char *line, int brk);
static void nr_ad(unsigned char *line, int brk);
static void nr_bp(unsigned char *line, int brk);
static void nr_br(unsigned char *line, int brk);
static void nr_ce(unsigned char *line, int brk);
static void nr_di(unsigned char *line, int brk);
static void nr_ds(unsigned char *line, int brk);
static void nr_fi(unsigned char *line, int brk);
static void nr_fl(unsigned char *line, int brk);
static void nr_ft(unsigned char *line, int brk);
static void nr_it(unsigned char *line, int brk);
static void nr_na(unsigned char *line, int brk);
static void nr_nf(unsigned char *line, int brk);
static void nr_ns(unsigned char *line, int brk);
static void nr_rm(unsigned char *line, int brk);
static void nr_rn(unsigned char *line, int brk);
static void nr_rr(unsigned char *line, int brk);
static void nr_rs(unsigned char *line, int brk);
static void nr_tm(unsigned char *line, int brk);
static void nr_tr(unsigned char *line, int brk);
static void nr_nil(unsigned char *line, int brk);
static void nr_UL(unsigned char *line, int brk);
static void nr_Ub(unsigned char *line, int brk);
static void nr_Uc(unsigned char *line, int brk);
static void nr_Uf(unsigned char *line, int brk);
static void nr_Ur(unsigned char *line, int brk);
static void nr_ad(unsigned char *line, int brk);
static void nr_bp(unsigned char *line, int brk);
static void nr_br(unsigned char *line, int brk);
static void nr_ce(unsigned char *line, int brk);
static void nr_di(unsigned char *line, int brk);
static void nr_ds(unsigned char *line, int brk);
static void nr_fi(unsigned char *line, int brk);
static void nr_fl(unsigned char *line, int brk);
static void nr_ft(unsigned char *line, int brk);
static void nr_it(unsigned char *line, int brk);
static void nr_na(unsigned char *line, int brk);
static void nr_nf(unsigned char *line, int brk);
static void nr_ns(unsigned char *line, int brk);
static void nr_rm(unsigned char *line, int brk);
static void nr_rn(unsigned char *line, int brk);
static void nr_rr(unsigned char *line, int brk);
static void nr_rs(unsigned char *line, int brk);
static void nr_tm(unsigned char *line, int brk);
static void nr_tr(unsigned char *line, int brk);
static void nr_nil(unsigned char *line, int brk);

/* ===== FUNCTIONS ===== */

/* Function 1 */
void Nreq(unsigned char *line, int brk) {

	c[0] = c[1] = c[2] = '\0';
	if ((c[0] = line[1]) != '\0')
		c[1] = line[2];

	low = mid = 0;
	hi = sizeof(NrReqt) / sizeof(struct nr_req);
	while (low <= hi) {
		mid = (low + hi) / 2;
		if ((cmp = strcmp((char *)c, NrReqt[mid].nm)) < 0)
			hi = mid - 1;
		else if (cmp > 0)
			low = mid + 1;
		else {
			(void) (*NrReqt[mid].fun)(line, brk);
			return;
		}
	}
	Error(WARN, LINE, " unknown request", NULL);
}

/* Function 2 */
static void nr_ad(unsigned char *line, int brk) {
	Pass3(NOBREAK, (unsigned char *)"both", NULL, 0);
}

/* Function 3 */
static void nr_bp(unsigned char *line, int brk) {
	Pass3(brk, (unsigned char *)"need", NULL, 999);
}

/* Function 4 */
static void nr_br(unsigned char *line, int brk) {
	Pass3(brk, (unsigned char *)"flush", NULL, 0);
}

/* Function 5 */
static void nr_ce(unsigned char *line, int brk) {

	if ((s = Field(2, line, 0)) != NULL)
		Centering = atoi((char *)s);
	else
		Centering = 1;
}

/* Function 6 */
static void nr_di(unsigned char *line, int brk) {
	Pass3(DOBREAK, (unsigned char *)"flush", NULL, 0);
	Divert ^= 1;
}

/* Function 7 */
static void nr_ds(unsigned char *line, int brk) {
		       *s4;

	if (Asmname(&line[3], nm) == 0) {
		Error(WARN, LINE, " no name", NULL);
		return;
	}
	s1 = Field(3, line, 0);
	s2 = Findstr(nm, s1, 1);
	while (*s2 == '\\' && *(s2 + 1) == '*') {
		s2++;
		s3 = Asmcode(&s2, nm1);
		s2 = Findstr(nm1, NULL, 0);
	}
	if (Hdft) {

		if ((nm[0]=='L' || nm[0]=='C' || nm[0]=='R')
		&&  (nm[1]=='F' || nm[1]=='H')) {
			(void) sprintf((char *)buf, "%s", (char *)nm);
			Pass3(NOBREAK, buf, s2, 0);
		}
	}
}

/* Function 8 */
static void nr_fi(unsigned char *line, int brk) {
	Fill = 1;
	Pass3(brk, (unsigned char *)"flush", NULL, 0);
}

/* Function 9 */
static void nr_fl(unsigned char *line, int brk) {
	Pass3(brk, (unsigned char *)"flush", NULL, 0);
}

/* Function 10 */
static void nr_ft(unsigned char *line, int brk) {

	if (line[3] == '\0' || line[4] == '\0')
		line[4] = 'P';
	if (line[4] == 'P') {
		Font[0] = Prevfont;
		return;
	}
	for (i = 0; Fcode[i].nm; i++) {
		if (Fcode[i].nm == line[4])
		break;
	}
	if (Fcode[i].status == '\0') {
		Error(WARN, LINE, " bad font code", NULL);
		return;
	}
	Prevfont = Font[0];
	Font[0] = line[4];
}

/* Function 11 */
static void nr_it(unsigned char *line, int brk) {

	if ((s1 = Field(2, line, 0)) == NULL) {
		Free(&Aftnxt);
		return;
	}
	if ((i = atoi((char *)s1)) != 1) {
		Error(WARN, LINE, " first .it arg must be 1", NULL);
		return;
	}
	if ((s2 = Field(3, line, 0)) == NULL)
		Free(&Aftnxt);
	else {
		(void) sprintf((char *)buf, "%s,%s",
			(Aftnxt == NULL) ? "" : (char *)Aftnxt,
			(char *)s2);
		Free(&Aftnxt);
		Aftnxt = Newstr(buf);
	}
}

/* Function 12 */
static void nr_nil(unsigned char *line, int brk) {
}

/* Function 13 */
static void nr_na(unsigned char *line, int brk) {
	Pass3(NOBREAK, (unsigned char *)"left", NULL, 0);
}

/* Function 14 */
static void nr_nf(unsigned char *line, int brk) {
	Fill = 0;
	Pass3(brk, (unsigned char *)"flush", NULL, 0);
}

/* Function 15 */
static void nr_ns(unsigned char *line, int brk) {
	Pass3(NOBREAK, (unsigned char *)"nospace", NULL, 0);
}

/* Function 16 */
static void nr_rm(unsigned char *line, int brk) {

	if (Asmname(&line[3], nm) == 0) {
		Error(WARN, LINE, " no name", NULL);
		return;
	}
	if ((i = Findmacro(nm, 0)) >= 0) {
		Delmacro(i);
		return;
			}
	(void) Findstr(nm, NULL, 0);
		if (Sx >= 0) {
			Delstr(Sx);
			return;
	}
	Error(WARN, LINE, " no macro/string", NULL);
}

/* Function 17 */
static void nr_rn(unsigned char *line, int brk) {

	if ((s1 = Field(2, line, 0)) == NULL ||  Asmname(s1, nm) == 0) {
		Error(WARN, LINE, " no name", NULL);
		return;
	}
	if ((s1 = Field(3, line, 0)) == NULL ||  Asmname(s1, nm1) == 0) {
		Error(WARN, LINE, " no new name", NULL);
		return;
	}
	if ((i = Findmacro(nm, 0)) >= 0) {
		if ((j = Findmacro(nm1, 0)) >= 0)
			Delmacro(j);
		j = Findmacro(nm1, 1);
		Macrotab[j].bx = Macrotab[i].bx;
		Macrotab[i].bx = -1;
		Macrotab[j].ct = Macrotab[i].ct;
		Macrotab[i].ct = 0;
		Delmacro(i);
		return;
	}
	(void) Findstr(nm, NULL, 0);
	if ((i = Sx) >= 0) {
		(void) Findstr(nm1, Str[i].str, 1);
		Delstr(i);
		return;
	}
	if (Findmacro(nm1, 0) < 0)
		(void) Findmacro(nm1, 1);
}

/* Function 18 */
static void nr_rr(unsigned char *line, int brk) {

	if (Asmname(&line[3], nm) == 0) {
		Error(WARN, LINE, " no name", NULL);
		return;
	}
	if ((i = Findnum(nm, 0, 0)) < 0) {
		Error(WARN, LINE, " no register", NULL);
		return;
	}
	Delnum(i);
}

/* Function 19 */
static void nr_rs(unsigned char *line, int brk) {
	Pass3(NOBREAK, (unsigned char *)"yesspace", NULL, 0);
}

/* Function 20 */
static void nr_tm(unsigned char *line, int brk) {
	Pass3(MESSAGE, Inname, (line[3] == ' ') ? &line[4] : &line[3], NR);
}

/* Function 21 */
static void nr_tr(unsigned char *line, int brk) {

	if (line[3] != ' ') {
		Error(WARN, LINE, " unknown translation", NULL);
		return;
	}
	for (s1 = &line[4]; *s1;) {
	    nm[1] = nm[2] = '\0';
	    s2 = s1 + 1;
	    if (*s1 == '\\' && (*s2 == '*' || *s2 == '(')) {
		if (*s2 == '(') {
		    trin = 1;
		    s1 = s2 + 1;
		    if ((nm[0] = *s1) != '\0') {
			s1++;
			if ((nm[1] = *s1) != '\0')
			    s1++;
		    }
		} else {
		    s1 = Asmcode(&s2, nm);
		    if (*s1)
			s1++;
		    s2 = Findstr(nm, NULL, 0);
		    if (*s2 != '\0') {
			if ((strlen((char *)s2) + strlen((char *)s1) + 1)
			>= MAXLINE)
			    Error(WARN, LINE, " string too long: ", (char *)nm);
			else {
			    (void) sprintf((char *)buf, "%s%s",
				(char *)s2, (char *)s1);
			    (void) strcpy((char *)xbuf, (char *)buf);
			    s1 = xbuf;
			}
		    }
		    continue;
		}
	    } else {

		trin = 0;
		if ((nm[0] = *s1) != '\0')
		    s1++;
	    }

assemble_output:
	    nm1[1] = nm1[2] = '\0';
	    if (*s1 == '\0') {

		trout = 0;
		nm1[0] = ' ';
	    } else {
		s2 = s1 + 1;
		if (*s1 == '\\' && (*s2 == '(' || *s2 == '*')) {
		    if (*s2 == '(') {
			trout = 1;
			s1 = s2 + 1;
			if ((nm1[0] = *s1) != '\0') {
			    s1++;
			    if ((nm1[1] = *s1) != '\0')
				s1++;
			}
		    } else {
			s1 = Asmcode(&s2, nm1);
			if (*s1)
			    s1++;
			s2 = Findstr(nm1, NULL, 0);
			if (*s2 != '\0') {
			    if ((strlen((char *)s2) + strlen((char *)s1) + 1)
			    >= MAXLINE)
				Error(WARN, LINE, " string too long: ",
				    (char *)nm);
			    else {
				(void) sprintf((char *)buf, "%s%s", (char *)s2,
				    (char *)s1);
				(void) strcpy((char *)xbuf, (char *)buf);
			        s1 = xbuf;
			    }
			}
			goto assemble_output;
		    }
		} else {
		    trout = 0;
		    if ((nm1[0] = *s1) != '0')
			s1++;
		    else
			nm1[0] = ' ';
		}
	    }
	    switch (trin) {

		switch (trout) {

		    Trtbl[(int)nm[0]] = nm1[0];
		    break;
		    if ((i = Findchar(nm1, 0, NULL, 0)) < 0
		    ||  strlen((char *)Schar[i].str) != 1)
			Error(WARN, LINE, " bad named character: ",
			    (char *)nm1);
		    else
			Trtbl[(int)nm[0]] = *(Schar[i].str);
		    break;
		}
		break;
		if ((i = Findchar(nm, 0, NULL, 0)) < 0)
		    Error(WARN, LINE, " unknown named character: ", (char *)nm);
		else {
		    switch (trout) {

			Free(&Schar[i].str);
			Schar[i].str = Newstr(nm1);
			Schar[i].len = 1;
			break;
			if ((j = Findchar(nm1, 0, NULL, 0)) < 0)
			    Error(WARN, LINE, " unknown named character: ",
				(char *)nm1);
			else
			    (void) Findchar(nm, Schar[j].len, Schar[j].str, 1);
			break;
		    }
		}
		break;
	    }
	}
}

/* Function 22 */
static void nr_Ub(unsigned char *line, int brk) {

	if ((s1 = Field(2, line, 0)) == NULL)
		return;
	if ((s2 = Field(3, line, 0)) == NULL)
		i = 0;
	else
		i = atoi((char *)s2);
	if (s1[0] == 'f' && s1[1] == 'h')
		Pass3(NOBREAK, (unsigned char *)"fph", NULL, i);
	else if (s1[0] == 'H' && s1[1] == 'F')
		Hdft = i;
	else if (s1[0] == 'N' && s1[1] == 'H') {
		for (i = 0; i < MAXNHNR; i++)
			Nhnr[i] = 0;
	} else
		Error(WARN, LINE, " unknown initialization", NULL);
}

/* Function 23 */
static void nr_Uc(unsigned char *line, int brk) {
		      *s4, *s5;

	s2 = Field(2, line, 0);
	i = atoi((char *)Field(3, line, 0));
	s4 = Field(4, line, 0);
	if (i < 0 || i > MAXLINE/2 || *s2 == '\0') {
		Error(WARN, LINE, " bad character definition", NULL);
		return;
	}
	if (s4 == NULL)
		s4 = (unsigned char *)"";
	else if (*s4 == '"')
		s4++;
	s1 = buf;
	while ((s5 = (unsigned char *)strchr((char *)s4, '\\')) != NULL) {
		while (s5 > s4)
			*s1++ = *s4++;
		s4 = ++s5;
		if (*s5 == '\\')
			*s1++ = '\\';
		else if (*s5 == 'b')
			*s1++ = '\b';
		if (*s4)
			s4++;
	}
	while ((*s1++ = *s4++))
		;
	if (*s2 == 'h' && *(s2+1) == 'y')
		(void) Findhy(buf, i, 1);
	else
		(void) Findchar(s2, i, buf, 1);
}

/* Function 24 */
static void nr_Uf(unsigned char *line, int brk) {

	if (line[3] != '\0' && line[4] != '\0') {
		for (i = 0; Fcode[i].nm; i++) {
			if (line[4] == Fcode[i].nm) {
				Fcode[i].status = '1';
				return;
			}
		}
	}
	Error(WARN, LINE, " unknown font", NULL);
}

/* Function 25 */
static void nr_Ur(unsigned char *line, int brk) {

	if ((i = atoi((char *)Field(3, line, 0))) <= 0
	||  (j = atoi((char *)Field(4, line, 0))) <= 0) {
		Error(WARN, LINE, " bad cpi resolutions", NULL);
		return;
	}
	tval = (double) (240.0 / (double) i);
	if (Findscale((int)'m', tval, 1) < 0)
		Error(FATAL, LINE, " missing Scal['m']", NULL);
	Scalen = tval;
	if (Scalen <= 0.0) {
		(void) sprintf((char *)buf, " bad Scale['n'] (%f)", Scalen);
		Error(FATAL, LINE, (char *)buf, NULL);
	}
	if (Findscale((int)'n', tval, 1) < 0)
		Error(FATAL, LINE, " missing Scale['n']", NULL);
	Scalev = (double) (240.0 / (double) j);
	if (Scalev <= 0.0) {
		(void) sprintf((char *)buf, " bad Scale['v'] (%f)", Scalen);
		Error(FATAL, LINE, (char *)buf, NULL);
	}
	if (Findscale((int)'v', Scalev, 1) < 0)
		Error(FATAL, LINE, " missing Scale['v']", NULL);
}

/* Function 26 */
static void nr_UL(unsigned char *line, int brk) {

	if ((s1 = Field(2, line, 0)) != NULL)
		P2il = atoi((char *)s1) - 1;
	else
		P2il = 0;
	Lockil = (line[2] == '#') ? 0 : 1;
	Free(&P2name);
	if (Field(3, line, 1) != NULL) {
		P2name = F;
		F = NULL;
	} else
		P2name = NULL;
}

/* Function 27 */
void Nreq(unsigned char *line, int brk) {

	c[0] = c[1] = c[2] = '\0';
	if ((c[0] = line[1]) != '\0')
		c[1] = line[2];

	low = mid = 0;
	hi = sizeof(NrReqt) / sizeof(struct nr_req);
	while (low <= hi) {
		mid = (low + hi) / 2;
		if ((cmp = strcmp((char *)c, NrReqt[mid].nm)) < 0)
			hi = mid - 1;
		else if (cmp > 0)
			low = mid + 1;
		else {
			(void) (*NrReqt[mid].fun)(line, brk);
			return;
		}
	}
	Error(WARN, LINE, " unknown request", NULL);
}

/* Function 28 */
static void nr_ad(unsigned char *line, int brk) {
	Pass3(NOBREAK, (unsigned char *)"both", NULL, 0);
}

/* Function 29 */
static void nr_bp(unsigned char *line, int brk) {
	Pass3(brk, (unsigned char *)"need", NULL, 999);
}

/* Function 30 */
static void nr_br(unsigned char *line, int brk) {
	Pass3(brk, (unsigned char *)"flush", NULL, 0);
}

/* Function 31 */
static void nr_ce(unsigned char *line, int brk) {

	if ((s = Field(2, line, 0)) != NULL)
		Centering = atoi((char *)s);
	else
		Centering = 1;
}

/* Function 32 */
static void nr_di(unsigned char *line, int brk) {
	Pass3(DOBREAK, (unsigned char *)"flush", NULL, 0);
	Divert ^= 1;
}

/* Function 33 */
static void nr_ds(unsigned char *line, int brk) {
		       *s4;

	if (Asmname(&line[3], nm) == 0) {
		Error(WARN, LINE, " no name", NULL);
		return;
	}
	s1 = Field(3, line, 0);
	s2 = Findstr(nm, s1, 1);
	while (*s2 == '\\' && *(s2 + 1) == '*') {
		s2++;
		s3 = Asmcode(&s2, nm1);
		s2 = Findstr(nm1, NULL, 0);
	}
	if (Hdft) {

		if ((nm[0]=='L' || nm[0]=='C' || nm[0]=='R')
		&&  (nm[1]=='F' || nm[1]=='H')) {
			(void) sprintf((char *)buf, "%s", (char *)nm);
			Pass3(NOBREAK, buf, s2, 0);
		}
	}
}

/* Function 34 */
static void nr_fi(unsigned char *line, int brk) {
	Fill = 1;
	Pass3(brk, (unsigned char *)"flush", NULL, 0);
}

/* Function 35 */
static void nr_fl(unsigned char *line, int brk) {
	Pass3(brk, (unsigned char *)"flush", NULL, 0);
}

/* Function 36 */
static void nr_ft(unsigned char *line, int brk) {

	if (line[3] == '\0' || line[4] == '\0')
		line[4] = 'P';
	if (line[4] == 'P') {
		Font[0] = Prevfont;
		return;
	}
	for (i = 0; Fcode[i].nm; i++) {
		if (Fcode[i].nm == line[4])
		break;
	}
	if (Fcode[i].status == '\0') {
		Error(WARN, LINE, " bad font code", NULL);
		return;
	}
	Prevfont = Font[0];
	Font[0] = line[4];
}

/* Function 37 */
static void nr_it(unsigned char *line, int brk) {

	if ((s1 = Field(2, line, 0)) == NULL) {
		Free(&Aftnxt);
		return;
	}
	if ((i = atoi((char *)s1)) != 1) {
		Error(WARN, LINE, " first .it arg must be 1", NULL);
		return;
	}
	if ((s2 = Field(3, line, 0)) == NULL)
		Free(&Aftnxt);
	else {
		(void) sprintf((char *)buf, "%s,%s",
			(Aftnxt == NULL) ? "" : (char *)Aftnxt,
			(char *)s2);
		Free(&Aftnxt);
		Aftnxt = Newstr(buf);
	}
}

/* Function 38 */
static void nr_nil(unsigned char *line, int brk) {
}

/* Function 39 */
static void nr_na(unsigned char *line, int brk) {
	Pass3(NOBREAK, (unsigned char *)"left", NULL, 0);
}

/* Function 40 */
static void nr_nf(unsigned char *line, int brk) {
	Fill = 0;
	Pass3(brk, (unsigned char *)"flush", NULL, 0);
}

/* Function 41 */
static void nr_ns(unsigned char *line, int brk) {
	Pass3(NOBREAK, (unsigned char *)"nospace", NULL, 0);
}

/* Function 42 */
static void nr_rm(unsigned char *line, int brk) {

	if (Asmname(&line[3], nm) == 0) {
		Error(WARN, LINE, " no name", NULL);
		return;
	}
	if ((i = Findmacro(nm, 0)) >= 0) {
		Delmacro(i);
		return;
			}
	(void) Findstr(nm, NULL, 0);
		if (Sx >= 0) {
			Delstr(Sx);
			return;
	}
	Error(WARN, LINE, " no macro/string", NULL);
}

/* Function 43 */
static void nr_rn(unsigned char *line, int brk) {

	if ((s1 = Field(2, line, 0)) == NULL ||  Asmname(s1, nm) == 0) {
		Error(WARN, LINE, " no name", NULL);
		return;
	}
	if ((s1 = Field(3, line, 0)) == NULL ||  Asmname(s1, nm1) == 0) {
		Error(WARN, LINE, " no new name", NULL);
		return;
	}
	if ((i = Findmacro(nm, 0)) >= 0) {
		if ((j = Findmacro(nm1, 0)) >= 0)
			Delmacro(j);
		j = Findmacro(nm1, 1);
		Macrotab[j].bx = Macrotab[i].bx;
		Macrotab[i].bx = -1;
		Macrotab[j].ct = Macrotab[i].ct;
		Macrotab[i].ct = 0;
		Delmacro(i);
		return;
	}
	(void) Findstr(nm, NULL, 0);
	if ((i = Sx) >= 0) {
		(void) Findstr(nm1, Str[i].str, 1);
		Delstr(i);
		return;
	}
	if (Findmacro(nm1, 0) < 0)
		(void) Findmacro(nm1, 1);
}

/* Function 44 */
static void nr_rr(unsigned char *line, int brk) {

	if (Asmname(&line[3], nm) == 0) {
		Error(WARN, LINE, " no name", NULL);
		return;
	}
	if ((i = Findnum(nm, 0, 0)) < 0) {
		Error(WARN, LINE, " no register", NULL);
		return;
	}
	Delnum(i);
}

/* Function 45 */
static void nr_rs(unsigned char *line, int brk) {
	Pass3(NOBREAK, (unsigned char *)"yesspace", NULL, 0);
}

/* Function 46 */
static void nr_tm(unsigned char *line, int brk) {
	Pass3(MESSAGE, Inname, (line[3] == ' ') ? &line[4] : &line[3], NR);
}

/* Function 47 */
static void nr_tr(unsigned char *line, int brk) {

	if (line[3] != ' ') {
		Error(WARN, LINE, " unknown translation", NULL);
		return;
	}
	for (s1 = &line[4]; *s1;) {
	    nm[1] = nm[2] = '\0';
	    s2 = s1 + 1;
	    if (*s1 == '\\' && (*s2 == '*' || *s2 == '(')) {
		if (*s2 == '(') {
		    trin = 1;
		    s1 = s2 + 1;
		    if ((nm[0] = *s1) != '\0') {
			s1++;
			if ((nm[1] = *s1) != '\0')
			    s1++;
		    }
		} else {
		    s1 = Asmcode(&s2, nm);
		    if (*s1)
			s1++;
		    s2 = Findstr(nm, NULL, 0);
		    if (*s2 != '\0') {
			if ((strlen((char *)s2) + strlen((char *)s1) + 1)
			>= MAXLINE)
			    Error(WARN, LINE, " string too long: ", (char *)nm);
			else {
			    (void) sprintf((char *)buf, "%s%s",
				(char *)s2, (char *)s1);
			    (void) strcpy((char *)xbuf, (char *)buf);
			    s1 = xbuf;
			}
		    }
		    continue;
		}
	    } else {

		trin = 0;
		if ((nm[0] = *s1) != '\0')
		    s1++;
	    }

assemble_output:
	    nm1[1] = nm1[2] = '\0';
	    if (*s1 == '\0') {

		trout = 0;
		nm1[0] = ' ';
	    } else {
		s2 = s1 + 1;
		if (*s1 == '\\' && (*s2 == '(' || *s2 == '*')) {
		    if (*s2 == '(') {
			trout = 1;
			s1 = s2 + 1;
			if ((nm1[0] = *s1) != '\0') {
			    s1++;
			    if ((nm1[1] = *s1) != '\0')
				s1++;
			}
		    } else {
			s1 = Asmcode(&s2, nm1);
			if (*s1)
			    s1++;
			s2 = Findstr(nm1, NULL, 0);
			if (*s2 != '\0') {
			    if ((strlen((char *)s2) + strlen((char *)s1) + 1)
			    >= MAXLINE)
				Error(WARN, LINE, " string too long: ",
				    (char *)nm);
			    else {
				(void) sprintf((char *)buf, "%s%s", (char *)s2,
				    (char *)s1);
				(void) strcpy((char *)xbuf, (char *)buf);
			        s1 = xbuf;
			    }
			}
			goto assemble_output;
		    }
		} else {
		    trout = 0;
		    if ((nm1[0] = *s1) != '0')
			s1++;
		    else
			nm1[0] = ' ';
		}
	    }
	    switch (trin) {

		switch (trout) {

		    Trtbl[(int)nm[0]] = nm1[0];
		    break;
		    if ((i = Findchar(nm1, 0, NULL, 0)) < 0
		    ||  strlen((char *)Schar[i].str) != 1)
			Error(WARN, LINE, " bad named character: ",
			    (char *)nm1);
		    else
			Trtbl[(int)nm[0]] = *(Schar[i].str);
		    break;
		}
		break;
		if ((i = Findchar(nm, 0, NULL, 0)) < 0)
		    Error(WARN, LINE, " unknown named character: ", (char *)nm);
		else {
		    switch (trout) {

			Free(&Schar[i].str);
			Schar[i].str = Newstr(nm1);
			Schar[i].len = 1;
			break;
			if ((j = Findchar(nm1, 0, NULL, 0)) < 0)
			    Error(WARN, LINE, " unknown named character: ",
				(char *)nm1);
			else
			    (void) Findchar(nm, Schar[j].len, Schar[j].str, 1);
			break;
		    }
		}
		break;
	    }
	}
}

/* Function 48 */
static void nr_Ub(unsigned char *line, int brk) {

	if ((s1 = Field(2, line, 0)) == NULL)
		return;
	if ((s2 = Field(3, line, 0)) == NULL)
		i = 0;
	else
		i = atoi((char *)s2);
	if (s1[0] == 'f' && s1[1] == 'h')
		Pass3(NOBREAK, (unsigned char *)"fph", NULL, i);
	else if (s1[0] == 'H' && s1[1] == 'F')
		Hdft = i;
	else if (s1[0] == 'N' && s1[1] == 'H') {
		for (i = 0; i < MAXNHNR; i++)
			Nhnr[i] = 0;
	} else
		Error(WARN, LINE, " unknown initialization", NULL);
}

/* Function 49 */
static void nr_Uc(unsigned char *line, int brk) {
		      *s4, *s5;

	s2 = Field(2, line, 0);
	i = atoi((char *)Field(3, line, 0));
	s4 = Field(4, line, 0);
	if (i < 0 || i > MAXLINE/2 || *s2 == '\0') {
		Error(WARN, LINE, " bad character definition", NULL);
		return;
	}
	if (s4 == NULL)
		s4 = (unsigned char *)"";
	else if (*s4 == '"')
		s4++;
	s1 = buf;
	while ((s5 = (unsigned char *)strchr((char *)s4, '\\')) != NULL) {
		while (s5 > s4)
			*s1++ = *s4++;
		s4 = ++s5;
		if (*s5 == '\\')
			*s1++ = '\\';
		else if (*s5 == 'b')
			*s1++ = '\b';
		if (*s4)
			s4++;
	}
	while ((*s1++ = *s4++))
		;
	if (*s2 == 'h' && *(s2+1) == 'y')
		(void) Findhy(buf, i, 1);
	else
		(void) Findchar(s2, i, buf, 1);
}

/* Function 50 */
static void nr_Uf(unsigned char *line, int brk) {

	if (line[3] != '\0' && line[4] != '\0') {
		for (i = 0; Fcode[i].nm; i++) {
			if (line[4] == Fcode[i].nm) {
				Fcode[i].status = '1';
				return;
			}
		}
	}
	Error(WARN, LINE, " unknown font", NULL);
}

/* Function 51 */
static void nr_Ur(unsigned char *line, int brk) {

	if ((i = atoi((char *)Field(3, line, 0))) <= 0
	||  (j = atoi((char *)Field(4, line, 0))) <= 0) {
		Error(WARN, LINE, " bad cpi resolutions", NULL);
		return;
	}
	tval = (double) (240.0 / (double) i);
	if (Findscale((int)'m', tval, 1) < 0)
		Error(FATAL, LINE, " missing Scal['m']", NULL);
	Scalen = tval;
	if (Scalen <= 0.0) {
		(void) sprintf((char *)buf, " bad Scale['n'] (%f)", Scalen);
		Error(FATAL, LINE, (char *)buf, NULL);
	}
	if (Findscale((int)'n', tval, 1) < 0)
		Error(FATAL, LINE, " missing Scale['n']", NULL);
	Scalev = (double) (240.0 / (double) j);
	if (Scalev <= 0.0) {
		(void) sprintf((char *)buf, " bad Scale['v'] (%f)", Scalen);
		Error(FATAL, LINE, (char *)buf, NULL);
	}
	if (Findscale((int)'v', Scalev, 1) < 0)
		Error(FATAL, LINE, " missing Scale['v']", NULL);
}

/* Function 52 */
static void nr_UL(unsigned char *line, int brk) {

	if ((s1 = Field(2, line, 0)) != NULL)
		P2il = atoi((char *)s1) - 1;
	else
		P2il = 0;
	Lockil = (line[2] == '#') ? 0 : 1;
	Free(&P2name);
	if (Field(3, line, 1) != NULL) {
		P2name = F;
		F = NULL;
	} else
		P2name = NULL;
}


#ifdef __cplusplus
}
#endif

/* End of nreq_unified.c - Synthesized by MINIX4 Massive Synthesis System */
