/**
 * @file config_minix4.h
 * @brief MINIX4 Configuration Definitions
 * @version 4.0.0
 * @date 2025-06-12
 */

#pragma once

/*===========================================================================*
 *                        SYSTEM CONFIGURATION                              *
 *===========================================================================*/

/** @brief MINIX4 version information */
#define MINIX4_VERSION_MAJOR    4
#define MINIX4_VERSION_MINOR    0  
#define MINIX4_VERSION_PATCH    0
#define MINIX4_VERSION_STRING   "4.0.0-unified"

/** @brief Build configuration */
#define CONFIG_MINIX4           1
#define CONFIG_MICROKERNEL      1
#define CONFIG_CAPABILITY_BASED 1

/** @brief Memory configuration */
#define CONFIG_MAX_MEMORY       (1ULL << 40)   /* 1TB */
#define CONFIG_MIN_MEMORY       (16 * 1024 * 1024) /* 16MB */
#define CONFIG_PAGE_SIZE        4096

/** @brief Process configuration */
#define CONFIG_MAX_PROCS        256
#define CONFIG_MAX_TASKS        32
#define CONFIG_MAX_CPUS         8

/** @brief Debugging configuration */
#ifdef DEBUG
#define CONFIG_DEBUG            1
#define CONFIG_ASSERTIONS       1
#define CONFIG_VERBOSE_BOOT     1
#else
#define CONFIG_DEBUG            0
#define CONFIG_ASSERTIONS       0
#define CONFIG_VERBOSE_BOOT     0
#endif

/** @brief Feature configuration */
#define CONFIG_SMP              1   /* Symmetric multiprocessing */
#define CONFIG_PREEMPTIVE       1   /* Preemptive scheduling */
#define CONFIG_CAPABILITY       1   /* Capability-based security */
#define CONFIG_VIRTUAL_MEMORY   1   /* Virtual memory support */

/** @brief Device configuration */
#define CONFIG_PCI              1   /* PCI bus support */
#define CONFIG_USB              1   /* USB support */
#define CONFIG_NETWORK          1   /* Network support */
