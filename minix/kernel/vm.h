/**
 * @file vm.h
 * @brief MINIX4 Virtual Memory Management Header
 * @details Unified virtual memory definitions and interfaces
 * @version 4.0.0
 * @date 2025-06-12
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Added kernel headers
#include <minix/kernel_types.h>
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <klib/include/kmemory.h>

/* Virtual memory constants */
#define VM_PAGE_SIZE        4096
#define VM_PAGE_MASK        (VM_PAGE_SIZE - 1)
#define VM_PAGE_SHIFT       12

/* Memory protection flags */
#define VM_PROT_NONE        0x00    /* No access */
#define VM_PROT_READ        0x01    /* Read access */
#define VM_PROT_WRITE       0x02    /* Write access */
#define VM_PROT_EXEC        0x04    /* Execute access */
#define VM_PROT_USER        0x08    /* User mode access */
#define VM_PROT_KERNEL      0x10    /* Kernel mode only */

/* Pseudo error codes for VM operations */
#define VMSUSPEND       (-996)
#define EFAULT_SRC      (-995)
#define EFAULT_DST      (-994)

/* Physical copy with fault catching */
#define PHYS_COPY_CATCH(src, dst, size, a) {	\
	catch_pagefaults++;			\
	a = phys_copy(src, dst, size);		\
	catch_pagefaults--;			\
	}

/* External variables */
extern volatile int catch_pagefaults;

/* Function prototypes */
int phys_copy(uint64_t src, uint64_t dst, size_t size);
void vm_init(void);
void vm_flush_tlb(void);
void vm_flush_tlb_page(void *vaddr);
