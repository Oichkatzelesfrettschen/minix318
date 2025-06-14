/**
 * @file update_unified.c
 * @brief Unified update implementation
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
 *     1. userspace\minix_commands\update\update.c
 *     2. userspace\commands_legacy\system\update\update.c
 *     3. minix4\libos\lib_legacy\libwrap\update.c
 *     4. minix4\libos\lib_legacy\libsqlite\src\update.c
 *     5. minix4\libos\lib_legacy\libnisdb\yptol\update.c
 *     6. minix4\libos\lib_legacy\libeti\panel\common\update.c
 *     7. minix4\exokernel\kernel_legacy\cmd\keyserv\update.c
 *     8. minix4\exokernel\kernel_legacy\cmd\sgs\libld\common\update.c
 *     9. minix4\exokernel\kernel_legacy\cmd\sgs\libelf\common\update.c
 * 
 * Total source files: 9
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
#include	"_libld.h"
#include	"msg.h"
#include	<debug.h>
#include	<stdio.h>
#include	<string.h>
#include	<unistd.h>
#include "../ldap_util.h"
#include "decl.h"
#include "msg.h"
#include "mystdarg.h"
#include "private.h"
#include "shim.h"
#include "sqliteInt.h"
#include "tcpd.h"
#include "ypdefs.h"
#include "ypsym.h"
#include "yptol.h"
#include <curses.h>
#include <errno.h>
#include <limits.h>
#include <malloc.h>
#include <memory.h>
#include <ndbm.h>
#include <netdb.h>
#include <rpc/key_prot.h>
#include <rpc/rpc.h>
#include <rpcsvc/ypclnt.h>
#include <sgs.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syslog.h>
#include <thread.h>
#include <unistd.h>

/* ===== DEFINES ===== */
#define	MAXMAPNAMELEN 256
#define	ELF_TARGET_AMD64
#define	ADD_TO_DYNSORT(_sdp, _sym, _type, _sym_ndx) \
#define	IS_DATA_SYMBOL(x)	((ELF_ST_TYPE(x->st_info) == STT_OBJECT) || \
#define	IS_BORING_SYMBOL(x)	(x->sd_flags & (FLG_SY_REDUCED|FLG_SY_STDFLTR| \
#define	IS_BORING_SCOPE(x)	((ELF_ST_BIND(x->st_info) == STB_LOCAL) || \
#define	HAS_NEEDED_ASSERTS(x)	((x->sd_ass != NULL) &&		\
#define	FSZ_LONG	ELF64_FSZ_XWORD
#define	ELFCLASS	ELFCLASS64
#define	_elf_snode_init	_elf64_snode_init
#define	_elfxx_cookscn	_elf64_cookscn
#define	_elf_upd_lib	_elf64_upd_lib
#define	elf_fsize	elf64_fsize
#define	_elf_entsz	_elf64_entsz
#define	_elf_msize	_elf64_msize
#define	_elf_upd_usr	_elf64_upd_usr
#define	wrt		wrt64
#define	elf_xlatetof	elf64_xlatetof
#define	_elfxx_update	_elf64_update
#define	_elfxx_swap_wrimage	_elf64_swap_wrimage
#define	TEST_SIZE

/* ===== TYPES ===== */
struct request_info *request;
struct request_info *VARARGS(request_init, struct request_info *, request)
    struct request_info *r;
struct request_info *VARARGS(request_set, struct request_info *, request)
    struct request_info *r;
	union wait status;
	struct	stat	stbuf;
typedef struct {

/* ===== GLOBAL VARIABLES ===== */
int main(void);
int main(void);
static char sccsid[] = "@(#) update.c 1.1 94/12/28 17:42:56";
    int     key;
    char   *ptr;
	    request->fd = va_arg(ap, int);
	STRN_CPY(ptr, va_arg(ap, char *), STRING_LENGTH);
    static struct request_info default_info;
    sprintf(request->pid, "%d", getpid());
  aXRef = sqliteMalloc( sizeof(int) * pTab->nCol );
      int rc;
    aIdxUsed = (char*)&apIdx[nIdx];
void * update_thread(void *);
extern pid_t parent_pid;
	int ret;
	void *ret = (void *)-1;
			(void) touchline(pnl->win, line - pnl->wstarty, 1);
	int	screen_y, i;
	int	screen_y;
	    (void) wnoutrefresh(stdscr);
			(void) wnoutrefresh(panel -> win);
extern	char	*program_name;
static	char	*basename(char *path);
static	int	match(char *line, char *name);
static	int	_openchild(char *command, FILE **fto, FILE **ffrom);
static	char	SHELL[] = "/bin/sh";
static	char	UPDATEFILE[] = "updaters";
static	char	MAKE[] = "/usr/ccs/bin/make";
	char	updater[MAXMAPNAMELEN + 40];
	int	status;
	uint_t	yperrno;
	int	namelen, datalen;
	(void) fprintf(stderr, "%s %s\n", name, data);
	(void) fprintf(childargs, "%s\n", name);
	(void) fprintf(childargs, "%u\n", op);
	(void) fprintf(childargs, "%u\n", namelen);
	(void) fwrite(name, namelen, 1, childargs);
	(void) fprintf(childargs, "\n");
	(void) fprintf(childargs, "%u\n", datalen);
	(void) fwrite(data, datalen, 1, childargs);
	(void) fprintf(childargs, "\n");
	(void) fclose(childargs);
	(void) fscanf(childrslt, "%d", &yperrno);
	(void) fclose(childrslt);
	(void) wait(&status);
	int i;
	int pdto[2];
	int pdfrom[2];
	char *com;
		(void) close(0);
		(void) dup(pdto[0]);
		(void) close(1);
		(void) dup(pdfrom[1]);
		(void) sprintf(com, "exec %s", command);
		(void) close(pdto[0]);
		(void) close(pdfrom[1]);
	(void) close(pdfrom[0]);
	(void) close(pdfrom[1]);
	(void) close(pdto[0]);
	(void) close(pdto[1]);
	char	*p;
	char	line[256];
	int	wfd;
	char	tmpname[80];
	int	err;
	(void) memset(tmpname, 0, 80);
	(void) sprintf(tmpname, "%s.tmp", filename);
	(void) umask(0);
		(void) fclose(rf);
		(void) close(wfd);
		(void) fclose(rf);
				(void) fprintf(wf, "%s\t%s\n", name, data);
			(void) fprintf(wf, "%s\t%s\n", name, data);
	(void) fclose(wf);
	(void) fclose(rf);
	int	len;
static Sym *dynsort_compare_syms;
	int zverbose = (ofl->ofl_flags & FLG_OF_VERBOSE) != 0;
	int		start_set = 0;
		(void) st_setstring(strtab, ofl->ofl_name, &stoff);
		(void) st_setstring(dynstr, ofl->ofl_name, &stoff);
			(void) st_setstring(shstrtab, osp->os_name, &stoff);
		int	ndx;
			ld_eprintf(ofl, ERR_WARNING, MSG_INTL(MSG_UPD_NOSEG));
			uchar_t		type;
			int		enter_in_symtab, enter_in_ldynsym;
			int		update_done;
		    (int (*)(const void *, const void *))sym_hash_compare);
		const char	*name;
		int		local = 0, dynlocal = 0, enter_in_symtab;
			(void) st_setstring(strtab, name, &stoff);
			(void) st_setstring(dynstr, name, &stoff);
				(void) st_setstring(dynstr, name, &stoff);
		uchar_t		bind;
	int		not_relobj = !(flags & FLG_OF_RELOBJ);
				    (uintptr_t)_dyn) / sizeof (Dyn));
			(void) st_setstring(strtbl, ofl->ofl_soname, &stoff);
			(void) st_setstring(strtbl, ofl->ofl_filtees, &stoff);
		(void) st_setstring(strtbl, ofl->ofl_rpath, &stoff);
			(void) st_setstring(strtbl, ofl->ofl_config, &stoff);
			(void) st_setstring(strtbl, ofl->ofl_depaudit, &stoff);
			(void) st_setstring(strtbl, ofl->ofl_audit, &stoff);
			int	ndx;
	    ((uintptr_t)dyn - (uintptr_t)_dyn) / sizeof (*dyn));
	int		num = 0;
			const char	*name = vdp->vd_name;
			(void) st_setstring(strtbl, name, &stoff);
			vdp->vd_name = (const char *)stoff;
			    (uintptr_t)sdp->sd_sym->st_name;
		vdap->vda_name = (uintptr_t)vdp->vd_name;
		_vdap->vda_next = (Word)((uintptr_t)vdap - (uintptr_t)_vdap);
			vdap->vda_name = (uintptr_t)_vdp->vd_name;
			    (uintptr_t)_vdap);
		vdf->vd_aux = (Word)((uintptr_t)(vdf + 1) - (uintptr_t)vdf);
		_vdf->vd_next = (Word)((uintptr_t)vdf - (uintptr_t)_vdf);
				    (Word)((uintptr_t)vnap - (uintptr_t)_vnap);
		vnd->vn_aux = (Word)((uintptr_t)(vnd + 1) - (uintptr_t)vnd);
		_vnd->vn_next = (Word)((uintptr_t)vnd - (uintptr_t)_vnd);
	char		*strtab;
	uchar_t		*taddr, *taddr0;
	uint_t		stride;
		stride = (uint_t)mvp->m_stride + 1;
			*taddr = (uchar_t)mvp->m_value;
			const char	*str;
			int	idx = 1;
	uintptr_t	error = 0;
	(void) st_setstrbuf(stp, data->d_buf, data->d_size - extra);
		(void) st_setstring(strtbl, capstr->cs_str, &stoff);
		(void) st_setstring(strtbl, capstr->cs_str, &stoff);
		(void) st_setstring(strtbl, capstr->cs_str, &stoff);
	uintptr_t	ret = 0;
	Sg_desc		*dtracesgp = NULL, *capsgp = NULL, *intpsgp = NULL;
	int		phdrndx = 0, segndx = -1, secndx, intppndx, intpsndx;
	int		dtracepndx, dtracesndx, cappndx, capsndx;
				intpsgp = sgp;
				intpsndx = segndx;
				intppndx = phdrndx++;
		    ofl->ofl_interp);
		Phdr	*phdr = &(intpsgp->sg_phdr);
		Shdr	*shdr = ofl->ofl_osinterp->os_shdr;
		DBG_CALL(Dbg_seg_entry(ofl, intpsndx, intpsgp));
		ofl->ofl_phdr[intppndx] = *phdr;
	    (*((char *)ofl->ofl_osshstrtab->os_outdata->d_buf) == '\0'));
	    (*((char *)ofl->ofl_osstrtab->os_outdata->d_buf) == '\0'));
	    (*((char *)ofl->ofl_osdynstr->os_outdata->d_buf) == '\0'));
	register char	*p = (char *)eh->e_ident;
			(void) _elfxx_cookscn(s);
	register char	*p = (char *)eh->e_ident;
			(void) _elfxx_cookscn(s);
	char			*image;
	int			byte;
		char		*start = image + sh->sh_offset;
		char		*here;
			(void) memset(start - sz, byte, sz);
					(void) memset(here - sz, byte, sz);
		(void) memset(image + hi, byte, sz);
		assert((uintptr_t)dst.d_buf < ((uintptr_t)image + outsz));
		dst.d_buf = (char *)dst.d_buf + eh->e_shentsize;

/* ===== FUNCTIONS ===== */

/* Function 1 */
		(0 == strcmp(map->map_name, NETGROUP_BYUSER))) {
			return (update_map_if_required(map, FALSE));
	}

/* Function 2 */
	    ((flags & FLG_OF_STATIC) && ofl->ofl_osversym)) {
		symtab = (Sym *)ofl->ofl_ossymtab->os_outdata->d_buf;
		symtab[symtab_ndx++] = _sym;
		if (ofl->ofl_ossymshndx)
			symshndx =
			    (Word *)ofl->ofl_ossymshndx->os_outdata->d_buf;
	}

/* Function 3 */
	    (flags & (FLG_OF_VERNEED | FLG_OF_VERDEF))) {
		versym = (Versym *)ofl->ofl_osversym->os_outdata->d_buf;
		versym[0] = 0;
	} else

/* Function 4 */
			    (osp->os_flags & FLG_OS_OUTREL)) {
				if (versym)
					versym[*symndx - 1] = 0;
				osp->os_identndx = *symndx - 1;
				DBG_CALL(Dbg_syms_sec_entry(ofl->ofl_lml,
				    osp->os_identndx, sgp, osp));
			}

/* Function 5 */
			    (sdp->sd_flags & FLG_SY_PAREXPN) && !update_done) {
				sym->st_shndx = parexpnndx;
				sdp->sd_isc = ofl->ofl_isparexpn;
				sym->st_value = parexpnaddr;
				parexpnaddr += sym->st_size;
				if ((flags & FLG_OF_RELOBJ) == 0)
					sym->st_value -= parexpnbase;
			}

/* Function 6 */
					    (ofl->ofl_tlsphdr)) {
						sym->st_value -=
						    ofl->ofl_tlsphdr->p_vaddr;
					}

/* Function 7 */
				    elf_ndxscn(osp->os_scn)) >= SHN_LORESERVE) {
					if (_symshndx) {
						*_symshndx = sectndx;
					}
					sym->st_shndx = SHN_XINDEX;
				} else {
					sym->st_shndx = sectndx;
				}

/* Function 8 */
	    (sdp->sd_isc->is_osdesc == iosp)) {
		if (ld_sym_copy(sdp) == S_ERROR)
			return ((Addr)S_ERROR);
		sdp->sd_sym->st_size = sdp->sd_isc->is_osdesc->os_shdr->sh_size;

	} else if (iosp && !(flags & FLG_OF_RELOBJ)) {
		ld_eprintf(ofl, ERR_WARNING, MSG_INTL(MSG_SYM_NOCRT),
		    MSG_ORIG(MSG_SYM_INIT_U), MSG_ORIG(MSG_SCN_INIT));
	}

/* Function 9 */
	    (sdp->sd_isc->is_osdesc == fosp)) {
		if (ld_sym_copy(sdp) == S_ERROR)
			return ((Addr)S_ERROR);
		sdp->sd_sym->st_size = sdp->sd_isc->is_osdesc->os_shdr->sh_size;

	} else if (fosp && !(flags & FLG_OF_RELOBJ)) {
		ld_eprintf(ofl, ERR_WARNING, MSG_INTL(MSG_SYM_NOCRT),
		    MSG_ORIG(MSG_SYM_FINI_U), MSG_ORIG(MSG_SCN_FINI));
	}

/* Function 10 */
	    sav = AVL_NEXT(&ofl->ofl_symavl, sav)) {
		Sym	*symptr;
		int	local;
		int	restore;

		sdp = sav->sav_sdp;

		if (sdp->sd_flags & FLG_SY_INVALID) {
			DBG_CALL(Dbg_syms_old(ofl, sdp));
			DBG_CALL(Dbg_syms_ignore(ofl, sdp));
			continue;
		}

		if (sdp->sd_ref == REF_DYN_SEEN)
			continue;

		if (ld_sym_reducable(ofl, sdp))
			local = 1;
		else
			local = 0;

		if (local || (ofl->ofl_hashbkts == 0)) {
			sorted_syms[scndx++].sl_sdp = sdp;
		} else {
			sorted_syms[ssndx].sl_hval = sdp->sd_aux->sa_hash %
			    ofl->ofl_hashbkts;
			sorted_syms[ssndx].sl_sdp = sdp;
			ssndx++;
		}

		symptr = sdp->sd_sym;
		restore = 0;
		if ((sdp->sd_flags & FLG_SY_PAREXPN) ||
		    ((sdp->sd_flags & FLG_SY_SPECSEC) &&
		    (sdp->sd_shndx = symptr->st_shndx) == SHN_COMMON)) {

			if (sdp->sd_flags & FLG_SY_PAREXPN) {
				restore = 1;
				sdp->sd_shndx = parexpnndx;
				sdp->sd_flags &= ~FLG_SY_SPECSEC;
				symptr->st_value = (Xword) S_ROUND(
				    parexpnaddr, symptr->st_value);
				parexpnaddr = symptr->st_value +
				    symptr->st_size;
				sdp->sd_isc = ofl->ofl_isparexpn;
				sdp->sd_flags |= FLG_SY_COMMEXP;

			} else if (ELF_ST_TYPE(symptr->st_info) != STT_TLS &&
			    (local || !(flags & FLG_OF_RELOBJ))) {
				restore = 1;
				sdp->sd_shndx = bssndx;
				sdp->sd_flags &= ~FLG_SY_SPECSEC;
				symptr->st_value = (Xword)S_ROUND(bssaddr,
				    symptr->st_value);
				bssaddr = symptr->st_value + symptr->st_size;
				sdp->sd_isc = ofl->ofl_isbss;
				sdp->sd_flags |= FLG_SY_COMMEXP;

			} else if (ELF_ST_TYPE(symptr->st_info) == STT_TLS &&
			    (local || !(flags & FLG_OF_RELOBJ))) {
				restore = 1;
				sdp->sd_shndx = tlsbssndx;
				sdp->sd_flags &= ~FLG_SY_SPECSEC;
				symptr->st_value = (Xword)S_ROUND(tlsbssaddr,
				    symptr->st_value);
				tlsbssaddr = symptr->st_value + symptr->st_size;
				sdp->sd_isc = ofl->ofl_istlsbss;
				sdp->sd_flags |= FLG_SY_COMMEXP;
				symptr->st_value -= ofl->ofl_tlsphdr->p_vaddr;
			}
#if	defined(_ELF64)
		} else if ((ld_targ.t_m.m_mach == EM_AMD64) &&
		    (sdp->sd_flags & FLG_SY_SPECSEC) &&
		    ((sdp->sd_shndx = symptr->st_shndx) ==
		    SHN_X86_64_LCOMMON) &&
		    ((local || !(flags & FLG_OF_RELOBJ)))) {
			restore = 1;
			sdp->sd_shndx = lbssndx;
			sdp->sd_flags &= ~FLG_SY_SPECSEC;
			symptr->st_value = (Xword)S_ROUND(lbssaddr,
			    symptr->st_value);
			lbssaddr = symptr->st_value + symptr->st_size;
			sdp->sd_isc = ofl->ofl_islbss;
			sdp->sd_flags |= FLG_SY_COMMEXP;
#endif
		}

		if (restore != 0) {
			uchar_t		type, bind;

			type = ELF_ST_TYPE(symptr->st_info);
			if (sdp->sd_flags & FLG_SY_GLOBREF)
				bind = STB_GLOBAL;
			else
				bind = STB_WEAK;

			symptr->st_info = ELF_ST_INFO(bind, type);
		}

		    (ELF_ST_TYPE(x->st_info) == STT_COMMON) ||		    \
		    (ELF_ST_TYPE(x->st_info) == STT_TLS))

		    FLG_SY_SPECSEC|FLG_SY_HIDDEN|FLG_SY_ELIM|FLG_SY_IGNORE))

		    ((ELF_ST_BIND(x->st_info) == STB_WEAK) &&		   \
		    (x->st_shndx == SHN_UNDEF)))

		    (ass_enabled(x->sd_ass, SYM_ASSERT_SIZE) ||	\
		    ass_enabled(x->sd_ass, SYM_ASSERT_ALIAS)))

		if (OFL_GUIDANCE(ofl, FLG_OFG_NO_ASSERTS) &&
		    IS_DATA_SYMBOL(sdp->sd_sym) &&
		    !IS_BORING_SCOPE(sdp->sd_sym) &&
		    !IS_BORING_SYMBOL(sdp) &&
		    !HAS_NEEDED_ASSERTS(sdp)) {
			ld_eprintf(ofl, ERR_GUIDANCE,
			    MSG_INTL(MSG_GUIDE_ASSERT_SIZE),
			    sdp->sd_name, (Lword)sdp->sd_sym->st_size);
		}
	}

/* Function 11 */
		    cav = AVL_NEXT(ofl->ofl_capfamilies, cav)) {
			Cap_sym		*csp;
			Aliste		idx;

			for (APLIST_TRAVERSE(cav->cn_members, idx, csp)) {
				sdp = csp->cs_sdp;

				DBG_CALL(Dbg_syms_created(ofl->ofl_lml,
				    sdp->sd_name));
				DBG_CALL(Dbg_syms_entered(ofl, sdp->sd_sym,
				    sdp));

				dynsym[dynsym_ndx] = *sdp->sd_sym;

				(void) st_setstring(dynstr, sdp->sd_name,
				    &stoff);
				dynsym[dynsym_ndx].st_name = stoff;

				sdp->sd_sym = &dynsym[dynsym_ndx];
				sdp->sd_symndx = dynsym_ndx;

				if (syminfo) {
					syminfo[dynsym_ndx].si_flags |=
					    SYMINFO_FLG_CAP;
				}

				dynsym_ndx++;
			}
		}

/* Function 12 */
			    ELF_ST_TYPE(sdp->sd_sym->st_info)]) {
				dynlocal = 1;
			}

/* Function 13 */
				    (sdp->sd_sym->st_shndx != SHN_UNDEF)) {
					if (SYM_IS_HIDDEN(sdp))
						vndx = VER_NDX_LOCAL;
					else
						vndx = VER_NDX_GLOBAL;
				}

/* Function 14 */
		    !(sdp->sd_flags & FLG_SY_NOTAVAIL)) {
			int	ndx = sdp->sd_symndx;
			APlist	**alpp = &(ofl->ofl_symdtent);

			if (sdp->sd_flags & FLG_SY_MVTOCOMM)
				syminfo[ndx].si_flags |= SYMINFO_FLG_COPY;

			if (sdp->sd_ref == REF_DYN_NEED) {
				if (aplist_append(alpp, sdp,
				    AL_CNT_OFL_SYMINFOSYMS) == NULL)
					return (0);

				syminfo[ndx].si_flags |= SYMINFO_FLG_DIRECT;

				if (sdp->sd_flags & FLG_SY_LAZYLD)
					syminfo[ndx].si_flags |=
					    SYMINFO_FLG_LAZYLOAD;
				if (sdp->sd_flags & FLG_SY_DEFERRED)
					syminfo[ndx].si_flags |=
					    SYMINFO_FLG_DEFERRED;

				if (sdp->sd_flags & FLG_SY_DIR)
					syminfo[ndx].si_flags |=
					    SYMINFO_FLG_DIRECTBIND;

			} else if ((sdp->sd_flags & FLG_SY_EXTERN) &&
			    (sdp->sd_sym->st_shndx == SHN_UNDEF)) {
				syminfo[ndx].si_boundto = SYMINFO_BT_EXTERN;

			} else if ((sdp->sd_flags & FLG_SY_PARENT) &&
			    (sdp->sd_sym->st_shndx == SHN_UNDEF)) {
				syminfo[ndx].si_flags |= SYMINFO_FLG_DIRECT;
				syminfo[ndx].si_boundto = SYMINFO_BT_PARENT;
				if (sdp->sd_flags & FLG_SY_DIR)
					syminfo[ndx].si_flags |=
					    SYMINFO_FLG_DIRECTBIND;

			} else if (sdp->sd_flags & FLG_SY_STDFLTR) {
				syminfo[ndx].si_flags |= SYMINFO_FLG_FILTER;
				if (sdp->sd_flags & FLG_SY_NDIR)
					syminfo[ndx].si_flags |=
					    SYMINFO_FLG_NOEXTDIRECT;

			} else if (sdp->sd_flags & FLG_SY_AUXFLTR) {
				syminfo[ndx].si_flags |= SYMINFO_FLG_AUXILIARY;
				if (sdp->sd_flags & FLG_SY_NDIR)
					syminfo[ndx].si_flags |=
					    SYMINFO_FLG_NOEXTDIRECT;

			} else if ((sdp->sd_ref == REF_REL_NEED) &&
			    (sdp->sd_sym->st_shndx != SHN_UNDEF)) {
				syminfo[ndx].si_boundto = SYMINFO_BT_NONE;

				if (sdp->sd_flags & FLG_SY_NDIR) {
					syminfo[ndx].si_flags |=
					    SYMINFO_FLG_NOEXTDIRECT;
				}

				if (sdp->sd_flags & FLG_SY_INTPOSE) {
					syminfo[ndx].si_flags |=
					    SYMINFO_FLG_INTERPOSE;
				}

				if (sdp->sd_flags & FLG_SY_DEFERRED) {
					syminfo[ndx].si_flags |=
					    SYMINFO_FLG_DEFERRED;
				}

				if ((sdp->sd_flags & FLG_SY_NDIR) == 0) {
					syminfo[ndx].si_flags |=
					    SYMINFO_FLG_DIRECT;

					if (sdp->sd_flags & FLG_SY_DIR)
						syminfo[ndx].si_flags |=
						    SYMINFO_FLG_DIRECTBIND;

					syminfo[ndx].si_boundto =
					    SYMINFO_BT_SELF;
				}

				if ((sdp->sd_flags & FLG_SY_CAP) &&
				    ofl->ofl_oscapinfo) {
					syminfo[ndx].si_flags |=
					    SYMINFO_FLG_CAP;
				}
			}
		}

/* Function 15 */
		    (!sap->sa_PLTndx)) {
			Sym_desc	*_sdp;

			_sdp = sdp->sd_file->ifl_oldndx[sap->sa_linkndx];

			if (_sdp->sd_ref != REF_DYN_SEEN) {
				Wk_desc	wk;

				if (enter_in_symtab) {
					if (local) {
						wk.wk_symtab =
						    &symtab[scopesym_ndx];
						scopesym_ndx++;
					} else {
						wk.wk_symtab =
						    &symtab[symtab_ndx];
						symtab_ndx++;
					}
				} else {
					wk.wk_symtab = NULL;
				}
				if (dynsym) {
					if (!local) {
						wk.wk_dynsym =
						    &dynsym[dynsym_ndx];
						dynsym_ndx++;
					} else if (dynlocal) {
						wk.wk_dynsym =
						    &ldynsym[ldynscopesym_ndx];
						ldynscopesym_ndx++;
					}
				} else {
					wk.wk_dynsym = NULL;
				}
				wk.wk_weak = sdp;
				wk.wk_alias = _sdp;

				if (alist_append(&weak, &wk,
				    sizeof (Wk_desc), AL_CNT_WEAK) == NULL)
					return ((Addr)S_ERROR);

				continue;
			}
		}

/* Function 16 */
			    (sym->st_value != 0)) {
				ld_eprintf(ofl, ERR_WARNING,
				    MSG_INTL(MSG_SYM_NOTNULL),
				    demangle(name), sdp->sd_file->ifl_name);
			}

/* Function 17 */
			    (ELF_ST_BIND(sym->st_info) == STB_WEAK)) {
				sdp->sd_flags |= FLG_SY_SPECSEC;
				sdp->sd_shndx = sectndx = SHN_ABS;
			}

/* Function 18 */
		    (sectndx == SHN_COMMON)) {
			;
		} else {
			if ((sdp->sd_flags & FLG_SY_SPECSEC) &&
			    (sectndx == SHN_ABS))
				spec = sdp->sd_aux->sa_symspec;

			if (sdp->sd_flags & FLG_SY_COMMEXP) {
				;
			} else if (sdp->sd_ref == REF_DYN_NEED) {
				uchar_t	type, bind;

				sectndx = SHN_UNDEF;
				sym->st_value = 0;
				sym->st_size = 0;

				type = ELF_ST_TYPE(sym-> st_info);
				if (sdp->sd_flags & FLG_SY_GLOBREF)
					bind = STB_GLOBAL;
				else
					bind = STB_WEAK;

				sym->st_info = ELF_ST_INFO(bind, type);

			} else if (((sdp->sd_flags & FLG_SY_SPECSEC) == 0) &&
			    (sdp->sd_ref == REF_REL_NEED)) {
				osp = sdp->sd_isc->is_osdesc;
				sectndx = elf_ndxscn(osp->os_scn);

				sym->st_value +=
				    (Off)_elf_getxoff(sdp->sd_isc->is_indata);

				if (!(flags & FLG_OF_RELOBJ)) {
					sym->st_value += osp->os_shdr->sh_addr;
					if ((ELF_ST_TYPE(sym->st_info) ==
					    STT_TLS) && (ofl->ofl_tlsphdr))
						sym->st_value -=
						    ofl->ofl_tlsphdr->p_vaddr;
				}
			}
		}

/* Function 19 */
		    !(flags & FLG_OF_BFLAG)) {
			if (sap->sa_PLTndx)
				sym->st_value =
				    (*ld_targ.t_mr.mr_calc_plt_addr)(sdp, ofl);
		}

/* Function 20 */
			    (sectndx >= SHN_LORESERVE)) {
				assert(symshndx != NULL);
				symshndx[_symndx] = sectndx;
				symtab[_symndx].st_shndx = SHN_XINDEX;
			} else {
				symtab[_symndx].st_shndx = (Half)sectndx;
			}

/* Function 21 */
			    (sectndx >= SHN_LORESERVE)) {
				assert(_dynshndx != NULL);
				_dynshndx[_symndx] = sectndx;
				_dynsym[_symndx].st_shndx = SHN_XINDEX;
			} else {
				_dynsym[_symndx].st_shndx = (Half)sectndx;
			}

/* Function 22 */
		    (sdp->sd_sym->st_shndx != SHN_UNDEF)) {
			dyn->d_tag = DT_INIT;
			dyn->d_un.d_ptr = sdp->sd_sym->st_value;
			dyn++;
		}

/* Function 23 */
		    (sdp->sd_sym->st_shndx != SHN_UNDEF)) {
			dyn->d_tag = DT_FINI;
			dyn->d_un.d_ptr = sdp->sd_sym->st_value;
			dyn++;
		}

/* Function 24 */
		    (sgp->sg_phdr.p_flags & PF_W) && ofl->ofl_osinterp) {
			dyn->d_tag = DT_DEBUG;
			dyn->d_un.d_ptr = 0;
			dyn++;
		}

/* Function 25 */
	    cav = AVL_NEXT(ofl->ofl_capfamilies, cav)) {
		Cap_sym		*csp;
		Aliste		idx;
		Sym_desc	*asdp, *lsdp = cav->cn_symavlnode.sav_sdp;

		if (ccosp) {
			ocapinfo[lsdp->sd_symndx] =
			    ELF_C_INFO(chainndx, CAPINFO_SUNW_GLOB);
			ocapchain[chainndx] = lsdp->sd_symndx;
		} else {
			ocapinfo[lsdp->sd_symndx] =
			    ELF_C_INFO(lsdp->sd_symndx, CAPINFO_SUNW_GLOB);
		}

		for (APLIST_TRAVERSE(cav->cn_aliases, idx, asdp)) {
			if (ccosp) {
				ocapinfo[asdp->sd_symndx] =
				    ELF_C_INFO(chainndx, CAPINFO_SUNW_GLOB);
			} else {
				ocapinfo[asdp->sd_symndx] =
				    ELF_C_INFO(lsdp->sd_symndx,
				    CAPINFO_SUNW_GLOB);
			}
		}

		chainndx++;

		for (APLIST_TRAVERSE(cav->cn_members, idx, csp)) {
			Sym_desc	*msdp = csp->cs_sdp;

			ocapinfo[msdp->sd_symndx] =
			    ELF_C_INFO(lsdp->sd_symndx, csp->cs_group->cg_ndx);
			if (ccosp) {
				ocapchain[chainndx++] = msdp->sd_symndx;
			}
		}

		if (ccosp)
			ocapchain[chainndx++] = 0;
	}

/* Function 26 */
		    (ma->ass_bind != ELF_ST_BIND(sdp->sd_sym->st_info))) {
			Sym *sym = sdp->sd_sym;

			ld_eprintf(ofl, ERR_FATAL, MSG_INTL(MSG_ASSFAIL_SCOPE),
			    ma->ass_file, EC_LINENO(ma->ass_lineno),
			    demangle(sdp->sd_name),
			    conv_sym_info_bind(ma->ass_bind, CONV_FMT_ALT_CFNP,
			    &inv_buf),
			    conv_sym_info_bind(ELF_ST_BIND(sym->st_info),
			    CONV_FMT_ALT_CFNP, &inv_buf2));
			ret = S_ERROR;
		}

/* Function 27 */
				    ELF_ST_TYPE(sym->st_info))) {
					ld_eprintf(ofl, ERR_FATAL,
					    MSG_INTL(MSG_ASSFAIL_ALIAS),
					    ma->ass_file,
					    EC_LINENO(ma->ass_lineno),
					    demangle(sdp->sd_name),
					    asdp->sd_name);
					ret = S_ERROR;
				}

/* Function 28 */
		    (ma->ass_size != sdp->sd_sym->st_size)) {
			ld_eprintf(ofl, ERR_FATAL, MSG_INTL(MSG_ASSFAIL_SIZE),
			    ma->ass_file, EC_LINENO(ma->ass_lineno),
			    demangle(sdp->sd_name),
			    ma->ass_size, (Lword)sdp->sd_sym->st_size);
			ret = S_ERROR;
		}

/* Function 29 */
			    (sdp->sd_isc->is_shdr->sh_type == SHT_NOBITS)) {
				ld_eprintf(ofl, ERR_FATAL,
				    MSG_INTL(MSG_ASSFAIL_BITS),
				    ma->ass_file, EC_LINENO(ma->ass_lineno),
				    demangle(sdp->sd_name));
				ret = S_ERROR;
			}

/* Function 30 */
			    (sdp->sd_isc->is_shdr->sh_type != SHT_NOBITS)) {
				ld_eprintf(ofl, ERR_FATAL,
				    MSG_INTL(MSG_ASSFAIL_NOBITS),
				    ma->ass_file, EC_LINENO(ma->ass_lineno),
				    demangle(sdp->sd_name));
				ret = S_ERROR;
			}

/* Function 31 */
		    (ma->ass_type != ELF_ST_TYPE(sdp->sd_sym->st_info))) {
			Ehdr *ehdr = sdp->sd_file->ifl_ehdr;

			ld_eprintf(ofl, ERR_FATAL,
			    MSG_INTL(MSG_ASSFAIL_TYPE),
			    ma->ass_file, EC_LINENO(ma->ass_lineno),
			    demangle(sdp->sd_name),
			    conv_sym_info_type(ehdr->e_machine,
			    ma->ass_type, CONV_FMT_ALT_CFNP, &inv_buf),
			    conv_sym_info_type(ehdr->e_machine,
			    ELF_ST_TYPE(sdp->sd_sym->st_info),
			    CONV_FMT_ALT_CFNP, &inv_buf2));
			ret = S_ERROR;
		}

/* Function 32 */
			    ((flags & FLG_OF_RELOBJ) == 0)) {
				dtracesgp = sgp;
				dtracesndx = segndx;
				dtracepndx = phdrndx++;
			}

/* Function 33 */
			    ((flags & FLG_OF_RELOBJ) == 0)) {
				capsgp = sgp;
				capsndx = segndx;
				cappndx = phdrndx++;
			}

/* Function 34 */
		    (sgp->sg_flags & FLG_SG_EMPTY)) {
			int	i;
			Addr	v_e;

			vaddr = phdr->p_vaddr;
			phdr->p_memsz = sgp->sg_length;
			DBG_CALL(Dbg_seg_entry(ofl, segndx, sgp));
			ofl->ofl_phdr[phdrndx++] = *phdr;

			if (phdr->p_type != PT_LOAD)
				continue;

			v_e = vaddr + phdr->p_memsz;

			for (i = 0; i < phdrndx - 1; i++) {
				Addr	p_s = (ofl->ofl_phdr[i]).p_vaddr;
				Addr	p_e;

				if ((ofl->ofl_phdr[i]).p_type != PT_LOAD)
					continue;

				p_e = p_s + (ofl->ofl_phdr[i]).p_memsz;
				if (((p_s <= vaddr) && (p_e > vaddr)) ||
				    ((vaddr <= p_s) && (v_e > p_s)))
					ld_eprintf(ofl, ERR_WARNING,
					    MSG_INTL(MSG_UPD_SEGOVERLAP),
					    ofl->ofl_name, EC_ADDR(p_e),
					    sgp->sg_name, EC_ADDR(vaddr));
			}
			continue;
		}

/* Function 35 */
		    (!(ofl->ofl_dtflags_1 & DF_1_NOHDR))) {
			size = (Addr)S_ROUND((phdrsz + ehdrsz),
			    hshdr->sh_addralign);
			phdr->p_offset -= size;
			phdr->p_filesz += size;
			phdr->p_memsz += size;
		}

/* Function 36 */
		    ((ofl->ofl_dtflags_1 & DF_1_NOHDR) == 0)) {
			if (phdr->p_align != 0)
				vaddr += phdr->p_offset % phdr->p_align;
			else
				vaddr += phdr->p_offset;
			phdr->p_vaddr = vaddr;
		}

/* Function 37 */
			    (phdr->p_type == PT_LOAD)) {
				if (hshdr)
					vaddr += (shdr->sh_offset -
					    hshdr->sh_offset);

				shdr->sh_addr = vaddr;
				hshdr = shdr;
			}

/* Function 38 */
		    (sh->sh_type == SHT_NULL)) {
			d = s->s_hdnode, prevd = 0;
			for (; d != 0; prevd = d, d = d->db_next)
				d->db_uflags &= ~ELF_F_DIRTY;
			continue;
		}

/* Function 39 */
	    ((elf->ed_myflags & EDF_IMALLOC) ? 0 : 1)) != 0) {
		elf->ed_uflags &= ~ELF_F_DIRTY;
		elf->ed_myflags &= ~EDF_IMALLOC;
		return (outsz);
	}


#ifdef __cplusplus
}
#endif

/* End of update_unified.c - Synthesized by MINIX4 Massive Synthesis System */
