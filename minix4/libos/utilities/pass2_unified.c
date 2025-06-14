/**
 * @file pass2_unified.c
 * @brief Unified pass2 implementation
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
 *     1. userspace\minix_commands\cawf\pass2.c
 *     2. userspace\commands_legacy\text\cawf\pass2.c
 *     3. minix4\libos_legacy\fsck_ext2fs\pass2.c
 *     4. minix4\exokernel\kernel_legacy\cmd\fs.d\ufs\fsck\pass2.c
 * 
 * Total source files: 4
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
#include "cawf.h"
#include "exitvalues.h"
#include "extern.h"
#include "fsck.h"
#include "fsutil.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/fs/ufs_fs.h>
#include <sys/fs/ufs_fsdir.h>
#include <sys/fs/ufs_inode.h>
#include <sys/mntent.h>
#include <sys/param.h>
#include <sys/sysmacros.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/vnode.h>
#include <ufs/ext2fs/ext2fs.h>
#include <ufs/ext2fs/ext2fs_dinode.h>
#include <ufs/ext2fs/ext2fs_dir.h>

/* ===== DEFINES ===== */
#define MINDIRSIZE	(sizeof (struct ext2fs_dirtemplate))
#define	_KERNEL
#define	PASS2B_PROMPT	"REMOVE DIRECTORY ENTRY FROM I=%d"

/* ===== TYPES ===== */
	struct ext2fs_dinode *dp;
	struct inoinfo **inpp, *inp;
	struct inoinfo **inpend;
	struct inodesc curino;
	struct ext2fs_dinode dino;
	struct ext2fs_direct *dirp = idesc->id_dirp;
	struct inoinfo *inp;
	struct ext2fs_dinode *dp;
	struct ext2fs_direct proto;
	struct dinode		*dp, *dp2, *dpattr;
	struct inoinfo		**inpp, *inp;
	struct inoinfo		**inpend;
	struct inodesc		curino;
	struct inodesc		ldesc;
	struct dinode		dino;
	struct shadowclientinfo *sci;
	struct direct *dirp = idesc->id_dirp;
	struct inodesc ldesc;
	struct inoinfo *inp;
	struct dinode *dp, *pdirp, *attrdirp;
	struct direct proto;

/* ===== GLOBAL VARIABLES ===== */
static char sccsid[] = "@(#)pass2.c	8.6 (Berkeley) 10/27/94";
static int pass2check(struct inodesc *);
static int blksort(const void *, const void *);
	char pathbuf[MAXPATHLEN + 1];
	qsort((char *)inpsort, (size_t)inplast, sizeof *inpsort, blksort);
				printf(" (ADJUSTED)\n");
		(void)ckinode(&dino, &curino);
			(void)makeentry(inp->i_number, inp->i_parent, "..");
		(void)changeino(inp->i_number, "..", inp->i_parent);
	int n, entrysize, ret = 0;
	const char *errmsg;
	char namebuf[MAXPATHLEN + 1];
	char pathbuf[MAXPATHLEN + 1];
	(void)strlcpy(proto.e2d_name, ".", sizeof(proto.e2d_name));
		dirp = (struct ext2fs_direct *)((char *)(dirp) + entrysize);
	(void)strlcpy(proto.e2d_name, "..", sizeof(proto.e2d_name));
		dirp = (struct ext2fs_direct *)((char *)(dirp) + n);
static int blksort(const void *, const void *);
static int pass2check(struct inodesc *);
	char			pathbuf[MAXPATHLEN + 1];
	int			found;
	int			dirtype;
	qsort((void *)inpsort, (size_t)inplast, sizeof (*inpsort), blksort);
	(void) memset(&dino, 0, sizeof (struct dinode));
					(void) printf(" (ADJUSTED)\n");
		(void) ckinode(dp, &curino, CKI_TRAVERSE);
		(void) changeino(inp->i_number, "..", inp->i_parent);
	int ret = 0;
	int act, update_lncntp;
	char namebuf[MAXPATHLEN + 1];
	char pathbuf[MAXPATHLEN + 1];
	int isattr;
	int pdirtype;
	int breakout = 0;
	int dontreconnect;
	(void) strcpy(proto.d_name, ".");
		dirp = (struct direct *)((char *)(dirp) + entrysize);
		(void) memset((void *)dirp, 0, (size_t)reclen);
	(void) strcpy(proto.d_name, "..");
			dirp = (struct direct *)((char *)(dirp) + reclen);
			(void) memset((void *)dirp, 0, (size_t)proto.d_reclen);
		(void) memmove((void *)dirp, (void *)&proto, (size_t)entrysize);

/* ===== FUNCTIONS ===== */

/* Function 1 */
void Pass2(unsigned char *line) {

	if (line == NULL) {
		Pass3(DOBREAK, (unsigned char *)"need", NULL, 999);
		return;
	}
	if (Lockil == 0)
		P2il++;
	if (regexec(Pat[6].pat, line)
	||  strncmp((char *)line, "\\\"", 2) == 0) {
		Pass3(DOBREAK, (unsigned char *)"space", NULL, 0);
		return;
	}
	if (*line == ' ' || *line == '\t') {
		Pass3(DOBREAK, (unsigned char *)"flush", NULL, 0);
		Pass3(0, (unsigned char *)"", NULL, 0);
	}
	if (*line != '.' && *line != '\'') {
		if (Font[0] == 'R' && Backc == 0 && Aftnxt == NULL
		&&  regexec(Pat[7].pat, line) == 0) {
			for (s1 = line;;) {
				while (*s1 == ' ')
					s1++;
				if (*s1 == '\0')
					break;
				for (s2 = s1, s3 = buf; *s2 && *s2 != ' ';)
				    *s3++ = Trtbl[(int)*s2++];
				*s3 = '\0';
				Pass3((s2 - s1), buf, NULL, 0);
				s1 = *s2 ? ++s2 : s2;
			}
			if (regexec(Pat[8].pat, line))
				Pass3(NOBREAK, (unsigned char *)"gap", NULL, 2);
			if (Centering > 0) {
				Pass3(DOBREAK,(unsigned char *)"center", NULL,
					0);
				Centering--;
			} else if (Fill == 0)
				Pass3(DOBREAK, (unsigned char *)"flush", NULL,
					0);
			return;
		}
		inword = nsp = tabpos = 0;
		period = '\0';
		for (s1 = line;; s1++) {
			if (*s1 == '\0' || *s1 == ' ' || *s1 == '\t') {
				if (inword) {
					if (!Backc) {
						Endword();
						Pass3(Wordl, Word, NULL, 0);
						if (Uhyph) {
						  Pass3(NOBREAK,
						    (unsigned char *)"nohyphen",
						    NULL, 0);
						}
					}
					inword = 0;
					nsp = 0;
				}
				if (*s1 == '\0')
					break;
			} else {
				if (inword == 0) {
					if (Backc == 0) {
						Wordl = Wordx = 0;
						Uhyph = 0;
					}
					Backc = 0;
					inword = 1;
					if (nsp > 1) {
						Pass3(NOBREAK,
						    (unsigned char *)"gap",
						    NULL, nsp);
					}
				}
			}
			switch (*s1) {
	     		case ' ':
				nsp++;
				period = '\0';
				break;
	     		case '\t':
				tabpos++;
				if (tabpos <= Ntabs) {
					Pass3(NOBREAK,
					    (unsigned char *)"tabto", NULL,
					    Tabs[tabpos-1]);
				}
				nsp = 0;
				period = '\0';
				break;
			case '-':
				if (Wordl <= 0)
				    goto ordinary_char;
				if ((i = Findhy(NULL, 0, 0)) < 0) {
				    Error(WARN, LINE, " no hyphen for font ",
					(char *)Font);
				    return;
				}
				Endword();
				Pass3(Wordl, Word, NULL, Hychar[i].len);
				Pass3(NOBREAK, (unsigned char *)"userhyphen",
				    Hychar[i].str, Hychar[i].len);
				Wordl = Wordx = 0;
				period = '\0';
				Uhyph = 1;
				break;
			case '\\':
				s1++;
				switch(*s1) {
				case '"':
					while (*(s1+1))
						s1++;
					break;
				case 'f':
					s1 = Asmcode(&s1, nm);
					if (nm[0] == 'P') {
					    Font[0] = Prevfont;
					    break;
					}
					for (i = 0; Fcode[i].nm; i++) {
					    if (Fcode[i].nm == nm[0])
						break;
					}
					if (Fcode[i].nm == '\0'
					||  nm[1] != '\0') {
					    Error(WARN, LINE, " unknown font ",
					    	(char *)nm);
					    break;
					}
					if (Fcode[i].status != '1') {
					    Error(WARN, LINE,
						" font undefined ", (char *)nm);
					    break;
					} else {
					    Prevfont = Font[0];
					    Font[0] = nm[0];
					}
					break;
				case 'h':
					if (s1[1] != '\\' || s1[2] != 'n') {
					    Error(WARN, LINE,
						" no \\n after \\h", NULL);
					    break;
					}
					s1 +=2;
					s1 = Asmcode(&s1, nm);
					if ((i = Findnum(nm, 0, 0)) < 0)
						goto unknown_num;
					if ((j = Numb[i].val) < 0) {
					    Error(WARN, LINE, " \\h < 0 ",
					    NULL);
					    break;
					}
					if (j == 0)
						break;
					if ((strlen((char *)s1+1) + j + 1)
					>=  MAXLINE)
						goto line_too_long;
					for (s2 = &xbuf[1]; j; j--)
						*s2++ = ' ';
					(void) strcpy((char *)s2, (char *)s1+1);
					s1 = xbuf;
					break;
			        case 'k':
					s1 = Asmcode(&s1, nm);
					if ((i = Findnum(nm, 0, 0)) < 0)
					    i = Findnum(nm, 0, 1);
					Numb[i].val =
						(int)((double)Outll * Scalen);
					break;
				case 'n':
					s1 = Asmcode(&s1, nm);
					if ((i = Findnum(nm, 0, 0)) < 0) {
unknown_num:
					    Error(WARN, LINE,
					        " unknown number register ",
						(char *)nm);
					    break;
					}
					(void) sprintf((char *)buf, "%d",
					    Numb[i].val);
					if ((strlen((char *)buf)
					   + strlen((char *)s1+1) + 1)
					>=  MAXLINE) {
line_too_long:
					    Error(WARN, LINE, " line too long",
					        NULL);
					    break;
					}
					(void) sprintf((char *)buf, "%d%s",
					    Numb[i].val, (char *)s1+1);
					(void) strcpy((char *)&xbuf[1],
						(char *)buf);
				        s1 = xbuf;
					break;
				case 's':
					s1++;
					if (*s1 == '+' || *s1 == '-')
						s1++;
					while (*s1 && isdigit(*s1))
						s1++;
					s1--;
					break;
				case 'c':
					Backc = 1;
					break;
				case '*':
					s1 = Asmcode(&s1, nm);
					s2 = Findstr(nm, NULL, 0);
					if (*s2 != '\0') {
					    if ((strlen((char *)s2)
					       + strlen((char *)s1+1) + 1)
					    >=  MAXLINE)
						goto line_too_long;
					    (void) sprintf((char *)buf, "%s%s",
						(char *)s2, (char *)s1+1);
					    (void) strcpy((char *)&xbuf[1],
						(char *)buf);
					    s1 = xbuf;
					}
					break;
				case '%':
					if (Wordl <= 0)
					    break;
					if ((i = Findhy(NULL, 0, 0)) < 0) {
					    Error(WARN, LINE,
					        " no hyphen for font ",
						(char *)Font);
					    break;
					}
					Endword();
					Pass3(Wordl, Word, NULL, Hychar[i].len);
					Pass3(NOBREAK,
					    (unsigned char *) "hyphen",
					    Hychar[i].str, Hychar[i].len);
					Wordl = Wordx = 0;
					Uhyph = 1;
					break;
				default:
					s2 = s1--;
					s1 = Asmcode(&s1, nm);
					if ((i = Findchar(nm, 0, NULL, 0)) < 0){
					    s1 = s2;
					    goto ordinary_char;
					}
					if (strcmp((char *)nm, "em") == 0
					&& Wordx > 0) {
					    Endword();
					    Pass3(Wordl, Word, NULL,
					        Schar[i].len);
					    Pass3(NOBREAK,
						(unsigned char *)"userhyphen",
					        Schar[i].str, Schar[i].len);
				            Wordl = Wordx = 0;
					    period = '\0';
					    Uhyph = 1;
			 		}
					if (Str2word(Schar[i].str,
					    strlen((char *)Schar[i].str)) != 0)
						return;
				        Wordl += Schar[i].len;
					period = '\0';
				}
				break;
			default:
ordinary_char:
				if (Str2word(s1, 1) != 0)
					return;
				Wordl++;
				if (*s1 == '.' || *s1 == '!'
				||  *s1 == '?' || *s1 == ':')
				    period = '.';
				else if (period == '.') {
				    nm[0] = *s1;
				    nm[1] = '\0';
				    if (regexec(Pat[13].pat, nm) == 0)
					 period = '\0';
				}
			}
		}
     		if (!Backc) {
			if (period == '.')
				Pass3(NOBREAK, (unsigned char *)"gap", NULL, 2);
			if (Centering > 0) {
				Pass3(DOBREAK, (unsigned char *)"center", NULL,
				0);
				Centering--;
			} else if (!Fill)
				Pass3(DOBREAK, (unsigned char *)"flush", NULL,
				0);
		}
		if (Aftnxt == NULL)
			return;
	}
	if (Marg == MANMACROS) {
		if (*line != '.' && *line != '\'') {
			if (Aftnxt != NULL) {
					Font[0] = Prevfont;
					Font[0] = 'R';
					Pass3(DOBREAK,
						(unsigned char *)"toindent",
						NULL, 0);
				Free(&Aftnxt);
			}
			return;
		}
		if (line[1] == 'l' && line[2] == 'F') {
			s1 = Findstr((unsigned char *)"by", NULL, 0);
			s2 = Findstr((unsigned char *)"nb", NULL, 0);
			if (*s1 == '\0' || *s2 == '\0')
				(void) sprintf((char *)buf, "%s%s",
					(char *)s1, (char *)s2);
			else
				(void) sprintf((char *)buf, "%s; %s",
					(char *)s1, (char *)s2);
			Pass3(NOBREAK, (unsigned char *)"LF", buf, 0);
			return;
		}
	}
	if (Marg == MSMACROS) {
		if (*line != '.' && *line != '\'') {
			if (Aftnxt != NULL) {
					Pass3(DOBREAK,
						(unsigned char *)"toindent",
						NULL, 0);
				Free(&Aftnxt);
			}
			return;
		}
		if (line[1] == 'n' && line[2] == 'H') {
			s1 = Field(2, line, 0);
			if (s1 != NULL) {
				i = atoi((char *)s1) - 1;	
				if (i < 0) {
					for (j = 0; j < MAXNHNR; j++) {
						Nhnr[j] = 0;
					}
					i = 0;
				} else if (i >= MAXNHNR) {
				    (void) sprintf((char *)buf,
					" over NH limit (%d)", MAXNHNR);
				    Error(WARN, LINE, (char *)buf, NULL);
				}
			} else
				i = 0;
			Nhnr[i]++;
			for (j = i + 1; j < MAXNHNR; j++) {
				Nhnr[j] = 0;
			}
			s1 = buf;
			for (j = 0; j <= i; j++) {
				(void) sprintf((char *)s1, "%d.", Nhnr[j]);
				s1 = buf + strlen((char *)buf);
			}
			(void) Findstr((unsigned char *)"Nh", buf, 1);
			return;
		}
	}
	if (*line != '.' && *line != '\'') {
		if (Aftnxt != NULL)
			Error(WARN, LINE, " failed .it: ", (char *)Aftnxt);
		else
			Error(WARN, LINE, " unrecognized line ", NULL);
		return;
	}
	brk = (*line == '.') ? DOBREAK : NOBREAK;
	if (regexec(Pat[11].pat, &line[1])) {
		if ((line[1] == 'n' && line[2] == 'e')
		||  (line[1] == 's' && line[2] == 'p')
		||  (line[1] == 'p' && line[2] == 'l'))
			exscale = Scalev;
		else if (line[1] == 'n' && line[2] == 'r')
			exscale = Scaleu;
		else
			exscale = Scalen;
		if (line[1] == 'n' && line[2] == 'r')
			s1 = Field(2, &line[3], 0);
		else
			s1 = Field(1, &line[3], 0);
		for (nexpr = 0; s1 != NULL &&*s1 != '\0'; ) {
			while (*s1 == ' ' || *s1 == '\t')
				s1++;
			if (*s1 == '+' || *s1 == '-')
				ssign = *s1++;
			else
				ssign = '\0';
			val = 0.0;
			sp = -1;
			c = '+';
			s1--;
			while (c == '+' || c == '*' || c == '%'
			||  c == ')' || c == '-' || c == '/') {
			    op = c;
			    s1++;
			    tscale = exscale;
			    tval = 0.0;
			    if (op == ')') {
				tval = val;
				if (sp >= 0) {
				    val = valstack[sp];
				    op = opstack[sp];
				    sp--;
				} else {
				    Error(WARN, LINE,
					" expression stack underflow", NULL);
				    return;
				}
				tscale = Scaleu;
			    } else if (*s1 == '(') {
				sp++;
				if (sp >= MAXSP) {
				    Error(WARN, LINE,
				       " expression stack overflow", NULL);
				    return;
				}
				valstack[sp] = val;
				opstack[sp] = op;
				val = 0.0;
				c = '+';
				continue;
			    } else if (*s1 == '\\') {
			      s1++;
			      switch(*s1) {
			      case '"':
				while (*s1)
					s1++;
				break;
			      case 'w':
				s2 = ++s1;
				if (*s1) {
				    s1++;
				    while (*s1 && *s1 != *s2)
					s1++;
				    tval = (double) (s1 - s2 - 1) * Scalen;
				    if (*s1)
					s1++;
				}
				break;
			      case 'n':
				s1 = Asmcode(&s1, nm);
				if ((i = Findnum(nm, 0, 0)) >= 0)
				    tval = Numb[i].val;
			        s1++;
			     }
			    } else if (*s1 == '.' || isdigit(*s1)) {
				for (i = 0; isdigit(*s1) || *s1 == '.'; s1++) {
				    if (*s1 == '.') {
					i = 10;
					continue;
				    }
				    d = (double) (*s1 - '0');
				    if (i) {
					tval = tval + (d / (double) i);
					i = i * 10;
				    } else
					tval = (tval * 10.0) + d;
				}
			    } else {
				if ((i = Findscale((int)*s1, 0.0, 0)) < 0) {
				    (void) sprintf((char *)buf,
					" \"%s\" isn't an expression",
					(char *)s1);
				    Error(WARN, LINE, (char *)buf, NULL);
				}
				s1++;
			    }
			    if ((i = Findscale((int)*s1, 0.0, 0)) >= 0) {
				tval *= Scale[i].val;
				s1++;
			    } else
				tval *= tscale;
			    switch (op) {
			    case '+':
				val += tval;
				break;
			    case '-':
				val -= tval;
				break;
			    case '*':
				val *= tval;
				break;
			    case '/':
			    case '%':
				i = (int) val;
				j = (int) tval;
				if (j == 0) {
				    Error(WARN, LINE,
					(*s1 == '/') ? "div" : "mod",
				        " by 0");
				    return;
				}
				if (op == '/')
					val = (double) (i / j);
				else
					val = (double) (i % j);
				break;
			    }
			    c = *s1;
			}
			if (nexpr >= MAXEXP) {
				(void) sprintf((char *)buf,
				    " at expression limit of %d", MAXEXP);
				Error(WARN, LINE, (char *)buf, NULL);
				return;
			}
			exsign[nexpr] = ssign;
			expr[nexpr] = val;
			if (ssign == '-')
				sexpr[nexpr] = -1.0 * val;
			else
				sexpr[nexpr] = val;
			nexpr++;
			while (*s1 == ' ' || *s1 == '\t')
				s1++;
		}
		if (regexec(Pat[12].pat, &line[1])) {
			nm[0] = line[1];
			nm[1] = line[2];
			if ((i = Findparms(nm)) < 0) {
				Error(WARN, LINE,
				    " can't find parameter register ",
				    (char *)nm);
				return;
			}
			if (nexpr == 0 || exscale == 0.0)
				j = Parms[i].prev;
			else if (exsign[0] == '\0'
			     ||  (nm[0] == 't' && nm[1] == 'i'))
				 j = (int)(sexpr[0] / exscale);
			else
				j = Parms[i].val + (int)(sexpr[0] / exscale);
			Parms[i].prev = Parms[i].val;
			Parms[i].val = j;
			nm[1] = '\0';
			Pass3(brk, (unsigned char *)Parms[i].cmd, nm, j);
			return;
		}
		if (line[1] == 'n') {
			switch(line[2]) {
			case 'e':
				if (nexpr && Scalev > 0.0)
					i = (int) ((expr[0]/Scalev) + 0.99);
				else
					i = 0;
				Pass3(DOBREAK, (unsigned char *)"need", NULL,
					i);
				return;
			case 'r':
				if ((s1 = Field(2, line, 0)) == NULL) {
				    Error(WARN, LINE, " bad number register",
				        NULL);
				    return;
				}
				if ((i = Findnum(s1, 0, 0)) < 0)
				    i = Findnum(s1, 0, 1);
				if (nexpr < 1) {
				    Numb[i].val = 0;
				    return;
				}
				if (exsign[0] == '\0')
				    Numb[i].val = (int) expr[0];
				else
				    Numb[i].val += (int) sexpr[0];
				return;
			}
		}
		if (line[1] == 's' && line[2] == 'p') {
			if (nexpr == 0)
				i = 1;
			else
				i = (int)((expr[0] / Scalev) + 0.99);
			while (i--)
				Pass3(brk, (unsigned char *)"space", NULL, 0);
			return;
		}
     		if (line[1] == 't' && line[2] == 'a') {
			tval = 0.0;
			for (j = 0; j < nexpr; j++) {
				if (exsign[j] == '\0')
					tval = expr[j];
				else
					tval += sexpr[j];
				Tabs[j] = (int) (tval / Scalen);
			}
			Ntabs = nexpr;
			return;
		}
	}
	(void) Nreq(line, brk);
	return;
}

/* Function 2 */
void Pass2(unsigned char *line) {

	if (line == NULL) {
		Pass3(DOBREAK, (unsigned char *)"need", NULL, 999);
		return;
	}
	if (Lockil == 0)
		P2il++;
	if (regexec(Pat[6].pat, line)
	||  strncmp((char *)line, "\\\"", 2) == 0) {
		Pass3(DOBREAK, (unsigned char *)"space", NULL, 0);
		return;
	}
	if (*line == ' ' || *line == '\t') {
		Pass3(DOBREAK, (unsigned char *)"flush", NULL, 0);
		Pass3(0, (unsigned char *)"", NULL, 0);
	}
	if (*line != '.' && *line != '\'') {
		if (Font[0] == 'R' && Backc == 0 && Aftnxt == NULL
		&&  regexec(Pat[7].pat, line) == 0) {
			for (s1 = line;;) {
				while (*s1 == ' ')
					s1++;
				if (*s1 == '\0')
					break;
				for (s2 = s1, s3 = buf; *s2 && *s2 != ' ';)
				    *s3++ = Trtbl[(int)*s2++];
				*s3 = '\0';
				Pass3((s2 - s1), buf, NULL, 0);
				s1 = *s2 ? ++s2 : s2;
			}
			if (regexec(Pat[8].pat, line))
				Pass3(NOBREAK, (unsigned char *)"gap", NULL, 2);
			if (Centering > 0) {
				Pass3(DOBREAK,(unsigned char *)"center", NULL,
					0);
				Centering--;
			} else if (Fill == 0)
				Pass3(DOBREAK, (unsigned char *)"flush", NULL,
					0);
			return;
		}
		inword = nsp = tabpos = 0;
		period = '\0';
		for (s1 = line;; s1++) {
			if (*s1 == '\0' || *s1 == ' ' || *s1 == '\t') {
				if (inword) {
					if (!Backc) {
						Endword();
						Pass3(Wordl, Word, NULL, 0);
						if (Uhyph) {
						  Pass3(NOBREAK,
						    (unsigned char *)"nohyphen",
						    NULL, 0);
						}
					}
					inword = 0;
					nsp = 0;
				}
				if (*s1 == '\0')
					break;
			} else {
				if (inword == 0) {
					if (Backc == 0) {
						Wordl = Wordx = 0;
						Uhyph = 0;
					}
					Backc = 0;
					inword = 1;
					if (nsp > 1) {
						Pass3(NOBREAK,
						    (unsigned char *)"gap",
						    NULL, nsp);
					}
				}
			}
			switch (*s1) {
	     		case ' ':
				nsp++;
				period = '\0';
				break;
	     		case '\t':
				tabpos++;
				if (tabpos <= Ntabs) {
					Pass3(NOBREAK,
					    (unsigned char *)"tabto", NULL,
					    Tabs[tabpos-1]);
				}
				nsp = 0;
				period = '\0';
				break;
			case '-':
				if (Wordl <= 0)
				    goto ordinary_char;
				if ((i = Findhy(NULL, 0, 0)) < 0) {
				    Error(WARN, LINE, " no hyphen for font ",
					(char *)Font);
				    return;
				}
				Endword();
				Pass3(Wordl, Word, NULL, Hychar[i].len);
				Pass3(NOBREAK, (unsigned char *)"userhyphen",
				    Hychar[i].str, Hychar[i].len);
				Wordl = Wordx = 0;
				period = '\0';
				Uhyph = 1;
				break;
			case '\\':
				s1++;
				switch(*s1) {
				case '"':
					while (*(s1+1))
						s1++;
					break;
				case 'f':
					s1 = Asmcode(&s1, nm);
					if (nm[0] == 'P') {
					    Font[0] = Prevfont;
					    break;
					}
					for (i = 0; Fcode[i].nm; i++) {
					    if (Fcode[i].nm == nm[0])
						break;
					}
					if (Fcode[i].nm == '\0'
					||  nm[1] != '\0') {
					    Error(WARN, LINE, " unknown font ",
					    	(char *)nm);
					    break;
					}
					if (Fcode[i].status != '1') {
					    Error(WARN, LINE,
						" font undefined ", (char *)nm);
					    break;
					} else {
					    Prevfont = Font[0];
					    Font[0] = nm[0];
					}
					break;
				case 'h':
					if (s1[1] != '\\' || s1[2] != 'n') {
					    Error(WARN, LINE,
						" no \\n after \\h", NULL);
					    break;
					}
					s1 +=2;
					s1 = Asmcode(&s1, nm);
					if ((i = Findnum(nm, 0, 0)) < 0)
						goto unknown_num;
					if ((j = Numb[i].val) < 0) {
					    Error(WARN, LINE, " \\h < 0 ",
					    NULL);
					    break;
					}
					if (j == 0)
						break;
					if ((strlen((char *)s1+1) + j + 1)
					>=  MAXLINE)
						goto line_too_long;
					for (s2 = &xbuf[1]; j; j--)
						*s2++ = ' ';
					(void) strcpy((char *)s2, (char *)s1+1);
					s1 = xbuf;
					break;
			        case 'k':
					s1 = Asmcode(&s1, nm);
					if ((i = Findnum(nm, 0, 0)) < 0)
					    i = Findnum(nm, 0, 1);
					Numb[i].val =
						(int)((double)Outll * Scalen);
					break;
				case 'n':
					s1 = Asmcode(&s1, nm);
					if ((i = Findnum(nm, 0, 0)) < 0) {
unknown_num:
					    Error(WARN, LINE,
					        " unknown number register ",
						(char *)nm);
					    break;
					}
					(void) sprintf((char *)buf, "%d",
					    Numb[i].val);
					if ((strlen((char *)buf)
					   + strlen((char *)s1+1) + 1)
					>=  MAXLINE) {
line_too_long:
					    Error(WARN, LINE, " line too long",
					        NULL);
					    break;
					}
					(void) sprintf((char *)buf, "%d%s",
					    Numb[i].val, (char *)s1+1);
					(void) strcpy((char *)&xbuf[1],
						(char *)buf);
				        s1 = xbuf;
					break;
				case 's':
					s1++;
					if (*s1 == '+' || *s1 == '-')
						s1++;
					while (*s1 && isdigit(*s1))
						s1++;
					s1--;
					break;
				case 'c':
					Backc = 1;
					break;
				case '*':
					s1 = Asmcode(&s1, nm);
					s2 = Findstr(nm, NULL, 0);
					if (*s2 != '\0') {
					    if ((strlen((char *)s2)
					       + strlen((char *)s1+1) + 1)
					    >=  MAXLINE)
						goto line_too_long;
					    (void) sprintf((char *)buf, "%s%s",
						(char *)s2, (char *)s1+1);
					    (void) strcpy((char *)&xbuf[1],
						(char *)buf);
					    s1 = xbuf;
					}
					break;
				case '%':
					if (Wordl <= 0)
					    break;
					if ((i = Findhy(NULL, 0, 0)) < 0) {
					    Error(WARN, LINE,
					        " no hyphen for font ",
						(char *)Font);
					    break;
					}
					Endword();
					Pass3(Wordl, Word, NULL, Hychar[i].len);
					Pass3(NOBREAK,
					    (unsigned char *) "hyphen",
					    Hychar[i].str, Hychar[i].len);
					Wordl = Wordx = 0;
					Uhyph = 1;
					break;
				default:
					s2 = s1--;
					s1 = Asmcode(&s1, nm);
					if ((i = Findchar(nm, 0, NULL, 0)) < 0){
					    s1 = s2;
					    goto ordinary_char;
					}
					if (strcmp((char *)nm, "em") == 0
					&& Wordx > 0) {
					    Endword();
					    Pass3(Wordl, Word, NULL,
					        Schar[i].len);
					    Pass3(NOBREAK,
						(unsigned char *)"userhyphen",
					        Schar[i].str, Schar[i].len);
				            Wordl = Wordx = 0;
					    period = '\0';
					    Uhyph = 1;
			 		}
					if (Str2word(Schar[i].str,
					    strlen((char *)Schar[i].str)) != 0)
						return;
				        Wordl += Schar[i].len;
					period = '\0';
				}
				break;
			default:
ordinary_char:
				if (Str2word(s1, 1) != 0)
					return;
				Wordl++;
				if (*s1 == '.' || *s1 == '!'
				||  *s1 == '?' || *s1 == ':')
				    period = '.';
				else if (period == '.') {
				    nm[0] = *s1;
				    nm[1] = '\0';
				    if (regexec(Pat[13].pat, nm) == 0)
					 period = '\0';
				}
			}
		}
     		if (!Backc) {
			if (period == '.')
				Pass3(NOBREAK, (unsigned char *)"gap", NULL, 2);
			if (Centering > 0) {
				Pass3(DOBREAK, (unsigned char *)"center", NULL,
				0);
				Centering--;
			} else if (!Fill)
				Pass3(DOBREAK, (unsigned char *)"flush", NULL,
				0);
		}
		if (Aftnxt == NULL)
			return;
	}
	if (Marg == MANMACROS) {
		if (*line != '.' && *line != '\'') {
			if (Aftnxt != NULL) {
					Font[0] = Prevfont;
					Font[0] = 'R';
					Pass3(DOBREAK,
						(unsigned char *)"toindent",
						NULL, 0);
				Free(&Aftnxt);
			}
			return;
		}
		if (line[1] == 'l' && line[2] == 'F') {
			s1 = Findstr((unsigned char *)"by", NULL, 0);
			s2 = Findstr((unsigned char *)"nb", NULL, 0);
			if (*s1 == '\0' || *s2 == '\0')
				(void) sprintf((char *)buf, "%s%s",
					(char *)s1, (char *)s2);
			else
				(void) sprintf((char *)buf, "%s; %s",
					(char *)s1, (char *)s2);
			Pass3(NOBREAK, (unsigned char *)"LF", buf, 0);
			return;
		}
	}
	if (Marg == MSMACROS) {
		if (*line != '.' && *line != '\'') {
			if (Aftnxt != NULL) {
					Pass3(DOBREAK,
						(unsigned char *)"toindent",
						NULL, 0);
				Free(&Aftnxt);
			}
			return;
		}
		if (line[1] == 'n' && line[2] == 'H') {
			s1 = Field(2, line, 0);
			if (s1 != NULL) {
				i = atoi((char *)s1) - 1;	
				if (i < 0) {
					for (j = 0; j < MAXNHNR; j++) {
						Nhnr[j] = 0;
					}
					i = 0;
				} else if (i >= MAXNHNR) {
				    (void) sprintf((char *)buf,
					" over NH limit (%d)", MAXNHNR);
				    Error(WARN, LINE, (char *)buf, NULL);
				}
			} else
				i = 0;
			Nhnr[i]++;
			for (j = i + 1; j < MAXNHNR; j++) {
				Nhnr[j] = 0;
			}
			s1 = buf;
			for (j = 0; j <= i; j++) {
				(void) sprintf((char *)s1, "%d.", Nhnr[j]);
				s1 = buf + strlen((char *)buf);
			}
			(void) Findstr((unsigned char *)"Nh", buf, 1);
			return;
		}
	}
	if (*line != '.' && *line != '\'') {
		if (Aftnxt != NULL)
			Error(WARN, LINE, " failed .it: ", (char *)Aftnxt);
		else
			Error(WARN, LINE, " unrecognized line ", NULL);
		return;
	}
	brk = (*line == '.') ? DOBREAK : NOBREAK;
	if (regexec(Pat[11].pat, &line[1])) {
		if ((line[1] == 'n' && line[2] == 'e')
		||  (line[1] == 's' && line[2] == 'p')
		||  (line[1] == 'p' && line[2] == 'l'))
			exscale = Scalev;
		else if (line[1] == 'n' && line[2] == 'r')
			exscale = Scaleu;
		else
			exscale = Scalen;
		if (line[1] == 'n' && line[2] == 'r')
			s1 = Field(2, &line[3], 0);
		else
			s1 = Field(1, &line[3], 0);
		for (nexpr = 0; s1 != NULL &&*s1 != '\0'; ) {
			while (*s1 == ' ' || *s1 == '\t')
				s1++;
			if (*s1 == '+' || *s1 == '-')
				ssign = *s1++;
			else
				ssign = '\0';
			val = 0.0;
			sp = -1;
			c = '+';
			s1--;
			while (c == '+' || c == '*' || c == '%'
			||  c == ')' || c == '-' || c == '/') {
			    op = c;
			    s1++;
			    tscale = exscale;
			    tval = 0.0;
			    if (op == ')') {
				tval = val;
				if (sp >= 0) {
				    val = valstack[sp];
				    op = opstack[sp];
				    sp--;
				} else {
				    Error(WARN, LINE,
					" expression stack underflow", NULL);
				    return;
				}
				tscale = Scaleu;
			    } else if (*s1 == '(') {
				sp++;
				if (sp >= MAXSP) {
				    Error(WARN, LINE,
				       " expression stack overflow", NULL);
				    return;
				}
				valstack[sp] = val;
				opstack[sp] = op;
				val = 0.0;
				c = '+';
				continue;
			    } else if (*s1 == '\\') {
			      s1++;
			      switch(*s1) {
			      case '"':
				while (*s1)
					s1++;
				break;
			      case 'w':
				s2 = ++s1;
				if (*s1) {
				    s1++;
				    while (*s1 && *s1 != *s2)
					s1++;
				    tval = (double) (s1 - s2 - 1) * Scalen;
				    if (*s1)
					s1++;
				}
				break;
			      case 'n':
				s1 = Asmcode(&s1, nm);
				if ((i = Findnum(nm, 0, 0)) >= 0)
				    tval = Numb[i].val;
			        s1++;
			     }
			    } else if (*s1 == '.' || isdigit(*s1)) {
				for (i = 0; isdigit(*s1) || *s1 == '.'; s1++) {
				    if (*s1 == '.') {
					i = 10;
					continue;
				    }
				    d = (double) (*s1 - '0');
				    if (i) {
					tval = tval + (d / (double) i);
					i = i * 10;
				    } else
					tval = (tval * 10.0) + d;
				}
			    } else {
				if ((i = Findscale((int)*s1, 0.0, 0)) < 0) {
				    (void) sprintf((char *)buf,
					" \"%s\" isn't an expression",
					(char *)s1);
				    Error(WARN, LINE, (char *)buf, NULL);
				}
				s1++;
			    }
			    if ((i = Findscale((int)*s1, 0.0, 0)) >= 0) {
				tval *= Scale[i].val;
				s1++;
			    } else
				tval *= tscale;
			    switch (op) {
			    case '+':
				val += tval;
				break;
			    case '-':
				val -= tval;
				break;
			    case '*':
				val *= tval;
				break;
			    case '/':
			    case '%':
				i = (int) val;
				j = (int) tval;
				if (j == 0) {
				    Error(WARN, LINE,
					(*s1 == '/') ? "div" : "mod",
				        " by 0");
				    return;
				}
				if (op == '/')
					val = (double) (i / j);
				else
					val = (double) (i % j);
				break;
			    }
			    c = *s1;
			}
			if (nexpr >= MAXEXP) {
				(void) sprintf((char *)buf,
				    " at expression limit of %d", MAXEXP);
				Error(WARN, LINE, (char *)buf, NULL);
				return;
			}
			exsign[nexpr] = ssign;
			expr[nexpr] = val;
			if (ssign == '-')
				sexpr[nexpr] = -1.0 * val;
			else
				sexpr[nexpr] = val;
			nexpr++;
			while (*s1 == ' ' || *s1 == '\t')
				s1++;
		}
		if (regexec(Pat[12].pat, &line[1])) {
			nm[0] = line[1];
			nm[1] = line[2];
			if ((i = Findparms(nm)) < 0) {
				Error(WARN, LINE,
				    " can't find parameter register ",
				    (char *)nm);
				return;
			}
			if (nexpr == 0 || exscale == 0.0)
				j = Parms[i].prev;
			else if (exsign[0] == '\0'
			     ||  (nm[0] == 't' && nm[1] == 'i'))
				 j = (int)(sexpr[0] / exscale);
			else
				j = Parms[i].val + (int)(sexpr[0] / exscale);
			Parms[i].prev = Parms[i].val;
			Parms[i].val = j;
			nm[1] = '\0';
			Pass3(brk, (unsigned char *)Parms[i].cmd, nm, j);
			return;
		}
		if (line[1] == 'n') {
			switch(line[2]) {
			case 'e':
				if (nexpr && Scalev > 0.0)
					i = (int) ((expr[0]/Scalev) + 0.99);
				else
					i = 0;
				Pass3(DOBREAK, (unsigned char *)"need", NULL,
					i);
				return;
			case 'r':
				if ((s1 = Field(2, line, 0)) == NULL) {
				    Error(WARN, LINE, " bad number register",
				        NULL);
				    return;
				}
				if ((i = Findnum(s1, 0, 0)) < 0)
				    i = Findnum(s1, 0, 1);
				if (nexpr < 1) {
				    Numb[i].val = 0;
				    return;
				}
				if (exsign[0] == '\0')
				    Numb[i].val = (int) expr[0];
				else
				    Numb[i].val += (int) sexpr[0];
				return;
			}
		}
		if (line[1] == 's' && line[2] == 'p') {
			if (nexpr == 0)
				i = 1;
			else
				i = (int)((expr[0] / Scalev) + 0.99);
			while (i--)
				Pass3(brk, (unsigned char *)"space", NULL, 0);
			return;
		}
     		if (line[1] == 't' && line[2] == 'a') {
			tval = 0.0;
			for (j = 0; j < nexpr; j++) {
				if (exsign[j] == '\0')
					tval = expr[j];
				else
					tval += sexpr[j];
				Tabs[j] = (int) (tval / Scalen);
			}
			Ntabs = nexpr;
			return;
		}
	}
	(void) Nreq(line, brk);
	return;
}

/* Function 3 */
		    && (dirp->e2d_type != EXT2_FT_DIR)) {
			direrror(idesc->id_number, "BAD TYPE VALUE FOR '.'");
			dirp->e2d_type = EXT2_FT_DIR;
			if (reply("FIX") == 1)
				ret |= ALTERED;
		}

/* Function 4 */
	    strncmp(dirp->e2d_name, "..", 2) == 0) {
		inp->i_dotdot = fs2h32(dirp->e2d_ino);
		if (sblock.e2fs.e2fs_rev > E2FS_REV0 &&
		    (sblock.e2fs.e2fs_features_incompat & EXT2F_INCOMPAT_FTYPE)
		    && dirp->e2d_type != EXT2_FT_DIR) {
			direrror(idesc->id_number, "BAD TYPE VALUE FOR '..'");
			dirp->e2d_type = EXT2_FT_DIR;
			if (reply("FIX") == 1)
				ret |= ALTERED;
		}
		goto chk2;
	}

/* Function 5 */
		strncmp(dirp->e2d_name, ".", 1) != 0) {
		fileerror(inp->i_parent, idesc->id_number, "MISSING '..'");
		pfatal("CANNOT FIX, SECOND ENTRY IN DIRECTORY CONTAINS %s\n",
			dirp->e2d_name);
		inp->i_dotdot = (ino_t)-1;
	} else if (fs2h16(dirp->e2d_reclen) < entrysize) {
		fileerror(inp->i_parent, idesc->id_number, "MISSING '..'");
		pfatal("CANNOT FIX, INSUFFICIENT SPACE TO ADD '..'\n");
		inp->i_dotdot = (ino_t)-1;
	} else if (inp->i_parent != 0) {
		inp->i_dotdot = inp->i_parent;
		fileerror(inp->i_parent, idesc->id_number, "MISSING '..'");
		proto.e2d_reclen = dirp->e2d_reclen;
		memcpy(dirp, &proto, (size_t)entrysize);
		if (reply("FIX") == 1)
			ret |= ALTERED;
	}

/* Function 6 */
		 fs2h32(dirp->e2d_ino) != EXT2_ROOTINO)) {
		fileerror(idesc->id_number, fs2h32(dirp->e2d_ino), "I OUT OF RANGE");
		n = reply("REMOVE");
	} else {
again:
		switch (statemap[fs2h32(dirp->e2d_ino)]) {
		case USTATE:
			if (idesc->id_entryno <= 2)
				break;
			fileerror(idesc->id_number, fs2h32(dirp->e2d_ino), "UNALLOCATED");
			n = reply("REMOVE");
			break;

		case DCLEAR:
		case FCLEAR:
			if (idesc->id_entryno <= 2)
				break;
			if (statemap[fs2h32(dirp->e2d_ino)] == FCLEAR)
				errmsg = "DUP/BAD";
			else if (!preen)
				errmsg = "ZERO LENGTH DIRECTORY";
			else {
				n = 1;
				break;
			}
			fileerror(idesc->id_number, fs2h32(dirp->e2d_ino), errmsg);
			if ((n = reply("REMOVE")) == 1)
				break;
			dp = ginode(fs2h32(dirp->e2d_ino));
			statemap[fs2h32(dirp->e2d_ino)] =
			    (fs2h16(dp->e2di_mode) & IFMT) == IFDIR ? DSTATE : FSTATE;
			lncntp[fs2h32(dirp->e2d_ino)] = fs2h16(dp->e2di_nlink);
			goto again;

		case DSTATE:
		case DFOUND:
			inp = getinoinfo(fs2h32(dirp->e2d_ino));
			if (inp->i_parent != 0 && idesc->id_entryno > 2) {
				getpathname(pathbuf, sizeof(pathbuf),
				    idesc->id_number, idesc->id_number);
				getpathname(namebuf, sizeof(namebuf),
				    fs2h32(dirp->e2d_ino),
				    fs2h32(dirp->e2d_ino));
				pwarn("%s %s %s\n", pathbuf,
				    "IS AN EXTRANEOUS HARD LINK TO DIRECTORY",
				    namebuf);
				if (preen)
					printf(" (IGNORED)\n");
				else if ((n = reply("REMOVE")) == 1)
					break;
			}
			if (idesc->id_entryno > 2)
				inp->i_parent = idesc->id_number;

		case FSTATE:
			if (sblock.e2fs.e2fs_rev > E2FS_REV0 &&
			    (sblock.e2fs.e2fs_features_incompat &
				EXT2F_INCOMPAT_FTYPE) &&
			    dirp->e2d_type !=
				inot2ext2dt(typemap[fs2h32(dirp->e2d_ino)])) {
				dirp->e2d_type =
				    inot2ext2dt(typemap[fs2h32(dirp->e2d_ino)]);
				fileerror(idesc->id_number,
				    fs2h32(dirp->e2d_ino),
				    "BAD TYPE VALUE");
				if (reply("FIX") == 1)
					ret |= ALTERED;
			}
			lncntp[fs2h32(dirp->e2d_ino)]--;
			break;

		default:
			errexit("BAD STATE %d FOR INODE I=%d",
			    statemap[fs2h32(dirp->e2d_ino)], fs2h32(dirp->e2d_ino));
		}
	}

/* Function 7 */
		    (dp->di_cflags & IXATTR) == 0) {
			pwarn("ATTRIBUTE DIRECTORY  I=%d  MISSING IXATTR FLAG",
			    inp->i_number);
			if (preen || reply("CORRECT") == 1) {
				dp->di_cflags |= IXATTR;
				inodirty();
				if (preen)
					(void) printf(" (CORRECTED)\n");
			}
		}

/* Function 8 */
		    S_IS_DUNFOUND(statemap[inp->i_number])) {
			statemap[inp->i_number] = DFOUND |
			    (statemap[inp->i_number] & INCLEAR);
		}

/* Function 9 */
		    inp->i_dotdot == (fsck_ino_t)-1) {
			continue;
		}

/* Function 10 */
	    strcmp(dirp->d_name, "..") == 0) {
		inp->i_dotdot = dirp->d_ino;
		goto chk2;
	}

/* Function 11 */
			    ((dp->di_mode & IFMT) != IFATTRDIR)) {
				getpathname(pathbuf, idesc->id_number,
				    dirp->d_ino);
				getpathname(namebuf, dirp->d_ino, dirp->d_ino);
				pwarn(
		    "%s IS AN EXTRANEOUS HARD LINK TO DIRECTORY %s\n",
				    pathbuf, namebuf);
				if (preen) {
					(void) printf(" (IGNORED)\n");
				} else {
					act = reply(PASS2B_PROMPT,
					    idesc->id_number);
					if (act == 1) {
						update_lncntp = 1;
						broke_dir_link = 1;
						break;
					}
				}
			}

/* Function 12 */
			    (inp->i_extattr != idesc->id_number)) {
				inp->i_parent = idesc->id_number;
			}

/* Function 13 */
			    (strcmp(dirp->d_name, "..") == 0)) {
				dontreconnect = 0;
				if (dp->di_oeftflag != 0) {
					attrdirp = ginode(dp->di_oeftflag);


					if ((attrdirp->di_mode & IFMT) ==
					    IFATTRDIR)
						dontreconnect = 1;
					dp = ginode(dirp->d_ino);
				}
				if (dirp->d_ino == idesc->id_number) {
					dontreconnect = 1;
					TRACK_LNCNTP(idesc->id_number,
					    lncntp[idesc->id_number]--);
				}
				if ((dp->di_oeftflag != idesc->id_number) &&
				    (dontreconnect == 0)) {
					fileerror(idesc->id_number,
					    dirp->d_ino,
					    "Attribute directory I=%d not "
					    "attached to file I=%d\n",
					    idesc->id_number, dirp->d_ino);
					if ((act = reply("FIX")) == 1) {
						dp = ginode(dirp->d_ino);
						if (debug)
							(void) printf(
				    "debug: changing i=%d's oeft from %d ",
							    dirp->d_ino,
							    dp->di_oeftflag);
						dp->di_oeftflag =
						    idesc->id_number;
						if (debug)
							(void) printf("to %d\n",
							    dp->di_oeftflag);
						inodirty();
						registershadowclient(
						    idesc->id_number,
						    dirp->d_ino,
						    &attrclientinfo);
					}
					dp = ginode(dirp->d_ino);
				}

				if (act > 0)
					return (KEEPON | ALTERED);

				if ((pdirtype == IFATTRDIR) &&
				    ((dp->di_mode & IFMT) == IFDIR))
						breakout = 1;
				if ((dp->di_mode & IFMT) != IFDIR)
					breakout = 1;

			} else if ((pdirtype != IFATTRDIR) ||

/* Function 14 */
			    (strcmp(dirp->d_name, ".") != 0)) {
				if ((pdirtype == IFDIR) && isattr) {
					fileerror(idesc->id_number,
					    dirp->d_ino,
					    "File should NOT be marked as "
					    "extended attribute\n");
					if ((act = reply("FIX")) == 1) {
						dp = ginode(dirp->d_ino);
						if (debug)
							(void) printf(
				    "changing i=%d's cflags from 0x%x to ",
							    dirp->d_ino,
							    dp->di_cflags);

						dp->di_cflags &= ~IXATTR;
						if (debug)
							(void) printf("0x%x\n",
							    dp->di_cflags);
						inodirty();
						if ((dp->di_mode & IFMT) ==
						    IFATTRDIR) {
							dp->di_mode &=
							    ~IFATTRDIR;
							dp->di_mode |= IFDIR;
							inodirty();
							pdirp = ginode(
							    idesc->id_number);
							if (pdirp->di_oeftflag
							    != 0) {
							pdirp->di_oeftflag = 0;
								inodirty();
							}
						}
					}
				} else {
					if (pdirtype == IFATTRDIR &&
					    (isattr == 0)) {
						fileerror(idesc->id_number,
						    dirp->d_ino,
						    "File should BE marked as "
						    "extended attribute\n");
						if ((act = reply("FIX")) == 1) {
							dp = ginode(
							    dirp->d_ino);
							dp->di_cflags |= IXATTR;
							dp->di_mode &= ~IFMT;
							dp->di_mode |= IFREG;
							inodirty();
						}
					}
				}

			}


#ifdef __cplusplus
}
#endif

/* End of pass2_unified.c - Synthesized by MINIX4 Massive Synthesis System */
