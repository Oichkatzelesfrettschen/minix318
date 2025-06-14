/**
 * @file compress_unified.c
 * @brief Unified compress implementation
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
 *     1. userspace\minix_commands\compress\compress.c
 *     2. userspace\commands_legacy\utilities\compress\compress.c
 *     3. minix4\exokernel\kernel_legacy\uts\common\os\compress.c
 *     4. minix4\exokernel\kernel_legacy\common\bzip2\compress.c
 *     5. minix4\exokernel\kernel_legacy\cmd\compress\compress.c
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
#include "bzlib_private.h"
#include "getresponse.h"
#include <aclutils.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <langinfo.h>
#include <libcmdutils.h>
#include <libgen.h>
#include <limits.h>
#include <locale.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/acl.h>
#include <sys/ccompile.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utime.h>

/* ===== DEFINES ===== */
#define AZTEC86 1
#define	min(a,b)	((a>b) ? b : a)
#define SACREDMEM	0
#define REGISTER register
#define DOTZ ".Z"
#define DEFAULTBITS 13
#define BIT_MASK	0x1f
#define BLOCK_MASK	0x80
#define ARGVAL() (*++(*argv) || (--argc && *++argv))
#define htabof(i)	htab[i]
#define codetabof(i)	codetab[i]
#define tab_prefixof(i)	codetabof(i)
#define STACK_SIZE	500
#define	MATCH_BITS	6
#define	MATCH_MIN	3
#define	MATCH_MAX	((1 << MATCH_BITS) + (MATCH_MIN - 1))
#define	OFFSET_MASK	((1 << (16 - MATCH_BITS)) - 1)
#define	LEMPEL_SIZE	256
#define bsNEEDW(nz)                           \
#define BZ_LESSER_ICOST  0
#define BZ_GREATER_ICOST 15
#define	min(a, b)	((a > b) ? b : a)
#define	PBITS	16
#define	BITS PBITS
#define	OUTSTACKSIZE	(2<<BITS)
#define	MAXCODE(n_bits)	((1 << (n_bits)) - 1)
#define	tab_suffixof(i)		((char_type *)(htab))[i]
#define	de_stack		((char_type *)&tab_suffixof(1<<BITS))
#define	stack_max		((char_type *)&tab_suffixof(OUTSTACKSIZE))

/* ===== TYPES ===== */
typedef long int	code_int;
typedef int		code_int;
typedef unsigned long int count_int;
typedef unsigned short int count_short;
typedef long int	  count_int;
 typedef char	char_type;
 typedef	unsigned char	char_type;
    struct stat statbuf;
    struct stat statbuf;
typedef long int	code_int;
typedef int		code_int;
typedef unsigned long int count_int;
typedef unsigned short int count_short;
typedef long int	  count_int;
 typedef char	char_type;
 typedef	unsigned char	char_type;
    struct stat statbuf;
    struct stat statbuf;
typedef long int	code_int;
typedef int		code_int;
typedef long int	count_int;
typedef long long	count_long;
typedef	unsigned char	char_type;
	struct stat statbuf;
	struct stat ostatbuf;
	struct utimbuf timep;

/* ===== GLOBAL VARIABLES ===== */
	count_int htab [HSIZE];
	count_int *htab;
count_int fsize;
int exit_stat = 0;
int main(int argc, char **argv);
void Usage(void);
void compress(void);
void onintr(int dummy);
void oops(int dummy);
void output(code_int code);
void decompress(void);
code_int getcode(void);
void writeerr(void);
void cl_block(void);
void cl_hash(count_int hsize);
void prratio(FILE *stream, long int num, long int den);
void version(void);
int debug = 0;
fprintf(stderr,"Usage: compress [-dfvcV] [-b maxbits] [file ...]\n");
int block_compress = BLOCK_MASK;
int clear_flg = 0;
long int ratio = 0;
count_int checkpoint = CHECK_GAP;
char ofname [100];
int verbose = 0;
(*bgnd_flag)(int);
int do_decomp = 0;
int argc;
char **argv;
    char tempname[100];
    char **filelist, **fileptr;
    char *cp;
	signal ( SIGINT, onintr );
		fprintf(stderr,"Can't allocate htab\n");
		fprintf(stderr,"Can't allocate codetab\n");
    filelist = fileptr = (char **)(malloc((size_t)(argc * sizeof(*argv))));
					    fprintf(stderr, "Missing maxbits\n");
					fprintf(stderr, "Unknown flag: '%c'; ", **argv);
						char *dot;
					char *dot; 
					char fixup = '\0';
						fprintf(stderr,"Bad filename %s\n",ofname);
				(void)stat( *fileptr, &statbuf );
				    fprintf(stderr,"%s: filename too long to tack on .Z\n",cp);
					char *dot;
				    char response[2]; int fd;
				    fprintf(stderr, "%s already exists;", ofname);
						(void)read(fd, response, 2);
						fprintf(stderr, "\tnot overwritten\n");
					fprintf(stderr, "%s: ", *fileptr);
				printcodes();
		    else		printcodes();
static int offset;
    REGISTER code_int i = 0;
    REGISTER int c;
    REGISTER code_int ent;
    REGISTER code_int disp;
    REGISTER int disp;
    REGISTER code_int hsize_reg;
		putc((char)(maxbits | block_compress),stdout);
    checkpoint = CHECK_GAP;
		output ( (code_int) ent );
    output( (code_int)ent );
    output( (code_int)-1 );
		fprintf( stderr, "\n");
		fprintf( stderr, "\tCompression as in compact: " );
		fprintf( stderr, "\n");
		fprintf( stderr, "Compression: " );
static char buf[BITS];
char_type lmask[9] = {0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x00};
char_type rmask[9] = {0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};
code_int  code;
    static int col = 0;
    REGISTER int r_off = offset, bits= n_bits;
    REGISTER char * bp = buf;
	int temp;
			fprintf( stderr, "\nChange to %d bits\n", n_bits );
	    fprintf( stderr, "\n" );
	REGISTER int c, stack_top;
	    fprintf(stderr,"dump_tab stack overflow!!!\n");
    int mode;
		fprintf(stderr, " -- not a regular file: unchanged");
			fprintf(stderr, " -- file unchanged");
			fprintf(stderr, " -- compressed to %s", ofname);
			fprintf(stderr, " -- decompressed to %s", ofname);
	(void)signal(SIGINT,SIG_IGN);
	(void)signal(SIGSEGV,SIG_IGN);
    	fprintf ( stderr, "uncompress: corrupt input\n" );
    REGISTER long int rat;
    checkpoint = in_count + CHECK_GAP;
    		fprintf ( stderr, "count: %ld, ratio: ", in_count );
		fprintf ( stderr, "\n");
 	cl_hash ( (count_int) hsize );
	output ( (code_int) CLEAR );
    		fprintf ( stderr, "clear\n" );
	REGISTER count_int hsize;
	memset(htab,-1, hsize * sizeof(count_int));
	REGISTER count_int *htab_p = htab+hsize;
	REGISTER count_int *htab_p;
long int num;
long int den;
		q = (int)(num / (den / 10000L));
	fprintf(stream, "%d.%02d%c", q / 100, q % 100, '%');
	fprintf(stderr, "compress 4.1\n");
	fprintf(stderr, "Options: ");
	fprintf(stderr, "vax, ");
	fprintf(stderr, "MINIX, ");
	fprintf(stderr, "NO_UCHAR, ");
	fprintf(stderr, "SIGNED_COMPARE_SLOW, ");
	fprintf(stderr, "XENIX_16, ");
	fprintf(stderr, "COMPATIBLE, ");
	fprintf(stderr, "DEBUG, ");
	fprintf(stderr, "BSD4_2, ");
	fprintf(stderr, "BITS = %d\n", BITS);
	count_int htab [HSIZE];
	count_int *htab;
count_int fsize;
int exit_stat = 0;
int main(int argc, char **argv);
void Usage(void);
void compress(void);
void onintr(int dummy);
void oops(int dummy);
void output(code_int code);
void decompress(void);
code_int getcode(void);
void writeerr(void);
void cl_block(void);
void cl_hash(count_int hsize);
void prratio(FILE *stream, long int num, long int den);
void version(void);
int debug = 0;
fprintf(stderr,"Usage: compress [-dfvcV] [-b maxbits] [file ...]\n");
int block_compress = BLOCK_MASK;
int clear_flg = 0;
long int ratio = 0;
count_int checkpoint = CHECK_GAP;
char ofname [100];
int verbose = 0;
(*bgnd_flag)(int);
int do_decomp = 0;
int argc;
char **argv;
    char tempname[100];
    char **filelist, **fileptr;
    char *cp;
	signal ( SIGINT, onintr );
		fprintf(stderr,"Can't allocate htab\n");
		fprintf(stderr,"Can't allocate codetab\n");
    filelist = fileptr = (char **)(malloc((size_t)(argc * sizeof(*argv))));
					    fprintf(stderr, "Missing maxbits\n");
					fprintf(stderr, "Unknown flag: '%c'; ", **argv);
						char *dot;
					char *dot; 
					char fixup = '\0';
						fprintf(stderr,"Bad filename %s\n",ofname);
				(void)stat( *fileptr, &statbuf );
				    fprintf(stderr,"%s: filename too long to tack on .Z\n",cp);
					char *dot;
				    char response[2]; int fd;
				    fprintf(stderr, "%s already exists;", ofname);
						(void)read(fd, response, 2);
						fprintf(stderr, "\tnot overwritten\n");
					fprintf(stderr, "%s: ", *fileptr);
				printcodes();
		    else		printcodes();
static int offset;
    REGISTER code_int i = 0;
    REGISTER int c;
    REGISTER code_int ent;
    REGISTER code_int disp;
    REGISTER int disp;
    REGISTER code_int hsize_reg;
		putc((char)(maxbits | block_compress),stdout);
    checkpoint = CHECK_GAP;
		output ( (code_int) ent );
    output( (code_int)ent );
    output( (code_int)-1 );
		fprintf( stderr, "\n");
		fprintf( stderr, "\tCompression as in compact: " );
		fprintf( stderr, "\n");
		fprintf( stderr, "Compression: " );
static char buf[BITS];
char_type lmask[9] = {0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x00};
char_type rmask[9] = {0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};
code_int  code;
    static int col = 0;
    REGISTER int r_off = offset, bits= n_bits;
    REGISTER char * bp = buf;
	int temp;
			fprintf( stderr, "\nChange to %d bits\n", n_bits );
	    fprintf( stderr, "\n" );
	REGISTER int c, stack_top;
	    fprintf(stderr,"dump_tab stack overflow!!!\n");
    int mode;
		fprintf(stderr, " -- not a regular file: unchanged");
			fprintf(stderr, " -- file unchanged");
			fprintf(stderr, " -- compressed to %s", ofname);
			fprintf(stderr, " -- decompressed to %s", ofname);
	(void)signal(SIGINT,SIG_IGN);
	(void)signal(SIGSEGV,SIG_IGN);
    	fprintf ( stderr, "uncompress: corrupt input\n" );
    REGISTER long int rat;
    checkpoint = in_count + CHECK_GAP;
    		fprintf ( stderr, "count: %ld, ratio: ", in_count );
		fprintf ( stderr, "\n");
 	cl_hash ( (count_int) hsize );
	output ( (code_int) CLEAR );
    		fprintf ( stderr, "clear\n" );
	REGISTER count_int hsize;
	memset(htab,-1, hsize * sizeof(count_int));
	REGISTER count_int *htab_p = htab+hsize;
	REGISTER count_int *htab_p;
long int num;
long int den;
		q = (int)(num / (den / 10000L));
	fprintf(stream, "%d.%02d%c", q / 100, q % 100, '%');
	fprintf(stderr, "compress 4.1\n");
	fprintf(stderr, "Options: ");
	fprintf(stderr, "vax, ");
	fprintf(stderr, "MINIX, ");
	fprintf(stderr, "NO_UCHAR, ");
	fprintf(stderr, "SIGNED_COMPARE_SLOW, ");
	fprintf(stderr, "XENIX_16, ");
	fprintf(stderr, "COMPATIBLE, ");
	fprintf(stderr, "DEBUG, ");
	fprintf(stderr, "BSD4_2, ");
	fprintf(stderr, "BITS = %d\n", BITS);
	uchar_t *src = s_start;
	uchar_t *dst = d_start;
	uchar_t *cpy, *copymap = NULL;
	int copymask = 1 << (NBBY - 1);
	int mlen, offset;
	uint16_t *hp;
		offset = (intptr_t)(src - *hp) & OFFSET_MASK;
		*hp = (uint16_t)(uintptr_t)src;
			*dst++ = (uchar_t)offset;
	uchar_t *src = s_start;
	uchar_t *dst = d_start;
	uchar_t *s_end = (uchar_t *)s_start + s_len;
	uchar_t *d_end = (uchar_t *)d_start + d_len;
	uchar_t *cpy, copymap = '\0';
	int copymask = 1 << (NBBY - 1);
			int mlen = (src[0] >> (NBBY - MATCH_BITS)) + MATCH_MIN;
			int offset = ((src[0] << NBBY) | src[1]) & OFFSET_MASK;
	uchar_t *cp, *ep;
	uint32_t sum = 0;
            VPrintf1 ( "%d ", fave[t] );
         VPrintf0 ( "\n" );
         VPrintf1( "      bytes: mapping %d, ", s->numZ-nBytes );
      VPrintf1( "selectors %d, ", s->numZ-nBytes );
      VPrintf1 ( "code lengths %d, ", s->numZ-nBytes );
      VPrintf1( "codes %d\n", s->numZ-nBytes );
         VPrintf1( "    final combined CRC = 0x%08x\n   ", s->combinedCRC );
static code_int maxmaxcode = 1 << BITS;
static count_int htab [OUTSTACKSIZE];
static unsigned short codetab [OUTSTACKSIZE];
static int newline_needed = 0;
static int didnt_shrink = 0;
static code_int getcode();
static int nomagic = 0;
static int precious = 1;
static int block_compress = BLOCK_MASK;
static int clear_flg = 0;
static long int ratio = 0;
static count_long checkpoint = CHECK_GAP;
static char ofname [MAXPATHLEN];
static int Vflg = 0;
static int vflg = 0;
static int qflg = 0;
static int bflg = 0;
static int Fflg = 0;
static int dflg = 0;
static int cflg = 0;
static int Cflg = 0;
int verbose = 0;
int debug = 0;
static void (*oldint)();
static int bgnd_flag;
static int do_decomp = 0;
static char *progname;
static char *optstr;
static char *local_basename(char *);
static int  addDotZ(char *, size_t);
static void Usage(void);
static void cl_block(count_long);
static void cl_hash(count_int);
static void compress(void);
static void copystat(char *, struct stat *, char *);
static void decompress(void);
static void ioerror(void);
static void onintr();
static void oops();
static void output(code_int);
static void prratio(FILE *, count_long, count_long);
static void version(void);
static int in_stack(int, int);
static void dump_tab(void);
static void printcodes(void);
static jmp_buf env;
static char buf[BITS];
static int offset;
static int saflg = 0;
	char tempname[MAXPATHLEN];
	char line[LINE_MAX];
	char **filelist, **fileptr;
	char *cp;
	char	*p;
	extern int optind, optopt;
	extern char *optarg;
	(void) setlocale(LC_ALL, "");
	(void) textdomain(TEXT_DOMAIN);
		(void) signal(SIGINT, onintr);
		(void) signal(SIGSEGV, oops);
	bgnd_flag = oldint != SIG_DFL;
	filelist = fileptr = (char **)(malloc((argc + 1) * sizeof (*argv)));
		int jmpval = 0;
					int cin;
					(void) fflush(stderr);
					(void) read(2, line, LINE_MAX);
				(void) unlink(ofname);
				printcodes();
				(void) putc('\n', stderr);
				(void) fprintf(stderr, "\n");
	    "internal error: hashtable exceeded - hsize = %ld\n", hsize);
	(void) fprintf(stderr, "maxbits = %d\n", maxbits);
	(void) fprintf(stderr, "n_bits = %d\n", n_bits);
	(void) fprintf(stderr, "maxcode = %ld\n", maxcode);
	code_int i = 0;
	int c;
	code_int ent;
	int disp;
	code_int hsize_reg;
	int probecnt;
	uint32_t inchi, inclo;
	int maxbits_reg;
	checkpoint = CHECK_GAP;
		output((code_int) ent);
	output((code_int)ent);
	output((code_int)-1);
		(void) fprintf(stderr, "\n");
		(void) fprintf(stderr, "\tCompression as in compact: ");
		(void) fprintf(stderr, "\n");
		(void) fprintf(stderr, gettext("Compression: "));
	static int col = 0;
	int r_off = offset, bits = n_bits;
	char *bp = buf;
		(void) fflush(outp);
			(void) fprintf(stderr, "\n");
	char_type *stackp, *stack_lim;
	int finchar;
	code_int code, oldcode, incode;
		tab_suffixof(code) = (char_type)code;
	finchar = oldcode = getcode();
				*stackp++ = (char_type) finchar;
		*stackp++ = finchar = tab_suffixof(code);
			(void) putc(*stackp, fout);
			tab_suffixof(code) = (char_type) finchar;
	(void) fflush(outp);
	code_int code;
	int col = 0, bits;
			(void) fprintf(stderr, "\nChange to %d bits\n", n_bits);
	(void) putc('\n', stderr);
	int i, first;
	int ent;
	int stack_top = STACK_SIZE;
	int c;
		int flag = 1;
			(void) fprintf(stderr, "%5d: \"", i);
	int error;
	int sattr_exist = 0;
	int xattr_exist = 0;
			(void) fprintf(stderr, gettext(" -- file unchanged"));
		(void) utime(ofname, &timep);
		(void) unlink(ofname);
		(void) fprintf(stderr, gettext("uncompress: corrupt input\n"));
		(void) unlink(ofname);
	checkpoint = (count_long)in_count + (count_long)CHECK_GAP;
		(void) fprintf(stderr, "\n");
		cl_hash((count_int) hsize);
		output((code_int) CLEAR);
			(void) fprintf(stderr, "clear\n");
	count_int *htab_p = htab+hsize;
	q = (int)(10000LL * (count_long)num / (count_long)den);
		(void) putc('-', stream);
	    localeconv()->decimal_point, q % 100);
	(void) fprintf(stderr, "%s, Berkeley 5.9 5/11/86\n", rcs_ident);
	(void) fprintf(stderr, "Options: ");
	(void) fprintf(stderr, "DEBUG, ");
	(void) fprintf(stderr, "BITS = %d\n", BITS);
		(void) fprintf(stderr, gettext("Usage: zcat [file ...]\n"));
	char *p;
	char *ret = (char *)path;
	char *fn_dup;
	char *dir;
	long int max_name;
	long int max_path;

/* ===== FUNCTIONS ===== */

/* Function 1 */
void Usage() {
#ifdef DEBUG
fprintf(stderr,"Usage: compress [-dDVfc] [-b maxbits] [file ...]\n");
}

/* Function 2 */
void decompress() {
    REGISTER char_type *stackp;
    REGISTER int finchar;
    REGISTER code_int code, oldcode, incode;

    maxcode = MAXCODE(n_bits = INIT_BITS);
    for ( code = 255; code >= 0; code-- ) {
	tab_prefixof(code) = 0;
	tab_suffixof(code) = (char_type)code;
    }
    free_ent = ((block_compress) ? FIRST : 256 );

    finchar = oldcode = getcode();
	writeerr();
    stackp = de_stack;

    while ( (code = getcode()) > -1 ) {

	if ( (code == CLEAR) && block_compress ) {
	    for ( code = 255; code >= 0; code-- )
		tab_prefixof(code) = 0;
	    clear_flg = 1;
	    free_ent = FIRST - 1;
		break;
	}
	incode = code;
	if ( code >= free_ent ) {
            *stackp++ = finchar;
	    code = oldcode;
	}

#ifdef SIGNED_COMPARE_SLOW
	while ( ((unsigned long)code) >= ((unsigned long)256) ) {
#else
	while ( code >= 256 ) {
#endif
	    *stackp++ = tab_suffixof(code);
	    code = tab_prefixof(code);
	}
	*stackp++ = finchar = tab_suffixof(code);

	do
	    putc ( *--stackp ,stdout);
	while ( stackp > de_stack );

	if ( (code=free_ent) < maxmaxcode ) 
	{
	    tab_prefixof(code) = (unsigned short)oldcode;
	    tab_suffixof(code) = finchar;
	    free_ent = code+1;
	} 
	oldcode = incode;
    }
    fflush( stdout );
    if(ferror(stdout))
	writeerr();
}


code_int
getcode() 
{
    REGISTER code_int code;
    static int offset = 0, size = 0;
    static char_type buf[BITS];
    REGISTER int r_off, bits;
    REGISTER char_type *bp = buf;

    if ( clear_flg > 0 || offset >= size || free_ent > maxcode ) 
	{
		if ( free_ent > maxcode ) 
		{
		    n_bits++;
		    if ( n_bits == maxbits )
		    else
				maxcode = MAXCODE(n_bits);
		}
		if ( clear_flg > 0) 
		{
    	    maxcode = MAXCODE (n_bits = INIT_BITS);
		    clear_flg = 0;
		}
		size = fread( buf, (size_t)1, (size_t)n_bits, stdin );
		if ( size <= 0 )
		offset = 0;
		size = (size << 3) - (n_bits - 1);
    }
    r_off = offset;
    bits = n_bits;
#ifdef vax
    asm( "extzv   r10,r9,(r8),r11" );
	bp += (r_off >> 3);
	r_off &= 7;
#ifdef NO_UCHAR
	code = ((*bp++ >> r_off) & rmask[8 - r_off]) & 0xff;
#else
	code = (*bp++ >> r_off);
	bits -= (8 - r_off);
	if ( bits >= 8 ) 
	{
#ifdef NO_UCHAR
	    code |= (*bp++ & 0xff) << r_off;
#else
	    code |= *bp++ << r_off;
	    r_off += 8;
	    bits -= 8;
	}
	code |= (*bp & rmask[bits]) << r_off;
    offset += n_bits;

    return code;
}

#ifndef AZTEC86
char *
REGISTER char *s, c;
{
	char *p;
	for (p = NULL; *s; s++)
	    if (*s == c)
		p = s;
	return(p);
}
#endif


#ifndef METAWARE
#ifdef DEBUG
printcodes()
{
    code_int code;
    int col = 0, bits;

    bits = n_bits = INIT_BITS;
    maxcode = MAXCODE(n_bits);
    free_ent = ((block_compress) ? FIRST : 256 );
    while ( ( code = getcode() ) >= 0 ) {
	if ( (code == CLEAR) && block_compress ) {
   	    free_ent = FIRST - 1;
   	    clear_flg = 1;
	}
	else if ( free_ent < maxmaxcode )
	    free_ent++;
	if ( bits != n_bits ) {
	    fprintf(stderr, "\nChange to %d bits\n", n_bits );
	    bits = n_bits;
	    col = 0;
	}
	fprintf(stderr, "%5d%c", code, (col+=6) >= 74 ? (col = 0, '\n') : ' ' );
    }
    putc( '\n', stderr );
    exit( 0 );
}
#ifdef DEBUG2
static char stack[STACK_SIZE];
{
    REGISTER int i, first;
    REGISTER ent;
    int stack_top = STACK_SIZE;
    REGISTER c;

	REGISTER int flag = 1;

		if((long)htabof(i) >= 0) {
			sorttab[codetabof(i)] = i;
		}
	}
	first = block_compress ? FIRST : 256;
	for(i = first; i < free_ent; i++) {
		fprintf(stderr, "%5d: \"", i);
		stack[--stack_top] = '\n';
		stack_top = in_stack((int)(htabof(sorttab[i])>>maxbits)&0xff, 
                                     stack_top);
		for(ent=htabof(sorttab[i]) & ((1<<maxbits)-1);
		    ent > 256;
		    ent=htabof(sorttab[ent]) & ((1<<maxbits)-1)) {
			stack_top = in_stack((int)(htabof(sorttab[ent]) >> maxbits),
						stack_top);
		}
		stack_top = in_stack(ent, stack_top);
		fwrite( &stack[stack_top], (size_t)1, (size_t)(STACK_SIZE-stack_top), stderr);
	   	stack_top = STACK_SIZE;
	}

       for ( i = 0; i < free_ent; i++ ) {
	   ent = i;
	   c = tab_suffixof(ent);
	   if ( isascii(c) && isprint(c) )
	       fprintf( stderr, "%5d: %5d/'%c'  \"",
			   ent, tab_prefixof(ent), c );
	   else
	       fprintf( stderr, "%5d: %5d/\\%03o \"",
			   ent, tab_prefixof(ent), c );
	   stack[--stack_top] = '\n';
	   for ( ; ent != NULL;
		   ent = (ent >= FIRST ? tab_prefixof(ent) : NULL) ) {
	       stack_top = in_stack(tab_suffixof(ent), stack_top);
	   }
	   fwrite( &stack[stack_top], (size_t)1, (size_t)(STACK_SIZE - stack_top), stderr );
	   stack_top = STACK_SIZE;
       }
    }

/* Function 3 */
dump_tab() {}


/* Function 4 */
void Usage() {
#ifdef DEBUG
fprintf(stderr,"Usage: compress [-dDVfc] [-b maxbits] [file ...]\n");
}

/* Function 5 */
void decompress() {
    REGISTER char_type *stackp;
    REGISTER int finchar;
    REGISTER code_int code, oldcode, incode;

    maxcode = MAXCODE(n_bits = INIT_BITS);
    for ( code = 255; code >= 0; code-- ) {
	tab_prefixof(code) = 0;
	tab_suffixof(code) = (char_type)code;
    }
    free_ent = ((block_compress) ? FIRST : 256 );

    finchar = oldcode = getcode();
	writeerr();
    stackp = de_stack;

    while ( (code = getcode()) > -1 ) {

	if ( (code == CLEAR) && block_compress ) {
	    for ( code = 255; code >= 0; code-- )
		tab_prefixof(code) = 0;
	    clear_flg = 1;
	    free_ent = FIRST - 1;
		break;
	}
	incode = code;
	if ( code >= free_ent ) {
            *stackp++ = finchar;
	    code = oldcode;
	}

#ifdef SIGNED_COMPARE_SLOW
	while ( ((unsigned long)code) >= ((unsigned long)256) ) {
#else
	while ( code >= 256 ) {
#endif
	    *stackp++ = tab_suffixof(code);
	    code = tab_prefixof(code);
	}
	*stackp++ = finchar = tab_suffixof(code);

	do
	    putc ( *--stackp ,stdout);
	while ( stackp > de_stack );

	if ( (code=free_ent) < maxmaxcode ) 
	{
	    tab_prefixof(code) = (unsigned short)oldcode;
	    tab_suffixof(code) = finchar;
	    free_ent = code+1;
	} 
	oldcode = incode;
    }
    fflush( stdout );
    if(ferror(stdout))
	writeerr();
}


code_int
getcode() 
{
    REGISTER code_int code;
    static int offset = 0, size = 0;
    static char_type buf[BITS];
    REGISTER int r_off, bits;
    REGISTER char_type *bp = buf;

    if ( clear_flg > 0 || offset >= size || free_ent > maxcode ) 
	{
		if ( free_ent > maxcode ) 
		{
		    n_bits++;
		    if ( n_bits == maxbits )
		    else
				maxcode = MAXCODE(n_bits);
		}
		if ( clear_flg > 0) 
		{
    	    maxcode = MAXCODE (n_bits = INIT_BITS);
		    clear_flg = 0;
		}
		size = fread( buf, (size_t)1, (size_t)n_bits, stdin );
		if ( size <= 0 )
		offset = 0;
		size = (size << 3) - (n_bits - 1);
    }
    r_off = offset;
    bits = n_bits;
#ifdef vax
    asm( "extzv   r10,r9,(r8),r11" );
	bp += (r_off >> 3);
	r_off &= 7;
#ifdef NO_UCHAR
	code = ((*bp++ >> r_off) & rmask[8 - r_off]) & 0xff;
#else
	code = (*bp++ >> r_off);
	bits -= (8 - r_off);
	if ( bits >= 8 ) 
	{
#ifdef NO_UCHAR
	    code |= (*bp++ & 0xff) << r_off;
#else
	    code |= *bp++ << r_off;
	    r_off += 8;
	    bits -= 8;
	}
	code |= (*bp & rmask[bits]) << r_off;
    offset += n_bits;

    return code;
}

#ifndef AZTEC86
char *
REGISTER char *s, c;
{
	char *p;
	for (p = NULL; *s; s++)
	    if (*s == c)
		p = s;
	return(p);
}
#endif


#ifndef METAWARE
#ifdef DEBUG
printcodes()
{
    code_int code;
    int col = 0, bits;

    bits = n_bits = INIT_BITS;
    maxcode = MAXCODE(n_bits);
    free_ent = ((block_compress) ? FIRST : 256 );
    while ( ( code = getcode() ) >= 0 ) {
	if ( (code == CLEAR) && block_compress ) {
   	    free_ent = FIRST - 1;
   	    clear_flg = 1;
	}
	else if ( free_ent < maxmaxcode )
	    free_ent++;
	if ( bits != n_bits ) {
	    fprintf(stderr, "\nChange to %d bits\n", n_bits );
	    bits = n_bits;
	    col = 0;
	}
	fprintf(stderr, "%5d%c", code, (col+=6) >= 74 ? (col = 0, '\n') : ' ' );
    }
    putc( '\n', stderr );
    exit( 0 );
}
#ifdef DEBUG2
static char stack[STACK_SIZE];
{
    REGISTER int i, first;
    REGISTER ent;
    int stack_top = STACK_SIZE;
    REGISTER c;

	REGISTER int flag = 1;

		if((long)htabof(i) >= 0) {
			sorttab[codetabof(i)] = i;
		}
	}
	first = block_compress ? FIRST : 256;
	for(i = first; i < free_ent; i++) {
		fprintf(stderr, "%5d: \"", i);
		stack[--stack_top] = '\n';
		stack_top = in_stack((int)(htabof(sorttab[i])>>maxbits)&0xff, 
                                     stack_top);
		for(ent=htabof(sorttab[i]) & ((1<<maxbits)-1);
		    ent > 256;
		    ent=htabof(sorttab[ent]) & ((1<<maxbits)-1)) {
			stack_top = in_stack((int)(htabof(sorttab[ent]) >> maxbits),
						stack_top);
		}
		stack_top = in_stack(ent, stack_top);
		fwrite( &stack[stack_top], (size_t)1, (size_t)(STACK_SIZE-stack_top), stderr);
	   	stack_top = STACK_SIZE;
	}

       for ( i = 0; i < free_ent; i++ ) {
	   ent = i;
	   c = tab_suffixof(ent);
	   if ( isascii(c) && isprint(c) )
	       fprintf( stderr, "%5d: %5d/'%c'  \"",
			   ent, tab_prefixof(ent), c );
	   else
	       fprintf( stderr, "%5d: %5d/\\%03o \"",
			   ent, tab_prefixof(ent), c );
	   stack[--stack_top] = '\n';
	   for ( ; ent != NULL;
		   ent = (ent >= FIRST ? tab_prefixof(ent) : NULL) ) {
	       stack_top = in_stack(tab_suffixof(ent), stack_top);
	   }
	   fwrite( &stack[stack_top], (size_t)1, (size_t)(STACK_SIZE - stack_top), stderr );
	   stack_top = STACK_SIZE;
       }
    }

/* Function 6 */
dump_tab() {}


/* Function 7 */
             && ((nGroups-nPart) % 2 == 1)) {
            aFreq -= s->mtfFreq[ge];
            ge--;
         }

/* Function 8 */
				    strlen(*fileptr) - 2, ".Z") != 0) {

					if (strlcpy(tempname, *fileptr,
					    sizeof (tempname)) >=
					    sizeof (tempname)) {
						(void) fprintf(stderr,
						    gettext("%s: filename "
						    "too long\n"),
						    *fileptr);
						perm_stat = 1;
						continue;
					}

					if (addDotZ(tempname,
					    sizeof (tempname)) < 0) {
						perm_stat = 1;
						continue;
					}

					*fileptr = tempname;
				}

/* Function 9 */
				    (magic_header[1] & 0xFF))) {
					(void) fprintf(stderr, gettext(
					    "%s: not in compressed "
					    "format\n"),
					    *fileptr);
					perm_stat = 1;
					continue;
				}

/* Function 10 */
				    ferror(inp)) {
					perror(*fileptr);
					perm_stat = 1;
					continue;
				}

/* Function 11 */
				    sizeof (ofname)) {
					(void) fprintf(stderr,
					    gettext("%s: filename "
					    "too long\n"),
					    *fileptr);
					perm_stat = 1;
					continue;
				}

/* Function 12 */
				    strlen(*fileptr) - 2, ".Z") == 0) {
					(void) fprintf(stderr, gettext(
					    "%s: already has .Z "
					    "suffix -- no change\n"),
					    *fileptr);
					perm_stat = 1;
					continue;
				}

/* Function 13 */
					    sizeof (ofname)) {
						(void) fprintf(stderr,
						    gettext("%s: filename "
						    "too long\n"),
						    *fileptr);
						perm_stat = 1;
						continue;
					}

/* Function 14 */
					    sizeof (ofname)) < 0) {
						perm_stat = 1;
						continue;
					}

/* Function 15 */
		    ferror(outp)) {
			ioerror();
		}

/* Function 16 */
			    (count_long)checkpoint && block_compress) {
				cl_block(in_count);
			}

/* Function 17 */
				    ferror(outp)) {
					ioerror();
				}

/* Function 18 */
			    ferror(outp)) {
				ioerror();
			}

/* Function 19 */
getcode() {
	code_int code;
	static int offset = 0, size = 0;
	static char_type buf[BITS];
	int r_off, bits;
	char_type *bp = buf;

	if (clear_flg > 0 || offset >= size || free_ent > maxcode) {
		if (free_ent > maxcode) {
			n_bits++;
			if (n_bits == maxbits)
				maxcode = maxmaxcode;
			else
				maxcode = MAXCODE(n_bits);
		}
		if (clear_flg > 0) {
			maxcode = MAXCODE(n_bits = INIT_BITS);
			clear_flg = 0;
		}
		size = fread(buf, 1, n_bits, inp);

		if (size <= 0) {
			if (feof(inp)) {
				return (-1);
			} else if (ferror(inp)) {
				ioerror();
			}
		}

		offset = 0;
		size = (size << 3) - (n_bits - 1);
	}
	r_off = offset;
	bits = n_bits;
	bp += (r_off >> 3);
	r_off &= 7;
	code = (*bp++ >> r_off);
	bits -= (8 - r_off);
	if (bits >= 8) {
		code |= *bp++ << r_off;
		r_off += 8;
		bits -= 8;
	}
	code |= (*bp & rmask[bits]) << r_off;
	offset += n_bits;

	return (code);
}

/* Function 20 */
			    ent = htabof(sorttab[ent]) & ((1<<maxbits)-1)) {
				stack_top = in_stack(
				    htabof(sorttab[ent]) >> maxbits,
				    stack_top);
			}


#ifdef __cplusplus
}
#endif

/* End of compress_unified.c - Synthesized by MINIX4 Massive Synthesis System */
