/**
 * @file kassert.h
 * @brief Kernel Assertion Macros
 * @details Safe assertion mechanisms for kernel debugging
 * @version 4.0.0
 * @date 2025-06-12
 */

#pragma once

#include <stdbool.h>

/* Kernel assertion function */
void kassert_fail(const char *expr, const char *file, int line, const char *func)
    __attribute__((noreturn));

/* Debug vs Release behavior */
#ifdef DEBUG
    #define KASSERT(expr) \
        do { \
            if (!(expr)) { \
                kassert_fail(#expr, __FILE__, __LINE__, __func__); \
            } \
        } while (0)
    
    #define KASSERT_MSG(expr, msg) \
        do { \
            if (!(expr)) { \
                kassert_fail(msg, __FILE__, __LINE__, __func__); \
            } \
        } while (0)
#else
    #define KASSERT(expr) ((void)0)
    #define KASSERT_MSG(expr, msg) ((void)0)
#endif

/* Always-on assertions for critical checks */
#define KASSERT_ALWAYS(expr) \
    do { \
        if (!(expr)) { \
            kassert_fail(#expr, __FILE__, __LINE__, __func__); \
        } \
    } while (0)

/* Compile-time assertions */
#define KASSERT_STATIC(expr) _Static_assert(expr, #expr)

/* Utility assertions */
#define KASSERT_NOT_NULL(ptr) KASSERT((ptr) != NULL)
#define KASSERT_RANGE(val, min, max) KASSERT((val) >= (min) && (val) <= (max))
#define KASSERT_ALIGNED(ptr, align) KASSERT(((uintptr_t)(ptr) % (align)) == 0)

/* Panic-like assertions */
#define KPANIC(msg) kassert_fail(msg, __FILE__, __LINE__, __func__)
#define KPANIC_IF(expr, msg) \
    do { \
        if (expr) { \
            kassert_fail(msg, __FILE__, __LINE__, __func__); \
        } \
    } while (0)
