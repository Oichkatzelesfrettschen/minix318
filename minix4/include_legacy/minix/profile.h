#ifndef _MINIX_PROFILE_H
#define _MINIX_PROFILE_H

#include <minix/types.h> // For u64_t or similar cycle counter type
#include <minix/sys_config.h> // For CONFIG_PROFILING build flag

#ifdef CONFIG_PROFILING

// Define the events/points to be profiled
// Using X-Macro technique for easy expansion
#define PROF_POINTS_LIST \
    X(IPC_SYSCALL_ENTRY)  /* Entry into sys_ipc_send_cap */ \
    X(IPC_MSG_COPY)       /* Message copy from user */ \
    X(IPC_CAP_LOOKUP)     /* cap_lookup call */ \
    X(IPC_MDLM_CHECK)     /* mdlm_access_check call (stubbed for now) */ \
    X(IPC_FAST_SEND)      /* Call to ipc_fast_send (which might call mini_send) */ \
    X(IPC_KERNEL_EXIT)    /* Just before returning to user from syscall */ \
    X(CTX_SWITCH_TO_PM)   /* Placeholder: Before context switch to PM */ \
    X(CTX_SWITCH_TO_APP)  /* Placeholder: Before context switch back to App */ \
    X(PM_MSG_PROCESSING)  /* Placeholder: Inside PM for its processing time */

#define X(name) PROF_ID_##name,
enum prof_point_id {
    PROF_POINTS_LIST
    PROF_ID_COUNT // Total number of profiling points
};
#undef X

// Structure to hold profiling data for each point
struct prof_point_data {
    const char *name;
    u64_t total_cycles;
    u64_t entry_cycles; // For interval timing
    u64_t count;
    u64_t min_cycles;
    u64_t max_cycles;
};

// External declaration of the profiling data table
extern struct prof_point_data kernel_prof_data[PROF_ID_COUNT];

// Function to read Time Stamp Counter (TSC) - architecture specific
extern u64_t arch_rdtsc(void);

// Macros for starting and ending a profiling section
#define PROF_START(point_id) \
    do { \
        kernel_prof_data[point_id].entry_cycles = arch_rdtsc(); \
    } while (0)

#define PROF_END(point_id) \
    do { \
        u64_t _end_cycles = arch_rdtsc(); \
        u64_t _duration = _end_cycles - kernel_prof_data[point_id].entry_cycles; \
        kernel_prof_data[point_id].total_cycles += _duration; \
        kernel_prof_data[point_id].count++; \
        if (kernel_prof_data[point_id].count == 1 || _duration < kernel_prof_data[point_id].min_cycles) { \
            kernel_prof_data[point_id].min_cycles = _duration; \
        } \
        if (_duration > kernel_prof_data[point_id].max_cycles) { \
            kernel_prof_data[point_id].max_cycles = _duration; \
        } \
    } while (0)

// Function to initialize profiling data
void k_prof_init(void);
// Function to print profiling data (e.g., via a new syscall or debug output)
void k_prof_print(void);

#else // CONFIG_PROFILING not defined

// Define empty macros if profiling is disabled
#define PROF_START(point_id) do {} while(0)
#define PROF_END(point_id)   do {} while(0)

// Define empty inline functions if profiling is disabled, so calls don't break
static inline void k_prof_init(void) {}
static inline void k_prof_print(void) {}

#endif /* CONFIG_PROFILING */

#endif /* _MINIX_PROFILE_H */
