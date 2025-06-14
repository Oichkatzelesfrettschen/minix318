/**
 * @file clock.h
 * @brief MINIX4 Kernel Clock Management Header
 * @details Definitions and declarations for the kernel clock subsystem,
 *          including timer interrupts, callouts, and time management.
 * @version 4.0.0
 * @date 2025-06-13
 * @author MINIX Development Team
 * @copyright Copyright (c) 2025 MINIX Project
 */

#pragma once

#define _POSIX_C_SOURCE 202311L

/* Standard C23 headers */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdatomic.h>

/* MINIX kernel base */
#include "../kernel.h"

/*===========================================================================*
 *                              CONSTANTS                                   *
 *===========================================================================*/

/** Clock system constants */
#define CLOCK_HZ                100         /**< Timer frequency (100Hz) */
#define CLOCK_QUANTUM_MS        10          /**< Scheduler quantum in milliseconds */
#define CLOCK_PROF_INTERVAL     4           /**< Profile timer interval in seconds */
#define CLOCK_MAX_CALLOUTS      128         /**< Maximum pending callouts */

/** Hardware clock registers */
#define CLOCK_CONTROL_REG       0x40        /**< Clock control register address */
#define CLOCK_RESTART_VALUE     0x115       /**< Value to restart clock */

/** Process status mode bits */
#define CLOCK_UMODE             0170000     /**< User mode bit mask */
#define CLOCK_INTMASK           0340        /**< Interrupt mask bits */

/** Scheduler constants */
#define CLOCK_SCHED_DECAY       10          /**< CPU usage decay factor */
#define CLOCK_MAX_PRIORITY      127         /**< Maximum priority value */

/** Error codes */
#define ENOENT                  2           /**< No such file or directory */
#define EINVAL                  22          /**< Invalid argument */
#define ENOMEM                  12          /**< Out of memory */
#define EALREADY                114         /**< Operation already in progress */
#define EIO                     5           /**< I/O error */
#define ENODEV                  19          /**< No such device */
#define EOVERFLOW               75          /**< Value too large */
#define EFAULT                  14          /**< Bad address */

/*===========================================================================*
 *                              TYPE DEFINITIONS                            *
 *===========================================================================*/

/** Function pointer type for callout functions */
typedef void (*callout_func_t)(void *arg);

/** Callout entry structure */
typedef struct callout {
    callout_func_t function;            /**< Function to call */
    void *argument;                     /**< Argument to pass to function */
    uint32_t time_left;                 /**< Ticks remaining until execution */
} callout_t;

/** Process state enumeration */
typedef enum proc_state {
    PROC_STATE_UNUSED = 0,              /**< Process slot is unused */
    PROC_STATE_RUNNING,                 /**< Process is running */
    PROC_STATE_READY,                   /**< Process is ready to run */
    PROC_STATE_BLOCKED,                 /**< Process is blocked */
    PROC_STATE_ZOMBIE                   /**< Process is zombie */
} proc_state_t;

/** Process structure (simplified for clock module) */
typedef struct process {
    proc_state_t state;                 /**< Current process state */
    uint32_t priority;                  /**< Process priority */
    uint32_t runtime;                   /**< Process runtime counter */
    volatile uint32_t cpu_usage;        /**< CPU usage counter */
    volatile uint64_t user_time;        /**< User mode time */
    volatile uint64_t system_time;      /**< System mode time */
    void *profile_buffer;               /**< Profile buffer pointer */
    uint32_t saved_registers;           /**< Saved register state */
} process_t;

/*===========================================================================*
 *                              GLOBAL VARIABLES                            *
 *===========================================================================*/

/** Global callout list */
extern callout_t callout_list[CLOCK_MAX_CALLOUTS];

/** Global process table */
extern process_t process_table[];

/** Maximum number of processes */
#define MAX_PROCESSES           256

/** Process priority constants */
#define PROC_PRIORITY_USER      50

/*===========================================================================*
 *                              FUNCTION PROTOTYPES                         *
 *===========================================================================*/

/* Core clock functions */
void clock_interrupt_handler(uint32_t dev, uint32_t stack_pointer, 
                           uint32_t r1, uint32_t nps, uint32_t r0,
                           uint32_t pc, uint32_t ps);
int32_t timeout(callout_func_t function, void *argument, uint32_t ticks_delay);
int32_t clock_init(void);
int32_t clock_shutdown(void);

/* Time query functions */
uint64_t clock_get_uptime_ticks(void);
uint64_t clock_get_uptime_seconds(void);
uint64_t clock_get_wall_time(void);

/* Hardware abstraction functions (to be implemented) */
int32_t timer_hardware_init(uint32_t frequency);
void timer_hardware_shutdown(void);

/* Display functions (to be implemented) */
void display_update_clock_status(void);

/* Interrupt management functions (to be implemented) */
uint32_t interrupt_save_and_disable(void);
void interrupt_restore(uint32_t state);
void interrupt_disable(uint32_t level);
void interrupt_enable(uint32_t level);

/* Process management functions (to be implemented) */
process_t *get_current_process(void);
void scheduler_request_reschedule(void);
void scheduler_update_priority(process_t *proc);
bool scheduler_has_runin_processes(void);
void scheduler_clear_runin(void);
extern void *scheduler_runin_queue;

/* Signal management functions (to be implemented) */
bool signal_is_pending(process_t *proc);
void signal_process_pending(process_t *proc);

/* Memory/profile functions (to be implemented) */
void profile_update_counter(uint32_t pc, void *profile_buffer);

/* Wakeup functions (to be implemented) */
void wakeup_processes(void *wait_channel);
void timeout_wakeup_expired(void);

/* Logging functions (to be implemented) */
void kdebug(const char *format, ...);
void kerror(const char *format, ...);
void kwarn(const char *format, ...);
void kinfo(const char *format, ...);
void panic(const char *format, ...) __attribute__((noreturn));

/* IRQ level constants */
#define IRQ_LEVEL_1             1
#define IRQ_LEVEL_5             5
#define IRQ_LEVEL_7             7

/* Legacy compatibility functions */
int boot_cpu_init_timer(unsigned freq);
int app_cpu_init_timer(unsigned freq);
int timer_int_handler(void);
int init_local_timer(unsigned freq);
void stop_local_timer(void);
void restart_local_timer(void);
uint64_t ms_2_cpu_time(unsigned ms);
unsigned cpu_time_2_ms(uint64_t cpu_time);
