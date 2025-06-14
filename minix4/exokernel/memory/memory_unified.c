/**
 * @file memory_unified.c
 * @brief Unified memory management
 * @details This file is a synthesized/unified implementation combining multiple
 *          legacy MINIX implementations into a single, modern, C23-compliant
 *          implementation for MINIX4.
 *
 * Original sources consolidated:
 * - minix4\microkernel\core\memory\memory.c
 * - minix4\libos\lib_legacy\libprtdiag\common\memory.c
 * - minix4\exokernel\minix_drivers\storage\memory\memory.c
 * - minix4\exokernel\kernel_legacy\tools\ctf\common\memory.c
 * - minix4\exokernel\kernel_legacy\cmd\mdb\common\modules\genunix\memory.c
 * - minix4\exokernel\kernel_legacy\arch\earm\memory.c
 * - minix4\exokernel\kernel_legacy\arch\i386\memory.c

 *
 * @version 4.0.0
 * @date 2025-06-13
 * @author MINIX4 Synthesis Engine
 * @copyright Copyright (c) 2025 MINIX Project
 */

#pragma once

#define _POSIX_C_SOURCE 202311L

/* Standard C23 headers */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdatomic.h>

/* MINIX4 system headers */
#include <minix4/kernel_types.h>
#include <minix4/config.h>

/* Architecture-specific headers */
#ifdef ARCH_X86_64
#include <minix4/arch/x86_64/arch.h>
#elif defined(ARCH_I386)
#include <minix4/arch/i386/arch.h>
#elif defined(ARCH_ARM)
#include <minix4/arch/arm/arch.h>
#endif


/* ============================================================================
 * SYNTHESIS NOTE: CAREFUL ANALYSIS REQUIRED
 * ============================================================================
 * This file contains 7 very different implementations that
 * require manual review and careful synthesis. The files have been
 * concatenated for analysis, but manual integration is needed.
 * 
 * TODO: Manual synthesis tasks:
 * 1. Analyze functional differences between implementations
 * 2. Determine best approach for each function
 * 3. Modernize to C23 standards
 * 4. Add proper error handling and security
 * 5. Create comprehensive unit tests
 * ============================================================================ */


/* ============================================================================
 * SOURCE 1/7: minix4\microkernel\core\memory\memory.c
 * Size: 2,387 bytes, Lines: 124
 * Hash: cd0159b5da87...
 * ============================================================================ */

/**
 * @file memory.c
 * @brief MINIX4 Memory Management Implementation
 * @version 4.0.0
 * @date 2025-06-12
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdatomic.h>
#include "spinlock.h"

// Memory layout constants
#define KERNEL_BASE     0xFFFFFFFF80000000UL
#define USER_BASE       0x0000000000400000UL
#define HEAP_BASE       0xFFFFFFFF40000000UL
#define PGSIZE          4096

// Page allocation structures
struct page {
    atomic_int refcount;
    uint32_t flags;
    struct page *next;
};

struct memory_zone {
    uint64_t start;
    uint64_t end;
    size_t free_pages;
    struct page *free_list;
    struct spinlock lock;
};

// Global memory state
static struct memory_zone zones[4];
static size_t total_memory = 0;
static size_t free_memory = 0;
static atomic_bool mm_initialized = false;

/**
 * @brief Initialize physical memory allocator
 */
int palloc_init(uint64_t available_memory)
{
    // Simple initialization - in real implementation this would
    // set up memory zones, page frames, etc.
    total_memory = available_memory;
    free_memory = available_memory * 80 / 100; // 80% available
    
    atomic_store(&mm_initialized, true);
    return 0;
}

/**
 * @brief Initialize virtual memory management
 */
int vmm_init(void)
{
    // Initialize page tables, memory mapping, etc.
    return 0;
}

/**
 * @brief Initialize kernel heap
 */
int kmalloc_init(void)
{
    // Initialize kernel heap allocator
    return 0;
}

/**
 * @brief Allocate a page
 */
void *palloc(void)
{
    if (!atomic_load(&mm_initialized)) {
        return NULL;
    }
    
    // Simple allocation - would use proper page allocator
    static uint64_t next_page = 0x200000; // Start after kernel
    void *page = (void *)next_page;
    next_page += PGSIZE;
    return page;
}

/**
 * @brief Free a page
 */
void pfree(void *page)
{
    // Add page back to free list
    (void)page;
}

/**
 * @brief Allocate kernel memory
 */
void *kmalloc(size_t size)
{
    // Simple kernel allocator - would use proper heap
    return palloc(); // For now, just allocate pages
}

/**
 * @brief Free kernel memory
 */
void kfree(void *ptr)
{
    pfree(ptr);
}

/**
 * @brief Get memory statistics
 */
void memory_get_stats(uint64_t *total, uint64_t *free, uint64_t *used)
{
    *total = total_memory;
    *free = free_memory;
    *used = total_memory - free_memory;
}



/* ============================================================================
 * SOURCE 2/7: minix4\libos\lib_legacy\libprtdiag\common\memory.c
 * Size: 13,269 bytes, Lines: 540
 * Hash: f93f18f2bcee...
 * ============================================================================ */

/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright (c) 1999-2001 by Sun Microsystems, Inc.
 * All rights reserved.
 * Copyright (c) 2020 Peter Tribble.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <kvm.h>
#include <varargs.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/openpromio.h>
#include <kstat.h>
#include <libintl.h>
#include <syslog.h>
#include <sys/dkio.h>
#include <sys/sbd_ioctl.h>
#include <sys/sbdp_mem.h>
#include <sys/serengeti.h>
#include <sys/mc.h>
#include "pdevinfo.h"
#include "display.h"
#include "pdevinfo_sun4u.h"
#include "display_sun4u.h"
#include "libprtdiag.h"

#if !defined(TEXT_DOMAIN)
#define	TEXT_DOMAIN	"SYS_TEST"
#endif

#define	KBYTE	1024
#define	MBYTE	(KBYTE * KBYTE)

#define	MEM_UK_SIZE_MASK	0x3FF

/*
 * Global variables.
 */
static memory_bank_t	*bank_head;
static memory_bank_t	*bank_tail;
static memory_seg_t	*seg_head;

/*
 * Local functions.
 */
static void add_bank_node(uint64_t mc_decode, int portid, char *bank_status);
static void add_seg_node(void);
static memory_seg_t *match_seg(uint64_t);


/*
 * Used for US-I and US-II systems
 */
/*ARGSUSED0*/
void
display_memorysize(Sys_tree *tree, struct system_kstat_data *kstats,
    struct mem_total *memory_total)
{
	log_printf(dgettext(TEXT_DOMAIN, "Memory size: "), 0);

	if (sysconf(_SC_PAGESIZE) == -1 || sysconf(_SC_PHYS_PAGES) == -1)
		log_printf(dgettext(TEXT_DOMAIN, "unable to determine\n"), 0);
	else {
		uint64_t	mem_size;

		mem_size =
		    (uint64_t)sysconf(_SC_PAGESIZE) * \
		    (uint64_t)sysconf(_SC_PHYS_PAGES);

		if (mem_size >= MBYTE)
			log_printf(dgettext(TEXT_DOMAIN, "%d Megabytes\n"),
			    (int)((mem_size+MBYTE-1) / MBYTE), 0);
		else
			log_printf(dgettext(TEXT_DOMAIN, "%d Kilobytes\n"),
			    (int)((mem_size+KBYTE-1) / KBYTE), 0);
	}
}

/*ARGSUSED0*/
void
display_memoryconf(Sys_tree *tree)
{
	/*
	 * This function is intentionally blank
	 */
}

/*
 * The following functions are for use by any US-III based systems.
 * All they need to do is to call get_us3_mem_regs()
 * and then display_us3_banks(). Each platform then needs to decide how
 * to format this data by over-riding the generic function
 * print_us3_memory_line().
 */
int
get_us3_mem_regs(Board_node *bnode)
{
	Prom_node	*pnode;
	int		portid;
	uint64_t	*ma_reg_arr;
	uint64_t	madr[NUM_MBANKS_PER_MC];
	void		*bank_status_array;
	char		*bank_status;
	int		i, status_offset;

	for (pnode = dev_find_node(bnode->nodes, "memory-controller");
	    pnode != NULL;
	    pnode = dev_next_node(pnode, "memory-controller")) {

		/* Get portid of this mc from libdevinfo. */
		portid = (*(int *)get_prop_val(find_prop(pnode, "portid")));

		/* read the logical_bank_ma_regs property for this mc node. */
		ma_reg_arr = (uint64_t *)get_prop_val(
		    find_prop(pnode, MEM_CFG_PROP_NAME));

		/*
		 * There are situations where a memory-controller node
		 * will not have the logical_bank_ma_regs property and
		 * we need to allow for these cases. They include:
		 *	- Excalibur/Littleneck systems that only
		 *	  support memory on one of their CPUs.
		 *	- Systems that support DR where a cpu board
		 *	  can be unconfigured but still connected.
		 * It is up to the caller of this function to ensure
		 * that the bank_head and seg_head pointers are not
		 * NULL after processing all memory-controllers in the
		 * system. This would indicate a situation where no
		 * memory-controllers in the system have a logical_bank_ma_regs
		 * property which should never happen.
		 */
		if (ma_reg_arr == NULL)
			continue;

		/*
		 * The first NUM_MBANKS_PER_MC of uint64_t's in the
		 * logical_bank_ma_regs property are the madr values.
		 */
		for (i = 0; i < NUM_MBANKS_PER_MC; i++) {
			madr[i] = *ma_reg_arr++;
		}

		/*
		 * Get the bank_status property for this mem controller from
		 * OBP. This contains the bank-status for each logical bank.
		 */
		bank_status_array = (void *)get_prop_val(
		    find_prop(pnode, "bank-status"));
		status_offset = 0;

		/*
		 * process each logical bank
		 */
		for (i = 0; i < NUM_MBANKS_PER_MC; i++) {
			/*
			 * Get the bank-status string for this bank
			 * from the bank_status_array we just retrieved
			 * from OBP. If the prop was not found, we
			 * malloc a bank_status and set it to "no_status".
			 */
			if (bank_status_array) {
				bank_status = ((char *)bank_status_array +
				    status_offset);

				/* Move offset to next bank_status string */
				status_offset += (strlen(bank_status) + 1);
			} else {
				bank_status = strdup("no_status");
			}

			/*
			 * create a bank_node for this bank
			 * and add it to the list.
			 */
			add_bank_node(madr[i], portid, bank_status);

			/*
			 * find the segment to which this bank
			 * belongs. If it doesn't already exist
			 * then create it. If it exists, add to it.
			 */
			add_seg_node();
		}
	}
	return (0);
}

static void
add_bank_node(uint64_t mc_decode, int portid, char *bank_status)
{
	static int	id = 0;
	memory_bank_t	*new, *bank;
	uint32_t	ifactor = MC_INTLV(mc_decode);
	uint64_t	seg_size;

	if ((new = malloc(sizeof (memory_bank_t))) == NULL) {
		perror("malloc");
		exit(1);
	}

	new->portid = portid;
	new->id = id++;
	new->valid = (mc_decode >> 63);
	new->uk = MC_UK(mc_decode);
	new->um = MC_UM(mc_decode);
	new->lk = MC_LK(mc_decode);
	new->lm = MC_LM(mc_decode);

	seg_size = ((((uint64_t)new->uk & MEM_UK_SIZE_MASK) + 1) << 26);
	new->bank_size = seg_size / ifactor;
	new->bank_status = bank_status;

	new->next = NULL;
	new->seg_next = NULL;

	/* Handle the first bank found */
	if (bank_head == NULL) {
		bank_head = new;
		bank_tail = new;
		return;
	}

	/* find last bank in list */
	bank = bank_head;
	while (bank->next)
		bank = bank->next;

	/* insert this bank into the list */
	bank->next = new;
	bank_tail = new;
}

void
display_us3_banks(void)
{
	uint64_t	base, bank_size;
	uint32_t	intlv;
	memory_bank_t	*bank, *tmp_bank;
	memory_seg_t	*seg;
	int		 mcid;
	uint64_t	dimm_size;
	uint64_t	total_bank_size = 0;
	uint64_t	total_sys_mem;
	static uint64_t	bank0_size, bank1_size, bank2_size, bank3_size;

	if ((bank_head == NULL) || (seg_head == NULL)) {
		log_printf("\nCannot find any memory bank/segment info.\n");
		return;
	}

	for (bank = bank_head; bank; bank = bank->next) {
		/*
		 * Interleave factor is determined from the
		 * lk bits in the Mem Addr Decode register.
		 *
		 * The Base Address of the memory segment in which this
		 * bank belongs is determined from the um abd uk bits
		 * of the Mem Addr Decode register.
		 *
		 * See section 9.1.5 of Cheetah Programmer's reference
		 * manual.
		 */
		intlv = ((bank->lk ^ 0xF) + 1);
		base = bank->um & ~(bank->uk);

		mcid = SG_PORTID_TO_SAFARI_ID(bank->portid);

		/* If bank is not valid, set size to zero incase it's garbage */
		if (bank->valid)
			bank_size = ((bank->bank_size) / MBYTE);
		else
			bank_size = 0;

		/*
		 * Keep track of all banks found so we can check later
		 * that this value matches the total memory in the
		 * system using the pagesize and number of pages.
		 */
		total_bank_size	+= bank_size;

		/* Find the matching segment for this bank. */
		seg = match_seg(base);

		/*
		 * Find the Dimm size by adding banks 0 + 2 and divide by 4
		 * and then adding banks 1 + 3 and divide by 4. We divide
		 * by 2 if one of the logical banks size is zero.
		 */
		switch ((bank->id) % 4) {
		case 0:
			/* have bank0_size, need bank2_size */
			bank0_size = bank_size;
			bank2_size = 0;

			tmp_bank = bank->next;
			while (tmp_bank) {
				if (tmp_bank->valid == 0) {
					tmp_bank = tmp_bank->next;
					continue;
				}
				/* Is next bank on the same mc ? */
				if (mcid != SG_PORTID_TO_SAFARI_ID(
				    tmp_bank->portid)) {
					break;
				}
				if ((tmp_bank->id) % 4 == 2) {
					bank2_size =
					    (tmp_bank->bank_size / MBYTE);
					break;
				}
				tmp_bank = tmp_bank->next;
			}
			if (bank2_size)
				dimm_size = (bank0_size + bank2_size) / 4;
			else
				dimm_size = bank0_size / 2;
			break;
		case 1:
			/* have bank1_size, need bank3_size */
			bank1_size = bank_size;
			bank3_size = 0;

			tmp_bank = bank->next;
			while (tmp_bank) {
				if (tmp_bank->valid == 0) {
					tmp_bank = tmp_bank->next;
					continue;
				}
				/* Is next bank on the same mc ? */
				if (mcid != SG_PORTID_TO_SAFARI_ID(
				    tmp_bank->portid)) {
					break;
				}
				if ((tmp_bank->id) % 4 == 3) {
					bank3_size =
					    (tmp_bank->bank_size / MBYTE);
					break;
				}
				tmp_bank = tmp_bank->next;
			}
			if (bank3_size)
				dimm_size = (bank1_size + bank3_size) / 4;
			else
				dimm_size = bank1_size / 2;
			break;
		case 2:
			/* have bank0_size and bank2_size */
			bank2_size = bank_size;
			if (bank0_size)
				dimm_size = (bank0_size + bank2_size) / 4;
			else
				dimm_size = bank2_size / 2;
			break;
		case 3:
			/* have bank1_size and bank3_size */
			bank3_size = bank_size;
			if (bank1_size)
				dimm_size = (bank1_size + bank3_size) / 4;
			else
				dimm_size = bank3_size / 4;
			break;
		}

		if (bank->valid == 0)
			continue;

		/*
		 * Call platform specific code for formatting memory
		 * information.
		 */
		print_us3_memory_line(bank->portid, bank->id, bank_size,
		    bank->bank_status, dimm_size, intlv, seg->id);
	}

	printf("\n");

	/*
	 * Sanity check to ensure that the total amount of system
	 * memory matches the total number of memory banks that
	 * we find here. Scream if there is a mis-match.
	 */
	total_sys_mem = (((uint64_t)sysconf(_SC_PAGESIZE) * \
	    (uint64_t)sysconf(_SC_PHYS_PAGES)) / MBYTE);

	if (total_bank_size != total_sys_mem) {
		log_printf(dgettext(TEXT_DOMAIN,
		    "\nError: total bank size [%lldMB] does not match total "
		    "system memory [%lldMB]\n"), total_bank_size,
		    total_sys_mem, 0);
	}

}

static void
add_seg_node(void)
{
	uint64_t	base;
	memory_seg_t	*new;
	static int	id = 0;
	memory_bank_t	*bank = bank_tail;

	if (bank->valid != 1)
		return;

	base = bank->um & ~(bank->uk);

	if ((new = match_seg(base)) == NULL) {
		/*
		 * This bank is part of a new segment, so create
		 * a struct for it and added to the list of segments
		 */
		if ((new = malloc(sizeof (memory_seg_t))) == NULL) {
			perror("malloc");
			exit(1);
		}
		new->id = id++;
		new->base = base;
		new->size = (((uint64_t)bank->uk +1) << 26);
		new->intlv = ((bank->lk ^ 0xF) + 1);

		/*
		 * add to the seg list
		 */
		new->next = seg_head;
		seg_head = new;
	}

	new->nbanks++;
	/*
	 * add bank into segs bank list.  Note we add at the head
	 */
	bank->seg_next = new->banks;
	new->banks = bank;
}

static memory_seg_t *
match_seg(uint64_t base)
{
	memory_seg_t	*cur_seg;

	for (cur_seg = seg_head; cur_seg; cur_seg = cur_seg->next) {
		if (cur_seg-> base == base)
			break;
	}
	return (cur_seg);
}

/*ARGSUSED0*/
void
print_us3_memory_line(int portid, int bank_id, uint64_t bank_size,
    char *bank_status, uint64_t dimm_size, uint32_t intlv, int seg_id)
{
	log_printf(dgettext(TEXT_DOMAIN,
	    "\n No print_us3_memory_line() function specified for"
	    " this platform\n"), 0);
}

int
display_us3_failed_banks(int system_failed)
{
	memory_bank_t	*bank;
	int		found_failed_bank = 0;

	if ((bank_head == NULL) || (seg_head == NULL)) {
		log_printf("\nCannot find any memory bank/segment info.\n");
		return (1);
	}

	for (bank = bank_head; bank; bank = bank->next) {
		/*
		 * check to see if the bank is invalid and also
		 * check if the bank_status is unpopulated.  Unpopulated
		 * means the bank is empty.
		 */

		if ((bank->valid == 0) &&
		    (strcmp(bank->bank_status, "unpopulated"))) {
			if (!system_failed && !found_failed_bank) {
				found_failed_bank = TRUE;
				log_printf("\n", 0);
				log_printf(dgettext(TEXT_DOMAIN,
				"Failed Field Replaceable Units (FRU) in "
				    "System:\n"), 0);
				log_printf("=========================="
				    "====================\n", 0);
			}
			/*
			 * Call platform specific code for formatting memory
			 * information.
			 */
			print_us3_failed_memory_line(bank->portid, bank->id,
			    bank->bank_status);
		}
	}
	if (found_failed_bank)
		return (1);
	else
		return (0);
}

/*ARGSUSED0*/
void
print_us3_failed_memory_line(int portid, int bank_id, char *bank_status)
{
	log_printf(dgettext(TEXT_DOMAIN,
	    "\n No print_us3_failed_memory_line() function specified for"
	    " this platform\n"), 0);
}



/* ============================================================================
 * SOURCE 3/7: minix4\exokernel\minix_drivers\storage\memory\memory.c
 * Size: 17,264 bytes, Lines: 600
 * Hash: abb37daf8ef9...
 * ============================================================================ */

/* This file contains the device dependent part of the drivers for the
 * following special files:
 *     /dev/ram		- RAM disk 
 *     /dev/mem		- absolute memory
 *     /dev/kmem	- kernel virtual memory
 *     /dev/null	- null device (data sink)
 *     /dev/boot	- boot device loaded from boot image 
 *     /dev/zero	- null byte stream generator
 *     /dev/imgrd	- boot image RAM disk
 *
 *  Changes:
 *	Apr 29, 2005	added null byte generator  (Jorrit N. Herder)
 *	Apr 09, 2005	added support for boot device  (Jorrit N. Herder)
 *	Jul 26, 2004	moved RAM driver to user-space  (Jorrit N. Herder)
 *	Apr 20, 1992	device dependent/independent split  (Kees J. Bot)
 */

#include <assert.h>
#include <minix/drivers.h>
#include <minix/chardriver.h>
#include <minix/blockdriver.h>
#include <sys/ioc_memory.h>
#include <minix/ds.h>
#include <minix/vm.h>
#include <machine/param.h>
#include <machine/vmparam.h>
#include <sys/mman.h>
#include "kernel/const.h"
#include "kernel/config.h"
#include "kernel/type.h"

#include <machine/vm.h>

#include "local.h"

/* ramdisks (/dev/ram*) */
#define RAMDISKS     6

#define RAM_DEV_LAST (RAM_DEV_FIRST+RAMDISKS-1)

#define NR_DEVS            (7+RAMDISKS)	/* number of minor devices */

static struct device m_geom[NR_DEVS];  /* base and size of each device */
static vir_bytes m_vaddrs[NR_DEVS];

static int openct[NR_DEVS];

static ssize_t m_char_read(devminor_t minor, u64_t position, endpoint_t endpt,
	cp_grant_id_t grant, size_t size, int flags, cdev_id_t id);
static ssize_t m_char_write(devminor_t minor, u64_t position, endpoint_t endpt,
	cp_grant_id_t grant, size_t size, int flags, cdev_id_t id);
static int m_char_open(devminor_t minor, int access, endpoint_t user_endpt);
static int m_char_close(devminor_t minor);

static struct device *m_block_part(devminor_t minor);
static ssize_t m_block_transfer(devminor_t minor, int do_write, u64_t position,
	endpoint_t endpt, iovec_t *iov, unsigned int nr_req, int flags);
static int m_block_open(devminor_t minor, int access);
static int m_block_close(devminor_t minor);
static int m_block_ioctl(devminor_t minor, unsigned long request, endpoint_t
	endpt, cp_grant_id_t grant, endpoint_t user_endpt);

/* Entry points to the CHARACTER part of this driver. */
static struct chardriver m_cdtab = {
  .cdr_open	= m_char_open,		/* open device */
  .cdr_close	= m_char_close,		/* close device */
  .cdr_read	= m_char_read,		/* read from device */
  .cdr_write	= m_char_write		/* write to device */
};

/* Entry points to the BLOCK part of this driver. */
static struct blockdriver m_bdtab = {
  .bdr_type	= BLOCKDRIVER_TYPE_DISK,/* handle partition requests */
  .bdr_open	= m_block_open,		/* open device */
  .bdr_close	= m_block_close,	/* nothing on a close */
  .bdr_transfer	= m_block_transfer,	/* do the I/O */
  .bdr_ioctl	= m_block_ioctl,	/* ram disk I/O control */
  .bdr_part	= m_block_part		/* return partition information */
};

/* SEF functions and variables. */
static void sef_local_startup(void);
static int sef_cb_init_fresh(int type, sef_init_info_t *info);

/*===========================================================================*
 *				   main 				     *
 *===========================================================================*/
int main(void)
{
  message msg;
  int r, ipc_status;

  /* SEF local startup. */
  sef_local_startup();

  /* The receive loop. */
  for (;;) {
	if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK)
		panic("memory: driver_receive failed (%d)", r);

	if (IS_BDEV_RQ(msg.m_type))
		blockdriver_process(&m_bdtab, &msg, ipc_status);
	else
		chardriver_process(&m_cdtab, &msg, ipc_status);
  }

  return(OK);
}

/*===========================================================================*
 *			       sef_local_startup			     *
 *===========================================================================*/
static void sef_local_startup()
{
  /* Register init callbacks. */
  sef_setcb_init_fresh(sef_cb_init_fresh);
  sef_setcb_init_restart(SEF_CB_INIT_RESTART_STATEFUL);

  /* Let SEF perform startup. */
  sef_startup();
}

/*===========================================================================*
 *		            sef_cb_init_fresh                                *
 *===========================================================================*/
static int sef_cb_init_fresh(int type, sef_init_info_t *UNUSED(info))
{
/* Initialize the memory driver. */
  int i;
#if 0
  struct kinfo kinfo;		/* kernel information */
  int s;

  if (OK != (s=sys_getkinfo(&kinfo))) {
      panic("Couldn't get kernel information: %d", s);
  }

  /* Map in kernel memory for /dev/kmem. */
  m_geom[KMEM_DEV].dv_base = kinfo.kmem_base;
  m_geom[KMEM_DEV].dv_size = kinfo.kmem_size;
  if((m_vaddrs[KMEM_DEV] = vm_map_phys(SELF, (void *) kinfo.kmem_base,
	kinfo.kmem_size)) == MAP_FAILED) {
	printf("MEM: Couldn't map in /dev/kmem.");
  }
#endif

  /* Ramdisk image built into the memory driver */
  m_geom[IMGRD_DEV].dv_base= 0;
  m_geom[IMGRD_DEV].dv_size= imgrd_size;
  m_vaddrs[IMGRD_DEV] = (vir_bytes) imgrd;

  for(i = 0; i < NR_DEVS; i++)
	openct[i] = 0;

  /* Set up memory range for /dev/mem. */
  m_geom[MEM_DEV].dv_base = 0;
  m_geom[MEM_DEV].dv_size = 0xffffffffULL;

  m_vaddrs[MEM_DEV] = (vir_bytes) MAP_FAILED; /* we are not mapping this in. */

  chardriver_announce();
  blockdriver_announce(type);

  return(OK);
}

/*===========================================================================*
 *				m_is_block				     *
 *===========================================================================*/
static int m_is_block(devminor_t minor)
{
/* Return TRUE iff the given minor device number is for a block device. */

  switch (minor) {
  case MEM_DEV:
  case KMEM_DEV:
  case NULL_DEV:
  case ZERO_DEV:
	return FALSE;

  default:
	return TRUE;
  }
}

/*===========================================================================*
 *				m_transfer_kmem				     *
 *===========================================================================*/
static ssize_t m_transfer_kmem(devminor_t minor, int do_write, u64_t position,
	endpoint_t endpt, cp_grant_id_t grant, size_t size)
{
/* Transfer from or to the KMEM device. */
  u64_t dv_size, dev_vaddr;
  int r;

  dv_size = m_geom[minor].dv_size;
  dev_vaddr = m_vaddrs[minor];

  if (!dev_vaddr || dev_vaddr == (vir_bytes) MAP_FAILED) {
	printf("MEM: dev %d not initialized\n", minor);
	return EIO;
  }

  if (position >= dv_size) return 0;	/* check for EOF */
  if (position + size > dv_size) size = dv_size - position;

  if (!do_write)			/* copy actual data */
	r = sys_safecopyto(endpt, grant, 0, dev_vaddr + position, size);
  else
	r = sys_safecopyfrom(endpt, grant, 0, dev_vaddr + position, size);

  return (r != OK) ? r : size;
}

/*===========================================================================*
 *				m_transfer_mem				     *
 *===========================================================================*/
static ssize_t m_transfer_mem(devminor_t minor, int do_write, u64_t position,
	endpoint_t endpt, cp_grant_id_t grant, size_t size)
{
/* Transfer from or to the MEM device. */
  static int any_mapped = 0;
  static phys_bytes pagestart_mapped;
  static char *vaddr;
  phys_bytes mem_phys, pagestart;
  size_t off, page_off, subcount;
  u64_t dv_size;
  int r;

  dv_size = m_geom[minor].dv_size;
  if (position >= dv_size) return 0;	/* check for EOF */
  if (position + size > dv_size) size = dv_size - position;

  /* Physical copying. Only used to access entire memory.
   * Transfer one 'page window' at a time.
   */
  off = 0;
  while (off < size) {
	mem_phys = (phys_bytes) position;

	page_off = (size_t) (mem_phys % PAGE_SIZE);
	pagestart = mem_phys - page_off;

	/* All memory to the map call has to be page-aligned.
	 * Don't have to map same page over and over.
	 */
	if (!any_mapped || pagestart_mapped != pagestart) {
		if (any_mapped) {
			if (vm_unmap_phys(SELF, vaddr, PAGE_SIZE) != OK)
				panic("vm_unmap_phys failed");
			any_mapped = 0;
		}

		vaddr = vm_map_phys(SELF, (void *) pagestart, PAGE_SIZE);
		if (vaddr == MAP_FAILED) {
			printf("memory: vm_map_phys failed\n");
			return ENOMEM;
		}
		any_mapped = 1;
		pagestart_mapped = pagestart;
	}

	/* how much to be done within this page. */
	subcount = PAGE_SIZE - page_off;
	if (subcount > size)
		subcount = size;

	if (!do_write)	/* copy data */
		r = sys_safecopyto(endpt, grant, off,
			(vir_bytes) vaddr + page_off, subcount);
	else
		r = sys_safecopyfrom(endpt, grant, off,
			(vir_bytes) vaddr + page_off, subcount);
	if (r != OK)
		return r;

	position += subcount;
	off += subcount;
  }

  return off;
}

/*===========================================================================*
 *				m_char_read				     *
 *===========================================================================*/
static ssize_t m_char_read(devminor_t minor, u64_t position, endpoint_t endpt,
	cp_grant_id_t grant, size_t size, int UNUSED(flags),
	cdev_id_t UNUSED(id))
{
/* Read from one of the driver's character devices. */
  ssize_t r;

  /* Check if the minor device number is ok. */
  if (minor < 0 || minor >= NR_DEVS || m_is_block(minor)) return ENXIO;

  switch (minor) {
  case NULL_DEV:
	r = 0;	/* always at EOF */
	break;

  case ZERO_DEV:
	/* Fill the target area with zeroes. In fact, let the kernel do it! */
	if ((r = sys_safememset(endpt, grant, 0, '\0', size)) == OK)
		r = size;
	break;

  case KMEM_DEV:
	r = m_transfer_kmem(minor, FALSE, position, endpt, grant, size);
	break;

  case MEM_DEV:
	r = m_transfer_mem(minor, FALSE, position, endpt, grant, size);
	break;

  default:
	panic("unknown character device %d", minor);
  }

  return r;
}

/*===========================================================================*
 *				m_char_write				     *
 *===========================================================================*/
static ssize_t m_char_write(devminor_t minor, u64_t position, endpoint_t endpt,
	cp_grant_id_t grant, size_t size, int UNUSED(flags),
	cdev_id_t UNUSED(id))
{
/* Write to one of the driver's character devices. */
  ssize_t r;

  /* Check if the minor device number is ok. */
  if (minor < 0 || minor >= NR_DEVS || m_is_block(minor)) return ENXIO;

  switch (minor) {
  case NULL_DEV:
  case ZERO_DEV:
	r = size;	/* just eat everything */
	break;

  case KMEM_DEV:
	r = m_transfer_kmem(minor, TRUE, position, endpt, grant, size);
	break;

  case MEM_DEV:
	r = m_transfer_mem(minor, TRUE, position, endpt, grant, size);
	break;

  default:
	panic("unknown character device %d", minor);
  }

  return r;
}

/*===========================================================================*
 *				m_char_open				     *
 *===========================================================================*/
static int m_char_open(devminor_t minor, int access, endpoint_t user_endpt)
{
/* Open a memory character device. */

  /* Check if the minor device number is ok. */
  if (minor < 0 || minor >= NR_DEVS || m_is_block(minor)) return ENXIO;

#if defined(__i386__)
  if (minor == MEM_DEV)
  {
	int r = sys_enable_iop(user_endpt);
	if (r != OK)
	{
		printf("m_char_open: sys_enable_iop failed for %d: %d\n",
			user_endpt, r);
		return r;
	}
  }
#endif

  openct[minor]++;

  return(OK);
}

/*===========================================================================*
 *				m_char_close				     *
 *===========================================================================*/
static int m_char_close(devminor_t minor)
{
/* Close a memory character device. */

  if (minor < 0 || minor >= NR_DEVS || m_is_block(minor)) return ENXIO;

  if(openct[minor] < 1) {
	printf("MEMORY: closing unopened device %d\n", minor);
	return(EINVAL);
  }
  openct[minor]--;

  return(OK);
}

/*===========================================================================*
 *				m_block_part				     *
 *===========================================================================*/
static struct device *m_block_part(devminor_t minor)
{
/* Prepare for I/O on a device: check if the minor device number is ok. */
  if (minor < 0 || minor >= NR_DEVS || !m_is_block(minor)) return(NULL);

  return(&m_geom[minor]);
}

/*===========================================================================*
 *				m_block_transfer			     *
 *===========================================================================*/
static int m_block_transfer(
  devminor_t minor,		/* minor device number */
  int do_write,			/* read or write? */
  u64_t position,		/* offset on device to read or write */
  endpoint_t endpt,		/* process doing the request */
  iovec_t *iov,			/* pointer to read or write request vector */
  unsigned int nr_req,		/* length of request vector */
  int UNUSED(flags)		/* transfer flags */
)
{
/* Read or write one the driver's block devices. */
  unsigned count;
  vir_bytes vir_offset = 0;
  struct device *dv;
  u64_t dv_size;
  int r;
  vir_bytes dev_vaddr;
  cp_grant_id_t grant;
  ssize_t total = 0;

  /* Get minor device information. */
  if ((dv = m_block_part(minor)) == NULL) return(ENXIO);
  dv_size = dv->dv_size;
  dev_vaddr = m_vaddrs[minor];

  if (ex64hi(position) != 0)
	return OK;	/* Beyond EOF */

  while (nr_req > 0) {

	/* How much to transfer and where to / from. */
	count = iov->iov_size;
	grant = (cp_grant_id_t) iov->iov_addr;

	/* Virtual copying. For RAM disks and internal FS. */
	if(!dev_vaddr || dev_vaddr == (vir_bytes) MAP_FAILED) {
		printf("MEM: dev %d not initialized\n", minor);
		return EIO;
	}
	if (position >= dv_size) return(total);	/* check for EOF */
	if (position + count > dv_size) count = dv_size - position;
	if (!do_write) {	/* copy actual data */
	        r=sys_safecopyto(endpt, grant, vir_offset,
		  dev_vaddr + position, count);
	} else {
	        r=sys_safecopyfrom(endpt, grant, vir_offset,
		  dev_vaddr + position, count);
	}
	if(r != OK) {
		panic("I/O copy failed: %d", r);
	}

	/* Book the number of bytes transferred. */
	position += count;
	vir_offset += count;
	total += count;
	if ((iov->iov_size -= count) == 0) { iov++; nr_req--; vir_offset = 0; }

  }
  return(total);
}

/*===========================================================================*
 *				m_block_open				     *
 *===========================================================================*/
static int m_block_open(devminor_t minor, int UNUSED(access))
{
/* Open a memory block device. */
  if (m_block_part(minor) == NULL) return(ENXIO);

  openct[minor]++;

  return(OK);
}

/*===========================================================================*
 *				m_block_close				     *
 *===========================================================================*/
static int m_block_close(devminor_t minor)
{
/* Close a memory block device. */
  if (m_block_part(minor) == NULL) return(ENXIO);

  if(openct[minor] < 1) {
	printf("MEMORY: closing unopened device %d\n", minor);
	return(EINVAL);
  }
  openct[minor]--;

  return(OK);
}

/*===========================================================================*
 *				m_block_ioctl				     *
 *===========================================================================*/
static int m_block_ioctl(devminor_t minor, unsigned long request,
	endpoint_t endpt, cp_grant_id_t grant, endpoint_t UNUSED(user_endpt))
{
/* I/O controls for the block devices of the memory driver. Currently there is
 * one I/O control specific to the memory driver:
 * - MIOCRAMSIZE: to set the size of the RAM disk.
 */
  struct device *dv;
  u32_t ramdev_size;
  int s;
  void *mem;
  int is_imgrd = 0;

  if (request != MIOCRAMSIZE)
	return EINVAL;

  if(minor == IMGRD_DEV) 
	is_imgrd = 1;

  /* Someone wants to create a new RAM disk with the given size.
   * A ramdisk can be created only once, and only on RAM disk device.
   */
  if ((dv = m_block_part(minor)) == NULL) return ENXIO;
  if((minor < RAM_DEV_FIRST || minor > RAM_DEV_LAST) &&
  	minor != RAM_DEV_OLD && !is_imgrd) {
	printf("MEM: MIOCRAMSIZE: %d not a ramdisk\n", minor);
	return EINVAL;
  }

  /* Get request structure */
  s= sys_safecopyfrom(endpt, grant, 0, (vir_bytes)&ramdev_size,
	sizeof(ramdev_size));
  if (s != OK)
	return s;
  if(is_imgrd)
  	ramdev_size = 0;
  if(m_vaddrs[minor] && dv->dv_size == (u64_t) ramdev_size) {
	return(OK);
  }
  /* openct is 1 for the ioctl(). */
  if(openct[minor] != 1) {
	printf("MEM: MIOCRAMSIZE: %d in use (count %d)\n",
		minor, openct[minor]);
	return(EBUSY);
  }
  if(m_vaddrs[minor]) {
	u32_t a, o;
	u64_t size;
	int r;
	if(ex64hi(dv->dv_size)) {
		panic("huge old ramdisk");
	}
	size = dv->dv_size;
	a = m_vaddrs[minor];
	if((o = a % PAGE_SIZE)) {
		vir_bytes l = PAGE_SIZE - o;
		a += l;
		size -= l;
	}
	size = rounddown(size, PAGE_SIZE);
	r = munmap((void *) a, size);
	if(r != OK) {
		printf("memory: WARNING: munmap failed: %d\n", r);
	}
	m_vaddrs[minor] = (vir_bytes) NULL;
	dv->dv_size = 0;
  }

#if DEBUG
  printf("MEM:%d: allocating ramdisk of size 0x%x\n", minor, ramdev_size);
#endif

  mem = NULL;

  /* Try to allocate a piece of memory for the RAM disk. */
  if(ramdev_size > 0 &&
  	(mem = mmap(NULL, ramdev_size, PROT_READ|PROT_WRITE,
		MAP_PREALLOC|MAP_ANON, -1, 0)) == MAP_FAILED) {
	printf("MEM: failed to get memory for ramdisk\n");
	return(ENOMEM);
  }

  m_vaddrs[minor] = (vir_bytes) mem;

  dv->dv_size = ramdev_size;

  return(OK);
}



/* ============================================================================
 * SOURCE 4/7: minix4\exokernel\kernel_legacy\tools\ctf\common\memory.c
 * Size: 1,872 bytes, Lines: 101
 * Hash: c1d26a43a655...
 * ============================================================================ */

/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright 2001-2002 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*
 * Routines for memory management
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

static void
memory_bailout(void)
{
	(void) fprintf(stderr, "Out of memory\n");
	exit(1);
}

void *
xmalloc(size_t size)
{
	void *mem;

	if ((mem = malloc(size)) == NULL)
		memory_bailout();

	return (mem);
}

void *
xcalloc(size_t size)
{
	void *mem;

	mem = xmalloc(size);
	bzero(mem, size);

	return (mem);
}

char *
xstrdup(const char *str)
{
	char *newstr;

	if ((newstr = strdup(str)) == NULL)
		memory_bailout();

	return (newstr);
}

char *
xstrndup(char *str, size_t len)
{
	char *newstr;

	if ((newstr = malloc(len + 1)) == NULL)
		memory_bailout();

	(void) strncpy(newstr, str, len);
	newstr[len] = '\0';

	return (newstr);
}

void *
xrealloc(void *ptr, size_t size)
{
	void *mem;

	if ((mem = realloc(ptr, size)) == NULL)
		memory_bailout();

	return (mem);
}



/* ============================================================================
 * SOURCE 5/7: minix4\exokernel\kernel_legacy\cmd\mdb\common\modules\genunix\memory.c
 * Size: 43,911 bytes, Lines: 1,709
 * Hash: c383bfe9a539...
 * ============================================================================ */

/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright (c) 2001, 2010, Oracle and/or its affiliates. All rights reserved.
 * Copyright 2019 Joyent, Inc.
 * Copyright 2025 Oxide Computer Company
 */

#include <mdb/mdb_param.h>
#include <mdb/mdb_modapi.h>
#include <mdb/mdb_ks.h>
#include <sys/types.h>
#include <sys/memlist.h>
#include <sys/swap.h>
#include <sys/systm.h>
#include <sys/thread.h>
#include <vm/anon.h>
#include <vm/as.h>
#include <vm/page.h>
#include <sys/thread.h>
#include <sys/swap.h>
#include <sys/memlist.h>
#include <sys/vnode.h>
#include <vm/seg_map.h>
#include <vm/seg_vn.h>
#include <vm/seg_hole.h>
#if defined(__i386) || defined(__amd64)
#include <sys/balloon_impl.h>
#endif

#include "avl.h"
#include "memory.h"

/*
 * Page walker.
 * By default, this will walk all pages in the system.  If given an
 * address, it will walk all pages belonging to the vnode at that
 * address.
 */

/*
 * page_walk_data
 *
 * pw_hashleft is set to -1 when walking a vnode's pages, and holds the
 * number of hash locations remaining in the page hash table when
 * walking all pages.
 *
 * The astute reader will notice that pw_hashloc is only used when
 * reading all pages (to hold a pointer to our location in the page
 * hash table), and that pw_first is only used when reading the pages
 * belonging to a particular vnode (to hold a pointer to the first
 * page).  While these could be combined to be a single pointer, they
 * are left separate for clarity.
 */
typedef struct page_walk_data {
	long		pw_hashleft;
	void		**pw_hashloc;
	uintptr_t	pw_first;
} page_walk_data_t;

int
page_walk_init(mdb_walk_state_t *wsp)
{
	page_walk_data_t	*pwd;
	void	**ptr;
	size_t	hashsz;
	vnode_t	vn;

	if (wsp->walk_addr == 0) {

		/*
		 * Walk all pages
		 */

		if ((mdb_readvar(&ptr, "page_hash") == -1) ||
		    (mdb_readvar(&hashsz, "page_hashsz") == -1) ||
		    (ptr == NULL) || (hashsz == 0)) {
			mdb_warn("page_hash, page_hashsz not found or invalid");
			return (WALK_ERR);
		}

		/*
		 * Since we are walking all pages, initialize hashleft
		 * to be the remaining number of entries in the page
		 * hash.  hashloc is set the start of the page hash
		 * table.  Setting the walk address to 0 indicates that
		 * we aren't currently following a hash chain, and that
		 * we need to scan the page hash table for a page.
		 */
		pwd = mdb_alloc(sizeof (page_walk_data_t), UM_SLEEP);
		pwd->pw_hashleft = hashsz;
		pwd->pw_hashloc = ptr;
		wsp->walk_addr = 0;
	} else {

		/*
		 * Walk just this vnode
		 */

		if (mdb_vread(&vn, sizeof (vnode_t), wsp->walk_addr) == -1) {
			mdb_warn("unable to read vnode_t at %#lx",
			    wsp->walk_addr);
			return (WALK_ERR);
		}

		/*
		 * We set hashleft to -1 to indicate that we are
		 * walking a vnode, and initialize first to 0 (it is
		 * used to terminate the walk, so it must not be set
		 * until after we have walked the first page).  The
		 * walk address is set to the first page.
		 */
		pwd = mdb_alloc(sizeof (page_walk_data_t), UM_SLEEP);
		pwd->pw_hashleft = -1;
		pwd->pw_first = 0;

		wsp->walk_addr = (uintptr_t)vn.v_pages;
	}

	wsp->walk_data = pwd;

	return (WALK_NEXT);
}

int
page_walk_step(mdb_walk_state_t *wsp)
{
	page_walk_data_t	*pwd = wsp->walk_data;
	page_t		page;
	uintptr_t	pp;

	pp = wsp->walk_addr;

	if (pwd->pw_hashleft < 0) {

		/* We're walking a vnode's pages */

		/*
		 * If we don't have any pages to walk, we have come
		 * back around to the first one (we finished), or we
		 * can't read the page we're looking at, we are done.
		 */
		if (pp == 0 || pp == pwd->pw_first)
			return (WALK_DONE);
		if (mdb_vread(&page, sizeof (page_t), pp) == -1) {
			mdb_warn("unable to read page_t at %#lx", pp);
			return (WALK_ERR);
		}

		/*
		 * Set the walk address to the next page, and if the
		 * first page hasn't been set yet (i.e. we are on the
		 * first page), set it.
		 */
		wsp->walk_addr = (uintptr_t)page.p_vpnext;
		if (pwd->pw_first == 0)
			pwd->pw_first = pp;

	} else if (pwd->pw_hashleft > 0) {

		/* We're walking all pages */

		/*
		 * If pp (the walk address) is NULL, we scan through
		 * the page hash table until we find a page.
		 */
		if (pp == 0) {

			/*
			 * Iterate through the page hash table until we
			 * find a page or reach the end.
			 */
			do {
				if (mdb_vread(&pp, sizeof (uintptr_t),
				    (uintptr_t)pwd->pw_hashloc) == -1) {
					mdb_warn("unable to read from %#p",
					    pwd->pw_hashloc);
					return (WALK_ERR);
				}
				pwd->pw_hashleft--;
				pwd->pw_hashloc++;
			} while (pwd->pw_hashleft && (pp == 0));

			/*
			 * We've reached the end; exit.
			 */
			if (pp == 0)
				return (WALK_DONE);
		}

		if (mdb_vread(&page, sizeof (page_t), pp) == -1) {
			mdb_warn("unable to read page_t at %#lx", pp);
			return (WALK_ERR);
		}

		/*
		 * Set the walk address to the next page.
		 */
		wsp->walk_addr = (uintptr_t)page.p_hash;

	} else {
		/* We've finished walking all pages. */
		return (WALK_DONE);
	}

	return (wsp->walk_callback(pp, &page, wsp->walk_cbdata));
}

void
page_walk_fini(mdb_walk_state_t *wsp)
{
	mdb_free(wsp->walk_data, sizeof (page_walk_data_t));
}

/*
 * allpages walks all pages in the system in order they appear in
 * the memseg structure
 */

#define	PAGE_BUFFER	128

int
allpages_walk_init(mdb_walk_state_t *wsp)
{
	if (wsp->walk_addr != 0) {
		mdb_warn("allpages only supports global walks.\n");
		return (WALK_ERR);
	}

	if (mdb_layered_walk("memseg", wsp) == -1) {
		mdb_warn("couldn't walk 'memseg'");
		return (WALK_ERR);
	}

	wsp->walk_data = mdb_alloc(sizeof (page_t) * PAGE_BUFFER, UM_SLEEP);
	return (WALK_NEXT);
}

int
allpages_walk_step(mdb_walk_state_t *wsp)
{
	const struct memseg *msp = wsp->walk_layer;
	page_t *buf = wsp->walk_data;
	size_t pg_read, i;
	size_t pg_num = msp->pages_end - msp->pages_base;
	const page_t *pg_addr = msp->pages;

	while (pg_num > 0) {
		pg_read = MIN(pg_num, PAGE_BUFFER);

		if (mdb_vread(buf, pg_read * sizeof (page_t),
		    (uintptr_t)pg_addr) == -1) {
			mdb_warn("can't read page_t's at %#lx", pg_addr);
			return (WALK_ERR);
		}
		for (i = 0; i < pg_read; i++) {
			int ret = wsp->walk_callback((uintptr_t)&pg_addr[i],
			    &buf[i], wsp->walk_cbdata);

			if (ret != WALK_NEXT)
				return (ret);
		}
		pg_num -= pg_read;
		pg_addr += pg_read;
	}

	return (WALK_NEXT);
}

void
allpages_walk_fini(mdb_walk_state_t *wsp)
{
	mdb_free(wsp->walk_data, sizeof (page_t) * PAGE_BUFFER);
}

/*
 * Hash table + LRU queue.
 * This table is used to cache recently read vnodes for the memstat
 * command, to reduce the number of mdb_vread calls.  This greatly
 * speeds the memstat command on on live, large CPU count systems.
 */

#define	VN_SMALL	401
#define	VN_LARGE	10007
#define	VN_HTABLE_KEY(p, hp)	((p) % ((hp)->vn_htable_buckets))

struct vn_htable_list {
	uint_t vn_flag;				/* v_flag from vnode	*/
	uintptr_t vn_ptr;			/* pointer to vnode	*/
	struct vn_htable_list *vn_q_next;	/* queue next pointer	*/
	struct vn_htable_list *vn_q_prev;	/* queue prev pointer	*/
	struct vn_htable_list *vn_h_next;	/* hash table pointer	*/
};

/*
 * vn_q_first        -> points to to head of queue: the vnode that was most
 *                      recently used
 * vn_q_last         -> points to the oldest used vnode, and is freed once a new
 *                      vnode is read.
 * vn_htable         -> hash table
 * vn_htable_buf     -> contains htable objects
 * vn_htable_size    -> total number of items in the hash table
 * vn_htable_buckets -> number of buckets in the hash table
 */
typedef struct vn_htable {
	struct vn_htable_list  *vn_q_first;
	struct vn_htable_list  *vn_q_last;
	struct vn_htable_list **vn_htable;
	struct vn_htable_list  *vn_htable_buf;
	int vn_htable_size;
	int vn_htable_buckets;
} vn_htable_t;


/* allocate memory, initilize hash table and LRU queue */
static void
vn_htable_init(vn_htable_t *hp, size_t vn_size)
{
	int i;
	int htable_size = MAX(vn_size, VN_LARGE);

	if ((hp->vn_htable_buf = mdb_zalloc(sizeof (struct vn_htable_list)
	    * htable_size, UM_NOSLEEP|UM_GC)) == NULL) {
		htable_size = VN_SMALL;
		hp->vn_htable_buf = mdb_zalloc(sizeof (struct vn_htable_list)
		    * htable_size, UM_SLEEP|UM_GC);
	}

	hp->vn_htable = mdb_zalloc(sizeof (struct vn_htable_list *)
	    * htable_size, UM_SLEEP|UM_GC);

	hp->vn_q_first  = &hp->vn_htable_buf[0];
	hp->vn_q_last   = &hp->vn_htable_buf[htable_size - 1];
	hp->vn_q_first->vn_q_next = &hp->vn_htable_buf[1];
	hp->vn_q_last->vn_q_prev = &hp->vn_htable_buf[htable_size - 2];

	for (i = 1; i < (htable_size-1); i++) {
		hp->vn_htable_buf[i].vn_q_next = &hp->vn_htable_buf[i + 1];
		hp->vn_htable_buf[i].vn_q_prev = &hp->vn_htable_buf[i - 1];
	}

	hp->vn_htable_size = htable_size;
	hp->vn_htable_buckets = htable_size;
}


/*
 * Find the vnode whose address is ptr, and return its v_flag in vp->v_flag.
 * The function tries to find needed information in the following order:
 *
 * 1. check if ptr is the first in queue
 * 2. check if ptr is in hash table (if so move it to the top of queue)
 * 3. do mdb_vread, remove last queue item from queue and hash table.
 *    Insert new information to freed object, and put this object in to the
 *    top of the queue.
 */
static int
vn_get(vn_htable_t *hp, struct vnode *vp, uintptr_t ptr)
{
	int hkey;
	struct vn_htable_list *hent, **htmp, *q_next, *q_prev;
	struct vn_htable_list  *q_first = hp->vn_q_first;

	/* 1. vnode ptr is the first in queue, just get v_flag and return */
	if (q_first->vn_ptr == ptr) {
		vp->v_flag = q_first->vn_flag;

		return (0);
	}

	/* 2. search the hash table for this ptr */
	hkey = VN_HTABLE_KEY(ptr, hp);
	hent = hp->vn_htable[hkey];
	while (hent && (hent->vn_ptr != ptr))
		hent = hent->vn_h_next;

	/* 3. if hent is NULL, we did not find in hash table, do mdb_vread */
	if (hent == NULL) {
		struct vnode vn;

		if (mdb_vread(&vn, sizeof (vnode_t), ptr) == -1) {
			mdb_warn("unable to read vnode_t at %#lx", ptr);
			return (-1);
		}

		/* we will insert read data into the last element in queue */
		hent = hp->vn_q_last;

		/* remove last hp->vn_q_last object from hash table */
		if (hent->vn_ptr) {
			htmp = &hp->vn_htable[VN_HTABLE_KEY(hent->vn_ptr, hp)];
			while (*htmp != hent)
				htmp = &(*htmp)->vn_h_next;
			*htmp = hent->vn_h_next;
		}

		/* insert data into new free object */
		hent->vn_ptr  = ptr;
		hent->vn_flag = vn.v_flag;

		/* insert new object into hash table */
		hent->vn_h_next = hp->vn_htable[hkey];
		hp->vn_htable[hkey] = hent;
	}

	/* Remove from queue. hent is not first, vn_q_prev is not NULL */
	q_next = hent->vn_q_next;
	q_prev = hent->vn_q_prev;
	if (q_next == NULL)
		hp->vn_q_last = q_prev;
	else
		q_next->vn_q_prev = q_prev;
	q_prev->vn_q_next = q_next;

	/* Add to the front of queue */
	hent->vn_q_prev = NULL;
	hent->vn_q_next = q_first;
	q_first->vn_q_prev = hent;
	hp->vn_q_first = hent;

	/* Set v_flag in vnode pointer from hent */
	vp->v_flag = hent->vn_flag;

	return (0);
}

/* Summary statistics of pages */
typedef struct memstat {
	struct vnode    *ms_unused_vp;	/* Unused pages vnode pointer	  */
	struct vnode    *ms_kvps;	/* Cached address of vnode array  */
	uint64_t	ms_kmem;	/* Pages of kernel memory	  */
	uint64_t	ms_zfs_data;	/* Pages of zfs data		  */
	uint64_t	ms_vmm_mem;	/* Pages of VMM mem		  */
	uint64_t	ms_anon;	/* Pages of anonymous memory	  */
	uint64_t	ms_vnode;	/* Pages of named (vnode) memory  */
	uint64_t	ms_exec;	/* Pages of exec/library memory	  */
	uint64_t	ms_cachelist;	/* Pages on the cachelist (free)  */
	uint64_t	ms_bootpages;	/* Pages on the bootpages list    */
	uint64_t	ms_total;	/* Pages on page hash		  */
	vn_htable_t	*ms_vn_htable;	/* Pointer to hash table	  */
	struct vnode	ms_vn;		/* vnode buffer			  */
} memstat_t;

#define	MS_PP_ISTYPE(pp, stats, index) \
	((pp)->p_vnode == &(stats->ms_kvps[index]))

/*
 * Summarize pages by type and update stat information
 */

/* ARGSUSED */
static int
memstat_callback(page_t *page, page_t *pp, memstat_t *stats)
{
	struct vnode *vp = &stats->ms_vn;

	if (PP_ISBOOTPAGES(pp))
		stats->ms_bootpages++;
	else if (pp->p_vnode == NULL || pp->p_vnode == stats->ms_unused_vp)
		return (WALK_NEXT);
	else if (MS_PP_ISTYPE(pp, stats, KV_KVP))
		stats->ms_kmem++;
	else if (MS_PP_ISTYPE(pp, stats, KV_ZVP))
		stats->ms_zfs_data++;
	else if (MS_PP_ISTYPE(pp, stats, KV_VVP))
		stats->ms_vmm_mem++;
	else if (PP_ISFREE(pp))
		stats->ms_cachelist++;
	else if (vn_get(stats->ms_vn_htable, vp, (uintptr_t)pp->p_vnode))
		return (WALK_ERR);
	else if (IS_SWAPFSVP(vp))
		stats->ms_anon++;
	else if ((vp->v_flag & VVMEXEC) != 0)
		stats->ms_exec++;
	else
		stats->ms_vnode++;

	stats->ms_total++;

	return (WALK_NEXT);
}

/* ARGSUSED */
int
memstat(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv)
{
	pgcnt_t total_pages, physmem;
	ulong_t freemem;
	memstat_t stats;
	GElf_Sym sym;
	vn_htable_t ht;
	uintptr_t vn_size = 0;
#if defined(__i386) || defined(__amd64)
	bln_stats_t bln_stats;
	ssize_t bln_size;
#endif

	bzero(&stats, sizeof (memstat_t));

	/*
	 * -s size, is an internal option. It specifies the size of vn_htable.
	 * Hash table size is set in the following order:
	 * If user has specified the size that is larger than VN_LARGE: try it,
	 * but if malloc failed default to VN_SMALL. Otherwise try VN_LARGE, if
	 * failed to allocate default to VN_SMALL.
	 * For a better efficiency of hash table it is highly recommended to
	 * set size to a prime number.
	 */
	if ((flags & DCMD_ADDRSPEC) || mdb_getopts(argc, argv,
	    's', MDB_OPT_UINTPTR, &vn_size, NULL) != argc)
		return (DCMD_USAGE);

	/* Initialize vnode hash list and queue */
	vn_htable_init(&ht, vn_size);
	stats.ms_vn_htable = &ht;

	/* Total physical memory */
	if (mdb_readvar(&total_pages, "total_pages") == -1) {
		mdb_warn("unable to read total_pages");
		return (DCMD_ERR);
	}

	/* Artificially limited memory */
	if (mdb_readvar(&physmem, "physmem") == -1) {
		mdb_warn("unable to read physmem");
		return (DCMD_ERR);
	}

	/* read kernel vnode array pointer */
	if (mdb_lookup_by_obj(MDB_OBJ_EXEC, "kvps",
	    (GElf_Sym *)&sym) == -1) {
		mdb_warn("unable to look up kvps");
		return (DCMD_ERR);
	}
	stats.ms_kvps = (struct vnode *)(uintptr_t)sym.st_value;

	/*
	 * If physmem != total_pages, then the administrator has limited the
	 * number of pages available in the system.  Excluded pages are
	 * associated with the unused pages vnode.  Read this vnode so the
	 * pages can be excluded in the page accounting.
	 */
	if (mdb_lookup_by_obj(MDB_OBJ_EXEC, "unused_pages_vp",
	    (GElf_Sym *)&sym) == -1) {
		mdb_warn("unable to read unused_pages_vp");
		return (DCMD_ERR);
	}
	stats.ms_unused_vp = (struct vnode *)(uintptr_t)sym.st_value;

	/* walk all pages, collect statistics */
	if (mdb_walk("allpages", (mdb_walk_cb_t)(uintptr_t)memstat_callback,
	    &stats) == -1) {
		mdb_warn("can't walk memseg");
		return (DCMD_ERR);
	}

#define	MS_PCT_TOTAL(x)	((ulong_t)((((5 * total_pages) + ((x) * 1000ull))) / \
		((physmem) * 10)))

	mdb_printf("Page Summary                Pages                MB"
	    "  %%Tot\n");
	mdb_printf("------------     ----------------  ----------------"
	    "  ----\n");
	mdb_printf("Kernel           %16llu  %16llu  %3lu%%\n",
	    stats.ms_kmem,
	    (uint64_t)stats.ms_kmem * PAGESIZE / (1024 * 1024),
	    MS_PCT_TOTAL(stats.ms_kmem));

	if (stats.ms_bootpages != 0) {
		mdb_printf("Boot pages       %16llu  %16llu  %3lu%%\n",
		    stats.ms_bootpages,
		    (uint64_t)stats.ms_bootpages * PAGESIZE / (1024 * 1024),
		    MS_PCT_TOTAL(stats.ms_bootpages));
	}

	if (stats.ms_zfs_data != 0) {
		mdb_printf("ZFS File Data    %16llu  %16llu  %3lu%%\n",
		    stats.ms_zfs_data,
		    (uint64_t)stats.ms_zfs_data * PAGESIZE / (1024 * 1024),
		    MS_PCT_TOTAL(stats.ms_zfs_data));
	}

	if (stats.ms_vmm_mem != 0) {
		mdb_printf("VMM Memory       %16llu  %16llu  %3lu%%\n",
		    stats.ms_vmm_mem,
		    (uint64_t)stats.ms_vmm_mem * PAGESIZE / (1024 * 1024),
		    MS_PCT_TOTAL(stats.ms_vmm_mem));
	}

	mdb_printf("Anon             %16llu  %16llu  %3lu%%\n",
	    stats.ms_anon,
	    (uint64_t)stats.ms_anon * PAGESIZE / (1024 * 1024),
	    MS_PCT_TOTAL(stats.ms_anon));
	mdb_printf("Exec and libs    %16llu  %16llu  %3lu%%\n",
	    stats.ms_exec,
	    (uint64_t)stats.ms_exec * PAGESIZE / (1024 * 1024),
	    MS_PCT_TOTAL(stats.ms_exec));
	mdb_printf("Page cache       %16llu  %16llu  %3lu%%\n",
	    stats.ms_vnode,
	    (uint64_t)stats.ms_vnode * PAGESIZE / (1024 * 1024),
	    MS_PCT_TOTAL(stats.ms_vnode));
	mdb_printf("Free (cachelist) %16llu  %16llu  %3lu%%\n",
	    stats.ms_cachelist,
	    (uint64_t)stats.ms_cachelist * PAGESIZE / (1024 * 1024),
	    MS_PCT_TOTAL(stats.ms_cachelist));

	/*
	 * occasionally, we double count pages above.  To avoid printing
	 * absurdly large values for freemem, we clamp it at zero.
	 */
	if (physmem > stats.ms_total)
		freemem = physmem - stats.ms_total;
	else
		freemem = 0;

#if defined(__i386) || defined(__amd64)
	/* Are we running under Xen?  If so, get balloon memory usage. */
	if ((bln_size = mdb_readvar(&bln_stats, "bln_stats")) != -1) {
		if (freemem > bln_stats.bln_hv_pages)
			freemem -= bln_stats.bln_hv_pages;
		else
			freemem = 0;
	}
#endif

	mdb_printf("Free (freelist)  %16lu  %16llu  %3lu%%\n", freemem,
	    (uint64_t)freemem * PAGESIZE / (1024 * 1024),
	    MS_PCT_TOTAL(freemem));

#if defined(__i386) || defined(__amd64)
	if (bln_size != -1) {
		mdb_printf("Balloon          %16lu  %16llu  %3lu%%\n",
		    bln_stats.bln_hv_pages,
		    (uint64_t)bln_stats.bln_hv_pages * PAGESIZE / (1024 * 1024),
		    MS_PCT_TOTAL(bln_stats.bln_hv_pages));
	}
#endif

	mdb_printf("\nTotal            %16lu  %16lu\n",
	    physmem,
	    (uint64_t)physmem * PAGESIZE / (1024 * 1024));

	if (physmem != total_pages) {
		mdb_printf("Physical         %16lu  %16lu\n",
		    total_pages,
		    (uint64_t)total_pages * PAGESIZE / (1024 * 1024));
	}

#undef MS_PCT_TOTAL

	return (DCMD_OK);
}

void
pagelookup_help(void)
{
	mdb_printf(
	    "Finds the page with name { %<b>vp%</b>, %<b>offset%</b> }.\n"
	    "\n"
	    "Can be invoked three different ways:\n\n"
	    "    ::pagelookup -v %<b>vp%</b> -o %<b>offset%</b>\n"
	    "    %<b>vp%</b>::pagelookup -o %<b>offset%</b>\n"
	    "    %<b>offset%</b>::pagelookup -v %<b>vp%</b>\n"
	    "\n"
	    "The latter two forms are useful in pipelines.\n");
}

int
pagelookup(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv)
{
	uintptr_t vp = -(uintptr_t)1;
	uint64_t offset = -(uint64_t)1;

	uintptr_t pageaddr;
	int hasaddr = (flags & DCMD_ADDRSPEC);
	int usedaddr = 0;

	if (mdb_getopts(argc, argv,
	    'v', MDB_OPT_UINTPTR, &vp,
	    'o', MDB_OPT_UINT64, &offset,
	    NULL) != argc) {
		return (DCMD_USAGE);
	}

	if (vp == -(uintptr_t)1) {
		if (offset == -(uint64_t)1) {
			mdb_warn(
			    "pagelookup: at least one of -v vp or -o offset "
			    "required.\n");
			return (DCMD_USAGE);
		}
		vp = addr;
		usedaddr = 1;
	} else if (offset == -(uint64_t)1) {
		offset = mdb_get_dot();
		usedaddr = 1;
	}
	if (usedaddr && !hasaddr) {
		mdb_warn("pagelookup: address required\n");
		return (DCMD_USAGE);
	}
	if (!usedaddr && hasaddr) {
		mdb_warn(
		    "pagelookup: address specified when both -v and -o were "
		    "passed");
		return (DCMD_USAGE);
	}

	pageaddr = mdb_page_lookup(vp, offset);
	if (pageaddr == 0) {
		mdb_warn("pagelookup: no page for {vp = %p, offset = %llp)\n",
		    vp, offset);
		return (DCMD_OK);
	}
	mdb_printf("%#lr\n", pageaddr);		/* this is PIPE_OUT friendly */
	return (DCMD_OK);
}

/*ARGSUSED*/
int
page_num2pp(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv)
{
	uintptr_t pp;

	if (argc != 0 || !(flags & DCMD_ADDRSPEC)) {
		return (DCMD_USAGE);
	}

	pp = mdb_pfn2page((pfn_t)addr);
	if (pp == 0) {
		return (DCMD_ERR);
	}

	if (flags & DCMD_PIPE_OUT) {
		mdb_printf("%#lr\n", pp);
	} else {
		mdb_printf("%lx has page_t at %#lx\n", (pfn_t)addr, pp);
	}

	return (DCMD_OK);
}

int
page(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv)
{
	page_t	p;

	if (!(flags & DCMD_ADDRSPEC)) {
		if (mdb_walk_dcmd("page", "page", argc, argv) == -1) {
			mdb_warn("can't walk pages");
			return (DCMD_ERR);
		}
		return (DCMD_OK);
	}

	if (DCMD_HDRSPEC(flags)) {
		mdb_printf("%<u>%?s %?s %16s %8s %3s %3s %2s %2s %2s%</u>\n",
		    "PAGE", "VNODE", "OFFSET", "SELOCK",
		    "LCT", "COW", "IO", "FS", "ST");
	}

	if (mdb_vread(&p, sizeof (page_t), addr) == -1) {
		mdb_warn("can't read page_t at %#lx", addr);
		return (DCMD_ERR);
	}

	mdb_printf("%0?lx %?p %16llx %8x %3d %3d %2x %2x %2x\n",
	    addr, p.p_vnode, p.p_offset, p.p_selock, p.p_lckcnt, p.p_cowcnt,
	    p.p_iolock_state, p.p_fsdata, p.p_state);

	return (DCMD_OK);
}

int
swap_walk_init(mdb_walk_state_t *wsp)
{
	void	*ptr;

	if ((mdb_readvar(&ptr, "swapinfo") == -1) || ptr == NULL) {
		mdb_warn("swapinfo not found or invalid");
		return (WALK_ERR);
	}

	wsp->walk_addr = (uintptr_t)ptr;

	return (WALK_NEXT);
}

int
swap_walk_step(mdb_walk_state_t *wsp)
{
	uintptr_t	sip;
	struct swapinfo	si;

	sip = wsp->walk_addr;

	if (sip == 0)
		return (WALK_DONE);

	if (mdb_vread(&si, sizeof (struct swapinfo), sip) == -1) {
		mdb_warn("unable to read swapinfo at %#lx", sip);
		return (WALK_ERR);
	}

	wsp->walk_addr = (uintptr_t)si.si_next;

	return (wsp->walk_callback(sip, &si, wsp->walk_cbdata));
}

int
swapinfof(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv)
{
	struct swapinfo	si;
	char		*name;

	if (!(flags & DCMD_ADDRSPEC)) {
		if (mdb_walk_dcmd("swapinfo", "swapinfo", argc, argv) == -1) {
			mdb_warn("can't walk swapinfo");
			return (DCMD_ERR);
		}
		return (DCMD_OK);
	}

	if (DCMD_HDRSPEC(flags)) {
		mdb_printf("%<u>%?s %?s %9s %9s %s%</u>\n",
		    "ADDR", "VNODE", "PAGES", "FREE", "NAME");
	}

	if (mdb_vread(&si, sizeof (struct swapinfo), addr) == -1) {
		mdb_warn("can't read swapinfo at %#lx", addr);
		return (DCMD_ERR);
	}

	name = mdb_alloc(si.si_pnamelen, UM_SLEEP | UM_GC);
	if (mdb_vread(name, si.si_pnamelen, (uintptr_t)si.si_pname) == -1)
		name = "*error*";

	mdb_printf("%0?lx %?p %9d %9d %s\n",
	    addr, si.si_vp, si.si_npgs, si.si_nfpgs, name);

	return (DCMD_OK);
}

int
memlist_walk_step(mdb_walk_state_t *wsp)
{
	uintptr_t	mlp;
	struct memlist	ml;

	mlp = wsp->walk_addr;

	if (mlp == 0)
		return (WALK_DONE);

	if (mdb_vread(&ml, sizeof (struct memlist), mlp) == -1) {
		mdb_warn("unable to read memlist at %#lx", mlp);
		return (WALK_ERR);
	}

	wsp->walk_addr = (uintptr_t)ml.ml_next;

	return (wsp->walk_callback(mlp, &ml, wsp->walk_cbdata));
}

int
memlist(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv)
{
	struct memlist	ml;

	if (!(flags & DCMD_ADDRSPEC)) {
		uintptr_t ptr;
		uint_t list = 0;
		int i;
		static const char *lists[] = {
			"phys_install",
			"phys_avail",
			"virt_avail"
		};

		if (mdb_getopts(argc, argv,
		    'i', MDB_OPT_SETBITS, (1 << 0), &list,
		    'a', MDB_OPT_SETBITS, (1 << 1), &list,
		    'v', MDB_OPT_SETBITS, (1 << 2), &list, NULL) != argc)
			return (DCMD_USAGE);

		if (!list)
			list = 1;

		for (i = 0; list; i++, list >>= 1) {
			if (!(list & 1))
				continue;
			if ((mdb_readvar(&ptr, lists[i]) == -1) ||
			    (ptr == 0)) {
				mdb_warn("%s not found or invalid", lists[i]);
				return (DCMD_ERR);
			}

			mdb_printf("%s:\n", lists[i]);
			if (mdb_pwalk_dcmd("memlist", "memlist", 0, NULL,
			    ptr) == -1) {
				mdb_warn("can't walk memlist");
				return (DCMD_ERR);
			}
		}
		return (DCMD_OK);
	}

	if (DCMD_HDRSPEC(flags))
		mdb_printf("%<u>%?s %16s %16s%</u>\n", "ADDR", "BASE", "SIZE");

	if (mdb_vread(&ml, sizeof (struct memlist), addr) == -1) {
		mdb_warn("can't read memlist at %#lx", addr);
		return (DCMD_ERR);
	}

	mdb_printf("%0?lx %16llx %16llx\n", addr, ml.ml_address, ml.ml_size);

	return (DCMD_OK);
}

int
seg_walk_init(mdb_walk_state_t *wsp)
{
	if (wsp->walk_addr == 0) {
		mdb_warn("seg walk must begin at struct as *\n");
		return (WALK_ERR);
	}

	/*
	 * this is really just a wrapper to AVL tree walk
	 */
	wsp->walk_addr = (uintptr_t)&((struct as *)wsp->walk_addr)->a_segtree;
	return (avl_walk_init(wsp));
}

/*ARGSUSED*/
int
seg(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv)
{
	struct seg s;

	if (argc != 0)
		return (DCMD_USAGE);

	if ((flags & DCMD_LOOPFIRST) || !(flags & DCMD_LOOP)) {
		mdb_printf("%<u>%?s %?s %?s %?s %s%</u>\n",
		    "SEG", "BASE", "SIZE", "DATA", "OPS");
	}

	if (mdb_vread(&s, sizeof (s), addr) == -1) {
		mdb_warn("failed to read seg at %p", addr);
		return (DCMD_ERR);
	}

	mdb_printf("%?p %?p %?lx %?p %a\n",
	    addr, s.s_base, s.s_size, s.s_data, s.s_ops);

	return (DCMD_OK);
}

typedef struct pmap_walk_types {
	uintptr_t pwt_segvn;
	uintptr_t pwt_seghole;
} pmap_walk_types_t;

/*ARGSUSED*/
static int
pmap_walk_count_pages(uintptr_t addr, const void *data, void *out)
{
	pgcnt_t *nres = out;

	(*nres)++;

	return (WALK_NEXT);
}

static int
pmap_walk_seg(uintptr_t addr, const struct seg *seg,
    const pmap_walk_types_t *types)
{
	const uintptr_t ops = (uintptr_t)seg->s_ops;

	mdb_printf("%0?p %0?p %7dk", addr, seg->s_base, seg->s_size / 1024);

	if (ops == types->pwt_segvn && seg->s_data != NULL) {
		struct segvn_data svn;
		pgcnt_t nres = 0;

		svn.vp = NULL;
		(void) mdb_vread(&svn, sizeof (svn), (uintptr_t)seg->s_data);

		/*
		 * Use the segvn_pages walker to find all of the in-core pages
		 * for this mapping.
		 */
		if (mdb_pwalk("segvn_pages", pmap_walk_count_pages, &nres,
		    (uintptr_t)seg->s_data) == -1) {
			mdb_warn("failed to walk segvn_pages (s_data=%p)",
			    seg->s_data);
		}
		mdb_printf(" %7ldk", (nres * PAGESIZE) / 1024);

		if (svn.vp != NULL) {
			char buf[29];

			mdb_vnode2path((uintptr_t)svn.vp, buf, sizeof (buf));
			mdb_printf(" %s", buf);
		} else {
			mdb_printf(" [ anon ]");
		}
	} else if (ops == types->pwt_seghole && seg->s_data != NULL) {
		seghole_data_t shd;
		char name[16];

		(void) mdb_vread(&shd, sizeof (shd), (uintptr_t)seg->s_data);
		if (shd.shd_name == NULL || mdb_readstr(name, sizeof (name),
		    (uintptr_t)shd.shd_name) == 0) {
			name[0] = '\0';
		}

		mdb_printf(" %8s [ hole%s%s ]", "-",
		    name[0] == '0' ? "" : ":", name);
	} else {
		mdb_printf(" %8s [ &%a ]", "?", seg->s_ops);
	}

	mdb_printf("\n");
	return (WALK_NEXT);
}

static int
pmap_walk_seg_quick(uintptr_t addr, const struct seg *seg,
    const pmap_walk_types_t *types)
{
	const uintptr_t ops = (uintptr_t)seg->s_ops;

	mdb_printf("%0?p %0?p %7dk", addr, seg->s_base, seg->s_size / 1024);

	if (ops == types->pwt_segvn && seg->s_data != NULL) {
		struct segvn_data svn;

		svn.vp = NULL;
		(void) mdb_vread(&svn, sizeof (svn), (uintptr_t)seg->s_data);

		if (svn.vp != NULL) {
			mdb_printf(" %0?p", svn.vp);
		} else {
			mdb_printf(" [ anon ]");
		}
	} else {
		mdb_printf(" [ &%a ]", seg->s_ops);
	}

	mdb_printf("\n");
	return (WALK_NEXT);
}

/*ARGSUSED*/
int
pmap(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv)
{
	proc_t proc;
	uint_t quick = FALSE;
	mdb_walk_cb_t cb = (mdb_walk_cb_t)pmap_walk_seg;
	pmap_walk_types_t wtypes = { 0 };

	GElf_Sym sym;

	if (!(flags & DCMD_ADDRSPEC))
		return (DCMD_USAGE);

	if (mdb_getopts(argc, argv,
	    'q', MDB_OPT_SETBITS, TRUE, &quick, NULL) != argc)
		return (DCMD_USAGE);

	if (mdb_vread(&proc, sizeof (proc), addr) == -1) {
		mdb_warn("failed to read proc at %p", addr);
		return (DCMD_ERR);
	}

	if (mdb_lookup_by_name("segvn_ops", &sym) == 0)
		wtypes.pwt_segvn = (uintptr_t)sym.st_value;
	if (mdb_lookup_by_name("seghole_ops", &sym) == 0)
		wtypes.pwt_seghole = (uintptr_t)sym.st_value;

	mdb_printf("%?s %?s %8s ", "SEG", "BASE", "SIZE");

	if (quick) {
		mdb_printf("VNODE\n");
		cb = (mdb_walk_cb_t)pmap_walk_seg_quick;
	} else {
		mdb_printf("%8s %s\n", "RES", "PATH");
	}

	if (mdb_pwalk("seg", cb, (void *)&wtypes, (uintptr_t)proc.p_as) == -1) {
		mdb_warn("failed to walk segments of as %p", proc.p_as);
		return (DCMD_ERR);
	}

	return (DCMD_OK);
}

typedef struct anon_walk_data {
	uintptr_t *aw_levone;
	uintptr_t *aw_levtwo;
	size_t aw_minslot;
	size_t aw_maxslot;
	pgcnt_t aw_nlevone;
	pgcnt_t aw_levone_ndx;
	size_t aw_levtwo_ndx;
	struct anon_map	*aw_ampp;
	struct anon_map aw_amp;
	struct anon_hdr	aw_ahp;
	int		aw_all;	/* report all anon pointers, even NULLs */
} anon_walk_data_t;

int
anon_walk_init_common(mdb_walk_state_t *wsp, ulong_t minslot, ulong_t maxslot)
{
	anon_walk_data_t *aw;

	if (wsp->walk_addr == 0) {
		mdb_warn("anon walk doesn't support global walks\n");
		return (WALK_ERR);
	}

	aw = mdb_alloc(sizeof (anon_walk_data_t), UM_SLEEP);
	aw->aw_ampp = (struct anon_map *)wsp->walk_addr;

	if (mdb_vread(&aw->aw_amp, sizeof (aw->aw_amp), wsp->walk_addr) == -1) {
		mdb_warn("failed to read anon map at %p", wsp->walk_addr);
		mdb_free(aw, sizeof (anon_walk_data_t));
		return (WALK_ERR);
	}

	if (mdb_vread(&aw->aw_ahp, sizeof (aw->aw_ahp),
	    (uintptr_t)(aw->aw_amp.ahp)) == -1) {
		mdb_warn("failed to read anon hdr ptr at %p", aw->aw_amp.ahp);
		mdb_free(aw, sizeof (anon_walk_data_t));
		return (WALK_ERR);
	}

	/* update min and maxslot with the given constraints */
	maxslot = MIN(maxslot, aw->aw_ahp.size);
	minslot = MIN(minslot, maxslot);

	if (aw->aw_ahp.size <= ANON_CHUNK_SIZE ||
	    (aw->aw_ahp.flags & ANON_ALLOC_FORCE)) {
		aw->aw_nlevone = maxslot;
		aw->aw_levone_ndx = minslot;
		aw->aw_levtwo = NULL;
	} else {
		aw->aw_nlevone =
		    (maxslot + ANON_CHUNK_OFF) >> ANON_CHUNK_SHIFT;
		aw->aw_levone_ndx = 0;
		aw->aw_levtwo =
		    mdb_zalloc(ANON_CHUNK_SIZE * sizeof (uintptr_t), UM_SLEEP);
	}

	aw->aw_levone =
	    mdb_alloc(aw->aw_nlevone * sizeof (uintptr_t), UM_SLEEP);
	aw->aw_all = (wsp->walk_arg == ANON_WALK_ALL);

	mdb_vread(aw->aw_levone, aw->aw_nlevone * sizeof (uintptr_t),
	    (uintptr_t)aw->aw_ahp.array_chunk);

	aw->aw_levtwo_ndx = 0;
	aw->aw_minslot = minslot;
	aw->aw_maxslot = maxslot;

out:
	wsp->walk_data = aw;
	return (0);
}

int
anon_walk_step(mdb_walk_state_t *wsp)
{
	anon_walk_data_t *aw = (anon_walk_data_t *)wsp->walk_data;
	struct anon anon;
	uintptr_t anonptr;
	ulong_t slot;

	/*
	 * Once we've walked through level one, we're done.
	 */
	if (aw->aw_levone_ndx >= aw->aw_nlevone) {
		return (WALK_DONE);
	}

	if (aw->aw_levtwo == NULL) {
		anonptr = aw->aw_levone[aw->aw_levone_ndx];
		aw->aw_levone_ndx++;
	} else {
		if (aw->aw_levtwo_ndx == 0) {
			uintptr_t levtwoptr;

			/* The first time through, skip to our first index. */
			if (aw->aw_levone_ndx == 0) {
				aw->aw_levone_ndx =
				    aw->aw_minslot / ANON_CHUNK_SIZE;
				aw->aw_levtwo_ndx =
				    aw->aw_minslot % ANON_CHUNK_SIZE;
			}

			levtwoptr = (uintptr_t)aw->aw_levone[aw->aw_levone_ndx];

			if (levtwoptr == 0) {
				if (!aw->aw_all) {
					aw->aw_levtwo_ndx = 0;
					aw->aw_levone_ndx++;
					return (WALK_NEXT);
				}
				bzero(aw->aw_levtwo,
				    ANON_CHUNK_SIZE * sizeof (uintptr_t));

			} else if (mdb_vread(aw->aw_levtwo,
			    ANON_CHUNK_SIZE * sizeof (uintptr_t), levtwoptr) ==
			    -1) {
				mdb_warn("unable to read anon_map %p's "
				    "second-level map %d at %p",
				    aw->aw_ampp, aw->aw_levone_ndx,
				    levtwoptr);
				return (WALK_ERR);
			}
		}
		slot = aw->aw_levone_ndx * ANON_CHUNK_SIZE + aw->aw_levtwo_ndx;
		anonptr = aw->aw_levtwo[aw->aw_levtwo_ndx];

		/* update the indices for next time */
		aw->aw_levtwo_ndx++;
		if (aw->aw_levtwo_ndx == ANON_CHUNK_SIZE) {
			aw->aw_levtwo_ndx = 0;
			aw->aw_levone_ndx++;
		}

		/* make sure the slot # is in the requested range */
		if (slot >= aw->aw_maxslot) {
			return (WALK_DONE);
		}
	}

	if (anonptr != 0) {
		mdb_vread(&anon, sizeof (anon), anonptr);
		return (wsp->walk_callback(anonptr, &anon, wsp->walk_cbdata));
	}
	if (aw->aw_all) {
		return (wsp->walk_callback(0, NULL, wsp->walk_cbdata));
	}
	return (WALK_NEXT);
}

void
anon_walk_fini(mdb_walk_state_t *wsp)
{
	anon_walk_data_t *aw = (anon_walk_data_t *)wsp->walk_data;

	if (aw->aw_levtwo != NULL)
		mdb_free(aw->aw_levtwo, ANON_CHUNK_SIZE * sizeof (uintptr_t));

	mdb_free(aw->aw_levone, aw->aw_nlevone * sizeof (uintptr_t));
	mdb_free(aw, sizeof (anon_walk_data_t));
}

int
anon_walk_init(mdb_walk_state_t *wsp)
{
	return (anon_walk_init_common(wsp, 0, ULONG_MAX));
}

int
segvn_anon_walk_init(mdb_walk_state_t *wsp)
{
	const uintptr_t		svd_addr = wsp->walk_addr;
	uintptr_t		amp_addr;
	uintptr_t		seg_addr;
	struct segvn_data	svd;
	struct anon_map		amp;
	struct seg		seg;

	if (svd_addr == 0) {
		mdb_warn("segvn_anon walk doesn't support global walks\n");
		return (WALK_ERR);
	}
	if (mdb_vread(&svd, sizeof (svd), svd_addr) == -1) {
		mdb_warn("segvn_anon walk: unable to read segvn_data at %p",
		    svd_addr);
		return (WALK_ERR);
	}
	if (svd.amp == NULL) {
		mdb_warn("segvn_anon walk: segvn_data at %p has no anon map\n",
		    svd_addr);
		return (WALK_ERR);
	}
	amp_addr = (uintptr_t)svd.amp;
	if (mdb_vread(&amp, sizeof (amp), amp_addr) == -1) {
		mdb_warn("segvn_anon walk: unable to read amp %p for "
		    "segvn_data %p", amp_addr, svd_addr);
		return (WALK_ERR);
	}
	seg_addr = (uintptr_t)svd.seg;
	if (mdb_vread(&seg, sizeof (seg), seg_addr) == -1) {
		mdb_warn("segvn_anon walk: unable to read seg %p for "
		    "segvn_data %p", seg_addr, svd_addr);
		return (WALK_ERR);
	}
	if ((seg.s_size + (svd.anon_index << PAGESHIFT)) > amp.size) {
		mdb_warn("anon map %p is too small for segment %p\n",
		    amp_addr, seg_addr);
		return (WALK_ERR);
	}

	wsp->walk_addr = amp_addr;
	return (anon_walk_init_common(wsp,
	    svd.anon_index, svd.anon_index + (seg.s_size >> PAGESHIFT)));
}


typedef struct {
	u_offset_t		svs_offset;
	uintptr_t		svs_page;
} segvn_sparse_t;
#define	SEGVN_MAX_SPARSE	((128 * 1024) / sizeof (segvn_sparse_t))

typedef struct {
	uintptr_t		svw_svdp;
	struct segvn_data	svw_svd;
	struct seg		svw_seg;
	size_t			svw_walkoff;
	ulong_t			svw_anonskip;
	segvn_sparse_t		*svw_sparse;
	size_t			svw_sparse_idx;
	size_t			svw_sparse_count;
	size_t			svw_sparse_size;
	uint8_t			svw_sparse_overflow;
	uint8_t			svw_all;
} segvn_walk_data_t;

static int
segvn_sparse_fill(uintptr_t addr, const void *pp_arg, void *arg)
{
	segvn_walk_data_t	*const	svw = arg;
	const page_t		*const	pp = pp_arg;
	const u_offset_t		offset = pp->p_offset;
	segvn_sparse_t		*const	cur =
	    &svw->svw_sparse[svw->svw_sparse_count];

	/* See if the page is of interest */
	if ((u_offset_t)(offset - svw->svw_svd.offset) >= svw->svw_seg.s_size) {
		return (WALK_NEXT);
	}
	/* See if we have space for the new entry, then add it. */
	if (svw->svw_sparse_count >= svw->svw_sparse_size) {
		svw->svw_sparse_overflow = 1;
		return (WALK_DONE);
	}
	svw->svw_sparse_count++;
	cur->svs_offset = offset;
	cur->svs_page = addr;
	return (WALK_NEXT);
}

static int
segvn_sparse_cmp(const void *lp, const void *rp)
{
	const segvn_sparse_t *const	l = lp;
	const segvn_sparse_t *const	r = rp;

	if (l->svs_offset < r->svs_offset) {
		return (-1);
	}
	if (l->svs_offset > r->svs_offset) {
		return (1);
	}
	return (0);
}

/*
 * Builds on the "anon_all" walker to walk all resident pages in a segvn_data
 * structure.  For segvn_datas without an anon structure, it just looks up
 * pages in the vnode.  For segvn_datas with an anon structure, NULL slots
 * pass through to the vnode, and non-null slots are checked for residency.
 */
int
segvn_pages_walk_init(mdb_walk_state_t *wsp)
{
	segvn_walk_data_t	*svw;
	struct segvn_data	*svd;

	if (wsp->walk_addr == 0) {
		mdb_warn("segvn walk doesn't support global walks\n");
		return (WALK_ERR);
	}

	svw = mdb_zalloc(sizeof (*svw), UM_SLEEP);
	svw->svw_svdp = wsp->walk_addr;
	svw->svw_anonskip = 0;
	svw->svw_sparse_idx = 0;
	svw->svw_walkoff = 0;
	svw->svw_all = (wsp->walk_arg == SEGVN_PAGES_ALL);

	if (mdb_vread(&svw->svw_svd, sizeof (svw->svw_svd), wsp->walk_addr) ==
	    -1) {
		mdb_warn("failed to read segvn_data at %p", wsp->walk_addr);
		mdb_free(svw, sizeof (*svw));
		return (WALK_ERR);
	}

	svd = &svw->svw_svd;
	if (mdb_vread(&svw->svw_seg, sizeof (svw->svw_seg),
	    (uintptr_t)svd->seg) == -1) {
		mdb_warn("failed to read seg at %p (from %p)",
		    svd->seg, &((struct segvn_data *)(wsp->walk_addr))->seg);
		mdb_free(svw, sizeof (*svw));
		return (WALK_ERR);
	}

	if (svd->amp == NULL && svd->vp == NULL) {
		/* make the walk terminate immediately;  no pages */
		svw->svw_walkoff = svw->svw_seg.s_size;

	} else if (svd->amp == NULL &&
	    (svw->svw_seg.s_size >> PAGESHIFT) >= SEGVN_MAX_SPARSE) {
		/*
		 * If we don't have an anon pointer, and the segment is large,
		 * we try to load the in-memory pages into a fixed-size array,
		 * which is then sorted and reported directly.  This is much
		 * faster than doing a mdb_page_lookup() for each possible
		 * offset.
		 *
		 * If the allocation fails, or there are too many pages
		 * in-core, we fall back to looking up the pages individually.
		 */
		svw->svw_sparse = mdb_alloc(
		    SEGVN_MAX_SPARSE * sizeof (*svw->svw_sparse), UM_NOSLEEP);
		if (svw->svw_sparse != NULL) {
			svw->svw_sparse_size = SEGVN_MAX_SPARSE;

			if (mdb_pwalk("page", segvn_sparse_fill, svw,
			    (uintptr_t)svd->vp) == -1 ||
			    svw->svw_sparse_overflow) {
				mdb_free(svw->svw_sparse, SEGVN_MAX_SPARSE *
				    sizeof (*svw->svw_sparse));
				svw->svw_sparse = NULL;
			} else {
				qsort(svw->svw_sparse, svw->svw_sparse_count,
				    sizeof (*svw->svw_sparse),
				    segvn_sparse_cmp);
			}
		}

	} else if (svd->amp != NULL) {
		const char *const layer = (!svw->svw_all && svd->vp == NULL) ?
		    "segvn_anon" : "segvn_anon_all";
		/*
		 * If we're not printing all offsets, and the segvn_data has
		 * no backing VP, we can use the "segvn_anon" walker, which
		 * efficiently skips NULL slots.
		 *
		 * Otherwise, we layer over the "segvn_anon_all" walker
		 * (which reports all anon slots, even NULL ones), so that
		 * segvn_pages_walk_step() knows the precise offset for each
		 * element.  It uses that offset information to look up the
		 * backing pages for NULL anon slots.
		 */
		if (mdb_layered_walk(layer, wsp) == -1) {
			mdb_warn("segvn_pages: failed to layer \"%s\" "
			    "for segvn_data %p", layer, svw->svw_svdp);
			mdb_free(svw, sizeof (*svw));
			return (WALK_ERR);
		}
	}

	wsp->walk_data = svw;
	return (WALK_NEXT);
}

int
segvn_pages_walk_step(mdb_walk_state_t *wsp)
{
	segvn_walk_data_t	*const	svw = wsp->walk_data;
	struct seg		*const	seg = &svw->svw_seg;
	struct segvn_data	*const	svd = &svw->svw_svd;
	uintptr_t		pp;
	page_t			page;

	/* If we've walked off the end of the segment, we're done. */
	if (svw->svw_walkoff >= seg->s_size) {
		return (WALK_DONE);
	}

	/*
	 * If we've got a sparse page array, just send it directly.
	 */
	if (svw->svw_sparse != NULL) {
		u_offset_t off;

		if (svw->svw_sparse_idx >= svw->svw_sparse_count) {
			pp = 0;
			if (!svw->svw_all) {
				return (WALK_DONE);
			}
		} else {
			segvn_sparse_t	*const svs =
			    &svw->svw_sparse[svw->svw_sparse_idx];
			off = svs->svs_offset - svd->offset;
			if (svw->svw_all && svw->svw_walkoff != off) {
				pp = 0;
			} else {
				pp = svs->svs_page;
				svw->svw_sparse_idx++;
			}
		}

	} else if (svd->amp == NULL || wsp->walk_addr == 0) {
		/*
		 * If there's no anon, or the anon slot is NULL, look up
		 * <vp, offset>.
		 */
		if (svd->vp != NULL) {
			pp = mdb_page_lookup((uintptr_t)svd->vp,
			    svd->offset + svw->svw_walkoff);
		} else {
			pp = 0;
		}

	} else {
		const struct anon	*const	anon = wsp->walk_layer;

		/*
		 * We have a "struct anon"; if it's not swapped out,
		 * look up the page.
		 */
		if (anon->an_vp != NULL || anon->an_off != 0) {
			pp = mdb_page_lookup((uintptr_t)anon->an_vp,
			    anon->an_off);
			if (pp == 0 && mdb_get_state() != MDB_STATE_RUNNING) {
				mdb_warn("walk segvn_pages: segvn_data %p "
				    "offset %ld, anon page <%p, %llx> not "
				    "found.\n", svw->svw_svdp, svw->svw_walkoff,
				    anon->an_vp, anon->an_off);
			}
		} else {
			if (anon->an_pvp == NULL) {
				mdb_warn("walk segvn_pages: useless struct "
				    "anon at %p\n", wsp->walk_addr);
			}
			pp = 0;	/* nothing at this offset */
		}
	}

	svw->svw_walkoff += PAGESIZE;	/* Update for the next call */
	if (pp != 0) {
		if (mdb_vread(&page, sizeof (page_t), pp) == -1) {
			mdb_warn("unable to read page_t at %#lx", pp);
			return (WALK_ERR);
		}
		return (wsp->walk_callback(pp, &page, wsp->walk_cbdata));
	}
	if (svw->svw_all) {
		return (wsp->walk_callback(0, NULL, wsp->walk_cbdata));
	}
	return (WALK_NEXT);
}

void
segvn_pages_walk_fini(mdb_walk_state_t *wsp)
{
	segvn_walk_data_t	*const	svw = wsp->walk_data;

	if (svw->svw_sparse != NULL) {
		mdb_free(svw->svw_sparse, SEGVN_MAX_SPARSE *
		    sizeof (*svw->svw_sparse));
	}
	mdb_free(svw, sizeof (*svw));
}

/*
 * Grumble, grumble.
 */
#define	SMAP_HASHFUNC(vp, off)	\
	((((uintptr_t)(vp) >> 6) + ((uintptr_t)(vp) >> 3) + \
	((off) >> MAXBSHIFT)) & smd_hashmsk)

int
vnode2smap(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv)
{
	long smd_hashmsk;
	int hash;
	uintptr_t offset = 0;
	struct smap smp;
	uintptr_t saddr, kaddr;
	uintptr_t smd_hash, smd_smap;
	struct seg seg;

	if (!(flags & DCMD_ADDRSPEC))
		return (DCMD_USAGE);

	if (mdb_readvar(&smd_hashmsk, "smd_hashmsk") == -1) {
		mdb_warn("failed to read smd_hashmsk");
		return (DCMD_ERR);
	}

	if (mdb_readvar(&smd_hash, "smd_hash") == -1) {
		mdb_warn("failed to read smd_hash");
		return (DCMD_ERR);
	}

	if (mdb_readvar(&smd_smap, "smd_smap") == -1) {
		mdb_warn("failed to read smd_hash");
		return (DCMD_ERR);
	}

	if (mdb_readvar(&kaddr, "segkmap") == -1) {
		mdb_warn("failed to read segkmap");
		return (DCMD_ERR);
	}

	if (mdb_vread(&seg, sizeof (seg), kaddr) == -1) {
		mdb_warn("failed to read segkmap at %p", kaddr);
		return (DCMD_ERR);
	}

	if (argc != 0) {
		const mdb_arg_t *arg = &argv[0];

		offset = (uintptr_t)mdb_argtoull(arg);
	}

	hash = SMAP_HASHFUNC(addr, offset);

	if (mdb_vread(&saddr, sizeof (saddr),
	    smd_hash + hash * sizeof (uintptr_t)) == -1) {
		mdb_warn("couldn't read smap at %p",
		    smd_hash + hash * sizeof (uintptr_t));
		return (DCMD_ERR);
	}

	do {
		if (mdb_vread(&smp, sizeof (smp), saddr) == -1) {
			mdb_warn("couldn't read smap at %p", saddr);
			return (DCMD_ERR);
		}

		if ((uintptr_t)smp.sm_vp == addr && smp.sm_off == offset) {
			mdb_printf("vnode %p, offs %p is smap %p, vaddr %p\n",
			    addr, offset, saddr, ((saddr - smd_smap) /
			    sizeof (smp)) * MAXBSIZE + seg.s_base);
			return (DCMD_OK);
		}

		saddr = (uintptr_t)smp.sm_hash;
	} while (saddr != 0);

	mdb_printf("no smap for vnode %p, offs %p\n", addr, offset);
	return (DCMD_OK);
}

/*ARGSUSED*/
int
addr2smap(uintptr_t addr, uint_t flags, int argc, const mdb_arg_t *argv)
{
	uintptr_t kaddr;
	struct seg seg;
	struct segmap_data sd;

	if (!(flags & DCMD_ADDRSPEC))
		return (DCMD_USAGE);

	if (mdb_readvar(&kaddr, "segkmap") == -1) {
		mdb_warn("failed to read segkmap");
		return (DCMD_ERR);
	}

	if (mdb_vread(&seg, sizeof (seg), kaddr) == -1) {
		mdb_warn("failed to read segkmap at %p", kaddr);
		return (DCMD_ERR);
	}

	if (mdb_vread(&sd, sizeof (sd), (uintptr_t)seg.s_data) == -1) {
		mdb_warn("failed to read segmap_data at %p", seg.s_data);
		return (DCMD_ERR);
	}

	mdb_printf("%p is smap %p\n", addr,
	    ((addr - (uintptr_t)seg.s_base) >> MAXBSHIFT) *
	    sizeof (struct smap) + (uintptr_t)sd.smd_sm);

	return (DCMD_OK);
}



/* ============================================================================
 * SOURCE 6/7: minix4\exokernel\kernel_legacy\arch\earm\memory.c
 * Size: 25,710 bytes, Lines: 869
 * Hash: 867a8bc3ea4e...
 * ============================================================================ */

#include "kernel/kernel.h"
#include "kernel/proc.h"
#include "kernel/vm.h"

// #include <machine/vm.h> // Kept, appears twice, will be one after cleaning -> Removed duplicate
#include <minix/type.h>    // Kept for now
#include <minix/board.h>   // Kept for now
// #include <minix/syslib.h> // Removed
#include <minix/cpufeature.h> // Kept for now
// #include <string.h>       // Replaced
// #include <assert.h>       // Replaced
// #include <signal.h>       // Replaced
// #include <stdlib.h>       // Removed

// Added kernel headers
#include <minix/kernel_types.h>
#include <sys/kassert.h>
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <klib/include/kmemory.h>

#include "arch_proto.h"
#include "kernel/proto.h"
#include "kernel/debug.h"
#include "bsp_timer.h"


#define HASPT(procptr) ((procptr)->p_seg.p_ttbr != 0)
static int nfreepdes = 0;
#define MAXFREEPDES	2
static int freepdes[MAXFREEPDES];

static u32_t phys_get32(phys_bytes v);

/* list of requested physical mapping */
static kern_phys_map *kern_phys_map_head;

void mem_clear_mapcache(void)
{
	int i;
	for(i = 0; i < nfreepdes; i++) {
		struct proc *ptproc = get_cpulocal_var(ptproc);
		int pde = freepdes[i];
		u32_t *ptv;
		KASSERT(ptproc);
		ptv = ptproc->p_seg.p_ttbr_v;
		KASSERT(ptv);
		ptv[pde] = 0;
	}
}

/* This function sets up a mapping from within the kernel's address
 * space to any other area of memory, either straight physical
 * memory (pr == NULL) or a process view of memory, in 1MB windows.
 * I.e., it maps in 1MB chunks of virtual (or physical) address space
 * to 1MB chunks of kernel virtual address space.
 *
 * It recognizes pr already being in memory as a special case (no
 * mapping required).
 *
 * The target (i.e. in-kernel) mapping area is one of the freepdes[]
 * VM has earlier already told the kernel about that is available. It is
 * identified as the 'pde' parameter. This value can be chosen freely
 * by the caller, as long as it is in range (i.e. 0 or higher and corresponds
 * to a known freepde slot). It is up to the caller to keep track of which
 * freepde's are in use, and to determine which ones are free to use.
 *
 * The logical number supplied by the caller is translated into an actual
 * pde number to be used, and a pointer to it (linear address) is returned
 * for actual use by phys_copy or memset.
 */
static phys_bytes createpde(
	const struct proc *pr,	/* Requested process, NULL for physical. */
	const phys_bytes linaddr,/* Address after segment translation. */
	phys_bytes *bytes,	/* Size of chunk, function may truncate it. */
	int free_pde_idx,	/* index of the free slot to use */
	int *changed		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_bytes offset;
	int pde;

	KASSERT(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	KASSERT(pde >= 0 && pde < 4096);

	if(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		return linaddr;
	}

	if(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		KASSERT(pr->p_seg.p_ttbr_v);
		pdeval = pr->p_seg.p_ttbr_v[ARM_VM_PDE(linaddr)];
	} else {
		/* Requested address is physical. Make up the PDE entry. */
		KASSERT(linaddr >= PHYS_MEM_BEGIN && linaddr <= PHYS_MEM_END);

		/* memory */
		pdeval = (linaddr & ARM_VM_SECTION_MASK)
			| ARM_VM_SECTION
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED
			| ARM_VM_SECTION_USER;
	}

	/* Write the pde value that we need into a pde that the kernel
	 * can access, into the currently loaded page table so it becomes
	 * visible.
	 */
	KASSERT(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	if(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] != pdeval) {
		get_cpulocal_var(ptproc)->p_seg.p_ttbr_v[pde] = pdeval;
		*changed = 1;
	}

	/* Memory is now available, but only the 1MB window of virtual
	 * address space that we have mapped; calculate how much of
	 * the requested range is visible and return that in *bytes,
	 * if that is less than the requested range.
	 */
	offset = linaddr & ARM_VM_OFFSET_MASK_1MB; /* Offset in 1MB window. */
	*bytes = MIN(*bytes, ARM_SECTION_SIZE - offset);

	/* Return the linear address of the start of the new mapping. */
	return ARM_SECTION_SIZE*pde + offset;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
static int check_resumed_caller(struct proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	if (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		KASSERT(caller->p_vmrequest.vmresult != VMSUSPEND);
		return caller->p_vmrequest.vmresult;
	}

	return OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
static int lin_lin_copy(struct proc *srcproc, vir_bytes srclinaddr, 
	struct proc *dstproc, vir_bytes dstlinaddr, vir_bytes bytes)
{
	u32_t addr;
	proc_nr_t procslot;

	KASSERT(get_cpulocal_var(ptproc));
	KASSERT(get_cpulocal_var(proc_ptr));
	KASSERT(read_ttbr0() == get_cpulocal_var(ptproc)->p_seg.p_ttbr);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	KASSERT(procslot >= 0 && procslot < ARM_VM_DIR_ENTRIES);

	if(srcproc) KASSERT(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	if(dstproc) KASSERT(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	KASSERT(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	KASSERT(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	if(srcproc) KASSERT(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	if(dstproc) KASSERT(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	while(bytes > 0) {
		phys_bytes srcptr, dstptr;
		vir_bytes chunk = bytes;
		int changed = 0;

#ifdef CONFIG_SMP
		unsigned cpu = cpuid;

		if (srcproc && GET_BIT(srcproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(srcproc->p_stale_tlb, cpu);
		}
		if (dstproc && GET_BIT(dstproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(dstproc->p_stale_tlb, cpu);
		}
#endif

		/* Set up 1MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		if(changed)
			reload_ttbr0();

		/* Check for overflow. */
		if (srcptr + chunk < srcptr) return EFAULT_SRC;
		if (dstptr + chunk < dstptr) return EFAULT_DST;

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		if(addr) {
			/* If addr is nonzero, a page fault was caught.
			 *
			 * phys_copy does all memory accesses word-aligned (rounded
			 * down), so pagefaults can occur at a lower address than
			 * the specified offsets. compute the lower bounds for sanity
			 * check use.
			 */
			vir_bytes src_aligned = srcptr & ~0x3, dst_aligned = dstptr & ~0x3;

			if(addr >= src_aligned && addr < (srcptr + chunk)) {
				return EFAULT_SRC;
			}
			if(addr >= dst_aligned && addr < (dstptr + chunk)) {
				return EFAULT_DST;
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			return EFAULT;
		}

		/* Update counter and addresses for next iteration, if any. */
		bytes -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	if(srcproc) KASSERT(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	if(dstproc) KASSERT(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	KASSERT(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	KASSERT(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);

	return OK;
}

static u32_t phys_get32(phys_bytes addr)
{
	u32_t v;
	int r;

	if((r=lin_lin_copy(NULL, addr, 
		proc_addr(SYSTEM), (phys_bytes) &v, sizeof(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	return v;
}

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_bytes umap_virtual(
  register struct proc *rp,		/* pointer to proc table entry for process */
  int seg,				/* T, D, or S segment */
  vir_bytes vir_addr,			/* virtual address in bytes within the seg */
  vir_bytes bytes			/* # of bytes to be copied */
)
{
	phys_bytes phys = 0;

	if(vm_lookup(rp, vir_addr, &phys, NULL) != OK) {
		kprintf_stub("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr); // MODIFIED
		kprintf_stub("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr); // MODIFIED
		phys = 0;
	} else {
		if(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	if(phys == 0) {
		kprintf_stub("SYSTEM:umap_virtual: lookup failed\n"); // MODIFIED
		kprintf_stub("SYSTEM:umap_virtual: lookup failed\n"); // MODIFIED
		return 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	if(bytes > 0 && vm_lookup_range(rp, vir_addr, NULL, bytes) != bytes) {
		kprintf_stub("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n", // MODIFIED
			rp->p_name, (unsigned long)bytes, vir_addr, vir_addr); // MODIFIED k_size_t cast for %lu
		kprintf_stub("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n", // MODIFIED
			rp->p_name, (unsigned long)bytes, vir_addr, vir_addr); // MODIFIED k_size_t cast for %lu
		return 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	KASSERT(phys);

	return phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
int vm_lookup(const struct proc *proc, const vir_bytes virtual,
 phys_bytes *physical, u32_t *ptent)
{
	u32_t *root, *pt;
	int pde, pte;
	u32_t pde_v, pte_v;

	KASSERT(proc);
	KASSERT(physical);
	KASSERT(!isemptyp(proc));
	KASSERT(HASPT(proc));

	/* Retrieve page directory entry. */
	root = (u32_t *) (proc->p_seg.p_ttbr & ARM_TTBR_ADDR_MASK);
	KASSERT(!((u32_t) root % ARM_PAGEDIR_SIZE));
	pde = ARM_VM_PDE(virtual);
	KASSERT(pde >= 0 && pde < ARM_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (root + pde));

	if(! ((pde_v & ARM_VM_PDE_PRESENT) 
		|| (pde_v & ARM_VM_SECTION_PRESENT)
	     )) {
		return EFAULT;
	}

	if(pde_v & ARM_VM_SECTION) {
		*physical = pde_v & ARM_VM_SECTION_MASK;
		if(ptent) *ptent = pde_v;
		*physical += virtual & ARM_VM_OFFSET_MASK_1MB;
	} else  {
		/* Retrieve page table entry. */
		pt = (u32_t *) (pde_v & ARM_VM_PDE_MASK);
		KASSERT(!((u32_t) pt % ARM_PAGETABLE_SIZE));
		pte = ARM_VM_PTE(virtual);
		KASSERT(pte >= 0 && pte < ARM_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		if(!(pte_v & ARM_VM_PTE_PRESENT)) {
			return EFAULT;
		}

		if(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = pte_v & ARM_VM_PTE_MASK;
		*physical += virtual % ARM_PAGE_SIZE;
	}

	return OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
k_size_t vm_lookup_range(const struct proc *proc, vir_bytes vir_addr, // MODIFIED size_t to k_size_t
	phys_bytes *phys_addr, k_size_t bytes) // MODIFIED size_t to k_size_t
k_size_t vm_lookup_range(const struct proc *proc, vir_bytes vir_addr, // MODIFIED size_t to k_size_t
	phys_bytes *phys_addr, k_size_t bytes) // MODIFIED size_t to k_size_t
{
	/* Look up the physical address corresponding to linear virtual address
	 * 'vir_addr' for process 'proc'. Return the size of the range covered
	 * by contiguous physical memory starting from that address; this may
	 * be anywhere between 0 and 'bytes' inclusive. If the return value is
	 * nonzero, and 'phys_addr' is non-NULL, 'phys_addr' will be set to the
	 * base physical address of the range. 'vir_addr' and 'bytes' need not
	 * be page-aligned, but the caller must have verified that the given
	 * linear range is valid for the given process at all.
	 */
	phys_bytes phys, next_phys;
	k_size_t len; // MODIFIED size_t to k_size_t
	k_size_t len; // MODIFIED size_t to k_size_t

	KASSERT(proc);
	KASSERT(bytes > 0);
	KASSERT(HASPT(proc));

	/* Look up the first page. */
	if (vm_lookup(proc, vir_addr, &phys, NULL) != OK)
		return 0;

	if (phys_addr != NULL)
		*phys_addr = phys;

	len = ARM_PAGE_SIZE - (vir_addr % ARM_PAGE_SIZE);
	vir_addr += len;
	next_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	while (len < bytes) {
		if (vm_lookup(proc, vir_addr, &phys, NULL) != OK)
			break;

		if (next_phys != phys)
			break;

		len += ARM_PAGE_SIZE;
		vir_addr += ARM_PAGE_SIZE;
		next_phys += ARM_PAGE_SIZE;
	}

	/* We might now have overshot the requested length somewhat. */
	return MIN(bytes, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
int vm_check_range(struct proc *caller, struct proc *target,
	vir_bytes vir_addr, k_size_t bytes, int writeflag) // MODIFIED size_t to k_size_t
	vir_bytes vir_addr, k_size_t bytes, int writeflag) // MODIFIED size_t to k_size_t
{
	/* Public interface to vm_suspend(), for use by kernel calls. On behalf
	 * of 'caller', call into VM to check linear virtual address range of
	 * process 'target', starting at 'vir_addr', for 'bytes' bytes. This
	 * function assumes that it will called twice if VM returned an error
	 * the first time (since nothing has changed in that case), and will
	 * then return the error code resulting from the first call. Upon the
	 * first call, a non-success error code is returned as well.
	 */
	int r;

	if ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		return r;

	vm_suspend(caller, target, vir_addr, bytes, VMSTYPE_KERNELCALL,
		writeflag);

	return VMSUSPEND;
}

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
int vm_memset(struct proc* caller, endpoint_t who, phys_bytes ph, int c,
	phys_bytes count)
{
	u32_t pattern;
	struct proc *whoptr = NULL;
	phys_bytes cur_ph = ph;
	phys_bytes left = count;
	phys_bytes ptr, chunk, pfa = 0;
	int new_ttbr, r = OK;

	if ((r = check_resumed_caller(caller)) != OK)
		return r;

	/* NONE for physical, otherwise virtual */
	if (who != NONE && !(whoptr = endpoint_lookup(who)))
		return ESRCH;

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	KASSERT(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	KASSERT(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 1MB chunk we mapped in.
	 */
	while (left > 0) {
		new_ttbr = 0;
		chunk = left;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_ttbr);

		if (new_ttbr) {
			reload_ttbr0();
		}
		/* If a page fault happens, pfa is non-null */
		if ((pfa = phys_memset(ptr, pattern, chunk))) {

			/* If a process pagefaults, VM may help out */
			if (whoptr) {
				vm_suspend(caller, whoptr, ph, count,
						   VMSTYPE_KERNELCALL, 1);
				KASSERT(catch_pagefaults);
				catch_pagefaults = 0;
				return VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		left -= chunk;
	}

	KASSERT(get_cpulocal_var(ptproc)->p_seg.p_ttbr_v);
	KASSERT(catch_pagefaults);
	catch_pagefaults = 0;

	return OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
int virtual_copy_f(
  struct proc * caller,
  struct vir_addr *src_addr,		/* source virtual address */
  struct vir_addr *dst_addr,		/* destination virtual address */
  vir_bytes bytes,			/* # of bytes to copy  */
  int vmcheck				/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  struct vir_addr *vir_addr[2];	/* virtual source and destination address */
  int i, r;
  struct proc *procs[2];

  KASSERT((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  if (bytes <= 0) return(EDOM);

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  for (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	int proc_nr;
	struct proc *p;

	if(proc_e == NONE) {
		p = NULL;
	} else {
		if(!isokendpt(proc_e, &proc_nr)) {
			kprintf_stub("virtual_copy: no reasonable endpoint\n"); // MODIFIED
			kprintf_stub("virtual_copy: no reasonable endpoint\n"); // MODIFIED
			return ESRCH;
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  if ((r = check_resumed_caller(caller)) != OK)
	return r;

  if((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->offset,
  	procs[_DST_], vir_addr[_DST_]->offset, bytes)) != OK) {
	int writeflag;
  	struct proc *target = NULL;
  	phys_bytes lin;
  	if(r != EFAULT_SRC && r != EFAULT_DST)
  		panic("lin_lin_copy failed: %d",  r);
  	if(!vmcheck || !caller) {
    		return r;
  	}

  	if(r == EFAULT_SRC) {
  		lin = vir_addr[_SRC_]->offset;
  		target = procs[_SRC_];
		writeflag = 0;
  	} else if(r == EFAULT_DST) {
  		lin = vir_addr[_DST_]->offset;
  		target = procs[_DST_];
		writeflag = 1;
  	} else {
  		panic("r strange: %d",  r);
  	}

	KASSERT(caller);
	KASSERT(target);

	vm_suspend(caller, target, lin, bytes, VMSTYPE_KERNELCALL, writeflag);
	return VMSUSPEND;
  }

  return OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
int data_copy(const endpoint_t from_proc, const vir_bytes from_addr,
	const endpoint_t to_proc, const vir_bytes to_addr,
	k_size_t bytes) // MODIFIED size_t to k_size_t
	k_size_t bytes) // MODIFIED size_t to k_size_t
{
  struct vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  KASSERT(src.proc_nr_e != NONE);
  KASSERT(dst.proc_nr_e != NONE);

  return virtual_copy(&src, &dst, bytes);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
int data_copy_vmcheck(struct proc * caller,
	const endpoint_t from_proc, const vir_bytes from_addr,
	const endpoint_t to_proc, const vir_bytes to_addr,
	k_size_t bytes) // MODIFIED size_t to k_size_t
	k_size_t bytes) // MODIFIED size_t to k_size_t
{
  struct vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  KASSERT(src.proc_nr_e != NONE);
  KASSERT(dst.proc_nr_e != NONE);

  return virtual_copy_vmcheck(caller, &src, &dst, bytes);
}

void memory_init(void)
{
	KASSERT(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_start++;
	freepdes[nfreepdes++] = kinfo.freepde_start++;

	KASSERT(kinfo.freepde_start < ARM_VM_DIR_ENTRIES);
	KASSERT(nfreepdes == 2);
	KASSERT(nfreepdes <= MAXFREEPDES);
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
void arch_proc_init(struct proc *pr, const u32_t ip, const u32_t sp,
	const u32_t ps_str, char *name)
{
	arch_proc_reset(pr);
	(void)kstrlcpy(pr->p_name, name, sizeof(pr->p_name)); /* FIXME: strcpy was here, validate size for kstrlcpy */ // MODIFIED
	(void)kstrlcpy(pr->p_name, name, sizeof(pr->p_name)); /* FIXME: strcpy was here, validate size for kstrlcpy */ // MODIFIED

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.retreg = ps_str; /* a.k.a r0*/
}

static int usermapped_glo_index = -1,
	usermapped_index = -1, first_um_idx = -1;


/* defined in kernel.lds */
extern char usermapped_start, usermapped_end, usermapped_nonglo_start;

int arch_phys_map(const int index,
			phys_bytes *addr,
			phys_bytes *len,
			int *flags)
{
	static int first = 1;
	kern_phys_map *phys_maps;

	int freeidx = 0;
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	if(first) {
		kmemset(&minix_kerninfo, 0, sizeof(minix_kerninfo)); // MODIFIED
		kmemset(&minix_kerninfo, 0, sizeof(minix_kerninfo)); // MODIFIED
		if(glo_len > 0) {
			usermapped_glo_index = freeidx++;
		}

		usermapped_index = freeidx++;
		first_um_idx = usermapped_index;
		if(usermapped_glo_index != -1)
			first_um_idx = usermapped_glo_index;
		first = 0;

		/* list over the maps and index them */
		phys_maps = kern_phys_map_head;
		while(phys_maps != NULL){
			phys_maps->index = freeidx++;
			phys_maps = phys_maps->next;
		}

	}

	if(index == usermapped_glo_index) {
		*addr = vir2phys(&usermapped_start);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		return OK;
	}
	else if(index == usermapped_index) {
		*addr = vir2phys(&usermapped_nonglo_start);
		*len = (u32_t) &usermapped_end -
			(u32_t) &usermapped_nonglo_start;
		*flags = VMMF_USER;
		return OK;
	}

	/* if this all fails loop over the maps */
	phys_maps = kern_phys_map_head;
	while(phys_maps != NULL){
		if(phys_maps->index == index){
			*addr = phys_maps->addr;
			*len =  phys_maps->size;
			*flags = phys_maps->vm_flags;
			return OK;
		}
		phys_maps = phys_maps->next;
	}

	return EINVAL;
}

int arch_phys_map_reply(const int index, const vir_bytes addr)
{
	kern_phys_map *phys_maps;

	if(index == first_um_idx) {
		u32_t usermapped_offset;
		KASSERT(addr > (u32_t) &usermapped_start);
		usermapped_offset = addr - (u32_t) &usermapped_start;
#define FIXEDPTR(ptr) (void *) ((u32_t)ptr + usermapped_offset)
#define FIXPTR(ptr) ptr = FIXEDPTR(ptr)
#define ASSIGN(minixstruct) minix_kerninfo.minixstruct = FIXEDPTR(&minixstruct)
		ASSIGN(kinfo);
		ASSIGN(machine);
		ASSIGN(kmessages);
		ASSIGN(loadinfo);
		ASSIGN(kuserinfo);
		ASSIGN(arm_frclock);
		ASSIGN(kclockinfo);

		/* adjust the pointers of the functions and the struct
		 * itself to the user-accessible mapping
		 */
		minix_kerninfo.kerninfo_magic = KERNINFO_MAGIC;
		minix_kerninfo.minix_feature_flags = minix_feature_flags;
		minix_kerninfo_user = (vir_bytes) FIXEDPTR(&minix_kerninfo);

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		return OK;
	}

	if (index == usermapped_index) {
		return OK;
	}

	/* if this all fails loop over the maps */
	/* list over the maps and index them */
	phys_maps = kern_phys_map_head;
	while(phys_maps != NULL){
		if(phys_maps->index == index){
			KASSERT(phys_maps->cb != NULL);
			/* only update the vir addr we are
			   going to call the callback in enable
			   paging
			*/
			phys_maps->vir = addr;
			return OK;
		}
		phys_maps = phys_maps->next;
	}

	return EINVAL;
}

int arch_enable_paging(struct proc * caller)
{
	kern_phys_map *phys_maps;
	KASSERT(caller->p_seg.p_ttbr);


	/* load caller's page table */
	switch_address_space(caller);

	/* We have now switched address spaces and the mappings are
	   valid. We can now remap previous mappings. This is not a 
	   good time to do printf as the initial massing is gone and
	   the new mapping is not in place */
	phys_maps = kern_phys_map_head;
	while(phys_maps != NULL){
		KASSERT(phys_maps->cb != NULL);
		phys_maps->cb(phys_maps->id, phys_maps->vir);
		phys_maps = phys_maps->next;
	}

	return OK;
}

void release_address_space(struct proc *pr)
{
	pr->p_seg.p_ttbr_v = NULL;
	barrier();
}



/*
 * Request a physical mapping
 */
int kern_req_phys_map( phys_bytes base_address, vir_bytes io_size, 
		       int vm_flags, kern_phys_map * priv, 
		       kern_phys_map_mapped cb, vir_bytes id)
{
	/* Assign the values to the given struct and add priv
	to the list */
	KASSERT(base_address != 0);
	KASSERT(io_size % ARM_PAGE_SIZE == 0);
	KASSERT(cb != NULL);

	priv->addr  = base_address;
	priv->size  = io_size;
	priv->vm_flags  = vm_flags;
	priv->cb  = cb;
	priv->id  = id;
	priv->index = -1;
	priv->next = NULL;
	

	if (kern_phys_map_head == NULL){
		/* keep a list of items this is the first one */
		kern_phys_map_head = priv;
		kern_phys_map_head->next = NULL;
	} else {
		/* insert the item head but first keep track
		   of the current by putting it in next */
		priv->next = kern_phys_map_head;
		/* replace the head */
		kern_phys_map_head = priv;
	}
	return 0;
}

/*
 * Callback implementation where the id given to the
 * kern_phys_map is a pointer to the io map base address.
 * this implementation will just change that base address.
 * once that area is remapped.
 */
int kern_phys_map_mapped_ptr(vir_bytes id, phys_bytes address){
	*((vir_bytes*)id) = address;
	return 0;
}

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
int kern_phys_map_ptr(
	phys_bytes base_address, 
	vir_bytes io_size, 
	int vm_flags, 
	kern_phys_map * priv,
	vir_bytes ptr)
{
	return kern_req_phys_map(base_address,io_size,vm_flags,priv,kern_phys_map_mapped_ptr,ptr);
}



/* ============================================================================
 * SOURCE 7/7: minix4\exokernel\kernel_legacy\arch\i386\memory.c
 * Size: 34,048 bytes, Lines: 1,093
 * Hash: a21674c42d96...
 * ============================================================================ */

#include "kernel/kernel.h"
#include "kernel/vm.h"

#include <machine/vm.h> // Kept (appears twice, will be one)

// #include <minix/syslib.h> // Removed
#include <minix/cpufeature.h> // Kept
// #include <string.h>       // Replaced
// #include <assert.h>       // Replaced
// #include <signal.h>       // Removed
// #include <stdlib.h>       // Removed

// Added kernel headers
#include <minix/kernel_types.h> // For k_size_t, k_errno_t, fixed-width types
#include <sys/kassert.h>
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <klib/include/kmemory.h>


#include "oxpcie.h"
#include "arch_proto.h"

#ifdef USE_APIC
#include "apic.h"
#ifdef USE_WATCHDOG
#include "kernel/watchdog.h"
#endif
#endif

phys_bytes video_mem_vaddr = 0;

#define HASPT(procptr) ((procptr)->p_seg.p_cr3 != 0)
static int nfreepdes = 0;
#define MAXFREEPDES	2
static int freepdes[MAXFREEPDES];

static u32_t phys_get32(phys_bytes v);

void mem_clear_mapcache(void)
{
	int i;
	for(i = 0; i < nfreepdes; i++) {
		struct proc *ptproc = get_cpulocal_var(ptproc);
		int pde = freepdes[i];
		u32_t *ptv;
		KASSERT(ptproc);
		ptv = ptproc->p_seg.p_cr3_v;
		KASSERT(ptv);
		ptv[pde] = 0;
	}
}

/* This function sets up a mapping from within the kernel's address
 * space to any other area of memory, either straight physical
 * memory (pr == NULL) or a process view of memory, in 4MB windows.
 * I.e., it maps in 4MB chunks of virtual (or physical) address space
 * to 4MB chunks of kernel virtual address space.
 *
 * It recognizes pr already being in memory as a special case (no
 * mapping required).
 *
 * The target (i.e. in-kernel) mapping area is one of the freepdes[]
 * VM has earlier already told the kernel about that is available. It is
 * identified as the 'pde' parameter. This value can be chosen freely
 * by the caller, as long as it is in range (i.e. 0 or higher and corresponds
 * to a known freepde slot). It is up to the caller to keep track of which
 * freepde's are in use, and to determine which ones are free to use.
 *
 * The logical number supplied by the caller is translated into an actual
 * pde number to be used, and a pointer to it (linear address) is returned
 * for actual use by phys_copy or memset.
 */
static phys_bytes createpde(
	const struct proc *pr,	/* Requested process, NULL for physical. */
	const phys_bytes linaddr,/* Address after segment translation. */
	phys_bytes *bytes,	/* Size of chunk, function may truncate it. */
	int free_pde_idx,	/* index of the free slot to use */
	int *changed		/* If mapping is made, this is set to 1. */
	)
{
	u32_t pdeval;
	phys_bytes offset;
	int pde;

	KASSERT(free_pde_idx >= 0 && free_pde_idx < nfreepdes);
	pde = freepdes[free_pde_idx];
	KASSERT(pde >= 0 && pde < 1024);

	if(pr && ((pr == get_cpulocal_var(ptproc)) || iskernelp(pr))) {
		/* Process memory is requested, and
		 * it's a process that is already in current page table, or
		 * the kernel, which is always there.
		 * Therefore linaddr is valid directly, with the requested
		 * size.
		 */
		return linaddr;
	}

	if(pr) {
		/* Requested address is in a process that is not currently
		 * accessible directly. Grab the PDE entry of that process'
		 * page table that corresponds to the requested address.
		 */
		KASSERT(pr->p_seg.p_cr3_v);
		pdeval = pr->p_seg.p_cr3_v[I386_VM_PDE(linaddr)];
	} else {
		/* Requested address is physical. Make up the PDE entry. */
		pdeval = (linaddr & I386_VM_ADDR_MASK_4MB) | 
			I386_VM_BIGPAGE | I386_VM_PRESENT | 
			I386_VM_WRITE | I386_VM_USER;
	}

	/* Write the pde value that we need into a pde that the kernel
	 * can access, into the currently loaded page table so it becomes
	 * visible.
	 */
	KASSERT(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	if(get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] != pdeval) {
		get_cpulocal_var(ptproc)->p_seg.p_cr3_v[pde] = pdeval;
		*changed = 1;
	}

	/* Memory is now available, but only the 4MB window of virtual
	 * address space that we have mapped; calculate how much of
	 * the requested range is visible and return that in *bytes,
	 * if that is less than the requested range.
	 */
	offset = linaddr & I386_VM_OFFSET_MASK_4MB; /* Offset in 4MB window. */
	*bytes = MIN(*bytes, I386_BIG_PAGE_SIZE - offset); 

	/* Return the linear address of the start of the new mapping. */
	return I386_BIG_PAGE_SIZE*pde + offset;
}


/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
static int check_resumed_caller(struct proc *caller)
{
	/* Returns the result from VM if caller was resumed, otherwise OK. */
	if (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
		KASSERT(caller->p_vmrequest.vmresult != VMSUSPEND);
		return caller->p_vmrequest.vmresult;
	}

	return OK;
}
  
/*===========================================================================*
 *				lin_lin_copy				     *
 *===========================================================================*/
static int lin_lin_copy(struct proc *srcproc, vir_bytes srclinaddr, 
	struct proc *dstproc, vir_bytes dstlinaddr, vir_bytes bytes)
{
	u32_t addr;
	proc_nr_t procslot;

	KASSERT(get_cpulocal_var(ptproc));
	KASSERT(get_cpulocal_var(proc_ptr));
	KASSERT(read_cr3() == get_cpulocal_var(ptproc)->p_seg.p_cr3);

	procslot = get_cpulocal_var(ptproc)->p_nr;

	KASSERT(procslot >= 0 && procslot < I386_VM_DIR_ENTRIES);

	if(srcproc) KASSERT(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	if(dstproc) KASSERT(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	KASSERT(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	KASSERT(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	if(srcproc) KASSERT(!RTS_ISSET(srcproc, RTS_VMINHIBIT));
	if(dstproc) KASSERT(!RTS_ISSET(dstproc, RTS_VMINHIBIT));

	while(bytes > 0) {
		phys_bytes srcptr, dstptr;
		vir_bytes chunk = bytes;
		int changed = 0;

#ifdef CONFIG_SMP
		unsigned cpu = cpuid;

		if (srcproc && GET_BIT(srcproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(srcproc->p_stale_tlb, cpu);
		}
		if (dstproc && GET_BIT(dstproc->p_stale_tlb, cpu)) {
			changed = 1;
			UNSET_BIT(dstproc->p_stale_tlb, cpu);
		}
#endif

		/* Set up 4MB ranges. */
		srcptr = createpde(srcproc, srclinaddr, &chunk, 0, &changed);
		dstptr = createpde(dstproc, dstlinaddr, &chunk, 1, &changed);
		if(changed)
			reload_cr3(); 

		/* Check for overflow. */
		if (srcptr + chunk < srcptr) return EFAULT_SRC; // EFAULT_SRC might be undefined
		if (dstptr + chunk < dstptr) return EFAULT_DST; // EFAULT_DST might be undefined
		if (srcptr + chunk < srcptr) return EFAULT_SRC; // EFAULT_SRC might be undefined
		if (dstptr + chunk < dstptr) return EFAULT_DST; // EFAULT_DST might be undefined

		/* Copy pages. */
		PHYS_COPY_CATCH(srcptr, dstptr, chunk, addr);

		if(addr) {
			/* If addr is nonzero, a page fault was caught. */

			if(addr >= srcptr && addr < (srcptr + chunk)) {
				return EFAULT_SRC; // EFAULT_SRC might be undefined
				return EFAULT_SRC; // EFAULT_SRC might be undefined
			}
			if(addr >= dstptr && addr < (dstptr + chunk)) {
				return EFAULT_DST; // EFAULT_DST might be undefined
				return EFAULT_DST; // EFAULT_DST might be undefined
			}

			panic("lin_lin_copy fault out of range");

			/* Not reached. */
			return EFAULT; // EFAULT might be undefined
			return EFAULT; // EFAULT might be undefined
		}

		/* Update counter and addresses for next iteration, if any. */
		bytes -= chunk;
		srclinaddr += chunk;
		dstlinaddr += chunk;
	}

	if(srcproc) KASSERT(!RTS_ISSET(srcproc, RTS_SLOT_FREE));
	if(dstproc) KASSERT(!RTS_ISSET(dstproc, RTS_SLOT_FREE));
	KASSERT(!RTS_ISSET(get_cpulocal_var(ptproc), RTS_SLOT_FREE));
	KASSERT(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);

	return OK;
}


static u32_t phys_get32(phys_bytes addr)
{
	u32_t v;
	int r;

	if((r=lin_lin_copy(NULL, addr, // NULL might be undefined
	if((r=lin_lin_copy(NULL, addr, // NULL might be undefined
		proc_addr(SYSTEM), (phys_bytes) &v, sizeof(v))) != OK) {
		panic("lin_lin_copy for phys_get32 failed: %d",  r);
	}

	return v;
}

#if 0
static char *cr0_str(u32_t e)
{
	static char str[80];
	(void)kstrlcpy(str, "", sizeof(str)); /* FIXME: strcpy was here ... */ // MODIFIED
#define FLAG(v) do { if(e & (v)) { /* FIXME: strcat was here */ /* kstrcat(str, #v " ", sizeof(str)); */ e &= ~v; } } while(0)
	(void)kstrlcpy(str, "", sizeof(str)); /* FIXME: strcpy was here ... */ // MODIFIED
#define FLAG(v) do { if(e & (v)) { /* FIXME: strcat was here */ /* kstrcat(str, #v " ", sizeof(str)); */ e &= ~v; } } while(0)
	FLAG(I386_CR0_PE);
	FLAG(I386_CR0_MP);
	FLAG(I386_CR0_EM);
	FLAG(I386_CR0_TS);
	FLAG(I386_CR0_ET);
	FLAG(I386_CR0_PG);
	FLAG(I386_CR0_WP);
	if(e) { /* FIXME: strcat was here */ /* kstrcat(str, " (++)", sizeof(str)); */ }
	if(e) { /* FIXME: strcat was here */ /* kstrcat(str, " (++)", sizeof(str)); */ }
	return str;
}

static char *cr4_str(u32_t e)
{
	static char str[80];
	(void)kstrlcpy(str, "", sizeof(str)); /* FIXME: strcpy was here ... */ // MODIFIED
	(void)kstrlcpy(str, "", sizeof(str)); /* FIXME: strcpy was here ... */ // MODIFIED
	FLAG(I386_CR4_VME);
	FLAG(I386_CR4_PVI);
	FLAG(I386_CR4_TSD);
	FLAG(I386_CR4_DE);
	FLAG(I386_CR4_PSE);
	FLAG(I386_CR4_PAE);
	FLAG(I386_CR4_MCE);
	FLAG(I386_CR4_PGE);
	if(e) { /* FIXME: strcat was here */ /* kstrcat(str, " (++)", sizeof(str)); */ }
	if(e) { /* FIXME: strcat was here */ /* kstrcat(str, " (++)", sizeof(str)); */ }
	return str;
}
#endif

/*===========================================================================*
 *                              umap_virtual                                 *
 *===========================================================================*/
phys_bytes umap_virtual(
  register struct proc *rp,		/* pointer to proc table entry for process */
  int seg,				/* T, D, or S segment */
  vir_bytes vir_addr,			/* virtual address in bytes within the seg */
  vir_bytes bytes			/* # of bytes to be copied */
)
{
	phys_bytes phys = 0;

	if(vm_lookup(rp, vir_addr, &phys, NULL) != OK) { // NULL might be undefined
		kprintf_stub("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr); // MODIFIED
	if(vm_lookup(rp, vir_addr, &phys, NULL) != OK) { // NULL might be undefined
		kprintf_stub("SYSTEM:umap_virtual: vm_lookup of %s: seg 0x%x: 0x%lx failed\n", rp->p_name, seg, vir_addr); // MODIFIED
		phys = 0;
	} else {
		if(phys == 0)
			panic("vm_lookup returned phys: 0x%lx",  phys);
	}

	if(phys == 0) {
		kprintf_stub("SYSTEM:umap_virtual: lookup failed\n"); // MODIFIED
		kprintf_stub("SYSTEM:umap_virtual: lookup failed\n"); // MODIFIED
		return 0;
	}

	/* Now make sure addresses are contiguous in physical memory
	 * so that the umap makes sense.
	 */
	if(bytes > 0 && vm_lookup_range(rp, vir_addr, NULL, bytes) != bytes) { // NULL might be undefined
		kprintf_stub("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n", // MODIFIED
			rp->p_name, (unsigned long)bytes, vir_addr, vir_addr); // MODIFIED k_size_t cast for %lu
	if(bytes > 0 && vm_lookup_range(rp, vir_addr, NULL, bytes) != bytes) { // NULL might be undefined
		kprintf_stub("umap_virtual: %s: %lu at 0x%lx (vir 0x%lx) not contiguous\n", // MODIFIED
			rp->p_name, (unsigned long)bytes, vir_addr, vir_addr); // MODIFIED k_size_t cast for %lu
		return 0;
	}

	/* phys must be larger than 0 (or the caller will think the call
	 * failed), and address must not cross a page boundary.
	 */
	KASSERT(phys);

	return phys;
}


/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
int vm_lookup(const struct proc *proc, const vir_bytes virtual,
 phys_bytes *physical, u32_t *ptent)
{
	u32_t *root, *pt;
	int pde, pte;
	u32_t pde_v, pte_v;

	KASSERT(proc);
	KASSERT(physical);
	KASSERT(!isemptyp(proc));
	KASSERT(HASPT(proc));

	/* Retrieve page directory entry. */
	root = (u32_t *) proc->p_seg.p_cr3;
	KASSERT(!((u32_t) root % I386_PAGE_SIZE));
	pde = I386_VM_PDE(virtual);
	KASSERT(pde >= 0 && pde < I386_VM_DIR_ENTRIES);
	pde_v = phys_get32((u32_t) (root + pde));

	if(!(pde_v & I386_VM_PRESENT)) {
		return EFAULT; // EFAULT might be undefined
		return EFAULT; // EFAULT might be undefined
	}

	/* We don't expect to ever see this. */
	if(pde_v & I386_VM_BIGPAGE) {
		*physical = pde_v & I386_VM_ADDR_MASK_4MB;
		if(ptent) *ptent = pde_v;
		*physical += virtual & I386_VM_OFFSET_MASK_4MB;
	} else {
		/* Retrieve page table entry. */
		pt = (u32_t *) I386_VM_PFA(pde_v);
		KASSERT(!((u32_t) pt % I386_PAGE_SIZE));
		pte = I386_VM_PTE(virtual);
		KASSERT(pte >= 0 && pte < I386_VM_PT_ENTRIES);
		pte_v = phys_get32((u32_t) (pt + pte));
		if(!(pte_v & I386_VM_PRESENT)) {
			return EFAULT; // EFAULT might be undefined
			return EFAULT; // EFAULT might be undefined
		}

		if(ptent) *ptent = pte_v;

		/* Actual address now known; retrieve it and add page offset. */
		*physical = I386_VM_PFA(pte_v);
		*physical += virtual % I386_PAGE_SIZE;
	}

	return OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
k_size_t vm_lookup_range(const struct proc *proc, vir_bytes vir_addr, // MODIFIED size_t
	phys_bytes *phys_addr, k_size_t bytes) // MODIFIED size_t
k_size_t vm_lookup_range(const struct proc *proc, vir_bytes vir_addr, // MODIFIED size_t
	phys_bytes *phys_addr, k_size_t bytes) // MODIFIED size_t
{
	/* Look up the physical address corresponding to linear virtual address
	 * 'vir_addr' for process 'proc'. Return the size of the range covered
	 * by contiguous physical memory starting from that address; this may
	 * be anywhere between 0 and 'bytes' inclusive. If the return value is
	 * nonzero, and 'phys_addr' is non-NULL, 'phys_addr' will be set to the
	 * base physical address of the range. 'vir_addr' and 'bytes' need not
	 * be page-aligned, but the caller must have verified that the given
	 * linear range is valid for the given process at all.
	 */
	phys_bytes phys, next_phys;
	k_size_t len; // MODIFIED size_t
	k_size_t len; // MODIFIED size_t

	KASSERT(proc);
	KASSERT(bytes > 0);
	KASSERT(HASPT(proc));

	/* Look up the first page. */
	if (vm_lookup(proc, vir_addr, &phys, NULL) != OK) // NULL might be undefined
	if (vm_lookup(proc, vir_addr, &phys, NULL) != OK) // NULL might be undefined
		return 0;

	if (phys_addr != NULL) // NULL might be undefined
	if (phys_addr != NULL) // NULL might be undefined
		*phys_addr = phys;

	len = I386_PAGE_SIZE - (vir_addr % I386_PAGE_SIZE);
	vir_addr += len;
	next_phys = phys + len;

	/* Look up any next pages and test physical contiguity. */
	while (len < bytes) {
		if (vm_lookup(proc, vir_addr, &phys, NULL) != OK) // NULL might be undefined
		if (vm_lookup(proc, vir_addr, &phys, NULL) != OK) // NULL might be undefined
			break;

		if (next_phys != phys)
			break;

		len += I386_PAGE_SIZE;
		vir_addr += I386_PAGE_SIZE;
		next_phys += I386_PAGE_SIZE;
	}

	/* We might now have overshot the requested length somewhat. */
	return MIN(bytes, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
int vm_check_range(struct proc *caller, struct proc *target,
	vir_bytes vir_addr, k_size_t bytes, int writeflag) // MODIFIED size_t
	vir_bytes vir_addr, k_size_t bytes, int writeflag) // MODIFIED size_t
{
	/* Public interface to vm_suspend(), for use by kernel calls. On behalf
	 * of 'caller', call into VM to check linear virtual address range of
	 * process 'target', starting at 'vir_addr', for 'bytes' bytes. This
	 * function assumes that it will called twice if VM returned an error
	 * the first time (since nothing has changed in that case), and will
	 * then return the error code resulting from the first call. Upon the
	 * first call, a non-success error code is returned as well.
	 */
	int r;

	if ((caller->p_misc_flags & MF_KCALL_RESUME) &&
			(r = caller->p_vmrequest.vmresult) != OK)
		return r;

	vm_suspend(caller, target, vir_addr, bytes, VMSTYPE_KERNELCALL,
		writeflag);

	return VMSUSPEND;
}

#if 0
static char *flagstr(u32_t e, const int dir)
{
	static char str[80];
	(void)kstrlcpy(str, "", sizeof(str)); /* FIXME: strcpy was here ... */ // MODIFIED
#define FLAG(v) do { if(e & (v)) { /* FIXME: strcat was here */ /* kstrcat(str, #v " ", sizeof(str)); */ e &= ~v; } } while(0)
	(void)kstrlcpy(str, "", sizeof(str)); /* FIXME: strcpy was here ... */ // MODIFIED
#define FLAG(v) do { if(e & (v)) { /* FIXME: strcat was here */ /* kstrcat(str, #v " ", sizeof(str)); */ e &= ~v; } } while(0)
	FLAG(I386_VM_PRESENT);
	FLAG(I386_VM_WRITE);
	FLAG(I386_VM_USER);
	FLAG(I386_VM_PWT);
	FLAG(I386_VM_PCD);
	FLAG(I386_VM_GLOBAL);
	if(dir)
		FLAG(I386_VM_BIGPAGE);	/* Page directory entry only */
	else
		FLAG(I386_VM_DIRTY);	/* Page table entry only */
	return str;
}

static void vm_pt_print(u32_t *pagetable, const u32_t v)
{
	int pte;
	int col = 0;

	KASSERT(!((u32_t) pagetable % I386_PAGE_SIZE));

	for(pte = 0; pte < I386_VM_PT_ENTRIES; pte++) {
		u32_t pte_v, pfa;
		pte_v = phys_get32((u32_t) (pagetable + pte));
		if(!(pte_v & I386_VM_PRESENT))
			continue;
		pfa = I386_VM_PFA(pte_v);
		kprintf_stub("%4d:%08lx:%08lx %2s ", // MODIFIED
		kprintf_stub("%4d:%08lx:%08lx %2s ", // MODIFIED
			pte, v + I386_PAGE_SIZE*pte, pfa,
			(pte_v & I386_VM_WRITE) ? "rw":"RO");
		col++;
		if(col == 3) { kprintf_stub("\n"); col = 0; } // MODIFIED
		if(col == 3) { kprintf_stub("\n"); col = 0; } // MODIFIED
	}
	if(col > 0) kprintf_stub("\n"); // MODIFIED
	if(col > 0) kprintf_stub("\n"); // MODIFIED

	return;
}

static void vm_print(u32_t *root)
{
	int pde;

	KASSERT(!((u32_t) root % I386_PAGE_SIZE));

	kprintf_stub("page table 0x%lx:\n", root); // MODIFIED
	kprintf_stub("page table 0x%lx:\n", root); // MODIFIED

	for(pde = 0; pde < I386_VM_DIR_ENTRIES; pde++) {
		u32_t pde_v;
		u32_t *pte_a;
		pde_v = phys_get32((u32_t) (root + pde));
		if(!(pde_v & I386_VM_PRESENT))
			continue;
		if(pde_v & I386_VM_BIGPAGE) {
			kprintf_stub("%4d: 0x%lx, flags %s\n", // MODIFIED
			kprintf_stub("%4d: 0x%lx, flags %s\n", // MODIFIED
				pde, I386_VM_PFA(pde_v), flagstr(pde_v, 1));
		} else {
			pte_a = (u32_t *) I386_VM_PFA(pde_v);
			kprintf_stub("%4d: pt %08lx %s\n", // MODIFIED
			kprintf_stub("%4d: pt %08lx %s\n", // MODIFIED
				pde, pte_a, flagstr(pde_v, 1));
			vm_pt_print(pte_a, pde * I386_VM_PT_ENTRIES * I386_PAGE_SIZE);
			kprintf_stub("\n"); // MODIFIED
			kprintf_stub("\n"); // MODIFIED
		}
	}


	return;
}
#endif

/*===========================================================================*
 *                                 vmmemset                                  *
 *===========================================================================*/
int vm_memset(struct proc* caller, endpoint_t who, phys_bytes ph, int c,
	phys_bytes count)
{
	u32_t pattern;
	struct proc *whoptr = NULL; // NULL might be undefined
	struct proc *whoptr = NULL; // NULL might be undefined
	phys_bytes cur_ph = ph;
	phys_bytes left = count;
	phys_bytes ptr, chunk, pfa = 0;
	int new_cr3, r = OK;

	if ((r = check_resumed_caller(caller)) != OK)
		return r;

	/* NONE for physical, otherwise virtual */
	if (who != NONE && !(whoptr = endpoint_lookup(who)))
		return ESRCH; // ESRCH might be undefined
		return ESRCH; // ESRCH might be undefined

	c &= 0xFF;
	pattern = c | (c << 8) | (c << 16) | (c << 24);

	KASSERT(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	KASSERT(!catch_pagefaults);
	catch_pagefaults = 1;

	/* We can memset as many bytes as we have remaining,
	 * or as many as remain in the 4MB chunk we mapped in.
	 */
	while (left > 0) {
		new_cr3 = 0;
		chunk = left;
		ptr = createpde(whoptr, cur_ph, &chunk, 0, &new_cr3);

		if (new_cr3)
			reload_cr3();

		/* If a page fault happens, pfa is non-null */
		if ((pfa = phys_memset(ptr, pattern, chunk))) {

			/* If a process pagefaults, VM may help out */
			if (whoptr) {
				vm_suspend(caller, whoptr, ph, count,
						   VMSTYPE_KERNELCALL, 1);
				KASSERT(catch_pagefaults);
				catch_pagefaults = 0;
				return VMSUSPEND;
			}

			/* Pagefault when phys copying ?! */
			panic("vm_memset: pf %lx addr=%lx len=%lu\n",
						pfa , ptr, chunk);
		}

		cur_ph += chunk;
		left -= chunk;
	}

	KASSERT(get_cpulocal_var(ptproc)->p_seg.p_cr3_v);
	KASSERT(catch_pagefaults);
	catch_pagefaults = 0;

	return OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
int virtual_copy_f(
  struct proc * caller,
  struct vir_addr *src_addr,		/* source virtual address */
  struct vir_addr *dst_addr,		/* destination virtual address */
  vir_bytes bytes,			/* # of bytes to copy  */
  int vmcheck				/* if nonzero, can return VMSUSPEND */
  struct vir_addr *src_addr,		/* source virtual address */
  struct vir_addr *dst_addr,		/* destination virtual address */
  vir_bytes bytes,			/* # of bytes to copy  */
  int vmcheck				/* if nonzero, can return VMSUSPEND */
)
{
/* Copy bytes from virtual address src_addr to virtual address dst_addr. */
  struct vir_addr *vir_addr[2];	/* virtual source and destination address */
  int i, r;
  struct proc *procs[2];

  KASSERT((vmcheck && caller) || (!vmcheck && !caller));

  /* Check copy count. */
  if (bytes <= 0) return(EDOM); // EDOM might be undefined
  if (bytes <= 0) return(EDOM); // EDOM might be undefined

  /* Do some more checks and map virtual addresses to physical addresses. */
  vir_addr[_SRC_] = src_addr;
  vir_addr[_DST_] = dst_addr;

  for (i=_SRC_; i<=_DST_; i++) {
  	endpoint_t proc_e = vir_addr[i]->proc_nr_e;
	int proc_nr;
	struct proc *p;

	if(proc_e == NONE) {
		p = NULL; // NULL might be undefined
		p = NULL; // NULL might be undefined
	} else {
		if(!isokendpt(proc_e, &proc_nr)) {
			kprintf_stub("virtual_copy: no reasonable endpoint\n"); // MODIFIED
			return ESRCH; // ESRCH might be undefined
			kprintf_stub("virtual_copy: no reasonable endpoint\n"); // MODIFIED
			return ESRCH; // ESRCH might be undefined
		}
		p = proc_addr(proc_nr);
	}

	procs[i] = p;
  }

  if ((r = check_resumed_caller(caller)) != OK)
	return r;

  if((r=lin_lin_copy(procs[_SRC_], vir_addr[_SRC_]->offset,
  	procs[_DST_], vir_addr[_DST_]->offset, bytes)) != OK) {
	int writeflag;
	struct proc *target = NULL; // NULL might be undefined
	struct proc *target = NULL; // NULL might be undefined
  	phys_bytes lin;
	if(r != EFAULT_SRC && r != EFAULT_DST) // EFAULT* might be undefined
	if(r != EFAULT_SRC && r != EFAULT_DST) // EFAULT* might be undefined
  		panic("lin_lin_copy failed: %d",  r);
  	if(!vmcheck || !caller) {
    		return r;
  	}

	if(r == EFAULT_SRC) { // EFAULT_SRC might be undefined
	if(r == EFAULT_SRC) { // EFAULT_SRC might be undefined
  		lin = vir_addr[_SRC_]->offset;
  		target = procs[_SRC_];
		writeflag = 0;
	} else if(r == EFAULT_DST) { // EFAULT_DST might be undefined
	} else if(r == EFAULT_DST) { // EFAULT_DST might be undefined
  		lin = vir_addr[_DST_]->offset;
  		target = procs[_DST_];
		writeflag = 1;
  	} else {
  		panic("r strange: %d",  r);
  	}

	KASSERT(caller);
	KASSERT(target);

	vm_suspend(caller, target, lin, bytes, VMSTYPE_KERNELCALL, writeflag);
	return VMSUSPEND;
  }

  return OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
int data_copy(const endpoint_t from_proc, const vir_bytes from_addr,
	const endpoint_t to_proc, const vir_bytes to_addr,
	k_size_t bytes) // MODIFIED size_t
	k_size_t bytes) // MODIFIED size_t
{
  struct vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  KASSERT(src.proc_nr_e != NONE);
  KASSERT(dst.proc_nr_e != NONE);

  return virtual_copy(&src, &dst, bytes);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
int data_copy_vmcheck(struct proc * caller,
	const endpoint_t from_proc, const vir_bytes from_addr,
	const endpoint_t to_proc, const vir_bytes to_addr,
	k_size_t bytes) // MODIFIED size_t
	k_size_t bytes) // MODIFIED size_t
{
  struct vir_addr src, dst;

  src.offset = from_addr;
  dst.offset = to_addr;
  src.proc_nr_e = from_proc;
  dst.proc_nr_e = to_proc;
  KASSERT(src.proc_nr_e != NONE);
  KASSERT(dst.proc_nr_e != NONE);

  return virtual_copy_vmcheck(caller, &src, &dst, bytes);
}

void memory_init(void)
{
	KASSERT(nfreepdes == 0);

	freepdes[nfreepdes++] = kinfo.freepde_start++;
	freepdes[nfreepdes++] = kinfo.freepde_start++;

	KASSERT(kinfo.freepde_start < I386_VM_DIR_ENTRIES);
	KASSERT(nfreepdes == 2);
	KASSERT(nfreepdes <= MAXFREEPDES);
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
void arch_proc_init(struct proc *pr, const u32_t ip, const u32_t sp,
	const u32_t ps_str, char *name)
{
	arch_proc_reset(pr);
	kstrlcpy(pr->p_name, name, sizeof(pr->p_name)); // MODIFIED
	kstrlcpy(pr->p_name, name, sizeof(pr->p_name)); // MODIFIED

	/* set custom state we know */
	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	pr->p_reg.bx = ps_str;
}

static int oxpcie_mapping_index = -1,
	lapic_mapping_index = -1,
	ioapic_first_index = -1,
	ioapic_last_index = -1,
	video_mem_mapping_index = -1,
	usermapped_glo_index = -1,
	usermapped_index = -1, first_um_idx = -1;

extern char *video_mem;

extern char usermapped_start, usermapped_end, usermapped_nonglo_start;

int arch_phys_map(const int index,
			phys_bytes *addr,
			phys_bytes *len,
			int *flags)
{
	static int first = 1;
	int freeidx = 0;
	static char *ser_var = NULL; // NULL might be undefined
	static char *ser_var = NULL; // NULL might be undefined
	u32_t glo_len = (u32_t) &usermapped_nonglo_start -
			(u32_t) &usermapped_start;

	if(first) {
		kmemset(&minix_kerninfo, 0, sizeof(minix_kerninfo)); // MODIFIED
		kmemset(&minix_kerninfo, 0, sizeof(minix_kerninfo)); // MODIFIED
		video_mem_mapping_index = freeidx++;
		if(glo_len > 0) {
			usermapped_glo_index = freeidx++;
		}
		
		usermapped_index = freeidx++;
		first_um_idx = usermapped_index;
		if(usermapped_glo_index != -1)
			first_um_idx = usermapped_glo_index;

#ifdef USE_APIC
		if(lapic_addr)
			lapic_mapping_index = freeidx++;
		if (ioapic_enabled) {
			ioapic_first_index = freeidx;
			KASSERT(nioapics > 0);
			freeidx += nioapics;
			ioapic_last_index = freeidx-1;
		}
#endif

#ifdef CONFIG_OXPCIE
		if((ser_var = env_get("oxpcie"))) {
			if(ser_var[0] != '0' || ser_var[1] != 'x') {
				kprintf_stub("oxpcie address in hex please\n"); // MODIFIED
				kprintf_stub("oxpcie address in hex please\n"); // MODIFIED
			} else {
				kprintf_stub("oxpcie address is %s\n", ser_var); // MODIFIED
				kprintf_stub("oxpcie address is %s\n", ser_var); // MODIFIED
				oxpcie_mapping_index = freeidx++;
			}
		}
#endif

		first = 0;
	}

	if(index == usermapped_glo_index) {
		*addr = vir2phys(&usermapped_start);
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		return OK;
	}
	else if(index == usermapped_index) {
		*addr = vir2phys(&usermapped_nonglo_start);
		*len = (u32_t) &usermapped_end -
			(u32_t) &usermapped_nonglo_start;
		*flags = VMMF_USER;
		return OK;
	}
	else if (index == video_mem_mapping_index) {
		/* map video memory in so we can print panic messages */
		*addr = MULTIBOOT_VIDEO_BUFFER;
		*len = I386_PAGE_SIZE;
		*flags = VMMF_WRITE;
		return OK;
	}
#ifdef USE_APIC
	else if (index == lapic_mapping_index) {
		/* map the local APIC if enabled */
		if (!lapic_addr)
			return EINVAL; // EINVAL might be undefined
			return EINVAL; // EINVAL might be undefined
		*addr = lapic_addr;
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		return OK;
	}
	else if (ioapic_enabled && index >= ioapic_first_index && index <= ioapic_last_index) {
		int ioapic_idx = index - ioapic_first_index;
		*addr = io_apic[ioapic_idx].paddr;
		KASSERT(*addr);
		*len = 4 << 10 /* 4kB */;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		kprintf_stub("ioapic map: addr 0x%lx\n", *addr); // MODIFIED
		kprintf_stub("ioapic map: addr 0x%lx\n", *addr); // MODIFIED
		return OK;
	}
#endif

#if CONFIG_OXPCIE
	if(index == oxpcie_mapping_index) {
		*addr = 0 /* FIXME: strtoul(ser_var+2, NULL, 16) replaced */; // MODIFIED (NULL might be undefined)
		*addr = 0 /* FIXME: strtoul(ser_var+2, NULL, 16) replaced */; // MODIFIED (NULL might be undefined)
		*len = 0x4000;
		*flags = VMMF_UNCACHED | VMMF_WRITE;
		return OK;
	}
#endif

	return EINVAL; // EINVAL might be undefined
	return EINVAL; // EINVAL might be undefined
}

int arch_phys_map_reply(const int index, const vir_bytes addr)
{
#ifdef USE_APIC
	/* if local APIC is enabled */
	if (index == lapic_mapping_index && lapic_addr) {
		lapic_addr_vaddr = addr;
		return OK;
	}
	else if (ioapic_enabled && index >= ioapic_first_index &&
		index <= ioapic_last_index) {
		int i = index - ioapic_first_index;
		io_apic[i].vaddr = addr;
		return OK;
	}
#endif

#if CONFIG_OXPCIE
	if (index == oxpcie_mapping_index) {
		oxpcie_set_vaddr((unsigned char *) addr);
		return OK;
	}
#endif
	if(index == first_um_idx) {
		extern struct minix_ipcvecs minix_ipcvecs_sysenter,
			minix_ipcvecs_syscall,
			minix_ipcvecs_softint;
		extern u32_t usermapped_offset;
		KASSERT(addr > (u32_t) &usermapped_start);
		usermapped_offset = addr - (u32_t) &usermapped_start;
#define FIXEDPTR(ptr) (void *) ((u32_t)ptr + usermapped_offset)
#define FIXPTR(ptr) ptr = FIXEDPTR(ptr)
#define ASSIGN(minixstruct) minix_kerninfo.minixstruct = FIXEDPTR(&minixstruct)
		ASSIGN(kinfo);
		ASSIGN(machine);
		ASSIGN(kmessages);
		ASSIGN(loadinfo);
		ASSIGN(kuserinfo);
		ASSIGN(arm_frclock); /* eh, why not. */
		ASSIGN(kclockinfo);

		/* select the right set of IPC routines to map into processes */
		if(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
			DEBUGBASIC(("kernel: selecting intel sysenter ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_sysenter;
		} else  if(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
			DEBUGBASIC(("kernel: selecting amd syscall ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_syscall;
		} else	{
			DEBUGBASIC(("kernel: selecting fallback (int) ipc style\n"));
			minix_kerninfo.minix_ipcvecs = &minix_ipcvecs_softint;
		}

		/* adjust the pointers of the functions and the struct
		 * itself to the user-accessible mapping
		 */
		FIXPTR(minix_kerninfo.minix_ipcvecs->send);
		FIXPTR(minix_kerninfo.minix_ipcvecs->receive);
		FIXPTR(minix_kerninfo.minix_ipcvecs->sendrec);
		FIXPTR(minix_kerninfo.minix_ipcvecs->senda);
		FIXPTR(minix_kerninfo.minix_ipcvecs->sendnb);
		FIXPTR(minix_kerninfo.minix_ipcvecs->notify);
		FIXPTR(minix_kerninfo.minix_ipcvecs->do_kernel_call);
		FIXPTR(minix_kerninfo.minix_ipcvecs);

		minix_kerninfo.kerninfo_magic = KERNINFO_MAGIC;
		minix_kerninfo.minix_feature_flags = minix_feature_flags;
		minix_kerninfo_user = (vir_bytes) FIXEDPTR(&minix_kerninfo);

		/* if libc_ipc is set, disable usermapped ipc functions
		 * and force binaries to use in-libc fallbacks.
		 */
		if(env_get("libc_ipc")) {
			kprintf_stub("kernel: forcing in-libc fallback ipc style\n"); // MODIFIED
			minix_kerninfo.minix_ipcvecs = NULL; // NULL might be undefined
			kprintf_stub("kernel: forcing in-libc fallback ipc style\n"); // MODIFIED
			minix_kerninfo.minix_ipcvecs = NULL; // NULL might be undefined
		} else {
			minix_kerninfo.ki_flags |= MINIX_KIF_IPCVECS;
		}

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		return OK;
	}

	if(index == usermapped_index) return OK;

	if (index == video_mem_mapping_index) {
		video_mem_vaddr =  addr;
		return OK;
	}

	return EINVAL; // EINVAL might be undefined
	return EINVAL; // EINVAL might be undefined
}

int arch_enable_paging(struct proc * caller)
{
	KASSERT(caller->p_seg.p_cr3);

	/* load caller's page table */
	switch_address_space(caller);

	video_mem = (char *) video_mem_vaddr;

#ifdef USE_APIC
	/* start using the virtual addresses */

	/* if local APIC is enabled */
	if (lapic_addr) {
		lapic_addr = lapic_addr_vaddr;
		lapic_eoi_addr = LAPIC_EOI;
	}
	/* if IO apics are enabled */
	if (ioapic_enabled) {
		int i;

		for (i = 0; i < nioapics; i++) {
			io_apic[i].addr = io_apic[i].vaddr;
		}
	}
#if CONFIG_SMP
	barrier();

	wait_for_APs_to_finish_booting();
#endif
#endif

#ifdef USE_WATCHDOG
	/*
	 * We make sure that we don't enable the watchdog until paging is turned
	 * on as we might get an NMI while switching and we might still use wrong
	 * lapic address. Bad things would happen. It is unfortunate but such is
	 * life
	 */
	if (watchdog_enabled)
		i386_watchdog_start();
#endif

	return OK;
}

void release_address_space(struct proc *pr)
{
	pr->p_seg.p_cr3_v = NULL; // NULL might be undefined
	pr->p_seg.p_cr3_v = NULL; // NULL might be undefined
}

/* computes a checksum of a buffer of a given length. The byte sum must be zero */
int platform_tbl_checksum_ok(void *ptr, unsigned int length)
{
	u8_t total = 0; // u8_t might be undefined
	u8_t total = 0; // u8_t might be undefined
	unsigned int i;
	for (i = 0; i < length; i++)
		total += ((unsigned char *)ptr)[i];
	return !total;
}

int platform_tbl_ptr(phys_bytes start,
					phys_bytes end,
					unsigned increment,
					void * buff,
					unsigned size, // Should this be k_size_t?
					unsigned size, // Should this be k_size_t?
					phys_bytes * phys_addr,
					int ((* cmp_f)(void *)))
{
	phys_bytes addr;

	for (addr = start; addr < end; addr += increment) {
		phys_copy (addr, (phys_bytes) buff, size);
		if (cmp_f(buff)) {
			if (phys_addr)
				*phys_addr = addr;
			return 1;
		}
	}
	return 0;
}

