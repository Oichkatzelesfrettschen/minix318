/**
 * @file arch_proto.h
 * @brief Architecture-specific function prototypes
 * @details Common interface for architecture-specific functions
 * @version 4.0.0
 * @date 2025-06-12
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Forward declarations */
struct proc;

/* Memory management functions */
void arch_memory_init(void);
void *arch_map_phys(uint64_t phys_addr, size_t size, uint32_t flags);
void arch_unmap_phys(void *virt_addr, size_t size);
uint64_t arch_virt_to_phys(void *virt_addr);
void arch_flush_tlb(void);
void arch_flush_page(void *virt_addr);

/* Process context functions */
void arch_proc_init(struct proc *p);
void arch_proc_cleanup(struct proc *p);
void arch_switch_to(struct proc *p);
void arch_save_context(struct proc *p);
void arch_restore_context(struct proc *p);

/* Interrupt and trap handling */
void arch_enable_interrupts(void);
void arch_disable_interrupts(void);
bool arch_interrupts_enabled(void);
void arch_set_interrupt_handler(int irq, void (*handler)(void));

/* System control */
void arch_halt(void) __attribute__((noreturn));
void arch_reset(void) __attribute__((noreturn));
void arch_idle(void);

/* Cache management */
void arch_flush_dcache(void);
void arch_flush_icache(void);
void arch_flush_cache_range(void *start, size_t size);

/* Atomic operations */
bool arch_atomic_test_and_set(volatile uint32_t *ptr);
void arch_atomic_clear(volatile uint32_t *ptr);
uint32_t arch_atomic_swap(volatile uint32_t *ptr, uint32_t value);

/* Platform identification */
const char *arch_get_platform_name(void);
uint32_t arch_get_cpu_features(void);
uint32_t arch_get_cpu_frequency(void);

/* Debug and profiling */
uint64_t arch_get_timestamp(void);
void arch_debug_break(void);
void arch_print_stacktrace(void);
