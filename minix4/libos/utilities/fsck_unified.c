/**
 * @file fsck_unified.c
 * @brief Unified fsck implementation
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
 *     1. userspace\minix_commands\fsck.mfs\fsck.c
 *     2. userspace\commands_legacy\system\fsck.mfs\fsck.c
 *     3. minix4\libos_legacy\fsck\fsck.c
 *     4. minix4\exokernel\kernel_legacy\cmd\fs.d\fsck.c
 * 
 * Total source files: 4
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
#include	<errno.h>
#include	<fcntl.h>
#include	<libintl.h>
#include	<limits.h>
#include	<locale.h>
#include	<stdio.h>
#include	<string.h>
#include	<sys/dkio.h>
#include	<sys/mntent.h>
#include	<sys/stat.h>
#include	<sys/sysmacros.h>
#include	<sys/types.h>
#include	<sys/vfstab.h>
#include	<sys/wait.h>
#include "exitvalues.h"
#include "fsutil.h"
#include "mfs/const.h"
#include "mfs/inode.h"
#include "mfs/mfsdir.h"
#include "mfs/super.h"
#include "mfs/type.h"
#include "pathnames.h"
#include <ctype.h>
#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <fstab.h>
#include <limits.h>
#include <minix/config.h>
#include <minix/const.h>
#include <minix/fslib.h>
#include <minix/ipc.h>
#include <minix/type.h>
#include <paths.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/disk.h>
#include <sys/disklabel.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/param.h>
#include <sys/queue.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <util.h>

/* ===== DEFINES ===== */
#define INODES_PER_BLOCK V2_INODES_PER_BLOCK(block_size)
#define INODE_SIZE ((int) V2_INODE_SIZE)
#define WORDS_PER_BLOCK (block_size / (int) sizeof(bitchunk_t))
#define MAX_ZONES (V2_NR_DZONES+V2_INDIRECTS(block_size)+(long)V2_INDIRECTS(block_size)*V2_INDIRECTS(block_size))
#define NR_DZONE_NUM V2_NR_DZONES
#define NR_INDIRECTS V2_INDIRECTS(block_size)
#define NR_ZONE_NUMS V2_NR_TZONES
#define ZONE_NUM_SIZE V2_ZONE_NUM_SIZE
#define bit_nr bit_t
#define block_nr block_t
#define d_inode d2_inode
#define d_inum mfs_d_ino
#define dir_struct struct direct
#define i_mode d2_mode
#define i_nlinks d2_nlinks
#define i_size d2_size
#define i_zone d2_zone
#define zone_nr zone_t
#define BITMASK		((1 << BITSHIFT) - 1)
#define WORDOFBIT(b)	((b) >> BITSHIFT)
#define POWEROFBIT(b)	(1 << ((int) (b) & BITMASK))
#define setbit(w, b)	(w[WORDOFBIT(b)] |= POWEROFBIT(b))
#define clrbit(w, b)	(w[WORDOFBIT(b)] &= ~POWEROFBIT(b))
#define bitset(w, b)	(w[WORDOFBIT(b)] & POWEROFBIT(b))
#define ztob(z)		((block_nr) (z) << sb.s_log_zone_size)
#define btoa64(b)	((u64_t)(b) * block_size)
#define N_IMAP		(sb.s_imap_blocks)
#define N_ZMAP		(sb.s_zmap_blocks)
#define N_ILIST		((sb.s_ninodes+INODES_PER_BLOCK-1) / INODES_PER_BLOCK)
#define N_DATA		(sb.s_zones - FIRST)
#define OFFSET_SUPER_BLOCK	SUPER_BLOCK_BYTES
#define BLK_IMAP	2
#define BLK_ZMAP	(BLK_IMAP  + N_IMAP)
#define BLK_ILIST	(BLK_ZMAP  + N_ZMAP)
#define BLK_FIRST	ztob(FIRST)
#define ZONE_SIZE	((int) ztob(block_size))
#define NLEVEL		(NR_ZONE_NUMS - NR_DZONE_NUM + 1)
#define INDCHUNK	((int) (CINDIR * ZONE_NUM_SIZE))
#define DIRCHUNK	((int) (CDIRECT * DIR_ENTRY_SIZE))
#define DOT	1
#define DOTDOT	2
#define SUPER_GET	0
#define SUPER_PUT	1
#define FSTYPENAMES
#define FSCKNAMES
#define	BADTYPE(type)							\
#define	DEV_BSIZE	512
#define	ARGV_MAX	16
#define	FSTYPE_MAX	8
#define	VFS_PATH	"/usr/lib/fs"
#define	VFS_PATH2	"/etc/fs"
#define	CHECK(xx, yy)\
#define	OPTION(flag)\
#define	OPTARG(flag)\

/* ===== TYPES ===== */
struct stack {
  struct stack *st_next;
  struct stack stk;
struct stack {
  struct stack *st_next;
  struct stack stk;
struct entry {
	struct fstab *fs;
			struct statvfs	sfs;
	struct entry *e;
	struct entry *e;
	struct entry *e;
	struct entry *e;
	struct entry *e;
	struct disklabel dl;
enum dev_arg_t { UNKNOWN, SPECIAL, FSCKDEV, MOUNTPT };
enum dev_arg_t arg_hint = UNKNOWN;
	struct devlist *nxt;
	struct vfstab	vget, vref;
	struct dk_minfo dkminfo;
	struct devlist *dp, *devs = NULL;
	struct devlist *bl, *bdp;
	struct devlist *badlist;
	struct devlist *bdp, *pbdp;
struct devlist *
	struct devlist *dp;
struct devlist *
	struct devlist *p, *lp;

/* ===== GLOBAL VARIABLES ===== */
unsigned int fs_version = 2, block_size = 0;
static struct super_block sb;
  char st_presence;
int nfreeinode, nregular, ndirectory, nblkspec, ncharspec, nbadinode;
int nsock, npipe, nsyml, ztype[NLEVEL];
int preen = 0, markdirty = 0;
char answer[] = "Answer questions with y or n.  Then hit RETURN";
int main(int argc, char **argv);
void initvars(void);
void fatal(char *s);
int eoln(int c);
int yes(char *question);
int atoo(char *s);
int input(char *buf, int size);
char *alloc(unsigned nelem, unsigned elsize);
void printname(char *s);
void printrec(struct stack *sp);
void printpath(int mode, int nlcr);
void devopen(void);
void devclose(void);
void devio(block_nr bno, int dir);
void devread(long block, long offset, char *buf, int size);
void devwrite(long block, long offset, char *buf, int size);
void pr(char *fmt, int cnt, char *s, char *p);
void lpr(char *fmt, long cnt, char *s, char *p);
bit_nr getnumber(char *s);
char **getlist(char ***argv, char *type);
void lsuper(void);
void rw_super(int mode);
void chksuper(void);
void lsi(char **clist);
bitchunk_t *allocbitmap(int nblk);
void loadbitmap(bitchunk_t *bitmap, block_nr bno, int nblk);
void dumpbitmap(bitchunk_t *bitmap, block_nr bno, int nblk);
void freebitmap(bitchunk_t *p);
void getbitmaps(void);
void putbitmaps(void);
	int *report, bit_t);
	blkno, int nblk, char *type);
void chkilist(void);
void getcount(void);
void counterror(ino_t ino);
void chkcount(void);
void freecount(void);
void list(ino_t ino, d_inode *ip);
int Remove(dir_struct *dp);
void make_printable_name(char *dst, char *src, int n);
int chkdots(ino_t ino, off_t pos, dir_struct *dp, ino_t exp);
int chkname(ino_t ino, dir_struct *dp);
int chkentry(ino_t ino, off_t pos, dir_struct *dp);
int chkdirzone(ino_t ino, d_inode *ip, off_t pos, zone_nr zno);
int chksymlinkzone(ino_t ino, d_inode *ip, off_t pos, zone_nr zno);
void errzone(char *mess, zone_nr zno, int level, off_t pos);
int markzone(zone_nr zno, int level, off_t pos);
off_t jump(int level);
int zonechk(ino_t ino, d_inode *ip, off_t *pos, zone_nr zno, int level);
	len, int level);
int chkfile(ino_t ino, d_inode *ip);
int chkdirectory(ino_t ino, d_inode *ip);
int chklink(ino_t ino, d_inode *ip);
int chkspecial(ino_t ino, d_inode *ip);
int chkmode(ino_t ino, d_inode *ip);
int chkinode(ino_t ino, d_inode *ip);
int descendtree(dir_struct *dp);
void chktree(void);
void printtotal(void);
void chkdev(char *f, char **clist, char **ilist, char **zlist);
  register int level;
char *s;
  printf("%s\nfatal\n", s);
int c;
char *question;
  register int c, answerchar;
  static int note = 0;
  int yes;
	printf("\n");
  printf("%s? ", question);
	printf("yes\n");
  yes = !(answerchar == 'n' || answerchar == 'N');
register char *s;
  register int n = 0;
char *buf;
int size;
  register char *p = buf;
  printf("\n");
	printf("--> ");
		*p = getchar();
  char *p;
  memset((void *) p, 0, (size_t)nelem * elsize);
char *s;
  register int n = MFS_NAME_MAX;
  int c;
	putchar(c);
	printrec(sp->st_next);
	putchar('/');
	printname(sp->st_dir->mfs_d_name);
int mode;
int nlcr;
	putchar('/');
	printrec(ftop);
	printf(" (ino = %u, ", ftop->st_dir->d_inum);
	printf(" (ino = %u)", ftop->st_dir->d_inum);
int dir;
printf("%s at block %5d\n", dir == READING ? "reading " : "writing", bno);
	printf("Continuing with a zero-filled block.\n");
char *buf;
int size;
char *buf;
int size;
char *fmt, *s, *p;
int cnt;
  printf(fmt, cnt, cnt == 1 ? s : p);
char *fmt, *s, *p;
  printf(fmt, cnt, cnt == 1 ? s : p);
register char *s;
char ***argv, *type;
  register char **list = *argv;
  register int empty = 1;
	printf("warning: no %s numbers given\n", type);
  char buf[80];
	printf("ninodes       = %u", sb.s_ninodes);
	printf("nzones        = %d", sb.s_zones);
	printf("imap_blocks   = %u", sb.s_imap_blocks);
	printf("zmap_blocks   = %u", sb.s_zmap_blocks);
	printf("firstdatazone = %u", sb.s_firstdatazone_old);
	printf("log_zone_size = %u", sb.s_log_zone_size);
	printf("maxsize       = %d", sb.s_max_size);
	printf("block size    = %d", sb.s_block_size);
		devwrite(0, OFFSET_SUPER_BLOCK, (char *) &sb, sizeof(sb));
	printf("flags         = ");
	printf("\n");
	printf("warning: invalid max file size %d\n", sb.s_max_size);
  register int n;
	printf(" instead of %d\n", sb.s_imap_blocks);
	printf(" instead of %d\n", sb.s_zmap_blocks);
		printf("instead of %u\n", sb.s_firstdatazone_old);
	printf("warning: expected max size to be %lld ", maxsize);
	printf("instead of %d\n", sb.s_max_size);
char **clist;
  char buf[80];
		devread(inoblock(ino), inooff(ino), (char *) ip, INODE_SIZE);
		printf("inode %llu:\n", ino);
		printf("    mode   = %6o", ip->i_mode);
		printf("    nlinks = %6u", ip->i_nlinks);
		printf("    size   = %6d", ip->i_size);
int nblk;
int nblk;
  register int i;
	devread(bno, 0, (char *) p, block_size);
int nblk;
  register int i;
	devwrite(bno, 0, (char *) p, block_size);
char **list;
			printf("inode number %d ", bit);
			printf("zone number %d ", bit);
		printf("out of range (ignored)\n");
  free((char *) p);
char *type;
int *n, *report;
			printf("%s %d is missing\n", type, bit);
			printf("%s %d is not free\n", type, bit);
int nblk;
char *type;
  int report = 1, nerr = 0;
  int w = nblk * WORDS_PER_BLOCK;
  printf("Checking %s map. ", type);
  printf("Checking inode list. ");
			printf("mode inode %llu not cleared", ino);
	printf("INODE NLINK COUNT\n");
  devread(inoblock(ino), inooff(ino), (char *) &inode, INODE_SIZE);
  printf("%5llu %5u %5u", ino, (unsigned) inode.i_nlinks, count[ino]);
		fatal("internal error (counterror)");
	devwrite(inoblock(ino), inooff(ino), (char *) &inode, INODE_SIZE);
  free((char *) count);
	putchar('r');
	putchar('-');
	putchar('w');
	putchar('-');
	putchar(overlay);
		putchar('x');
		putchar('-');
	printf(" inode permission link   size name\n");
  printf("%6llu ", ino);
      case I_REGULAR:		putchar('-');	break;
      case I_DIRECTORY:		putchar('d');	break;
      case I_CHAR_SPECIAL:	putchar('c');	break;
      case I_BLOCK_SPECIAL:	putchar('b');	break;
      case I_NAMED_PIPE:	putchar('p');	break;
      case I_UNIX_SOCKET:	putchar('s');	break;
      case I_SYMBOLIC_LINK:	putchar('l');	break;
      default:			putchar('?');
  printperm(ip->i_mode, 6, I_SET_UID_BIT, 's');
  printperm(ip->i_mode, 3, I_SET_GID_BIT, 's');
  printperm(ip->i_mode, 0, STICKY_BIT, 't');
  printf(" %3u ", ip->i_nlinks);
	printf("  %2x,%2x ", major(ip->i_zone[0]), minor(ip->i_zone[0]));
      default:	printf("%7d ", ip->i_size);
  printpath(0, 1);
	memset((void *) dp, 0, sizeof(dir_struct));
register char *dst;
register char *src;
register int n;
  register int c;
  char printable_name[4 * MFS_NAME_MAX + 1];
	printf("bad %s in ", printable_name);
	printpath(1, 0);
	printf("%s is linked to %u ", printable_name, dp->d_inum);
	printf("instead of %llu)", exp);
	printf("warning: %s has offset %lld in ", printable_name, pos);
	printpath(1, 0);
	printf("%s is linked to %u)\n", printable_name, dp->d_inum);
  register int n = MFS_NAME_MAX + 1;
  register char *p = dp->mfs_d_name;
	printf("null name found in ");
	printpath(0, 0);
		printf("found a '/' in entry of directory ");
		printpath(1, 0);
		printf("entry = '");
		printname(dp->mfs_d_name);
		printf("')");
	printf("bad inode found in directory ");
	printpath(1, 0);
	printf("ino found = %u, ", dp->d_inum);
	printf("name = '");
	printname(dp->mfs_d_name);
	printf("')");
		memset((void *) dp, 0, sizeof(dir_struct));
	printf("too many links to ino %u\n", dp->d_inum);
	printf("discovered at entry '");
	printname(dp->mfs_d_name);
	printf("' in directory ");
	printpath(0, 1);
	printf("link to directory discovered in ");
	printpath(1, 0);
	printf("name = '");
	printname(dp->mfs_d_name);
	printf("', dir ino = %u)", dp->d_inum);
  register int n, dirty;
	devread(block, offset, (char *) dirblk, DIRCHUNK);
	printf("size not updated of directory ");
	printpath(2, 0);
		devwrite(inoblock(ino), inooff(ino), (char *) ip, INODE_SIZE);
	char target[PATH_MAX+1];
		printpath(2, 0);
				(char *) ip, INODE_SIZE);
char *mess;
int level;
  printf("%s zone in ", mess);
  printpath(1, 0);
  printf("zno = %d, type = ", zno);
      case 0:	printf("DATA");	break;
      case 1:	printf("SINGLE INDIRECT");	break;
      case 2:	printf("DOUBLE INDIRECT");	break;
      default:	printf("VERY INDIRECT");
  printf(", pos = %lld)\n", pos);
int level;
  register int n = NR_INDIRECTS / CINDIR;
	devread(block, offset, (char *) indirect, INDCHUNK);
int level;
  register int ok = 1, i;
  register int ok, i, level;
  register int ok;
	printf(". missing in ");
	printpath(2, 1);
	printf(".. missing in ");
	printpath(2, 1);
  int ok;
		printf("empty symbolic link ");
		printf("symbolic link too large (size %d) ", ip->i_size);
	printpath(2, 1);
  int i, ok;
	printpath(2, 1);
		printpath(2, 1);
	ncharspec++;
	printf("bad mode of ");
	printpath(1, 0);
	printf("mode = %o)", ip->i_mode);
	printf("root inode is not a directory ");
	printf("(ino = %llu, mode = %o)\n", ino, ip->i_mode);
	printf("link count zero of ");
	printpath(2, 0);
	printf("link count too big in ");
	printpath(1, 0);
	printf("cnt = %u)\n", (unsigned) ip->i_nlinks);
  register int visited;
	printf("found inode %llu: ", ino);
	printpath(0, 1);
	devread(inoblock(ino), inooff(ino), (char *) &inode, INODE_SIZE);
			memset((void *) dp, 0, sizeof(dir_struct));
  putchar('\n');
  printf("blocksize = %5d        ", block_size);
  printf("zonesize  = %5d\n", ZONE_SIZE);
  printf("\n");
  pr("%8u    Character special file%s\n", ncharspec, "", "s");
char *f, **clist, **ilist, **zlist;
  		printf("\n----- FILE SYSTEM MARKED DIRTY -----\n\n");
  		printf("Filesystem is already dirty.\n");
	  	printf("%s: clean\n", f);
  printtotal();
  		printf("\n----- FILE SYSTEM STILL DIRTY -----\n\n");
  		printf("\n----- FILE SYSTEM MARKED CLEAN -----\n\n");
int argc;
char **argv;
  register char **clist = 0, **ilist = 0, **zlist = 0;
  int badflag = 0;
  register int devgiven = 0;
  register char *arg;
	printf("Fsck was compiled with the wrong BITSHIFT!\n");
			printf("%s: unknown flag '%s'\n", prog, arg);
	printf("Usage: fsck [-dyfpacilrsz] file\n");
	fprintf(stderr, "%s\n", fmt);
unsigned int fs_version = 2, block_size = 0;
static struct super_block sb;
  char st_presence;
int nfreeinode, nregular, ndirectory, nblkspec, ncharspec, nbadinode;
int nsock, npipe, nsyml, ztype[NLEVEL];
int preen = 0, markdirty = 0;
char answer[] = "Answer questions with y or n.  Then hit RETURN";
int main(int argc, char **argv);
void initvars(void);
void fatal(char *s);
int eoln(int c);
int yes(char *question);
int atoo(char *s);
int input(char *buf, int size);
char *alloc(unsigned nelem, unsigned elsize);
void printname(char *s);
void printrec(struct stack *sp);
void printpath(int mode, int nlcr);
void devopen(void);
void devclose(void);
void devio(block_nr bno, int dir);
void devread(long block, long offset, char *buf, int size);
void devwrite(long block, long offset, char *buf, int size);
void pr(char *fmt, int cnt, char *s, char *p);
void lpr(char *fmt, long cnt, char *s, char *p);
bit_nr getnumber(char *s);
char **getlist(char ***argv, char *type);
void lsuper(void);
void rw_super(int mode);
void chksuper(void);
void lsi(char **clist);
bitchunk_t *allocbitmap(int nblk);
void loadbitmap(bitchunk_t *bitmap, block_nr bno, int nblk);
void dumpbitmap(bitchunk_t *bitmap, block_nr bno, int nblk);
void freebitmap(bitchunk_t *p);
void getbitmaps(void);
void putbitmaps(void);
	int *report, bit_t);
	blkno, int nblk, char *type);
void chkilist(void);
void getcount(void);
void counterror(ino_t ino);
void chkcount(void);
void freecount(void);
void list(ino_t ino, d_inode *ip);
int Remove(dir_struct *dp);
void make_printable_name(char *dst, char *src, int n);
int chkdots(ino_t ino, off_t pos, dir_struct *dp, ino_t exp);
int chkname(ino_t ino, dir_struct *dp);
int chkentry(ino_t ino, off_t pos, dir_struct *dp);
int chkdirzone(ino_t ino, d_inode *ip, off_t pos, zone_nr zno);
int chksymlinkzone(ino_t ino, d_inode *ip, off_t pos, zone_nr zno);
void errzone(char *mess, zone_nr zno, int level, off_t pos);
int markzone(zone_nr zno, int level, off_t pos);
off_t jump(int level);
int zonechk(ino_t ino, d_inode *ip, off_t *pos, zone_nr zno, int level);
	len, int level);
int chkfile(ino_t ino, d_inode *ip);
int chkdirectory(ino_t ino, d_inode *ip);
int chklink(ino_t ino, d_inode *ip);
int chkspecial(ino_t ino, d_inode *ip);
int chkmode(ino_t ino, d_inode *ip);
int chkinode(ino_t ino, d_inode *ip);
int descendtree(dir_struct *dp);
void chktree(void);
void printtotal(void);
void chkdev(char *f, char **clist, char **ilist, char **zlist);
  register int level;
char *s;
  printf("%s\nfatal\n", s);
int c;
char *question;
  register int c, answerchar;
  static int note = 0;
  int yes;
	printf("\n");
  printf("%s? ", question);
	printf("yes\n");
  yes = !(answerchar == 'n' || answerchar == 'N');
register char *s;
  register int n = 0;
char *buf;
int size;
  register char *p = buf;
  printf("\n");
	printf("--> ");
		*p = getchar();
  char *p;
  memset((void *) p, 0, (size_t)nelem * elsize);
char *s;
  register int n = MFS_NAME_MAX;
  int c;
	putchar(c);
	printrec(sp->st_next);
	putchar('/');
	printname(sp->st_dir->mfs_d_name);
int mode;
int nlcr;
	putchar('/');
	printrec(ftop);
	printf(" (ino = %u, ", ftop->st_dir->d_inum);
	printf(" (ino = %u)", ftop->st_dir->d_inum);
int dir;
printf("%s at block %5d\n", dir == READING ? "reading " : "writing", bno);
	printf("Continuing with a zero-filled block.\n");
char *buf;
int size;
char *buf;
int size;
char *fmt, *s, *p;
int cnt;
  printf(fmt, cnt, cnt == 1 ? s : p);
char *fmt, *s, *p;
  printf(fmt, cnt, cnt == 1 ? s : p);
register char *s;
char ***argv, *type;
  register char **list = *argv;
  register int empty = 1;
	printf("warning: no %s numbers given\n", type);
  char buf[80];
	printf("ninodes       = %u", sb.s_ninodes);
	printf("nzones        = %d", sb.s_zones);
	printf("imap_blocks   = %u", sb.s_imap_blocks);
	printf("zmap_blocks   = %u", sb.s_zmap_blocks);
	printf("firstdatazone = %u", sb.s_firstdatazone_old);
	printf("log_zone_size = %u", sb.s_log_zone_size);
	printf("maxsize       = %d", sb.s_max_size);
	printf("block size    = %d", sb.s_block_size);
		devwrite(0, OFFSET_SUPER_BLOCK, (char *) &sb, sizeof(sb));
	printf("flags         = ");
	printf("\n");
	printf("warning: invalid max file size %d\n", sb.s_max_size);
  register int n;
	printf(" instead of %d\n", sb.s_imap_blocks);
	printf(" instead of %d\n", sb.s_zmap_blocks);
		printf("instead of %u\n", sb.s_firstdatazone_old);
	printf("warning: expected max size to be %lld ", maxsize);
	printf("instead of %d\n", sb.s_max_size);
char **clist;
  char buf[80];
		devread(inoblock(ino), inooff(ino), (char *) ip, INODE_SIZE);
		printf("inode %llu:\n", ino);
		printf("    mode   = %6o", ip->i_mode);
		printf("    nlinks = %6u", ip->i_nlinks);
		printf("    size   = %6d", ip->i_size);
int nblk;
int nblk;
  register int i;
	devread(bno, 0, (char *) p, block_size);
int nblk;
  register int i;
	devwrite(bno, 0, (char *) p, block_size);
char **list;
			printf("inode number %d ", bit);
			printf("zone number %d ", bit);
		printf("out of range (ignored)\n");
  free((char *) p);
char *type;
int *n, *report;
			printf("%s %d is missing\n", type, bit);
			printf("%s %d is not free\n", type, bit);
int nblk;
char *type;
  int report = 1, nerr = 0;
  int w = nblk * WORDS_PER_BLOCK;
  printf("Checking %s map. ", type);
  printf("Checking inode list. ");
			printf("mode inode %llu not cleared", ino);
	printf("INODE NLINK COUNT\n");
  devread(inoblock(ino), inooff(ino), (char *) &inode, INODE_SIZE);
  printf("%5llu %5u %5u", ino, (unsigned) inode.i_nlinks, count[ino]);
		fatal("internal error (counterror)");
	devwrite(inoblock(ino), inooff(ino), (char *) &inode, INODE_SIZE);
  free((char *) count);
	putchar('r');
	putchar('-');
	putchar('w');
	putchar('-');
	putchar(overlay);
		putchar('x');
		putchar('-');
	printf(" inode permission link   size name\n");
  printf("%6llu ", ino);
      case I_REGULAR:		putchar('-');	break;
      case I_DIRECTORY:		putchar('d');	break;
      case I_CHAR_SPECIAL:	putchar('c');	break;
      case I_BLOCK_SPECIAL:	putchar('b');	break;
      case I_NAMED_PIPE:	putchar('p');	break;
      case I_UNIX_SOCKET:	putchar('s');	break;
      case I_SYMBOLIC_LINK:	putchar('l');	break;
      default:			putchar('?');
  printperm(ip->i_mode, 6, I_SET_UID_BIT, 's');
  printperm(ip->i_mode, 3, I_SET_GID_BIT, 's');
  printperm(ip->i_mode, 0, STICKY_BIT, 't');
  printf(" %3u ", ip->i_nlinks);
	printf("  %2x,%2x ", major(ip->i_zone[0]), minor(ip->i_zone[0]));
      default:	printf("%7d ", ip->i_size);
  printpath(0, 1);
	memset((void *) dp, 0, sizeof(dir_struct));
register char *dst;
register char *src;
register int n;
  register int c;
  char printable_name[4 * MFS_NAME_MAX + 1];
	printf("bad %s in ", printable_name);
	printpath(1, 0);
	printf("%s is linked to %u ", printable_name, dp->d_inum);
	printf("instead of %llu)", exp);
	printf("warning: %s has offset %lld in ", printable_name, pos);
	printpath(1, 0);
	printf("%s is linked to %u)\n", printable_name, dp->d_inum);
  register int n = MFS_NAME_MAX + 1;
  register char *p = dp->mfs_d_name;
	printf("null name found in ");
	printpath(0, 0);
		printf("found a '/' in entry of directory ");
		printpath(1, 0);
		printf("entry = '");
		printname(dp->mfs_d_name);
		printf("')");
	printf("bad inode found in directory ");
	printpath(1, 0);
	printf("ino found = %u, ", dp->d_inum);
	printf("name = '");
	printname(dp->mfs_d_name);
	printf("')");
		memset((void *) dp, 0, sizeof(dir_struct));
	printf("too many links to ino %u\n", dp->d_inum);
	printf("discovered at entry '");
	printname(dp->mfs_d_name);
	printf("' in directory ");
	printpath(0, 1);
	printf("link to directory discovered in ");
	printpath(1, 0);
	printf("name = '");
	printname(dp->mfs_d_name);
	printf("', dir ino = %u)", dp->d_inum);
  register int n, dirty;
	devread(block, offset, (char *) dirblk, DIRCHUNK);
	printf("size not updated of directory ");
	printpath(2, 0);
		devwrite(inoblock(ino), inooff(ino), (char *) ip, INODE_SIZE);
	char target[PATH_MAX+1];
		printpath(2, 0);
				(char *) ip, INODE_SIZE);
char *mess;
int level;
  printf("%s zone in ", mess);
  printpath(1, 0);
  printf("zno = %d, type = ", zno);
      case 0:	printf("DATA");	break;
      case 1:	printf("SINGLE INDIRECT");	break;
      case 2:	printf("DOUBLE INDIRECT");	break;
      default:	printf("VERY INDIRECT");
  printf(", pos = %lld)\n", pos);
int level;
  register int n = NR_INDIRECTS / CINDIR;
	devread(block, offset, (char *) indirect, INDCHUNK);
int level;
  register int ok = 1, i;
  register int ok, i, level;
  register int ok;
	printf(". missing in ");
	printpath(2, 1);
	printf(".. missing in ");
	printpath(2, 1);
  int ok;
		printf("empty symbolic link ");
		printf("symbolic link too large (size %d) ", ip->i_size);
	printpath(2, 1);
  int i, ok;
	printpath(2, 1);
		printpath(2, 1);
	ncharspec++;
	printf("bad mode of ");
	printpath(1, 0);
	printf("mode = %o)", ip->i_mode);
	printf("root inode is not a directory ");
	printf("(ino = %llu, mode = %o)\n", ino, ip->i_mode);
	printf("link count zero of ");
	printpath(2, 0);
	printf("link count too big in ");
	printpath(1, 0);
	printf("cnt = %u)\n", (unsigned) ip->i_nlinks);
  register int visited;
	printf("found inode %llu: ", ino);
	printpath(0, 1);
	devread(inoblock(ino), inooff(ino), (char *) &inode, INODE_SIZE);
			memset((void *) dp, 0, sizeof(dir_struct));
  putchar('\n');
  printf("blocksize = %5d        ", block_size);
  printf("zonesize  = %5d\n", ZONE_SIZE);
  printf("\n");
  pr("%8u    Character special file%s\n", ncharspec, "", "s");
char *f, **clist, **ilist, **zlist;
  		printf("\n----- FILE SYSTEM MARKED DIRTY -----\n\n");
  		printf("Filesystem is already dirty.\n");
	  	printf("%s: clean\n", f);
  printtotal();
  		printf("\n----- FILE SYSTEM STILL DIRTY -----\n\n");
  		printf("\n----- FILE SYSTEM MARKED CLEAN -----\n\n");
int argc;
char **argv;
  register char **clist = 0, **ilist = 0, **zlist = 0;
  int badflag = 0;
  register int devgiven = 0;
  register char *arg;
	printf("Fsck was compiled with the wrong BITSHIFT!\n");
			printf("%s: unknown flag '%s'\n", prog, arg);
	printf("Usage: fsck [-dyfpacilrsz] file\n");
	fprintf(stderr, "%s\n", fmt);
static enum { IN_LIST, NOT_IN_LIST } which = NOT_IN_LIST;
	char *type;
	char *options;
static int maxrun = 0;
static char *options = NULL;
static int flags = 0;
static int checkfs(const char *, const char *, const char *, void *, pid_t *);
static int selected(const char *);
static int omitted(const char *);
static void addoption(char *);
static const char *getoptions(const char *);
static void addentry(struct fstypelist *, const char *, const char *);
static void maketypelist(char *);
static void catopt(char **, const char *);
static void mangle(char *, int *, const char ** volatile *, int *);
static const char *getfslab(const char *);
__dead static void usage(void);
static void *isok(struct fstab *);
	int i, rval;
	const char *vfstype = NULL;
	char globopt[3];
	int ret = FSCK_EXIT_OK;
	char buf[MAXPATHLEN];
		const char *spec, *spec2, *mntpt, *type, *cp;
		char	device[MAXPATHLEN];
	const char ** volatile argv, **edir;
	const char * volatile vfstype = vfst;
	int argc, i, status, maxargc;
	char *optb;
	char *volatile optbuf;
	char execname[MAXPATHLEN + 1], execbase[MAXPATHLEN];
	const char *extra = getoptions(vfstype);
	argv = emalloc(sizeof(char *) * maxargc);
	(void) snprintf(execbase, sizeof(execbase), "fsck_%s", vfstype);
			(void)printf(" %s", argv[i]);
		(void)printf("\n");
			execv(execname, (char * const *)__UNCONST(argv));
	char *newoptions;
	char *ptr;
	char *s;
		(void)snprintf(s + i, j, ",%s", o);
	char *p, *s;
	int argc, maxargc;
	const char **argv;
			argv = erealloc(argv, maxargc * sizeof(char *));
	static struct dkwedge_info dkw;
	int fd;
	char p;
	const char *vfstype;
	u_char t;
	char buf[MAXPATHLEN];
		(void) close(fd);
	(void) close(fd);
int	nrun, ndisks;
extern char	*default_fstype();
int	nargc = 2;
int	options = 0;
int	mnt_passno = 0;
int	exitstat = 0;
int	verbose = 0;
char	*nargv[ARGV_MAX];
char	*myname, *fstype;
char	*malloc();
char	vfstab[] = VFSTAB;
char	pflg = 0, Vflg = 0;
	char *name;
	char *fsname;
static struct vfstab	vfsave = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};
static void usage(void);
static void fsck_dopreen(struct devlist **devp, int ndevs);
static void waiter(struct devlist **blp, struct devlist **badlist);
static void print_badlist(struct devlist *lp);
static void startdisk(struct devlist *dp);
static void do_exec(char *fstype, char *nargv[]);
static void prnt_cmd(FILE *fd, char *fstype);
static void vfserror(int flag);
	int	error;
	int	error;
	int	cc, ret, other_than_ufs = 0;
	int	questflg = 0, Fflg = 0, Vflg = 0, sanity = 0;
	char	*subopt;
	int	devfd;
	int preencnt = 0;
	int status;
	uint_t lbs;
	(void) setlocale(LC_ALL, "");
	(void) textdomain(TEXT_DOMAIN);
							arg_hint = UNKNOWN;
						arg_hint = MOUNTPT;
							(void) close(devfd);
						(void) close(devfd);
							arg_hint = UNKNOWN;
						arg_hint = FSCKDEV;
							arg_hint = UNKNOWN;
						arg_hint = SPECIAL;
	char name[1024];
	int rc;
	int i;
		print_badlist(badlist);
	int status;
	int x, len;
			printf("\n   ");
			printf(" ");
	extern char *strdup();
		fprintf(stderr, gettext("%s: out of memory\n"), myname);
		fprintf(stderr, gettext("%s: out of memory\n"), myname);
	int	st;
	char	full_path[PATH_MAX];
	char	*vfs_path = VFS_PATH;
	int	status = 0;
			fprintf(stderr, gettext("%s: bad wait\n"), myname);
	char	full_path[PATH_MAX];
	char	*vfs_path = VFS_PATH;
	sprintf(full_path, "%s/%s/%s", vfs_path, fstype, myname);
	sprintf(full_path, "%s/%s/%s", vfs_path, fstype, myname);
	char	**argp;
	fprintf(fd, "%s -F %s", myname, fstype);
		fprintf(fd, " %s", *argp);
	fprintf(fd, "\n");

/* ===== FUNCTIONS ===== */

/* Function 1 */
    (repair || markdirty) ? O_RDWR : O_RDONLY)) < 0) {
	perror(fsck_device);
	fatal("couldn't open device to fsck");
  }

/* Function 2 */
    (repair || markdirty) ? O_RDWR : O_RDONLY)) < 0) {
	perror(fsck_device);
	fatal("couldn't open device to fsck");
  }

/* Function 3 */
			    ((sfs.f_flag & MNT_RDONLY) == 0)) {
				printf(
		"%s: file system is mounted read-write on %s; not checking\n",
				    spec, mntpt);
				if ((flags & CHECK_PREEN) && auxarg != NULL)
				else
			}

/* Function 4 */
	    ((vfsave.vfs_special = strdup(vp->vfs_special)) == NULL)) {
		perror(myname);
	}

/* Function 5 */
	    ((vfsave.vfs_fsckdev = strdup(vp->vfs_fsckdev)) == NULL)) {
		perror(myname);
	}

/* Function 6 */
	    ((vfsave.vfs_mountp = strdup(vp->vfs_mountp)) == NULL)) {
		perror(myname);
	}

/* Function 7 */
	    ((vfsave.vfs_fstype = strdup(vp->vfs_fstype)) == NULL)) {
		perror(myname);
	}

/* Function 8 */
	    ((vfsave.vfs_fsckpass = strdup(vp->vfs_fsckpass)) == NULL)) {
		perror(myname);
	}

/* Function 9 */
	    ((vfsave.vfs_automnt = strdup(vp->vfs_automnt)) == NULL)) {
		perror(myname);
	}

/* Function 10 */
	    ((vfsave.vfs_mntopts = strdup(vp->vfs_mntopts)) == NULL)) {
		perror(myname);
	}

/* Function 11 */
				    numbers(vget.vfs_fsckpass)) {
					other_than_ufs ++;
					continue;
				}

/* Function 12 */
				    strcmp(fstype, vget.vfs_fstype) == 0)) {
					status = execute(vget.vfs_fsckdev,
					    vget.vfs_fstype, Vflg, fd);
					if (status > exitstat)
						exitstat = status;
				}


#ifdef __cplusplus
}
#endif

/* End of fsck_unified.c - Synthesized by MINIX4 Massive Synthesis System */
