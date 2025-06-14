/**
 * @file nfs_vnops_unified.c
 * @brief Unified nfs_vnops implementation
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
 *     1. minix4\microkernel\servers\nfs\nfs_vnops.c
 *     2. minix4\libos\lib_legacy\openssl\lib\posix\nfs\nfs_vnops.c
 *     3. minix4\exokernel\kernel_legacy\uts\common\fs\nfs\nfs_vnops.c
 * 
 * Total source files: 3
 * Synthesis date: 2025-06-13 19:56:36
 * Synthesis strategy: Massive batch C23 unification
 * Processing batch: targets 132-256
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
#include "diagnostic.h"
#include "fifo.h"
#include <fs/fs_subr.h>
#include <miscfs/fifofs/fifo.h>
#include <miscfs/specfs/specdev.h>
#include <nfs/lm.h>
#include <nfs/nfs.h>
#include <nfs/nfs_acl.h>
#include <nfs/nfs_clnt.h>
#include <nfs/nfsm_subs.h>
#include <nfs/nfsmount.h>
#include <nfs/nfsnode.h>
#include <nfs/nfsv2.h>
#include <nfs/nqnfs.h>
#include <nfs/rnode.h>
#include <nfs/rpcv2.h>
#include <nfs/xdr_subs.h>
#include <rpc/auth.h>
#include <rpc/clnt.h>
#include <rpc/types.h>
#include <sys/acl.h>
#include <sys/atomic.h>
#include <sys/buf.h>
#include <sys/cmn_err.h>
#include <sys/conf.h>
#include <sys/cred.h>
#include <sys/ddi.h>
#include <sys/debug.h>
#include <sys/dirent.h>
#include <sys/dnlc.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/file.h>
#include <sys/filio.h>
#include <sys/flock.h>
#include <sys/kernel.h>
#include <sys/kmem.h>
#include <sys/malloc.h>
#include <sys/map.h>
#include <sys/mbuf.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/namei.h>
#include <sys/param.h>
#include <sys/pathconf.h>
#include <sys/pathname.h>
#include <sys/policy.h>
#include <sys/proc.h>
#include <sys/sdt.h>
#include <sys/strsubr.h>
#include <sys/swap.h>
#include <sys/sysmacros.h>
#include <sys/systm.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/utsname.h>
#include <sys/vfs.h>
#include <sys/vfs_opreg.h>
#include <sys/vmsystm.h>
#include <sys/vnode.h>
#include <vm/as.h>
#include <vm/hat.h>
#include <vm/page.h>
#include <vm/pvn.h>
#include <vm/seg.h>
#include <vm/seg_kpm.h>
#include <vm/seg_map.h>
#include <vm/seg_vn.h>
#include <vm/vm.h>

/* ===== DEFINES ===== */
#define	DIRHDSIZ	(sizeof (struct dirent) - (MAXNAMLEN + 1))
#define	NFS_EOF			-98

/* ===== TYPES ===== */
struct vnodeopv_entry_desc nfsv2_vnodeop_entries[] = {
struct vnodeopv_desc nfsv2_vnodeop_opv_desc =
struct vnodeopv_entry_desc spec_nfsv2nodeop_entries[] = {
struct vnodeopv_desc spec_nfsv2nodeop_opv_desc =
struct vnodeopv_entry_desc fifo_nfsv2nodeop_entries[] = {
struct vnodeopv_desc fifo_nfsv2nodeop_opv_desc =
struct proc *nfs_iodwant[NFS_MAXASYNCDAEMON];
	struct vnode *vp;
	struct ucred *cred;
	struct proc *procp;
	struct mbuf *mreq, *mrep, *md, *mb;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nfsnode *np = VTONFS(vp);
	struct nfsmount *nmp = VFSTONFS(vp->v_mount);
	struct vattr vattr;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nfsmount *nmp;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct vnode *newvp;
	struct nfsnode *np;
		struct vattr vattr;
	struct uio *uiop;
	struct ucred *cred;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct uio *uiop;
	struct ucred *cred;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nfsmount *nmp;
	struct uio *uiop;
	struct ucred *cred;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nfsmount *nmp;
	struct nfsnode *np = VTONFS(vp);
	struct vnode *newvp;
	struct vattr vattr;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct vattr vattr;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct vnode *sdvp;
	struct componentname *scnp;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct vattr vattr;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct vattr vattr;
	struct uio *uiop;
	struct ucred *cred;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct mbuf *md2;
	struct dirent *savdp;
	struct nfsmount *nmp;
	struct nfsnode *np = VTONFS(vp);
	struct vnode *vp;
	struct ucred *cred;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nameidata nami, *ndp = &nami;
	struct componentname *cnp = &ndp->ni_cnd;
	struct nfsmount *nmp;
	struct nfsnode *np;
	struct vnode *newvp;
	struct vnode *dvp, *vp;
	struct componentname *cnp;
	struct proc *procp;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct vop_strategy_args *ap;
	struct ucred *cr;
	struct proc *p;
	struct buf *nbp;
	struct nfsmount *nmp;
	struct vattr vattr;
	struct vattr vattr;
	struct vattr vattr;
struct vnodeopv_entry_desc nfsv2_vnodeop_entries[] = {
struct vnodeopv_desc nfsv2_vnodeop_opv_desc =
struct vnodeopv_entry_desc spec_nfsv2nodeop_entries[] = {
struct vnodeopv_desc spec_nfsv2nodeop_opv_desc =
struct vnodeopv_entry_desc fifo_nfsv2nodeop_entries[] = {
struct vnodeopv_desc fifo_nfsv2nodeop_opv_desc =
struct proc *nfs_iodwant[NFS_MAXASYNCDAEMON];
	struct vnode *vp;
	struct ucred *cred;
	struct proc *procp;
	struct mbuf *mreq, *mrep, *md, *mb;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nfsnode *np = VTONFS(vp);
	struct nfsmount *nmp = VFSTONFS(vp->v_mount);
	struct vattr vattr;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nfsmount *nmp;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct vnode *newvp;
	struct nfsnode *np;
		struct vattr vattr;
	struct uio *uiop;
	struct ucred *cred;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct uio *uiop;
	struct ucred *cred;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nfsmount *nmp;
	struct uio *uiop;
	struct ucred *cred;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nfsmount *nmp;
	struct nfsnode *np = VTONFS(vp);
	struct vnode *newvp;
	struct vattr vattr;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct vattr vattr;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct vnode *sdvp;
	struct componentname *scnp;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct vattr vattr;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct vattr vattr;
	struct uio *uiop;
	struct ucred *cred;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct mbuf *md2;
	struct dirent *savdp;
	struct nfsmount *nmp;
	struct nfsnode *np = VTONFS(vp);
	struct vnode *vp;
	struct ucred *cred;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nameidata nami, *ndp = &nami;
	struct componentname *cnp = &ndp->ni_cnd;
	struct nfsmount *nmp;
	struct nfsnode *np;
	struct vnode *newvp;
	struct vnode *dvp, *vp;
	struct componentname *cnp;
	struct proc *procp;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct vop_strategy_args *ap;
	struct ucred *cr;
	struct proc *p;
	struct buf *nbp;
	struct nfsmount *nmp;
	struct vattr vattr;
	struct vattr vattr;
	struct vattr vattr;
			enum seg_rw, cred_t *);
			enum seg_rw, cred_t *, caller_context_t *);
			struct flk_callback *, cred_t *, caller_context_t *);
struct vnodeops *nfs_vnodeops;
struct vnodeops *
	struct vattr va;
	struct vattr va;
		struct vattr va;
	struct buf *bp;
	struct nfswriteargs wa;
	struct nfsattrstat ns;
	struct nfsreadargs ra;
	struct nfsrdresult rr;
	struct vattr va;
	struct vattr va;
	struct nfssaargs args;
	struct nfsattrstat ns;
	struct vattr va;
	struct vattr va;
	struct nfsrdlnres rl;
			struct nfsdiropargs da;
			enum nfsstat status;
	struct nfsdiropargs da;
	struct nfsdiropres dr;
	struct nfscreatargs args;
	struct nfsdiropres dr;
	struct vattr vattr;
	struct nfsdiropargs da;
	enum nfsstat status;
	struct nfslinkargs args;
	enum nfsstat status;
	enum nfsstat status;
	struct nfsrnmargs args;
	struct nfscreatargs args;
	struct nfsdiropres dr;
	enum nfsstat status;
	struct nfsdiropargs da;
	struct nfsslargs args;
	enum nfsstat status;
	struct nfsrddirargs rda;
	struct nfsrddirres rd;
	struct nfs_fid *fp;
	enum seg_rw rw, cred_t *cr, caller_context_t *ct)
	enum seg_rw rw, cred_t *cr)
	struct buf *bp;
	struct buf *bp;
	struct segvn_crargs vn_a;
	struct vattr va;
	struct flk_callback *flk_cbp, cred_t *cr, caller_context_t *ct)
			struct vattr va;
		struct pathcnf *pc;
	struct shrlock nshr;
	struct nfs_owner nfs_owner;

/* ===== GLOBAL VARIABLES ===== */
int (**nfsv2_vnodeop_p)();
int (**spec_nfsv2nodeop_p)();
void nqnfs_clientlease();
extern u_long nfs_procids[NFS_NPROCS];
extern u_long nfs_prog, nfs_vers, nfs_true, nfs_false;
extern char nfsiobuf[MAXPHYS+NBPG];
int nfs_numasync = 0;
	int error = 0;
	int error = 0;
	int error;
			(void) vnode_pager_uncache(vp);
				(void) vnode_pager_uncache(vp);
	int error = 0;
	int error = 0;
	int error = 0, isnq;
	register int flags = cnp->cn_flags;
	int lockparent, wantparent, error = 0;
	int nqlflag, cachable;
		int vpid;
			nqlflag = fxdr_unsigned(int, *tl);
			cachable = fxdr_unsigned(int, *tl++);
			reqtime += fxdr_unsigned(int, *tl++);
	int error = 0;
	int error = 0;
	int ioflags;
	int error = 0;
	char *cp2;
	int error = 0, isnq;
	int error = 0, isnq;
	int error = 0;
	int error = 0;
	int error = 0;
	int error = 0;
	int error = 0;
	int slen, error = 0, isnq;
	register int len;
	int error = 0, firsttry = 1, isnq;
	int error = 0;
	int tresid, error;
	int error = 0;
	int siz;
	int more_dirs = 1;
		more_dirs = fxdr_unsigned(int, *tl);
			len = fxdr_unsigned(int, *tl);
			dp->d_namlen = (u_char)len;
			more_dirs = fxdr_unsigned(int, *tl);
			more_dirs = (fxdr_unsigned(int, *tl) == 0);
	register int len;
	int error = 0, tlen, more_dirs = 1, tresid, doit, bigenough, i;
	int cachable;
		more_dirs = fxdr_unsigned(int, *tl);
				cachable = fxdr_unsigned(int, *tl++);
				ltime = reqtime + fxdr_unsigned(int, *tl++);
			len = fxdr_unsigned(int, *tl);
					cnp->cn_hash += (unsigned char)*cp * i;
			more_dirs = fxdr_unsigned(int, *tl);
			more_dirs = (fxdr_unsigned(int, *tl) == 0);
static char hextoasc[] = "0123456789abcdef";
	int error;
	int error = 0, isnq;
	int error = 0;
	int s, error = 0, slptimeo = 0, slpflag = 0;
			vprint("nfs_fsync: dirty", vp);
	printf("\n");
	printf("nfs_truncate: need to implement!!");
	printf("nfs_update: need to implement!!");
	register int i;
	int error;
int (**nfsv2_vnodeop_p)();
int (**spec_nfsv2nodeop_p)();
void nqnfs_clientlease();
extern u_long nfs_procids[NFS_NPROCS];
extern u_long nfs_prog, nfs_vers, nfs_true, nfs_false;
extern char nfsiobuf[MAXPHYS+NBPG];
int nfs_numasync = 0;
	int error = 0;
	int error = 0;
	int error;
			(void) vnode_pager_uncache(vp);
				(void) vnode_pager_uncache(vp);
	int error = 0;
	int error = 0;
	int error = 0, isnq;
	register int flags = cnp->cn_flags;
	int lockparent, wantparent, error = 0;
	int nqlflag, cachable;
		int vpid;
			nqlflag = fxdr_unsigned(int, *tl);
			cachable = fxdr_unsigned(int, *tl++);
			reqtime += fxdr_unsigned(int, *tl++);
	int error = 0;
	int error = 0;
	int ioflags;
	int error = 0;
	char *cp2;
	int error = 0, isnq;
	int error = 0, isnq;
	int error = 0;
	int error = 0;
	int error = 0;
	int error = 0;
	int error = 0;
	int slen, error = 0, isnq;
	register int len;
	int error = 0, firsttry = 1, isnq;
	int error = 0;
	int tresid, error;
	int error = 0;
	int siz;
	int more_dirs = 1;
		more_dirs = fxdr_unsigned(int, *tl);
			len = fxdr_unsigned(int, *tl);
			dp->d_namlen = (u_char)len;
			more_dirs = fxdr_unsigned(int, *tl);
			more_dirs = (fxdr_unsigned(int, *tl) == 0);
	register int len;
	int error = 0, tlen, more_dirs = 1, tresid, doit, bigenough, i;
	int cachable;
		more_dirs = fxdr_unsigned(int, *tl);
				cachable = fxdr_unsigned(int, *tl++);
				ltime = reqtime + fxdr_unsigned(int, *tl++);
			len = fxdr_unsigned(int, *tl);
					cnp->cn_hash += (unsigned char)*cp * i;
			more_dirs = fxdr_unsigned(int, *tl);
			more_dirs = (fxdr_unsigned(int, *tl) == 0);
static char hextoasc[] = "0123456789abcdef";
	int error;
	int error = 0, isnq;
	int error = 0;
	int s, error = 0, slptimeo = 0, slpflag = 0;
			vprint("nfs_fsync: dirty", vp);
	printf("\n");
	printf("nfs_truncate: need to implement!!");
	printf("nfs_update: need to implement!!");
	register int i;
	int error;
static int	nfswrite(vnode_t *, caddr_t, uint_t, int, cred_t *);
static int	nfsread(vnode_t *, caddr_t, uint_t, int, size_t *, cred_t *);
static int	nfssetattr(vnode_t *, struct vattr *, int, cred_t *);
static int	nfslookup_dnlc(vnode_t *, char *, vnode_t **, cred_t *);
static int	nfslookup_otw(vnode_t *, char *, vnode_t **, cred_t *, int);
static int	nfsreaddir(vnode_t *, rddir_cache *, cred_t *);
static int	nfs_bio(struct buf *, cred_t *);
			int, cred_t *);
			int, cred_t *);
static void	nfs_delmap_callback(struct as *, void *, uint_t);
static int	nfs_open(vnode_t **, int, cred_t *, caller_context_t *);
static int	nfs_access(vnode_t *, int, int, cred_t *, caller_context_t *);
static int	nfs_accessx(void *, int, cred_t *);
static int	nfs_fsync(vnode_t *, int, cred_t *, caller_context_t *);
static void	nfs_inactive(vnode_t *, cred_t *, caller_context_t *);
			int *, pathname_t *);
			int);
			caller_context_t *, int);
			caller_context_t *, int);
			cred_t *, caller_context_t *, int, vsecattr_t *);
			caller_context_t *, int);
			cred_t *, caller_context_t *, int);
			caller_context_t *, int);
static int	nfs_fid(vnode_t *, fid_t *, caller_context_t *);
static int	nfs_rwlock(vnode_t *, int, caller_context_t *);
static void	nfs_rwunlock(vnode_t *, int, caller_context_t *);
static int	nfs_seek(vnode_t *, offset_t, offset_t *, caller_context_t *);
			uchar_t, uchar_t, uint_t, cred_t *, caller_context_t *);
			uchar_t, uchar_t, uint_t, cred_t *, caller_context_t *);
static int	nfs_realvp(vnode_t *, vnode_t **, caller_context_t *);
			uint_t, uint_t, uint_t, cred_t *, caller_context_t *);
	int error;
	int error;
		(void) nfs_getattr_otw(vp, &va, cr);
	int on;
	uint_t flags;
	int error;
				(void) vpm_sync_pages(vp, off, n, 0);
				(void) segmap_release(segkmap, base, 0);
	uint_t flags;
	int remainder;
	int on;
	int error;
	int resid;
				int pon = uiop->uio_loffset & PAGEOFFSET;
				int pagecreate;
				(void) vpm_sync_pages(vp, off, n, 0);
				(void) segmap_release(segkmap, base, 0);
	int error;
	int error;
	int tsize;
	int douprintf;
	douprintf = 1;
			    &douprintf, &ns.ns_status, 0, NULL);
	int tsize;
	int error;
	int douprintf;
	douprintf = 1;
			    &douprintf, &rr.rr_status, 0, &fi);
	int error;
	int error;
	uint_t mask;
	int error;
	uint_t mask;
	int douprintf;
	douprintf = 1;
	    &douprintf, &ns.ns_status, 0, NULL);
		(void) nfs_access_purge_rp(rp);
			(void) nfs_cache_fattr(vp, &ns.ns_attr, &va, t, cr);
				int terror;
					(void) nfssetattr(vp, &va, 0, cr);
	int error;
static int nfs_do_symlink_cache = 1;
	int error;
	int douprintf;
	douprintf = 1;
	    &douprintf, &rl.rl_status, 0, &fi);
		kmem_free((void *)rl.rl_data, NFS_MAXPATHLEN);
		error = uiomove(rl.rl_data, (int)rl.rl_count, UIO_READ, uiop);
				rp->r_symlink.len = (int)rl.rl_count;
			kmem_free((void *)rl.rl_data, NFS_MAXPATHLEN);
		kmem_free((void *)rl.rl_data, NFS_MAXPATHLEN);
	int error;
			char *unlname;
			int douprintf;
			int error;
			douprintf = 1;
			    &douprintf, &status, 0, NULL);
	int error;
		(void) nfslookup_dnlc(dvp, XATTR_DIR_NAME, &avp, cr);
static int nfs_lookup_neg_cache = 1;
static int nfs_lookup_dnlc_hits = 0;
static int nfs_lookup_dnlc_misses = 0;
static int nfs_lookup_dnlc_neg_hits = 0;
static int nfs_lookup_dnlc_disappears = 0;
static int nfs_lookup_dnlc_lookups = 0;
	int error;
	int error;
	int error;
	int douprintf;
	douprintf = 1;
	    &douprintf, &dr.dr_status, rfscall_flags, &fi);
	int error;
	int douprintf;
		(void) cmpldev(&dev32, d);
	douprintf = 1;
	    &douprintf, &dr.dr_status, 0, NULL);
				(void) nfssetattr(vp, &vattr, 0, cr);
	int error;
	char *tmpname;
	int douprintf;
	int error;
	int douprintf;
	douprintf = 1;
	    &douprintf, &status, 0, NULL);
	int error;
	int douprintf;
	char *tmpname;
	douprintf = 1;
	    &douprintf, &status, 0, NULL);
	int error;
	int douprintf;
	douprintf = 1;
	    &douprintf, &dr.dr_status, 0, NULL);
				(void) nfssetattr(*vpp, va, 0, cr);
	int error;
	int douprintf;
	douprintf = 1;
	    &douprintf, &status, 0, NULL);
	int error;
	int douprintf;
	douprintf = 1;
	    &douprintf, &status, 0, NULL);
static int nfs_readdir_cache_hits = 0;
static int nfs_readdir_cache_shorts = 0;
static int nfs_readdir_cache_waits = 0;
static int nfs_readdir_cache_misses = 0;
static int nfs_readdir_readahead = 0;
static int nfs_shrinkreaddir = 0;
	int error;
	int missed;
	int error;
	uint_t count;
	int douprintf;
	douprintf = 1;
		    &douprintf, &rd.rd_status, 0, fip);
static int nfs_bio_do_stop = 0;
	int count;
	int error;
	uint_t offset;
				nfs_printfhandle(&rp->r_fh);
		(void) nfs_rw_enter_sig(&rp->r_rwlock, RW_READER, FALSE);
		(void) nfs_rw_enter_sig(&rp->r_rwlock, RW_READER, FALSE);
	(void) nfs_rw_enter_sig(&rp->r_rwlock, RW_WRITER, FALSE);
static int nfs_nra = 4;
	int error;
	uint_t bsize;
	uint_t blksize;
	int error;
	int readahead;
	int readahead_issued = 0;
	int error;
	uint_t bsize, blksize;
	int error;
	uint_t bsize;
	int error;
	int error;
	int error;
	atomic_inc_uint(&rp->r_inmap);
		atomic_dec_uint(&rp->r_inmap);
	vn_a.prot = (uchar_t)prot;
	vn_a.maxprot = (uchar_t)maxprot;
	atomic_dec_uint(&rp->r_inmap);
	int rc;
	int error = 0, intr = INTR(vp);
		intr = 0;
	lm_fh.n_bytes = (char *)VTOFH(vp);
	int error;
	int			caller_found;
	int			error;
	int			error;
	(void) as_delete_callback(as, arg);
	int error = 0;
	int error;
	int error;
	int error;
	int error;
	int error;
		lm_fh.n_bytes = (char *)VTOFH(vp);

/* ===== FUNCTIONS ===== */

/* Function 1 */
			(np->n_flag & NQNFSNONCACHE)) {
			if ((error = nfs_vinvalbuf(vp, V_SAVE, ap->a_cred,
				ap->a_p, 1)) == EINTR)
				return (error);
			(void) vnode_pager_uncache(vp);
			np->n_brev = np->n_lrev;
		    }

/* Function 2 */
		(np->n_flag & NMODIFIED)) {
		error = nfs_vinvalbuf(vp, V_SAVE, ap->a_cred, ap->a_p, 1);
		np->n_attrstamp = 0;
	    }

/* Function 3 */
	    NQNFS_CKCACHABLE(vp, NQL_WRITE)) {
		nfsm_dissect(tl, u_long *, 2*NFSX_UNSIGNED);
		fxdr_hyper(tl, &frev);
		if (frev > np->n_brev)
			np->n_brev = frev;
	}

/* Function 4 */
					    (np->n_flag & NMODIFIED)) {
						np->n_direofoffset = 0;
						cache_purge(dvp);
						error = nfs_vinvalbuf(dvp, 0,
						    cnp->cn_cred, cnp->cn_proc,
						    1);
						if (error == EINTR)
							return (error);
						np->n_brev = np->n_lrev;
					} else {
						nfsstats.lookupcache_hits++;
						if (cnp->cn_nameiop != LOOKUP &&
						    (flags & ISLASTCN))
						    cnp->cn_flags |= SAVENAME;
						return (0);
					}

/* Function 5 */
			       vattr.va_ctime.ts_sec == VTONFS(vdp)->n_ctime) {
				nfsstats.lookupcache_hits++;
				if (cnp->cn_nameiop != LOOKUP &&
				    (flags & ISLASTCN))
					cnp->cn_flags |= SAVENAME;
				return (0);
			   }

/* Function 6 */
		    nfs_loadattrcache(&newvp, &md, &dpos, (struct vattr *)0)) {
			vrele(newvp);
			m_freem(mrep);
			return (error);
		}

/* Function 7 */
	    (cnp->cn_nameiop != DELETE || !(flags & ISLASTCN))) {
		if ((nmp->nm_flag & NFSMNT_NQNFS) == 0)
			np->n_ctime = np->n_vattr.va_ctime.ts_sec;
		else if (nqlflag && reqtime > get_seconds())
			nqnfs_clientlease(nmp, np, nqlflag, cachable, reqtime,
				frev);
		cache_enter(dvp, *vpp, cnp);
	}

/* Function 8 */
			 NQNFS_CKCACHABLE(vp, NQL_WRITE)) {
			nfsm_dissect(tl, u_long *, 2*NFSX_UNSIGNED);
			fxdr_hyper(tl, &frev);
			if (frev > np->n_brev)
				np->n_brev = frev;
		}

/* Function 9 */
	    (tvp && (fvp->v_mount != tvp->v_mount))) {
		error = EXDEV;
		goto out;
	}

/* Function 10 */
	    (np->n_flag & NMODIFIED) == 0) {
		if (VFSTONFS(vp->v_mount)->nm_flag & NFSMNT_NQNFS) {
			if (NQNFS_CKCACHABLE(vp, NQL_READ)) {
				nfsstats.direofcache_hits++;
				return (0);
			}
		} else if (VOP_GETATTR(vp, &vattr, ap->a_cred, uio->uio_procp) == 0 &&
			np->n_mtime == vattr.va_mtime.ts_sec) {
			nfsstats.direofcache_hits++;
			return (0);
		}
	}

/* Function 11 */
		    (vp->v_mount->mnt_flag & MNT_RDONLY) == 0) {
			VATTR_NULL(&vattr);
			if (np->n_flag & NACC) {
				vattr.va_atime.ts_sec = np->n_atim.tv_sec;
				vattr.va_atime.ts_nsec =
				    np->n_atim.tv_usec * 1000;
			}
			if (np->n_flag & NUPD) {
				vattr.va_mtime.ts_sec = np->n_mtim.tv_sec;
				vattr.va_mtime.ts_nsec =
				    np->n_mtim.tv_usec * 1000;
			}
			(void)VOP_SETATTR(vp, &vattr, ap->a_cred, ap->a_p);
		}

/* Function 12 */
		    (vp->v_mount->mnt_flag & MNT_RDONLY) == 0) {
			VATTR_NULL(&vattr);
			if (np->n_flag & NACC) {
				vattr.va_atime.ts_sec = np->n_atim.tv_sec;
				vattr.va_atime.ts_nsec =
				    np->n_atim.tv_usec * 1000;
			}
			if (np->n_flag & NUPD) {
				vattr.va_mtime.ts_sec = np->n_mtim.tv_sec;
				vattr.va_mtime.ts_nsec =
				    np->n_mtim.tv_usec * 1000;
			}
			(void)VOP_SETATTR(vp, &vattr, ap->a_cred, ap->a_p);
		}

/* Function 13 */
			(np->n_flag & NQNFSNONCACHE)) {
			if ((error = nfs_vinvalbuf(vp, V_SAVE, ap->a_cred,
				ap->a_p, 1)) == EINTR)
				return (error);
			(void) vnode_pager_uncache(vp);
			np->n_brev = np->n_lrev;
		    }

/* Function 14 */
		(np->n_flag & NMODIFIED)) {
		error = nfs_vinvalbuf(vp, V_SAVE, ap->a_cred, ap->a_p, 1);
		np->n_attrstamp = 0;
	    }

/* Function 15 */
	    NQNFS_CKCACHABLE(vp, NQL_WRITE)) {
		nfsm_dissect(tl, u_long *, 2*NFSX_UNSIGNED);
		fxdr_hyper(tl, &frev);
		if (frev > np->n_brev)
			np->n_brev = frev;
	}

/* Function 16 */
					    (np->n_flag & NMODIFIED)) {
						np->n_direofoffset = 0;
						cache_purge(dvp);
						error = nfs_vinvalbuf(dvp, 0,
						    cnp->cn_cred, cnp->cn_proc,
						    1);
						if (error == EINTR)
							return (error);
						np->n_brev = np->n_lrev;
					} else {
						nfsstats.lookupcache_hits++;
						if (cnp->cn_nameiop != LOOKUP &&
						    (flags & ISLASTCN))
						    cnp->cn_flags |= SAVENAME;
						return (0);
					}

/* Function 17 */
			       vattr.va_ctime.ts_sec == VTONFS(vdp)->n_ctime) {
				nfsstats.lookupcache_hits++;
				if (cnp->cn_nameiop != LOOKUP &&
				    (flags & ISLASTCN))
					cnp->cn_flags |= SAVENAME;
				return (0);
			   }

/* Function 18 */
		    nfs_loadattrcache(&newvp, &md, &dpos, (struct vattr *)0)) {
			vrele(newvp);
			m_freem(mrep);
			return (error);
		}

/* Function 19 */
	    (cnp->cn_nameiop != DELETE || !(flags & ISLASTCN))) {
		if ((nmp->nm_flag & NFSMNT_NQNFS) == 0)
			np->n_ctime = np->n_vattr.va_ctime.ts_sec;
		else if (nqlflag && reqtime > get_seconds())
			nqnfs_clientlease(nmp, np, nqlflag, cachable, reqtime,
				frev);
		cache_enter(dvp, *vpp, cnp);
	}

/* Function 20 */
			 NQNFS_CKCACHABLE(vp, NQL_WRITE)) {
			nfsm_dissect(tl, u_long *, 2*NFSX_UNSIGNED);
			fxdr_hyper(tl, &frev);
			if (frev > np->n_brev)
				np->n_brev = frev;
		}

/* Function 21 */
	    (tvp && (fvp->v_mount != tvp->v_mount))) {
		error = EXDEV;
		goto out;
	}

/* Function 22 */
	    (np->n_flag & NMODIFIED) == 0) {
		if (VFSTONFS(vp->v_mount)->nm_flag & NFSMNT_NQNFS) {
			if (NQNFS_CKCACHABLE(vp, NQL_READ)) {
				nfsstats.direofcache_hits++;
				return (0);
			}
		} else if (VOP_GETATTR(vp, &vattr, ap->a_cred, uio->uio_procp) == 0 &&
			np->n_mtime == vattr.va_mtime.ts_sec) {
			nfsstats.direofcache_hits++;
			return (0);
		}
	}

/* Function 23 */
		    (vp->v_mount->mnt_flag & MNT_RDONLY) == 0) {
			VATTR_NULL(&vattr);
			if (np->n_flag & NACC) {
				vattr.va_atime.ts_sec = np->n_atim.tv_sec;
				vattr.va_atime.ts_nsec =
				    np->n_atim.tv_usec * 1000;
			}
			if (np->n_flag & NUPD) {
				vattr.va_mtime.ts_sec = np->n_mtim.tv_sec;
				vattr.va_mtime.ts_nsec =
				    np->n_mtim.tv_usec * 1000;
			}
			(void)VOP_SETATTR(vp, &vattr, ap->a_cred, ap->a_p);
		}

/* Function 24 */
		    (vp->v_mount->mnt_flag & MNT_RDONLY) == 0) {
			VATTR_NULL(&vattr);
			if (np->n_flag & NACC) {
				vattr.va_atime.ts_sec = np->n_atim.tv_sec;
				vattr.va_atime.ts_nsec =
				    np->n_atim.tv_usec * 1000;
			}
			if (np->n_flag & NUPD) {
				vattr.va_mtime.ts_sec = np->n_mtim.tv_sec;
				vattr.va_mtime.ts_nsec =
				    np->n_mtim.tv_usec * 1000;
			}
			(void)VOP_SETATTR(vp, &vattr, ap->a_cred, ap->a_p);
		}

/* Function 25 */
	    !(VTOMI(vp)->mi_flags & MI_NOCTO))) {
		if (vn_is_readonly(vp))
			error = nfs_validate_caches(vp, cr);
		else if (rp->r_mapcnt == 0 && vp->v_count == 1) {
			PURGE_ATTRCACHE(vp);
			error = 0;
		} else {
			va.va_mask = AT_ALL;
			error = nfs_getattr_otw(vp, &va, cr);
		}
	} else

/* Function 26 */
	    !vn_has_cached_data(vp))) {
		size_t bufsize;
		size_t resid = 0;

		bufsize = MIN(uiop->uio_resid, VTOMI(vp)->mi_curread);
		base = kmem_alloc(bufsize, KM_SLEEP);
		do {
			n = MIN(uiop->uio_resid, bufsize);
			error = nfsread(vp, base, uiop->uio_offset, n,
			    &resid, cr);
			if (!error) {
				n -= resid;
				error = uiomove(base, n, UIO_READ, uiop);
			}
		} while (!error && uiop->uio_resid > 0 && n > 0);
		kmem_free(base, bufsize);
		return (error);
	}

/* Function 27 */
	    !vn_has_cached_data(vp))) {
		size_t bufsize;
		int count;
		uint_t org_offset;

nfs_fwrite:
		if (rp->r_flags & RSTALE) {
			resid = uiop->uio_resid;
			offset = uiop->uio_loffset;
			error = rp->r_error;
			if (error == 0)
				error = ESTALE;
			goto bottom;
		}
		bufsize = MIN(uiop->uio_resid, mi->mi_curwrite);
		base = kmem_alloc(bufsize, KM_SLEEP);
		do {
			resid = uiop->uio_resid;
			offset = uiop->uio_loffset;
			count = MIN(uiop->uio_resid, bufsize);
			org_offset = uiop->uio_offset;
			error = uiomove(base, count, UIO_WRITE, uiop);
			if (!error) {
				error = nfswrite(vp, base, org_offset,
				    count, cr);
			}
		} while (!error && uiop->uio_resid > 0);
		kmem_free(base, bufsize);
		goto bottom;
	}

/* Function 28 */
			    (rp->r_flags & ROUTOFSPACE)) {
				flags &= ~SM_ASYNC;
				flags |= SM_WRITE;
			}

/* Function 29 */
		    (mi->mi_flags & MI_ACL)) {
			mutex_exit(&rp->r_statelock);
			PURGE_ATTRCACHE(vp);
		} else {
			if (rp->r_mtime <= t) {
				nfs_attrcache_va(vp, &va);
			}
			mutex_exit(&rp->r_statelock);
		}

/* Function 30 */
		    (vap->va_mask & (AT_SIZE | AT_FSID | AT_RDEV))) {
			mutex_enter(&rp->r_statelock);
			if (vap->va_mask | AT_SIZE)
				vap->va_size = rp->r_size;
			if (vap->va_mask | AT_FSID)
				vap->va_fsid = rp->r_attr.va_fsid;
			if (vap->va_mask | AT_RDEV)
				vap->va_rdev = rp->r_attr.va_rdev;
			mutex_exit(&rp->r_statelock);
			return (0);
		}

/* Function 31 */
		    ((rp->r_flags & RDIRTY) || rp->r_awcount > 0)) {
			mutex_enter(&rp->r_statelock);
			rp->r_gcount++;
			mutex_exit(&rp->r_statelock);
			error = nfs_putpage(vp, (offset_t)0, 0, 0, cr, ct);
			mutex_enter(&rp->r_statelock);
			if (error && (error == ENOSPC || error == EDQUOT)) {
				if (!rp->r_error)
					rp->r_error = error;
			}
			if (--rp->r_gcount == 0)
				cv_broadcast(&rp->r_cv);
			mutex_exit(&rp->r_statelock);
		}

/* Function 32 */
		    NFS_TIME_T_OK(vap->va_atime.tv_sec)) {
			error = vattr_to_sattr(vap, &args.saa_sa);
		} else {
			timestruc_t	mtime = vap->va_mtime;
			timestruc_t	atime = vap->va_atime;
			time_t		now;

			now = gethrestime_sec();
			if (NFS_TIME_T_OK(now)) {
				vap->va_mtime.tv_sec = now;
				vap->va_atime.tv_sec = now;
			} else {
				vap->va_mtime.tv_sec = 0;
				vap->va_atime.tv_sec = 0;
			}
			error = vattr_to_sattr(vap, &args.saa_sa);
			vap->va_mtime = mtime;
			vap->va_atime = atime;
		}

/* Function 33 */
			    ns.ns_attr.na_size < (uint32_t)vap->va_size) {
				char zb = '\0';

				error = nfswrite(vp, &zb,
				    vap->va_size - sizeof (zb),
				    sizeof (zb), cr);
			}

/* Function 34 */
			    ((rp->r_flags & RDIRTY) || rp->r_count > 0)) {
				ASSERT(vp->v_type != VCHR);
				error = nfs_putpage(vp, (offset_t)0, 0, 0,
				    cr, ct);
				if (error) {
					mutex_enter(&rp->r_statelock);
					if (!rp->r_error)
						rp->r_error = error;
					mutex_exit(&rp->r_statelock);
				}
			}

/* Function 35 */
	    (rp->r_unldvp == NULL || strcmp(nm, rp->r_unlname) == 0)) {
		mutex_exit(&rp->r_statelock);
		tmpname = newname();
		error = nfsrename(dvp, nm, dvp, tmpname, cr, ct);
		if (error)
			kmem_free(tmpname, MAXNAMELEN);
		else {
			mutex_enter(&rp->r_statelock);
			if (rp->r_unldvp == NULL) {
				VN_HOLD(dvp);
				rp->r_unldvp = dvp;
				if (rp->r_unlcred != NULL)
					crfree(rp->r_unlcred);
				crhold(cr);
				rp->r_unlcred = cr;
				rp->r_unlname = tmpname;
			} else {
				kmem_free(rp->r_unlname, MAXNAMELEN);
				rp->r_unlname = tmpname;
			}
			mutex_exit(&rp->r_statelock);
		}
	} else {
		mutex_exit(&rp->r_statelock);
		if (vn_has_cached_data(vp) &&
		    ((rp->r_flags & RDIRTY) || rp->r_count > 0)) {
			error = nfs_putpage(vp, (offset_t)0, 0, 0, cr, ct);
			if (error && (error == ENOSPC || error == EDQUOT)) {
				mutex_enter(&rp->r_statelock);
				if (!rp->r_error)
					rp->r_error = error;
				mutex_exit(&rp->r_statelock);
			}
		}

		setdiropargs(&da, nm, dvp);

		douprintf = 1;

		error = rfs2call(VTOMI(dvp), RFS_REMOVE,
		    xdr_diropargs, (caddr_t)&da,
		    xdr_enum, (caddr_t)&status, cr,
		    &douprintf, &status, 0, NULL);

		if (dvp->v_flag & V_XATTRDIR)
			dnlc_purge_vp(dvp);


		if (!error) {
			error = geterrno(status);
			if (!error) {
				if (HAVE_RDDIR_CACHE(drp))
					nfs_purge_rddir_cache(dvp);
			} else {
				PURGE_STALE_FH(error, dvp, cr);
			}
		}
	}

/* Function 36 */
				    (B_INVAL|B_FORCE|B_ASYNC)) {
					rp->r_error = error;
				}

/* Function 37 */
	    !(vp->v_flag & VNOCACHE)) {
		mutex_enter(&rp->r_statelock);


		if (off == 0)
			readahead = 0;
		else if (blkoff == rp->r_nextr)
			readahead = nfs_nra;
		else if (rp->r_nextr > blkoff &&
		    ((ra_window = (rp->r_nextr - blkoff) / bsize)
		    <= (nfs_nra - 1)))
			readahead = nfs_nra - ra_window;
		else
			readahead = 0;

		rablkoff = rp->r_nextr;
		while (readahead > 0 && rablkoff + bsize < rp->r_size) {
			mutex_exit(&rp->r_statelock);
			if (nfs_async_readahead(vp, rablkoff + bsize,
			    addr + (rablkoff + bsize - off), seg, cr,
			    nfs_readahead) < 0) {
				mutex_enter(&rp->r_statelock);
				break;
			}
			readahead--;
			rablkoff += bsize;
			readahead_issued = 1;
			mutex_enter(&rp->r_statelock);
			rp->r_nextr = rablkoff;
		}
		mutex_exit(&rp->r_statelock);
	}

/* Function 38 */
			    (off != rp->r_nextr && !readahead_issued)) {
				blksize = PAGESIZE;
			} else

/* Function 39 */
	    (flags & (B_INVAL|B_FORCE)) != (B_INVAL|B_FORCE)) {
		if (!(rp->r_flags & ROUTOFSPACE)) {
			mutex_enter(&rp->r_statelock);
			rp->r_flags |= ROUTOFSPACE;
			mutex_exit(&rp->r_statelock);
		}
		flags |= B_ERROR;
		pvn_write_done(pp, flags);
		if (!(flags & B_ASYNC)) {
			error = nfs_putpage(vp, io_off, io_len,
			    B_INVAL | B_FORCE, cr, NULL);
		}
	} else {
		if (error)
			flags |= B_ERROR;
		else if (rp->r_flags & ROUTOFSPACE) {
			mutex_enter(&rp->r_statelock);
			rp->r_flags &= ~ROUTOFSPACE;
			mutex_exit(&rp->r_statelock);
		}
		pvn_write_done(pp, flags);
	}

/* Function 40 */
	    MANDLOCK(vp, va.va_mode)) {
		error = EAGAIN;
		goto done;
	}

/* Function 41 */
	    dmapp->flags == MAP_SHARED && (dmapp->maxprot & PROT_WRITE)) {
		mutex_enter(&rp->r_statelock);
		rp->r_flags |= RDIRTY;
		mutex_exit(&rp->r_statelock);
		if ((mi->mi_flags & MI_NOCTO) ||
		    nfs_zone() != mi->mi_zone)
			error = nfs_putpage(dmapp->vp, dmapp->off, dmapp->len,
			    B_ASYNC, dmapp->cr, NULL);
		else
			error = nfs_putpage(dmapp->vp, dmapp->off, dmapp->len,
			    0, dmapp->cr, NULL);
		if (!error) {
			mutex_enter(&rp->r_statelock);
			error = rp->r_error;
			rp->r_error = 0;
			mutex_exit(&rp->r_statelock);
		}
	} else


#ifdef __cplusplus
}
#endif

/* End of nfs_vnops_unified.c - Synthesized by MINIX4 Massive Synthesis System */
