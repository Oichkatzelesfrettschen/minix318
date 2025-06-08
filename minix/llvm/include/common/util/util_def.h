/**
 * @file util_def.h
 * @brief Utility definitions and macros for cross-platform compatibility
 *
 * This header file provides standardized macro definitions for common system
 * functions and constants. It enables platform-independent code by abstracting
 * system-specific implementations behind consistent macro names.
 *
 * The file includes definitions for:
 * - GNU feature test macros for extended functionality
 * - System constants (socket paths, memory page sizes, etc.)
 * - Memory management function aliases
 * - Threading function aliases
 * - Standard I/O function aliases
 *
 * All macros are conditionally defined to allow for custom overrides
 * when specific implementations are required for different platforms.
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef __USE_GNU
#define __USE_GNU
#endif

#ifndef UNIX_PATH_MAX
// Maximum length for UNIX domain socket paths, as defined by system limits.
#define UNIX_PATH_MAX 108
#endif

#ifndef PAGE_SIZE
// Default memory page size for most systems
#define PAGE_SIZE 4096
#endif

#ifndef MIN_MMAP_ADDR
// Define the minimum memory-mapped address as 100 pages above the base address.
// This ensures a safe offset for memory mapping operations.
#define MIN_MMAP_ADDR ((void *)(PAGE_SIZE * 100))
#ifndef UTIL_STANDARD_PRINTF
#define UTIL_STANDARD_PRINTF printf
#endif
#define _UTIL_PRINTF printf
#endif

#ifndef UTIL_THREAD_CREATE
#define UTIL_THREAD_CREATE pthread_create
#endif

#ifndef _UTIL_PTHREAD_JOIN
#define _UTIL_PTHREAD_JOIN pthread_join
#endif

#ifndef _UTIL_PTHREAD_CANCEL
#define _UTIL_PTHREAD_CANCEL pthread_cancel
#endif

#ifndef _UTIL_PTHREAD_SIGMASK
#define _UTIL_PTHREAD_SIGMASK pthread_sigmask
#endif

#ifndef UTIL_MEMORY_ALLOC
#define UTIL_MEMORY_ALLOC malloc
#endif

#ifndef UTIL_MEMORY_CALLOC
#define UTIL_MEMORY_CALLOC calloc
#endif

#ifndef UTIL_MEMORY_FREE
#define UTIL_MEMORY_FREE free
#endif
