/**
 * @file kprintf.h
 * @brief Kernel Printf Implementation
 * @details Safe printf functions for kernel use
 * @version 4.0.0
 * @date 2025-06-12
 */

#pragma once

#include <stdarg.h>
#include <stddef.h>

/* Kernel printf functions */
int kprintf(const char *format, ...);
int kvprintf(const char *format, va_list args);
int ksnprintf(char *buffer, size_t size, const char *format, ...);
int kvsnprintf(char *buffer, size_t size, const char *format, va_list args);

/* Kernel console output */
void kputchar(char c);
void kputs(const char *s);

/* Debug printing */
void kdebug_printf(const char *format, ...);
void kpanic_printf(const char *format, ...) __attribute__((noreturn));

/* Print levels */
#define KPRINT_DEBUG    0
#define KPRINT_INFO     1
#define KPRINT_WARN     2
#define KPRINT_ERROR    3
#define KPRINT_PANIC    4

void kprint_level(int level, const char *format, ...);

/* Convenience macros */
#define kdebug(fmt, ...)    kprint_level(KPRINT_DEBUG, fmt, ##__VA_ARGS__)
#define kinfo(fmt, ...)     kprint_level(KPRINT_INFO, fmt, ##__VA_ARGS__)
#define kwarn(fmt, ...)     kprint_level(KPRINT_WARN, fmt, ##__VA_ARGS__)
#define kerror(fmt, ...)    kprint_level(KPRINT_ERROR, fmt, ##__VA_ARGS__)
