/*	Id: preconv.c,v 1.6 2013/06/02 03:52:21 schwarze Exp  */
/*
 * Copyright (c) 2011 Kristaps Dzonsons <kristaps@bsd.lv>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_MMAP
#include <sys/stat.h>
#include <sys/mman.h>
#endif

//#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#define _POSIX_C_SOURCE 200809L
#include <stdbool.h>  /* For C23: prefer <stdbool.h> for Boolean support. */
#include <strings.h>  /* For strncasecmp */

/**
 * @file preconv.c
 * @brief Main driver for character encoding pre-conversion.
 * 
 * This module reads an input file (possibly from stdin), detects and
 * converts character encodings (UTF-8, US-ASCII, Latin-1), and outputs them
 * in a uniform format. It optionally detects a UTF-8 BOM and respects
 * certain inline encoding cues. 
 *
 * The code is based on the original version under the license shown above.
 * It has been lightly updated for clarity in a C23 context.
 */

/** Possible encodings. */
enum enc {
	ENC_UTF_8,     /**< UTF-8 */
	ENC_US_ASCII,  /**< US-ASCII */
	ENC_LATIN_1,   /**< Latin-1 */
	ENC__MAX
};

/**
 * @struct buf
 * @brief Holds file content in memory.
 */
struct buf {
	char   *buf;  /**< Raw data. */
	size_t  sz;   /**< Size of the buffer (bytes). */
	size_t  offs; /**< Offset tracking for scanning. */
};

/** Function-pointer mapping for encoders. */
struct encode {
	const char *name;
	bool      (*conv)(const struct buf *);
};

/* Forward declarations. */
static bool   conv_latin_1(const struct buf *);
static bool   conv_us_ascii(const struct buf *);
static bool   conv_utf_8(const struct buf *);
static bool   read_whole_file(const char *, int, struct buf *, bool *);
static void   resize_buf(struct buf *, size_t);
static int    cue_enc(const struct buf *, size_t *, enum enc *);
static void   usage(void);

/* Global table of encoders. */
static const struct encode encs[ENC__MAX] = {
	{ "utf-8",    conv_utf_8    }, /* ENC_UTF_8 */
	{ "us-ascii", conv_us_ascii }, /* ENC_US_ASCII */
	{ "latin-1",  conv_latin_1  }, /* ENC_LATIN_1 */
};

static const char *progname = NULL;

/**
 * @brief Print usage information to stderr.
 */
static void usage(void)
{
	fprintf(stderr, "usage: %s [-D enc] [-e ENC] [file]\n", progname);
}

/**
 * @brief Convert all characters from buffer (Latin-1).
 * 
 * Mark 8-bit chars with a Unicode escape. Pass others through unchanged.
 * 
 * @param b Buffer holding file data and offset.
 * @return true on success, false on errors.
 */
static bool conv_latin_1(const struct buf *b)
{
	const char *cp = b->buf + b->offs;
	for (size_t i = b->offs; i < b->sz; i++) {
		unsigned char c = (unsigned char)*cp++;
		if (c < 160U) {
			putchar(c);
		} else {
			printf("\\[u%.4X]", c);
		}
	}
	return true;
}

/**
 * @brief Convert US-ASCII (trivial pass-through).
 * 
 * @param b Buffer holding file data and offset.
 * @return true on success, false otherwise.
 */
static bool conv_us_ascii(const struct buf *b)
{
	fwrite(b->buf, 1, b->sz, stdout);
	return true;
}

/**
 * @brief Convert UTF-8 sequences from buffer, output as escapes if non-ASCII.
 * 
 * @param b Buffer referencing the data.
 * @return true on success, false if malformed sequences encountered.
 */
static bool conv_utf_8(const struct buf *b)
{
	const char      *cp    = b->buf + b->offs;
	size_t           i     = 0;
	int              state = 0;
	unsigned int     accum = 0U;
	bool             is_be = false;

	/* Quick test for big-endian. */
	{
		const long one = 1L;
		is_be = (*((const char *)(&one)) == 0);
	}

	for (i = b->offs; i < b->sz; i++) {
		unsigned char c = (unsigned char)*cp++;
		if (state > 0) {
			/* Processing a multibyte sequence. */
			if (!(c & 128) || (c & 64)) {
				return false;
			}
			if (c > 191 || c < 128) {
				return false;
			}
			accum |= (c & 63U) << (--state * 6);
			if (state == 0) {
				if (is_be) {
					/* Swap to correct endianness if needed. */
					accum = (accum >> 24) |
							((accum << 8) & 0x00FF0000) |
							((accum >> 8) & 0x0000FF00) |
							(accum << 24);
				}
				if (accum < 128U) {
					putchar((int)accum);
				} else {
					printf("\\[u%.4X]", accum);
				}
				accum = 0U;
			}
		} else if (c & (1U << 7)) {
			/* Start of a multibyte sequence. */
			for (state = 0; state < 7; state++) {
				if (!(c & (1U << (7 - state)))) {
					break;
				}
			}
			switch (state) {
			case 4:
				if (c < 240U || c > 244U) {
					return false;
				}
				accum = (c & 7U) << 18;
				break;
			case 3:
				if (c < 224U || c > 239U) {
					return false;
				}
				accum = (c & 15U) << 12;
				break;
			case 2:
				if (c < 194U || c > 223U) {
					return false;
				}
				accum = (c & 31U) << 6;
				break;
			default:
				return false;
			}
			--state;
		} else {
			putchar(c);
		}
	}
	if (state != 0) {
		return false;
	}
	return true;
}

/**
 * @brief Resize the buffer as needed for reading.
 * 
 * @param buf The structure to resize.
 * @param initial Guess for initial size or chunk size.
 */
static void resize_buf(struct buf *buf, size_t initial)
{
	buf->sz = (buf->sz > initial / 2) ? (2 * buf->sz) : initial;
	char *resized = realloc(buf->buf, buf->sz);
	if (!resized) {
		perror("realloc");
		exit(EXIT_FAILURE);
	}
	buf->buf = resized;
}

/**
 * @brief Read the entire file into memory. May attempt mmap if available.
 * 
 * @param f Filename or label for error messages.
 * @param fd File descriptor to read from.
 * @param fb Output buffer reference.
 * @param with_mmap True if using mmap, false if fallback read used.
 * @return true on success, false on errors.
 */
static bool read_whole_file(const char *f, int fd, struct buf *fb, bool *with_mmap)
{
	fb->buf  = NULL;
	fb->sz   = 0;
	fb->offs = 0;

#ifdef HAVE_MMAP
	struct stat st;
	if (fstat(fd, &st) == -1) {
		perror(f);
		return false;
	}
	if (S_ISREG(st.st_mode) && (st.st_size >= (1LL << 31))) {
		fprintf(stderr, "%s: input too large\n", f);
		return false;
	}
	if (S_ISREG(st.st_mode)) {
		*with_mmap = true;
		fb->sz     = (size_t)st.st_size;
		fb->buf    = mmap(NULL, fb->sz, PROT_READ, MAP_SHARED, fd, 0);
		if (fb->buf != MAP_FAILED) {
			return true;
		}
		/* Fallback if mmap fails. */
	}
#endif
	*with_mmap = false;

	size_t off = 0;
	for (;;) {
		if (off == fb->sz && fb->sz == (1ULL << 31)) {
			fprintf(stderr, "%s: input too large\n", f);
			break;
		}
		if (off == fb->sz) {
			resize_buf(fb, 65536U);
		}
		size_t to_read = fb->sz - off;
		if (!to_read) {
			break;
		}
		ssize_t ssz = read(fd, fb->buf + off, to_read);
		if (ssz == 0) {
			fb->sz = off;
			return true;
		}
		if (ssz == -1) {
			perror(f);
			break;
		}
		off += (size_t)ssz;
	}
	free(fb->buf);
	fb->buf = NULL;
	return false;
}

/**
 * @brief Attempt to detect an encoding from special in-file cues.
 * 
 * Looks for lines like: 
 * .\" -*- coding: xyz -*-
 * 
 * @param b     Buffer to scan.
 * @param offs  Offset pointer to track lines.
 * @param enc   Encoding guess output.
 * @return 1 if found, 0 if no match, -1 if incomplete line.
 */
static int cue_enc(const struct buf *b, size_t *offs, enum enc *enc)
{
	const char *ln   = b->buf + *offs;
	size_t      sz   = b->sz - *offs;
	const char *eoln = memchr(ln, '\n', sz);
	if (!eoln) {
		return -1;
	}
	*offs = (size_t)(eoln + 1 - b->buf);
	size_t line_len = (size_t)(eoln - ln);
	if (line_len < 10 ||
		memcmp(ln, ".\\\" -*-", 7) != 0 ||
		memcmp(eoln - 3, "-*-", 3) != 0) {
		return 0;
	}
	ln += 7;
	size_t rem = line_len - 10;
	while (rem > 0) {
		while (rem > 0 && *ln == ' ') {
			ln++;
			rem--;
		}
		if (!rem) {
			break;
		}
		const char *eoph = memchr(ln, ';', rem);
		if (!eoph) {
			eoph = eoln - 3;
		} else {
			eoph++;
		}
		size_t phsz = (size_t)(eoph - ln);
		if (phsz >= 7 && strncasecmp(ln, "coding:", 7) == 0) {
			ln  += 7;
			rem -= 7;
			while (rem > 0 && *ln == ' ') {
				ln++;
				rem--;
			}
			if (!rem) {
				break;
			}
			for (int i = 0; i < (int)ENC__MAX; i++) {
				size_t name_len = strlen(encs[i].name);
				if (phsz >= name_len && 
					strncasecmp(ln, encs[i].name, name_len) == 0) {
					*enc = (enum enc)i;
					return 1;
				}
			}
			*enc = ENC__MAX;
			return 1;
		}
		ln  += phsz;
		rem -= phsz;
	}
	return 0;
}

/**
 * @brief Program entry point.
 * 
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return 0 on success, non-zero on error.
 */
int main(int argc, char *argv[])
{
	progname = strrchr(argv[0], '/');
	if (!progname) {
		progname = argv[0];
	} else {
		progname++;
	}

	enum enc enc = ENC__MAX, def_enc = ENC__MAX;
	int      rc  = EXIT_FAILURE;
	bool     map = false;
	struct buf b;
	memset(&b, 0, sizeof(b));

	int ch;
	while ((ch = getopt(argc, argv, "D:e:rdvh")) != -1) {
		switch (ch) {
		case 'D':
		case 'e':
			/* Attempt to parse the selected encoding. */
			{
				bool found = false;
				for (int i = 0; i < (int)ENC__MAX; i++) {
					if (strcasecmp(optarg, encs[i].name) == 0) {
						if (ch == 'D') {
							def_enc = (enum enc)i;
						} else {
							enc = (enum enc)i;
						}
						found = true;
						break;
					}
				}
				if (!found) {
					fprintf(stderr, "%s: Bad encoding\n", optarg);
					return EXIT_FAILURE;
				}
			}
			break;
		case 'r':
		case 'd':
		case 'v':
			/* No-op for compatibility. */
			break;
		case 'h':
		default:
			usage();
			return EXIT_FAILURE;
		}
	}
	argc -= optind;
	argv += optind;

	const char *fn = "<stdin>";
	int         fd = STDIN_FILENO;
	if (argc > 0) {
		fn = argv[0];
		fd = open(fn, O_RDONLY);
		if (fd == -1) {
			perror(fn);
			return EXIT_FAILURE;
		}
	}
	if (!read_whole_file(fn, fd, &b, &map)) {
		goto cleanup;
	}

	/* Detect UTF-8 BOM. */
	static const unsigned char bom[3] = {0xEF, 0xBB, 0xBF};
	if (enc == ENC__MAX && b.sz > 3 && memcmp(b.buf, bom, 3) == 0) {
		b.offs += 3;
		enc = ENC_UTF_8;
	}

	/* Try encoding from the cue lines. */
	if (enc == ENC__MAX) {
		size_t offs = b.offs;
		int c1 = cue_enc(&b, &offs, &enc);
		if (c1 == 0) {
			cue_enc(&b, &offs, &enc);
		}
	}
	/* Use default or Latin-1 if nothing else. */
	if (enc == ENC__MAX) {
		enc = (def_enc == ENC__MAX) ? ENC_LATIN_1 : def_enc;
	}

	/* Perform the actual conversion. */
	if (!encs[enc].conv(&b)) {
		fprintf(stderr, "%s: Bad encoding\n", fn);
		goto cleanup;
	}
	rc = EXIT_SUCCESS;

cleanup:
#ifdef HAVE_MMAP
	if (map) {
		munmap(b.buf, b.sz);
	} else
#endif
	{
		free(b.buf);
	}
	if (fd > STDIN_FILENO) {
		close(fd);
	}
	return rc;
}
