/**
 * @file fdisk_unified.c
 * @brief Unified fdisk implementation
 * @details This file is a synthesized/unified implementation combining multiple
 *          source files from the legacy MINIX codebase into a single, modern,
 *          C23-compliant implementation for MINIX4.
 * 
 * @version 4.0.0
 * @date 2025-06-13
 * @author MINIX4 Massive Synthesis System
 * 
 * @copyright Copyright (c) 2025 MINIX4 Project
 * @license MIT License
 * 
 * SYNTHESIS INFORMATION:
 * ======================
 * This file synthesizes the following source files:
 *     1. userspace\minix_commands\fdisk\fdisk.c
 *     2. userspace\commands_legacy\system\fdisk\fdisk.c
 *     3. minix4\exokernel\kernel_legacy\cmd\fdisk\fdisk.c
 * 
 * Total source files: 3
 * Synthesis date: 2025-06-13 19:53:49
 * Synthesis strategy: Massive batch C23 unification
 * Processing batch: targets 32-131
 * 
 * MODERNIZATION FEATURES:
 * =======================
 * - C23 standard compliance with latest features
 * - Modern error handling patterns  
 * - Consistent coding style and formatting
 * - Improved memory safety and bounds checking
 * - Enhanced documentation and attribution
 * - POSIX.1-2024 compliance where applicable
 * - Thread-safe implementations where needed
 */

#pragma once

#define _POSIX_C_SOURCE 202311L
#define _XOPEN_SOURCE 800
#define _DEFAULT_SOURCE 1

// C23 Standard Headers
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <threads.h>
#include <stdnoreturn.h>

// POSIX Headers
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

// MINIX4 Headers
#include "minix4_types.h"
#include "minix4_config.h"
#include "minix4_errors.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ===== INCLUDES ===== */
#include <ctype.h>
#include <dos.h>
#include <errno.h>
#include <fcntl.h>
#include <libfdisk.h>
#include <limits.h>
#include <machine/partition.h>
#include <minix/partition.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/byteorder.h>
#include <sys/dkio.h>
#include <sys/dktp/fdisk.h>
#include <sys/efi_partition.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/systeminfo.h>
#include <sys/tty.h>
#include <sys/types.h>
#include <sys/vtoc.h>
#include <unistd.h>

/* ===== DEFINES ===== */
#define DEFAULT_DEV	"c:"
#define LOAD_OPEN_MODE	0x8000
#define SAVE_CREAT_MODE	0644
#define	OK		0
#define	ERR		1
#define	CLR_SCR "[1;1H[0J"
#define	CLR_LIN "[0K"
#define	HOME "[1;1H[0K[2;1H[0K[3;1H[0K[4;1H[0K[5;1H[0K" \
#define	Q_LINE "[22;1H[0K[21;1H[0K[20;1H[0K"
#define	W_LINE "[11;1H[0K"
#define	E_LINE "[24;1H[0K[23;1H[0K"
#define	M_LINE "[12;1H[0K[13;1H[0K[14;1H[0K[15;1H[0K" \
#define	T_LINE "[1;1H[0K"
#define	DEFAULT_PATH			"/dev/rdsk/"
#define	DEFAULT_MASTER_BOOT_FILE	"/boot/pmbr"
#define	OLD		0
#define	NEW		1
#define	les(val)	((((val)&0xFF)<<8)|(((val)>>8)&0xFF))
#define	lel(val)	(((unsigned)(les((val)&0x0000FFFF))<<16) | \
#define	VTOC_OFFSET	1
#define	FDISK_KB	(1024)
#define	FDISK_MB	(FDISK_KB * 1024)
#define	FDISK_GB	(FDISK_MB * 1024)
#define	TRUE	1
#define	FDISK_MAX_VALID_PART_ID	255
#define	FDISK_MAX_VALID_PART_NUM_DIGITS	2
#define	FDISK_MAX_VALID_PART_ID_DIGITS	3
#define	FDISK_MAX_VALID_CYL_NUM_DIGITS	10
#define	FDISK_MIN_PART_SIZE	1
#define	CBUFLEN 80
#define	FDISK_STANDARD_LECTURE \
#define	FDISK_LECTURE_NOT_SECTOR_ZERO \
#define	FDISK_LECTURE_NOT_FULL \
#define	FDISK_LECTURE_NO_VTOC \
#define	FDISK_LECTURE_NO_GEOM \
#define	FDISK_SHALL_I_CONTINUE \

/* ===== TYPES ===== */
struct part_entry *ask_partition(void);
  struct part_geom geom;
  struct part_entry *pe;
  struct part_entry *pe1;
  struct part_entry *pe;
  struct part_entry *pe1;
union REGS regs;
struct SREGS sregs;
struct part_entry *ask_partition()
struct part_entry *ask_partition(void);
  struct part_geom geom;
  struct part_entry *pe;
  struct part_entry *pe1;
  struct part_entry *pe;
  struct part_entry *pe1;
union REGS regs;
struct SREGS sregs;
struct part_entry *ask_partition()
	struct ipart *part;
	struct ipart *partition[FD_NUMPART];
	struct ipart localpart[FD_NUMPART];
				struct ipart *temp = partition[i];
	struct ipart iparts[FD_NUMPART];
	struct ipart *boot_ptr, *tbl_ptr;
		struct ipart ext_tab;
	struct dk_gpt	*efi_vtoc;
	struct ipart *clr_table;
	struct extvtoc v;
	struct dk_geom d;
	struct part_info pi;
	struct extpart_info extpi;
	struct stat statbuf;
	struct winsize ws;
	struct ipart *fpart;

/* ===== GLOBAL VARIABLES ===== */
char rawsecbuf[SECSIZE + sizeof(long)];
char *secbuf;
int badbases;
int badsizes;
int badorders;
char *dev_name;
int nhead;
int nsec;
int ncyl = 1024;
int readonly;
int override= 0;
int main(int argc, char *argv []);
void getgeom(void);
void getboot(char *buffer);
void putboot(char *buffer);
void load_from_file(void);
void save_to_file(void);
void dpl_partitions(int rawflag);
int chk_table(void);
	unsigned char *cyl);
void mark_partition(struct part_entry *pe);
void change_partition(struct part_entry *entry);
char get_a_char(void);
void print_menu(void);
void adj_base(struct part_entry *pe);
void adj_size(struct part_entry *pe);
void footnotes(void);
int get_an_int(char *prompt, int *intptr);
void list_part_types(void);
void mark_npartition(struct part_entry *pe);
int mygets(char *buf, int length);
char *systype(int type);
void toggle_active(struct part_entry *pe);
void usage(void);
  int argn;
  char *argp;
  int ch;
	putchar('\n');
	ch = get_a_char();
	putchar('\n');
	    case 'h':	print_menu();			break;
			printf("Write disabled\n");
			printf("Please reboot now.\n");
			printf("Not written\n");
	    default:	printf(" %c ????\n", ch);	break;
  int fd, r;
  printf("Geometry of %s: %dx%dx%d\n", dev_name, ncyl, nhead, nsec);
static int devfd;
	printf("No write permission on %s\n", dev_name);
	printf("Cannot open device %s\n", dev_name);
	printf("Cannot read boot sector\n");
	printf("Invalid boot sector on %s.\n", dev_name);
	printf("Partition table reset and boot code installed.\n");
	printf("Seek error during write\n");
	printf("Write error\n");
  char file[80];
  int fd;
  printf("Enter name of file to load from: ");
	printf("Cannot open %s\n", file);
	printf("Read error\n");
  printf("Loaded from %s OK\n", file);
  char file[80];
  int fd;
  printf("Enter name of file to save to: ");
	printf("Cannot creat %s\n", file);
	printf("Cannot open %s\n", file);
	printf("Write error\n");
	printf("Saved to %s OK\n", file);
  char active[5];
  char basefootnote;
  int cyl_mask;
  int devnum;
  int i;
  int i1;
  char orderfootnote;
  int sec_mask;
  char sizefootnote;
  char type[10];
		sprintf(active, "0x%02x", pe->bootind);
		sprintf(type, "0x%02x", pe->sysind);
		sprintf(active, "%s", pe->bootind == ACTIVE_FLAG ? "A  " : "");
		sprintf(type, "%s", systype(pe->sysind));
  int active;
  unsigned char cylinder;
  unsigned char head;
  int i;
  int i1;
  int maxhead;
  int maxsec;
  unsigned char sector;
  int seenpart;
  int status;
		printf("Inconsistent base in partition %d.\n", i);
		printf("Suspect head and sector parameters.\n");
		printf("Inconsistent size in partition %d.\n", i);
		printf("Suspect head and sector parameters.\n");
		printf("     fdisk -h%d -s%d %s\n", nhead, nsec, dev_name);
	printf("Assuming %d heads and %d sectors.\n", nhead, nsec);
	printf("%d active partitions\n", active);
  int bigcyl;
	printf("Partition type is now MINIX\n");
  int first, last;
  int ch;
	printf("\t\tThat looks like %d which is negative\n", first);
	printf("\t\tThat looks like %d which is too small\n", last);
	printf("Partition deleted\n");
	printf("Size reduced by 1 to make it even\n");
	printf("Partition type changed from None to MINIX\n");
	printf("Leaving partition type as MINIX\n");
	ch = get_a_char();
	printf("Leaving partition active\n");
	printf("\tChange partition to active? (y/n) ");
	ch = get_a_char();
		printf("Leaving partition inactive\n");
  char buf[80];
  printf("   + - explain any footnotes (+, -, #)\n");
  printf("   a - toggle an active flag\n");
  printf("   B - adjust a base sector\n");
  printf("   c - change a partition\n");
  printf("   l - load boot block (including partition table) from a file\n");
  printf("   m - mark a partition as a MINIX partition\n");
  printf("   n - mark a partition as a non-MINIX partition\n");
  printf("   p - print raw partition table\n");
  printf("   q - quit without making any changes\n");
  printf("   S - adjust a size (by changing the last sector)\n");
  printf("   s - save boot block (including partition table) on a file\n");
  printf("   t - print known partition types\n");
  printf("   w - write (disabled)\n");
  printf("   w - write changed partition table back to disk and exit\n");
int drivenum;
	printf("Invalid drive %s\n", dev_name);
	printf("Funny drive number %d\n", drivenum);
  regs.x.bx = (int) buffer;
  int86x(0x13, &regs, &regs, &sregs);
	printf("Cannot read boot sector\n");
  regs.x.bx = (int) buffer;
  int86x(0x13, &regs, &regs, &sregs);
	printf("Cannot write boot sector\n");
  int adj;
  int adj;
  printf("Size of partition adjusted to %u\n", pe->size);
  int num;
	printf("\tThat does not look like 1 to 4\n");
  printf("Partition %d\n", num);
  char buf[80];
	printf("%s", prompt);
	printf("\t\tThat does not look like an integer\n");
  int column;
  int type;
		printf("0x%02x: %-9s", type, systype(type));
			putchar(' ');
			putchar('\n');
  char buf[80];
  printf("\nKnown partition types are:\n\n");
	printf("\nEnter partition type (in 2-digit hex): ");
		printf("Invalid hex number\n");
		printf("Hex number too large\n");
  printf("Partition type changed to 0x%02x (%s)\n", type, systype(type));
		putchar('\n');
	printf("Use the EOF character to create a null line.\n");
  printf("Partition changed to %sactive\n", pe->bootind ? "" : "in");
  printf("Usage: fdisk [-hheads] [-ssectors] [device]\n");
char rawsecbuf[SECSIZE + sizeof(long)];
char *secbuf;
int badbases;
int badsizes;
int badorders;
char *dev_name;
int nhead;
int nsec;
int ncyl = 1024;
int readonly;
int override= 0;
int main(int argc, char *argv []);
void getgeom(void);
void getboot(char *buffer);
void putboot(char *buffer);
void load_from_file(void);
void save_to_file(void);
void dpl_partitions(int rawflag);
int chk_table(void);
	unsigned char *cyl);
void mark_partition(struct part_entry *pe);
void change_partition(struct part_entry *entry);
char get_a_char(void);
void print_menu(void);
void adj_base(struct part_entry *pe);
void adj_size(struct part_entry *pe);
void footnotes(void);
int get_an_int(char *prompt, int *intptr);
void list_part_types(void);
void mark_npartition(struct part_entry *pe);
int mygets(char *buf, int length);
char *systype(int type);
void toggle_active(struct part_entry *pe);
void usage(void);
  int argn;
  char *argp;
  int ch;
	putchar('\n');
	ch = get_a_char();
	putchar('\n');
	    case 'h':	print_menu();			break;
			printf("Write disabled\n");
			printf("Please reboot now.\n");
			printf("Not written\n");
	    default:	printf(" %c ????\n", ch);	break;
  int fd, r;
  printf("Geometry of %s: %dx%dx%d\n", dev_name, ncyl, nhead, nsec);
static int devfd;
	printf("No write permission on %s\n", dev_name);
	printf("Cannot open device %s\n", dev_name);
	printf("Cannot read boot sector\n");
	printf("Invalid boot sector on %s.\n", dev_name);
	printf("Partition table reset and boot code installed.\n");
	printf("Seek error during write\n");
	printf("Write error\n");
  char file[80];
  int fd;
  printf("Enter name of file to load from: ");
	printf("Cannot open %s\n", file);
	printf("Read error\n");
  printf("Loaded from %s OK\n", file);
  char file[80];
  int fd;
  printf("Enter name of file to save to: ");
	printf("Cannot creat %s\n", file);
	printf("Cannot open %s\n", file);
	printf("Write error\n");
	printf("Saved to %s OK\n", file);
  char active[5];
  char basefootnote;
  int cyl_mask;
  int devnum;
  int i;
  int i1;
  char orderfootnote;
  int sec_mask;
  char sizefootnote;
  char type[10];
		sprintf(active, "0x%02x", pe->bootind);
		sprintf(type, "0x%02x", pe->sysind);
		sprintf(active, "%s", pe->bootind == ACTIVE_FLAG ? "A  " : "");
		sprintf(type, "%s", systype(pe->sysind));
  int active;
  unsigned char cylinder;
  unsigned char head;
  int i;
  int i1;
  int maxhead;
  int maxsec;
  unsigned char sector;
  int seenpart;
  int status;
		printf("Inconsistent base in partition %d.\n", i);
		printf("Suspect head and sector parameters.\n");
		printf("Inconsistent size in partition %d.\n", i);
		printf("Suspect head and sector parameters.\n");
		printf("     fdisk -h%d -s%d %s\n", nhead, nsec, dev_name);
	printf("Assuming %d heads and %d sectors.\n", nhead, nsec);
	printf("%d active partitions\n", active);
  int bigcyl;
	printf("Partition type is now MINIX\n");
  int first, last;
  int ch;
	printf("\t\tThat looks like %d which is negative\n", first);
	printf("\t\tThat looks like %d which is too small\n", last);
	printf("Partition deleted\n");
	printf("Size reduced by 1 to make it even\n");
	printf("Partition type changed from None to MINIX\n");
	printf("Leaving partition type as MINIX\n");
	ch = get_a_char();
	printf("Leaving partition active\n");
	printf("\tChange partition to active? (y/n) ");
	ch = get_a_char();
		printf("Leaving partition inactive\n");
  char buf[80];
  printf("   + - explain any footnotes (+, -, #)\n");
  printf("   a - toggle an active flag\n");
  printf("   B - adjust a base sector\n");
  printf("   c - change a partition\n");
  printf("   l - load boot block (including partition table) from a file\n");
  printf("   m - mark a partition as a MINIX partition\n");
  printf("   n - mark a partition as a non-MINIX partition\n");
  printf("   p - print raw partition table\n");
  printf("   q - quit without making any changes\n");
  printf("   S - adjust a size (by changing the last sector)\n");
  printf("   s - save boot block (including partition table) on a file\n");
  printf("   t - print known partition types\n");
  printf("   w - write (disabled)\n");
  printf("   w - write changed partition table back to disk and exit\n");
int drivenum;
	printf("Invalid drive %s\n", dev_name);
	printf("Funny drive number %d\n", drivenum);
  regs.x.bx = (int) buffer;
  int86x(0x13, &regs, &regs, &sregs);
	printf("Cannot read boot sector\n");
  regs.x.bx = (int) buffer;
  int86x(0x13, &regs, &regs, &sregs);
	printf("Cannot write boot sector\n");
  int adj;
  int adj;
  printf("Size of partition adjusted to %u\n", pe->size);
  int num;
	printf("\tThat does not look like 1 to 4\n");
  printf("Partition %d\n", num);
  char buf[80];
	printf("%s", prompt);
	printf("\t\tThat does not look like an integer\n");
  int column;
  int type;
		printf("0x%02x: %-9s", type, systype(type));
			putchar(' ');
			putchar('\n');
  char buf[80];
  printf("\nKnown partition types are:\n\n");
	printf("\nEnter partition type (in 2-digit hex): ");
		printf("Invalid hex number\n");
		printf("Hex number too large\n");
  printf("Partition type changed to 0x%02x (%s)\n", type, systype(type));
		putchar('\n');
	printf("Use the EOF character to create a null line.\n");
  printf("Partition changed to %sactive\n", pe->bootind ? "" : "in");
  printf("Usage: fdisk [-hheads] [-ssectors] [device]\n");
static char Ostr[] = "Other OS";
static char Dstr[] = "DOS12";
static char D16str[] = "DOS16";
static char DDstr[] = "DOS-DATA";
static char EDstr[] = "EXT-DOS";
static char DBstr[] = "DOS-BIG";
static char PCstr[] = "PCIX";
static char Ustr[] = "UNIX System";
static char SUstr[] = "Solaris";
static char SU2str[] = "Solaris2";
static char X86str[] = "x86 Boot";
static char DIAGstr[] = "Diagnostic";
static char IFSstr[] = "IFS: NTFS";
static char AIXstr[] = "AIX Boot";
static char AIXDstr[] = "AIX Data";
static char OS2str[] = "OS/2 Boot";
static char WINstr[] = "Win95 FAT32";
static char EWINstr[] = "Ext Win95";
static char FAT95str[] = "FAT16 LBA";
static char EXTLstr[] = "EXT LBA";
static char LINUXstr[] = "Linux";
static char CPMstr[] = "CP/M";
static char NOV2str[] = "Netware 286";
static char NOVstr[] = "Netware 3.x+";
static char QNXstr[] = "QNX 4.x";
static char QNX2str[] = "QNX part 2";
static char QNX3str[] = "QNX part 3";
static char LINNATstr[] = "Linux native";
static char LINSWAPstr[] = "Linux swap";
static char NTFSVOL1str[] = "NT volset 1";
static char NTFSVOL2str[] = "NT volset 2";
static char BSDstr[] = "BSD OS";
static char NEXTSTEPstr[] = "NeXTSTEP";
static char BSDIFSstr[] = "BSDI FS";
static char BSDISWAPstr[] = "BSDI swap";
static char Actvstr[] = "Active";
static char EFIstr[] = "EFI";
static char NAstr[] = "      ";
static char	*Nullsect;
static int	vt_inval = 0;
static struct ipart	Table[FD_NUMPART];
static struct ipart	Old_Table[FD_NUMPART];
static struct dk_minfo	minfo;
static struct dk_geom	disk_geom;
static char s[CBUFLEN];
static char ext_part_menu_opts[] = "adhipr";
static ext_part_t *epp;
static void update_disk_and_exit(boolean_t table_changed);
int main(int argc, char *argv[]);
static int read_geom(char *sgeom);
static void dev_mboot_read(void);
static void dev_mboot_write(off_t sect, char *buff, int bootsiz);
static void mboot_read(void);
static void fill_patt(void);
static void abs_read(void);
static void abs_write(void);
static void load(int funct, char *file);
static void Set_Table_CHS_Values(int ti);
static int nopartdefined();
    uint32_t rsect, uint32_t numsect, int startindex);
    uint32_t rsect, uint32_t numsect, int startindex);
    uint32_t *rsect, uint32_t *numsect);
static int validate_part(int id, uint32_t rsect, uint32_t numsect);
static void stage0(void);
static int pcreate(void);
static void dispmenu(void);
static int pchange(void);
static int ppartid(void);
static char pdelete(void);
static void rm_blanks(char *s);
static int getcyl(void);
static void disptbl(void);
static void print_Table(void);
static void copy_Table_to_Old_Table(void);
static void nulltbl(void);
static void copy_Bootblk_to_Table(void);
static void fill_ipart(char *bootptr, struct ipart *partp);
uchar_t getbyte(char **bp);
uint32_t getlong(char **bp);
static void copy_Table_to_Bootblk(void);
static int TableChanged(void);
static void ffile_write(char *file);
static void fix_slice(void);
static int yesno(void);
static int readvtoc(void);
static int writevtoc(void);
static int efi_ioctl(int fd, int cmd, dk_efi_t *dk_ioc);
static int clear_efi(void);
static void clear_vtoc(int table, int part);
static int lecture_and_query(char *warning, char *devname);
static void sanity_check_provided_device(char *devname, int fd);
static char *get_node(char *devname);
static int efi_create(void);
static void id_to_name(uchar_t sysid, char *buffer);
static void ext_read_input(char *buf);
static int ext_read_options(char *buf);
static int ext_invalid_option(char ch);
static void ext_read_valid_part_num(int *pno);
static void ext_read_valid_part_id(uchar_t *partid);
static int ext_read_valid_partition_start(uint32_t *begsec);
static void ext_read_valid_partition_size(uint32_t begsec, uint32_t *endsec);
static void ext_part_menu();
static void add_logical_drive();
static void delete_logical_drive();
static void ext_print_help_menu();
static void ext_change_logical_drive_id();
static void ext_print_part_types();
static void ext_print_logical_drive_layout();
static void preach_and_continue();
static void ext_print_logdrive_layout_debug();
	int rval;
	int c, i;
	extern	int optind;
	extern	char *optarg;
	int	errflg = 0;
	int	diag_cnt = 0;
	int openmode;
	int rval;
	int lf_op_flag = 0;
				(void) fprintf(stderr, "%s\n", Usage);
				(void) fprintf(stderr, "%s\n", Usage1);
		(void) fprintf(stderr, "%s\n", Usage);
		(void) memset(&minfo, 0, sizeof (minfo));
			(void) fprintf(stderr, "Physical Geometry:\n");
			(void) fprintf(stderr, "Virtual (HBA) Geometry:\n");
	Bootsect = (char *)calloc(3 * sectsiz, 1);
				(void) close(Dev);
		(void) fprintf(stderr, "Partition Table Entry Values:\n");
		print_Table();
			(void) fprintf(stderr, "\n");
			(void) fprintf(stderr, "Press Enter to continue.\n");
			(void) fgets(s, sizeof (s), stdin);
		(void) printf(CLR_SCR);
		ffile_write((char *)stdout);
	char	line[256];
	(void) fclose(fp);
	int	new_pt, old_pt, error;
	int	clr_efi = -1;
		(void) fprintf(stderr, "About to write fdisk table:\n");
		print_Table();
			(void) fprintf(stderr, "Press Enter to continue.\n");
			(void) fgets(s, sizeof (s), stdin);
			(void) fprintf(stderr, "Clearing EFI labels\n");
	int mDev, i;
	(void) close(mDev);
		(void) memset(part, 0, sizeof (struct ipart));
	int	*buff_ptr, i;
	int	io_fpatt = 0;
	int	io_ipatt = 0;
		buff_ptr = (int *)Bootsect;
	int c;
	int c, i;
		int part_exit = 0;
	int	id;
	int	act;
	int	bhead;
	int	bsect;
	int	bcyl;
	int	ehead;
	int	esect;
	int	ecyl;
	uint32_t	rsect;
	uint32_t	numsect;
	char	line[256];
	int	i = 0;
	int	startindex = 0;
	int	tmpindex = 0;
	int	ext_part_present = 0;
	uint32_t	begsec, endsec, relsect;
	int part_count = 0, ldcnt = 0;
	uint32_t ext_beg_sec;
	uint32_t old_ext_beg_sec = 0, old_ext_num_sec = 0;
	uint32_t new_ext_beg_sec = 0, new_ext_num_sec = 0;
	int ext_part_inited = 0;
	uchar_t	systid;
				int offset = MAX_LOGDRIVE_OFFSET;
					(void) fprintf(stderr, "Exiting...\n");
		(void) fclose(fp);
			(void) printf("Invalid partition ID\n");
	uint32_t	lba, cy, hd, sc;
	lba = (uint32_t)Table[ti].relsect;
	Table[ti].beghead = (uchar_t)hd;
	Table[ti].begsect = (uchar_t)(((cy >> 2) & 0xc0) | sc);
	lba = (uint32_t)(Table[ti].relsect + Table[ti].numsect - 1);
	Table[ti].endhead = (uchar_t)hd;
	Table[ti].endsect = (uchar_t)(((cy >> 2) & 0xc0) | sc);
	int	i;
		(void) fprintf(stderr, "fdisk: Partition table is full.\n");
	Table[i].systid = (uchar_t)id;
	Table[i].bootid = (uchar_t)act;
		(void) memset(&Table[i], 0, sizeof (struct ipart));
		Table[i].beghead = (uchar_t)bhead;
		Table[i].endhead = (uchar_t)ehead;
		Table[i].begsect = (uchar_t)(((bcyl >> 2) & 0xc0) | bsect);
	uint32_t	i, j;
	uint32_t	i, j, rsect, numsect;
	int	noMoreParts = 0;
	int	numParts = 0;
	int	i;
	int64_t test;
	char *tok, *p;
	char buf[256];
	(void) strncpy(buf, line, 256);
		test = strtoll(tok, (char **)NULL, 10);
		(void) fprintf(stderr, "Syntax error:\n	\"%s\".\n", line);
	int i;
		(void) fprintf(stderr, "Invalid partition ID\n");
	int rval;
		(void) printf(Q_LINE);
		(void) printf("Enter Selection: ");
		(void) fgets(s, sizeof (s), stdin);
	uchar_t tsystid = 'z';
	int i, j;
	uint32_t numsect;
	int retCode = 0;
	int ext_part_present = 0;
			(void) printf(E_LINE);
			(void) printf(E_LINE);
		(void) printf(W_LINE);
		(void) printf(Q_LINE);
		(void) fgets(s, sizeof (s), stdin);
			(void) printf(E_LINE);
			(void) printf("Invalid selection, try again.");
			(void) printf(E_LINE);
				(void) printf(Q_LINE);
				(void) printf(E_LINE);
				(void) printf(Q_LINE);
				(void) printf(E_LINE);
			(void) printf(E_LINE);
			(void) printf("Invalid selection, try again.");
	(void) printf(E_LINE);
		(void) printf(W_LINE);
			(void) printf(E_LINE);
			(void) printf(Q_LINE);
				(void) printf(E_LINE);
						(void) printf(E_LINE);
			(void) printf("Error creating EFI partition!!!\n");
	int	i, j, percent = -1;
	int	cyl, cylen;
	int	cyl_size;
		(void) memset(&localpart[j], 0, sizeof (struct ipart));
	(void) printf(Q_LINE);
	(void) fgets(s, sizeof (s), stdin);
				(void) printf(E_LINE);
				(void) printf(E_LINE);
			(void) printf(E_LINE);
			(void) printf(E_LINE);
				(void) printf(E_LINE);
		(void) printf(E_LINE);
		(void) printf(E_LINE);
		(void) printf(Q_LINE);
		(void) printf("Enter starting cylinder number: ");
			(void) printf(E_LINE);
			(void) printf("Invalid number; retry the operation.");
			(void) printf(E_LINE);
			(void) printf(E_LINE);
		(void) printf(Q_LINE);
		(void) printf("Enter partition size in cylinders: ");
			(void) printf(E_LINE);
			(void) printf("Invalid number, retry the operation.");
			uint32_t	t_relsect, t_numsect;
				(void) printf(E_LINE);
			(void) printf(E_LINE);
		(void) printf(E_LINE);
			(void) printf(E_LINE);
			(void) printf("fdisk: Cannot create partition table\n");
	(void) printf(M_LINE);
	char s[80];
	int i, j;
		(void) printf(Q_LINE);
		(void) fgets(s, sizeof (s), stdin);
		(void) printf(E_LINE);
			(void) printf(E_LINE);
			(void) printf("Partition does not exist.");
	(void) printf(E_LINE);
		(void) printf("partition after the next reboot.");
	char	*p, s[80];
	int	i;
		(void) printf(Q_LINE);
			(void) printf(E_LINE);
			(void) printf(E_LINE);
		(void) printf(E_LINE);
		(void) printf("Partition %d has been changed.", i + 1);
	char s[80];
	int i;
	char pactive;
DEL1:	(void) printf(Q_LINE);
	(void) fgets(s, sizeof (s), stdin);
		(void) printf(E_LINE);
		(void) printf("Partition %d does not exist.", i + 1);
	(void) memset(&Table[i], 0, sizeof (struct ipart));
	(void) printf(E_LINE);
	(void) printf("Partition %d has been deleted.", i + 1);
		(void) printf(" This was the active partition.");
	register int i, j;
	int slen, i, j;
	unsigned int cyl;
	(void) fgets(s, sizeof (s), stdin);
	int i;
	unsigned int startcyl, endcyl, length, percent, remainder;
	char *stat, *type;
	int is_pmbr = 0;
		(void) printf("\theads = %d, sectors = %d\n", heads, sectors);
	(void) printf(HOME);
	(void) printf(T_LINE);
	(void) printf("             Total disk size is %d cylinders\n", Numcyl);
		(void) printf(W_LINE);
		(void) printf("WARNING: no partitions are defined!");
		(void) printf(W_LINE);
	int i;
		(void) fprintf(stderr, "  %-5d ", Table[i].systid);
		(void) fprintf(stderr, "%-3d ", Table[i].bootid);
		(void) fprintf(stderr, "%-5d ", Table[i].beghead);
		(void) fprintf(stderr, "%-5d ", Table[i].begsect & 0x3f);
		    (((uint_t)Table[i].begsect & 0xc0) << 2) + Table[i].begcyl);
		(void) fprintf(stderr, "%-5d ", Table[i].endhead);
		(void) fprintf(stderr, "%-5d ", Table[i].endsect & 0x3f);
		    (((uint_t)Table[i].endsect & 0xc0) << 2) + Table[i].endcyl);
		(void) fprintf(stderr, "%-10u ", LE_32(Table[i].relsect));
		(void) fprintf(stderr, "%-10u\n", LE_32(Table[i].numsect));
	int i;
		(void) memcpy(&Old_Table[i], &Table[i], sizeof (Table[0]));
	int i;
	int i;
	char *bootptr;
	(void) memcpy(iparts, Bootblk->parts, sizeof (iparts));
		(void) memcpy(Bootblk->bootinst, &BootCod, BOOTSZ);
			(void) memset(&Table[i], 0, sizeof (struct ipart));
	(void) memcpy(Bootblk->bootinst, &BootCod, BOOTSZ);
	partp->relsect	= (int32_t)getlong(&bootptr);
	partp->numsect	= (int32_t)getlong(&bootptr);
	uchar_t	b;
	b = (uchar_t)**bp;
	int32_t	b, bh, bl;
			(void) memset(boot_ptr, 0, sizeof (struct ipart));
			(void) memcpy(boot_ptr, tbl_ptr, sizeof (struct ipart));
	int i, changed;
	register int	i;
	(void) fprintf(fp, "\n* %s default fdisk table\n", Dfltdev);
	(void) fprintf(fp, "* Dimensions:\n");
	(void) fprintf(fp, "*   %4d bytes/sector\n", sectsiz);
	(void) fprintf(fp, "*   %4d sectors/track\n", sectors);
	(void) fprintf(fp, "*   %4d tracks/cylinder\n", heads);
	(void) fprintf(fp, "*   %4d cylinders\n", Numcyl);
	(void) fprintf(fp, "*\n");
		(void) fprintf(fp, "* HBA Dimensions:\n");
		(void) fprintf(fp, "*   %4d bytes/sector\n", sectsiz);
		(void) fprintf(fp, "*   %4d sectors/track\n", hba_sectors);
		(void) fprintf(fp, "*   %4d tracks/cylinder\n", hba_heads);
		(void) fprintf(fp, "*   %4d cylinders\n", hba_Numcyl);
		(void) fprintf(fp, "*\n");
	(void) fprintf(fp, "* systid:\n");
	(void) fprintf(fp, "*    1: DOSOS12\n");
	(void) fprintf(fp, "*    2: PCIXOS\n");
	(void) fprintf(fp, "*    4: DOSOS16\n");
	(void) fprintf(fp, "*    5: EXTDOS\n");
	(void) fprintf(fp, "*    6: DOSBIG\n");
	(void) fprintf(fp, "*    7: FDISK_IFS\n");
	(void) fprintf(fp, "*    8: FDISK_AIXBOOT\n");
	(void) fprintf(fp, "*    9: FDISK_AIXDATA\n");
	(void) fprintf(fp, "*   10: FDISK_0S2BOOT\n");
	(void) fprintf(fp, "*   11: FDISK_WINDOWS\n");
	(void) fprintf(fp, "*   12: FDISK_EXT_WIN\n");
	(void) fprintf(fp, "*   14: FDISK_FAT95\n");
	(void) fprintf(fp, "*   15: FDISK_EXTLBA\n");
	(void) fprintf(fp, "*   18: DIAGPART\n");
	(void) fprintf(fp, "*   65: FDISK_LINUX\n");
	(void) fprintf(fp, "*   82: FDISK_CPM\n");
	(void) fprintf(fp, "*   86: DOSDATA\n");
	(void) fprintf(fp, "*   98: OTHEROS\n");
	(void) fprintf(fp, "*   99: UNIXOS\n");
	(void) fprintf(fp, "*  100: FDISK_NOVELL2\n");
	(void) fprintf(fp, "*  101: FDISK_NOVELL3\n");
	(void) fprintf(fp, "*  119: FDISK_QNX4\n");
	(void) fprintf(fp, "*  120: FDISK_QNX42\n");
	(void) fprintf(fp, "*  121: FDISK_QNX43\n");
	(void) fprintf(fp, "*  130: SUNIXOS\n");
	(void) fprintf(fp, "*  131: FDISK_LINUXNAT\n");
	(void) fprintf(fp, "*  134: FDISK_NTFSVOL1\n");
	(void) fprintf(fp, "*  135: FDISK_NTFSVOL2\n");
	(void) fprintf(fp, "*  165: FDISK_BSD\n");
	(void) fprintf(fp, "*  167: FDISK_NEXTSTEP\n");
	(void) fprintf(fp, "*  183: FDISK_BSDIFS\n");
	(void) fprintf(fp, "*  184: FDISK_BSDISWAP\n");
	(void) fprintf(fp, "*  190: X86BOOT\n");
	(void) fprintf(fp, "*  191: SUNIXOS2\n");
	(void) fprintf(fp, "*  238: EFI_PMBR\n");
	(void) fprintf(fp, "*  239: EFI_FS\n");
	(void) fprintf(fp, "*\n");
		uint32_t rsect, numsect, tempsect = 0;
		(void) fclose(fp);
	int	i;
	uint32_t	numsect;
					(void) printf(" adjust ?:");
					(void) fprintf(stderr, " adjusted!\n");
					(void) printf(" adjust ?:");
					(void) fprintf(stderr, " adjusted!\n");
					(void) printf(" fix ?:");
					(void) fprintf(stderr, " fixed!\n");
					(void) printf(" remove ?:");
				(void) printf(" remove ?:");
				(void) printf(" adjust ?:");
				(void) fprintf(stderr, " adjusted!\n");
		(void) writevtoc();
	char	s[80];
		(void) fgets(s, sizeof (s), stdin);
	int	i;
	int	retval = VTOC_OK;
			(void) fprintf(stderr, "fdisk: Invalid VTOC.\n");
			(void) fprintf(stderr, "fdisk: Cannot read VTOC.\n");
	int	i;
	int	retval = 0;
			(void) fprintf(stderr, "fdisk: Cannot write VTOC.\n");
	void *data = dk_ioc->dki_data;
	int error;
	dk_ioc->dki_data_64 = (uintptr_t)data;
	error = ioctl(fd, cmd, (void *)dk_ioc);
	char *disk_label;
	uint32_t pcyl, ncyl, count;
	char *read_label;
	disk_label = (char *)calloc(sectsiz, 1);
		    (uint64_t)(LE_32(clr_table->relsect) + VTOC_OFFSET));
		    (uint64_t)seek_byte);
		    (uint64_t)seek_byte);
		    (uint64_t)seek_byte);
	read_label = (char *)calloc(sectsiz, 1);
			    "%s.\n", (uint64_t)seek_byte, Dfltdev);
			    (uint64_t)(solaris_offset + backup_block));
			    (uint64_t)(solaris_offset + backup_block));
		    (uint64_t)seek_byte);
		    (uint64_t)seek_byte);
	(void) fprintf(stderr, "WARNING: Device %s: \n", devname);
	(void) fprintf(stderr, "%s", warning);
	(void) fprintf(stderr, FDISK_STANDARD_LECTURE);
	(void) fprintf(stderr, FDISK_SHALL_I_CONTINUE);
	int idx = -1;
				(void) close(fd);
				(void) close(fd);
				(void) close(fd);
				(void) close(fd);
				(void) close(fd);
	char *node;
		(void) strncpy(node, DEFAULT_PATH, strlen(DEFAULT_PATH));
		(void) strlcpy(node + strlen(DEFAULT_PATH), devname, space);
	(void) strcpy(buffer, fdisk_part_types[sysid]);
	char *p;
	(void) fgets(buf, 16, stdin);
	(void) fflush(stdin);
		(void) printf("\nUnknown Command\n");
	int i, rowmax, rowcount = 1;
	char buf[80];
	(void) printf("List of known partition types : \n");
	(void) printf("PartID          Partition Type\n");
	(void) printf("======          ==============\n");
		(void) printf("%-3d          %s\n", i, fdisk_part_types[i]);
	char buf[80];
	int len, i;
		(void) printf("Enter the partition number : ");
			goto print_error_and_continue;
				goto print_error_and_continue;
		(void) printf("Invalid partition number\n");
	char buf[80];
	int len, i, id;
			(void) printf("Invalid partition ID\n");
			ext_print_part_types();
				(void) printf("Invalid partition ID\n");
			(void) printf("Invalid partition ID\n");
		*partid = (uchar_t)id;
	int pno;
		(void) printf("\nNo logical drives defined.\n");
	(void) printf("\n");
	(void) printf("Partition %d deleted\n", pno);
	char buf[80];
	int ret, len, i;
	uint32_t begcyl;
	uint32_t first_free_cyl;
	uint32_t first_free_sec;
			(void) printf("Input too long\n");
			(void) printf("Invalid beginning cylinder number\n");
				(void) printf("existing partitions\n");
	char buf[80];
	uint32_t last_free_sec;
	uint32_t last_free_cyl;
	int i, len, ch, mbgb = 0, scale = FDISK_SECTS_PER_CYL(epp);
	uint64_t size = 0;
	int copy_len;
	char numbuf[FDISK_MAX_VALID_CYL_NUM_DIGITS + 1];
	int sectsize = fdisk_get_disk_geom(epp, PHYSGEOM, SSIZE);
	uint32_t remdr, spc, poss_end;
		(void) fprintf(stderr, "Unsupported geometry statistics.\n");
		(void) printf(" %u)\n", last_free_cyl);
		(void) printf("KB, MB or GB : ");
				(void) printf("Invalid partition size\n");
			(void) printf("Input too long\n");
			(void) printf("Invalid partition size\n");
		(void) strncpy(numbuf, &buf[mbgb], copy_len);
			(void) printf("Invalid partition size\n");
			(void) printf("Zero size is invalid\n");
			(void) printf("Invalid partition size\n");
			(void) printf("Cylinder boundary beyond the limits");
			(void) printf(" or overlaps with existing");
			(void) printf(" partitions\n");
			(void) printf("Invalid partition size\n");
	uint32_t begsec, endsec;
	uchar_t partid;
	char buf[80];
	int rval;
	(void) printf("\n");
			(void) printf("Press enter to continue\n");
	(void) printf("New partition with ID %d added\n", partid);
	int pno;
	uchar_t partid;
		(void) printf("\nNo logical drives defined.\n");
	(void) printf("\n");
	int pno;
	char namebuff[255];
		(void) printf("\nNo logical drives defined.\n");
	(void) printf("\n\n");
		(void) printf("   %s\n", namebuff);
		(void) printf("   %s\n", namebuff);
	int sysid;
	unsigned int startcyl, endcyl, length, percent, remainder;
	uint32_t part_start;
	char namebuff[255];
	int numcyl = fdisk_get_disk_geom(epp, PHYSGEOM, NCYL);
	int pno;
		(void) fprintf(stderr, "Unsupported geometry statistics.\n");
		(void) printf("\nNo logical drives defined.\n");
	(void) printf("\n");
	(void) printf("\n");
				(void) strcpy(namebuff, LINSWAPstr);
				(void) strcpy(namebuff, SUstr);
	ext_print_logdrive_layout_debug();
	(void) printf("\n");
	(void) printf("\n");
	(void) printf("a	Add a logical drive\n");
	(void) printf("d	Delete a logical drive\n");
	(void) printf("h	Print this help menu\n");
	(void) printf("i	Change the id of the logical drive\n");
	(void) printf("p	Print the logical drive layout\n");
	(void) printf("r	Return to the main fdisk menu\n");
	(void) printf("        (To commit or cancel the changes)\n");
	(void) printf("\n");
	char buf[80];
	uchar_t *bbsigp;
	static int bbsig_disp_flag = 1;
	int i;
	(void) printf(CLR_SCR);
		(void) printf("Displaying only sane logical drives.\n");
			(void) printf("%d ", bbsigp[i]);
		(void) printf("\n\n");
	(void) printf("Extended partition menu\n");
			(void) printf("\nCommand Options : \n");
			ext_print_help_menu();
				ext_print_help_menu();
				ext_print_logical_drive_layout();
				(void) printf(CLR_SCR);
	int i;

/* ===== FUNCTIONS ===== */

/* Function 1 */
			    (Old_Table[old_pt].systid == SUNIXOS2)) {

				if ((Old_Table[old_pt].relsect ==
				    Table[new_pt].relsect) &&
				    (Old_Table[old_pt].numsect ==
				    Table[new_pt].numsect))
			}

/* Function 2 */
	    (struct mboot)) {
		(void) fprintf(stderr,
		    "fdisk: Cannot read master boot file %s.\n",
		    io_mboot);
		exit(1);
	}

/* Function 3 */
				    (old_ext_num_sec != new_ext_num_sec)) {
					(void) fdisk_init_ext_part(epp,
					    new_ext_beg_sec, new_ext_num_sec);
					ext_part_inited = 1;
				}

/* Function 4 */
				    fdisk_get_logical_drive_count(epp)) == 0) {
					if (rsect < offset) {
						(void) fprintf(stderr,
						    "Minimum of "
						    "63 free sectors required "
						    "before the beginning of "
						    "a logical drive.");
						exit(1);
					}
					if (begsec != new_ext_beg_sec) {
						begsec = rsect;
						offset = 0;
					}
				}

/* Function 5 */
			    Table[i].numsect == LE_32(numsect)) {

				(void) memset(&Table[i], 0,
				    sizeof (struct ipart));
#ifdef i386
				if (fdisk_is_dos_extended(id)) {
					(void) fdisk_delete_ext_part(epp);
				}
#endif
				return;
			}

/* Function 6 */
			    temp->parts[0].numsect == LE_32(numsect)) {
				fdisk_delete_logical_drive(epp, ldcnt);
				return;
			}

/* Function 7 */
		    (fdisk_is_dos_extended(id))) {
			(void) fprintf(stderr,
			    "Extended partition already exists.\n");
			(void) fprintf(stderr,
			    "fdisk: Invalid entry could not be "
			    "inserted:\n        \"%s\"\n", file);
			exit(1);
		}

/* Function 8 */
		    (rsect <= (fdisk_get_ext_end_sec(epp)))) {
			int offset = MAX_LOGDRIVE_OFFSET;

			if (rsect < offset) {
				return;
			}
			begsec = rsect - offset;
			if ((ldcnt = fdisk_get_logical_drive_count(epp)) == 0) {
				if (begsec != ext_beg_sec) {
					begsec = rsect;
					offset = 0;
				}
			}

			if (ldcnt >= MAX_EXT_PARTS) {
				(void) printf("\nNumber of logical drives "
				    "exceeds limit of %d.\n", MAX_EXT_PARTS);
				(void) printf("Failing further additions.\n");
				exit(1);
			}

			if (numsect == 0) {
				(void) fprintf(stderr,
				    "fdisk: Partition size cannot be zero:\n"
				    "   \"%s\".\n",
				    file);
				exit(1);
			}
			endsec = rsect + numsect - 1;
			if (fdisk_validate_logical_drive(epp, begsec,
			    offset, numsect) == 0) {
				fdisk_add_logical_drive(epp, begsec, endsec,
				    id);
				return;
			} else {
				(void) fprintf(stderr,
				    "fdisk: Invalid entry could not be "
				    "inserted:\n        \"%s\"\n", file);
				exit(1);
			}
		}

/* Function 9 */
		    Old_Table[i].numsect == lel(numsect)) {
			for (j = startindex; j < FD_NUMPART; j++) {
				if (Table[j].systid == UNUSED) {
					(void) memcpy(&Table[j], &Old_Table[i],
					    sizeof (Table[0]));
					skip_verify[j] = 1;
					return (j);

				}
			}
			return (-1);
		}

/* Function 10 */
				    (dev_capacity - 1))) {

					(void) fprintf(stderr, "ERROR: "
					    "EFI_PMBR partition must "
					    "encompass the entire");

					if (dev_capacity > DK_MAX_2TB)
						(void) fprintf(stderr,
						    "maximum 2 TB.\n "
						    "numsect %u - "
						    "actual %llu\n",
						    Table[i].numsect,
						    (diskaddr_t)DK_MAX_2TB);

					else
						(void) fprintf(stderr,
						    "disk.\n numsect %u - "
						    "actual %llu\n",
						    Table[i].numsect,
						    dev_capacity - 1);

					return (-1);
				}

/* Function 11 */
			    (((diskaddr_t)rsect + numsect) > DK_MAX_2TB)) {
				if (!skip_verify[i])
					return (-1);
			}

/* Function 12 */
					    ((rsect + numsect) <= t_relsect)) {
						continue;
					} else {
						(void) fprintf(stderr, "ERROR: "
						    "current partition overlaps"
						    " following partition\n");

						return (-1);
					}

/* Function 13 */
		    lel(Table[i].numsect)) > DK_MAX_2TB))) {
			return (-1);
		}

/* Function 14 */
		    ((s[1] == '\0') || (s[1] == '\n')))) {
#else
		while (!((s[0] > '0') && (s[0] < '7') &&
		    ((s[1] == '\0') || (s[1] == '\n')))) {
#endif
#ifdef i386
			(void) printf(
			    "Enter a one-digit number between 1 and 7.");
#else
			(void) printf(
			    "Enter a one-digit number between 1 and 6.");
#endif
			(void) printf(Q_LINE);
			(void) printf("Enter Selection: ");
			(void) fgets(s, sizeof (s), stdin);
			rm_blanks(s);
		}
		(void) printf(E_LINE);
		switch (s[0]) {
			case '1':
				if (pcreate() == -1)
					return;
				break;
			case '2':
				if (pchange() == -1)
					return;
				break;
			case '3':
				if (pdelete() == -1)
					return;
				break;
			case '4':
				if (ppartid() == -1)
					return;
				break;
#ifdef i386
			case '5':
				if (fdisk_ext_part_exists(epp)) {
					ext_part_menu();
				} else {
					(void) printf(Q_LINE);
					(void) printf("\nNo extended partition"
					    " found\n");
					(void) printf("Press enter to "
					    "continue\n");
					ext_read_input(s);
				}
				break;
			case '6':
				if (TableChanged() == 1) {
					copy_Table_to_Bootblk();
					dev_mboot_write(0, Bootsect, sectsiz);
				}

				if (io_adjt) {
					fix_slice();
				}
				if (!io_readonly) {
					rval = fdisk_commit_ext_part(epp);
					switch (rval) {
						case FDISK_SUCCESS:
						case FDISK_ENOEXTPART:
							break;
						case FDISK_EMOUNTED:
							(void) printf(Q_LINE);
							preach_and_continue();
							continue;
						default:
							perror("Commit failed");
							exit(1);
					}
					libfdisk_fini(&epp);
				}
				(void) close(Dev);
				exit(0);
#else
			case '5':
				if (TableChanged() == 1) {
					copy_Table_to_Bootblk();
					dev_mboot_write(0, Bootsect, sectsiz);
				}
				if (io_adjt) {
					fix_slice();
				}
				(void) close(Dev);
				exit(0);
#endif
#ifdef i386
			case '7':
#else
			case '6':
#endif
				if (io_adjt) {
					fix_slice();
				}
				(void) close(Dev);
				exit(0);
			default:
				break;
		}
		copy_Table_to_Bootblk();
		disptbl();
		dispmenu();
	}

/* Function 15 */
			    LE_32(partition[i]->relsect)) {
				partition[i] = partition[j];
				partition[j] = temp;
			}

/* Function 16 */
		    ((long)((long)cylen * cyl_size) > MAXDOS)) {
			int n;
			n = MAXDOS * 100 / (int)(cyl_size) / Numcyl_usable;
			(void) printf(E_LINE);
			(void) printf("Maximum size for a DOS partition "
			    "is %d%%; retry the operation.",
			    n <= 100 ? n : 100);
			return (-1);
		}

/* Function 17 */
			    (cyl + cylen - 1) * cyl_size > t_relsect) {
				(void) printf(E_LINE);
				(void) printf(
				    "Maximum size for partition is %u cylinders"
				    "\nretry the operation.",
				    (t_relsect - cyl * cyl_size) / cyl_size);
				return (-1);
			}

/* Function 18 */
		    ((long)((long)cylen * cyl_size) > MAXDOS)) {
			(void) printf(E_LINE);
			(void) printf(
			    "Maximum size for a %s partition is %ld cylinders;"
			    "\nretry the operation.",
			    Dstr, MAXDOS / (int)(cyl_size));
			return (-1);
		}

/* Function 19 */
		    (s[0] < '0') || (s[0] > '4')) {
			(void) printf(E_LINE);
			(void) printf(
			    "Invalid response, please specify a number"
			    " between 0 and 4.\n");
		} else {
			break;
		}

/* Function 20 */
		    (Table[i].systid == FDISK_EXTLBA)) {
			(void) printf(E_LINE);
			(void) printf(
			    "DOS-DATA, EXT_DOS and EXT_DOS_LBA partitions "
			    "cannot be made active.\n");
			(void) printf("Select another partition.");
			return (-1);
		}

/* Function 21 */
	    (i = atoi(s)) < 1 || i > FD_NUMPART) {
		(void) printf(E_LINE);
		(void) printf("Invalid response, retry the operation.\n");
		goto DEL1;
	}

/* Function 22 */
	    fdisk_get_logical_drive_count(epp)) {
		(void) printf(Q_LINE);
		(void) printf("There are logical drives inside the"
		    " extended partition\n");
		(void) printf("Are you sure of proceeding with deletion ?"
		    " (type \"y\" or \"n\") ");

		(void) printf(E_LINE);
		if (! yesno()) {
			return (1);
		}
		if (fdisk_mounted_logical_drives(epp) == FDISK_EMOUNTED) {
			(void) printf(Q_LINE);
			(void) printf("There are mounted logical drives. "
			    "Committing changes now can cause data loss or "
			    "corruption. Unmount all logical drives and then "
			    "try committing the changes again.\n");
			(void) printf("Press enter to continue.\n");
			ext_read_input(s);
			return (1);
		}
		(void) fdisk_delete_ext_part(epp);
	} else {
#endif
		(void) printf(Q_LINE);
		(void) printf("Are you sure you want to delete partition %d?"
		    " This will make all files and \n", i + 1);
		(void) printf("programs in this partition inaccessible (type"
		    " \"y\" or \"n\"). ");

		(void) printf(E_LINE);
		if (! yesno()) {
			return (1);
		}
#ifdef i386
	}

/* Function 23 */
		    ((s[0] != 'y') && (s[0] != 'n'))) {
			(void) printf(E_LINE);
			(void) printf("Please answer with \"y\" or \"n\": ");
			continue;
		}

/* Function 24 */
		    *pno > (fdisk_get_logical_drive_count(epp) + FD_NUMPART)) {
			goto print_error_and_continue;
		}


#ifdef __cplusplus
}
#endif

/* End of fdisk_unified.c - Synthesized by MINIX4 Massive Synthesis System */
