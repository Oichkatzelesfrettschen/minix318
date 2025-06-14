/**
 * @file kernel_types.h
 * @brief MINIX4 Kernel Type Definitions
 * @details Unified type definitions for the MINIX4 kernel
 * @version 4.0.0
 * @date 2025-06-12
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Kernel-specific type definitions */
typedef uint64_t phys_bytes;            /* Physical address type */
typedef uint64_t vir_bytes;             /* Virtual address type */
typedef int32_t  k_errno_t;             /* Kernel error type */
typedef size_t   k_size_t;              /* Kernel size type */
typedef int      endpoint_t;            /* Process endpoint type */
typedef int      cp_grant_id_t;         /* Grant ID type */

/* Process and message types */
typedef int      proc_nr_t;             /* Process number type */
typedef uint32_t message_type_t;        /* Message type */
typedef uint32_t ipc_status_t;          /* IPC status type */

/* Time and clock types */
typedef uint64_t clock_t;               /* Clock tick type */
typedef uint64_t time_t;                /* Time in seconds */
typedef uint32_t timer_t;               /* Timer identifier */

/* Interrupt and IRQ types */
typedef int      irq_id_t;              /* IRQ identifier */
typedef void (*irq_handler_t)(int irq); /* IRQ handler function */

/* Memory and I/O types */
typedef uint32_t ioport_t;              /* I/O port address */
typedef uint8_t  *iobase_t;             /* I/O base address */
typedef uint32_t reg_t;                 /* Register type */

/* Device and driver types */
typedef int      major_t;               /* Major device number */
typedef int      minor_t;               /* Minor device number */
typedef uint32_t dev_t;                 /* Device number */

/* File system types */
typedef uint32_t ino_t;                 /* Inode number */
typedef uint32_t block_t;               /* Block number */
typedef int32_t  off_t;                 /* File offset */

/* Network types */
typedef uint32_t ip_addr_t;             /* IP address */
typedef uint16_t port_t;                /* Port number */

/* Capability and security types */
typedef uint64_t capability_t;          /* Capability bits */
typedef uint32_t security_level_t;      /* Security level */

/* Architecture-specific types */
#if defined(__x86_64__)
typedef uint64_t arch_reg_t;            /* Architecture register */
typedef uint64_t arch_addr_t;           /* Architecture address */
#elif defined(__i386__)
typedef uint32_t arch_reg_t;
typedef uint32_t arch_addr_t;
#elif defined(__arm__) || defined(__aarch64__)
typedef uint32_t arch_reg_t;
typedef uint32_t arch_addr_t;
#else
typedef uint32_t arch_reg_t;
typedef uint32_t arch_addr_t;
#endif

/* Common constants */
#define K_OK                0           /* Success */
#define K_ERROR            -1           /* Generic error */
#define K_INVALID_PARAM    -2           /* Invalid parameter */
#define K_NO_MEMORY        -3           /* Out of memory */
#define K_TIMEOUT          -4           /* Operation timeout */
#define K_NOT_FOUND        -5           /* Resource not found */
#define K_PERMISSION       -6           /* Permission denied */

/* Process limits */
#define MAX_PROC_NAME      16           /* Maximum process name length */
#define MAX_ENDPOINTS      256          /* Maximum endpoints */

/* Message limits */
#define MAX_MESSAGE_SIZE   1024         /* Maximum message size */
#define MAX_GRANT_SIZE     (64*1024)    /* Maximum grant size */

/* Memory limits */
#define MAX_PHYS_ADDR      UINT64_MAX   /* Maximum physical address */
#define MAX_VIRT_ADDR      UINTPTR_MAX  /* Maximum virtual address */

/* Utility macros for type safety */
#define PHYS_ADDR(addr)    ((phys_bytes)(addr))
#define VIRT_ADDR(addr)    ((vir_bytes)(addr))
#define PROC_NR(nr)        ((proc_nr_t)(nr))
#define ENDPOINT(ep)       ((endpoint_t)(ep))

/* NULL-like values for kernel types */
#define NULL_PHYS          ((phys_bytes)0)
#define NULL_VIRT          ((vir_bytes)0)
#define INVALID_PROC       ((proc_nr_t)-1)
#define INVALID_ENDPOINT   ((endpoint_t)-1)
#define INVALID_GRANT      ((cp_grant_id_t)-1)
