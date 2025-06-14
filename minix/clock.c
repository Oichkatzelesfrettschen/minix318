/**
 * @file clock.c
 * @brief MINIX4 Kernel Clock Management System
 * @details Modern C23 implementation of the kernel clock subsystem providing
 *          timer interrupts, callout scheduling, process accounting, and
 *          real-time clock management for the MINIX4 microkernel.
 * 
 * This module implements:
 * - Hardware timer interrupt handling
 * - Kernel callout/timeout mechanism 
 * - Process CPU time accounting
 * - System time maintenance
 * - Scheduler quantum management
 * - Profile timer support
 * 
 * @version 4.0.0
 * @date 2025-06-13
 * @author MINIX Development Team
 * @copyright Copyright (c) 2025 MINIX Project
 */

#define _POSIX_C_SOURCE 202311L

/* Standard C23 headers */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* MINIX kernel headers */
#include "minix/kernel/include/clock.h"

/* Clock system constants */
#define CLOCK_MAGIC         0xC10CC10C  /**< Magic number for validation */

/** Clock subsystem state */
typedef struct clock_state {
    uint32_t magic;                     /**< Structure validation magic */
    volatile uint64_t ticks;            /**< Total clock ticks since boot */
    volatile uint64_t wall_time;        /**< Wall clock time (seconds) */
    volatile uint32_t lightning_bolt;   /**< Lightning bolt counter */
    volatile bool initialized;          /**< Initialization flag */
    uint32_t reserved[4];              /**< Reserved for future use */
} clock_state_t;

/** Global clock state */
static clock_state_t g_clock_state = {
    .magic = CLOCK_MAGIC,
    .ticks = 0,
    .wall_time = 0,
    .lightning_bolt = 0,
    .initialized = false
};

/** Global callout list */
callout_t callout_list[CLOCK_MAX_CALLOUTS];

/** Global process table stub */
process_t process_table[MAX_PROCESSES];

/* Forward declarations */
static int32_t clock_handle_callouts(void);
static void clock_update_process_times(uint32_t processor_status);
static void clock_update_system_time(uint32_t processor_status);
static void clock_schedule_tick(void);
static bool clock_is_user_mode(uint32_t processor_status);

/**
 * @brief Main clock interrupt handler
 * @details Called directly from the real-time clock interrupt. Handles all
 *          time-related kernel operations including callouts, process
 *          accounting, system time updates, and scheduler notifications.
 * 
 * This function performs the following operations:
 * 1. Restart the hardware clock
 * 2. Update system display registers
 * 3. Process pending callouts
 * 4. Update process CPU time accounting
 * 5. Maintain system wall clock time
 * 6. Handle scheduler quantum expiration
 * 7. Process profile timer events
 * 8. Signal pending alarm events
 * 
 * @param[in] dev Device number (hardware timer device)
 * @param[in] stack_pointer Current stack pointer at interrupt
 * @param[in] r1 Register 1 state
 * @param[in] nps New processor status
 * @param[in] r0 Register 0 state  
 * @param[in] pc Program counter at interrupt
 * @param[in] ps Processor status word
 * 
 * @return void (interrupt handler)
 * 
 * @note This function runs in interrupt context with elevated privileges
 * @warning Must complete quickly to avoid timer overruns
 * @see timeout(), wakeup(), setpri()
 */
void clock_interrupt_handler(uint32_t dev, uint32_t stack_pointer, 
                           uint32_t r1, uint32_t nps, uint32_t r0,
                           uint32_t pc, uint32_t ps)
{
    /* Validate clock subsystem state */
    if (g_clock_state.magic != CLOCK_MAGIC || 
        !atomic_load(&g_clock_state.initialized)) {
        panic("Clock subsystem not properly initialized");
        return;
    }

    /* Step 1: Restart hardware clock timer */
    *((volatile uint32_t*)CLOCK_CONTROL_REG) = CLOCK_RESTART_VALUE;

    /* Step 2: Update system display (for debugging/monitoring) */
    display_update_clock_status();

    /* Step 3: Handle pending callouts if not in high interrupt context */
    if ((ps & CLOCK_INTMASK) == 0) {
        int32_t result = clock_handle_callouts();
        if (result < 0) {
            kdebug("Clock callout processing failed: %d", result);
        }
    }

    /* Step 4: Update process and system time accounting */
    clock_update_process_times(ps);

    /* Step 5: Update system time and handle periodic events */
    clock_update_system_time(ps);

    /* Increment global tick counter */
    atomic_fetch_add(&g_clock_state.ticks, 1);
}

/**
 * @brief Handle kernel callout processing
 * @details Processes all pending kernel callouts (delayed function calls).
 *          Callouts are stored in a sorted linked list where each entry
 *          contains the relative time until execution.
 * 
 * @return 0 on success, negative errno on failure
 * @retval 0 Success
 * @retval -ENOENT No callouts pending
 * @retval -EFAULT Invalid callout structure
 * 
 * @note Must be called with interrupts at appropriate level
 * @warning Callout functions must complete quickly
 */
static int32_t clock_handle_callouts(void)
{
    callout_t *current = NULL;
    callout_t *next = NULL;
    
    /* Check if any callouts are pending */
    if (callout_list[0].function == NULL) {
        return -ENOENT; /* No callouts pending */
    }

    /* Find first callout with expired time */
    current = &callout_list[0];
    while (current->time_left <= 0 && current->function != NULL) {
        current++;
    }
    
    /* Decrement time for next pending callout */
    if (current->function != NULL) {
        current->time_left--;
    }

    /* Execute all expired callouts */
    interrupt_disable(IRQ_LEVEL_5);
    
    if (callout_list[0].time_left <= 0) {
        current = &callout_list[0];
        
        /* Execute all expired callouts */
        while (current->function != NULL && current->time_left <= 0) {
            /* Validate function pointer before calling */
            if (current->function == NULL) {
                kwarn("Invalid callout function pointer detected");
                break;
            }
            
            /* Execute the callout function */
            (current->function)(current->argument);
            current++;
        }
        
        /* Compact the callout list by removing executed entries */
        next = &callout_list[0];
        while ((next->function = current->function) != NULL) {
            next->time_left = current->time_left;
            next->argument = current->argument;
            current++;
            next++;
        }
    }
    
    interrupt_enable(IRQ_LEVEL_5);
    return 0;
}

/**
 * @brief Update process CPU time accounting
 * @details Updates user and system time counters for the current process
 *          and handles profile timer events if profiling is enabled.
 * 
 * @param[in] processor_status Current processor status word
 * 
 * @note Differentiates between user mode and kernel mode execution time
 * @see incupc() for profile counter updates
 */
static void clock_update_process_times(uint32_t processor_status)
{
    process_t *current_proc = get_current_process();
    
    if (current_proc == NULL) {
        return; /* No current process */
    }

    /* Update time counters based on execution mode */
    if (clock_is_user_mode(processor_status)) {
        /* Process was executing in user mode */
        atomic_fetch_add(&current_proc->user_time, 1);
        
        /* Handle profile timer if enabled */
        if (current_proc->profile_buffer != NULL) {
            profile_update_counter(processor_status & 0xFFFF, 
                                 current_proc->profile_buffer);
        }
    } else {
        /* Process was executing in kernel mode */
        atomic_fetch_add(&current_proc->system_time, 1);
    }

    /* Update CPU usage counter with overflow protection */
    uint32_t old_cpu = atomic_load(&current_proc->cpu_usage);
    if (old_cpu < UINT32_MAX) {
        atomic_fetch_add(&current_proc->cpu_usage, 1);
    }
}

/**
 * @brief Update system time and handle periodic events
 * @details Maintains system wall clock, handles lightning bolt events,
 *          updates process scheduling priorities, and signals waiting
 *          processes when appropriate.
 * 
 * @param[in] processor_status Current processor status word
 * 
 * @note Lightning bolt events occur every 4 seconds for housekeeping
 */
static void clock_update_system_time(uint32_t processor_status)
{
    uint64_t current_ticks = atomic_load(&g_clock_state.ticks);
    
    /* Check if a full second has elapsed (HZ ticks) */
    if ((current_ticks % CLOCK_HZ) == 0) {
        /* Avoid processing if in high interrupt context */
        if ((processor_status & CLOCK_INTMASK) != 0) {
            return;
        }

        /* Update wall clock time */
        atomic_fetch_add(&g_clock_state.wall_time, 1);
        
        /* Signal processes waiting on timeout */
        interrupt_disable(IRQ_LEVEL_1);
        timeout_wakeup_expired();
        
        /* Handle lightning bolt events (every 4 seconds) */
        uint32_t lightning = atomic_fetch_add(&g_clock_state.lightning_bolt, 1);
        if ((lightning & 0x03) == 0) {
            /* Schedule quantum-expired processes */
            scheduler_request_reschedule();
            wakeup_processes(&g_clock_state.lightning_bolt);
        }
        
        /* Update all process priorities and aging */
        clock_schedule_tick();
        
        /* Handle run queue management */
        if (scheduler_has_runin_processes()) {
            scheduler_clear_runin();
            wakeup_processes(&scheduler_runin_queue);
        }
        
        /* Handle signal processing for user mode processes */
        if (clock_is_user_mode(processor_status)) {
            process_t *current = get_current_process();
            if (current != NULL) {
                current->saved_registers = processor_status & 0xFFFF;
                
                if (signal_is_pending(current)) {
                    signal_process_pending(current);
                }
                
                scheduler_update_priority(current);
            }
        }
        
        interrupt_enable(IRQ_LEVEL_1);
    }
}

/**
 * @brief Perform scheduler tick processing
 * @details Updates all process priorities, ages CPU usage counters,
 *          and performs other periodic scheduler maintenance tasks.
 * 
 * @note Called once per second during system time updates
 * @warning Must complete quickly to avoid timer delays
 */
static void clock_schedule_tick(void)
{
    process_t *proc = NULL;
    
    /* Iterate through all processes in the process table */
    for (size_t i = 0; i < MAX_PROCESSES; i++) {
        proc = &process_table[i];
        
        /* Skip empty process slots */
        if (proc->state == PROC_STATE_UNUSED) {
            continue;
        }
        
        /* Age the process (increment runtime counter) */
        if (proc->runtime < CLOCK_MAX_PRIORITY) {
            proc->runtime++;
        }
        
        /* Decay CPU usage to prevent starvation */
        uint32_t cpu_usage = atomic_load(&proc->cpu_usage);
        if (cpu_usage > CLOCK_SCHED_DECAY) {
            atomic_fetch_sub(&proc->cpu_usage, CLOCK_SCHED_DECAY);
        } else {
            atomic_store(&proc->cpu_usage, 0);
        }
        
        /* Recalculate priority if it's a user process */
        if (proc->priority > PROC_PRIORITY_USER) {
            scheduler_update_priority(proc);
        }
    }
}

/**
 * @brief Check if processor was in user mode
 * @details Examines processor status word to determine execution mode.
 * 
 * @param[in] processor_status Processor status word
 * @return true if in user mode, false if in kernel mode
 */
static bool clock_is_user_mode(uint32_t processor_status)
{
    return (processor_status & CLOCK_UMODE) == CLOCK_UMODE;
}

/**
 * @brief Schedule a delayed function call (timeout/callout)
 * @details Arranges for a function to be called after a specified delay.
 *          The callout is inserted into a sorted list where each entry
 *          contains the relative time until the next entry should execute.
 * 
 * @param[in] function Function pointer to call when timeout expires
 * @param[in] argument Argument to pass to the function
 * @param[in] ticks_delay Delay in clock ticks (1/HZ seconds)
 * 
 * @return 0 on success, negative errno on failure
 * @retval 0 Success
 * @retval -EINVAL Invalid function pointer or delay
 * @retval -ENOMEM No free callout entries available
 * @retval -EOVERFLOW Delay too large
 * 
 * @note The actual delay may be up to one tick longer than specified
 * @warning Function must complete quickly when called
 * 
 * @example
 * ```c
 * // Schedule function to run in 5 seconds
 * int result = timeout(my_function, my_arg, 5 * CLOCK_HZ);
 * if (result != 0) {
 *     kerror("Failed to schedule timeout: %d", result);
 * }
 * ```
 */
int32_t timeout(callout_func_t function, void *argument, uint32_t ticks_delay)
{
    callout_t *insert_pos = NULL;
    callout_t *scan_pos = NULL;
    uint32_t remaining_time = ticks_delay;
    uint32_t saved_interrupt_state = 0;

    /* Validate input parameters */
    if (function == NULL) {
        return -EINVAL;
    }
    
    if (ticks_delay == 0 || ticks_delay > UINT32_MAX / 2) {
        return -EOVERFLOW;
    }

    /* Save interrupt state and disable interrupts */
    saved_interrupt_state = interrupt_save_and_disable();
    
    /* Find the correct insertion point in the sorted callout list */
    insert_pos = &callout_list[0];
    
    /* Skip entries whose time is less than our delay */
    while (insert_pos->function != NULL && insert_pos->time_left <= remaining_time) {
        remaining_time -= insert_pos->time_left;
        insert_pos++;
    }
    
    /* Adjust the time of the entry we're inserting before */
    if (insert_pos->function != NULL) {
        insert_pos->time_left -= remaining_time;
    }
    
    /* Find the end of the callout list */
    scan_pos = insert_pos;
    while (scan_pos->function != NULL) {
        scan_pos++;
    }
    
    /* Check if we have room for another entry */
    if (scan_pos >= &callout_list[CLOCK_MAX_CALLOUTS - 1]) {
        interrupt_restore(saved_interrupt_state);
        return -ENOMEM;
    }
    
    /* Shift existing entries to make room */
    while (scan_pos >= insert_pos) {
        (scan_pos + 1)->time_left = scan_pos->time_left;
        (scan_pos + 1)->function = scan_pos->function;
        (scan_pos + 1)->argument = scan_pos->argument;
        scan_pos--;
    }
    
    /* Insert the new callout entry */
    insert_pos->time_left = remaining_time;
    insert_pos->function = function;
    insert_pos->argument = argument;
    
    /* Restore interrupt state */
    interrupt_restore(saved_interrupt_state);
    
    return 0;
}

/**
 * @brief Initialize the clock subsystem
 * @details Initializes the kernel clock management system, sets up
 *          the callout table, and prepares timer hardware.
 * 
 * @return 0 on success, negative errno on failure
 * @retval 0 Success
 * @retval -EALREADY Already initialized
 * @retval -EIO Hardware initialization failed
 * 
 * @note Must be called during kernel initialization
 * @see clock_shutdown()
 */
int32_t clock_init(void)
{
    /* Check if already initialized */
    if (atomic_load(&g_clock_state.initialized)) {
        return -EALREADY;
    }

    /* Initialize callout table */
    for (size_t i = 0; i < CLOCK_MAX_CALLOUTS; i++) {
        callout_list[i].function = NULL;
        callout_list[i].argument = NULL;
        callout_list[i].time_left = 0;
    }

    /* Initialize hardware timer */
    int32_t hw_result = timer_hardware_init(CLOCK_HZ);
    if (hw_result != 0) {
        kerror("Failed to initialize timer hardware: %d", hw_result);
        return -EIO;
    }

    /* Mark subsystem as initialized */
    atomic_store(&g_clock_state.initialized, true);
    
    kinfo("Clock subsystem initialized (HZ=%d)", CLOCK_HZ);
    return 0;
}

/**
 * @brief Shutdown the clock subsystem
 * @details Cleanly shuts down the clock subsystem, cancels pending
 *          callouts, and disables timer hardware.
 * 
 * @return 0 on success, negative errno on failure
 * @retval 0 Success
 * @retval -ENODEV Not initialized
 * 
 * @note Should be called during kernel shutdown
 * @see clock_init()
 */
int32_t clock_shutdown(void)
{
    /* Check if initialized */
    if (!atomic_load(&g_clock_state.initialized)) {
        return -ENODEV;
    }

    /* Disable timer hardware */
    timer_hardware_shutdown();

    /* Clear all pending callouts */
    uint32_t saved_state = interrupt_save_and_disable();
    
    for (size_t i = 0; i < CLOCK_MAX_CALLOUTS; i++) {
        callout_list[i].function = NULL;
        callout_list[i].argument = NULL;
        callout_list[i].time_left = 0;
    }
    
    interrupt_restore(saved_state);

    /* Mark as not initialized */
    atomic_store(&g_clock_state.initialized, false);
    
    kinfo("Clock subsystem shutdown complete");
    return 0;
}

/**
 * @brief Get current system uptime in ticks
 * @details Returns the number of clock ticks since system boot.
 * 
 * @return Current tick count
 * @note Tick frequency is CLOCK_HZ (typically 100 Hz)
 */
uint64_t clock_get_uptime_ticks(void)
{
    return atomic_load(&g_clock_state.ticks);
}

/**
 * @brief Get current system uptime in seconds
 * @details Returns the number of seconds since system boot.
 * 
 * @return Current uptime in seconds
 */
uint64_t clock_get_uptime_seconds(void)
{
    return atomic_load(&g_clock_state.ticks) / CLOCK_HZ;
}

/**
 * @brief Get current wall clock time
 * @details Returns the current wall clock time in seconds since
 *          the Unix epoch (January 1, 1970).
 * 
 * @return Current wall clock time
 * @note This requires the system time to be properly set
 */
uint64_t clock_get_wall_time(void)
{
    return atomic_load(&g_clock_state.wall_time);
}
