/**
 * @file panic.h
 * @brief MINIX4 Panic and Error Handling
 * @version 4.0.0
 * @date 2025-06-12
 */

#pragma once

#include <stdnoreturn.h>

/**
 * @brief Panic codes
 */
enum panic_code {
    PANIC_GENERAL = 0,          /**< General panic */
    PANIC_MEMORY,               /**< Memory allocation failure */
    PANIC_IPC,                  /**< IPC system failure */
    PANIC_CAPABILITY,           /**< Capability system failure */
    PANIC_SCHEDULER,            /**< Scheduler failure */
    PANIC_DEVICE,               /**< Device driver failure */
    PANIC_FILESYSTEM,           /**< File system failure */
    PANIC_NETWORK,              /**< Network failure */
    PANIC_SECURITY,             /**< Security violation */
    PANIC_HARDWARE,             /**< Hardware failure */
    PANIC_CORRUPTION,           /**< Data corruption detected */
    PANIC_ASSERTION,            /**< Assertion failure */
    PANIC_MAX
};

/**
 * @brief Panic function
 * @param message Panic message
 */
noreturn void panic(const char *message);

/**
 * @brief Panic with code
 * @param code Panic code
 * @param message Panic message
 */
noreturn void panic_code(enum panic_code code, const char *message);

/**
 * @brief Panic with location information
 * @param message Panic message
 * @param file Source file name
 * @param line Line number
 */
noreturn void panic_at(const char *message, const char *file, int line);

/**
 * @brief Assertion macro
 */
#ifdef DEBUG
#define ASSERT(expr) do { \
    if (!(expr)) { \
        panic_at("Assertion failed: " #expr, __FILE__, __LINE__); \
    } \
} while (0)
#else
#define ASSERT(expr) ((void)0)
#endif

/**
 * @brief Panic macro
 */
#define PANIC(msg) panic_at((msg), __FILE__, __LINE__)

/**
 * @brief Not reached macro
 */
#define NOT_REACHED() PANIC("Code should not be reached")

// Legacy DDE kit compatibility
void ddekit_panic(char *fmt, ...);
void ddekit_debug(char *fmt, ...);
