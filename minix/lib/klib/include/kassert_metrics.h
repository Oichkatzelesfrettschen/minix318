/**
 * @file kassert_metrics.h
 * @brief Declarations for Kernel Assertion Metrics Collection.
 *
 * Provides function prototypes for recording and reporting kernel assertion
 * metrics. These are part of an assertion effectiveness framework.
 * @see kassert_metrics.c
 */

#ifndef _KLIB_KASSERT_METRICS_H
#define _KLIB_KASSERT_METRICS_H

/*
 * No specific klib headers are strictly needed for these function declarations
 * as they use basic types (const char*, int). However, if klib evolves to use
 * its own string types or complex structures in these signatures, relevant
 * klib includes might be added here. For now, it's self-contained.
 */

/**
 * @brief Records an assertion hit (STUB).
 *
 * Called to indicate an assertion at a specific code location was encountered.
 * An enhanced KASSERT macro would typically call this function.
 *
 * @param file The source file of the assertion. Must not be NULL.
 * @param line The line number of the assertion.
 * @param condition_string The literal condition string of the assertion. Must not be NULL.
 */
void kassert_metrics_record_hit(const char* file, int line, const char* condition_string);

/**
 * @brief Reports collected assertion metrics (STUB).
 *
 * Outputs a summary of assertion metrics, potentially including hit counts
 * and data for calculating assertion effectiveness E(a).
 * The exact format and destination of the report (e.g., kernel console)
 * would be defined by its implementation.
 */
void kassert_metrics_report(void);

#endif /* _KLIB_KASSERT_METRICS_H */
