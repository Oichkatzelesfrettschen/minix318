/**
 * @file types.h
 * @brief x86_64 Architecture-Specific Types
 * @version 4.0.0
 * @date 2025-06-12
 */

#pragma once

#include <stdint.h>

/*===========================================================================*
 *                          x86_64 SPECIFIC TYPES                           *
 *===========================================================================*/

/** @brief x86_64 register type */
typedef uint64_t reg_t;

/** @brief x86_64 segment selector */
typedef uint16_t seg_t;

/** @brief Page table entry type */
typedef uint64_t pte_t;

/** @brief Page directory entry type */
typedef uint64_t pde_t;

/** @brief CR3 register type */
typedef uint64_t cr3_t;

/*===========================================================================*
 *                           x86_64 CONSTANTS                               *
 *===========================================================================*/

/** @brief Virtual address space layout */
#define USER_SPACE_START    0x0000000000400000UL
#define USER_SPACE_END      0x00007FFFFFFFFFFFUL
#define KERNEL_SPACE_START  0xFFFF800000000000UL
#define KERNEL_SPACE_END    0xFFFFFFFFFFFFFFFFUL

/** @brief Page table constants */
#define PAGE_SHIFT          12
#define PAGE_SIZE           (1UL << PAGE_SHIFT)
#define PAGE_MASK           (PAGE_SIZE - 1)

/** @brief x86_64 specific features */
#define ARCH_HAS_MMU        1
#define ARCH_HAS_FPU        1
#define ARCH_HAS_SSE        1
#define ARCH_WORD_SIZE      64
