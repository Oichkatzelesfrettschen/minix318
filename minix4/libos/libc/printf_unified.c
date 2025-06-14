/**
 * @file printf_unified.c
 * @brief Unified printf implementation
 * @details This file is a synthesized/unified implementation combining multiple
 *          legacy MINIX implementations into a single, modern, C23-compliant
 *          implementation for MINIX4.
 *
 * Original sources consolidated:
 * - minix4\libos_legacy\utilities\printf.c
 * - minix4\libos\lib_legacy\libucb\port\stdio\printf.c
 * - minix4\libos\lib_legacy\libsqlite\src\printf.c
 * - minix4\libos\lib_legacy\libfakekernel\common\printf.c
 * - minix4\libos\lib_legacy\libddekit\src\printf.c
 * - minix4\exokernel\kernel_legacy\printf.c
 * - minix4\exokernel\kernel_legacy\uts\common\os\printf.c
 * - minix4\exokernel\kernel_legacy\contrib\ast\src\lib\libast\stdio\printf.c
 * - minix4\exokernel\kernel_legacy\cmd\csh\printf.c
 * - minix4\exokernel\kernel_legacy\cmd\printf\printf.c
 * - ... and 3 more files
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
 * This file contains 13 very different implementations that
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
 * SOURCE 1/13: minix4\libos_legacy\utilities\printf.c
 * Size: 1,486 bytes, Lines: 86
 * Hash: 72e13c336e69...
 * ============================================================================ */

#include "types.h"
#include "stat.h"
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
}

static void
printint(int fd, int xx, int base, int sgn)
{
  static char digits[] = "0123456789ABCDEF";
  char buf[16];
  int i, neg;
  uint32_t x;

  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);
  if(neg)
    buf[i++] = '-';

  while(--i >= 0)
    putc(fd, buf[i]);
}

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, const char *fmt, ...)
{
  char *s;
  int c, i, state;
  uintptr_t *ap;

  state = 0;
  ap = (uintptr_t*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
        state = '%';
      } else {
        putc(fd, c);
      }
    } else if(state == '%'){
      if(c == 'd'){
        printint(fd, *ap, 10, 1);
        ap++;
      } else if(c == 'x' || c == 'p'){
        printint(fd, *ap, 16, 0);
        ap++;
      } else if(c == 's'){
        s = (char*)*ap;
        ap++;
        if(s == 0)
          s = "(null)";
        while(*s != 0){
          putc(fd, *s);
          s++;
        }
      } else if(c == 'c'){
        putc(fd, *ap);
        ap++;
      } else if(c == '%'){
        putc(fd, c);
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
    }
  }
}



/* ============================================================================
 * SOURCE 2/13: minix4\libos\lib_legacy\libucb\port\stdio\printf.c
 * Size: 1,524 bytes, Lines: 59
 * Hash: 655c474413c6...
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

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*LINTLIBRARY*/

#include <sys/types.h>
#include "file64.h"
#include <stdio.h>
#include <stdarg.h>
#include "libc.h"

int
printf(const char *format, ...)
{
	int count;
	va_list ap;

	va_start(ap, format);
	if (!(stdout->_flag & _IOWRT)) {
		/* if no write flag */
		if (stdout->_flag & _IORW) {
			/* if ok, cause read-write */
			stdout->_flag |= _IOWRT;
		} else {
			/* else error */
			return (EOF);
		}
	}
	count = _doprnt((char *) format, ap, stdout);
	va_end(ap);
	return (count);
}



/* ============================================================================
 * SOURCE 3/13: minix4\libos\lib_legacy\libsqlite\src\printf.c
 * Size: 31,012 bytes, Lines: 859
 * Hash: 108e6ab6f0aa...
 * ============================================================================ */

/*
** The "printf" code that follows dates from the 1980's.  It is in
** the public domain.  The original comments are included here for
** completeness.  They are very out-of-date but might be useful as
** an historical reference.  Most of the "enhancements" have been backed
** out so that the functionality is now the same as standard printf().
**
**************************************************************************
**
** The following modules is an enhanced replacement for the "printf" subroutines
** found in the standard C library.  The following enhancements are
** supported:
**
**      +  Additional functions.  The standard set of "printf" functions
**         includes printf, fprintf, sprintf, vprintf, vfprintf, and
**         vsprintf.  This module adds the following:
**
**           *  snprintf -- Works like sprintf, but has an extra argument
**                          which is the size of the buffer written to.
**
**           *  mprintf --  Similar to sprintf.  Writes output to memory
**                          obtained from malloc.
**
**           *  xprintf --  Calls a function to dispose of output.
**
**           *  nprintf --  No output, but returns the number of characters
**                          that would have been output by printf.
**
**           *  A v- version (ex: vsnprintf) of every function is also
**              supplied.
**
**      +  A few extensions to the formatting notation are supported:
**
**           *  The "=" flag (similar to "-") causes the output to be
**              be centered in the appropriately sized field.
**
**           *  The %b field outputs an integer in binary notation.
**
**           *  The %c field now accepts a precision.  The character output
**              is repeated by the number of times the precision specifies.
**
**           *  The %' field works like %c, but takes as its character the
**              next character of the format string, instead of the next
**              argument.  For example,  printf("%.78'-")  prints 78 minus
**              signs, the same as  printf("%.78c",'-').
**
**      +  When compiled using GCC on a SPARC, this version of printf is
**         faster than the library printf for SUN OS 4.1.
**
**      +  All functions are fully reentrant.
**
*/
#include "sqliteInt.h"

/*
** Conversion types fall into various categories as defined by the
** following enumeration.
*/
#define etRADIX       1 /* Integer types.  %d, %x, %o, and so forth */
#define etFLOAT       2 /* Floating point.  %f */
#define etEXP         3 /* Exponentional notation. %e and %E */
#define etGENERIC     4 /* Floating or exponential, depending on exponent. %g */
#define etSIZE        5 /* Return number of characters processed so far. %n */
#define etSTRING      6 /* Strings. %s */
#define etDYNSTRING   7 /* Dynamically allocated strings. %z */
#define etPERCENT     8 /* Percent symbol. %% */
#define etCHARX       9 /* Characters. %c */
#define etERROR      10 /* Used to indicate no such conversion type */
/* The rest are extensions, not normally found in printf() */
#define etCHARLIT    11 /* Literal characters.  %' */
#define etSQLESCAPE  12 /* Strings with '\'' doubled.  %q */
#define etSQLESCAPE2 13 /* Strings with '\'' doubled and enclosed in '',
                          NULL pointers replaced by SQL NULL.  %Q */
#define etTOKEN      14 /* a pointer to a Token structure */
#define etSRCLIST    15 /* a pointer to a SrcList */


/*
** An "etByte" is an 8-bit unsigned value.
*/
typedef unsigned char etByte;

/*
** Each builtin conversion character (ex: the 'd' in "%d") is described
** by an instance of the following structure
*/
typedef struct et_info {   /* Information about each format field */
  char fmttype;            /* The format field code letter */
  etByte base;             /* The base for radix conversion */
  etByte flags;            /* One or more of FLAG_ constants below */
  etByte type;             /* Conversion paradigm */
  char *charset;           /* The character set for conversion */
  char *prefix;            /* Prefix on non-zero values in alt format */
} et_info;

/*
** Allowed values for et_info.flags
*/
#define FLAG_SIGNED  1     /* True if the value to convert is signed */
#define FLAG_INTERN  2     /* True if for internal use only */


/*
** The following table is searched linearly, so it is good to put the
** most frequently used conversion types first.
*/
static et_info fmtinfo[] = {
  {  'd', 10, 1, etRADIX,      "0123456789",       0    },
  {  's',  0, 0, etSTRING,     0,                  0    },
  {  'z',  0, 2, etDYNSTRING,  0,                  0    },
  {  'q',  0, 0, etSQLESCAPE,  0,                  0    },
  {  'Q',  0, 0, etSQLESCAPE2, 0,                  0    },
  {  'c',  0, 0, etCHARX,      0,                  0    },
  {  'o',  8, 0, etRADIX,      "01234567",         "0"  },
  {  'u', 10, 0, etRADIX,      "0123456789",       0    },
  {  'x', 16, 0, etRADIX,      "0123456789abcdef", "x0" },
  {  'X', 16, 0, etRADIX,      "0123456789ABCDEF", "X0" },
  {  'f',  0, 1, etFLOAT,      0,                  0    },
  {  'e',  0, 1, etEXP,        "e",                0    },
  {  'E',  0, 1, etEXP,        "E",                0    },
  {  'g',  0, 1, etGENERIC,    "e",                0    },
  {  'G',  0, 1, etGENERIC,    "E",                0    },
  {  'i', 10, 1, etRADIX,      "0123456789",       0    },
  {  'n',  0, 0, etSIZE,       0,                  0    },
  {  '%',  0, 0, etPERCENT,    0,                  0    },
  {  'p', 10, 0, etRADIX,      "0123456789",       0    },
  {  'T',  0, 2, etTOKEN,      0,                  0    },
  {  'S',  0, 2, etSRCLIST,    0,                  0    },
};
#define etNINFO  (sizeof(fmtinfo)/sizeof(fmtinfo[0]))

/*
** If NOFLOATINGPOINT is defined, then none of the floating point
** conversions will work.
*/
#ifndef etNOFLOATINGPOINT
/*
** "*val" is a double such that 0.1 <= *val < 10.0
** Return the ascii code for the leading digit of *val, then
** multiply "*val" by 10.0 to renormalize.
**
** Example:
**     input:     *val = 3.14159
**     output:    *val = 1.4159    function return = '3'
**
** The counter *cnt is incremented each time.  After counter exceeds
** 16 (the number of significant digits in a 64-bit float) '0' is
** always returned.
*/
static int et_getdigit(LONGDOUBLE_TYPE *val, int *cnt){
  int digit;
  LONGDOUBLE_TYPE d;
  if( (*cnt)++ >= 16 ) return '0';
  digit = (int)*val;
  d = digit;
  digit += '0';
  *val = (*val - d)*10.0;
  return digit;
}
#endif

#define etBUFSIZE 1000  /* Size of the output buffer */

/*
** The root program.  All variations call this core.
**
** INPUTS:
**   func   This is a pointer to a function taking three arguments
**            1. A pointer to anything.  Same as the "arg" parameter.
**            2. A pointer to the list of characters to be output
**               (Note, this list is NOT null terminated.)
**            3. An integer number of characters to be output.
**               (Note: This number might be zero.)
**
**   arg    This is the pointer to anything which will be passed as the
**          first argument to "func".  Use it for whatever you like.
**
**   fmt    This is the format string, as in the usual print.
**
**   ap     This is a pointer to a list of arguments.  Same as in
**          vfprint.
**
** OUTPUTS:
**          The return value is the total number of characters sent to
**          the function "func".  Returns -1 on a error.
**
** Note that the order in which automatic variables are declared below
** seems to make a big difference in determining how fast this beast
** will run.
*/
static int vxprintf(
  void (*func)(void*,const char*,int),     /* Consumer of text */
  void *arg,                         /* First argument to the consumer */
  int useExtended,                   /* Allow extended %-conversions */
  const char *fmt,                   /* Format string */
  va_list ap                         /* arguments */
){
  int c;                     /* Next character in the format string */
  char *bufpt;               /* Pointer to the conversion buffer */
  int precision;             /* Precision of the current field */
  int length;                /* Length of the field */
  int idx;                   /* A general purpose loop counter */
  int count;                 /* Total number of characters output */
  int width;                 /* Width of the current field */
  etByte flag_leftjustify;   /* True if "-" flag is present */
  etByte flag_plussign;      /* True if "+" flag is present */
  etByte flag_blanksign;     /* True if " " flag is present */
  etByte flag_alternateform; /* True if "#" flag is present */
  etByte flag_zeropad;       /* True if field width constant starts with zero */
  etByte flag_long;          /* True if "l" flag is present */
  unsigned long longvalue;   /* Value for integer types */
  LONGDOUBLE_TYPE realvalue; /* Value for real types */
  et_info *infop;            /* Pointer to the appropriate info structure */
  char buf[etBUFSIZE];       /* Conversion buffer */
  char prefix;               /* Prefix character.  "+" or "-" or " " or '\0'. */
  etByte errorflag = 0;      /* True if an error is encountered */
  etByte xtype;              /* Conversion paradigm */
  char *zExtra;              /* Extra memory used for etTCLESCAPE conversions */
  static char spaces[] = "                                                  ";
#define etSPACESIZE (sizeof(spaces)-1)
#ifndef etNOFLOATINGPOINT
  int  exp;                  /* exponent of real numbers */
  double rounder;            /* Used for rounding floating point values */
  etByte flag_dp;            /* True if decimal point should be shown */
  etByte flag_rtz;           /* True if trailing zeros should be removed */
  etByte flag_exp;           /* True to force display of the exponent */
  int nsd;                   /* Number of significant digits returned */
#endif

  func(arg,"",0);
  count = length = 0;
  bufpt = 0;
  for(; (c=(*fmt))!=0; ++fmt){
    if( c!='%' ){
      int amt;
      bufpt = (char *)fmt;
      amt = 1;
      while( (c=(*++fmt))!='%' && c!=0 ) amt++;
      (*func)(arg,bufpt,amt);
      count += amt;
      if( c==0 ) break;
    }
    if( (c=(*++fmt))==0 ){
      errorflag = 1;
      (*func)(arg,"%",1);
      count++;
      break;
    }
    /* Find out what flags are present */
    flag_leftjustify = flag_plussign = flag_blanksign =
     flag_alternateform = flag_zeropad = 0;
    do{
      switch( c ){
        case '-':   flag_leftjustify = 1;     c = 0;   break;
        case '+':   flag_plussign = 1;        c = 0;   break;
        case ' ':   flag_blanksign = 1;       c = 0;   break;
        case '#':   flag_alternateform = 1;   c = 0;   break;
        case '0':   flag_zeropad = 1;         c = 0;   break;
        default:                                       break;
      }
    }while( c==0 && (c=(*++fmt))!=0 );
    /* Get the field width */
    width = 0;
    if( c=='*' ){
      width = va_arg(ap,int);
      if( width<0 ){
        flag_leftjustify = 1;
        width = -width;
      }
      c = *++fmt;
    }else{
      while( c>='0' && c<='9' ){
        width = width*10 + c - '0';
        c = *++fmt;
      }
    }
    if( width > etBUFSIZE-10 ){
      width = etBUFSIZE-10;
    }
    /* Get the precision */
    if( c=='.' ){
      precision = 0;
      c = *++fmt;
      if( c=='*' ){
        precision = va_arg(ap,int);
        if( precision<0 ) precision = -precision;
        c = *++fmt;
      }else{
        while( c>='0' && c<='9' ){
          precision = precision*10 + c - '0';
          c = *++fmt;
        }
      }
      /* Limit the precision to prevent overflowing buf[] during conversion */
      if( precision>etBUFSIZE-40 ) precision = etBUFSIZE-40;
    }else{
      precision = -1;
    }
    /* Get the conversion type modifier */
    if( c=='l' ){
      flag_long = 1;
      c = *++fmt;
    }else{
      flag_long = 0;
    }
    /* Fetch the info entry for the field */
    infop = 0;
    xtype = etERROR;
    for(idx=0; idx<etNINFO; idx++){
      if( c==fmtinfo[idx].fmttype ){
        infop = &fmtinfo[idx];
        if( useExtended || (infop->flags & FLAG_INTERN)==0 ){
          xtype = infop->type;
        }
        break;
      }
    }
    zExtra = 0;

    /*
    ** At this point, variables are initialized as follows:
    **
    **   flag_alternateform          TRUE if a '#' is present.
    **   flag_plussign               TRUE if a '+' is present.
    **   flag_leftjustify            TRUE if a '-' is present or if the
    **                               field width was negative.
    **   flag_zeropad                TRUE if the width began with 0.
    **   flag_long                   TRUE if the letter 'l' (ell) prefixed
    **                               the conversion character.
    **   flag_blanksign              TRUE if a ' ' is present.
    **   width                       The specified field width.  This is
    **                               always non-negative.  Zero is the default.
    **   precision                   The specified precision.  The default
    **                               is -1.
    **   xtype                       The class of the conversion.
    **   infop                       Pointer to the appropriate info struct.
    */
    switch( xtype ){
      case etRADIX:
        if( flag_long )  longvalue = va_arg(ap,long);
        else             longvalue = va_arg(ap,int);
#if 1
        /* For the format %#x, the value zero is printed "0" not "0x0".
        ** I think this is stupid. */
        if( longvalue==0 ) flag_alternateform = 0;
#else
        /* More sensible: turn off the prefix for octal (to prevent "00"),
        ** but leave the prefix for hex. */
        if( longvalue==0 && infop->base==8 ) flag_alternateform = 0;
#endif
        if( infop->flags & FLAG_SIGNED ){
          if( *(long*)&longvalue<0 ){
            longvalue = -*(long*)&longvalue;
            prefix = '-';
          }else if( flag_plussign )  prefix = '+';
          else if( flag_blanksign )  prefix = ' ';
          else                       prefix = 0;
        }else                        prefix = 0;
        if( flag_zeropad && precision<width-(prefix!=0) ){
          precision = width-(prefix!=0);
        }
        bufpt = &buf[etBUFSIZE-1];
        {
          register char *cset;      /* Use registers for speed */
          register int base;
          cset = infop->charset;
          base = infop->base;
          do{                                           /* Convert to ascii */
            *(--bufpt) = cset[longvalue%base];
            longvalue = longvalue/base;
          }while( longvalue>0 );
        }
        length = &buf[etBUFSIZE-1]-bufpt;
        for(idx=precision-length; idx>0; idx--){
          *(--bufpt) = '0';                             /* Zero pad */
        }
        if( prefix ) *(--bufpt) = prefix;               /* Add sign */
        if( flag_alternateform && infop->prefix ){      /* Add "0" or "0x" */
          char *pre, x;
          pre = infop->prefix;
          if( *bufpt!=pre[0] ){
            for(pre=infop->prefix; (x=(*pre))!=0; pre++) *(--bufpt) = x;
          }
        }
        length = &buf[etBUFSIZE-1]-bufpt;
        break;
      case etFLOAT:
      case etEXP:
      case etGENERIC:
        realvalue = va_arg(ap,double);
#ifndef etNOFLOATINGPOINT
        if( precision<0 ) precision = 6;         /* Set default precision */
        if( precision>etBUFSIZE-10 ) precision = etBUFSIZE-10;
        if( realvalue<0.0 ){
          realvalue = -realvalue;
          prefix = '-';
        }else{
          if( flag_plussign )          prefix = '+';
          else if( flag_blanksign )    prefix = ' ';
          else                         prefix = 0;
        }
        if( infop->type==etGENERIC && precision>0 ) precision--;
        rounder = 0.0;
#if 0
        /* Rounding works like BSD when the constant 0.4999 is used.  Wierd! */
        for(idx=precision, rounder=0.4999; idx>0; idx--, rounder*=0.1);
#else
        /* It makes more sense to use 0.5 */
        for(idx=precision, rounder=0.5; idx>0; idx--, rounder*=0.1);
#endif
        if( infop->type==etFLOAT ) realvalue += rounder;
        /* Normalize realvalue to within 10.0 > realvalue >= 1.0 */
        exp = 0;
        if( realvalue>0.0 ){
          while( realvalue>=1e8 && exp<=350 ){ realvalue *= 1e-8; exp+=8; }
          while( realvalue>=10.0 && exp<=350 ){ realvalue *= 0.1; exp++; }
          while( realvalue<1e-8 && exp>=-350 ){ realvalue *= 1e8; exp-=8; }
          while( realvalue<1.0 && exp>=-350 ){ realvalue *= 10.0; exp--; }
          if( exp>350 || exp<-350 ){
            bufpt = "NaN";
            length = 3;
            break;
          }
        }
        bufpt = buf;
        /*
        ** If the field type is etGENERIC, then convert to either etEXP
        ** or etFLOAT, as appropriate.
        */
        flag_exp = xtype==etEXP;
        if( xtype!=etFLOAT ){
          realvalue += rounder;
          if( realvalue>=10.0 ){ realvalue *= 0.1; exp++; }
        }
        if( xtype==etGENERIC ){
          flag_rtz = !flag_alternateform;
          if( exp<-4 || exp>precision ){
            xtype = etEXP;
          }else{
            precision = precision - exp;
            xtype = etFLOAT;
          }
        }else{
          flag_rtz = 0;
        }
        /*
        ** The "exp+precision" test causes output to be of type etEXP if
        ** the precision is too large to fit in buf[].
        */
        nsd = 0;
        if( xtype==etFLOAT && exp+precision<etBUFSIZE-30 ){
          flag_dp = (precision>0 || flag_alternateform);
          if( prefix ) *(bufpt++) = prefix;         /* Sign */
          if( exp<0 )  *(bufpt++) = '0';            /* Digits before "." */
          else for(; exp>=0; exp--) *(bufpt++) = et_getdigit(&realvalue,&nsd);
          if( flag_dp ) *(bufpt++) = '.';           /* The decimal point */
          for(exp++; exp<0 && precision>0; precision--, exp++){
            *(bufpt++) = '0';
          }
          while( (precision--)>0 ) *(bufpt++) = et_getdigit(&realvalue,&nsd);
          *(bufpt--) = 0;                           /* Null terminate */
          if( flag_rtz && flag_dp ){     /* Remove trailing zeros and "." */
            while( bufpt>=buf && *bufpt=='0' ) *(bufpt--) = 0;
            if( bufpt>=buf && *bufpt=='.' ) *(bufpt--) = 0;
          }
          bufpt++;                            /* point to next free slot */
        }else{    /* etEXP or etGENERIC */
          flag_dp = (precision>0 || flag_alternateform);
          if( prefix ) *(bufpt++) = prefix;   /* Sign */
          *(bufpt++) = et_getdigit(&realvalue,&nsd);  /* First digit */
          if( flag_dp ) *(bufpt++) = '.';     /* Decimal point */
          while( (precision--)>0 ) *(bufpt++) = et_getdigit(&realvalue,&nsd);
          bufpt--;                            /* point to last digit */
          if( flag_rtz && flag_dp ){          /* Remove tail zeros */
            while( bufpt>=buf && *bufpt=='0' ) *(bufpt--) = 0;
            if( bufpt>=buf && *bufpt=='.' ) *(bufpt--) = 0;
          }
          bufpt++;                            /* point to next free slot */
          if( exp || flag_exp ){
            *(bufpt++) = infop->charset[0];
            if( exp<0 ){ *(bufpt++) = '-'; exp = -exp; } /* sign of exp */
            else       { *(bufpt++) = '+'; }
            if( exp>=100 ){
              *(bufpt++) = (exp/100)+'0';                /* 100's digit */
              exp %= 100;
            }
            *(bufpt++) = exp/10+'0';                     /* 10's digit */
            *(bufpt++) = exp%10+'0';                     /* 1's digit */
          }
        }
        /* The converted number is in buf[] and zero terminated. Output it.
        ** Note that the number is in the usual order, not reversed as with
        ** integer conversions. */
        length = bufpt-buf;
        bufpt = buf;

        /* Special case:  Add leading zeros if the flag_zeropad flag is
        ** set and we are not left justified */
        if( flag_zeropad && !flag_leftjustify && length < width){
          int i;
          int nPad = width - length;
          for(i=width; i>=nPad; i--){
            bufpt[i] = bufpt[i-nPad];
          }
          i = prefix!=0;
          while( nPad-- ) bufpt[i++] = '0';
          length = width;
        }
#endif
        break;
      case etSIZE:
        *(va_arg(ap,int*)) = count;
        length = width = 0;
        break;
      case etPERCENT:
        buf[0] = '%';
        bufpt = buf;
        length = 1;
        break;
      case etCHARLIT:
      case etCHARX:
        c = buf[0] = (xtype==etCHARX ? va_arg(ap,int) : *++fmt);
        if( precision>=0 ){
          for(idx=1; idx<precision; idx++) buf[idx] = c;
          length = precision;
        }else{
          length =1;
        }
        bufpt = buf;
        break;
      case etSTRING:
      case etDYNSTRING:
        bufpt = va_arg(ap,char*);
        if( bufpt==0 ){
          bufpt = "";
        }else if( xtype==etDYNSTRING ){
          zExtra = bufpt;
        }
        length = strlen(bufpt);
        if( precision>=0 && precision<length ) length = precision;
        break;
      case etSQLESCAPE:
      case etSQLESCAPE2:
        {
          int i, j, n, c, isnull;
          char *arg = va_arg(ap,char*);
          isnull = arg==0;
          if( isnull ) arg = (xtype==etSQLESCAPE2 ? "NULL" : "(NULL)");
          for(i=n=0; (c=arg[i])!=0; i++){
            if( c=='\'' )  n++;
          }
          n += i + 1 + ((!isnull && xtype==etSQLESCAPE2) ? 2 : 0);
          if( n>etBUFSIZE ){
            bufpt = zExtra = sqliteMalloc( n );
            if( bufpt==0 ) return -1;
          }else{
            bufpt = buf;
          }
          j = 0;
          if( !isnull && xtype==etSQLESCAPE2 ) bufpt[j++] = '\'';
          for(i=0; (c=arg[i])!=0; i++){
            bufpt[j++] = c;
            if( c=='\'' ) bufpt[j++] = c;
          }
          if( !isnull && xtype==etSQLESCAPE2 ) bufpt[j++] = '\'';
          bufpt[j] = 0;
          length = j;
          if( precision>=0 && precision<length ) length = precision;
        }
        break;
      case etTOKEN: {
        Token *pToken = va_arg(ap, Token*);
        (*func)(arg, pToken->z, pToken->n);
        length = width = 0;
        break;
      }
      case etSRCLIST: {
        SrcList *pSrc = va_arg(ap, SrcList*);
        int k = va_arg(ap, int);
        struct SrcList_item *pItem = &pSrc->a[k];
        assert( k>=0 && k<pSrc->nSrc );
        if( pItem->zDatabase && pItem->zDatabase[0] ){
          (*func)(arg, pItem->zDatabase, strlen(pItem->zDatabase));
          (*func)(arg, ".", 1);
        }
        (*func)(arg, pItem->zName, strlen(pItem->zName));
        length = width = 0;
        break;
      }
      case etERROR:
        buf[0] = '%';
        buf[1] = c;
        errorflag = 0;
        idx = 1+(c!=0);
        (*func)(arg,"%",idx);
        count += idx;
        if( c==0 ) fmt--;
        break;
    }/* End switch over the format type */
    /*
    ** The text of the conversion is pointed to by "bufpt" and is
    ** "length" characters long.  The field width is "width".  Do
    ** the output.
    */
    if( !flag_leftjustify ){
      register int nspace;
      nspace = width-length;
      if( nspace>0 ){
        count += nspace;
        while( nspace>=etSPACESIZE ){
          (*func)(arg,spaces,etSPACESIZE);
          nspace -= etSPACESIZE;
        }
        if( nspace>0 ) (*func)(arg,spaces,nspace);
      }
    }
    if( length>0 ){
      (*func)(arg,bufpt,length);
      count += length;
    }
    if( flag_leftjustify ){
      register int nspace;
      nspace = width-length;
      if( nspace>0 ){
        count += nspace;
        while( nspace>=etSPACESIZE ){
          (*func)(arg,spaces,etSPACESIZE);
          nspace -= etSPACESIZE;
        }
        if( nspace>0 ) (*func)(arg,spaces,nspace);
      }
    }
    if( zExtra ){
      sqliteFree(zExtra);
    }
  }/* End for loop over the format string */
  return errorflag ? -1 : count;
} /* End of function */


/* This structure is used to store state information about the
** write to memory that is currently in progress.
*/
struct sgMprintf {
  char *zBase;     /* A base allocation */
  char *zText;     /* The string collected so far */
  int  nChar;      /* Length of the string so far */
  int  nTotal;     /* Output size if unconstrained */
  int  nAlloc;     /* Amount of space allocated in zText */
  void *(*xRealloc)(void*,int);  /* Function used to realloc memory */
};

/*
** This function implements the callback from vxprintf.
**
** This routine add nNewChar characters of text in zNewText to
** the sgMprintf structure pointed to by "arg".
*/
static void mout(void *arg, const char *zNewText, int nNewChar){
  struct sgMprintf *pM = (struct sgMprintf*)arg;
  pM->nTotal += nNewChar;
  if( pM->nChar + nNewChar + 1 > pM->nAlloc ){
    if( pM->xRealloc==0 ){
      nNewChar =  pM->nAlloc - pM->nChar - 1;
    }else{
      pM->nAlloc = pM->nChar + nNewChar*2 + 1;
      if( pM->zText==pM->zBase ){
        pM->zText = pM->xRealloc(0, pM->nAlloc);
        if( pM->zText && pM->nChar ){
          memcpy(pM->zText, pM->zBase, pM->nChar);
        }
      }else{
        pM->zText = pM->xRealloc(pM->zText, pM->nAlloc);
      }
    }
  }
  if( pM->zText ){
    if( nNewChar>0 ){
      memcpy(&pM->zText[pM->nChar], zNewText, nNewChar);
      pM->nChar += nNewChar;
    }
    pM->zText[pM->nChar] = 0;
  }
}

/*
** This routine is a wrapper around xprintf() that invokes mout() as
** the consumer.
*/
static char *base_vprintf(
  void *(*xRealloc)(void*,int),   /* Routine to realloc memory. May be NULL */
  int useInternal,                /* Use internal %-conversions if true */
  char *zInitBuf,                 /* Initially write here, before mallocing */
  int nInitBuf,                   /* Size of zInitBuf[] */
  const char *zFormat,            /* format string */
  va_list ap                      /* arguments */
){
  struct sgMprintf sM;
  sM.zBase = sM.zText = zInitBuf;
  sM.nChar = sM.nTotal = 0;
  sM.nAlloc = nInitBuf;
  sM.xRealloc = xRealloc;
  vxprintf(mout, &sM, useInternal, zFormat, ap);
  if( xRealloc ){
    if( sM.zText==sM.zBase ){
      sM.zText = xRealloc(0, sM.nChar+1);
      memcpy(sM.zText, sM.zBase, sM.nChar+1);
    }else if( sM.nAlloc>sM.nChar+10 ){
      sM.zText = xRealloc(sM.zText, sM.nChar+1);
    }
  }
  return sM.zText;
}

/*
** Realloc that is a real function, not a macro.
*/
static void *printf_realloc(void *old, int size){
  return sqliteRealloc(old,size);
}

/*
** Print into memory obtained from sqliteMalloc().  Use the internal
** %-conversion extensions.
*/
char *sqliteVMPrintf(const char *zFormat, va_list ap){
  char zBase[1000];
  return base_vprintf(printf_realloc, 1, zBase, sizeof(zBase), zFormat, ap);
}

/*
** Print into memory obtained from sqliteMalloc().  Use the internal
** %-conversion extensions.
*/
char *sqliteMPrintf(const char *zFormat, ...){
  va_list ap;
  char *z;
  char zBase[1000];
  va_start(ap, zFormat);
  z = base_vprintf(printf_realloc, 1, zBase, sizeof(zBase), zFormat, ap);
  va_end(ap);
  return z;
}

/*
** Print into memory obtained from malloc().  Do not use the internal
** %-conversion extensions.  This routine is for use by external users.
*/
char *sqlite_mprintf(const char *zFormat, ...){
  va_list ap;
  char *z;
  char zBuf[200];

  va_start(ap,zFormat);
  z = base_vprintf((void*(*)(void*,int))realloc, 0,
                   zBuf, sizeof(zBuf), zFormat, ap);
  va_end(ap);
  return z;
}

/* This is the varargs version of sqlite_mprintf.
*/
char *sqlite_vmprintf(const char *zFormat, va_list ap){
  char zBuf[200];
  return base_vprintf((void*(*)(void*,int))realloc, 0,
                      zBuf, sizeof(zBuf), zFormat, ap);
}

/*
** sqlite_snprintf() works like snprintf() except that it ignores the
** current locale settings.  This is important for SQLite because we
** are not able to use a "," as the decimal point in place of "." as
** specified by some locales.
*/
char *sqlite_snprintf(int n, char *zBuf, const char *zFormat, ...){
  char *z;
  va_list ap;

  va_start(ap,zFormat);
  z = base_vprintf(0, 0, zBuf, n, zFormat, ap);
  va_end(ap);
  return z;
}

/*
** The following four routines implement the varargs versions of the
** sqlite_exec() and sqlite_get_table() interfaces.  See the sqlite.h
** header files for a more detailed description of how these interfaces
** work.
**
** These routines are all just simple wrappers.
*/
int sqlite_exec_printf(
  sqlite *db,                   /* An open database */
  const char *sqlFormat,        /* printf-style format string for the SQL */
  sqlite_callback xCallback,    /* Callback function */
  void *pArg,                   /* 1st argument to callback function */
  char **errmsg,                /* Error msg written here */
  ...                           /* Arguments to the format string. */
){
  va_list ap;
  int rc;

  va_start(ap, errmsg);
  rc = sqlite_exec_vprintf(db, sqlFormat, xCallback, pArg, errmsg, ap);
  va_end(ap);
  return rc;
}
int sqlite_exec_vprintf(
  sqlite *db,                   /* An open database */
  const char *sqlFormat,        /* printf-style format string for the SQL */
  sqlite_callback xCallback,    /* Callback function */
  void *pArg,                   /* 1st argument to callback function */
  char **errmsg,                /* Error msg written here */
  va_list ap                    /* Arguments to the format string. */
){
  char *zSql;
  int rc;

  zSql = sqlite_vmprintf(sqlFormat, ap);
  rc = sqlite_exec(db, zSql, xCallback, pArg, errmsg);
  free(zSql);
  return rc;
}
int sqlite_get_table_printf(
  sqlite *db,            /* An open database */
  const char *sqlFormat, /* printf-style format string for the SQL */
  char ***resultp,       /* Result written to a char *[]  that this points to */
  int *nrow,             /* Number of result rows written here */
  int *ncol,             /* Number of result columns written here */
  char **errmsg,         /* Error msg written here */
  ...                    /* Arguments to the format string */
){
  va_list ap;
  int rc;

  va_start(ap, errmsg);
  rc = sqlite_get_table_vprintf(db, sqlFormat, resultp, nrow, ncol, errmsg, ap);
  va_end(ap);
  return rc;
}
int sqlite_get_table_vprintf(
  sqlite *db,            /* An open database */
  const char *sqlFormat, /* printf-style format string for the SQL */
  char ***resultp,       /* Result written to a char *[]  that this points to */
  int *nrow,             /* Number of result rows written here */
  int *ncolumn,          /* Number of result columns written here */
  char **errmsg,         /* Error msg written here */
  va_list ap             /* Arguments to the format string */
){
  char *zSql;
  int rc;

  zSql = sqlite_vmprintf(sqlFormat, ap);
  rc = sqlite_get_table(db, zSql, resultp, nrow, ncolumn, errmsg);
  free(zSql);
  return rc;
}



/* ============================================================================
 * SOURCE 4/13: minix4\libos\lib_legacy\libfakekernel\common\printf.c
 * Size: 4,818 bytes, Lines: 217
 * Hash: 0837efeaaa71...
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
 * Copyright (c) 2005, 2010, Oracle and/or its affiliates. All rights reserved.
 * Copyright (c) 2012 by Delphix. All rights reserved.
 * Copyright 2021 Tintri by DDN, Inc. All rights reserved.
 * Copyright 2022 RackTop Systems, Inc.
 * Copyright (c) 2018, Joyent, Inc.
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/varargs.h>
#include <sys/systm.h>
#include <sys/cmn_err.h>
#include <sys/log.h>
#include <upanic.h>

#include <fakekernel.h>

void	debug_enter(char *);

char *volatile panicstr;
va_list  panicargs;
char panicbuf[512];

int aok;

static const int
ce_flags[CE_IGNORE] = { SL_NOTE, SL_NOTE, SL_WARN, SL_FATAL };
static const char
ce_prefix[CE_IGNORE][10] = { "", "NOTICE: ", "WARNING: ", "" };
static const char
ce_suffix[CE_IGNORE][2] = { "", "\n", "\n", "" };


/*
 * This function is just a stub, exported NODIRECT so that
 * comsumers like fksmbd can provide their own.
 * (One that actually prints the messages.)
 *
 * It's used by fakekernel_cprintf() below.
 * The flags are SL_... from strlog.h
 */
/* ARGSUSED */
void
fakekernel_putlog(char *msg, size_t len, int flags)
{
}

/*
 * fakekernel_cprintf() corresponds to os/printf.c:cprintf()
 * This formats the message and calls fakekernel_putlog().
 * It's exported NODIRECT to allow replacment.
 * The flags are SL_... from strlog.h
 */
void
fakekernel_cprintf(const char *fmt, va_list adx, int flags,
    const char *prefix, const char *suffix)
{
	size_t bufsize = LOG_MSGSIZE;
	char buf[LOG_MSGSIZE];
	char *bufp = buf;
	char *msgp, *bufend;
	size_t len;

	if (strchr("^!?", fmt[0]) != NULL) {
		if (fmt[0] == '^')
			flags |= SL_CONSONLY;
		else if (fmt[0] == '!')
			flags |= SL_LOGONLY;
		fmt++;
	}

	bufend = bufp + bufsize;
	msgp = bufp;
	msgp += snprintf(msgp, bufend - msgp, "[fake_kernel] ");
	msgp += snprintf(msgp, bufend - msgp, prefix);
	msgp += vsnprintf(msgp, bufend - msgp, fmt, adx);
	msgp += snprintf(msgp, bufend - msgp, suffix);
	len = msgp - bufp;

	fakekernel_putlog(bufp, len, flags);
}

/* ARGSUSED */
void
vzprintf(zoneid_t zoneid, const char *fmt, va_list adx)
{
	fakekernel_cprintf(fmt, adx, SL_CONSOLE | SL_NOTE, "", "");
}

/*PRINTFLIKE2*/
void
zprintf(zoneid_t zoneid, const char *fmt, ...)
{
	va_list adx;

	va_start(adx, fmt);
	vzprintf(zoneid, fmt, adx);
	va_end(adx);
}

/*
 * "User-level crash dump", if you will.
 */
void
vpanic(const char *fmt, va_list adx)
{
	va_list tmpargs;

	panicstr = (char *)fmt;
	va_copy(panicargs, adx);

	va_copy(tmpargs, adx);
	fakekernel_cprintf(fmt, tmpargs, SL_FATAL, "fatal: ", "\n");

	(void) vsnprintf(panicbuf, sizeof (panicbuf), fmt, adx);
	debug_enter(panicbuf);

	/* Call libc`upanic() so that mdb ::status works */
	upanic(panicbuf, sizeof (panicbuf));
}

void
panic(const char *fmt, ...)
{
	va_list adx;

	va_start(adx, fmt);
	vpanic(fmt, adx);
	va_end(adx);
}

void
fm_panic(const char *fmt, ...)
{
	va_list adx;

	va_start(adx, fmt);
	vpanic(fmt, adx);
	va_end(adx);
}

void
vcmn_err(int ce, const char *fmt, va_list adx)
{

	if (ce == CE_PANIC)
		vpanic(fmt, adx);
	if (ce >= CE_IGNORE)
		return;

	fakekernel_cprintf(fmt, adx, ce_flags[ce] | SL_CONSOLE,
	    ce_prefix[ce], ce_suffix[ce]);
}

/*PRINTFLIKE2*/
void
cmn_err(int ce, const char *fmt, ...)
{
	va_list adx;

	va_start(adx, fmt);
	vcmn_err(ce, fmt, adx);
	va_end(adx);
}

/* ARGSUSED */
void
debug_enter(char *str)
{
	/* Just a place for a break point. */
}

void
assfail(const char *a, const char *f, int l)
{
	if (!aok)
		panic("assertion failed: %s, file: %s, line: %d", a, f, l);

	fprintf(stderr, "ASSERTION CAUGHT: %s, file: %s, line: %d\n", a, f, l);
}

void
assfail3(const char *a, uintmax_t lv, const char *op, uintmax_t rv,
    const char *f, int l)
{
	if (!aok) {
		panic("assertion failed: %s (0x%llx %s 0x%llx), file: %s, "
		    "line: %d", a, (u_longlong_t)lv, op, (u_longlong_t)rv,
		    f, l);
	}

	fprintf(stderr, "ASSERTION CAUGHT: %s (0x%llx %s 0x%llx), file: %s, "
	    "line: %d\n", a, (u_longlong_t)lv, op, (u_longlong_t)rv,
	    f, l);
}



/* ============================================================================
 * SOURCE 5/13: minix4\libos\lib_legacy\libddekit\src\printf.c
 * Size: 1,036 bytes, Lines: 34
 * Hash: eba31d9197a6...
 * ============================================================================ */

#include "common.h"
#include <ddekit/printf.h>

/****************************************************************************/
/*     ddekit_print                                                         */
/****************************************************************************/
int ddekit_print(const char* c)
{
	return ddekit_printf(c);
}

/****************************************************************************/
/*     ddekit_printf                                                        */
/****************************************************************************/
int ddekit_printf(const char* fmt, ...)
{
	int r;
	va_list va;

	va_start(va,fmt);
	r = vprintf(fmt, va);
	va_end(va);
	
	return r;
}

/****************************************************************************/
/*     ddekit_vprintf                                                       */
/****************************************************************************/
int ddekit_vprintf(const char *fmt, va_list va) 
{
	return vprintf(fmt, va);
}



/* ============================================================================
 * SOURCE 6/13: minix4\exokernel\kernel_legacy\printf.c
 * Size: 596 bytes, Lines: 36
 * Hash: d0f57cf80210...
 * ============================================================================ */

/* Copyright (c) 1979 Regents of the University of California */
/*
 * Hacked "printf" which prints through putchar.
 * DONT USE WITH STDIO!
 */
printf(fmt, args)
char *fmt;
{
	_doprnt(fmt, &args, 0);
}

_strout(count, string, adjust, foo, fillch)
register char *string;
register int count;
int adjust;
register struct { int a[6]; } *foo;
{

	if (foo != 0)
		abort();
	while (adjust < 0) {
		if (*string=='-' && fillch=='0') {
			putchar(*string++);
			count--;
		}
		putchar(fillch);
		adjust++;
	}
	while (--count>=0)
		putchar(*string++);
	while (adjust) {
		putchar(fillch);
		adjust--;
	}
}



/* ============================================================================
 * SOURCE 7/13: minix4\exokernel\kernel_legacy\uts\common\os\printf.c
 * Size: 8,786 bytes, Lines: 368
 * Hash: ddbfd187d0bf...
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
 *
 * Copyright 2015 Nexenta Systems, Inc.  All rights reserved.
 * Copyright 2016 Joyent, Inc.
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <sys/inline.h>
#include <sys/varargs.h>
#include <sys/systm.h>
#include <sys/conf.h>
#include <sys/cmn_err.h>
#include <sys/syslog.h>
#include <sys/log.h>
#include <sys/proc.h>
#include <sys/vnode.h>
#include <sys/session.h>
#include <sys/stream.h>
#include <sys/kmem.h>
#include <sys/kobj.h>
#include <sys/atomic.h>
#include <sys/console.h>
#include <sys/cpuvar.h>
#include <sys/modctl.h>
#include <sys/reboot.h>
#include <sys/debug.h>
#include <sys/panic.h>
#include <sys/spl.h>
#include <sys/zone.h>
#include <sys/sunddi.h>

/*
 * In some debugging situations it's useful to log all messages to panicbuf,
 * which is persistent across reboots (on most platforms).  The range
 * panicbuf[panicbuf_log..PANICBUFSIZE-1] may be used for this purpose.
 * By default, panicbuf_log == PANICBUFSIZE and no messages are logged.
 * To enable panicbuf logging, set panicbuf_log to a small value, say 1K;
 * this will reserve 1K for panic information and 7K for message logging.
 */
uint32_t panicbuf_log = PANICBUFSIZE;
uint32_t panicbuf_index = PANICBUFSIZE;

int aask, aok;
static int ce_to_sl[CE_IGNORE] = { SL_NOTE, SL_NOTE, SL_WARN, SL_FATAL };
static char ce_prefix[CE_IGNORE][10] = { "", "NOTICE: ", "WARNING: ", "" };
static char ce_suffix[CE_IGNORE][2] = { "", "\n", "\n", "" };

static void
cprintf(const char *fmt, va_list adx, int sl, const char *prefix,
    const char *suffix, void *site, int mid, int sid, int level,
    zoneid_t zoneid, dev_info_t *dip)
{
	uint32_t msgid;
	size_t bufsize = LOG_MSGSIZE;
	char buf[LOG_MSGSIZE];
	char *bufp = buf;
	char *body, *msgp, *bufend;
	mblk_t *mp;
	int s, on_intr;
	size_t len;

	s = splhi();
	on_intr = CPU_ON_INTR(CPU) ||
	    (interrupts_unleashed && (spltoipl(s) > LOCK_LEVEL));
	splx(s);

	ASSERT(zoneid == GLOBAL_ZONEID || !on_intr);

	STRLOG_MAKE_MSGID(fmt, msgid);

	if (strchr("^!?", fmt[0]) != NULL) {
		if (fmt[0] == '^')
			sl |= SL_CONSONLY;
		else if (fmt[0] == '!' ||
		    (prefix[0] == '\0' && !(boothowto & RB_VERBOSE)))
			sl = (sl & ~(SL_USER | SL_NOTE)) | SL_LOGONLY;
		fmt++;
	}

	if ((sl & SL_USER) && (MUTEX_HELD(&pidlock) || on_intr)) {
		zoneid = getzoneid();
		sl = sl & ~(SL_USER | SL_LOGONLY) | SL_CONSOLE;
	}

retry:
	bufend = bufp + bufsize;
	msgp = bufp;
	body = msgp += snprintf(msgp, bufend - msgp,
	    "%s: [ID %u FACILITY_AND_PRIORITY] ",
	    mod_containing_pc(site), msgid);
	msgp += snprintf(msgp, bufend - msgp, prefix);
	if (dip != NULL)
		msgp += snprintf(msgp, bufend - msgp, "%s%d: ",
		    ddi_driver_name(dip), ddi_get_instance(dip));
	msgp += vsnprintf(msgp, bufend - msgp, fmt, adx);
	msgp += snprintf(msgp, bufend - msgp, suffix);
	len = strlen(body);

	if (((sl & SL_CONSONLY) && panicstr) ||
	    (zoneid == GLOBAL_ZONEID && log_global.lz_active == 0)) {
		console_printf("%s", body);
		goto out;
	}

	if (msgp - bufp >= bufsize && !on_intr) {
		ASSERT(bufp == buf);
		bufsize = msgp - bufp + 1;
		bufp = kmem_alloc(bufsize, KM_NOSLEEP);
		if (bufp != NULL)
			goto retry;
		bufsize = LOG_MSGSIZE;
		bufp = buf;
	}

	mp = log_makemsg(mid, sid, level, sl, LOG_KERN, bufp,
	    MIN(bufsize, msgp - bufp + 1), on_intr);
	if (mp == NULL) {
		if ((sl & (SL_CONSOLE | SL_LOGONLY)) == SL_CONSOLE && !on_intr)
			console_printf("%s", body);
		goto out;
	}

	if (sl & SL_USER) {
		ssize_t resid;
		sess_t *sp;

		if ((sp = tty_hold()) != NULL) {
			if (sp->s_vp != NULL)
				(void) vn_rdwr(UIO_WRITE, sp->s_vp, body,
				    len, 0LL, UIO_SYSSPACE, FAPPEND,
				    (rlim64_t)LOG_HIWAT, kcred, &resid);
			tty_rele(sp);
		}
	}

	if (on_intr && !panicstr) {
		(void) putq(log_intrq, mp);
		softcall((void (*)(void *))log_flushq, log_intrq);
	} else {
		log_sendmsg(mp, zoneid);
	}
out:
	if (panicbuf_log + len < PANICBUFSIZE) {
		uint32_t old, new;
		do {
			old = panicbuf_index;
			new = old + len;
			if (new >= PANICBUFSIZE)
				new = panicbuf_log + len;
		} while (atomic_cas_32(&panicbuf_index, old, new) != old);
		bcopy(body, &panicbuf[new - len], len);
	}
	if (bufp != buf)
		kmem_free(bufp, bufsize);
}

void
vzprintf(zoneid_t zoneid, const char *fmt, va_list adx)
{
	cprintf(fmt, adx, SL_CONSOLE | SL_NOTE, "", "", caller(), 0, 0, 0,
	    zoneid, NULL);
}

void
vprintf(const char *fmt, va_list adx)
{
	vzprintf(GLOBAL_ZONEID, fmt, adx);
}

/*PRINTFLIKE1*/
void
printf(const char *fmt, ...)
{
	va_list adx;

	va_start(adx, fmt);
	cprintf(fmt, adx, SL_CONSOLE | SL_NOTE, "", "", caller(), 0, 0, 0,
	    GLOBAL_ZONEID, NULL);
	va_end(adx);
}

/*PRINTFLIKE2*/
void
zprintf(zoneid_t zoneid, const char *fmt, ...)
{
	va_list adx;

	va_start(adx, fmt);
	cprintf(fmt, adx, SL_CONSOLE | SL_NOTE, "", "", caller(), 0, 0, 0,
	    zoneid, NULL);
	va_end(adx);
}

void
vuprintf(const char *fmt, va_list adx)
{
	va_list adxcp;
	va_copy(adxcp, adx);

	/* Message the user tty, if any, and the global zone syslog */
	cprintf(fmt, adx, SL_CONSOLE | SL_LOGONLY | SL_USER | SL_NOTE,
	    "", "", caller(), 0, 0, 0, GLOBAL_ZONEID, NULL);

	/* Now message the local zone syslog */
	if (!INGLOBALZONE(curproc))
		cprintf(fmt, adxcp, SL_CONSOLE | SL_LOGONLY | SL_NOTE,
		    "", "", caller(), 0, 0, 0, getzoneid(), NULL);

	va_end(adxcp);
}

/*PRINTFLIKE1*/
void
uprintf(const char *fmt, ...)
{
	va_list adx;

	va_start(adx, fmt);

	vuprintf(fmt, adx);

	va_end(adx);
}

static void
vzdcmn_err(zoneid_t zoneid, void *site, int ce, const char *fmt, va_list adx,
    dev_info_t *dip)
{
	if (ce == CE_PANIC) {
		panic_dip = dip;
		vpanic(fmt, adx);
	}

	if ((uint_t)ce < CE_IGNORE) {
		cprintf(fmt, adx, ce_to_sl[ce] | SL_CONSOLE,
		    ce_prefix[ce], ce_suffix[ce], site, 0, 0, 0,
		    zoneid, dip);
	}
}

void
vzcmn_err(zoneid_t zoneid, int ce, const char *fmt, va_list adx)
{
	vzdcmn_err(zoneid, caller(), ce, fmt, adx, NULL);
}

void
vcmn_err(int ce, const char *fmt, va_list adx)
{
	vzdcmn_err(GLOBAL_ZONEID, caller(), ce, fmt, adx, NULL);
}

/*PRINTFLIKE3*/
void
vdev_err(dev_info_t *dip, int ce, const char *fmt, va_list adx)
{
	vzdcmn_err(GLOBAL_ZONEID, caller(), ce, fmt, adx, dip);
}

/*PRINTFLIKE2*/
void
cmn_err(int ce, const char *fmt, ...)
{
	va_list adx;

	va_start(adx, fmt);
	vzdcmn_err(GLOBAL_ZONEID, caller(), ce, fmt, adx, NULL);
	va_end(adx);
}

/*PRINTFLIKE3*/
void
zcmn_err(zoneid_t zoneid, int ce, const char *fmt, ...)
{
	va_list adx;

	va_start(adx, fmt);
	vzdcmn_err(zoneid, caller(), ce, fmt, adx, NULL);
	va_end(adx);
}

/*PRINTFLIKE3*/
void
dev_err(dev_info_t *dip, int ce, char *fmt, ...)
{
	va_list adx;

	va_start(adx, fmt);
	vzdcmn_err(GLOBAL_ZONEID, caller(), ce, fmt, adx, dip);
	va_end(adx);
}

void
assfail(const char *a, const char *f, int l)
{
	if (aask)  {
		printf("ASSERTION CAUGHT: %s, file: %s, line: %d", a, f, l);
		debug_enter(NULL);
	}

	if (!aok && !panicstr)
		panic("assertion failed: %s, file: %s, line: %d", a, f, l);
}

void
assfail3(const char *a, uintmax_t lv, const char *op, uintmax_t rv,
    const char *f, int l)
{
	if (aask)  {
		printf("ASSERTION CAUGHT: %s (0x%llx %s 0x%llx), file: %s, "
		    "line: %d", a, (u_longlong_t)lv, op, (u_longlong_t)rv,
		    f, l);
		debug_enter(NULL);
	}

	if (!aok && !panicstr)
		panic("assertion failed: %s (0x%llx %s 0x%llx), file: %s, "
		    "line: %d", a, (u_longlong_t)lv, op, (u_longlong_t)rv,
		    f, l);
}

int
strlog(short mid, short sid, char level, ushort_t sl, char *fmt, ...)
{
	if (sl & log_global.lz_active) {
		va_list adx;
		va_start(adx, fmt);
		cprintf(fmt, adx, sl, "", "", caller(), mid, sid, level,
		    GLOBAL_ZONEID, NULL);
		va_end(adx);
	}
	return (1);
}

int
vstrlog(short mid, short sid, char level, ushort_t sl, char *fmt, va_list adx)
{
	if (sl & log_global.lz_active)
		cprintf(fmt, adx, sl, "", "", caller(), mid, sid, level,
		    GLOBAL_ZONEID, NULL);
	return (1);
}



/* ============================================================================
 * SOURCE 8/13: minix4\exokernel\kernel_legacy\contrib\ast\src\lib\libast\stdio\printf.c
 * Size: 1,720 bytes, Lines: 37
 * Hash: 6339341d3199...
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

#include "stdhdr.h"

int
printf(const char* fmt, ...)
{
	va_list	args;
	int	v;

	va_start(args, fmt);
	v = sfvprintf(sfstdout, fmt, args);
	va_end(args);
	return v;
}



/* ============================================================================
 * SOURCE 9/13: minix4\exokernel\kernel_legacy\cmd\csh\printf.c
 * Size: 15,859 bytes, Lines: 763
 * Hash: 14f965a6431c...
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
 * Hacked "printf" which prints through putbyte and Putchar.
 * putbyte() is used to send a pure byte, which might be a part
 * of a mutlibyte character, mainly for %s.  A control character
 * for putbyte() may be QUOTE'd meaning not to convert it to ^x
 * sequence.  In all other cases Putchar() is used to send a character
 * in tchar (== wchar_t + * optional QUOE.)
 * DONT USE WITH STDIO!
 * This printf has been hacked again so that it understands tchar string
 * when the format specifier %t is used.  Also %c has been expanded
 * to take a tchar character as well as normal int.
 * %t is supported in its simplest form; no width or precision will
 * be understood.
 * Assumption here is that sizeof(tchar)<=sizeof(int) so that tchar is
 * passed as int.  Otherwise, %T must be specified instead of %c to
 * print a character in tchar.
 */

#include <stdarg.h>
#include <values.h>
#include "sh.h" /* For tchar. */

#define	HIBITLL		(1ULL << 63)

void _print(char *format, va_list *args);

static char *p;

int
printf(const char *format, ...)
{
	va_list stupid;

	p = (char *)gettext(format);
	va_start(stupid, format);
	_print(p, &stupid);
	va_end(stupid);

	return (0);
}

/*
 *	Floating-point code is included or not, depending
 *	on whether the preprocessor variable FLOAT is 1 or 0.
 */

/* Maximum number of digits in any integer (long) representation */
#define	MAXDIGS	20

/* Convert a digit character to the corresponding number */
#define	tonumber(x)	((x) - '0')

/* Convert a number between 0 and 9 to the corresponding digit */
#define	todigit(x)	((x) + '0')

/* Maximum total number of digits in E format */
#define	MAXECVT	17

/* Maximum number of digits after decimal point in F format */
#define	MAXFCVT	60

/* Maximum significant figures in a floating-point number */
#define	MAXFSIG	17

/* Maximum number of characters in an exponent */
#define	MAXESIZ	4

/* Maximum (positive) exponent or greater */
#define	MAXEXP	40



#define	max(a, b) ((a) > (b) ? (a) : (b))
#define	min(a, b) ((a) < (b) ? (a) : (b))

/* If this symbol is nonzero, allow '0' as a flag */
#define	FZERO 1

#if FLOAT
/*
 *	System-supplied routines for floating conversion
 */
char *fcvt();
char *ecvt();
#endif

void
_print(char *format, va_list *args)
{
	/* Current position in format */
	char *cp;

	/* Starting and ending points for value to be printed */
	char *bp, *p;
	tchar *tbp, *tep;	/* For "%t". */
	tchar tcbuf[2];		/* For "%c" or "%T". */

	/* Field width and precision */
	int width, prec;

	/* Format code */
	char fcode;

	/* Number of padding zeroes required on the left */
	int lzero;

	/* Flags - nonzero if corresponding character appears in format */
	bool length;		/* l */
	bool double_length;	/* ll */
	bool fplus;		/* + */
	bool fminus;		/* - */
	bool fblank;		/* blank */
	bool fsharp;		/* # */
#if FZERO
	bool fzero;		/* 0 */
#endif

	/* Pointer to sign, "0x", "0X", or empty */
	char *prefix;
#if FLOAT
	/* Exponent or empty */
	char *suffix;

	/* Buffer to create exponent */
	char expbuf[MAXESIZ + 1];

	/* Number of padding zeroes required on the right */
	int rzero;

	/* The value being converted, if real */
	double dval;

	/* Output values from fcvt and ecvt */
	int decpt, sign;

	/* Scratch */
	int k;

	/* Values are developed in this buffer */
	char buf[max(MAXDIGS, max(MAXFCVT + DMAXEXP, MAXECVT) + 1)];
#else
	char buf[MAXDIGS];
#endif
	/* The value being converted, if integer */
	long long val;

	/* Set to point to a translate table for digits of whatever radix */
	char *tab;

	/* Work variables */
	int n, hradix, lowbit;

	cp = format;

	/*
	 *	The main loop -- this loop goes through one iteration
	 *	for each ordinary character or format specification.
	 */
	while (*cp)
		if (*cp != '%') {
			/* Ordinary (non-%) character */
			putbyte (*cp++);
		} else {
			/*
			 *	% has been found.
			 *	First, parse the format specification.
			 */

			/* Scan the <flags> */
			fplus = fminus = fblank = fsharp = 0;
#if FZERO
			fzero = 0;
#endif
scan:
			switch (*++cp) {
			case '+':
				fplus = 1;
				goto scan;
			case '-':
				fminus = 1;
				goto scan;
			case ' ':
				fblank = 1;
				goto scan;
			case '#':
				fsharp = 1;
				goto scan;
#if FZERO
			case '0':
				fzero = 1;
				goto scan;
#endif
			}

			/* Scan the field width */
			if (*cp == '*') {
				width = va_arg (*args, int);
				if (width < 0) {
					width = -width;
					fminus = 1;
				}
				cp++;
			} else {
				width = 0;
				while (isdigit(*cp)) {
					n = tonumber(*cp++);
					width = width * 10 + n;
				}
			}

			/* Scan the precision */
			if (*cp == '.') {

				/* '*' instead of digits? */
				if (*++cp == '*') {
					prec = va_arg(*args, int);
					cp++;
				} else {
					prec = 0;
					while (isdigit(*cp)) {
						n = tonumber(*cp++);
						prec = prec * 10 + n;
					}
				}
			} else {
				prec = -1;
			}

			/* Scan the length modifier */
			double_length = length = 0;
			switch (*cp) {
			case 'l':
				if (*(cp + 1) == 'l') {
					cp++;
					double_length = 1;
				} else {
					length = 1;
				}
				/* No break */
			case 'h':
				cp++;
			}

			/*
			 *	The character addressed by cp must be the
			 *	format letter -- there is nothing left for
			 *	it to be.
			 *
			 *	The status of the +, -, #, blank, and 0
			 *	flags are reflected in the variables
			 *	"fplus", "fminus", "fsharp", "fblank",
			 *	and "fzero", respectively.
			 *	"width" and "prec" contain numbers
			 *	corresponding to the digit strings
			 *	before and after the decimal point,
			 *	respectively. If there was no decimal
			 *	point, "prec" is -1.
			 *
			 *	The following switch sets things up
			 *	for printing.  What ultimately gets
			 *	printed will be padding blanks, a prefix,
			 *	left padding zeroes, a value, right padding
			 *	zeroes, a suffix, and more padding
			 *	blanks.  Padding blanks will not appear
			 *	simultaneously on both the left and the
			 *	right.  Each case in this switch will
			 *	compute the value, and leave in several
			 *	variables the information necessary to
			 *	construct what is to be printed.
			 *
			 *	The prefix is a sign, a blank, "0x", "0X",
			 *	or null, and is addressed by "prefix".
			 *
			 *	The suffix is either null or an exponent,
			 *	and is addressed by "suffix".
			 *
			 *	The value to be printed starts at "bp"
			 *	and continues up to and not including "p".
			 *
			 *	"lzero" and "rzero" will contain the number
			 *	of padding zeroes required on the left
			 *	and right, respectively.  If either of
			 *	these variables is negative, it will be
			 *	treated as if it were zero.
			 *
			 *	The number of padding blanks, and whether
			 *	they go on the left or the right, will be
			 *	computed on exit from the switch.
			 */

			lzero = 0;
			prefix = "";
#if FLOAT
			rzero = lzero;
			suffix = prefix;
#endif
			switch (fcode = *cp++) {

			/*
			 *	fixed point representations
			 *
			 *	"hradix" is half the radix for the conversion.
			 *	Conversion is unsigned unless fcode is 'd'.
			 *	HIBITLL is 1000...000 binary, and is equal to
			 *		the maximum negative number.
			 *	We assume a 2's complement machine
			 */

			case 'D':
			case 'U':
				length = 1;
			case 'd':
			case 'u':
				hradix = 5;
				goto fixed;

			case 'O':
				length = 1;
			case 'o':
				hradix = 4;
				goto fixed;

			case 'X':
			case 'x':
				hradix = 8;

fixed:
				/* Establish default precision */
				if (prec < 0) {
					prec = 1;
				}

				/* Fetch the argument to be printed */
				if (double_length) {
					val = va_arg(*args, long long);
				} else if (length) {
					val = va_arg(*args, long);
				} else if (fcode == 'd') {
					val = va_arg(*args, int);
				} else {
					val = va_arg(*args, unsigned);
				}

				/* If signed conversion, establish sign */
				if (fcode == 'd' || fcode == 'D') {
					if (val < 0) {
						prefix = "-";
						/*
						 *	Negate, checking in
						 *	advance for possible
						 *	overflow.
						 */
						if (val != HIBITLL) {
							val = -val;
						}
					} else if (fplus) {
						prefix = "+";
					} else if (fblank) {
						prefix = " ";
					}
				}
#if FZERO
				if (fzero) {
					int n = width - strlen(prefix);
					if (n > prec) {
						prec = n;
					}
				}
#endif
				/* Set translate table for digits */
				if (fcode == 'X') {
					tab = "0123456789ABCDEF";
				} else {
					tab = "0123456789abcdef";
				}

				/* Develop the digits of the value */
				p = bp = buf + MAXDIGS;
				while (val) {
					lowbit = val & 1;
					val = (val >> 1) & ~HIBITLL;
					*--bp = tab[val % hradix * 2 + lowbit];
					val /= hradix;
				}

				/* Calculate padding zero requirement */
				lzero = bp - p + prec;

				/* Handle the # flag */
				if (fsharp && bp != p) {
					switch (fcode) {
					case 'o':
						if (lzero < 1)
							lzero = 1;
						break;
					case 'x':
						prefix = "0x";
						break;
					case 'X':
						prefix = "0X";
						break;
					}
				}

				break;
#if FLOAT
			case 'E':
			case 'e':
				/*
				 *	E-format.  The general strategy
				 *	here is fairly easy: we take
				 *	what ecvt gives us and re-format it.
				 */

				/* Establish default precision */
				if (prec < 0) {
					prec = 6;
				}

				/* Fetch the value */
				dval = va_arg(*args, double);

				/* Develop the mantissa */
				bp = ecvt(dval,
				    min(prec + 1, MAXECVT),
				    &decpt,
				    &sign);

				/* Determine the prefix */
e_merge:
				if (sign) {
					prefix = "-";
				} else if (fplus) {
					prefix = "+";
				} else if (fblank) {
					prefix = " ";
				}

				/* Place the first digit in the buffer */
				p = &buf[0];
				*p++ = *bp != '\0' ? *bp++ : '0';

				/* Put in a decimal point if needed */
				if (prec != 0 || fsharp) {
					*p++ = '.';
				}

				/* Create the rest of the mantissa */
				rzero = prec;
				while (rzero > 0 && *bp != '\0') {
					--rzero;
					*p++ = *bp++;
				}

				bp = &buf[0];

				/* Create the exponent */
				suffix = &expbuf[MAXESIZ];
				*suffix = '\0';
				if (dval != 0) {
					n = decpt - 1;
					if (n < 0) {
						n = -n;
					}
					while (n != 0) {
						*--suffix = todigit(n % 10);
						n /= 10;
					}
				}

				/* Prepend leading zeroes to the exponent */
				while (suffix > &expbuf[MAXESIZ - 2]) {
					*--suffix = '0';
				}

				/* Put in the exponent sign */
				*--suffix = (decpt > 0 || dval == 0) ?
				    '+' : '-';

				/* Put in the e */
				*--suffix = isupper(fcode) ? 'E' : 'e';

				break;

			case 'f':
				/*
				 *	F-format floating point.  This is
				 *	a good deal less simple than E-format.
				 *	The overall strategy will be to call
				 *	fcvt, reformat its result into buf,
				 *	and calculate how many trailing
				 *	zeroes will be required.  There will
				 *	never be any leading zeroes needed.
				 */

				/* Establish default precision */
				if (prec < 0) {
					prec = 6;
				}

				/* Fetch the value */
				dval = va_arg(*args, double);

				/* Do the conversion */
				bp = fcvt(dval,
				    min(prec, MAXFCVT),
				    &decpt,
				    &sign);

				/* Determine the prefix */
f_merge:
				if (sign && decpt > -prec &&
				    *bp != '\0' && *bp != '0') {
					prefix = "-";
				} else if (fplus) {
					prefix = "+";
				} else if (fblank) {
					prefix = " ";
				}

				/* Initialize buffer pointer */
				p = &buf[0];

				/* Emit the digits before the decimal point */
				n = decpt;
				k = 0;
				if (n <= 0) {
					*p++ = '0';
				} else {
					do {
						if (*bp == '\0' ||
						    k >= MAXFSIG) {
							*p++ = '0';
						} else {
							*p++ = *bp++;
							++k;
						}
					} while (--n != 0);
				}

				/* Decide whether we need a decimal point */
				if (fsharp || prec > 0) {
					*p++ = '.';
				}

				/* Digits (if any) after the decimal point */
				n = min(prec, MAXFCVT);
				rzero = prec - n;
				while (--n >= 0) {
					if (++decpt <= 0 || *bp == '\0' ||
					    k >= MAXFSIG) {
						*p++ = '0';
					} else {
						*p++ = *bp++;
						++k;
					}
				}

				bp = &buf[0];

				break;

			case 'G':
			case 'g':
				/*
				 *	g-format.  We play around a bit
				 *	and then jump into e or f, as needed.
				 */

				/* Establish default precision */
				if (prec < 0) {
					prec = 6;
				}

				/* Fetch the value */
				dval = va_arg(*args, double);

				/* Do the conversion */
				bp = ecvt(dval,
				    min(prec, MAXECVT),
				    &decpt,
				    &sign);
				if (dval == 0) {
					decpt = 1;
				}

				k = prec;
				if (!fsharp) {
					n = strlen(bp);
					if (n < k) {
						k = n;
					}
					while (k >= 1 && bp[k-1] == '0') {
						--k;
					}
				}

				if (decpt < -3 || decpt > prec) {
					prec = k - 1;
					goto e_merge;
				} else {
					prec = k - decpt;
					goto f_merge;
				}

#endif
			case 'c':
#ifdef MBCHAR_1 /* sizeof(int)>=sizeof(tchar) */
/*
 * A tchar arg is passed as int so we used the normal %c to specify
 * such an arugument.
 */
				tcbuf[0] = va_arg(*args, int);
				tbp = &tcbuf[0];
				tep = tbp + 1;
				fcode = 't'; /* Fake the rest of code. */
				break;
#else
/*
 * We would have to invent another new format speficier such as "%T" to
 * take a tchar arg.  Let's worry about when that time comes.
 */
				/*
				 * Following code take care of a char arg
				 * only.
				 */
				buf[0] = va_arg(*args, int);
				bp = &buf[0];
				p = bp + 1;
				break;
			case 'T': /* Corresponding arg is tchar. */
				tcbuf[0] = va_arg(*args, tchar);
				tbp = &tcbuf[0];
				tep = tbp + 1;
				fcode = 't'; /* Fake the rest of code. */
				break;
#endif
			case 's':
				bp = va_arg(*args, char *);
				if (bp == 0) {
nullstr:				bp = "(null)";
					p = bp + strlen("(null)");
					break;
				}
				if (prec < 0) {
					prec = MAXINT;
				}
				for (n = 0; *bp++ != '\0' && n < prec; n++)
					;
				p = --bp;
				bp -= n;
				break;

			case 't':
				/*
				 * Special format specifier "%t" tells
				 * printf() to print char strings written
				 * as tchar string.
				 */
				tbp = va_arg(*args, tchar *);
				if (tbp == 0) {
					fcode = 's'; /* Act as if it were %s. */
					goto nullstr;
				}
				if (prec < 0) {
					prec = MAXINT;
				}
				for (n = 0; *tbp++ != 0 && n < prec; n++)
					;
				tep = --tbp;
				tbp -= n;

				/*
				 * Just to make the following padding
				 * calculation not to go very crazy...
				 */
				bp = NULL;
				p = bp + n;
				break;

			case '\0':
				cp--;
				break;

			default:
				p = bp = &fcode;
				p++;
				break;

			}
			if (fcode != '\0') {
				/* Calculate number of padding blanks */
				int nblank;
				nblank = width
#if FLOAT
					- (rzero < 0 ? 0:  rzero)
					- strlen(suffix)
#endif
					- (p - bp)
					- (lzero < 0 ? 0 : lzero)
					- strlen(prefix);

				/* Blanks on left if required */
				if (!fminus) {
					while (--nblank >= 0) {
						Putchar(' ');
					}
				}

				/* Prefix, if any */
				while (*prefix != '\0') {
					Putchar(*prefix++);
				}

				/* Zeroes on the left */
				while (--lzero >= 0) {
					Putchar('0');
				}

				/* The value itself */
				if (fcode == 't') {	/* %t is special. */
					while (tbp < tep) {
					    Putchar(*tbp++);
					}
				} else {	/* For rest of the cases. */
					while (bp < p) {
					    putbyte(*bp++);
					}
				}
#if FLOAT
				/* Zeroes on the right */
				while (--rzero >= 0)
					Putchar('0');

				/* The suffix */
				while (*suffix != '\0') {
					Putchar(*suffix++);
				}
#endif
				/* Blanks on the right if required */
				if (fminus) {
					while (--nblank >= 0) {
						Putchar(' ');
					}
				}
			}
		}
}



/* ============================================================================
 * SOURCE 10/13: minix4\exokernel\kernel_legacy\cmd\printf\printf.c
 * Size: 13,288 bytes, Lines: 644
 * Hash: d2928dec5eed...
 * ============================================================================ */

/*
 * Copyright 2014 Garrett D'Amore <garrett@damore.org>
 * Copyright 2010 Nexenta Systems, Inc.  All rights reserved.
 * Copyright (c) 1989, 1993
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
 * 4. Neither the name of the University nor the names of its contributors
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

#include <sys/types.h>

#include <err.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <alloca.h>
#include <ctype.h>
#include <locale.h>
#include <note.h>

#define	warnx1(a, b, c)		warnx(a)
#define	warnx2(a, b, c)		warnx(a, b)
#define	warnx3(a, b, c)		warnx(a, b, c)

#define	PTRDIFF(x, y)	((uintptr_t)(x) - (uintptr_t)(y))

#define	_(x)	gettext(x)

#define	PF(f, func) do {						\
	char *b = NULL;							\
	if (havewidth)							\
		if (haveprec)						\
			(void) asprintf(&b, f, fieldwidth, precision, func); \
		else							\
			(void) asprintf(&b, f, fieldwidth, func);	\
	else if (haveprec)						\
		(void) asprintf(&b, f, precision, func);		\
	else								\
		(void) asprintf(&b, f, func);				\
	if (b) {							\
		(void) fputs(b, stdout);				\
		free(b);						\
	}								\
_NOTE(CONSTCOND) } while (0)

static int	 asciicode(void);
static char	*doformat(char *, int *);
static int	 escape(char *, int, size_t *);
static int	 getchr(void);
static int	 getfloating(long double *, int);
static int	 getint(int *);
static int	 getnum(intmax_t *, uintmax_t *, int);
static const char
		*getstr(void);
static char	*mknum(char *, char);
static void	 usage(void);

static const char digits[] = "0123456789";

static int  myargc;
static char **myargv;
static char **gargv;
static char **maxargv;

int
main(int argc, char *argv[])
{
	size_t len;
	int end, rval;
	char *format, *fmt, *start;

	(void) setlocale(LC_ALL, "");

	argv++;
	argc--;

	/*
	 * POSIX says: Standard utilities that do not accept options,
	 * but that do accept operands, shall recognize "--" as a
	 * first argument to be discarded.
	 */
	if (argc && strcmp(argv[0], "--") == 0) {
		argc--;
		argv++;
	}

	if (argc < 1) {
		usage();
		return (1);
	}

	/*
	 * Basic algorithm is to scan the format string for conversion
	 * specifications -- once one is found, find out if the field
	 * width or precision is a '*'; if it is, gather up value.  Note,
	 * format strings are reused as necessary to use up the provided
	 * arguments, arguments of zero/null string are provided to use
	 * up the format string.
	 */
	fmt = format = *argv;
	(void) escape(fmt, 1, &len);	/* backslash interpretation */
	rval = end = 0;
	gargv = ++argv;

	for (;;) {
		maxargv = gargv;

		myargv = gargv;
		for (myargc = 0; gargv[myargc]; myargc++)
			/* nop */;
		start = fmt;
		while (fmt < format + len) {
			if (fmt[0] == '%') {
				(void) fwrite(start, 1, PTRDIFF(fmt, start),
				    stdout);
				if (fmt[1] == '%') {
					/* %% prints a % */
					(void) putchar('%');
					fmt += 2;
				} else {
					fmt = doformat(fmt, &rval);
					if (fmt == NULL)
						return (1);
					end = 0;
				}
				start = fmt;
			} else
				fmt++;
			if (gargv > maxargv)
				maxargv = gargv;
		}
		gargv = maxargv;

		if (end == 1) {
			warnx1(_("missing format character"), NULL, NULL);
			return (1);
		}
		(void) fwrite(start, 1, PTRDIFF(fmt, start), stdout);
		if (!*gargv)
			return (rval);
		/* Restart at the beginning of the format string. */
		fmt = format;
		end = 1;
	}
	/* NOTREACHED */
}


static char *
doformat(char *fmt, int *rval)
{
	static const char skip1[] = "#'-+ 0";
	int fieldwidth, haveprec, havewidth, mod_ldbl, precision;
	char convch, nextch;
	char *start;
	char **fargv;
	char *dptr;
	int l;

	start = alloca(strlen(fmt) + 1);

	dptr = start;
	*dptr++ = '%';
	*dptr = 0;

	fmt++;

	/* look for "n$" field index specifier */
	l = strspn(fmt, digits);
	if ((l > 0) && (fmt[l] == '$')) {
		int idx = atoi(fmt);
		if (idx <= myargc) {
			gargv = &myargv[idx - 1];
		} else {
			gargv = &myargv[myargc];
		}
		if (gargv > maxargv) {
			maxargv = gargv;
		}
		fmt += l + 1;

		/* save format argument */
		fargv = gargv;
	} else {
		fargv = NULL;
	}

	/* skip to field width */
	while (*fmt && strchr(skip1, *fmt) != NULL) {
		*dptr++ = *fmt++;
		*dptr = 0;
	}


	if (*fmt == '*') {

		fmt++;
		l = strspn(fmt, digits);
		if ((l > 0) && (fmt[l] == '$')) {
			int idx = atoi(fmt);
			if (fargv == NULL) {
				warnx1(_("incomplete use of n$"), NULL, NULL);
				return (NULL);
			}
			if (idx <= myargc) {
				gargv = &myargv[idx - 1];
			} else {
				gargv = &myargv[myargc];
			}
			fmt += l + 1;
		} else if (fargv != NULL) {
			warnx1(_("incomplete use of n$"), NULL, NULL);
			return (NULL);
		}

		if (getint(&fieldwidth))
			return (NULL);
		if (gargv > maxargv) {
			maxargv = gargv;
		}
		havewidth = 1;

		*dptr++ = '*';
		*dptr = 0;
	} else {
		havewidth = 0;

		/* skip to possible '.', get following precision */
		while (isdigit(*fmt)) {
			*dptr++ = *fmt++;
			*dptr = 0;
		}
	}

	if (*fmt == '.') {
		/* precision present? */
		fmt++;
		*dptr++ = '.';

		if (*fmt == '*') {

			fmt++;
			l = strspn(fmt, digits);
			if ((l > 0) && (fmt[l] == '$')) {
				int idx = atoi(fmt);
				if (fargv == NULL) {
					warnx1(_("incomplete use of n$"),
					    NULL, NULL);
					return (NULL);
				}
				if (idx <= myargc) {
					gargv = &myargv[idx - 1];
				} else {
					gargv = &myargv[myargc];
				}
				fmt += l + 1;
			} else if (fargv != NULL) {
				warnx1(_("incomplete use of n$"), NULL, NULL);
				return (NULL);
			}

			if (getint(&precision))
				return (NULL);
			if (gargv > maxargv) {
				maxargv = gargv;
			}
			haveprec = 1;
			*dptr++ = '*';
			*dptr = 0;
		} else {
			haveprec = 0;

			/* skip to conversion char */
			while (isdigit(*fmt)) {
				*dptr++ = *fmt++;
				*dptr = 0;
			}
		}
	} else
		haveprec = 0;
	if (!*fmt) {
		warnx1(_("missing format character"), NULL, NULL);
		return (NULL);
	}
	*dptr++ = *fmt;
	*dptr = 0;

	/*
	 * Look for a length modifier.  POSIX doesn't have these, so
	 * we only support them for floating-point conversions, which
	 * are extensions.  This is useful because the L modifier can
	 * be used to gain extra range and precision, while omitting
	 * it is more likely to produce consistent results on different
	 * architectures.  This is not so important for integers
	 * because overflow is the only bad thing that can happen to
	 * them, but consider the command  printf %a 1.1
	 */
	if (*fmt == 'L') {
		mod_ldbl = 1;
		fmt++;
		if (!strchr("aAeEfFgG", *fmt)) {
			warnx2(_("bad modifier L for %%%c"), *fmt, NULL);
			return (NULL);
		}
	} else {
		mod_ldbl = 0;
	}

	/* save the current arg offset, and set to the format arg */
	if (fargv != NULL) {
		gargv = fargv;
	}

	convch = *fmt;
	nextch = *++fmt;

	*fmt = '\0';
	switch (convch) {
	case 'b': {
		size_t len;
		char *p;
		int getout;

		p = strdup(getstr());
		if (p == NULL) {
			warnx2("%s", strerror(ENOMEM), NULL);
			return (NULL);
		}
		getout = escape(p, 0, &len);
		(void) fputs(p, stdout);
		free(p);

		if (getout)
			exit(*rval);
		break;
	}
	case 'c': {
		char p;

		p = getchr();
		PF(start, p);
		break;
	}
	case 's': {
		const char *p;

		p = getstr();
		PF(start, p);
		break;
	}
	case 'd': case 'i': case 'o': case 'u': case 'x': case 'X': {
		char *f;
		intmax_t val;
		uintmax_t uval;
		int signedconv;

		signedconv = (convch == 'd' || convch == 'i');
		if ((f = mknum(start, convch)) == NULL)
			return (NULL);
		if (getnum(&val, &uval, signedconv))
			*rval = 1;
		if (signedconv)
			PF(f, val);
		else
			PF(f, uval);
		break;
	}
	case 'e': case 'E':
	case 'f': case 'F':
	case 'g': case 'G':
	case 'a': case 'A': {
		long double p;

		if (getfloating(&p, mod_ldbl))
			*rval = 1;
		if (mod_ldbl)
			PF(start, p);
		else
			PF(start, (double)p);
		break;
	}
	default:
		warnx2(_("illegal format character %c"), convch, NULL);
		return (NULL);
	}
	*fmt = nextch;

	/* return the gargv to the next element */
	return (fmt);
}

static char *
mknum(char *str, char ch)
{
	static char *copy;
	static size_t copy_size;
	char *newcopy;
	size_t len, newlen;

	len = strlen(str) + 2;
	if (len > copy_size) {
		newlen = ((len + 1023) >> 10) << 10;
		if ((newcopy = realloc(copy, newlen)) == NULL) {
			warnx2("%s", strerror(ENOMEM), NULL);
			return (NULL);
		}
		copy = newcopy;
		copy_size = newlen;
	}

	(void) memmove(copy, str, len - 3);
	copy[len - 3] = 'j';
	copy[len - 2] = ch;
	copy[len - 1] = '\0';
	return (copy);
}

static int
escape(char *fmt, int percent, size_t *len)
{
	char *save, *store, c;
	int value;

	for (save = store = fmt; ((c = *fmt) != 0); ++fmt, ++store) {
		if (c != '\\') {
			*store = c;
			continue;
		}
		switch (*++fmt) {
		case '\0':		/* EOS, user error */
			*store = '\\';
			*++store = '\0';
			*len = PTRDIFF(store, save);
			return (0);
		case '\\':		/* backslash */
		case '\'':		/* single quote */
			*store = *fmt;
			break;
		case 'a':		/* bell/alert */
			*store = '\a';
			break;
		case 'b':		/* backspace */
			*store = '\b';
			break;
		case 'c':
			if (!percent) {
				*store = '\0';
				*len = PTRDIFF(store, save);
				return (1);
			}
			*store = 'c';
			break;
		case 'f':		/* form-feed */
			*store = '\f';
			break;
		case 'n':		/* newline */
			*store = '\n';
			break;
		case 'r':		/* carriage-return */
			*store = '\r';
			break;
		case 't':		/* horizontal tab */
			*store = '\t';
			break;
		case 'v':		/* vertical tab */
			*store = '\v';
			break;
					/* octal constant */
		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
			c = (!percent && *fmt == '0') ? 4 : 3;
			for (value = 0;
			    c-- && *fmt >= '0' && *fmt <= '7'; ++fmt) {
				value <<= 3;
				value += *fmt - '0';
			}
			--fmt;
			if (percent && value == '%') {
				*store++ = '%';
				*store = '%';
			} else
				*store = (char)value;
			break;
		default:
			*store = *fmt;
			break;
		}
	}
	*store = '\0';
	*len = PTRDIFF(store, save);
	return (0);
}

static int
getchr(void)
{
	if (!*gargv)
		return ('\0');
	return ((int)**gargv++);
}

static const char *
getstr(void)
{
	if (!*gargv)
		return ("");
	return (*gargv++);
}

static int
getint(int *ip)
{
	intmax_t val;
	uintmax_t uval;
	int rval;

	if (getnum(&val, &uval, 1))
		return (1);
	rval = 0;
	if (val < INT_MIN || val > INT_MAX) {
		warnx3("%s: %s", *gargv, strerror(ERANGE));
		rval = 1;
	}
	*ip = (int)val;
	return (rval);
}

static int
getnum(intmax_t *ip, uintmax_t *uip, int signedconv)
{
	char *ep;
	int rval;

	if (!*gargv) {
		*ip = *uip = 0;
		return (0);
	}
	if (**gargv == '"' || **gargv == '\'') {
		if (signedconv)
			*ip = asciicode();
		else
			*uip = asciicode();
		return (0);
	}
	rval = 0;
	errno = 0;
	if (signedconv)
		*ip = strtoimax(*gargv, &ep, 0);
	else
		*uip = strtoumax(*gargv, &ep, 0);
	if (ep == *gargv) {
		warnx2(_("%s: expected numeric value"), *gargv, NULL);
		rval = 1;
	} else if (*ep != '\0') {
		warnx2(_("%s: not completely converted"), *gargv, NULL);
		rval = 1;
	}
	if (errno == ERANGE) {
		warnx3("%s: %s", *gargv, strerror(ERANGE));
		rval = 1;
	}
	++gargv;
	return (rval);
}

static int
getfloating(long double *dp, int mod_ldbl)
{
	char *ep;
	int rval;

	if (!*gargv) {
		*dp = 0.0;
		return (0);
	}
	if (**gargv == '"' || **gargv == '\'') {
		*dp = asciicode();
		return (0);
	}
	rval = 0;
	errno = 0;
	if (mod_ldbl)
		*dp = strtold(*gargv, &ep);
	else
		*dp = strtod(*gargv, &ep);
	if (ep == *gargv) {
		warnx2(_("%s: expected numeric value"), *gargv, NULL);
		rval = 1;
	} else if (*ep != '\0') {
		warnx2(_("%s: not completely converted"), *gargv, NULL);
		rval = 1;
	}
	if (errno == ERANGE) {
		warnx3("%s: %s", *gargv, strerror(ERANGE));
		rval = 1;
	}
	++gargv;
	return (rval);
}

static int
asciicode(void)
{
	int ch;

	ch = **gargv;
	if (ch == '\'' || ch == '"')
		ch = (*gargv)[1];
	++gargv;
	return (ch);
}

static void
usage(void)
{
	(void) fprintf(stderr, _("usage: printf format [arguments ...]\n"));
}



/* ============================================================================
 * SOURCE 11/13: minix4\exokernel\kernel_legacy\cmd\vi\port\printf.c
 * Size: 9,385 bytes, Lines: 405
 * Hash: f528f576c349...
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
 * Copyright 2005 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*
 * University Copyright- Copyright (c) 1982, 1986, 1988
 * The Regents of the University of California
 * All Rights Reserved
 *
 * University Acknowledgment- Portions of this document are derived from
 * software developed by the University of California, Berkeley, and its
 * contributors.
 */

#include <stdarg.h>
#include <stdlib.h>
#include <limits.h>
extern short putoctal;
/*
 * This version of printf is compatible with the Version 7 C
 * printf. The differences are only minor except that
 * viprintf assumes it is to print through putchar. Version 7
 * printf is more general (and is much larger) and includes
 * provisions for floating point.
 */


#define MAXOCT	11	/* Maximum octal digits in a long */
#define MAXINT	32767	/* largest normal length positive integer */
#define BIG	1000000000  /* largest power of 10 less than an unsigned long */
#define MAXDIGS	10	/* number of digits in BIG */

static int width, sign, fill;

unsigned char *_p_dconv();
void _p_emit(unsigned char *, unsigned char *);

/*VARARGS*/
void
viprintf(unsigned char *fmt, ...)
{
	va_list ap;
	unsigned char fcode;
	wchar_t wfcode;
	int prec;
	int length,mask1,nbits,n;
	int length2;
	long int mask2, num;
	unsigned char *bptr;
	unsigned char *ptr;
	unsigned char buf[134];

	va_start(ap, fmt);
	for (;;) {
		/* process format string first */
		for (;;) {
			length2 = mbtowc(&wfcode, (char *)fmt, MB_LEN_MAX);
			if(length2 <= 0) {
				if (*fmt == '\0') {
					fmt++;
					return;
				}
				putchar(*fmt++);
			} else {
				if (wfcode == '%') {
					fmt++;
					break;
				}
				/* ordinary (non-%) character */
				putchar(wfcode);
				fmt += length2;
			}
		}
		/* length modifier: -1 for h, 1 for l, 0 for none */
		length = 0;
		/* check for a leading - sign */
		sign = 0;
		if (*fmt == '-') {
			sign++;
			fmt++;
		}
		/* a '0' may follow the - sign */
		/* this is the requested fill character */
		fill = 1;
		if (*fmt == '0') {
			fill--;
			fmt++;
		}

		/* Now comes a digit string which may be a '*' */
		if (*fmt == '*') {
			width = va_arg(ap, int);
			if (width < 0) {
				width = -width;
				sign = !sign;
			}
			fmt++;
		}
		else {
			width = 0;
			while (*fmt>='0' && *fmt<='9')
				width = width * 10 + (*fmt++ - '0');
		}

		/* maybe a decimal point followed by more digits (or '*') */
		if (*fmt=='.') {
			if (*++fmt == '*') {
				prec = va_arg(ap, int);
				fmt++;
			}
			else {
				prec = 0;
				while (*fmt>='0' && *fmt<='9')
					prec = prec * 10 + (*fmt++ - '0');
			}
		}
		else
			prec = -1;

		/*
		 * At this point, "sign" is nonzero if there was
		 * a sign, "fill" is 0 if there was a leading
		 * zero and 1 otherwise, "width" and "prec"
		 * contain numbers corresponding to the digit
		 * strings before and after the decimal point,
		 * respectively, and "fmt" addresses the next
		 * character after the whole mess. If there was
		 * no decimal point, "prec" will be -1.
		 */
		switch (*fmt) {
			case 'L':
			case 'l':
				length = 2;
				/* FALLTHROUGH */
			case 'h':
			case 'H':
				length--;
				fmt++;
				break;
		}

		/*
		 * At exit from the following switch, we will
		 * emit the characters starting at "bptr" and
		 * ending at "ptr"-1, unless fcode is '\0'.
		 */
		switch (fcode = *fmt++) {
			/* process characters and strings first */
			case 'c':
				buf[0] = va_arg(ap, int);
				ptr = bptr = &buf[0];
				if (buf[0] != '\0')
					ptr++;
				break;
			case 's':
				bptr = va_arg(ap,unsigned char *);
				if (bptr==0)
					bptr = (unsigned char *)"(null pointer)";
				if (prec < 0)
					prec = MAXINT;
				for (n=0; *bptr++ && n < prec; n++) ;
				ptr = --bptr;
				bptr -= n;
				break;
			case 'O':
				length = 1;
				fcode = 'o';
				/* FALLTHROUGH */
			case 'o':
			case 'X':
			case 'x':
				if (length > 0)
					num = va_arg(ap,long);
				else
					num = (unsigned)va_arg(ap,int);
				if (fcode=='o') {
					mask1 = 0x7;
					mask2 = 0x1fffffffL;
					nbits = 3;
				}
				else {
					mask1 = 0xf;
					mask2 = 0x0fffffffL;
					nbits = 4;
				}
				n = (num!=0);
				bptr = buf + MAXOCT + 3;
				/* shift and mask for speed */
				do
				    if (((int) num & mask1) < 10)
					*--bptr = ((int) num & mask1) + 060;
				    else
					*--bptr = ((int) num & mask1) + 0127;
				while (num = (num >> nbits) & mask2);

				if (fcode=='o') {
					if (n)
						*--bptr = '0';
				}
				else
					if (!sign && fill <= 0) {
						putchar('0');
						putchar(fcode);
						width -= 2;
					}
					else {
						*--bptr = fcode;
						*--bptr = '0';
					}
				ptr = buf + MAXOCT + 3;
				break;
			case 'D':
			case 'U':
			case 'I':
				length = 1;
				fcode = fcode + 'a' - 'A';
				/* FALLTHROUGH */
			case 'd':
			case 'i':
			case 'u':
				if (length > 0)
					num = va_arg(ap,long);
				else {
					n = va_arg(ap,int);
					if (fcode=='u')
						num = (unsigned) n;
					else
						num = (long) n;
				}
				if (n = (fcode != 'u' && num < 0))
					num = -num;
				/* now convert to digits */
				bptr = _p_dconv(num, buf);
				if (n)
					*--bptr = '-';
				if (fill == 0)
					fill = -1;
				ptr = buf + MAXDIGS + 1;
				break;
			default:
				/* not a control character,
				 * print it.
				 */
				ptr = bptr = &fcode;
				ptr++;
				break;
			}
			if (fcode != '\0')
				_p_emit(bptr,ptr);
	}
	va_end(ap);
}

/* _p_dconv converts the unsigned long integer "value" to
 * printable decimal and places it in "buffer", right-justified.
 * The value returned is the address of the first non-zero character,
 * or the address of the last character if all are zero.
 * The result is NOT null terminated, and is MAXDIGS characters long,
 * starting at buffer[1] (to allow for insertion of a sign).
 *
 * This program assumes it is running on 2's complement machine
 * with reasonable overflow treatment.
 */
unsigned char *
_p_dconv(value, buffer)
	long value;
	unsigned char *buffer;
{
	unsigned char *bp;
	int svalue;
	int n;
	long lval;

	bp = buffer;

	/* zero is a special case */
	if (value == 0) {
		bp += MAXDIGS;
		*bp = '0';
		return(bp);
	}

	/* develop the leading digit of the value in "n" */
	n = 0;
	while (value < 0) {
		value -= BIG;	/* will eventually underflow */
		n++;
	}
	while ((lval = value - BIG) >= 0) {
		value = lval;
		n++;
	}

	/* stash it in buffer[1] to allow for a sign */
	bp[1] = n + '0';
	/*
	 * Now develop the rest of the digits. Since speed counts here,
	 * we do it in two loops. The first gets "value" down until it
	 * is no larger than MAXINT. The second one uses integer divides
	 * rather than long divides to speed it up.
	 */
	bp += MAXDIGS + 1;
	while (value > MAXINT) {
		*--bp = (int)(value % 10) + '0';
		value /= 10;
	}

	/* cannot lose precision */
	svalue = value;
	while (svalue > 0) {
		*--bp = (svalue % 10) + '0';
		svalue /= 10;
	}

	/* fill in intermediate zeroes if needed */
	if (buffer[1] != '0') {
		while (bp > buffer + 2)
			*--bp = '0';
		--bp;
	}
	return(bp);
}

/*
 * This program sends string "s" to putchar. The character after
 * the end of "s" is given by "send". This allows the size of the
 * field to be computed; it is stored in "alen". "width" contains the
 * user specified length. If width<alen, the width will be taken to
 * be alen. "sign" is zero if the string is to be right-justified
 * in the field, nonzero if it is to be left-justified. "fill" is
 * 0 if the string is to be padded with '0', positive if it is to be
 * padded with ' ', and negative if an initial '-' should appear before
 * any padding in right-justification (to avoid printing "-3" as
 * "000-3" where "-0003" was intended).
 */
void
_p_emit(unsigned char *s, unsigned char *send)
{
	unsigned char cfill;
	int alen;
	int npad, length;
	wchar_t wchar;

	alen = send - s;
	if (alen > width)
		width = alen;
	cfill = fill>0? ' ': '0';

	/* we may want to print a leading '-' before anything */
	if (*s == '-' && fill < 0) {
		putchar(*s++);
		alen--;
		width--;
	}
	npad = width - alen;

	/* emit any leading pad characters */
	if (!sign)
		while (--npad >= 0)
			putchar(cfill);

	/* emit the string itself */
	while (--alen >= 0) {
		length = mbtowc(&wchar, (char *)s, MB_LEN_MAX);
		if(length <= 0) {
			putoctal = 1;
			putchar((unsigned char)*s++);
			putoctal = 0;
		} else {
			putchar(wchar);
			s += length;
			alen = alen - length + 1;
		}
	}
	/* emit trailing pad characters */
	if (sign)
		while (--npad >= 0)
			putchar(cfill);
}



/* ============================================================================
 * SOURCE 12/13: minix4\exokernel\kernel_legacy\boot\libsa\printf.c
 * Size: 12,527 bytes, Lines: 590
 * Hash: 1bf5687a16a3...
 * ============================================================================ */

/*
 * Copyright (c) 1986, 1988, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
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
 *
 *	@(#)subr_prf.c	8.3 (Berkeley) 1/21/94
 */

#include <sys/cdefs.h>

/*
 * Standaloneified version of the FreeBSD kernel printf family.
 */

#include <sys/types.h>
#include <sys/stddef.h>
#include <sys/stdint.h>
#include <limits.h>
#include <string.h>
#include "stand.h"

/*
 * Note that stdarg.h and the ANSI style va_start macro is used for both
 * ANSI and traditional C compilers.
 */
#include <machine/stdarg.h>

#define	MAXNBUF (sizeof (intmax_t) * CHAR_BIT + 1)

typedef void (kvprintf_fn_t)(int, void *);

static char *ksprintn(char *, uintmax_t, int, int *, int);
static int kvprintf(char const *, kvprintf_fn_t *, void *, int, va_list);

static void
putchar_wrapper(int cc, void *arg __unused)
{

	putchar(cc);
}

int
printf(const char *fmt, ...)
{
	va_list ap;
	int retval;

	va_start(ap, fmt);
	retval = kvprintf(fmt, putchar_wrapper, NULL, 10, ap);
	va_end(ap);
	return (retval);
}

void
vprintf(const char *fmt, va_list ap)
{

	kvprintf(fmt, putchar_wrapper, NULL, 10, ap);
}

int
sprintf(char *buf, const char *cfmt, ...)
{
	int retval;
	va_list ap;

	va_start(ap, cfmt);
	retval = kvprintf(cfmt, NULL, (void *)buf, 10, ap);
	buf[retval] = '\0';
	va_end(ap);
	return (retval);
}

struct print_buf {
	char *buf;
	size_t size;
};

static void
snprint_func(int ch, void *arg)
{
	struct print_buf *pbuf = arg;

	if (pbuf->size < 2) {
		/*
		 * Reserve last buffer position for the terminating
		 * character:
		 */
		return;
	}
	*(pbuf->buf)++ = ch;
	pbuf->size--;
}

int
asprintf(char **buf, const char *cfmt, ...)
{
	int retval;
	struct print_buf arg;
	va_list ap;

	*buf = NULL;
	va_start(ap, cfmt);
	retval = kvprintf(cfmt, NULL, NULL, 10, ap);
	va_end(ap);
	if (retval <= 0)
		return (-1);

	arg.size = retval + 1;
	arg.buf = *buf = malloc(arg.size);
	if (*buf == NULL)
		return (-1);

	va_start(ap, cfmt);
	retval = kvprintf(cfmt, &snprint_func, &arg, 10, ap);
	va_end(ap);

	if (arg.size >= 1)
		*(arg.buf)++ = 0;
	return (retval);
}

int
snprintf(char *buf, size_t size, const char *cfmt, ...)
{
	int retval;
	va_list ap;
	struct print_buf arg;

	arg.buf = buf;
	arg.size = size;

	va_start(ap, cfmt);
	retval = kvprintf(cfmt, &snprint_func, &arg, 10, ap);
	va_end(ap);

	if (arg.size >= 1)
		*(arg.buf)++ = 0;
	return (retval);
}

void
vsprintf(char *buf, const char *cfmt, va_list ap)
{
	int	retval;

	retval = kvprintf(cfmt, NULL, (void *)buf, 10, ap);
	buf[retval] = '\0';
}

void
vsnprintf(char *buf, size_t size, const char *cfmt, va_list ap)
{
	int	retval;
	struct print_buf arg;

	arg.buf = buf;
	arg.size = size;

	retval = kvprintf(cfmt, &snprint_func, &arg, 10, ap);
	buf[retval] = '\0';
}

/*
 * Put a NUL-terminated ASCII number (base <= 36) in a buffer in reverse
 * order; return an optional length and a pointer to the last character
 * written in the buffer (i.e., the first character of the string).
 * The buffer pointed to by `nbuf' must have length >= MAXNBUF.
 */
static char *
ksprintn(char *nbuf, uintmax_t num, int base, int *lenp, int upper)
{
	char *p, c;

	p = nbuf;
	*p = '\0';
	do {
		c = hex2ascii(num % base);
		*++p = upper ? toupper(c) : c;
	} while (num /= base);
	if (lenp)
		*lenp = p - nbuf;
	return (p);
}

/*
 * Scaled down version of printf(3).
 *
 * Two additional formats:
 *
 * The format %b is supported to decode error registers.
 * Its usage is:
 *
 *	printf("reg=%b\n", regval, "<base><arg>*");
 *
 * where <base> is the output base expressed as a control character, e.g.
 * \10 gives octal; \20 gives hex.  Each arg is a sequence of characters,
 * the first of which gives the bit number to be inspected (origin 1), and
 * the next characters (up to a control character, i.e. a character <= 32),
 * give the name of the register.  Thus:
 *
 *	kvprintf("reg=%b\n", 3, "\10\2BITTWO\1BITONE");
 *
 * would produce output:
 *
 *	reg=3<BITTWO,BITONE>
 *
 * XXX:  %D  -- Hexdump, takes pointer and separator string:
 *		("%6D", ptr, ":")   -> XX:XX:XX:XX:XX:XX
 *		("%*D", len, ptr, " " -> XX XX XX XX ...
 */
static int
kvprintf(char const *fmt, kvprintf_fn_t *func, void *arg, int radix, va_list ap)
{
#define	PCHAR(c) { \
	int cc = (c);				\
						\
	if (func) {				\
		(*func)(cc, arg);		\
	} else if (d != NULL) {			\
		*d++ = cc;			\
	}					\
	retval++;				\
	}

	char nbuf[MAXNBUF];
	char *d;
	const char *p, *percent, *q;
	uint16_t *S;
	uchar_t *up;
	int ch, n;
	uintmax_t num;
	int base, lflag, qflag, tmp, width, ladjust, sharpflag, neg, sign, dot;
	int cflag, hflag, jflag, tflag, zflag;
	int dwidth, upper;
	char padc;
	int stop = 0, retval = 0;

	num = 0;
	if (!func)
		d = (char *)arg;
	else
		d = NULL;

	if (fmt == NULL)
		fmt = "(fmt null)\n";

	if (radix < 2 || radix > 36)
		radix = 10;

	for (;;) {
		padc = ' ';
		width = 0;
		while ((ch = (uchar_t)*fmt++) != '%' || stop) {
			if (ch == '\0')
				return (retval);
			PCHAR(ch);
		}
		percent = fmt - 1;
		qflag = 0; lflag = 0; ladjust = 0; sharpflag = 0; neg = 0;
		sign = 0; dot = 0; dwidth = 0; upper = 0;
		cflag = 0; hflag = 0; jflag = 0; tflag = 0; zflag = 0;
reswitch:	switch (ch = (uchar_t)*fmt++) {
		case '.':
			dot = 1;
			goto reswitch;
		case '#':
			sharpflag = 1;
			goto reswitch;
		case '+':
			sign = 1;
			goto reswitch;
		case '-':
			ladjust = 1;
			goto reswitch;
		case '%':
			PCHAR(ch);
			break;
		case '*':
			if (!dot) {
				width = va_arg(ap, int);
				if (width < 0) {
					ladjust = !ladjust;
					width = -width;
				}
			} else {
				dwidth = va_arg(ap, int);
			}
			goto reswitch;
		case '0':
			if (!dot) {
				padc = '0';
				goto reswitch;
			}
			/* FALLTHROUGH */
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
				for (n = 0; ; ++fmt) {
					n = n * 10 + ch - '0';
					ch = *fmt;
					if (ch < '0' || ch > '9')
						break;
				}
			if (dot)
				dwidth = n;
			else
				width = n;
			goto reswitch;
		case 'b':
			num = (uint_t)va_arg(ap, int);
			p = va_arg(ap, char *);
			for (q = ksprintn(nbuf, num, *p++, NULL, 0); *q; )
				PCHAR(*q--);

			if (num == 0)
				break;

			for (tmp = 0; *p; ) {
				n = *p++;
				if (num & (1 << (n - 1))) {
					PCHAR(tmp ? ',' : '<');
					for (; (n = *p) > ' '; ++p)
						PCHAR(n);
					tmp = 1;
				} else
					for (; *p > ' '; ++p)
						continue;
			}
			if (tmp)
				PCHAR('>');
			break;
		case 'c':
			PCHAR(va_arg(ap, int));
			break;
		case 'D':
			up = va_arg(ap, uchar_t *);
			p = va_arg(ap, char *);
			if (!width)
				width = 16;
			while (width--) {
				PCHAR(hex2ascii(*up >> 4));
				PCHAR(hex2ascii(*up & 0x0f));
				up++;
				if (width)
					for (q = p; *q; q++)
						PCHAR(*q);
			}
			break;
		case 'd':
		case 'i':
			base = 10;
			sign = 1;
			goto handle_sign;
		case 'h':
			if (hflag) {
				hflag = 0;
				cflag = 1;
			} else
				hflag = 1;
			goto reswitch;
		case 'j':
			jflag = 1;
			goto reswitch;
		case 'l':
			if (lflag) {
				lflag = 0;
				qflag = 1;
			} else
				lflag = 1;
			goto reswitch;
		case 'n':
			if (jflag)
				*(va_arg(ap, intmax_t *)) = retval;
			else if (qflag)
				*(va_arg(ap, quad_t *)) = retval;
			else if (lflag)
				*(va_arg(ap, long *)) = retval;
			else if (zflag)
				*(va_arg(ap, size_t *)) = retval;
			else if (hflag)
				*(va_arg(ap, short *)) = retval;
			else if (cflag)
				*(va_arg(ap, char *)) = retval;
			else
				*(va_arg(ap, int *)) = retval;
			break;
		case 'o':
			base = 8;
			goto handle_nosign;
		case 'p':
			base = 16;
			sharpflag = (width == 0);
			sign = 0;
			num = (uintptr_t)va_arg(ap, void *);
			goto number;
		case 'q':
			qflag = 1;
			goto reswitch;
		case 'r':
			base = radix;
			if (sign)
				goto handle_sign;
			goto handle_nosign;
		case 's':
			p = va_arg(ap, char *);
			if (p == NULL)
				p = "(null)";
			if (!dot)
				n = strlen(p);
			else
				for (n = 0; n < dwidth && p[n]; n++)
					continue;

			width -= n;

			if (!ladjust && width > 0)
				while (width--)
					PCHAR(padc);
			while (n--)
				PCHAR(*p++);
			if (ladjust && width > 0)
				while (width--)
					PCHAR(padc);
			break;
		case 'S':	/* Assume console can cope with wide chars */
			S = va_arg(ap, uint16_t *);
			if (S == NULL)
				S = (uint16_t *)L"(null)";
			if (!dot) {
				for (n = 0; S[n] != 0; n++)
					continue;
			} else {
				for (n = 0; n < dwidth && S[n]; n++)
					continue;
			}

			width -= n;

			if (!ladjust && width > 0)
				while (width--)
					PCHAR(padc);
			while (n--)
				PCHAR(*S++);
			if (ladjust && width > 0)
				while (width--)
					PCHAR(padc);
			break;
		case 't':
			tflag = 1;
			goto reswitch;
		case 'u':
			base = 10;
			goto handle_nosign;
		case 'X':
			upper = 1;
			/* FALLTHROUGH */
		case 'x':
			base = 16;
			goto handle_nosign;
		case 'y':
			base = 16;
			sign = 1;
			goto handle_sign;
		case 'z':
			zflag = 1;
			goto reswitch;
handle_nosign:
			sign = 0;
			if (jflag)
				num = va_arg(ap, uintmax_t);
			else if (qflag)
				num = va_arg(ap, uint64_t);
			else if (tflag)
				num = va_arg(ap, ptrdiff_t);
			else if (lflag)
				num = va_arg(ap, ulong_t);
			else if (zflag)
				num = va_arg(ap, size_t);
			else if (hflag)
				num = (ushort_t)va_arg(ap, int);
			else if (cflag)
				num = (uchar_t)va_arg(ap, int);
			else
				num = va_arg(ap, uint_t);
			goto number;
handle_sign:
			if (jflag)
				num = va_arg(ap, intmax_t);
			else if (qflag)
				num = va_arg(ap, quad_t);
			else if (tflag)
				num = va_arg(ap, ptrdiff_t);
			else if (lflag)
				num = va_arg(ap, long);
			else if (zflag)
				num = va_arg(ap, ssize_t);
			else if (hflag)
				num = (short)va_arg(ap, int);
			else if (cflag)
				num = (char)va_arg(ap, int);
			else
				num = va_arg(ap, int);
number:
			if (sign && (intmax_t)num < 0) {
				neg = 1;
				num = -(intmax_t)num;
			}
			p = ksprintn(nbuf, num, base, &n, upper);
			tmp = 0;
			if (sharpflag && num != 0) {
				if (base == 8)
					tmp++;
				else if (base == 16)
					tmp += 2;
			}
			if (neg)
				tmp++;

			if (!ladjust && padc == '0')
				dwidth = width - tmp;
			width -= tmp + imax(dwidth, n);
			dwidth -= n;
			if (!ladjust)
				while (width-- > 0)
					PCHAR(' ');
			if (neg)
				PCHAR('-');
			if (sharpflag && num != 0) {
				if (base == 8) {
					PCHAR('0');
				} else if (base == 16) {
					PCHAR('0');
					PCHAR('x');
				}
			}
			while (dwidth-- > 0)
				PCHAR('0');

			while (*p)
				PCHAR(*p--);

			if (ladjust)
				while (width-- > 0)
					PCHAR(' ');

			break;
		default:
			while (percent < fmt)
				PCHAR(*percent++);
			/*
			 * Since we ignore a formatting argument it is no
			 * longer safe to obey the remaining formatting
			 * arguments as the arguments will no longer match
			 * the format specs.
			 */
			stop = 1;
			break;
		}
	}
#undef PCHAR
}



/* ============================================================================
 * SOURCE 13/13: minix\utilities\printf.c
 * Size: 883 bytes, Lines: 57
 * Hash: ea3b6840d78a...
 * ============================================================================ */

/* Copyright (c) 1979 Regents of the University of California */
#ifdef TRACE
#include <stdio.h>
#undef putchar
#endif

printf(fmt, args)
{

	_doprnt(fmt, &args, 0);
}

_strout(count, string, adjust, flail, fillch)
register char *string;
register count;
register int adjust;
#ifdef TRACE
register struct _iobuf *flail;
#else
register struct {int i; } *flail;
#endif
{
	if (adjust < 0) {
		if (*string=='-' && fillch=='0') {
#ifdef TRACE
			if (flail)
				putc(*string++, flail);
			else
#endif
				putchar(*string++);
			count--;
		}
		adjust= -adjust;
		while (--adjust>=0)
#ifdef TRACE
			if (flail)
				putc(fillch, flail);
			else
#endif
				putchar(fillch);
	}
	while (--count>=0)
#ifdef TRACE
		if (flail)
			putc(*string++, flail);
		else
#endif
			putchar(*string++);
	while (--adjust>=0)
#ifdef TRACE
		if (flail)
			putc(fillch, flail);
		else
#endif
			putchar(fillch);
}

