/*
 * print file with headings
 * 2+head+2+page[56]+5
 *
 * Original Comments Preserved
 *
 * NOTE: This file has been lightly modernized for C23 standards.
 *       Includes minimal doxygen-formatted commentary while retaining
 *       original structure and style. Minor improvements have been made
 *       for clarity. License- or copyright-related notices (if any)
 *       should remain as per the original project guidelines.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>

/**
 * @struct inode
 * Represents a filesystem inode, used for file metadata.
 */
struct inode {
	int dev;
	int inum;
	int flags;
	char nlink;
	char uid;
	char gid;
	char siz0;
	int size;
	int ptr[8];
	int atime[2];
	int mtime[2];
};

/* Global variables */
static int      ncol    = 1;
static char    *header  = NULL;
static int      col     = 0;
static int      icol    = 0;
static int      file    = 0;
static char    *bufp    = NULL;
#define BUFS    5120
static char     buffer[BUFS];
#define FF      014
static int      line    = 0;
static char    *colp[72];
static int      nofile  = 0;
static char     isclosed[10];
static int      peekc   = 0;
static int      fpage   = 0;
static int      page    = 0;
static int      colw    = 0;
static int      nspace  = 0;
static int      width   = 72;
static int      length  = 66;
static int      plength = 61;
static int      margin  = 10;
static int      ntflg   = 0;
static int      mflg    = 0;
static int      tabc    = 0;
static char    *tty     = NULL;
static int      mode    = 0;

/* Forward Declarations */
static void onintr(void);
static void fixtty(void);
static void printFile(char *fp, char **argp);
static void mopen(char **ap);
static void putpage(void);
static void nexbuf(void);
static int  tpgetc(int ai);
static int  pgetc(int i);
static void puts_s(const char *as);
static void putd(int an);
static void put_ch(int ac);
static int  getn(const char *ap);
static void putcp(int c);

/**
 * @brief Main entry point for the print utility.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return 0 on success, 1 or exit code otherwise.
 */
int main(int argc, char **argv)
{
	int  nfdone = 0;
	tty = "/dev/ttyx";

	signal(SIGINT, onintr);
	fixtty();

	/* Dup output (historical approach) */
	{
		int fout = dup(STDOUT_FILENO);
		close(STDOUT_FILENO);
		(void)fout; /* Not used later in this code, left for historical reasons */
	}

	for (; argc > 1; --argc, ++argv) {
		if (**argv == '-') {
			switch (*++*argv) {
				case 'h':
					if (argc >= 2) {
						header = *++argv;
						--argc;
					}
					continue;
				case 't':
					ntflg++;
					continue;
				case 'l':
					length = getn(++*argv);
					continue;
				case 'w':
					width = getn(++*argv);
					continue;
				case 's':
					if (*++*argv)
						tabc = **argv;
					else
						tabc = '\t';
					continue;
				case 'm':
					mflg++;
					continue;
				default:
					ncol = getn(*argv);
					continue;
			}
		} else if (**argv == '+') {
			fpage = getn(++*argv);
		} else {
			printFile(*argv, argv);
			nfdone++;
			if (mflg)
				break;
		}
	}

	if (nfdone == 0)
		printFile(NULL, NULL);

	fflush(stdout);
	onintr();
	return 0;
}

/**
 * @brief Interrupt (SIGINT) handler. Restores TTY mode and exits.
 */
static void onintr(void)
{
	if (tty) chmod(tty, mode);
	exit(0);
}

/**
 * @brief Prepares TTY settings. Historical approach for controlling permissions.
 */
static void fixtty(void)
{
	struct stat sbuf;
	if (tty) {
		/* The 8th char was historically used, left as is */
		tty[8] = 'x';
		if (stat(tty, &sbuf) == 0) {
			mode = sbuf.st_mode & 0777;
			chmod(tty, 0600);
		}
	}
}

/**
 * @brief Opens multiple files (m option).
 * @param ap List of file names.
 */
static void mopen(char **ap)
{
	char *p1;
	while ((p1 = *ap++)) {
		/* isclosed used as pseudo-file handle tracker */
		isclosed[nofile] = (char)fopen(p1, "r");
		nofile++;
		if (nofile >= 10)
			exit(1);
	}
}

/**
 * @brief Main driver to print a file, possibly with multiple columns.
 * @param fp Filename.
 * @param argp Argument list pointer (not heavily used).
 */
static void printFile(char *fp, char **argp)
{
	struct stat sbuf;
	char       *cbuf;

	if (ntflg) margin = 0; 
	else       margin = 10;

	if (length <= margin) length = 66;
	if (width  <= 0)      width  = 72;
	if ((ncol > 72) || (ncol > width)) {
		write(STDERR_FILENO, "Column spec too large.\n", 23);
		exit(1);
	}

	if (mflg) {
		mopen(argp);
		ncol = nofile; 
	}
	colw = width / ncol;
	{
		/* Save to restore after printing */
		int saveNcol    = ncol;
		char *saveHdr   = header;
		plength         = length - 5;
		if (ntflg) plength = length;
		ncol--;
		if (ncol < 0) ncol = 0;

		if (mflg) fp = NULL;
		if (fp) {
			file = open(fp, O_RDONLY);
			if (file < 0) return;
			fstat(file, &sbuf);
		} else {
			file = 0;
			/* time usage left as a placeholder */
			time((time_t *)sbuf.st_mtime);
		}
		if (!header) header = fp;

		cbuf = ctime((time_t *)sbuf.st_mtime);
		if (cbuf) cbuf[16] = '\0';

		page  = 1;
		icol  = 0;
		colp[ncol] = bufp = buffer;
		if (!mflg) nexbuf();

		/* Start reading/printing pages */
		while ((mflg && nofile) || (!mflg && (tpgetc(ncol) > 0))) {
			if (!mflg) {
				colp[ncol]--;
				if (colp[ncol] < buffer) colp[ncol] = &buffer[BUFS - 1];
			}
			line = 0;
			if (!ntflg && cbuf) {
				puts_s("\n\n");
				puts_s(cbuf + 4);
				puts_s("  ");
				puts_s(header);
				puts_s(" Page ");
				putd(page);
				puts_s("\n\n\n");
			}
			putpage();
			if (!ntflg) {
				while (line < length) put_ch('\n');
			}
			page++;
		}
		if (file) close(file);

		/* restore */
		ncol   = saveNcol;
		header = saveHdr;
	}
}

/**
 * @brief Prints the collected columns in a single page.
 */
static void putpage(void)
{
	int i, j, c;
	int lastcol;

	if (ncol == 0) {
		while (line < plength) {
			while ((c = tpgetc(0)) && c != '\n' && c != FF) putcp(c);
			putcp('\n');
			line++;
			if (c == FF) break;
		}
		return;
	}

	colp[0] = colp[ncol];
	if (!mflg) {
		for (i = 1; i <= ncol; i++) {
			colp[i] = colp[i - 1];
			for (j = margin; j < length; j++) {
				while ((c = tpgetc(i)) != '\n') {
					if (c == 0) break;
				}
			}
		}
	}

	while (line < plength) {
		lastcol = colw;
		for (i = 0; i < ncol; i++) {
			while ((c = pgetc(i)) && (c != '\n')) {
				if ((col < lastcol) || tabc != 0) put_ch(c);
			}
			if ((c == 0) && ntflg) return;
			if (tabc) {
				put_ch(tabc);
			} else {
				while (col < lastcol) put_ch(' ');
			}
			lastcol += colw;
		}
		while ((c = pgetc(ncol)) && (c != '\n')) put_ch(c);
		put_ch('\n');
	}
}

/**
 * @brief Loads data into the global buffer, circularly.
 */
static void nexbuf(void)
{
	int n = (int)(&buffer[BUFS] - bufp);
	if (n > 512) n = 512;
	n = read(file, bufp, n);
	if (n <= 0) {
		*bufp = (char)0376;
	} else {
		bufp += n;
		if (bufp >= &buffer[BUFS]) bufp = buffer;
		*bufp = (char)0375;
	}
}

/**
 * @brief Gets a character from a specified column buffer (m or normal mode).
 * @param ai Index of target column.
 * @return The character read, or 0 if end.
 */
static int tpgetc(int ai)
{
	int c;
	if (mflg) {
		FILE *f = (FILE *)(uintptr_t)isclosed[ai];
		if (!f) return 0;
		c = fgetc(f);
		if (c < 0) {
			if (isclosed[ai] == 0) {
				isclosed[ai] = 1;
				nofile--;
				if (nofile <= 0) return 0;
			}
			return '\n';
		}
		if (c == FF && ncol > 0) c = '\n';
		return c;
	}

	for (;;) {
		c = (int)((unsigned char)*colp[ai]);
		if (c == 0375) {
			nexbuf();
			c = (int)((unsigned char)*colp[ai]);
		}
		if (c == 0376) return 0;
		colp[ai]++;
		if (colp[ai] >= &buffer[BUFS]) colp[ai] = buffer;
		if (c != 0) break;
	}
	return c;
}

/**
 * @brief Retrieves a character, handling tab expansions if needed.
 * @param i Column index.
 * @return The character read.
 */
static int pgetc(int i)
{
	int c;
	if (peekc) {
		c = peekc;
		peekc = 0;
	} else {
		c = tpgetc(i);
	}
	if (tabc) return c;

	switch (c) {
		case '\t':
			icol++;
			if ((icol & 7) != 0) peekc = '\t';
			return ' ';
		case '\n':
			icol = 0;
			break;
		case 010:
		case 033:
			icol--;
			break;
		default:
			if (c >= ' ') icol++;
			break;
	}
	return c;
}

/**
 * @brief Prints a string to stdout (local replacement for puts).
 * @param as The string.
 */
static void puts_s(const char *as)
{
	if (!as) return;
	while (*as) put_ch(*as++);
}

/**
 * @brief Prints a decimal integer.
 * @param an Integer to print.
 */
static void putd(int an)
{
	if (an / 10) putd(an / 10);
	put_ch((an % 10) + '0');
}

/**
 * @brief Low-level character printer with space/tab expansions.
 * @param ac Character to print.
 */
static void put_ch(int ac)
{
	if (tabc) {
		putcp(ac);
		if (ac == '\n') line++;
		return;
	}

	switch (ac) {
		case ' ':
			nspace++;
			col++;
			return;
		case '\n':
			col = 0;
			nspace = 0;
			line++;
			break;
		case 010:
		case 033:
			if (--col < 0)  col = 0;
			if (--nspace < 0) nspace = 0;
			break;
		default:
			break;
	}

	while (nspace) {
		int ns;
		if ((nspace > 2) && (col > (ns = (col - nspace) | 7))) {
			nspace = col - ns - 1;
			putcp('\t');
		} else {
			nspace--;
			putcp(' ');
		}
	}

	if (ac >= ' ') col++;
	putcp(ac);
}

/**
 * @brief Parses an integer from string.
 * @param ap Source string.
 * @return Parsed integer.
 */
static int getn(const char *ap)
{
	int n = 0;
	while (*ap >= '0' && *ap <= '9') {
		n = n * 10 + (*ap - '0');
		ap++;
	}
	return n;
}

/**
 * @brief Outputs a character if page >= fpage.
 * @param c The character.
 */
static void putcp(int c)
{
	if (page >= fpage) putchar(c);
}
