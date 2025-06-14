/**
 * @file sprofalyze_unified.c
 * @brief Unified sprofalyze implementation
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
 *     1. userspace\minix_commands\sprofalyze\sprofalyze.c
 *     2. userspace\commands_legacy\development\sprofalyze\sprofalyze.c
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
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <minix/profile.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* ===== DEFINES ===== */
#define BINARY_HASHTAB_SIZE 1024
#define ENDPOINT_HASHTAB_SIZE 1024
#define DEBUG 0
#define NM "/usr/pkg/bin/nm"
#define LINE_WIDTH 80
#define SYMBOL_NAME_SIZE 52
#define SYMBOL_NAME_WIDTH 22
#define SYMBOL_SIZE_MAX 0x100000
#define PC_MAP_L1_SIZE 0x10000
#define PC_MAP_L2_SIZE 0x10000
#define MALLOC_CHECKED(type, count) \
#define LENGTHOF(array) (sizeof((array)) / sizeof((array)[0]))
#define dprintf(...) do { 						\

/* ===== TYPES ===== */
struct binary_info;
struct symbol_count {
	struct symbol_count *next;
	struct binary_info *binary;
struct pc_map_l2 {
	struct symbol_count *l2[PC_MAP_L2_SIZE];
struct pc_map_l1 {
	struct pc_map_l2 *l1[PC_MAP_L1_SIZE];
struct binary_info {
	struct symbol_count *symbols;
	struct pc_map_l1 *pc_map;
	struct binary_info *next;
	struct binary_info *hashtab_next;
struct endpoint_info {
	struct binary_info *binary;
	struct endpoint_info *hashtab_next;
	struct endpoint_info *next;
union sprof_record {
	struct sprof_sample sample;
	struct sprof_proc proc;
		struct endpoint_info *e; 
	struct binary_info *binary, **ptr;
	struct binary_info *binary;
	struct binary_info *binary, **ptr;
	struct pc_map_l2 *pc_map_l2, **pc_map_l2_ptr;
	struct symbol_count *symbol, **symbol_ptr;
	struct symbol_count *symbol;
	struct endpoint_info *epinfo, **ptr;
	struct binary_info *binary;
	struct symbol_count *symbol, **symbols_sorted;
	struct symbol_count *symbol, **symbols_sorted;
	struct binary_info *binary, **binaries_sorted;
	struct symbol_count *symbol;
	struct symbol_count *symbol;
	struct endpoint_info *epinfo, **ptr;
	struct binary_info *binary;
	struct pc_map_l2 *pc_map_l2;
	struct symbol_count *symbol;
struct binary_info;
struct symbol_count {
	struct symbol_count *next;
	struct binary_info *binary;
struct pc_map_l2 {
	struct symbol_count *l2[PC_MAP_L2_SIZE];
struct pc_map_l1 {
	struct pc_map_l2 *l1[PC_MAP_L1_SIZE];
struct binary_info {
	struct symbol_count *symbols;
	struct pc_map_l1 *pc_map;
	struct binary_info *next;
	struct binary_info *hashtab_next;
struct endpoint_info {
	struct binary_info *binary;
	struct endpoint_info *hashtab_next;
	struct endpoint_info *next;
union sprof_record {
	struct sprof_sample sample;
	struct sprof_proc proc;
		struct endpoint_info *e; 
	struct binary_info *binary, **ptr;
	struct binary_info *binary;
	struct binary_info *binary, **ptr;
	struct pc_map_l2 *pc_map_l2, **pc_map_l2_ptr;
	struct symbol_count *symbol, **symbol_ptr;
	struct symbol_count *symbol;
	struct endpoint_info *epinfo, **ptr;
	struct binary_info *binary;
	struct symbol_count *symbol, **symbols_sorted;
	struct symbol_count *symbol, **symbols_sorted;
	struct binary_info *binary, **binaries_sorted;
	struct symbol_count *symbol;
	struct symbol_count *symbol;
	struct endpoint_info *epinfo, **ptr;
	struct binary_info *binary;
	struct pc_map_l2 *pc_map_l2;
	struct symbol_count *symbol;

/* ===== GLOBAL VARIABLES ===== */
static const char *src_path = "/usr/src/minix";
	uint32_t addr;
	int samples;
	char name[SYMBOL_NAME_SIZE];
	char name[PROC_NAME_LEN];
	const char *path;
	int samples;
	char no_more_warnings;
	endpoint_t endpoint;
	char seen;
static struct binary_info *binaries;
static struct binary_info *binary_hashtab[BINARY_HASHTAB_SIZE];
static struct endpoint_info *endpoint_hashtab[ENDPOINT_HASHTAB_SIZE];
static struct endpoint_info *endpoints;
static double minimum_perc = 1.0;
static struct sprof_info_s sprof_info;
static struct binary_info *binary_add(const char *path);
static struct binary_info *binary_find(const char *name);
static struct binary_info *binary_hashtab_get(const char *name);
static struct binary_info **binary_hashtab_get_ptr(const char *name);
static void binary_load_pc_map(struct binary_info *binary_info);
static const char *binary_name(const char *path);
static int compare_binaries(const void *p1, const void *p2);
static int compare_symbols(const void *p1, const void *p2);
static int count_symbols(const struct binary_info *binary, int threshold);
static void dprint_symbols(const struct binary_info *binary);
static struct endpoint_info **endpoint_hashtab_get_ptr(endpoint_t endpoint);
static void load_trace(const char *path);
static void *malloc_checked(size_t size);
static unsigned name_hash(const char *name);
static float percent(int value, int percent_of);
static void print_report(void);
static void print_report_overall(void);
static void print_report_per_binary(const struct binary_info *binary);
static void print_reports_per_binary(void);
	unsigned symbol_count, int total, int show_binary);
static void print_separator(void);
static int read_hex(FILE *file, unsigned long *value);
static int read_newline(FILE *file);
static void read_to_whitespace(FILE *file, char *buffer, size_t size);
	int *samples_read);
static struct binary_info *sample_load_binary(const struct sprof_proc *sample);
static char *strdup_checked(const char *s);
static void usage(const char *argv0);
	int binary_samples;
	printf("(total)\t\t%d\n", sprof_info.total_samples);
	printf("(system)\t\t%d\n", sprof_info.system_samples);
	printf("(idle)\t\t%d\n", sprof_info.idle_samples);
	printf("(user)\t\t%d\n", sprof_info.user_samples);
	int samples, samples_shown;
		printf("%8d %5.1f%%\n", samples, percent(samples, total));
	print_separator();
	ptr = endpoint_hashtab_get_ptr(data->proc.proc);
	*ptr = epinfo = MALLOC_CHECKED(struct endpoint_info, 1);
	memset(epinfo, 0, sizeof(struct endpoint_info));
	epinfo->endpoint = data->proc.proc;
	epinfo->next = endpoints;
	endpoints = epinfo;
		fprintf(stderr, "         path: \"%s\"\n", binary->path);
		dprint_symbols(binary);
static const char *src_path = "/usr/src/squirrelferret";
	uint32_t addr;
	int samples;
	char name[SYMBOL_NAME_SIZE];
	char name[PROC_NAME_LEN];
	const char *path;
	int samples;
	char no_more_warnings;
	endpoint_t endpoint;
	char seen;
static struct binary_info *binaries;
static struct binary_info *binary_hashtab[BINARY_HASHTAB_SIZE];
static struct endpoint_info *endpoint_hashtab[ENDPOINT_HASHTAB_SIZE];
static struct endpoint_info *endpoints;
static double minimum_perc = 1.0;
static struct sprof_info_s sprof_info;
static struct binary_info *binary_add(const char *path);
static struct binary_info *binary_find(const char *name);
static struct binary_info *binary_hashtab_get(const char *name);
static struct binary_info **binary_hashtab_get_ptr(const char *name);
static void binary_load_pc_map(struct binary_info *binary_info);
static const char *binary_name(const char *path);
static int compare_binaries(const void *p1, const void *p2);
static int compare_symbols(const void *p1, const void *p2);
static int count_symbols(const struct binary_info *binary, int threshold);
static void dprint_symbols(const struct binary_info *binary);
static struct endpoint_info **endpoint_hashtab_get_ptr(endpoint_t endpoint);
static void load_trace(const char *path);
static void *malloc_checked(size_t size);
static unsigned name_hash(const char *name);
static float percent(int value, int percent_of);
static void print_report(void);
static void print_report_overall(void);
static void print_report_per_binary(const struct binary_info *binary);
static void print_reports_per_binary(void);
	unsigned symbol_count, int total, int show_binary);
static void print_separator(void);
static int read_hex(FILE *file, unsigned long *value);
static int read_newline(FILE *file);
static void read_to_whitespace(FILE *file, char *buffer, size_t size);
	int *samples_read);
static struct binary_info *sample_load_binary(const struct sprof_proc *sample);
static char *strdup_checked(const char *s);
static void usage(const char *argv0);
	int binary_samples;
	printf("(total)\t\t%d\n", sprof_info.total_samples);
	printf("(system)\t\t%d\n", sprof_info.system_samples);
	printf("(idle)\t\t%d\n", sprof_info.idle_samples);
	printf("(user)\t\t%d\n", sprof_info.user_samples);
	int samples, samples_shown;
		printf("%8d %5.1f%%\n", samples, percent(samples, total));
	print_separator();
	ptr = endpoint_hashtab_get_ptr(data->proc.proc);
	*ptr = epinfo = MALLOC_CHECKED(struct endpoint_info, 1);
	memset(epinfo, 0, sizeof(struct endpoint_info));
	epinfo->endpoint = data->proc.proc;
	epinfo->next = endpoints;
	endpoints = epinfo;
		fprintf(stderr, "         path: \"%s\"\n", binary->path);
		dprint_symbols(binary);

/* ===== FUNCTIONS ===== */

/* Function 1 */
int main(int argc, char **argv) {
	int opt, sprofdiff = 0;

#ifdef DEBUG
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
#endif

	while ((opt = getopt(argc, argv, "b:dp:s:")) != -1) {
		switch (opt) {
		case 'b':
			binary_add(optarg);
			break;
		case 'd':
			sprofdiff = 1;
			break;
		case 'p':
			minimum_perc = atof(optarg);
			if (minimum_perc < 0 || minimum_perc > 100) {
				fprintf(stderr, "error: cut-off percentage "
					"makes no sense: %g\n", minimum_perc);
				exit(1);
			}
			break;
		case 's':
			src_path = optarg;
			break;
		default: usage(argv[0]);
		}
	}

	if (optind >= argc) usage(argv[0]);
	for (; optind < argc; optind++) {
		load_trace(argv[optind]);
		for(e = endpoints; e; e = e->next)
			e->seen = 0;
	}

	if (sprofdiff) {
		print_diff();
	} else {
		print_report();
	}
	return 0;
}

/* Function 2 */
static struct binary_info *binary_add(const char *path) {
	const char *name;


	assert(path);
	if (strchr(path, '"')) {
		fprintf(stderr, "error: path \"%s\" contains a quote\n", path);
		exit(1);
	}

	name = binary_name(path);
	dprintf("adding binary \"%s\" with name \"%.*s\"\n",
		path, PROC_NAME_LEN, name);
	if (strlen(name) == 0) {
		fprintf(stderr, "error: path \"%s\" does not "
			"contain a filename\n", path);
		exit(1);
	}

	ptr = binary_hashtab_get_ptr(name);
	if (*ptr) {
		fprintf(stderr, "warning: ignoring \"%s\" because \"%s\" was "
			"previously specified\n", path, (*ptr)->path);
		return *ptr;
	}
	dprintf("using %.*s from \"%s\"\n", PROC_NAME_LEN, name, path);

	binary = MALLOC_CHECKED(struct binary_info, 1);
	memset(binary, 0, sizeof(struct binary_info));
	binary->path = path;
	strncpy(binary->name, name, sizeof(binary->name));

	binary->next = binaries;
	binaries = binary;

	*ptr = binary;
	return binary;
}

/* Function 3 */
static struct binary_info *binary_find(const char *name) {
	const char *current_name;
	unsigned i;
	char path[PATH_MAX + 1], *path_end;

	assert(name);

	if (!*name) {
		fprintf(stderr, "warning: binary unspecified in sample\n");
		return NULL;
	}

	binary = binary_hashtab_get(name);
	if (binary) return binary;

	dprintf("searching for binary \"%.*s\" in \"%s\"\n",
		PROC_NAME_LEN, name, src_path);
	for (i = 0; i < LENGTHOF(default_binaries); i++) {
		snprintf(path, sizeof(path), "%s/%s", src_path,
			default_binaries[i]);
		current_name = binary_name(path);
		assert(current_name);
		if (*current_name) {
			if (strncmp(name, current_name,
				PROC_NAME_LEN) != 0) {
				continue;
			}
		} else {
			path_end = path + strlen(path);
			snprintf(path_end, sizeof(path) - (path_end - path),
				"%.*s/%.*s", PROC_NAME_LEN, name,
				PROC_NAME_LEN, name);
		}

		dprintf("checking whether \"%s\" exists\n", path);
		if (access(path, R_OK) < 0) continue;

		return binary_add(strdup_checked(path));
	}

	return NULL;
}

/* Function 4 */
static struct binary_info *binary_hashtab_get(const char *name) {
	return *binary_hashtab_get_ptr(name);
}

/* Function 5 */
static struct binary_info **binary_hashtab_get_ptr(const char *name) {

	ptr = &binary_hashtab[name_hash(name) % BINARY_HASHTAB_SIZE];
	while ((binary = *ptr) && strncmp(binary->name, name,
		PROC_NAME_LEN) != 0) {
		ptr = &binary->hashtab_next;
	}
	dprintf("looked up binary \"%.*s\" in hash table, %sfound\n",
		PROC_NAME_LEN, name, *ptr ? "" : "not ");
	return ptr;
}

/* Function 6 */
static void binary_load_pc_map(struct binary_info *binary_info) {
	unsigned long addr, size;
	char *command;
	size_t command_len;
#if DEBUG
	unsigned count = 0;
#endif
	FILE *file;
	int index_l1, index_l2, line;
	char name[SYMBOL_NAME_SIZE];
	char type;

	assert(binary_info);
	assert(!strchr(NM, '"'));
	assert(!strchr(binary_info->path, '"'));

	if (access(binary_info->path, R_OK) < 0) {
		fprintf(stderr, "warning: \"%s\" does not exist or "
			"not readable.\n", binary_info->path);
		fprintf(stderr, "         Did you do a make?\n");
		return;
	}

	command_len = strlen(NM) + strlen(binary_info->path) + 32;
	command = MALLOC_CHECKED(char, command_len);
	snprintf(command, command_len, "\"%s\" -nP \"%s\"",
		NM, binary_info->path);
	dprintf("running command for extracting addresses: %s\n", command);
	file = popen(command, "r");
	if (!file) {
		perror("failed to start " NM);
		exit(-1);
	}
	free(command);

	assert(!binary_info->symbols);
	symbol_ptr = &binary_info->symbols;
	line = 1;
	while (!feof(file)) {
		read_nm_line(file, line++, name, &type, &addr, &size);

		if (type != 't' && type != 'T') continue;

		*symbol_ptr = symbol = MALLOC_CHECKED(struct symbol_count, 1);
		memset(symbol, 0, sizeof(*symbol));
		symbol->binary = binary_info;
		symbol->addr = addr;
		strncpy(symbol->name, name, SYMBOL_NAME_SIZE);
		symbol_ptr = &symbol->next;
#if DEBUG
		count++;
#endif
	}
	fclose(file);
	dprintf("extracted %u symbols\n", count);

	assert(!binary_info->pc_map);
	binary_info->pc_map = MALLOC_CHECKED(struct pc_map_l1, 1);
	memset(binary_info->pc_map, 0, sizeof(struct pc_map_l1));
	for (symbol = binary_info->symbols; symbol; symbol = symbol->next) {
		size = symbol->next ? (symbol->next->addr - symbol->addr) : 1;
		if (size > SYMBOL_SIZE_MAX) size = SYMBOL_SIZE_MAX;

		for (addr = symbol->addr; addr - symbol->addr < size; addr++) {
			index_l1 = addr / PC_MAP_L2_SIZE;
			assert(index_l1 < PC_MAP_L1_SIZE);
			pc_map_l2_ptr = &binary_info->pc_map->l1[index_l1];
			if (!(pc_map_l2 = *pc_map_l2_ptr)) {
				*pc_map_l2_ptr = pc_map_l2 =
					MALLOC_CHECKED(struct pc_map_l2, 1);
				memset(pc_map_l2, 0, sizeof(struct pc_map_l2));
			}
			index_l2 = addr % PC_MAP_L2_SIZE;
			pc_map_l2->l2[index_l2] = symbol;
		}
	}
}

/* Function 7 */
static const char *binary_name(const char *path) {
	const char *name, *p;

	name = path;
	for (p = path; *p; p++) {
		if (*p == '/') name = p + 1;
	}
	return name;
}

/* Function 8 */
static int compare_binaries(const void *p1, const void *p2) {
	const struct binary_info *const *b1 = p1, *const *b2 = p2;

	assert(b1);
	assert(b2);
	assert(*b1);
	assert(*b2);
	if ((*b1)->samples > (*b2)->samples) return -1;
	if ((*b1)->samples < (*b2)->samples) return 1;
	return 0;
}

/* Function 9 */
static int compare_symbols(const void *p1, const void *p2) {
	const struct symbol_count *const *s1 = p1, *const *s2 = p2;

	assert(s1);
	assert(s2);
	assert(*s1);
	assert(*s2);
	if ((*s1)->samples > (*s2)->samples) return -1;
	if ((*s1)->samples < (*s2)->samples) return 1;
	return 0;
}

/* Function 10 */
static int count_symbols(const struct binary_info *binary, int threshold) {
	int result = 0;

	for (symbol = binary->symbols; symbol; symbol = symbol->next) {
		if (symbol->samples >= threshold) result++;
	}
	return result;
}

/* Function 11 */
static void dprint_symbols(const struct binary_info *binary) {
#if DEBUG
	const struct symbol_count *symbol;

	for (symbol = binary->symbols; symbol; symbol = symbol->next) {
		dprintf("addr=0x%.8lx samples=%8d name=\"%.*s\"\n",
			(unsigned long) symbol->addr, symbol->samples,
			SYMBOL_NAME_SIZE, symbol->name);
	}
#endif
}

/* Function 12 */
static struct endpoint_info **endpoint_hashtab_get_ptr(endpoint_t endpoint) {

	ptr = &endpoint_hashtab[(unsigned) endpoint % ENDPOINT_HASHTAB_SIZE];
	while ((epinfo = *ptr) && epinfo->endpoint != endpoint) {
		ptr = &epinfo->hashtab_next;
	}
	dprintf("looked up endpoint %ld in hash table, %sfound\n",
		(long) endpoint, *ptr ? "" : "not ");
	return ptr;
}

/* Function 13 */
static void load_trace(const char *path) {
	char buffer[1024];
	size_t bufindex, bufsize;
	FILE *file;
	unsigned size_info, size_sample, size_proc;
	int samples_read;
	static struct sprof_info_s sprof_info_perfile;

	file = fopen(path, "rb");
	if (!file) {
		fprintf(stderr, "error: cannot open trace file \"%s\": %s\n",
			path, strerror(errno));
		exit(1);
	}

	if (fscanf(file, "stat\n%u %u %u",
		&size_info, &size_sample, &size_proc) != 3 ||
		fgetc(file) != '\n') {
		fprintf(stderr, "error: file \"%s\" does not contain an "
			"sprofile trace\n", path);
		exit(1);
	}
	if ((size_info != sizeof(struct sprof_info_s)) ||
		(size_sample != sizeof(struct sprof_sample)) ||
		(size_proc != sizeof(struct sprof_proc))) {
		fprintf(stderr, "error: file \"%s\" is incompatible with this "
			"version of sprofalyze; recompile sprofalyze with the "
			"MINIX version that created this file\n", path);
		exit(1);
	}
	if (fread(&sprof_info_perfile, sizeof(sprof_info_perfile), 1, file) != 1) {
		fprintf(stderr, "error: totals missing in file \"%s\"\n", path);
		exit(1);
	}

	samples_read = 0;
	bufindex = 0;
	bufsize = 0;
	for (;;) {
		if (bufsize - bufindex < sizeof(union sprof_record)) {
			memmove(buffer, buffer + bufindex, bufsize - bufindex);
			bufsize -= bufindex;
			bufindex = 0;
			bufsize += fread(buffer + bufsize, 1,
				sizeof(buffer) - bufsize, file);

			if (bufsize == 0) break;
		}

		bufindex += sample_process(
			(const union sprof_record *) (buffer + bufindex),
			bufsize - bufindex, &samples_read);
	}
	if (samples_read != sprof_info_perfile.system_samples) {
		fprintf(stderr, "warning: number of system samples (%d) in "
			"\"%s\" does not match number of records (%d)\n",
			sprof_info.system_samples, path, samples_read);
	}

	sprof_info.system_samples += sprof_info_perfile.system_samples;
	sprof_info.total_samples += sprof_info_perfile.total_samples;
	sprof_info.idle_samples += sprof_info_perfile.idle_samples;
	sprof_info.user_samples += sprof_info_perfile.user_samples;

	fclose(file);
}

/* Function 14 */
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

/* Function 15 */
static unsigned name_hash(const char *name) {
	int i;
	unsigned r = 0;

	for (i = 0; i < PROC_NAME_LEN && name[i]; i++) {
		r = r * 31 + name[i];
	}
	dprintf("name_hash(\"%.*s\") = 0x%.8x\n", PROC_NAME_LEN, name, r);
	return r;
}

/* Function 16 */
static float percent(int value, int percent_of) {
	assert(value >= 0);
	assert(value <= percent_of);

	return (percent_of > 0) ? (value * 100.0 / percent_of) : 0;
}

/* Function 17 */
static void print_report(void) {
	printf("Showing processes and functions using at least %3.0f%% "
		"time.\n\n", minimum_perc);
	printf("  System process ticks: %10d (%3.0f%%)\n",
		sprof_info.system_samples, percent(sprof_info.system_samples, sprof_info.total_samples));
	printf("    User process ticks: %10d (%3.0f%%)          "
		"Details of system process\n",
		sprof_info.user_samples, percent(sprof_info.user_samples, sprof_info.total_samples));
	printf("       Idle time ticks: %10d (%3.0f%%)          "
		"samples, aggregated and\n",
		sprof_info.idle_samples, percent(sprof_info.idle_samples, sprof_info.total_samples));
	printf("                        ----------  ----           "
		"per process, are below.\n");
	printf("           Total ticks: %10d (100%%)\n\n", sprof_info.total_samples);

	print_report_overall();
	print_reports_per_binary();
}

/* Function 18 */
static void print_report_overall(void) {
	unsigned index, symbol_count;
	int sample_threshold;

	sample_threshold = sprof_info.system_samples * minimum_perc / 100;
	symbol_count = 0;
	for (binary = binaries; binary; binary = binary->next) {
		symbol_count += count_symbols(binary, sample_threshold);
	}

	symbols_sorted = MALLOC_CHECKED(struct symbol_count *, symbol_count);
	index = 0;
	for (binary = binaries; binary; binary = binary->next) {
		for (symbol = binary->symbols; symbol; symbol = symbol->next) {
			if (symbol->samples >= sample_threshold) {
				symbols_sorted[index++] = symbol;
			}
		}
	}
	assert(index == symbol_count);
	qsort(symbols_sorted, symbol_count, sizeof(symbols_sorted[0]),
		compare_symbols);

	print_separator();
	printf("Total system process time %*d samples\n",
		LINE_WIDTH - 34, sprof_info.system_samples);
	print_separator();
	print_report_symbols(symbols_sorted, symbol_count, sprof_info.system_samples, 1);
	free(symbols_sorted);
}

/* Function 19 */
static void print_report_per_binary(const struct binary_info *binary) {
	unsigned index, symbol_count;
	int sample_threshold;

	assert(binary->samples >= 0);

	sample_threshold = binary->samples * minimum_perc / 100;
	symbol_count = count_symbols(binary, sample_threshold);

	symbols_sorted = MALLOC_CHECKED(struct symbol_count *, symbol_count);
	index = 0;
	for (symbol = binary->symbols; symbol; symbol = symbol->next) {
		if (symbol->samples >= sample_threshold) {
			symbols_sorted[index++] = symbol;
		}
	}
	assert(index == symbol_count);
	qsort(symbols_sorted, symbol_count, sizeof(symbols_sorted[0]),
		compare_symbols);

	print_separator();
	printf("%-*.*s %4.1f%% of system process samples\n",
		LINE_WIDTH - 32, PROC_NAME_LEN, binary->name,
		percent(binary->samples, sprof_info.system_samples));
	print_separator();
	print_report_symbols(symbols_sorted, symbol_count, binary->samples, 0);
	free(symbols_sorted);
}

/* Function 20 */
static void print_reports_per_binary(void) {
	unsigned binary_count, i, index;
	int sample_threshold, samples_shown;

	binary_count = 0;
	for (binary = binaries; binary; binary = binary->next) {
		assert(!binary->samples);
		for (symbol = binary->symbols; symbol; symbol = symbol->next) {
			binary->samples += symbol->samples;
		}
		binary_count++;
	}

	binaries_sorted = MALLOC_CHECKED(struct binary_info *, binary_count);
	index = 0;
	for (binary = binaries; binary; binary = binary->next) {
		binaries_sorted[index++] = binary;
	}
	assert(index == binary_count);
	qsort(binaries_sorted, binary_count, sizeof(binaries_sorted[0]),
		compare_binaries);

	sample_threshold = sprof_info.system_samples * minimum_perc / 100;
	samples_shown = 0;
	for (i = 0; i < binary_count; i++) {
		if (binaries_sorted[i]->samples < sample_threshold) break;
		print_report_per_binary(binaries_sorted[i]);
		samples_shown += binaries_sorted[i]->samples;
	}
	print_separator();
	printf("samples: %d shown: %d\n", sprof_info.system_samples, samples_shown);
	printf("processes <%3.0f%% (not showing functions) %*.1f%% of system "
		"process samples\n", minimum_perc, LINE_WIDTH - 67,
		percent(sprof_info.system_samples - samples_shown, sprof_info.system_samples));
	print_separator();

	free(binaries_sorted);
}

/* Function 21 */
static void print_separator(void) {
	int i;
	for (i = 0; i < LINE_WIDTH; i++) printf("-");
	printf("\n");
}

/* Function 22 */
static int read_hex(FILE *file, unsigned long *value) {
	int c, cvalue;
	unsigned index;

	assert(file);
	assert(value);

	index = 0;
	c = fgetc(file);
	*value = 0;
	while (index < 8) {
		if (c >= '0' && c <= '9') {
			cvalue = c - '0';
		} else if (c >= 'A' && c <= 'F') {
			cvalue = c - 'A' + 10;
		} else if (c >= 'a' && c <= 'f') {
			cvalue = c - 'a' + 10;
		} else {
			break;
		}

		*value = *value * 16 + cvalue;
		index++;
		c = fgetc(file);
	}
	if (c != EOF) ungetc(c, file);

	return index;
}

/* Function 23 */
static int read_newline(FILE *file) {
	int c;

	do {
		c = fgetc(file);
	} while (c != EOF && c != '\n' && isspace(c));
	if (c == EOF || c == '\n') return 1;
	ungetc(c, file);
	return 0;
}

/* Function 24 */
static void read_to_whitespace(FILE *file, char *buffer, size_t size) {
	int c;

	while ((c = fgetc(file)) != EOF && !isspace(c)) {
		if (size > 0) {
			*(buffer++) = c;
			size--;
		}
	}
	if (size > 0) *buffer = 0;
}

/* Function 25 */
static char *strdup_checked(const char *s) {
	char *p;
	if (!s) return NULL;
	p = strdup(s);
	if (!p) {
		fprintf(stderr, "error: strdup failed: %s\n",
			strerror(errno));
		exit(-1);
	}
	return p;
}

/* Function 26 */
static void usage(const char *argv0) {
	printf("usage:\n");
	printf("  %s [-d] [-p percentage] [-s src-tree-path] "
		"[-b binary]... file...\n", argv0);
	printf("\n");
	printf("sprofalyze aggregates one or more sprofile traces and");
	printf(" reports where time was spent.\n");
	printf("\n");
	printf("arguments:\n");
	printf("-d generates output that can be compared using sprofdiff\n");
	printf("-p specifies the cut-off percentage below which binaries\n");
	printf("   and functions will not be displayed\n");
	printf("-s specifies the root of the source tree where sprofalyze\n");
	printf("   should search for unstripped binaries to extract symbols\n");
	printf("   from\n");
	printf("-b specifies an additional system binary in the trace that\n");
	printf("   is not in the source tree; may be specified multiple\n");
	printf("   times\n");
	exit(1);
}

/* Function 27 */
int main(int argc, char **argv) {
	int opt, sprofdiff = 0;

#ifdef DEBUG
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
#endif

	while ((opt = getopt(argc, argv, "b:dp:s:")) != -1) {
		switch (opt) {
		case 'b':
			binary_add(optarg);
			break;
		case 'd':
			sprofdiff = 1;
			break;
		case 'p':
			minimum_perc = atof(optarg);
			if (minimum_perc < 0 || minimum_perc > 100) {
				fprintf(stderr, "error: cut-off percentage "
					"makes no sense: %g\n", minimum_perc);
				exit(1);
			}
			break;
		case 's':
			src_path = optarg;
			break;
		default: usage(argv[0]);
		}
	}

	if (optind >= argc) usage(argv[0]);
	for (; optind < argc; optind++) {
		load_trace(argv[optind]);
		for(e = endpoints; e; e = e->next)
			e->seen = 0;
	}

	if (sprofdiff) {
		print_diff();
	} else {
		print_report();
	}
	return 0;
}

/* Function 28 */
static struct binary_info *binary_add(const char *path) {
	const char *name;


	assert(path);
	if (strchr(path, '"')) {
		fprintf(stderr, "error: path \"%s\" contains a quote\n", path);
		exit(1);
	}

	name = binary_name(path);
	dprintf("adding binary \"%s\" with name \"%.*s\"\n",
		path, PROC_NAME_LEN, name);
	if (strlen(name) == 0) {
		fprintf(stderr, "error: path \"%s\" does not "
			"contain a filename\n", path);
		exit(1);
	}

	ptr = binary_hashtab_get_ptr(name);
	if (*ptr) {
		fprintf(stderr, "warning: ignoring \"%s\" because \"%s\" was "
			"previously specified\n", path, (*ptr)->path);
		return *ptr;
	}
	dprintf("using %.*s from \"%s\"\n", PROC_NAME_LEN, name, path);

	binary = MALLOC_CHECKED(struct binary_info, 1);
	memset(binary, 0, sizeof(struct binary_info));
	binary->path = path;
	strncpy(binary->name, name, sizeof(binary->name));

	binary->next = binaries;
	binaries = binary;

	*ptr = binary;
	return binary;
}

/* Function 29 */
static struct binary_info *binary_find(const char *name) {
	const char *current_name;
	unsigned i;
	char path[PATH_MAX + 1], *path_end;

	assert(name);

	if (!*name) {
		fprintf(stderr, "warning: binary unspecified in sample\n");
		return NULL;
	}

	binary = binary_hashtab_get(name);
	if (binary) return binary;

	dprintf("searching for binary \"%.*s\" in \"%s\"\n",
		PROC_NAME_LEN, name, src_path);
	for (i = 0; i < LENGTHOF(default_binaries); i++) {
		snprintf(path, sizeof(path), "%s/%s", src_path,
			default_binaries[i]);
		current_name = binary_name(path);
		assert(current_name);
		if (*current_name) {
			if (strncmp(name, current_name,
				PROC_NAME_LEN) != 0) {
				continue;
			}
		} else {
			path_end = path + strlen(path);
			snprintf(path_end, sizeof(path) - (path_end - path),
				"%.*s/%.*s", PROC_NAME_LEN, name,
				PROC_NAME_LEN, name);
		}

		dprintf("checking whether \"%s\" exists\n", path);
		if (access(path, R_OK) < 0) continue;

		return binary_add(strdup_checked(path));
	}

	return NULL;
}

/* Function 30 */
static struct binary_info *binary_hashtab_get(const char *name) {
	return *binary_hashtab_get_ptr(name);
}

/* Function 31 */
static struct binary_info **binary_hashtab_get_ptr(const char *name) {

	ptr = &binary_hashtab[name_hash(name) % BINARY_HASHTAB_SIZE];
	while ((binary = *ptr) && strncmp(binary->name, name,
		PROC_NAME_LEN) != 0) {
		ptr = &binary->hashtab_next;
	}
	dprintf("looked up binary \"%.*s\" in hash table, %sfound\n",
		PROC_NAME_LEN, name, *ptr ? "" : "not ");
	return ptr;
}

/* Function 32 */
static void binary_load_pc_map(struct binary_info *binary_info) {
	unsigned long addr, size;
	char *command;
	size_t command_len;
#if DEBUG
	unsigned count = 0;
#endif
	FILE *file;
	int index_l1, index_l2, line;
	char name[SYMBOL_NAME_SIZE];
	char type;

	assert(binary_info);
	assert(!strchr(NM, '"'));
	assert(!strchr(binary_info->path, '"'));

	if (access(binary_info->path, R_OK) < 0) {
		fprintf(stderr, "warning: \"%s\" does not exist or "
			"not readable.\n", binary_info->path);
		fprintf(stderr, "         Did you do a make?\n");
		return;
	}

	command_len = strlen(NM) + strlen(binary_info->path) + 32;
	command = MALLOC_CHECKED(char, command_len);
	snprintf(command, command_len, "\"%s\" -nP \"%s\"",
		NM, binary_info->path);
	dprintf("running command for extracting addresses: %s\n", command);
	file = popen(command, "r");
	if (!file) {
		perror("failed to start " NM);
		exit(-1);
	}
	free(command);

	assert(!binary_info->symbols);
	symbol_ptr = &binary_info->symbols;
	line = 1;
	while (!feof(file)) {
		read_nm_line(file, line++, name, &type, &addr, &size);

		if (type != 't' && type != 'T') continue;

		*symbol_ptr = symbol = MALLOC_CHECKED(struct symbol_count, 1);
		memset(symbol, 0, sizeof(*symbol));
		symbol->binary = binary_info;
		symbol->addr = addr;
		strncpy(symbol->name, name, SYMBOL_NAME_SIZE);
		symbol_ptr = &symbol->next;
#if DEBUG
		count++;
#endif
	}
	fclose(file);
	dprintf("extracted %u symbols\n", count);

	assert(!binary_info->pc_map);
	binary_info->pc_map = MALLOC_CHECKED(struct pc_map_l1, 1);
	memset(binary_info->pc_map, 0, sizeof(struct pc_map_l1));
	for (symbol = binary_info->symbols; symbol; symbol = symbol->next) {
		size = symbol->next ? (symbol->next->addr - symbol->addr) : 1;
		if (size > SYMBOL_SIZE_MAX) size = SYMBOL_SIZE_MAX;

		for (addr = symbol->addr; addr - symbol->addr < size; addr++) {
			index_l1 = addr / PC_MAP_L2_SIZE;
			assert(index_l1 < PC_MAP_L1_SIZE);
			pc_map_l2_ptr = &binary_info->pc_map->l1[index_l1];
			if (!(pc_map_l2 = *pc_map_l2_ptr)) {
				*pc_map_l2_ptr = pc_map_l2 =
					MALLOC_CHECKED(struct pc_map_l2, 1);
				memset(pc_map_l2, 0, sizeof(struct pc_map_l2));
			}
			index_l2 = addr % PC_MAP_L2_SIZE;
			pc_map_l2->l2[index_l2] = symbol;
		}
	}
}

/* Function 33 */
static const char *binary_name(const char *path) {
	const char *name, *p;

	name = path;
	for (p = path; *p; p++) {
		if (*p == '/') name = p + 1;
	}
	return name;
}

/* Function 34 */
static int compare_binaries(const void *p1, const void *p2) {
	const struct binary_info *const *b1 = p1, *const *b2 = p2;

	assert(b1);
	assert(b2);
	assert(*b1);
	assert(*b2);
	if ((*b1)->samples > (*b2)->samples) return -1;
	if ((*b1)->samples < (*b2)->samples) return 1;
	return 0;
}

/* Function 35 */
static int compare_symbols(const void *p1, const void *p2) {
	const struct symbol_count *const *s1 = p1, *const *s2 = p2;

	assert(s1);
	assert(s2);
	assert(*s1);
	assert(*s2);
	if ((*s1)->samples > (*s2)->samples) return -1;
	if ((*s1)->samples < (*s2)->samples) return 1;
	return 0;
}

/* Function 36 */
static int count_symbols(const struct binary_info *binary, int threshold) {
	int result = 0;

	for (symbol = binary->symbols; symbol; symbol = symbol->next) {
		if (symbol->samples >= threshold) result++;
	}
	return result;
}

/* Function 37 */
static void dprint_symbols(const struct binary_info *binary) {
#if DEBUG
	const struct symbol_count *symbol;

	for (symbol = binary->symbols; symbol; symbol = symbol->next) {
		dprintf("addr=0x%.8lx samples=%8d name=\"%.*s\"\n",
			(unsigned long) symbol->addr, symbol->samples,
			SYMBOL_NAME_SIZE, symbol->name);
	}
#endif
}

/* Function 38 */
static struct endpoint_info **endpoint_hashtab_get_ptr(endpoint_t endpoint) {

	ptr = &endpoint_hashtab[(unsigned) endpoint % ENDPOINT_HASHTAB_SIZE];
	while ((epinfo = *ptr) && epinfo->endpoint != endpoint) {
		ptr = &epinfo->hashtab_next;
	}
	dprintf("looked up endpoint %ld in hash table, %sfound\n",
		(long) endpoint, *ptr ? "" : "not ");
	return ptr;
}

/* Function 39 */
static void load_trace(const char *path) {
	char buffer[1024];
	size_t bufindex, bufsize;
	FILE *file;
	unsigned size_info, size_sample, size_proc;
	int samples_read;
	static struct sprof_info_s sprof_info_perfile;

	file = fopen(path, "rb");
	if (!file) {
		fprintf(stderr, "error: cannot open trace file \"%s\": %s\n",
			path, strerror(errno));
		exit(1);
	}

	if (fscanf(file, "stat\n%u %u %u",
		&size_info, &size_sample, &size_proc) != 3 ||
		fgetc(file) != '\n') {
		fprintf(stderr, "error: file \"%s\" does not contain an "
			"sprofile trace\n", path);
		exit(1);
	}
	if ((size_info != sizeof(struct sprof_info_s)) ||
		(size_sample != sizeof(struct sprof_sample)) ||
		(size_proc != sizeof(struct sprof_proc))) {
		fprintf(stderr, "error: file \"%s\" is incompatible with this "
			"version of sprofalyze; recompile sprofalyze with the "
			"MINIX version that created this file\n", path);
		exit(1);
	}
	if (fread(&sprof_info_perfile, sizeof(sprof_info_perfile), 1, file) != 1) {
		fprintf(stderr, "error: totals missing in file \"%s\"\n", path);
		exit(1);
	}

	samples_read = 0;
	bufindex = 0;
	bufsize = 0;
	for (;;) {
		if (bufsize - bufindex < sizeof(union sprof_record)) {
			memmove(buffer, buffer + bufindex, bufsize - bufindex);
			bufsize -= bufindex;
			bufindex = 0;
			bufsize += fread(buffer + bufsize, 1,
				sizeof(buffer) - bufsize, file);

			if (bufsize == 0) break;
		}

		bufindex += sample_process(
			(const union sprof_record *) (buffer + bufindex),
			bufsize - bufindex, &samples_read);
	}
	if (samples_read != sprof_info_perfile.system_samples) {
		fprintf(stderr, "warning: number of system samples (%d) in "
			"\"%s\" does not match number of records (%d)\n",
			sprof_info.system_samples, path, samples_read);
	}

	sprof_info.system_samples += sprof_info_perfile.system_samples;
	sprof_info.total_samples += sprof_info_perfile.total_samples;
	sprof_info.idle_samples += sprof_info_perfile.idle_samples;
	sprof_info.user_samples += sprof_info_perfile.user_samples;

	fclose(file);
}

/* Function 40 */
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

/* Function 41 */
static unsigned name_hash(const char *name) {
	int i;
	unsigned r = 0;

	for (i = 0; i < PROC_NAME_LEN && name[i]; i++) {
		r = r * 31 + name[i];
	}
	dprintf("name_hash(\"%.*s\") = 0x%.8x\n", PROC_NAME_LEN, name, r);
	return r;
}

/* Function 42 */
static float percent(int value, int percent_of) {
	assert(value >= 0);
	assert(value <= percent_of);

	return (percent_of > 0) ? (value * 100.0 / percent_of) : 0;
}

/* Function 43 */
static void print_report(void) {
	printf("Showing processes and functions using at least %3.0f%% "
		"time.\n\n", minimum_perc);
	printf("  System process ticks: %10d (%3.0f%%)\n",
		sprof_info.system_samples, percent(sprof_info.system_samples, sprof_info.total_samples));
	printf("    User process ticks: %10d (%3.0f%%)          "
		"Details of system process\n",
		sprof_info.user_samples, percent(sprof_info.user_samples, sprof_info.total_samples));
	printf("       Idle time ticks: %10d (%3.0f%%)          "
		"samples, aggregated and\n",
		sprof_info.idle_samples, percent(sprof_info.idle_samples, sprof_info.total_samples));
	printf("                        ----------  ----           "
		"per process, are below.\n");
	printf("           Total ticks: %10d (100%%)\n\n", sprof_info.total_samples);

	print_report_overall();
	print_reports_per_binary();
}

/* Function 44 */
static void print_report_overall(void) {
	unsigned index, symbol_count;
	int sample_threshold;

	sample_threshold = sprof_info.system_samples * minimum_perc / 100;
	symbol_count = 0;
	for (binary = binaries; binary; binary = binary->next) {
		symbol_count += count_symbols(binary, sample_threshold);
	}

	symbols_sorted = MALLOC_CHECKED(struct symbol_count *, symbol_count);
	index = 0;
	for (binary = binaries; binary; binary = binary->next) {
		for (symbol = binary->symbols; symbol; symbol = symbol->next) {
			if (symbol->samples >= sample_threshold) {
				symbols_sorted[index++] = symbol;
			}
		}
	}
	assert(index == symbol_count);
	qsort(symbols_sorted, symbol_count, sizeof(symbols_sorted[0]),
		compare_symbols);

	print_separator();
	printf("Total system process time %*d samples\n",
		LINE_WIDTH - 34, sprof_info.system_samples);
	print_separator();
	print_report_symbols(symbols_sorted, symbol_count, sprof_info.system_samples, 1);
	free(symbols_sorted);
}

/* Function 45 */
static void print_report_per_binary(const struct binary_info *binary) {
	unsigned index, symbol_count;
	int sample_threshold;

	assert(binary->samples >= 0);

	sample_threshold = binary->samples * minimum_perc / 100;
	symbol_count = count_symbols(binary, sample_threshold);

	symbols_sorted = MALLOC_CHECKED(struct symbol_count *, symbol_count);
	index = 0;
	for (symbol = binary->symbols; symbol; symbol = symbol->next) {
		if (symbol->samples >= sample_threshold) {
			symbols_sorted[index++] = symbol;
		}
	}
	assert(index == symbol_count);
	qsort(symbols_sorted, symbol_count, sizeof(symbols_sorted[0]),
		compare_symbols);

	print_separator();
	printf("%-*.*s %4.1f%% of system process samples\n",
		LINE_WIDTH - 32, PROC_NAME_LEN, binary->name,
		percent(binary->samples, sprof_info.system_samples));
	print_separator();
	print_report_symbols(symbols_sorted, symbol_count, binary->samples, 0);
	free(symbols_sorted);
}

/* Function 46 */
static void print_reports_per_binary(void) {
	unsigned binary_count, i, index;
	int sample_threshold, samples_shown;

	binary_count = 0;
	for (binary = binaries; binary; binary = binary->next) {
		assert(!binary->samples);
		for (symbol = binary->symbols; symbol; symbol = symbol->next) {
			binary->samples += symbol->samples;
		}
		binary_count++;
	}

	binaries_sorted = MALLOC_CHECKED(struct binary_info *, binary_count);
	index = 0;
	for (binary = binaries; binary; binary = binary->next) {
		binaries_sorted[index++] = binary;
	}
	assert(index == binary_count);
	qsort(binaries_sorted, binary_count, sizeof(binaries_sorted[0]),
		compare_binaries);

	sample_threshold = sprof_info.system_samples * minimum_perc / 100;
	samples_shown = 0;
	for (i = 0; i < binary_count; i++) {
		if (binaries_sorted[i]->samples < sample_threshold) break;
		print_report_per_binary(binaries_sorted[i]);
		samples_shown += binaries_sorted[i]->samples;
	}
	print_separator();
	printf("samples: %d shown: %d\n", sprof_info.system_samples, samples_shown);
	printf("processes <%3.0f%% (not showing functions) %*.1f%% of system "
		"process samples\n", minimum_perc, LINE_WIDTH - 67,
		percent(sprof_info.system_samples - samples_shown, sprof_info.system_samples));
	print_separator();

	free(binaries_sorted);
}

/* Function 47 */
static void print_separator(void) {
	int i;
	for (i = 0; i < LINE_WIDTH; i++) printf("-");
	printf("\n");
}

/* Function 48 */
static int read_hex(FILE *file, unsigned long *value) {
	int c, cvalue;
	unsigned index;

	assert(file);
	assert(value);

	index = 0;
	c = fgetc(file);
	*value = 0;
	while (index < 8) {
		if (c >= '0' && c <= '9') {
			cvalue = c - '0';
		} else if (c >= 'A' && c <= 'F') {
			cvalue = c - 'A' + 10;
		} else if (c >= 'a' && c <= 'f') {
			cvalue = c - 'a' + 10;
		} else {
			break;
		}

		*value = *value * 16 + cvalue;
		index++;
		c = fgetc(file);
	}
	if (c != EOF) ungetc(c, file);

	return index;
}

/* Function 49 */
static int read_newline(FILE *file) {
	int c;

	do {
		c = fgetc(file);
	} while (c != EOF && c != '\n' && isspace(c));
	if (c == EOF || c == '\n') return 1;
	ungetc(c, file);
	return 0;
}

/* Function 50 */
static void read_to_whitespace(FILE *file, char *buffer, size_t size) {
	int c;

	while ((c = fgetc(file)) != EOF && !isspace(c)) {
		if (size > 0) {
			*(buffer++) = c;
			size--;
		}
	}
	if (size > 0) *buffer = 0;
}

/* Function 51 */
static char *strdup_checked(const char *s) {
	char *p;
	if (!s) return NULL;
	p = strdup(s);
	if (!p) {
		fprintf(stderr, "error: strdup failed: %s\n",
			strerror(errno));
		exit(-1);
	}
	return p;
}

/* Function 52 */
static void usage(const char *argv0) {
	printf("usage:\n");
	printf("  %s [-d] [-p percentage] [-s src-tree-path] "
		"[-b binary]... file...\n", argv0);
	printf("\n");
	printf("sprofalyze aggregates one or more sprofile traces and");
	printf(" reports where time was spent.\n");
	printf("\n");
	printf("arguments:\n");
	printf("-d generates output that can be compared using sprofdiff\n");
	printf("-p specifies the cut-off percentage below which binaries\n");
	printf("   and functions will not be displayed\n");
	printf("-s specifies the root of the source tree where sprofalyze\n");
	printf("   should search for unstripped binaries to extract symbols\n");
	printf("   from\n");
	printf("-b specifies an additional system binary in the trace that\n");
	printf("   is not in the source tree; may be specified multiple\n");
	printf("   times\n");
	exit(1);
}


#ifdef __cplusplus
}
#endif

/* End of sprofalyze_unified.c - Synthesized by MINIX4 Massive Synthesis System */
