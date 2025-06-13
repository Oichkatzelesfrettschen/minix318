/**
 * @file acpi_unified.h
 * @brief Consolidated ACPI header files for analysis.
 */

/**
 * @section drivers_power_acpi_acpi_globals_h
 * @brief Original file: minix/drivers/power/acpi/acpi_globals.h
 */
#ifndef __ACPI_GLOBALS_H__
#define __ACPI_GLOBALS_H__

EXTERN int acpi_enabled;
EXTERN struct machine machine;

#endif /* __ACPI_GLOBALS_H__ */

/**
 * @section kernel_arch_i386_acpi_h
 * @brief Original file: minix/kernel/arch/i386/acpi.h
 */
#ifndef __ACPI_H__
#define __ACPI_H__

#include "kernel/kernel.h"

// Added kernel headers (precautionary for consistency)
#include <klib/include/kmemory.h>
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <minix/kernel_types.h>

/* ACPI root system description pointer */
struct acpi_rsdp {
  char signature[8]; /* must be "RSD PTR " */
  u8_t checksum;
  char oemid[6];
  u8_t revision;
  u32_t rsdt_addr;
  u32_t length;
};

#define ACPI_SDT_SIGNATURE_LEN 4

#define ACPI_SDT_SIGNATURE(name) #name

/* header common to all system description tables */
struct acpi_sdt_header {
  char signature[ACPI_SDT_SIGNATURE_LEN];
  u32_t length;
  u8_t revision;
  u8_t checksum;
  char oemid[6];
  char oem_table_id[8];
  u32_t oem_revision;
  u32_t creator_id;
  u32_t creator_revision;
};

struct acpi_generic_address {
  u8_t address_space_id;
  u8_t register_bit_width;
  u8_t register_bit_offset;
  u8_t access_size;
  u64_t address;
};

struct acpi_fadt_header {
  struct acpi_sdt_header hdr;
  u32_t facs;
  u32_t dsdt;
  u8_t model;
  u8_t preferred_pm_profile;
  u16_t sci_int;
  u32_t smi_cmd;
  u8_t acpi_enable;
  u8_t acpi_disable;
  u8_t s4bios_req;
  u8_t pstate_cnt;
  u32_t pm1a_evt_blk;
  u32_t pm1b_evt_blk;
  u32_t pm1a_cnt_blk;
  u32_t pm1b_cnt_blk;
  u32_t pm2_cnt_blk;
  u32_t pm_tmr_blk;
  u32_t gpe0_blk;
  u32_t gpe1_blk;
  u8_t pm1_evt_len;
  u8_t pm1_cnt_len;
  u8_t pm2_cnt_len;
  u8_t pm_tmr_len;
  u8_t gpe0_blk_len;
  u8_t gpe1_blk_len;
  u8_t gpe1_base;
  u8_t cst_cnt;
  u16_t p_lvl2_lat;
  u16_t p_lvl3_lat;
  u16_t flush_size;
  u16_t flush_stride;
  u8_t duty_offset;
  u8_t duty_width;
  u8_t day_alrm;
  u8_t mon_alrm;
  u8_t century;
  u16_t iapc_boot_arch;
  u8_t reserved1;
  u32_t flags;
  struct acpi_generic_address reset_reg;
  u8_t reset_value;
  u8_t reserved2[3];
  u64_t xfacs;
  u64_t xdsdt;
  struct acpi_generic_address xpm1a_evt_blk;
  struct acpi_generic_address xpm1b_evt_blk;
  struct acpi_generic_address xpm1a_cnt_blk;
  struct acpi_generic_address xpm1b_cnt_blk;
  struct acpi_generic_address xpm2_cnt_blk;
  struct acpi_generic_address xpm_tmr_blk;
  struct acpi_generic_address xgpe0_blk;
  struct acpi_generic_address xgpe1_blk;
};

struct acpi_madt_hdr {
  struct acpi_sdt_header hdr;
  u32_t local_apic_address;
  u32_t flags;
};

#define ACPI_MADT_TYPE_LAPIC 0
#define ACPI_MADT_TYPE_IOAPIC 1
#define ACPI_MADT_TYPE_INT_SRC 2
#define ACPI_MADT_TYPE_NMI_SRC 3
#define ACPI_MADT_TYPE_LAPIC_NMI 4
#define ACPI_MADT_TYPE_LAPIC_ADRESS 5
#define ACPI_MADT_TYPE_IOSAPIC 6
#define ACPI_MADT_TYPE_LSAPIC 7
#define ACPI_MADT_TYPE_PLATFORM_INT_SRC 8
#define ACPI_MADT_TYPE_Lx2APIC 9
#define ACPI_MADT_TYPE_Lx2APIC_NMI 10

struct acpi_madt_item_hdr {
  u8_t type;
  u8_t length;
};

struct acpi_madt_lapic {
  struct acpi_madt_item_hdr hdr;
  u8_t acpi_cpu_id;
  u8_t apic_id;
  u32_t flags;
};

struct acpi_madt_ioapic {
  struct acpi_madt_item_hdr hdr;
  u8_t id;
  u8_t __reserved;
  u32_t address;
  u32_t global_int_base;
};

struct acpi_madt_int_src {
  struct acpi_madt_item_hdr hdr;
  u8_t bus;
  u8_t bus_int;
  u32_t global_int;
  u16_t mps_flags;
};

struct acpi_madt_nmi {
  struct acpi_madt_item_hdr hdr;
  u16_t flags;
  u32_t global_int;
};

void acpi_init(void);

void acpi_poweroff(void);

/*
 * Returns a pointer to the io acpi structure in the MADT table in ACPI. The
 * pointer is valid only until paging is turned off. No memory is allocated in
 * this function thus no memory needs to be freed
 */
struct acpi_madt_ioapic *acpi_get_ioapic_next(void);
/* same as above for local APICs */
struct acpi_madt_lapic *acpi_get_lapic_next(void);

#endif /* __ACPI_H__ */

/**
 * @section kernel_uts_i86pc_sys_acpidev_impl_h
 * @brief Original file: minix/kernel/uts/i86pc/sys/acpidev_impl.h
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
 * Copyright (c) 2009-2010, Intel Corporation.
 * All rights reserved.
 */

#ifndef _SYS_ACPIDEV_IMPL_H
#define _SYS_ACPIDEV_IMPL_H
#include <sys/acpi/acpi.h>
#include <sys/acpica.h>
#include <sys/acpidev.h>
#include <sys/acpidev_dr.h>
#include <sys/bitmap.h>
#include <sys/cmn_err.h>
#include <sys/sunddi.h>
#include <sys/synch.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _KERNEL

#define ACPIDEV_ARRAY_PARAM(a) (a), (sizeof(a) / sizeof((a)[0]))

/* Debug support facilities. */
extern int acpidev_debug;
#define ACPIDEV_DEBUG(lvl, ...)                                                \
  if (acpidev_debug)                                                           \
  cmn_err((lvl), __VA_ARGS__)

/* Data attached to an ACPI object to maintain device status information. */
struct acpidev_data_impl {
  uint32_t aod_eflag; /* External flags */
  uint32_t aod_iflag; /* Internal flags */
  uint32_t aod_level;
  int aod_status; /* Cached _STA value */
  ACPI_HANDLE *aod_hdl;
  dev_info_t *aod_dip;
  acpidev_class_t *aod_class;
  acpidev_class_list_t **aod_class_list;
  acpidev_board_type_t aod_bdtype; /* Type of board. */
  uint32_t aod_bdnum;              /* Board # for DR. */
  uint32_t aod_portid;             /* Port id for DR. */
  uint32_t aod_bdidx;              /* Index # of AP */
  volatile uint32_t aod_chidx;     /* Index # of child */
  uint32_t aod_memidx;             /* Index # of memory */
  acpidev_class_id_t aod_class_id; /* Dev type for DR. */
};

#define ACPIDEV_ODF_STATUS_VALID 0x1
#define ACPIDEV_ODF_DEVINFO_CREATED 0x2
#define ACPIDEV_ODF_DEVINFO_TAGGED 0x4
#define ACPIDEV_ODF_HOTPLUG_CAPABLE 0x100
#define ACPIDEV_ODF_HOTPLUG_READY 0x200
#define ACPIDEV_ODF_HOTPLUG_FAILED 0x400

#define ACPIDEV_DR_IS_BOARD(hdl)                                               \
  ((hdl)->aod_iflag & ACPIDEV_ODF_HOTPLUG_CAPABLE)

#define ACPIDEV_DR_SET_BOARD(hdl)                                              \
  (hdl)->aod_iflag |= ACPIDEV_ODF_HOTPLUG_CAPABLE

#define ACPIDEV_DR_IS_READY(hdl) ((hdl)->aod_iflag & ACPIDEV_ODF_HOTPLUG_READY)

#define ACPIDEV_DR_SET_READY(hdl) (hdl)->aod_iflag |= ACPIDEV_ODF_HOTPLUG_READY

#define ACPIDEV_DR_IS_FAILED(hdl)                                              \
  ((hdl)->aod_iflag & ACPIDEV_ODF_HOTPLUG_FAILED)

#define ACPIDEV_DR_SET_FAILED(hdl)                                             \
  (hdl)->aod_iflag |= ACPIDEV_ODF_HOTPLUG_FAILED

#define ACPIDEV_DR_IS_WORKING(hdl)                                             \
  (((hdl)->aod_iflag &                                                         \
    (ACPIDEV_ODF_HOTPLUG_READY | ACPIDEV_ODF_HOTPLUG_FAILED)) ==               \
   ACPIDEV_ODF_HOTPLUG_READY)

#define ACPIDEV_DR_IS_PROCESSED(hdl)                                           \
  ((hdl)->aod_iflag &                                                          \
   (ACPIDEV_ODF_HOTPLUG_READY | ACPIDEV_ODF_HOTPLUG_FAILED |                   \
    ACPIDEV_ODF_HOTPLUG_CAPABLE))

#define ACPIDEV_DR_BOARD_READY(hdl)                                            \
  (((hdl)->aod_iflag &                                                         \
    (ACPIDEV_ODF_HOTPLUG_READY | ACPIDEV_ODF_HOTPLUG_CAPABLE)) ==              \
   (ACPIDEV_ODF_HOTPLUG_READY | ACPIDEV_ODF_HOTPLUG_CAPABLE))

/*
 * List of registered device class drivers.
 * Class drivers on the same list will be called from head to tail in turn.
 */
struct acpidev_class_list {
  acpidev_class_list_t *acl_next;
  acpidev_class_t *acl_class;
};

typedef struct acpidev_pseudo_uid {
  struct acpidev_pseudo_uid *apu_next;
  char *apu_uid;
  acpidev_class_id_t apu_cid;
  uint_t apu_nid;
} acpidev_pseudo_uid_t;

typedef struct acpidev_pseudo_uid_head {
  kmutex_t apuh_lock;
  uint32_t apuh_id;
  acpidev_pseudo_uid_t *apuh_first;
} acpidev_pseudo_uid_head_t;

typedef struct acpidev_dr_capacity {
  uint_t cpu_vendor;
  uint_t cpu_family;
  uint_t cpu_model_min;
  uint_t cpu_model_max;
  uint_t cpu_step_min;
  uint_t cpu_step_max;
  boolean_t hotplug_supported;
  uint64_t memory_alignment;
} acpidev_dr_capacity_t;

extern int acpidev_dr_enable;
extern krwlock_t acpidev_class_lock;
extern ulong_t acpidev_object_type_mask[BT_BITOUL(ACPI_TYPE_NS_NODE_MAX + 1)];
extern ACPI_TABLE_SRAT *acpidev_srat_tbl_ptr;
extern ACPI_TABLE_SLIT *acpidev_slit_tbl_ptr;

#endif /* _KERNEL */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_ACPIDEV_IMPL_H */

/**
 * @section drivers_power_acpi_include_acapps_h
 * @brief Original file: minix/drivers/power/acpi/include/acapps.h
 */
/******************************************************************************
 *
 * Module Name: acapps - common include for ACPI applications/tools
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
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 */

#ifndef _ACAPPS
#define _ACAPPS

#ifdef _MSC_VER /* disable some level-4 warnings */
#pragma warning(                                                               \
    disable : 4100) /* warning C4100: unreferenced formal parameter */
#endif

/* Common info for tool signons */

#define ACPICA_NAME "Intel ACPI Component Architecture"
#define ACPICA_COPYRIGHT "Copyright (c) 2000 - 2014 Intel Corporation"

#if ACPI_MACHINE_WIDTH == 64
#define ACPI_WIDTH "-64"

#elif ACPI_MACHINE_WIDTH == 32
#define ACPI_WIDTH "-32"

#else
#error unknown ACPI_MACHINE_WIDTH
#define ACPI_WIDTH "-??"

#endif

/* Macros for signons and file headers */

#define ACPI_COMMON_SIGNON(UtilityName)                                        \
  "\n%s\n%s version %8.8X%s [%s]\n%s\n\n", ACPICA_NAME, UtilityName,           \
      ((UINT32)ACPI_CA_VERSION), ACPI_WIDTH, __DATE__, ACPICA_COPYRIGHT

#define ACPI_COMMON_HEADER(UtilityName, Prefix)                                \
  "%s%s\n%s%s version %8.8X%s [%s]\n%s%s\n%s\n", Prefix, ACPICA_NAME, Prefix,  \
      UtilityName, ((UINT32)ACPI_CA_VERSION), ACPI_WIDTH, __DATE__, Prefix,    \
      ACPICA_COPYRIGHT, Prefix

/* Macros for usage messages */

#define ACPI_USAGE_HEADER(Usage) AcpiOsPrintf("Usage: %s\nOptions:\n", Usage);

#define ACPI_USAGE_TEXT(Description) AcpiOsPrintf(Description);

#define ACPI_OPTION(Name, Description)                                         \
  AcpiOsPrintf("  %-18s%s\n", Name, Description);

#define FILE_SUFFIX_DISASSEMBLY "dsl"
#define ACPI_TABLE_FILE_SUFFIX ".dat"

/*
 * getopt
 */
int AcpiGetopt(int argc, char **argv, char *opts);

int AcpiGetoptArgument(int argc, char **argv);

extern int AcpiGbl_Optind;
extern int AcpiGbl_Opterr;
extern int AcpiGbl_SubOptChar;
extern char *AcpiGbl_Optarg;

/*
 * cmfsize - Common get file size function
 */
UINT32
CmGetFileSize(ACPI_FILE File);

#ifndef ACPI_DUMP_APP
/*
 * adisasm
 */
ACPI_STATUS
AdAmlDisassemble(BOOLEAN OutToFile, char *Filename, char *Prefix,
                 char **OutFilename);

void AdPrintStatistics(void);

ACPI_STATUS
AdFindDsdt(UINT8 **DsdtPtr, UINT32 *DsdtLength);

void AdDumpTables(void);

ACPI_STATUS
AdGetLocalTables(void);

ACPI_STATUS
AdParseTable(ACPI_TABLE_HEADER *Table, ACPI_OWNER_ID *OwnerId,
             BOOLEAN LoadTable, BOOLEAN External);

ACPI_STATUS
AdDisplayTables(char *Filename, ACPI_TABLE_HEADER *Table);

ACPI_STATUS
AdDisplayStatistics(void);

/*
 * adwalk
 */
void AcpiDmCrossReferenceNamespace(ACPI_PARSE_OBJECT *ParseTreeRoot,
                                   ACPI_NAMESPACE_NODE *NamespaceRoot,
                                   ACPI_OWNER_ID OwnerId);

void AcpiDmDumpTree(ACPI_PARSE_OBJECT *Origin);

void AcpiDmFindOrphanMethods(ACPI_PARSE_OBJECT *Origin);

void AcpiDmFinishNamespaceLoad(ACPI_PARSE_OBJECT *ParseTreeRoot,
                               ACPI_NAMESPACE_NODE *NamespaceRoot,
                               ACPI_OWNER_ID OwnerId);

void AcpiDmConvertResourceIndexes(ACPI_PARSE_OBJECT *ParseTreeRoot,
                                  ACPI_NAMESPACE_NODE *NamespaceRoot);

/*
 * adfile
 */
ACPI_STATUS
AdInitialize(void);

char *FlGenerateFilename(char *InputFilename, char *Suffix);

ACPI_STATUS
FlSplitInputPathname(char *InputPath, char **OutDirectoryPath,
                     char **OutFilename);

char *AdGenerateFilename(char *Prefix, char *TableId);

void AdWriteTable(ACPI_TABLE_HEADER *Table, UINT32 Length, char *TableName,
                  char *OemTableId);
#endif

#endif /* _ACAPPS */

/**
 * @section drivers_power_acpi_include_acbuffer_h
 * @brief Original file: minix/drivers/power/acpi/include/acbuffer.h
 */
/******************************************************************************
 *
 * Name: acbuffer.h - Support for buffers returned by ACPI predefined names
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
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 */

#ifndef __ACBUFFER_H__
#define __ACBUFFER_H__

/*
 * Contains buffer structures for these predefined names:
 * _FDE, _GRT, _GTM, _PLD, _SRT
 */

/*
 * Note: C bitfields are not used for this reason:
 *
 * "Bitfields are great and easy to read, but unfortunately the C language
 * does not specify the layout of bitfields in memory, which means they are
 * essentially useless for dealing with packed data in on-disk formats or
 * binary wire protocols." (Or ACPI tables and buffers.) "If you ask me,
 * this decision was a design error in C. Ritchie could have picked an order
 * and stuck with it." Norman Ramsey.
 * See http://stackoverflow.com/a/1053662/41661
 */

/* _FDE return value */

typedef struct acpi_fde_info {
  UINT32 Floppy0;
  UINT32 Floppy1;
  UINT32 Floppy2;
  UINT32 Floppy3;
  UINT32 Tape;

} ACPI_FDE_INFO;

/*
 * _GRT return value
 * _SRT input value
 */
typedef struct acpi_grt_info {
  UINT16 Year;
  UINT8 Month;
  UINT8 Day;
  UINT8 Hour;
  UINT8 Minute;
  UINT8 Second;
  UINT8 Valid;
  UINT16 Milliseconds;
  UINT16 Timezone;
  UINT8 Daylight;
  UINT8 Reserved[3];

} ACPI_GRT_INFO;

/* _GTM return value */

typedef struct acpi_gtm_info {
  UINT32 PioSpeed0;
  UINT32 DmaSpeed0;
  UINT32 PioSpeed1;
  UINT32 DmaSpeed1;
  UINT32 Flags;

} ACPI_GTM_INFO;

/*
 * Formatted _PLD return value. The minimum size is a package containing
 * one buffer.
 * Revision 1: Buffer is 16 bytes (128 bits)
 * Revision 2: Buffer is 20 bytes (160 bits)
 *
 * Note: This structure is returned from the AcpiDecodePldBuffer
 * interface.
 */
typedef struct acpi_pld_info {
  UINT8 Revision;
  UINT8 IgnoreColor;
  UINT8 Red;
  UINT8 Green;
  UINT8 Blue;
  UINT16 Width;
  UINT16 Height;
  UINT8 UserVisible;
  UINT8 Dock;
  UINT8 Lid;
  UINT8 Panel;
  UINT8 VerticalPosition;
  UINT8 HorizontalPosition;
  UINT8 Shape;
  UINT8 GroupOrientation;
  UINT8 GroupToken;
  UINT8 GroupPosition;
  UINT8 Bay;
  UINT8 Ejectable;
  UINT8 OspmEjectRequired;
  UINT8 CabinetNumber;
  UINT8 CardCageNumber;
  UINT8 Reference;
  UINT8 Rotation;
  UINT8 Order;
  UINT8 Reserved;
  UINT16 VerticalOffset;
  UINT16 HorizontalOffset;

} ACPI_PLD_INFO;

/*
 * Macros to:
 *     1) Convert a _PLD buffer to internal ACPI_PLD_INFO format - ACPI_PLD_GET*
 *        (Used by AcpiDecodePldBuffer)
 *     2) Construct a _PLD buffer - ACPI_PLD_SET*
 *        (Intended for BIOS use only)
 */
#define ACPI_PLD_REV1_BUFFER_SIZE                                              \
  16 /* For Revision 1 of the buffer (From ACPI spec) */
#define ACPI_PLD_BUFFER_SIZE                                                   \
  20 /* For Revision 2 of the buffer (From ACPI spec) */

/* First 32-bit dword, bits 0:32 */

#define ACPI_PLD_GET_REVISION(dword) ACPI_GET_BITS(dword, 0, ACPI_7BIT_MASK)
#define ACPI_PLD_SET_REVISION(dword, value)                                    \
  ACPI_SET_BITS(dword, 0, ACPI_7BIT_MASK, value) /* Offset 0, Len 7 */

#define ACPI_PLD_GET_IGNORE_COLOR(dword) ACPI_GET_BITS(dword, 7, ACPI_1BIT_MASK)
#define ACPI_PLD_SET_IGNORE_COLOR(dword, value)                                \
  ACPI_SET_BITS(dword, 7, ACPI_1BIT_MASK, value) /* Offset 7, Len 1 */

#define ACPI_PLD_GET_RED(dword) ACPI_GET_BITS(dword, 8, ACPI_8BIT_MASK)
#define ACPI_PLD_SET_RED(dword, value)                                         \
  ACPI_SET_BITS(dword, 8, ACPI_8BIT_MASK, value) /* Offset 8, Len 8 */

#define ACPI_PLD_GET_GREEN(dword) ACPI_GET_BITS(dword, 16, ACPI_8BIT_MASK)
#define ACPI_PLD_SET_GREEN(dword, value)                                       \
  ACPI_SET_BITS(dword, 16, ACPI_8BIT_MASK, value) /* Offset 16, Len 8 */

#define ACPI_PLD_GET_BLUE(dword) ACPI_GET_BITS(dword, 24, ACPI_8BIT_MASK)
#define ACPI_PLD_SET_BLUE(dword, value)                                        \
  ACPI_SET_BITS(dword, 24, ACPI_8BIT_MASK, value) /* Offset 24, Len 8 */

/* Second 32-bit dword, bits 33:63 */

#define ACPI_PLD_GET_WIDTH(dword) ACPI_GET_BITS(dword, 0, ACPI_16BIT_MASK)
#define ACPI_PLD_SET_WIDTH(dword, value)                                       \
  ACPI_SET_BITS(dword, 0, ACPI_16BIT_MASK, value) /* Offset 32+0=32, Len 16 */

#define ACPI_PLD_GET_HEIGHT(dword) ACPI_GET_BITS(dword, 16, ACPI_16BIT_MASK)
#define ACPI_PLD_SET_HEIGHT(dword, value)                                      \
  ACPI_SET_BITS(dword, 16, ACPI_16BIT_MASK, value) /* Offset 32+16=48, Len 16  \
                                                    */

/* Third 32-bit dword, bits 64:95 */

#define ACPI_PLD_GET_USER_VISIBLE(dword) ACPI_GET_BITS(dword, 0, ACPI_1BIT_MASK)
#define ACPI_PLD_SET_USER_VISIBLE(dword, value)                                \
  ACPI_SET_BITS(dword, 0, ACPI_1BIT_MASK, value) /* Offset 64+0=64, Len 1 */

#define ACPI_PLD_GET_DOCK(dword) ACPI_GET_BITS(dword, 1, ACPI_1BIT_MASK)
#define ACPI_PLD_SET_DOCK(dword, value)                                        \
  ACPI_SET_BITS(dword, 1, ACPI_1BIT_MASK, value) /* Offset 64+1=65, Len 1 */

#define ACPI_PLD_GET_LID(dword) ACPI_GET_BITS(dword, 2, ACPI_1BIT_MASK)
#define ACPI_PLD_SET_LID(dword, value)                                         \
  ACPI_SET_BITS(dword, 2, ACPI_1BIT_MASK, value) /* Offset 64+2=66, Len 1 */

#define ACPI_PLD_GET_PANEL(dword) ACPI_GET_BITS(dword, 3, ACPI_3BIT_MASK)
#define ACPI_PLD_SET_PANEL(dword, value)                                       \
  ACPI_SET_BITS(dword, 3, ACPI_3BIT_MASK, value) /* Offset 64+3=67, Len 3 */

#define ACPI_PLD_GET_VERTICAL(dword) ACPI_GET_BITS(dword, 6, ACPI_2BIT_MASK)
#define ACPI_PLD_SET_VERTICAL(dword, value)                                    \
  ACPI_SET_BITS(dword, 6, ACPI_2BIT_MASK, value) /* Offset 64+6=70, Len 2 */

#define ACPI_PLD_GET_HORIZONTAL(dword) ACPI_GET_BITS(dword, 8, ACPI_2BIT_MASK)
#define ACPI_PLD_SET_HORIZONTAL(dword, value)                                  \
  ACPI_SET_BITS(dword, 8, ACPI_2BIT_MASK, value) /* Offset 64+8=72, Len 2 */

#define ACPI_PLD_GET_SHAPE(dword) ACPI_GET_BITS(dword, 10, ACPI_4BIT_MASK)
#define ACPI_PLD_SET_SHAPE(dword, value)                                       \
  ACPI_SET_BITS(dword, 10, ACPI_4BIT_MASK, value) /* Offset 64+10=74, Len 4 */

#define ACPI_PLD_GET_ORIENTATION(dword) ACPI_GET_BITS(dword, 14, ACPI_1BIT_MASK)
#define ACPI_PLD_SET_ORIENTATION(dword, value)                                 \
  ACPI_SET_BITS(dword, 14, ACPI_1BIT_MASK, value) /* Offset 64+14=78, Len 1 */

#define ACPI_PLD_GET_TOKEN(dword) ACPI_GET_BITS(dword, 15, ACPI_8BIT_MASK)
#define ACPI_PLD_SET_TOKEN(dword, value)                                       \
  ACPI_SET_BITS(dword, 15, ACPI_8BIT_MASK, value) /* Offset 64+15=79, Len 8 */

#define ACPI_PLD_GET_POSITION(dword) ACPI_GET_BITS(dword, 23, ACPI_8BIT_MASK)
#define ACPI_PLD_SET_POSITION(dword, value)                                    \
  ACPI_SET_BITS(dword, 23, ACPI_8BIT_MASK, value) /* Offset 64+23=87, Len 8 */

#define ACPI_PLD_GET_BAY(dword) ACPI_GET_BITS(dword, 31, ACPI_1BIT_MASK)
#define ACPI_PLD_SET_BAY(dword, value)                                         \
  ACPI_SET_BITS(dword, 31, ACPI_1BIT_MASK, value) /* Offset 64+31=95, Len 1 */

/* Fourth 32-bit dword, bits 96:127 */

#define ACPI_PLD_GET_EJECTABLE(dword) ACPI_GET_BITS(dword, 0, ACPI_1BIT_MASK)
#define ACPI_PLD_SET_EJECTABLE(dword, value)                                   \
  ACPI_SET_BITS(dword, 0, ACPI_1BIT_MASK, value) /* Offset 96+0=96, Len 1 */

#define ACPI_PLD_GET_OSPM_EJECT(dword) ACPI_GET_BITS(dword, 1, ACPI_1BIT_MASK)
#define ACPI_PLD_SET_OSPM_EJECT(dword, value)                                  \
  ACPI_SET_BITS(dword, 1, ACPI_1BIT_MASK, value) /* Offset 96+1=97, Len 1 */

#define ACPI_PLD_GET_CABINET(dword) ACPI_GET_BITS(dword, 2, ACPI_8BIT_MASK)
#define ACPI_PLD_SET_CABINET(dword, value)                                     \
  ACPI_SET_BITS(dword, 2, ACPI_8BIT_MASK, value) /* Offset 96+2=98, Len 8 */

#define ACPI_PLD_GET_CARD_CAGE(dword) ACPI_GET_BITS(dword, 10, ACPI_8BIT_MASK)
#define ACPI_PLD_SET_CARD_CAGE(dword, value)                                   \
  ACPI_SET_BITS(dword, 10, ACPI_8BIT_MASK, value) /* Offset 96+10=106, Len 8   \
                                                   */

#define ACPI_PLD_GET_REFERENCE(dword) ACPI_GET_BITS(dword, 18, ACPI_1BIT_MASK)
#define ACPI_PLD_SET_REFERENCE(dword, value)                                   \
  ACPI_SET_BITS(dword, 18, ACPI_1BIT_MASK, value) /* Offset 96+18=114, Len 1   \
                                                   */

#define ACPI_PLD_GET_ROTATION(dword) ACPI_GET_BITS(dword, 19, ACPI_4BIT_MASK)
#define ACPI_PLD_SET_ROTATION(dword, value)                                    \
  ACPI_SET_BITS(dword, 19, ACPI_4BIT_MASK, value) /* Offset 96+19=115, Len 4   \
                                                   */

#define ACPI_PLD_GET_ORDER(dword) ACPI_GET_BITS(dword, 23, ACPI_5BIT_MASK)
#define ACPI_PLD_SET_ORDER(dword, value)                                       \
  ACPI_SET_BITS(dword, 23, ACPI_5BIT_MASK, value) /* Offset 96+23=119, Len 5   \
                                                   */

/* Fifth 32-bit dword, bits 128:159 (Revision 2 of _PLD only) */

#define ACPI_PLD_GET_VERT_OFFSET(dword) ACPI_GET_BITS(dword, 0, ACPI_16BIT_MASK)
#define ACPI_PLD_SET_VERT_OFFSET(dword, value)                                 \
  ACPI_SET_BITS(dword, 0, ACPI_16BIT_MASK, value) /* Offset 128+0=128, Len 16  \
                                                   */

#define ACPI_PLD_GET_HORIZ_OFFSET(dword)                                       \
  ACPI_GET_BITS(dword, 16, ACPI_16BIT_MASK)
#define ACPI_PLD_SET_HORIZ_OFFSET(dword, value)                                \
  ACPI_SET_BITS(dword, 16, ACPI_16BIT_MASK,                                    \
                value) /* Offset 128+16=144, Len 16 */

#endif /* ACBUFFER_H */
/**
 * @section kernel_uts_intel_sys_acpi_accommon_h
 * @brief Original file: minix/kernel/uts/intel/sys/acpi/accommon.h
 */
/******************************************************************************
 *
 * Name: accommon.h - Common include files for generation of ACPICA source
 *
 *****************************************************************************/

/******************************************************************************
 *
 * 1. Copyright Notice
 *
 * Some or all of this work - Copyright (c) 1999 - 2018, Intel Corp.
 * All rights reserved.
 *
 * 2. License
 *
 * 2.1. This is your license from Intel Corp. under its intellectual property
 * rights. You may have additional license terms from the party that provided
 * you this software, covering your right to use that party's intellectual
 * property rights.
 *
 * 2.2. Intel grants, free of charge, to any person ("Licensee") obtaining a
 * copy of the source code appearing in this file ("Covered Code") an
 * irrevocable, perpetual, worldwide license under Intel's copyrights in the
 * base code distributed originally by Intel ("Original Intel Code") to copy,
 * make derivatives, distribute, use and display any portion of the Covered
 * Code in any form, with the right to sublicense such rights; and
 *
 * 2.3. Intel grants Licensee a non-exclusive and non-transferable patent
 * license (with the right to sublicense), under only those claims of Intel
 * patents that are infringed by the Original Intel Code, to make, use, sell,
 * offer to sell, and import the Covered Code and derivative works thereof
 * solely to the minimum extent necessary to exercise the above copyright
 * license, and in no event shall the patent license extend to any additions
 * to or modifications of the Original Intel Code. No other license or right
 * is granted directly or by implication, estoppel or otherwise;
 *
 * The above copyright and patent license is granted only if the following
 * conditions are met:
 *
 * 3. Conditions
 *
 * 3.1. Redistribution of Source with Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification with rights to further distribute source must include
 * the above Copyright Notice, the above License, this list of Conditions,
 * and the following Disclaimer and Export Compliance provision. In addition,
 * Licensee must cause all Covered Code to which Licensee contributes to
 * contain a file documenting the changes Licensee made to create that Covered
 * Code and the date of any change. Licensee must include in that file the
 * documentation of any changes made by any predecessor Licensee. Licensee
 * must include a prominent statement that the modification is derived,
 * directly or indirectly, from Original Intel Code.
 *
 * 3.2. Redistribution of Source with no Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification without rights to further distribute source must
 * include the following Disclaimer and Export Compliance provision in the
 * documentation and/or other materials provided with distribution. In
 * addition, Licensee may not authorize further sublicense of source of any
 * portion of the Covered Code, and must include terms to the effect that the
 * license from Licensee to its licensee is limited to the intellectual
 * property embodied in the software Licensee provides to its licensee, and
 * not to intellectual property embodied in modifications its licensee may
 * make.
 *
 * 3.3. Redistribution of Executable. Redistribution in executable form of any
 * substantial portion of the Covered Code or modification must reproduce the
 * above Copyright Notice, and the following Disclaimer and Export Compliance
 * provision in the documentation and/or other materials provided with the
 * distribution.
 *
 * 3.4. Intel retains all right, title, and interest in and to the Original
 * Intel Code.
 *
 * 3.5. Neither the name Intel nor any other trademark owned or controlled by
 * Intel shall be used in advertising or otherwise to promote the sale, use or
 * other dealings in products derived from or relating to the Covered Code
 * without prior written authorization from Intel.
 *
 * 4. Disclaimer and Export Compliance
 *
 * 4.1. INTEL MAKES NO WARRANTY OF ANY KIND REGARDING ANY SOFTWARE PROVIDED
 * HERE. ANY SOFTWARE ORIGINATING FROM INTEL OR DERIVED FROM INTEL SOFTWARE
 * IS PROVIDED "AS IS," AND INTEL WILL NOT PROVIDE ANY SUPPORT, ASSISTANCE,
 * INSTALLATION, TRAINING OR OTHER SERVICES. INTEL WILL NOT PROVIDE ANY
 * UPDATES, ENHANCEMENTS OR EXTENSIONS. INTEL SPECIFICALLY DISCLAIMS ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * 4.2. IN NO EVENT SHALL INTEL HAVE ANY LIABILITY TO LICENSEE, ITS LICENSEES
 * OR ANY OTHER THIRD PARTY, FOR ANY LOST PROFITS, LOST DATA, LOSS OF USE OR
 * COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, OR FOR ANY INDIRECT,
 * SPECIAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THIS AGREEMENT, UNDER ANY
 * CAUSE OF ACTION OR THEORY OF LIABILITY, AND IRRESPECTIVE OF WHETHER INTEL
 * HAS ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES. THESE LIMITATIONS
 * SHALL APPLY NOTWITHSTANDING THE FAILURE OF THE ESSENTIAL PURPOSE OF ANY
 * LIMITED REMEDY.
 *
 * 4.3. Licensee shall not export, either directly or indirectly, any of this
 * software or system incorporating such software without first obtaining any
 * required license or other approval from the U. S. Department of Commerce or
 * any other agency or department of the United States Government. In the
 * event Licensee exports any such software from the United States or
 * re-exports any such software from a foreign destination, Licensee shall
 * ensure that the distribution and export/re-export of the software is in
 * compliance with all laws, regulations, orders, or other restrictions of the
 * U.S. Export Administration Regulations. Licensee agrees that neither it nor
 * any of its subsidiaries will export/re-export any technical data, process,
 * software, or service, directly or indirectly, to any country for which the
 * United States government or any agency thereof requires an export license,
 * other governmental approval, or letter of assurance, without first obtaining
 * such license, approval or letter.
 *
 *****************************************************************************
 *
 * Alternatively, you may choose to be licensed under the terms of the
 * following license:
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Alternatively, you may choose to be licensed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 *****************************************************************************/

#ifndef __ACCOMMON_H__
#define __ACCOMMON_H__

/*
 * Common set of includes for all ACPICA source files.
 * We put them here because we don't want to duplicate them
 * in the the source code again and again.
 *
 * Note: The order of these include files is important.
 */
#include "acconfig.h" /* Global configuration constants */
#include "acglobal.h" /* All global variables */
#include "achware.h"  /* Hardware defines and interfaces */
#include "aclocal.h"  /* Internal data types */
#include "acmacros.h" /* C macros */
#include "acobject.h" /* ACPI internal object */
#include "acstruct.h" /* Common structures */
#include "acutils.h"  /* Utility interfaces */
#ifndef ACPI_USE_SYSTEM_CLIBRARY
#include "acclib.h" /* C library interfaces */
#endif              /* !ACPI_USE_SYSTEM_CLIBRARY */

#endif /* __ACCOMMON_H__ */
/**
 * @section kernel_uts_intel_sys_acpi_acconfig_h
 * @brief Original file: minix/kernel/uts/intel/sys/acpi/acconfig.h
 */
/******************************************************************************
 *
 * Name: acconfig.h - Global configuration constants
 *
 *****************************************************************************/

/******************************************************************************
 *
 * 1. Copyright Notice
 *
 * Some or all of this work - Copyright (c) 1999 - 2018, Intel Corp.
 * All rights reserved.
 *
 * 2. License
 *
 * 2.1. This is your license from Intel Corp. under its intellectual property
 * rights. You may have additional license terms from the party that provided
 * you this software, covering your right to use that party's intellectual
 * property rights.
 *
 * 2.2. Intel grants, free of charge, to any person ("Licensee") obtaining a
 * copy of the source code appearing in this file ("Covered Code") an
 * irrevocable, perpetual, worldwide license under Intel's copyrights in the
 * base code distributed originally by Intel ("Original Intel Code") to copy,
 * make derivatives, distribute, use and display any portion of the Covered
 * Code in any form, with the right to sublicense such rights; and
 *
 * 2.3. Intel grants Licensee a non-exclusive and non-transferable patent
 * license (with the right to sublicense), under only those claims of Intel
 * patents that are infringed by the Original Intel Code, to make, use, sell,
 * offer to sell, and import the Covered Code and derivative works thereof
 * solely to the minimum extent necessary to exercise the above copyright
 * license, and in no event shall the patent license extend to any additions
 * to or modifications of the Original Intel Code. No other license or right
 * is granted directly or by implication, estoppel or otherwise;
 *
 * The above copyright and patent license is granted only if the following
 * conditions are met:
 *
 * 3. Conditions
 *
 * 3.1. Redistribution of Source with Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification with rights to further distribute source must include
 * the above Copyright Notice, the above License, this list of Conditions,
 * and the following Disclaimer and Export Compliance provision. In addition,
 * Licensee must cause all Covered Code to which Licensee contributes to
 * contain a file documenting the changes Licensee made to create that Covered
 * Code and the date of any change. Licensee must include in that file the
 * documentation of any changes made by any predecessor Licensee. Licensee
 * must include a prominent statement that the modification is derived,
 * directly or indirectly, from Original Intel Code.
 *
 * 3.2. Redistribution of Source with no Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification without rights to further distribute source must
 * include the following Disclaimer and Export Compliance provision in the
 * documentation and/or other materials provided with distribution. In
 * addition, Licensee may not authorize further sublicense of source of any
 * portion of the Covered Code, and must include terms to the effect that the
 * license from Licensee to its licensee is limited to the intellectual
 * property embodied in the software Licensee provides to its licensee, and
 * not to intellectual property embodied in modifications its licensee may
 * make.
 *
 * 3.3. Redistribution of Executable. Redistribution in executable form of any
 * substantial portion of the Covered Code or modification must reproduce the
 * above Copyright Notice, and the following Disclaimer and Export Compliance
 * provision in the documentation and/or other materials provided with the
 * distribution.
 *
 * 3.4. Intel retains all right, title, and interest in and to the Original
 * Intel Code.
 *
 * 3.5. Neither the name Intel nor any other trademark owned or controlled by
 * Intel shall be used in advertising or otherwise to promote the sale, use or
 * other dealings in products derived from or relating to the Covered Code
 * without prior written authorization from Intel.
 *
 * 4. Disclaimer and Export Compliance
 *
 * 4.1. INTEL MAKES NO WARRANTY OF ANY KIND REGARDING ANY SOFTWARE PROVIDED
 * HERE. ANY SOFTWARE ORIGINATING FROM INTEL OR DERIVED FROM INTEL SOFTWARE
 * IS PROVIDED "AS IS," AND INTEL WILL NOT PROVIDE ANY SUPPORT, ASSISTANCE,
 * INSTALLATION, TRAINING OR OTHER SERVICES. INTEL WILL NOT PROVIDE ANY
 * UPDATES, ENHANCEMENTS OR EXTENSIONS. INTEL SPECIFICALLY DISCLAIMS ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * 4.2. IN NO EVENT SHALL INTEL HAVE ANY LIABILITY TO LICENSEE, ITS LICENSEES
 * OR ANY OTHER THIRD PARTY, FOR ANY LOST PROFITS, LOST DATA, LOSS OF USE OR
 * COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, OR FOR ANY INDIRECT,
 * SPECIAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THIS AGREEMENT, UNDER ANY
 * CAUSE OF ACTION OR THEORY OF LIABILITY, AND IRRESPECTIVE OF WHETHER INTEL
 * HAS ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES. THESE LIMITATIONS
 * SHALL APPLY NOTWITHSTANDING THE FAILURE OF THE ESSENTIAL PURPOSE OF ANY
 * LIMITED REMEDY.
 *
 * 4.3. Licensee shall not export, either directly or indirectly, any of this
 * software or system incorporating such software without first obtaining any
 * required license or other approval from the U. S. Department of Commerce or
 * any other agency or department of the United States Government. In the
 * event Licensee exports any such software from the United States or
 * re-exports any such software from a foreign destination, Licensee shall
 * ensure that the distribution and export/re-export of the software is in
 * compliance with all laws, regulations, orders, or other restrictions of the
 * U.S. Export Administration Regulations. Licensee agrees that neither it nor
 * any of its subsidiaries will export/re-export any technical data, process,
 * software, or service, directly or indirectly, to any country for which the
 * United States government or any agency thereof requires an export license,
 * other governmental approval, or letter of assurance, without first obtaining
 * such license, approval or letter.
 *
 *****************************************************************************
 *
 * Alternatively, you may choose to be licensed under the terms of the
 * following license:
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Alternatively, you may choose to be licensed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 *****************************************************************************/

#ifndef _ACCONFIG_H
#define _ACCONFIG_H

/******************************************************************************
 *
 * Configuration options
 *
 *****************************************************************************/

/*
 * ACPI_DEBUG_OUTPUT    - This switch enables all the debug facilities of the
 *                        ACPI subsystem. This includes the DEBUG_PRINT output
 *                        statements. When disabled, all DEBUG_PRINT
 *                        statements are compiled out.
 *
 * ACPI_APPLICATION     - Use this switch if the subsystem is going to be run
 *                        at the application level.
 *
 */

/*
 * OS name, used for the _OS object. The _OS object is essentially obsolete,
 * but there is a large base of ASL/AML code in existing machines that check
 * for the string below. The use of this string usually guarantees that
 * the ASL will execute down the most tested code path. Also, there is some
 * code that will not execute the _OSI method unless _OS matches the string
 * below. Therefore, change this string at your own risk.
 */
#define ACPI_OS_NAME "Microsoft Windows NT"

/* Maximum objects in the various object caches */

#define ACPI_MAX_STATE_CACHE_DEPTH 96     /* State objects */
#define ACPI_MAX_PARSE_CACHE_DEPTH 96     /* Parse tree objects */
#define ACPI_MAX_EXTPARSE_CACHE_DEPTH 96  /* Parse tree objects */
#define ACPI_MAX_OBJECT_CACHE_DEPTH 96    /* Interpreter operand objects */
#define ACPI_MAX_NAMESPACE_CACHE_DEPTH 96 /* Namespace objects */
#define ACPI_MAX_COMMENT_CACHE_DEPTH 96   /* Comments for the -ca option */

/*
 * Should the subsystem abort the loading of an ACPI table if the
 * table checksum is incorrect?
 */
#ifndef ACPI_CHECKSUM_ABORT
#define ACPI_CHECKSUM_ABORT FALSE
#endif

/*
 * Generate a version of ACPICA that only supports "reduced hardware"
 * platforms (as defined in ACPI 5.0). Set to TRUE to generate a specialized
 * version of ACPICA that ONLY supports the ACPI 5.0 "reduced hardware"
 * model. In other words, no ACPI hardware is supported.
 *
 * If TRUE, this means no support for the following:
 *      PM Event and Control registers
 *      SCI interrupt (and handler)
 *      Fixed Events
 *      General Purpose Events (GPEs)
 *      Global Lock
 *      ACPI PM timer
 *      FACS table (Waking vectors and Global Lock)
 */
#ifndef ACPI_REDUCED_HARDWARE
#define ACPI_REDUCED_HARDWARE FALSE
#endif

/******************************************************************************
 *
 * Subsystem Constants
 *
 *****************************************************************************/

/* Version of ACPI supported */

#define ACPI_CA_SUPPORT_LEVEL 5

/* Maximum count for a semaphore object */

#define ACPI_MAX_SEMAPHORE_COUNT 256

/* Maximum object reference count (detects object deletion issues) */

#define ACPI_MAX_REFERENCE_COUNT 0x800

/* Default page size for use in mapping memory for operation regions */

#define ACPI_DEFAULT_PAGE_SIZE 4096 /* Must be power of 2 */

/* OwnerId tracking. 8 entries allows for 255 OwnerIds */

#define ACPI_NUM_OWNERID_MASKS 8

/* Size of the root table array is increased by this increment */

#define ACPI_ROOT_TABLE_SIZE_INCREMENT 4

/* Maximum sleep allowed via Sleep() operator */

#define ACPI_MAX_SLEEP 2000 /* 2000 millisec == two seconds */

/* Address Range lists are per-SpaceId (Memory and I/O only) */

#define ACPI_ADDRESS_RANGE_MAX 2

/* Maximum time (default 30s) of While() loops before abort */

#define ACPI_MAX_LOOP_TIMEOUT 30

/******************************************************************************
 *
 * ACPI Specification constants (Do not change unless the specification changes)
 *
 *****************************************************************************/

/* Method info (in WALK_STATE), containing local variables and argumetns */

#define ACPI_METHOD_NUM_LOCALS 8
#define ACPI_METHOD_MAX_LOCAL 7

#define ACPI_METHOD_NUM_ARGS 7
#define ACPI_METHOD_MAX_ARG 6

/*
 * Operand Stack (in WALK_STATE), Must be large enough to contain METHOD_MAX_ARG
 */
#define ACPI_OBJ_NUM_OPERANDS 8
#define ACPI_OBJ_MAX_OPERAND 7

/* Number of elements in the Result Stack frame, can be an arbitrary value */

#define ACPI_RESULTS_FRAME_OBJ_NUM 8

/*
 * Maximal number of elements the Result Stack can contain,
 * it may be an arbitray value not exceeding the types of
 * ResultSize and ResultCount (now UINT8).
 */
#define ACPI_RESULTS_OBJ_NUM_MAX 255

/* Constants used in searching for the RSDP in low memory */

#define ACPI_EBDA_PTR_LOCATION 0x0000040E /* Physical Address */
#define ACPI_EBDA_PTR_LENGTH 2
#define ACPI_EBDA_WINDOW_SIZE 1024
#define ACPI_HI_RSDP_WINDOW_BASE 0x000E0000 /* Physical Address */
#define ACPI_HI_RSDP_WINDOW_SIZE 0x00020000
#define ACPI_RSDP_SCAN_STEP 16

/* Operation regions */

#define ACPI_USER_REGION_BEGIN 0x80

/* Maximum SpaceIds for Operation Regions */

#define ACPI_MAX_ADDRESS_SPACE 255
#define ACPI_NUM_DEFAULT_SPACES 4

/* Array sizes. Used for range checking also */

#define ACPI_MAX_MATCH_OPCODE 5

/* RSDP checksums */

#define ACPI_RSDP_CHECKSUM_LENGTH 20
#define ACPI_RSDP_XCHECKSUM_LENGTH 36

/* SMBus, GSBus and IPMI bidirectional buffer size */

#define ACPI_SMBUS_BUFFER_SIZE 34
#define ACPI_GSBUS_BUFFER_SIZE 34
#define ACPI_IPMI_BUFFER_SIZE 66

/* _SxD and _SxW control methods */

#define ACPI_NUM_SxD_METHODS 4
#define ACPI_NUM_SxW_METHODS 5

/******************************************************************************
 *
 * Miscellaneous constants
 *
 *****************************************************************************/

/* UUID constants */

#define UUID_BUFFER_LENGTH 16 /* Length of UUID in memory */
#define UUID_STRING_LENGTH 36 /* Total length of a UUID string */

/* Positions for required hyphens (dashes) in UUID strings */

#define UUID_HYPHEN1_OFFSET 8
#define UUID_HYPHEN2_OFFSET 13
#define UUID_HYPHEN3_OFFSET 18
#define UUID_HYPHEN4_OFFSET 23

/******************************************************************************
 *
 * ACPI AML Debugger
 *
 *****************************************************************************/

#define ACPI_DEBUGGER_MAX_ARGS                                                 \
  ACPI_METHOD_NUM_ARGS + 4 /* Max command line arguments */
#define ACPI_DB_LINE_BUFFER_SIZE 512

#define ACPI_DEBUGGER_COMMAND_PROMPT '-'
#define ACPI_DEBUGGER_EXECUTE_PROMPT '%'

#endif /* _ACCONFIG_H */
/**
 * @section kernel_uts_intel_sys_acpi_acdebug_h
 * @brief Original file: minix/kernel/uts/intel/sys/acpi/acdebug.h
 */
/******************************************************************************
 *
 * Name: acdebug.h - ACPI/AML debugger
 *
 *****************************************************************************/

/******************************************************************************
 *
 * 1. Copyright Notice
 *
 * Some or all of this work - Copyright (c) 1999 - 2018, Intel Corp.
 * All rights reserved.
 *
 * 2. License
 *
 * 2.1. This is your license from Intel Corp. under its intellectual property
 * rights. You may have additional license terms from the party that provided
 * you this software, covering your right to use that party's intellectual
 * property rights.
 *
 * 2.2. Intel grants, free of charge, to any person ("Licensee") obtaining a
 * copy of the source code appearing in this file ("Covered Code") an
 * irrevocable, perpetual, worldwide license under Intel's copyrights in the
 * base code distributed originally by Intel ("Original Intel Code") to copy,
 * make derivatives, distribute, use and display any portion of the Covered
 * Code in any form, with the right to sublicense such rights; and
 *
 * 2.3. Intel grants Licensee a non-exclusive and non-transferable patent
 * license (with the right to sublicense), under only those claims of Intel
 * patents that are infringed by the Original Intel Code, to make, use, sell,
 * offer to sell, and import the Covered Code and derivative works thereof
 * solely to the minimum extent necessary to exercise the above copyright
 * license, and in no event shall the patent license extend to any additions
 * to or modifications of the Original Intel Code. No other license or right
 * is granted directly or by implication, estoppel or otherwise;
 *
 * The above copyright and patent license is granted only if the following
 * conditions are met:
 *
 * 3. Conditions
 *
 * 3.1. Redistribution of Source with Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification with rights to further distribute source must include
 * the above Copyright Notice, the above License, this list of Conditions,
 * and the following Disclaimer and Export Compliance provision. In addition,
 * Licensee must cause all Covered Code to which Licensee contributes to
 * contain a file documenting the changes Licensee made to create that Covered
 * Code and the date of any change. Licensee must include in that file the
 * documentation of any changes made by any predecessor Licensee. Licensee
 * must include a prominent statement that the modification is derived,
 * directly or indirectly, from Original Intel Code.
 *
 * 3.2. Redistribution of Source with no Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification without rights to further distribute source must
 * include the following Disclaimer and Export Compliance provision in the
 * documentation and/or other materials provided with distribution. In
 * addition, Licensee may not authorize further sublicense of source of any
 * portion of the Covered Code, and must include terms to the effect that the
 * license from Licensee to its licensee is limited to the intellectual
 * property embodied in the software Licensee provides to its licensee, and
 * not to intellectual property embodied in modifications its licensee may
 * make.
 *
 * 3.3. Redistribution of Executable. Redistribution in executable form of any
 * substantial portion of the Covered Code or modification must reproduce the
 * above Copyright Notice, and the following Disclaimer and Export Compliance
 * provision in the documentation and/or other materials provided with the
 * distribution.
 *
 * 3.4. Intel retains all right, title, and interest in and to the Original
 * Intel Code.
 *
 * 3.5. Neither the name Intel nor any other trademark owned or controlled by
 * Intel shall be used in advertising or otherwise to promote the sale, use or
 * other dealings in products derived from or relating to the Covered Code
 * without prior written authorization from Intel.
 *
 * 4. Disclaimer and Export Compliance
 *
 * 4.1. INTEL MAKES NO WARRANTY OF ANY KIND REGARDING ANY SOFTWARE PROVIDED
 * HERE. ANY SOFTWARE ORIGINATING FROM INTEL OR DERIVED FROM INTEL SOFTWARE
 * IS PROVIDED "AS IS," AND INTEL WILL NOT PROVIDE ANY SUPPORT, ASSISTANCE,
 * INSTALLATION, TRAINING OR OTHER SERVICES. INTEL WILL NOT PROVIDE ANY
 * UPDATES, ENHANCEMENTS OR EXTENSIONS. INTEL SPECIFICALLY DISCLAIMS ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * 4.2. IN NO EVENT SHALL INTEL HAVE ANY LIABILITY TO LICENSEE, ITS LICENSEES
 * OR ANY OTHER THIRD PARTY, FOR ANY LOST PROFITS, LOST DATA, LOSS OF USE OR
 * COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, OR FOR ANY INDIRECT,
 * SPECIAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THIS AGREEMENT, UNDER ANY
 * CAUSE OF ACTION OR THEORY OF LIABILITY, AND IRRESPECTIVE OF WHETHER INTEL
 * HAS ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES. THESE LIMITATIONS
 * SHALL APPLY NOTWITHSTANDING THE FAILURE OF THE ESSENTIAL PURPOSE OF ANY
 * LIMITED REMEDY.
 *
 * 4.3. Licensee shall not export, either directly or indirectly, any of this
 * software or system incorporating such software without first obtaining any
 * required license or other approval from the U. S. Department of Commerce or
 * any other agency or department of the United States Government. In the
 * event Licensee exports any such software from the United States or
 * re-exports any such software from a foreign destination, Licensee shall
 * ensure that the distribution and export/re-export of the software is in
 * compliance with all laws, regulations, orders, or other restrictions of the
 * U.S. Export Administration Regulations. Licensee agrees that neither it nor
 * any of its subsidiaries will export/re-export any technical data, process,
 * software, or service, directly or indirectly, to any country for which the
 * United States government or any agency thereof requires an export license,
 * other governmental approval, or letter of assurance, without first obtaining
 * such license, approval or letter.
 *
 *****************************************************************************
 *
 * Alternatively, you may choose to be licensed under the terms of the
 * following license:
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Alternatively, you may choose to be licensed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 *****************************************************************************/

#ifndef __ACDEBUG_H__
#define __ACDEBUG_H__

/* The debugger is used in conjunction with the disassembler most of time */

#ifdef ACPI_DISASSEMBLER
#include "acdisasm.h"
#endif

#define ACPI_DEBUG_BUFFER_SIZE 0x4000 /* 16K buffer for return objects */

typedef struct acpi_db_command_info {
  const char *Name; /* Command Name */
  UINT8 MinArgs;    /* Minimum arguments required */

} ACPI_DB_COMMAND_INFO;

typedef struct acpi_db_command_help {
  UINT8 LineCount;   /* Number of help lines */
  char *Invocation;  /* Command Invocation */
  char *Description; /* Command Description */

} ACPI_DB_COMMAND_HELP;

typedef struct acpi_db_argument_info {
  const char *Name; /* Argument Name */

} ACPI_DB_ARGUMENT_INFO;

typedef struct acpi_db_execute_walk {
  UINT32 Count;
  UINT32 MaxCount;

} ACPI_DB_EXECUTE_WALK;

#define PARAM_LIST(pl) pl

#define EX_NO_SINGLE_STEP 1
#define EX_SINGLE_STEP 2

/*
 * dbxface - external debugger interfaces
 */
ACPI_DBR_DEPENDENT_RETURN_OK(ACPI_STATUS AcpiDbSingleStep(
    ACPI_WALK_STATE *WalkState, ACPI_PARSE_OBJECT *Op, UINT32 OpType))

ACPI_DBR_DEPENDENT_RETURN_VOID(
    void AcpiDbSignalBreakPoint(ACPI_WALK_STATE *WalkState))

/*
 * dbcmds - debug commands and output routines
 */
ACPI_NAMESPACE_NODE *AcpiDbConvertToNode(char *InString);

void AcpiDbDisplayTableInfo(char *TableArg);

void AcpiDbDisplayTemplate(char *BufferArg);

void AcpiDbUnloadAcpiTable(char *Name);

void AcpiDbSendNotify(char *Name, UINT32 Value);

void AcpiDbDisplayInterfaces(char *ActionArg, char *InterfaceNameArg);

ACPI_STATUS
AcpiDbSleep(char *ObjectArg);

void AcpiDbTrace(char *EnableArg, char *MethodArg, char *OnceArg);

void AcpiDbDisplayLocks(void);

void AcpiDbDisplayResources(char *ObjectArg);

ACPI_HW_DEPENDENT_RETURN_VOID(void AcpiDbDisplayGpes(void))

void AcpiDbDisplayHandlers(void);

ACPI_HW_DEPENDENT_RETURN_VOID(void AcpiDbGenerateGpe(char *GpeArg,
                                                     char *BlockArg))

ACPI_HW_DEPENDENT_RETURN_VOID(void AcpiDbGenerateSci(void))

void AcpiDbExecuteTest(char *TypeArg);

/*
 * dbconvert - miscellaneous conversion routines
 */
ACPI_STATUS
AcpiDbHexCharToValue(int HexChar, UINT8 *ReturnValue);

ACPI_STATUS
AcpiDbConvertToPackage(char *String, ACPI_OBJECT *Object);

ACPI_STATUS
AcpiDbConvertToObject(ACPI_OBJECT_TYPE Type, char *String, ACPI_OBJECT *Object);

UINT8 *AcpiDbEncodePldBuffer(ACPI_PLD_INFO *PldInfo);

void AcpiDbDumpPldBuffer(ACPI_OBJECT *ObjDesc);

/*
 * dbmethod - control method commands
 */
void AcpiDbSetMethodBreakpoint(char *Location, ACPI_WALK_STATE *WalkState,
                               ACPI_PARSE_OBJECT *Op);

void AcpiDbSetMethodCallBreakpoint(ACPI_PARSE_OBJECT *Op);

void AcpiDbSetMethodData(char *TypeArg, char *IndexArg, char *ValueArg);

ACPI_STATUS
AcpiDbDisassembleMethod(char *Name);

void AcpiDbDisassembleAml(char *Statements, ACPI_PARSE_OBJECT *Op);

void AcpiDbEvaluatePredefinedNames(void);

/*
 * dbnames - namespace commands
 */
void AcpiDbSetScope(char *Name);

void AcpiDbDumpNamespace(char *StartArg, char *DepthArg);

void AcpiDbDumpNamespacePaths(void);

void AcpiDbDumpNamespaceByOwner(char *OwnerArg, char *DepthArg);

ACPI_STATUS
AcpiDbFindNameInNamespace(char *NameArg);

void AcpiDbCheckPredefinedNames(void);

ACPI_STATUS
AcpiDbDisplayObjects(char *ObjTypeArg, char *DisplayCountArg);

void AcpiDbCheckIntegrity(void);

void AcpiDbFindReferences(char *ObjectArg);

void AcpiDbGetBusInfo(void);

/*
 * dbdisply - debug display commands
 */
void AcpiDbDisplayMethodInfo(ACPI_PARSE_OBJECT *Op);

void AcpiDbDecodeAndDisplayObject(char *Target, char *OutputType);

ACPI_DBR_DEPENDENT_RETURN_VOID(void AcpiDbDisplayResultObject(
    ACPI_OPERAND_OBJECT *ObjDesc, ACPI_WALK_STATE *WalkState))

ACPI_STATUS
AcpiDbDisplayAllMethods(char *DisplayCountArg);

void AcpiDbDisplayArguments(void);

void AcpiDbDisplayLocals(void);

void AcpiDbDisplayResults(void);

void AcpiDbDisplayCallingTree(void);

void AcpiDbDisplayObjectType(char *ObjectArg);

ACPI_DBR_DEPENDENT_RETURN_VOID(void AcpiDbDisplayArgumentObject(
    ACPI_OPERAND_OBJECT *ObjDesc, ACPI_WALK_STATE *WalkState))

/*
 * dbexec - debugger control method execution
 */
void AcpiDbExecute(char *Name, char **Args, ACPI_OBJECT_TYPE *Types,
                   UINT32 Flags);

void AcpiDbCreateExecutionThread(char *MethodNameArg, char **Arguments,
                                 ACPI_OBJECT_TYPE *Types);

void AcpiDbCreateExecutionThreads(char *NumThreadsArg, char *NumLoopsArg,
                                  char *MethodNameArg);

void AcpiDbDeleteObjects(UINT32 Count, ACPI_OBJECT *Objects);

#ifdef ACPI_DBG_TRACK_ALLOCATIONS
UINT32
AcpiDbGetCacheInfo(ACPI_MEMORY_LIST *Cache);
#endif

/*
 * dbfileio - Debugger file I/O commands
 */
ACPI_OBJECT_TYPE
AcpiDbMatchArgument(char *UserArgument, ACPI_DB_ARGUMENT_INFO *Arguments);

void AcpiDbCloseDebugFile(void);

void AcpiDbOpenDebugFile(char *Name);

ACPI_STATUS
AcpiDbLoadAcpiTable(char *Filename);

ACPI_STATUS
AcpiDbLoadTables(ACPI_NEW_TABLE_DESC *ListHead);

/*
 * dbhistry - debugger HISTORY command
 */
void AcpiDbAddToHistory(char *CommandLine);

void AcpiDbDisplayHistory(void);

char *AcpiDbGetFromHistory(char *CommandNumArg);

char *AcpiDbGetHistoryByIndex(UINT32 CommanddNum);

/*
 * dbinput - user front-end to the AML debugger
 */
ACPI_STATUS
AcpiDbCommandDispatch(char *InputBuffer, ACPI_WALK_STATE *WalkState,
                      ACPI_PARSE_OBJECT *Op);

void ACPI_SYSTEM_XFACE AcpiDbExecuteThread(void *Context);

ACPI_STATUS
AcpiDbUserCommands(void);

char *AcpiDbGetNextToken(char *String, char **Next,
                         ACPI_OBJECT_TYPE *ReturnType);

/*
 * dbobject
 */
void AcpiDbDecodeInternalObject(ACPI_OPERAND_OBJECT *ObjDesc);

void AcpiDbDisplayInternalObject(ACPI_OPERAND_OBJECT *ObjDesc,
                                 ACPI_WALK_STATE *WalkState);

void AcpiDbDecodeArguments(ACPI_WALK_STATE *WalkState);

void AcpiDbDecodeLocals(ACPI_WALK_STATE *WalkState);

void AcpiDbDumpMethodInfo(ACPI_STATUS Status, ACPI_WALK_STATE *WalkState);

/*
 * dbstats - Generation and display of ACPI table statistics
 */
void AcpiDbGenerateStatistics(ACPI_PARSE_OBJECT *Root, BOOLEAN IsMethod);

ACPI_STATUS
AcpiDbDisplayStatistics(char *TypeArg);

/*
 * dbutils - AML debugger utilities
 */
void AcpiDbSetOutputDestination(UINT32 Where);

void AcpiDbDumpExternalObject(ACPI_OBJECT *ObjDesc, UINT32 Level);

void AcpiDbPrepNamestring(char *Name);

ACPI_NAMESPACE_NODE *AcpiDbLocalNsLookup(char *Name);

void AcpiDbUint32ToHexString(UINT32 Value, char *Buffer);

#endif /* __ACDEBUG_H__ */

/**
 * @section minix_drivers_power_acpi_include_acdebug_h
 * @brief Original file: minix/drivers/power/acpi/include/acdebug.h
 */
/******************************************************************************
 *
 * Name: acdebug.h - ACPI/AML debugger
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
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 */

#ifndef __ACDEBUG_H__
#define __ACDEBUG_H__

#define ACPI_DEBUG_BUFFER_SIZE 0x4000 /* 16K buffer for return objects */

typedef struct acpi_db_command_info {
  char *Name;    /* Command Name */
  UINT8 MinArgs; /* Minimum arguments required */

} ACPI_DB_COMMAND_INFO;

typedef struct acpi_db_command_help {
  UINT8 LineCount;   /* Number of help lines */
  char *Invocation;  /* Command Invocation */
  char *Description; /* Command Description */

} ACPI_DB_COMMAND_HELP;

typedef struct acpi_db_argument_info {
  char *Name; /* Argument Name */

} ACPI_DB_ARGUMENT_INFO;

typedef struct acpi_db_execute_walk {
  UINT32 Count;
  UINT32 MaxCount;

} ACPI_DB_EXECUTE_WALK;

#define PARAM_LIST(pl) pl
#define DBTEST_OUTPUT_LEVEL(lvl) if (AcpiGbl_DbOpt_verbose)
#define VERBOSE_PRINT(fp)                                                      \
  DBTEST_OUTPUT_LEVEL(lvl) { AcpiOsPrintf PARAM_LIST(fp); }

#define EX_NO_SINGLE_STEP 1
#define EX_SINGLE_STEP 2

/*
 * dbxface - external debugger interfaces
 */
ACPI_STATUS
AcpiDbInitialize(void);

void AcpiDbTerminate(void);

ACPI_STATUS
AcpiDbSingleStep(ACPI_WALK_STATE *WalkState, ACPI_PARSE_OBJECT *Op,
                 UINT32 OpType);

/*
 * dbcmds - debug commands and output routines
 */
ACPI_NAMESPACE_NODE *AcpiDbConvertToNode(char *InString);

void AcpiDbDisplayTableInfo(char *TableArg);

void AcpiDbDisplayTemplate(char *BufferArg);

void AcpiDbUnloadAcpiTable(char *Name);

void AcpiDbSendNotify(char *Name, UINT32 Value);

void AcpiDbDisplayInterfaces(char *ActionArg, char *InterfaceNameArg);

ACPI_STATUS
AcpiDbSleep(char *ObjectArg);

void AcpiDbDisplayLocks(void);

void AcpiDbDisplayResources(char *ObjectArg);

ACPI_HW_DEPENDENT_RETURN_VOID(void AcpiDbDisplayGpes(void))

void AcpiDbDisplayHandlers(void);

ACPI_HW_DEPENDENT_RETURN_VOID(void AcpiDbGenerateGpe(char *GpeArg,
                                                     char *BlockArg))

ACPI_HW_DEPENDENT_RETURN_VOID(void AcpiDbGenerateSci(void))

void AcpiDbExecuteTest(char *TypeArg);

/*
 * dbconvert - miscellaneous conversion routines
 */
ACPI_STATUS
AcpiDbHexCharToValue(int HexChar, UINT8 *ReturnValue);

ACPI_STATUS
AcpiDbConvertToPackage(char *String, ACPI_OBJECT *Object);

ACPI_STATUS
AcpiDbConvertToObject(ACPI_OBJECT_TYPE Type, char *String, ACPI_OBJECT *Object);

UINT8 *AcpiDbEncodePldBuffer(ACPI_PLD_INFO *PldInfo);

void AcpiDbDumpPldBuffer(ACPI_OBJECT *ObjDesc);

/*
 * dbmethod - control method commands
 */
void AcpiDbSetMethodBreakpoint(char *Location, ACPI_WALK_STATE *WalkState,
                               ACPI_PARSE_OBJECT *Op);

void AcpiDbSetMethodCallBreakpoint(ACPI_PARSE_OBJECT *Op);

void AcpiDbSetMethodData(char *TypeArg, char *IndexArg, char *ValueArg);

ACPI_STATUS
AcpiDbDisassembleMethod(char *Name);

void AcpiDbDisassembleAml(char *Statements, ACPI_PARSE_OBJECT *Op);

void AcpiDbBatchExecute(char *CountArg);

/*
 * dbnames - namespace commands
 */
void AcpiDbSetScope(char *Name);

void AcpiDbDumpNamespace(char *StartArg, char *DepthArg);

void AcpiDbDumpNamespacePaths(void);

void AcpiDbDumpNamespaceByOwner(char *OwnerArg, char *DepthArg);

ACPI_STATUS
AcpiDbFindNameInNamespace(char *NameArg);

void AcpiDbCheckPredefinedNames(void);

ACPI_STATUS
AcpiDbDisplayObjects(char *ObjTypeArg, char *DisplayCountArg);

void AcpiDbCheckIntegrity(void);

void AcpiDbFindReferences(char *ObjectArg);

void AcpiDbGetBusInfo(void);

/*
 * dbdisply - debug display commands
 */
void AcpiDbDisplayMethodInfo(ACPI_PARSE_OBJECT *Op);

void AcpiDbDecodeAndDisplayObject(char *Target, char *OutputType);

void AcpiDbDisplayResultObject(ACPI_OPERAND_OBJECT *ObjDesc,
                               ACPI_WALK_STATE *WalkState);

ACPI_STATUS
AcpiDbDisplayAllMethods(char *DisplayCountArg);

void AcpiDbDisplayArguments(void);

void AcpiDbDisplayLocals(void);

void AcpiDbDisplayResults(void);

void AcpiDbDisplayCallingTree(void);

void AcpiDbDisplayObjectType(char *ObjectArg);

void AcpiDbDisplayArgumentObject(ACPI_OPERAND_OBJECT *ObjDesc,
                                 ACPI_WALK_STATE *WalkState);

/*
 * dbexec - debugger control method execution
 */
void AcpiDbExecute(char *Name, char **Args, ACPI_OBJECT_TYPE *Types,
                   UINT32 Flags);

void AcpiDbCreateExecutionThreads(char *NumThreadsArg, char *NumLoopsArg,
                                  char *MethodNameArg);

void AcpiDbDeleteObjects(UINT32 Count, ACPI_OBJECT *Objects);

#ifdef ACPI_DBG_TRACK_ALLOCATIONS
UINT32
AcpiDbGetCacheInfo(ACPI_MEMORY_LIST *Cache);
#endif

/*
 * dbfileio - Debugger file I/O commands
 */
ACPI_OBJECT_TYPE
AcpiDbMatchArgument(char *UserArgument, ACPI_DB_ARGUMENT_INFO *Arguments);

void AcpiDbCloseDebugFile(void);

void AcpiDbOpenDebugFile(char *Name);

ACPI_STATUS
AcpiDbLoadAcpiTable(char *Filename);

ACPI_STATUS
AcpiDbGetTableFromFile(char *Filename, ACPI_TABLE_HEADER **Table);

/*
 * dbhistry - debugger HISTORY command
 */
void AcpiDbAddToHistory(char *CommandLine);

void AcpiDbDisplayHistory(void);

char *AcpiDbGetFromHistory(char *CommandNumArg);

char *AcpiDbGetHistoryByIndex(UINT32 CommanddNum);

/*
 * dbinput - user front-end to the AML debugger
 */
ACPI_STATUS
AcpiDbCommandDispatch(char *InputBuffer, ACPI_WALK_STATE *WalkState,
                      ACPI_PARSE_OBJECT *Op);

void ACPI_SYSTEM_XFACE AcpiDbExecuteThread(void *Context);

ACPI_STATUS
AcpiDbUserCommands(char Prompt, ACPI_PARSE_OBJECT *Op);

char *AcpiDbGetNextToken(char *String, char **Next,
                         ACPI_OBJECT_TYPE *ReturnType);

/*
 * dbstats - Generation and display of ACPI table statistics
 */
void AcpiDbGenerateStatistics(ACPI_PARSE_OBJECT *Root, BOOLEAN IsMethod);

ACPI_STATUS
AcpiDbDisplayStatistics(char *TypeArg);

/*
 * dbutils - AML debugger utilities
 */
void AcpiDbSetOutputDestination(UINT32 Where);

void AcpiDbDumpExternalObject(ACPI_OBJECT *ObjDesc, UINT32 Level);

void AcpiDbPrepNamestring(char *Name);

ACPI_NAMESPACE_NODE *AcpiDbLocalNsLookup(char *Name);

void AcpiDbUint32ToHexString(UINT32 Value, char *Buffer);

#endif /* __ACDEBUG_H__ */

/**
 * @section minix_drivers_power_acpi_include_acdisasm_h
 * @brief Original file: minix/drivers/power/acpi/include/acdisasm.h
 */
/******************************************************************************
 *
 * Name: acdisasm.h - AML disassembler
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
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 */

#ifndef __ACDISASM_H__
#define __ACDISASM_H__

#include "amlresrc.h"

#define BLOCK_NONE 0
#define BLOCK_PAREN 1
#define BLOCK_BRACE 2
#define BLOCK_COMMA_LIST 4
#define ACPI_DEFAULT_RESNAME *(UINT32 *)"__RD"

/*
 * Raw table data header. Used by disassembler and data table compiler.
 * Do not change.
 */
#define ACPI_RAW_TABLE_DATA_HEADER "Raw Table Data"

typedef const struct acpi_dmtable_info {
  UINT8 Opcode;
  UINT16 Offset;
  char *Name;
  UINT8 Flags;

} ACPI_DMTABLE_INFO;

/* Values for Flags field above */

#define DT_LENGTH 0x01   /* Field is a subtable length */
#define DT_FLAG 0x02     /* Field is a flag value */
#define DT_NON_ZERO 0x04 /* Field must be non-zero */
#define DT_OPTIONAL 0x08 /* Field is optional */
#define DT_DESCRIBES_OPTIONAL                                                  \
  0x10                /* Field describes an optional field (length, etc.) */
#define DT_COUNT 0x20 /* Currently not used */

/*
 * Values for Opcode above.
 * Note: 0-7 must not change, they are used as a flag shift value. Other
 * than those, new values can be added wherever appropriate.
 */
typedef enum {
  /* Simple Data Types */

  ACPI_DMT_FLAG0 = 0,
  ACPI_DMT_FLAG1 = 1,
  ACPI_DMT_FLAG2 = 2,
  ACPI_DMT_FLAG3 = 3,
  ACPI_DMT_FLAG4 = 4,
  ACPI_DMT_FLAG5 = 5,
  ACPI_DMT_FLAG6 = 6,
  ACPI_DMT_FLAG7 = 7,
  ACPI_DMT_FLAGS0,
  ACPI_DMT_FLAGS1,
  ACPI_DMT_FLAGS2,
  ACPI_DMT_FLAGS4,
  ACPI_DMT_UINT8,
  ACPI_DMT_UINT16,
  ACPI_DMT_UINT24,
  ACPI_DMT_UINT32,
  ACPI_DMT_UINT40,
  ACPI_DMT_UINT48,
  ACPI_DMT_UINT56,
  ACPI_DMT_UINT64,
  ACPI_DMT_BUF7,
  ACPI_DMT_BUF10,
  ACPI_DMT_BUF16,
  ACPI_DMT_BUF128,
  ACPI_DMT_SIG,
  ACPI_DMT_STRING,
  ACPI_DMT_NAME4,
  ACPI_DMT_NAME6,
  ACPI_DMT_NAME8,

  /* Types that are decoded to strings and miscellaneous */

  ACPI_DMT_ACCWIDTH,
  ACPI_DMT_CHKSUM,
  ACPI_DMT_GAS,
  ACPI_DMT_SPACEID,
  ACPI_DMT_UNICODE,
  ACPI_DMT_UUID,

  /* Types used only for the Data Table Compiler */

  ACPI_DMT_BUFFER,
  ACPI_DMT_DEVICE_PATH,
  ACPI_DMT_LABEL,
  ACPI_DMT_PCI_PATH,

  /* Types that are specific to particular ACPI tables */

  ACPI_DMT_ASF,
  ACPI_DMT_DMAR,
  ACPI_DMT_DMAR_SCOPE,
  ACPI_DMT_EINJACT,
  ACPI_DMT_EINJINST,
  ACPI_DMT_ERSTACT,
  ACPI_DMT_ERSTINST,
  ACPI_DMT_FADTPM,
  ACPI_DMT_GTDT,
  ACPI_DMT_HEST,
  ACPI_DMT_HESTNTFY,
  ACPI_DMT_HESTNTYP,
  ACPI_DMT_IVRS,
  ACPI_DMT_LPIT,
  ACPI_DMT_MADT,
  ACPI_DMT_PCCT,
  ACPI_DMT_PMTT,
  ACPI_DMT_SLIC,
  ACPI_DMT_SRAT,

  /* Special opcodes */

  ACPI_DMT_EXTRA_TEXT,
  ACPI_DMT_EXIT

} ACPI_ENTRY_TYPES;

typedef void (*ACPI_DMTABLE_HANDLER)(ACPI_TABLE_HEADER *Table);

typedef ACPI_STATUS (*ACPI_CMTABLE_HANDLER)(void **PFieldList);

typedef struct acpi_dmtable_data {
  char *Signature;
  ACPI_DMTABLE_INFO *TableInfo;
  ACPI_DMTABLE_HANDLER TableHandler;
  ACPI_CMTABLE_HANDLER CmTableHandler;
  const unsigned char *Template;
  char *Name;

} ACPI_DMTABLE_DATA;

typedef struct acpi_op_walk_info {
  UINT32 Level;
  UINT32 LastLevel;
  UINT32 Count;
  UINT32 BitOffset;
  UINT32 Flags;
  ACPI_WALK_STATE *WalkState;
  ACPI_PARSE_OBJECT *MappingOp;

} ACPI_OP_WALK_INFO;

/*
 * TBD - another copy of this is in asltypes.h, fix
 */
#ifndef ASL_WALK_CALLBACK_DEFINED
typedef ACPI_STATUS (*ASL_WALK_CALLBACK)(ACPI_PARSE_OBJECT *Op, UINT32 Level,
                                         void *Context);
#define ASL_WALK_CALLBACK_DEFINED
#endif

typedef void (*ACPI_RESOURCE_HANDLER)(ACPI_OP_WALK_INFO *Info,
                                      AML_RESOURCE *Resource, UINT32 Length,
                                      UINT32 Level);

typedef struct acpi_resource_tag {
  UINT32 BitIndex;
  char *Tag;

} ACPI_RESOURCE_TAG;

/* Strings used for decoding flags to ASL keywords */

extern const char *AcpiGbl_WordDecode[];
extern const char *AcpiGbl_IrqDecode[];
extern const char *AcpiGbl_LockRule[];
extern const char *AcpiGbl_AccessTypes[];
extern const char *AcpiGbl_UpdateRules[];
extern const char *AcpiGbl_MatchOps[];

extern ACPI_DMTABLE_INFO AcpiDmTableInfoAsf0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoAsf1[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoAsf1a[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoAsf2[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoAsf2a[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoAsf3[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoAsf4[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoAsfHdr[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoBoot[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoBert[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoBgrt[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoCpep[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoCpep0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoCsrt0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoCsrt1[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoCsrt2[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoDbg2[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoDbg2Device[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoDbg2Addr[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoDbg2Size[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoDbg2Name[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoDbg2OemData[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoDbgp[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoDmar[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoDmarHdr[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoDmarScope[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoDmar0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoDmar1[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoDmar2[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoDmar3[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoDmar4[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoDrtm[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoEcdt[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoEinj[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoEinj0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoErst[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoErst0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoFacs[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoFadt1[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoFadt2[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoFadt3[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoFadt5[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoFpdt[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoFpdtHdr[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoFpdt0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoFpdt1[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoGas[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoGtdt[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoGtdtHdr[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoGtdt0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoGtdt0a[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoGtdt1[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoHeader[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoHest[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoHest0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoHest1[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoHest2[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoHest6[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoHest7[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoHest8[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoHest9[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoHestNotify[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoHestBank[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoHpet[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoLpitHdr[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoLpit0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoLpit1[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoIvrs[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoIvrs0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoIvrs1[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoIvrs4[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoIvrs8a[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoIvrs8b[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoIvrs8c[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoIvrsHdr[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMadt[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMadt0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMadt1[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMadt2[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMadt3[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMadt4[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMadt5[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMadt6[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMadt7[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMadt8[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMadt9[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMadt10[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMadt11[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMadt12[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMadt13[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMadt14[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMadtHdr[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMcfg[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMcfg0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMchi[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMpst[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMpst0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMpst0A[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMpst0B[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMpst1[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMpst2[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMsct[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMsct0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMtmr[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoMtmr0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoPmtt[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoPmtt0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoPmtt1[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoPmtt1a[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoPmtt2[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoPmttHdr[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoPcct[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoPcctHdr[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoPcct0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoPcct1[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoRsdp1[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoRsdp2[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoS3pt[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoS3ptHdr[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoS3pt0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoS3pt1[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoSbst[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoSlicHdr[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoSlic0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoSlic1[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoSlit[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoSpcr[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoSpmi[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoSrat[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoSratHdr[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoSrat0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoSrat1[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoSrat2[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoSrat3[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoTcpa[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoTpm2[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoUefi[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoVrtc[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoVrtc0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoWaet[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoWdat[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoWdat0[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoWddt[];
extern ACPI_DMTABLE_INFO AcpiDmTableInfoWdrt[];

extern ACPI_DMTABLE_INFO AcpiDmTableInfoGeneric[][2];

/*
 * dmtable
 */
extern ACPI_DMTABLE_DATA AcpiDmTableData[];

UINT8
AcpiDmGenerateChecksum(void *Table, UINT32 Length, UINT8 OriginalChecksum);

ACPI_DMTABLE_DATA *AcpiDmGetTableData(char *Signature);

void AcpiDmDumpDataTable(ACPI_TABLE_HEADER *Table);

ACPI_STATUS
AcpiDmDumpTable(UINT32 TableLength, UINT32 TableOffset, void *Table,
                UINT32 SubTableLength, ACPI_DMTABLE_INFO *Info);

void AcpiDmLineHeader(UINT32 Offset, UINT32 ByteLength, char *Name);

void AcpiDmLineHeader2(UINT32 Offset, UINT32 ByteLength, char *Name,
                       UINT32 Value);

/*
 * dmtbdump
 */
void AcpiDmDumpAsf(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpCpep(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpCsrt(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpDbg2(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpDmar(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpEinj(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpErst(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpFadt(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpFpdt(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpGtdt(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpHest(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpIvrs(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpLpit(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpMadt(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpMcfg(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpMpst(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpMsct(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpMtmr(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpPcct(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpPmtt(ACPI_TABLE_HEADER *Table);

UINT32
AcpiDmDumpRsdp(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpRsdt(ACPI_TABLE_HEADER *Table);

UINT32
AcpiDmDumpS3pt(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpSlic(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpSlit(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpSrat(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpVrtc(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpWdat(ACPI_TABLE_HEADER *Table);

void AcpiDmDumpXsdt(ACPI_TABLE_HEADER *Table);

/*
 * dmwalk
 */
void AcpiDmDisassemble(ACPI_WALK_STATE *WalkState, ACPI_PARSE_OBJECT *Origin,
                       UINT32 NumOpcodes);

void AcpiDmWalkParseTree(ACPI_PARSE_OBJECT *Op,
                         ASL_WALK_CALLBACK DescendingCallback,
                         ASL_WALK_CALLBACK AscendingCallback, void *Context);

/*
 * dmopcode
 */
void AcpiDmDisassembleOneOp(ACPI_WALK_STATE *WalkState, ACPI_OP_WALK_INFO *Info,
                            ACPI_PARSE_OBJECT *Op);

void AcpiDmDecodeInternalObject(ACPI_OPERAND_OBJECT *ObjDesc);

UINT32
AcpiDmListType(ACPI_PARSE_OBJECT *Op);

void AcpiDmMethodFlags(ACPI_PARSE_OBJECT *Op);

void AcpiDmDisplayTargetPathname(ACPI_PARSE_OBJECT *Op);

void AcpiDmNotifyDescription(ACPI_PARSE_OBJECT *Op);

void AcpiDmPredefinedDescription(ACPI_PARSE_OBJECT *Op);

void AcpiDmFieldPredefinedDescription(ACPI_PARSE_OBJECT *Op);

void AcpiDmFieldFlags(ACPI_PARSE_OBJECT *Op);

void AcpiDmAddressSpace(UINT8 SpaceId);

void AcpiDmRegionFlags(ACPI_PARSE_OBJECT *Op);

void AcpiDmMatchOp(ACPI_PARSE_OBJECT *Op);

/*
 * dmnames
 */
UINT32
AcpiDmDumpName(UINT32 Name);

ACPI_STATUS
AcpiPsDisplayObjectPathname(ACPI_WALK_STATE *WalkState, ACPI_PARSE_OBJECT *Op);

void AcpiDmNamestring(char *Name);

/*
 * dmobject
 */
void AcpiDmDisplayInternalObject(ACPI_OPERAND_OBJECT *ObjDesc,
                                 ACPI_WALK_STATE *WalkState);

void AcpiDmDisplayArguments(ACPI_WALK_STATE *WalkState);

void AcpiDmDisplayLocals(ACPI_WALK_STATE *WalkState);

void AcpiDmDumpMethodInfo(ACPI_STATUS Status, ACPI_WALK_STATE *WalkState,
                          ACPI_PARSE_OBJECT *Op);

/*
 * dmbuffer
 */
void AcpiDmDisasmByteList(UINT32 Level, UINT8 *ByteData, UINT32 ByteCount);

void AcpiDmByteList(ACPI_OP_WALK_INFO *Info, ACPI_PARSE_OBJECT *Op);

void AcpiDmCheckForHardwareId(ACPI_PARSE_OBJECT *Op);

void AcpiDmDecompressEisaId(UINT32 EncodedId);

BOOLEAN
AcpiDmIsUuidBuffer(ACPI_PARSE_OBJECT *Op);

BOOLEAN
AcpiDmIsUnicodeBuffer(ACPI_PARSE_OBJECT *Op);

BOOLEAN
AcpiDmIsStringBuffer(ACPI_PARSE_OBJECT *Op);

BOOLEAN
AcpiDmIsPldBuffer(ACPI_PARSE_OBJECT *Op);

/*
 * dmdeferred
 */
ACPI_STATUS
AcpiDmParseDeferredOps(ACPI_PARSE_OBJECT *Root);

/*
 * dmextern
 */
ACPI_STATUS
AcpiDmAddToExternalFileList(char *PathList);

void AcpiDmClearExternalFileList(void);

void AcpiDmAddOpToExternalList(ACPI_PARSE_OBJECT *Op, char *Path, UINT8 Type,
                               UINT32 Value, UINT16 Flags);

void AcpiDmAddNodeToExternalList(ACPI_NAMESPACE_NODE *Node, UINT8 Type,
                                 UINT32 Value, UINT16 Flags);

void AcpiDmAddExternalsToNamespace(void);

UINT32
AcpiDmGetExternalMethodCount(void);

void AcpiDmClearExternalList(void);

void AcpiDmEmitExternals(void);

void AcpiDmUnresolvedWarning(UINT8 Type);

void AcpiDmGetExternalsFromFile(void);

/*
 * dmresrc
 */
void AcpiDmDumpInteger8(UINT8 Value, char *Name);

void AcpiDmDumpInteger16(UINT16 Value, char *Name);

void AcpiDmDumpInteger32(UINT32 Value, char *Name);

void AcpiDmDumpInteger64(UINT64 Value, char *Name);

void AcpiDmResourceTemplate(ACPI_OP_WALK_INFO *Info, ACPI_PARSE_OBJECT *Op,
                            UINT8 *ByteData, UINT32 ByteCount);

ACPI_STATUS
AcpiDmIsResourceTemplate(ACPI_WALK_STATE *WalkState, ACPI_PARSE_OBJECT *Op);

void AcpiDmBitList(UINT16 Mask);

void AcpiDmDescriptorName(void);

/*
 * dmresrcl
 */
void AcpiDmWordDescriptor(ACPI_OP_WALK_INFO *Info, AML_RESOURCE *Resource,
                          UINT32 Length, UINT32 Level);

void AcpiDmDwordDescriptor(ACPI_OP_WALK_INFO *Info, AML_RESOURCE *Resource,
                           UINT32 Length, UINT32 Level);

void AcpiDmExtendedDescriptor(ACPI_OP_WALK_INFO *Info, AML_RESOURCE *Resource,
                              UINT32 Length, UINT32 Level);

void AcpiDmQwordDescriptor(ACPI_OP_WALK_INFO *Info, AML_RESOURCE *Resource,
                           UINT32 Length, UINT32 Level);

void AcpiDmMemory24Descriptor(ACPI_OP_WALK_INFO *Info, AML_RESOURCE *Resource,
                              UINT32 Length, UINT32 Level);

void AcpiDmMemory32Descriptor(ACPI_OP_WALK_INFO *Info, AML_RESOURCE *Resource,
                              UINT32 Length, UINT32 Level);

void AcpiDmFixedMemory32Descriptor(ACPI_OP_WALK_INFO *Info,
                                   AML_RESOURCE *Resource, UINT32 Length,
                                   UINT32 Level);

void AcpiDmGenericRegisterDescriptor(ACPI_OP_WALK_INFO *Info,
                                     AML_RESOURCE *Resource, UINT32 Length,
                                     UINT32 Level);

void AcpiDmInterruptDescriptor(ACPI_OP_WALK_INFO *Info, AML_RESOURCE *Resource,
                               UINT32 Length, UINT32 Level);

void AcpiDmVendorLargeDescriptor(ACPI_OP_WALK_INFO *Info,
                                 AML_RESOURCE *Resource, UINT32 Length,
                                 UINT32 Level);

void AcpiDmGpioDescriptor(ACPI_OP_WALK_INFO *Info, AML_RESOURCE *Resource,
                          UINT32 Length, UINT32 Level);

void AcpiDmSerialBusDescriptor(ACPI_OP_WALK_INFO *Info, AML_RESOURCE *Resource,
                               UINT32 Length, UINT32 Level);

void AcpiDmVendorCommon(char *Name, UINT8 *ByteData, UINT32 Length,
                        UINT32 Level);

/*
 * dmresrcs
 */
void AcpiDmIrqDescriptor(ACPI_OP_WALK_INFO *Info, AML_RESOURCE *Resource,
                         UINT32 Length, UINT32 Level);

void AcpiDmDmaDescriptor(ACPI_OP_WALK_INFO *Info, AML_RESOURCE *Resource,
                         UINT32 Length, UINT32 Level);

void AcpiDmFixedDmaDescriptor(ACPI_OP_WALK_INFO *Info, AML_RESOURCE *Resource,
                              UINT32 Length, UINT32 Level);

void AcpiDmIoDescriptor(ACPI_OP_WALK_INFO *Info, AML_RESOURCE *Resource,
                        UINT32 Length, UINT32 Level);

void AcpiDmFixedIoDescriptor(ACPI_OP_WALK_INFO *Info, AML_RESOURCE *Resource,
                             UINT32 Length, UINT32 Level);

void AcpiDmStartDependentDescriptor(ACPI_OP_WALK_INFO *Info,
                                    AML_RESOURCE *Resource, UINT32 Length,
                                    UINT32 Level);

void AcpiDmEndDependentDescriptor(ACPI_OP_WALK_INFO *Info,
                                  AML_RESOURCE *Resource, UINT32 Length,
                                  UINT32 Level);

void AcpiDmVendorSmallDescriptor(ACPI_OP_WALK_INFO *Info,
                                 AML_RESOURCE *Resource, UINT32 Length,
                                 UINT32 Level);

/*
 * dmutils
 */
void AcpiDmDecodeAttribute(UINT8 Attribute);

void AcpiDmIndent(UINT32 Level);

BOOLEAN
AcpiDmCommaIfListMember(ACPI_PARSE_OBJECT *Op);

void AcpiDmCommaIfFieldMember(ACPI_PARSE_OBJECT *Op);

/*
 * dmrestag
 */
void AcpiDmFindResources(ACPI_PARSE_OBJECT *Root);

void AcpiDmCheckResourceReference(ACPI_PARSE_OBJECT *Op,
                                  ACPI_WALK_STATE *WalkState);

/*
 * dmcstyle
 */
BOOLEAN
AcpiDmCheckForSymbolicOpcode(ACPI_PARSE_OBJECT *Op, ACPI_OP_WALK_INFO *Info);

void AcpiDmCloseOperator(ACPI_PARSE_OBJECT *Op);

/*
 * acdisasm
 */
void AdDisassemblerHeader(char *Filename);

#endif /* __ACDISASM_H__ */

/**
 * @section minix_drivers_power_acpi_include_acdispat_h
 * @brief Original file: minix/drivers/power/acpi/include/acdispat.h
 */
/******************************************************************************
 *
 * Name: acdispat.h - dispatcher (parser to interpreter interface)
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
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 */

#ifndef _ACDISPAT_H_
#define _ACDISPAT_H_

#define NAMEOF_LOCAL_NTE "__L0"
#define NAMEOF_ARG_NTE "__A0"

/*
 * dsargs - execution of dynamic arguments for static objects
 */
ACPI_STATUS
AcpiDsGetBufferFieldArguments(ACPI_OPERAND_OBJECT *ObjDesc);

ACPI_STATUS
AcpiDsGetBankFieldArguments(ACPI_OPERAND_OBJECT *ObjDesc);

ACPI_STATUS
AcpiDsGetRegionArguments(ACPI_OPERAND_OBJECT *RgnDesc);

ACPI_STATUS
AcpiDsGetBufferArguments(ACPI_OPERAND_OBJECT *ObjDesc);

ACPI_STATUS
AcpiDsGetPackageArguments(ACPI_OPERAND_OBJECT *ObjDesc);

/*
 * dscontrol - support for execution control opcodes
 */
ACPI_STATUS
AcpiDsExecBeginControlOp(ACPI_WALK_STATE *WalkState, ACPI_PARSE_OBJECT *Op);

ACPI_STATUS
AcpiDsExecEndControlOp(ACPI_WALK_STATE *WalkState, ACPI_PARSE_OBJECT *Op);

/*
 * dsopcode - support for late operand evaluation
 */
ACPI_STATUS
AcpiDsEvalBufferFieldOperands(ACPI_WALK_STATE *WalkState,
                              ACPI_PARSE_OBJECT *Op);

ACPI_STATUS
AcpiDsEvalRegionOperands(ACPI_WALK_STATE *WalkState, ACPI_PARSE_OBJECT *Op);

ACPI_STATUS
AcpiDsEvalTableRegionOperands(ACPI_WALK_STATE *WalkState,
                              ACPI_PARSE_OBJECT *Op);

ACPI_STATUS
AcpiDsEvalDataObjectOperands(ACPI_WALK_STATE *WalkState, ACPI_PARSE_OBJECT *Op,
                             ACPI_OPERAND_OBJECT *ObjDesc);

ACPI_STATUS
AcpiDsEvalBankFieldOperands(ACPI_WALK_STATE *WalkState, ACPI_PARSE_OBJECT *Op);

ACPI_STATUS
AcpiDsInitializeRegion(ACPI_HANDLE ObjHandle);

/*
 * dsexec - Parser/Interpreter interface, method execution callbacks
 */
ACPI_STATUS
AcpiDsGetPredicateValue(ACPI_WALK_STATE *WalkState,
                        ACPI_OPERAND_OBJECT *ResultObj);

ACPI_STATUS
AcpiDsExecBeginOp(ACPI_WALK_STATE *WalkState, ACPI_PARSE_OBJECT **OutOp);

ACPI_STATUS
AcpiDsExecEndOp(ACPI_WALK_STATE *State);

/*
 * dsfield - Parser/Interpreter interface for AML fields
 */
ACPI_STATUS
AcpiDsCreateField(ACPI_PARSE_OBJECT *Op, ACPI_NAMESPACE_NODE *RegionNode,
                  ACPI_WALK_STATE *WalkState);

ACPI_STATUS
AcpiDsCreateBankField(ACPI_PARSE_OBJECT *Op, ACPI_NAMESPACE_NODE *RegionNode,
                      ACPI_WALK_STATE *WalkState);

ACPI_STATUS
AcpiDsCreateIndexField(ACPI_PARSE_OBJECT *Op, ACPI_NAMESPACE_NODE *RegionNode,
                       ACPI_WALK_STATE *WalkState);

ACPI_STATUS
AcpiDsCreateBufferField(ACPI_PARSE_OBJECT *Op, ACPI_WALK_STATE *WalkState);

ACPI_STATUS
AcpiDsInitFieldObjects(ACPI_PARSE_OBJECT *Op, ACPI_WALK_STATE *WalkState);

/*
 * dsload - Parser/Interpreter interface
 */
ACPI_STATUS
AcpiDsInitCallbacks(ACPI_WALK_STATE *WalkState, UINT32 PassNumber);

/* dsload - pass 1 namespace load callbacks */

ACPI_STATUS
AcpiDsLoad1BeginOp(ACPI_WALK_STATE *WalkState, ACPI_PARSE_OBJECT **OutOp);

ACPI_STATUS
AcpiDsLoad1EndOp(ACPI_WALK_STATE *WalkState);

/* dsload - pass 2 namespace load callbacks */

ACPI_STATUS
AcpiDsLoad2BeginOp(ACPI_WALK_STATE *WalkState, ACPI_PARSE_OBJECT **OutOp);

ACPI_STATUS
AcpiDsLoad2EndOp(ACPI_WALK_STATE *WalkState);

/*
 * dsmthdat - method data (locals/args)
 */
ACPI_STATUS
AcpiDsStoreObjectToLocal(UINT8 Type, UINT32 Index, ACPI_OPERAND_OBJECT *SrcDesc,
                         ACPI_WALK_STATE *WalkState);

ACPI_STATUS
AcpiDsMethodDataGetEntry(UINT16 Opcode, UINT32 Index,
                         ACPI_WALK_STATE *WalkState,
                         ACPI_OPERAND_OBJECT ***Node);

void AcpiDsMethodDataDeleteAll(ACPI_WALK_STATE *WalkState);

BOOLEAN
AcpiDsIsMethodValue(ACPI_OPERAND_OBJECT *ObjDesc);

ACPI_STATUS
AcpiDsMethodDataGetValue(UINT8 Type, UINT32 Index, ACPI_WALK_STATE *WalkState,
                         ACPI_OPERAND_OBJECT **DestDesc);

ACPI_STATUS
AcpiDsMethodDataInitArgs(ACPI_OPERAND_OBJECT **Params, UINT32 MaxParamCount,
                         ACPI_WALK_STATE *WalkState);

ACPI_STATUS
AcpiDsMethodDataGetNode(UINT8 Type, UINT32 Index, ACPI_WALK_STATE *WalkState,
                        ACPI_NAMESPACE_NODE **Node);

void AcpiDsMethodDataInit(ACPI_WALK_STATE *WalkState);

/*
 * dsmethod - Parser/Interpreter interface - control method parsing
 */
ACPI_STATUS
AcpiDsAutoSerializeMethod(ACPI_NAMESPACE_NODE *Node,
                          ACPI_OPERAND_OBJECT *ObjDesc);

ACPI_STATUS
AcpiDsCallControlMethod(ACPI_THREAD_STATE *Thread, ACPI_WALK_STATE *WalkState,
                        ACPI_PARSE_OBJECT *Op);

ACPI_STATUS
AcpiDsRestartControlMethod(ACPI_WALK_STATE *WalkState,
                           ACPI_OPERAND_OBJECT *ReturnDesc);

void AcpiDsTerminateControlMethod(ACPI_OPERAND_OBJECT *MethodDesc,
                                  ACPI_WALK_STATE *WalkState);

ACPI_STATUS
AcpiDsBeginMethodExecution(ACPI_NAMESPACE_NODE *MethodNode,
                           ACPI_OPERAND_OBJECT *ObjDesc,
                           ACPI_WALK_STATE *WalkState);

ACPI_STATUS
AcpiDsMethodError(ACPI_STATUS Status, ACPI_WALK_STATE *WalkState);

/*
 * dsinit
 */
ACPI_STATUS
AcpiDsInitializeObjects(UINT32 TableIndex, ACPI_NAMESPACE_NODE *StartNode);

/*
 * dsobject - Parser/Interpreter interface - object initialization and
 * conversion
 */
ACPI_STATUS
AcpiDsBuildInternalBufferObj(ACPI_WALK_STATE *WalkState, ACPI_PARSE_OBJECT *Op,
                             UINT32 BufferLength,
                             ACPI_OPERAND_OBJECT **ObjDescPtr);

ACPI_STATUS
AcpiDsBuildInternalPackageObj(ACPI_WALK_STATE *WalkState, ACPI_PARSE_OBJECT *op,
                              UINT32 PackageLength,
                              ACPI_OPERAND_OBJECT **ObjDesc);

ACPI_STATUS
AcpiDsInitObjectFromOp(ACPI_WALK_STATE *WalkState, ACPI_PARSE_OBJECT *Op,
                       UINT16 Opcode, ACPI_OPERAND_OBJECT **ObjDesc);

ACPI_STATUS
AcpiDsCreateNode(ACPI_WALK_STATE *WalkState, ACPI_NAMESPACE_NODE *Node,
                 ACPI_PARSE_OBJECT *Op);

/*
 * dsutils - Parser/Interpreter interface utility routines
 */
void AcpiDsClearImplicitReturn(ACPI_WALK_STATE *WalkState);

BOOLEAN
AcpiDsDoImplicitReturn(ACPI_OPERAND_OBJECT *ReturnDesc,
                       ACPI_WALK_STATE *WalkState, BOOLEAN AddReference);

BOOLEAN
AcpiDsIsResultUsed(ACPI_PARSE_OBJECT *Op, ACPI_WALK_STATE *WalkState);

void AcpiDsDeleteResultIfNotUsed(ACPI_PARSE_OBJECT *Op,
                                 ACPI_OPERAND_OBJECT *ResultObj,
                                 ACPI_WALK_STATE *WalkState);

ACPI_STATUS
AcpiDsCreateOperand(ACPI_WALK_STATE *WalkState, ACPI_PARSE_OBJECT *Arg,
                    UINT32 ArgsRemaining);

ACPI_STATUS
AcpiDsCreateOperands(ACPI_WALK_STATE *WalkState, ACPI_PARSE_OBJECT *FirstArg);

ACPI_STATUS
AcpiDsResolveOperands(ACPI_WALK_STATE *WalkState);

void AcpiDsClearOperands(ACPI_WALK_STATE *WalkState);

ACPI_STATUS
AcpiDsEvaluateNamePath(ACPI_WALK_STATE *WalkState);

/*
 * dswscope - Scope Stack manipulation
 */
ACPI_STATUS
AcpiDsScopeStackPush(ACPI_NAMESPACE_NODE *Node, ACPI_OBJECT_TYPE Type,
                     ACPI_WALK_STATE *WalkState);

ACPI_STATUS
AcpiDsScopeStackPop(ACPI_WALK_STATE *WalkState);

void AcpiDsScopeStackClear(ACPI_WALK_STATE *WalkState);

/*
 * dswstate - parser WALK_STATE management routines
 */
ACPI_STATUS
AcpiDsObjStackPush(void *Object, ACPI_WALK_STATE *WalkState);

ACPI_STATUS
AcpiDsObjStackPop(UINT32 PopCount, ACPI_WALK_STATE *WalkState);

ACPI_WALK_STATE *AcpiDsCreateWalkState(ACPI_OWNER_ID OwnerId,
                                       ACPI_PARSE_OBJECT *Origin,
                                       ACPI_OPERAND_OBJECT *MthDesc,
                                       ACPI_THREAD_STATE *Thread);

ACPI_STATUS
AcpiDsInitAmlWalk(ACPI_WALK_STATE *WalkState, ACPI_PARSE_OBJECT *Op,
                  ACPI_NAMESPACE_NODE *MethodNode, UINT8 *AmlStart,
                  UINT32 AmlLength, ACPI_EVALUATE_INFO *Info, UINT8 PassNumber);

void AcpiDsObjStackPopAndDelete(UINT32 PopCount, ACPI_WALK_STATE *WalkState);

void AcpiDsDeleteWalkState(ACPI_WALK_STATE *WalkState);

ACPI_WALK_STATE *AcpiDsPopWalkState(ACPI_THREAD_STATE *Thread);

void AcpiDsPushWalkState(ACPI_WALK_STATE *WalkState, ACPI_THREAD_STATE *Thread);

ACPI_STATUS
AcpiDsResultStackClear(ACPI_WALK_STATE *WalkState);

ACPI_WALK_STATE *AcpiDsGetCurrentWalkState(ACPI_THREAD_STATE *Thread);

ACPI_STATUS
AcpiDsResultPop(ACPI_OPERAND_OBJECT **Object, ACPI_WALK_STATE *WalkState);

ACPI_STATUS
AcpiDsResultPush(ACPI_OPERAND_OBJECT *Object, ACPI_WALK_STATE *WalkState);

#endif /* _ACDISPAT_H_ */

/**
 * @section minix_drivers_power_acpi_include_acevents_h
 * @brief Original file: minix/drivers/power/acpi/include/acevents.h
 */
/******************************************************************************
 *
 * Name: acevents.h - Event subcomponent prototypes and defines
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
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 */

#ifndef __ACEVENTS_H__
#define __ACEVENTS_H__

/*
 * evevent
 */
ACPI_STATUS
AcpiEvInitializeEvents(void);

ACPI_STATUS
AcpiEvInstallXruptHandlers(void);

UINT32
AcpiEvFixedEventDetect(void);

/*
 * evmisc
 */
BOOLEAN
AcpiEvIsNotifyObject(ACPI_NAMESPACE_NODE *Node);

UINT32
AcpiEvGetGpeNumberIndex(UINT32 GpeNumber);

ACPI_STATUS
AcpiEvQueueNotifyRequest(ACPI_NAMESPACE_NODE *Node, UINT32 NotifyValue);

/*
 * evglock - Global Lock support
 */
ACPI_STATUS
AcpiEvInitGlobalLockHandler(void);

ACPI_HW_DEPENDENT_RETURN_OK(ACPI_STATUS AcpiEvAcquireGlobalLock(UINT16 Timeout))

ACPI_HW_DEPENDENT_RETURN_OK(ACPI_STATUS AcpiEvReleaseGlobalLock(void))

ACPI_STATUS
AcpiEvRemoveGlobalLockHandler(void);

/*
 * evgpe - Low-level GPE support
 */
UINT32
AcpiEvGpeDetect(ACPI_GPE_XRUPT_INFO *GpeXruptList);

ACPI_STATUS
AcpiEvUpdateGpeEnableMask(ACPI_GPE_EVENT_INFO *GpeEventInfo);

ACPI_STATUS
AcpiEvEnableGpe(ACPI_GPE_EVENT_INFO *GpeEventInfo);

ACPI_STATUS
AcpiEvAddGpeReference(ACPI_GPE_EVENT_INFO *GpeEventInfo);

ACPI_STATUS
AcpiEvRemoveGpeReference(ACPI_GPE_EVENT_INFO *GpeEventInfo);

ACPI_GPE_EVENT_INFO *AcpiEvGetGpeEventInfo(ACPI_HANDLE GpeDevice,
                                           UINT32 GpeNumber);

ACPI_GPE_EVENT_INFO *AcpiEvLowGetGpeInfo(UINT32 GpeNumber,
                                         ACPI_GPE_BLOCK_INFO *GpeBlock);

ACPI_STATUS
AcpiEvFinishGpe(ACPI_GPE_EVENT_INFO *GpeEventInfo);

/*
 * evgpeblk - Upper-level GPE block support
 */
ACPI_STATUS
AcpiEvCreateGpeBlock(ACPI_NAMESPACE_NODE *GpeDevice, UINT64 Address,
                     UINT8 SpaceId, UINT32 RegisterCount,
                     UINT16 GpeBlockBaseNumber, UINT32 InterruptNumber,
                     ACPI_GPE_BLOCK_INFO **ReturnGpeBlock);

ACPI_STATUS
AcpiEvInitializeGpeBlock(ACPI_GPE_XRUPT_INFO *GpeXruptInfo,
                         ACPI_GPE_BLOCK_INFO *GpeBlock, void *Context);

ACPI_HW_DEPENDENT_RETURN_OK(
    ACPI_STATUS AcpiEvDeleteGpeBlock(ACPI_GPE_BLOCK_INFO *GpeBlock))

UINT32
AcpiEvGpeDispatch(ACPI_NAMESPACE_NODE *GpeDevice,
                  ACPI_GPE_EVENT_INFO *GpeEventInfo, UINT32 GpeNumber);

/*
 * evgpeinit - GPE initialization and update
 */
ACPI_STATUS
AcpiEvGpeInitialize(void);

ACPI_HW_DEPENDENT_RETURN_VOID(void AcpiEvUpdateGpes(ACPI_OWNER_ID TableOwnerId))

ACPI_STATUS
AcpiEvMatchGpeMethod(ACPI_HANDLE ObjHandle, UINT32 Level, void *Context,
                     void **ReturnValue);

/*
 * evgpeutil - GPE utilities
 */
ACPI_STATUS
AcpiEvWalkGpeList(ACPI_GPE_CALLBACK GpeWalkCallback, void *Context);

BOOLEAN
AcpiEvValidGpeEvent(ACPI_GPE_EVENT_INFO *GpeEventInfo);

ACPI_STATUS
AcpiEvGetGpeDevice(ACPI_GPE_XRUPT_INFO *GpeXruptInfo,
                   ACPI_GPE_BLOCK_INFO *GpeBlock, void *Context);

ACPI_STATUS
AcpiEvGetGpeXruptBlock(UINT32 InterruptNumber,
                       ACPI_GPE_XRUPT_INFO **GpeXruptBlock);

ACPI_STATUS
AcpiEvDeleteGpeXrupt(ACPI_GPE_XRUPT_INFO *GpeXrupt);

ACPI_STATUS
AcpiEvDeleteGpeHandlers(ACPI_GPE_XRUPT_INFO *GpeXruptInfo,
                        ACPI_GPE_BLOCK_INFO *GpeBlock, void *Context);

/*
 * evhandler - Address space handling
 */
BOOLEAN
AcpiEvHasDefaultHandler(ACPI_NAMESPACE_NODE *Node, ACPI_ADR_SPACE_TYPE SpaceId);

ACPI_STATUS
AcpiEvInstallRegionHandlers(void);

ACPI_STATUS
AcpiEvInstallSpaceHandler(ACPI_NAMESPACE_NODE *Node,
                          ACPI_ADR_SPACE_TYPE SpaceId,
                          ACPI_ADR_SPACE_HANDLER Handler,
                          ACPI_ADR_SPACE_SETUP Setup, void *Context);

/*
 * evregion - Operation region support
 */
ACPI_STATUS
AcpiEvInitializeOpRegions(void);

ACPI_STATUS
AcpiEvAddressSpaceDispatch(ACPI_OPERAND_OBJECT *RegionObj,
                           ACPI_OPERAND_OBJECT *FieldObj, UINT32 Function,
                           UINT32 RegionOffset, UINT32 BitWidth, UINT64 *Value);

ACPI_STATUS
AcpiEvAttachRegion(ACPI_OPERAND_OBJECT *HandlerObj,
                   ACPI_OPERAND_OBJECT *RegionObj, BOOLEAN AcpiNsIsLocked);

void AcpiEvDetachRegion(ACPI_OPERAND_OBJECT *RegionObj, BOOLEAN AcpiNsIsLocked);

ACPI_STATUS
AcpiEvExecuteRegMethods(ACPI_NAMESPACE_NODE *Node, ACPI_ADR_SPACE_TYPE SpaceId);

ACPI_STATUS
AcpiEvExecuteRegMethod(ACPI_OPERAND_OBJECT *RegionObj, UINT32 Function);

/*
 * evregini - Region initialization and setup
 */
ACPI_STATUS
AcpiEvSystemMemoryRegionSetup(ACPI_HANDLE Handle, UINT32 Function,
                              void *HandlerContext, void **RegionContext);

ACPI_STATUS
AcpiEvIoSpaceRegionSetup(ACPI_HANDLE Handle, UINT32 Function,
                         void *HandlerContext, void **RegionContext);

ACPI_STATUS
AcpiEvPciConfigRegionSetup(ACPI_HANDLE Handle, UINT32 Function,
                           void *HandlerContext, void **RegionContext);

ACPI_STATUS
AcpiEvCmosRegionSetup(ACPI_HANDLE Handle, UINT32 Function, void *HandlerContext,
                      void **RegionContext);

ACPI_STATUS
AcpiEvPciBarRegionSetup(ACPI_HANDLE Handle, UINT32 Function,
                        void *HandlerContext, void **RegionContext);

ACPI_STATUS
AcpiEvDefaultRegionSetup(ACPI_HANDLE Handle, UINT32 Function,
                         void *HandlerContext, void **RegionContext);

ACPI_STATUS
AcpiEvInitializeRegion(ACPI_OPERAND_OBJECT *RegionObj, BOOLEAN AcpiNsLocked);

/*
 * evsci - SCI (System Control Interrupt) handling/dispatch
 */
UINT32 ACPI_SYSTEM_XFACE AcpiEvGpeXruptHandler(void *Context);

UINT32
AcpiEvSciDispatch(void);

UINT32
AcpiEvInstallSciHandler(void);

ACPI_STATUS
AcpiEvRemoveAllSciHandlers(void);

ACPI_HW_DEPENDENT_RETURN_VOID(void AcpiEvTerminate(void))

#endif /* __ACEVENTS_H__  */

/**
 * @section minix_drivers_power_acpi_include_acexcep_h
 * @brief Original file: minix/drivers/power/acpi/include/acexcep.h
 */
/******************************************************************************
 *
 * Name: acexcep.h - Exception codes returned by the ACPI subsystem
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
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 */

#ifndef __ACEXCEP_H__
#define __ACEXCEP_H__

/* This module contains all possible exception codes for ACPI_STATUS */

/*
 * Exception code classes
 */
#define AE_CODE_ENVIRONMENTAL 0x0000 /* General ACPICA environment */
#define AE_CODE_PROGRAMMER 0x1000    /* External ACPICA interface caller */
#define AE_CODE_ACPI_TABLES 0x2000   /* ACPI tables */
#define AE_CODE_AML 0x3000           /* From executing AML code */
#define AE_CODE_CONTROL 0x4000       /* Internal control codes */

#define AE_CODE_MAX 0x4000
#define AE_CODE_MASK 0xF000

/*
 * Macros to insert the exception code classes
 */
#define EXCEP_ENV(code) ((ACPI_STATUS)(code | AE_CODE_ENVIRONMENTAL))
#define EXCEP_PGM(code) ((ACPI_STATUS)(code | AE_CODE_PROGRAMMER))
#define EXCEP_TBL(code) ((ACPI_STATUS)(code | AE_CODE_ACPI_TABLES))
#define EXCEP_AML(code) ((ACPI_STATUS)(code | AE_CODE_AML))
#define EXCEP_CTL(code) ((ACPI_STATUS)(code | AE_CODE_CONTROL))

/*
 * Exception info table. The "Description" field is used only by the
 * ACPICA help application (acpihelp).
 */
typedef struct acpi_exception_info {
  char *Name;

#ifdef ACPI_HELP_APP
  char *Description;
#endif
} ACPI_EXCEPTION_INFO;

#ifdef ACPI_HELP_APP
#define EXCEP_TXT(Name, Description) {Name, Description}
#else
#define EXCEP_TXT(Name, Description) {Name}
#endif

/*
 * Success is always zero, failure is non-zero
 */
#define ACPI_SUCCESS(a) (!(a))
#define ACPI_FAILURE(a) (a)

#define AE_OK (ACPI_STATUS)0x0000

/*
 * Environmental exceptions
 */
#define AE_ERROR EXCEP_ENV(0x0001)
#define AE_NO_ACPI_TABLES EXCEP_ENV(0x0002)
#define AE_NO_NAMESPACE EXCEP_ENV(0x0003)
#define AE_NO_MEMORY EXCEP_ENV(0x0004)
#define AE_NOT_FOUND EXCEP_ENV(0x0005)
#define AE_NOT_EXIST EXCEP_ENV(0x0006)
#define AE_ALREADY_EXISTS EXCEP_ENV(0x0007)
#define AE_TYPE EXCEP_ENV(0x0008)
#define AE_NULL_OBJECT EXCEP_ENV(0x0009)
#define AE_NULL_ENTRY EXCEP_ENV(0x000A)
#define AE_BUFFER_OVERFLOW EXCEP_ENV(0x000B)
#define AE_STACK_OVERFLOW EXCEP_ENV(0x000C)
#define AE_STACK_UNDERFLOW EXCEP_ENV(0x000D)
#define AE_NOT_IMPLEMENTED EXCEP_ENV(0x000E)
#define AE_SUPPORT EXCEP_ENV(0x000F)
#define AE_LIMIT EXCEP_ENV(0x0010)
#define AE_TIME EXCEP_ENV(0x0011)
#define AE_ACQUIRE_DEADLOCK EXCEP_ENV(0x0012)
#define AE_RELEASE_DEADLOCK EXCEP_ENV(0x0013)
#define AE_NOT_ACQUIRED EXCEP_ENV(0x0014)
#define AE_ALREADY_ACQUIRED EXCEP_ENV(0x0015)
#define AE_NO_HARDWARE_RESPONSE EXCEP_ENV(0x0016)
#define AE_NO_GLOBAL_LOCK EXCEP_ENV(0x0017)
#define AE_ABORT_METHOD EXCEP_ENV(0x0018)
#define AE_SAME_HANDLER EXCEP_ENV(0x0019)
#define AE_NO_HANDLER EXCEP_ENV(0x001A)
#define AE_OWNER_ID_LIMIT EXCEP_ENV(0x001B)
#define AE_NOT_CONFIGURED EXCEP_ENV(0x001C)
#define AE_ACCESS EXCEP_ENV(0x001D)

#define AE_CODE_ENV_MAX 0x001D

/*
 * Programmer exceptions
 */
#define AE_BAD_PARAMETER EXCEP_PGM(0x0001)
#define AE_BAD_CHARACTER EXCEP_PGM(0x0002)
#define AE_BAD_PATHNAME EXCEP_PGM(0x0003)
#define AE_BAD_DATA EXCEP_PGM(0x0004)
#define AE_BAD_HEX_CONSTANT EXCEP_PGM(0x0005)
#define AE_BAD_OCTAL_CONSTANT EXCEP_PGM(0x0006)
#define AE_BAD_DECIMAL_CONSTANT EXCEP_PGM(0x0007)
#define AE_MISSING_ARGUMENTS EXCEP_PGM(0x0008)
#define AE_BAD_ADDRESS EXCEP_PGM(0x0009)

#define AE_CODE_PGM_MAX 0x0009

/*
 * Acpi table exceptions
 */
#define AE_BAD_SIGNATURE EXCEP_TBL(0x0001)
#define AE_BAD_HEADER EXCEP_TBL(0x0002)
#define AE_BAD_CHECKSUM EXCEP_TBL(0x0003)
#define AE_BAD_VALUE EXCEP_TBL(0x0004)
#define AE_INVALID_TABLE_LENGTH EXCEP_TBL(0x0005)

#define AE_CODE_TBL_MAX 0x0005

/*
 * AML exceptions. These are caused by problems with
 * the actual AML byte stream
 */
#define AE_AML_BAD_OPCODE EXCEP_AML(0x0001)
#define AE_AML_NO_OPERAND EXCEP_AML(0x0002)
#define AE_AML_OPERAND_TYPE EXCEP_AML(0x0003)
#define AE_AML_OPERAND_VALUE EXCEP_AML(0x0004)
#define AE_AML_UNINITIALIZED_LOCAL EXCEP_AML(0x0005)
#define AE_AML_UNINITIALIZED_ARG EXCEP_AML(0x0006)
#define AE_AML_UNINITIALIZED_ELEMENT EXCEP_AML(0x0007)
#define AE_AML_NUMERIC_OVERFLOW EXCEP_AML(0x0008)
#define AE_AML_REGION_LIMIT EXCEP_AML(0x0009)
#define AE_AML_BUFFER_LIMIT EXCEP_AML(0x000A)
#define AE_AML_PACKAGE_LIMIT EXCEP_AML(0x000B)
#define AE_AML_DIVIDE_BY_ZERO EXCEP_AML(0x000C)
#define AE_AML_BAD_NAME EXCEP_AML(0x000D)
#define AE_AML_NAME_NOT_FOUND EXCEP_AML(0x000E)
#define AE_AML_INTERNAL EXCEP_AML(0x000F)
#define AE_AML_INVALID_SPACE_ID EXCEP_AML(0x0010)
#define AE_AML_STRING_LIMIT EXCEP_AML(0x0011)
#define AE_AML_NO_RETURN_VALUE EXCEP_AML(0x0012)
#define AE_AML_METHOD_LIMIT EXCEP_AML(0x0013)
#define AE_AML_NOT_OWNER EXCEP_AML(0x0014)
#define AE_AML_MUTEX_ORDER EXCEP_AML(0x0015)
#define AE_AML_MUTEX_NOT_ACQUIRED EXCEP_AML(0x0016)
#define AE_AML_INVALID_RESOURCE_TYPE EXCEP_AML(0x0017)
#define AE_AML_INVALID_INDEX EXCEP_AML(0x0018)
#define AE_AML_REGISTER_LIMIT EXCEP_AML(0x0019)
#define AE_AML_NO_WHILE EXCEP_AML(0x001A)
#define AE_AML_ALIGNMENT EXCEP_AML(0x001B)
#define AE_AML_NO_RESOURCE_END_TAG EXCEP_AML(0x001C)
#define AE_AML_BAD_RESOURCE_VALUE EXCEP_AML(0x001D)
#define AE_AML_CIRCULAR_REFERENCE EXCEP_AML(0x001E)
#define AE_AML_BAD_RESOURCE_LENGTH EXCEP_AML(0x001F)
#define AE_AML_ILLEGAL_ADDRESS EXCEP_AML(0x0020)
#define AE_AML_INFINITE_LOOP EXCEP_AML(0x0021)

#define AE_CODE_AML_MAX 0x0021

/*
 * Internal exceptions used for control
 */
#define AE_CTRL_RETURN_VALUE EXCEP_CTL(0x0001)
#define AE_CTRL_PENDING EXCEP_CTL(0x0002)
#define AE_CTRL_TERMINATE EXCEP_CTL(0x0003)
#define AE_CTRL_TRUE EXCEP_CTL(0x0004)
#define AE_CTRL_FALSE EXCEP_CTL(0x0005)
#define AE_CTRL_DEPTH EXCEP_CTL(0x0006)
#define AE_CTRL_END EXCEP_CTL(0x0007)
#define AE_CTRL_TRANSFER EXCEP_CTL(0x0008)
#define AE_CTRL_BREAK EXCEP_CTL(0x0009)
#define AE_CTRL_CONTINUE EXCEP_CTL(0x000A)
#define AE_CTRL_SKIP EXCEP_CTL(0x000B)
#define AE_CTRL_PARSE_CONTINUE EXCEP_CTL(0x000C)
#define AE_CTRL_PARSE_PENDING EXCEP_CTL(0x000D)

#define AE_CODE_CTRL_MAX 0x000D

/* Exception strings for AcpiFormatException */

#ifdef ACPI_DEFINE_EXCEPTION_TABLE

/*
 * String versions of the exception codes above
 * These strings must match the corresponding defines exactly
 */
static const ACPI_EXCEPTION_INFO AcpiGbl_ExceptionNames_Env[] = {
    EXCEP_TXT("AE_OK", "No error"),
    EXCEP_TXT("AE_ERROR", "Unspecified error"),
    EXCEP_TXT("AE_NO_ACPI_TABLES", "ACPI tables could not be found"),
    EXCEP_TXT("AE_NO_NAMESPACE", "A namespace has not been loaded"),
    EXCEP_TXT("AE_NO_MEMORY", "Insufficient dynamic memory"),
    EXCEP_TXT("AE_NOT_FOUND", "A requested entity is not found"),
    EXCEP_TXT("AE_NOT_EXIST", "A required entity does not exist"),
    EXCEP_TXT("AE_ALREADY_EXISTS", "An entity already exists"),
    EXCEP_TXT("AE_TYPE", "The object type is incorrect"),
    EXCEP_TXT("AE_NULL_OBJECT", "A required object was missing"),
    EXCEP_TXT("AE_NULL_ENTRY", "The requested object does not exist"),
    EXCEP_TXT("AE_BUFFER_OVERFLOW", "The buffer provided is too small"),
    EXCEP_TXT("AE_STACK_OVERFLOW", "An internal stack overflowed"),
    EXCEP_TXT("AE_STACK_UNDERFLOW", "An internal stack underflowed"),
    EXCEP_TXT("AE_NOT_IMPLEMENTED", "The feature is not implemented"),
    EXCEP_TXT("AE_SUPPORT", "The feature is not supported"),
    EXCEP_TXT("AE_LIMIT", "A predefined limit was exceeded"),
    EXCEP_TXT("AE_TIME", "A time limit or timeout expired"),
    EXCEP_TXT("AE_ACQUIRE_DEADLOCK", "Internal error, attempt was made to "
                                     "acquire a mutex in improper order"),
    EXCEP_TXT("AE_RELEASE_DEADLOCK", "Internal error, attempt was made to "
                                     "release a mutex in improper order"),
    EXCEP_TXT("AE_NOT_ACQUIRED", "An attempt to release a mutex or Global Lock "
                                 "without a previous acquire"),
    EXCEP_TXT("AE_ALREADY_ACQUIRED",
              "Internal error, attempt was made to acquire a mutex twice"),
    EXCEP_TXT("AE_NO_HARDWARE_RESPONSE",
              "Hardware did not respond after an I/O operation"),
    EXCEP_TXT("AE_NO_GLOBAL_LOCK", "There is no FACS Global Lock"),
    EXCEP_TXT("AE_ABORT_METHOD", "A control method was aborted"),
    EXCEP_TXT("AE_SAME_HANDLER", "Attempt was made to install the same handler "
                                 "that is already installed"),
    EXCEP_TXT("AE_NO_HANDLER", "A handler for the operation is not installed"),
    EXCEP_TXT("AE_OWNER_ID_LIMIT", "There are no more Owner IDs available for "
                                   "ACPI tables or control methods"),
    EXCEP_TXT(
        "AE_NOT_CONFIGURED",
        "The interface is not part of the current subsystem configuration"),
    EXCEP_TXT("AE_ACCESS", "Permission denied for the requested operation")};

static const ACPI_EXCEPTION_INFO AcpiGbl_ExceptionNames_Pgm[] = {
    EXCEP_TXT(NULL, NULL),
    EXCEP_TXT("AE_BAD_PARAMETER", "A parameter is out of range or invalid"),
    EXCEP_TXT("AE_BAD_CHARACTER", "An invalid character was found in a name"),
    EXCEP_TXT("AE_BAD_PATHNAME",
              "An invalid character was found in a pathname"),
    EXCEP_TXT("AE_BAD_DATA", "A package or buffer contained incorrect data"),
    EXCEP_TXT("AE_BAD_HEX_CONSTANT", "Invalid character in a Hex constant"),
    EXCEP_TXT("AE_BAD_OCTAL_CONSTANT",
              "Invalid character in an Octal constant"),
    EXCEP_TXT("AE_BAD_DECIMAL_CONSTANT",
              "Invalid character in a Decimal constant"),
    EXCEP_TXT("AE_MISSING_ARGUMENTS",
              "Too few arguments were passed to a control method"),
    EXCEP_TXT("AE_BAD_ADDRESS", "An illegal null I/O address")};

static const ACPI_EXCEPTION_INFO AcpiGbl_ExceptionNames_Tbl[] = {
    EXCEP_TXT(NULL, NULL),
    EXCEP_TXT("AE_BAD_SIGNATURE", "An ACPI table has an invalid signature"),
    EXCEP_TXT("AE_BAD_HEADER", "Invalid field in an ACPI table header"),
    EXCEP_TXT("AE_BAD_CHECKSUM", "An ACPI table checksum is not correct"),
    EXCEP_TXT("AE_BAD_VALUE", "An invalid value was found in a table"),
    EXCEP_TXT("AE_INVALID_TABLE_LENGTH",
              "The FADT or FACS has improper length")};

static const ACPI_EXCEPTION_INFO AcpiGbl_ExceptionNames_Aml[] = {
    EXCEP_TXT(NULL, NULL),
    EXCEP_TXT("AE_AML_BAD_OPCODE", "Invalid AML opcode encountered"),
    EXCEP_TXT("AE_AML_NO_OPERAND", "A required operand is missing"),
    EXCEP_TXT("AE_AML_OPERAND_TYPE",
              "An operand of an incorrect type was encountered"),
    EXCEP_TXT("AE_AML_OPERAND_VALUE",
              "The operand had an inappropriate or invalid value"),
    EXCEP_TXT("AE_AML_UNINITIALIZED_LOCAL",
              "Method tried to use an uninitialized local variable"),
    EXCEP_TXT("AE_AML_UNINITIALIZED_ARG",
              "Method tried to use an uninitialized argument"),
    EXCEP_TXT("AE_AML_UNINITIALIZED_ELEMENT",
              "Method tried to use an empty package element"),
    EXCEP_TXT("AE_AML_NUMERIC_OVERFLOW",
              "Overflow during BCD conversion or other"),
    EXCEP_TXT("AE_AML_REGION_LIMIT",
              "Tried to access beyond the end of an Operation Region"),
    EXCEP_TXT("AE_AML_BUFFER_LIMIT",
              "Tried to access beyond the end of a buffer"),
    EXCEP_TXT("AE_AML_PACKAGE_LIMIT",
              "Tried to access beyond the end of a package"),
    EXCEP_TXT("AE_AML_DIVIDE_BY_ZERO",
              "During execution of AML Divide operator"),
    EXCEP_TXT("AE_AML_BAD_NAME", "An ACPI name contains invalid character(s)"),
    EXCEP_TXT("AE_AML_NAME_NOT_FOUND", "Could not resolve a named reference"),
    EXCEP_TXT("AE_AML_INTERNAL", "An internal error within the interprete"),
    EXCEP_TXT("AE_AML_INVALID_SPACE_ID",
              "An Operation Region SpaceID is invalid"),
    EXCEP_TXT("AE_AML_STRING_LIMIT", "String is longer than 200 characters"),
    EXCEP_TXT("AE_AML_NO_RETURN_VALUE",
              "A method did not return a required value"),
    EXCEP_TXT("AE_AML_METHOD_LIMIT",
              "A control method reached the maximum reentrancy limit of 255"),
    EXCEP_TXT("AE_AML_NOT_OWNER",
              "A thread tried to release a mutex that it does not own"),
    EXCEP_TXT("AE_AML_MUTEX_ORDER", "Mutex SyncLevel release mismatch"),
    EXCEP_TXT("AE_AML_MUTEX_NOT_ACQUIRED",
              "Attempt to release a mutex that was not previously acquired"),
    EXCEP_TXT("AE_AML_INVALID_RESOURCE_TYPE",
              "Invalid resource type in resource list"),
    EXCEP_TXT("AE_AML_INVALID_INDEX", "Invalid Argx or Localx (x too large)"),
    EXCEP_TXT("AE_AML_REGISTER_LIMIT",
              "Bank value or Index value beyond range of register"),
    EXCEP_TXT("AE_AML_NO_WHILE", "Break or Continue without a While"),
    EXCEP_TXT(
        "AE_AML_ALIGNMENT",
        "Non-aligned memory transfer on platform that does not support this"),
    EXCEP_TXT("AE_AML_NO_RESOURCE_END_TAG", "No End Tag in a resource list"),
    EXCEP_TXT("AE_AML_BAD_RESOURCE_VALUE",
              "Invalid value of a resource element"),
    EXCEP_TXT("AE_AML_CIRCULAR_REFERENCE",
              "Two references refer to each other"),
    EXCEP_TXT("AE_AML_BAD_RESOURCE_LENGTH",
              "The length of a Resource Descriptor in the AML is incorrect"),
    EXCEP_TXT("AE_AML_ILLEGAL_ADDRESS",
              "A memory, I/O, or PCI configuration address is invalid"),
    EXCEP_TXT("AE_AML_INFINITE_LOOP",
              "An apparent infinite AML While loop, method was aborted")};

static const ACPI_EXCEPTION_INFO AcpiGbl_ExceptionNames_Ctrl[] = {
    EXCEP_TXT(NULL, NULL),
    EXCEP_TXT("AE_CTRL_RETURN_VALUE", "A Method returned a value"),
    EXCEP_TXT("AE_CTRL_PENDING", "Method is calling another method"),
    EXCEP_TXT("AE_CTRL_TERMINATE", "Terminate the executing method"),
    EXCEP_TXT("AE_CTRL_TRUE", "An If or While predicate result"),
    EXCEP_TXT("AE_CTRL_FALSE", "An If or While predicate result"),
    EXCEP_TXT("AE_CTRL_DEPTH", "Maximum search depth has been reached"),
    EXCEP_TXT("AE_CTRL_END", "An If or While predicate is false"),
    EXCEP_TXT("AE_CTRL_TRANSFER", "Transfer control to called method"),
    EXCEP_TXT("AE_CTRL_BREAK", "A Break has been executed"),
    EXCEP_TXT("AE_CTRL_CONTINUE", "A Continue has been executed"),
    EXCEP_TXT("AE_CTRL_SKIP", "Not currently used"),
    EXCEP_TXT("AE_CTRL_PARSE_CONTINUE", "Used to skip over bad opcodes"),
    EXCEP_TXT("AE_CTRL_PARSE_PENDING", "Used to implement AML While loops")};

#endif /* EXCEPTION_TABLE */

#endif /* __ACEXCEP_H__ */
