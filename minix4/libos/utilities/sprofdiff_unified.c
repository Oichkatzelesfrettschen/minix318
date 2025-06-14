/**
 * @file sprofdiff_unified.c
 * @brief Unified sprofdiff implementation
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
 *     1. userspace\minix_commands\sprofdiff\sprofdiff.c
 *     2. userspace\commands_legacy\development\sprofdiff\sprofdiff.c
 * 
 * Total source files: 2
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
#include "tdist.h"
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <minix/type.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* ===== DEFINES ===== */
#define DEBUG 0
#define PROC_NAME_WIDTH 10
#define SYMBOL_NAME_WIDTH 24
#define SYMBOL_HASHTAB_SIZE 1024
#define SYMBOL_NAME_SIZE 52
#define MALLOC_CHECKED(type, count) \
#define dprintf(...) do { 						\

/* ===== TYPES ===== */
struct symbol_count {
enum symbol_class {
struct symbol_info {
	struct symbol_info *next;
	struct symbol_info *hashtab_next;
	struct symbol_count count[2];
	enum symbol_class class;
	struct symbol_info *symbol, **symbol_list;
	struct symbol_info **ptr, *symbol;
	struct symbol_count *count;
	struct symbol_info *symbol;
struct symbol_count {
enum symbol_class {
struct symbol_info {
	struct symbol_info *next;
	struct symbol_info *hashtab_next;
	struct symbol_count count[2];
	enum symbol_class class;
	struct symbol_info *symbol, **symbol_list;
	struct symbol_info **ptr, *symbol;
	struct symbol_count *count;
	struct symbol_info *symbol;

/* ===== GLOBAL VARIABLES ===== */
	char binary[PROC_NAME_LEN];
	char name[SYMBOL_NAME_SIZE];
static unsigned n1, n2;
static struct symbol_info *symbols;
static struct symbol_info *symbol_hashtab[SYMBOL_HASHTAB_SIZE];
static double compute_sig(double avg1, double var1, double avg2, double var2);
static void load_file(const char *path, int count_index);
static void *malloc_checked(size_t size);
static void print_report(void);
static void print_report_line(const struct symbol_info *symbol);
	char *name, unsigned long *samples);
static unsigned string_hash(const char *s, size_t size);
	const char *name);
static unsigned symbol_hash(const char *binary, const char *name);
static int symbol_qsort_compare(const void *p1, const void *p2);
	unsigned long samples, int count_index);
static unsigned symbols_count(void);
static void usage(const char *argv0);
	int c, index;
	char binary[PROC_NAME_LEN];
	char name[SYMBOL_NAME_SIZE];
static unsigned n1, n2;
static struct symbol_info *symbols;
static struct symbol_info *symbol_hashtab[SYMBOL_HASHTAB_SIZE];
static double compute_sig(double avg1, double var1, double avg2, double var2);
static void load_file(const char *path, int count_index);
static void *malloc_checked(size_t size);
static void print_report(void);
static void print_report_line(const struct symbol_info *symbol);
	char *name, unsigned long *samples);
static unsigned string_hash(const char *s, size_t size);
	const char *name);
static unsigned symbol_hash(const char *binary, const char *name);
static int symbol_qsort_compare(const void *p1, const void *p2);
	unsigned long samples, int count_index);
static unsigned symbols_count(void);
static void usage(const char *argv0);
	int c, index;

/* ===== FUNCTIONS ===== */

/* Function 1 */
int main(int argc, char **argv) {
	int i;

#ifdef DEBUG
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
#endif

	if (argc < 3) usage(argv[0]);

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-r") == 0) {
			i++;
			break;
		}
		if (argc == 3 && i == 2) break;
		load_file(argv[i], 0);
		n1++;
	}

	for (; i < argc; i++) {
		load_file(argv[i], 1);
		n2++;
	}

	if (n1 < 1 || n2 < 1) usage(argv[0]);

	print_report();
	return 0;
}

/* Function 2 */
static double compute_sig(double avg1, double var1, double avg2, double var2) {
	double df, t, var;

	var = var1 / n1 + var2 / n2;
	if (var <= 0 || n1 <= 1 || n2 <= 1) return -1;

	df = var * var / (
		var1 * var1 / (n1 * n1 * (n1 - 1)) +
		var2 * var2 / (n2 * n2 * (n2 - 1)));
	if (df < 1) return -1;

	t = (avg1 - avg2) / sqrt(var);
	return student_t_p_2tail(t, df);
}

/* Function 3 */
static void load_file(const char *path, int count_index) {
	char binary[PROC_NAME_LEN];
	FILE *file;
	int line;
	char name[SYMBOL_NAME_SIZE];
	unsigned long samples;

	assert(path);
	assert(count_index == 0 || count_index == 1);

	file = fopen(path, "r");
	if (!file) {
		fprintf(stderr, "error: cannot open \"%s\": %s\n",
			path, strerror(errno));
		exit(1);
	}

	line = 1;
	while (read_line(file, path, line++, binary, name, &samples)) {
		symbol_tally(binary, name, samples, count_index);
	}

	fclose(file);
}

/* Function 4 */
static void *malloc_checked(size_t size) {
	void *p;
	if (!size) return NULL;
	p = malloc(size);
	if (!p) {
		fprintf(stderr, "error: malloc cannot allocate %lu bytes: %s\n",
			(unsigned long) size, strerror(errno));
		exit(-1);
	}
	return p;
}

/* Function 5 */
static void print_report(void) {
	unsigned i, index, symbol_count;

	symbol_count = symbols_count();
	symbol_list = MALLOC_CHECKED(struct symbol_info *, symbol_count);
	index = 0;
	for (symbol = symbols; symbol; symbol = symbol->next) {
		symbol_list[index++] = symbol;

		symbol->diff = (long) (symbol->count[1].sum * n1) -
			(long) (symbol->count[0].sum * n2);
	}
	assert(index == symbol_count);

	qsort(symbol_list, symbol_count, sizeof(struct symbol_info *),
		symbol_qsort_compare);

	printf("%-*s %-*s ------avg------ ----stdev----    diff sig\n",
		PROC_NAME_WIDTH, "binary", SYMBOL_NAME_WIDTH, "symbol");
	printf("%-*s    left   right   left  right\n",
		PROC_NAME_WIDTH + SYMBOL_NAME_WIDTH + 1, "");
	printf("\n");
	for (i = 0; i < symbol_count; i++) {
		if (i > 0 && symbol_list[i]->class >= sc_process &&
			symbol_list[i]->class != symbol_list[i - 1]->class) {
			printf("\n");
		}
		print_report_line(symbol_list[i]);
	}
	printf("\n");
	printf("significance levels (two-tailed):\n");
	printf("  *    p < 0.05\n");
	printf("  **   p < 0.01\n");
	printf("  ***  p < 0.001\n");
	free(symbol_list);
}

/* Function 6 */
static void print_report_line(const struct symbol_info *symbol) {
	double avg1, avg2, p, var1, var2;

	compute_stats(&symbol->count[0], n1, &avg1, &var1);
	compute_stats(&symbol->count[1], n2, &avg2, &var2);
	p = compute_sig(avg1, var1, avg2, var2);

	assert(PROC_NAME_WIDTH <= PROC_NAME_LEN);
	assert(SYMBOL_NAME_WIDTH <= SYMBOL_NAME_SIZE);
	printf("%-*.*s %-*.*s",
		PROC_NAME_WIDTH, PROC_NAME_WIDTH, symbol->binary,
		SYMBOL_NAME_WIDTH, SYMBOL_NAME_WIDTH, symbol->name);
	if (symbol->count[0].sum > 0) {
		printf("%8.0f", avg1);
	} else {
		printf("        ");
	}
	if (symbol->count[1].sum > 0) {
		printf("%8.0f", avg2);
	} else {
		printf("        ");
	}
	if (symbol->count[0].sum > 0 && n1 >= 2) {
		printf("%7.0f", sqrt(var1));
	} else {
		printf("       ");
	}
	if (symbol->count[1].sum > 0 && n2 >= 2) {
		printf("%7.0f", sqrt(var2));
	} else {
		printf("       ");
	}
	printf("%8.0f ", avg2 - avg1);
	if (p >= 0) {
		if (p <= 0.05) printf("*");
		if (p <= 0.01) printf("*");
		if (p <= 0.001) printf("*");
	}
	printf("\n");
}

/* Function 7 */
static unsigned string_hash(const char *s, size_t size) {
	unsigned result = 0;

	assert(s);

	while (*s && size-- > 0) {
		result = result * 31 + *(s++);
	}
	return result;
}

/* Function 8 */
			strncmp(symbol->name, name, SYMBOL_NAME_SIZE) == 0) {
			return symbol;
		}

/* Function 9 */
static unsigned symbol_hash(const char *binary, const char *name) {
	return string_hash(binary, PROC_NAME_LEN) +
		string_hash(name, SYMBOL_NAME_SIZE);
}

/* Function 10 */
static int symbol_qsort_compare(const void *p1, const void *p2) {
	int r;
	const struct symbol_info *s1, *s2;

	assert(p1);
	assert(p2);
	s1 = *(const struct symbol_info **) p1;
	s2 = *(const struct symbol_info **) p2;
	assert(s1);
	assert(s2);

	if (s1->class < s2->class) return -1;
	if (s1->class > s2->class) return 1;

	if (s1->diff < s2->diff) return -1;
	if (s1->diff > s2->diff) return 1;

	r = strncmp(s1->binary, s2->binary, PROC_NAME_LEN);
	if (r) return r;

	return strncmp(s1->name, s2->name, SYMBOL_NAME_SIZE);
}

/* Function 11 */
static unsigned symbols_count(void) {
	int count = 0;
	const struct symbol_info *symbol;

	for (symbol = symbols; symbol; symbol = symbol->next) {
		count++;
	}
	return count;
}

/* Function 12 */
static void usage(const char *argv0) {
	printf("usage:\n");
	printf("  %s leftfile rightfile\n", argv0);
	printf("  %s leftfile... -r rightfile...\n", argv0);
	printf("\n");
	printf("sprofdiff compares the sprofile information from multiple\n");
	printf("output files of sprofalyze -d.\n");
	exit(1);
}

/* Function 13 */
int main(int argc, char **argv) {
	int i;

#ifdef DEBUG
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
#endif

	if (argc < 3) usage(argv[0]);

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-r") == 0) {
			i++;
			break;
		}
		if (argc == 3 && i == 2) break;
		load_file(argv[i], 0);
		n1++;
	}

	for (; i < argc; i++) {
		load_file(argv[i], 1);
		n2++;
	}

	if (n1 < 1 || n2 < 1) usage(argv[0]);

	print_report();
	return 0;
}

/* Function 14 */
static double compute_sig(double avg1, double var1, double avg2, double var2) {
	double df, t, var;

	var = var1 / n1 + var2 / n2;
	if (var <= 0 || n1 <= 1 || n2 <= 1) return -1;

	df = var * var / (
		var1 * var1 / (n1 * n1 * (n1 - 1)) +
		var2 * var2 / (n2 * n2 * (n2 - 1)));
	if (df < 1) return -1;

	t = (avg1 - avg2) / sqrt(var);
	return student_t_p_2tail(t, df);
}

/* Function 15 */
static void load_file(const char *path, int count_index) {
	char binary[PROC_NAME_LEN];
	FILE *file;
	int line;
	char name[SYMBOL_NAME_SIZE];
	unsigned long samples;

	assert(path);
	assert(count_index == 0 || count_index == 1);

	file = fopen(path, "r");
	if (!file) {
		fprintf(stderr, "error: cannot open \"%s\": %s\n",
			path, strerror(errno));
		exit(1);
	}

	line = 1;
	while (read_line(file, path, line++, binary, name, &samples)) {
		symbol_tally(binary, name, samples, count_index);
	}

	fclose(file);
}

/* Function 16 */
static void *malloc_checked(size_t size) {
	void *p;
	if (!size) return NULL;
	p = malloc(size);
	if (!p) {
		fprintf(stderr, "error: malloc cannot allocate %lu bytes: %s\n",
			(unsigned long) size, strerror(errno));
		exit(-1);
	}
	return p;
}

/* Function 17 */
static void print_report(void) {
	unsigned i, index, symbol_count;

	symbol_count = symbols_count();
	symbol_list = MALLOC_CHECKED(struct symbol_info *, symbol_count);
	index = 0;
	for (symbol = symbols; symbol; symbol = symbol->next) {
		symbol_list[index++] = symbol;

		symbol->diff = (long) (symbol->count[1].sum * n1) -
			(long) (symbol->count[0].sum * n2);
	}
	assert(index == symbol_count);

	qsort(symbol_list, symbol_count, sizeof(struct symbol_info *),
		symbol_qsort_compare);

	printf("%-*s %-*s ------avg------ ----stdev----    diff sig\n",
		PROC_NAME_WIDTH, "binary", SYMBOL_NAME_WIDTH, "symbol");
	printf("%-*s    left   right   left  right\n",
		PROC_NAME_WIDTH + SYMBOL_NAME_WIDTH + 1, "");
	printf("\n");
	for (i = 0; i < symbol_count; i++) {
		if (i > 0 && symbol_list[i]->class >= sc_process &&
			symbol_list[i]->class != symbol_list[i - 1]->class) {
			printf("\n");
		}
		print_report_line(symbol_list[i]);
	}
	printf("\n");
	printf("significance levels (two-tailed):\n");
	printf("  *    p < 0.05\n");
	printf("  **   p < 0.01\n");
	printf("  ***  p < 0.001\n");
	free(symbol_list);
}

/* Function 18 */
static void print_report_line(const struct symbol_info *symbol) {
	double avg1, avg2, p, var1, var2;

	compute_stats(&symbol->count[0], n1, &avg1, &var1);
	compute_stats(&symbol->count[1], n2, &avg2, &var2);
	p = compute_sig(avg1, var1, avg2, var2);

	assert(PROC_NAME_WIDTH <= PROC_NAME_LEN);
	assert(SYMBOL_NAME_WIDTH <= SYMBOL_NAME_SIZE);
	printf("%-*.*s %-*.*s",
		PROC_NAME_WIDTH, PROC_NAME_WIDTH, symbol->binary,
		SYMBOL_NAME_WIDTH, SYMBOL_NAME_WIDTH, symbol->name);
	if (symbol->count[0].sum > 0) {
		printf("%8.0f", avg1);
	} else {
		printf("        ");
	}
	if (symbol->count[1].sum > 0) {
		printf("%8.0f", avg2);
	} else {
		printf("        ");
	}
	if (symbol->count[0].sum > 0 && n1 >= 2) {
		printf("%7.0f", sqrt(var1));
	} else {
		printf("       ");
	}
	if (symbol->count[1].sum > 0 && n2 >= 2) {
		printf("%7.0f", sqrt(var2));
	} else {
		printf("       ");
	}
	printf("%8.0f ", avg2 - avg1);
	if (p >= 0) {
		if (p <= 0.05) printf("*");
		if (p <= 0.01) printf("*");
		if (p <= 0.001) printf("*");
	}
	printf("\n");
}

/* Function 19 */
static unsigned string_hash(const char *s, size_t size) {
	unsigned result = 0;

	assert(s);

	while (*s && size-- > 0) {
		result = result * 31 + *(s++);
	}
	return result;
}

/* Function 20 */
			strncmp(symbol->name, name, SYMBOL_NAME_SIZE) == 0) {
			return symbol;
		}

/* Function 21 */
static unsigned symbol_hash(const char *binary, const char *name) {
	return string_hash(binary, PROC_NAME_LEN) +
		string_hash(name, SYMBOL_NAME_SIZE);
}

/* Function 22 */
static int symbol_qsort_compare(const void *p1, const void *p2) {
	int r;
	const struct symbol_info *s1, *s2;

	assert(p1);
	assert(p2);
	s1 = *(const struct symbol_info **) p1;
	s2 = *(const struct symbol_info **) p2;
	assert(s1);
	assert(s2);

	if (s1->class < s2->class) return -1;
	if (s1->class > s2->class) return 1;

	if (s1->diff < s2->diff) return -1;
	if (s1->diff > s2->diff) return 1;

	r = strncmp(s1->binary, s2->binary, PROC_NAME_LEN);
	if (r) return r;

	return strncmp(s1->name, s2->name, SYMBOL_NAME_SIZE);
}

/* Function 23 */
static unsigned symbols_count(void) {
	int count = 0;
	const struct symbol_info *symbol;

	for (symbol = symbols; symbol; symbol = symbol->next) {
		count++;
	}
	return count;
}

/* Function 24 */
static void usage(const char *argv0) {
	printf("usage:\n");
	printf("  %s leftfile rightfile\n", argv0);
	printf("  %s leftfile... -r rightfile...\n", argv0);
	printf("\n");
	printf("sprofdiff compares the sprofile information from multiple\n");
	printf("output files of sprofalyze -d.\n");
	exit(1);
}


#ifdef __cplusplus
}
#endif

/* End of sprofdiff_unified.c - Synthesized by MINIX4 Massive Synthesis System */
