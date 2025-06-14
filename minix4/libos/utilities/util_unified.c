/**
 * @file util_unified.c
 * @brief Unified util implementation
 * @details This file is a synthesized/unified implementation combining multiple
 *          source files from the legacy MINIX codebase into a single, modern,
 *          C23-compliant implementation for MINIX4.
 * 
 * @version 4.0.0
 * @date 2025-06-13
 * @author MINIX4 Synthesis System
 * 
 * @copyright Copyright (c) 2024 MINIX4 Project
 * @license MIT License
 * 
 * SYNTHESIS INFORMATION:
 * ======================
 * This file synthesizes the following source files:
 *    1. tar\util.c
 *    2. userspace\usr_bin_legacy\grep\util.c
 *    3. userspace\minix_commands\minix-service\util.c
 *    4. userspace\commands_legacy\system\minix-service\util.c
 *    5. minix4\microkernel\servers\api\socket\util.c
 *    6. minix4\libos_legacy\ifconfig\util.c
 *    7. minix4\libos_legacy\ls\util.c
 *    8. minix4\libos_legacy\rcp\util.c
 *    9. minix4\libos\lib_legacy\util\compose\util.c
 *   10. minix4\libos\lib_legacy\libzpool\common\util.c
 *   11. minix4\libos\lib_legacy\libsqlite\src\util.c
 *   12. minix4\libos\lib_legacy\libresolv2\common\irs\util.c
 *   13. minix4\libos\lib_legacy\libmp\common\util.c
 *   14. minix4\libos\lib_legacy\libdemangle\common\util.c
 *   15. minix4\libos\lib_legacy\libcryptoutil\common\util.c
 *   16. minix4\libos\lib_legacy\fm\topo\modules\common\pcibus\util.c
 *   17. minix4\exokernel\minix_drivers\storage\filter\util.c
 *   18. minix4\exokernel\kernel_legacy\cmd\cdrw\util.c
 *   19. minix4\exokernel\kernel_legacy\cmd\genmsg\util.c
 *   20. minix4\exokernel\kernel_legacy\cmd\latencytop\util.c
 *   21. minix4\exokernel\kernel_legacy\cmd\mailx\util.c
 *   22. minix4\exokernel\kernel_legacy\cmd\msgfmt\util.c
 *   23. minix4\exokernel\kernel_legacy\cmd\saf\util.c
 *   24. minix4\exokernel\kernel_legacy\cmd\ypcmd\revnetgroup\util.c
 *   25. minix4\exokernel\kernel_legacy\cmd\sgs\rtld\common\util.c
 *   26. minix4\exokernel\kernel_legacy\cmd\sgs\librtld\common\util.c
 *   27. minix4\exokernel\kernel_legacy\cmd\sgs\liblddbg\common\util.c
 *   28. minix4\exokernel\kernel_legacy\cmd\sgs\libld\common\util.c
 *   29. minix4\exokernel\kernel_legacy\cmd\sgs\libcrle\common\util.c
 *   30. minix4\exokernel\kernel_legacy\cmd\sgs\elfedit\common\util.c
 *   31. minix4\exokernel\kernel_legacy\cmd\sgs\crle\common\util.c
 *   32. minix4\exokernel\kernel_legacy\cmd\sendmail\libsm\util.c
 *   33. minix4\exokernel\kernel_legacy\cmd\sendmail\src\util.c
 *   34. minix4\exokernel\kernel_legacy\cmd\powertop\common\util.c
 *   35. minix4\exokernel\kernel_legacy\cmd\krb5\kadmin\dbutil\util.c
 *   36. minix4\exokernel\kernel_legacy\cmd\hal\hald\util.c
 *   37. minix4\exokernel\kernel_legacy\cmd\fm\modules\common\disk-monitor\util.c
 *   38. minix4\exokernel\kernel_legacy\cmd\cmd-inet\usr.lib\inetd\util.c
 *   39. minix4\exokernel\kernel_legacy\cmd\cmd-inet\sbin\dhcpagent\util.c
 *   40. minix4\exokernel\kernel_legacy\cmd\cmd-inet\lib\nwamd\util.c
 *   41. minix4\exokernel\kernel_legacy\cmd\cmd-crypto\kmfcfg\util.c
 *   42. minix4\exokernel\kernel_legacy\cmd\abi\spectrans\spec2map\util.c
 *   43. minix4\exokernel\kernel_legacy\cmd\abi\spectrans\spec2trace\util.c
 *   44. minix4\exokernel\kernel_legacy\boot\common\util.c
 * 
 * Total source files: 44
 * Synthesis date: 2025-06-13 19:49:03
 * Synthesis strategy: Modern C23 unification with legacy compatibility
 * 
 * MODERNIZATION FEATURES:
 * =======================
 * - C23 standard compliance
 * - Modern error handling patterns
 * - Consistent coding style
 * - Improved memory safety
 * - Enhanced documentation
 * - POSIX compliance where applicable
 */

#ifndef UTIL_UNIFIED_C_H
#define UTIL_UNIFIED_C_H

// C23 Standard Headers
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <threads.h>

// POSIX Headers
#include <unistd.h>
#include <errno.h>
#include <string.h>

// MINIX4 Headers
#include "minix4_types.h"
#include "minix4_config.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ===== INCLUDES ===== */
#include	"_audit.h"
#include	"_debug.h"
#include	"_elf.h"
#include	"_rtld.h"
#include	"libld.h"
#include	"msg.h"
#include	<_elfedit.h>
#include	<conv.h>
#include	<ctype.h>
#include	<debug.h>
#include	<dlfcn.h>
#include	<elfedit.h>
#include	<fcntl.h>
#include	<libelf.h>
#include	<libintl.h>
#include	<limits.h>
#include	<link.h>
#include	<msg.h>
#include	<stdarg.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<strings.h>
#include	<sys/auxv.h>
#include	<sys/debug.h>
#include	<sys/elf_SPARC.h>
#include	<sys/elf_amd64.h>
#include	<sys/lwp.h>
#include	<sys/machelf.h>
#include	<sys/mman.h>
#include	<sys/systeminfo.h>
#include	<sys/time.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<upanic.h>
#include "./kdb5_edit.h"
#include "_crle.h"
#include "_libld.h"
#include "agent.h"
#include "bsdtar.h"
#include "bsdtar_platform.h"
#include "bstream.h"
#include "common.h"
#include "cons.h"
#include "defaults.h"
#include "demangle-sys.h"
#include "demangle_int.h"
#include "device_info.h"
#include "disk_monitor.h"
#include "env.h"
#include "err.h"
#include "errlog.h"
#include "extern.h"
#include "genmsg.h"
#include "grep.h"
#include "hald.h"
#include "hald_dbus.h"
#include "hald_runner.h"
#include "inc.h"
#include "inetd_impl.h"
#include "interface.h"
#include "irs_p.h"
#include "latencytop.h"
#include "libmp.h"
#include "llp.h"
#include "logger.h"
#include "ls.h"
#include "main.h"
#include "misc.h"
#include "misc_scsi.h"
#include "mmc.h"
#include "msg.h"
#include "msgs.h"
#include "osspec.h"
#include "packet.h"
#include "parser.h"
#include "port_after.h"
#include "port_before.h"
#include "powertop.h"
#include "prog_ops.h"
#include "rcv.h"
#include "rtc.h"
#include "sgs.h"
#include "sqliteInt.h"
#include "states.h"
#include "structs.h"
#include "trace.h"
#include "trackio.h"
#include "transport.h"
#include "util.h"
#include "xk_mach.h"
#include "xlator.h"
#include <alloca.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <assert.h>
#include <auth_attr.h>
#include <auth_list.h>
#include <cryptoutil.h>
#include <ctype.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>
#include <debug.h>
#include <dhcp_hostconf.h>
#include <dhcp_inittab.h>
#include <dhcp_symbol.h>
#include <dhcpmsg.h>
#include <dlfcn.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <fm/fmd_api.h>
#include <fm/libtopo.h>
#include <fm/topo_hc.h>
#include <fm/topo_mod.h>
#include <fts.h>
#include <glib.h>
#include <ifaddrs.h>
#include <inttypes.h>
#include <io.h>
#include <irs.h>
#include <k5-int.h>
#include <kmfapiP.h>
#include <kstat.h>
#include <libdllink.h>
#include <libgen.h>
#include <libintl.h>
#include <libipadm.h>
#include <libscf.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libzutil.h>
#include <limits.h>
#include <locale.h>
#include <mach/mach_host.h>
#include <mp.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/if_dl.h>
#include <net/route.h>
#include <netdb.h>
#include <netinet/dhcp.h>
#include <netinet/in.h>
#include <netinet/in.h>		/* XXX */
#include <netinet/in.h>		/* struct in_addr */
#include <paths.h>
#include <procfs.h>
#include <pthread.h>
#include <pwd.h>
#include <regex.h>
#include <resolv.h>
#include <sac.h>
#include <secdb.h>
#include <sendmail.h>
#include <sgs.h>
#include <signal.h>
#include <sm/assert.h>
#include <sm/conf.h>
#include <sm/gen.h>
#include <sm/heap.h>
#include <sm/sendmail.h>
#include <sm/setjmp.h>
#include <sm/string.h>
#include <sm/xtrap.h>
#include <smbios.h>
#include <spawn.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stropts.h>
#include <sys/avl.h>
#include <sys/cdefs.h>
#include <sys/crypto/common.h>
#include <sys/file.h>
#include <sys/fm/protocol.h>
#include <sys/fs/zfs.h>
#include <sys/hexdump.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/refcount.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/spa.h>
#include <sys/stat.h>
#include <sys/systeminfo.h>
#include <sys/types.h>
#include <sys/types.h>  /* Linux doesn't define mode_t, etc. in sys/stat.h. */
#include <sys/utsname.h>
#include <sys/wait.h>
#include <sys/zfs_context.h>
#include <sys/zfs_ioctl.h>
#include <sysexits.h>
#include <syslog.h>
#include <time.h>
#include <tzfile.h>
#include <unistd.h>
#include <util.h>
#include <vis.h>
#include <wait.h>
#include <wchar.h>
#include <wctype.h>
#include <zlib.h>
#include <zone.h>

/* ===== DEFINES ===== */
#define iswprint isprint
#define read _read
#define isword(x) (isalnum((unsigned char)x) || (x) == '_')
#define wmatch(d, l, s, e)	\
#define	tohex(x)	(isdigit(x) ? (x) - '0' : tolower(x) - 'a' + 10)
#define N_GUARD 1
#define _64e3  (64.0 * 64.0 * 64.0)
#define _64e4  (64.0 * 64.0 * 64.0 * 64.0)
#define _64e15 (_64e3 * _64e4 * _64e4 * _64e4)
#define _64e16 (_64e4 * _64e4 * _64e4 * _64e4)
#define _64e63 (_64e15 * _64e16 * _64e16 * _64e16)
#define _64e64 (_64e16 * _64e16 * _64e16 * _64e16)
#define sqliteNextChar(X)  while( (0xc0&*++(X))==0x80 ){}
#define sqliteCharVal(X)   sqlite_utf8_to_int(X)
#define sqliteNextChar(X)  (++(X));
#define sqliteCharVal(X)   ((int)*(X))
#define	BASEBITS	(8 * (unsigned int)sizeof (short) - 1)
#define	SET_TOKEN	"$set"
#define	DELSET_TOKEN	"$delset"
#define	QUOTE_TOKEN	"$quote"
#define	SkipSpace(s)	while (*(s) == ' ' || *(s) == '\t') s++
#define	ENV_TYP_IGNORE		0x01		/* ignore - variable is for */
#define	ENV_TYP_ISA		0x02		/* variable is ISA specific */
#define	ENV_TYP_CONFIG		0x04		/* variable obtained from a */
#define	ENV_TYP_PERMANT		0x08		/* variable is permanent */
#define	ENV_TYP_CMDLINE		0x10		/* variable provide with -e */
#define	ENV_TYP_NULL		0x20		/* variable is null */
#define	ENV_FLG_AUDIT		0x0000000000001ULL
#define	ENV_FLG_AUDIT_ARGS	0x0000000000002ULL
#define	ENV_FLG_BIND_NOW	0x0000000000004ULL
#define	ENV_FLG_BIND_NOT	0x0000000000008ULL
#define	ENV_FLG_BINDINGS	0x0000000000010ULL
#define	ENV_FLG_CONFGEN		0x0000000000020ULL
#define	ENV_FLG_CONFIG		0x0000000000040ULL
#define	ENV_FLG_DEBUG		0x0000000000080ULL
#define	ENV_FLG_DEBUG_OUTPUT	0x0000000000100ULL
#define	ENV_FLG_DEMANGLE	0x0000000000200ULL
#define	ENV_FLG_FLAGS		0x0000000000400ULL
#define	ENV_FLG_INIT		0x0000000000800ULL
#define	ENV_FLG_LIBPATH		0x0000000001000ULL
#define	ENV_FLG_LOADAVAIL	0x0000000002000ULL
#define	ENV_FLG_LOADFLTR	0x0000000004000ULL
#define	ENV_FLG_NOAUDIT		0x0000000008000ULL
#define	ENV_FLG_NOAUXFLTR	0x0000000010000ULL
#define	ENV_FLG_NOBAPLT		0x0000000020000ULL
#define	ENV_FLG_NOCONFIG	0x0000000040000ULL
#define	ENV_FLG_NODIRCONFIG	0x0000000080000ULL
#define	ENV_FLG_NODIRECT	0x0000000100000ULL
#define	ENV_FLG_NOENVCONFIG	0x0000000200000ULL
#define	ENV_FLG_NOLAZY		0x0000000400000ULL
#define	ENV_FLG_NOOBJALTER	0x0000000800000ULL
#define	ENV_FLG_NOVERSION	0x0000001000000ULL
#define	ENV_FLG_PRELOAD		0x0000002000000ULL
#define	ENV_FLG_PROFILE		0x0000004000000ULL
#define	ENV_FLG_PROFILE_OUTPUT	0x0000008000000ULL
#define	ENV_FLG_SIGNAL		0x0000010000000ULL
#define	ENV_FLG_TRACE_OBJS	0x0000020000000ULL
#define	ENV_FLG_TRACE_PTHS	0x0000040000000ULL
#define	ENV_FLG_UNREF		0x0000080000000ULL
#define	ENV_FLG_UNUSED		0x0000100000000ULL
#define	ENV_FLG_VERBOSE		0x0000200000000ULL
#define	ENV_FLG_WARN		0x0000400000000ULL
#define	ENV_FLG_NOFLTCONFIG	0x0000800000000ULL
#define	ENV_FLG_BIND_LAZY	0x0001000000000ULL
#define	ENV_FLG_NOUNRESWEAK	0x0002000000000ULL
#define	ENV_FLG_NOPAREXT	0x0004000000000ULL
#define	ENV_FLG_HWCAP		0x0008000000000ULL
#define	ENV_FLG_SFCAP		0x0010000000000ULL
#define	ENV_FLG_MACHCAP		0x0020000000000ULL
#define	ENV_FLG_PLATCAP		0x0040000000000ULL
#define	ENV_FLG_CAP_FILES	0x0080000000000ULL
#define	ENV_FLG_DEFERRED	0x0100000000000ULL
#define	ENV_FLG_NOENVIRON	0x0200000000000ULL
#define	SEL_REPLACE		0x0001
#define	SEL_PERMANT		0x0002
#define	SEL_ACT_RT		0x0100	/* setting rtld_flags */
#define	SEL_ACT_RT2		0x0200	/* setting rtld_flags2 */
#define	SEL_ACT_STR		0x0400	/* setting string value */
#define	SEL_ACT_LML		0x0800	/* setting lml_flags */
#define	SEL_ACT_LMLT		0x1000	/* setting lml_tflags */
#define	SEL_ACT_SPEC_1		0x2000	/* for FLG_{FLAGS, LIBPATH} */
#define	SEL_ACT_SPEC_2		0x4000	/* need special handling */
#define	LOC_LANG	1
#define	LOC_MESG	2
#define	LOC_ALL		3
#define	FLG_UT_MINUS	0x0001	/* - */
#define	FLG_UT_SHARP	0x0002	/* # */
#define	FLG_UT_DOTSEEN	0x0008	/* dot appeared in format spec */
#define	PUTC(c) \
#define	NUM_SIZE	22
#define SM_MM_QUOTE(ch) (((ch) & 0377) == METAQUOTE || (((ch) & 0340) == 0200))
#define PUTX(limit)							\
#define VALID_INITIAL_NAME_CHARACTER(c)          \
#define VALID_NAME_CHARACTER(c)                  \
#define	MSG_BUF_SIZE		1024
#define	POLLFDS_GROWTH_SIZE	16
#define	ETCNODENAME		"/etc/nodename"
#define	IFLINE_MAX	1024	/* maximum length of a hostname.<if> line */
#define	TABLE_INITIAL	50
#define	TABLE_INCREMENT	50

/* ===== TYPES ===== */
struct paddr_prefix *
	union {
		struct sockaddr sa;
		struct sockaddr_in sin;
		struct sockaddr_in6 sin6;
	struct paddr_prefix *pfx;
	struct ifreq ifr;
	struct ifaddrs *ifa, *ifap;
	struct if_laddrreq iflr;
	struct if_addrprefreq ifap;
	struct ifaddrs *ifa;
	struct stat stb;
	struct stat stb;
	struct group *grp;
	struct passwd *pw;
typedef struct _SetID *SetID;
typedef struct _MsgID *MsgID;
typedef struct _SetID SetIDRec;
struct _SetID {
typedef struct _MsgID MsgIDRec;
struct _MsgID {
	struct stat buf;
	struct stat buf;
	struct timeval p;
	struct tm tms;
	struct stat64 st;
	struct stat sbuf;
	struct stat64	lbuf;
	struct stat64	lbuf;
	struct {
	struct metamac *mp;
	struct stat st;
		struct stat stbuf;
	struct stat64 st;
	struct stat st;
	struct stat statbuf;
typedef struct procs	PROCS_T;
struct procs
typedef struct {
	struct q_node *qnp =
	struct q_node *nodep;
	struct q_node *nextnode;
typedef enum {
struct pollfd	*poll_fds = NULL;
struct pollfd *
	struct pollfd	*p;
	struct pollfd *p;
	struct pollfd *p = find_pollfd(fd);
	struct {
		struct rt_msghdr	rm_mh;
		struct sockaddr_in	rm_dst;
		struct sockaddr_in	rm_gw;
		struct sockaddr_in	rm_mask;
		struct sockaddr_dl	rm_ifp;
	struct sockaddr_in	sin;
	struct sockaddr_in6	sin6;
	struct __res_state	res_state;
typedef struct scf_resources {
typedef struct	av_opts_s {

/* ===== GLOBAL VARIABLES ===== */
__FBSDID("$FreeBSD: src/usr.bin/tar/util.c,v 1.23 2008/12/15 06:00:25 kientzle Exp $");
static size_t	bsdtar_expand_char(char *, size_t, char);
static const char *strip_components(const char *path, int elements);
	int fmtbuff_length;
	int length, n;
	va_list ap;
	const char *p;
	unsigned i;
	wchar_t wc;
	char try_wc;
	fmtbuff_heap = NULL;
	fmtbuff_length = sizeof(fmtbuff_stack);
	fmtbuff = fmtbuff_stack;
	va_start(ap, fmt);
	length = vsnprintf(fmtbuff, fmtbuff_length, fmt, ap);
	va_end(ap);
		fmtbuff_length = length+1;
		fmtbuff_heap = malloc(fmtbuff_length);
			fmtbuff = fmtbuff_heap;
			va_start(ap, fmt);
			length = vsnprintf(fmtbuff, fmtbuff_length, fmt, ap);
			va_end(ap);
			length = sizeof(fmtbuff_stack) - 1;
	p = fmtbuff;
	i = 0;
	try_wc = 1;
			length -= n;
					outbuff[i++] = *p++;
					    outbuff, i, *p++);
			i += (unsigned)bsdtar_expand_char(outbuff, i, *p++);
			try_wc = 0;
			outbuff[i] = '\0';
			fprintf(f, "%s", outbuff);
			i = 0;
	outbuff[i] = '\0';
	fprintf(f, "%s", outbuff);
		free(fmtbuff_heap);
	size_t i = offset;
		buff[i++] = c;
		buff[i++] = '\\';
		case '\a': buff[i++] = 'a'; break;
		case '\b': buff[i++] = 'b'; break;
		case '\f': buff[i++] = 'f'; break;
		case '\n': buff[i++] = 'n'; break;
		case '\r': buff[i++] = 'r'; break;
		case '\t': buff[i++] = 't'; break;
		case '\v': buff[i++] = 'v'; break;
		case '\\': buff[i++] = '\\'; break;
			sprintf(buff + i, "%03o", 0xFF & (int)c);
			i += 3;
	char buff[32];
	char *p;
	ssize_t l;
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, " (y/N)? ");
	fflush(stderr);
	l = read(2, buff, sizeof(buff) - 1);
	buff[l] = 0;
			continue;
		free(bsdtar->pending_chdir);
		bsdtar->pending_chdir = NULL;
		bsdtar->pending_chdir = strdup(newdir);
		char *old_pending = bsdtar->pending_chdir;
		size_t old_len = strlen(old_pending);
		bsdtar->pending_chdir = malloc(old_len + strlen(newdir) + 2);
			old_pending[old_len - 1] = '\0';
			    old_pending, newdir);
		free(old_pending);
		lafe_errc(1, errno, "No memory");
		    bsdtar->pending_chdir);
	free(bsdtar->pending_chdir);
	bsdtar->pending_chdir = NULL;
			elements--;
			break;
			++p;
			break;
	const char *name = archive_entry_pathname(entry);
	char *subst_name;
	int r;
	r = apply_substitution(bsdtar, name, &subst_name, 0);
		lafe_warnc(0, "Invalid substitution, skipping entry");
		archive_entry_copy_pathname(entry, subst_name);
			free(subst_name);
			free(subst_name);
		name = archive_entry_pathname(entry);
		r = apply_substitution(bsdtar, archive_entry_hardlink(entry), &subst_name, 1);
			lafe_warnc(0, "Invalid substitution, skipping entry");
			archive_entry_copy_hardlink(entry, subst_name);
			free(subst_name);
		r = apply_substitution(bsdtar, archive_entry_symlink(entry), &subst_name, 1);
			lafe_warnc(0, "Invalid substitution, skipping entry");
			archive_entry_copy_symlink(entry, subst_name);
			free(subst_name);
		const char *linkname = archive_entry_hardlink(entry);
		name = strip_components(name, bsdtar->strip_components);
			    bsdtar->strip_components);
			archive_entry_copy_hardlink(entry, linkname);
		const char *rp, *p = name;
		int slashonly = 1;
				p += 8;
				p += 4;
			slashonly = 0;
			rp = p;
				p += 2;
				slashonly = 0;
				    name[0]);
				    "member names");
			bsdtar->warned_lead_slash = 1;
			name = ".";
			name = p;
			name++;
		char *q = strdup(name);
		archive_entry_copy_pathname(entry, q);
		free(q);
	static char buff[24];
	int64_t n = n0 < 0 ? -n0 : n0;
	char *p = buff + sizeof(buff);
	*--p = '\0';
		*--p = '0' + (int)(n % 10);
		n /= 10;
		*--p = '-';
		a += 2;
		b += 2;
		a++;
		b++;
static int	linesqueued;
static int	procline(str_t *l, int);
	regmatch_t *pmatch);
static int	grep_cmp(const unsigned char *, const unsigned char *, size_t);
static void	grep_revstr(unsigned char *, int);
	FTS	*fts;
	FTSENT	*p;
	int	c, fts_flags;
	c = 0;
	fts_flags = FTS_PHYSICAL | FTS_NOSTAT | FTS_NOCHDIR;
		err(2, NULL);
			break;
			file_err = 1;
				errno = p->fts_errno;
				warn("%s", p->fts_path);
			break;
			break;
			c += procfile(p->fts_path);
			break;
		err(2, "fts_read");
	str_t ln;
	file_t *f;
	int c, t, z, nottext;
		fn = "(standard input)";
		f = grep_fdopen(STDIN_FILENO, "r");
		f = grep_open(fn, "r");
		file_err = 1;
			warn("%s", fn);
	nottext = grep_bin_file(f);
		grep_close(f);
	ln.file = fn;
	ln.line_no = 0;
	ln.len = 0;
	linesqueued = 0;
	tail = 0;
	ln.off = -1;
		initqueue();
		ln.off += ln.len + 1;
			break;
			--ln.len;
		ln.line_no++;
		z = tail;
			enqueue(&ln);
			linesqueued++;
		c += t;
		clearqueue();
	grep_close(f);
			printf("%s:", ln.file);
		printf("%u\n", c);
		printf("%s\n", fn);
		printf("%s\n", fn);
		printf("Binary file %s matches\n", fn);
	regmatch_t	pmatch;
	int		c, i, r;
	regoff_t	offset;
		errx(2, "Line is too big to process");
	c = 0;
	i = 0;
		c = 1;
		goto print;
		offset = 0;
			    &pmatch);
			pmatch.rm_so += offset;
			pmatch.rm_eo += offset;
			pmatch.rm_so = offset;
			pmatch.rm_eo = l->len;
			r = regexec(&r_pattern[i], l->dat, 1, &pmatch, eflags);
				r = REG_NOMATCH;
			c = 1;
				goto print;
			break;
		c = !c;
				printf("--\n");
			first = 1;
			tail = Aflag;
				printqueue();
			linesqueued = 0;
			printline(l, ':', oflag ? &pmatch : NULL);
			printline(l, '-', oflag ? &pmatch : NULL);
			tail--;
		offset = pmatch.rm_eo;
		goto redo;
	int i;
	fg->patternLen = strlen((const char *)pat);
	fg->bol = 0;
	fg->eol = 0;
	fg->wmatch = wflag;
	fg->reversedSearch = 0;
		fg->pattern = grep_malloc(fg->patternLen + 1);
			fg->pattern[i] = toupper(pat[i]);
		fg->pattern[fg->patternLen] = '\0';
		fg->qsBc[i] = fg->patternLen;
		fg->qsBc[fg->pattern[i]] = fg->patternLen - i;
			fg->qsBc[tolower(fg->pattern[i])] = fg->patternLen - i;
	int i;
	int bol = 0;
	int eol = 0;
	int hasDot = 0;
	int firstHalfDot = -1;
	int firstLastHalfDot = -1;
	int lastHalfDot = 0;
	fg->patternLen = strlen(pat);
	fg->bol = 0;
	fg->eol = 0;
	fg->wmatch = 0;
	fg->reversedSearch = 0;
		eol++;
		fg->eol = 1;
		fg->patternLen--;
		bol++;
		fg->bol = 1;
		fg->patternLen--;
		int extra = Eflag ? 1 : 2;
		fg->patternLen -= 14 + 2 * extra;
		fg->wmatch = 7 + extra;
	fg->pattern = grep_malloc(fg->patternLen + 1);
	memcpy(fg->pattern, pat + bol + fg->wmatch, fg->patternLen);
	fg->pattern[fg->patternLen] = '\0';
			hasDot = i;
					firstHalfDot = i;
				lastHalfDot = i;
					firstLastHalfDot = i;
			break;
				goto nonspecial;
			free(fg->pattern);
			fg->pattern = NULL;
				fg->pattern[i] = toupper(fg->pattern[i]);
			break;
		fg->qsBc[fg->pattern[i]] = fg->patternLen - i;
			fg->qsBc[tolower(fg->pattern[i])] = fg->patternLen - i;
		grep_revstr(fg->pattern, fg->patternLen);
	regoff_t j;
	int rtrnVal = REG_NOMATCH;
	pmatch->rm_so = -1;
	pmatch->rm_eo = -1;
				j = dataLen - fg->patternLen;
				j = 0;
					pmatch->rm_so = j;
					pmatch->rm_eo = j + fg->patternLen;
						rtrnVal = 0;
		j = dataLen;
				pmatch->rm_so = j - fg->patternLen;
				pmatch->rm_eo = j;
					rtrnVal = 0;
					break;
				break;
			j -= fg->qsBc[(unsigned char)data[j - fg->patternLen - 1]];
		j = 0;
				pmatch->rm_so = j;
				pmatch->rm_eo = j + fg->patternLen;
					rtrnVal = 0;
					break;
				break;
				j += fg->qsBc[(unsigned char)data[j + fg->patternLen]];
	void	*ptr;
		err(2, "malloc");
	void	*ptr;
		err(2, "calloc");
		err(2, "realloc");
	size_t i;
			continue;
	int i;
	char c;
		c = str[i];
		str[i] = str[len - i - 1];
		str[len - i - 1] = c;
	int n;
	n = 0;
		fputs(line->file, stdout);
		++n;
			putchar(sep);
		printf("%d", line->line_no);
		++n;
			putchar(sep);
		printf("%lld", (long long)line->off);
		++n;
		putchar(sep);
		    pmatch->rm_eo - pmatch->rm_so, 1, stdout);
		fwrite(line->dat, line->len, 1, stdout);
	putchar('\n');
	va_list ap;
	fprintf(stderr, "minix-service: fatal error: ");
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	exit(1);
	va_list ap;
	fprintf(stderr, "minix-service: warning: ");
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	va_list ap;
	fprintf(stderr, "minix-service: fatal error: ");
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	exit(1);
	va_list ap;
	fprintf(stderr, "minix-service: warning: ");
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
    processor_set_t set;
    processor_set_name_t name;
    kr = thread_get_assignment(mach_thread_self(), &name);
	     mach_error_string(kr));
    kr = host_processor_set_priv(mach_host_priv_self(), name, &set);
	     mach_error_string(kr));
    kr = processor_set_policy_enable(set, POLICY_FIXEDPRI);
	     mach_error_string(kr));
    kr = thread_policy(mach_thread_self(), POLICY_FIXEDPRI, priority);
	     mach_error_string(kr));
__RCSID("$NetBSD: util.c,v 1.20 2020/10/11 21:32:37 roy Exp $");
	static int oaf = -1, s;
		prog_close(s);
		naf = AF_INET;
	s = prog_socket(naf, SOCK_DGRAM, 0);
		oaf = -1;
		oaf = naf;
	int len;
	bool hexstr;
	u_int8_t *p;
	len = *lenp;
	p = buf;
	hexstr = hexok && val[0] == '0' && tolower((u_char)val[1]) == 'x';
		val += 2;
			break;
			val++;
			break;
				warnx("hexadecimal digits too long");
				warnx("strings too long");
			    tohex((u_char)val[1]);
			val += 2;
			*p++ = *val++;
	len = p - buf;
		memset(p, 0, *lenp - len);
	*lenp = len;
	int i;
	bool hasspc;
	i = 0;
	hasspc = false;
				break;
				hasspc = true;
			printf("\"%.*s\"", len, buf);
			printf("%.*s", len, buf);
		printf("0x");
			printf("%02x", buf[i]);
	} u;
	size_t addrlen;
	uint8_t *addr;
	int nbit;
	memset(&u, 0, sizeof(u));
		addrlen = sizeof(u.sin.sin_addr);
		addr = (uint8_t *)&u.sin.sin_addr;
		u.sa.sa_len = sizeof(u.sin);
		break;
		addrlen = sizeof(u.sin6.sin6_addr);
		addr = (uint8_t *)&u.sin6.sin6_addr;
		u.sa.sa_len = sizeof(u.sin6);
		break;
		errno = EINVAL;
	u.sa.sa_family = af;
		errno = EINVAL;
		plen = addrlen * NBBY;
	memset(addr, 0xff, (plen + NBBY - 1) / NBBY);
	nbit = plen % NBBY;
		addr[plen / NBBY] &= ~((uint8_t)0xff >> nbit);
	pfx = malloc(offsetof(struct paddr_prefix, pfx_addr) + u.sa.sa_len);
	pfx->pfx_len = plen;
	memcpy(&pfx->pfx_addr, &u.sa, u.sa.sa_len);
	int s;
		err(EXIT_FAILURE, "getsock");
	char hbuf[NI_MAXHOST];
	int s;
	const struct sockaddr_dl *sdl;
		err(EXIT_FAILURE, "%s: getsock", __func__);
			continue;
			continue;
			warn("%s: ioctl SIOCGLIFADDR", __func__);
			continue;
			continue;
			    print_active_only ? "address:" : "link", hbuf);
	int s;
		err(EXIT_FAILURE, "socket");
		warn("SIOCGIFADDRPREF");
	int16_t preference;
	printf(" preference %" PRId16, preference);
			continue;
			continue;
static char sccsid[] = "@(#)util.c	8.5 (Berkeley) 4/28/95";
__RCSID("$NetBSD: util.c,v 1.34 2011/08/29 14:44:21 joerg Exp $");
	size_t len;
	char *name;
	int flags;
	flags = VIS_NL | VIS_OCTAL | VIS_WHITE;
		flags |= VIS_CSTYLE;
	len = strlen(src);
		errx(EXIT_FAILURE, "%s: name too long", src);
	name = (char *)malloc(4*len+1);
		len = strvis(name, src, flags);
		(void)printf("%s", name);
		free(name);
		errx(EXIT_FAILURE, "out of memory!");
	size_t size;
	char buf[MB_LEN_MAX];
	size = wcrtomb(buf, wc, pst);
			--size;
		fwrite(buf, 1, size, stdout);
	int n = 0;
	mbstate_t src_state, stdout_state;
	const char *endptr = src + strlen(src) + 1;
	memset(&src_state, 0, sizeof(src_state));
	memset(&stdout_state, 0, sizeof(stdout_state));
		wchar_t wc;
		size_t rv, span = endptr - src;
			span = MB_CUR_MAX;
		rv = mbrtowc(&wc, src, span, &src_state);
			n += printwc(wc, &stdout_state);
			break;
			n += printwc(L'?', &stdout_state);
			src++;
			memset(&src_state, 0, sizeof(src_state));
			n += printwc(iswprint(wc) ? wc : L'?', &stdout_state);
			src += rv;
static char sccsid[] = "@(#)util.c	8.2 (Berkeley) 4/2/94";
__RCSID("$NetBSD: util.c,v 1.11 2006/12/15 22:45:34 christos Exp $");
	char *ep;
		ep = cp + (strlen(cp) - 1);
			*ep = '\0';
			++cp;
		errno = ENOTDIR;
	run_err("%s: %s", cp, strerror(errno));
	exit(1);
	int c;
	char *cp;
	cp = cp0;
		c = *cp;
			goto bad;
			goto bad;
bad:	warnx("%s: invalid user name", cp0);
	sig_t istat, qstat;
	int status;
	pid_t pid;
		(void)execl(_PATH_BSHELL, "sh", "-c", s, NULL);
		_exit(127);
	istat = signal(SIGINT, SIG_IGN);
	qstat = signal(SIGQUIT, SIG_IGN);
		status = -1;
	(void)signal(SIGINT, istat);
	(void)signal(SIGQUIT, qstat);
	size_t size;
	char *nbuf;
		run_err("fstat: %s", strerror(errno));
	size = roundup(stb.st_blksize, blksize);
		size = blksize;
		free(bp->buf);
		bp->buf = NULL;
		bp->cnt = 0;
		run_err("%s", strerror(errno));
	bp->buf = nbuf;
	bp->cnt = size;
		warnx("lost connection");
	exit(1);
void *	malloc( unsigned );
  char *temp;
  int len;
  assert(str);
  len = strlen(str) + 2;
  temp = (char *) malloc(len);
  strcpy(temp, str);
  char *temp;
  int len;
  len = strlen(str1) + strlen(str2) + 2;
  temp = (char *) malloc(len);
  strcpy(temp, str1);
  strcat(temp, str2);
extern void nicenum(uint64_t num, char *buf, size_t);
	vdev_stat_t *vs;
	vdev_stat_t *v0 = { 0 };
	uint64_t sec;
	uint64_t is_log = 0;
	nvlist_t **child;
	uint_t c, children;
	char used[6], avail[6];
	char rops[6], wops[6], rbytes[6], wbytes[6], rerr[6], werr[6], cerr[6];
	v0 = umem_zalloc(sizeof (*v0), UMEM_NOFAIL);
		    " capacity   operations   bandwidth  ---- errors ----\n");
		    "used avail  read write  read write  read write cksum\n");
		char *suffix = "", *bias = NULL;
		char bias_suffix[32];
		(void) nvlist_lookup_uint64(nv, ZPOOL_CONFIG_IS_LOG, &is_log);
		    &bias);
			vs = v0;
			    " (%s)", bias);
			suffix = bias_suffix;
		sec = MAX(1, vs->vs_timestamp / NANOSEC);
		nicenum(vs->vs_alloc, used, sizeof (used));
		nicenum(vs->vs_space - vs->vs_alloc, avail, sizeof (avail));
		nicenum(vs->vs_ops[ZIO_TYPE_READ] / sec, rops, sizeof (rops));
		nicenum(vs->vs_ops[ZIO_TYPE_WRITE] / sec, wops, sizeof (wops));
		    sizeof (rbytes));
		    sizeof (wbytes));
		nicenum(vs->vs_read_errors, rerr, sizeof (rerr));
		nicenum(vs->vs_write_errors, werr, sizeof (werr));
		nicenum(vs->vs_checksum_errors, cerr, sizeof (cerr));
		    rops, wops, rbytes, wbytes, rerr, werr, cerr);
	umem_free(v0, sizeof (*v0));
		nvlist_t *cnv = child[c];
		char *cname, *tname;
		uint64_t np;
			cname = "<unknown>";
		tname = calloc(1, strlen(cname) + 2);
		(void) strcpy(tname, cname);
			tname[strlen(tname)] = '0' + np;
		show_vdev_stats(tname, ctype, cnv, indent + 2);
		free(tname);
	nvlist_t *config, *nvroot;
	char *name;
	VERIFY(spa_get_stats(spa_name(spa), &config, NULL, 0) == 0);
	    &nvroot) == 0);
	    &name) == 0);
	show_vdev_stats(name, ZPOOL_CONFIG_CHILDREN, nvroot, 0);
	show_vdev_stats(NULL, ZPOOL_CONFIG_L2CACHE, nvroot, 0);
	show_vdev_stats(NULL, ZPOOL_CONFIG_SPARES, nvroot, 0);
	nvlist_free(config);
	void *zpoolhdl;
	char *varname = arg, *varval;
	u_longlong_t val;
	    "little-endian systems\n", varname);
		*varval = '\0';
		varval++;
		val = strtoull(varval, NULL, 0);
			    "be a 32-bit unsigned integer\n", varname);
	zpoolhdl = dlopen("libzpool.so", RTLD_LAZY);
		uint32_t *var;
		var = dlsym(zpoolhdl, varname);
			    "in libzpool.so\n", varname);
		*var = (uint32_t)val;
		dlclose(zpoolhdl);
		    "variable\n");
	zfs_cmd_t *zcp;
	nvlist_t *innvl;
	char *packed = NULL;
	size_t size = 0;
	int fd, ret;
	fd = open("/dev/zfs", O_RDWR);
	zcp = umem_zalloc(sizeof (zfs_cmd_t), UMEM_NOFAIL);
	innvl = fnvlist_alloc();
	(void) strlcpy(zcp->zc_name, name, sizeof (zcp->zc_name));
	packed = fnvlist_pack(innvl, &size);
	zcp->zc_nvlist_src = (uint64_t)(uintptr_t)packed;
	zcp->zc_nvlist_src_size = size;
	ret = ioctl(fd, ZFS_IOC_POOL_SYNC, zcp);
	fnvlist_pack_free(packed, size);
	free((void *)(uintptr_t)zcp->zc_nvlist_dst);
	nvlist_free(innvl);
	umem_free(zcp, sizeof (zfs_cmd_t));
	(void) close(fd);
	*isactive = (ret == 0);
};
int sqlite_malloc_failed = 0;
static int memcnt = 0;
};
    v1 = v1*10.0 + (*z - '0');
    z++;
    LONGDOUBLE_TYPE divisor = 1.0;
    z++;
      v1 = v1*10.0 + (*z - '0');
      divisor *= 10.0;
      z++;
    v1 /= divisor;
    int esign = 1;
    int eval = 0;
    LONGDOUBLE_TYPE scale = 1.0;
    z++;
      esign = -1;
      z++;
      eval = eval*10 + *z - '0';
      z++;
      v1 /= scale;
      v1 *= scale;
  isNumA = sqliteIsNumber(atext);
  isNumB = sqliteIsNumber(btext);
      result = -1;
      double rA, rB;
      rA = sqliteAtoF(atext, 0);
      rB = sqliteAtoF(btext, 0);
        result = -1;
        result = 0;
    result = strcmp(atext, btext);
        break;
        res = strcmp(&a[1],&b[1]);
    a += strlen(&a[1]) + 2;
    b += strlen(&b[1]) + 2;
    exp = -exp;
    r = -r;
  exp += 1024;
  r += 0.5;
    strcpy(z, "~~~~~~~~~~~~");
  *z++ = zDigit[(exp>>6)&0x3f];
  *z++ = zDigit[exp & 0x3f];
    int digit;
    r *= 64.0;
    digit = (int)r;
    assert( digit>=0 && digit<64 );
    *z++ = zDigit[digit & 0x3f];
    r -= digit;
    cnt++;
  *z = 0;
            prior_c = c2;
            prior_c = c2;
          sqliteNextChar(zPattern);
        sqliteNextChar(zString);
        zPattern++;
        break;
        zPattern++;
        zString++;
        break;
	int i, ng;
	int ret, maxgroups;
	ret = -1;
	ng = 0;
	maxgroups = *ngroups;
	 * When installing primary group, duplicate it;
		goto done;
	groups[ng++] = basegid;
		goto done;
	groups[ng++] = basegid;
	(*this->rewind)(this);
			continue;
					goto done;
				groups[ng++] = grp->gr_gid;
				break;
	ret = 0;
	*ngroups = ng;
	int i, j;
	_mp_xfree(b);
	b->len = a->len;
		i = -i;
	b->val = _mp_xalloc(i, "_mp_move");
		b->val[j] = a->val[j];
	short *i;
	(void) fprintf(stderr, "%s: %p\n", s, i);
		_mp_fatal("mp: no free space");
	(void) fprintf(stderr, "%s\n", s);
	(void) fflush(stdout);
	(void) sleep(2);
	abort();
	(void) fprintf(stderr, "xfree ");
		free(c->val);
		c->len = 0;
	int i, j;
		i = -i;
		;
		_mp_xfree(a);
		a->len = j;
		a->len = -j;
	MINT *a;
	a = malloc(sizeof (MINT));
		a->len = 1;
		a->val = _mp_xalloc(1, "mp_itom1");
		*a->val = n;
		a->val = _mp_xalloc(1, "mp_itom2");
		*a->val = -n;
		a->len = 0;
	MINT c;
	int res;
	_mp_mcan(a);
	_mp_mcan(b);
	c.len = 0;
	mp_msub(a, b, &c);
	res = c.len;
	_mp_xfree(&c);
	short digit;
	MINT *m = mp_itom(0);
	MINT *d;
	MINT *sixteen;
	sixteen = mp_itom(16);
		digit = xtoi(*key);
		d = mp_itom(digit);
		mp_mult(m, sixteen, m);
		mp_madd(m, d, m);
		mp_mfree(d);
	mp_mfree(sixteen);
	d &= 15;
	MINT *m = mp_itom(0);
	MINT *zero = mp_itom(0);
	short r;
	char *p;
	char c;
	char *s;
	char *hex;
	int size;
		size = key->len;
		size = -key->len;
	hex = malloc((size_t)((size * BASEBITS + 3)) / 4 + (size ? 1 : 2));
	_mp_move(key, m);
	p = hex;
		mp_sdiv(m, 16, m, &r);
		*p++ = itox(r);
	mp_mfree(m);
	mp_mfree(zero);
	*p = 0;
		c = *p;
		*p = *s;
		*s = c;
	_mp_xfree(a);
	free(a);
	void *p = ops->alloc(len);
		(void) memset(p, 0, len);
		abort();
	uint64_t sz;
		errno = ENOMEM;
		errno = ENOMEM;
	ops->free(p, len);
	VERIFY3U(newsz, >, oldsz);
	void *temp = zalloc(ops, newsz);
		(void) memcpy(temp, p, oldsz);
		xfree(ops, p, oldsz);
	size_t len = strlen(src);
	char *str = zalloc(ops, len + 1);
	(void) memcpy(str, src, len);
	free(p);
};
sysdem_ops_t *sysdem_ops_default = &i_sysdem_ops_default;
};
	char *fullpath = NULL;
	int pathlen = 0;
	int dirlen = 0;
		pathlen = strlen(filepath);
		dirlen = strlen(dir);
			int len = pathlen + dirlen + 2;
			fullpath = (char *)malloc(len);
				    dir, filepath);
	int num;
	char timetok[10];
		*ltime = 0;
	(void) memset(timetok, 0, sizeof (timetok));
	int e;
	e = topo_node_range_create(mp, tn, cnm, imin, imax);
		    cnm, topo_strerror(topo_mod_errno(mp)));
	ulong_t r;
	char *e;
	r = strtoul(str, &e, 16);
		    "Trouble converting %s to a number!\n", str);
		*err = -1;
	*err = 0;
	char *s = NULL, *p = NULL;
	di_prom_handle_t promtree = DI_PROM_HANDLE_NIL;
		    "get vpd data: di_prom_handle_init failed.\n");
		*serial = topo_mod_strdup(mp, s);
		*part = topo_mod_strdup(mp, p);
	nvlist_t *fmri;
	tnode_t *ntn;
	nvlist_t *auth;
	char *serial = NULL, *part = NULL;
	auth = topo_mod_auth(mp, parent);
		(void) get_pci_vpd_sn_pn(mp, priv, &serial, &part);
	    auth, part, NULL, serial);
	nvlist_free(auth);
	topo_mod_strfree(mp, serial);
	topo_mod_strfree(mp, part);
	ntn = topo_node_bind(mp, parent, name, i, fmri);
		    topo_strerror(topo_mod_errno(mp)));
		nvlist_free(fmri);
	nvlist_free(fmri);
	int err;
	*out = NULL;
static clock_t next_alarm;
	char *p;
		panic("out of memory: %d", size);
		free_contig(buf, size);
	int r;
	r = sys_setalarm(dt, 0);
		panic("sys_setalarm failed: %d", r);
			panic("clearing unset alarm: %d", r);
		next_alarm = 0;
			panic("overwriting alarm: %d", r);
		next_alarm = getticks() + dt;
char strbuf[81];
int priv_change_needed = 0;
	void *ret;
	ret = malloc(size);
			(void) sleep(1);
			ret = malloc(size);
			(void) err_msg("%s\n", gettext(strerror(errno)));
			    "Memory allocation failure, Exiting...\n"));
			exit(1);
	(void) memset(ret, 0, size);
		(void) memset(strbuf, 8, pos);
		strbuf[pos] = 0;
		(void) printf(strbuf);
		(void) memset(strbuf, ' ', pos);
		strbuf[pos] = 0;
		(void) printf(strbuf);
		(void) memset(strbuf, 8, pos);
		strbuf[pos] = 0;
		(void) printf(strbuf);
	(void) printf("%s", str);
	(void) fflush(stdout);
	char *msg, *msg1;
	msg = gettext("System could not supply data at the required rate.\n");
		err_msg(gettext("System error: %s\n"), strerror(te->te_errno));
		err_msg(gettext("Transport mechanism error:\n"));
				err_msg(gettext("Bad media.\n"));
				    te->key, te->asc, te->ascq);
				err_msg(gettext("I/O error\n"));
			err_msg("%s\n", strerror(te->te_errno));
		err_msg(gettext("User abort.\n"));
		err_msg(gettext("Unknown error type.\n"));
			err_msg("Trackio err type %d\n", te->err_type);
int progress_pos;
static uint64_t last_total;
time_t tm;
	progress_pos = 0;
	last_total = 0;
	tm = time(NULL);
	char s[BUFSIZE];
	uint64_t total = (uint64_t)arg;
		progress_pos = str_print("(flushing ...)", progress_pos+2);
			tm = time(NULL);
			    gettext("%d bytes written"), completed);
			progress_pos = str_print(s, progress_pos);
		total = (((uint64_t)completed) * 100)/total;
		last_total = total;
				(void) printf("\nWrote more than 100 %% !!\n");
			(void) snprintf(s, BUFSIZE, gettext("done.\n"));
			(void) snprintf(s, BUFSIZE, "%d %%", (uint_t)total);
		progress_pos = str_print(s, progress_pos);
			cur_uid = 0;
			cur_uid = ruid;
	pw = getpwuid(uid);
	hrtime_t start, req;
	start = gethrtime();
	req = start + ((hrtime_t)ms * 1000000);
		yield();
	int id;
	char *comment;
	MsgID top;
	SetID next;
};
	int no_write;
	int id;
	char *msg;
	int line;
	char *file;
	char *comment;
	MsgID next;
};
static SetID setid_top;
static char *msg_comment;
static char *set_comment;
static int msgid_table[NL_SETMAX+1];
static char quote = QUOTE;
static void add_msgid(SetID, int, char *, char *, int, int);
static void add_setid(int, int, char *, char *, int, int);
static SetID lookup_setid(int);
static MsgID lookup_msgid(SetID, int, char *, char *, int);
static void print_prefix(FILE *, char *, int, char *);
static int is_bs_terminated(char *);
static char *ustrdup(char *);
static void makeup_msg(char **);
	SetID si;
			add_msgid(si, msgid, msg, file, line, no_write);
		add_setid(setid, msgid, msg, file, line, no_write);
	FILE *fp;
	SetID si = setid_top;
	char *mode = "w";
	char pquote[2];
		    file);
		mode = "a";
		prg_err(gettext("cannot create \"%s\""), file);
		pquote[0] = quote;
		pquote[0] = '\0';
	pquote[1] = '\0';
		(void) fprintf(fp, "\n$quote %s\n\n", pquote);
		int is_set = FALSE;
		MsgID mi = si->top;
				mi = mi->next;
				continue;
				(void) fprintf(fp, "\n");
				is_set = TRUE;
			makeup_msg(&(mi->msg));
			    mi->id, pquote, mi->msg, pquote);
					    mi->comment);
					print_prefix(fp, "$ ", FALSE, msg);
			(void) fprintf(fp, "%s", msg);
					    mi->comment);
					print_prefix(fp, "$ ", FALSE, msg);
			(void) fprintf(fp, "\n");
			mi = mi->next;
		si = si->next;
	(void) fclose(fp);
	SetID si = setid_top;
		si = si->next;
	MsgID mi = si->top;
				    mi->msg, mi->file, mi->line);
		mi = mi->next;
	MsgID mi = si->top, newmi, prev = NULL;
	int len = strlen(msg);
		    msgid);
		    msgid);
		src_err(file, line, gettext("too long message text"));
			break;
		prev = mi;
		mi = mi->next;
		prg_err(gettext("fatal: out of memory"));
		exit(EXIT_FAILURE);
	newmi->no_write = no_write;
	newmi->id = msgid;
	newmi->msg = ustrdup(msg);
	newmi->file = ustrdup(file);
	newmi->line = line;
	newmi->next = mi;
		newmi->comment = ustrdup(msg_comment);
		free(msg_comment);
		msg_comment = NULL;
		newmi->comment = NULL;
		si->top = newmi;
		prev->next = newmi;
	SetID si = setid_top, newsi, prev = NULL;
			break;
		prev = si;
		si = si->next;
		prg_err(gettext("fatal: out of memory"));
		exit(EXIT_FAILURE);
	newsi->id = setid;
	newsi->top = NULL;
	newsi->next = si;
		newsi->comment = ustrdup(set_comment);
		free(set_comment);
		set_comment = NULL;
		newsi->comment = NULL;
		setid_top = newsi;
		prev->next = newsi;
	add_msgid(newsi, msgid, msg, file, line, no_write);
	(void) fprintf(fp, "%s", prefix);
		(void) fputc(*str, fp);
			(void) fprintf(fp, "%s", prefix);
				str++;
				SkipSpace(str);
				continue;
		str++;
		(void) fputc('\n', fp);
	FILE *fp;
	char line[LINE_MAX];
		char *p = line;
		int n, setid, msgid;
		SkipSpace(p);
			continue;
		n = sscanf(p, "%d %d", &setid, &msgid);
				    file, setid);
				continue;
			msgid_table[setid] = msgid;
			    "warning: %s: missing or invalid entry"), file);
	(void) fclose(fp);
	FILE *fp;
	register int i;
		    file);
		prg_err(gettext("cannot create \"%s\""), file);
			SetID si;
			char *com = NULL;
				print_prefix(fp, "# ", TRUE, com);
			(void) fprintf(fp, "%d\t%d\n", i, msgid_table[i]);
				print_prefix(fp, "# ", TRUE, com);
	(void) fclose(fp);
	SetID si = setid_top;
	int id = msgid_table[setid];
			MsgID mi = si->top;
				mi = mi->next;
		si = si->next;
	id++;
		    setid);
		msgid_table[setid] = msgid;
	char *tag = (mode == MsgCommentMode) ? mctag : sctag;
	    ? &msg_comment : &set_comment;
		free(*comment);
	*comment = ustrdup(str);
	FILE *fp;
	char c = 0;
	int line = 0;
	int inmsg = FALSE;
	int setid = 0, unsetid = -1, msgid = 0;
		prg_err(gettext("cannot open \"%s\""), file);
		ResetActiveMode(AppendMode);
		ResetActiveMode(AppendMode);
	quote = c;
		char buf[LINE_MAX];
		char *ptr;
		char msg[NL_TEXTMAX];
			break;
		line++;
		ptr = &buf[0];
		SkipSpace(ptr);
				setid = 0;
			inmsg = FALSE;
			continue;
			continue;
				(void) strlcat(msg, ptr, sizeof (msg));
				inmsg = TRUE;
				int len = strlen(ptr);
				*(ptr + len - 1) = '\0';
					*(ptr + len - 2) = '\0';
				(void) strlcat(msg, ptr, sizeof (msg));
				add_msg(setid, msgid, msg, file, line, TRUE);
				inmsg = FALSE;
			continue;
			char	*pptr;
			SkipSpace(ptr);
			msgid = (int)strtol(ptr, &pptr, 10);
			ptr = pptr;
			SkipSpace(ptr);
				(void) memset(msg, 0, sizeof (msg));
					ptr++;
				(void) strlcpy(msg, ptr, sizeof (msg));
				inmsg = TRUE;
				int len = strlen(ptr);
				*(ptr + len - 1) = '\0';
				add_msg(setid, msgid, ptr, file, line, TRUE);
				inmsg = FALSE;
	(void) fclose(fp);
	int len = strlen(msg);
			continue;
	char *tmp = strdup(str);
		prg_err(gettext("fatal: out of memory"));
		exit(EXIT_FAILURE);
	int ret = TRUE;
	FILE *fin, *fout;
	int c;
	sigset_t newmask, oldmask;
	(void) sigemptyset(&newmask);
	(void) sigaddset(&newmask, SIGQUIT);
	(void) sigaddset(&newmask, SIGINT);
	(void) sigaddset(&newmask, SIGHUP);
	(void) sigaddset(&newmask, SIGTERM);
	(void) sigprocmask(SIG_BLOCK, &newmask, &oldmask);
		prg_err(gettext("cannot open \"%s\""), in);
		ret = FALSE;
		goto done;
		prg_err(gettext("cannot create \"%s\""), out);
		ret = FALSE;
		goto done;
		(void) putc(c, fout);
	(void) fclose(fin);
	(void) fclose(fout);
	(void) sigprocmask(SIG_SETMASK, &oldmask, NULL);
	char buf[NL_TEXTMAX];
	char *msg;
	msg = *pmsg;
	buf[0] = '\0';
		int len = strlen(msg);
			msg[len-2] = '\0';
			(void) strlcat(buf, msg, sizeof (buf));
			(void) strlcat(buf, msg, sizeof (buf));
			(void) strlcat(buf, msg, sizeof (buf));
			(void) strlcat(buf, "\\n", sizeof (buf));
			(void) strlcat(buf, msg, sizeof (buf));
			(void) strlcat(buf, msg, sizeof (buf));
			(void) strlcat(buf, msg, sizeof (buf));
		free(msg);
		*pmsg = ustrdup(buf);
	msg = *pmsg;
	buf[0] = '\0';
		(void) strlcat(buf, premsg, sizeof (buf));
		(void) strlcat(buf, msg, sizeof (buf));
		free(msg);
		*pmsg = ustrdup(buf);
	msg = *pmsg;
	buf[0] = '\0';
		int len = strlen(msg);
			msg[len-2] = '\0';
			(void) strlcat(buf, msg, sizeof (buf));
			(void) strlcat(buf, sufmsg, sizeof (buf));
			(void) strlcat(buf, "\\n", sizeof (buf));
			(void) strlcat(buf, msg, sizeof (buf));
			(void) strlcat(buf, sufmsg, sizeof (buf));
		free(msg);
		*pmsg = ustrdup(buf);
	va_list ap;
	va_start(ap, fmt);
	(void) fprintf(stderr, "%s: ", program);
	(void) vfprintf(stderr, fmt, ap);
	(void) fprintf(stderr, "\n");
	va_end(ap);
	va_list ap;
	va_start(ap, fmt);
	(void) fprintf(stderr, gettext("\"%s\", line %d: "), file, line);
	(void) vfprintf(stderr, fmt, ap);
	(void) fprintf(stderr, "\n");
	va_end(ap);
static int signal_pipe[2];
	(void) gettimeofday(&p, NULL);
		(void) fprintf(stderr, "Out of memory!\n");
		g_assert(0);
		exit(2);
	void *ret = malloc(size);
	lt_check_null(ret);
	void *ret = calloc(size, 1);
	lt_check_null(ret);
	char *ret = strdup(str);
	lt_check_null(ret);
	time_t t;
	int i;
	(void) time(&t);
	(void) gmtime_r(&t, &tms);
	(void) asctime_r(&tms, buffer, len);
			buffer[i] = '\0';
			break;
	char name[PATH_MAX];
	int fd;
	int ret;
	psinfo_t psinfo;
	(void) snprintf(name, PATH_MAX, "/proc/%d/psinfo", (int)pid);
	fd = open(name, O_RDONLY);
	ret = read(fd, (char *)&psinfo, sizeof (psinfo_t));
	(void) close(fd);
		entry->lt_s_count += value;
		break;
		entry->lt_s_total += value;
		break;
			entry->lt_s_max = value;
		break;
		break;
	g_assert(a != NULL && b != NULL);
	g_assert(a != NULL && b != NULL);
	g_assert(a != NULL && b != NULL);
	double avg_a, avg_b;
	g_assert(a != NULL && b != NULL);
	avg_a = (double)a->lt_se_data.lt_s_total / a->lt_se_data.lt_s_count;
	avg_b = (double)b->lt_se_data.lt_s_total / b->lt_se_data.lt_s_count;
	(void) pipe(signal_pipe);
	(void) close(signal_pipe[0]);
	(void) close(signal_pipe[1]);
	(void) write(signal_pipe[1], ch, 1);
static char	*phrase(char *name, int token, int comma);
static char	*ripoff(register char *buf);
	register char *cp, *cp2, *top;
		;
	top = (char *)salloc((unsigned)(cp-str + 1));
		*cp2++ = *cp;
	*cp2 = 0;
	fprintf(stderr, gettext("mailx: Panic - %s\n"), str);
	exit(1);
	register struct message *mp;
	mp = &message[mesg-1];
	mp->m_flag |= MTOUCH;
		mp->m_flag |= MREAD|MSTATUS;
	register char **ap;
		;
	register FILE *ibuf;
	char linebuf[LINESIZE];
	register long lc;
	char *r = NOSTR;
	ibuf = setinput(mp);
	lc--;
			r = (*add)(r, hcontents(linebuf));
	char line2[LINESIZE];
	register char *cp, *cp2;
	register int c;
		rem--;
			continue;
				break;
			c = getc(f);
			ungetc(c, f);
				break;
				break;
			rem--;
			cp2 = line2;
				;
				break;
			cp = &linebuf[strlen(linebuf)];
				cp--;
			*cp++ = ' ';
				;
			nstrcpy(cp, LINESIZE - (cp - linebuf), cp2);
			cp = &linebuf[c-1];
				cp--;
			*++cp = 0;
	register char *cp;
	*cp = 0;
		*cp = ':';
	*cp = ':';
	register char *cp;
	cp++;
		cp++;
	register char *cp, *cp2;
	cp2 = dst;
	cp = src;
		*cp2++ = tolower(*cp++);
	*cp2 = '\0';
} *sstack;
	register FILE *fi;
	register char *cp;
		printf(gettext("Unable to open %s\n"), cp);
			maxfiles = _NFILE;
		sstack = (struct sstack *)calloc(maxfiles, sizeof(struct sstack));
			fclose(fi);
	sstack[++ssp].s_file = input;
	sstack[ssp].s_cond = cond;
	sstack[ssp].s_loading = loading;
	loading = 0;
	cond = CANY;
	input = fi;
	sourcing++;
		printf(gettext("\"Source\" stack over-pop.\n"));
		sourcing = 0;
	fclose(input);
	cond = sstack[ssp].s_cond;
	loading = sstack[ssp].s_loading;
	input = sstack[ssp--].s_file;
		sourcing = loading;
	int rc = utime(name, utimep);
	extern int errno;
name);
		fprintf(stderr, gettext("Errno: %d\n"), errno);
	register const char *cp;
	register char c;
	register char *cp, *cp2;
	char *bufend, *nbufp;
	int gotlt, lastsp, didq;
	char nbuf[LINESIZE];
	int nesting;
		nbufp = (char *)salloc(strlen(name));
		nbufp = nbuf;
	gotlt = 0;
	lastsp = 0;
	bufend = nbufp;
			nesting = 1;
				cp++;
					cp++;
					break;
					nesting++;
					break;
					--nesting;
					break;
			lastsp = 0;
			break;
			didq = 0;
				cp++;
					cp++;
					break;
					goto outqs;
						lastsp = 0;
						*cp2++ = ' ';
						*cp2++ = '"';
						didq++;
					*cp2++ = c;
				*cp2++ = '"';
			lastsp = 0;
			break;
				cp[-1] = 0;
			lastsp = 1;
			break;
			*cp2++ = c;
					goto done;
				bufend = cp2;
				gotlt = 0;
			break;
			cp2 = bufend;
			gotlt = c;
			lastsp = 0;
			break;
				gotlt = c;
				break;
					lastsp = 0;
					*cp2++ = ' ';
				*cp2++ = c;
			break;
	*cp2 = 0;
	    nbufp == nbuf ? savestr(nbuf) : nbufp);
	char *cp;
		name++;
	cp = phrase(name, 1, comma);
	nstrcpy(word, sz, name);
	char namebuf[LINESIZE];
	char linebuf[LINESIZE];
	register char *cp, *cp2;
	register FILE *ibuf;
	int first = 1, wint = 0;
	char	*tmp;
	ibuf = setinput(mp);
	copy("", namebuf);
		;
		cp++;
		;
	*cp2 = '\0';
			break;
			continue;
			cp = linebuf;
			cp = linebuf + 1;
			break;
			break;
			cp += wint + 12;
				copy(cp, namebuf);
				first = 0;
				tmp = strrchr(namebuf, '!') + 1;
					cp);
			nstrcat(namebuf, sizeof (namebuf), "!");
			goto newname;
			break;
		cp = strchr(cp, '!') + 1;
			host, cp);
		cp = linebuf;
	char buf[LINESIZE];
	char *cp, *cp2;
		cp2 = strchr(cp, '>');
			nstrcpy(buf, sizeof (buf), addr);
				cp - hdr + 1, hdr, addr, cp2);
			addr, cp);
		nstrcpy(buf, sizeof (buf), addr);
	register char *cp;
	cp = buf + strlen(buf);
		cp--;
	*++cp = 0;
	register int c;
	char realfld[BUFSIZ];
	istrcpy(realfld, sizeof (realfld), field);
	register struct ignore *igp;
	int i, j, len1, len2;
	len1 = strlen(string1);
	len2 = strlen(string2);
			;
	char *cp, *cp2;
	cp2 = dst;
	cp = src;
		*cp2++ = *cp++;
	*cp2 = '\0';
	char *cp, *cp2;
	cp2 = dst;
	cp = src;
		cp2++;
		dstsize--;
		*cp2++ = *cp++;
	*cp2 = '\0';
	va_list	ap;
	va_start(ap, err);
	(void) fprintf(stderr, gettext(ERR_ERROR));
	(void) vfprintf(stderr, err, ap);
	va_end(ap);
	exit(2);
	va_list	ap;
	va_start(ap, err);
	(void) fprintf(stderr, gettext(WARN_WARNING));
	(void) vfprintf(stderr, err, ap);
	va_end(ap);
	va_list	ap;
	va_start(ap, err);
	(void) vfprintf(stderr, err, ap);
	va_end(ap);
	void	*t;
	t = malloc(size);
		error(gettext(ERR_MALLOC));
	void	*t;
	t = calloc(nelem, elsize);
		error(gettext(ERR_MALLOC));
	void	*t;
	t = realloc(ptr, size);
		free(ptr);
		error(gettext(ERR_MALLOC));
	char	*t;
	t = strdup(str);
		error(gettext(ERR_MALLOC));
char *str;
register char *delim;
int ros;
	p = (str == NULL) ? savep : str ;
	ep = strpbrk(p, delim);
		savep = NULL;
	savep = ep + 1;
	*ep = '\0';
register char *p;
register struct sactab *sp;
	p = nexttok(p, DELIM, FALSE);
		error(E_BADFILE, EXIT);
		Saferrno = E_SAFERR;
		error("_sactab file is corrupt");
		p[PMTAGSIZE] = '\0';
		(void) sprintf(scratch, "tag too long, truncated to <%s>", p);
		log(scratch);
		(void) fprintf(stderr, "tag too long, truncated to <%s>", p);
	(void) strcpy(sp->sc_tag, p);
	p = nexttok(NULL, DELIM, FALSE);
		error(E_BADFILE, EXIT);
		Saferrno = E_SAFERR;
		error("_sactab file is corrupt");
		p[PMTYPESIZE] = '\0';
		(void) sprintf(scratch, "type too long, truncated to <%s>", p);
		log(scratch);
		(void) fprintf(stderr, "type too long, truncated to <%s>", p);
	(void) strcpy(sp->sc_type, p);
	p = nexttok(NULL, DELIM, FALSE);
		error(E_BADFILE, EXIT);
		Saferrno = E_SAFERR;
		error("_sactab file is corrupt");
	sp->sc_flags = 0;
			sp->sc_flags |= D_FLAG;
			break;
			sp->sc_flags |= X_FLAG;
			break;
			(void) sprintf(scratch, "Unrecognized flag <%c>", *(p - 1));
			log(scratch);
			Saferrno = E_SAFERR;
			error(scratch);
			break;
	p = nexttok(NULL, DELIM, FALSE);
		error(E_BADFILE, EXIT);
		Saferrno = E_SAFERR;
		error("_sactab file is corrupt");
	sp->sc_rsmax = atoi(p);
	p = nexttok(NULL, DELIM, FALSE);
		error(E_BADFILE, EXIT);
		Saferrno = E_SAFERR;
		error("_sactab file is corrupt");
		error(E_MALLOC, EXIT);
		Saferrno = E_SAFERR;
		error("malloc failed");
	(void) strcpy(sp->sc_cmd, p);
		error(E_MALLOC, EXIT);
		Saferrno = E_SAFERR;
		error("malloc failed");
	(void) strcpy(sp->sc_comment, Comment);
register char *p;
	tp = strchr(p, COMMENT);
	Comment[0] = '\0';
		*tp = '\0';
		tp = strchr(Comment, '\n');
			*tp ='\0';
		*tp = '\0';
		error(E_BADSTATE, EXIT);
		Saferrno = E_SAFERR;
		error("Improper message from SAC\n");
	char *line;
	int maxlen;
	FILE *fp;
	register char *p;
	register char *start;
	int c;
	start = line;
				start = p - 1;
				maxlen++;
				goto nextline;
		maxlen--;
		(void) ungetc(*(p-1), fp);
			break;
	maxlen = strlen(line) + 1;
		line[maxlen - 2] = '\n';
			"ignored\n--- %s", line);
	char *message;
	(void) fprintf(stderr, "fatal error: %s\n", message);
	exit(1);
	    r_debug.rtd_rdebug.r_state));
	    r_debug.rtd_rdebug.r_state));
	    r_debug.rtd_rdebug.r_state));
	void	(*fptr)(Lm_list *);
		fptr = rtld_db_preinit;
		break;
		fptr = rtld_db_postinit;
		break;
			rtld_flags &= ~RT_FL_DBNOTIF;
			break;
			rtld_flags |= RT_FL_DBNOTIF;
			break;
		};
		fptr = rtld_db_dlactivity;
		break;
		break;
	};
	r_debug.rtd_rdebug.r_state = state;
	r_debug.rtd_rdebug.r_rdevent = event;
	fptr(lml);
	r_debug.rtd_rdebug.r_rdevent = RD_NONE;
	int		ndx;
	long		*argc;
	char		**oargv, **nargv;
	char		**oenvp, **nenvp;
	auxv_t		*oauxv, *nauxv;
	nargv = &argv[0];
	oargv = &argv[rmcnt];
		nargv[ndx] = oargv[ndx];
	nargv[ndx] = oargv[ndx];
	argc = (long *)((uintptr_t)argv - sizeof (long *));
	*argc -= rmcnt;
	ndx++;
	nenvp = &nargv[ndx];
	oenvp = &oargv[ndx];
	*envp = nenvp;
		nenvp[ndx] = oenvp[ndx];
	nenvp[ndx] = oenvp[ndx];
	ndx++;
	nauxv = (auxv_t *)&nenvp[ndx];
	oauxv = (auxv_t *)&oenvp[ndx];
	*auxv = nauxv;
		nauxv[ndx] = oauxv[ndx];
	nauxv[ndx] = oauxv[ndx];
	uint_t		hash1, hash2;
	const char	*st1, *st2;
	int		rc;
	hash1 = ((PathNode *)n1)->pn_hash;
	hash2 = ((PathNode *)n2)->pn_hash;
	st1 = ((PathNode *)n1)->pn_name;
	st2 = ((PathNode *)n2)->pn_name;
	rc = strcmp(st1, st2);
	avl_tree_t	*avlt;
	avl_create(avlt, pnavl_compare, size, SGSOFFSETOF(PathNode, pn_avl));
	PathNode	pn;
	pn.pn_name = name;
		pn.pn_hash = sgs_str_hash(name);
	FullPathNode	fpn, *fpnp;
	fpn.fpn_node.pn_name = name;
		fpn.fpn_node.pn_hash = sgs_str_hash(name);
	FullPathNode	*fpnp;
	uint_t		hash = sgs_str_hash(name);
		Rt_map	*_lmp __maybe_unused;
		_lmp = fpavl_recorded(lml, name, hash, &where);
		ASSERT(_lmp == NULL);
	fpnp->fpn_node.pn_name = name;
	fpnp->fpn_node.pn_hash = hash;
	fpnp->fpn_lmp = lmp;
		free(fpnp);
	ASSERT(lml->lm_fpavl != NULL);
	avl_insert(lml->lm_fpavl, fpnp, where);
	FullPathNode	*fpnp;
	Aliste		idx;
		avl_remove(LIST(lmp)->lm_fpavl, fpnp);
		free(fpnp);
	free(FPNODE(lmp));
	FPNODE(lmp) = NULL;
	PathNode	*pnp;
	uint_t		hash = sgs_str_hash(name);
		int	in_nfavl __maybe_unused;
		in_nfavl = pnavl_recorded(&nfavl, name, hash, &where);
		ASSERT(in_nfavl == 0);
		pnp->pn_name = name;
		pnp->pn_hash = hash;
		avl_insert(nfavl, pnp, where);
	char		buffer[PATH_MAX], *str;
	size_t		size;
	avl_index_t	where;
	PathNode	*pnp;
	uint_t		hash;
		size = 1;
		size = str - name;
	(void) strncpy(buffer, name, size);
	buffer[size] = '\0';
	hash = sgs_str_hash(buffer);
		pnp->pn_name = strdup(buffer);
		pnp->pn_hash = hash;
		avl_insert(spavl, pnp, where);
static avl_tree_t	*stravl = NULL;
static char		*strbuf = NULL;
static PathNode		*pnbuf = NULL;
static size_t		strsize = 0, pnsize = 0;
	char		str[PATH_MAX];
	PathNode	*pnp;
	avl_index_t	where;
		nsize = strlen(name) + 1;
		pnsize = syspagsz;
	pnbuf->pn_name = name;
		pnbuf->pn_hash = sgs_str_hash(name);
		strsize = S_ROUND(nsize, syspagsz);
	(void) memcpy(strbuf, name, nsize);
	pnp = pnbuf;
	pnp->pn_name = strbuf;
	avl_insert(stravl, pnp, where);
	strbuf += nsize;
	strsize -= nsize;
	pnbuf++;
	pnsize -= sizeof (PathNode);
	Rt_map	**tobj;
	(void) rt_mutex_lock(&dlmp->rt_lock);
	(void) rt_mutex_unlock(&dlmp->rt_lock);
		tobj[0] = dlmp;
		call_init(tobj, DBG_INIT_DYN);
	int	start, stop, incr, ndx;
	uint_t	arraycnt = (uint_t)(arraysz / sizeof (Addr));
		start = arraycnt - 1;
		stop = incr = -1;
		start = 0;
		stop = arraycnt;
		incr = 1;
		uint_t	rtldflags;
		void	(*fptr)(void) = (void(*)())array[ndx];
		DBG_CALL(Dbg_util_call_array(lmp, (void *)fptr, ndx, shtype));
		APPLICATION_ENTER(rtldflags);
		leave(LIST(lmp), 0);
		(*fptr)();
		(void) enter(0);
		APPLICATION_RETURN(rtldflags);
	Rt_map		**_tobj, **_nobj;
	static APlist	*pending = NULL;
		(void) aplist_append(&pending, tobj, AL_CNT_PENDING);
		Rt_map	*lmp = *_tobj;
		void	(*iptr)() = INIT(lmp);
			continue;
		FLAGS(lmp) |= FLG_RT_INITCALL;
		lmp->rt_init_thread = rt_thr_self();
			rtld_flags |= RT_FL_INITFIRST;
			DBG_CALL(Dbg_util_call_init(lmp, flag));
			uint_t	rtldflags;
			APPLICATION_ENTER(rtldflags);
			leave(LIST(lmp), 0);
			(*iptr)();
			(void) enter(0);
			APPLICATION_RETURN(rtldflags);
		    SHT_INIT_ARRAY);
			DBG_CALL(Dbg_util_call_init(lmp, DBG_INIT_DONE));
		(void) rt_mutex_lock(&lmp->rt_lock);
		FLAGS(lmp) |= FLG_RT_INITDONE;
		lmp->rt_init_thread = (thread_t)0;
		(void) _lwp_cond_broadcast(&lmp->rt_cv);
		(void) rt_mutex_unlock(&lmp->rt_lock);
		SORTVAL(lmp) = -1;
	free(tobj);
	Rt_map **_tobj;
		Rt_map		*lmp = *_tobj;
			void	(*fptr)(void) = FINI(lmp);
				DBG_CALL(Dbg_util_call_fini(lmp));
			    SHT_FINI_ARRAY);
				uint_t	rtldflags;
				APPLICATION_ENTER(rtldflags);
				leave(lml, 0);
				(*fptr)();
				(void) enter(0);
				APPLICATION_RETURN(rtldflags);
			continue;
			audit_objclose(lmp, clmp);
	    pltcntu32, pltcntu44, pltcntfull, pltcntfar));
	free(tobj);
	Rt_map	**tobj, *lmp;
	Lm_list	*lml;
	Aliste	idx;
	(void) enter(0);
	rtld_flags |= RT_FL_ATEXIT;
	lml = &lml_main;
	lml->lm_flags |= LML_FLG_ATEXIT;
	lml->lm_flags &= ~LML_FLG_INTRPOSETSORT;
	lmp = (Rt_map *)lml->lm_head;
		call_fini(lml, tobj, NULL);
	unused(lml);
			continue;
		lml->lm_flags |= LML_FLG_ATEXIT;
		lml->lm_flags &= ~LML_FLG_INTRPOSETSORT;
			call_fini(lml, tobj, NULL);
		unused(lml);
	lml = &lml_main;
	lmp = (Rt_map *)lml->lm_head;
		audit_objclose((Rt_map *)lml_rtld.lm_head, lmp);
		audit_objclose(lmp, lmp);
			continue;
		lml->lm_flags |= LML_FLG_ATEXIT;
		lml->lm_flags &= ~LML_FLG_INTRPOSETSORT;
			call_fini(lml, tobj, NULL);
		unused(lml);
	lml = &lml_rtld;
	lml->lm_flags |= LML_FLG_ATEXIT;
	lml->lm_flags &= ~LML_FLG_INTRPOSETSORT;
	lmp = (Rt_map *)lml->lm_head;
		call_fini(lml, tobj, NULL);
	leave(&lml_main, 0);
	Rt_map	**tobj = NULL;
	Lm_list	*nlml;
		nlml = LIST(nlmp);
		(void) rt_get_extern(nlml, nlmp);
		rt_thr_init(nlml);
		call_init(tobj, DBG_INIT_SORT);
	Lm_cntl	*lmc;
	int	add = 1;
	(lml->lm_obj)++;
		rd_event(lml, RD_DLACTIVITY, RT_ADD);
	lmc = (Lm_cntl *)alist_item_by_offset(lml->lm_lists, lmco);
		lmc->lc_head = lmc->lc_tail = lmp;
		add = 0;
		NEXT(lmc->lc_tail) = (Link_map *)lmp;
		PREV(lmp) = (Link_map *)lmc->lc_tail;
		lmc->lc_tail = lmp;
	CNTL(lmp) = lmco;
		lml->lm_flags |= LML_FLG_INTRPOSE;
		lml->lm_head = lmc->lc_head;
		lml->lm_tail = lmc->lc_tail;
	Lm_cntl	*lmc;
		rd_event(lml, RD_DLACTIVITY, RT_DELETE);
		audit_activity(clmp, LA_ACT_DELETE);
	lmc = (Lm_cntl *)alist_item_by_offset(lml->lm_lists, CNTL(lmp));
		lmc->lc_head = NEXT_RT_MAP(lmp);
		NEXT(PREV_RT_MAP(lmp)) = (void *)NEXT(lmp);
		lmc->lc_tail = PREV_RT_MAP(lmp);
		PREV(NEXT_RT_MAP(lmp)) = PREV(lmp);
		lml->lm_head = lmc->lc_head;
		lml->lm_tail = lmc->lc_tail;
	(lml->lm_obj)--;
	Rt_map	*lmp;
		rd_event(lml, RD_DLACTIVITY, RT_ADD);
	DBG_CALL(Dbg_file_cntl(lml, nlmco, plmco));
		CNTL(lmp) = plmco;
			lml->lm_init++;
			lml->lm_flags |= LML_FLG_OBJADDED;
		plmc->lc_head = nlmc->lc_head;
		PREV(nlmc->lc_head) = NULL;
		NEXT(plmc->lc_tail) = (Link_map *)nlmc->lc_head;
		PREV(nlmc->lc_head) = (Link_map *)plmc->lc_tail;
	plmc->lc_tail = nlmc->lc_tail;
	nlmc->lc_head = nlmc->lc_tail = NULL;
		lml->lm_head = plmc->lc_head;
		lml->lm_tail = plmc->lc_tail;
	u_longlong_t	variable = 0;
	ushort_t	select = 0;
	const char	**str;
	Word		val = 0;
		select |= SEL_PERMANT;
		select |= SEL_REPLACE;
			select |= SEL_ACT_SPEC_2;
			variable = ENV_FLG_AUDIT_ARGS;
			select |= SEL_ACT_STR;
			str = &dbg_file;
			variable = ENV_FLG_DEBUG_OUTPUT;
			select |= SEL_ACT_RT;
			val = RT_FL_NODIRCFG;
			variable = ENV_FLG_NODIRCONFIG;
			select |= SEL_ACT_RT;
			val = RT_FL_NOENVCFG;
			variable = ENV_FLG_NOENVCONFIG;
			select |= SEL_ACT_RT2;
			val = RT_FL2_NOFLTCFG;
			variable = ENV_FLG_NOFLTCONFIG;
			select |= SEL_ACT_RT;
			val = RT_FL_NOOBJALT;
			variable = ENV_FLG_NOOBJALTER;
			select |= SEL_ACT_LML;
			val = LML_FLG_TRC_NOUNRESWEAK;
			variable = ENV_FLG_NOUNRESWEAK;
			select |= SEL_ACT_STR;
			str = &profile_out;
			variable = ENV_FLG_PROFILE_OUTPUT;
			cmdisa |= variable;
			cmdgen |= variable;
			rplisa |= variable;
			prmisa |= variable;
			rplgen |= variable;
			prmgen |= variable;
			rtld_flags |= val;
			rtld_flags &= ~val;
			rtld_flags2 |= val;
			rtld_flags2 &= ~val;
			*str = NULL;
			*str = s2;
			*lmflags |= val;
			*lmflags &= ~val;
			*lmtflags |= val;
			*lmtflags &= ~val;
			*str = NULL;
			*str = s2;
					env_info |= ENV_INF_FLAGCFG;
					env_info |= ENV_INF_PATHCFG;
					env_info &= ~ENV_INF_FLAGCFG;
					env_info &= ~ENV_INF_PATHCFG;
				audit_argcnt = atoi(s2);
				audit_argcnt += audit_argcnt % 2;
				audit_argcnt = 0;
			break;
				rpl_debug = MSG_ORIG(MSG_TKN_BINDINGS);
				rpl_debug = NULL;
			break;
				rtld_flags |= RT_FL_CONFGEN;
				*lmflags |= LML_FLG_IGNRELERR;
				rtld_flags &= ~RT_FL_CONFGEN;
				*lmflags &= ~LML_FLG_IGNRELERR;
			break;
				*lmtflags |= LML_TFLG_LOADFLTR;
					rtld_flags |= RT_FL_WARNFLTR;
				*lmtflags &= ~LML_TFLG_LOADFLTR;
				rtld_flags &= ~RT_FL_WARNFLTR;
			break;
			profile_name = s2;
					    MSG_ORIG(MSG_PTH_LDPROFSE_64);
					    MSG_ORIG(MSG_PTH_LDPROFSE);
					    MSG_ORIG(MSG_PTH_LDPROF_64);
					    MSG_ORIG(MSG_PTH_LDPROF);
				profile_lib = NULL;
			break;
			killsig = s2 ? atoi(s2) : SIGKILL;
			break;
				*lmflags |= LML_FLG_TRC_ENABLE;
					*lmflags |= LML_FLG_TRC_LDDSTUB;
				    ~(LML_FLG_TRC_ENABLE | LML_FLG_TRC_LDDSTUB);
			break;
	size_t	_len = *len - 3;
			*len = *len - 3;
			*len = *len - 3;
	char	*nstr, *sstr, *estr = NULL;
	size_t	nlen, len;
	len = strlen(str);
	(void) strcpy(nstr, str);
		int	flags = 0;
					estr = sstr;
						*sstr = *sstr - ('a' - 'A');
			continue;
		*sstr = '\0';
			nlen = estr - nstr;
				estr = NULL;
			nlen = sstr - nstr;
			flags |= ENV_TYP_NULL;
			estr = (char *)MSG_ORIG(MSG_STR_ONE);
			    lmtflags, (env_flags | flags));
			break;
		nstr = sstr + 1;
		estr = NULL;
	int	ndx;
		char	*str;
			break;
			ndx++;
			break;
			ndx++;
			str = argv[ndx];
			str = &argv[ndx][2];
			str += 3;
	stack_cleanup(argv, envp, auxv, ndx);
	const char	*s2;
	size_t		len;
	int		flags;
		s1 += MSG_LD_BRAND_PREFIX_SIZE;
		len = strlen(s1);
		s2 = NULL;
		s2 = NULL;
		len = s2 - s1 - 1;
			s2++;
	env_flags |= flags;
	ld_generic_env(s1, len, s2, lmflags, lmtflags, env_flags);
	char		*locale;
	const char	*s1;
	int		loc = 0;
		const char	*s2;
			continue;
		s2 = s1;
				s2 += MSG_LC_ALL_SIZE;
					    (char *)s2;
					loc = LOC_ALL;
				s2 += MSG_LC_MESSAGES_SIZE;
					    (char *)s2;
					loc = LOC_MESG;
			continue;
		s2 = s1;
			glcs[CI_LCMESSAGES].lc_un.lc_ptr = NULL;
			glcs[CI_LCMESSAGES].lc_un.lc_ptr = strdup(locale);
	Aliste		idx;
	const char	*s1;
		ld_str_env(s1, lmflags, lmtflags, 0);
		rpl_ldflags = NULL;
		rpl_audit = profile_lib = profile_name = NULL;
		*lmflags &= ~LML_MSK_TRC;
		rtld_flags2 &= ~RT_FL2_BINDLAZY;
		*lmflags |= LML_FLG_TRC_NOPAREXT;
	Word		*lmflags = &(lml_main.lm_flags);
	Word		*lmtflags = &(lml_main.lm_tflags);
		uint_t		env_flags = ENV_TYP_CONFIG;
		const char	*s1 = (const char *)(envtbl->env_str + addr);
			env_flags |= ENV_TYP_PERMANT;
			ld_str_env(s1, lmflags, lmtflags, env_flags);
		envtbl++;
		prm_audit = profile_lib = profile_name = NULL;
		*lmflags &= ~LML_MSK_TRC;
	(void) write(prf->pr_fd, prf->pr_buf, prf->pr_cur - prf->pr_buf);
	prf->pr_cur = prf->pr_buf;
	char	c;
	char	*bp = prf->pr_cur;
	char	*bufend = prf->pr_buf + prf->pr_len;
	size_t	bufsiz = prf->pr_len;
			PUTC(c);
			int	base = 0, flag = 0, width = 0, prec = 0;
			size_t	_i;
			int	_c, _n;
			char	*_s;
			int	ls = 0;
				flag |= FLG_UT_MINUS;
				goto again;
				flag |= FLG_UT_SHARP;
				goto again;
				flag |= FLG_UT_DOTSEEN;
				goto again;
					prec = (prec * 10) + c - '0';
					width = (width * 10) + c - '0';
				goto again;
				base = 16;
				break;
				base = 10;
				flag &= ~FLG_UT_SHARP;
				break;
				base = 10;
					goto again;
				break;
				base = 8;
				break;
				_c = va_arg(args, int);
						PUTC(c);
					PUTC(c);
				break;
				_s = va_arg(args, char *);
				_i = strlen(_s);
				_n = (int)(width - _i);
					prec = (int)_i;
						PUTC(' ');
					PUTC(c);
						PUTC(' ');
				break;
				PUTC('%');
				break;
				break;
				char		local[NUM_SIZE];
				size_t		ssize = 0, psize = 0;
				    MSG_ORIG(MSG_STR_HEXNUM);
				    MSG_ORIG(MSG_STR_EMPTY);
				u_longlong_t	num;
					    va_arg(args, uint_t);
					break;
					    va_arg(args, ulong_t);
					break;
					num = va_arg(args, u_longlong_t);
					break;
						prefix = MSG_ORIG(MSG_STR_HEX);
						psize = 2;
						prefix = MSG_ORIG(MSG_STR_ZERO);
						psize = 1;
					prefix = MSG_ORIG(MSG_STR_NEGATE);
					psize = MSG_STR_NEGATE_SIZE;
					num = (u_longlong_t)(-(longlong_t)num);
				_s = local;
					*_s++ = string[num % base];
					num /= base;
					ssize++;
				ASSERT(ssize < sizeof (local));
					_n = (int)(prec - ssize);
					_n = (int)(width - ssize - psize);
						PUTC(' ');
					PUTC(*prefix++);
					PUTC(*--_s);
					_n = (int)(width - ssize - psize);
						PUTC(' ');
	PUTC('\0');
	prf->pr_cur = bp;
	char	*ocur = prf->pr_cur;
	va_list	args;
	int	len;
	Prfbuf	prf;
	prf.pr_buf = prf.pr_cur = buf;
	prf.pr_len = 0;
	prf.pr_fd = -1;
	va_end(args);
	va_list	args;
	int	len;
	Prfbuf	prf;
	prf.pr_buf = prf.pr_cur = buf;
	prf.pr_len = n;
	prf.pr_fd = -1;
	va_end(args);
	va_list	args;
	int	len;
	va_end(args);
	va_list	args;
	char	buffer[ERRSIZE];
	Prfbuf	prf;
	prf.pr_buf = prf.pr_cur = buffer;
	prf.pr_len = ERRSIZE;
	prf.pr_fd = 1;
	va_end(args);
	prf.pr_cur--;
static char	errbuf[ERRSIZE], *nextptr = errbuf, *prevptr = NULL;
	int		overflow = 0;
	static int	lock = 0;
	Prfbuf		prf;
	lock = 1;
	prf.pr_buf = prf.pr_cur = nextptr;
	prf.pr_len = ERRSIZE - (nextptr - errbuf);
		prf.pr_fd = 2;
		prf.pr_fd = -1;
			error = ERR_WARNING;
				    MSG_INTL(MSG_ERR_WARNING);
			break;
				    MSG_INTL(MSG_ERR_WARNING);
			break;
				    MSG_INTL(MSG_ERR_GUIDANCE);
			break;
				err_strs[ERR_ELF] = MSG_INTL(MSG_ERR_ELF);
			break;
				err_strs[ERR_FATAL] = MSG_INTL(MSG_ERR_FATAL);
			break;
				overflow = 1;
				overflow = 1;
			prf.pr_cur--;
		overflow = 1;
		static int		(*elfeno)() = 0;
		static const char	*(*elfemg)();
		const char		*emsg;
		Rt_map			*dlmp, *lmp = lml_rtld.lm_head;
				elfeno = 0;
			prf.pr_cur--;
				overflow = 1;
		*(prf.pr_cur - 1) = '\n';
		(void) dowrite(&prf);
		*(prf.pr_cur - 1) = '\0';
	DBG_CALL(Dbg_util_str(lml, nextptr));
		char	*str = (char *)MSG_INTL(MSG_EMG_BUFOVRFLW);
			lasterr = str;
				(void) write(2, str, strlen(str));
				    MSG_STR_NL_SIZE);
		DBG_CALL(Dbg_util_str(lml, str));
		lock = 0;
		nextptr = errbuf + ERRSIZE;
		lasterr = nextptr;
	lock = 0;
	va_list		args;
	va_end(args);
static const char rtld_panicstr[] = "rtld assertion failure";
	(void) printf("assertion failed: %s, file: %s, line: %d\n", a, f, l);
	(void) _lwp_kill(_lwp_self(), SIGABRT);
	upanic(rtld_panicstr, sizeof (rtld_panicstr));
	    (unsigned long long)b, f, l);
	(void) _lwp_kill(_lwp_self(), SIGABRT);
	upanic(rtld_panicstr, sizeof (rtld_panicstr));
				char	*errptr = errbuf;
				char	*errend = errbuf + ERRSIZE;
					size_t	size = strlen(errptr);
					(void) write(2, errptr, size);
					    MSG_STR_NL_SIZE);
					errptr += (size + 1);
		leave(lml, 0);
		(void) _lwp_kill(_lwp_self(), killsig);
	_exit(status);
	caddr_t	va;
static int	nu_fd = FD_UNAVAIL;
	caddr_t	va;
	int	err;
			err = errno;
			    MSG_ORIG(MSG_PTH_DEVNULL), strerror(err));
		err = errno;
		    MSG_ORIG(MSG_PTH_DEVNULL), strerror(err));
			(void) rt_mutex_lock(&rtldlock);
			ld_entry_cnt++;
	Pdesc	*pdp;
	Aliste	idx;
		const char	*fmt, *name;
			continue;
			name = pdp->pd_pname;
				DBG_CALL(Dbg_util_nl(lml, DBG_NL_STD));
			    (pdp->pd_flags & PD_FLG_DUPLICAT), obj));
			continue;
					fmt = MSG_INTL(MSG_DUP_LDLIBPATHC);
					fmt = MSG_INTL(MSG_USD_LDLIBPATHC);
					fmt = MSG_INTL(MSG_DUP_LDLIBPATH);
					fmt = MSG_INTL(MSG_USD_LDLIBPATH);
			continue;
			(void) printf(MSG_ORIG(MSG_STR_NL));
		(void) printf(fmt, name, obj);
	Rt_map		*lmp;
	int		nl = 0;
	Word		unref, unuse;
	unref = lml->lm_flags & LML_FLG_TRC_UNREF;
	unuse = lml->lm_flags & LML_FLG_TRC_UNUSED;
		is_path_used(lml, unref, &nl, rpl_libdirs, config->c_name);
		is_path_used(lml, unref, &nl, prm_libdirs, config->c_name);
	nl = 0;
	lmp = lml->lm_head;
		is_path_used(lml, unref, &nl, RLIST(lmp), NAME(lmp));
	nl = 0;
			is_path_used(lml, unref, &nl, RLIST(lmp), NAME(lmp));
			Bnd_desc	*bdp;
			Aliste		idx;
				Rt_map	*clmp;
					continue;
				clmp = bdp->b_caller;
					continue;
					    (void) printf(MSG_ORIG(MSG_STR_NL));
						DBG_NL_STD));
					NAME(lmp), NAME(clmp));
				    DBG_CALL(Dbg_unused_unref(lmp, NAME(clmp)));
			continue;
				(void) printf(MSG_ORIG(MSG_STR_NL));
				DBG_CALL(Dbg_util_nl(lml, DBG_NL_STD));
				    NAME(lmp), CYCGROUP(lmp));
				    CYCGROUP(lmp)));
				    NAME(lmp));
				DBG_CALL(Dbg_unused_file(lml, NAME(lmp), 0, 0));
	DBG_CALL(Dbg_util_nl(lml, DBG_NL_STD));
	rd_event(lml, RD_DLACTIVITY, RT_CONSISTENT);
		audit_activity(lml->lm_head, LA_ACT_CONSISTENT);
		lml->lm_flags &= ~LML_FLG_ACTAUDIT;
		(void) close(nu_fd);
		nu_fd = FD_UNAVAIL;
	nextptr = errbuf;
	prevptr = NULL;
		defrag();
			(void) rt_mutex_unlock(&rtldlock);
		(void) rt_bind_clear(THR_FLG_RTLD | thr_flg_nolock | flags);
	APlist		*calp, *dalp;
	Aliste		idx1, idx2;
	Grp_hdl		*ghp1, *ghp2;
			continue;
			Grp_desc	*gdp;
			Aliste		idx3;
				continue;
					continue;
	Slookup		sl;
	Sresult		sr;
	uint_t		binfo;
	    ld_entry_cnt, 0, 0, 0, 0, LKUP_WEAK);
	SRESULT_INIT(sr, MSG_ORIG(MSG_SYM_ENVIRON));
		Rt_map	*dlmp = sr.sr_dmap;
		lml->lm_environ = (char ***)sr.sr_sym->st_value;
			    (uintptr_t)ADDR(dlmp));
		*(lml->lm_environ) = (char **)environ;
		lml->lm_flags |= LML_FLG_ENVIRON;
			rtld_flags |= RT_FL_SECURE;
		uid = getuid();
			euid = geteuid();
			rtld_flags |= RT_FL_SECURE;
				gid = getgid();
				egid = getegid();
				rtld_flags |= RT_FL_SECURE;
	rtld_stat_t	status;
	const char	*name;
		name = interp->i_name;
		name = NAME(lml_rtld.lm_head);
	char	info[SYS_NMLN];
	size_t	size;
		scapset->sc_platsz = (size_t)-1;
	(void) strcpy(scapset->sc_plat, info);
	char	info[SYS_NMLN];
	size_t	size;
		scapset->sc_machsz = (size_t)-1;
	(void) strcpy(scapset->sc_mach, info);
	extern	int	errno;
	extern int	errno;
		errno = EOVERFLOW;
	buf->st_dev = lbuf->st_dev;
	buf->st_ino = lbuf->st_ino;
	buf->st_mode = lbuf->st_mode;
	buf->st_uid = lbuf->st_uid;
	buf->st_size = (off_t)lbuf->st_size;
	buf->st_mtim = lbuf->st_mtim;
	buf->st_blksize = lbuf->st_blksize;
	int		r;
	r = stat64(path, &lbuf);
		r = rtld_stat_process(r, &lbuf, buf);
	int		r;
	r = fstat64(fildes, &lbuf);
		r = rtld_stat_process(r, &lbuf, buf);
	Lm_list	*lml = LIST(lmp);
	Dbg_util_nl(lml, DBG_NL_FRC);
	dbg_print(lml, MSG_INTL(MSG_UTL_TRANS), NAME(lmp));
	Dbg_util_nl(lml, DBG_NL_FRC);
	Lm_list		*lml = LIST(lmp);
	const char	*str;
		str = MSG_INTL(MSG_UTL_SORT);
		str = MSG_INTL(MSG_UTL_PEND);
		str = MSG_INTL(MSG_UTL_DYN);
		str = MSG_INTL(MSG_UTL_DONE);
	Dbg_util_nl(lml, DBG_NL_STD);
	dbg_print(lml, MSG_INTL(MSG_UTL_INIT), str, NAME(lmp));
	Dbg_util_nl(lml, DBG_NL_STD);
	Lm_list	*lml = LIST(lmp);
	Dbg_util_nl(lml, DBG_NL_STD);
	dbg_print(lml, MSG_INTL(MSG_UTL_INTOOLATE), NAME(lmp));
	Dbg_util_nl(lml, DBG_NL_STD);
	const char	*estr;
	const char	*sstr;
		estr = MSG_ORIG(MSG_UTL_EVNT_PREINIT);
		sstr = MSG_INTL(MSG_STR_NULL);
		break;
		estr = MSG_ORIG(MSG_UTL_EVNT_POSTINIT);
		sstr = MSG_INTL(MSG_STR_NULL);
		break;
		estr = MSG_ORIG(MSG_UTL_EVNT_DLACT);
			sstr = MSG_ORIG(MSG_UTL_STA_CONSIST);
			break;
			sstr = MSG_ORIG(MSG_UTL_STA_ADD);
			break;
			sstr = MSG_ORIG(MSG_UTL_STA_DELETE);
			break;
			sstr = MSG_INTL(MSG_STR_NULL);
			break;
		break;
		sstr = MSG_INTL(MSG_STR_NULL);
		estr = MSG_INTL(MSG_STR_UNKNOWN);
		break;
	Dbg_util_nl(lml, DBG_NL_STD);
	dbg_print(lml, MSG_INTL(MSG_UTL_DBNOTIFY), estr, sstr);
	Dbg_util_nl(lml, DBG_NL_STD);
	Lm_list		*lml = LIST(lmp);
	const char	*str;
		str = MSG_ORIG(MSG_SCN_INITARRAY);
		str = MSG_ORIG(MSG_SCN_FINIARRAY);
		str = MSG_ORIG(MSG_SCN_PREINITARRAY);
	    NAME(lmp));
	Lm_list	*lml = LIST(lmp);
	Dbg_util_nl(lml, DBG_NL_STD);
	dbg_print(lml, MSG_INTL(MSG_UTL_FINI), NAME(lmp));
	Dbg_util_nl(lml, DBG_NL_STD);
	Dbg_util_nl(lml, DBG_NL_STD);
	Dbg_util_nl(lml, DBG_NL_FRC);
	dbg_print(lml, MSG_ORIG(MSG_FMT_STR), str);
	Dbg_util_nl(lml, DBG_NL_FRC);
	Dbg_util_nl(lml, DBG_NL_STD);
	const char	*_sec;
		_sec = MSG_INTL(MSG_UTL_SCC_SUBI);
		_sec = MSG_INTL(MSG_UTL_SCC_SUBF);
	Dbg_util_nl(lml, DBG_NL_STD);
	dbg_print(lml, MSG_INTL(MSG_UTL_SCC_TITLE), _sec);
	dbg_print(LIST(lmp), MSG_ORIG(MSG_UTL_SCC_ENTRY), idx, NAME(lmp));
static	int ectoggle = 0;
	Conv_bnd_type_buf_t	bnd_type_buf;
	const char		*str;
		str = MSG_ORIG(MSG_SCN_INIT);
		str = MSG_ORIG(MSG_SCN_FINI);
		Dbg_util_nl(lml, DBG_NL_STD);
			dbg_print(lml, MSG_INTL(MSG_UTL_EDGE_TITLE_I), str);
			dbg_print(lml, MSG_INTL(MSG_UTL_EDGE_TITLE_S), str);
		dbg_print(lml, MSG_INTL(MSG_UTL_EDGE_START), ndx, NAME(dlmp));
		    NAME(clmp), conv_bnd_type(flags, &bnd_type_buf));
	ectoggle = 1;
	    NAME(clmp), NAME(dlmp));
	Lm_list		*lml = LIST(lmp);
	const char	*str;
		str = MSG_ORIG(MSG_SCN_INIT);
		str = MSG_ORIG(MSG_SCN_FINI);
		Dbg_util_nl(lml, DBG_NL_STD);
		ectoggle = 0;
	dbg_print(lml, MSG_INTL(MSG_UTL_COLLECT), ndx, NAME(lmp), str);
};
	const char	*str;
	Conv_inv_buf_t	inv_buf;
		str = MSG_ORIG(tags[tag]);
		str = conv_invalid_val(&inv_buf, tag, 0);
	    EC_NATPTR(val));
	const char	*str;
	Conv_inv_buf_t	inv_buf;
		str = MSG_ORIG(tags[tag]);
		str = conv_invalid_val(&inv_buf, tag, 0);
	    NAME(olmp));
	dbg_print(lml, MSG_ORIG(MSG_STR_EMPTY));
		dbg_desc->d_extra |= DBG_E_STDNL;
	static char *buf = NULL;
	free(buf);
	buf = (char *)conv_demangle_name(name);
		buf = NULL;
	static char *buf = NULL;
	free(buf);
	buf = (char *)conv_demangle_name(name);
		buf = NULL;
	Aliste		idx;
	Sdf_desc	*sdf;
	Sdf_desc	*sdf;
	sdf->sdf_name = name;
	char	*new;
		char	*_str;
		size_t	len;
		len = strlen(old) + strlen(str) + 2;
		(void) snprintf(new, len, MSG_ORIG(MSG_FMT_COLPATH), old, str);
		(void) strcpy(new, str);
	char    *str;
	size_t  len = MSG_ARG_WRAP_SIZE + strlen(optarg) + 1;
	    MSG_ORIG(MSG_ARG_WRAP), optarg);
	optarg = str;
			DBG_CALL(Dbg_args_str2chr(lml, ndx, opt, c));
			optind += 1;
			optopt = c;
	int	c;
		char	*arg = &argv[optind][1];
			break;
			break;
			break;
				optarg = (char *)MSG_ORIG(MSG_ARG_RESCAN_START);
			break;
				optarg = (char *)MSG_ORIG(MSG_ARG_RESCAN_END);
			break;
					    (char *)MSG_ORIG(MSG_ARG_MULDEFS);
				break;
				break;
					    MSG_ORIG(MSG_ARG_RESCAN_END);
				break;
					    MSG_ORIG(MSG_ARG_FATWARN);
				break;
					optarg = (char *)MSG_ORIG(MSG_ARG_HELP);
				break;
				break;
					    MSG_ORIG(MSG_ARG_NOFATWARN);
					optarg = (char *)MSG_ORIG(MSG_ARG_DEFS);
					    (char *)MSG_ORIG(MSG_ARG_DFLEXTRT);
				break;
				break;
				break;
					    MSG_ORIG(MSG_ARG_RESCAN_START);
				break;
				break;
				break;
					    (char *)MSG_ORIG(MSG_ARG_ALLEXTRT);
				break;
			break;
	uint_t		hash1, hash2;
	const char	*st1, *st2;
	int		rc;
	hash1 = ((Isd_node *)n1)->isd_hash;
	hash2 = ((Isd_node *)n2)->isd_hash;
	st1 = ((Isd_node *)n1)->isd_name;
	st2 = ((Isd_node *)n2)->isd_name;
	rc = strcmp(st1, st2);
	Capstr		*capstr1;
	Aliste		idx1;
	int		match = 0;
	Word		nitems;
		Capstr		*capstr2;
		Aliste		idx2;
				continue;
			match++;
			break;
		elfedit_msg(ELFEDIT_MSG_ERR, MSG_INTL(MSG_ERR_BADGETVAL));
		elfedit_msg(ELFEDIT_MSG_ERR, MSG_INTL(MSG_ERR_BADGETVAL));
	    value, required));
	size_t		cmp_len;
	const char	*tail;
		str++;
	tail = str + strlen(str);
		tail--;
	cmp_len = tail - str;
	size_t		cmp_len;
	const char	*tail;
		str++;
	tail = str + strlen(str);
		tail--;
	cmp_len = tail - str;
		elfedit_cpl_match(cpldata, sym->sym_name, 1);
		elfedit_cpl_match(cpldata, sym->sym_name, 1);
	elfedit_cpl_atoui(cpldata, elfedit_const_to_atoui(const_type));
	char		*endptr;
	*v = strtoll(str, &endptr, 0);
	elfedit_atoi_t v;
		    MSG_INTL(MSG_ERR_BADATOISTR), str);
	char		*endptr;
	*v = strtoull(str, &endptr, 0);
	elfedit_atoui_t v;
		    MSG_INTL(MSG_ERR_BADATOISTR), str);
	    (*v >= min) && (*v <= max));
	elfedit_atoi_t v = elfedit_atoi(str, sym);
		    item_name, EC_XWORD(min), EC_XWORD(max), EC_XWORD(v));
	    (*v >= min) && (*v <= max));
	elfedit_atoui_t v = elfedit_atoui(str, sym);
		    item_name, EC_XWORD(min), EC_XWORD(max), EC_XWORD(v));
	    elfedit_const_to_atoui(const_type), v));
	    elfedit_const_to_atoui(const_type)));
	    ELFEDIT_CONST_BOOL) != 0);
	elfedit_atoui_t ndx;
	ndx = elfedit_atoconst(str, ELFEDIT_CONST_SHN);
		    EC_WORD(ndx), EC_WORD(shnum-1));
	int		ret;
	elfedit_atoui_t	value;
	    elfedit_const_to_atoui(ELFEDIT_CONST_OUTSTYLE), &value);
		*outstyle = value;
	elfeditGC_cmd_t *cmd = elfedit_curcmd();
	state->go_argc = argc;
	state->go_argv = argv;
	state->go_optarg = cmd->cmd_opt;
	state->go_idmask = 0;
	state->go_done = 0;
	state->go_sglgrp = NULL;
	elfedit_cmd_optarg_t	*optarg;
	const char		*argstr;
	int			argc = *(state->go_argc);
	const char		**argv = *(state->go_argv);
	elfedit_optarg_item_t	item;
		int			valid;
		int			is_outstyle;
		elfedit_getopt_ret_t	ret;
		elfedit_cmd_oa_mask_t	excmask;
	} sgl_with_value;
		argstr = argv[0];
			elfedit_command_usage();
			(*state->go_argc)--;
			(*state->go_argv)++;
		sgl_with_value.valid = 0;
			    (optarg->oa_name == ELFEDIT_STDOA_OPT_O);
			int need_value;
			elfedit_next_optarg(&optarg, &item);
			need_value = item.oai_flags & ELFEDIT_CMDOA_F_VALUE;
				(*state->go_argc) = --argc;
				(*state->go_argv) = ++argv;
					elfedit_command_usage();
				state->go_idmask |= item.oai_idmask;
				state->go_ret.gor_idmask = item.oai_idmask;
						elfedit_command_usage();
					state->go_ret.gor_value = argv[0];
					(*state->go_argc)--;
					(*state->go_argv)++;
					state->go_ret.gor_value = NULL;
					    state->go_ret.gor_value);
			(*state->go_argc)--;
			(*state->go_argv)++;
				elfedit_command_usage();
			state->go_idmask |= sgl_with_value.ret.gor_idmask;
			state->go_ret = sgl_with_value.ret;
				    state->go_ret.gor_value);
		int ch = *state->go_sglgrp++;
			(*state->go_argc)--;
			(*state->go_argv)++;
			state->go_sglgrp = NULL;
			elfedit_next_optarg(&optarg, &item);
	elfedit_command_usage();
	int	nbits = sizeof_orig_v * 8;
	int	mask;
	int	cnt = 0;
			cnt++;
	char	*data = data_start;
		    name_str, EC_WORD(num_ent), EC_WORD(num_ent - 1));
		size_t ncpy = num_ent - (start_ndx + cnt);
		    data + (start_ndx * entsize), ncpy * entsize);
			    EC_WORD(start_ndx + cnt), EC_WORD(start_ndx));
			    EC_WORD(start_ndx + ncpy - 1));
	bzero(data + ((num_ent - cnt) * entsize), entsize * cnt);
		    name_str, EC_WORD(num_ent - 1));
		    EC_WORD(num_ent - 1), EC_WORD(cnt));
	char	*data = data_start;
		    name_str, EC_WORD(num_ent), EC_WORD(num_ent - 1));
		size_t low, hi;
			low = dstndx;
			hi = srcndx;
			low = srcndx;
			hi = dstndx;
			    EC_WORD(dstndx), EC_WORD(dstndx + cnt - 1));
		    name_str, EC_WORD(srcndx), EC_WORD(dstndx));
		    EC_WORD(dstndx), EC_WORD(dstndx + cnt - 1));
		srcndx += cnt - 1;
		dstndx += cnt - 1;
			 *	save = dyn[srcndx];
			bcopy(data + (srcndx * entsize), scr_item, entsize);
			 *		dyn[i] = dyn[i + 1];
			    (dstndx - srcndx) * entsize);
			 *	dyn[dstndx] = save;
			bcopy(scr_item, data + (dstndx * entsize), entsize);
			 *	save = dyn[srcndx];
			bcopy(data + (srcndx * entsize), scr_item, entsize);
			 *		dyn[i] = dyn[i - 1];
			    (srcndx - dstndx) * entsize);
			 *	dyn[dstndx] = save;
			bcopy(scr_item, data + (dstndx * entsize), entsize);
	Env_desc	*env;
	char		*str;
	size_t		varsz, totsz = strlen(arg) + 1;
		Aliste	idx;
		varsz = (size_t)(str - arg);
		Aliste	idx;
				continue;
					    env->e_str);
				env->e_flags &= ~RTC_ENV_CONFIG;
				env->e_flags |= flags;
		varsz = 0;
	env->e_varsz = varsz;
	env->e_totsz = totsz;
	env->e_flags = flags;
	crle->c_envnum++;
	crle->c_strsize += totsz;
	char		*str, *arg;
	char		*lasts;
	size_t		tlen = strlen(args) + 1;
	const char	*colon = MSG_ORIG(MSG_STR_COLON);
	str = alloca(tlen);
	(void) strcpy(str, args);
	arg = str;
			size_t	llen, alen = strlen(arg);
					continue;
				llen = strlen(*lib);
				tlen = llen + 1;
				llen = 0;
				tlen = 0;
			alen += 1;
			tlen += alen;
				int err = errno;
				    crle->c_name, strerror(err));
				(void) strcpy(str, arg);
				    MSG_ORIG(MSG_FMT_COLON), arg);
			*lib = str;
			crle->c_strsize += alen;
	int		_flags;
	char		*tok, *_arg;
	char		*lasts;
	const char	*separate = MSG_ORIG(MSG_MOD_SEPARATE);
	(void) strcpy(_arg, arg);
			_flags |= RTLD_REL_RELATIVE;
			_flags |= RTLD_REL_EXEC;
			_flags |= RTLD_REL_DEPENDS;
			_flags |= RTLD_REL_PRELOAD;
			_flags |= RTLD_REL_SELF;
			_flags |= RTLD_REL_WEAK;
			_flags |= RTLD_REL_ALL;
			_flags |= RTLD_MEMORY;
			_flags |= RTLD_STRIP;
			_flags |= RTLD_NOHEAP;
			_flags |= RTLD_CONFGEN;
			    crle->c_name, tok);
			free(_arg);
		    crle->c_name, arg);
	free(_arg);
	char *s;
	int l;
	char c, *h;
	bool ok;
	static int len = 0;
	static char *buf = NULL;
	ok = true;
			++l;
			ok = false;
			l += 3;
			ok = false;
		char *nbuf = sm_pmalloc_x(l);
			sm_free(buf);
		len = l;
		buf = nbuf;
		c = *s;
			*h++ = c;
			*h++ = '\\';
			--l;
				*h++ = '\\';
				break;
				*h++ = 't';
				break;
				*h++ = 'n';
				break;
				*h++ = 'r';
				break;
				SM_ASSERT(l >= 2);
					(unsigned int)((unsigned char) c));
				l -= 2;
				h += 3;
				break;
	*h = '\0';
	buf[len - 1] = '\0';
	char *ibp;
	char *obp;
	int *bsp;
	char *ip, *op;
	int bufused, olen;
	bool buffer_same, needs_quoting;
	buffer_same = ibp == obp;
	needs_quoting = false;
			olen++;
			needs_quoting = true;
		obp = sm_malloc_x(olen);
		buffer_same = false;
		*bsp = olen;
			bufused = sm_strlcpy(obp, ibp, *bsp);
			SM_ASSERT(bufused <= olen);
		obp = sm_malloc_x(olen);
		buffer_same = false;
		*bsp = olen;
			SM_ASSERT(bufused < olen);
			op[bufused++] = METAQUOTE;
		SM_ASSERT(bufused < olen);
		op[bufused++] = *ip;
	op[bufused] = '\0';
	char *ibp;
	char *obp;
	int obs;
	char *ip, *op;
	int len;
	bool quoted;
	quoted = false;
	len = 0;
			quoted = true;
			continue;
			*op++ = *ip;
			++len;
		quoted = false;
	*op = '\0';
	const char *s;
	size_t l;
	char *n;
	l = strlen(s);
	SM_ASSERT(l + 1 > l);
	n = xalloc(l + 1);
	sm_strlcpy(n, s, l + 1);
	char *s;
	SM_RPOOL_T *rpool;
	int len = 0;
	char c;
	char *p = s, *q, *r;
		len++;
			len++;
	q = r = sm_rpool_malloc_x(rpool, len + 3);
	p = s;
	*q++ = '"';
			*q++ = '\\';
		*q++ = c;
	*q++ = '"';
	*q = '\0';
	char *s;
	char *p, *q, c;
	p = q = s;
		p++;
		c = *q++ = *p++;
	char *s;
	bool quoted = false;
	int commentlev = 0;
	char *c = s;
			c++;
			quoted = !quoted;
					commentlev--;
				commentlev++;
		c++;
	char *string;
	size_t length;
	bool backslash = false;
	bool quoted = false;
	size_t slen;
	int parencount = 0;
	char *ptr = string;
	slen = strlen(string);
		length = slen;
			backslash = false;
			goto increment;
			backslash = true;
				parencount++;
				parencount = 0;
			quoted = !quoted;
				backslash = false;
				parencount--;
				quoted = false;
			break;
		ptr++;
			*ptr = ')';
		ptr++;
			*ptr = '"';
		ptr++;
		*ptr = '\0';
	char *string;
	int character;
	bool backslash = false;
	bool quoted = false;
	int parencount = 0;
			backslash = false;
				break;
			string++;
			continue;
			backslash = true;
			break;
				parencount++;
			break;
				parencount = 0;
			break;
			string++;
			continue;
			quoted = !quoted;
			break;
		string++;
	char *bodytype;
	char *str;
	size_t len;
	int delim;
	char *p;
	*(str + len - 1) = '\0';
		*p = '\0';
			*p++ = (char) delim;
			*p = '\0';
			(void) sm_strlcat(str, "...", len);
		(void) sm_strlcpy(str, "...", len);
		str[0] = '\0';
	register int sz;
	char *file;
	int line;
	register int sz;
	register char *p;
	SM_REQUIRE(sz >= 0);
		sz = 1;
	sm_xtrap_raise_x(&SmHeapOutOfMemory);
	p = sm_malloc_tagged((unsigned) sz, file, line, sm_heap_group());
		sm_exc_raise_x(&SmHeapOutOfMemory);
	char **list;
	bool copycont;
	SM_RPOOL_T *rpool;
	register char **vp;
	register char **newvp;
		continue;
	vp++;
	newvp = (char **) sm_rpool_malloc_x(rpool, (vp - list) * sizeof(*vp));
	memmove((char *) newvp, (char *) list, (int) (vp - list) * sizeof(*vp));
			*vp = sm_rpool_strdup_x(rpool, *vp);
	ADDRESS *addr;
	SM_RPOOL_T *rpool;
	register ADDRESS *newaddr;
	ADDRESS *ret;
	register ADDRESS **tail = &ret;
							sizeof(*newaddr));
			STRUCTCOPY(*addr, *newaddr);
			*tail = newaddr;
			tail = &newaddr->q_next;
		addr = addr->q_next;
	*tail = NULL;
static SM_FILE_T	*Pidf = NULL;
	ENVELOPE *e;
	long sff;
	char pidpath[MAXPATHLEN];
	extern char *CommandLineArgs;
	sff = SFF_NOLINK|SFF_ROOTOK|SFF_REGONLY|SFF_CREAT|SFF_NBLOCK;
		sff |= SFF_OPENASROOT;
	expand(PidFile, pidpath, sizeof(pidpath), e);
	Pidf = safefopen(pidpath, O_WRONLY|O_TRUNC, FileMode, sff);
				  pidpath);
				  pidpath, sm_errstring(errno));
		PidFilePid = getpid();
				     (long) PidFilePid);
				     CommandLineArgs);
		(void) sm_io_flush(Pidf, SM_TIME_DEFAULT);
		sm_syslog(LOG_INFO, NOQID, "started as: %s", CommandLineArgs);
	(void) sm_io_close(Pidf, SM_TIME_DEFAULT);
	Pidf = NULL;
	int mode;
	ENVELOPE *e;
	char buf[2];
	e->e_sendmode = (char) mode;
	buf[0] = (char) mode;
	buf[1] = '\0';
	int mode;
	char buf[2];
	extern ENVELOPE BlankEnvelope;
	OpMode = (char) mode;
	buf[0] = (char) mode;
	buf[1] = '\0';
	macdefine(&BlankEnvelope.e_macro, A_TEMP, MID_OPMODE, buf);
	SM_FILE_T *fp;
	char **av;
			sm_dprintf("\n\t%08lx=", (unsigned long) *av);
			(void) sm_io_putc(fp, SM_TIME_DEFAULT, ' ');
			sm_dprintf("%s", str2prt(*av++));
			xputs(fp, *av++);
	(void) sm_io_putc(fp, SM_TIME_DEFAULT, '\n');
	SM_FILE_T *fp;
	const char *s;
	int c;
	extern struct metamac MetaMacros[];
		"@(#)$Debug: ANSI - enable reverse video in debug output $");
			TermEscape.te_rv_on = "\033[7m";
			TermEscape.te_normal = "\033[0m";
			TermEscape.te_rv_on = "";
			TermEscape.te_normal = "";
				     TermEscape.te_rv_on, TermEscape.te_normal);
					     TermEscape.te_normal);
						     TermEscape.te_rv_on);
					continue;
				c = *s++ & 0377;
				goto printchar;
						     TermEscape.te_rv_on);
							  SM_TIME_DEFAULT, '&');
					continue;
							  *s++);
							     macname(bitidx(*s++)));
							     *s++);
				continue;
							     mp->metaname);
					break;
							     macname(bitidx(*s++)));
							     *s++);
				continue;
					     TermEscape.te_rv_on);
			c &= 0177;
			(void) sm_io_putc(fp, SM_TIME_DEFAULT, c);
			continue;
			c = 'n';
			break;
			c = 'r';
			break;
			c = 't';
			break;
					     TermEscape.te_rv_on);
			(void) sm_io_putc(fp, SM_TIME_DEFAULT, '\\');
			(void) sm_io_putc(fp, SM_TIME_DEFAULT, c);
			(void) sm_io_fprintf(fp, SM_TIME_DEFAULT, " %o ", c);
			(void) sm_io_fprintf(fp, SM_TIME_DEFAULT, " %#x ", c);
			(void) sm_io_putc(fp, SM_TIME_DEFAULT, '^');
			(void) sm_io_putc(fp, SM_TIME_DEFAULT, c ^ 0100);
				     TermEscape.te_normal);
	(void) sm_io_flush(fp, SM_TIME_DEFAULT);
	register char *p;
	register char c;
			*p = tolower(c);
	char *line;
	bool stripnl;
	register char *p;
	p = strchr(line, '\n');
		p--;
		*p++ = '\n';
	*p = '\0';
	register char *l;
	register MCI *mci;
	register char *l;
	size_t len;
	register MCI *mci;
	int pxflags;
	register char *p, *end;
	int slop;
	bool dead, quotenext, strip8bit;
		    bitset(PXLF_STRIP8BIT, pxflags);
	dead = false;
	slop = 0;
	end = l + len;
		bool noeol = false;
		p = memchr(l, '\n', end - l);
			p = end;
			noeol = true;
					     "%05d >>> ", (int) CurrentPid);
			register char *q = &l[mci->mci_mailer->m_linelimit - slop - 1];
					dead = true;
							  SM_TIME_DEFAULT, '.');
					dead = true;
							  '>');
				break;
			PUTX(q);
				break;
				dead = true;
				break;
						     (int) CurrentPid);
			slop = 1;
			break;
				dead = true;
				break;
						  SM_TIME_DEFAULT, '.');
				dead = true;
				break;
						  SM_TIME_DEFAULT, '>');
		PUTX(p);
			break;
					  '\n');
			mci->mci_flags &= ~MCIF_INLONGLINE;
				dead = true;
				break;
			mci->mci_flags |= MCIF_INLONGLINE;
					dead = true;
					break;
							  SM_TIME_DEFAULT, ' ');
	char *f;
	register int i;
	int save_errno;
		sm_syslog(LOG_DEBUG, CurEnv->e_id, "unlink %s", f);
	i = unlink(f);
	save_errno = errno;
			  f, errno);
		SYNC_DIR(f, false);
	errno = save_errno;
	char *buf;
	int siz;
	SM_FILE_T *fp;
	time_t timeout;
	char *during;
	register char *p;
	int save_errno;
	int io_timeout;
	SM_REQUIRE(siz > 0);
	SM_REQUIRE(buf != NULL);
		buf[0] = '\0';
		errno = EBADF;
	p = NULL;
	errno = 0;
	io_timeout = (timeout <= 0) ? SM_TIME_DEFAULT : timeout * 1000;
		errno = 0;
		p = sm_io_fgets(fp, io_timeout, buf, siz);
					  during);
			buf[0] = '\0';
			checkfd012(during);
						     (int) CurrentPid);
			errno = ETIMEDOUT;
			break;
		(void) sm_io_clearerr(fp);
	save_errno = errno;
	LineNumber++;
		buf[0] = '\0';
					     (int) CurrentPid);
		errno = save_errno;
				     "%05d <<< %s", (int) CurrentPid, buf);
			*p &= ~0200;
				HasEightBits = true;
				break;
	char *buf;
	int *np;
	SM_FILE_T *f;
	register char *p = buf;
	char *bp = buf;
	register int i;
	int n;
	SM_REQUIRE(np != NULL);
	n = *np;
	SM_REQUIRE(n > 0);
	SM_REQUIRE(buf != NULL);
		buf[0] = '\0';
		errno = EBADF;
	n--;
			i = sm_io_getc(f, SM_TIME_DEFAULT);
							    i);
				i = '\r';
			char *nbp;
			int nn;
			nn = (p - bp);
				nn *= 2;
				nn += MEMCHUNKSIZE;
			nbp = sm_malloc_x(nn);
			memmove(nbp, bp, p - bp);
			p = &nbp[p - bp];
				sm_free(bp);
			bp = nbp;
			n = nn - (p - bp);
			*np = nn;
		*p++ = i;
			LineNumber++;
			i = sm_io_getc(f, SM_TIME_DEFAULT);
				(void) sm_io_ungetc(f, SM_TIME_DEFAULT, i);
				break;
		p--;
	*p = '\0';
	auto time_t t;
	(void) time(&t);
	register char *s;
	register char *s;
	register int i = 0;
		i = (i << 3) | (*s++ - '0');
	BITMAP256 a;
	BITMAP256 b;
	int i;
	BITMAP256 map;
	int i;
	bool icase;
	register char *a;
	register char *b;
	int la;
	int lb;
	int c;
	la = strlen(a);
	lb = strlen(b);
	c = *a;
		c = tolower(c);
				continue;
				continue;
	char *where;
	register int i;
		fill_fd(i, where);
	int fd;
	char *where;
		syserr("checkfdopen(%d): %s not open as expected!", fd, where);
		printopenfds(true);
	char *where;
	int maxfd;
	register int fd;
	bool printhdr = true;
	int save_errno = errno;
	static BITMAP256 baseline;
	extern int DtableSize;
		maxfd = BITMAPBITS;
		maxfd = DtableSize;
		clrbitmap(baseline);
				continue;
			clrbitn(fd, baseline);
			setbitn(fd, baseline);
			continue;
			continue;
				  where);
			printhdr = false;
		dumpfd(fd, true, true);
	errno = save_errno;
	bool logit;
	register int fd;
	extern int DtableSize;
		dumpfd(fd, false, logit);
	int fd;
	bool printclosed;
	bool logit;
	register char *p;
	char *hp;
	SOCKADDR sa;
	auto SOCKADDR_LEN_T slen;
	int i;
	char buf[200];
	p = buf;
	(void) sm_snprintf(p, SPACELEFT(buf, p), "%3d: ", fd);
	p += strlen(p);
				sm_errstring(errno));
			goto printit;
			(void) sm_snprintf(p, SPACELEFT(buf, p), "CLOSED");
			goto printit;
	i = fcntl(fd, F_GETFL, 0);
		(void) sm_snprintf(p, SPACELEFT(buf, p), "fl=0x%x, ", i);
		p += strlen(p);
			(int) st.st_mode);
	p += strlen(p);
		(void) sm_snprintf(p, SPACELEFT(buf, p), "SOCK ");
		p += strlen(p);
		memset(&sa, '\0', sizeof(sa));
		slen = sizeof(sa);
				 sm_errstring(errno));
			hp = hostnamebyanyaddr(&sa);
					"%s/%d", hp, ntohs(sa.sin.sin_port));
					"%s/%d", hp, ntohs(sa.sin6.sin6_port));
					"%s", hp);
		p += strlen(p);
		(void) sm_snprintf(p, SPACELEFT(buf, p), "->");
		p += strlen(p);
		slen = sizeof(sa);
					sm_errstring(errno));
			hp = hostnamebyanyaddr(&sa);
					"%s/%d", hp, ntohs(sa.sin.sin_port));
					"%s/%d", hp, ntohs(sa.sin6.sin6_port));
					"%s", hp);
		break;
		(void) sm_snprintf(p, SPACELEFT(buf, p), "CHR: ");
		p += strlen(p);
		goto defprint;
		(void) sm_snprintf(p, SPACELEFT(buf, p), "BLK: ");
		p += strlen(p);
		goto defprint;
		(void) sm_snprintf(p, SPACELEFT(buf, p), "FIFO: ");
		p += strlen(p);
		goto defprint;
		(void) sm_snprintf(p, SPACELEFT(buf, p), "DIR: ");
		p += strlen(p);
		goto defprint;
		(void) sm_snprintf(p, SPACELEFT(buf, p), "LNK: ");
		p += strlen(p);
		goto defprint;
			 (int) st.st_gid);
		p += strlen(p);
			 (ULONGLONG_T) st.st_size);
		break;
			  "%.800s", buf);
		sm_dprintf("%s\n", buf);
	char host[];
	register char *p;
	char *mydom;
	int i;
	bool canon = false;
	i = strlen(host);
	p = &host[(i == 0) ? 0 : i - 1];
		*p = '\0';
		canon = true;
	p = strchr(host, '.');
	mydom = macvalue('m', CurEnv);
		mydom = "";
	i = strlen(++p);
		*--p = '\0';
	char **argv;
	int *pfd;
	ENVELOPE *e;
	pid_t pid;
	int save_errno;
	int sff;
	int ret;
	int fdv[2];
	char *p, *q;
	char buf[MAXPATHLEN];
	extern int DtableSize;
		(void) close(fdv[0]);
		(void) close(fdv[1]);
		(void) close(fdv[1]);
		*pfd = fdv[0];
	RestartRequest = NULL;
	RestartWorkGroup = false;
	ShutdownRequest = NULL;
	PendingSignal = 0;
	CurrentPid = getpid();
	sm_exc_newthread(fatal_error);
	(void) close(0);
	(void) close(fdv[0]);
		_exit(EX_OSERR);
	(void) close(fdv[1]);
		int xfd;
		xfd = sm_io_getinfo(e->e_xfp, SM_IO_WHAT_FD, NULL);
			_exit(EX_OSERR);
		int fd;
		fd = sm_io_getinfo(e->e_lockfp, SM_IO_WHAT_FD, NULL);
			(void) close(fd);
			syserr("%s: lockfp does not have a fd", argv[0]);
		expand(ProgMailer->m_rootdir, buf, sizeof(buf), e);
			syserr("prog_open: cannot chroot(%s)", buf);
			exit(EX_TEMPFAIL);
			syserr("prog_open: cannot chdir(/)");
			exit(EX_TEMPFAIL);
	endpwent();
	sm_mbdb_terminate();
	(void) sm_memstat_close();
		syserr("prog_open: setgid(%ld) failed", (long) DefGid);
		exit(EX_TEMPFAIL);
		syserr("prog_open: setuid(%ld) failed", (long) DefUid);
		exit(EX_TEMPFAIL);
		p = ProgMailer->m_execdir;
		p = NULL;
		q = strchr(p, ':');
			*q = '\0';
		expand(p, buf, sizeof(buf), e);
			*q++ = ':';
			break;
			(void) chdir("/");
	sff = SFF_ROOTOK|SFF_EXECOK;
		sff |= SFF_NOGWFILES|SFF_NOWWFILES;
		sff |= SFF_NOPATHCHECK;
		sff |= SFF_SAFEDIRPATH;
	ret = safefile(argv[0], DefUid, DefGid, DefUser, sff, 0, NULL);
			  argv[0], sm_errstring(ret));
	sm_close_on_exec(STDERR_FILENO + 1, DtableSize);
	(void) execve(argv[0], (ARGV_T) argv, (ARGV_T) UserEnviron);
	save_errno = errno;
	syserr("%s: cannot exec", argv[0]);
		_exit(EX_OSERR);
	_exit(EX_CONFIG);
	char line[];
	int col;
	int delim;
	char buf[];
	int buflen;
	char *p;
	char *begin, *end;
	int i;
	char delimbuf[4];
		(void) sm_strlcpy(delimbuf, "\n\t ", sizeof(delimbuf));
		delimbuf[0] = (char) delim;
		delimbuf[1] = '\0';
	p = line;
	begin = line;
			begin++;
		begin++;
				begin++;
	end = strpbrk(begin, delimbuf);
		i = strlen(begin);
		i = end - begin;
		i = buflen - 1;
	(void) sm_strlcpy(buf, begin, i + 1);
	register char *t;
	register char *f;
	int l;
	(void) denlstring(f, true, true);
		syserr("!cleanstrcpy: length == 0");
	l--;
		f++;
	*t = '\0';
	char *s;
	bool strict;
	bool logattacks;
	register char *p;
	int l;
	static char *bp = NULL;
	static int bl = 0;
	p = s;
			break;
	l = strlen(s) + 1;
		char *nbp = sm_pmalloc_x(l);
			sm_free(bp);
		bp = nbp;
		bl = l;
	(void) sm_strlcpy(bp, s, l);
		*p++ = ' ';
			  shortenstring(bp, MAXSHORTSTR));
	char *s;
	int c;
	bool ok;
	ok = true;
			*s = c;
			ok = false;
		++s;
	char *pathname;
	bool createflag;
		errno = ENOTDIR;
		errno = EACCES;
	pid_t		proc_pid;
	char		*proc_task;
	int		proc_type;
	int		proc_count;
	int		proc_other;
	SOCKADDR	proc_hostaddr;
};
static PROCS_T	*volatile ProcListVec = NULL;
static int	ProcListSize = 0;
	pid_t pid;
	char *task;
	int type;
	int count;
	int other;
	SOCKADDR *hostaddr;
	int i;
			break;
		proc_list_probe();
				break;
		int chldwasblocked;
		PROCS_T *npv;
		SM_ASSERT(ProcListSize < INT_MAX - PROC_LIST_SEG);
					       (ProcListSize + PROC_LIST_SEG));
		chldwasblocked = sm_blocksignal(SIGCHLD);
				ProcListSize * sizeof(PROCS_T));
			sm_free(ProcListVec);
			npv[i].proc_pid = NO_PID;
			npv[i].proc_task = NULL;
			npv[i].proc_type = PROC_NONE;
		i = ProcListSize;
		ProcListSize += PROC_LIST_SEG;
		ProcListVec = npv;
			(void) sm_releasesignal(SIGCHLD);
	ProcListVec[i].proc_pid = pid;
	PSTRSET(ProcListVec[i].proc_task, task);
	ProcListVec[i].proc_type = type;
	ProcListVec[i].proc_count = count;
	ProcListVec[i].proc_other = other;
		ProcListVec[i].proc_hostaddr = *hostaddr;
			sizeof(ProcListVec[i].proc_hostaddr));
		SM_ASSERT(CurChildren < INT_MAX);
		CurChildren++;
	pid_t pid;
	char *task;
	int i;
			PSTRSET(ProcListVec[i].proc_task, task);
			break;
	pid_t pid;
	int st;
	int *other;
	int i;
	int type = PROC_NONE;
			ProcListVec[i].proc_pid = NO_PID;
			type = ProcListVec[i].proc_type;
				*other = ProcListVec[i].proc_other;
				CurChildren--;
			break;
			RestartRequest = "control socket";
			ShutdownRequest = "control socket";
		mark_work_group_restart(ProcListVec[i].proc_other, st);
		CurRunners -= ProcListVec[i].proc_count;
	int i;
		ProcListVec[i].proc_pid = NO_PID;
	CurChildren = 0;
	int i, children;
	int chldwasblocked;
	pid_t pid;
	children = 0;
	chldwasblocked = sm_blocksignal(SIGCHLD);
		pid = ProcListVec[i].proc_pid;
			continue;
					  (int) ProcListVec[i].proc_pid);
			ProcListVec[i].proc_pid = NO_PID;
			SM_FREE_CLR(ProcListVec[i].proc_task);
			CurChildren--;
			++children;
		CurChildren = 0;
		(void) sm_releasesignal(SIGCHLD);
			  children, CurChildren);
	SM_FILE_T *out;
	char *prefix;
	int i;
			continue;
				      OpMode == MD_ARPAFTP) ? "\r" : "");
	int type;
	int signal;
	int chldwasblocked;
	int alrmwasblocked;
	int i;
	pid_t mypid = getpid();
	chldwasblocked = sm_blocksignal(SIGCHLD);
	alrmwasblocked = sm_blocksignal(SIGALRM);
			continue;
			continue;
		(void) kill(ProcListVec[i].proc_pid, signal);
		(void) sm_releasesignal(SIGALRM);
		(void) sm_releasesignal(SIGCHLD);
	SOCKADDR *hostaddr;
	int i, n;
	n = 1;
			continue;
			continue;
			n++;
			n++;
static char 	PROG_FMT[] = "%s: ";
static char 	ERR_FMT[] = ": %s";
static char 	*progname;
	progname = basename(name);
	int 	err = errno;
	va_list alist;
		(void) fprintf(stderr, PROG_FMT, progname);
	va_end(alist);
		(void) fprintf(stderr, ERR_FMT, strerror(err));
	int	cpuid;
	int	max, cpus_conf;
	uint_t	ncpus = 0;
	max 		= sysconf(_SC_CPUID_MAX);
	cpus_conf	= sysconf(_SC_NPROCESSORS_CONF);
		max = cpus_conf = 1;
			g_cpu_table[ncpus] = cpuid;
			ncpus++;
	(void) fprintf(stderr, "%s   %s\n\n", TITLE, COPYRIGHT_INTEL);
	(void) fprintf(stderr, "usage: powertop [option]\n");
	    "times and print list of top offenders\n");
	    "data in seconds [1-30s]\n");
	    "kernel cyclic activity\n");
	    " CPU\n");
	    "message\n");
	char	buf[BIT_DEPTH_BUF];
	event_info_t i = *((event_info_t *)p1);
	event_info_t j = *((event_info_t *)p2);
char *s1;
char *s2;
   int s2len;
   int i;
   char *temp_ptr;
   temp_ptr = s1;
        temp_ptr += 1;
char *token_in;
int  *must_be_first_char;
int  *num_tokens;
char *tokens_out;
    int i, j;
    int token_count = 0;
    i = 0;
    j = 0;
    *must_be_first_char = 1;
	*must_be_first_char = 0;
	*num_tokens = 0;
    token_count++;
	tokens_out[j] = token_in[i];
        j++;
	i++;
	tokens_out[j] = '\0';
	*num_tokens = token_count;
	   token_count++;
	   tokens_out[j] = '\t';
	   tokens_out[j] = token_in[i];
	i++;
	j++;
    tokens_out[j] = '\0';
	token_count--;
	tokens_out[j - 1] = '\0';
    *num_tokens = token_count;
int must_be_first_character;
char *search_field;
krb5_db_entry *chk_entry;
int num_tokens;
int type;
    char token1[256];
    char *found1;
    char token2[256];
    char *found2;
    char token3[256];
    char *found3;
    char *local_entry;
    local_entry = chk_entry->princ->data[type].data;
    token1[0] = token2[0] = token3[0] = '\0';
    (void) sscanf(search_field, "%s\t%s\t%s", token1, token2, token3);
    found1 = strstr(local_entry, token1);
	found2 = strstr(local_entry, token2);
	found3 = strstr(local_entry, token3);
	gchar *c = NULL;
	c = strrchr (path, '/');
		HAL_WARNING (("Invalid path %s", path));
		*c = '\0';
	int len;
	const gchar *p;
	len = strlen (s);
	guint i;
	guint len;
	gchar *parent_path;
	parent_path = g_strndup (path, HAL_PATH_MAX);
	len = strlen (parent_path);
		parent_path[i] = '\0';
	parent_path[i] = '\0';
	int len1;
	int len2;
	const gchar *p1;
	const gchar *p2;
	gchar buf[HAL_PATH_MAX];
	len1 = strlen (path1);
	len2 = strlen (path2);
	p1 = path1 + len1;
	p2 = path2;
		p2 += 3;
			;
	strncpy (buf, path1, (p1-path1));
	buf[p1-path1] = '\0';
	FILE *f;
	char buf[64];
	gchar path[HAL_PATH_MAX];
	gboolean ret;
	f = NULL;
	ret = FALSE;
	g_snprintf (path, sizeof (path), "%s/%s", directory, file);
	f = fopen (path, "rb");
		HAL_ERROR (("Cannot open '%s'", path));
		goto out;
		HAL_ERROR (("Cannot read from '%s'", path));
		goto out;
	*result = strtol (buf, NULL, base);
	ret = TRUE;
		fclose (f);
	gint value;
	gboolean ret;
	ret = FALSE;
		ret = hal_device_property_set_int (d, key, value);
	FILE *f;
	char buf[64];
	gchar path[HAL_PATH_MAX];
	gboolean ret;
	f = NULL;
	ret = FALSE;
	g_snprintf (path, sizeof (path), "%s/%s", directory, file);
	f = fopen (path, "rb");
		HAL_ERROR (("Cannot open '%s'", path));
		goto out;
		HAL_ERROR (("Cannot read from '%s'", path));
		goto out;
	*result = strtoll (buf, NULL, base);
	ret = TRUE;
		fclose (f);
	guint64 value;
	gboolean ret;
	ret = FALSE;
		ret = hal_device_property_set_uint64 (d, key, value);
	FILE *f;
	char buf[64];
	gchar path[HAL_PATH_MAX];
	gboolean ret;
	gint digit;
	gint left, right;
	gboolean passed_white_space;
	gint num_prec;
	gsize len;
	gchar c;
	guint i;
	f = NULL;
	ret = FALSE;
	g_snprintf (path, sizeof (path), "%s/%s", directory, file);
	f = fopen (path, "rb");
		HAL_ERROR (("Cannot open '%s'", path));
		goto out;
		HAL_ERROR (("Cannot read from '%s'", path));
		goto out;
	left = 0;
	len = strlen (buf);
	passed_white_space = FALSE;
				break;
				continue;
		passed_white_space = TRUE;
		left *= 16;
		c = buf[i];
		digit = (int) (c - '0');
		left += digit;
	i++;
	right = 0;
	num_prec = 0;
			break;
			break;
		right *= 16;
		c = buf[i];
		digit = (int) (c - '0');
		right += digit;
		num_prec++;
		right *= 16;
	*result = left * 256 + (right & 255);
	ret = TRUE;
		fclose (f);
	gint value;
	gboolean ret;
	ret = FALSE;
		ret = hal_device_property_set_int (d, key, value);
	FILE *f;
	static gchar buf[256];
	gchar path[HAL_PATH_MAX];
	gchar *result;
	gsize len;
	gint i;
	f = NULL;
	result = NULL;
	g_snprintf (path, sizeof (path), "%s/%s", directory, file);
	f = fopen (path, "rb");
		HAL_ERROR (("Cannot open '%s'", path));
		goto out;
	buf[0] = '\0';
		HAL_ERROR (("Cannot read from '%s'", path));
		goto out;
	len = strlen (buf);
		buf[len-1] = '\0';
			break;
		buf[i] = '\0';
	result = buf;
		fclose (f);
	gchar *buf;
	gboolean ret;
	ret = FALSE;
		ret = hal_device_property_set_string (d, key, buf);
	guint i;
	va_list args;
	gchar buf[256];
	va_end (args);
		    "1234567890", '_');
	g_strlcpy (dst, buf, dstsize);
		goto out;
		g_snprintf (dst, dstsize, "%s_%d", buf, i);
			goto out;
	;
	gchar *p;
	p = strrchr (path, '/');
	*p = '\0';
static gboolean _grep_can_reuse = FALSE;
	_grep_can_reuse = FALSE;
	static gchar buf[2048];
	static unsigned int bufsize;
	static gchar filename[HAL_PATH_MAX];
	static gchar oldfilename[HAL_PATH_MAX];
	gchar *result;
	gsize linestart_len;
	gchar *p;
	result = NULL;
		snprintf (filename, sizeof (filename), "%s/%s", directory, file);
		strncpy (filename, directory, sizeof (filename));
		FILE *f;
		f = fopen (filename, "r");
			goto out;
		bufsize = fread (buf, sizeof (char), sizeof (buf) - 1, f);
		buf[bufsize] = '\0';
		fclose (f);
	_grep_can_reuse = TRUE;
	strncpy (oldfilename, filename, sizeof(oldfilename));
	linestart_len = strlen (linestart);
	p = buf;
		unsigned int linelen;
		static char line[256];
			;
			strncpy (line, p, linelen);
			line[linelen] = '\0';
				result = line + linestart_len;
				goto out;
		p += linelen + 1;
	gchar *line;
	gchar *res;
	static gchar buf[256];
	gchar **tokens;
	guint i, j;
	res = NULL;
	tokens = NULL;
		goto out;
	tokens = g_strsplit_set (line, " \t:", 0);
			continue;
			strncpy (buf, tokens[i], sizeof (buf));
			res = buf;
			goto out;
		j++;
		g_strfreev (tokens);
	gchar *endptr;
	gchar *strvalue;
	int value;
	value = G_MAXINT;
	strvalue = hal_util_grep_string_elem_from_file (directory, file, linestart, elem, reuse);
		goto out;
	value = strtol (strvalue, &endptr, base);
		value = G_MAXINT;
		goto out;
	gboolean res;
	gchar *value;
	res = FALSE;
		goto out;
	res = hal_device_property_set_string (d, key, value);
	gchar *endptr;
	gboolean res;
	gchar *strvalue;
	int value;
	res = FALSE;
	strvalue = hal_util_grep_string_elem_from_file (directory, file, linestart, elem, reuse);
		goto out;
	value = strtol (strvalue, &endptr, base);
		goto out;
	res = hal_device_property_set_int (d, key, value);
	gchar *line;
	gboolean res;
	gchar **tokens;
	guint i, j;
	res = FALSE;
	tokens = NULL;
		goto out;
	tokens = g_strsplit_set (line, " \t:", 0);
			continue;
			hal_device_property_set_bool (d, key, strcmp (tokens[i], expected) == 0);
			res = TRUE;
			goto out;
		j++;
		g_strfreev (tokens);
	guint j;
	guint len;
	gchar **strv;
	GSList *i;
	len = g_slist_length (strlist);
	strv = g_new (char *, len + 1);
		strv[j] = g_strdup ((const gchar *) i->data);
	strv[j] = NULL;
	HalDevice *d;
	gchar **programs;
	gchar **extra_env;
	guint next_program;
	HalCalloutsDone callback;
	gpointer userdata1;
	gpointer userdata2;
} Callout;
static void callout_do_next (Callout *c);
	Callout *c;
	c = (Callout *) data1;
	callout_do_next (c);
		HalDevice *d;
		gpointer userdata1;
		gpointer userdata2;
		HalCalloutsDone callback;
		d = c->d;
		userdata1 = c->userdata1;
		userdata2 = c->userdata2;
		callback = c->callback;
		g_strfreev (c->programs);
		g_strfreev (c->extra_env);
		g_free (c);
		callback (d, userdata1, userdata2);
                    (gpointer)c, NULL);
		c->next_program++;
	Callout *c;
	c = g_new0 (Callout, 1);
	c->d = d;
	c->callback = callback;
	c->userdata1 = userdata1;
	c->userdata2 = userdata2;
	c->programs = hal_util_dup_strv_from_g_slist (programs);
	c->extra_env = g_strdupv (extra_env);
	c->next_program = 0;
	callout_do_next (c);
	GSList *programs;
	gchar *extra_env[2] = {"HALD_ACTION=add", NULL};
		callback (d, userdata1, userdata2);
		goto out;
	hal_callout_device (d, callback, userdata1, userdata2, programs, extra_env);
	;
	GSList *programs;
	gchar *extra_env[2] = {"HALD_ACTION=remove", NULL};
		callback (d, userdata1, userdata2);
		goto out;
	hal_callout_device (d, callback, userdata1, userdata2, programs, extra_env);
	;
	GSList *programs;
	gchar *extra_env[2] = {"HALD_ACTION=preprobe", NULL};
		callback (d, userdata1, userdata2);
		goto out;
	hal_callout_device (d, callback, userdata1, userdata2, programs, extra_env);
	;
	char *endchar;
	char *newstr;
	unsigned int count = 0;
	newstr = g_strdup (str);
		*endchar = '?';
		count++;
	(void) printf ("Dumping %d=0x%x bytes\n", size, size);
	(void) hexdump_file(mem, size, HDF_DEFAULT, stdout);
	int i;
	FILE *hal_mtab;
	int hal_mtab_len;
	int num_read;
	char *hal_mtab_buf;
	char **lines;
	gboolean found;
	hal_mtab = NULL;
	hal_mtab_buf = NULL;
	found = FALSE;
	hal_mtab = fopen ("/media/.hal-mtab", "r");
		HAL_ERROR (("Cannot open /media/.hal-mtab"));
		goto out;
		HAL_ERROR (("Cannot seek to end of /media/.hal-mtab"));
		goto out;
	hal_mtab_len = ftell (hal_mtab);
		HAL_ERROR (("Cannot determine size of /media/.hal-mtab"));
		goto out;
	rewind (hal_mtab);
	hal_mtab_buf = g_new0 (char, hal_mtab_len + 1);
	num_read = fread (hal_mtab_buf, 1, hal_mtab_len, hal_mtab);
		HAL_ERROR (("Cannot read from /media/.hal-mtab"));
		goto out;
	fclose (hal_mtab);
	hal_mtab = NULL;
	lines = g_strsplit (hal_mtab_buf, "\n", 0);
	g_free (hal_mtab_buf);
	hal_mtab_buf = NULL;
		char **line_elements;
			continue;
		line_elements = g_strsplit (lines[i], "\t", 6);
			HAL_DEBUG (("  devfile     = '%s'", line_elements[0]));
			HAL_DEBUG (("  uid         = '%s'", line_elements[1]));
			HAL_DEBUG (("  session id  = '%s'", line_elements[2]));
			HAL_DEBUG (("  fs          = '%s'", line_elements[3]));
			HAL_DEBUG (("  options     = '%s'", line_elements[4]));
			HAL_DEBUG (("  mount_point = '%s'", line_elements[5]));
			HAL_DEBUG (("  (comparing against '%s')", mount_point));
				found = TRUE;
		g_strfreev (line_elements);
	g_strfreev (lines);
		fclose (hal_mtab);
		g_free (hal_mtab_buf);
	GSList *children;
	GSList *i;
	HalDevice *d;
		hal_device_property_set_bool (root, "info.claimed", claimed);
		hal_device_property_set_string (root, "info.claimed.service", service);
		hal_device_property_set_int (root, "info.claimed.uid", uid);
		hal_device_property_remove (root, "info.claimed");
		hal_device_property_remove (root, "info.claimed.service");
		hal_device_property_remove (root, "info.claimed.uid");
	    "info.parent", root->udi);
		d = HAL_DEVICE (i->data);
		hal_util_branch_claim (store, d, claimed, service, uid);
	g_slist_free (children);
extern log_class_t g_verbose;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
	int error = errno;
	dm_assert(pthread_mutex_lock(&log_mutex) == 0);
	fmd_hdl_vdebug(g_fm_hdl, fmt, ap);
		fmd_hdl_debug(g_fm_hdl, ": %s\n", strerror(error));
	dm_assert(pthread_mutex_unlock(&log_mutex) == 0);
	int error = errno;
	dm_assert(pthread_mutex_lock(&log_mutex) == 0);
	fmd_hdl_debug(g_fm_hdl, "WARNING: ");
	fmd_hdl_vdebug(g_fm_hdl, fmt, ap);
		fmd_hdl_debug(g_fm_hdl, ": %s\n", strerror(error));
	dm_assert(pthread_mutex_unlock(&log_mutex) == 0);
	dm_assert(pthread_mutex_lock(&log_mutex) == 0);
	fmd_hdl_debug(g_fm_hdl, "WARNING: ");
	fmd_hdl_vdebug(g_fm_hdl, fmt, ap);
	dm_assert(pthread_mutex_unlock(&log_mutex) == 0);
	int error = errno;
	dm_assert(pthread_mutex_lock(&log_mutex) == 0);
	fmd_hdl_vdebug(g_fm_hdl, fmt, ap);
		fmd_hdl_debug(g_fm_hdl, ": %s\n", strerror(error));
	dm_assert(pthread_mutex_unlock(&log_mutex) == 0);
	va_list ap;
	dm_assert(pthread_mutex_lock(&log_mutex) == 0);
	va_start(ap, fmt);
	fmd_hdl_vdebug(g_fm_hdl, fmt, ap);
	va_end(ap);
	dm_assert(pthread_mutex_unlock(&log_mutex) == 0);
	va_list ap;
	va_start(ap, fmt);
	verror(fmt, ap);
	va_end(ap);
	va_list ap;
	va_start(ap, fmt);
	vwarn(fmt, ap);
	va_end(ap);
	va_list ap;
	va_start(ap, fmt);
	vwarn_e(fmt, ap);
	va_end(ap);
	fmd_hdl_free(g_fm_hdl, p, sz);
	fmd_hdl_strfree(g_fm_hdl, s);
	    (struct q_node *)qp->nalloc(sizeof (struct q_node));
	qnp->data = data;
	qnp->next = NULL;
	dm_assert(pthread_mutex_lock(&qp->mutex) == 0);
		qp->nodep = qnp;
		nodep = qp->nodep;
			nodep = nodep->next;
		nodep->next = qnp;
		dm_assert(pthread_cond_broadcast(&qp->cvar) == 0);
	dm_assert(pthread_mutex_unlock(&qp->mutex) == 0);
	void *rv = NULL;
	dm_assert(pthread_mutex_lock(&qp->mutex) == 0);
		(void) pthread_cond_wait(&qp->cvar, &qp->mutex);
		rv = qp->nodep->data;
		nextnode = qp->nodep->next;
		qp->nfree(qp->nodep, sizeof (struct q_node));
		qp->nodep = nextnode;
	dm_assert(pthread_mutex_unlock(&qp->mutex) == 0);
	qu_t *newqp = (qu_t *)dmalloc(sizeof (qu_t));
	newqp->boe = block_on_empty;
	newqp->nalloc = nodealloc;
	newqp->nfree = nodefree;
	newqp->data_dealloc = data_deallocator;
	dm_assert(pthread_mutex_init(&newqp->mutex, NULL) == 0);
	dm_assert(pthread_cond_init(&newqp->cvar, NULL) == 0);
	newqp->nodep = NULL;
	qu_t *qp = *qpp;
	void *item;
	dm_assert(pthread_mutex_destroy(&qp->mutex) == 0);
	dm_assert(pthread_cond_destroy(&qp->cvar) == 0);
	qp->boe = B_FALSE;
		qp->data_dealloc(item);
	dm_assert(qp->nodep == NULL);
	dfree(qp, sizeof (qu_t));
	*qpp = NULL;
		    line, func);
		    "%s, file: %s, line: %d", assertion, file, line);
} si_msg_type_t;
};
nfds_t		num_pollfds;
boolean_t	syslog_open = B_FALSE;
boolean_t	debug_enabled = B_FALSE;
	openlog(SYSLOG_IDENT, LOG_PID|LOG_CONS, LOG_DAEMON);
	syslog_open = B_TRUE;
	syslog_open = B_FALSE;
	closelog();
	char		buf[MSG_BUF_SIZE];
		syslog(LOG_ERR, "%s", buf);
	va_list ap;
	closelog();
	openlog(SYSLOG_IDENT, LOG_PID, LOG_DAEMON);
	va_end(ap);
	closelog();
	openlog(SYSLOG_IDENT, LOG_PID|LOG_CONS, LOG_DAEMON);
	va_list ap;
	va_end(ap);
	va_list ap;
	va_end(ap);
		free(poll_fds);
		poll_fds = NULL;
	nfds_t n;
	int		i;
	p = find_pollfd(fd);
		poll_fds = p;
			poll_fds[num_pollfds + i].fd = -1;
		p = &poll_fds[num_pollfds];
		num_pollfds += POLLFDS_GROWTH_SIZE;
	p->fd = fd;
	p->events = events;
	p->revents = 0;
		p->fd = -1;
		p->events = 0;
		p->revents = 0;
	int	ret;
	size_t  cnt = 0;
	char    *cp = (char *)buf;
				    strerror(errno));
			break;
			cnt += ret;
	    (inst->copies >= inst->config->basic->max_copies));
	(void) iu_cancel_timer(timer_queue, inst->timer_id, NULL);
	inst->timer_id = -1;
	(void) iu_cancel_timer(timer_queue, inst->bind_timer_id, NULL);
	inst->bind_timer_id = -1;
	int flags = fcntl(fd, F_GETFL, 0);
	(void) fcntl(fd, F_SETFL, (flags & ~O_NONBLOCK));
	int flags = fcntl(fd, F_GETFL, 0);
	(void) fcntl(fd, F_SETFL, (flags | O_NONBLOCK));
static	boolean_t	is_fqdn(const char *);
			    dhcp_smach_t *dsmp);
	};
	};
	time_t	time = monosec_to_time(monosec);
	char	*time_string = ctime(&time);
	time_string[strlen(time_string) - 1] = '\0';
		    dsmp->dsm_name, msglen, msg);
	int exitval;
		exitval = EXIT_SUCCESS;
		exitval = EXIT_FAILURE;
	_exit(exitval);
		(void) signal(SIGALRM, alrm_exit);
			(void) signal(SIGALRM, SIG_DFL);
		(void) setsid();
			(void) signal(SIGHUP, SIG_IGN);
			(void) chdir("/");
			(void) umask(022);
			closefrom(0);
			break;
			_exit(EXIT_SUCCESS);
		break;
			(void) signal(SIGCHLD, SIG_IGN);
				    "adoption.");
			    "wait finished");
		_exit(EXIT_SUCCESS);
	} rtmsg;
	(void) memset(&rtmsg, 0, sizeof (rtmsg));
	rtmsg.rm_mh.rtm_version = RTM_VERSION;
	rtmsg.rm_mh.rtm_msglen	= sizeof (rtmsg);
	rtmsg.rm_mh.rtm_type	= type;
	rtmsg.rm_mh.rtm_pid	= getpid();
	rtmsg.rm_mh.rtm_flags	= RTF_GATEWAY | RTF_STATIC | flags;
	rtmsg.rm_mh.rtm_addrs	= RTA_GATEWAY | RTA_DST | RTA_NETMASK | RTA_IFP;
	rtmsg.rm_gw.sin_family	= AF_INET;
	rtmsg.rm_gw.sin_addr	= *gateway_nbo;
	rtmsg.rm_dst.sin_family = AF_INET;
	rtmsg.rm_dst.sin_addr.s_addr = htonl(INADDR_ANY);
	rtmsg.rm_mask.sin_family = AF_INET;
	rtmsg.rm_mask.sin_addr.s_addr = htonl(0);
	dhcpmsg(MSG_VERBOSE, "inactivity_shutdown: timed out");
	iu_stop_handling_events(eh, DHCP_REASON_INACTIVITY, NULL, NULL);
	    DHCP_REASON_SIGNAL), drain_script, NULL);
	int			on = 1;
	(void) memset(&sin, 0, sizeof (struct sockaddr_in));
	sin.sin_family = AF_INET;
	sin.sin_port   = htons(port_hbo);
	sin.sin_addr.s_addr = htonl(addr_hbo);
	(void) setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (int));
	int			on = 1;
	(void) memset(&sin6, 0, sizeof (struct sockaddr_in6));
	sin6.sin6_family = AF_INET6;
	sin6.sin6_port   = htons(port_hbo);
		    sizeof (sin6.sin6_addr));
	(void) setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (int));
	FILE		*fp;
	fp = fopen(path, "r");
		char *p;
				(void) fclose(fp);
				(void) fclose(fp);
				char *nlptr;
				(void) fclose(fp);
					p++;
					*nlptr = '\0';
					    " host name too long");
					    " host name not valid");
				(void) fclose(fp);
	(void) fclose(fp);
	dt->dt_id = -1;
	dt->dt_start = startval;
		dt->dt_id = -1;
	dt->dt_id = iu_schedule_timer(tq, dt->dt_start, cbfunc, arg);
	uint16_t status;
	};
	static char sbuf[32];
	*estr = "";
	*msg = "";
	*msglenp = 0;
	olen -= sizeof (*d6o);
		*estr = "garbled status code";
	*msg = (const char *)(d6o + 1) + 2;
	*msglenp = olen - 2;
	(void) memcpy(&status, d6o + 1, sizeof (status));
	status = ntohs(status);
			    status);
			*estr = sbuf;
			*estr = v6_status[status];
	PKT_LIST *plp[2];
	const char *hcfile;
	plp[0] = dsmp->dsm_ack;
	plp[1] = dsmp->dsm_orig_ack;
		dhcpmsg(MSG_DEBUG, "wrote lease to %s", hcfile);
		    "system; not saving lease", hcfile);
		    "not use cached configuration)", hcfile);
 *   input: const char *: file name;
 *	    char *: allocated buffer space;
 *	    size_t: space available in buf;
	char	value[SYS_NMLN], *c;
	int	fd, i;
		    filename);
		*buf = '\0';
			    filename);
			*buf = '\0';
			value[i] = '\0';
				*c = '\0';
				*c = '\0';
				    " long value, %s", value);
				*buf = '\0';
		(void) close(fd);
 *   input: char *: allocated buffer space;
 *	    size_t: space available in buf;
 *   input: dhcp_pkt_t *: pointer to DHCP message being constructed;
 *	    dhcp_smach_t *: pointer to interface DHCP state machine;
	const char	*reqhost;
	char		nodename[MAXNAMELEN];
	dhcpmsg(MSG_DEBUG, "dhcp_add_hostname_opt: DF_REQUEST_HOSTNAME");
		free(dsmp->dsm_reqhost);
		    dsmp->dsm_reqhost, dsmp->dsm_name);
		    strlen(dsmp->dsm_reqhost));
		    dsmp->dsm_name);
 *   input: dhcp_pkt_t *: pointer to DHCP message being constructed;
 *	    dhcp_smach_t *: pointer to interface DHCP state machine;
	 * address) DNS updates;
	 * be used by clients ....";
	const uint8_t	S_BIT_POS = 7;
	const uint8_t	E_BIT_POS = 5;
	const uint8_t	S_BIT = 1 << (7 - S_BIT_POS);
	const uint8_t	E_BIT = 1 << (7 - E_BIT_POS);
	const size_t	OPT_FQDN_METALEN = 3;
	char		fqdnbuf[MAXNAMELEN];
	uchar_t		enc_fqdnbuf[MAXNAMELEN];
	uint8_t		fqdnopt[MAXNAMELEN + OPT_FQDN_METALEN];
	uint_t		fqdncode;
	size_t		len, metalen;
		    " name %s", fqdnbuf);
	bzero(fqdnopt, sizeof (fqdnopt));
	fqdncode = CD_CLIENTFQDN;
	metalen = OPT_FQDN_METALEN;
	*fqdnopt = S_BIT | E_BIT;
	(void) memcpy(fqdnopt + metalen, enc_fqdnbuf, len);
	(void) add_pkt_opt(dpkt, fqdncode, fqdnopt, metalen + len);
 *   input: char *: pointer to buffer to which domain name will be written;
 *	    size_t length of buffer;
 *	    dhcp_smach_t *: pointer to interface DHCP state machine;
	const char		*domainname;
	int			lasterrno;
	domainname = dsmp->dsm_dhcp_domainname;
		bzero(&res_state, sizeof (struct __res_state));
			    " initializing resolver", lasterrno);
		domainname = NULL;
			domainname = res_state.defdname;
		res_ndestroy(&res_state);
 *   input: char *: pointer to buffer to which domain name will be written;
 *	    size_t length of buffer;
 *	    dhcp_smach_t *: pointer to interface DHCP state machine;
	const char	*domainname;
	    DF_DNS_DOMAINNAME);
			    dsmp->dsm_name);
 *   input: char *: pointer to buffer to which FQDN will be written;
 *	    size_t length of buffer;
 *	    dhcp_smach_t *: pointer to interface DHCP state machine;
	char		nodename[MAXNAMELEN], *reqhost;
	size_t		pos, len;
	dhcpmsg(MSG_DEBUG, "dhcp_assemble_fqdn: DF_REQUEST_FQDN");
	bzero(fqdnbuf, buflen);
	reqhost = dsmp->dsm_msg_reqhost;
		    dsmp->dsm_name);
		char		domainname[MAXNAMELEN];
		size_t		needdots;
			    dsmp->dsm_name);
		len = strlen(domainname);
		needdots = 1 + (domainname[len - 1] != '.');
			    dsmp->dsm_name);
		fqdnbuf[pos++] = '.';
		pos += len;
 *   input: const char *: the hostname to inspect;
 *	    criteria above; otherwise, B_FALSE;
	const char *c;
	size_t i;
	i = strlen(hostname);
	c = hostname;
	i = 0;
		++i;
		++c;
 *   input: char *: NULL or a null-terminated string;
		char	*sp;
		sp = strchr(value, ' ');
			*sp = '\0';
 *   input: dhcp_smach_t *: the state machine REQUESTs are being sent from;
 *	    PKT_LIST *: the best packet to be used to construct a REQUEST;
 *  output: char *: NULL or a copy of the domain name ('\0' terminated);
	char		*domainname = NULL;
	DHCP_OPT	*opt;
		uchar_t		*valptr;
		dhcp_symbol_t	*symp;
		valptr = (uchar_t *)opt + DHCP_OPT_META_LEN;
		    ITAB_CAT_STANDARD, ITAB_CONS_INFO, opt->code);
			    opt->len, B_TRUE);
			terminate_at_space(domainname);
			free(symp);
 *   input: dhcp_smach_t *: the state machine REQUESTs are being sent from;
 *	    PKT_LIST *: the best packet to be used to construct a REQUEST;
	char	*domainname = NULL;
	free(dsmp->dsm_dhcp_domainname);
	dsmp->dsm_dhcp_domainname = NULL;
		domainname = get_offered_domainname_v4(offer);
	dsmp->dsm_dhcp_domainname = domainname;
extern char **environ;
extern sigset_t original_sigmask;
	scf_handle_t *sr_handle;
	scf_instance_t *sr_inst;
	scf_snapshot_t *sr_snap;
	scf_propertygroup_t *sr_pg;
	scf_property_t *sr_prop;
	scf_value_t *sr_val;
	scf_transaction_t *sr_tx;
	scf_transaction_entry_t *sr_ent;
} scf_resources_t;
static pthread_mutex_t uid_mutex = PTHREAD_MUTEX_INITIALIZER;
static uid_t uid;
static int uid_cnt;
	posix_spawnattr_t attr;
	sigset_t fullset;
	int i, rc, status, n;
	pid_t pid;
	char vbuf[1024];
	vbuf[0] = 0;
	n = sizeof (vbuf);
		n -= strlcat(vbuf, " ", n);
		n -= strlcat(vbuf, argv[i], n);
		nlog(LOG_ERR, "nwamd_start_childv can't log full arg vector");
		    rc, strerror(rc));
	(void) sigfillset(&fullset);
		nlog(LOG_DEBUG, "setsigdefault %d %s\n", rc, strerror(rc));
		nlog(LOG_DEBUG, "setsigmask %d %s\n", rc, strerror(rc));
		nlog(LOG_DEBUG, "setflags %d %s\n", rc, strerror(rc));
		nlog(LOG_DEBUG, "posix_spawnp failed errno %d", rc);
		    rc, strerror(rc));
	(void) waitpid(pid, &status, 0);
		i = WIFSIGNALED(status) ? WTERMSIG(status) : WSTOPSIG(status);
		    strsignal(i));
		    WEXITSTATUS(status));
	zoneid_t zoneid;
	char zonename[ZONENAME_MAX];
	int ret;
	zoneid = getzoneid();
		datalink_id_t linkid;
		dladm_status_t status;
		char errstr[DLADM_STRSIZE];
			    linkname, dladm_status2str(status, errstr));
		zoneid = ALL_ZONES;
		ret = zone_check_datalink(&zoneid, linkid);
			(void) getzonenamebyid(zoneid, zonename, ZONENAME_MAX);
			    linkname, zonename);
	scf_entry_destroy(res->sr_ent);
	scf_transaction_destroy(res->sr_tx);
	scf_value_destroy(res->sr_val);
	scf_property_destroy(res->sr_prop);
	scf_pg_destroy(res->sr_pg);
	scf_snapshot_destroy(res->sr_snap);
	scf_instance_destroy(res->sr_inst);
	(void) scf_handle_unbind(res->sr_handle);
	scf_handle_destroy(res->sr_handle);
	res->sr_tx = NULL;
	res->sr_ent = NULL;
	res->sr_inst = NULL;
	res->sr_snap = NULL;
	res->sr_pg = NULL;
	res->sr_prop = NULL;
	res->sr_val = NULL;
		scf_handle_destroy(res->sr_handle);
		goto failure;
		goto failure;
		goto failure;
		goto failure;
		goto failure;
		goto failure;
		goto failure;
		goto failure;
		goto failure;
	    scf_strerror(scf_error()));
	release_scf_resources(res);
		goto failure;
		goto failure;
		goto failure;
	release_scf_resources(res);
	int result = -1;
	scf_resources_t res;
	uint8_t prop_val;
		goto cleanup;
	*answer = (boolean_t)prop_val;
	result = 0;
	release_scf_resources(&res);
	int result = -1;
	scf_resources_t res;
		goto cleanup;
	result = 0;
	release_scf_resources(&res);
	int result = -1;
	scf_resources_t res;
		goto cleanup;
	result = 0;
	release_scf_resources(&res);
	int result = -1;
	boolean_t new;
	new = (scf_pg_get_property(res->sr_pg, propname, res->sr_prop) != 0);
		goto failure;
			goto failure;
			goto failure;
		goto failure;
	result = scf_transaction_commit(res->sr_tx);
		scf_transaction_reset(res->sr_tx);
			goto failure;
		goto retry;
		goto failure;
	scf_resources_t res;
			goto failure;
			goto failure;
	scf_value_set_count(res.sr_val, (uint64_t)count);
		goto failure;
	release_scf_resources(&res);
	    "setting %s", scf_strerror(scf_error()), prop);
	release_scf_resources(&res);
	scf_resources_t res;
			goto failure;
			goto failure;
		goto failure;
		goto failure;
	release_scf_resources(&res);
	    "setting %s", scf_strerror(scf_error()), prop);
	release_scf_resources(&res);
	scf_resources_t res;
	int result = -1;
			goto failure;
			goto failure;
		goto failure;
		goto failure;
		goto failure;
	result = scf_transaction_commit(res.sr_tx);
		scf_transaction_reset(res.sr_tx);
			goto failure;
		goto retry;
		goto failure;
	release_scf_resources(&res);
	release_scf_resources(&res);
} av_opts;
static av_opts		*opts_av = NULL;
static const char	*_save_optstr = NULL;
static int		_save_numopts = 0;
int			optind_av = 1;
char			*optarg_av = NULL;
	POLICY_LIST *n = plist, *old;
		old = n;
		kmf_free_policy_record(&n->plc);
		n = n->next;
		free(old);
	plist = NULL;
	int rv = KC_OK;
	KMF_RETURN kmfrv = KMF_OK;
	POLICY_LIST *newitem, *plist = NULL;
	xmlParserCtxtPtr ctxt;
	xmlDocPtr doc = NULL;
	xmlNodePtr cur, node;
	ctxt = xmlNewParserCtxt();
	    XML_PARSE_DTDVALID | XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
		kmfrv = KMF_ERR_POLICY_DB_FORMAT;
		goto end;
	cur = xmlDocGetRootElement(doc);
		kmfrv = KMF_ERR_POLICY_DB_FORMAT;
		goto end;
	node = cur->xmlChildrenNode;
		char *c;
		node = node->next;
		xmlFreeParserCtxt(ctxt);
		xmlFreeDoc(doc);
		free_policy_list(plist);
		rv = KC_ERR_LOADDB;
		*policy_list = plist;
	uint16_t cur_bit;
	uint16_t kubits = 0;
	char *p;
	p = strtok(kustring, ",");
		cur_bit = kmf_string_to_ku(p);
			kubits = 0;
			break;
		kubits |= cur_bit;
		p = strtok(NULL, ",");
		ekus->eku_count++;
		    ekus->ekulist, ekus->eku_count * sizeof (KMF_OID));
			ekus->ekulist[ekus->eku_count-1] = *newoid;
	int rv = KC_OK;
	char *p;
	KMF_OID *newoid;
	KMF_EKU_POLICY *ekus = &plc->eku_set;
	p = strtok(ekulist, ",");
		newoid = kmf_ekuname_to_oid(ekulist);
			addToEKUList(ekus, newoid);
			free(newoid);
			rv = KC_ERR_USAGE;
		newoid = kmf_ekuname_to_oid(p);
			addToEKUList(ekus, newoid);
			free(newoid);
			rv = KC_ERR_USAGE;
			break;
		p = strtok(NULL, ",");
		kmf_free_eku_policy(ekus);
	int rv = KC_OK;
	char *p;
	KMF_OID newoid = { 0, NULL };
	KMF_EKU_POLICY *ekus = &plc->eku_set;
	p = strtok(ekulist, ",");
			addToEKUList(ekus, &newoid);
			rv = KC_ERR_USAGE;
			addToEKUList(ekus, &newoid);
			rv = KC_ERR_USAGE;
			break;
		p = strtok(NULL, ",");
		kmf_free_eku_policy(ekus);
	char *p;
	int len, i;
	char *retstr = NULL;
			*err_flag = KC_ERR_USAGE;
	p = str;
		p++;
			*err_flag = KC_ERR_USAGE;
	len = strlen(p);
		len--;
			*err_flag = KC_ERR_USAGE;
	i = 0;
			i++;
				*err_flag = KC_ERR_USAGE;
	retstr = malloc(len + 1);
			*err_flag = KC_ERR_MEMORY;
		*err_flag = KC_OK;
	(void) strncpy(retstr, p, len);
	retstr[len] = '\0';
	int		i;
	av_opts		*temp;
	char		*marker;
			opts_av = (av_opts *)temp;
			marker++;
			opts_av[i].has_arg = B_TRUE;
			marker++;
			opts_av[i].longnm = marker;
			opts_av[i].longnm_len = strcspn(marker, ")");
			optstring = marker + opts_av[i].longnm_len + 1;
			opts_av[i].longnm = optstring;
			opts_av[i].longnm_len = 1;
			optstring = marker;
	int	i;
	int	len;
			free(opts_av);
		opts_av = NULL;
		_save_optstr = optstring;
		_save_numopts = populate_opts((char *)optstring);
			optind_av++;
			break;
		len = strcspn(argv[optind_av], "=");
				optind_av++;
				optarg_av = &(argv[optind_av][len+1]);
				optind_av++;
			optarg_av = NULL;
			optind_av++;
		(void) fprintf(stderr, gettext("Error in the policy name\n"));
		break;
		    gettext("Error in trust anchor attributes\n"));
		break;
		    gettext("Error in OCSP policy attributes\n"));
		break;
		break;
	conf_entrylist_t *phead = NULL;
	conf_entrylist_t *ptr;
	conf_entry_t	*rtn_entry = NULL;
	ptr = phead;
			break;
		ptr = ptr->next;
		rtn_entry = dup_entry(ptr->entry);
	free_entrylist(phead);
static char *strset(char *, char *);
	table_t *t;
	t->nelem = size;
	t->used = -1;
	table_t *t2;
	int i;
		seterrline(__LINE__, __FILE__, NULL, NULL);
			"a NULL table");
	++t->used;
			print_stringtable(t);
			seterrline(__LINE__, __FILE__, NULL, NULL);
				"string table");
		t = t2;
		t->nelem += TABLE_INCREMENT;
			t->elements[i] = NULL;
	t->elements[t->used] = strset(t->elements[t->used], value);
		t->used = -1;
	int i;
	int i;
		sizeof (table_t) + (sizeof (char *) * t->nelem));
			get_stringtable(t, i));
			sizeof (char *), compare);
	size_t vlen;
	assert(value != NULL, "passed a null value to strset");
	vlen = strlen(value);
			seterrline(__LINE__, __FILE__, NULL, NULL);
			    "string");
			seterrline(__LINE__, __FILE__, NULL, NULL);
			    "a string");
	(void) strcpy(string, value);
	char	*p;
			*p = ' ';
	p--;
		*p-- = '\0';
		str++;
	char	*p;
		continue;
	p--;
		*p-- = '\0';
	char	*p;
		*p = tolower(*p);
	size_t	vlen;
	assert(value != NULL, "passed a NULL value to strset");
	vlen = strlen(value);
			errlog(FATAL, "malloc ran out of space");
			errlog(FATAL, "realloc ran out of space", "", 0);
	(void) strcpy(string, value);
	char	*q;
	char save;
	errlog(END, "}");
		p++;
	char	*q;
	q = strend(p);
	q--;
		q--;
		p++;
		p++;
	char    *p;
	static char basename[MAXLINE];
	p = strrchr(name, '/');
		*p = '\0';
		p = strrchr(name, '/');
	(void) strlcpy(basename, p? p+1: name, MAXLINE);
		*p = '\0';
	table_t	*t;
	t->nelem = size;
	t->used = -1;
	errlog(END, "}");
	table_t *t2;
	int	i;
			"a NULL table");
	t->used++;
			errlog(FATAL, "out of memory extending string table");
		t = t2;
		t->nelem += TABLE_INCREMENT;
			t->elements[i] = NULL;
	t->elements[t->used] = strset(t->elements[t->used], value);
	int	i;
	size_t	len = strlen(value);
			sizeof (char *), compare);
__FBSDID("$FreeBSD$");
	const char *s = src;
	char *d = dst;
		*d++ = *s++;
	char *bp = b;
		*bp++ = (unsigned char)c;
	const unsigned char *p1, *p2;
		;
		;
		*dst++ = *src++;
	*dst = '\0';
		dst++;
		*dst++ = *src++;
	*dst = '\0';
	size_t len = 0;
		len++;
	va_list ap;
	const char *hex = "0123456789abcdef";
	char buf[32], *s;
	uint16_t *S;
	unsigned long long u;
	int c, l;
	va_start(ap, fmt);
			putchar(c);
			continue;
		l = 0;
		c = *fmt++;
			l++;
			goto nextfmt;
			putchar(va_arg(ap, int));
			break;
				putchar(*s);
			break;
				putchar(*S);
			break;
				u = va_arg(ap, unsigned long long);
				break;
				u = va_arg(ap, unsigned long);
				break;
				u = va_arg(ap, unsigned int);
				break;
			s = buf;
					*s++ = '0' + (u % 10U);
					*s++ = hex[u & 0xfu];
				putchar(*s);
			break;
	va_end(ap);

/* ===== FUNCTIONS ===== */

/* Function 1 */
					(p[3] == '/' || p[3] == '\\')) {
					p += 3; /* Remove "/..", leave "/"
							 * for next pass. */
					slashonly = 0;
				} else

/* Function 2 */
	    strncmp(pat + fg->bol + fg->patternLen - 7, "[[:>:]]", 7) == 0) {
		fg->patternLen -= 14;
		fg->wmatch = 7;
	}

/* Function 3 */
	    ((fg->patternLen - (lastHalfDot + 1)) < firstHalfDot))))) {
		fg->reversedSearch = 1;
		hasDot = fg->patternLen - (firstHalfDot < 0 ?
		    firstLastHalfDot : firstHalfDot) - 1;
		grep_revstr(fg->pattern, fg->patternLen);
	}

/* Function 4 */
	} else if (fg->reversedSearch) {
		/* Quick Search algorithm. */

/* Function 5 */
			    !isxdigit((u_char)val[1])) {
				warnx("bad hexadecimal digits");
				return NULL;
			}

/* Function 6 */
		} else if (rv == (size_t)-2) {
			if (span < MB_CUR_MAX) { /* incomplete char */
				n += printwc(L'?', &stdout_state);
				break;
			}

/* Function 7 */
		} else if (is_log) {
			suffix = " (log)";

/* Function 8 */
void *sqliteMalloc_(int n, int bZero, char *zFile, int line){
  void *p;
  int *pi;
  int i, k;
  if( sqlite_iMallocFail>=0 ){
    sqlite_iMallocFail--;
    if( sqlite_iMallocFail==0 ){
      sqlite_malloc_failed++;
#if MEMORY_DEBUG>1
      fprintf(stderr,"**** failed to allocate %d bytes at %s:%d\n",
              n, zFile,line);
#endif
      sqlite_iMallocFail--;
      return 0;
    }
  }
  if( n==0 ) return 0;
  k = (n+sizeof(int)-1)/sizeof(int);
  pi = malloc( (N_GUARD*2+1+k)*sizeof(int));
  if( pi==0 ){
    sqlite_malloc_failed++;
    return 0;
  }
  sqlite_nMalloc++;
  for(i=0; i<N_GUARD; i++) pi[i] = 0xdead1122;
  pi[N_GUARD] = n;
  for(i=0; i<N_GUARD; i++) pi[k+1+N_GUARD+i] = 0xdead3344;
  p = &pi[N_GUARD+1];
  memset(p, bZero==0, n);
#if MEMORY_DEBUG>1
  fprintf(stderr,"%06d malloc %d bytes at 0x%x from %s:%d\n",
      ++memcnt, n, (int)p, zFile,line);
#endif
  return p;
}

/* Function 9 */
void sqliteCheckMemory(void *p, int N){
  int *pi = p;
  int n, i, k;
  pi -= N_GUARD+1;
  for(i=0; i<N_GUARD; i++){
    assert( pi[i]==0xdead1122 );
  }
  n = pi[N_GUARD];
  assert( N>=0 && N<n );
  k = (n+sizeof(int)-1)/sizeof(int);
  for(i=0; i<N_GUARD; i++){
    assert( pi[k+N_GUARD+1+i]==0xdead3344 );
  }
}

/* Function 10 */
void sqliteFree_(void *p, char *zFile, int line){
  if( p ){
    int *pi, i, k, n;
    pi = p;
    pi -= N_GUARD+1;
    sqlite_nFree++;
    for(i=0; i<N_GUARD; i++){
      if( pi[i]!=0xdead1122 ){
        fprintf(stderr,"Low-end memory corruption at 0x%x\n", (int)p);
        return;
      }
    }
    n = pi[N_GUARD];
    k = (n+sizeof(int)-1)/sizeof(int);
    for(i=0; i<N_GUARD; i++){
      if( pi[k+N_GUARD+1+i]!=0xdead3344 ){
        fprintf(stderr,"High-end memory corruption at 0x%x\n", (int)p);
        return;
      }
    }
    memset(pi, 0xff, (k+N_GUARD*2+1)*sizeof(int));
#if MEMORY_DEBUG>1
    fprintf(stderr,"%06d free %d bytes at 0x%x from %s:%d\n",
         ++memcnt, n, (int)p, zFile,line);
#endif
    free(pi);
  }
}

/* Function 11 */
void *sqliteRealloc_(void *oldP, int n, char *zFile, int line){
  int *oldPi, *pi, i, k, oldN, oldK;
  void *p;
  if( oldP==0 ){
    return sqliteMalloc_(n,1,zFile,line);
  }
  if( n==0 ){
    sqliteFree_(oldP,zFile,line);
    return 0;
  }
  oldPi = oldP;
  oldPi -= N_GUARD+1;
  if( oldPi[0]!=0xdead1122 ){
    fprintf(stderr,"Low-end memory corruption in realloc at 0x%x\n", (int)oldP);
    return 0;
  }
  oldN = oldPi[N_GUARD];
  oldK = (oldN+sizeof(int)-1)/sizeof(int);
  for(i=0; i<N_GUARD; i++){
    if( oldPi[oldK+N_GUARD+1+i]!=0xdead3344 ){
      fprintf(stderr,"High-end memory corruption in realloc at 0x%x\n",
              (int)oldP);
      return 0;
    }
  }
  k = (n + sizeof(int) - 1)/sizeof(int);
  pi = malloc( (k+N_GUARD*2+1)*sizeof(int) );
  if( pi==0 ){
    sqlite_malloc_failed++;
    return 0;
  }
  for(i=0; i<N_GUARD; i++) pi[i] = 0xdead1122;
  pi[N_GUARD] = n;
  for(i=0; i<N_GUARD; i++) pi[k+N_GUARD+1+i] = 0xdead3344;
  p = &pi[N_GUARD+1];
  memcpy(p, oldP, n>oldN ? oldN : n);
  if( n>oldN ){
    memset(&((char*)p)[oldN], 0, n-oldN);
  }
  memset(oldPi, 0xab, (oldK+N_GUARD+2)*sizeof(int));
  free(oldPi);
#if MEMORY_DEBUG>1
  fprintf(stderr,"%06d realloc %d to %d bytes at 0x%x to 0x%x at %s:%d\n",
    ++memcnt, oldN, n, (int)oldP, (int)p, zFile, line);
#endif
  return p;
}

/* Function 12 */
void sqliteStrRealloc(char **pz){
  char *zNew;
  if( pz==0 || *pz==0 ) return;
  zNew = malloc( strlen(*pz) + 1 );
  if( zNew==0 ){
    sqlite_malloc_failed++;
    sqliteFree(*pz);
    *pz = 0;
  }
  strcpy(zNew, *pz);
  sqliteFree(*pz);
  *pz = zNew;
}

/* Function 13 */
char *sqliteStrDup_(const char *z, char *zFile, int line){
  char *zNew;
  if( z==0 ) return 0;
  zNew = sqliteMalloc_(strlen(z)+1, 0, zFile, line);
  if( zNew ) strcpy(zNew, z);
  return zNew;
}

/* Function 14 */
char *sqliteStrNDup_(const char *z, int n, char *zFile, int line){
  char *zNew;
  if( z==0 ) return 0;
  zNew = sqliteMalloc_(n+1, 0, zFile, line);
  if( zNew ){
    memcpy(zNew, z, n);
    zNew[n] = 0;
  }
  return zNew;
}

/* Function 15 */
void *sqliteMalloc(int n){
  void *p;
  if( (p = malloc(n))==0 ){
    if( n>0 ) sqlite_malloc_failed++;
  }else{
    memset(p, 0, n);
  }
  return p;
}

/* Function 16 */
void *sqliteMallocRaw(int n){
  void *p;
  if( (p = malloc(n))==0 ){
    if( n>0 ) sqlite_malloc_failed++;
  }
  return p;
}

/* Function 17 */
void sqliteFree(void *p){
  if( p ){
    free(p);
  }
}

/* Function 18 */
void *sqliteRealloc(void *p, int n){
  void *p2;
  if( p==0 ){
    return sqliteMalloc(n);
  }
  if( n==0 ){
    sqliteFree(p);
    return 0;
  }
  p2 = realloc(p, n);
  if( p2==0 ){
    sqlite_malloc_failed++;
  }
  return p2;
}

/* Function 19 */
char *sqliteStrDup(const char *z){
  char *zNew;
  if( z==0 ) return 0;
  zNew = sqliteMallocRaw(strlen(z)+1);
  if( zNew ) strcpy(zNew, z);
  return zNew;
}

/* Function 20 */
char *sqliteStrNDup(const char *z, int n){
  char *zNew;
  if( z==0 ) return 0;
  zNew = sqliteMallocRaw(n+1);
  if( zNew ){
    memcpy(zNew, z, n);
    zNew[n] = 0;
  }
  return zNew;
}

/* Function 21 */
void sqliteSetString(char **pz, const char *zFirst, ...){
  va_list ap;
  int nByte;
  const char *z;
  char *zResult;

  if( pz==0 ) return;
  nByte = strlen(zFirst) + 1;
  va_start(ap, zFirst);
  while( (z = va_arg(ap, const char*))!=0 ){
    nByte += strlen(z);
  }
  va_end(ap);
  sqliteFree(*pz);
  *pz = zResult = sqliteMallocRaw( nByte );
  if( zResult==0 ){
    return;
  }
  strcpy(zResult, zFirst);
  zResult += strlen(zResult);
  va_start(ap, zFirst);
  while( (z = va_arg(ap, const char*))!=0 ){
    strcpy(zResult, z);
    zResult += strlen(zResult);
  }
  va_end(ap);
#ifdef MEMORY_DEBUG
#if MEMORY_DEBUG>1
  fprintf(stderr,"string at 0x%x is %s\n", (int)*pz, *pz);
#endif
#endif
}

/* Function 22 */
void sqliteSetNString(char **pz, ...){
  va_list ap;
  int nByte;
  const char *z;
  char *zResult;
  int n;

  if( pz==0 ) return;
  nByte = 0;
  va_start(ap, pz);
  while( (z = va_arg(ap, const char*))!=0 ){
    n = va_arg(ap, int);
    if( n<=0 ) n = strlen(z);
    nByte += n;
  }
  va_end(ap);
  sqliteFree(*pz);
  *pz = zResult = sqliteMallocRaw( nByte + 1 );
  if( zResult==0 ) return;
  va_start(ap, pz);
  while( (z = va_arg(ap, const char*))!=0 ){
    n = va_arg(ap, int);
    if( n<=0 ) n = strlen(z);
    strncpy(zResult, z, n);
    zResult += n;
  }
  *zResult = 0;
#ifdef MEMORY_DEBUG
#if MEMORY_DEBUG>1
  fprintf(stderr,"string at 0x%x is %s\n", (int)*pz, *pz);
#endif
#endif
  va_end(ap);
}

/* Function 23 */
void sqliteErrorMsg(Parse *pParse, const char *zFormat, ...){
  va_list ap;
  pParse->nErr++;
  sqliteFree(pParse->zErrMsg);
  va_start(ap, zFormat);
  pParse->zErrMsg = sqliteVMPrintf(zFormat, ap);
  va_end(ap);
}

/* Function 24 */
void sqliteDequote(char *z){
  int quote;
  int i, j;
  if( z==0 ) return;
  quote = z[0];
  switch( quote ){
    case '\'':  break;
    case '"':   break;
    case '[':   quote = ']';  break;
    default:    return;
  }
  for(i=1, j=0; z[i]; i++){
    if( z[i]==quote ){
      if( z[i+1]==quote ){
        z[j++] = quote;
        i++;
      }else{
        z[j++] = 0;
        break;
      }
    }else{
      z[j++] = z[i];
    }
  }
}

/* Function 25 */
int sqliteHashNoCase(const char *z, int n){
  int h = 0;
  if( n<=0 ) n = strlen(z);
  while( n > 0  ){
    h = (h<<3) ^ h ^ UpperToLower[(unsigned char)*z++];
    n--;
  }
  return h & 0x7fffffff;
}

/* Function 26 */
int sqliteStrICmp(const char *zLeft, const char *zRight){
  register unsigned char *a, *b;
  a = (unsigned char *)zLeft;
  b = (unsigned char *)zRight;
  while( *a!=0 && UpperToLower[*a]==UpperToLower[*b]){ a++; b++; }
  return UpperToLower[*a] - UpperToLower[*b];
}

/* Function 27 */
int sqliteStrNICmp(const char *zLeft, const char *zRight, int N){
  register unsigned char *a, *b;
  a = (unsigned char *)zLeft;
  b = (unsigned char *)zRight;
  while( N-- > 0 && *a!=0 && UpperToLower[*a]==UpperToLower[*b]){ a++; b++; }
  return N<0 ? 0 : UpperToLower[*a] - UpperToLower[*b];
}

/* Function 28 */
int sqliteIsNumber(const char *z){
  if( *z=='-' || *z=='+' ) z++;
  if( !isdigit(*z) ){
    return 0;
  }
  z++;
  while( isdigit(*z) ){ z++; }
  if( *z=='.' ){
    z++;
    if( !isdigit(*z) ) return 0;
    while( isdigit(*z) ){ z++; }
  }
  if( *z=='e' || *z=='E' ){
    z++;
    if( *z=='+' || *z=='-' ) z++;
    if( !isdigit(*z) ) return 0;
    while( isdigit(*z) ){ z++; }
  }
  return *z==0;
}

/* Function 29 */
  }else if( *z=='+' ){
    z++;

/* Function 30 */
    }else if( *z=='+' ){
      z++;

/* Function 31 */
int sqliteFitsIn32Bits(const char *zNum){
  int i, c;
  if( *zNum=='-' || *zNum=='+' ) zNum++;
  for(i=0; (c=zNum[i])>='0' && c<='9'; i++){}
  return i<10 || (i==10 && memcmp(zNum,"2147483647",10)<=0);
}

/* Function 32 */
  }else if( btext==0 ){
    return 1;

/* Function 33 */
      }else if( rA>rB ){
        result = +1;

/* Function 34 */
  }else if( isNumB ){
    result = +1;

/* Function 35 */
      }else if( isNumB ){
        res = +1;

/* Function 36 */
  }else if( r<(0.5/64.0) ){
    while( r < 0.5/_64e64 && exp > -961  ){ r *= _64e64;  exp -= 64; }

/* Function 37 */
  }else if( r>=0.5 ){
    while( r >= 0.5*_64e63 && exp < 960  ){ r *= 1.0/_64e64; exp += 64; }

/* Function 38 */
static int sqlite_utf8_to_int(const unsigned char *z){
  int c;
  static const int initVal[] = {
      0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
     15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
     30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,
     45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
     60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,
     75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,
     90,  91,  92,  93,  94,  95,  96,  97,  98,  99, 100, 101, 102, 103, 104,
    105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134,
    135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
    150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164,
    165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
    180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,   0,   1,   2,
      3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,
     18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,   0,
      1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
      0,   1,   2,   3,   4,   5,   6,   7,   0,   1,   2,   3,   0,   1, 254,
    255,
  };
  c = initVal[*(z++)];
  while( (0xc0&*z)==0x80 ){
    c = (c<<6) | (0x3f&*(z++));
  }
  return c;
}

/* Function 39 */
          }else if( c==c2 ){
            seen = 1;

/* Function 40 */
sqliteLikeCompare(const unsigned char *zPattern, const unsigned char *zString){
  register int c;
  int c2;

  while( (c = UpperToLower[*zPattern])!=0 ){
    switch( c ){
      case '%': {
        while( (c=zPattern[1]) == '%' || c == '_' ){
          if( c=='_' ){
            if( *zString==0 ) return 0;
            sqliteNextChar(zString);
          }
          zPattern++;
        }
        if( c==0 ) return 1;
        c = UpperToLower[c];
        while( (c2=UpperToLower[*zString])!=0 ){
          while( c2 != 0 && c2 != c ){ c2 = UpperToLower[*++zString]; }
          if( c2==0 ) return 0;
          if( sqliteLikeCompare(&zPattern[1],zString) ) return 1;
          sqliteNextChar(zString);
        }
        return 0;
      }
      case '_': {
        if( *zString==0 ) return 0;
        sqliteNextChar(zString);
        zPattern++;
        break;
      }
      default: {
        if( c != UpperToLower[*zString] ) return 0;
        zPattern++;
        zString++;
        break;
      }
    }
  }
  return *zString==0;
}

/* Function 41 */
int sqliteSafetyOn(sqlite *db){
  if( db->magic==SQLITE_MAGIC_OPEN ){
    db->magic = SQLITE_MAGIC_BUSY;
    return 0;
  }else if( db->magic==SQLITE_MAGIC_BUSY || db->magic==SQLITE_MAGIC_ERROR
             || db->want_to_close ){
    db->magic = SQLITE_MAGIC_ERROR;
    db->flags |= SQLITE_Interrupt;
  }
  return 1;
}

/* Function 42 */
int sqliteSafetyOff(sqlite *db){
  if( db->magic==SQLITE_MAGIC_BUSY ){
    db->magic = SQLITE_MAGIC_OPEN;
    return 0;
  }else if( db->magic==SQLITE_MAGIC_OPEN || db->magic==SQLITE_MAGIC_ERROR
             || db->want_to_close ){
    db->magic = SQLITE_MAGIC_ERROR;
    db->flags |= SQLITE_Interrupt;
  }
  return 1;
}

/* Function 43 */
int sqliteSafetyCheck(sqlite *db){
  if( db->pVdbe!=0 ){
    db->magic = SQLITE_MAGIC_ERROR;
    return 1;
  }
  return 0;
}

/* Function 44 */
map_v4v6_address(const char *src, char *dst) {
	u_char *p = (u_char *)dst;
	char tmp[NS_INADDRSZ];
	int i;

	/* Stash a temporary copy so our caller can update in place. */
	memcpy(tmp, src, NS_INADDRSZ);
	/* Mark this ipv6 addr as a mapped ipv4. */
	for (i = 0; i < 10; i++)
		*p++ = 0x00;
	*p++ = 0xff;
	*p++ = 0xff;
	/* Retrieve the saved copy and we're done. */
	memcpy((void*)p, tmp, NS_INADDRSZ);
}

/* Function 45 */
	} else if (n < 0) {
		a->len = -1;

/* Function 46 */
	} else if (c >= 'a' && c <= 'f') {
		return (c - 'a' + 10);

/* Function 47 */
	} else if (c >= 'A' && c <= 'F') {
		return (c - 'A' + 10);

/* Function 48 */
	} else if (pathlen > 0) {
		fullpath = (char *)strdup(filepath);

/* Function 49 */
	} else if (dirlen > 0) {
		fullpath = (char *)strdup(dir);

/* Function 50 */
	    !strcasecmp(timetok, "seconds")) {
		*ltime = num;
	} else if (!strcasecmp(timetok, "minute") ||

/* Function 51 */
	    !strcasecmp(timetok, "minutes")) {
		*ltime = num * SECSPERMIN;
	} else if (!strcasecmp(timetok, "day") ||

/* Function 52 */
	    !strcasecmp(timetok, "days")) {
		*ltime = num * SECSPERDAY;
	} else if (!strcasecmp(timetok, "hour") ||

/* Function 53 */
	    !strcasecmp(timetok, "hours")) {
		*ltime = num * SECSPERHOUR;
	} else {
		*ltime = 0;
		return (-1);
	}

/* Function 54 */
			    (te->ascq == 9)) {
				err_msg(msg);
				err_msg(msg1);
				return;
			}

/* Function 55 */
				    !IsActiveMode(BackCommentMode)) {
					(void) fprintf(fp, "\n");
					print_prefix(fp, "$ ", TRUE,
					    si->comment);
					(void) fprintf(fp, "$set\t%d\n",
					    si->id);
				} else {
					(void) fprintf(fp, "\n$set\t%d\n",
					    si->id);
				}

/* Function 56 */
				    IsActiveMode(BackCommentMode)) {
					print_prefix(fp, "$ ", TRUE,
					    si->comment);
				}

/* Function 57 */
				    IsActiveMode(LineInfoMode)) {
					(void) fprintf(fp,
					    "$ File:%s, line:%d\n",
					    basename(mi->file), mi->line);
				}

/* Function 58 */
				    IsActiveMode(LineInfoMode)) {
					(void) fprintf(fp,
					    "$ File:%s, line:%d\n",
					    basename(mi->file), mi->line);
				}

/* Function 59 */
			    (si = lookup_setid(i)) && si->comment) {
				com = si->comment;
			}

/* Function 60 */
		    ((*ptr == '\n') && inmsg == FALSE)) {
			inmsg = FALSE;
			continue;
		}

/* Function 61 */
		    sizeof (DELSET_TOKEN) - 1) == 0) {
			if (sscanf(ptr, "%*s %d", &unsetid) != 1) {
				unsetid = -1;
			}
			inmsg = FALSE;
			continue;
		} else if (strncmp(ptr, QUOTE_TOKEN,

/* Function 62 */
		    sizeof (QUOTE_TOKEN) - 1) == 0) {
			if (sscanf(ptr, "%*s %c", &c) != 1) {
				c = 0;
			}
			quote = c;
			inmsg = FALSE;
			continue;
		}

/* Function 63 */
				    (*(ptr + len - 2) == c))) {
					*(ptr + len - 2) = '\0';
					ptr++;
				}

/* Function 64 */
		} else if (msg[len] == '\\') {
			len--;

/* Function 65 */
	} else if (b->lt_se_data.lt_s_total < a->lt_se_data.lt_s_total) {
		return (-1);

/* Function 66 */
	} else if (b->lt_se_data.lt_s_max < a->lt_se_data.lt_s_max) {
		return (-1);

/* Function 67 */
	} else if (b->lt_se_data.lt_s_count < a->lt_se_data.lt_s_count) {
		return (-1);

/* Function 68 */
	} else if (avg_b < avg_a) {
		return (-1);

/* Function 69 */
	} else if (cp = strchr(hdr, '(')) {
		snprintf(buf, sizeof (buf), "%s %s",

/* Function 70 */
		else if (c == '\\') {
			/*
			 * Ignore the next character except EOF
			 */
			if (getc(fp) == EOF)
				break;
		}

/* Function 71 */
	else if (substr) {
		/*
		 * The string passed to us may be a multiple path string for
		 * which we only need the first component.  Using the provided
		 * size, strip out the required string.
		 */
		(void) strncpy(str, name, nsize);
		str[nsize - 1] = '\0';
		name = str;
	}

/* Function 72 */
	    (FLG_RT_RELOCED | FLG_RT_INITCALL)) {
		leave(LIST(dlmp), 0);
		(void) _lwp_cond_wait(&dlmp->rt_cv, (mutex_t *)&dlmp->rt_lock);
		(void) rt_mutex_unlock(&dlmp->rt_lock);
		(void) enter(0);
		(void) rt_mutex_lock(&dlmp->rt_lock);
	}

/* Function 73 */
		    ((*_nobj == NULL) || !(FLAGS(*_nobj) & FLG_RT_INITFRST))) {
			Aliste	idx;
			Rt_map	**pobj;

			rtld_flags &= ~RT_FL_INITFIRST;

			for (APLIST_TRAVERSE(pending, idx, pobj)) {
				aplist_delete(pending, &idx);
				call_init(pobj, DBG_INIT_PEND);
			}
		}

/* Function 74 */
	    (rtld_flags2 & (RT_FL2_PLMSETUP | RT_FL2_NOPLM)))) {
		if ((tobj = tsort(nlmp, nlml->lm_init,
		    RT_SORT_REV)) == (Rt_map **)S_ERROR)
			tobj = NULL;
	}

/* Function 75 */
	    (rtld_flags2 & (RT_FL2_PLMSETUP | RT_FL2_NOPLM)))) {
		Rt_map	*lmp;

		for (lmp = nlmp; lmp; lmp = NEXT_RT_MAP(lmp)) {
			if (PTTLS(lmp) && PTTLS(lmp)->p_memsz)
				tls_modaddrem(lmp, TM_FLG_MODADD);
		}
		nlml->lm_tls = 0;
	}

/* Function 76 */
	} else if (FLAGS(lmp) & FLG_RT_OBJINTPO) {
		Rt_map	*tlmp;

/* Function 77 */
		    tlmp = NEXT_RT_MAP(tlmp)) {

			if (FLAGS(tlmp) & FLG_RT_OBJINTPO)
				continue;

			/*
			 * Insert the new link-map before this non-interposer,
			 * and indicate an interposer is found.
			 */
			NEXT(PREV_RT_MAP(tlmp)) = (Link_map *)lmp;
			PREV(lmp) = PREV(tlmp);

			NEXT(lmp) = (Link_map *)tlmp;
			PREV(tlmp) = (Link_map *)lmp;

			lmc->lc_flags |= LMC_FLG_REANALYZE;
			add = 0;
			break;
		}

/* Function 78 */
	    (FLAGS(lml->lm_head) & FLG_RT_RELOCED)) {
		Lm_cntl *lmc;

		if ((lmc = alist_append(&lml->lm_lists, NULL, sizeof (Lm_cntl),
		    AL_CNT_LMLISTS)) == NULL)
			return (0);

		return ((Aliste)((char *)lmc - (char *)lml->lm_lists));
	}

/* Function 79 */
		    MSG_ORIG(MSG_LD_AUDIT), MSG_LD_AUDIT_SIZE) == 0)) {
			/*
			 * Replaceable and permanent audit objects can exist.
			 */
			select |= SEL_ACT_STR;
			str = (select & SEL_REPLACE) ? &rpl_audit : &prm_audit;
			variable = ENV_FLG_AUDIT;
		} else if ((len == MSG_LD_AUDIT_ARGS_SIZE) &&

/* Function 80 */
		    MSG_ORIG(MSG_LD_BIND_NOW), MSG_LD_BIND_NOW_SIZE) == 0)) {
			select |= SEL_ACT_RT2;
			val = RT_FL2_BINDNOW;
			variable = ENV_FLG_BIND_NOW;
		} else if ((len == MSG_LD_BIND_NOT_SIZE) && (strncmp(s1,

/* Function 81 */
		    MSG_ORIG(MSG_LD_BIND_NOT), MSG_LD_BIND_NOT_SIZE) == 0)) {
			/*
			 * Another trick, initially implemented to help debug
			 * a.out executables under SunOS 4 binary
			 * compatibility (now removed), not documented for
			 * general use, but still useful for debugging around
			 * the PLT, etc.
			 */
			select |= SEL_ACT_RT;
			val = RT_FL_NOBIND;
			variable = ENV_FLG_BIND_NOT;
		} else if ((len == MSG_LD_BINDINGS_SIZE) && (strncmp(s1,

/* Function 82 */
		    MSG_ORIG(MSG_LD_BINDINGS), MSG_LD_BINDINGS_SIZE) == 0)) {
			/*
			 * This variable is simply for backward compatibility.
			 * If this and LD_DEBUG are both specified, only one of
			 * the strings is going to get processed.
			 */
			select |= SEL_ACT_SPEC_2;
			variable = ENV_FLG_BINDINGS;
		}

/* Function 83 */
		    MSG_ORIG(MSG_LD_CAP_FILES), MSG_LD_CAP_FILES_SIZE) == 0)) {
			select |= SEL_ACT_STR;
			str = (select & SEL_REPLACE) ?
			    &rpl_cap_files : &prm_cap_files;
			variable = ENV_FLG_CAP_FILES;
		} else if ((len == MSG_LD_CONFGEN_SIZE) && (strncmp(s1,

/* Function 84 */
		    MSG_ORIG(MSG_LD_CONFGEN), MSG_LD_CONFGEN_SIZE) == 0)) {
			/*
			 * This variable is not documented for general use.
			 * Although originaly designed for internal use with
			 * crle(1), this variable is in use by the Studio
			 * auditing tools.  Hence, it can't be removed.
			 */
			select |= SEL_ACT_SPEC_2;
			variable = ENV_FLG_CONFGEN;
		} else if ((len == MSG_LD_CONFIG_SIZE) && (strncmp(s1,

/* Function 85 */
		    MSG_ORIG(MSG_LD_CONFIG), MSG_LD_CONFIG_SIZE) == 0)) {
			/*
			 * Secure applications must use a default configuration
			 * file.  A setting from a configuration file doesn't
			 * make sense (given we must be reading a configuration
			 * file to have gotten this).
			 */
			if ((rtld_flags & RT_FL_SECURE) ||
			    (env_flags & ENV_TYP_CONFIG))
				return;
			select |= SEL_ACT_STR;
			str = &config->c_name;
			variable = ENV_FLG_CONFIG;
		}

/* Function 86 */
		    MSG_ORIG(MSG_LD_DEBUG), MSG_LD_DEBUG_SIZE) == 0)) {
			select |= SEL_ACT_STR;
			str = (select & SEL_REPLACE) ? &rpl_debug : &prm_debug;
			variable = ENV_FLG_DEBUG;
		} else if ((len == MSG_LD_DEBUG_OUTPUT_SIZE) && (strncmp(s1,

/* Function 87 */
		    MSG_ORIG(MSG_LD_DEFERRED), MSG_LD_DEFERRED_SIZE) == 0)) {
			select |= SEL_ACT_RT;
			val = RT_FL_DEFERRED;
			variable = ENV_FLG_DEFERRED;
		} else if ((len == MSG_LD_DEMANGLE_SIZE) && (strncmp(s1,

/* Function 88 */
		    MSG_ORIG(MSG_LD_DEMANGLE), MSG_LD_DEMANGLE_SIZE) == 0)) {
			select |= SEL_ACT_RT;
			val = RT_FL_DEMANGLE;
			variable = ENV_FLG_DEMANGLE;
		}

/* Function 89 */
		    MSG_ORIG(MSG_LD_FLAGS), MSG_LD_FLAGS_SIZE) == 0)) {
			select |= SEL_ACT_SPEC_1;
			str = (select & SEL_REPLACE) ? &rpl_ldflags :
			    &prm_ldflags;
			variable = ENV_FLG_FLAGS;
		}

/* Function 90 */
		    MSG_ORIG(MSG_LD_HWCAP), MSG_LD_HWCAP_SIZE) == 0)) {
			select |= SEL_ACT_STR;
			str = (select & SEL_REPLACE) ?
			    &rpl_hwcap : &prm_hwcap;
			variable = ENV_FLG_HWCAP;
		}

/* Function 91 */
		    MSG_ORIG(MSG_LD_INIT), MSG_LD_INIT_SIZE) == 0)) {
			select |= SEL_ACT_LML;
			val = LML_FLG_TRC_INIT;
			variable = ENV_FLG_INIT;
		}

/* Function 92 */
		    MSG_ORIG(MSG_LD_LIBPATH), MSG_LD_LIBPATH_SIZE) == 0)) {
			select |= SEL_ACT_SPEC_1;
			str = (select & SEL_REPLACE) ? &rpl_libpath :
			    &prm_libpath;
			variable = ENV_FLG_LIBPATH;
		} else if ((len == MSG_LD_LOADAVAIL_SIZE) && (strncmp(s1,

/* Function 93 */
		    MSG_ORIG(MSG_LD_LOADAVAIL), MSG_LD_LOADAVAIL_SIZE) == 0)) {
			/*
			 * This variable is not documented for general use.
			 * Although originaly designed for internal use with
			 * crle(1), this variable is in use by the Studio
			 * auditing tools.  Hence, it can't be removed.
			 */
			select |= SEL_ACT_LML;
			val = LML_FLG_LOADAVAIL;
			variable = ENV_FLG_LOADAVAIL;
		} else if ((len == MSG_LD_LOADFLTR_SIZE) && (strncmp(s1,

/* Function 94 */
		    MSG_ORIG(MSG_LD_LOADFLTR), MSG_LD_LOADFLTR_SIZE) == 0)) {
			select |= SEL_ACT_SPEC_2;
			variable = ENV_FLG_LOADFLTR;
		}

/* Function 95 */
		    MSG_ORIG(MSG_LD_MACHCAP), MSG_LD_MACHCAP_SIZE) == 0)) {
			select |= SEL_ACT_STR;
			str = (select & SEL_REPLACE) ?
			    &rpl_machcap : &prm_machcap;
			variable = ENV_FLG_MACHCAP;
		}

/* Function 96 */
		    MSG_ORIG(MSG_LD_NOAUDIT), MSG_LD_NOAUDIT_SIZE) == 0)) {
			select |= SEL_ACT_RT;
			val = RT_FL_NOAUDIT;
			variable = ENV_FLG_NOAUDIT;
		} else if ((len == MSG_LD_NOAUXFLTR_SIZE) && (strncmp(s1,

/* Function 97 */
		    MSG_ORIG(MSG_LD_NOAUXFLTR), MSG_LD_NOAUXFLTR_SIZE) == 0)) {
			select |= SEL_ACT_RT;
			val = RT_FL_NOAUXFLTR;
			variable = ENV_FLG_NOAUXFLTR;
		} else if ((len == MSG_LD_NOBAPLT_SIZE) && (strncmp(s1,

/* Function 98 */
		    MSG_ORIG(MSG_LD_NOBAPLT), MSG_LD_NOBAPLT_SIZE) == 0)) {
			select |= SEL_ACT_RT;
			val = RT_FL_NOBAPLT;
			variable = ENV_FLG_NOBAPLT;
		} else if ((len == MSG_LD_NOCONFIG_SIZE) && (strncmp(s1,

/* Function 99 */
		    MSG_ORIG(MSG_LD_NOCONFIG), MSG_LD_NOCONFIG_SIZE) == 0)) {
			select |= SEL_ACT_RT;
			val = RT_FL_NOCFG;
			variable = ENV_FLG_NOCONFIG;
		} else if ((len == MSG_LD_NODIRCONFIG_SIZE) && (strncmp(s1,

/* Function 100 */
		    MSG_ORIG(MSG_LD_NODIRECT), MSG_LD_NODIRECT_SIZE) == 0)) {
			select |= SEL_ACT_LMLT;
			val = LML_TFLG_NODIRECT;
			variable = ENV_FLG_NODIRECT;
		} else if ((len == MSG_LD_NOENVCONFIG_SIZE) && (strncmp(s1,

/* Function 101 */
		    MSG_ORIG(MSG_LD_NOLAZY), MSG_LD_NOLAZY_SIZE) == 0)) {
			select |= SEL_ACT_LMLT;
			val = LML_TFLG_NOLAZYLD;
			variable = ENV_FLG_NOLAZY;
		} else if ((len == MSG_LD_NOOBJALTER_SIZE) && (strncmp(s1,

/* Function 102 */
		    MSG_ORIG(MSG_LD_NOVERSION), MSG_LD_NOVERSION_SIZE) == 0)) {
			select |= SEL_ACT_RT;
			val = RT_FL_NOVERSION;
			variable = ENV_FLG_NOVERSION;
		} else if ((len == MSG_LD_NOUNRESWEAK_SIZE) && (strncmp(s1,

/* Function 103 */
		    MSG_ORIG(MSG_LD_NOPAREXT), MSG_LD_NOPAREXT_SIZE) == 0)) {
			select |= SEL_ACT_LML;
			val = LML_FLG_TRC_NOPAREXT;
			variable = ENV_FLG_NOPAREXT;
		} else if ((len == MSG_LD_NOENVIRON_SIZE) && (strncmp(s1,

/* Function 104 */
		    MSG_ORIG(MSG_LD_NOENVIRON), MSG_LD_NOENVIRON_SIZE) == 0)) {
			/*
			 * LD_NOENVIRON can only be set with ld.so.1 -e.
			 */
			select |= SEL_ACT_RT;
			val = RT_FL_NOENVIRON;
			variable = ENV_FLG_NOENVIRON;
		}

/* Function 105 */
		    MSG_ORIG(MSG_LD_PLATCAP), MSG_LD_PLATCAP_SIZE) == 0)) {
			select |= SEL_ACT_STR;
			str = (select & SEL_REPLACE) ?
			    &rpl_platcap : &prm_platcap;
			variable = ENV_FLG_PLATCAP;
		} else if ((len == MSG_LD_PRELOAD_SIZE) && (strncmp(s1,

/* Function 106 */
		    MSG_ORIG(MSG_LD_PRELOAD), MSG_LD_PRELOAD_SIZE) == 0)) {
			select |= SEL_ACT_STR;
			str = (select & SEL_REPLACE) ? &rpl_preload :
			    &prm_preload;
			variable = ENV_FLG_PRELOAD;
		} else if ((len == MSG_LD_PROFILE_SIZE) && (strncmp(s1,

/* Function 107 */
		    MSG_ORIG(MSG_LD_PROFILE), MSG_LD_PROFILE_SIZE) == 0)) {
			/*
			 * Only one user library can be profiled at a time.
			 */
			select |= SEL_ACT_SPEC_2;
			variable = ENV_FLG_PROFILE;
		} else if ((len == MSG_LD_PROFILE_OUTPUT_SIZE) && (strncmp(s1,

/* Function 108 */
		    MSG_ORIG(MSG_LD_SFCAP), MSG_LD_SFCAP_SIZE) == 0)) {
			select |= SEL_ACT_STR;
			str = (select & SEL_REPLACE) ?
			    &rpl_sfcap : &prm_sfcap;
			variable = ENV_FLG_SFCAP;
		} else if ((len == MSG_LD_SIGNAL_SIZE) &&

/* Function 109 */
		    ((rtld_flags & RT_FL_SECURE) == 0)) {
			select |= SEL_ACT_SPEC_2;
			variable = ENV_FLG_SIGNAL;
		}

/* Function 110 */
	else if (*s1 == 'T') {
		if (((len == MSG_LD_TRACE_OBJS_SIZE) &&
		    (strncmp(s1, MSG_ORIG(MSG_LD_TRACE_OBJS),
		    MSG_LD_TRACE_OBJS_SIZE) == 0)) ||
		    ((len == MSG_LD_TRACE_OBJS_E_SIZE) &&
		    (strncmp(s1, MSG_ORIG(MSG_LD_TRACE_OBJS_E),
		    MSG_LD_TRACE_OBJS_E_SIZE) == 0))) {
			char	*s0 = (char *)s1;

			select |= SEL_ACT_SPEC_2;
			variable = ENV_FLG_TRACE_OBJS;

#if	defined(__sparc) || defined(__x86)
			/*
			 * The simplest way to "disable" this variable is to
			 * truncate this string to "LD_'\0'". This string is
			 * ignored by any ld.so.1 environment processing.
			 * Use of such interfaces as unsetenv(3c) are overkill,
			 * and would drag too much libc implementation detail
			 * into ld.so.1.
			 */
			*s0 = '\0';
#else
/*
 * Verify that the above write is appropriate for any new platforms.
 */
#error	unsupported architecture!
#endif
		} else if ((len == MSG_LD_TRACE_PTHS_SIZE) && (strncmp(s1,
		    MSG_ORIG(MSG_LD_TRACE_PTHS),
		    MSG_LD_TRACE_PTHS_SIZE) == 0)) {
			select |= SEL_ACT_LML;
			val = LML_FLG_TRC_SEARCH;
			variable = ENV_FLG_TRACE_PTHS;
		}
	}

/* Function 111 */
		    MSG_ORIG(MSG_LD_UNREF), MSG_LD_UNREF_SIZE) == 0)) {
			select |= SEL_ACT_LML;
			val = LML_FLG_TRC_UNREF;
			variable = ENV_FLG_UNREF;
		} else if ((len == MSG_LD_UNUSED_SIZE) && (strncmp(s1,

/* Function 112 */
		    MSG_ORIG(MSG_LD_UNUSED), MSG_LD_UNUSED_SIZE) == 0)) {
			select |= SEL_ACT_LML;
			val = LML_FLG_TRC_UNUSED;
			variable = ENV_FLG_UNUSED;
		}

/* Function 113 */
		    MSG_ORIG(MSG_LD_VERBOSE), MSG_LD_VERBOSE_SIZE) == 0)) {
			select |= SEL_ACT_LML;
			val = LML_FLG_TRC_VERBOSE;
			variable = ENV_FLG_VERBOSE;
		}

/* Function 114 */
		    MSG_ORIG(MSG_LD_WARN), MSG_LD_WARN_SIZE) == 0)) {
			select |= SEL_ACT_LML;
			val = LML_FLG_TRC_WARN;
			variable = ENV_FLG_WARN;
		}

/* Function 115 */
	} else if (select & SEL_ACT_RT2) {
		if (s2)

/* Function 116 */
	} else if (select & SEL_ACT_STR) {
		if (env_flags & ENV_TYP_NULL)

/* Function 117 */
	} else if (select & SEL_ACT_LML) {
		if (s2)

/* Function 118 */
	} else if (select & SEL_ACT_LMLT) {
		if (s2)

/* Function 119 */
	} else if (select & SEL_ACT_SPEC_1) {
		/*

/* Function 120 */
	} else if (select & SEL_ACT_SPEC_2) {
		/*

/* Function 121 */
	} else if (*++s2 == '\0') {
		len = strlen(s1) - 1;

/* Function 122 */
		    (*s2++ == '=') && (*s2 != '\0') && (loc < LOC_LANG)) {
			glcs[CI_LCMESSAGES].lc_un.lc_ptr = (char *)s2;
			loc = LOC_LANG;
			continue;
		}

/* Function 123 */
					    (ssize < sizeof (local))) {
						*_s++ = '0';
						ssize++;
					}

/* Function 124 */
		    ((prevptr == NULL) || (strcmp(prevptr, nextptr) != 0))) {
			prevptr = nextptr;
			nextptr = prf.pr_cur;
			*nextptr = '\0';
		}

/* Function 125 */
			    (lasterr > (errbuf + ERRSIZE)))) {
				(void) write(2, lasterr, strlen(lasterr));
				(void) write(2, MSG_ORIG(MSG_STR_NL),
				    MSG_STR_NL_SIZE);
			}

/* Function 126 */
	    (flags | MAP_ANON), -1, 0)) == MAP_FAILED) {
		int	err = errno;
		eprintf(lml, ERR_FATAL, MSG_INTL(MSG_SYS_MMAPANON),
		    strerror(err));
		return (MAP_FAILED);
	}

/* Function 127 */
			    (gettimeofday(&DBG_TOTALTIME, NULL) == 0)) {
				DBG_DELTATIME = DBG_TOTALTIME;
				DBG_ONRESET();
			}

/* Function 128 */
		} else if (pdp->pd_flags & LA_SER_RUNPATH) {
			fmt = MSG_INTL(MSG_USD_RUNPATH);

/* Function 129 */
	    (status.st_uid == 0) && (status.st_mode & S_ISUID))) {
		rtld_flags2 |= RT_FL2_SETUID;
		return (1);
	}

/* Function 130 */
	    ((arg[optsz] == '=') && strncmp(arg, opt, optsz) == 0)) {
		/*
		 * Otherwise, compare the option name, which may be
		 * concatenated with the option argument.
		 */
		DBG_CALL(Dbg_args_str2chr(lml, ndx, opt, c));

		if (arg[optsz] == '\0') {
			/*
			 * Optarg is the next argument (white space separated).
			 * Make sure an optarg is available, and if not return
			 * a failure to prevent any fall-through to the generic
			 * getopt() processing.
			 *
			 * Since we'll be completely failing this option we
			 * don't want to update optopt with the translation,
			 * but also need to set it to _something_.  Setting it
			 * to the '-' of the argument causes us to behave
			 * correctly.
			 */
			if ((++optind + 1) > argc) {
				optopt = arg[0];
				return ('?');
			}
			optarg = argv[optind];
			optind++;
		} else {
			/*
			 * GNU option/option argument pairs can be represented
			 * with a "=" separator.  If this is the case, remove
			 * the separator.
			 */
			optarg = &arg[optsz];
			optind++;
			if (*optarg == '=') {
				if (*(++optarg) == '\0') {
					optopt = arg[0];
					return ('?');
				}
			}
		}

		if (cbfunc != NULL)
			c = (*cbfunc)(c);
		optopt = c;
		return (c);
	}

/* Function 131 */
			    MSG_ORIG(MSG_ARG_T_SHARED), 0, NULL)) != 0) {
				return (c);

			/* Translate -soname <optarg> to -h <optarg> */
			} else if ((c = str2chr(lml, ndx, argc, argv, arg, 'h',

/* Function 132 */
		    MSG_ORIG(MSG_ARG_T_WL), MSG_ARG_T_WL_SIZE) == 0)) {
			DBG_CALL(Dbg_args_Wldel(lml, ndx, optarg));
			c = optarg[MSG_ARG_T_WL_SIZE];
			optarg += MSG_ARG_T_WL_SIZE + 1;
		}

/* Function 133 */
		    (strncasecmp(sym->sym_name, str, cmp_len) == 0)) {
			*value = sym->sym_value;
			return (1);
		}

/* Function 134 */
		    (strncasecmp(sym->sym_name, str, cmp_len) == 0)) {
			*value = sym->sym_value;
			return (1);
		}

/* Function 135 */
		    (argc <= 0) || (*(argv[0]) != '-')) {
			state->go_done = 1;
			return (NULL);
		}

/* Function 136 */
			    (argstr[2] != '\0')) {
				/*
				 * We have a match. However, there may also
				 * be a straightforward match that we have
				 * not yet found. If so, we want to prefer that
				 * case over this one. So rather than return
				 * it immediately, we capture the information
				 * and keep looking. If nothing else surfaces,
				 * we'll use this later.
				 */
				sgl_with_value.valid = 1;
				sgl_with_value.ret.gor_idmask = item.oai_idmask;
				sgl_with_value.excmask = item.oai_excmask;
				sgl_with_value.ret.gor_value = argstr + 2;
				sgl_with_value.is_outstyle = is_outstyle;
				continue;
			}

/* Function 137 */
			    (ch == item.oai_name[1])) {
				/*
				 * It matches. If the option requires a value
				 * then it cannot be in a group.
				 */
				if (item.oai_flags & ELFEDIT_CMDOA_F_VALUE)
					elfedit_command_usage();

				/* Mutually exclusive option already seen? */
				if (item.oai_excmask & state->go_idmask)
					elfedit_command_usage();

				/* Return the match */
				state->go_idmask |= item.oai_idmask;
				state->go_ret.gor_idmask = item.oai_idmask;
				state->go_ret.gor_value = NULL;
				return (&state->go_ret);
			}

/* Function 138 */
			    (strncmp(env->e_str, arg, varsz) == 0)) {
				/*
				 * If the user has already specified this string
				 * given them a warning, and ignore the new one.
				 */
				if ((env->e_flags & RTC_ENV_CONFIG) == 0) {
					(void) fprintf(stderr,
					    MSG_INTL(MSG_WARN_ENV),
					    crle->c_name, (int)varsz,
					    env->e_str);
					return (2);
				}

				/*
				 * Otherwise the original string must have been
				 * retrieved from a config file.  In this case
				 * allow the user to override it.
				 */
				free((void *)env->e_str);
				crle->c_strsize -= env->e_totsz;
				crle->c_strsize += totsz;

				if ((env->e_str = strdup(arg)) == 0) {
					int err = errno;
					(void) fprintf(stderr,
					    MSG_INTL(MSG_SYS_MALLOC),
					    crle->c_name, strerror(err));
					return (0);
				}
				env->e_varsz = varsz;
				env->e_totsz = totsz;
				env->e_flags &= ~RTC_ENV_CONFIG;
				env->e_flags |= flags;

				return (1);
			}

/* Function 139 */
	    ((env->e_str = strdup(arg)) == NULL)) {
		int err = errno;
		(void) fprintf(stderr, MSG_INTL(MSG_SYS_MALLOC),
		    crle->c_name, strerror(err));
		free(env);
		return (0);
	}

/* Function 140 */
	macdefine(&e->e_macro, A_TEMP, macid("{deliveryMode}"), buf);
}

/* Function 141 */
		    (isalnum(*f) || strchr("!#$%&'*+-./^_`{|}~", *f) != NULL))
		{
			l--;
			*t++ = *f;
		}

/* Function 142 */
is_valid_interface_name(const char *name) {

	const char *end;
	const char *last_dot;

	last_dot = NULL;

	if (strlen(name) == 0)
		return FALSE;

	end = name + strlen(name);

	if (*name == '.')    /* disallow starting with a . */
		return FALSE;
	else if (!VALID_INITIAL_NAME_CHARACTER (*name))
		return FALSE;
	else
		name++;

	while (name != end) {
		if (*name == '.') {
			if ((name + 1) == end)
				return FALSE;
			else if (!VALID_INITIAL_NAME_CHARACTER (*(name + 1)))
				return FALSE;
			last_dot = name;
			name++;    /* we just validated the next char, so skip two */
		} else if (!VALID_NAME_CHARACTER (*name))
			return FALSE;
		name++;
	}
	if (last_dot == NULL)
		return FALSE;

	return TRUE;
}

/* Function 143 */
	} else if (type == MT_WARN) {
		syslog(LOG_WARNING, "%s", buf);

/* Function 144 */
	} else if (debug_enabled && type == MT_DEBUG) {
		syslog(LOG_DEBUG, "%s", buf);

/* Function 145 */
		    sizeof (struct pollfd)))) == NULL) {
			return (-1);
		}

/* Function 146 */
			} else if (errno != EINTR) {
				error_msg(gettext("Unexpected read error: %s"),

/* Function 147 */
	} else if (errno == EROFS) {
		dhcpmsg(MSG_DEBUG, "%s is on a read-only file "

/* Function 148 */
	    ipadm_is_valid_hostname(dsmp->dsm_msg_reqhost)) {
		reqhost = dsmp->dsm_msg_reqhost;
	} else {
		char		hostfile[PATH_MAX + 1];

		(void) snprintf(hostfile, sizeof (hostfile),
		    "/etc/hostname.%s", dsmp->dsm_name);
		reqhost = iffile_to_hostname(hostfile);
	}

/* Function 149 */
	    dhcp_get_nodename(nodename, sizeof (nodename))) {
		reqhost = nodename;
	}

/* Function 150 */
	    dhcp_get_nodename(nodename, sizeof (nodename))) {
		reqhost = nodename;
	}

/* Function 151 */
nwamd_escalate(void) {
	priv_set_t *priv_set;
	priv_set = priv_str_to_set("zone", ",", NULL);

	if (priv_set == NULL)
		pfail("creating privilege set: %s", strerror(errno));

	(void) pthread_mutex_lock(&uid_mutex);
	if (uid == 0)
		uid = getuid();
	if (uid_cnt++ == 0) {
		if (setppriv(PRIV_SET, PRIV_EFFECTIVE, priv_set) == -1) {
			priv_freeset(priv_set);
			pfail("setppriv effective: %s", strerror(errno));
		}
	}
	(void) pthread_mutex_unlock(&uid_mutex);

	priv_freeset(priv_set);
}

/* Function 152 */
nwamd_deescalate(void) {
	(void) pthread_mutex_lock(&uid_mutex);

	assert(uid_cnt > 0);
	if (--uid_cnt == 0) {
		priv_set_t *priv_set, *allpriv_set;

		/* build up our minimal set of privs. */
		priv_set = priv_str_to_set("basic", ",", NULL);
		allpriv_set = priv_str_to_set("zone", ",", NULL);
		if (priv_set == NULL || allpriv_set == NULL)
			pfail("converting privilege sets: %s", strerror(errno));

		(void) priv_addset(priv_set, PRIV_FILE_CHOWN_SELF);
		(void) priv_addset(priv_set, PRIV_FILE_DAC_READ);
		(void) priv_addset(priv_set, PRIV_FILE_DAC_WRITE);
		(void) priv_addset(priv_set, PRIV_NET_RAWACCESS);
		(void) priv_addset(priv_set, PRIV_NET_PRIVADDR);
		(void) priv_addset(priv_set, PRIV_PROC_AUDIT);
		(void) priv_addset(priv_set, PRIV_PROC_OWNER);
		(void) priv_addset(priv_set, PRIV_PROC_SETID);
		(void) priv_addset(priv_set, PRIV_SYS_CONFIG);
		(void) priv_addset(priv_set, PRIV_SYS_IP_CONFIG);
		(void) priv_addset(priv_set, PRIV_SYS_IPC_CONFIG);
		(void) priv_addset(priv_set, PRIV_SYS_MOUNT);
		(void) priv_addset(priv_set, PRIV_SYS_NET_CONFIG);
		(void) priv_addset(priv_set, PRIV_SYS_RES_CONFIG);
		(void) priv_addset(priv_set, PRIV_SYS_RESOURCE);

		/*
		 * Since our zone might not have all these privs,
		 * just ask for those that are available.
		 */
		priv_intersect(allpriv_set, priv_set);

		if (setppriv(PRIV_SET, PRIV_INHERITABLE, priv_set) == -1) {
			priv_freeset(allpriv_set);
			priv_freeset(priv_set);
			pfail("setppriv inheritable: %s", strerror(errno));
		}
		/*
		 * Need to ensure permitted set contains all privs so we can
		 * escalate later.
		 */
		if (setppriv(PRIV_SET, PRIV_PERMITTED, allpriv_set) == -1) {
			priv_freeset(allpriv_set);
			priv_freeset(priv_set);
			pfail("setppriv permitted: %s", strerror(errno));
		}
		/*
		 * We need to find a smaller set of privs that are important to
		 * us.  Otherwise we really are not gaining much by doing this.
		 */
		if (setppriv(PRIV_SET, PRIV_EFFECTIVE, priv_set) == -1) {
			priv_freeset(allpriv_set);
			priv_freeset(priv_set);
			pfail("setppriv effective: %s", strerror(errno));
		}

		priv_freeset(priv_set);
		priv_freeset(allpriv_set);
	}
	(void) pthread_mutex_unlock(&uid_mutex);
}

/* Function 153 */
		    (const xmlChar *)KMF_POLICY_ELEMENT)) {
			/* Check the name attribute */
			c = (char *)xmlGetProp(node,
			    (const xmlChar *)KMF_POLICY_NAME_ATTR);

			/* If a match, parse the rest of the data */
			if (c != NULL) {
				xmlFree(c);
				newitem = malloc(sizeof (POLICY_LIST));
				if (newitem != NULL) {
					(void) memset(newitem, 0,
					    sizeof (POLICY_LIST));
					kmfrv = parsePolicyElement(node,
					    &newitem->plc);
				} else {
					kmfrv = KMF_ERR_MEMORY;
					goto end;
				}
				/* add to linked list */
				if (plist == NULL) {
					plist = newitem;
				} else {
					POLICY_LIST *n = plist;
					while (n->next != NULL)
						n = n->next;

					n->next = newitem;
					newitem->next = NULL;
				}
			}
		}

/* Function 154 */
		    realloc(opts_av, (i+1) * sizeof (av_opts)))) == NULL) {
			free(opts_av);
			opts_av = NULL;
			return (0);
		} else

/* Function 155 */
	    ((sizeof (char *)) * size)))) == NULL) {
		errlog(FATAL,
		    "\nOut of memory.\n"
		    "We wish to hold the whole sky,\n"
		    "But we never will.\n");
	}

/* Function 156 */
	} else if (index > t->used) {
		return (NULL);

/* Function 157 */
	} else if (strlen(string) < vlen) {
		/* Reallocate bigger. */

/* Function 158 */
	} else if (strlen(string) < vlen) {


/* Function 159 */
	errlog(BEGIN, "in_string_set( p = \"%s\", set = \"%s\") {", p, set);

	for (;;) {
		set = skipb(set);
		q = nextsep(set);
		if (q == set) {
			/* We've hit the end */
			break;
		}
		save = *q;
		*q = '\0';
		if (strcmp(p, set) == 0) {
			*q = save;
			errlog(VERBOSE, "return YES");
			errlog(END, "}");
			return (YES);
		}
		*q = save;
		set = q;
	}

/* Function 160 */
	errlog(BEGIN, "skipsep() {");
	errlog(VERBOSE, "p (in) = %s", p);
	while (*p && !(isalnum(*p) || *p == '_' || *p == '$'))
		p++;
	errlog(VERBOSE, "p (out) = %s", p);
	errlog(END, "}");

/* Function 161 */
	errlog(BEGIN, "nextsep() {");
	errlog(VERBOSE, "p (in) = %s", p);
	while (*p && isalnum(*p) || *p == '_' || *p == '$')
		p++;
	errlog(VERBOSE, "p (out) = %s", p);
	errlog(END, "}");

/* Function 162 */
	errlog(BEGIN, "nextsep() {");
	errlog(VERBOSE, "p (in) = %s", p);
	while (*p && isalnum(*p) || *p == '_' || *p == '$' || *p == '.')
		p++;
	errlog(VERBOSE, "p (out) = %s", p);
	errlog(END, "}");

/* Function 163 */
	    (size_t)(sizeof (table_t) + (sizeof (char *)*size)))) == NULL) {
		errlog(FATAL, "out of memory creating a string table");
	}

/* Function 164 */
	errlog(BEGIN, "free_string_table() {");
	if (t != NULL) {
		t->used = -1;
	}
	errlog(END, "}");

/* Function 165 */
	} else if (index > t->used) {
		return (NULL);


#ifdef __cplusplus
}
#endif

#endif /* UTIL_UNIFIED_C_H */

/* End of util_unified.c - Synthesized by MINIX4 Synthesis System */
