/**
 * @file kernel.h  
 * @brief MINIX4 Unified Kernel Header
 * @details Central include file for all kernel code, providing standardized
 *          types, constants, and interfaces for the MINIX4 kernel.
 * @version 4.0.0
 * @date 2025-06-12
 */

#pragma once

#define _POSIX_C_SOURCE 202311L

/* Standard C23 headers */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdnoreturn.h>
#include <stdalign.h>
#include <stdatomic.h>

/* MINIX4 core definitions */
#include "config_minix4.h"

/*===========================================================================*
 *                              BASIC TYPES                                 *
 *===========================================================================*/

/** @brief Process endpoint identifier */
typedef int32_t endpoint_t;

/** @brief Process ID type */
typedef int32_t pid_t;

/** @brief User ID type */
typedef uint32_t uid_t;

/** @brief Group ID type */
typedef uint32_t gid_t;

/** @brief Size type for kernel operations */
typedef size_t k_size_t;

/** @brief Physical address type */
typedef uint64_t phys_addr_t;

/** @brief Virtual address type */
typedef uint64_t virt_addr_t;

/** @brief Physical memory size type */
typedef uint64_t phys_bytes;

/*===========================================================================*
 *                            KERNEL CONSTANTS                              *
 *===========================================================================*/

/** @brief Maximum number of processes */
#define NR_PROCS        256

/** @brief Maximum number of tasks */
#define NR_TASKS        32

/** @brief Total process table size */
#define NR_PROC_TOTAL   (NR_PROCS + NR_TASKS)

/** @brief Number of scheduling priority queues */
#define NR_SCHED_QUEUES 16

/** @brief Maximum number of CPUs */
#ifndef CONFIG_MAX_CPUS
#define CONFIG_MAX_CPUS 8
#endif

/** @brief Process magic number for validation */
#define PROC_MAGIC      0x50524F43UL  /* "PROC" */

/** @brief Page size (4KB) */
#define PAGE_SIZE       4096

/** @brief Number of bits in a chunk for bitmaps */
#define CHUNK_SIZE      (sizeof(uint64_t) * 8)

/** @brief Number of bitmap chunks */
#define BITMAP_CHUNKS   ((NR_PROC_TOTAL + CHUNK_SIZE - 1) / CHUNK_SIZE)

/*===========================================================================*
 *                          PROCESS DEFINITIONS                             *
 *===========================================================================*/

/** @brief Special endpoint values */
#define ANY             (-1)    /**< Accept message from any source */
#define NONE            (-2)    /**< No process specified */
#define SELF            (-3)    /**< Current process */
#define IDLE            (-4)    /**< Idle process */

/** @brief Process state flags */
#define RTS_SLOT_FREE   0x01    /**< Process slot is free */
#define RTS_PROC_STOP   0x02    /**< Process is stopped */
#define RTS_SENDING     0x04    /**< Process is sending */
#define RTS_RECEIVING   0x08    /**< Process is receiving */
#define RTS_SIGNALED    0x10    /**< Process has pending signals */
#define RTS_SIG_PENDING 0x20    /**< Signal delivery pending */
#define RTS_P_STOP      0x40    /**< Process stopped by parent */
#define RTS_NO_PRIV     0x80    /**< No privilege structure */

/*===========================================================================*
 *                            IPC DEFINITIONS                               *
 *===========================================================================*/

/** @brief IPC operation flags */
#define IPC_NOWAIT      0x01    /**< Non-blocking operation */
#define IPC_INTR        0x02    /**< Interruptible operation */

/** @brief System call numbers */
#define SEND            1       /**< Send a message */
#define RECEIVE         2       /**< Receive a message */
#define SENDREC         3       /**< Send and receive */
#define NOTIFY          4       /**< Send notification */
#define SENDNB          5       /**< Non-blocking send */
#define SENDA           6       /**< Asynchronous send array */

/** @brief Special return values */
#define SUSPEND         (-998)  /**< Process suspended */
#define OK              0       /**< Success */

/*===========================================================================*
 *                         MESSAGE STRUCTURE                                *
 *===========================================================================*/

/** @brief IPC message structure */
typedef struct message {
    endpoint_t m_source;        /**< Message source endpoint */
    int m_type;                 /**< Message type */
    
    union {
        struct {
            int i1, i2, i3;     /**< Integer parameters */
            long l1, l2;        /**< Long parameters */
            char *cp1, *cp2;    /**< Character pointer parameters */
        } m_m1;
        
        struct {
            int i1, i2, i3, i4; /**< Integer parameters */
            long l1, l2, l3;    /**< Long parameters */
        } m_m2;
        
        struct {
            int i1;             /**< Integer parameter */
            char ca1[52];       /**< Character array */
        } m_m3;
        
        struct {
            long l1, l2, l3, l4, l5; /**< Long parameters */
        } m_m4;
        
        struct {
            char ca1[56];       /**< Large character array */
        } m_m5;
        
        struct {
            uint8_t data[56];   /**< Raw data */
        } m_data;
    };
} message;

/*===========================================================================*
 *                         FORWARD DECLARATIONS                             *
 *===========================================================================*/

/* Process structure forward declaration */
struct proc;
struct priv;

/* Memory management forward declarations */
struct page;
struct memory_zone;

/* IPC forward declarations */
typedef struct message message;

/*===========================================================================*
 *                          FUNCTION PROTOTYPES                             *
 *===========================================================================*/

/* Kernel printf function */
int kprintf(const char *format, ...);

/* Memory management functions */
void *kmalloc(size_t size);
void kfree(void *ptr);
void *palloc(void);
void pfree(void *page);

/* Process management functions */
struct proc *proc_lookup(endpoint_t endpoint);
int proc_nr(struct proc *rp);
endpoint_t proc_endpoint(struct proc *rp);

/* IPC functions */
int sys_call(struct proc *caller, int call_nr, endpoint_t src_dest, message *msg_ptr);

/* Bitmap operations */
void set_sys_bit(uint64_t *map, int bit_nr);
void unset_sys_bit(uint64_t *map, int bit_nr);
int get_sys_bit(uint64_t *map, int bit_nr);

/* Utility macros */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/* Endpoint manipulation macros */
#define _ENDPOINT_P(ep) ((ep) >= 0)
#define _ENDPOINT(gen, nr) (((gen) << 16) | ((nr) & 0xFFFF))

/*===========================================================================*
 *                         ARCHITECTURE INCLUDES                            *
 *===========================================================================*/

/* Include architecture-specific definitions */
#if defined(__x86_64__)
#include "arch/x86_64/types.h"
#elif defined(__i386__)
#include "arch/i386/types.h" 
#elif defined(__arm__) || defined(__earm__)
#include "arch/arm/types.h"
#elif defined(__aarch64__)
#include "arch/aarch64/types.h"
#endif

/*===========================================================================*
 *                          ERROR HANDLING                                  *
 *===========================================================================*/

/** @brief Kernel panic function */
noreturn void panic(const char *message, ...);

/** @brief Kernel assertion macro */
#define KASSERT(expr) \
    do { \
        if (!(expr)) { \
            panic("Assertion failed: %s at %s:%d", #expr, __FILE__, __LINE__); \
        } \
    } while (0)
