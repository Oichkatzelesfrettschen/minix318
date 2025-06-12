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
