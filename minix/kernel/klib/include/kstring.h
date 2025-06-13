/**
 * @file kstring.h
 * @brief Kernel String Functions
 * @details Safe string manipulation functions for kernel use
 * @version 4.0.0
 * @date 2025-06-12
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

/* String length and comparison */
size_t kstrlen(const char *s);
int kstrcmp(const char *s1, const char *s2);
int kstrncmp(const char *s1, const char *s2, size_t n);
int kstrcasecmp(const char *s1, const char *s2);

/* String copying */
char *kstrcpy(char *dest, const char *src);
char *kstrncpy(char *dest, const char *src, size_t n);
size_t kstrlcpy(char *dest, const char *src, size_t size);

/* String concatenation */
char *kstrcat(char *dest, const char *src);
char *kstrncat(char *dest, const char *src, size_t n);
size_t kstrlcat(char *dest, const char *src, size_t size);

/* String searching */
char *kstrchr(const char *s, int c);
char *kstrrchr(const char *s, int c);
char *kstrstr(const char *haystack, const char *needle);
char *kstrtok(char *str, const char *delim);

/* String conversion */
long kstrtol(const char *str, char **endptr, int base);
unsigned long kstrtoul(const char *str, char **endptr, int base);
int katoi(const char *str);

/* Memory comparison and search */
int kmemcmp(const void *s1, const void *s2, size_t n);
void *kmemchr(const void *s, int c, size_t n);
void *kmemrchr(const void *s, int c, size_t n);

/* Safe string utilities */
bool kstr_is_valid(const char *s, size_t max_len);
size_t kstr_sanitize(char *dest, const char *src, size_t size);
bool kstr_ends_with(const char *str, const char *suffix);
bool kstr_starts_with(const char *str, const char *prefix);
