/**
 * @file types.h
 * @brief i386 Architecture-Specific Types
 * @version 4.0.0
 * @date 2025-06-12
 */

#pragma once

#include <stdint.h>

/*===========================================================================*
 *                           i386 SPECIFIC TYPES                            *
 *===========================================================================*/

/** @brief i386 register type */
typedef uint32_t reg_t;

/** @brief i386 segment selector */
typedef uint16_t seg_t;

/** @brief Page table entry type */
typedef uint32_t pte_t;

/** @brief Page directory entry type */
typedef uint32_t pde_t;

/** @brief CR3 register type */
typedef uint32_t cr3_t;

/*===========================================================================*
 *                            i386 CONSTANTS                                *
 *===========================================================================*/

/** @brief Virtual address space layout */
#define USER_SPACE_START    0x00400000UL
#define USER_SPACE_END      0xBFFFFFFFUL
#define KERNEL_SPACE_START  0xC0000000UL
#define KERNEL_SPACE_END    0xFFFFFFFFUL

/** @brief Page table constants */
#define PAGE_SHIFT          12
#define PAGE_SIZE           (1UL << PAGE_SHIFT)
#define PAGE_MASK           (PAGE_SIZE - 1)

/** @brief i386 specific features */
#define ARCH_HAS_MMU        1
#define ARCH_HAS_FPU        1
#define ARCH_HAS_SSE        0
#define ARCH_WORD_SIZE      32
