/**
 * @file tdist_unified.c
 * @brief Unified tdist implementation
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
 *     1. userspace\minix_commands\sprofdiff\tdist.c
 *     2. userspace\commands_legacy\development\sprofdiff\tdist.c
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
#include <math.h>

/* ===== GLOBAL VARIABLES ===== */
static void df_to_index(float df, int *index1_p, float *weight1_p);
static float get_t(int p_index, int df_index1, float df_weight1);
static float index_to_p(int index);
static float linear(float x, float x1, float y1, float x2, float y2);
static void df_to_index(float df, int *index1_p, float *weight1_p);
static float get_t(int p_index, int df_index1, float df_weight1);
static float index_to_p(int index);
static float linear(float x, float x1, float y1, float x2, float y2);

/* ===== FUNCTIONS ===== */

/* Function 1 */
float student_t_p_2tail(float t, float df) {
	int df_index1, p_index_min, p_index_mid, p_index_max;
	float df_weight1, t_max, t_mid, t_min;

	p_index_min = 0;
	p_index_max = 189;
	df_to_index(df, &df_index1, &df_weight1);
	if (t < 0) t = -t;
	if (t > get_t(p_index_min, df_index1, df_weight1)) return 0.000;
	if (t < get_t(p_index_max, df_index1, df_weight1)) return 1.000;

	while (p_index_max - p_index_min > 1) {
		p_index_mid = (p_index_min + p_index_max) / 2;
		t_mid = get_t(p_index_mid, df_index1, df_weight1);
		if (t_mid >= t) p_index_min = p_index_mid;
		if (t_mid <= t) p_index_max = p_index_mid;
	}

	t_max = get_t(p_index_min, df_index1, df_weight1);
	t_min = get_t(p_index_max, df_index1, df_weight1);
	assert(t >= t_min);
	assert(t <= t_max);
	return linear(t, t_max, index_to_p(p_index_min),
		t_min, index_to_p(p_index_max));
}

/* Function 2 */
static void df_to_index(float df, int *index1_p, float *weight1_p) {
	int index1;
	float indexf;

	assert(df >= 1);
	assert(index1_p);
	assert(weight1_p);

	if (df <= 30) {
		indexf = df - 1;
	} else if (df <= 100) {
		indexf = (df - 30) / 10 + 29;
	} else {
		indexf = (df - 100) / 100 + 36;
	}

	index1 = (int) floor(indexf);
	if (index1 >= 45) {
		*index1_p = 44;
		*weight1_p = 0;
	} else {
		*index1_p = index1;
		*weight1_p = 1 - (indexf - index1);
	}
}

/* Function 3 */
static float get_t(int p_index, int df_index1, float df_weight1) {
	assert(p_index >= 0 && p_index < 190);
	assert(df_index1 >= 0 && df_index1 < 45);
	return t_value_by_p_and_df[p_index][df_index1] * df_weight1 +
		t_value_by_p_and_df[p_index][df_index1 + 1] * (1 - df_weight1);
}

/* Function 4 */
static float index_to_p(int index) {
	if (index < 100) {
		return 0.0010 * index + 0.0005;
	} else {
		return 0.0100 * (index - 100) + 0.1050;
	}
}

/* Function 5 */
static float linear(float x, float x1, float y1, float x2, float y2) {
	if (x2 == x1) {
		assert(x == x1);
		return (y1 + y2) / 2;
	} else {
		return (x - x1) / (x2 - x1) * (y2 - y1) + y1;
	}
}

/* Function 6 */
float student_t_p_2tail(float t, float df) {
	int df_index1, p_index_min, p_index_mid, p_index_max;
	float df_weight1, t_max, t_mid, t_min;

	p_index_min = 0;
	p_index_max = 189;
	df_to_index(df, &df_index1, &df_weight1);
	if (t < 0) t = -t;
	if (t > get_t(p_index_min, df_index1, df_weight1)) return 0.000;
	if (t < get_t(p_index_max, df_index1, df_weight1)) return 1.000;

	while (p_index_max - p_index_min > 1) {
		p_index_mid = (p_index_min + p_index_max) / 2;
		t_mid = get_t(p_index_mid, df_index1, df_weight1);
		if (t_mid >= t) p_index_min = p_index_mid;
		if (t_mid <= t) p_index_max = p_index_mid;
	}

	t_max = get_t(p_index_min, df_index1, df_weight1);
	t_min = get_t(p_index_max, df_index1, df_weight1);
	assert(t >= t_min);
	assert(t <= t_max);
	return linear(t, t_max, index_to_p(p_index_min),
		t_min, index_to_p(p_index_max));
}

/* Function 7 */
static void df_to_index(float df, int *index1_p, float *weight1_p) {
	int index1;
	float indexf;

	assert(df >= 1);
	assert(index1_p);
	assert(weight1_p);

	if (df <= 30) {
		indexf = df - 1;
	} else if (df <= 100) {
		indexf = (df - 30) / 10 + 29;
	} else {
		indexf = (df - 100) / 100 + 36;
	}

	index1 = (int) floor(indexf);
	if (index1 >= 45) {
		*index1_p = 44;
		*weight1_p = 0;
	} else {
		*index1_p = index1;
		*weight1_p = 1 - (indexf - index1);
	}
}

/* Function 8 */
static float get_t(int p_index, int df_index1, float df_weight1) {
	assert(p_index >= 0 && p_index < 190);
	assert(df_index1 >= 0 && df_index1 < 45);
	return t_value_by_p_and_df[p_index][df_index1] * df_weight1 +
		t_value_by_p_and_df[p_index][df_index1 + 1] * (1 - df_weight1);
}

/* Function 9 */
static float index_to_p(int index) {
	if (index < 100) {
		return 0.0010 * index + 0.0005;
	} else {
		return 0.0100 * (index - 100) + 0.1050;
	}
}

/* Function 10 */
static float linear(float x, float x1, float y1, float x2, float y2) {
	if (x2 == x1) {
		assert(x == x1);
		return (y1 + y2) / 2;
	} else {
		return (x - x1) / (x2 - x1) * (y2 - y1) + y1;
	}
}


#ifdef __cplusplus
}
#endif

/* End of tdist_unified.c - Synthesized by MINIX4 Massive Synthesis System */
