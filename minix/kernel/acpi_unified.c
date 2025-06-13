/**
 * @file acpi_unified.c
 * @brief Consolidated ACPI source files for analysis.
 */

/**
 * @section drivers_power_acpi_acpi_c
 * @brief Original file: minix/drivers/power/acpi/acpi.c
 */
#include <acpi.h>
#include <assert.h>
#include <minix/acpi.h>
#include <minix/driver.h>

#include "pci.h"

int acpi_enabled;
struct machine machine;

/* don't know where ACPI tables are, we may need to access any memory */
static int init_mem_priv(void) {
  struct minix_mem_range mr;

  mr.mr_base = 0;
  mr.mr_limit = 0xffffffff;

  return sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr);
}

static void set_machine_mode(void) {
  ACPI_OBJECT arg1;
  ACPI_OBJECT_LIST args;
  ACPI_STATUS as;

  arg1.Type = ACPI_TYPE_INTEGER;
  arg1.Integer.Value = machine.apic_enabled ? 1 : 0;
  args.Count = 1;
  args.Pointer = &arg1;

  as = AcpiEvaluateObject(ACPI_ROOT_OBJECT, "_PIC", &args, NULL);
  /*
   * We can silently ignore failure as it may not be implemented, ACPI should
   * provide us with correct information anyway
   */
  if (ACPI_SUCCESS(as))
    printf("ACPI: machine set to %s mode\n",
           machine.apic_enabled ? "APIC" : "PIC");
}

static ACPI_STATUS init_acpica(void) {
  ACPI_STATUS status;

  status = AcpiInitializeSubsystem();
  if (ACPI_FAILURE(status))
    return status;

  status = AcpiInitializeTables(NULL, 16, FALSE);
  if (ACPI_FAILURE(status))
    return status;

  status = AcpiLoadTables();
  if (ACPI_FAILURE(status))
    return status;

  status = AcpiEnableSubsystem(0);
  if (ACPI_FAILURE(status))
    return status;

  status = AcpiInitializeObjects(0);
  if (ACPI_FAILURE(status))
    return status;

  set_machine_mode();

  pci_scan_devices();

  return AE_OK;
}

void init_acpi(void) {
  ACPI_STATUS acpi_err;
  /* test conditions for acpi */
  if (sys_getmachine(&machine)) {
    printf("ACPI: no machine\n");
    return;
  }
  if (machine.acpi_rsdp == 0) {
    printf("ACPI: no RSDP\n");
    return;
  }
  if (init_mem_priv()) {
    printf("ACPI: no mem access\n");
    return;
  }

  if ((acpi_err = init_acpica()) == AE_OK) {
    acpi_enabled = 1;
    printf("ACPI: ACPI enabled\n");
  } else {
    acpi_enabled = 0;
    printf("ACPI: ACPI failed with err %d\n", acpi_err);
  }
}

static int sef_cb_init_fresh(int type, sef_init_info_t *info) {
  int r;

  init_acpi();

  /* Let SEF know about ACPI special cache word. */
  r = sef_llvm_add_special_mem_region((void *)0xCACACACA, 1,
                                      "%MMAP_CACHE_WORD");
  if (r < 0) {
    printf("acpi: sef_llvm_add_special_mem_region failed %d\n", r);
  }

  /* XXX To-do: acpi requires custom state transfer handlers for
   * unions acpi_operand_object and acpi_generic_state (and nested unions)
   * for generic state transfer to work correctly.
   */

  return OK;
}

static void sef_local_startup() {
  /* Register init callbacks. */
  sef_setcb_init_fresh(sef_cb_init_fresh);
  sef_setcb_init_lu(sef_cb_init_fresh);
  sef_setcb_init_restart(sef_cb_init_fresh);

  /* Let SEF perform startup. */
  sef_startup();
}

int main(void) {
  int err;
  message m;
  int ipc_status;

  sef_local_startup();

  for (;;) {
    err = driver_receive(ANY, &m, &ipc_status);
    if (err != OK) {
      printf("ACPI: driver_receive failed: %d\n", err);
      continue;
    }

    switch (((struct acpi_request_hdr *)&m)->request) {
    case ACPI_REQ_GET_IRQ:
      do_get_irq(&m);
      break;
    case ACPI_REQ_MAP_BRIDGE:
      do_map_bridge(&m);
      break;
    default:
      printf("ACPI: ignoring unsupported request %d "
             "from %d\n",
             ((struct acpi_request_hdr *)&m)->request,
             ((struct acpi_request_hdr *)&m)->m_source);
    }

    err = ipc_send(m.m_source, &m);
    if (err != OK) {
      printf("ACPI: ipc_send failed: %d\n", err);
    }
  }
}

/**
 * @section kernel_arch_i386_acpi_c
 * @brief Original file: minix/kernel/arch/i386/acpi.c
 */
// #include <string.h> // Replaced
// #include <string.h> // Replaced

#include "acpi.h"
#include "arch_proto.h"

// Added kernel headers
#include <klib/include/kmemory.h>
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <minix/kernel_types.h>

typedef int((*acpi_read_t)(phys_bytes addr, void *buff,
                           k_size_t size)); // MODIFIED size_t
// Added kernel headers
#include <klib/include/kmemory.h>
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <minix/kernel_types.h>

typedef int((*acpi_read_t)(phys_bytes addr, void *buff,
                           k_size_t size)); // MODIFIED size_t

struct acpi_rsdp acpi_rsdp;

static acpi_read_t read_func;

#define MAX_RSDT 35           /* ACPI defines 35 signatures */
#define SLP_EN_CODE (1 << 13) /* ACPI SLP_EN_CODE code */
#define AMI_PACKAGE_OP_CODE (0x12)
#define AMI_NAME_OP_CODE (0x8)
#define AMI_BYTE_PREFIX_CODE (0xA)
#define AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK (0xC0)
#define AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT (6)
#define AMI_MIN_PACKAGE_LENGTH (1)
#define AMI_NUM_ELEMENTS_LENGTH (1)
#define AMI_SLP_TYPA_SHIFT (10)
#define AMI_SLP_TYPB_SHIFT (10)
#define AMI_S5_NAME_OP_OFFSET_1 (-1)
#define AMI_S5_NAME_OP_OFFSET_2 (-2)
#define AMI_S5_PACKAGE_OP_OFFSET (4)
#define AMI_S5_PACKET_LENGTH_OFFSET (5)

static struct acpi_rsdt {
  struct acpi_sdt_header hdr;
  u32_t data[MAX_RSDT];
} rsdt;

static struct {
  char signature[ACPI_SDT_SIGNATURE_LEN + 1];
  k_size_t length; // MODIFIED size_t
  k_size_t length; // MODIFIED size_t
} sdt_trans[MAX_RSDT];

static int sdt_count;
static u16_t pm1a_cnt_blk = 0;
static u16_t pm1b_cnt_blk = 0;
static u16_t slp_typa = 0;
static u16_t slp_typb = 0;

static int acpi_check_csum(struct acpi_sdt_header *tb,
                           k_size_t size) // MODIFIED size_t
    static int acpi_check_csum(struct acpi_sdt_header *tb,
                               k_size_t size) // MODIFIED size_t
{
  u8_t total = 0;
  k_size_t i; // MODIFIED int to k_size_t for loop consistency with size
  k_size_t i; // MODIFIED int to k_size_t for loop consistency with size
  for (i = 0; i < size; i++)
    total += ((unsigned char *)tb)[i];
  return total == 0 ? 0 : -1;
}

static int acpi_check_signature(const char *orig, const char *match) {
  // MODIFIED strncmp to placeholder
  return kstrncmp(orig, match, ACPI_SDT_SIGNATURE_LEN);
}

static u32_t acpi_phys2vir(u32_t p) {
  if (!vm_running) {
    DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
    return p;
  }
  panic("acpi: can't get virtual address of arbitrary physical address");
}

static int acpi_phys_copy(phys_bytes phys, void *target,
                          k_size_t len) // MODIFIED size_t
    static int acpi_phys_copy(phys_bytes phys, void *target,
                              k_size_t len) // MODIFIED size_t
{
  if (!vm_running) {
    kmemcpy(target, (void *)phys, len); // MODIFIED
    kmemcpy(target, (void *)phys, len); // MODIFIED
    return 0;
  }
  panic("can't acpi_phys_copy with vm");
}

static int acpi_read_sdt_at(phys_bytes addr, struct acpi_sdt_header *tb,
                            k_size_t size, // MODIFIED size_t
                            k_size_t size, // MODIFIED size_t
                            const char *name) {
  struct acpi_sdt_header hdr;

  /* if 0 is supplied, we only return the size of the table */
  if (tb == 0) { // NULL might be undefined
    if (read_func(addr, &hdr, sizeof(struct acpi_sdt_header))) {
      kprintf_stub("ERROR acpi cannot read %s header\n", name); // MODIFIED
      kprintf_stub("ERROR acpi cannot read %s header\n", name); // MODIFIED
      return -1;
    }

    return hdr.length;
  }

  if (read_func(addr, tb, sizeof(struct acpi_sdt_header))) {
    kprintf_stub("ERROR acpi cannot read %s header\n", name); // MODIFIED
    kprintf_stub("ERROR acpi cannot read %s header\n", name); // MODIFIED
    return -1;
  }

  if (acpi_check_signature(tb->signature, name)) {
    kprintf_stub("ERROR acpi %s signature does not match\n", name); // MODIFIED
    kprintf_stub("ERROR acpi %s signature does not match\n", name); // MODIFIED
    return -1;
  }

  if (size < tb->length) {
    kprintf_stub("ERROR acpi buffer too small for %s\n", name); // MODIFIED
    kprintf_stub("ERROR acpi buffer too small for %s\n", name); // MODIFIED
    return -1;
  }

  if (read_func(addr, tb,
                size)) { // size was k_size_t, read_func expects k_size_t
    kprintf_stub("ERROR acpi cannot read %s\n", name); // MODIFIED
    if (read_func(addr, tb,
                  size)) { // size was k_size_t, read_func expects k_size_t
      kprintf_stub("ERROR acpi cannot read %s\n", name); // MODIFIED
      return -1;
    }

    if (acpi_check_csum(tb, tb->length)) { // tb->length is u32_t,
                                           // acpi_check_csum expects k_size_t
      kprintf_stub("ERROR acpi %s checksum does not match\n", name); // MODIFIED
      if (acpi_check_csum(tb, tb->length)) { // tb->length is u32_t,
                                             // acpi_check_csum expects k_size_t
        kprintf_stub("ERROR acpi %s checksum does not match\n",
                     name); // MODIFIED
        return -1;
      }

      return tb->length;
    }

    phys_bytes acpi_get_table_base(const char *name) {
      int i;

      for (i = 0; i < sdt_count; i++) {
        // MODIFIED strncmp to placeholder
        if (kstrncmp(name, sdt_trans[i].signature, ACPI_SDT_SIGNATURE_LEN) == 0)
          return (phys_bytes)rsdt.data[i];
      }

      return (phys_bytes)0; // NULL might be undefined
    }

    k_size_t acpi_get_table_length(const char *name)     // MODIFIED size_t
        k_size_t acpi_get_table_length(const char *name) // MODIFIED size_t
    {
      int i;

      for (i = 0; i < sdt_count; i++) {
        // MODIFIED strncmp to placeholder
        if (kstrncmp(name, sdt_trans[i].signature, ACPI_SDT_SIGNATURE_LEN) == 0)
          return sdt_trans[i].length;
      }

      return 0;
    }

    static void *acpi_madt_get_typed_item(struct acpi_madt_hdr * hdr,
                                          unsigned char type, unsigned idx) {
      u8_t *t, *end;
      int i;

      t = (u8_t *)hdr + sizeof(struct acpi_madt_hdr);
      end = (u8_t *)hdr + hdr->hdr.length;

      i = 0;
      while (t < end) {
        if (type == ((struct acpi_madt_item_hdr *)t)->type) {
          if (i == idx)
            return t;
          else
            i++;
        }
        t += ((struct acpi_madt_item_hdr *)t)->length;
      }

      return 0; // NULL might be undefined
    }

#if 0
static void * acpi_madt_get_item(struct acpi_madt_hdr * hdr,
				unsigned idx)
{
	u8_t * t, * end;
	int i;

	t = (u8_t *) hdr + sizeof(struct acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	for(i = 0 ; i <= idx && t < end; i++) {
		if (i == idx)
			return t;
		t += ((struct acpi_madt_item_hdr *) t)->length;
	}

	return 0; // NULL might be undefined
}
#endif

    static int acpi_rsdp_test(void *buff) {
      struct acpi_rsdp *rsdp = (struct acpi_rsdp *)buff;

      if (!platform_tbl_checksum_ok(buff, 20))
        return 0;
      // MODIFIED strncmp to placeholder
      if (kstrncmp(rsdp->signature, "RSD PTR ", 8))
        return 0;

      return 1;
    }

    static int get_acpi_rsdp(void) {
      u16_t ebda;
      /*
       * Read 40:0Eh - to find the starting address of the EBDA.
       */
      acpi_phys_copy(0x40E, &ebda, sizeof(ebda));
      if (ebda) {
        ebda <<= 4;
        if (platform_tbl_ptr(ebda, ebda + 0x400, 16, &acpi_rsdp,
                             sizeof(acpi_rsdp), &machine.acpi_rsdp,
                             acpi_rsdp_test))
          return 1;
      }

      /* try BIOS read only mem space */
      if (platform_tbl_ptr(0xE0000, 0x100000, 16, &acpi_rsdp, sizeof(acpi_rsdp),
                           &machine.acpi_rsdp, acpi_rsdp_test))
        return 1;

      machine.acpi_rsdp = 0; /* RSDP cannot be found at this address therefore
                                it is a valid negative value */
      return 0;
    }

    static void acpi_init_poweroff(void) {
      u8_t *ptr = 0;                            // NULL might be undefined
      u8_t *start = 0;                          // NULL might be undefined
      u8_t *end = 0;                            // NULL might be undefined
      struct acpi_fadt_header *fadt_header = 0; // NULL might be undefined
      struct acpi_rsdt *dsdt_header = 0;        // NULL might be undefined
      char *msg = 0;                            // NULL might be undefined

      /* Everything used here existed since ACPI spec 1.0 */
      /* So we can safely use them */
      fadt_header =
          (struct acpi_fadt_header *)acpi_phys2vir(acpi_get_table_base("FACP"));
      if (fadt_header == 0) { // NULL might be undefined
        msg = "Could not load FACP";
        goto exit;
      }

      dsdt_header =
          (struct acpi_rsdt *)acpi_phys2vir((phys_bytes)fadt_header->dsdt);
      if (dsdt_header == 0) { // NULL might be undefined
        msg = "Could not load DSDT";
        goto exit;
      }

      pm1a_cnt_blk = fadt_header->pm1a_cnt_blk;
      pm1b_cnt_blk = fadt_header->pm1b_cnt_blk;

      ptr = start = (u8_t *)dsdt_header->data;
      end = start + dsdt_header->hdr.length - 4;

      /* See http://forum.osdev.org/viewtopic.php?t=16990 */
      /* for layout of \_S5 */
      // MODIFIED memcmp to placeholder
      while (ptr < end && (kmemcmp(ptr, "_S5_", 4) != 0))
        ptr++;

      msg = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
      if (ptr >= end || ptr == start)
        goto exit;

      /* validate AML structure */
      if (*(ptr + AMI_S5_PACKAGE_OP_OFFSET) != AMI_PACKAGE_OP_CODE)
        goto exit;

      if ((ptr < start + (-AMI_S5_NAME_OP_OFFSET_2) ||
           (*(ptr + AMI_S5_NAME_OP_OFFSET_2) != AMI_NAME_OP_CODE ||
            *(ptr + AMI_S5_NAME_OP_OFFSET_2 + 1) != '\\')) &&
          *(ptr + AMI_S5_NAME_OP_OFFSET_1) != AMI_NAME_OP_CODE)
        goto exit;

      ptr += AMI_S5_PACKET_LENGTH_OFFSET;
      if (ptr >= end)
        goto exit;

      /* package length */
      ptr += ((*ptr & AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK) >>
              AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT) +
             AMI_MIN_PACKAGE_LENGTH + AMI_NUM_ELEMENTS_LENGTH;
      if (ptr >= end)
        goto exit;

      if (*ptr == AMI_BYTE_PREFIX_CODE)
        ptr++; /* skip byte prefix */

      slp_typa = (*ptr) << AMI_SLP_TYPA_SHIFT;

      ptr++; /* move to SLP_TYPb */
      if (*ptr == AMI_BYTE_PREFIX_CODE)
        ptr++; /* skip byte prefix */

      slp_typb = (*ptr) << AMI_SLP_TYPB_SHIFT;

      msg = "poweroff initialized";

    exit:
      if (msg) { // Check if msg is not 0
        DEBUGBASIC(("acpi: %s\n", msg));
      }
    }

    void acpi_init(void) {
      int s, i;
      read_func = acpi_phys_copy;

      if (!get_acpi_rsdp()) {
        kprintf_stub("WARNING : Cannot configure ACPI\n"); // MODIFIED
        kprintf_stub("WARNING : Cannot configure ACPI\n"); // MODIFIED
        return;
      }

      s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (struct acpi_sdt_header *)&rsdt,
                           sizeof(struct acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

      sdt_count = (s - sizeof(struct acpi_sdt_header)) / sizeof(u32_t);

      for (i = 0; i < sdt_count; i++) {
        struct acpi_sdt_header hdr;
        int j;
        if (read_func(rsdt.data[i], &hdr, sizeof(struct acpi_sdt_header))) {
                        kprintf_stub("ERROR acpi cannot read header at 0x%x\n", // MODIFIED
			kprintf_stub("ERROR acpi cannot read header at 0x%x\n", // MODIFIED
								rsdt.data[i]);
			return;
        }

        for (j = 0; j < ACPI_SDT_SIGNATURE_LEN; j++)
          sdt_trans[i].signature[j] = hdr.signature[j];
        sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
        sdt_trans[i].length = hdr.length;
      }

      acpi_init_poweroff();
    }

    struct acpi_madt_ioapic *acpi_get_ioapic_next(void) {
      static unsigned idx = 0;
      static struct acpi_madt_hdr *madt_hdr;

      struct acpi_madt_ioapic *ret;

      if (idx == 0) {
        madt_hdr =
            (struct acpi_madt_hdr *)acpi_phys2vir(acpi_get_table_base("APIC"));
        if (madt_hdr == 0) // NULL might be undefined
          return 0;        // NULL might be undefined
      }

      ret = (struct acpi_madt_ioapic *)acpi_madt_get_typed_item(
          madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
      if (ret)
        idx++;

      return ret;
    }

    struct acpi_madt_lapic *acpi_get_lapic_next(void) {
      static unsigned idx = 0;
      static struct acpi_madt_hdr *madt_hdr;

      struct acpi_madt_lapic *ret;

      if (idx == 0) {
        madt_hdr =
            (struct acpi_madt_hdr *)acpi_phys2vir(acpi_get_table_base("APIC"));
        if (madt_hdr == 0) // NULL might be undefined
          return 0;        // NULL might be undefined
      }

      for (;;) {
        ret = (struct acpi_madt_lapic *)acpi_madt_get_typed_item(
            madt_hdr, ACPI_MADT_TYPE_LAPIC, idx);
        if (!ret)
          break;

        idx++;

        /* report only usable CPUs */
        if (ret->flags & 1)
          break;
      }

      return ret;
    }

    void __k_unpaged_acpi_poweroff(void) { /* NO OP poweroff symbol*/ }

    void acpi_poweroff(void) {
      if (pm1a_cnt_blk == 0) {
        return;
      }
      outw(pm1a_cnt_blk, slp_typa | SLP_EN_CODE);
      if (pm1b_cnt_blk != 0) {
        outw(pm1b_cnt_blk, slp_typb | SLP_EN_CODE);
      }
    }

    /**
     * @section kernel_uts_i86pc_os_acpi_stubs_c
     * @brief Original file: minix/kernel/uts/i86pc/os/acpi_stubs.c
     */
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
     * Copyright 2007 Sun Microsystems, Inc.  All rights reserved.
     * Use is subject to license terms.
     */

#include <sys/types.h>

#include <sys/acpi/acpi.h>
#include <sys/acpica.h>

    /*
     * This file contains ACPI functions that are needed by the kernel before
     * the ACPI module is loaded.  Any functions or definitions need to be
     * able to deal with the possibility that ACPI doesn't get loaded, or
     * doesn't contain the required method.
     */

    int (*acpi_fp_setwake)();

    /*
     *
     */
    int acpi_ddi_setwake(dev_info_t * dip, int level) {
      if (acpi_fp_setwake == NULL)
        return (AE_ERROR);

      return ((*acpi_fp_setwake)(dip, level));
    }

    /**
     * @section drivers_power_acpi_dispatcher_dsargs_c
     * @brief Original file: minix/drivers/power/acpi/dispatcher/dsargs.c
     */
    /******************************************************************************
     *
     * Module Name: dsargs - Support for execution of dynamic arguments for
     *static objects (regions, fields, buffer fields, etc.)
     *
     *****************************************************************************/

    /*
     * Copyright (C) 2000 - 2014, Intel Corp.
     * All rights reserved.
     *
     * Redistribution and use in source and binary forms, with or without
     * modification, are permitted provided that the following conditions
     * are met:
     * 1. Redistributions of source code must retain the above copyright
     *    notice, this list of conditions, and the following disclaimer,
     *    without modification.
     * 2. Redistributions in binary form must reproduce at minimum a disclaimer
     *    substantially similar to the "NO WARRANTY" disclaimer below
     *    ("Disclaimer") and any redistribution must be conditioned upon
     *    including a substantially similar Disclaimer requirement for further
     *    binary redistribution.
     * 3. Neither the names of the above-listed copyright holders nor the names
     *    of any contributors may be used to endorse or promote products derived
     *    from this software without specific prior written permission.
     *
     * Alternatively, this software may be distributed under the terms of the
     * GNU General Public License ("GPL") version 2 as published by the Free
     * Software Foundation.
     *
     * NO WARRANTY
     * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
     * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
     * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR
     * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
     * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR
     * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
     * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
     * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
     * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
     * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
     * THE POSSIBILITY OF SUCH DAMAGES.
     */

#include "accommon.h"
#include "acdispat.h"
#include "acnamesp.h"
#include "acparser.h"
#include "acpi.h"
#include "amlcode.h"

#define _COMPONENT ACPI_DISPATCHER
    ACPI_MODULE_NAME("dsargs")

    /* Local prototypes */

    static ACPI_STATUS AcpiDsExecuteArguments(
        ACPI_NAMESPACE_NODE * Node, ACPI_NAMESPACE_NODE * ScopeNode,
        UINT32 AmlLength, UINT8 * AmlStart);

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsExecuteArguments
     *
     * PARAMETERS:  Node                - Object NS node
     *              ScopeNode           - Parent NS node
     *              AmlLength           - Length of executable AML
     *              AmlStart            - Pointer to the AML
     *
     * RETURN:      Status.
     *
     * DESCRIPTION: Late (deferred) execution of region or field arguments
     *
     ******************************************************************************/

    static ACPI_STATUS AcpiDsExecuteArguments(
        ACPI_NAMESPACE_NODE * Node, ACPI_NAMESPACE_NODE * ScopeNode,
        UINT32 AmlLength, UINT8 * AmlStart) {
      ACPI_STATUS Status;
      ACPI_PARSE_OBJECT *Op;
      ACPI_WALK_STATE *WalkState;

      ACPI_FUNCTION_TRACE(DsExecuteArguments);

      /* Allocate a new parser op to be the root of the parsed tree */

      Op = AcpiPsAllocOp(AML_INT_EVAL_SUBTREE_OP);
      if (!Op) {
        return_ACPI_STATUS(AE_NO_MEMORY);
      }

      /* Save the Node for use in AcpiPsParseAml */

      Op->Common.Node = ScopeNode;

      /* Create and initialize a new parser state */

      WalkState = AcpiDsCreateWalkState(0, NULL, NULL, NULL);
      if (!WalkState) {
        Status = AE_NO_MEMORY;
        goto Cleanup;
      }

      Status = AcpiDsInitAmlWalk(WalkState, Op, NULL, AmlStart, AmlLength, NULL,
                                 ACPI_IMODE_LOAD_PASS1);
      if (ACPI_FAILURE(Status)) {
        AcpiDsDeleteWalkState(WalkState);
        goto Cleanup;
      }

      /* Mark this parse as a deferred opcode */

      WalkState->ParseFlags = ACPI_PARSE_DEFERRED_OP;
      WalkState->DeferredNode = Node;

      /* Pass1: Parse the entire declaration */

      Status = AcpiPsParseAml(WalkState);
      if (ACPI_FAILURE(Status)) {
        goto Cleanup;
      }

      /* Get and init the Op created above */

      Op->Common.Node = Node;
      AcpiPsDeleteParseTree(Op);

      /* Evaluate the deferred arguments */

      Op = AcpiPsAllocOp(AML_INT_EVAL_SUBTREE_OP);
      if (!Op) {
        return_ACPI_STATUS(AE_NO_MEMORY);
      }

      Op->Common.Node = ScopeNode;

      /* Create and initialize a new parser state */

      WalkState = AcpiDsCreateWalkState(0, NULL, NULL, NULL);
      if (!WalkState) {
        Status = AE_NO_MEMORY;
        goto Cleanup;
      }

      /* Execute the opcode and arguments */

      Status = AcpiDsInitAmlWalk(WalkState, Op, NULL, AmlStart, AmlLength, NULL,
                                 ACPI_IMODE_EXECUTE);
      if (ACPI_FAILURE(Status)) {
        AcpiDsDeleteWalkState(WalkState);
        goto Cleanup;
      }

      /* Mark this execution as a deferred opcode */

      WalkState->DeferredNode = Node;
      Status = AcpiPsParseAml(WalkState);

    Cleanup:
      AcpiPsDeleteParseTree(Op);
      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsGetBufferFieldArguments
     *
     * PARAMETERS:  ObjDesc         - A valid BufferField object
     *
     * RETURN:      Status.
     *
     * DESCRIPTION: Get BufferField Buffer and Index. This implements the late
     *              evaluation of these field attributes.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsGetBufferFieldArguments(ACPI_OPERAND_OBJECT * ObjDesc) {
      ACPI_OPERAND_OBJECT *ExtraDesc;
      ACPI_NAMESPACE_NODE *Node;
      ACPI_STATUS Status;

      ACPI_FUNCTION_TRACE_PTR(DsGetBufferFieldArguments, ObjDesc);

      if (ObjDesc->Common.Flags & AOPOBJ_DATA_VALID) {
        return_ACPI_STATUS(AE_OK);
      }

      /* Get the AML pointer (method object) and BufferField node */

      ExtraDesc = AcpiNsGetSecondaryObject(ObjDesc);
      Node = ObjDesc->BufferField.Node;

      ACPI_DEBUG_EXEC(
          AcpiUtDisplayInitPathname(ACPI_TYPE_BUFFER_FIELD, Node, NULL));

      ACPI_DEBUG_PRINT((ACPI_DB_EXEC, "[%4.4s] BufferField Arg Init\n",
                        AcpiUtGetNodeName(Node)));

      /* Execute the AML code for the TermArg arguments */

      Status =
          AcpiDsExecuteArguments(Node, Node->Parent, ExtraDesc->Extra.AmlLength,
                                 ExtraDesc->Extra.AmlStart);
      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsGetBankFieldArguments
     *
     * PARAMETERS:  ObjDesc         - A valid BankField object
     *
     * RETURN:      Status.
     *
     * DESCRIPTION: Get BankField BankValue. This implements the late
     *              evaluation of these field attributes.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsGetBankFieldArguments(ACPI_OPERAND_OBJECT * ObjDesc) {
      ACPI_OPERAND_OBJECT *ExtraDesc;
      ACPI_NAMESPACE_NODE *Node;
      ACPI_STATUS Status;

      ACPI_FUNCTION_TRACE_PTR(DsGetBankFieldArguments, ObjDesc);

      if (ObjDesc->Common.Flags & AOPOBJ_DATA_VALID) {
        return_ACPI_STATUS(AE_OK);
      }

      /* Get the AML pointer (method object) and BankField node */

      ExtraDesc = AcpiNsGetSecondaryObject(ObjDesc);
      Node = ObjDesc->BankField.Node;

      ACPI_DEBUG_EXEC(
          AcpiUtDisplayInitPathname(ACPI_TYPE_LOCAL_BANK_FIELD, Node, NULL));

      ACPI_DEBUG_PRINT((ACPI_DB_EXEC, "[%4.4s] BankField Arg Init\n",
                        AcpiUtGetNodeName(Node)));

      /* Execute the AML code for the TermArg arguments */

      Status =
          AcpiDsExecuteArguments(Node, Node->Parent, ExtraDesc->Extra.AmlLength,
                                 ExtraDesc->Extra.AmlStart);
      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsGetBufferArguments
     *
     * PARAMETERS:  ObjDesc         - A valid Buffer object
     *
     * RETURN:      Status.
     *
     * DESCRIPTION: Get Buffer length and initializer byte list. This implements
     *              the late evaluation of these attributes.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsGetBufferArguments(ACPI_OPERAND_OBJECT * ObjDesc) {
      ACPI_NAMESPACE_NODE *Node;
      ACPI_STATUS Status;

      ACPI_FUNCTION_TRACE_PTR(DsGetBufferArguments, ObjDesc);

      if (ObjDesc->Common.Flags & AOPOBJ_DATA_VALID) {
        return_ACPI_STATUS(AE_OK);
      }

      /* Get the Buffer node */

      Node = ObjDesc->Buffer.Node;
      if (!Node) {
        ACPI_ERROR((AE_INFO,
                    "No pointer back to namespace node in buffer object %p",
                    ObjDesc));
        return_ACPI_STATUS(AE_AML_INTERNAL);
      }

      ACPI_DEBUG_PRINT((ACPI_DB_EXEC, "Buffer Arg Init\n"));

      /* Execute the AML code for the TermArg arguments */

      Status = AcpiDsExecuteArguments(Node, Node, ObjDesc->Buffer.AmlLength,
                                      ObjDesc->Buffer.AmlStart);
      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsGetPackageArguments
     *
     * PARAMETERS:  ObjDesc         - A valid Package object
     *
     * RETURN:      Status.
     *
     * DESCRIPTION: Get Package length and initializer byte list. This
     *implements the late evaluation of these attributes.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsGetPackageArguments(ACPI_OPERAND_OBJECT * ObjDesc) {
      ACPI_NAMESPACE_NODE *Node;
      ACPI_STATUS Status;

      ACPI_FUNCTION_TRACE_PTR(DsGetPackageArguments, ObjDesc);

      if (ObjDesc->Common.Flags & AOPOBJ_DATA_VALID) {
        return_ACPI_STATUS(AE_OK);
      }

      /* Get the Package node */

      Node = ObjDesc->Package.Node;
      if (!Node) {
        ACPI_ERROR((AE_INFO, "No pointer back to namespace node in package %p",
                    ObjDesc));
        return_ACPI_STATUS(AE_AML_INTERNAL);
      }

      ACPI_DEBUG_PRINT((ACPI_DB_EXEC, "Package Arg Init\n"));

      /* Execute the AML code for the TermArg arguments */

      Status = AcpiDsExecuteArguments(Node, Node, ObjDesc->Package.AmlLength,
                                      ObjDesc->Package.AmlStart);
      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsGetRegionArguments
     *
     * PARAMETERS:  ObjDesc         - A valid region object
     *
     * RETURN:      Status.
     *
     * DESCRIPTION: Get region address and length. This implements the late
     *              evaluation of these region attributes.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsGetRegionArguments(ACPI_OPERAND_OBJECT * ObjDesc) {
      ACPI_NAMESPACE_NODE *Node;
      ACPI_STATUS Status;
      ACPI_OPERAND_OBJECT *ExtraDesc;

      ACPI_FUNCTION_TRACE_PTR(DsGetRegionArguments, ObjDesc);

      if (ObjDesc->Region.Flags & AOPOBJ_DATA_VALID) {
        return_ACPI_STATUS(AE_OK);
      }

      ExtraDesc = AcpiNsGetSecondaryObject(ObjDesc);
      if (!ExtraDesc) {
        return_ACPI_STATUS(AE_NOT_EXIST);
      }

      /* Get the Region node */

      Node = ObjDesc->Region.Node;

      ACPI_DEBUG_EXEC(AcpiUtDisplayInitPathname(ACPI_TYPE_REGION, Node, NULL));

      ACPI_DEBUG_PRINT((ACPI_DB_EXEC, "[%4.4s] OpRegion Arg Init at AML %p\n",
                        AcpiUtGetNodeName(Node), ExtraDesc->Extra.AmlStart));

      /* Execute the argument AML */

      Status = AcpiDsExecuteArguments(Node, ExtraDesc->Extra.ScopeNode,
                                      ExtraDesc->Extra.AmlLength,
                                      ExtraDesc->Extra.AmlStart);
      if (ACPI_FAILURE(Status)) {
        return_ACPI_STATUS(Status);
      }

      Status = AcpiUtAddAddressRange(ObjDesc->Region.SpaceId,
                                     ObjDesc->Region.Address,
                                     ObjDesc->Region.Length, Node);
      return_ACPI_STATUS(Status);
    }

    /**
     * @section drivers_power_acpi_executer_exsystem_c
     * @brief Original file: minix/drivers/power/acpi/executer/exsystem.c
     */
    /******************************************************************************
     *
     * Module Name: exsystem - Interface to OS services
     *
     *****************************************************************************/

    /*
     * Copyright (C) 2000 - 2014, Intel Corp.
     * All rights reserved.
     *
     * Redistribution and use in source and binary forms, with or without
     * modification, are permitted provided that the following conditions
     * are met:
     * 1. Redistributions of source code must retain the above copyright
     *    notice, this list of conditions, and the following disclaimer,
     *    without modification.
     * 2. Redistributions in binary form must reproduce at minimum a disclaimer
     *    substantially similar to the "NO WARRANTY" disclaimer below
     *    ("Disclaimer") and any redistribution must be conditioned upon
     *    including a substantially similar Disclaimer requirement for further
     *    binary redistribution.
     * 3. Neither the names of the above-listed copyright holders nor the names
     *    of any contributors may be used to endorse or promote products derived
     *    from this software without specific prior written permission.
     *
     * Alternatively, this software may be distributed under the terms of the
     * GNU General Public License ("GPL") version 2 as published by the Free
     * Software Foundation.
     *
     * NO WARRANTY
     * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
     * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
     * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR
     * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
     * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR
     * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
     * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
     * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
     * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
     * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
     * THE POSSIBILITY OF SUCH DAMAGES.
     */

#include "accommon.h"
#include "acinterp.h"
#include "acpi.h"

#define _COMPONENT ACPI_EXECUTER
    ACPI_MODULE_NAME("exsystem")

    /*******************************************************************************
     *
     * FUNCTION:    AcpiExSystemWaitSemaphore
     *
     * PARAMETERS:  Semaphore       - Semaphore to wait on
     *              Timeout         - Max time to wait
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Implements a semaphore wait with a check to see if the
     *              semaphore is available immediately. If it is not, the
     *              interpreter is released before waiting.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiExSystemWaitSemaphore(ACPI_SEMAPHORE Semaphore, UINT16 Timeout) {
      ACPI_STATUS Status;

      ACPI_FUNCTION_TRACE(ExSystemWaitSemaphore);

      Status = AcpiOsWaitSemaphore(Semaphore, 1, ACPI_DO_NOT_WAIT);
      if (ACPI_SUCCESS(Status)) {
        return_ACPI_STATUS(Status);
      }

      if (Status == AE_TIME) {
        /* We must wait, so unlock the interpreter */

        AcpiExExitInterpreter();

        Status = AcpiOsWaitSemaphore(Semaphore, 1, Timeout);

        ACPI_DEBUG_PRINT((ACPI_DB_EXEC, "*** Thread awake after blocking, %s\n",
                          AcpiFormatException(Status)));

        /* Reacquire the interpreter */

        AcpiExEnterInterpreter();
      }

      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiExSystemWaitMutex
     *
     * PARAMETERS:  Mutex           - Mutex to wait on
     *              Timeout         - Max time to wait
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Implements a mutex wait with a check to see if the
     *              mutex is available immediately. If it is not, the
     *              interpreter is released before waiting.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiExSystemWaitMutex(ACPI_MUTEX Mutex, UINT16 Timeout) {
      ACPI_STATUS Status;

      ACPI_FUNCTION_TRACE(ExSystemWaitMutex);

      Status = AcpiOsAcquireMutex(Mutex, ACPI_DO_NOT_WAIT);
      if (ACPI_SUCCESS(Status)) {
        return_ACPI_STATUS(Status);
      }

      if (Status == AE_TIME) {
        /* We must wait, so unlock the interpreter */

        AcpiExExitInterpreter();

        Status = AcpiOsAcquireMutex(Mutex, Timeout);

        ACPI_DEBUG_PRINT((ACPI_DB_EXEC, "*** Thread awake after blocking, %s\n",
                          AcpiFormatException(Status)));

        /* Reacquire the interpreter */

        AcpiExEnterInterpreter();
      }

      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiExSystemDoStall
     *
     * PARAMETERS:  HowLong         - The amount of time to stall,
     *                                in microseconds
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Suspend running thread for specified amount of time.
     *              Note: ACPI specification requires that Stall() does not
     *              relinquish the processor, and delays longer than 100 usec
     *              should use Sleep() instead. We allow stalls up to 255 usec
     *              for compatibility with other interpreters and existing
     *BIOSs.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiExSystemDoStall(UINT32 HowLong) {
      ACPI_STATUS Status = AE_OK;

      ACPI_FUNCTION_ENTRY();

      if (HowLong > 255) /* 255 microseconds */
      {
        /*
         * Longer than 255 usec, this is an error
         *
         * (ACPI specifies 100 usec as max, but this gives some slack in
         * order to support existing BIOSs)
         */
        ACPI_ERROR((AE_INFO, "Time parameter is too large (%u)", HowLong));
        Status = AE_AML_OPERAND_VALUE;
      } else {
        AcpiOsStall(HowLong);
      }

      return (Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiExSystemDoSleep
     *
     * PARAMETERS:  HowLong         - The amount of time to sleep,
     *                                in milliseconds
     *
     * RETURN:      None
     *
     * DESCRIPTION: Sleep the running thread for specified amount of time.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiExSystemDoSleep(UINT64 HowLong) {
      ACPI_FUNCTION_ENTRY();

      /* Since this thread will sleep, we must release the interpreter */

      AcpiExExitInterpreter();

      /*
       * For compatibility with other ACPI implementations and to prevent
       * accidental deep sleeps, limit the sleep time to something reasonable.
       */
      if (HowLong > ACPI_MAX_SLEEP) {
        HowLong = ACPI_MAX_SLEEP;
      }

      AcpiOsSleep(HowLong);

      /* And now we must get the interpreter again */

      AcpiExEnterInterpreter();
      return (AE_OK);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiExSystemSignalEvent
     *
     * PARAMETERS:  ObjDesc         - The object descriptor for this op
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Provides an access point to perform synchronization
     *operations within the AML.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiExSystemSignalEvent(ACPI_OPERAND_OBJECT * ObjDesc) {
      ACPI_STATUS Status = AE_OK;

      ACPI_FUNCTION_TRACE(ExSystemSignalEvent);

      if (ObjDesc) {
        Status = AcpiOsSignalSemaphore(ObjDesc->Event.OsSemaphore, 1);
      }

      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiExSystemWaitEvent
     *
     * PARAMETERS:  TimeDesc        - The 'time to delay' object descriptor
     *              ObjDesc         - The object descriptor for this op
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Provides an access point to perform synchronization
     *operations within the AML. This operation is a request to wait for an
     *              event.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiExSystemWaitEvent(ACPI_OPERAND_OBJECT * TimeDesc,
                          ACPI_OPERAND_OBJECT * ObjDesc) {
      ACPI_STATUS Status = AE_OK;

      ACPI_FUNCTION_TRACE(ExSystemWaitEvent);

      if (ObjDesc) {
        Status = AcpiExSystemWaitSemaphore(ObjDesc->Event.OsSemaphore,
                                           (UINT16)TimeDesc->Integer.Value);
      }

      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiExSystemResetEvent
     *
     * PARAMETERS:  ObjDesc         - The object descriptor for this op
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Reset an event to a known state.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiExSystemResetEvent(ACPI_OPERAND_OBJECT * ObjDesc) {
      ACPI_STATUS Status = AE_OK;
      ACPI_SEMAPHORE TempSemaphore;

      ACPI_FUNCTION_ENTRY();

      /*
       * We are going to simply delete the existing semaphore and
       * create a new one!
       */
      Status = AcpiOsCreateSemaphore(ACPI_NO_UNIT_LIMIT, 0, &TempSemaphore);
      if (ACPI_SUCCESS(Status)) {
        (void)AcpiOsDeleteSemaphore(ObjDesc->Event.OsSemaphore);
        ObjDesc->Event.OsSemaphore = TempSemaphore;
      }

      return (Status);
    }
    /**
     * @section kernel_uts_intel_io_acpica_acpica_c
     * @brief Original file: minix/kernel/uts/intel/io/acpica/acpica.c
     */
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
     * Copyright (c) 2005, 2010, Oracle and/or its affiliates. All rights
     * reserved. Copyright 2011 Nexenta Systems, Inc.  All rights reserved.
     * Copyright 2016, Joyent, Inc.
     */
    /*
     * Copyright (c) 2009, Intel Corporation.
     * All rights reserved.
     */
    /*
     * Solaris x86 ACPI CA services
     */

#include <sys/conf.h>
#include <sys/ddi.h>
#include <sys/errno.h>
#include <sys/esunddi.h>
#include <sys/file.h>
#include <sys/kstat.h>
#include <sys/modctl.h>
#include <sys/open.h>
#include <sys/spl.h>
#include <sys/stat.h>
#include <sys/sunddi.h>
#include <sys/x86_archext.h>

#include <sys/acpi/acpi.h>
#include <sys/acpica.h>
#include <sys/archsystm.h>

    /*
     *
     */
    static struct modlmisc modlmisc = {
        &mod_miscops,
        "ACPI interpreter",
    };

    static struct modlinkage modlinkage = {
        MODREV_1,          /* MODREV_1 manual */
        (void *)&modlmisc, /* module linkage */
        NULL,              /* list terminator */
    };

    /*
     * Local prototypes
     */

    struct parsed_prw {
      ACPI_HANDLE prw_gpeobj;
      int prw_gpebit;
      int prw_level;
    };

    static void acpica_init_kstats(void);
    static ACPI_STATUS acpica_init_PRW(ACPI_HANDLE hdl, UINT32 lvl, void *ctxp,
                                       void **rvpp);

    static ACPI_STATUS acpica_parse_PRW(ACPI_BUFFER * prw_buf,
                                        struct parsed_prw * prw);

    /*
     * Local data
     */

    static kmutex_t acpica_module_lock;
    static kstat_t *acpica_ksp;

    /*
     * State of acpica subsystem
     * After successful initialization, will be ACPICA_INITIALIZED
     */
    int acpica_init_state = ACPICA_NOT_INITIALIZED;

    void *AcpiGbl_DbBuffer;
    uint32_t AcpiGbl_DbConsoleDebugLevel;

    /*
     * Following are set by acpica_process_user_options()
     *
     * acpica_enable = FALSE prevents initialization of ACPI CA
     * completely
     *
     * acpi_init_level determines level of ACPI CA functionality
     * enabled in acpica_init()
     */
    int acpica_enable;
    UINT32 acpi_init_level;

    /*
     * Non-zero enables lax behavior with respect to some
     * common ACPI BIOS issues; see ACPI CA documentation
     * Setting this to zero causes ACPI CA to enforce strict
     * compliance with ACPI specification
     */
    int acpica_enable_interpreter_slack = 1;

/*
 * For non-DEBUG builds, set the ACPI CA debug level to 0
 * to quiet chatty BIOS output into /var/adm/messages
 * Field-patchable for diagnostic use.
 */
#ifdef DEBUG
    int acpica_muzzle_debug_output = 0;
#else
    int acpica_muzzle_debug_output = 1;
#endif

    /*
     * ACPI DDI hooks
     */
    static int acpica_ddi_setwake(dev_info_t * dip, int level);

    int _init(void) {
      int error = EBUSY;
      int status;
      extern int (*acpi_fp_setwake)();
      extern kmutex_t cpu_map_lock;

      mutex_init(&acpica_module_lock, NULL, MUTEX_DRIVER, NULL);
      mutex_init(&cpu_map_lock, NULL, MUTEX_SPIN,
                 (ddi_iblock_cookie_t)ipltospl(DISP_LEVEL));

      if ((error = mod_install(&modlinkage)) != 0) {
        mutex_destroy(&acpica_module_lock);
        goto load_error;
      }

      AcpiGbl_EnableInterpreterSlack = (acpica_enable_interpreter_slack != 0);

      /* global ACPI CA initialization */
      if (ACPI_FAILURE(status = AcpiInitializeSubsystem()))
        cmn_err(CE_WARN, "!AcpiInitializeSubsystem failed: %d", status);

      /* initialize table manager */
      if (ACPI_FAILURE(status = AcpiInitializeTables(NULL, 0, 0)))
        cmn_err(CE_WARN, "!AcpiInitializeTables failed: %d", status);

      acpi_fp_setwake = acpica_ddi_setwake;

    load_error:
      return (error);
    }

    int _info(struct modinfo * modinfop) {
      return (mod_info(&modlinkage, modinfop));
    }

    int _fini(void) {
      /*
       * acpica module is never unloaded at run-time; there's always
       * a PSM depending on it, at the very least
       */
      return (EBUSY);
    }

    /*
     * Install acpica-provided (default) address-space handlers
     * that may be needed before AcpiEnableSubsystem() runs.
     * See the comment in AcpiInstallAddressSpaceHandler().
     * Default handlers for remaining address spaces are
     * installed later, in AcpiEnableSubsystem.
     */
    static int acpica_install_handlers() {
      ACPI_STATUS rv = AE_OK;
      ACPI_STATUS res;

      /*
       * Install ACPI CA default handlers
       */
      if ((res = AcpiInstallAddressSpaceHandler(
               ACPI_ROOT_OBJECT, ACPI_ADR_SPACE_SYSTEM_MEMORY,
               ACPI_DEFAULT_HANDLER, NULL, NULL)) != AE_OK &&
          res != AE_SAME_HANDLER) {
        cmn_err(CE_WARN, "!acpica: no default handler for"
                         " system memory");
        rv = AE_ERROR;
      }

      if ((res = AcpiInstallAddressSpaceHandler(
               ACPI_ROOT_OBJECT, ACPI_ADR_SPACE_SYSTEM_IO, ACPI_DEFAULT_HANDLER,
               NULL, NULL)) != AE_OK &&
          res != AE_SAME_HANDLER) {
        cmn_err(CE_WARN, "!acpica: no default handler for"
                         " system I/O");
        rv = AE_ERROR;
      }

      if ((res = AcpiInstallAddressSpaceHandler(
               ACPI_ROOT_OBJECT, ACPI_ADR_SPACE_PCI_CONFIG,
               ACPI_DEFAULT_HANDLER, NULL, NULL)) != AE_OK &&
          res != AE_SAME_HANDLER) {
        cmn_err(CE_WARN, "!acpica: no default handler for"
                         " PCI Config");
        rv = AE_ERROR;
      }

      if ((res = AcpiInstallAddressSpaceHandler(
               ACPI_ROOT_OBJECT, ACPI_ADR_SPACE_DATA_TABLE,
               ACPI_DEFAULT_HANDLER, NULL, NULL)) != AE_OK &&
          res != AE_SAME_HANDLER) {
        cmn_err(CE_WARN, "!acpica: no default handler for"
                         " Data Table");
        rv = AE_ERROR;
      }

      return (rv);
    }

    /*
     * Find the BIOS date, and return TRUE if supplied
     * date is same or later than the BIOS date, or FALSE
     * if the BIOS date can't be fetched for any reason
     */
    static int acpica_check_bios_date(int yy, int mm, int dd) {

      char *datep;
      int bios_year, bios_month, bios_day;

      /* If firmware has no bios, skip the check */
      if (ddi_prop_exists(DDI_DEV_T_ANY, ddi_root_node(), DDI_PROP_DONTPASS,
                          "efi-systab"))
        return (TRUE);

      /*
       * PC BIOSes contain a string in the form of
       * "mm/dd/yy" at absolute address 0xffff5,
       * where mm, dd and yy are all ASCII digits.
       * We map the string, pluck out the values,
       * and accept all BIOSes from 1 Jan 1999 on
       * as valid.
       */

      if ((datep = (char *)AcpiOsMapMemory(0xffff5, 8)) == NULL)
        return (FALSE);

      /* year */
      bios_year = ((int)(*(datep + 6) - '0') * 10) + (*(datep + 7) - '0');
      /* month */
      bios_month = ((int)(*datep - '0') * 10) + (*(datep + 1) - '0');
      /* day */
      bios_day = ((int)(*(datep + 3) - '0') * 10) + (*(datep + 4) - '0');

      AcpiOsUnmapMemory((void *)datep, 8);

      if (bios_year < 0 || bios_year > 99 || bios_month < 0 ||
          bios_month > 99 || bios_day < 0 || bios_day > 99) {
        /* non-digit chars in BIOS date */
        return (FALSE);
      }

      /*
       * Adjust for 2-digit year; note to grand-children:
       * need a new scheme before 2080 rolls around
       */
      bios_year += (bios_year >= 80 && bios_year <= 99) ? 1900 : 2000;

      if (bios_year < yy)
        return (FALSE);
      else if (bios_year > yy)
        return (TRUE);

      if (bios_month < mm)
        return (FALSE);
      else if (bios_month > mm)
        return (TRUE);

      if (bios_day < dd)
        return (FALSE);

      return (TRUE);
    }

    /*
     * Check for Metropolis systems with BIOSes older than 10/12/04
     * return TRUE if BIOS requires legacy mode, FALSE otherwise
     */
    static int acpica_metro_old_bios() {
      ACPI_TABLE_HEADER *fadt;

      /* get the FADT */
      if (AcpiGetTable(ACPI_SIG_FADT, 1, (ACPI_TABLE_HEADER **)&fadt) != AE_OK)
        return (FALSE);

      /* compare OEM Table ID to "SUNmetro" - no match, return false */
      if (strncmp("SUNmetro", fadt->OemTableId, 8))
        return (FALSE);

      /* On a Metro - return FALSE if later than 10/12/04 */
      return (!acpica_check_bios_date(2004, 10, 12));
    }

    /*
     * Process acpi-user-options property  if present
     */
    static void acpica_process_user_options() {
      static int processed = 0;
      int acpi_user_options;
      char *acpi_prop;

      /*
       * return if acpi-user-options has already been processed
       */
      if (processed)
        return;
      else
        processed = 1;

      /* converts acpi-user-options from type string to int, if any */
      if (ddi_prop_lookup_string(DDI_DEV_T_ANY, ddi_root_node(),
                                 DDI_PROP_DONTPASS, "acpi-user-options",
                                 &acpi_prop) == DDI_PROP_SUCCESS) {
        long data;
        int ret;
        ret = ddi_strtol(acpi_prop, NULL, 0, &data);
        if (ret == 0) {
          e_ddi_prop_remove(DDI_DEV_T_NONE, ddi_root_node(),
                            "acpi-user-options");
          e_ddi_prop_update_int(DDI_DEV_T_NONE, ddi_root_node(),
                                "acpi-user-options", data);
        }
        ddi_prop_free(acpi_prop);
      }

      /*
       * fetch the optional options property
       */
      acpi_user_options =
          ddi_prop_get_int(DDI_DEV_T_ANY, ddi_root_node(), DDI_PROP_DONTPASS,
                           "acpi-user-options", 0);

      /*
       * Note that 'off' has precedence over 'on'
       * Also note - all cases of ACPI_OUSER_MASK
       * provided here, no default: case is present
       */
      switch (acpi_user_options & ACPI_OUSER_MASK) {
      case ACPI_OUSER_DFLT:
        acpica_enable = acpica_check_bios_date(1999, 1, 1);
        break;
      case ACPI_OUSER_ON:
        acpica_enable = TRUE;
        break;
      case ACPI_OUSER_OFF:
      case ACPI_OUSER_OFF | ACPI_OUSER_ON:
        acpica_enable = FALSE;
        break;
      }

      acpi_init_level = ACPI_FULL_INITIALIZATION;

      /*
       * special test here; may be generalized in the
       * future - test for a machines that are known to
       * work only in legacy mode, and set OUSER_LEGACY if
       * we're on one
       */
      if (acpica_metro_old_bios())
        acpi_user_options |= ACPI_OUSER_LEGACY;

      /*
       * If legacy mode is specified, set initialization
       * options to avoid entering ACPI mode and hooking SCI
       * - basically try to act like legacy acpi_intp
       */
      if ((acpi_user_options & ACPI_OUSER_LEGACY) != 0)
        acpi_init_level |= (ACPI_NO_ACPI_ENABLE | ACPI_NO_HANDLER_INIT);

      /*
       * modify default ACPI CA debug output level for non-DEBUG builds
       * (to avoid BIOS debug chatter in /var/adm/messages)
       */
      if (acpica_muzzle_debug_output)
        AcpiDbgLevel = 0;
    }

    /*
     * Initialize the CA subsystem if it hasn't been done already
     */
    int acpica_init() {
      extern void acpica_find_ioapics(void);
      ACPI_STATUS status;

      /*
       * Make sure user options are processed,
       * then fail to initialize if ACPI CA has been
       * disabled
       */
      acpica_process_user_options();
      if (!acpica_enable)
        return (AE_ERROR);

      mutex_enter(&acpica_module_lock);
      if (acpica_init_state == ACPICA_INITIALIZED) {
        mutex_exit(&acpica_module_lock);
        return (AE_OK);
      }

      if (ACPI_FAILURE(status = AcpiLoadTables()))
        goto error;

      if (ACPI_FAILURE(status = acpica_install_handlers()))
        goto error;

      /*
       * Create ACPI-to-devinfo mapping now so _INI and _STA
       * methods can access PCI config space when needed
       */
      scan_d2a_map();

      if (ACPI_FAILURE(status = AcpiEnableSubsystem(acpi_init_level)))
        goto error;

      /* do after AcpiEnableSubsystem() so GPEs are initialized */
      acpica_ec_init(); /* initialize EC if present */

      /* This runs all device _STA and _INI methods. */
      if (ACPI_FAILURE(status = AcpiInitializeObjects(0)))
        goto error;

      acpica_init_state = ACPICA_INITIALIZED;

      /*
       * [ACPI, sec. 4.4.1.1]
       * As of ACPICA version 20101217 (December 2010), the _PRW methods
       * (Power Resources for Wake) are no longer automatically executed
       * as part of the ACPICA initialization.  The OS must do this.
       */
      (void)AcpiWalkNamespace(ACPI_TYPE_DEVICE, ACPI_ROOT_OBJECT, UINT32_MAX,
                              acpica_init_PRW, NULL, NULL, NULL);
      (void)AcpiUpdateAllGpes();

      /*
       * If we are running on the Xen hypervisor as dom0 we need to
       * find the ioapics so we can prevent ACPI from trying to
       * access them.
       */
      if (get_hwenv() == HW_XEN_PV && is_controldom())
        acpica_find_ioapics();
      acpica_init_kstats();
    error:
      if (acpica_init_state != ACPICA_INITIALIZED) {
        cmn_err(CE_NOTE, "!failed to initialize ACPI services");
      }

      /*
       * Set acpi-status to 13 if acpica has been initialized successfully.
       * This indicates that acpica is up and running.  This variable name
       * and value were chosen in order to remain compatible with acpi_intp.
       */
      e_ddi_prop_update_int(
          DDI_DEV_T_NONE, ddi_root_node(), "acpi-status",
          (ACPI_SUCCESS(status))
              ? (ACPI_BOOT_INIT | ACPI_BOOT_ENABLE | ACPI_BOOT_BOOTCONF)
              : 0);

      /* Mark acpica subsystem as fully initialized. */
      if (ACPI_SUCCESS(status) && acpi_init_level == ACPI_FULL_INITIALIZATION) {
        acpica_set_core_feature(ACPI_FEATURE_FULL_INIT);
      }

      mutex_exit(&acpica_module_lock);
      return (status);
    }

    /*
     * SCI handling
     */

    ACPI_STATUS
    acpica_get_sci(int *sci_irq, iflag_t *sci_flags) {
      ACPI_SUBTABLE_HEADER *ap;
      ACPI_TABLE_MADT *mat;
      ACPI_MADT_INTERRUPT_OVERRIDE *mio;
      ACPI_TABLE_FADT *fadt;
      int madt_seen, madt_size;

      /*
       * Make sure user options are processed,
       * then return error if ACPI CA has been
       * disabled or system is not running in ACPI
       * and won't need/understand SCI
       */
      acpica_process_user_options();
      if ((!acpica_enable) || (acpi_init_level & ACPI_NO_ACPI_ENABLE))
        return (AE_ERROR);

      /*
       * according to Intel ACPI developers, SCI
       * conforms to PCI bus conventions; level/low
       * unless otherwise directed by overrides.
       */
      sci_flags->intr_el = INTR_EL_LEVEL;
      sci_flags->intr_po = INTR_PO_ACTIVE_LOW;
      sci_flags->bustype = BUS_PCI; /*  we *do* conform to PCI */

      /* get the SCI from the FADT */
      if (AcpiGetTable(ACPI_SIG_FADT, 1, (ACPI_TABLE_HEADER **)&fadt) != AE_OK)
        return (AE_ERROR);

      *sci_irq = fadt->SciInterrupt;

      /* search for ISOs that modify it */
      /* if we don't find a MADT, that's OK; no ISOs then */
      if (AcpiGetTable(ACPI_SIG_MADT, 1, (ACPI_TABLE_HEADER **)&mat) != AE_OK)
        return (AE_OK);

      ap = (ACPI_SUBTABLE_HEADER *)(mat + 1);
      madt_size = mat->Header.Length;
      madt_seen = sizeof(*mat);

      while (madt_seen < madt_size) {
        switch (ap->Type) {
        case ACPI_MADT_TYPE_INTERRUPT_OVERRIDE:
          mio = (ACPI_MADT_INTERRUPT_OVERRIDE *)ap;
          if (mio->SourceIrq == *sci_irq) {
            *sci_irq = mio->GlobalIrq;
            sci_flags->intr_el = (mio->IntiFlags & ACPI_MADT_TRIGGER_MASK) >> 2;
            sci_flags->intr_po = mio->IntiFlags & ACPI_MADT_POLARITY_MASK;
          }
          break;
        }

        /* advance to next entry */
        madt_seen += ap->Length;
        ap = (ACPI_SUBTABLE_HEADER *)(((char *)ap) + ap->Length);
      }

      /*
       * One more check; if ISO said "conform", revert to default
       */
      if (sci_flags->intr_el == INTR_EL_CONFORM)
        sci_flags->intr_el = INTR_EL_LEVEL;
      if (sci_flags->intr_po == INTR_PO_CONFORM)
        sci_flags->intr_po = INTR_PO_ACTIVE_LOW;

      return (AE_OK);
    }

    /*
     * Call-back function used for _PRW initialization.  For every
     * device node that has a _PRW method, evaluate, parse, and do
     * AcpiSetupGpeForWake().
     */
    static ACPI_STATUS acpica_init_PRW(ACPI_HANDLE devhdl, UINT32 depth,
                                       void *ctxp, void **rvpp) {
      ACPI_STATUS status;
      ACPI_BUFFER prw_buf;
      struct parsed_prw prw;

      prw_buf.Pointer = NULL;
      prw_buf.Length = ACPI_ALLOCATE_BUFFER;

      /*
       * Attempt to evaluate _PRW object.
       * If no valid object is found, return quietly, since not all
       * devices have _PRW objects.
       */
      status = AcpiEvaluateObject(devhdl, "_PRW", NULL, &prw_buf);
      if (ACPI_FAILURE(status))
        goto done;
      status = acpica_parse_PRW(&prw_buf, &prw);
      if (ACPI_FAILURE(status))
        goto done;

      (void)AcpiSetupGpeForWake(devhdl, prw.prw_gpeobj, prw.prw_gpebit);

    done:
      if (prw_buf.Pointer != NULL)
        AcpiOsFree(prw_buf.Pointer);

      return (AE_OK);
    }

    /*
     * Sets ACPI wake state for device referenced by dip.
     * If level is S0 (0), disables wake event; otherwise,
     * enables wake event which will wake system from level.
     */
    static int acpica_ddi_setwake(dev_info_t * dip, int level) {
      ACPI_STATUS status;
      ACPI_HANDLE devobj;
      ACPI_BUFFER prw_buf;
      ACPI_OBJECT_LIST arglist;
      ACPI_OBJECT args[3];
      struct parsed_prw prw;
      int rv;

      /*
       * initialize these early so we can use a common
       * exit point below
       */
      prw_buf.Pointer = NULL;
      prw_buf.Length = ACPI_ALLOCATE_BUFFER;
      rv = 0;

      /*
       * Attempt to get a handle to a corresponding ACPI object.
       * If no object is found, return quietly, since not all
       * devices have corresponding ACPI objects.
       */
      status = acpica_get_handle(dip, &devobj);
      if (ACPI_FAILURE(status)) {
        char pathbuf[MAXPATHLEN];
        ddi_pathname(dip, pathbuf);
#ifdef DEBUG
        cmn_err(CE_NOTE,
                "!acpica_ddi_setwake: could not get"
                " handle for %s, %s:%d",
                pathbuf, ddi_driver_name(dip), ddi_get_instance(dip));
#endif
        goto done;
      }

      /*
       * ACPI3.0 7.2.1: only use the _PSW method if OSPM does not support
       * _DSW or if the _DSW method is not present.
       *
       * _DSW arguments:
       * args[0] - Enable/Disable
       * args[1] - Target system state
       * args[2] - Target device state
       */

      arglist.Count = 3;
      arglist.Pointer = args;
      args[0].Type = ACPI_TYPE_INTEGER;
      args[0].Integer.Value = level ? 1 : 0;
      args[1].Type = ACPI_TYPE_INTEGER;
      args[1].Integer.Value = level;
      args[2].Type = ACPI_TYPE_INTEGER;
      args[2].Integer.Value = level;
      if (ACPI_FAILURE(
              status = AcpiEvaluateObject(devobj, "_DSW", &arglist, NULL))) {

        if (status == AE_NOT_FOUND) {
          arglist.Count = 1;
          args[0].Type = ACPI_TYPE_INTEGER;
          args[0].Integer.Value = level ? 1 : 0;

          if (ACPI_FAILURE(status = AcpiEvaluateObject(devobj, "_PSW", &arglist,
                                                       NULL))) {

            if (status != AE_NOT_FOUND) {
              cmn_err(CE_NOTE, "!_PSW failure %d for device %s", status,
                      ddi_driver_name(dip));
            }
          }

        } else {
          cmn_err(CE_NOTE, "!_DSW failure %d for device %s", status,
                  ddi_driver_name(dip));
        }
      }

      /*
       * Attempt to evaluate _PRW object.
       * If no valid object is found, return quietly, since not all
       * devices have _PRW objects.
       */
      status = AcpiEvaluateObject(devobj, "_PRW", NULL, &prw_buf);
      if (ACPI_FAILURE(status))
        goto done;
      status = acpica_parse_PRW(&prw_buf, &prw);
      if (ACPI_FAILURE(status))
        goto done;

      rv = -1;
      if (level == 0) {
        status = AcpiDisableGpe(prw.prw_gpeobj, prw.prw_gpebit);
        if (ACPI_FAILURE(status))
          goto done;
      } else if (prw.prw_level >= level) {
        status =
            AcpiSetGpeWakeMask(prw.prw_gpeobj, prw.prw_gpebit, ACPI_GPE_ENABLE);
        if (ACPI_SUCCESS(status)) {
          status = AcpiEnableGpe(prw.prw_gpeobj, prw.prw_gpebit);
          if (ACPI_FAILURE(status))
            goto done;
        }
      }
      rv = 0;
    done:
      if (prw_buf.Pointer != NULL)
        AcpiOsFree(prw_buf.Pointer);
      return (rv);
    }

    static ACPI_STATUS acpica_parse_PRW(ACPI_BUFFER * prw_buf,
                                        struct parsed_prw * p_prw) {
      ACPI_HANDLE gpeobj;
      ACPI_OBJECT *prw, *gpe;
      int gpebit, prw_level;

      if (prw_buf->Length == 0 || prw_buf->Pointer == NULL)
        return (AE_NULL_OBJECT);

      prw = prw_buf->Pointer;
      if (prw->Type != ACPI_TYPE_PACKAGE || prw->Package.Count < 2 ||
          prw->Package.Elements[1].Type != ACPI_TYPE_INTEGER)
        return (AE_TYPE);

      /* fetch the lowest wake level from the _PRW */
      prw_level = prw->Package.Elements[1].Integer.Value;

      /*
       * process the GPE description
       */
      switch (prw->Package.Elements[0].Type) {
      case ACPI_TYPE_INTEGER:
        gpeobj = NULL;
        gpebit = prw->Package.Elements[0].Integer.Value;
        break;
      case ACPI_TYPE_PACKAGE:
        gpe = &prw->Package.Elements[0];
        if (gpe->Package.Count != 2 ||
            gpe->Package.Elements[1].Type != ACPI_TYPE_INTEGER)
          return (AE_TYPE);
        gpeobj = gpe->Package.Elements[0].Reference.Handle;
        gpebit = gpe->Package.Elements[1].Integer.Value;
        if (gpeobj == NULL)
          return (AE_NULL_OBJECT);
        break;
      default:
        return (AE_TYPE);
      }

      p_prw->prw_gpeobj = gpeobj;
      p_prw->prw_gpebit = gpebit;
      p_prw->prw_level = prw_level;

      return (AE_OK);
    }

    /*
     * kstat access to a limited set of ACPI propertis
     */
    static void acpica_init_kstats() {
      ACPI_HANDLE s3handle;
      ACPI_STATUS status;
      ACPI_TABLE_FADT *fadt;
      kstat_named_t *knp;

      /*
       * Create a small set of named kstats; just return in the rare
       * case of a failure, * in which case, the kstats won't be present.
       */
      if ((acpica_ksp = kstat_create("acpi", 0, "acpi", "misc",
                                     KSTAT_TYPE_NAMED, 2, 0)) == NULL)
        return;

      /*
       * initialize kstat 'S3' to reflect the presence of \_S3 in
       * the ACPI namespace (1 = present, 0 = not present)
       */
      knp = acpica_ksp->ks_data;
      knp->value.l = (AcpiGetHandle(NULL, "\\_S3", &s3handle) == AE_OK);
      kstat_named_init(knp, "S3", KSTAT_DATA_LONG);
      knp++; /* advance to next named kstat */

      /*
       * initialize kstat 'preferred_pm_profile' to the value
       * contained in the (always present) FADT
       */
      status = AcpiGetTable(ACPI_SIG_FADT, 1, (ACPI_TABLE_HEADER **)&fadt);
      knp->value.l = (status == AE_OK) ? fadt->PreferredProfile : -1;
      kstat_named_init(knp, "preferred_pm_profile", KSTAT_DATA_LONG);

      /*
       * install the named kstats
       */
      kstat_install(acpica_ksp);
    }

    /*
     * Attempt to save the current ACPI settings (_CRS) for the device
     * which corresponds to the supplied devinfo node.  The settings are
     * saved as a property on the dip.  If no ACPI object is found to be
     * associated with the devinfo node, no action is taken and no error
     * is reported.
     */
    void acpica_ddi_save_resources(dev_info_t * dip) {
      ACPI_HANDLE devobj;
      ACPI_BUFFER resbuf;
      int ret;

      resbuf.Length = ACPI_ALLOCATE_BUFFER;
      if (ACPI_FAILURE(acpica_get_handle(dip, &devobj)) ||
          ACPI_FAILURE(AcpiGetCurrentResources(devobj, &resbuf)))
        return;

      ret = ddi_prop_create(DDI_DEV_T_NONE, dip, DDI_PROP_CANSLEEP, "acpi-crs",
                            resbuf.Pointer, resbuf.Length);

      ASSERT(ret == DDI_PROP_SUCCESS);

      AcpiOsFree(resbuf.Pointer);
    }

    /*
     * If the supplied devinfo node has an ACPI settings property attached,
     * restore them to the associated ACPI device using _SRS.  The property
     * is deleted from the devinfo node afterward.
     */
    void acpica_ddi_restore_resources(dev_info_t * dip) {
      ACPI_HANDLE devobj;
      ACPI_BUFFER resbuf;
      uchar_t *propdata;
      uint_t proplen;

      if (ACPI_FAILURE(acpica_get_handle(dip, &devobj)))
        return;

      if (ddi_prop_lookup_byte_array(DDI_DEV_T_ANY, dip, DDI_PROP_DONTPASS,
                                     "acpi-crs", &propdata,
                                     &proplen) != DDI_PROP_SUCCESS)
        return;

      resbuf.Pointer = propdata;
      resbuf.Length = proplen;
      (void)AcpiSetCurrentResources(devobj, &resbuf);
      ddi_prop_free(propdata);
      (void)ddi_prop_remove(DDI_DEV_T_NONE, dip, "acpi-crs");
    }

    void acpi_reset_system(void) {
      ACPI_STATUS status;
      int ten;

      status = AcpiReset();
      if (status == AE_OK) {
        /*
         * Wait up to 500 milliseconds for AcpiReset() to make its
         * way.
         */
        ten = 50000;
        while (ten-- > 0)
          tenmicrosec();
      }
    }
    /**
     * @section kernel_uts_intel_io_acpica_acpi_enum_c
     * @brief Original file: minix/kernel/uts/intel/io/acpica/acpi_enum.c
     */
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
     * Copyright 2009 Sun Microsystems, Inc.  All rights reserved.
     * Use is subject to license terms.
     *
     * Copyright (c) 2012 Gary Mills
     * Copyright 2018, Joyent, Inc.
     * Copyright 2021 Racktop Systems, Inc.
     */

    /*
     * ACPI enumerator
     */

#include <sys/acpi/acpi.h>
#include <sys/acpica.h>
#include <sys/ddi.h>
#include <sys/note.h>
#include <sys/sunddi.h>
#include <sys/sunndi.h>
#include <util/qsort.h>
#include <util/sscanf.h>

    /*
     * Used to track the interrupts used by a resource, as well as the set of
     * interrupts used overall. The IRQ values are ints for historical purposes
     * (the "interrupts" property has traditionally been an array of ints) even
     * though negative IRQ values do not make much sense.
     */
    typedef struct intrs {
      int *i_intrs;
      uint_t i_num;
      uint_t i_alloc;
    } intrs_t;

    static uint32_t acpi_enum_debug = 0x00;
#define PARSE_RESOURCES_DEBUG 0x0001
#define ISAPNP_LOOKUP_DEBUG 0x0002
#define DEVICES_NOT_ENUMED 0x0004
#define PARSE_RES_IRQ 0x0008
#define PARSE_RES_DMA 0x0010
#define PARSE_RES_MEMORY 0x0020
#define PARSE_RES_IO 0x0040
#define PARSE_RES_ADDRESS 0x0080
#define ISA_DEVICE_ENUM 0x1000
#define PROCESS_CIDS 0x2000

    static dev_info_t *usedrdip = NULL;
    static intrs_t used_interrupts;
    static unsigned short used_dmas = 0;
    typedef struct used_io_mem {
      unsigned int start_addr;
      unsigned int length;
      struct used_io_mem *next;
    } used_io_mem_t;
    static used_io_mem_t *used_io_head = NULL;
    static used_io_mem_t *used_mem_head = NULL;
    static int used_io_count = 0;
    static int used_mem_count = 0;

#define MAX_PARSED_ACPI_RESOURCES 255
#define ACPI_ISA_LIMIT 16
    static int dma[ACPI_ISA_LIMIT];
#define ACPI_ELEMENT_PACKAGE_LIMIT 32
#define EISA_ID_SIZE 7

    static void add_interrupt(intrs_t * intrs, int irq) {
      /* We only want to add the value once */
      for (uint_t i = 0; i < intrs->i_num; i++) {
        if (intrs->i_intrs[i] == irq)
          return;
      }

      /*
       * Initially, i_num and i_alloc will be 0, and we allocate
       * i_intrs to hold ACPI_ISA_LIMIT values on the initial add attempt.
       * Since ISA buses could only use at most ACPI_ISA_LIMIT (16)
       * interrupts, this seems like a reasonable size. The extended IRQ
       * resource however exists explicitly to support IRQ values beyond
       * 16. That suggests it may be possible on some hardware to exceed
       * the initial allocation. If we do exceed the initial allocation, we
       * grow i_intrs in chunks of ACPI_ISA_LIMIT since that's as good an
       * amount as any.
       */
      if (intrs->i_num == intrs->i_alloc) {
        uint_t newlen = intrs->i_alloc + ACPI_ISA_LIMIT;
        size_t newsz = newlen * sizeof(int);
        size_t oldsz = intrs->i_alloc * sizeof(int);
        int *newar = kmem_alloc(newsz, KM_SLEEP);

        if (intrs->i_num > 0) {
          bcopy(intrs->i_intrs, newar, oldsz);
          kmem_free(intrs->i_intrs, oldsz);
        }

        intrs->i_intrs = newar;
        intrs->i_alloc = newlen;
      }

      intrs->i_intrs[intrs->i_num++] = irq;
    }

    /*
     * insert used io/mem in increasing order
     */
    static void insert_used_resource(used_io_mem_t * used, int *used_count,
                                     used_io_mem_t **head) {
      used_io_mem_t *curr, *prev;

      (*used_count)++;
      if (*head == NULL) {
        *head = used;
        return;
      }
      curr = prev = *head;
      /* find a place to insert */
      while ((curr != NULL) && (curr->start_addr < used->start_addr)) {
        prev = curr;
        curr = curr->next;
      }
      if (prev == curr) {
        /* head */
        *head = used;
        used->next = curr;
        return;
      } else {
        prev->next = used;
      }
      used->next = curr;
    }

    static void add_used_io_mem(struct regspec * io, int io_count) {
      int i;
      used_io_mem_t *used;

      for (i = 0; i < io_count; i++) {
        used = kmem_zalloc(sizeof(used_io_mem_t), KM_SLEEP);
        used->start_addr = io[i].regspec_addr;
        used->length = io[i].regspec_size;
        if (io[i].regspec_bustype == 1) {
          insert_used_resource(used, &used_io_count, &used_io_head);
        } else {
          insert_used_resource(used, &used_mem_count, &used_mem_head);
        }
      }
    }

    static void parse_resources_irq(ACPI_RESOURCE * resource_ptr,
                                    intrs_t * intrs) {
      uint_t i;

      for (i = 0; i < resource_ptr->Data.Irq.InterruptCount; i++) {
        uint8_t irq = resource_ptr->Data.Irq.Interrupts[i];

        add_interrupt(intrs, irq);
        add_interrupt(&used_interrupts, irq);

        if (acpi_enum_debug & PARSE_RES_IRQ) {
          cmn_err(CE_NOTE, "!%s() IRQ num %u, intr # = %u", __func__, i, irq);
        }
      }
    }

    static void parse_resources_extended_irq(ACPI_RESOURCE * resource_ptr,
                                             intrs_t * intrs) {
      uint_t i;

      for (i = 0; i < resource_ptr->Data.ExtendedIrq.InterruptCount; i++) {
        uint32_t irq = resource_ptr->Data.ExtendedIrq.Interrupts[i];

        /*
         * As noted in the definition of intrs_t above, traditionally
         * the "interrupts" property is an array of ints. This is
         * more precautionary than anything since it seems unlikely
         * that anything will have an irq value > 2^31 anytime soon.
         */
        if (irq > INT32_MAX) {
          if (acpi_enum_debug & PARSE_RES_IRQ) {
            cmn_err(CE_NOTE, "!%s() intr # = %u out of range", __func__, irq);
          }
          continue;
        }

        add_interrupt(intrs, irq);
        add_interrupt(&used_interrupts, irq);

        if (acpi_enum_debug & PARSE_RES_IRQ) {
          cmn_err(CE_NOTE, "!%s() IRQ num %u, intr # = %u", __func__, i, irq);
        }
      }
    }

    static void parse_resources_dma(ACPI_RESOURCE * resource_ptr,
                                    int *dma_count) {
      int i;

      for (i = 0; i < resource_ptr->Data.Dma.ChannelCount; i++) {
        dma[(*dma_count)++] = resource_ptr->Data.Dma.Channels[i];
        used_dmas |= 1 << resource_ptr->Data.Dma.Channels[i];
        if (acpi_enum_debug & PARSE_RES_DMA) {
          cmn_err(CE_NOTE,
                  "!parse_resources() "
                  "DMA num %u, channel # = %u",
                  i, resource_ptr->Data.Dma.Channels[i]);
        }
      }
    }

    static void parse_resources_io(ACPI_RESOURCE * resource_ptr,
                                   struct regspec * io, int *io_count) {
      ACPI_RESOURCE_IO acpi_io = resource_ptr->Data.Io;

      if (acpi_io.AddressLength == 0)
        return;

      io[*io_count].regspec_bustype = 1; /* io */
      io[*io_count].regspec_size = acpi_io.AddressLength;
      io[*io_count].regspec_addr = acpi_io.Minimum;
      if (acpi_enum_debug & PARSE_RES_IO) {
        cmn_err(CE_NOTE,
                "!parse_resources() "
                "IO min 0x%X, max 0x%X, length: 0x%X",
                acpi_io.Minimum, acpi_io.Maximum, acpi_io.AddressLength);
      }
      (*io_count)++;
    }

    static void parse_resources_fixed_io(ACPI_RESOURCE * resource_ptr,
                                         struct regspec * io, int *io_count) {
      ACPI_RESOURCE_FIXED_IO fixed_io = resource_ptr->Data.FixedIo;

      if (fixed_io.AddressLength == 0)
        return;

      io[*io_count].regspec_bustype = 1; /* io */
      io[*io_count].regspec_addr = fixed_io.Address;
      io[*io_count].regspec_size = fixed_io.AddressLength;
      if (acpi_enum_debug & PARSE_RES_IO) {
        cmn_err(CE_NOTE,
                "!parse_resources() "
                "Fixed IO 0x%X, length: 0x%X",
                fixed_io.Address, fixed_io.AddressLength);
      }
      (*io_count)++;
    }

    static void parse_resources_fixed_mem32(
        ACPI_RESOURCE * resource_ptr, struct regspec * io, int *io_count) {
      ACPI_RESOURCE_FIXED_MEMORY32 fixed_mem32 =
          resource_ptr->Data.FixedMemory32;

      if (fixed_mem32.AddressLength == 0)
        return;

      io[*io_count].regspec_bustype = 0; /* memory */
      io[*io_count].regspec_addr = fixed_mem32.Address;
      io[*io_count].regspec_size = fixed_mem32.AddressLength;
      if (acpi_enum_debug & PARSE_RES_MEMORY) {
        cmn_err(CE_NOTE,
                "!parse_resources() "
                "Fixed Mem 32 %ul, length: %ul",
                fixed_mem32.Address, fixed_mem32.AddressLength);
      }
      (*io_count)++;
    }

    static void parse_resources_mem32(ACPI_RESOURCE * resource_ptr,
                                      struct regspec * io, int *io_count) {
      ACPI_RESOURCE_MEMORY32 mem32 = resource_ptr->Data.Memory32;

      if (mem32.AddressLength == 0)
        return;

      if (resource_ptr->Data.Memory32.Minimum ==
          resource_ptr->Data.Memory32.Maximum) {
        io[*io_count].regspec_bustype = 0; /* memory */
        io[*io_count].regspec_addr = mem32.Minimum;
        io[*io_count].regspec_size = mem32.AddressLength;
        (*io_count)++;
        if (acpi_enum_debug & PARSE_RES_MEMORY) {
          cmn_err(CE_NOTE,
                  "!parse_resources() "
                  "Mem 32 0x%X, length: 0x%X",
                  mem32.Minimum, mem32.AddressLength);
        }
        return;
      }
      if (acpi_enum_debug & PARSE_RES_MEMORY) {
        cmn_err(CE_NOTE, "!parse_resources() "
                         "MEM32 Min Max not equal!");
        cmn_err(CE_NOTE,
                "!parse_resources() "
                "Mem 32 Minimum 0x%X, Maximum: 0x%X",
                mem32.Minimum, mem32.Maximum);
      }
    }

    static void parse_resources_addr16(ACPI_RESOURCE * resource_ptr,
                                       struct regspec * io, int *io_count) {
      ACPI_RESOURCE_ADDRESS16 addr16 = resource_ptr->Data.Address16;

      if (addr16.Address.AddressLength == 0)
        return;

      if (acpi_enum_debug & PARSE_RES_ADDRESS) {
        if (addr16.ResourceType == ACPI_MEMORY_RANGE) {
          cmn_err(CE_NOTE, "!parse_resources() "
                           "ADDRESS 16 MEMORY RANGE");
        } else if (addr16.ResourceType == ACPI_IO_RANGE) {
          cmn_err(CE_NOTE, "!parse_resources() "
                           "ADDRESS 16 IO RANGE");
        } else {
          cmn_err(CE_NOTE, "!parse_resources() "
                           "ADDRESS 16 OTHER");
        }
        cmn_err(CE_NOTE,
                "!parse_resources() "
                "%s "
                "MinAddressFixed 0x%X, "
                "MaxAddressFixed 0x%X, "
                "Minimum 0x%X, "
                "Maximum 0x%X, "
                "length: 0x%X\n",
                addr16.ProducerConsumer == ACPI_CONSUMER ? "CONSUMER"
                                                         : "PRODUCER",
                addr16.MinAddressFixed, addr16.MaxAddressFixed,
                addr16.Address.Minimum, addr16.Address.Maximum,
                addr16.Address.AddressLength);
      }
      if (addr16.ProducerConsumer == ACPI_PRODUCER ||
          (addr16.ResourceType != ACPI_MEMORY_RANGE &&
           addr16.ResourceType != ACPI_IO_RANGE)) {
        return;
      }
      if (addr16.Address.AddressLength > 0) {
        if (addr16.ResourceType == ACPI_MEMORY_RANGE) {
          /* memory */
          io[*io_count].regspec_bustype = 0;
        } else {
          /* io */
          io[*io_count].regspec_bustype = 1;
        }
        io[*io_count].regspec_addr = addr16.Address.Minimum;
        io[*io_count].regspec_size = addr16.Address.AddressLength;
        (*io_count)++;
      }
    }

    static void parse_resources_addr32(ACPI_RESOURCE * resource_ptr,
                                       struct regspec * io, int *io_count) {
      ACPI_RESOURCE_ADDRESS32 addr32 = resource_ptr->Data.Address32;

      if (addr32.Address.AddressLength == 0)
        return;

      if (acpi_enum_debug & PARSE_RES_ADDRESS) {
        if (addr32.ResourceType == ACPI_MEMORY_RANGE) {
          cmn_err(CE_NOTE, "!parse_resources() "
                           "ADDRESS 32 MEMORY RANGE");
        } else if (addr32.ResourceType == ACPI_IO_RANGE) {
          cmn_err(CE_NOTE, "!parse_resources() "
                           "ADDRESS 32 IO RANGE");
        } else {
          cmn_err(CE_NOTE, "!parse_resources() "
                           "ADDRESS 32 OTHER");
        }
        cmn_err(CE_NOTE,
                "!parse_resources() "
                "%s "
                "MinAddressFixed 0x%X, "
                "MaxAddressFixed 0x%X, "
                "Minimum 0x%X, "
                "Maximum 0x%X, "
                "length: 0x%X\n",
                addr32.ProducerConsumer == ACPI_CONSUMER ? "CONSUMER"
                                                         : "PRODUCER",
                addr32.MinAddressFixed, addr32.MaxAddressFixed,
                addr32.Address.Minimum, addr32.Address.Maximum,
                addr32.Address.AddressLength);
      }
      if (addr32.ProducerConsumer == ACPI_PRODUCER ||
          (addr32.ResourceType != ACPI_MEMORY_RANGE &&
           addr32.ResourceType != ACPI_IO_RANGE)) {
        return;
      }
      if (addr32.Address.AddressLength > 0) {
        if (addr32.ResourceType == ACPI_MEMORY_RANGE) {
          /* memory */
          io[*io_count].regspec_bustype = 0;
        } else {
          /* io */
          io[*io_count].regspec_bustype = 1;
        }
        io[*io_count].regspec_addr = addr32.Address.Minimum;
        io[*io_count].regspec_size = addr32.Address.AddressLength;
        (*io_count)++;
      }
    }

    static void parse_resources_addr64(ACPI_RESOURCE * resource_ptr,
                                       struct regspec * io, int *io_count) {
      ACPI_RESOURCE_ADDRESS64 addr64 = resource_ptr->Data.Address64;

      if (addr64.Address.AddressLength == 0)
        return;

      if (acpi_enum_debug & PARSE_RES_ADDRESS) {
        if (addr64.ResourceType == ACPI_MEMORY_RANGE) {
          cmn_err(CE_NOTE, "!parse_resources() "
                           "ADDRESS 64 MEMORY RANGE");
        } else if (addr64.ResourceType == ACPI_IO_RANGE) {
          cmn_err(CE_NOTE, "!parse_resources() "
                           "ADDRESS 64 IO RANGE");
        } else {
          cmn_err(CE_NOTE, "!parse_resources() "
                           "ADDRESS 64 OTHER");
        }
#ifdef _LP64
        cmn_err(CE_NOTE,
                "!parse_resources() "
                "%s "
                "MinAddressFixed 0x%X, "
                "MaxAddressFixed 0x%X, "
                "Minimum 0x%lX, "
                "Maximum 0x%lX, "
                "length: 0x%lX\n",
                addr64.ProducerConsumer == ACPI_CONSUMER ? "CONSUMER"
                                                         : "PRODUCER",
                addr64.MinAddressFixed, addr64.MaxAddressFixed,
                addr64.Address.Minimum, addr64.Address.Maximum,
                addr64.Address.AddressLength);
#else
        cmn_err(CE_NOTE,
                "!parse_resources() "
                "%s "
                "MinAddressFixed 0x%X, "
                "MaxAddressFixed 0x%X, "
                "Minimum 0x%llX, "
                "Maximum 0x%llX, "
                "length: 0x%llX\n",
                addr64.ProducerConsumer == ACPI_CONSUMER ? "CONSUMER"
                                                         : "PRODUCER",
                addr64.MinAddressFixed, addr64.MaxAddressFixed,
                addr64.Address.Minimum, addr64.Address.Maximum,
                addr64.Address.AddressLength);
#endif
      }
      if (addr64.ProducerConsumer == ACPI_PRODUCER ||
          (addr64.ResourceType != ACPI_MEMORY_RANGE &&
           addr64.ResourceType != ACPI_IO_RANGE)) {
        return;
      }
      if (addr64.Address.AddressLength > 0) {
        if (addr64.ResourceType == ACPI_MEMORY_RANGE) {
          /* memory */
          io[*io_count].regspec_bustype = 0;
        } else {
          /* io */
          io[*io_count].regspec_bustype = 1;
        }
        io[*io_count].regspec_addr = addr64.Address.Minimum;
        io[*io_count].regspec_size = addr64.Address.AddressLength;
        (*io_count)++;
      }
    }

    static ACPI_STATUS parse_resources(ACPI_HANDLE handle, dev_info_t * xdip,
                                       char *path) {
      ACPI_BUFFER buf;
      ACPI_RESOURCE *resource_ptr;
      ACPI_STATUS status;
      char *current_ptr, *last_ptr;
      struct regspec *io;
      intrs_t intrs = {0};
      int io_count = 0, dma_count = 0;
      int i;

      buf.Length = ACPI_ALLOCATE_BUFFER;
      status = AcpiGetCurrentResources(handle, &buf);
      switch (status) {
      case AE_OK:
        break;
      case AE_NOT_FOUND:
        /*
         * Workaround for faulty DSDT tables that omit the _CRS
         * method for the UAR3 device but have a valid _PRS method
         * for that device.
         */
        status = AcpiGetPossibleResources(handle, &buf);
        if (status != AE_OK) {
          return (status);
        }
        break;
      default:
        cmn_err(CE_WARN,
                "!AcpiGetCurrentResources failed for %s, exception: %s", path,
                AcpiFormatException(status));
        return (status);
        break;
      }
      io = kmem_zalloc(sizeof(struct regspec) * MAX_PARSED_ACPI_RESOURCES,
                       KM_SLEEP);
      current_ptr = buf.Pointer;
      last_ptr = (char *)buf.Pointer + buf.Length;
      while (current_ptr < last_ptr) {
        if (io_count >= MAX_PARSED_ACPI_RESOURCES) {
          break;
        }
        resource_ptr = (ACPI_RESOURCE *)current_ptr;
        current_ptr += resource_ptr->Length;
        switch (resource_ptr->Type) {
        case ACPI_RESOURCE_TYPE_END_TAG:
          current_ptr = last_ptr;
          break;
        case ACPI_RESOURCE_TYPE_IO:
          parse_resources_io(resource_ptr, io, &io_count);
          break;
        case ACPI_RESOURCE_TYPE_FIXED_IO:
          parse_resources_fixed_io(resource_ptr, io, &io_count);
          break;
        case ACPI_RESOURCE_TYPE_FIXED_MEMORY32:
          parse_resources_fixed_mem32(resource_ptr, io, &io_count);
          break;
        case ACPI_RESOURCE_TYPE_MEMORY32:
          parse_resources_mem32(resource_ptr, io, &io_count);
          break;
        case ACPI_RESOURCE_TYPE_ADDRESS16:
          parse_resources_addr16(resource_ptr, io, &io_count);
          break;
        case ACPI_RESOURCE_TYPE_ADDRESS32:
          parse_resources_addr32(resource_ptr, io, &io_count);
          break;
        case ACPI_RESOURCE_TYPE_ADDRESS64:
          parse_resources_addr64(resource_ptr, io, &io_count);
          break;
        case ACPI_RESOURCE_TYPE_IRQ:
          parse_resources_irq(resource_ptr, &intrs);
          break;
        case ACPI_RESOURCE_TYPE_DMA:
          parse_resources_dma(resource_ptr, &dma_count);
          break;
        case ACPI_RESOURCE_TYPE_START_DEPENDENT:
          cmn_err(CE_NOTE, "!ACPI source type"
                           " ACPI_RESOURCE_TYPE_START_DEPENDENT"
                           " not supported");
          break;
        case ACPI_RESOURCE_TYPE_END_DEPENDENT:
          cmn_err(CE_NOTE, "!ACPI source type"
                           " ACPI_RESOURCE_TYPE_END_DEPENDENT"
                           " not supported");
          break;
        case ACPI_RESOURCE_TYPE_VENDOR:
          cmn_err(CE_NOTE, "!ACPI source type"
                           " ACPI_RESOURCE_TYPE_VENDOR"
                           " not supported");
          break;
        case ACPI_RESOURCE_TYPE_MEMORY24:
          cmn_err(CE_NOTE, "!ACPI source type"
                           " ACPI_RESOURCE_TYPE_MEMORY24"
                           " not supported");
          break;
        case ACPI_RESOURCE_TYPE_EXTENDED_IRQ:
          parse_resources_extended_irq(resource_ptr, &intrs);
          break;
        default:
          /* Some types are not yet implemented (See CA 6.4) */
          cmn_err(CE_NOTE, "!ACPI resource type (0X%X) not yet supported",
                  resource_ptr->Type);
          break;
        }
      }

      if (io_count) {
        /*
         * on LX50, you get interrupts of mouse and keyboard
         * from separate PNP id...
         */
        if (io_count == 2) {
          if ((io[0].regspec_addr == 0x60 && io[1].regspec_addr == 0x64) ||
              (io[0].regspec_addr == 0x64 && io[1].regspec_addr == 0x60)) {
            intrs.i_num = 0;
            add_interrupt(&intrs, 0x1);
            add_interrupt(&intrs, 0xc);
            add_interrupt(&used_interrupts, 0x1);
            add_interrupt(&used_interrupts, 0xc);
          }
        }
        add_used_io_mem(io, io_count);
        if (xdip != NULL) {
          (void)ndi_prop_update_int_array(DDI_DEV_T_NONE, xdip, "reg",
                                          (int *)io, 3 * io_count);
        }
      }
      if (intrs.i_num > 0) {
        if (xdip != NULL) {
          (void)ndi_prop_update_int_array(DDI_DEV_T_NONE, xdip, "interrupts",
                                          intrs.i_intrs, intrs.i_num);
        }
        kmem_free(intrs.i_intrs, intrs.i_alloc * sizeof(int));
      }
      if (dma_count && (xdip != NULL)) {
        (void)ndi_prop_update_int_array(DDI_DEV_T_NONE, xdip, "dma-channels",
                                        (int *)dma, dma_count);
      }
      AcpiOsFree(buf.Pointer);
      kmem_free(io, sizeof(struct regspec) * MAX_PARSED_ACPI_RESOURCES);
      return (status);
    }

    /* keyboard mouse is under i8042, everything else under isa */
    static dev_info_t *get_bus_dip(const char *nodename, dev_info_t *isa_dip) {
      static dev_info_t *i8042_dip = NULL;
      struct regspec i8042_regs[] = {{1, 0x60, 0x1}, {1, 0x64, 0x1}};
      int i8042_intrs[] = {0x1, 0xc};

      if (strcmp(nodename, "keyboard") != 0 && strcmp(nodename, "mouse") != 0)
        return (isa_dip);

      if (i8042_dip)
        return (i8042_dip);

      ndi_devi_alloc_sleep(isa_dip, "i8042", (pnode_t)DEVI_SID_NODEID,
                           &i8042_dip);
      (void)ndi_prop_update_int_array(DDI_DEV_T_NONE, i8042_dip, "reg",
                                      (int *)i8042_regs, 6);
      (void)ndi_prop_update_int_array(DDI_DEV_T_NONE, i8042_dip, "interrupts",
                                      (int *)i8042_intrs, 2);
      (void)ndi_prop_update_string(DDI_DEV_T_NONE, i8042_dip, "unit-address",
                                   "1,60");
      (void)ndi_devi_bind_driver(i8042_dip, 0);
      return (i8042_dip);
    }

    void eisa_to_str(ACPI_INTEGER id, char *np) {
      static const char hextab[] = "0123456789ABCDEF";

      /*
       *  Expand an EISA device name:
       *
       * This routine converts a 32-bit EISA device "id" to a
       * 7-byte ASCII device name, which is stored at "np".
       */

      *np++ = '@' + ((id >> 2) & 0x1F);
      *np++ = '@' + ((id << 3) & 0x18) + ((id >> 13) & 0x07);
      *np++ = '@' + ((id >> 8) & 0x1F);
      *np++ = hextab[(id >> 20) & 0x0F];
      *np++ = hextab[(id >> 16) & 0x0F];
      *np++ = hextab[(id >> 28) & 0x0F];
      *np++ = hextab[(id >> 24) & 0x0F];
      *np = 0;
    }

    /*
     * process_cids() -- process multiple CIDs in a package
     */
    static void process_cids(ACPI_OBJECT * rv, device_id_t * *dd) {
      device_id_t *d;
      char tmp_cidstr[8]; /* 7-character EISA ID */
      int i;

      if ((rv->Package.Count == 0) || rv->Package.Elements == NULL)
        return; /* empty package */

      /*
       * Work the package 'backwards' so the resulting list is
       * in original order of preference.
       */
      for (i = rv->Package.Count - 1; i >= 0; i--) {
        /* get the actual acpi_object */
        ACPI_OBJECT obj = rv->Package.Elements[i];
        switch (obj.Type) {
        case ACPI_TYPE_INTEGER:
          eisa_to_str(obj.Integer.Value, tmp_cidstr);
          d = kmem_zalloc(sizeof(device_id_t), KM_SLEEP);
          d->id = strdup(tmp_cidstr);
          d->next = *dd;
          *dd = d;
          break;
        case ACPI_TYPE_STRING:
          d = kmem_zalloc(sizeof(device_id_t), KM_SLEEP);
          d->id = strdup(obj.String.Pointer);
          d->next = *dd;
          *dd = d;
          break;
        default:
          if (acpi_enum_debug & PROCESS_CIDS) {
            cmn_err(CE_NOTE, "!unexpected CID type: %d", obj.Type);
          }
          break;
        }
      }
    }

    /*
     * Convert "raw" PNP and ACPI IDs to IEEE 1275-compliant form.
     * Some liberty is taken here, treating "ACPI" as a special form
     * of PNP vendor ID.  strsize specifies size of buffer.
     */
    static void convert_to_pnp1275(char *pnpid, char *str, int strsize) {
      char vendor[5];
      uint_t id;

      if (strncmp(pnpid, "ACPI", 4) == 0) {
        /* Assume ACPI ID: ACPIxxxx */
        sscanf(pnpid, "%4s%x", vendor, &id);
      } else {
        /* Assume PNP ID: aaaxxxx */
        sscanf(pnpid, "%3s%x", vendor, &id);
      }

      snprintf(str, strsize, "pnp%s,%x", vendor, id);
    }

    /*
     * Given a list of device ID elements in most-to-least-specific
     * order, create a "compatible" property.
     */
    static void create_compatible_property(dev_info_t * dip,
                                           device_id_t * ids) {
      char **strs;
      int list_len, i;
      device_id_t *d;

      /* count list length */
      list_len = 0;
      d = ids;
      while (d != NULL) {
        list_len++;
        d = d->next;
      }

      strs = kmem_zalloc(list_len * sizeof(char *), KM_SLEEP);
      i = 0;
      d = ids;
      while (d != NULL) {
        /* strlen("pnpXXXX,xxxx") + 1 = 13 */
        strs[i] = kmem_zalloc(13, KM_SLEEP);
        convert_to_pnp1275(d->id, strs[i++], 13);
        d = d->next;
      }

      /* update property */
      (void)ndi_prop_update_string_array(DDI_DEV_T_NONE, dip, "compatible",
                                         strs, list_len);

      /* free memory */
      for (i = 0; i < list_len; i++)
        kmem_free(strs[i], 13);

      kmem_free(strs, list_len * sizeof(char *));
    }

    /*
     * isa_acpi_callback()
     */
    static ACPI_STATUS isa_acpi_callback(
        ACPI_HANDLE ObjHandle, uint32_t NestingLevel, void *a, void **b) {
      _NOTE(ARGUNUSED(NestingLevel, b))

      ACPI_BUFFER rb;
      ACPI_DEVICE_INFO *info = NULL;
      char *path = NULL;
      char *hidstr = NULL;
      char tmp_cidstr[8]; /* EISAID size */
      dev_info_t *dip = (dev_info_t *)a;
      dev_info_t *xdip = NULL;
      device_id_t *d, *device_ids = NULL;
      const isapnp_desc_t *m;
      int status;

      /*
       * get full ACPI pathname for object
       */
      rb.Length = ACPI_ALLOCATE_BUFFER;
      rb.Pointer = NULL;
      if (AcpiGetName(ObjHandle, ACPI_FULL_PATHNAME, &rb) != AE_OK) {
        cmn_err(CE_WARN, "!acpi_enum: could not get pathname");
        goto done;
      }
      path = (char *)rb.Pointer;

      /*
       * Get device info object
       */
      if (AcpiGetObjectInfo(ObjHandle, &info) != AE_OK) {
        cmn_err(CE_WARN,
                "!acpi_enum: could not get device"
                " info for %s",
                path);
        goto done;
      }

      /*
       * If device isn't present, we don't enumerate
       * NEEDSWORK: what about docking bays and the like?
       */
      if (ACPI_FAILURE(acpica_get_object_status(ObjHandle, &status))) {
        cmn_err(CE_WARN, "!acpi_enum: no _STA for %s", path);
        goto done;
      }

      /*
       * CA 6.3.6 _STA method
       * Bit 0 -- device is present
       * Bit 1 -- device is enabled
       * Bit 2 -- device is shown in UI
       */
      if ((status & 0x7) != 0x7) {
        if (acpi_enum_debug & DEVICES_NOT_ENUMED) {
          cmn_err(CE_NOTE,
                  "!parse_resources() "
                  "Bad status 0x%x for %s",
                  status, path);
        }
        goto done;
      }

      /*
       * Keep track of _HID value
       */
      if (!(info->Valid & ACPI_VALID_HID)) {
        /* No _HID, we skip this node */
        if (acpi_enum_debug & DEVICES_NOT_ENUMED) {
          cmn_err(CE_NOTE,
                  "!parse_resources() "
                  "No _HID for %s",
                  path);
        }
        goto done;
      }
      hidstr = info->HardwareId.String;

      /*
       * Attempt to get _CID value
       */
      rb.Length = ACPI_ALLOCATE_BUFFER;
      rb.Pointer = NULL;
      if (AcpiEvaluateObject(ObjHandle, "_CID", NULL, &rb) == AE_OK &&
          rb.Length != 0) {
        ACPI_OBJECT *rv = rb.Pointer;

        switch (rv->Type) {
        case ACPI_TYPE_INTEGER:
          eisa_to_str(rv->Integer.Value, tmp_cidstr);
          d = kmem_zalloc(sizeof(device_id_t), KM_SLEEP);
          d->id = strdup(tmp_cidstr);
          d->next = device_ids;
          device_ids = d;
          break;
        case ACPI_TYPE_STRING:
          d = kmem_zalloc(sizeof(device_id_t), KM_SLEEP);
          d->id = strdup(rv->String.Pointer);
          d->next = device_ids;
          device_ids = d;
          break;
        case ACPI_TYPE_PACKAGE:
          process_cids(rv, &device_ids);
          break;
        default:
          break;
        }
        AcpiOsFree(rb.Pointer);
      }

      /*
       * Add _HID last so it's at the head of the list
       */
      d = kmem_zalloc(sizeof(device_id_t), KM_SLEEP);
      d->id = strdup(hidstr);
      d->next = device_ids;
      device_ids = d;

      /*
       * isapnp_desc_lookup() expects _HID first in device_ids
       */
      if ((m = isapnp_desc_lookup(device_ids)) != NULL) {
        /* PNP description found in isapnp table */
        if (!(strncmp(hidstr, "ACPI", 4))) {
          dip = ddi_root_node();
        } else {
          dip = get_bus_dip(m->ipnp_name, dip);
        }
        ndi_devi_alloc_sleep(dip, m->ipnp_name, (pnode_t)DEVI_SID_NODEID,
                             &xdip);
        (void)ndi_prop_update_string(DDI_DEV_T_NONE, xdip, "model",
                                     (char *)m->ipnp_model);

        if (m->ipnp_compat != NULL) {
          (void)ndi_prop_update_string(DDI_DEV_T_NONE, xdip, "compatible",
                                       (char *)m->ipnp_compat);
        }
      } else {
        (void)parse_resources(ObjHandle, xdip, path);
        goto done;
      }

      (void)ndi_prop_update_string(DDI_DEV_T_NONE, xdip, "acpi-namespace",
                                   path);

      (void)parse_resources(ObjHandle, xdip, path);

      /* Special processing for mouse and keyboard devices per IEEE 1275 */
      if (strcmp(m->ipnp_name, "keyboard") == 0) {
        (void)ndi_prop_update_int(DDI_DEV_T_NONE, xdip, "reg", 0);
        (void)ndi_prop_update_string(DDI_DEV_T_NONE, xdip, "device-type",
                                     "keyboard");
      } else if (strcmp(m->ipnp_name, "mouse") == 0) {
        (void)ndi_prop_update_int(DDI_DEV_T_NONE, xdip, "reg", 1);
        (void)ndi_prop_update_string(DDI_DEV_T_NONE, xdip, "device-type",
                                     "mouse");
      }

      /*
       * Create default "compatible" property if required
       */
      if (!ddi_prop_exists(DDI_DEV_T_ANY, xdip, DDI_PROP_DONTPASS,
                           "compatible"))
        create_compatible_property(xdip, device_ids);

      (void)ndi_devi_bind_driver(xdip, 0);

    done:
      /* discard _HID/_CID list */
      d = device_ids;
      while (d != NULL) {
        device_id_t *next;

        next = d->next;
        if (d->id != NULL)
          strfree(d->id);

        kmem_free(d, sizeof(device_id_t));
        d = next;
      }

      if (path != NULL)
        AcpiOsFree(path);
      if (info != NULL)
        AcpiOsFree(info);

      return (AE_OK);
    }

    static int irq_cmp(const void *a, const void *b) {
      const int *l = a;
      const int *r = b;

      if (*l < *r)
        return (-1);
      if (*l > *r)
        return (1);
      return (0);
    }

    static void used_res_interrupts(void) {
      if (used_interrupts.i_num == 0)
        return;

      /*
       * add_known_used_resources() in usr/src/uts/i86pc.io/isa.c (used
       * when ACPI enumeration is disabled) states that the interrupt values
       * in the interrupts property of usedrdip should be in increasing order.
       * It does not state the reason for the requirement, however out of
       * an abundance of caution, we ensure the interrupt values are also
       * stored in the interrupts property in increasing order.
       */
      qsort(used_interrupts.i_intrs, used_interrupts.i_num, sizeof(int),
            irq_cmp);

      (void)ndi_prop_update_int_array(DDI_DEV_T_NONE, usedrdip, "interrupts",
                                      used_interrupts.i_intrs,
                                      used_interrupts.i_num);

      kmem_free(used_interrupts.i_intrs, used_interrupts.i_alloc * sizeof(int));
      bzero(&used_interrupts, sizeof(used_interrupts));
    }

    static void used_res_dmas(void) {
      int dma[ACPI_ISA_LIMIT];
      int count = 0;
      int i;

      for (i = 0; i < ACPI_ISA_LIMIT; i++) {
        if ((used_dmas >> i) & 1) {
          dma[count++] = i;
        }
      }
      (void)ndi_prop_update_int_array(DDI_DEV_T_NONE, usedrdip, "dma-channels",
                                      (int *)dma, count);
    }

    static void used_res_io_mem(char *nodename, int *count,
                                used_io_mem_t **head) {
      int *io;
      used_io_mem_t *used = *head;
      int i;

      *count *= 2;
      io = kmem_zalloc(sizeof(int) * (*count), KM_SLEEP);
      for (i = 0; i < *count; i += 2) {
        used_io_mem_t *prev;
        if (used != NULL) {
          io[i] = used->start_addr;
          io[i + 1] = used->length;
          prev = used;
          used = used->next;
          kmem_free(prev, sizeof(used_io_mem_t));
        }
      }
      (void)ndi_prop_update_int_array(DDI_DEV_T_NONE, usedrdip, nodename,
                                      (int *)io, *count);
      kmem_free(io, sizeof(int) * (*count));
      *head = NULL;
    }

    /*
     * acpi_isa_device_enum() -- call from isa nexus driver
     * returns 1 if deviced enumeration is successful
     *         0 if deviced enumeration fails
     */
    int acpi_isa_device_enum(dev_info_t * isa_dip) {
      char *acpi_prop;

      if (ddi_prop_lookup_string(DDI_DEV_T_ANY, ddi_root_node(),
                                 DDI_PROP_DONTPASS, "acpi_enum_debug",
                                 &acpi_prop) == DDI_PROP_SUCCESS) {
        unsigned long data;
        if (ddi_strtoul(acpi_prop, NULL, 0, &data) == 0) {
          acpi_enum_debug = (uint32_t)data;
          e_ddi_prop_remove(DDI_DEV_T_NONE, ddi_root_node(), "acpi_enum_debug");
          e_ddi_prop_update_int(DDI_DEV_T_NONE, ddi_root_node(),
                                "acpi_enum_debug", data);
        }
        ddi_prop_free(acpi_prop);
      }

      if (acpi_enum_debug & ISA_DEVICE_ENUM) {
        cmn_err(CE_NOTE, "!acpi_isa_device_enum() called");
      }

      if (acpica_init() != AE_OK) {
        cmn_err(CE_WARN, "!acpi_isa_device_enum: init failed");
        /*
         * Note: `acpi-enum` is a private boolean property that is
         * respected both as a user-set property (by the isa nexus
         * which calls us), and set by us on failure (here) to
         * communicate to the i8042 nexus that ACPI enumeration has
         * not taken place and that it must enumerate.
         */
        (void)e_ddi_prop_update_string(DDI_DEV_T_NONE, ddi_root_node(),
                                       "acpi-enum", "off");
        return (0);
      }

      usedrdip = ddi_find_devinfo("used-resources", -1, 0);
      if (usedrdip == NULL) {
        ndi_devi_alloc_sleep(ddi_root_node(), "used-resources",
                             (pnode_t)DEVI_SID_NODEID, &usedrdip);
      }

      /*
       * Do the actual enumeration.  Avoid AcpiGetDevices because it
       * has an unnecessary internal callback that duplicates
       * determining if the device is present.
       */
      (void)AcpiWalkNamespace(ACPI_TYPE_DEVICE, ACPI_ROOT_OBJECT, UINT32_MAX,
                              isa_acpi_callback, NULL, isa_dip, NULL);

      used_res_interrupts();
      used_res_dmas();
      used_res_io_mem("device-memory", &used_mem_count, &used_mem_head);
      used_res_io_mem("io-space", &used_io_count, &used_io_head);
      (void)ndi_devi_bind_driver(usedrdip, 0);

      return (1);
    }
    /**
     * @section kernel_uts_i86pc_io_acpi_acpidev_acpidev_cpu_c
     * @brief Original file:
     * minix/kernel/uts/i86pc/io/acpi/acpidev/acpidev_cpu.c
     */
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
     * Copyright 2009 Sun Microsystems, Inc.  All rights reserved.
     * Use is subject to license terms.
     * Copyright 2016 Nexenta Systems, Inc.
     */
    /*
     * Copyright (c) 2009-2010, Intel Corporation.
     * All rights reserved.
     */

    /*
     * [Support of X2APIC]
     * According to the ACPI Spec, when using the X2APIC interrupt model,
     * logical processors with APIC ID values of 255 and greater are required to
     * have a Processor Device object and must convey the Processor's APIC
     * information to OSPM using the Processor Local X2APIC structure. Logical
     * Processors with APIC ID values less than 255 must use the Processor Local
     * XAPIC structure to convey their APIC information to OSPM.
     *
     * Some systems ignore that requirement of ACPI Spec and use Processor Local
     * X2APIC structures even for Logical Processors with APIC ID values less
     * than 255.
     */

#include <sys/acpi/acpi.h>
#include <sys/acpica.h>
#include <sys/acpidev.h>
#include <sys/acpidev_impl.h>
#include <sys/atomic.h>
#include <sys/bootconf.h>
#include <sys/cpuvar.h>
#include <sys/machsystm.h>
#include <sys/note.h>
#include <sys/psm_types.h>
#include <sys/sunddi.h>
#include <sys/sunndi.h>
#include <sys/types.h>
#include <sys/x86_archext.h>

    struct acpidev_cpu_map_item {
      uint32_t proc_id;
      uint32_t apic_id;
    };

    struct acpidev_cpu_MAT_arg {
      boolean_t found;
      boolean_t enabled;
      uint32_t proc_id;
      uint32_t apic_id;
    };

    static ACPI_STATUS acpidev_cpu_pre_probe(acpidev_walk_info_t * infop);
    static ACPI_STATUS acpidev_cpu_post_probe(acpidev_walk_info_t * infop);
    static ACPI_STATUS acpidev_cpu_probe(acpidev_walk_info_t * infop);
    static acpidev_filter_result_t acpidev_cpu_filter(
        acpidev_walk_info_t * infop, char *devname, int maxlen);
    static ACPI_STATUS acpidev_cpu_init(acpidev_walk_info_t * infop);
    static void acpidev_cpu_fini(ACPI_HANDLE hdl, acpidev_data_handle_t dhdl,
                                 acpidev_class_t * clsp);

    static acpidev_filter_result_t acpidev_cpu_filter_func(
        acpidev_walk_info_t * infop, ACPI_HANDLE hdl,
        acpidev_filter_rule_t * afrp, char *devname, int len);
    static int acpidev_cpu_create_dip(cpu_t *, dev_info_t **);
    static int acpidev_cpu_get_dip(cpu_t *, dev_info_t **);

    /*
     * Default class driver for ACPI processor/CPU objects.
     */
    acpidev_class_t acpidev_class_cpu = {
        0,                      /* adc_refcnt */
        ACPIDEV_CLASS_REV1,     /* adc_version */
        ACPIDEV_CLASS_ID_CPU,   /* adc_class_id */
        "ACPI CPU",             /* adc_class_name */
        ACPIDEV_TYPE_CPU,       /* adc_dev_type */
        NULL,                   /* adc_private */
        acpidev_cpu_pre_probe,  /* adc_pre_probe */
        acpidev_cpu_post_probe, /* adc_post_probe */
        acpidev_cpu_probe,      /* adc_probe */
        acpidev_cpu_filter,     /* adc_filter */
        acpidev_cpu_init,       /* adc_init */
        acpidev_cpu_fini,       /* adc_fini */
    };

    /*
     * List of class drivers which will be called in order when handling
     * children of ACPI cpu/processor objects.
     */
    acpidev_class_list_t *acpidev_class_list_cpu = NULL;

    /* Filter rule table for the first probe at boot time. */
    static acpidev_filter_rule_t acpidev_cpu_filters[] = {
        {
            /* Skip all processors under root node, should be there. */
            NULL,
            0,
            ACPIDEV_FILTER_SKIP,
            NULL,
            1,
            1,
            NULL,
            NULL,
        },
        {
            /* Create and scan other processor objects */
            acpidev_cpu_filter_func,
            0,
            ACPIDEV_FILTER_DEFAULT,
            &acpidev_class_list_cpu,
            2,
            INT_MAX,
            NULL,
            ACPIDEV_NODE_NAME_CPU,
        }};

    /* ACPI/PNP hardware id for processor. */
    static char *acpidev_processor_device_ids[] = {
        ACPIDEV_HID_CPU,
    };

    static char *acpidev_cpu_uid_formats[] = {
        "SCK%x-CPU%x",
    };

    static ACPI_HANDLE acpidev_cpu_map_hdl;
    static uint32_t acpidev_cpu_map_count;
    static struct acpidev_cpu_map_item *acpidev_cpu_map;

    extern int (*psm_cpu_create_devinfo)(cpu_t *, dev_info_t **);
    static int (*psm_cpu_create_devinfo_old)(cpu_t *, dev_info_t **);
    extern int (*psm_cpu_get_devinfo)(cpu_t *, dev_info_t **);
    static int (*psm_cpu_get_devinfo_old)(cpu_t *, dev_info_t **);

    /* Count how many enabled CPUs are in the MADT table. */
    static ACPI_STATUS acpidev_cpu_count_MADT(ACPI_SUBTABLE_HEADER * ap,
                                              void *context) {
      uint32_t *cntp;
      ACPI_MADT_LOCAL_APIC *mpa;
      ACPI_MADT_LOCAL_X2APIC *mpx2a;

      cntp = (uint32_t *)context;
      switch (ap->Type) {
      case ACPI_MADT_TYPE_LOCAL_APIC:
        mpa = (ACPI_MADT_LOCAL_APIC *)ap;
        if (mpa->LapicFlags & ACPI_MADT_ENABLED) {
          ASSERT(mpa->Id != 255);
          (*cntp)++;
        }
        break;

      case ACPI_MADT_TYPE_LOCAL_X2APIC:
        mpx2a = (ACPI_MADT_LOCAL_X2APIC *)ap;
        if ((mpx2a->LapicFlags & ACPI_MADT_ENABLED)) {
          (*cntp)++;
        }
        break;

      default:
        break;
      }

      return (AE_OK);
    }

    /* Extract information from the enabled CPUs using the MADT table. */
    static ACPI_STATUS acpidev_cpu_parse_MADT(ACPI_SUBTABLE_HEADER * ap,
                                              void *context) {
      uint32_t *cntp;
      ACPI_MADT_LOCAL_APIC *mpa;
      ACPI_MADT_LOCAL_X2APIC *mpx2a;

      cntp = (uint32_t *)context;
      switch (ap->Type) {
      case ACPI_MADT_TYPE_LOCAL_APIC:
        mpa = (ACPI_MADT_LOCAL_APIC *)ap;
        if (mpa->LapicFlags & ACPI_MADT_ENABLED) {
          ASSERT(mpa->Id != 255);
          ASSERT(*cntp < acpidev_cpu_map_count);
          acpidev_cpu_map[*cntp].proc_id = mpa->ProcessorId;
          acpidev_cpu_map[*cntp].apic_id = mpa->Id;
          (*cntp)++;
        }
        break;

      case ACPI_MADT_TYPE_LOCAL_X2APIC:
        mpx2a = (ACPI_MADT_LOCAL_X2APIC *)ap;
        /* See comment at beginning about 255 limitation. */
        if (mpx2a->LocalApicId < 255) {
          ACPIDEV_DEBUG(CE_WARN,
                        "!acpidev: encountered CPU with X2APIC Id < 255.");
        }
        if (mpx2a->LapicFlags & ACPI_MADT_ENABLED) {
          ASSERT(*cntp < acpidev_cpu_map_count);
          acpidev_cpu_map[*cntp].proc_id = mpx2a->Uid;
          acpidev_cpu_map[*cntp].apic_id = mpx2a->LocalApicId;
          (*cntp)++;
        }
        break;

      default:
        break;
      }

      return (AE_OK);
    }

    static ACPI_STATUS acpidev_cpu_get_apicid(uint32_t procid,
                                              uint32_t *apicidp) {
      uint32_t i;

      for (i = 0; i < acpidev_cpu_map_count; i++) {
        if (acpidev_cpu_map[i].proc_id == procid) {
          *apicidp = acpidev_cpu_map[i].apic_id;
          return (AE_OK);
        }
      }

      return (AE_NOT_FOUND);
    }

    /*
     * Extract information for enabled CPUs from the buffer returned
     * by the _MAT method.
     */
    static ACPI_STATUS acpidev_cpu_query_MAT(ACPI_SUBTABLE_HEADER * ap,
                                             void *context) {
      ACPI_MADT_LOCAL_APIC *mpa;
      ACPI_MADT_LOCAL_X2APIC *mpx2a;
      struct acpidev_cpu_MAT_arg *rp;

      rp = (struct acpidev_cpu_MAT_arg *)context;
      switch (ap->Type) {
      case ACPI_MADT_TYPE_LOCAL_APIC:
        mpa = (ACPI_MADT_LOCAL_APIC *)ap;
        rp->found = B_TRUE;
        rp->proc_id = mpa->ProcessorId;
        rp->apic_id = mpa->Id;
        if (mpa->LapicFlags & ACPI_MADT_ENABLED) {
          ASSERT(mpa->Id != 255);
          rp->enabled = B_TRUE;
        } else {
          rp->enabled = B_FALSE;
        }
        return (AE_CTRL_TERMINATE);

      case ACPI_MADT_TYPE_LOCAL_X2APIC:
        mpx2a = (ACPI_MADT_LOCAL_X2APIC *)ap;
        if (mpx2a->LocalApicId < 255) {
          ACPIDEV_DEBUG(CE_WARN, "!acpidev: encountered CPU "
                                 "with X2APIC Id < 255 in _MAT.");
        }
        rp->found = B_TRUE;
        rp->proc_id = mpx2a->Uid;
        rp->apic_id = mpx2a->LocalApicId;
        if (mpx2a->LapicFlags & ACPI_MADT_ENABLED) {
          rp->enabled = B_TRUE;
        } else {
          rp->enabled = B_FALSE;
        }
        return (AE_CTRL_TERMINATE);

      case ACPI_MADT_TYPE_LOCAL_APIC_NMI:
        /* UNIMPLEMENTED */
        break;

      case ACPI_MADT_TYPE_LOCAL_X2APIC_NMI:
        /* UNIMPLEMENTED */
        break;

      default:
        /*
         * According to the ACPI Spec, the buffer returned by _MAT
         * for a processor object should only contain Local APIC,
         * Local SAPIC, and local APIC NMI entries.
         * x2APIC Specification extends it to support Processor
         * x2APIC and x2APIC NMI Structure.
         */
        ACPIDEV_DEBUG(CE_NOTE, "!acpidev: unknown APIC entry type %u in _MAT.",
                      ap->Type);
        break;
      }

      return (AE_OK);
    }

    /*
     * Query ACPI processor ID by evaluating ACPI _MAT, _UID, and PROCESSOR
     * objects.
     */
    static ACPI_STATUS acpidev_cpu_get_procid(acpidev_walk_info_t * infop,
                                              uint32_t *idp) {
      int id;
      ACPI_HANDLE hdl;
      struct acpidev_cpu_MAT_arg mat;

      if (infop->awi_info->Type != ACPI_TYPE_PROCESSOR &&
          infop->awi_info->Type != ACPI_TYPE_DEVICE) {
        ACPIDEV_DEBUG(CE_WARN,
                      "!acpidev: object %s is not PROCESSOR or DEVICE.",
                      infop->awi_name);
        return (AE_BAD_PARAMETER);
      }
      hdl = infop->awi_hdl;

      /*
       * First try to evaluate _MAT.
       * According to the ACPI Spec3.0b, it's legal for ACPI PROCESSOR objects
       * to have ACPI method objects.
       */
      bzero(&mat, sizeof(mat));
      (void)acpidev_walk_apic(NULL, hdl, ACPIDEV_METHOD_NAME_MAT,
                              acpidev_cpu_query_MAT, &mat);
      if (mat.found) {
        *idp = mat.proc_id;
        return (AE_OK);
      }

      /* Then evalute PROCESSOR object. */
      if (infop->awi_info->Type == ACPI_TYPE_PROCESSOR) {
        ACPI_BUFFER rb;

        rb.Pointer = NULL;
        rb.Length = ACPI_ALLOCATE_BUFFER;
        if (ACPI_SUCCESS(AcpiEvaluateObjectTyped(hdl, NULL, NULL, &rb,
                                                 ACPI_TYPE_PROCESSOR))) {
          *idp = ((ACPI_OBJECT *)rb.Pointer)->Processor.ProcId;
          AcpiOsFree(rb.Pointer);
          return (AE_OK);
        } else {
          ACPIDEV_DEBUG(CE_WARN, "!acpidev: failed to evaluate ACPI object %s.",
                        infop->awi_name);
        }
      }

      /*
       * Finally, try to evalute the _UID method.
       * According to the ACPI Spec3.0b, it's legal for ACPI PROCESSOR objects
       * to have ACPI method objects.
       * The CPU _UID method should return Processor Id as an integer on x86.
       */
      if (ACPI_SUCCESS(acpica_eval_int(hdl, METHOD_NAME__UID, &id))) {
        *idp = id;
        return (AE_OK);
      }

      return (AE_NOT_FOUND);
    }

    static ACPI_STATUS acpidev_cpu_get_proximity_id(
        ACPI_HANDLE hdl, uint32_t apicid, uint32_t *pxmidp) {
      int len, off;
      ACPI_SUBTABLE_HEADER *sp;
      ACPI_SRAT_CPU_AFFINITY *xp;
      ACPI_SRAT_X2APIC_CPU_AFFINITY *x2p;

      ASSERT(hdl != NULL);
      ASSERT(pxmidp != NULL);
      *pxmidp = UINT32_MAX;

      if (ACPI_SUCCESS(acpidev_eval_pxm(hdl, pxmidp))) {
        return (AE_OK);
      }
      if (acpidev_srat_tbl_ptr == NULL) {
        return (AE_NOT_FOUND);
      }

      /* Search the static ACPI SRAT table for proximity domain id. */
      sp = (ACPI_SUBTABLE_HEADER *)(acpidev_srat_tbl_ptr + 1);
      len = acpidev_srat_tbl_ptr->Header.Length;
      off = sizeof(*acpidev_srat_tbl_ptr);
      while (off < len) {
        switch (sp->Type) {
        case ACPI_SRAT_TYPE_CPU_AFFINITY:
          xp = (ACPI_SRAT_CPU_AFFINITY *)sp;
          if ((xp->Flags & ACPI_SRAT_CPU_ENABLED) && xp->ApicId == apicid) {
            *pxmidp = xp->ProximityDomainLo;
            *pxmidp |= xp->ProximityDomainHi[0] << 8;
            *pxmidp |= xp->ProximityDomainHi[1] << 16;
            *pxmidp |= xp->ProximityDomainHi[2] << 24;
            return (AE_OK);
          }
          break;

        case ACPI_SRAT_TYPE_X2APIC_CPU_AFFINITY:
          x2p = (ACPI_SRAT_X2APIC_CPU_AFFINITY *)sp;
          if ((x2p->Flags & ACPI_SRAT_CPU_ENABLED) && x2p->ApicId == apicid) {
            *pxmidp = x2p->ProximityDomain;
            return (AE_OK);
          }
          break;
        }
        off += sp->Length;
        sp = (ACPI_SUBTABLE_HEADER *)(((char *)sp) + sp->Length);
      }

      return (AE_NOT_FOUND);
    }

    static ACPI_STATUS acpidev_cpu_pre_probe(acpidev_walk_info_t * infop) {
      uint32_t count = 0;

      /* Parse and cache APIC info in MADT on the first probe at boot time. */
      ASSERT(infop != NULL);
      if (infop->awi_op_type == ACPIDEV_OP_BOOT_PROBE &&
          acpidev_cpu_map_hdl == NULL) {
        /* Parse CPU relative information in the ACPI MADT table. */
        (void)acpidev_walk_apic(NULL, NULL, NULL, acpidev_cpu_count_MADT,
                                &acpidev_cpu_map_count);
        acpidev_cpu_map = kmem_zalloc(
            sizeof(acpidev_cpu_map[0]) * acpidev_cpu_map_count, KM_SLEEP);
        (void)acpidev_walk_apic(NULL, NULL, NULL, acpidev_cpu_parse_MADT,
                                &count);
        ASSERT(count == acpidev_cpu_map_count);
        acpidev_cpu_map_hdl = infop->awi_hdl;

        /* Cache pointer to the ACPI SRAT table. */
        if (ACPI_FAILURE(
                AcpiGetTable(ACPI_SIG_SRAT, 1,
                             (ACPI_TABLE_HEADER **)&acpidev_srat_tbl_ptr))) {
          acpidev_srat_tbl_ptr = NULL;
        }
      }

      return (AE_OK);
    }

    static ACPI_STATUS acpidev_cpu_post_probe(acpidev_walk_info_t * infop) {
      /* Free cached APIC info on the second probe at boot time. */
      ASSERT(infop != NULL);
      if (infop->awi_op_type == ACPIDEV_OP_BOOT_REPROBE &&
          acpidev_cpu_map_hdl != NULL &&
          infop->awi_hdl == acpidev_cpu_map_hdl) {
        if (acpidev_cpu_map != NULL && acpidev_cpu_map_count != 0) {
          kmem_free(acpidev_cpu_map,
                    sizeof(acpidev_cpu_map[0]) * acpidev_cpu_map_count);
        }
        acpidev_cpu_map = NULL;
        acpidev_cpu_map_count = 0;
        acpidev_cpu_map_hdl = NULL;

        /* replace psm_cpu_create_devinfo with local implementation. */
        psm_cpu_create_devinfo_old = psm_cpu_create_devinfo;
        psm_cpu_create_devinfo = acpidev_cpu_create_dip;
        psm_cpu_get_devinfo_old = psm_cpu_get_devinfo;
        psm_cpu_get_devinfo = acpidev_cpu_get_dip;
      }

      return (AE_OK);
    }

    static ACPI_STATUS acpidev_cpu_probe(acpidev_walk_info_t * infop) {
      ACPI_STATUS rc = AE_OK;
      int flags;

      ASSERT(infop != NULL);
      ASSERT(infop->awi_hdl != NULL);
      ASSERT(infop->awi_info != NULL);
      ASSERT(infop->awi_class_curr == &acpidev_class_cpu);
      if (infop->awi_info->Type != ACPI_TYPE_PROCESSOR &&
          (infop->awi_info->Type != ACPI_TYPE_DEVICE ||
           acpidev_match_device_id(
               infop->awi_info,
               ACPIDEV_ARRAY_PARAM(acpidev_processor_device_ids)) == 0)) {
        return (AE_OK);
      }

      flags = ACPIDEV_PROCESS_FLAG_SCAN;
      switch (infop->awi_op_type) {
      case ACPIDEV_OP_BOOT_PROBE:
        /*
         * Mark device as offline. It will be changed to online state
         * when the corresponding CPU starts up.
         */
        if (acpica_get_devcfg_feature(ACPI_DEVCFG_CPU)) {
          flags |= ACPIDEV_PROCESS_FLAG_CREATE | ACPIDEV_PROCESS_FLAG_OFFLINE;
        }
        break;

      case ACPIDEV_OP_BOOT_REPROBE:
        break;

      case ACPIDEV_OP_HOTPLUG_PROBE:
        if (acpica_get_devcfg_feature(ACPI_DEVCFG_CPU)) {
          flags |= ACPIDEV_PROCESS_FLAG_CREATE | ACPIDEV_PROCESS_FLAG_OFFLINE |
                   ACPIDEV_PROCESS_FLAG_SYNCSTATUS |
                   ACPIDEV_PROCESS_FLAG_HOLDBRANCH;
        }
        break;

      default:
        ACPIDEV_DEBUG(CE_WARN,
                      "!acpidev: unknown operation type %u in "
                      "acpidev_cpu_probe().",
                      infop->awi_op_type);
        rc = AE_BAD_PARAMETER;
        break;
      }

      if (rc == AE_OK) {
        rc = acpidev_process_object(infop, flags);
      }
      if (ACPI_FAILURE(rc) && rc != AE_NOT_EXIST && rc != AE_ALREADY_EXISTS) {
        cmn_err(CE_WARN, "!acpidev: failed to process processor object %s.",
                infop->awi_name);
      } else {
        rc = AE_OK;
      }

      return (rc);
    }

    static acpidev_filter_result_t acpidev_cpu_filter_func(
        acpidev_walk_info_t * infop, ACPI_HANDLE hdl,
        acpidev_filter_rule_t * afrp, char *devname, int len) {
      acpidev_filter_result_t res;

      ASSERT(afrp != NULL);
      if (infop->awi_op_type == ACPIDEV_OP_BOOT_PROBE ||
          infop->awi_op_type == ACPIDEV_OP_BOOT_REPROBE) {
        uint32_t procid;
        uint32_t apicid;

        if (acpidev_cpu_get_procid(infop, &procid) != 0) {
          ACPIDEV_DEBUG(CE_WARN,
                        "!acpidev: failed to query processor id for %s.",
                        infop->awi_name);
          return (ACPIDEV_FILTER_SKIP);
        } else if (acpidev_cpu_get_apicid(procid, &apicid) != 0) {
          ACPIDEV_DEBUG(CE_WARN, "!acpidev: failed to query apic id for %s.",
                        infop->awi_name);
          return (ACPIDEV_FILTER_SKIP);
        }

        infop->awi_scratchpad[0] = procid;
        infop->awi_scratchpad[1] = apicid;
      } else if (infop->awi_op_type == ACPIDEV_OP_HOTPLUG_PROBE) {
        struct acpidev_cpu_MAT_arg mat;

        bzero(&mat, sizeof(mat));
        (void)acpidev_walk_apic(NULL, hdl, ACPIDEV_METHOD_NAME_MAT,
                                acpidev_cpu_query_MAT, &mat);
        if (!mat.found) {
          cmn_err(CE_WARN, "!acpidev: failed to walk apic resource for %s.",
                  infop->awi_name);
          return (ACPIDEV_FILTER_SKIP);
        } else if (!mat.enabled) {
          ACPIDEV_DEBUG(CE_NOTE, "!acpidev: CPU %s has been disabled.",
                        infop->awi_name);
          return (ACPIDEV_FILTER_SKIP);
        }
        /* Save processor id and APIC id in scratchpad memory. */
        infop->awi_scratchpad[0] = mat.proc_id;
        infop->awi_scratchpad[1] = mat.apic_id;
      }

      res = acpidev_filter_default(infop, hdl, afrp, devname, len);

      return (res);
    }

    static acpidev_filter_result_t acpidev_cpu_filter(
        acpidev_walk_info_t * infop, char *devname, int maxlen) {
      acpidev_filter_result_t res;

      ASSERT(infop != NULL);
      ASSERT(devname == NULL || maxlen >= ACPIDEV_MAX_NAMELEN);
      if (infop->awi_op_type == ACPIDEV_OP_BOOT_PROBE ||
          infop->awi_op_type == ACPIDEV_OP_BOOT_REPROBE ||
          infop->awi_op_type == ACPIDEV_OP_HOTPLUG_PROBE) {
        res = acpidev_filter_device(infop, infop->awi_hdl,
                                    ACPIDEV_ARRAY_PARAM(acpidev_cpu_filters),
                                    devname, maxlen);
      } else {
        res = ACPIDEV_FILTER_FAILED;
      }

      return (res);
    }

    static ACPI_STATUS acpidev_cpu_init(acpidev_walk_info_t * infop) {
      int count;
      uint32_t pxmid;
      dev_info_t *dip;
      ACPI_HANDLE hdl;
      char unitaddr[64];
      char **compatpp;
      static char *compatible[] = {ACPIDEV_HID_PROCESSOR, ACPIDEV_TYPE_CPU,
                                   "cpu"};

      ASSERT(infop != NULL);
      dip = infop->awi_dip;
      hdl = infop->awi_hdl;

      /* Create "apic_id", "processor_id" and "proximity_id" properties. */
      if (ndi_prop_update_int(DDI_DEV_T_NONE, dip,
                              ACPIDEV_PROP_NAME_PROCESSOR_ID,
                              infop->awi_scratchpad[0]) != NDI_SUCCESS) {
        cmn_err(CE_WARN,
                "!acpidev: failed to set processor_id property for %s.",
                infop->awi_name);
        return (AE_ERROR);
      }
      if (ndi_prop_update_int(DDI_DEV_T_NONE, dip,
                              ACPIDEV_PROP_NAME_LOCALAPIC_ID,
                              infop->awi_scratchpad[1]) != NDI_SUCCESS) {
        cmn_err(CE_WARN, "!acpidev: failed to set apic_id property for %s.",
                infop->awi_name);
        return (AE_ERROR);
      }
      if (ACPI_SUCCESS(acpidev_cpu_get_proximity_id(
              infop->awi_hdl, infop->awi_scratchpad[1], &pxmid))) {
        if (ndi_prop_update_int(DDI_DEV_T_NONE, dip,
                                ACPIDEV_PROP_NAME_PROXIMITY_ID,
                                pxmid) != NDI_SUCCESS) {
          cmn_err(CE_WARN,
                  "!acpidev: failed to set proximity id "
                  "property for %s.",
                  infop->awi_name);
          return (AE_ERROR);
        }
      }

      /* Set "compatible" property for CPU dip */
      count = sizeof(compatible) / sizeof(compatible[0]);
      if (infop->awi_info->Type == ACPI_TYPE_PROCESSOR) {
        compatpp = compatible;
      } else if (infop->awi_info->Type == ACPI_TYPE_DEVICE) {
        /*
         * skip first item for pseudo processor HID.
         * acpidev_set_compatible() will handle HID/CID for CPU device.
         */
        compatpp = &compatible[1];
        count--;
      } else {
        return (AE_BAD_PARAMETER);
      }
      if (ACPI_FAILURE(acpidev_set_compatible(infop, compatpp, count))) {
        return (AE_ERROR);
      }

      /*
       * Set device unit-address property.
       * First try to generate meaningful unit address from _UID,
       * then use Processor Id if that fails.
       */
      if ((infop->awi_info->Valid & ACPI_VALID_UID) == 0 ||
          acpidev_generate_unitaddr(
              infop->awi_info->UniqueId.String,
              ACPIDEV_ARRAY_PARAM(acpidev_cpu_uid_formats), unitaddr,
              sizeof(unitaddr)) == NULL) {
        (void)snprintf(unitaddr, sizeof(unitaddr), "%u",
                       (uint32_t)infop->awi_scratchpad[0]);
      }
      if (ACPI_FAILURE(acpidev_set_unitaddr(infop, NULL, 0, unitaddr))) {
        return (AE_ERROR);
      }

      /*
       * Build binding information for CPUs.
       */
      if (infop->awi_op_type == ACPIDEV_OP_BOOT_PROBE ||
          infop->awi_op_type == ACPIDEV_OP_BOOT_REPROBE ||
          infop->awi_op_type == ACPIDEV_OP_HOTPLUG_PROBE) {
        if (ACPI_FAILURE(acpica_add_processor_to_map(
                infop->awi_scratchpad[0], hdl, infop->awi_scratchpad[1]))) {
          cmn_err(CE_WARN,
                  "!acpidev: failed to bind processor "
                  "id/object handle for %s.",
                  infop->awi_name);
          return (AE_ERROR);
        }
      } else {
        ACPIDEV_DEBUG(
            CE_WARN, "!acpidev: unknown operation type %u in acpidev_cpu_init.",
            infop->awi_op_type);
        return (AE_BAD_PARAMETER);
      }

      return (AE_OK);
    }

    static void acpidev_cpu_fini(ACPI_HANDLE hdl, acpidev_data_handle_t dhdl,
                                 acpidev_class_t * clsp) {
      _NOTE(ARGUNUSED(clsp, dhdl));

      int rc;
      uint32_t procid;

      rc = acpica_get_procid_by_object(hdl, &procid);
      ASSERT(ACPI_SUCCESS(rc));
      if (ACPI_SUCCESS(rc)) {
        rc = acpica_remove_processor_from_map(procid);
        ASSERT(ACPI_SUCCESS(rc));
        if (ACPI_FAILURE(rc)) {
          cmn_err(CE_WARN, "!acpidev: failed to remove "
                           "processor from ACPICA.");
        }
      }
    }

    /*
     * Lookup the dip for a CPU if ACPI CPU autoconfig is enabled.
     */
    static int acpidev_cpu_lookup_dip(cpu_t * cp, dev_info_t * *dipp) {
      uint32_t apicid;
      ACPI_HANDLE hdl;
      dev_info_t *dip = NULL;

      *dipp = NULL;
      if (acpica_get_devcfg_feature(ACPI_DEVCFG_CPU)) {
        apicid = cpuid_get_apicid(cp);
        if (acpica_get_cpu_object_by_cpuid(cp->cpu_id, &hdl) == 0 ||
            (apicid != UINT32_MAX &&
             acpica_get_cpu_object_by_apicid(apicid, &hdl) == 0)) {
          ASSERT(hdl != NULL);
          if (ACPI_SUCCESS(acpica_get_devinfo(hdl, &dip))) {
            ASSERT(dip != NULL);
            *dipp = dip;
            return (PSM_SUCCESS);
          }
        }
        ACPIDEV_DEBUG(CE_WARN, "!acpidev: failed to lookup dip for cpu %d(%p).",
                      cp->cpu_id, (void *)cp);
      }

      return (PSM_FAILURE);
    }

    static int acpidev_cpu_create_dip(cpu_t * cp, dev_info_t * *dipp) {
      if (acpidev_cpu_lookup_dip(cp, dipp) == PSM_SUCCESS) {
        ndi_hold_devi(*dipp);
        return (PSM_SUCCESS);
      }
      if (psm_cpu_create_devinfo_old != NULL) {
        return (psm_cpu_create_devinfo_old(cp, dipp));
      } else {
        return (PSM_FAILURE);
      }
    }

    static int acpidev_cpu_get_dip(cpu_t * cp, dev_info_t * *dipp) {
      if (acpidev_cpu_lookup_dip(cp, dipp) == PSM_SUCCESS) {
        return (PSM_SUCCESS);
      }
      if (psm_cpu_get_devinfo_old != NULL) {
        return (psm_cpu_get_devinfo_old(cp, dipp));
      } else {
        return (PSM_FAILURE);
      }
    }

    /**
     * @section minix_drivers_power_acpi_dispatcher_dscontrol_c
     * @brief Original file: minix/drivers/power/acpi/dispatcher/dscontrol.c
     */
    /******************************************************************************
     *
     * Module Name: dscontrol - Support for execution control opcodes -
     *                          if/else/while/return
     *
     *****************************************************************************/

    /*
     * Copyright (C) 2000 - 2014, Intel Corp.
     * All rights reserved.
     *
     * Redistribution and use in source and binary forms, with or without
     * modification, are permitted provided that the following conditions
     * are met:
     * 1. Redistributions of source code must retain the above copyright
     *    notice, this list of conditions, and the following disclaimer,
     *    without modification.
     * 2. Redistributions in binary form must reproduce at minimum a disclaimer
     *    substantially similar to the "NO WARRANTY" disclaimer below
     *    ("Disclaimer") and any redistribution must be conditioned upon
     *    including a substantially similar Disclaimer requirement for further
     *    binary redistribution.
     * 3. Neither the names of the above-listed copyright holders nor the names
     *    of any contributors may be used to endorse or promote products derived
     *    from this software without specific prior written permission.
     *
     * Alternatively, this software may be distributed under the terms of the
     * GNU General Public License ("GPL") version 2 as published by the Free
     * Software Foundation.
     *
     * NO WARRANTY
     * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
     * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
     * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR
     * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
     * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR
     * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
     * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
     * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
     * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
     * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
     * THE POSSIBILITY OF SUCH DAMAGES.
     */

#include "accommon.h"
#include "acdispat.h"
#include "acinterp.h"
#include "acpi.h"
#include "amlcode.h"

#define _COMPONENT ACPI_DISPATCHER
    ACPI_MODULE_NAME("dscontrol")

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsExecBeginControlOp
     *
     * PARAMETERS:  WalkList        - The list that owns the walk stack
     *              Op              - The control Op
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Handles all control ops encountered during control method
     *              execution.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsExecBeginControlOp(ACPI_WALK_STATE * WalkState,
                             ACPI_PARSE_OBJECT * Op) {
      ACPI_STATUS Status = AE_OK;
      ACPI_GENERIC_STATE *ControlState;

      ACPI_FUNCTION_NAME(DsExecBeginControlOp);

      ACPI_DEBUG_PRINT((ACPI_DB_DISPATCH, "Op=%p Opcode=%2.2X State=%p\n", Op,
                        Op->Common.AmlOpcode, WalkState));

      switch (Op->Common.AmlOpcode) {
      case AML_WHILE_OP:
        /*
         * If this is an additional iteration of a while loop, continue.
         * There is no need to allocate a new control state.
         */
        if (WalkState->ControlState) {
          if (WalkState->ControlState->Control.AmlPredicateStart ==
              (WalkState->ParserState.Aml - 1)) {
            /* Reset the state to start-of-loop */

            WalkState->ControlState->Common.State =
                ACPI_CONTROL_CONDITIONAL_EXECUTING;
            break;
          }
        }

        /*lint -fallthrough */

      case AML_IF_OP:
        /*
         * IF/WHILE: Create a new control state to manage these
         * constructs. We need to manage these as a stack, in order
         * to handle nesting.
         */
        ControlState = AcpiUtCreateControlState();
        if (!ControlState) {
          Status = AE_NO_MEMORY;
          break;
        }
        /*
         * Save a pointer to the predicate for multiple executions
         * of a loop
         */
        ControlState->Control.AmlPredicateStart =
            WalkState->ParserState.Aml - 1;
        ControlState->Control.PackageEnd = WalkState->ParserState.PkgEnd;
        ControlState->Control.Opcode = Op->Common.AmlOpcode;

        /* Push the control state on this walk's control stack */

        AcpiUtPushGenericState(&WalkState->ControlState, ControlState);
        break;

      case AML_ELSE_OP:

        /* Predicate is in the state object */
        /* If predicate is true, the IF was executed, ignore ELSE part */

        if (WalkState->LastPredicate) {
          Status = AE_CTRL_TRUE;
        }

        break;

      case AML_RETURN_OP:

        break;

      default:

        break;
      }

      return (Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsExecEndControlOp
     *
     * PARAMETERS:  WalkList        - The list that owns the walk stack
     *              Op              - The control Op
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Handles all control ops encountered during control method
     *              execution.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsExecEndControlOp(ACPI_WALK_STATE * WalkState,
                           ACPI_PARSE_OBJECT * Op) {
      ACPI_STATUS Status = AE_OK;
      ACPI_GENERIC_STATE *ControlState;

      ACPI_FUNCTION_NAME(DsExecEndControlOp);

      switch (Op->Common.AmlOpcode) {
      case AML_IF_OP:

        ACPI_DEBUG_PRINT((ACPI_DB_DISPATCH, "[IF_OP] Op=%p\n", Op));

        /*
         * Save the result of the predicate in case there is an
         * ELSE to come
         */
        WalkState->LastPredicate =
            (BOOLEAN)WalkState->ControlState->Common.Value;

        /*
         * Pop the control state that was created at the start
         * of the IF and free it
         */
        ControlState = AcpiUtPopGenericState(&WalkState->ControlState);
        AcpiUtDeleteGenericState(ControlState);
        break;

      case AML_ELSE_OP:

        break;

      case AML_WHILE_OP:

        ACPI_DEBUG_PRINT((ACPI_DB_DISPATCH, "[WHILE_OP] Op=%p\n", Op));

        ControlState = WalkState->ControlState;
        if (ControlState->Common.Value) {
          /* Predicate was true, the body of the loop was just executed */

          /*
           * This loop counter mechanism allows the interpreter to escape
           * possibly infinite loops. This can occur in poorly written AML
           * when the hardware does not respond within a while loop and the
           * loop does not implement a timeout.
           */
          ControlState->Control.LoopCount++;
          if (ControlState->Control.LoopCount > ACPI_MAX_LOOP_ITERATIONS) {
            Status = AE_AML_INFINITE_LOOP;
            break;
          }

          /*
           * Go back and evaluate the predicate and maybe execute the loop
           * another time
           */
          Status = AE_CTRL_PENDING;
          WalkState->AmlLastWhile = ControlState->Control.AmlPredicateStart;
          break;
        }

        /* Predicate was false, terminate this while loop */

        ACPI_DEBUG_PRINT(
            (ACPI_DB_DISPATCH, "[WHILE_OP] termination! Op=%p\n", Op));

        /* Pop this control state and free it */

        ControlState = AcpiUtPopGenericState(&WalkState->ControlState);
        AcpiUtDeleteGenericState(ControlState);
        break;

      case AML_RETURN_OP:

        ACPI_DEBUG_PRINT((ACPI_DB_DISPATCH, "[RETURN_OP] Op=%p Arg=%p\n", Op,
                          Op->Common.Value.Arg));

        /*
         * One optional operand -- the return value
         * It can be either an immediate operand or a result that
         * has been bubbled up the tree
         */
        if (Op->Common.Value.Arg) {
          /* Since we have a real Return(), delete any implicit return */

          AcpiDsClearImplicitReturn(WalkState);

          /* Return statement has an immediate operand */

          Status = AcpiDsCreateOperands(WalkState, Op->Common.Value.Arg);
          if (ACPI_FAILURE(Status)) {
            return (Status);
          }

          /*
           * If value being returned is a Reference (such as
           * an arg or local), resolve it now because it may
           * cease to exist at the end of the method.
           */
          Status = AcpiExResolveToValue(&WalkState->Operands[0], WalkState);
          if (ACPI_FAILURE(Status)) {
            return (Status);
          }

          /*
           * Get the return value and save as the last result
           * value. This is the only place where WalkState->ReturnDesc
           * is set to anything other than zero!
           */
          WalkState->ReturnDesc = WalkState->Operands[0];
        } else if (WalkState->ResultCount) {
          /* Since we have a real Return(), delete any implicit return */

          AcpiDsClearImplicitReturn(WalkState);

          /*
           * The return value has come from a previous calculation.
           *
           * If value being returned is a Reference (such as
           * an arg or local), resolve it now because it may
           * cease to exist at the end of the method.
           *
           * Allow references created by the Index operator to return
           * unchanged.
           */
          if ((ACPI_GET_DESCRIPTOR_TYPE(
                   WalkState->Results->Results.ObjDesc[0]) ==
               ACPI_DESC_TYPE_OPERAND) &&
              ((WalkState->Results->Results.ObjDesc[0])->Common.Type ==
               ACPI_TYPE_LOCAL_REFERENCE) &&
              ((WalkState->Results->Results.ObjDesc[0])->Reference.Class !=
               ACPI_REFCLASS_INDEX)) {
            Status = AcpiExResolveToValue(
                &WalkState->Results->Results.ObjDesc[0], WalkState);
            if (ACPI_FAILURE(Status)) {
              return (Status);
            }
          }

          WalkState->ReturnDesc = WalkState->Results->Results.ObjDesc[0];
        } else {
          /* No return operand */

          if (WalkState->NumOperands) {
            AcpiUtRemoveReference(WalkState->Operands[0]);
          }

          WalkState->Operands[0] = NULL;
          WalkState->NumOperands = 0;
          WalkState->ReturnDesc = NULL;
        }

        ACPI_DEBUG_PRINT((ACPI_DB_DISPATCH,
                          "Completed RETURN_OP State=%p, RetVal=%p\n",
                          WalkState, WalkState->ReturnDesc));

        /* End the control method execution right now */

        Status = AE_CTRL_TERMINATE;
        break;

      case AML_NOOP_OP:

        /* Just do nothing! */

        break;

      case AML_BREAK_POINT_OP:

        /*
         * Set the single-step flag. This will cause the debugger (if present)
         * to break to the console within the AML debugger at the start of the
         * next AML instruction.
         */
        ACPI_DEBUGGER_EXEC(AcpiGbl_CmSingleStep = TRUE);
        ACPI_DEBUGGER_EXEC(
            AcpiOsPrintf("**break** Executed AML BreakPoint opcode\n"));

        /* Call to the OSL in case OS wants a piece of the action */

        Status = AcpiOsSignal(ACPI_SIGNAL_BREAKPOINT,
                              "Executed AML Breakpoint opcode");
        break;

      case AML_BREAK_OP:
      case AML_CONTINUE_OP: /* ACPI 2.0 */

        /* Pop and delete control states until we find a while */

        while (WalkState->ControlState &&
               (WalkState->ControlState->Control.Opcode != AML_WHILE_OP)) {
          ControlState = AcpiUtPopGenericState(&WalkState->ControlState);
          AcpiUtDeleteGenericState(ControlState);
        }

        /* No while found? */

        if (!WalkState->ControlState) {
          return (AE_AML_NO_WHILE);
        }

        /* Was: WalkState->AmlLastWhile =
         * WalkState->ControlState->Control.AmlPredicateStart; */

        WalkState->AmlLastWhile = WalkState->ControlState->Control.PackageEnd;

        /* Return status depending on opcode */

        if (Op->Common.AmlOpcode == AML_BREAK_OP) {
          Status = AE_CTRL_BREAK;
        } else {
          Status = AE_CTRL_CONTINUE;
        }
        break;

      default:

        ACPI_ERROR((AE_INFO, "Unknown control opcode=0x%X Op=%p",
                    Op->Common.AmlOpcode, Op));

        Status = AE_AML_BAD_OPCODE;
        break;
      }

      return (Status);
    }

    /**
     * @section minix_drivers_power_acpi_dispatcher_dsfield_c
     * @brief Original file: minix/drivers/power/acpi/dispatcher/dsfield.c
     */
    /******************************************************************************
     *
     * Module Name: dsfield - Dispatcher field routines
     *
     *****************************************************************************/

    /*
     * Copyright (C) 2000 - 2014, Intel Corp.
     * All rights reserved.
     *
     * Redistribution and use in source and binary forms, with or without
     * modification, are permitted provided that the following conditions
     * are met:
     * 1. Redistributions of source code must retain the above copyright
     *    notice, this list of conditions, and the following disclaimer,
     *    without modification.
     * 2. Redistributions in binary form must reproduce at minimum a disclaimer
     *    substantially similar to the "NO WARRANTY" disclaimer below
     *    ("Disclaimer") and any redistribution must be conditioned upon
     *    including a substantially similar Disclaimer requirement for further
     *    binary redistribution.
     * 3. Neither the names of the above-listed copyright holders nor the names
     *    of any contributors may be used to endorse or promote products derived
     *    from this software without specific prior written permission.
     *
     * Alternatively, this software may be distributed under the terms of the
     * GNU General Public License ("GPL") version 2 as published by the Free
     * Software Foundation.
     *
     * NO WARRANTY
     * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
     * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
     * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR
     * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
     * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR
     * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
     * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
     * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
     * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
     * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
     * THE POSSIBILITY OF SUCH DAMAGES.
     */

#include "accommon.h"
#include "acdispat.h"
#include "acinterp.h"
#include "acnamesp.h"
#include "acparser.h"
#include "acpi.h"
#include "amlcode.h"

#define _COMPONENT ACPI_DISPATCHER
    ACPI_MODULE_NAME("dsfield")

    /* Local prototypes */

#ifdef ACPI_ASL_COMPILER
#include "acdisasm.h"

    static ACPI_STATUS AcpiDsCreateExternalRegion(
        ACPI_STATUS LookupStatus, ACPI_PARSE_OBJECT * Op, char *Path,
        ACPI_WALK_STATE *WalkState, ACPI_NAMESPACE_NODE **Node);
#endif

    static ACPI_STATUS AcpiDsGetFieldNames(ACPI_CREATE_FIELD_INFO * Info,
                                           ACPI_WALK_STATE * WalkState,
                                           ACPI_PARSE_OBJECT * Arg);

#ifdef ACPI_ASL_COMPILER
    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsCreateExternalRegion (iASL Disassembler only)
     *
     * PARAMETERS:  LookupStatus    - Status from NsLookup operation
     *              Op              - Op containing the Field definition and
     *args Path            - Pathname of the region `           WalkState -
     *Current method state Node            - Where the new region node is
     *returned
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Add region to the external list if NOT_FOUND. Create a new
     *              region node/object.
     *
     ******************************************************************************/

    static ACPI_STATUS AcpiDsCreateExternalRegion(
        ACPI_STATUS LookupStatus, ACPI_PARSE_OBJECT * Op, char *Path,
        ACPI_WALK_STATE *WalkState, ACPI_NAMESPACE_NODE **Node) {
      ACPI_STATUS Status;
      ACPI_OPERAND_OBJECT *ObjDesc;

      if (LookupStatus != AE_NOT_FOUND) {
        return (LookupStatus);
      }

      /*
       * Table disassembly:
       * OperationRegion not found. Generate an External for it, and
       * insert the name into the namespace.
       */
      AcpiDmAddOpToExternalList(Op, Path, ACPI_TYPE_REGION, 0, 0);
      Status = AcpiNsLookup(WalkState->ScopeInfo, Path, ACPI_TYPE_REGION,
                            ACPI_IMODE_LOAD_PASS1, ACPI_NS_SEARCH_PARENT,
                            WalkState, Node);
      if (ACPI_FAILURE(Status)) {
        return (Status);
      }

      /* Must create and install a region object for the new node */

      ObjDesc = AcpiUtCreateInternalObject(ACPI_TYPE_REGION);
      if (!ObjDesc) {
        return (AE_NO_MEMORY);
      }

      ObjDesc->Region.Node = *Node;
      Status = AcpiNsAttachObject(*Node, ObjDesc, ACPI_TYPE_REGION);
      return (Status);
    }
#endif

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsCreateBufferField
     *
     * PARAMETERS:  Op                  - Current parse op (CreateXXField)
     *              WalkState           - Current state
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Execute the CreateField operators:
     *              CreateBitFieldOp,
     *              CreateByteFieldOp,
     *              CreateWordFieldOp,
     *              CreateDwordFieldOp,
     *              CreateQwordFieldOp,
     *              CreateFieldOp       (all of which define a field in a
     *buffer)
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsCreateBufferField(ACPI_PARSE_OBJECT * Op,
                            ACPI_WALK_STATE * WalkState) {
      ACPI_PARSE_OBJECT *Arg;
      ACPI_NAMESPACE_NODE *Node;
      ACPI_STATUS Status;
      ACPI_OPERAND_OBJECT *ObjDesc;
      ACPI_OPERAND_OBJECT *SecondDesc = NULL;
      UINT32 Flags;

      ACPI_FUNCTION_TRACE(DsCreateBufferField);

      /*
       * Get the NameString argument (name of the new BufferField)
       */
      if (Op->Common.AmlOpcode == AML_CREATE_FIELD_OP) {
        /* For CreateField, name is the 4th argument */

        Arg = AcpiPsGetArg(Op, 3);
      } else {
        /* For all other CreateXXXField operators, name is the 3rd argument */

        Arg = AcpiPsGetArg(Op, 2);
      }

      if (!Arg) {
        return_ACPI_STATUS(AE_AML_NO_OPERAND);
      }

      if (WalkState->DeferredNode) {
        Node = WalkState->DeferredNode;
        Status = AE_OK;
      } else {
        /* Execute flag should always be set when this function is entered */

        if (!(WalkState->ParseFlags & ACPI_PARSE_EXECUTE)) {
          return_ACPI_STATUS(AE_AML_INTERNAL);
        }

        /* Creating new namespace node, should not already exist */

        Flags = ACPI_NS_NO_UPSEARCH | ACPI_NS_DONT_OPEN_SCOPE |
                ACPI_NS_ERROR_IF_FOUND;

        /*
         * Mark node temporary if we are executing a normal control
         * method. (Don't mark if this is a module-level code method)
         */
        if (WalkState->MethodNode &&
            !(WalkState->ParseFlags & ACPI_PARSE_MODULE_LEVEL)) {
          Flags |= ACPI_NS_TEMPORARY;
        }

        /* Enter the NameString into the namespace */

        Status = AcpiNsLookup(WalkState->ScopeInfo, Arg->Common.Value.String,
                              ACPI_TYPE_ANY, ACPI_IMODE_LOAD_PASS1, Flags,
                              WalkState, &Node);
        if (ACPI_FAILURE(Status)) {
          ACPI_ERROR_NAMESPACE(Arg->Common.Value.String, Status);
          return_ACPI_STATUS(Status);
        }
      }

      /*
       * We could put the returned object (Node) on the object stack for later,
       * but for now, we will put it in the "op" object that the parser uses,
       * so we can get it again at the end of this scope.
       */
      Op->Common.Node = Node;

      /*
       * If there is no object attached to the node, this node was just created
       * and we need to create the field object. Otherwise, this was a lookup
       * of an existing node and we don't want to create the field object again.
       */
      ObjDesc = AcpiNsGetAttachedObject(Node);
      if (ObjDesc) {
        return_ACPI_STATUS(AE_OK);
      }

      /*
       * The Field definition is not fully parsed at this time.
       * (We must save the address of the AML for the buffer and index operands)
       */

      /* Create the buffer field object */

      ObjDesc = AcpiUtCreateInternalObject(ACPI_TYPE_BUFFER_FIELD);
      if (!ObjDesc) {
        Status = AE_NO_MEMORY;
        goto Cleanup;
      }

      /*
       * Remember location in AML stream of the field unit opcode and operands
       * -- since the buffer and index operands must be evaluated.
       */
      SecondDesc = ObjDesc->Common.NextObject;
      SecondDesc->Extra.AmlStart = Op->Named.Data;
      SecondDesc->Extra.AmlLength = Op->Named.Length;
      ObjDesc->BufferField.Node = Node;

      /* Attach constructed field descriptors to parent node */

      Status = AcpiNsAttachObject(Node, ObjDesc, ACPI_TYPE_BUFFER_FIELD);
      if (ACPI_FAILURE(Status)) {
        goto Cleanup;
      }

    Cleanup:

      /* Remove local reference to the object */

      AcpiUtRemoveReference(ObjDesc);
      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsGetFieldNames
     *
     * PARAMETERS:  Info            - CreateField info structure
     *  `           WalkState       - Current method state
     *              Arg             - First parser arg for the field name list
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Process all named fields in a field declaration. Names are
     *              entered into the namespace.
     *
     ******************************************************************************/

    static ACPI_STATUS AcpiDsGetFieldNames(ACPI_CREATE_FIELD_INFO * Info,
                                           ACPI_WALK_STATE * WalkState,
                                           ACPI_PARSE_OBJECT * Arg) {
      ACPI_STATUS Status;
      UINT64 Position;
      ACPI_PARSE_OBJECT *Child;

      ACPI_FUNCTION_TRACE_PTR(DsGetFieldNames, Info);

      /* First field starts at bit zero */

      Info->FieldBitPosition = 0;

      /* Process all elements in the field list (of parse nodes) */

      while (Arg) {
        /*
         * Four types of field elements are handled:
         * 1) Name - Enters a new named field into the namespace
         * 2) Offset - specifies a bit offset
         * 3) AccessAs - changes the access mode/attributes
         * 4) Connection - Associate a resource template with the field
         */
        switch (Arg->Common.AmlOpcode) {
        case AML_INT_RESERVEDFIELD_OP:

          Position =
              (UINT64)Info->FieldBitPosition + (UINT64)Arg->Common.Value.Size;

          if (Position > ACPI_UINT32_MAX) {
            ACPI_ERROR(
                (AE_INFO, "Bit offset within field too large (> 0xFFFFFFFF)"));
            return_ACPI_STATUS(AE_SUPPORT);
          }

          Info->FieldBitPosition = (UINT32)Position;
          break;

        case AML_INT_ACCESSFIELD_OP:
        case AML_INT_EXTACCESSFIELD_OP:
          /*
           * Get new AccessType, AccessAttribute, and AccessLength fields
           * -- to be used for all field units that follow, until the
           * end-of-field or another AccessAs keyword is encountered.
           * NOTE. These three bytes are encoded in the integer value
           * of the parseop for convenience.
           *
           * In FieldFlags, preserve the flag bits other than the
           * ACCESS_TYPE bits.
           */

          /* AccessType (ByteAcc, WordAcc, etc.) */

          Info->FieldFlags =
              (UINT8)((Info->FieldFlags & ~(AML_FIELD_ACCESS_TYPE_MASK)) |
                      ((UINT8)((UINT32)(Arg->Common.Value.Integer & 0x07))));

          /* AccessAttribute (AttribQuick, AttribByte, etc.) */

          Info->Attribute = (UINT8)((Arg->Common.Value.Integer >> 8) & 0xFF);

          /* AccessLength (for serial/buffer protocols) */

          Info->AccessLength =
              (UINT8)((Arg->Common.Value.Integer >> 16) & 0xFF);
          break;

        case AML_INT_CONNECTION_OP:
          /*
           * Clear any previous connection. New connection is used for all
           * fields that follow, similar to AccessAs
           */
          Info->ResourceBuffer = NULL;
          Info->ConnectionNode = NULL;
          Info->PinNumberIndex = 0;

          /*
           * A Connection() is either an actual resource descriptor (buffer)
           * or a named reference to a resource template
           */
          Child = Arg->Common.Value.Arg;
          if (Child->Common.AmlOpcode == AML_INT_BYTELIST_OP) {
            Info->ResourceBuffer = Child->Named.Data;
            Info->ResourceLength = (UINT16)Child->Named.Value.Integer;
          } else {
            /* Lookup the Connection() namepath, it should already exist */

            Status = AcpiNsLookup(WalkState->ScopeInfo,
                                  Child->Common.Value.Name, ACPI_TYPE_ANY,
                                  ACPI_IMODE_EXECUTE, ACPI_NS_DONT_OPEN_SCOPE,
                                  WalkState, &Info->ConnectionNode);
            if (ACPI_FAILURE(Status)) {
              ACPI_ERROR_NAMESPACE(Child->Common.Value.Name, Status);
              return_ACPI_STATUS(Status);
            }
          }
          break;

        case AML_INT_NAMEDFIELD_OP:

          /* Lookup the name, it should already exist */

          Status = AcpiNsLookup(WalkState->ScopeInfo, (char *)&Arg->Named.Name,
                                Info->FieldType, ACPI_IMODE_EXECUTE,
                                ACPI_NS_DONT_OPEN_SCOPE, WalkState,
                                &Info->FieldNode);
          if (ACPI_FAILURE(Status)) {
            ACPI_ERROR_NAMESPACE((char *)&Arg->Named.Name, Status);
            return_ACPI_STATUS(Status);
          } else {
            Arg->Common.Node = Info->FieldNode;
            Info->FieldBitLength = Arg->Common.Value.Size;

            /*
             * If there is no object attached to the node, this node was
             * just created and we need to create the field object.
             * Otherwise, this was a lookup of an existing node and we
             * don't want to create the field object again.
             */
            if (!AcpiNsGetAttachedObject(Info->FieldNode)) {
              Status = AcpiExPrepFieldValue(Info);
              if (ACPI_FAILURE(Status)) {
                return_ACPI_STATUS(Status);
              }
            }
          }

          /* Keep track of bit position for the next field */

          Position =
              (UINT64)Info->FieldBitPosition + (UINT64)Arg->Common.Value.Size;

          if (Position > ACPI_UINT32_MAX) {
            ACPI_ERROR((AE_INFO,
                        "Field [%4.4s] bit offset too large (> 0xFFFFFFFF)",
                        ACPI_CAST_PTR(char, &Info->FieldNode->Name)));
            return_ACPI_STATUS(AE_SUPPORT);
          }

          Info->FieldBitPosition += Info->FieldBitLength;
          Info->PinNumberIndex++; /* Index relative to previous Connection() */
          break;

        default:

          ACPI_ERROR((AE_INFO, "Invalid opcode in field list: 0x%X",
                      Arg->Common.AmlOpcode));
          return_ACPI_STATUS(AE_AML_BAD_OPCODE);
        }

        Arg = Arg->Common.Next;
      }

      return_ACPI_STATUS(AE_OK);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsCreateField
     *
     * PARAMETERS:  Op              - Op containing the Field definition and
     *args RegionNode      - Object for the containing Operation Region `
     *WalkState       - Current method state
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Create a new field in the specified operation region
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsCreateField(ACPI_PARSE_OBJECT * Op, ACPI_NAMESPACE_NODE * RegionNode,
                      ACPI_WALK_STATE * WalkState) {
      ACPI_STATUS Status;
      ACPI_PARSE_OBJECT *Arg;
      ACPI_CREATE_FIELD_INFO Info;

      ACPI_FUNCTION_TRACE_PTR(DsCreateField, Op);

      /* First arg is the name of the parent OpRegion (must already exist) */

      Arg = Op->Common.Value.Arg;

      if (!RegionNode) {
        Status = AcpiNsLookup(WalkState->ScopeInfo, Arg->Common.Value.Name,
                              ACPI_TYPE_REGION, ACPI_IMODE_EXECUTE,
                              ACPI_NS_SEARCH_PARENT, WalkState, &RegionNode);
#ifdef ACPI_ASL_COMPILER
        Status = AcpiDsCreateExternalRegion(Status, Arg, Arg->Common.Value.Name,
                                            WalkState, &RegionNode);
#endif
        if (ACPI_FAILURE(Status)) {
          ACPI_ERROR_NAMESPACE(Arg->Common.Value.Name, Status);
          return_ACPI_STATUS(Status);
        }
      }

      ACPI_MEMSET(&Info, 0, sizeof(ACPI_CREATE_FIELD_INFO));

      /* Second arg is the field flags */

      Arg = Arg->Common.Next;
      Info.FieldFlags = (UINT8)Arg->Common.Value.Integer;
      Info.Attribute = 0;

      /* Each remaining arg is a Named Field */

      Info.FieldType = ACPI_TYPE_LOCAL_REGION_FIELD;
      Info.RegionNode = RegionNode;

      Status = AcpiDsGetFieldNames(&Info, WalkState, Arg->Common.Next);
      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsInitFieldObjects
     *
     * PARAMETERS:  Op              - Op containing the Field definition and
     *args `           WalkState       - Current method state
     *
     * RETURN:      Status
     *
     * DESCRIPTION: For each "Field Unit" name in the argument list that is
     *              part of the field declaration, enter the name into the
     *              namespace.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsInitFieldObjects(ACPI_PARSE_OBJECT * Op,
                           ACPI_WALK_STATE * WalkState) {
      ACPI_STATUS Status;
      ACPI_PARSE_OBJECT *Arg = NULL;
      ACPI_NAMESPACE_NODE *Node;
      UINT8 Type = 0;
      UINT32 Flags;

      ACPI_FUNCTION_TRACE_PTR(DsInitFieldObjects, Op);

      /* Execute flag should always be set when this function is entered */

      if (!(WalkState->ParseFlags & ACPI_PARSE_EXECUTE)) {
        if (WalkState->ParseFlags & ACPI_PARSE_DEFERRED_OP) {
          /* BankField Op is deferred, just return OK */

          return_ACPI_STATUS(AE_OK);
        }

        return_ACPI_STATUS(AE_AML_INTERNAL);
      }

      /*
       * Get the FieldList argument for this opcode. This is the start of the
       * list of field elements.
       */
      switch (WalkState->Opcode) {
      case AML_FIELD_OP:

        Arg = AcpiPsGetArg(Op, 2);
        Type = ACPI_TYPE_LOCAL_REGION_FIELD;
        break;

      case AML_BANK_FIELD_OP:

        Arg = AcpiPsGetArg(Op, 4);
        Type = ACPI_TYPE_LOCAL_BANK_FIELD;
        break;

      case AML_INDEX_FIELD_OP:

        Arg = AcpiPsGetArg(Op, 3);
        Type = ACPI_TYPE_LOCAL_INDEX_FIELD;
        break;

      default:

        return_ACPI_STATUS(AE_BAD_PARAMETER);
      }

      /* Creating new namespace node(s), should not already exist */

      Flags = ACPI_NS_NO_UPSEARCH | ACPI_NS_DONT_OPEN_SCOPE |
              ACPI_NS_ERROR_IF_FOUND;

      /*
       * Mark node(s) temporary if we are executing a normal control
       * method. (Don't mark if this is a module-level code method)
       */
      if (WalkState->MethodNode &&
          !(WalkState->ParseFlags & ACPI_PARSE_MODULE_LEVEL)) {
        Flags |= ACPI_NS_TEMPORARY;
      }

      /*
       * Walk the list of entries in the FieldList
       * Note: FieldList can be of zero length. In this case, Arg will be NULL.
       */
      while (Arg) {
        /*
         * Ignore OFFSET/ACCESSAS/CONNECTION terms here; we are only interested
         * in the field names in order to enter them into the namespace.
         */
        if (Arg->Common.AmlOpcode == AML_INT_NAMEDFIELD_OP) {
          Status =
              AcpiNsLookup(WalkState->ScopeInfo, (char *)&Arg->Named.Name, Type,
                           ACPI_IMODE_LOAD_PASS1, Flags, WalkState, &Node);
          if (ACPI_FAILURE(Status)) {
            ACPI_ERROR_NAMESPACE((char *)&Arg->Named.Name, Status);
            if (Status != AE_ALREADY_EXISTS) {
              return_ACPI_STATUS(Status);
            }

            /* Name already exists, just ignore this error */

            Status = AE_OK;
          }

          Arg->Common.Node = Node;
        }

        /* Get the next field element in the list */

        Arg = Arg->Common.Next;
      }

      return_ACPI_STATUS(AE_OK);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsCreateBankField
     *
     * PARAMETERS:  Op              - Op containing the Field definition and
     *args RegionNode      - Object for the containing Operation Region
     *              WalkState       - Current method state
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Create a new bank field in the specified operation region
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsCreateBankField(ACPI_PARSE_OBJECT * Op,
                          ACPI_NAMESPACE_NODE * RegionNode,
                          ACPI_WALK_STATE * WalkState) {
      ACPI_STATUS Status;
      ACPI_PARSE_OBJECT *Arg;
      ACPI_CREATE_FIELD_INFO Info;

      ACPI_FUNCTION_TRACE_PTR(DsCreateBankField, Op);

      /* First arg is the name of the parent OpRegion (must already exist) */

      Arg = Op->Common.Value.Arg;
      if (!RegionNode) {
        Status = AcpiNsLookup(WalkState->ScopeInfo, Arg->Common.Value.Name,
                              ACPI_TYPE_REGION, ACPI_IMODE_EXECUTE,
                              ACPI_NS_SEARCH_PARENT, WalkState, &RegionNode);
#ifdef ACPI_ASL_COMPILER
        Status = AcpiDsCreateExternalRegion(Status, Arg, Arg->Common.Value.Name,
                                            WalkState, &RegionNode);
#endif
        if (ACPI_FAILURE(Status)) {
          ACPI_ERROR_NAMESPACE(Arg->Common.Value.Name, Status);
          return_ACPI_STATUS(Status);
        }
      }

      /* Second arg is the Bank Register (Field) (must already exist) */

      Arg = Arg->Common.Next;
      Status =
          AcpiNsLookup(WalkState->ScopeInfo, Arg->Common.Value.String,
                       ACPI_TYPE_ANY, ACPI_IMODE_EXECUTE, ACPI_NS_SEARCH_PARENT,
                       WalkState, &Info.RegisterNode);
      if (ACPI_FAILURE(Status)) {
        ACPI_ERROR_NAMESPACE(Arg->Common.Value.String, Status);
        return_ACPI_STATUS(Status);
      }

      /*
       * Third arg is the BankValue
       * This arg is a TermArg, not a constant
       * It will be evaluated later, by AcpiDsEvalBankFieldOperands
       */
      Arg = Arg->Common.Next;

      /* Fourth arg is the field flags */

      Arg = Arg->Common.Next;
      Info.FieldFlags = (UINT8)Arg->Common.Value.Integer;

      /* Each remaining arg is a Named Field */

      Info.FieldType = ACPI_TYPE_LOCAL_BANK_FIELD;
      Info.RegionNode = RegionNode;

      /*
       * Use Info.DataRegisterNode to store BankField Op
       * It's safe because DataRegisterNode will never be used when create bank
       * field We store AmlStart and AmlLength in the BankField Op for late
       * evaluation Used in AcpiExPrepFieldValue(Info)
       *
       * TBD: Or, should we add a field in ACPI_CREATE_FIELD_INFO, like "void
       * *ParentOp"?
       */
      Info.DataRegisterNode = (ACPI_NAMESPACE_NODE *)Op;

      Status = AcpiDsGetFieldNames(&Info, WalkState, Arg->Common.Next);
      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsCreateIndexField
     *
     * PARAMETERS:  Op              - Op containing the Field definition and
     *args RegionNode      - Object for the containing Operation Region `
     *WalkState       - Current method state
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Create a new index field in the specified operation region
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsCreateIndexField(ACPI_PARSE_OBJECT * Op,
                           ACPI_NAMESPACE_NODE * RegionNode,
                           ACPI_WALK_STATE * WalkState) {
      ACPI_STATUS Status;
      ACPI_PARSE_OBJECT *Arg;
      ACPI_CREATE_FIELD_INFO Info;

      ACPI_FUNCTION_TRACE_PTR(DsCreateIndexField, Op);

      /* First arg is the name of the Index register (must already exist) */

      Arg = Op->Common.Value.Arg;
      Status =
          AcpiNsLookup(WalkState->ScopeInfo, Arg->Common.Value.String,
                       ACPI_TYPE_ANY, ACPI_IMODE_EXECUTE, ACPI_NS_SEARCH_PARENT,
                       WalkState, &Info.RegisterNode);
      if (ACPI_FAILURE(Status)) {
        ACPI_ERROR_NAMESPACE(Arg->Common.Value.String, Status);
        return_ACPI_STATUS(Status);
      }

      /* Second arg is the data register (must already exist) */

      Arg = Arg->Common.Next;
      Status =
          AcpiNsLookup(WalkState->ScopeInfo, Arg->Common.Value.String,
                       ACPI_TYPE_ANY, ACPI_IMODE_EXECUTE, ACPI_NS_SEARCH_PARENT,
                       WalkState, &Info.DataRegisterNode);
      if (ACPI_FAILURE(Status)) {
        ACPI_ERROR_NAMESPACE(Arg->Common.Value.String, Status);
        return_ACPI_STATUS(Status);
      }

      /* Next arg is the field flags */

      Arg = Arg->Common.Next;
      Info.FieldFlags = (UINT8)Arg->Common.Value.Integer;

      /* Each remaining arg is a Named Field */

      Info.FieldType = ACPI_TYPE_LOCAL_INDEX_FIELD;
      Info.RegionNode = RegionNode;

      Status = AcpiDsGetFieldNames(&Info, WalkState, Arg->Common.Next);
      return_ACPI_STATUS(Status);
    }

    /**
     * @section minix_drivers_power_acpi_dispatcher_dsinit_c
     * @brief Original file: minix/drivers/power/acpi/dispatcher/dsinit.c
     */
    /******************************************************************************
     *
     * Module Name: dsinit - Object initialization namespace walk
     *
     *****************************************************************************/

    /*
     * Copyright (C) 2000 - 2014, Intel Corp.
     * All rights reserved.
     *
     * Redistribution and use in source and binary forms, with or without
     * modification, are permitted provided that the following conditions
     * are met:
     * 1. Redistributions of source code must retain the above copyright
     *    notice, this list of conditions, and the following disclaimer,
     *    without modification.
     * 2. Redistributions in binary form must reproduce at minimum a disclaimer
     *    substantially similar to the "NO WARRANTY" disclaimer below
     *    ("Disclaimer") and any redistribution must be conditioned upon
     *    including a substantially similar Disclaimer requirement for further
     *    binary redistribution.
     * 3. Neither the names of the above-listed copyright holders nor the names
     *    of any contributors may be used to endorse or promote products derived
     *    from this software without specific prior written permission.
     *
     * Alternatively, this software may be distributed under the terms of the
     * GNU General Public License ("GPL") version 2 as published by the Free
     * Software Foundation.
     *
     * NO WARRANTY
     * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
     * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
     * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR
     * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
     * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR
     * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
     * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
     * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
     * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
     * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
     * THE POSSIBILITY OF SUCH DAMAGES.
     */

#include "accommon.h"
#include "acdispat.h"
#include "acnamesp.h"
#include "acpi.h"
#include "actables.h"

#define _COMPONENT ACPI_DISPATCHER
    ACPI_MODULE_NAME("dsinit")

    /* Local prototypes */

    static ACPI_STATUS AcpiDsInitOneObject(ACPI_HANDLE ObjHandle, UINT32 Level,
                                           void *Context, void **ReturnValue);

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsInitOneObject
     *
     * PARAMETERS:  ObjHandle       - Node for the object
     *              Level           - Current nesting level
     *              Context         - Points to a init info struct
     *              ReturnValue     - Not used
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Callback from AcpiWalkNamespace. Invoked for every object
     *              within the namespace.
     *
     *              Currently, the only objects that require initialization are:
     *              1) Methods
     *              2) Operation Regions
     *
     ******************************************************************************/

    static ACPI_STATUS AcpiDsInitOneObject(ACPI_HANDLE ObjHandle, UINT32 Level,
                                           void *Context, void **ReturnValue) {
      ACPI_INIT_WALK_INFO *Info = (ACPI_INIT_WALK_INFO *)Context;
      ACPI_NAMESPACE_NODE *Node = (ACPI_NAMESPACE_NODE *)ObjHandle;
      ACPI_STATUS Status;
      ACPI_OPERAND_OBJECT *ObjDesc;

      ACPI_FUNCTION_ENTRY();

      /*
       * We are only interested in NS nodes owned by the table that
       * was just loaded
       */
      if (Node->OwnerId != Info->OwnerId) {
        return (AE_OK);
      }

      Info->ObjectCount++;

      /* And even then, we are only interested in a few object types */

      switch (AcpiNsGetType(ObjHandle)) {
      case ACPI_TYPE_REGION:

        Status = AcpiDsInitializeRegion(ObjHandle);
        if (ACPI_FAILURE(Status)) {
          ACPI_EXCEPTION((AE_INFO, Status,
                          "During Region initialization %p [%4.4s]", ObjHandle,
                          AcpiUtGetNodeName(ObjHandle)));
        }

        Info->OpRegionCount++;
        break;

      case ACPI_TYPE_METHOD:
        /*
         * Auto-serialization support. We will examine each method that is
         * NotSerialized to determine if it creates any Named objects. If
         * it does, it will be marked serialized to prevent problems if
         * the method is entered by two or more threads and an attempt is
         * made to create the same named object twice -- which results in
         * an AE_ALREADY_EXISTS exception and method abort.
         */
        Info->MethodCount++;
        ObjDesc = AcpiNsGetAttachedObject(Node);
        if (!ObjDesc) {
          break;
        }

        /* Ignore if already serialized */

        if (ObjDesc->Method.InfoFlags & ACPI_METHOD_SERIALIZED) {
          Info->SerialMethodCount++;
          break;
        }

        if (AcpiGbl_AutoSerializeMethods) {
          /* Parse/scan method and serialize it if necessary */

          AcpiDsAutoSerializeMethod(Node, ObjDesc);
          if (ObjDesc->Method.InfoFlags & ACPI_METHOD_SERIALIZED) {
            /* Method was just converted to Serialized */

            Info->SerialMethodCount++;
            Info->SerializedMethodCount++;
            break;
          }
        }

        Info->NonSerialMethodCount++;
        break;

      case ACPI_TYPE_DEVICE:

        Info->DeviceCount++;
        break;

      default:

        break;
      }

      /*
       * We ignore errors from above, and always return OK, since
       * we don't want to abort the walk on a single error.
       */
      return (AE_OK);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsInitializeObjects
     *
     * PARAMETERS:  TableDesc       - Descriptor for parent ACPI table
     *              StartNode       - Root of subtree to be initialized.
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Walk the namespace starting at "StartNode" and perform any
     *              necessary initialization on the objects found therein
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsInitializeObjects(UINT32 TableIndex,
                            ACPI_NAMESPACE_NODE * StartNode) {
      ACPI_STATUS Status;
      ACPI_INIT_WALK_INFO Info;
      ACPI_TABLE_HEADER *Table;
      ACPI_OWNER_ID OwnerId;

      ACPI_FUNCTION_TRACE(DsInitializeObjects);

      Status = AcpiTbGetOwnerId(TableIndex, &OwnerId);
      if (ACPI_FAILURE(Status)) {
        return_ACPI_STATUS(Status);
      }

      ACPI_DEBUG_PRINT(
          (ACPI_DB_DISPATCH,
           "**** Starting initialization of namespace objects ****\n"));

      /* Set all init info to zero */

      ACPI_MEMSET(&Info, 0, sizeof(ACPI_INIT_WALK_INFO));

      Info.OwnerId = OwnerId;
      Info.TableIndex = TableIndex;

      /* Walk entire namespace from the supplied root */

      Status = AcpiUtAcquireMutex(ACPI_MTX_NAMESPACE);
      if (ACPI_FAILURE(Status)) {
        return_ACPI_STATUS(Status);
      }

      /*
       * We don't use AcpiWalkNamespace since we do not want to acquire
       * the namespace reader lock.
       */
      Status = AcpiNsWalkNamespace(ACPI_TYPE_ANY, StartNode, ACPI_UINT32_MAX,
                                   ACPI_NS_WALK_UNLOCK, AcpiDsInitOneObject,
                                   NULL, &Info, NULL);
      if (ACPI_FAILURE(Status)) {
        ACPI_EXCEPTION((AE_INFO, Status, "During WalkNamespace"));
      }
      (void)AcpiUtReleaseMutex(ACPI_MTX_NAMESPACE);

      Status = AcpiGetTableByIndex(TableIndex, &Table);
      if (ACPI_FAILURE(Status)) {
        return_ACPI_STATUS(Status);
      }

      ACPI_DEBUG_PRINT_RAW(
          (ACPI_DB_INIT,
           "Table [%4.4s] (id %4.4X) - %4u Objects with %3u Devices, "
           "%3u Regions, %3u Methods (%u/%u/%u Serial/Non/Cvt)\n",
           Table->Signature, OwnerId, Info.ObjectCount, Info.DeviceCount,
           Info.OpRegionCount, Info.MethodCount, Info.SerialMethodCount,
           Info.NonSerialMethodCount, Info.SerializedMethodCount));

      ACPI_DEBUG_PRINT((ACPI_DB_DISPATCH, "%u Methods, %u Regions\n",
                        Info.MethodCount, Info.OpRegionCount));

      return_ACPI_STATUS(AE_OK);
    }

    /**
     * @section minix_drivers_power_acpi_dispatcher_dsmethod_c
     * @brief Original file: minix/drivers/power/acpi/dispatcher/dsmethod.c
     */
    /******************************************************************************
     *
     * Module Name: dsmethod - Parser/Interpreter interface - control method
     *parsing
     *
     *****************************************************************************/

    /*
     * Copyright (C) 2000 - 2014, Intel Corp.
     * All rights reserved.
     *
     * Redistribution and use in source and binary forms, with or without
     * modification, are permitted provided that the following conditions
     * are met:
     * 1. Redistributions of source code must retain the above copyright
     *    notice, this list of conditions, and the following disclaimer,
     *    without modification.
     * 2. Redistributions in binary form must reproduce at minimum a disclaimer
     *    substantially similar to the "NO WARRANTY" disclaimer below
     *    ("Disclaimer") and any redistribution must be conditioned upon
     *    including a substantially similar Disclaimer requirement for further
     *    binary redistribution.
     * 3. Neither the names of the above-listed copyright holders nor the names
     *    of any contributors may be used to endorse or promote products derived
     *    from this software without specific prior written permission.
     *
     * Alternatively, this software may be distributed under the terms of the
     * GNU General Public License ("GPL") version 2 as published by the Free
     * Software Foundation.
     *
     * NO WARRANTY
     * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
     * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
     * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR
     * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
     * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR
     * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
     * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
     * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
     * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
     * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
     * THE POSSIBILITY OF SUCH DAMAGES.
     */

#include "accommon.h"
#include "acdisasm.h"
#include "acdispat.h"
#include "acinterp.h"
#include "acnamesp.h"
#include "acparser.h"
#include "acpi.h"
#include "amlcode.h"

#define _COMPONENT ACPI_DISPATCHER
    ACPI_MODULE_NAME("dsmethod")

    /* Local prototypes */

    static ACPI_STATUS AcpiDsDetectNamedOpcodes(ACPI_WALK_STATE * WalkState,
                                                ACPI_PARSE_OBJECT * *OutOp);

    static ACPI_STATUS AcpiDsCreateMethodMutex(ACPI_OPERAND_OBJECT *
                                               MethodDesc);

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsAutoSerializeMethod
     *
     * PARAMETERS:  Node                        - Namespace Node of the method
     *              ObjDesc                     - Method object attached to node
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Parse a control method AML to scan for control methods that
     *              need serialization due to the creation of named objects.
     *
     * NOTE: It is a bit of overkill to mark all such methods serialized, since
     * there is only a problem if the method actually blocks during execution.
     * A blocking operation is, for example, a Sleep() operation, or any access
     * to an operation region. However, it is probably not possible to easily
     * detect whether a method will block or not, so we simply mark all
     *suspicious methods as serialized.
     *
     * NOTE2: This code is essentially a generic routine for parsing a single
     * control method.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsAutoSerializeMethod(ACPI_NAMESPACE_NODE * Node,
                              ACPI_OPERAND_OBJECT * ObjDesc) {
      ACPI_STATUS Status;
      ACPI_PARSE_OBJECT *Op = NULL;
      ACPI_WALK_STATE *WalkState;

      ACPI_FUNCTION_TRACE_PTR(DsAutoSerializeMethod, Node);

      ACPI_DEBUG_PRINT((ACPI_DB_PARSE,
                        "Method auto-serialization parse [%4.4s] %p\n",
                        AcpiUtGetNodeName(Node), Node));

      /* Create/Init a root op for the method parse tree */

      Op = AcpiPsAllocOp(AML_METHOD_OP);
      if (!Op) {
        return_ACPI_STATUS(AE_NO_MEMORY);
      }

      AcpiPsSetName(Op, Node->Name.Integer);
      Op->Common.Node = Node;

      /* Create and initialize a new walk state */

      WalkState = AcpiDsCreateWalkState(Node->OwnerId, NULL, NULL, NULL);
      if (!WalkState) {
        return_ACPI_STATUS(AE_NO_MEMORY);
      }

      Status = AcpiDsInitAmlWalk(WalkState, Op, Node, ObjDesc->Method.AmlStart,
                                 ObjDesc->Method.AmlLength, NULL, 0);
      if (ACPI_FAILURE(Status)) {
        AcpiDsDeleteWalkState(WalkState);
        return_ACPI_STATUS(Status);
      }

      WalkState->DescendingCallback = AcpiDsDetectNamedOpcodes;

      /* Parse the method, scan for creation of named objects */

      Status = AcpiPsParseAml(WalkState);
      if (ACPI_FAILURE(Status)) {
        return_ACPI_STATUS(Status);
      }

      AcpiPsDeleteParseTree(Op);
      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsDetectNamedOpcodes
     *
     * PARAMETERS:  WalkState       - Current state of the parse tree walk
     *              OutOp           - Unused, required for parser interface
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Descending callback used during the loading of ACPI tables.
     *              Currently used to detect methods that must be marked
     *serialized in order to avoid problems with the creation of named objects.
     *
     ******************************************************************************/

    static ACPI_STATUS AcpiDsDetectNamedOpcodes(ACPI_WALK_STATE * WalkState,
                                                ACPI_PARSE_OBJECT * *OutOp) {

      ACPI_FUNCTION_NAME(AcpiDsDetectNamedOpcodes);

      /* We are only interested in opcodes that create a new name */

      if (!(WalkState->OpInfo->Flags & (AML_NAMED | AML_CREATE | AML_FIELD))) {
        return (AE_OK);
      }

      /*
       * At this point, we know we have a Named object opcode.
       * Mark the method as serialized. Later code will create a mutex for
       * this method to enforce serialization.
       *
       * Note, ACPI_METHOD_IGNORE_SYNC_LEVEL flag means that we will ignore the
       * Sync Level mechanism for this method, even though it is now serialized.
       * Otherwise, there can be conflicts with existing ASL code that actually
       * uses sync levels.
       */
      WalkState->MethodDesc->Method.SyncLevel = 0;
      WalkState->MethodDesc->Method.InfoFlags |=
          (ACPI_METHOD_SERIALIZED | ACPI_METHOD_IGNORE_SYNC_LEVEL);

      ACPI_DEBUG_PRINT(
          (ACPI_DB_INFO, "Method serialized [%4.4s] %p - [%s] (%4.4X)\n",
           WalkState->MethodNode->Name.Ascii, WalkState->MethodNode,
           WalkState->OpInfo->Name, WalkState->Opcode));

      /* Abort the parse, no need to examine this method any further */

      return (AE_CTRL_TERMINATE);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsMethodError
     *
     * PARAMETERS:  Status          - Execution status
     *              WalkState       - Current state
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Called on method error. Invoke the global exception handler
     *if present, dump the method data if the disassembler is configured
     *
     *              Note: Allows the exception handler to change the status code
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsMethodError(ACPI_STATUS Status, ACPI_WALK_STATE * WalkState) {
      ACPI_FUNCTION_ENTRY();

      /* Ignore AE_OK and control exception codes */

      if (ACPI_SUCCESS(Status) || (Status & AE_CODE_CONTROL)) {
        return (Status);
      }

      /* Invoke the global exception handler */

      if (AcpiGbl_ExceptionHandler) {
        /* Exit the interpreter, allow handler to execute methods */

        AcpiExExitInterpreter();

        /*
         * Handler can map the exception code to anything it wants, including
         * AE_OK, in which case the executing method will not be aborted.
         */
        Status = AcpiGbl_ExceptionHandler(
            Status,
            WalkState->MethodNode ? WalkState->MethodNode->Name.Integer : 0,
            WalkState->Opcode, WalkState->AmlOffset, NULL);
        AcpiExEnterInterpreter();
      }

      AcpiDsClearImplicitReturn(WalkState);

#ifdef ACPI_DISASSEMBLER
      if (ACPI_FAILURE(Status)) {
        /* Display method locals/args if disassembler is present */

        AcpiDmDumpMethodInfo(Status, WalkState, WalkState->Op);
      }
#endif

      return (Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsCreateMethodMutex
     *
     * PARAMETERS:  ObjDesc             - The method object
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Create a mutex object for a serialized control method
     *
     ******************************************************************************/

    static ACPI_STATUS AcpiDsCreateMethodMutex(ACPI_OPERAND_OBJECT *
                                               MethodDesc) {
      ACPI_OPERAND_OBJECT *MutexDesc;
      ACPI_STATUS Status;

      ACPI_FUNCTION_TRACE(DsCreateMethodMutex);

      /* Create the new mutex object */

      MutexDesc = AcpiUtCreateInternalObject(ACPI_TYPE_MUTEX);
      if (!MutexDesc) {
        return_ACPI_STATUS(AE_NO_MEMORY);
      }

      /* Create the actual OS Mutex */

      Status = AcpiOsCreateMutex(&MutexDesc->Mutex.OsMutex);
      if (ACPI_FAILURE(Status)) {
        AcpiUtDeleteObjectDesc(MutexDesc);
        return_ACPI_STATUS(Status);
      }

      MutexDesc->Mutex.SyncLevel = MethodDesc->Method.SyncLevel;
      MethodDesc->Method.Mutex = MutexDesc;
      return_ACPI_STATUS(AE_OK);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsBeginMethodExecution
     *
     * PARAMETERS:  MethodNode          - Node of the method
     *              ObjDesc             - The method object
     *              WalkState           - current state, NULL if not yet
     *executing a method.
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Prepare a method for execution. Parses the method if
     *necessary, increments the thread count, and waits at the method semaphore
     *              for clearance to execute.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsBeginMethodExecution(ACPI_NAMESPACE_NODE * MethodNode,
                               ACPI_OPERAND_OBJECT * ObjDesc,
                               ACPI_WALK_STATE * WalkState) {
      ACPI_STATUS Status = AE_OK;

      ACPI_FUNCTION_TRACE_PTR(DsBeginMethodExecution, MethodNode);

      if (!MethodNode) {
        return_ACPI_STATUS(AE_NULL_ENTRY);
      }

      /* Prevent wraparound of thread count */

      if (ObjDesc->Method.ThreadCount == ACPI_UINT8_MAX) {
        ACPI_ERROR((AE_INFO, "Method reached maximum reentrancy limit (255)"));
        return_ACPI_STATUS(AE_AML_METHOD_LIMIT);
      }

      /*
       * If this method is serialized, we need to acquire the method mutex.
       */
      if (ObjDesc->Method.InfoFlags & ACPI_METHOD_SERIALIZED) {
        /*
         * Create a mutex for the method if it is defined to be Serialized
         * and a mutex has not already been created. We defer the mutex creation
         * until a method is actually executed, to minimize the object count
         */
        if (!ObjDesc->Method.Mutex) {
          Status = AcpiDsCreateMethodMutex(ObjDesc);
          if (ACPI_FAILURE(Status)) {
            return_ACPI_STATUS(Status);
          }
        }

        /*
         * The CurrentSyncLevel (per-thread) must be less than or equal to
         * the sync level of the method. This mechanism provides some
         * deadlock prevention.
         *
         * If the method was auto-serialized, we just ignore the sync level
         * mechanism, because auto-serialization of methods can interfere
         * with ASL code that actually uses sync levels.
         *
         * Top-level method invocation has no walk state at this point
         */
        if (WalkState &&
            (!(ObjDesc->Method.InfoFlags & ACPI_METHOD_IGNORE_SYNC_LEVEL)) &&
            (WalkState->Thread->CurrentSyncLevel >
             ObjDesc->Method.Mutex->Mutex.SyncLevel)) {
          ACPI_ERROR((AE_INFO,
                      "Cannot acquire Mutex for method [%4.4s], current "
                      "SyncLevel is too large (%u)",
                      AcpiUtGetNodeName(MethodNode),
                      WalkState->Thread->CurrentSyncLevel));

          return_ACPI_STATUS(AE_AML_MUTEX_ORDER);
        }

        /*
         * Obtain the method mutex if necessary. Do not acquire mutex for a
         * recursive call.
         */
        if (!WalkState || !ObjDesc->Method.Mutex->Mutex.ThreadId ||
            (WalkState->Thread->ThreadId !=
             ObjDesc->Method.Mutex->Mutex.ThreadId)) {
          /*
           * Acquire the method mutex. This releases the interpreter if we
           * block (and reacquires it before it returns)
           */
          Status = AcpiExSystemWaitMutex(ObjDesc->Method.Mutex->Mutex.OsMutex,
                                         ACPI_WAIT_FOREVER);
          if (ACPI_FAILURE(Status)) {
            return_ACPI_STATUS(Status);
          }

          /* Update the mutex and walk info and save the original SyncLevel */

          if (WalkState) {
            ObjDesc->Method.Mutex->Mutex.OriginalSyncLevel =
                WalkState->Thread->CurrentSyncLevel;

            ObjDesc->Method.Mutex->Mutex.ThreadId = WalkState->Thread->ThreadId;
            WalkState->Thread->CurrentSyncLevel = ObjDesc->Method.SyncLevel;
          } else {
            ObjDesc->Method.Mutex->Mutex.OriginalSyncLevel =
                ObjDesc->Method.Mutex->Mutex.SyncLevel;
          }
        }

        /* Always increase acquisition depth */

        ObjDesc->Method.Mutex->Mutex.AcquisitionDepth++;
      }

      /*
       * Allocate an Owner ID for this method, only if this is the first thread
       * to begin concurrent execution. We only need one OwnerId, even if the
       * method is invoked recursively.
       */
      if (!ObjDesc->Method.OwnerId) {
        Status = AcpiUtAllocateOwnerId(&ObjDesc->Method.OwnerId);
        if (ACPI_FAILURE(Status)) {
          goto Cleanup;
        }
      }

      /*
       * Increment the method parse tree thread count since it has been
       * reentered one more time (even if it is the same thread)
       */
      ObjDesc->Method.ThreadCount++;
      AcpiMethodCount++;
      return_ACPI_STATUS(Status);

    Cleanup:
      /* On error, must release the method mutex (if present) */

      if (ObjDesc->Method.Mutex) {
        AcpiOsReleaseMutex(ObjDesc->Method.Mutex->Mutex.OsMutex);
      }
      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsCallControlMethod
     *
     * PARAMETERS:  Thread              - Info for this thread
     *              ThisWalkState       - Current walk state
     *              Op                  - Current Op to be walked
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Transfer execution to a called control method
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsCallControlMethod(ACPI_THREAD_STATE * Thread,
                            ACPI_WALK_STATE * ThisWalkState,
                            ACPI_PARSE_OBJECT * Op) {
      ACPI_STATUS Status;
      ACPI_NAMESPACE_NODE *MethodNode;
      ACPI_WALK_STATE *NextWalkState = NULL;
      ACPI_OPERAND_OBJECT *ObjDesc;
      ACPI_EVALUATE_INFO *Info;
      UINT32 i;

      ACPI_FUNCTION_TRACE_PTR(DsCallControlMethod, ThisWalkState);

      ACPI_DEBUG_PRINT((ACPI_DB_DISPATCH,
                        "Calling method %p, currentstate=%p\n",
                        ThisWalkState->PrevOp, ThisWalkState));

      /*
       * Get the namespace entry for the control method we are about to call
       */
      MethodNode = ThisWalkState->MethodCallNode;
      if (!MethodNode) {
        return_ACPI_STATUS(AE_NULL_ENTRY);
      }

      ObjDesc = AcpiNsGetAttachedObject(MethodNode);
      if (!ObjDesc) {
        return_ACPI_STATUS(AE_NULL_OBJECT);
      }

      /* Init for new method, possibly wait on method mutex */

      Status = AcpiDsBeginMethodExecution(MethodNode, ObjDesc, ThisWalkState);
      if (ACPI_FAILURE(Status)) {
        return_ACPI_STATUS(Status);
      }

      /* Begin method parse/execution. Create a new walk state */

      NextWalkState =
          AcpiDsCreateWalkState(ObjDesc->Method.OwnerId, NULL, ObjDesc, Thread);
      if (!NextWalkState) {
        Status = AE_NO_MEMORY;
        goto Cleanup;
      }

      /*
       * The resolved arguments were put on the previous walk state's operand
       * stack. Operands on the previous walk state stack always
       * start at index 0. Also, null terminate the list of arguments
       */
      ThisWalkState->Operands[ThisWalkState->NumOperands] = NULL;

      /*
       * Allocate and initialize the evaluation information block
       * TBD: this is somewhat inefficient, should change interface to
       * DsInitAmlWalk. For now, keeps this struct off the CPU stack
       */
      Info = ACPI_ALLOCATE_ZEROED(sizeof(ACPI_EVALUATE_INFO));
      if (!Info) {
        Status = AE_NO_MEMORY;
        goto Cleanup;
      }

      Info->Parameters = &ThisWalkState->Operands[0];

      Status = AcpiDsInitAmlWalk(
          NextWalkState, NULL, MethodNode, ObjDesc->Method.AmlStart,
          ObjDesc->Method.AmlLength, Info, ACPI_IMODE_EXECUTE);

      ACPI_FREE(Info);
      if (ACPI_FAILURE(Status)) {
        goto Cleanup;
      }

      /*
       * Delete the operands on the previous walkstate operand stack
       * (they were copied to new objects)
       */
      for (i = 0; i < ObjDesc->Method.ParamCount; i++) {
        AcpiUtRemoveReference(ThisWalkState->Operands[i]);
        ThisWalkState->Operands[i] = NULL;
      }

      /* Clear the operand stack */

      ThisWalkState->NumOperands = 0;

      ACPI_DEBUG_PRINT(
          (ACPI_DB_DISPATCH,
           "**** Begin nested execution of [%4.4s] **** WalkState=%p\n",
           MethodNode->Name.Ascii, NextWalkState));

      /* Invoke an internal method if necessary */

      if (ObjDesc->Method.InfoFlags & ACPI_METHOD_INTERNAL_ONLY) {
        Status = ObjDesc->Method.Dispatch.Implementation(NextWalkState);
        if (Status == AE_OK) {
          Status = AE_CTRL_TERMINATE;
        }
      }

      return_ACPI_STATUS(Status);

    Cleanup:

      /* On error, we must terminate the method properly */

      AcpiDsTerminateControlMethod(ObjDesc, NextWalkState);
      if (NextWalkState) {
        AcpiDsDeleteWalkState(NextWalkState);
      }

      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsRestartControlMethod
     *
     * PARAMETERS:  WalkState           - State for preempted method (caller)
     *              ReturnDesc          - Return value from the called method
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Restart a method that was preempted by another (nested)
     *method invocation. Handle the return value (if any) from the callee.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsRestartControlMethod(ACPI_WALK_STATE * WalkState,
                               ACPI_OPERAND_OBJECT * ReturnDesc) {
      ACPI_STATUS Status;
      int SameAsImplicitReturn;

      ACPI_FUNCTION_TRACE_PTR(DsRestartControlMethod, WalkState);

      ACPI_DEBUG_PRINT((ACPI_DB_DISPATCH,
                        "****Restart [%4.4s] Op %p ReturnValueFromCallee %p\n",
                        AcpiUtGetNodeName(WalkState->MethodNode),
                        WalkState->MethodCallOp, ReturnDesc));

      ACPI_DEBUG_PRINT(
          (ACPI_DB_DISPATCH,
           "    ReturnFromThisMethodUsed?=%X ResStack %p Walk %p\n",
           WalkState->ReturnUsed, WalkState->Results, WalkState));

      /* Did the called method return a value? */

      if (ReturnDesc) {
        /* Is the implicit return object the same as the return desc? */

        SameAsImplicitReturn = (WalkState->ImplicitReturnObj == ReturnDesc);

        /* Are we actually going to use the return value? */

        if (WalkState->ReturnUsed) {
          /* Save the return value from the previous method */

          Status = AcpiDsResultPush(ReturnDesc, WalkState);
          if (ACPI_FAILURE(Status)) {
            AcpiUtRemoveReference(ReturnDesc);
            return_ACPI_STATUS(Status);
          }

          /*
           * Save as THIS method's return value in case it is returned
           * immediately to yet another method
           */
          WalkState->ReturnDesc = ReturnDesc;
        }

        /*
         * The following code is the optional support for the so-called
         * "implicit return". Some AML code assumes that the last value of the
         * method is "implicitly" returned to the caller, in the absence of an
         * explicit return value.
         *
         * Just save the last result of the method as the return value.
         *
         * NOTE: this is optional because the ASL language does not actually
         * support this behavior.
         */
        else if (!AcpiDsDoImplicitReturn(ReturnDesc, WalkState, FALSE) ||
                 SameAsImplicitReturn) {
          /*
           * Delete the return value if it will not be used by the
           * calling method or remove one reference if the explicit return
           * is the same as the implicit return value.
           */
          AcpiUtRemoveReference(ReturnDesc);
        }
      }

      return_ACPI_STATUS(AE_OK);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsTerminateControlMethod
     *
     * PARAMETERS:  MethodDesc          - Method object
     *              WalkState           - State associated with the method
     *
     * RETURN:      None
     *
     * DESCRIPTION: Terminate a control method. Delete everything that the
     *method created, delete all locals and arguments, and delete the parse tree
     *if requested.
     *
     * MUTEX:       Interpreter is locked
     *
     ******************************************************************************/

    void AcpiDsTerminateControlMethod(ACPI_OPERAND_OBJECT * MethodDesc,
                                      ACPI_WALK_STATE * WalkState) {

      ACPI_FUNCTION_TRACE_PTR(DsTerminateControlMethod, WalkState);

      /* MethodDesc is required, WalkState is optional */

      if (!MethodDesc) {
        return_VOID;
      }

      if (WalkState) {
        /* Delete all arguments and locals */

        AcpiDsMethodDataDeleteAll(WalkState);

        /*
         * If method is serialized, release the mutex and restore the
         * current sync level for this thread
         */
        if (MethodDesc->Method.Mutex) {
          /* Acquisition Depth handles recursive calls */

          MethodDesc->Method.Mutex->Mutex.AcquisitionDepth--;
          if (!MethodDesc->Method.Mutex->Mutex.AcquisitionDepth) {
            WalkState->Thread->CurrentSyncLevel =
                MethodDesc->Method.Mutex->Mutex.OriginalSyncLevel;

            AcpiOsReleaseMutex(MethodDesc->Method.Mutex->Mutex.OsMutex);
            MethodDesc->Method.Mutex->Mutex.ThreadId = 0;
          }
        }

        /*
         * Delete any namespace objects created anywhere within the
         * namespace by the execution of this method. Unless:
         * 1) This method is a module-level executable code method, in which
         *    case we want make the objects permanent.
         * 2) There are other threads executing the method, in which case we
         *    will wait until the last thread has completed.
         */
        if (!(MethodDesc->Method.InfoFlags & ACPI_METHOD_MODULE_LEVEL) &&
            (MethodDesc->Method.ThreadCount == 1)) {
          /* Delete any direct children of (created by) this method */

          AcpiNsDeleteNamespaceSubtree(WalkState->MethodNode);

          /*
           * Delete any objects that were created by this method
           * elsewhere in the namespace (if any were created).
           * Use of the ACPI_METHOD_MODIFIED_NAMESPACE optimizes the
           * deletion such that we don't have to perform an entire
           * namespace walk for every control method execution.
           */
          if (MethodDesc->Method.InfoFlags & ACPI_METHOD_MODIFIED_NAMESPACE) {
            AcpiNsDeleteNamespaceByOwner(MethodDesc->Method.OwnerId);
            MethodDesc->Method.InfoFlags &= ~ACPI_METHOD_MODIFIED_NAMESPACE;
          }
        }
      }

      /* Decrement the thread count on the method */

      if (MethodDesc->Method.ThreadCount) {
        MethodDesc->Method.ThreadCount--;
      } else {
        ACPI_ERROR((AE_INFO, "Invalid zero thread count in method"));
      }

      /* Are there any other threads currently executing this method? */

      if (MethodDesc->Method.ThreadCount) {
        /*
         * Additional threads. Do not release the OwnerId in this case,
         * we immediately reuse it for the next thread executing this method
         */
        ACPI_DEBUG_PRINT(
            (ACPI_DB_DISPATCH,
             "*** Completed execution of one thread, %u threads remaining\n",
             MethodDesc->Method.ThreadCount));
      } else {
        /* This is the only executing thread for this method */

        /*
         * Support to dynamically change a method from NotSerialized to
         * Serialized if it appears that the method is incorrectly written and
         * does not support multiple thread execution. The best example of this
         * is if such a method creates namespace objects and blocks. A second
         * thread will fail with an AE_ALREADY_EXISTS exception.
         *
         * This code is here because we must wait until the last thread exits
         * before marking the method as serialized.
         */
        if (MethodDesc->Method.InfoFlags & ACPI_METHOD_SERIALIZED_PENDING) {
          if (WalkState) {
            ACPI_INFO((AE_INFO,
                       "Marking method %4.4s as Serialized because of "
                       "AE_ALREADY_EXISTS error",
                       WalkState->MethodNode->Name.Ascii));
          }

          /*
           * Method tried to create an object twice and was marked as
           * "pending serialized". The probable cause is that the method
           * cannot handle reentrancy.
           *
           * The method was created as NotSerialized, but it tried to create
           * a named object and then blocked, causing the second thread
           * entrance to begin and then fail. Workaround this problem by
           * marking the method permanently as Serialized when the last
           * thread exits here.
           */
          MethodDesc->Method.InfoFlags &= ~ACPI_METHOD_SERIALIZED_PENDING;
          MethodDesc->Method.InfoFlags |=
              (ACPI_METHOD_SERIALIZED | ACPI_METHOD_IGNORE_SYNC_LEVEL);
          MethodDesc->Method.SyncLevel = 0;
        }

        /* No more threads, we can free the OwnerId */

        if (!(MethodDesc->Method.InfoFlags & ACPI_METHOD_MODULE_LEVEL)) {
          AcpiUtReleaseOwnerId(&MethodDesc->Method.OwnerId);
        }
      }

      return_VOID;
    }

    /**
     * @section minix_drivers_power_acpi_dispatcher_dsmthdat_c
     * @brief Original file: minix/drivers/power/acpi/dispatcher/dsmthdat.c
     */
    /*******************************************************************************
     *
     * Module Name: dsmthdat - control method arguments and local variables
     *
     ******************************************************************************/

    /*
     * Copyright (C) 2000 - 2014, Intel Corp.
     * All rights reserved.
     *
     * Redistribution and use in source and binary forms, with or without
     * modification, are permitted provided that the following conditions
     * are met:
     * 1. Redistributions of source code must retain the above copyright
     *    notice, this list of conditions, and the following disclaimer,
     *    without modification.
     * 2. Redistributions in binary form must reproduce at minimum a disclaimer
     *    substantially similar to the "NO WARRANTY" disclaimer below
     *    ("Disclaimer") and any redistribution must be conditioned upon
     *    including a substantially similar Disclaimer requirement for further
     *    binary redistribution.
     * 3. Neither the names of the above-listed copyright holders nor the names
     *    of any contributors may be used to endorse or promote products derived
     *    from this software without specific prior written permission.
     *
     * Alternatively, this software may be distributed under the terms of the
     * GNU General Public License ("GPL") version 2 as published by the Free
     * Software Foundation.
     *
     * NO WARRANTY
     * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
     * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
     * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR
     * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
     * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR
     * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
     * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
     * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
     * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
     * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
     * THE POSSIBILITY OF SUCH DAMAGES.
     */

#include "accommon.h"
#include "acdispat.h"
#include "acinterp.h"
#include "acnamesp.h"
#include "acpi.h"

#define _COMPONENT ACPI_DISPATCHER
    ACPI_MODULE_NAME("dsmthdat")

    /* Local prototypes */

    static void AcpiDsMethodDataDeleteValue(UINT8 Type, UINT32 Index,
                                            ACPI_WALK_STATE * WalkState);

    static ACPI_STATUS AcpiDsMethodDataSetValue(UINT8 Type, UINT32 Index,
                                                ACPI_OPERAND_OBJECT * Object,
                                                ACPI_WALK_STATE * WalkState);

#ifdef ACPI_OBSOLETE_FUNCTIONS
    ACPI_OBJECT_TYPE
    AcpiDsMethodDataGetType(UINT16 Opcode, UINT32 Index,
                            ACPI_WALK_STATE * WalkState);
#endif

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsMethodDataInit
     *
     * PARAMETERS:  WalkState           - Current walk state object
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Initialize the data structures that hold the method's
     *arguments and locals. The data struct is an array of namespace nodes for
     *              each - this allows RefOf and DeRefOf to work properly for
     *these special data types.
     *
     * NOTES:       WalkState fields are initialized to zero by the
     *              ACPI_ALLOCATE_ZEROED().
     *
     *              A pseudo-Namespace Node is assigned to each argument and
     *local so that RefOf() can return a pointer to the Node.
     *
     ******************************************************************************/

    void AcpiDsMethodDataInit(ACPI_WALK_STATE * WalkState) {
      UINT32 i;

      ACPI_FUNCTION_TRACE(DsMethodDataInit);

      /* Init the method arguments */

      for (i = 0; i < ACPI_METHOD_NUM_ARGS; i++) {
        ACPI_MOVE_32_TO_32(&WalkState->Arguments[i].Name, NAMEOF_ARG_NTE);
        WalkState->Arguments[i].Name.Integer |= (i << 24);
        WalkState->Arguments[i].DescriptorType = ACPI_DESC_TYPE_NAMED;
        WalkState->Arguments[i].Type = ACPI_TYPE_ANY;
        WalkState->Arguments[i].Flags = ANOBJ_METHOD_ARG;
      }

      /* Init the method locals */

      for (i = 0; i < ACPI_METHOD_NUM_LOCALS; i++) {
        ACPI_MOVE_32_TO_32(&WalkState->LocalVariables[i].Name,
                           NAMEOF_LOCAL_NTE);

        WalkState->LocalVariables[i].Name.Integer |= (i << 24);
        WalkState->LocalVariables[i].DescriptorType = ACPI_DESC_TYPE_NAMED;
        WalkState->LocalVariables[i].Type = ACPI_TYPE_ANY;
        WalkState->LocalVariables[i].Flags = ANOBJ_METHOD_LOCAL;
      }

      return_VOID;
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsMethodDataDeleteAll
     *
     * PARAMETERS:  WalkState           - Current walk state object
     *
     * RETURN:      None
     *
     * DESCRIPTION: Delete method locals and arguments. Arguments are only
     *              deleted if this method was called from another method.
     *
     ******************************************************************************/

    void AcpiDsMethodDataDeleteAll(ACPI_WALK_STATE * WalkState) {
      UINT32 Index;

      ACPI_FUNCTION_TRACE(DsMethodDataDeleteAll);

      /* Detach the locals */

      for (Index = 0; Index < ACPI_METHOD_NUM_LOCALS; Index++) {
        if (WalkState->LocalVariables[Index].Object) {
          ACPI_DEBUG_PRINT((ACPI_DB_EXEC, "Deleting Local%u=%p\n", Index,
                            WalkState->LocalVariables[Index].Object));

          /* Detach object (if present) and remove a reference */

          AcpiNsDetachObject(&WalkState->LocalVariables[Index]);
        }
      }

      /* Detach the arguments */

      for (Index = 0; Index < ACPI_METHOD_NUM_ARGS; Index++) {
        if (WalkState->Arguments[Index].Object) {
          ACPI_DEBUG_PRINT((ACPI_DB_EXEC, "Deleting Arg%u=%p\n", Index,
                            WalkState->Arguments[Index].Object));

          /* Detach object (if present) and remove a reference */

          AcpiNsDetachObject(&WalkState->Arguments[Index]);
        }
      }

      return_VOID;
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsMethodDataInitArgs
     *
     * PARAMETERS:  *Params         - Pointer to a parameter list for the method
     *              MaxParamCount   - The arg count for this method
     *              WalkState       - Current walk state object
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Initialize arguments for a method. The parameter list is a
     *list of ACPI operand objects, either null terminated or whose length is
     *defined by MaxParamCount.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsMethodDataInitArgs(ACPI_OPERAND_OBJECT * *Params,
                             UINT32 MaxParamCount,
                             ACPI_WALK_STATE * WalkState) {
      ACPI_STATUS Status;
      UINT32 Index = 0;

      ACPI_FUNCTION_TRACE_PTR(DsMethodDataInitArgs, Params);

      if (!Params) {
        ACPI_DEBUG_PRINT((ACPI_DB_EXEC, "No param list passed to method\n"));
        return_ACPI_STATUS(AE_OK);
      }

      /* Copy passed parameters into the new method stack frame */

      while ((Index < ACPI_METHOD_NUM_ARGS) && (Index < MaxParamCount) &&
             Params[Index]) {
        /*
         * A valid parameter.
         * Store the argument in the method/walk descriptor.
         * Do not copy the arg in order to implement call by reference
         */
        Status = AcpiDsMethodDataSetValue(ACPI_REFCLASS_ARG, Index,
                                          Params[Index], WalkState);
        if (ACPI_FAILURE(Status)) {
          return_ACPI_STATUS(Status);
        }

        Index++;
      }

      ACPI_DEBUG_PRINT((ACPI_DB_EXEC, "%u args passed to method\n", Index));
      return_ACPI_STATUS(AE_OK);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsMethodDataGetNode
     *
     * PARAMETERS:  Type                - Either ACPI_REFCLASS_LOCAL or
     *                                    ACPI_REFCLASS_ARG
     *              Index               - Which Local or Arg whose type to get
     *              WalkState           - Current walk state object
     *              Node                - Where the node is returned.
     *
     * RETURN:      Status and node
     *
     * DESCRIPTION: Get the Node associated with a local or arg.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsMethodDataGetNode(UINT8 Type, UINT32 Index,
                            ACPI_WALK_STATE * WalkState,
                            ACPI_NAMESPACE_NODE * *Node) {
      ACPI_FUNCTION_TRACE(DsMethodDataGetNode);

      /*
       * Method Locals and Arguments are supported
       */
      switch (Type) {
      case ACPI_REFCLASS_LOCAL:

        if (Index > ACPI_METHOD_MAX_LOCAL) {
          ACPI_ERROR((AE_INFO, "Local index %u is invalid (max %u)", Index,
                      ACPI_METHOD_MAX_LOCAL));
          return_ACPI_STATUS(AE_AML_INVALID_INDEX);
        }

        /* Return a pointer to the pseudo-node */

        *Node = &WalkState->LocalVariables[Index];
        break;

      case ACPI_REFCLASS_ARG:

        if (Index > ACPI_METHOD_MAX_ARG) {
          ACPI_ERROR((AE_INFO, "Arg index %u is invalid (max %u)", Index,
                      ACPI_METHOD_MAX_ARG));
          return_ACPI_STATUS(AE_AML_INVALID_INDEX);
        }

        /* Return a pointer to the pseudo-node */

        *Node = &WalkState->Arguments[Index];
        break;

      default:

        ACPI_ERROR((AE_INFO, "Type %u is invalid", Type));
        return_ACPI_STATUS(AE_TYPE);
      }

      return_ACPI_STATUS(AE_OK);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsMethodDataSetValue
     *
     * PARAMETERS:  Type                - Either ACPI_REFCLASS_LOCAL or
     *                                    ACPI_REFCLASS_ARG
     *              Index               - Which Local or Arg to get
     *              Object              - Object to be inserted into the stack
     *entry WalkState           - Current walk state object
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Insert an object onto the method stack at entry
     *Opcode:Index. Note: There is no "implicit conversion" for locals.
     *
     ******************************************************************************/

    static ACPI_STATUS AcpiDsMethodDataSetValue(UINT8 Type, UINT32 Index,
                                                ACPI_OPERAND_OBJECT * Object,
                                                ACPI_WALK_STATE * WalkState) {
      ACPI_STATUS Status;
      ACPI_NAMESPACE_NODE *Node;

      ACPI_FUNCTION_TRACE(DsMethodDataSetValue);

      ACPI_DEBUG_PRINT((ACPI_DB_EXEC, "NewObj %p Type %2.2X, Refs=%u [%s]\n",
                        Object, Type, Object->Common.ReferenceCount,
                        AcpiUtGetTypeName(Object->Common.Type)));

      /* Get the namespace node for the arg/local */

      Status = AcpiDsMethodDataGetNode(Type, Index, WalkState, &Node);
      if (ACPI_FAILURE(Status)) {
        return_ACPI_STATUS(Status);
      }

      /*
       * Increment ref count so object can't be deleted while installed.
       * NOTE: We do not copy the object in order to preserve the call by
       * reference semantics of ACPI Control Method invocation.
       * (See ACPI Specification 2.0C)
       */
      AcpiUtAddReference(Object);

      /* Install the object */

      Node->Object = Object;
      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsMethodDataGetValue
     *
     * PARAMETERS:  Type                - Either ACPI_REFCLASS_LOCAL or
     *                                    ACPI_REFCLASS_ARG
     *              Index               - Which localVar or argument to get
     *              WalkState           - Current walk state object
     *              DestDesc            - Where Arg or Local value is returned
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Retrieve value of selected Arg or Local for this method
     *              Used only in AcpiExResolveToValue().
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsMethodDataGetValue(UINT8 Type, UINT32 Index,
                             ACPI_WALK_STATE * WalkState,
                             ACPI_OPERAND_OBJECT * *DestDesc) {
      ACPI_STATUS Status;
      ACPI_NAMESPACE_NODE *Node;
      ACPI_OPERAND_OBJECT *Object;

      ACPI_FUNCTION_TRACE(DsMethodDataGetValue);

      /* Validate the object descriptor */

      if (!DestDesc) {
        ACPI_ERROR((AE_INFO, "Null object descriptor pointer"));
        return_ACPI_STATUS(AE_BAD_PARAMETER);
      }

      /* Get the namespace node for the arg/local */

      Status = AcpiDsMethodDataGetNode(Type, Index, WalkState, &Node);
      if (ACPI_FAILURE(Status)) {
        return_ACPI_STATUS(Status);
      }

      /* Get the object from the node */

      Object = Node->Object;

      /* Examine the returned object, it must be valid. */

      if (!Object) {
        /*
         * Index points to uninitialized object.
         * This means that either 1) The expected argument was
         * not passed to the method, or 2) A local variable
         * was referenced by the method (via the ASL)
         * before it was initialized. Either case is an error.
         */

        /* If slack enabled, init the LocalX/ArgX to an Integer of value zero */

        if (AcpiGbl_EnableInterpreterSlack) {
          Object = AcpiUtCreateIntegerObject((UINT64)0);
          if (!Object) {
            return_ACPI_STATUS(AE_NO_MEMORY);
          }

          Node->Object = Object;
        }

        /* Otherwise, return the error */

        else
          switch (Type) {
          case ACPI_REFCLASS_ARG:

            ACPI_ERROR(
                (AE_INFO, "Uninitialized Arg[%u] at node %p", Index, Node));

            return_ACPI_STATUS(AE_AML_UNINITIALIZED_ARG);

          case ACPI_REFCLASS_LOCAL:
            /*
             * No error message for this case, will be trapped again later to
             * detect and ignore cases of Store(LocalX,LocalX)
             */
            return_ACPI_STATUS(AE_AML_UNINITIALIZED_LOCAL);

          default:

            ACPI_ERROR((AE_INFO, "Not a Arg/Local opcode: 0x%X", Type));
            return_ACPI_STATUS(AE_AML_INTERNAL);
          }
      }

      /*
       * The Index points to an initialized and valid object.
       * Return an additional reference to the object
       */
      *DestDesc = Object;
      AcpiUtAddReference(Object);

      return_ACPI_STATUS(AE_OK);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsMethodDataDeleteValue
     *
     * PARAMETERS:  Type                - Either ACPI_REFCLASS_LOCAL or
     *                                    ACPI_REFCLASS_ARG
     *              Index               - Which localVar or argument to delete
     *              WalkState           - Current walk state object
     *
     * RETURN:      None
     *
     * DESCRIPTION: Delete the entry at Opcode:Index. Inserts
     *              a null into the stack slot after the object is deleted.
     *
     ******************************************************************************/

    static void AcpiDsMethodDataDeleteValue(UINT8 Type, UINT32 Index,
                                            ACPI_WALK_STATE * WalkState) {
      ACPI_STATUS Status;
      ACPI_NAMESPACE_NODE *Node;
      ACPI_OPERAND_OBJECT *Object;

      ACPI_FUNCTION_TRACE(DsMethodDataDeleteValue);

      /* Get the namespace node for the arg/local */

      Status = AcpiDsMethodDataGetNode(Type, Index, WalkState, &Node);
      if (ACPI_FAILURE(Status)) {
        return_VOID;
      }

      /* Get the associated object */

      Object = AcpiNsGetAttachedObject(Node);

      /*
       * Undefine the Arg or Local by setting its descriptor
       * pointer to NULL. Locals/Args can contain both
       * ACPI_OPERAND_OBJECTS and ACPI_NAMESPACE_NODEs
       */
      Node->Object = NULL;

      if ((Object) &&
          (ACPI_GET_DESCRIPTOR_TYPE(Object) == ACPI_DESC_TYPE_OPERAND)) {
        /*
         * There is a valid object.
         * Decrement the reference count by one to balance the
         * increment when the object was stored.
         */
        AcpiUtRemoveReference(Object);
      }

      return_VOID;
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsStoreObjectToLocal
     *
     * PARAMETERS:  Type                - Either ACPI_REFCLASS_LOCAL or
     *                                    ACPI_REFCLASS_ARG
     *              Index               - Which Local or Arg to set
     *              ObjDesc             - Value to be stored
     *              WalkState           - Current walk state
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Store a value in an Arg or Local. The ObjDesc is installed
     *              as the new value for the Arg or Local and the reference
     *count for ObjDesc is incremented.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsStoreObjectToLocal(UINT8 Type, UINT32 Index,
                             ACPI_OPERAND_OBJECT * ObjDesc,
                             ACPI_WALK_STATE * WalkState) {
      ACPI_STATUS Status;
      ACPI_NAMESPACE_NODE *Node;
      ACPI_OPERAND_OBJECT *CurrentObjDesc;
      ACPI_OPERAND_OBJECT *NewObjDesc;

      ACPI_FUNCTION_TRACE(DsStoreObjectToLocal);
      ACPI_DEBUG_PRINT(
          (ACPI_DB_EXEC, "Type=%2.2X Index=%u Obj=%p\n", Type, Index, ObjDesc));

      /* Parameter validation */

      if (!ObjDesc) {
        return_ACPI_STATUS(AE_BAD_PARAMETER);
      }

      /* Get the namespace node for the arg/local */

      Status = AcpiDsMethodDataGetNode(Type, Index, WalkState, &Node);
      if (ACPI_FAILURE(Status)) {
        return_ACPI_STATUS(Status);
      }

      CurrentObjDesc = AcpiNsGetAttachedObject(Node);
      if (CurrentObjDesc == ObjDesc) {
        ACPI_DEBUG_PRINT(
            (ACPI_DB_EXEC, "Obj=%p already installed!\n", ObjDesc));
        return_ACPI_STATUS(Status);
      }

      /*
       * If the reference count on the object is more than one, we must
       * take a copy of the object before we store. A reference count
       * of exactly 1 means that the object was just created during the
       * evaluation of an expression, and we can safely use it since it
       * is not used anywhere else.
       */
      NewObjDesc = ObjDesc;
      if (ObjDesc->Common.ReferenceCount > 1) {
        Status = AcpiUtCopyIobjectToIobject(ObjDesc, &NewObjDesc, WalkState);
        if (ACPI_FAILURE(Status)) {
          return_ACPI_STATUS(Status);
        }
      }

      /*
       * If there is an object already in this slot, we either
       * have to delete it, or if this is an argument and there
       * is an object reference stored there, we have to do
       * an indirect store!
       */
      if (CurrentObjDesc) {
        /*
         * Check for an indirect store if an argument
         * contains an object reference (stored as an Node).
         * We don't allow this automatic dereferencing for
         * locals, since a store to a local should overwrite
         * anything there, including an object reference.
         *
         * If both Arg0 and Local0 contain RefOf (Local4):
         *
         * Store (1, Arg0)             - Causes indirect store to local4
         * Store (1, Local0)           - Stores 1 in local0, overwriting
         *                                  the reference to local4
         * Store (1, DeRefof (Local0)) - Causes indirect store to local4
         *
         * Weird, but true.
         */
        if (Type == ACPI_REFCLASS_ARG) {
          /*
           * If we have a valid reference object that came from RefOf(),
           * do the indirect store
           */
          if ((ACPI_GET_DESCRIPTOR_TYPE(CurrentObjDesc) ==
               ACPI_DESC_TYPE_OPERAND) &&
              (CurrentObjDesc->Common.Type == ACPI_TYPE_LOCAL_REFERENCE) &&
              (CurrentObjDesc->Reference.Class == ACPI_REFCLASS_REFOF)) {
            ACPI_DEBUG_PRINT(
                (ACPI_DB_EXEC,
                 "Arg (%p) is an ObjRef(Node), storing in node %p\n",
                 NewObjDesc, CurrentObjDesc));

            /*
             * Store this object to the Node (perform the indirect store)
             * NOTE: No implicit conversion is performed, as per the ACPI
             * specification rules on storing to Locals/Args.
             */
            Status = AcpiExStoreObjectToNode(
                NewObjDesc, CurrentObjDesc->Reference.Object, WalkState,
                ACPI_NO_IMPLICIT_CONVERSION);

            /* Remove local reference if we copied the object above */

            if (NewObjDesc != ObjDesc) {
              AcpiUtRemoveReference(NewObjDesc);
            }
            return_ACPI_STATUS(Status);
          }
        }

        /* Delete the existing object before storing the new one */

        AcpiDsMethodDataDeleteValue(Type, Index, WalkState);
      }

      /*
       * Install the Obj descriptor (*NewObjDesc) into
       * the descriptor for the Arg or Local.
       * (increments the object reference count by one)
       */
      Status = AcpiDsMethodDataSetValue(Type, Index, NewObjDesc, WalkState);

      /* Remove local reference if we copied the object above */

      if (NewObjDesc != ObjDesc) {
        AcpiUtRemoveReference(NewObjDesc);
      }

      return_ACPI_STATUS(Status);
    }

#ifdef ACPI_OBSOLETE_FUNCTIONS
    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsMethodDataGetType
     *
     * PARAMETERS:  Opcode              - Either AML_LOCAL_OP or AML_ARG_OP
     *              Index               - Which Local or Arg whose type to get
     *              WalkState           - Current walk state object
     *
     * RETURN:      Data type of current value of the selected Arg or Local
     *
     * DESCRIPTION: Get the type of the object stored in the Local or Arg
     *
     ******************************************************************************/

    ACPI_OBJECT_TYPE
    AcpiDsMethodDataGetType(UINT16 Opcode, UINT32 Index,
                            ACPI_WALK_STATE * WalkState) {
      ACPI_STATUS Status;
      ACPI_NAMESPACE_NODE *Node;
      ACPI_OPERAND_OBJECT *Object;

      ACPI_FUNCTION_TRACE(DsMethodDataGetType);

      /* Get the namespace node for the arg/local */

      Status = AcpiDsMethodDataGetNode(Opcode, Index, WalkState, &Node);
      if (ACPI_FAILURE(Status)) {
        return_VALUE((ACPI_TYPE_NOT_FOUND));
      }

      /* Get the object */

      Object = AcpiNsGetAttachedObject(Node);
      if (!Object) {
        /* Uninitialized local/arg, return TYPE_ANY */

        return_VALUE(ACPI_TYPE_ANY);
      }

      /* Get the object type */

      return_VALUE(Object->Type);
    }
#endif

    /**
     * @section minix_drivers_power_acpi_dispatcher_dsobject_c
     * @brief Original file: minix/drivers/power/acpi/dispatcher/dsobject.c
     */
    /******************************************************************************
     *
     * Module Name: dsobject - Dispatcher object management routines
     *
     *****************************************************************************/

    /*
     * Copyright (C) 2000 - 2014, Intel Corp.
     * All rights reserved.
     *
     * Redistribution and use in source and binary forms, with or without
     * modification, are permitted provided that the following conditions
     * are met:
     * 1. Redistributions of source code must retain the above copyright
     *    notice, this list of conditions, and the following disclaimer,
     *    without modification.
     * 2. Redistributions in binary form must reproduce at minimum a disclaimer
     *    substantially similar to the "NO WARRANTY" disclaimer below
     *    ("Disclaimer") and any redistribution must be conditioned upon
     *    including a substantially similar Disclaimer requirement for further
     *    binary redistribution.
     * 3. Neither the names of the above-listed copyright holders nor the names
     *    of any contributors may be used to endorse or promote products derived
     *    from this software without specific prior written permission.
     *
     * Alternatively, this software may be distributed under the terms of the
     * GNU General Public License ("GPL") version 2 as published by the Free
     * Software Foundation.
     *
     * NO WARRANTY
     * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
     * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
     * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR
     * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
     * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR
     * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
     * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
     * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
     * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
     * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
     * THE POSSIBILITY OF SUCH DAMAGES.
     */

#include "accommon.h"
#include "acdispat.h"
#include "acinterp.h"
#include "acnamesp.h"
#include "acparser.h"
#include "acpi.h"
#include "amlcode.h"

#define _COMPONENT ACPI_DISPATCHER
    ACPI_MODULE_NAME("dsobject")

    /* Local prototypes */

    static ACPI_STATUS AcpiDsBuildInternalObject(
        ACPI_WALK_STATE * WalkState, ACPI_PARSE_OBJECT * Op,
        ACPI_OPERAND_OBJECT * *ObjDescPtr);

#ifndef ACPI_NO_METHOD_EXECUTION
    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsBuildInternalObject
     *
     * PARAMETERS:  WalkState       - Current walk state
     *              Op              - Parser object to be translated
     *              ObjDescPtr      - Where the ACPI internal object is returned
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Translate a parser Op object to the equivalent namespace
     *object Simple objects are any objects other than a package object!
     *
     ******************************************************************************/

    static ACPI_STATUS AcpiDsBuildInternalObject(
        ACPI_WALK_STATE * WalkState, ACPI_PARSE_OBJECT * Op,
        ACPI_OPERAND_OBJECT * *ObjDescPtr) {
      ACPI_OPERAND_OBJECT *ObjDesc;
      ACPI_STATUS Status;
      ACPI_OBJECT_TYPE Type;

      ACPI_FUNCTION_TRACE(DsBuildInternalObject);

      *ObjDescPtr = NULL;
      if (Op->Common.AmlOpcode == AML_INT_NAMEPATH_OP) {
        /*
         * This is a named object reference. If this name was
         * previously looked up in the namespace, it was stored in this op.
         * Otherwise, go ahead and look it up now
         */
        if (!Op->Common.Node) {
          Status = AcpiNsLookup(
              WalkState->ScopeInfo, Op->Common.Value.String, ACPI_TYPE_ANY,
              ACPI_IMODE_EXECUTE,
              ACPI_NS_SEARCH_PARENT | ACPI_NS_DONT_OPEN_SCOPE, NULL,
              ACPI_CAST_INDIRECT_PTR(ACPI_NAMESPACE_NODE, &(Op->Common.Node)));
          if (ACPI_FAILURE(Status)) {
            /* Check if we are resolving a named reference within a package */

            if ((Status == AE_NOT_FOUND) && (AcpiGbl_EnableInterpreterSlack) &&

                ((Op->Common.Parent->Common.AmlOpcode == AML_PACKAGE_OP) ||
                 (Op->Common.Parent->Common.AmlOpcode == AML_VAR_PACKAGE_OP))) {
              /*
               * We didn't find the target and we are populating elements
               * of a package - ignore if slack enabled. Some ASL code
               * contains dangling invalid references in packages and
               * expects that no exception will be issued. Leave the
               * element as a null element. It cannot be used, but it
               * can be overwritten by subsequent ASL code - this is
               * typically the case.
               */
              ACPI_DEBUG_PRINT(
                  (ACPI_DB_INFO,
                   "Ignoring unresolved reference in package [%4.4s]\n",
                   WalkState->ScopeInfo->Scope.Node->Name.Ascii));

              return_ACPI_STATUS(AE_OK);
            } else {
              ACPI_ERROR_NAMESPACE(Op->Common.Value.String, Status);
            }

            return_ACPI_STATUS(Status);
          }
        }

        /* Special object resolution for elements of a package */

        if ((Op->Common.Parent->Common.AmlOpcode == AML_PACKAGE_OP) ||
            (Op->Common.Parent->Common.AmlOpcode == AML_VAR_PACKAGE_OP)) {
          /*
           * Attempt to resolve the node to a value before we insert it into
           * the package. If this is a reference to a common data type,
           * resolve it immediately. According to the ACPI spec, package
           * elements can only be "data objects" or method references.
           * Attempt to resolve to an Integer, Buffer, String or Package.
           * If cannot, return the named reference (for things like Devices,
           * Methods, etc.) Buffer Fields and Fields will resolve to simple
           * objects (int/buf/str/pkg).
           *
           * NOTE: References to things like Devices, Methods, Mutexes, etc.
           * will remain as named references. This behavior is not described
           * in the ACPI spec, but it appears to be an oversight.
           */
          ObjDesc = ACPI_CAST_PTR(ACPI_OPERAND_OBJECT, Op->Common.Node);

          Status = AcpiExResolveNodeToValue(
              ACPI_CAST_INDIRECT_PTR(ACPI_NAMESPACE_NODE, &ObjDesc), WalkState);
          if (ACPI_FAILURE(Status)) {
            return_ACPI_STATUS(Status);
          }

          /*
           * Special handling for Alias objects. We need to setup the type
           * and the Op->Common.Node to point to the Alias target. Note,
           * Alias has at most one level of indirection internally.
           */
          Type = Op->Common.Node->Type;
          if (Type == ACPI_TYPE_LOCAL_ALIAS) {
            Type = ObjDesc->Common.Type;
            Op->Common.Node =
                ACPI_CAST_PTR(ACPI_NAMESPACE_NODE, Op->Common.Node->Object);
          }

          switch (Type) {
          /*
           * For these types, we need the actual node, not the subobject.
           * However, the subobject did not get an extra reference count above.
           *
           * TBD: should ExResolveNodeToValue be changed to fix this?
           */
          case ACPI_TYPE_DEVICE:
          case ACPI_TYPE_THERMAL:

            AcpiUtAddReference(Op->Common.Node->Object);

            /*lint -fallthrough */
          /*
           * For these types, we need the actual node, not the subobject.
           * The subobject got an extra reference count in ExResolveNodeToValue.
           */
          case ACPI_TYPE_MUTEX:
          case ACPI_TYPE_METHOD:
          case ACPI_TYPE_POWER:
          case ACPI_TYPE_PROCESSOR:
          case ACPI_TYPE_EVENT:
          case ACPI_TYPE_REGION:

            /* We will create a reference object for these types below */
            break;

          default:
            /*
             * All other types - the node was resolved to an actual
             * object, we are done.
             */
            goto Exit;
          }
        }
      }

      /* Create and init a new internal ACPI object */

      ObjDesc = AcpiUtCreateInternalObject(
          (AcpiPsGetOpcodeInfo(Op->Common.AmlOpcode))->ObjectType);
      if (!ObjDesc) {
        return_ACPI_STATUS(AE_NO_MEMORY);
      }

      Status =
          AcpiDsInitObjectFromOp(WalkState, Op, Op->Common.AmlOpcode, &ObjDesc);
      if (ACPI_FAILURE(Status)) {
        AcpiUtRemoveReference(ObjDesc);
        return_ACPI_STATUS(Status);
      }

    Exit:
      *ObjDescPtr = ObjDesc;
      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsBuildInternalBufferObj
     *
     * PARAMETERS:  WalkState       - Current walk state
     *              Op              - Parser object to be translated
     *              BufferLength    - Length of the buffer
     *              ObjDescPtr      - Where the ACPI internal object is returned
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Translate a parser Op package object to the equivalent
     *              namespace object
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsBuildInternalBufferObj(ACPI_WALK_STATE * WalkState,
                                 ACPI_PARSE_OBJECT * Op, UINT32 BufferLength,
                                 ACPI_OPERAND_OBJECT * *ObjDescPtr) {
      ACPI_PARSE_OBJECT *Arg;
      ACPI_OPERAND_OBJECT *ObjDesc;
      ACPI_PARSE_OBJECT *ByteList;
      UINT32 ByteListLength = 0;

      ACPI_FUNCTION_TRACE(DsBuildInternalBufferObj);

      /*
       * If we are evaluating a Named buffer object "Name (xxxx, Buffer)".
       * The buffer object already exists (from the NS node), otherwise it must
       * be created.
       */
      ObjDesc = *ObjDescPtr;
      if (!ObjDesc) {
        /* Create a new buffer object */

        ObjDesc = AcpiUtCreateInternalObject(ACPI_TYPE_BUFFER);
        *ObjDescPtr = ObjDesc;
        if (!ObjDesc) {
          return_ACPI_STATUS(AE_NO_MEMORY);
        }
      }

      /*
       * Second arg is the buffer data (optional) ByteList can be either
       * individual bytes or a string initializer. In either case, a
       * ByteList appears in the AML.
       */
      Arg = Op->Common.Value.Arg; /* skip first arg */

      ByteList = Arg->Named.Next;
      if (ByteList) {
        if (ByteList->Common.AmlOpcode != AML_INT_BYTELIST_OP) {
          ACPI_ERROR((AE_INFO,
                      "Expecting bytelist, found AML opcode 0x%X in op %p",
                      ByteList->Common.AmlOpcode, ByteList));

          AcpiUtRemoveReference(ObjDesc);
          return (AE_TYPE);
        }

        ByteListLength = (UINT32)ByteList->Common.Value.Integer;
      }

      /*
       * The buffer length (number of bytes) will be the larger of:
       * 1) The specified buffer length and
       * 2) The length of the initializer byte list
       */
      ObjDesc->Buffer.Length = BufferLength;
      if (ByteListLength > BufferLength) {
        ObjDesc->Buffer.Length = ByteListLength;
      }

      /* Allocate the buffer */

      if (ObjDesc->Buffer.Length == 0) {
        ObjDesc->Buffer.Pointer = NULL;
        ACPI_DEBUG_PRINT(
            (ACPI_DB_EXEC,
             "Buffer defined with zero length in AML, creating\n"));
      } else {
        ObjDesc->Buffer.Pointer = ACPI_ALLOCATE_ZEROED(ObjDesc->Buffer.Length);
        if (!ObjDesc->Buffer.Pointer) {
          AcpiUtDeleteObjectDesc(ObjDesc);
          return_ACPI_STATUS(AE_NO_MEMORY);
        }

        /* Initialize buffer from the ByteList (if present) */

        if (ByteList) {
          ACPI_MEMCPY(ObjDesc->Buffer.Pointer, ByteList->Named.Data,
                      ByteListLength);
        }
      }

      ObjDesc->Buffer.Flags |= AOPOBJ_DATA_VALID;
      Op->Common.Node = ACPI_CAST_PTR(ACPI_NAMESPACE_NODE, ObjDesc);
      return_ACPI_STATUS(AE_OK);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsBuildInternalPackageObj
     *
     * PARAMETERS:  WalkState       - Current walk state
     *              Op              - Parser object to be translated
     *              ElementCount    - Number of elements in the package - this
     *is the NumElements argument to Package() ObjDescPtr      - Where the ACPI
     *internal object is returned
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Translate a parser Op package object to the equivalent
     *              namespace object
     *
     * NOTE: The number of elements in the package will be always be the
     *NumElements count, regardless of the number of elements in the package
     *list. If NumElements is smaller, only that many package list elements are
     *used. if NumElements is larger, the Package object is padded out with
     * objects of type Uninitialized (as per ACPI spec.)
     *
     * Even though the ASL compilers do not allow NumElements to be smaller
     * than the Package list length (for the fixed length package opcode), some
     * BIOS code modifies the AML on the fly to adjust the NumElements, and
     * this code compensates for that. This also provides compatibility with
     * other AML interpreters.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsBuildInternalPackageObj(ACPI_WALK_STATE * WalkState,
                                  ACPI_PARSE_OBJECT * Op, UINT32 ElementCount,
                                  ACPI_OPERAND_OBJECT * *ObjDescPtr) {
      ACPI_PARSE_OBJECT *Arg;
      ACPI_PARSE_OBJECT *Parent;
      ACPI_OPERAND_OBJECT *ObjDesc = NULL;
      ACPI_STATUS Status = AE_OK;
      UINT32 i;
      UINT16 Index;
      UINT16 ReferenceCount;

      ACPI_FUNCTION_TRACE(DsBuildInternalPackageObj);

      /* Find the parent of a possibly nested package */

      Parent = Op->Common.Parent;
      while ((Parent->Common.AmlOpcode == AML_PACKAGE_OP) ||
             (Parent->Common.AmlOpcode == AML_VAR_PACKAGE_OP)) {
        Parent = Parent->Common.Parent;
      }

      /*
       * If we are evaluating a Named package object "Name (xxxx, Package)",
       * the package object already exists, otherwise it must be created.
       */
      ObjDesc = *ObjDescPtr;
      if (!ObjDesc) {
        ObjDesc = AcpiUtCreateInternalObject(ACPI_TYPE_PACKAGE);
        *ObjDescPtr = ObjDesc;
        if (!ObjDesc) {
          return_ACPI_STATUS(AE_NO_MEMORY);
        }

        ObjDesc->Package.Node = Parent->Common.Node;
      }

      /*
       * Allocate the element array (array of pointers to the individual
       * objects) based on the NumElements parameter. Add an extra pointer slot
       * so that the list is always null terminated.
       */
      ObjDesc->Package.Elements =
          ACPI_ALLOCATE_ZEROED(((ACPI_SIZE)ElementCount + 1) * sizeof(void *));

      if (!ObjDesc->Package.Elements) {
        AcpiUtDeleteObjectDesc(ObjDesc);
        return_ACPI_STATUS(AE_NO_MEMORY);
      }

      ObjDesc->Package.Count = ElementCount;

      /*
       * Initialize the elements of the package, up to the NumElements count.
       * Package is automatically padded with uninitialized (NULL) elements
       * if NumElements is greater than the package list length. Likewise,
       * Package is truncated if NumElements is less than the list length.
       */
      Arg = Op->Common.Value.Arg;
      Arg = Arg->Common.Next;
      for (i = 0; Arg && (i < ElementCount); i++) {
        if (Arg->Common.AmlOpcode == AML_INT_RETURN_VALUE_OP) {
          if (Arg->Common.Node->Type == ACPI_TYPE_METHOD) {
            /*
             * A method reference "looks" to the parser to be a method
             * invocation, so we special case it here
             */
            Arg->Common.AmlOpcode = AML_INT_NAMEPATH_OP;
            Status = AcpiDsBuildInternalObject(WalkState, Arg,
                                               &ObjDesc->Package.Elements[i]);
          } else {
            /* This package element is already built, just get it */

            ObjDesc->Package.Elements[i] =
                ACPI_CAST_PTR(ACPI_OPERAND_OBJECT, Arg->Common.Node);
          }
        } else {
          Status = AcpiDsBuildInternalObject(WalkState, Arg,
                                             &ObjDesc->Package.Elements[i]);
        }

        if (*ObjDescPtr) {
          /* Existing package, get existing reference count */

          ReferenceCount = (*ObjDescPtr)->Common.ReferenceCount;
          if (ReferenceCount > 1) {
            /* Make new element ref count match original ref count */

            for (Index = 0; Index < (ReferenceCount - 1); Index++) {
              AcpiUtAddReference((ObjDesc->Package.Elements[i]));
            }
          }
        }

        Arg = Arg->Common.Next;
      }

      /* Check for match between NumElements and actual length of PackageList */

      if (Arg) {
        /*
         * NumElements was exhausted, but there are remaining elements in the
         * PackageList. Truncate the package to NumElements.
         *
         * Note: technically, this is an error, from ACPI spec: "It is an error
         * for NumElements to be less than the number of elements in the
         * PackageList". However, we just print a message and
         * no exception is returned. This provides Windows compatibility. Some
         * BIOSs will alter the NumElements on the fly, creating this type
         * of ill-formed package object.
         */
        while (Arg) {
          /*
           * We must delete any package elements that were created earlier
           * and are not going to be used because of the package truncation.
           */
          if (Arg->Common.Node) {
            AcpiUtRemoveReference(
                ACPI_CAST_PTR(ACPI_OPERAND_OBJECT, Arg->Common.Node));
            Arg->Common.Node = NULL;
          }

          /* Find out how many elements there really are */

          i++;
          Arg = Arg->Common.Next;
        }

        ACPI_INFO((AE_INFO,
                   "Actual Package length (%u) is larger than NumElements "
                   "field (%u), truncated",
                   i, ElementCount));
      } else if (i < ElementCount) {
        /*
         * Arg list (elements) was exhausted, but we did not reach NumElements
         * count. Note: this is not an error, the package is padded out with
         * NULLs.
         */
        ACPI_DEBUG_PRINT((ACPI_DB_INFO,
                          "Package List length (%u) smaller than NumElements "
                          "count (%u), padded with null elements\n",
                          i, ElementCount));
      }

      ObjDesc->Package.Flags |= AOPOBJ_DATA_VALID;
      Op->Common.Node = ACPI_CAST_PTR(ACPI_NAMESPACE_NODE, ObjDesc);
      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsCreateNode
     *
     * PARAMETERS:  WalkState       - Current walk state
     *              Node            - NS Node to be initialized
     *              Op              - Parser object to be translated
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Create the object to be associated with a namespace node
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsCreateNode(ACPI_WALK_STATE * WalkState, ACPI_NAMESPACE_NODE * Node,
                     ACPI_PARSE_OBJECT * Op) {
      ACPI_STATUS Status;
      ACPI_OPERAND_OBJECT *ObjDesc;

      ACPI_FUNCTION_TRACE_PTR(DsCreateNode, Op);

      /*
       * Because of the execution pass through the non-control-method
       * parts of the table, we can arrive here twice. Only init
       * the named object node the first time through
       */
      if (AcpiNsGetAttachedObject(Node)) {
        return_ACPI_STATUS(AE_OK);
      }

      if (!Op->Common.Value.Arg) {
        /* No arguments, there is nothing to do */

        return_ACPI_STATUS(AE_OK);
      }

      /* Build an internal object for the argument(s) */

      Status =
          AcpiDsBuildInternalObject(WalkState, Op->Common.Value.Arg, &ObjDesc);
      if (ACPI_FAILURE(Status)) {
        return_ACPI_STATUS(Status);
      }

      /* Re-type the object according to its argument */

      Node->Type = ObjDesc->Common.Type;

      /* Attach obj to node */

      Status = AcpiNsAttachObject(Node, ObjDesc, Node->Type);

      /* Remove local reference to the object */

      AcpiUtRemoveReference(ObjDesc);
      return_ACPI_STATUS(Status);
    }

#endif /* ACPI_NO_METHOD_EXECUTION */

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsInitObjectFromOp
     *
     * PARAMETERS:  WalkState       - Current walk state
     *              Op              - Parser op used to init the internal object
     *              Opcode          - AML opcode associated with the object
     *              RetObjDesc      - Namespace object to be initialized
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Initialize a namespace object from a parser Op and its
     *              associated arguments. The namespace object is a more compact
     *              representation of the Op and its arguments.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsInitObjectFromOp(ACPI_WALK_STATE * WalkState, ACPI_PARSE_OBJECT * Op,
                           UINT16 Opcode, ACPI_OPERAND_OBJECT * *RetObjDesc) {
      const ACPI_OPCODE_INFO *OpInfo;
      ACPI_OPERAND_OBJECT *ObjDesc;
      ACPI_STATUS Status = AE_OK;

      ACPI_FUNCTION_TRACE(DsInitObjectFromOp);

      ObjDesc = *RetObjDesc;
      OpInfo = AcpiPsGetOpcodeInfo(Opcode);
      if (OpInfo->Class == AML_CLASS_UNKNOWN) {
        /* Unknown opcode */

        return_ACPI_STATUS(AE_TYPE);
      }

      /* Perform per-object initialization */

      switch (ObjDesc->Common.Type) {
      case ACPI_TYPE_BUFFER:
        /*
         * Defer evaluation of Buffer TermArg operand
         */
        ObjDesc->Buffer.Node =
            ACPI_CAST_PTR(ACPI_NAMESPACE_NODE, WalkState->Operands[0]);
        ObjDesc->Buffer.AmlStart = Op->Named.Data;
        ObjDesc->Buffer.AmlLength = Op->Named.Length;
        break;

      case ACPI_TYPE_PACKAGE:
        /*
         * Defer evaluation of Package TermArg operand
         */
        ObjDesc->Package.Node =
            ACPI_CAST_PTR(ACPI_NAMESPACE_NODE, WalkState->Operands[0]);
        ObjDesc->Package.AmlStart = Op->Named.Data;
        ObjDesc->Package.AmlLength = Op->Named.Length;
        break;

      case ACPI_TYPE_INTEGER:

        switch (OpInfo->Type) {
        case AML_TYPE_CONSTANT:
          /*
           * Resolve AML Constants here - AND ONLY HERE!
           * All constants are integers.
           * We mark the integer with a flag that indicates that it started
           * life as a constant -- so that stores to constants will perform
           * as expected (noop). ZeroOp is used as a placeholder for optional
           * target operands.
           */
          ObjDesc->Common.Flags = AOPOBJ_AML_CONSTANT;

          switch (Opcode) {
          case AML_ZERO_OP:

            ObjDesc->Integer.Value = 0;
            break;

          case AML_ONE_OP:

            ObjDesc->Integer.Value = 1;
            break;

          case AML_ONES_OP:

            ObjDesc->Integer.Value = ACPI_UINT64_MAX;

            /* Truncate value if we are executing from a 32-bit ACPI table */

#ifndef ACPI_NO_METHOD_EXECUTION
            (void)AcpiExTruncateFor32bitTable(ObjDesc);
#endif
            break;

          case AML_REVISION_OP:

            ObjDesc->Integer.Value = ACPI_CA_VERSION;
            break;

          default:

            ACPI_ERROR((AE_INFO, "Unknown constant opcode 0x%X", Opcode));
            Status = AE_AML_OPERAND_TYPE;
            break;
          }
          break;

        case AML_TYPE_LITERAL:

          ObjDesc->Integer.Value = Op->Common.Value.Integer;

#ifndef ACPI_NO_METHOD_EXECUTION
          if (AcpiExTruncateFor32bitTable(ObjDesc)) {
            /* Warn if we found a 64-bit constant in a 32-bit table */

            ACPI_WARNING((AE_INFO,
                          "Truncated 64-bit constant found in 32-bit table: "
                          "%8.8X%8.8X => %8.8X",
                          ACPI_FORMAT_UINT64(Op->Common.Value.Integer),
                          (UINT32)ObjDesc->Integer.Value));
          }
#endif
          break;

        default:

          ACPI_ERROR((AE_INFO, "Unknown Integer type 0x%X", OpInfo->Type));
          Status = AE_AML_OPERAND_TYPE;
          break;
        }
        break;

      case ACPI_TYPE_STRING:

        ObjDesc->String.Pointer = Op->Common.Value.String;
        ObjDesc->String.Length = (UINT32)ACPI_STRLEN(Op->Common.Value.String);

        /*
         * The string is contained in the ACPI table, don't ever try
         * to delete it
         */
        ObjDesc->Common.Flags |= AOPOBJ_STATIC_POINTER;
        break;

      case ACPI_TYPE_METHOD:
        break;

      case ACPI_TYPE_LOCAL_REFERENCE:

        switch (OpInfo->Type) {
        case AML_TYPE_LOCAL_VARIABLE:

          /* Local ID (0-7) is (AML opcode - base AML_LOCAL_OP) */

          ObjDesc->Reference.Value = ((UINT32)Opcode) - AML_LOCAL_OP;
          ObjDesc->Reference.Class = ACPI_REFCLASS_LOCAL;

#ifndef ACPI_NO_METHOD_EXECUTION
          Status = AcpiDsMethodDataGetNode(
              ACPI_REFCLASS_LOCAL, ObjDesc->Reference.Value, WalkState,
              ACPI_CAST_INDIRECT_PTR(ACPI_NAMESPACE_NODE,
                                     &ObjDesc->Reference.Object));
#endif
          break;

        case AML_TYPE_METHOD_ARGUMENT:

          /* Arg ID (0-6) is (AML opcode - base AML_ARG_OP) */

          ObjDesc->Reference.Value = ((UINT32)Opcode) - AML_ARG_OP;
          ObjDesc->Reference.Class = ACPI_REFCLASS_ARG;

#ifndef ACPI_NO_METHOD_EXECUTION
          Status = AcpiDsMethodDataGetNode(
              ACPI_REFCLASS_ARG, ObjDesc->Reference.Value, WalkState,
              ACPI_CAST_INDIRECT_PTR(ACPI_NAMESPACE_NODE,
                                     &ObjDesc->Reference.Object));
#endif
          break;

        default: /* Object name or Debug object */

          switch (Op->Common.AmlOpcode) {
          case AML_INT_NAMEPATH_OP:

            /* Node was saved in Op */

            ObjDesc->Reference.Node = Op->Common.Node;
            ObjDesc->Reference.Object = Op->Common.Node->Object;
            ObjDesc->Reference.Class = ACPI_REFCLASS_NAME;
            break;

          case AML_DEBUG_OP:

            ObjDesc->Reference.Class = ACPI_REFCLASS_DEBUG;
            break;

          default:

            ACPI_ERROR((AE_INFO,
                        "Unimplemented reference type for AML opcode: 0x%4.4X",
                        Opcode));
            return_ACPI_STATUS(AE_AML_OPERAND_TYPE);
          }
          break;
        }
        break;

      default:

        ACPI_ERROR(
            (AE_INFO, "Unimplemented data type: 0x%X", ObjDesc->Common.Type));

        Status = AE_AML_OPERAND_TYPE;
        break;
      }

      return_ACPI_STATUS(Status);
    }

    /**
     * @section minix_drivers_power_acpi_dispatcher_dsopcode_c
     * @brief Original file: minix/drivers/power/acpi/dispatcher/dsopcode.c
     */
    /******************************************************************************
     *
     * Module Name: dsopcode - Dispatcher support for regions and fields
     *
     *****************************************************************************/

    /*
     * Copyright (C) 2000 - 2014, Intel Corp.
     * All rights reserved.
     *
     * Redistribution and use in source and binary forms, with or without
     * modification, are permitted provided that the following conditions
     * are met:
     * 1. Redistributions of source code must retain the above copyright
     *    notice, this list of conditions, and the following disclaimer,
     *    without modification.
     * 2. Redistributions in binary form must reproduce at minimum a disclaimer
     *    substantially similar to the "NO WARRANTY" disclaimer below
     *    ("Disclaimer") and any redistribution must be conditioned upon
     *    including a substantially similar Disclaimer requirement for further
     *    binary redistribution.
     * 3. Neither the names of the above-listed copyright holders nor the names
     *    of any contributors may be used to endorse or promote products derived
     *    from this software without specific prior written permission.
     *
     * Alternatively, this software may be distributed under the terms of the
     * GNU General Public License ("GPL") version 2 as published by the Free
     * Software Foundation.
     *
     * NO WARRANTY
     * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
     * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
     * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR
     * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
     * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR
     * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
     * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
     * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
     * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
     * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
     * THE POSSIBILITY OF SUCH DAMAGES.
     */

#include "accommon.h"
#include "acdispat.h"
#include "acevents.h"
#include "acinterp.h"
#include "acnamesp.h"
#include "acparser.h"
#include "acpi.h"
#include "actables.h"
#include "amlcode.h"

#define _COMPONENT ACPI_DISPATCHER
    ACPI_MODULE_NAME("dsopcode")

    /* Local prototypes */

    static ACPI_STATUS AcpiDsInitBufferField(
        UINT16 AmlOpcode, ACPI_OPERAND_OBJECT * ObjDesc,
        ACPI_OPERAND_OBJECT * BufferDesc, ACPI_OPERAND_OBJECT * OffsetDesc,
        ACPI_OPERAND_OBJECT * LengthDesc, ACPI_OPERAND_OBJECT * ResultDesc);

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsInitializeRegion
     *
     * PARAMETERS:  ObjHandle       - Region namespace node
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Front end to EvInitializeRegion
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsInitializeRegion(ACPI_HANDLE ObjHandle) {
      ACPI_OPERAND_OBJECT *ObjDesc;
      ACPI_STATUS Status;

      ObjDesc = AcpiNsGetAttachedObject(ObjHandle);

      /* Namespace is NOT locked */

      Status = AcpiEvInitializeRegion(ObjDesc, FALSE);
      return (Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsInitBufferField
     *
     * PARAMETERS:  AmlOpcode       - CreateXxxField
     *              ObjDesc         - BufferField object
     *              BufferDesc      - Host Buffer
     *              OffsetDesc      - Offset into buffer
     *              LengthDesc      - Length of field (CREATE_FIELD_OP only)
     *              ResultDesc      - Where to store the result
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Perform actual initialization of a buffer field
     *
     ******************************************************************************/

    static ACPI_STATUS AcpiDsInitBufferField(
        UINT16 AmlOpcode, ACPI_OPERAND_OBJECT * ObjDesc,
        ACPI_OPERAND_OBJECT * BufferDesc, ACPI_OPERAND_OBJECT * OffsetDesc,
        ACPI_OPERAND_OBJECT * LengthDesc, ACPI_OPERAND_OBJECT * ResultDesc) {
      UINT32 Offset;
      UINT32 BitOffset;
      UINT32 BitCount;
      UINT8 FieldFlags;
      ACPI_STATUS Status;

      ACPI_FUNCTION_TRACE_PTR(DsInitBufferField, ObjDesc);

      /* Host object must be a Buffer */

      if (BufferDesc->Common.Type != ACPI_TYPE_BUFFER) {
        ACPI_ERROR((AE_INFO,
                    "Target of Create Field is not a Buffer object - %s",
                    AcpiUtGetObjectTypeName(BufferDesc)));

        Status = AE_AML_OPERAND_TYPE;
        goto Cleanup;
      }

      /*
       * The last parameter to all of these opcodes (ResultDesc) started
       * out as a NameString, and should therefore now be a NS node
       * after resolution in AcpiExResolveOperands().
       */
      if (ACPI_GET_DESCRIPTOR_TYPE(ResultDesc) != ACPI_DESC_TYPE_NAMED) {
        ACPI_ERROR((AE_INFO, "(%s) destination not a NS Node [%s]",
                    AcpiPsGetOpcodeName(AmlOpcode),
                    AcpiUtGetDescriptorName(ResultDesc)));

        Status = AE_AML_OPERAND_TYPE;
        goto Cleanup;
      }

      Offset = (UINT32)OffsetDesc->Integer.Value;

      /*
       * Setup the Bit offsets and counts, according to the opcode
       */
      switch (AmlOpcode) {
      case AML_CREATE_FIELD_OP:

        /* Offset is in bits, count is in bits */

        FieldFlags = AML_FIELD_ACCESS_BYTE;
        BitOffset = Offset;
        BitCount = (UINT32)LengthDesc->Integer.Value;

        /* Must have a valid (>0) bit count */

        if (BitCount == 0) {
          ACPI_ERROR((AE_INFO, "Attempt to CreateField of length zero"));
          Status = AE_AML_OPERAND_VALUE;
          goto Cleanup;
        }
        break;

      case AML_CREATE_BIT_FIELD_OP:

        /* Offset is in bits, Field is one bit */

        BitOffset = Offset;
        BitCount = 1;
        FieldFlags = AML_FIELD_ACCESS_BYTE;
        break;

      case AML_CREATE_BYTE_FIELD_OP:

        /* Offset is in bytes, field is one byte */

        BitOffset = 8 * Offset;
        BitCount = 8;
        FieldFlags = AML_FIELD_ACCESS_BYTE;
        break;

      case AML_CREATE_WORD_FIELD_OP:

        /* Offset is in bytes, field is one word */

        BitOffset = 8 * Offset;
        BitCount = 16;
        FieldFlags = AML_FIELD_ACCESS_WORD;
        break;

      case AML_CREATE_DWORD_FIELD_OP:

        /* Offset is in bytes, field is one dword */

        BitOffset = 8 * Offset;
        BitCount = 32;
        FieldFlags = AML_FIELD_ACCESS_DWORD;
        break;

      case AML_CREATE_QWORD_FIELD_OP:

        /* Offset is in bytes, field is one qword */

        BitOffset = 8 * Offset;
        BitCount = 64;
        FieldFlags = AML_FIELD_ACCESS_QWORD;
        break;

      default:

        ACPI_ERROR(
            (AE_INFO, "Unknown field creation opcode 0x%02X", AmlOpcode));
        Status = AE_AML_BAD_OPCODE;
        goto Cleanup;
      }

      /* Entire field must fit within the current length of the buffer */

      if ((BitOffset + BitCount) > (8 * (UINT32)BufferDesc->Buffer.Length)) {
        ACPI_ERROR((AE_INFO,
                    "Field [%4.4s] at %u exceeds Buffer [%4.4s] size %u (bits)",
                    AcpiUtGetNodeName(ResultDesc), BitOffset + BitCount,
                    AcpiUtGetNodeName(BufferDesc->Buffer.Node),
                    8 * (UINT32)BufferDesc->Buffer.Length));
        Status = AE_AML_BUFFER_LIMIT;
        goto Cleanup;
      }

      /*
       * Initialize areas of the field object that are common to all fields
       * For FieldFlags, use LOCK_RULE = 0 (NO_LOCK),
       * UPDATE_RULE = 0 (UPDATE_PRESERVE)
       */
      Status = AcpiExPrepCommonFieldObject(ObjDesc, FieldFlags, 0, BitOffset,
                                           BitCount);
      if (ACPI_FAILURE(Status)) {
        goto Cleanup;
      }

      ObjDesc->BufferField.BufferObj = BufferDesc;

      /* Reference count for BufferDesc inherits ObjDesc count */

      BufferDesc->Common.ReferenceCount =
          (UINT16)(BufferDesc->Common.ReferenceCount +
                   ObjDesc->Common.ReferenceCount);

    Cleanup:

      /* Always delete the operands */

      AcpiUtRemoveReference(OffsetDesc);
      AcpiUtRemoveReference(BufferDesc);

      if (AmlOpcode == AML_CREATE_FIELD_OP) {
        AcpiUtRemoveReference(LengthDesc);
      }

      /* On failure, delete the result descriptor */

      if (ACPI_FAILURE(Status)) {
        AcpiUtRemoveReference(ResultDesc); /* Result descriptor */
      } else {
        /* Now the address and length are valid for this BufferField */

        ObjDesc->BufferField.Flags |= AOPOBJ_DATA_VALID;
      }

      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsEvalBufferFieldOperands
     *
     * PARAMETERS:  WalkState       - Current walk
     *              Op              - A valid BufferField Op object
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Get BufferField Buffer and Index
     *              Called from AcpiDsExecEndOp during BufferField parse tree
     *walk
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsEvalBufferFieldOperands(ACPI_WALK_STATE * WalkState,
                                  ACPI_PARSE_OBJECT * Op) {
      ACPI_STATUS Status;
      ACPI_OPERAND_OBJECT *ObjDesc;
      ACPI_NAMESPACE_NODE *Node;
      ACPI_PARSE_OBJECT *NextOp;

      ACPI_FUNCTION_TRACE_PTR(DsEvalBufferFieldOperands, Op);

      /*
       * This is where we evaluate the address and length fields of the
       * CreateXxxField declaration
       */
      Node = Op->Common.Node;

      /* NextOp points to the op that holds the Buffer */

      NextOp = Op->Common.Value.Arg;

      /* Evaluate/create the address and length operands */

      Status = AcpiDsCreateOperands(WalkState, NextOp);
      if (ACPI_FAILURE(Status)) {
        return_ACPI_STATUS(Status);
      }

      ObjDesc = AcpiNsGetAttachedObject(Node);
      if (!ObjDesc) {
        return_ACPI_STATUS(AE_NOT_EXIST);
      }

      /* Resolve the operands */

      Status = AcpiExResolveOperands(Op->Common.AmlOpcode, ACPI_WALK_OPERANDS,
                                     WalkState);
      if (ACPI_FAILURE(Status)) {
        ACPI_ERROR((AE_INFO, "(%s) bad operand(s), status 0x%X",
                    AcpiPsGetOpcodeName(Op->Common.AmlOpcode), Status));

        return_ACPI_STATUS(Status);
      }

      /* Initialize the Buffer Field */

      if (Op->Common.AmlOpcode == AML_CREATE_FIELD_OP) {
        /* NOTE: Slightly different operands for this opcode */

        Status = AcpiDsInitBufferField(
            Op->Common.AmlOpcode, ObjDesc, WalkState->Operands[0],
            WalkState->Operands[1], WalkState->Operands[2],
            WalkState->Operands[3]);
      } else {
        /* All other, CreateXxxField opcodes */

        Status = AcpiDsInitBufferField(
            Op->Common.AmlOpcode, ObjDesc, WalkState->Operands[0],
            WalkState->Operands[1], NULL, WalkState->Operands[2]);
      }

      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsEvalRegionOperands
     *
     * PARAMETERS:  WalkState       - Current walk
     *              Op              - A valid region Op object
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Get region address and length
     *              Called from AcpiDsExecEndOp during OpRegion parse tree walk
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsEvalRegionOperands(ACPI_WALK_STATE * WalkState,
                             ACPI_PARSE_OBJECT * Op) {
      ACPI_STATUS Status;
      ACPI_OPERAND_OBJECT *ObjDesc;
      ACPI_OPERAND_OBJECT *OperandDesc;
      ACPI_NAMESPACE_NODE *Node;
      ACPI_PARSE_OBJECT *NextOp;

      ACPI_FUNCTION_TRACE_PTR(DsEvalRegionOperands, Op);

      /*
       * This is where we evaluate the address and length fields of the
       * OpRegion declaration
       */
      Node = Op->Common.Node;

      /* NextOp points to the op that holds the SpaceID */

      NextOp = Op->Common.Value.Arg;

      /* NextOp points to address op */

      NextOp = NextOp->Common.Next;

      /* Evaluate/create the address and length operands */

      Status = AcpiDsCreateOperands(WalkState, NextOp);
      if (ACPI_FAILURE(Status)) {
        return_ACPI_STATUS(Status);
      }

      /* Resolve the length and address operands to numbers */

      Status = AcpiExResolveOperands(Op->Common.AmlOpcode, ACPI_WALK_OPERANDS,
                                     WalkState);
      if (ACPI_FAILURE(Status)) {
        return_ACPI_STATUS(Status);
      }

      ObjDesc = AcpiNsGetAttachedObject(Node);
      if (!ObjDesc) {
        return_ACPI_STATUS(AE_NOT_EXIST);
      }

      /*
       * Get the length operand and save it
       * (at Top of stack)
       */
      OperandDesc = WalkState->Operands[WalkState->NumOperands - 1];

      ObjDesc->Region.Length = (UINT32)OperandDesc->Integer.Value;
      AcpiUtRemoveReference(OperandDesc);

      /*
       * Get the address and save it
       * (at top of stack - 1)
       */
      OperandDesc = WalkState->Operands[WalkState->NumOperands - 2];

      ObjDesc->Region.Address =
          (ACPI_PHYSICAL_ADDRESS)OperandDesc->Integer.Value;
      AcpiUtRemoveReference(OperandDesc);

      ACPI_DEBUG_PRINT((ACPI_DB_EXEC, "RgnObj %p Addr %8.8X%8.8X Len %X\n",
                        ObjDesc,
                        ACPI_FORMAT_NATIVE_UINT(ObjDesc->Region.Address),
                        ObjDesc->Region.Length));

      /* Now the address and length are valid for this opregion */

      ObjDesc->Region.Flags |= AOPOBJ_DATA_VALID;

      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsEvalTableRegionOperands
     *
     * PARAMETERS:  WalkState       - Current walk
     *              Op              - A valid region Op object
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Get region address and length.
     *              Called from AcpiDsExecEndOp during DataTableRegion parse
     *              tree walk.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsEvalTableRegionOperands(ACPI_WALK_STATE * WalkState,
                                  ACPI_PARSE_OBJECT * Op) {
      ACPI_STATUS Status;
      ACPI_OPERAND_OBJECT *ObjDesc;
      ACPI_OPERAND_OBJECT **Operand;
      ACPI_NAMESPACE_NODE *Node;
      ACPI_PARSE_OBJECT *NextOp;
      UINT32 TableIndex;
      ACPI_TABLE_HEADER *Table;

      ACPI_FUNCTION_TRACE_PTR(DsEvalTableRegionOperands, Op);

      /*
       * This is where we evaluate the Signature string, OemId string,
       * and OemTableId string of the Data Table Region declaration
       */
      Node = Op->Common.Node;

      /* NextOp points to Signature string op */

      NextOp = Op->Common.Value.Arg;

      /*
       * Evaluate/create the Signature string, OemId string,
       * and OemTableId string operands
       */
      Status = AcpiDsCreateOperands(WalkState, NextOp);
      if (ACPI_FAILURE(Status)) {
        return_ACPI_STATUS(Status);
      }

      /*
       * Resolve the Signature string, OemId string,
       * and OemTableId string operands
       */
      Status = AcpiExResolveOperands(Op->Common.AmlOpcode, ACPI_WALK_OPERANDS,
                                     WalkState);
      if (ACPI_FAILURE(Status)) {
        return_ACPI_STATUS(Status);
      }

      Operand = &WalkState->Operands[0];

      /* Find the ACPI table */

      Status = AcpiTbFindTable(Operand[0]->String.Pointer,
                               Operand[1]->String.Pointer,
                               Operand[2]->String.Pointer, &TableIndex);
      if (ACPI_FAILURE(Status)) {
        return_ACPI_STATUS(Status);
      }

      AcpiUtRemoveReference(Operand[0]);
      AcpiUtRemoveReference(Operand[1]);
      AcpiUtRemoveReference(Operand[2]);

      Status = AcpiGetTableByIndex(TableIndex, &Table);
      if (ACPI_FAILURE(Status)) {
        return_ACPI_STATUS(Status);
      }

      ObjDesc = AcpiNsGetAttachedObject(Node);
      if (!ObjDesc) {
        return_ACPI_STATUS(AE_NOT_EXIST);
      }

      ObjDesc->Region.Address = (ACPI_PHYSICAL_ADDRESS)ACPI_TO_INTEGER(Table);
      ObjDesc->Region.Length = Table->Length;

      ACPI_DEBUG_PRINT((ACPI_DB_EXEC, "RgnObj %p Addr %8.8X%8.8X Len %X\n",
                        ObjDesc,
                        ACPI_FORMAT_NATIVE_UINT(ObjDesc->Region.Address),
                        ObjDesc->Region.Length));

      /* Now the address and length are valid for this opregion */

      ObjDesc->Region.Flags |= AOPOBJ_DATA_VALID;

      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsEvalDataObjectOperands
     *
     * PARAMETERS:  WalkState       - Current walk
     *              Op              - A valid DataObject Op object
     *              ObjDesc         - DataObject
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Get the operands and complete the following data object
     *types: Buffer, Package.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsEvalDataObjectOperands(ACPI_WALK_STATE * WalkState,
                                 ACPI_PARSE_OBJECT * Op,
                                 ACPI_OPERAND_OBJECT * ObjDesc) {
      ACPI_STATUS Status;
      ACPI_OPERAND_OBJECT *ArgDesc;
      UINT32 Length;

      ACPI_FUNCTION_TRACE(DsEvalDataObjectOperands);

      /* The first operand (for all of these data objects) is the length */

      /*
       * Set proper index into operand stack for AcpiDsObjStackPush
       * invoked inside AcpiDsCreateOperand.
       */
      WalkState->OperandIndex = WalkState->NumOperands;

      Status = AcpiDsCreateOperand(WalkState, Op->Common.Value.Arg, 1);
      if (ACPI_FAILURE(Status)) {
        return_ACPI_STATUS(Status);
      }

      Status = AcpiExResolveOperands(
          WalkState->Opcode, &(WalkState->Operands[WalkState->NumOperands - 1]),
          WalkState);
      if (ACPI_FAILURE(Status)) {
        return_ACPI_STATUS(Status);
      }

      /* Extract length operand */

      ArgDesc = WalkState->Operands[WalkState->NumOperands - 1];
      Length = (UINT32)ArgDesc->Integer.Value;

      /* Cleanup for length operand */

      Status = AcpiDsObjStackPop(1, WalkState);
      if (ACPI_FAILURE(Status)) {
        return_ACPI_STATUS(Status);
      }

      AcpiUtRemoveReference(ArgDesc);

      /*
       * Create the actual data object
       */
      switch (Op->Common.AmlOpcode) {
      case AML_BUFFER_OP:

        Status = AcpiDsBuildInternalBufferObj(WalkState, Op, Length, &ObjDesc);
        break;

      case AML_PACKAGE_OP:
      case AML_VAR_PACKAGE_OP:

        Status = AcpiDsBuildInternalPackageObj(WalkState, Op, Length, &ObjDesc);
        break;

      default:

        return_ACPI_STATUS(AE_AML_BAD_OPCODE);
      }

      if (ACPI_SUCCESS(Status)) {
        /*
         * Return the object in the WalkState, unless the parent is a package -
         * in this case, the return object will be stored in the parse tree
         * for the package.
         */
        if ((!Op->Common.Parent) ||
            ((Op->Common.Parent->Common.AmlOpcode != AML_PACKAGE_OP) &&
             (Op->Common.Parent->Common.AmlOpcode != AML_VAR_PACKAGE_OP) &&
             (Op->Common.Parent->Common.AmlOpcode != AML_NAME_OP))) {
          WalkState->ResultObj = ObjDesc;
        }
      }

      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsEvalBankFieldOperands
     *
     * PARAMETERS:  WalkState       - Current walk
     *              Op              - A valid BankField Op object
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Get BankField BankValue
     *              Called from AcpiDsExecEndOp during BankField parse tree walk
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsEvalBankFieldOperands(ACPI_WALK_STATE * WalkState,
                                ACPI_PARSE_OBJECT * Op) {
      ACPI_STATUS Status;
      ACPI_OPERAND_OBJECT *ObjDesc;
      ACPI_OPERAND_OBJECT *OperandDesc;
      ACPI_NAMESPACE_NODE *Node;
      ACPI_PARSE_OBJECT *NextOp;
      ACPI_PARSE_OBJECT *Arg;

      ACPI_FUNCTION_TRACE_PTR(DsEvalBankFieldOperands, Op);

      /*
       * This is where we evaluate the BankValue field of the
       * BankField declaration
       */

      /* NextOp points to the op that holds the Region */

      NextOp = Op->Common.Value.Arg;

      /* NextOp points to the op that holds the Bank Register */

      NextOp = NextOp->Common.Next;

      /* NextOp points to the op that holds the Bank Value */

      NextOp = NextOp->Common.Next;

      /*
       * Set proper index into operand stack for AcpiDsObjStackPush
       * invoked inside AcpiDsCreateOperand.
       *
       * We use WalkState->Operands[0] to store the evaluated BankValue
       */
      WalkState->OperandIndex = 0;

      Status = AcpiDsCreateOperand(WalkState, NextOp, 0);
      if (ACPI_FAILURE(Status)) {
        return_ACPI_STATUS(Status);
      }

      Status = AcpiExResolveToValue(&WalkState->Operands[0], WalkState);
      if (ACPI_FAILURE(Status)) {
        return_ACPI_STATUS(Status);
      }

      ACPI_DUMP_OPERANDS(ACPI_WALK_OPERANDS,
                         AcpiPsGetOpcodeName(Op->Common.AmlOpcode), 1);
      /*
       * Get the BankValue operand and save it
       * (at Top of stack)
       */
      OperandDesc = WalkState->Operands[0];

      /* Arg points to the start Bank Field */

      Arg = AcpiPsGetArg(Op, 4);
      while (Arg) {
        /* Ignore OFFSET and ACCESSAS terms here */

        if (Arg->Common.AmlOpcode == AML_INT_NAMEDFIELD_OP) {
          Node = Arg->Common.Node;

          ObjDesc = AcpiNsGetAttachedObject(Node);
          if (!ObjDesc) {
            return_ACPI_STATUS(AE_NOT_EXIST);
          }

          ObjDesc->BankField.Value = (UINT32)OperandDesc->Integer.Value;
        }

        /* Move to next field in the list */

        Arg = Arg->Common.Next;
      }

      AcpiUtRemoveReference(OperandDesc);
      return_ACPI_STATUS(Status);
    }

    /**
     * @section minix_drivers_power_acpi_dispatcher_dsutils_c
     * @brief Original file: minix/drivers/power/acpi/dispatcher/dsutils.c
     */
    /*******************************************************************************
     *
     * Module Name: dsutils - Dispatcher utilities
     *
     ******************************************************************************/

    /*
     * Copyright (C) 2000 - 2014, Intel Corp.
     * All rights reserved.
     *
     * Redistribution and use in source and binary forms, with or without
     * modification, are permitted provided that the following conditions
     * are met:
     * 1. Redistributions of source code must retain the above copyright
     *    notice, this list of conditions, and the following disclaimer,
     *    without modification.
     * 2. Redistributions in binary form must reproduce at minimum a disclaimer
     *    substantially similar to the "NO WARRANTY" disclaimer below
     *    ("Disclaimer") and any redistribution must be conditioned upon
     *    including a substantially similar Disclaimer requirement for further
     *    binary redistribution.
     * 3. Neither the names of the above-listed copyright holders nor the names
     *    of any contributors may be used to endorse or promote products derived
     *    from this software without specific prior written permission.
     *
     * Alternatively, this software may be distributed under the terms of the
     * GNU General Public License ("GPL") version 2 as published by the Free
     * Software Foundation.
     *
     * NO WARRANTY
     * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
     * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
     * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR
     * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
     * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR
     * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
     * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
     * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
     * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
     * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
     * THE POSSIBILITY OF SUCH DAMAGES.
     */

#include "accommon.h"
#include "acdebug.h"
#include "acdispat.h"
#include "acinterp.h"
#include "acnamesp.h"
#include "acparser.h"
#include "acpi.h"
#include "amlcode.h"

#define _COMPONENT ACPI_DISPATCHER
    ACPI_MODULE_NAME("dsutils")

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsClearImplicitReturn
     *
     * PARAMETERS:  WalkState           - Current State
     *
     * RETURN:      None.
     *
     * DESCRIPTION: Clear and remove a reference on an implicit return value.
     *Used to delete "stale" return values (if enabled, the return value from
     *every operator is saved at least momentarily, in case the parent method
     *exits.)
     *
     ******************************************************************************/

    void AcpiDsClearImplicitReturn(ACPI_WALK_STATE * WalkState) {
      ACPI_FUNCTION_NAME(DsClearImplicitReturn);

      /*
       * Slack must be enabled for this feature
       */
      if (!AcpiGbl_EnableInterpreterSlack) {
        return;
      }

      if (WalkState->ImplicitReturnObj) {
        /*
         * Delete any "stale" implicit return. However, in
         * complex statements, the implicit return value can be
         * bubbled up several levels.
         */
        ACPI_DEBUG_PRINT(
            (ACPI_DB_DISPATCH,
             "Removing reference on stale implicit return obj %p\n",
             WalkState->ImplicitReturnObj));

        AcpiUtRemoveReference(WalkState->ImplicitReturnObj);
        WalkState->ImplicitReturnObj = NULL;
      }
    }

#ifndef ACPI_NO_METHOD_EXECUTION
    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsDoImplicitReturn
     *
     * PARAMETERS:  ReturnDesc          - The return value
     *              WalkState           - Current State
     *              AddReference        - True if a reference should be added to
     *the return object
     *
     * RETURN:      TRUE if implicit return enabled, FALSE otherwise
     *
     * DESCRIPTION: Implements the optional "implicit return".  We save the
     *result of every ASL operator and control method invocation in case the
     *              parent method exit. Before storing a new return value, we
     *              delete the previous return value.
     *
     ******************************************************************************/

    BOOLEAN
    AcpiDsDoImplicitReturn(ACPI_OPERAND_OBJECT * ReturnDesc,
                           ACPI_WALK_STATE * WalkState, BOOLEAN AddReference) {
      ACPI_FUNCTION_NAME(DsDoImplicitReturn);

      /*
       * Slack must be enabled for this feature, and we must
       * have a valid return object
       */
      if ((!AcpiGbl_EnableInterpreterSlack) || (!ReturnDesc)) {
        return (FALSE);
      }

      ACPI_DEBUG_PRINT((ACPI_DB_DISPATCH,
                        "Result %p will be implicitly returned; Prev=%p\n",
                        ReturnDesc, WalkState->ImplicitReturnObj));

      /*
       * Delete any "stale" implicit return value first. However, in
       * complex statements, the implicit return value can be
       * bubbled up several levels, so we don't clear the value if it
       * is the same as the ReturnDesc.
       */
      if (WalkState->ImplicitReturnObj) {
        if (WalkState->ImplicitReturnObj == ReturnDesc) {
          return (TRUE);
        }
        AcpiDsClearImplicitReturn(WalkState);
      }

      /* Save the implicit return value, add a reference if requested */

      WalkState->ImplicitReturnObj = ReturnDesc;
      if (AddReference) {
        AcpiUtAddReference(ReturnDesc);
      }

      return (TRUE);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsIsResultUsed
     *
     * PARAMETERS:  Op                  - Current Op
     *              WalkState           - Current State
     *
     * RETURN:      TRUE if result is used, FALSE otherwise
     *
     * DESCRIPTION: Check if a result object will be used by the parent
     *
     ******************************************************************************/

    BOOLEAN
    AcpiDsIsResultUsed(ACPI_PARSE_OBJECT * Op, ACPI_WALK_STATE * WalkState) {
      const ACPI_OPCODE_INFO *ParentInfo;

      ACPI_FUNCTION_TRACE_PTR(DsIsResultUsed, Op);

      /* Must have both an Op and a Result Object */

      if (!Op) {
        ACPI_ERROR((AE_INFO, "Null Op"));
        return_UINT8(TRUE);
      }

      /*
       * We know that this operator is not a
       * Return() operator (would not come here.) The following code is the
       * optional support for a so-called "implicit return". Some AML code
       * assumes that the last value of the method is "implicitly" returned
       * to the caller. Just save the last result as the return value.
       * NOTE: this is optional because the ASL language does not actually
       * support this behavior.
       */
      (void)AcpiDsDoImplicitReturn(WalkState->ResultObj, WalkState, TRUE);

      /*
       * Now determine if the parent will use the result
       *
       * If there is no parent, or the parent is a ScopeOp, we are executing
       * at the method level. An executing method typically has no parent,
       * since each method is parsed separately. A method invoked externally
       * via ExecuteControlMethod has a ScopeOp as the parent.
       */
      if ((!Op->Common.Parent) ||
          (Op->Common.Parent->Common.AmlOpcode == AML_SCOPE_OP)) {
        /* No parent, the return value cannot possibly be used */

        ACPI_DEBUG_PRINT((ACPI_DB_DISPATCH,
                          "At Method level, result of [%s] not used\n",
                          AcpiPsGetOpcodeName(Op->Common.AmlOpcode)));
        return_UINT8(FALSE);
      }

      /* Get info on the parent. The RootOp is AML_SCOPE */

      ParentInfo = AcpiPsGetOpcodeInfo(Op->Common.Parent->Common.AmlOpcode);
      if (ParentInfo->Class == AML_CLASS_UNKNOWN) {
        ACPI_ERROR((AE_INFO, "Unknown parent opcode Op=%p", Op));
        return_UINT8(FALSE);
      }

      /*
       * Decide what to do with the result based on the parent. If
       * the parent opcode will not use the result, delete the object.
       * Otherwise leave it as is, it will be deleted when it is used
       * as an operand later.
       */
      switch (ParentInfo->Class) {
      case AML_CLASS_CONTROL:

        switch (Op->Common.Parent->Common.AmlOpcode) {
        case AML_RETURN_OP:

          /* Never delete the return value associated with a return opcode */

          goto ResultUsed;

        case AML_IF_OP:
        case AML_WHILE_OP:
          /*
           * If we are executing the predicate AND this is the predicate op,
           * we will use the return value
           */
          if ((WalkState->ControlState->Common.State ==
               ACPI_CONTROL_PREDICATE_EXECUTING) &&
              (WalkState->ControlState->Control.PredicateOp == Op)) {
            goto ResultUsed;
          }
          break;

        default:

          /* Ignore other control opcodes */

          break;
        }

        /* The general control opcode returns no result */

        goto ResultNotUsed;

      case AML_CLASS_CREATE:
        /*
         * These opcodes allow TermArg(s) as operands and therefore
         * the operands can be method calls. The result is used.
         */
        goto ResultUsed;

      case AML_CLASS_NAMED_OBJECT:

        if ((Op->Common.Parent->Common.AmlOpcode == AML_REGION_OP) ||
            (Op->Common.Parent->Common.AmlOpcode == AML_DATA_REGION_OP) ||
            (Op->Common.Parent->Common.AmlOpcode == AML_PACKAGE_OP) ||
            (Op->Common.Parent->Common.AmlOpcode == AML_VAR_PACKAGE_OP) ||
            (Op->Common.Parent->Common.AmlOpcode == AML_BUFFER_OP) ||
            (Op->Common.Parent->Common.AmlOpcode == AML_INT_EVAL_SUBTREE_OP) ||
            (Op->Common.Parent->Common.AmlOpcode == AML_BANK_FIELD_OP)) {
          /*
           * These opcodes allow TermArg(s) as operands and therefore
           * the operands can be method calls. The result is used.
           */
          goto ResultUsed;
        }

        goto ResultNotUsed;

      default:
        /*
         * In all other cases. the parent will actually use the return
         * object, so keep it.
         */
        goto ResultUsed;
      }

    ResultUsed:
      ACPI_DEBUG_PRINT(
          (ACPI_DB_DISPATCH, "Result of [%s] used by Parent [%s] Op=%p\n",
           AcpiPsGetOpcodeName(Op->Common.AmlOpcode),
           AcpiPsGetOpcodeName(Op->Common.Parent->Common.AmlOpcode), Op));

      return_UINT8(TRUE);

    ResultNotUsed:
      ACPI_DEBUG_PRINT(
          (ACPI_DB_DISPATCH, "Result of [%s] not used by Parent [%s] Op=%p\n",
           AcpiPsGetOpcodeName(Op->Common.AmlOpcode),
           AcpiPsGetOpcodeName(Op->Common.Parent->Common.AmlOpcode), Op));

      return_UINT8(FALSE);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsDeleteResultIfNotUsed
     *
     * PARAMETERS:  Op              - Current parse Op
     *              ResultObj       - Result of the operation
     *              WalkState       - Current state
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Used after interpretation of an opcode. If there is an
     *internal result descriptor, check if the parent opcode will actually use
     *              this result. If not, delete the result now so that it will
     *              not become orphaned.
     *
     ******************************************************************************/

    void AcpiDsDeleteResultIfNotUsed(ACPI_PARSE_OBJECT * Op,
                                     ACPI_OPERAND_OBJECT * ResultObj,
                                     ACPI_WALK_STATE * WalkState) {
      ACPI_OPERAND_OBJECT *ObjDesc;
      ACPI_STATUS Status;

      ACPI_FUNCTION_TRACE_PTR(DsDeleteResultIfNotUsed, ResultObj);

      if (!Op) {
        ACPI_ERROR((AE_INFO, "Null Op"));
        return_VOID;
      }

      if (!ResultObj) {
        return_VOID;
      }

      if (!AcpiDsIsResultUsed(Op, WalkState)) {
        /* Must pop the result stack (ObjDesc should be equal to ResultObj) */

        Status = AcpiDsResultPop(&ObjDesc, WalkState);
        if (ACPI_SUCCESS(Status)) {
          AcpiUtRemoveReference(ResultObj);
        }
      }

      return_VOID;
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsResolveOperands
     *
     * PARAMETERS:  WalkState           - Current walk state with operands on
     *stack
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Resolve all operands to their values. Used to prepare
     *              arguments to a control method invocation (a call from one
     *              method to another.)
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsResolveOperands(ACPI_WALK_STATE * WalkState) {
      UINT32 i;
      ACPI_STATUS Status = AE_OK;

      ACPI_FUNCTION_TRACE_PTR(DsResolveOperands, WalkState);

      /*
       * Attempt to resolve each of the valid operands
       * Method arguments are passed by reference, not by value. This means
       * that the actual objects are passed, not copies of the objects.
       */
      for (i = 0; i < WalkState->NumOperands; i++) {
        Status = AcpiExResolveToValue(&WalkState->Operands[i], WalkState);
        if (ACPI_FAILURE(Status)) {
          break;
        }
      }

      return_ACPI_STATUS(Status);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsClearOperands
     *
     * PARAMETERS:  WalkState           - Current walk state with operands on
     *stack
     *
     * RETURN:      None
     *
     * DESCRIPTION: Clear all operands on the current walk state operand stack.
     *
     ******************************************************************************/

    void AcpiDsClearOperands(ACPI_WALK_STATE * WalkState) {
      UINT32 i;

      ACPI_FUNCTION_TRACE_PTR(DsClearOperands, WalkState);

      /* Remove a reference on each operand on the stack */

      for (i = 0; i < WalkState->NumOperands; i++) {
        /*
         * Remove a reference to all operands, including both
         * "Arguments" and "Targets".
         */
        AcpiUtRemoveReference(WalkState->Operands[i]);
        WalkState->Operands[i] = NULL;
      }

      WalkState->NumOperands = 0;
      return_VOID;
    }
#endif

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsCreateOperand
     *
     * PARAMETERS:  WalkState       - Current walk state
     *              Arg             - Parse object for the argument
     *              ArgIndex        - Which argument (zero based)
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Translate a parse tree object that is an argument to an AML
     *              opcode to the equivalent interpreter object. This may
     *include looking up a name or entering a new name into the internal
     *              namespace.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsCreateOperand(ACPI_WALK_STATE * WalkState, ACPI_PARSE_OBJECT * Arg,
                        UINT32 ArgIndex) {
      ACPI_STATUS Status = AE_OK;
      char *NameString;
      UINT32 NameLength;
      ACPI_OPERAND_OBJECT *ObjDesc;
      ACPI_PARSE_OBJECT *ParentOp;
      UINT16 Opcode;
      ACPI_INTERPRETER_MODE InterpreterMode;
      const ACPI_OPCODE_INFO *OpInfo;

      ACPI_FUNCTION_TRACE_PTR(DsCreateOperand, Arg);

      /* A valid name must be looked up in the namespace */

      if ((Arg->Common.AmlOpcode == AML_INT_NAMEPATH_OP) &&
          (Arg->Common.Value.String) &&
          !(Arg->Common.Flags & ACPI_PARSEOP_IN_STACK)) {
        ACPI_DEBUG_PRINT((ACPI_DB_DISPATCH, "Getting a name: Arg=%p\n", Arg));

        /* Get the entire name string from the AML stream */

        Status = AcpiExGetNameString(ACPI_TYPE_ANY, Arg->Common.Value.Buffer,
                                     &NameString, &NameLength);

        if (ACPI_FAILURE(Status)) {
          return_ACPI_STATUS(Status);
        }

        /* All prefixes have been handled, and the name is in NameString */

        /*
         * Special handling for BufferField declarations. This is a deferred
         * opcode that unfortunately defines the field name as the last
         * parameter instead of the first. We get here when we are performing
         * the deferred execution, so the actual name of the field is already
         * in the namespace. We don't want to attempt to look it up again
         * because we may be executing in a different scope than where the
         * actual opcode exists.
         */
        if ((WalkState->DeferredNode) &&
            (WalkState->DeferredNode->Type == ACPI_TYPE_BUFFER_FIELD) &&
            (ArgIndex ==
             (UINT32)((WalkState->Opcode == AML_CREATE_FIELD_OP) ? 3 : 2))) {
          ObjDesc = ACPI_CAST_PTR(ACPI_OPERAND_OBJECT, WalkState->DeferredNode);
          Status = AE_OK;
        } else /* All other opcodes */
        {
          /*
           * Differentiate between a namespace "create" operation
           * versus a "lookup" operation (IMODE_LOAD_PASS2 vs.
           * IMODE_EXECUTE) in order to support the creation of
           * namespace objects during the execution of control methods.
           */
          ParentOp = Arg->Common.Parent;
          OpInfo = AcpiPsGetOpcodeInfo(ParentOp->Common.AmlOpcode);
          if ((OpInfo->Flags & AML_NSNODE) &&
              (ParentOp->Common.AmlOpcode != AML_INT_METHODCALL_OP) &&
              (ParentOp->Common.AmlOpcode != AML_REGION_OP) &&
              (ParentOp->Common.AmlOpcode != AML_INT_NAMEPATH_OP)) {
            /* Enter name into namespace if not found */

            InterpreterMode = ACPI_IMODE_LOAD_PASS2;
          } else {
            /* Return a failure if name not found */

            InterpreterMode = ACPI_IMODE_EXECUTE;
          }

          Status = AcpiNsLookup(
              WalkState->ScopeInfo, NameString, ACPI_TYPE_ANY, InterpreterMode,
              ACPI_NS_SEARCH_PARENT | ACPI_NS_DONT_OPEN_SCOPE, WalkState,
              ACPI_CAST_INDIRECT_PTR(ACPI_NAMESPACE_NODE, &ObjDesc));
          /*
           * The only case where we pass through (ignore) a NOT_FOUND
           * error is for the CondRefOf opcode.
           */
          if (Status == AE_NOT_FOUND) {
            if (ParentOp->Common.AmlOpcode == AML_COND_REF_OF_OP) {
              /*
               * For the Conditional Reference op, it's OK if
               * the name is not found;  We just need a way to
               * indicate this to the interpreter, set the
               * object to the root
               */
              ObjDesc = ACPI_CAST_PTR(ACPI_OPERAND_OBJECT, AcpiGbl_RootNode);
              Status = AE_OK;
            } else {
              /*
               * We just plain didn't find it -- which is a
               * very serious error at this point
               */
              Status = AE_AML_NAME_NOT_FOUND;
            }
          }

          if (ACPI_FAILURE(Status)) {
            ACPI_ERROR_NAMESPACE(NameString, Status);
          }
        }

        /* Free the namestring created above */

        ACPI_FREE(NameString);

        /* Check status from the lookup */

        if (ACPI_FAILURE(Status)) {
          return_ACPI_STATUS(Status);
        }

        /* Put the resulting object onto the current object stack */

        Status = AcpiDsObjStackPush(ObjDesc, WalkState);
        if (ACPI_FAILURE(Status)) {
          return_ACPI_STATUS(Status);
        }
        ACPI_DEBUGGER_EXEC(AcpiDbDisplayArgumentObject(ObjDesc, WalkState));
      } else {
        /* Check for null name case */

        if ((Arg->Common.AmlOpcode == AML_INT_NAMEPATH_OP) &&
            !(Arg->Common.Flags & ACPI_PARSEOP_IN_STACK)) {
          /*
           * If the name is null, this means that this is an
           * optional result parameter that was not specified
           * in the original ASL. Create a Zero Constant for a
           * placeholder. (Store to a constant is a Noop.)
           */
          Opcode = AML_ZERO_OP; /* Has no arguments! */

          ACPI_DEBUG_PRINT((ACPI_DB_DISPATCH, "Null namepath: Arg=%p\n", Arg));
        } else {
          Opcode = Arg->Common.AmlOpcode;
        }

        /* Get the object type of the argument */

        OpInfo = AcpiPsGetOpcodeInfo(Opcode);
        if (OpInfo->ObjectType == ACPI_TYPE_INVALID) {
          return_ACPI_STATUS(AE_NOT_IMPLEMENTED);
        }

        if ((OpInfo->Flags & AML_HAS_RETVAL) ||
            (Arg->Common.Flags & ACPI_PARSEOP_IN_STACK)) {
          ACPI_DEBUG_PRINT((ACPI_DB_DISPATCH,
                            "Argument previously created, already stacked\n"));

          ACPI_DEBUGGER_EXEC(AcpiDbDisplayArgumentObject(
              WalkState->Operands[WalkState->NumOperands - 1], WalkState));

          /*
           * Use value that was already previously returned
           * by the evaluation of this argument
           */
          Status = AcpiDsResultPop(&ObjDesc, WalkState);
          if (ACPI_FAILURE(Status)) {
            /*
             * Only error is underflow, and this indicates
             * a missing or null operand!
             */
            ACPI_EXCEPTION((AE_INFO, Status, "Missing or null operand"));
            return_ACPI_STATUS(Status);
          }
        } else {
          /* Create an ACPI_INTERNAL_OBJECT for the argument */

          ObjDesc = AcpiUtCreateInternalObject(OpInfo->ObjectType);
          if (!ObjDesc) {
            return_ACPI_STATUS(AE_NO_MEMORY);
          }

          /* Initialize the new object */

          Status = AcpiDsInitObjectFromOp(WalkState, Arg, Opcode, &ObjDesc);
          if (ACPI_FAILURE(Status)) {
            AcpiUtDeleteObjectDesc(ObjDesc);
            return_ACPI_STATUS(Status);
          }
        }

        /* Put the operand object on the object stack */

        Status = AcpiDsObjStackPush(ObjDesc, WalkState);
        if (ACPI_FAILURE(Status)) {
          return_ACPI_STATUS(Status);
        }

        ACPI_DEBUGGER_EXEC(AcpiDbDisplayArgumentObject(ObjDesc, WalkState));
      }

      return_ACPI_STATUS(AE_OK);
    }

    /*******************************************************************************
     *
     * FUNCTION:    AcpiDsCreateOperands
     *
     * PARAMETERS:  WalkState           - Current state
     *              FirstArg            - First argument of a parser argument
     *tree
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Convert an operator's arguments from a parse tree format to
     *              namespace objects and place those argument object on the
     *object stack in preparation for evaluation by the interpreter.
     *
     ******************************************************************************/

    ACPI_STATUS
    AcpiDsCreateOperands(ACPI_WALK_STATE * WalkState,
                         ACPI_PARSE_OBJECT * FirstArg) {
      ACPI_STATUS Status = AE_OK;
      ACPI_PARSE_OBJECT *Arg;
      ACPI_PARSE_OBJECT *Arguments[ACPI_OBJ_NUM_OPERANDS];
      UINT32 ArgCount = 0;
      UINT32 Index = WalkState->NumOperands;
      UINT32 i;

      ACPI_FUNCTION_TRACE_PTR(DsCreateOperands, FirstArg);

      /* Get all arguments in the list */

      Arg = FirstArg;
      while (Arg) {
        if (Index >= ACPI_OBJ_NUM_OPERANDS) {
          return_ACPI_STATUS(AE_BAD_DATA);
        }

        Arguments[Index] = Arg;
        WalkState->Operands[Index] = NULL;

        /* Move on to next argument, if any */

        Arg = Arg->Common.Next;
        ArgCount++;
        Index++;
      }

      ACPI_DEBUG_PRINT((ACPI_DB_DISPATCH,
                        "NumOperands %d, ArgCount %d, Index %d\n",
                        WalkState->NumOperands, ArgCount, Index));

      /* Create the interpreter arguments, in reverse order */

      Index--;
      for (i = 0; i < ArgCount; i++) {
        Arg = Arguments[Index];
        WalkState->OperandIndex = (UINT8)Index;

        Status = AcpiDsCreateOperand(WalkState, Arg, Index);
        if (ACPI_FAILURE(Status)) {
          goto Cleanup;
        }

        ACPI_DEBUG_PRINT((ACPI_DB_DISPATCH,
                          "Created Arg #%u (%p) %u args total\n", Index, Arg,
                          ArgCount));
        Index--;
      }

      return_ACPI_STATUS(Status);

    Cleanup:
      /*
       * We must undo everything done above; meaning that we must
       * pop everything off of the operand stack and delete those
       * objects
       */
      AcpiDsObjStackPopAndDelete(ArgCount, WalkState);

      ACPI_EXCEPTION((AE_INFO, Status, "While creating Arg %u", Index));
      return_ACPI_STATUS(Status);
    }

    /*****************************************************************************
     *
     * FUNCTION:    AcpiDsEvaluateNamePath
     *
     * PARAMETERS:  WalkState       - Current state of the parse tree walk,
     *                                the opcode of current operation should be
     *                                AML_INT_NAMEPATH_OP
     *
     * RETURN:      Status
     *
     * DESCRIPTION: Translate the -NamePath- parse tree object to the equivalent
     *              interpreter object, convert it to value, if needed,
     *duplicate it, if needed, and push it onto the current result stack.
     *
     ****************************************************************************/

    ACPI_STATUS
    AcpiDsEvaluateNamePath(ACPI_WALK_STATE * WalkState) {
      ACPI_STATUS Status = AE_OK;
      ACPI_PARSE_OBJECT *Op = WalkState->Op;
      ACPI_OPERAND_OBJECT **Operand = &WalkState->Operands[0];
      ACPI_OPERAND_OBJECT *NewObjDesc;
      UINT8 Type;

      ACPI_FUNCTION_TRACE_PTR(DsEvaluateNamePath, WalkState);

      if (!Op->Common.Parent) {
        /* This happens after certain exception processing */

        goto Exit;
      }

      if ((Op->Common.Parent->Common.AmlOpcode == AML_PACKAGE_OP) ||
          (Op->Common.Parent->Common.AmlOpcode == AML_VAR_PACKAGE_OP) ||
          (Op->Common.Parent->Common.AmlOpcode == AML_REF_OF_OP)) {
        /* TBD: Should we specify this feature as a bit of OpInfo->Flags of
         * these opcodes? */

        goto Exit;
      }

      Status = AcpiDsCreateOperand(WalkState, Op, 0);
      if (ACPI_FAILURE(Status)) {
        goto Exit;
      }

      if (Op->Common.Flags & ACPI_PARSEOP_TARGET) {
        NewObjDesc = *Operand;
        goto PushResult;
      }

      Type = (*Operand)->Common.Type;

      Status = AcpiExResolveToValue(Operand, WalkState);
      if (ACPI_FAILURE(Status)) {
        goto Exit;
      }

      if (Type == ACPI_TYPE_INTEGER) {
        /* It was incremented by AcpiExResolveToValue */

        AcpiUtRemoveReference(*Operand);

        Status = AcpiUtCopyIobjectToIobject(*Operand, &NewObjDesc, WalkState);
        if (ACPI_FAILURE(Status)) {
          goto Exit;
        }
      } else {
        /*
         * The object either was anew created or is
         * a Namespace node - don't decrement it.
         */
        NewObjDesc = *Operand;
      }

      /* Cleanup for name-path operand */

      Status = AcpiDsObjStackPop(1, WalkState);
      if (ACPI_FAILURE(Status)) {
        WalkState->ResultObj = NewObjDesc;
        goto Exit;
      }

    PushResult:

      WalkState->ResultObj = NewObjDesc;

      Status = AcpiDsResultPush(WalkState->ResultObj, WalkState);
      if (ACPI_SUCCESS(Status)) {
        /* Force to take it from stack */

        Op->Common.Flags |= ACPI_PARSEOP_IN_STACK;
      }

    Exit:

      return_ACPI_STATUS(Status);
    }
