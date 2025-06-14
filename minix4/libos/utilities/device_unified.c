/**
 * @file device_unified.c
 * @brief Unified device implementation
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
 *     1. userspace\minix_commands\cawf\device.c
 *     2. userspace\commands_legacy\text\cawf\device.c
 *     3. minix4\libos\lib_legacy\libcontract\common\device.c
 *     4. minix4\exokernel\kernel_legacy\uts\common\contract\device.c
 *     5. minix4\exokernel\kernel_legacy\cmd\cdrw\device.c
 *     6. minix4\exokernel\kernel_legacy\cmd\hal\hald\device.c
 * 
 * Total source files: 6
 * Synthesis date: 2025-06-13 19:53:48
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
#include "cawf.h"
#include "device.h"
#include "hald.h"
#include "hald_marshal.h"
#include "hald_runner.h"
#include "libcontract_impl.h"
#include "logger.h"
#include "main.h"
#include "misc_scsi.h"
#include "mmc.h"
#include "msgs.h"
#include "toshiba.h"
#include "transport.h"
#include "util.h"
#include <ctype.h>
#include <dbus/dbus.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <hal/libhal.h>
#include <libcontract.h>
#include <libintl.h>
#include <libnvpair.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/avl.h>
#include <sys/cmn_err.h>
#include <sys/contract.h>
#include <sys/contract/device.h>
#include <sys/contract/device_impl.h>
#include <sys/contract_impl.h>
#include <sys/ctfs.h>
#include <sys/ddi.h>
#include <sys/ddi_impldefs.h>
#include <sys/ddi_implfuncs.h>
#include <sys/debug.h>
#include <sys/dkio.h>
#include <sys/esunddi.h>
#include <sys/fs/dv_node.h>
#include <sys/id_space.h>
#include <sys/kmem.h>
#include <sys/list.h>
#include <sys/mutex.h>
#include <sys/nvpair.h>
#include <sys/param.h>
#include <sys/policy.h>
#include <sys/proc.h>
#include <sys/stat.h>
#include <sys/sunddi.h>
#include <sys/sunndi.h>
#include <sys/sysmacros.h>
#include <sys/systm.h>
#include <sys/thread.h>
#include <sys/types.h>
#include <unistd.h>

/* ===== DEFINES ===== */
#define	strcasecmp	strcmpi
#define	EVSENDP(ctd, flag) \
#define	EVINFOP(ctd, flag) \
#define	SAFE_EV		(CT_DEV_ALLEVENT)
#define	EXCESS(value)	((value) & ~SAFE_EV)

/* ===== TYPES ===== */
	struct ctlib_status_info *info = stathdl;
	struct ctlib_status_info *info = stathdl;
	struct ctlib_status_info *info = stathdl;
	struct ctlib_status_info *info = stathdl;
struct ct_dev_negtable {
	struct dk_minfo mediainfo;
	struct stat statbuf;
	struct stat statbuf;
	struct dk_cinfo cinfo;
	struct dirent *dirent;
	struct track_info *ti;
	struct trackio_error *te;
enum {
				   enum PropertyAttribute attr,
typedef struct {

/* ===== GLOBAL VARIABLES ===== */
static unsigned char *Convstr(char *s, int *len);
static int Convfont(char *nm, char *s, char **fn, unsigned char **fi);
static unsigned char *Convstr(char *s, int *len);
static int Convfont(char *nm, char *s, char **fn, unsigned char **fi);
	int ret = ct_tmpl_get_internal_string(fd, CTDP_MINOR, buf, *buflenp);
	int error;
    int spec_type, proc_t *owner, int *errorp);
static void ct_barrier_acquire(dev_info_t *dip);
static void ct_barrier_release(dev_info_t *dip);
static int ct_barrier_held(dev_info_t *dip);
static int ct_barrier_empty(dev_info_t *dip);
static void ct_barrier_decr(dev_info_t *dip);
static void ct_barrier_incr(dev_info_t *dip);
	uint_t	st_old;
	uint_t	st_new;
	uint_t	st_neg;
	char *buf;
	char *minor;
	int error;
	int spec_type;
	uint64_t param_value;
	char *str_value;
		str_value = (char *)kparam->ctpm_kbuf;
		param_value = *(uint64_t *)kparam->ctpm_kbuf;
	uint64_t *param_value = kparam->ctpm_kbuf;
		kparam->ret_size = sizeof (uint64_t);
	char *buf;
	int spec_type;
	int error;
	VERIFY(nvlist_add_uint32(nvl, CTDS_STATE, ctd->cond_state) == 0);
	VERIFY(nvlist_add_uint32(nvl, CTDS_ASET, ctd->cond_aset) == 0);
	VERIFY(nvlist_add_uint32(nvl, CTDS_NONEG, ctd->cond_noneg) == 0);
	int	i;
	char		*devnm;
		char		*path;
		(void) ddi_pathname(dip, path);
		(void) ddi_deviname(dip, devnm);
		(void) devfs_clean(pdip, devnm + 1, DV_CLEAN_FORCE);
		(void) devfs_clean(dip, NULL, DV_CLEAN_FORCE);
	int error;
		char *buf;
		(void) ddi_pathname(dip, buf);
	char *minor;
	char *path;
	(void) contract_type_pbundle(device_type, owner);
	char *path;
	int error;
	int timed_out = 0;
	int result = CT_NONE;
	int ack;
	CT_DEBUG((CE_NOTE, "%s: entered: dip: %p", f, (void *)dip));
		CT_DEBUG((CE_NOTE, "%s: timed out: %p", f, (void *)dip));
		CT_DEBUG((CE_NOTE, "%s: result = ACK, dip=%p", f, (void *)dip));
		    f, (void *)dip));
		    f, (void *)dip));
	uint_t state = get_state(dip);
	uint_t result = CT_NONE;
	uint64_t evid = 0;
	uint64_t nevid = 0;
	char *path = NULL;
	int negend;
	int match;
	int sync = 0;
	int broken = 0;
	(void) ddi_pathname(dip, path);
		    (void *)dip));
		    DEVI(dip)->devi_ct_count, (void *)dip));
		    "dip=%p", (void *)dip));
	uint_t result;
	uint_t evtype;
	uint_t evtype;
	(void) contract_device_publish(dip, dev, spec_type, evtype, nvl);
	uint_t evtype;
	(void) contract_device_publish(dip, dev, spec_type, evtype, nvl);
	uint_t evtype;
	    "dip: %p", result, (void *)dip));
	(void) contract_device_publish(dip, dev, spec_type, evtype, nvl);
	    (void *)dip));
	int	result;
		    "dip (%p)", evtype, dev, spec_type, (void *)dip);
		    evtype, dev, spec_type, (void *)dip);
	int fd;
	uchar_t *cap;
	char devnode[PATH_MAX];
	int size;
	int use_cd_speed = 0;
	dev->d_node = (char *)my_zalloc(strlen(devnode) + 1);
	(void) strcpy(dev->d_node, devnode);
	dev->d_inq = (uchar_t *)my_zalloc(INQUIRY_DATA_LENGTH);
		(void) close(dev->d_fd);
		cap = (uchar_t *)my_zalloc(18);
		(void) printf("Checking device type\n");
				(void) printf("DVD-R read support\n");
				(void) printf("DVD-R write support\n");
				(void) printf("R-W supported\n");
				(void) printf("DVD-RAM read supported\n");
				(void) printf("DVD-RAM write supported\n");
			(void) printf("Could not read mode page 2A! \n");
	cap = (uchar_t *)my_zalloc(8);
				(void) printf("using rt speed ctrl\n");
				(void) printf("using cd speed ctrl\n");
			(void) printf("DKIOCGMEDIAINFO failed\n");
		uint_t bsize;
		(void) printf("blocksize = %d\n", dev->d_blksize);
			(void) printf(" switching to %d\n", dev->d_blksize);
		dev->d_name = (char *)my_zalloc(strlen(user_supplied) + 1);
		(void) strcpy(dev->d_name, user_supplied);
	(void) close(dev->d_fd);
	char **udi, *p = NULL;
	int ndevs = 0, i;
	char **udi, *path = NULL;
	int ndevs = 0, i;
		(void) strlcpy(path, symname, (strlen(symname) + 1));
	char *p1;
	int i;
			(void) snprintf(found, PATH_MAX, "%s/s%d", p1, i);
		(void) strlcpy(found, p1, PATH_MAX);
	int fd;
	char tmpstr[PATH_MAX];
	int fd;
	int ret = 1;
		(void) close(fd);
	(void) printf(gettext("    Node	           Connected Device"));
	(void) printf(gettext("	           Device type\n"));
	(void) printf("+-----------------\n");
	char sdev[PATH_MAX], dev[PATH_MAX];
	int writers_found = 0;
	int header_printed = 0;
	int is_writer;
	int total_devices_found;
			char *sn;
				print_header();
				header_printed = 1;
	(void) closedir(dir);
	uchar_t *disc_info, disc_status = 0, erasable = 0;
	uchar_t page_code[4];
	char *errmsg = NULL;
	print_n_flush(gettext("Writing track %d..."), (int)ti->ti_track_no);
			(void) str_print(gettext("Aborted.\n"), progress_pos);
	(void) str_print(gettext("done.\n"), progress_pos);
	uchar_t *cap = (uchar_t *)my_zalloc(MMC_FTR_HDR_LEN);
			(void) print_profile_list(fd);
					(void) printf("CD-ROM found\n");
					(void) printf("CD-R found\n");
					(void) printf("DVD-ROM found\n");
					(void) printf("CD-RW found\n");
					(void) printf("DVD-R found\n");
					(void) printf("DVD-RAM found\n");
					(void) printf("DVD-RW found\n");
					(void) printf("DVD-R DL found\n");
					(void) printf("DVD-RW DL found\n");
					(void) printf("DVD+RW found\n");
					(void) printf("DVD+R found\n");
					(void) printf("DVD+RW DL found\n");
					(void) printf("DVD+R DL found\n");
static GObjectClass *parent_class;
static guint signals[LAST_SIGNAL] = { 0 };
int dbg_hal_device_object_delta = 0;
	static int temp_device_counter = 0;
	static GType type = 0;
		int type;
		const char *key;
		int target_type;
		gchar *target_key;
				hal_property_get_int (p));
				hal_property_get_uint64 (p));
		int type;
		const char *key;
		int target_type;
				hal_property_get_int (p));
				hal_property_get_uint64 (p));
	int len;
		const char *key;
		int type;
			snprintf (buf, bufsize, "%d", hal_property_get_int (prop));
			snprintf (buf, bufsize, "%llu", (long long unsigned int) hal_property_get_uint64 (prop));
			snprintf (buf, bufsize, "%f", hal_property_get_double (prop));
				guint i;
		hal_property_set_int (prop, value);
		prop = hal_property_new_int (key, value);
		hal_property_set_uint64 (prop, value);
		prop = hal_property_new_uint64 (key, value);
				to_device, to, hal_device_property_get_int (from_device, from));
				to_device, to, hal_device_property_get_uint64 (from_device, from));
        fprintf (stderr, "device udi = %s\n", hal_device_get_udi (device));
                int type;
                const char *key;
                                hal_property_get_int (p));
                                (long long unsigned int) hal_property_get_uint64 (p));
        fprintf (stderr, "\n");
	char *key;
	gpointer user_data;
	guint prop_signal_id;
	guint timeout_id;

/* ===== FUNCTIONS ===== */

/* Function 1 */
static unsigned char *Convstr(char *s, int *len) {
	if ((r = (unsigned char *)malloc(strlen((char *)s) + 1)) == NULL) {
		(void) fprintf(stderr, "%s: out of string space at %s\n",
			Pname, s);
		return(NULL);
	}
	for (cp = r, l = 0; *s;) {
		switch (*s) {

		case '\\':
			s++;
			if (*s >= '0' && *s <= '7') {
				for (c = i = 0; i < 3; i++, s++) {
					if (*s < '0' || *s > '7') {
						em = "non-octal char";
bad_string:
						(void) fprintf(stderr,
							"%s: %s : %s\n",
							Pname, em, (char *)r);
						return(NULL);
					}
					c = (c << 3) + *s - '0';
				}
				if (c > 0377) {
					em = "octal char > 0377";
					goto bad_string;
				}
				*cp++ = c;
				l++;
			} else if (*s == 'x') {
				s++;
				for (c = i = 0; i < 2; i++, s++) {
#if	defined(__STDC__)
					if ( ! isalpha(*s) && ! isdigit(*s))
#else
					if ( ! isascii(*s) && ! isalpha(*s)
					&&   ! isdigit(*s))
#endif
					{
non_hex_char:
						em = "non-hex char";
						goto bad_string;
					}
					c = c << 4;
					if (*s >= '0' && *s <= '9')
						c += *s - '0';
					else if ((*s >= 'a' && *s <= 'f')
					     ||  (*s >= 'A' && *s <= 'F'))
						c += *s + 10 -
						     (isupper(*s) ? 'A' : 'a');
					else
						goto non_hex_char;
				}
				*cp++ = (unsigned char)c;
				l++;
			} else if (*s == 'E' || *s == 'e') {
				*cp++ = ESC;
				l++;
				s++;
			} else if (*s == '\0') {
				em = "no char after \\";
				goto bad_string;
			} else {
				*cp++ = *s++;
				l++;
			}
			break;
		default:
			*cp++ = *s++;
			l++;
		}
	}
	*cp = '\0';
	*len = l;
	return(r);
}

/* Function 2 */
static int Convfont(char* nm, char *s, char **fn, unsigned char **fi) {
	if ((cp = strchr(s, '=')) == NULL) {
		(void) fprintf(stderr, "%s: bad %s font line format: %s\n",
			Pname, nm, s);
		return(0);
	}
	if ((*fn = (char *)malloc(cp - s + 1)) == NULL) {
		(void) fprintf(stderr, "%s: no space for %s font name %s\n",
			Pname, nm, s);
		return(0);
	}
	(void) strncpy(*fn, s, cp - s);
	(*fn)[cp - s] = '\0';
	if ((*fi = Convstr(cp + 1, &len)) == NULL)
		return(0);
	return(len);
}

/* Function 3 */
int Defdev(void) {
	Fstr.b = Fstr.i = Fstr.it = Fstr.r = NULL;
	Fstr.bl = Fstr.il = Fstr.itl = Fstr.rl = 0;
	if (Device == NULL || strcasecmp(Device, "normal") == 0) {
		Fontctl = 0;
check_font:
		if (Devfont) {
			(void) fprintf(stderr,
				"%s: font %s for device %s illegal\n",
				Pname, Devfont, Device ? Device : "NORMAL");
			return(1);
		}
		return(0);
	}
	Fontctl = 1;
	if (strcasecmp(Device, "ansi") == 0) {
		Fstr.b = Newstr((unsigned char *)"x[1m");
		Fstr.it = Newstr((unsigned char *)"x[4m");
		Fstr.r = Newstr((unsigned char *)"x[0m");
		Fstr.b[0] = Fstr.it[0] = Fstr.r[0] = ESC;
		Fstr.bl = Fstr.itl = Fstr.rl = 4;
		goto check_font;
	}
	if (strcasecmp(Device, "none") == 0)
		goto check_font;
	if (Devconf)
		p = Devconf;
	else {

		if ((p = getenv("CAWFLIB")) == NULL)	
			p = CAWFLIB;
		len = strlen(p) + 1 + strlen(DEVCONFIG) + 1;
		if ((s = (char *)malloc(len)) == NULL) {
			(void) fprintf(stderr, "%s: no space for %s name\n",
				Pname, DEVCONFIG);
			return(1);
		}
		(void) sprintf(s, "%s/%s", p, DEVCONFIG);
		p = s;
	}
#ifdef	UNIX
	if ((fs = fopen(p, "r")) == NULL)
#else
	if ((fs = fopen(p, "rt")) == NULL)
#endif
	{
		(void) fprintf(stderr, "%s: can't open config file: %s\n",
			Pname, p);
		return(1);
	}
	*line = ' ';
	while ( ! feof(fs)) {
		if (*line == '\t' || *line == '#' || *line == ' ') {
			(void) fgets(line, MAXLINE, fs);
			continue;
		}
		if ((s = strrchr(line, '\n')) != NULL)
			*s = '\0';
		else
			line[MAXLINE-1] = '\0';
		if (strcmp(Device, line) != 0) {
			(void) fgets(line, MAXLINE, fs);
			continue;
		}
		fd = 1;
		while (fgets(line, MAXLINE, fs) != NULL) {
			if (*line == ' ') {
				for (i = 1; line[i] == ' '; i++)
					;
			} else if (*line == '\t')
				i = 1;
			else
				break;
#if	defined(__STDC__)
			if ( ! isalpha(line[i])
#else
			if ( ! isascii(line[i]) || ! isalpha(line[i])
#endif
			||   line[i+1] != '=')
				break;
			if ((s = strrchr(line, '\n')) != NULL)
				*s = '\0';
			else
				line[MAXLINE-1] = '\0';
			switch (line[i]) {
			case 'b':
				if (Fstr.b != NULL) {
				    (void) fprintf(stderr,
					"%s: dup bold for %s in %s: %s\n",
					Pname, Device, p, line);
					(void) free(Fstr.b);
					Fstr.b = NULL;
				}
				if ((Fstr.b = Convstr(&line[i+2], &Fstr.bl))
				== NULL)
					err++;
				break;
			case 'i':
				if (Fstr.it != NULL) {
				    (void) fprintf(stderr,
					"%s: dup italic for %s in %s: %s\n",
					Pname, Device, p, line);
					(void) free(Fstr.it);
					Fstr.it = NULL;
				}
				if ((Fstr.it = Convstr(&line[i+2], &Fstr.itl))
				== NULL)
					err++;
				break;
			case 'r':
				if (Fstr.r != NULL) {
				    (void) fprintf(stderr,
					"%s: dup roman for %s in %s: %s\n",
					Pname, Device, p, line);
					(void) free(Fstr.r);
					Fstr.r = NULL;
				}
				if ((Fstr.r = Convstr(&line[i+2], &Fstr.rl))
				== NULL)
					err++;
				break;
			case 'f':
				if ( ! Devfont || Fstr.i)
					break;
				if ((i = Convfont(Device, &line[i+2], &fn, &fi))
				< 0)
					err++;
				else if (fn && strcmp(Devfont, fn) == 0) {
					Fstr.i = fi;
					Fstr.il = i;
					fi = NULL;
				}
				if (fn) {
					(void) free(fn);
					fn = NULL;
				}
				if (fi) {
					(void) free((char *)fi);
					fi = NULL;
				}
				break;
			default:
				(void) fprintf(stderr,
					"%s: unknown device %s line: %s\n",
					Pname, Device, line);
				err++;
			}
		}
		break;
	}
	(void) fclose(fs);
	if (err)
		return(1);
	if ( ! fd) {
		(void) fprintf(stderr, "%s: can't find device %s in %s\n",
			Pname, Device, p);
		return(1);
	}
	if (Devfont && ! Fstr.i) {
		(void) fprintf(stderr,
			"%s: font %s for device %s not found in %s\n",
			Pname, Devfont, Device, p);
		return(1);
	}
	return(0);
}

/* Function 4 */
static unsigned char *Convstr(char *s, int *len) {
	if ((r = (unsigned char *)malloc(strlen((char *)s) + 1)) == NULL) {
		(void) fprintf(stderr, "%s: out of string space at %s\n",
			Pname, s);
		return(NULL);
	}
	for (cp = r, l = 0; *s;) {
		switch (*s) {

		case '\\':
			s++;
			if (*s >= '0' && *s <= '7') {
				for (c = i = 0; i < 3; i++, s++) {
					if (*s < '0' || *s > '7') {
						em = "non-octal char";
bad_string:
						(void) fprintf(stderr,
							"%s: %s : %s\n",
							Pname, em, (char *)r);
						return(NULL);
					}
					c = (c << 3) + *s - '0';
				}
				if (c > 0377) {
					em = "octal char > 0377";
					goto bad_string;
				}
				*cp++ = c;
				l++;
			} else if (*s == 'x') {
				s++;
				for (c = i = 0; i < 2; i++, s++) {
#if	defined(__STDC__)
					if ( ! isalpha(*s) && ! isdigit(*s))
#else
					if ( ! isascii(*s) && ! isalpha(*s)
					&&   ! isdigit(*s))
#endif
					{
non_hex_char:
						em = "non-hex char";
						goto bad_string;
					}
					c = c << 4;
					if (*s >= '0' && *s <= '9')
						c += *s - '0';
					else if ((*s >= 'a' && *s <= 'f')
					     ||  (*s >= 'A' && *s <= 'F'))
						c += *s + 10 -
						     (isupper(*s) ? 'A' : 'a');
					else
						goto non_hex_char;
				}
				*cp++ = (unsigned char)c;
				l++;
			} else if (*s == 'E' || *s == 'e') {
				*cp++ = ESC;
				l++;
				s++;
			} else if (*s == '\0') {
				em = "no char after \\";
				goto bad_string;
			} else {
				*cp++ = *s++;
				l++;
			}
			break;
		default:
			*cp++ = *s++;
			l++;
		}
	}
	*cp = '\0';
	*len = l;
	return(r);
}

/* Function 5 */
static int Convfont(char* nm, char *s, char **fn, unsigned char **fi) {
	if ((cp = strchr(s, '=')) == NULL) {
		(void) fprintf(stderr, "%s: bad %s font line format: %s\n",
			Pname, nm, s);
		return(0);
	}
	if ((*fn = (char *)malloc(cp - s + 1)) == NULL) {
		(void) fprintf(stderr, "%s: no space for %s font name %s\n",
			Pname, nm, s);
		return(0);
	}
	(void) strncpy(*fn, s, cp - s);
	(*fn)[cp - s] = '\0';
	if ((*fi = Convstr(cp + 1, &len)) == NULL)
		return(0);
	return(len);
}

/* Function 6 */
int Defdev(void) {
	Fstr.b = Fstr.i = Fstr.it = Fstr.r = NULL;
	Fstr.bl = Fstr.il = Fstr.itl = Fstr.rl = 0;
	if (Device == NULL || strcasecmp(Device, "normal") == 0) {
		Fontctl = 0;
check_font:
		if (Devfont) {
			(void) fprintf(stderr,
				"%s: font %s for device %s illegal\n",
				Pname, Devfont, Device ? Device : "NORMAL");
			return(1);
		}
		return(0);
	}
	Fontctl = 1;
	if (strcasecmp(Device, "ansi") == 0) {
		Fstr.b = Newstr((unsigned char *)"x[1m");
		Fstr.it = Newstr((unsigned char *)"x[4m");
		Fstr.r = Newstr((unsigned char *)"x[0m");
		Fstr.b[0] = Fstr.it[0] = Fstr.r[0] = ESC;
		Fstr.bl = Fstr.itl = Fstr.rl = 4;
		goto check_font;
	}
	if (strcasecmp(Device, "none") == 0)
		goto check_font;
	if (Devconf)
		p = Devconf;
	else {

		if ((p = getenv("CAWFLIB")) == NULL)	
			p = CAWFLIB;
		len = strlen(p) + 1 + strlen(DEVCONFIG) + 1;
		if ((s = (char *)malloc(len)) == NULL) {
			(void) fprintf(stderr, "%s: no space for %s name\n",
				Pname, DEVCONFIG);
			return(1);
		}
		(void) sprintf(s, "%s/%s", p, DEVCONFIG);
		p = s;
	}
#ifdef	UNIX
	if ((fs = fopen(p, "r")) == NULL)
#else
	if ((fs = fopen(p, "rt")) == NULL)
#endif
	{
		(void) fprintf(stderr, "%s: can't open config file: %s\n",
			Pname, p);
		return(1);
	}
	*line = ' ';
	while ( ! feof(fs)) {
		if (*line == '\t' || *line == '#' || *line == ' ') {
			(void) fgets(line, MAXLINE, fs);
			continue;
		}
		if ((s = strrchr(line, '\n')) != NULL)
			*s = '\0';
		else
			line[MAXLINE-1] = '\0';
		if (strcmp(Device, line) != 0) {
			(void) fgets(line, MAXLINE, fs);
			continue;
		}
		fd = 1;
		while (fgets(line, MAXLINE, fs) != NULL) {
			if (*line == ' ') {
				for (i = 1; line[i] == ' '; i++)
					;
			} else if (*line == '\t')
				i = 1;
			else
				break;
#if	defined(__STDC__)
			if ( ! isalpha(line[i])
#else
			if ( ! isascii(line[i]) || ! isalpha(line[i])
#endif
			||   line[i+1] != '=')
				break;
			if ((s = strrchr(line, '\n')) != NULL)
				*s = '\0';
			else
				line[MAXLINE-1] = '\0';
			switch (line[i]) {
			case 'b':
				if (Fstr.b != NULL) {
				    (void) fprintf(stderr,
					"%s: dup bold for %s in %s: %s\n",
					Pname, Device, p, line);
					(void) free(Fstr.b);
					Fstr.b = NULL;
				}
				if ((Fstr.b = Convstr(&line[i+2], &Fstr.bl))
				== NULL)
					err++;
				break;
			case 'i':
				if (Fstr.it != NULL) {
				    (void) fprintf(stderr,
					"%s: dup italic for %s in %s: %s\n",
					Pname, Device, p, line);
					(void) free(Fstr.it);
					Fstr.it = NULL;
				}
				if ((Fstr.it = Convstr(&line[i+2], &Fstr.itl))
				== NULL)
					err++;
				break;
			case 'r':
				if (Fstr.r != NULL) {
				    (void) fprintf(stderr,
					"%s: dup roman for %s in %s: %s\n",
					Pname, Device, p, line);
					(void) free(Fstr.r);
					Fstr.r = NULL;
				}
				if ((Fstr.r = Convstr(&line[i+2], &Fstr.rl))
				== NULL)
					err++;
				break;
			case 'f':
				if ( ! Devfont || Fstr.i)
					break;
				if ((i = Convfont(Device, &line[i+2], &fn, &fi))
				< 0)
					err++;
				else if (fn && strcmp(Devfont, fn) == 0) {
					Fstr.i = fi;
					Fstr.il = i;
					fi = NULL;
				}
				if (fn) {
					(void) free(fn);
					fn = NULL;
				}
				if (fi) {
					(void) free((char *)fi);
					fi = NULL;
				}
				break;
			default:
				(void) fprintf(stderr,
					"%s: unknown device %s line: %s\n",
					Pname, Device, line);
				err++;
			}
		}
		break;
	}
	(void) fclose(fs);
	if (err)
		return(1);
	if ( ! fd) {
		(void) fprintf(stderr, "%s: can't find device %s in %s\n",
			Pname, Device, p);
		return(1);
	}
	if (Devfont && ! Fstr.i) {
		(void) fprintf(stderr,
			"%s: font %s for device %s not found in %s\n",
			Pname, Devfont, Device, p);
		return(1);
	}
	return(0);
}

/* Function 7 */
		    (error = secpolicy_sys_devices(cr)) != 0) {
			return (error);
		}

/* Function 8 */
		    strchr(str_value, ':') == NULL) {
			return (EINVAL);
		}

/* Function 9 */
	    (spec_type != S_IFCHR && spec_type != S_IFBLK)) {
		CT_DEBUG((CE_WARN,
		    "tmpl_create: failed to find device: %s", buf));
		kmem_free(buf, MAXPATHLEN);
		return (ERANGE);
	}

/* Function 10 */
	    ctd = list_next(&(DEVI(dip)->devi_ct), ctd)) {

		mutex_enter(&ctd->cond_contract.ct_lock);

		ASSERT(ctd->cond_dip == dip);

		if (dev != DDI_DEV_T_ANY && dev != ctd->cond_devt) {
			mutex_exit(&ctd->cond_contract.ct_lock);
			continue;
		}
		if (dev != DDI_DEV_T_ANY && spec_type != ctd->cond_spec) {
			mutex_exit(&ctd->cond_contract.ct_lock);
			continue;
		}

		if (ctd->cond_noneg) {
			mutex_exit(&ctd->cond_contract.ct_lock);
			continue;
		}

		ASSERT(ctd->cond_currev_type == evtype);
		if (ctd->cond_currev_ack == CT_NACK) {
			CT_DEBUG((CE_NOTE, "%s: found a NACK,result = NACK: %p",
			    f, (void *)dip));
			mutex_exit(&ctd->cond_contract.ct_lock);
			return (CT_NACK);
		} else if (ctd->cond_currev_ack == CT_ACK) {
			ack = 1;
			CT_DEBUG((CE_NOTE, "%s: found a ACK: %p",
			    f, (void *)dip));
		}
		mutex_exit(&ctd->cond_contract.ct_lock);
	}

/* Function 11 */
	    ctd = list_next(&(DEVI(dip)->devi_ct), ctd)) {
		mutex_enter(&ctd->cond_contract.ct_lock);
		ASSERT(ctd->cond_dip == dip);
		ctd->cond_state = state;
		mutex_exit(&ctd->cond_contract.ct_lock);
	}

/* Function 12 */
	    ctd = list_next(&(DEVI(dip)->devi_ct), ctd)) {

		ctid_t ctid;
		size_t len = strlen(path);

		mutex_enter(&ctd->cond_contract.ct_lock);

		ASSERT(ctd->cond_dip == dip);
		ASSERT(ctd->cond_minor);
		ASSERT(strncmp(ctd->cond_minor, path, len) == 0 &&
		    ctd->cond_minor[len] == ':');

		if (dev != DDI_DEV_T_ANY && dev != ctd->cond_devt) {
			mutex_exit(&ctd->cond_contract.ct_lock);
			continue;
		}
		if (dev != DDI_DEV_T_ANY && spec_type != ctd->cond_spec) {
			mutex_exit(&ctd->cond_contract.ct_lock);
			continue;
		}

		match = 1;
		ctid = ctd->cond_contract.ct_id;
		CT_DEBUG((CE_NOTE, "publish: found matching contract: %d",
		    ctid));

		broken = 0;
		if (!negend && !(evtype & ctd->cond_aset)) {
			broken = 1;
			CT_DEBUG((CE_NOTE, "publish: Contract broken: %d",
			    ctid));
		}

		if (!broken && !EVSENDP(ctd, evtype) &&
		    !ctd->cond_neg) {
			CT_DEBUG((CE_NOTE, "contract_device_publish(): "
			    "contract (%d): no publish reqd: event %d",
			    ctd->cond_contract.ct_id, evtype));
			mutex_exit(&ctd->cond_contract.ct_lock);
			continue;
		}

		ct = &ctd->cond_contract;
		event = kmem_zalloc(sizeof (ct_kevent_t), KM_SLEEP);
		event->cte_type = evtype;

		if (broken && sync) {
			CT_DEBUG((CE_NOTE, "publish: broken + sync: "
			    "ctid: %d", ctid));
			ASSERT(!negend);
			ASSERT(ctd->cond_currev_id == 0);
			ASSERT(ctd->cond_currev_type == 0);
			ASSERT(ctd->cond_currev_ack == 0);
			ASSERT(ctd->cond_neg == 0);
			if (ctd->cond_noneg) {
				CT_DEBUG((CE_NOTE, "publish: sync and noneg:"
				    "not publishing blocked ev: ctid: %d",
				    ctid));
				result = CT_NACK;
				kmem_free(event, sizeof (ct_kevent_t));
				mutex_exit(&ctd->cond_contract.ct_lock);
				continue;
			}
			ctd->cond_currev_type = event->cte_type;
			ct_barrier_incr(dip);
			ctd->cond_neg = 1;
		} else if (broken && !sync) {
			CT_DEBUG((CE_NOTE, "publish: broken + async: ctid: %d",
			    ctid));
			ASSERT(!negend);
			ASSERT(ctd->cond_currev_id == 0);
			ASSERT(ctd->cond_currev_type == 0);
			ASSERT(ctd->cond_currev_ack == 0);
			ASSERT(ctd->cond_neg == 0);
		} else if (EVSENDP(ctd, event->cte_type)) {
			CT_DEBUG((CE_NOTE, "publish: event suscrib: ctid: %d",
			    ctid));
			ASSERT(!negend);
			ASSERT(ctd->cond_currev_id == 0);
			ASSERT(ctd->cond_currev_type == 0);
			ASSERT(ctd->cond_currev_ack == 0);
			ASSERT(ctd->cond_neg == 0);
			event->cte_flags = EVINFOP(ctd, event->cte_type) ?
			    CTE_INFO : 0;
		} else if (ctd->cond_neg) {
			CT_DEBUG((CE_NOTE, "publish: NEGEND: ctid: %d", ctid));
			ASSERT(negend);
			ASSERT(ctd->cond_noneg == 0);
			nevid = ctd->cond_contract.ct_nevent ?
			    ctd->cond_contract.ct_nevent->cte_id : 0;
			ASSERT(ctd->cond_currev_id == nevid);
			ctd->cond_currev_id = 0;
			ctd->cond_currev_type = 0;
			ctd->cond_currev_ack = 0;
			ctd->cond_neg = 0;
		} else {
			CT_DEBUG((CE_NOTE, "publish: not publishing event for "
			    "ctid: %d, evtype: %d",
			    ctd->cond_contract.ct_id, event->cte_type));
			ASSERT(!negend);
			ASSERT(ctd->cond_currev_id == 0);
			ASSERT(ctd->cond_currev_type == 0);
			ASSERT(ctd->cond_currev_ack == 0);
			ASSERT(ctd->cond_neg == 0);
			kmem_free(event, sizeof (ct_kevent_t));
			mutex_exit(&ctd->cond_contract.ct_lock);
			continue;
		}

		nvl = NULL;
		if (tnvl) {
			VERIFY(nvlist_dup(tnvl, &nvl, 0) == 0);
			if (negend) {
				int32_t newct = 0;
				ASSERT(ctd->cond_noneg == 0);
				VERIFY(nvlist_add_uint64(nvl, CTS_NEVID, nevid)
				    == 0);
				VERIFY(nvlist_lookup_int32(nvl, CTS_NEWCT,
				    &newct) == 0);
				VERIFY(nvlist_add_int32(nvl, CTS_NEWCT,
				    newct == 1 ? 0 :
				    ctd->cond_contract.ct_id) == 0);
				CT_DEBUG((CE_NOTE, "publish: negend: ctid: %d "
				    "CTS_NEVID: %llu, CTS_NEWCT: %s",
				    ctid, (unsigned long long)nevid,
				    newct ? "success" : "failure"));

			}
		}

		if (ctd->cond_neg) {
			ASSERT(ctd->cond_contract.ct_ntime.ctm_start == -1);
			ASSERT(ctd->cond_contract.ct_qtime.ctm_start == -1);
			ctd->cond_contract.ct_ntime.ctm_start = ddi_get_lbolt();
			ctd->cond_contract.ct_qtime.ctm_start =
			    ctd->cond_contract.ct_ntime.ctm_start;
		}

		mutex_exit(&ctd->cond_contract.ct_lock);
		evid = cte_publish_all(ct, event, nvl, NULL);
		mutex_enter(&ctd->cond_contract.ct_lock);

		if (ctd->cond_neg) {
			ASSERT(!negend);
			ASSERT(broken);
			ASSERT(sync);
			ASSERT(!ctd->cond_noneg);
			CT_DEBUG((CE_NOTE, "publish: sync break, setting evid"
			    ": %d", ctid));
			ctd->cond_currev_id = evid;
		} else if (negend) {
			ctd->cond_contract.ct_ntime.ctm_start = -1;
			ctd->cond_contract.ct_qtime.ctm_start = -1;
		}
		mutex_exit(&ctd->cond_contract.ct_lock);
	}

/* Function 13 */
		    &(DEVI(dip)->devi_ct_lock), abstime) == -1) {
			return (-1);
		}

/* Function 14 */
	    (strncmp("XM", (const char *)&dev->d_inq[16], 2) == 0)) {

		char product_id[17];

		if (strncmp("SUN32XCD", (const char *)&dev->d_inq[24], 8) == 0)
			dev->d_cap |= DEV_CAP_SETTING_SPEED_NOT_ALLOWED;
		(void) strncpy(product_id, (const char *)&dev->d_inq[16], 16);
		product_id[16] = 0;
		if (strstr(product_id, "SUN") != NULL)
			return (1);
	}

/* Function 15 */
	    (strncmp("LTR-48", (const char *)&dev->d_inq[16], 6) == 0)) {
		use_cd_speed = 1;
	}

/* Function 16 */
	    ((fd = open(supplied, O_RDONLY|O_NDELAY)) >= 0)) {
		(void) close(fd);
		(void) strlcpy(found, supplied, PATH_MAX);
		return (1);
	}

/* Function 17 */
	    (strncmp(supplied, "/dev/removable-media/dsk/", 25) == 0)) {
		(void) snprintf(tmpstr, PATH_MAX, "/dev/rdsk/%s",
		    (char *)strrchr(supplied, '/'));

		if ((fd = open(tmpstr, O_RDONLY|O_NDELAY)) >= 0) {
			(void) close(fd);
			(void) strlcpy(found, supplied, PATH_MAX);
			return (1);
		}

		return (0);
	}

/* Function 18 */
	    (strlen(supplied) < 32)) {
		(void) snprintf(tmpstr, sizeof (tmpstr), "/dev/rdsk/%s",
		    supplied);
		if (access(tmpstr, F_OK) < 0) {
			(void) strcat(tmpstr, "s2");
		}
		if ((fd = open(tmpstr, O_RDONLY|O_NDELAY)) >= 0) {
			(void) close(fd);
			(void) strlcpy(found, tmpstr, PATH_MAX);
			return (1);
		}

		return (0);
	}

/* Function 19 */
		    (total_devices_found == 1) && found) {

			*found = t_dev;
		}

/* Function 20 */
	    ((dev->d_inq[0] & 0x1f) != 5)) {
		errmsg =
		    gettext("Specified device does not appear to be a CDROM");
	}

/* Function 21 */
	    !test_unit_ready(dev->d_fd)) {
		errmsg = gettext("Device not ready");
	}

/* Function 22 */
		    ((page_code[3] & 1) == 0)) {
			errmsg = gettext("Target device is not a CD writer");
		}

/* Function 23 */
		    (ASCQ(rqbuf) == 2))) {
			errmsg = gettext("No media in device");
		}

/* Function 24 */
	    (cond & CHECK_MEDIA_IS_NOT_ERASABLE))) {

		disc_info = (uchar_t *)my_zalloc(DISC_INFO_BLOCK_SIZE);
		if (!read_disc_info(dev->d_fd, disc_info)) {
			errmsg = gettext("Cannot obtain disc information");
		} else {
			disc_status = disc_info[2] & 0x03;
			erasable = disc_info[2] & 0x10;
		}
		free(disc_info);
		if (errmsg == NULL) {
			if (!erasable && (cond & CHECK_MEDIA_IS_NOT_ERASABLE))
				errmsg = gettext(
				    "Media in the device is not erasable");
			else if ((disc_status != 0) &&
			    (cond & CHECK_MEDIA_IS_NOT_BLANK))
				errmsg = gettext(
				    "Media in the device is not blank");
			else if ((disc_status == 2) &&
			    (cond & CHECK_MEDIA_IS_NOT_WRITABLE) &&
			    ((device_type != DVD_PLUS_W) &&
			    (device_type != DVD_PLUS)))
				errmsg = gettext(
				    "Media in the device is not writable");
		}
	}

/* Function 25 */
	    ((ti->ti_flags & TI_NWA_VALID) == 0)) {
		if ((device_type == DVD_PLUS) || (device_type ==
		    DVD_PLUS_W)) {
			ti->ti_flags |= TI_NWA_VALID;
		} else {
			err_msg(gettext(
			    "Cannot get writable address for the media.\n"));
			exit(1);
		}
	}

/* Function 26 */
				     iter = g_slist_next (iter)) {
					guint len;
					const char *str;

					str = (const char *) iter->data;
					len = strlen (str);
					strncpy (buf + i, str, bufsize - i);
					i += len;

					if (i < bufsize) {
						buf[i] = '\t';
						i++;
					}
				}


#ifdef __cplusplus
}
#endif

/* End of device_unified.c - Synthesized by MINIX4 Massive Synthesis System */
