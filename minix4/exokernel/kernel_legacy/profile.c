#include <minix/profile.h>
#include <minix/sys_config.h> // For CONFIG_PROFILING
#include <minix/types.h>
#include <klib/include/kprintf.h> // For kprintf (assuming kprintf_stub is for very early prints)
#include <limits.h> // For ULLONG_MAX

#ifdef CONFIG_PROFILING

// Define the profiling data table
struct prof_point_data kernel_prof_data[PROF_ID_COUNT];

// X-Macro to generate names for printout
#define X(name) #name,
static const char *prof_point_names[] = {
    PROF_POINTS_LIST
    /* PROF_ID_COUNT is the total number of points, so no name for it */
};
#undef X

// Architecture-specific rdtsc implementation (x86 example)
// This should ideally be in an arch-specific file and declared in an arch header.
#if defined(__i386__) || defined(__x86_64__)
u64_t arch_rdtsc(void) {
    u32_t lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((u64_t)hi << 32) | lo;
}
#else
// Fallback or error for non-x86 architectures if TSC is not available
u64_t arch_rdtsc(void) {
    kprintf("Warning: arch_rdtsc() not implemented for this architecture. Profiling data will be zero.\n");
    return 0;
}
#endif

void k_prof_init(void) {
    for (int i = 0; i < PROF_ID_COUNT; i++) {
        if (i < (sizeof(prof_point_names) / sizeof(prof_point_names[0]))) { // Check bounds for safety
             kernel_prof_data[i].name = prof_point_names[i];
        } else {
             kernel_prof_data[i].name = "PROF_ID_UNKNOWN"; // Should not happen if list is correct
        }
        kernel_prof_data[i].total_cycles = 0;
        kernel_prof_data[i].entry_cycles = 0;
        kernel_prof_data[i].count = 0;
        kernel_prof_data[i].min_cycles = ULLONG_MAX;
        kernel_prof_data[i].max_cycles = 0;
    }
    kprintf("Kernel profiling initialized (%d points).\n", PROF_ID_COUNT);
}

void k_prof_print(void) {
    kprintf("--- Kernel Profile Data ---\n");
    for (int i = 0; i < PROF_ID_COUNT; i++) {
        if (kernel_prof_data[i].count > 0) {
            u64_t avg_cycles = kernel_prof_data[i].total_cycles / kernel_prof_data[i].count;
            kprintf("%-20s: Cnt=%llu, Avg=%llu, Min=%llu, Max=%llu, Tot=%llu cyc\n",
                      kernel_prof_data[i].name ? kernel_prof_data[i].name : "INVALID_PROF_NAME",
                      kernel_prof_data[i].count,
                      avg_cycles,
                      (kernel_prof_data[i].min_cycles == ULLONG_MAX) ? 0 : kernel_prof_data[i].min_cycles,
                      kernel_prof_data[i].max_cycles,
                      kernel_prof_data[i].total_cycles);
        } else {
            kprintf("%-20s: No data.\n", kernel_prof_data[i].name ? kernel_prof_data[i].name : "INVALID_PROF_NAME");
        }
    }
    kprintf("---------------------------\n");
}

#endif /* CONFIG_PROFILING */
