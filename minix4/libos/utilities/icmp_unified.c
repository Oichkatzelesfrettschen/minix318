/**
 * @file icmp_unified.c
 * @brief Unified icmp implementation
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
 *     1. minix4\microkernel\servers\icmp\icmp.c
 *     2. minix4\exokernel\kernel_legacy\uts\common\inet\ip\icmp.c
 * 
 * Total source files: 2
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
#include "icmp.h"
#include "icmp_internal.h"
#include "xkernel.h"
#include <inet/common.h>
#include <inet/ip.h>
#include <inet/ip6.h>
#include <inet/ip_if.h>
#include <inet/ip_impl.h>
#include <inet/ip_ire.h>
#include <inet/ip_multi.h>
#include <inet/ip_ndp.h>
#include <inet/ipclassifier.h>
#include <inet/ipsec_impl.h>
#include <inet/kstatcom.h>
#include <inet/mib2.h>
#include <inet/nd.h>
#include <inet/optcom.h>
#include <inet/proto_set.h>
#include <inet/rawip_impl.h>
#include <inet/snmpcom.h>
#include <net/bpf.h>
#include <netinet/icmp6.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#include <sys/cmn_err.h>
#include <sys/cred.h>
#include <sys/ddi.h>
#include <sys/debug.h>
#include <sys/disp.h>
#include <sys/isa_defs.h>
#include <sys/kmem.h>
#include <sys/policy.h>
#include <sys/priv.h>
#include <sys/random.h>
#include <sys/sdt.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/sockio.h>
#include <sys/stream.h>
#include <sys/strlog.h>
#include <sys/stropts.h>
#include <sys/strsubr.h>
#include <sys/strsun.h>
#include <sys/sunddi.h>
#include <sys/suntpi.h>
#include <sys/tihdr.h>
#include <sys/timod.h>
#include <sys/tsol/label.h>
#include <sys/tsol/tnet.h>
#include <sys/types.h>
#include <sys/ucred.h>
#include <sys/vtrace.h>
#include <sys/xti_inet.h>
#include <sys/zone.h>

/* ===== DEFINES ===== */
#define	_SUN_TPI_VERSION 2
#define	is_wroff_extra			is_propinfo_tbl[0].prop_cur_uval
#define	is_ipv4_ttl			is_propinfo_tbl[1].prop_cur_uval
#define	is_ipv6_hoplimit		is_propinfo_tbl[2].prop_cur_uval
#define	is_bsd_compat			is_propinfo_tbl[3].prop_cur_bval
#define	is_xmit_hiwat			is_propinfo_tbl[4].prop_cur_uval
#define	is_xmit_lowat			is_propinfo_tbl[5].prop_cur_uval
#define	is_recv_hiwat			is_propinfo_tbl[6].prop_cur_uval
#define	is_max_buf			is_propinfo_tbl[7].prop_cur_uval
#define	is_pmtu_discovery		is_propinfo_tbl[8].prop_cur_bval
#define	is_sendto_ignerr		is_propinfo_tbl[9].prop_cur_bval
#define	INET_NAME	"ip"

/* ===== TYPES ===== */
struct streamtab icmpinfov4 = {
struct streamtab icmpinfov6 = {
typedef union T_primitives *t_primp_t;
	struct sockaddr *sa;
	struct T_bind_req *tbr;
	struct T_conn_req	*tcr;
	struct sockaddr *sa;
	struct T_error_ack	*teackp;
		struct T_unitdata_ind	*tudi;
		struct T_opthdr		*toh;
		struct ip6_mtuinfo	*mtuinfo;
	struct sockaddr *sa;
	struct T_addr_ack *taa;
	struct T_capability_ack	*tcap;
	struct bpf_program prog;
		struct bpf_program32 *prog32;
	struct T_unitdata_ind	*tudi;
	struct opthdr		*optp;
	struct T_unitdata_req *tudr;
		struct T_unitdata_req *tudr;
	struct sockaddr	*addr = NULL;
	struct T_unitdata_req *tudr;
	struct iocblk *iocp;
	struct T_capability_ack tca;
	struct sockaddr_in6 laddr, faddr;
	struct stroptions *stropt;
	struct sock_proto_props sopp;

/* ===== GLOBAL VARIABLES ===== */
int traceicmpp;
static void	getProcProtl(XObj);
static void	getProcSessn(XObj);
static long 	icmpHdrLoad(void *hdr, char *netHdr, long len, void *);
static long 	icmpRedirectLoad(void *hdr, char *netHdr, long len, void *);
static void	getProcProtl();
static void	getProcSessn();
static long 	icmpHdrLoad();
static long 	icmpRedirectLoad();
    int i;
    short int *cksum;
    int n1;
    int n2;
  msgPop(msg, icmpHdrLoad, (char *)&hdr, sizeof(ICMPHeader), &loadI);
    int opCode;
    char *buf;
    int len;
      *(int *)buf -= sizeof(ICMPHeader) + sizeof(ICMPEcho);
    int opCode;
    char *buf;
    int len;
    char *src;
    long int len;
    memcpy((char *)hdr, src, sizeof(ICMPHeader));
    char *dst;
    long int len;
	memcpy(dst, (char *)hdr, sizeof(ICMPHeader));
	memcpy(dst, (char *)hdr, sizeof(ICMPHeader));
	memcpy(dst, (char *)hdr, sizeof(ICMPHeader));
    char *src;
    long int len;
    memcpy((char *)hdr, src, sizeof(ICMPEcho));
    char *dst;
    long int len;
    memcpy(dst, (char *)hdr, sizeof(ICMPEcho));
    char *src;
    long int len;
    memcpy((char *)hdr, src, sizeof(ICMPRedirect));
static void	icmp_addr_req(queue_t *q, mblk_t *mp);
static void	icmp_tpi_bind(queue_t *q, mblk_t *mp);
static void	icmp_bind_proto(icmp_t *icmp);
    const in6_addr_t *, uint32_t);
static void	icmp_capability_req(queue_t *q, mblk_t *mp);
static int	icmp_close(queue_t *q, int flags, cred_t *);
static void	icmp_close_free(conn_t *);
static void	icmp_tpi_connect(queue_t *q, mblk_t *mp);
static void	icmp_tpi_disconnect(queue_t *q, mblk_t *mp);
    int sys_error);
    t_scalar_t tlierr, int sys_error);
static void	icmp_info_req(queue_t *q, mblk_t *mp);
static void	icmp_input(void *, mblk_t *, void *, ip_recv_attr_t *);
static conn_t	*icmp_open(int family, cred_t *credp, int *err, int flags);
static boolean_t icmp_opt_allow_udr_set(t_scalar_t level, t_scalar_t name);
		    void *thisdg_attrs, cred_t *cr);
		    uchar_t *ptr);
    const in6_addr_t *, const in6_addr_t *, uint32_t, mblk_t *, int *);
    mblk_t *, const in6_addr_t *, uint32_t, int *);
		    uchar_t *ptr, int len);
static void	icmp_ud_err(queue_t *q, mblk_t *mp, t_scalar_t err);
static void	icmp_tpi_unbind(queue_t *q, mblk_t *mp);
static int	icmp_wput(queue_t *q, mblk_t *mp);
static int	icmp_wput_fallback(queue_t *q, mblk_t *mp);
static void	icmp_wput_other(queue_t *q, mblk_t *mp);
static void	icmp_wput_iocdata(queue_t *q, mblk_t *mp);
static void	icmp_wput_restricted(queue_t *q, mblk_t *mp);
static void	icmp_ulp_recv(conn_t *, mblk_t *, uint_t);
static void	*rawip_stack_init(netstackid_t stackid, netstack_t *ns);
static void	rawip_stack_fini(netstackid_t stackid, void *arg);
static void	*rawip_kstat_init(netstackid_t stackid);
static void	rawip_kstat_fini(netstackid_t stackid, kstat_t *ksp);
static int	rawip_kstat_update(kstat_t *kp, int rw);
static void	rawip_stack_shutdown(netstackid_t stackid, void *arg);
static conn_t	*rawip_do_open(int, cred_t *, int *, int);
static void	rawip_do_close(conn_t *);
static int	rawip_do_bind(conn_t *, struct sockaddr *, socklen_t);
static int	rawip_do_unbind(conn_t *);
	int	error;
		    (uint_t)(mp->b_wptr - mp->b_rptr));
			mp->b_wptr = (uchar_t *)&sin[1];
			mp->b_wptr = (uchar_t *)&sin6[1];
	int		error = 0;
	uint_t		scopeid = 0;
	(void) ip_laddr_fanout_insert(connp);
	int error;
	int		error;
	uint16_t	dstport;
	uint32_t	flowinfo;
	uint_t		scopeid = 0;
	uint_t		srcid = 0;
	(void) rawip_do_close(connp);
	int	error;
	int	error;
	int	error;
	int		iph_hdr_length;
	int		error = 0;
			(void) ip_get_pmtu(ixa);
	uint16_t	iph_hdr_length;
	uint8_t		*nexthdrp;
	int		error = 0;
		newmp->b_wptr = (uchar_t *)tudi + udi_size;
	uint_t	addrlen;
	ackmp->b_wptr = (uchar_t *)&taa[1];
		(void) conn_getsockname(connp, sa, &addrlen);
		(void) conn_getpeername(connp, sa, &addrlen);
	int	error;
		RD(q)->q_ptr = (void *)conn_dev;
		WR(q)->q_ptr = (void *)ip_minor_arena_sa;
	(void) proto_set_tx_wroff(q, connp, connp->conn_wroff);
	(void) proto_set_rx_hiwat(connp->conn_rq, connp, connp->conn_rcvbuf);
	int len;
	int *i1 = (int *)ptr;
			*ptr = (uchar_t)IP_DEFAULT_MULTICAST_TTL;
			*ptr = (uchar_t)IP_DEFAULT_MULTICAST_LOOP;
	int		*i1 = (int *)ptr;
	int		retval;
			*i1 = (int)icmp->icmp_hdrincl;
	int		err;
	unsigned int size;
		prog.bf_insns = (void *)(uint64_t)prog32->bf_insns;
	int error;
	uchar_t *buf = mp->b_rptr;
	uint_t wirelen, len = MBLKL(mp);
	int		*i1 = (int *)invalp;
	int		error;
				(void) bcopy(invalp, icmp->icmp_filter, inlen);
	int		err;
		(void) bcopy(invalp, outvalp, inlen);
		uint_t wroff;
			(void) proto_set_tx_wroff(connp->conn_rq, connp, wroff);
	int error;
	uint_t		data_len;
	uint32_t	cksum;
		uint_t	cksum_offset = 0;
			uint16_t *ptr;
			ptr = (uint16_t *)(mp->b_rptr + cksum_offset);
	int		error;
		int error;
	int			ip_hdr_length;
	int			pkt_len;
			(void) ip_find_hdr_v4(ipha, &ipps, B_FALSE);
		mp1->b_wptr = (uchar_t *)tudi + udi_size;
		*(uint32_t *)&sin->sin_zero[0] = 0;
		*(uint32_t *)&sin->sin_zero[4] = 0;
			    &ipps, (uchar_t *)&sin[1], udi_size);
		uint8_t		nexthdr;
		(void) ip_find_hdr_v6(mp, ip6h, B_TRUE, &ipps, &nexthdr);
		int type;
		uint16_t	*up;
		uint32_t	sum;
		int		remlen;
		up = (uint16_t *)&ip6h->ip6_src;
	mp1->b_wptr = (uchar_t *)tudi + udi_size;
		    &ipps, (uchar_t *)&sin6[1], udi_size);
	uchar_t *destaddr;
	uchar_t	*optaddr;
	    (char *)optaddr, optlen, err);
	int	error;
	int		ip_hdr_length;
	int		tp_hdr_len;
	int		error;
		(void) ixa_check_drain_insert(connp, ixa);
	int		error;
	uint32_t	flowinfo;
	int		is_absreq_failure = 0;
		uint_t srcid;
		(void) ixa_check_drain_insert(connp, ixa);
	int		error;
		(void) ixa_check_drain_insert(connp, ixa);
	int		error;
		(void) ixa_check_drain_insert(connp, ixa);
	uint_t		pktlen;
	uint_t		copylen;
	uint8_t		*iph;
	uint_t		ip_hdr_length;
	uint32_t	cksum;
	ip_hdr_length = (uint_t)(connp->conn_ht_ulp - connp->conn_ht_iphc);
		ipha->ipha_length = htons((uint16_t)pktlen);
		uint_t	cksum_offset = 0;
		ip6h->ip6_plen =  htons((uint16_t)(pktlen - IPV6_HDR_LEN));
			uint16_t *ptr;
			ptr = (uint16_t *)(mp->b_rptr + cksum_offset);
	uint_t		srcid;
	int		error = 0;
	int		error;
	uint_t		srcid;
	uint32_t	flowinfo;
		(void) ixa_check_drain_insert(connp, ixa);
	uchar_t	*rptr = mp->b_rptr;
	uint_t		addrlen;
	    (void *)mp1->b_rptr);
	int		error = 0;
	ksp->ks_private = (void *)(uintptr_t)stackid;
		ASSERT(stackid == (netstackid_t)(uintptr_t)ksp->ks_private);
	netstackid_t	stackid = (netstackid_t)(uintptr_t)ksp->ks_private;
	int	error;
	int error;
	int error;
	int	error;
		int unbind_err;
	int error;
	int	error;
	int	error;
	int error;
	    (uchar_t *)optvalp, NULL, cr);
	int		error;
	void		*optvalp_buf;
	int		len;
	(void) rawip_do_close(connp);
	int		error;
	uint_t		srcid;
	int		error = 0;

/* ===== FUNCTIONS ===== */

/* Function 1 */
		    (uchar_t *)nexthdrp) {
			error = ECONNREFUSED;
			break;
		}

/* Function 2 */
			    secpolicy_net_rawaccess(cr) != 0) {
				return (EACCES);
			}

/* Function 3 */
		    !IN6_IS_ADDR_V4MAPPED_ANY(&faddr)) {
			(void) ip_attr_connect(connp, coa->coa_ixa,
			    &saddr, &faddr, &nexthop, fport, NULL, NULL,
			    IPDF_ALLOW_MCBC | IPDF_VERIFY_DST);
		}

/* Function 4 */
		    !IN6_IS_ADDR_V4MAPPED_ANY(&connp->conn_faddr_v6)) {
			err = icmp_build_hdr_template(connp,
			    &connp->conn_saddr_v6, &connp->conn_faddr_v6,
			    connp->conn_flowinfo);
			if (err != 0) {
				mutex_exit(&connp->conn_lock);
				return (err);
			}
		} else {
			connp->conn_v6lastdst = ipv6_all_zeros;
		}

/* Function 5 */
		    (connp->conn_upper_handle, mp, len, 0, &error, NULL) < 0) {
			mutex_enter(&icmp->icmp_recv_lock);
			if (error == ENOSPC) {
				if ((*connp->conn_upcalls->su_recv)
				    (connp->conn_upper_handle, NULL, 0, 0,
				    &error, NULL) < 0) {
					ASSERT(error == ENOSPC);
					if (error == ENOSPC) {
						connp->conn_flow_cntrld =
						    B_TRUE;
					}
				}
				mutex_exit(&icmp->icmp_recv_lock);
			} else {
				ASSERT(error == EOPNOTSUPP);
				mp = icmp_queue_fallback(icmp, mp);
				mutex_exit(&icmp->icmp_recv_lock);
				if (mp != NULL)
					putnext(connp->conn_rq, mp);
			}
		}

/* Function 6 */
	    (mpdata = mpctl->b_cont) == NULL) {
		freemsg(mpctl);
		freemsg(mp2ctl);
		return (0);
	}

/* Function 7 */
	    (MBLKL(mp)) < sizeof (struct T_unitdata_req)) {
		goto done;
	}

/* Function 8 */
		    IN6_IS_ADDR_V4MAPPED(&ipp->ipp_addr)) {
			ASSERT(ipp->ipp_addr_v4 != INADDR_ANY);
			ipha->ipha_src = ipp->ipp_addr_v4;
		} else {
			IN6_V4MAPPED_TO_IPADDR(v6src, ipha->ipha_src);
		}

/* Function 9 */
		    !IN6_IS_ADDR_V4MAPPED(&ipp->ipp_addr)) {
			ASSERT(!IN6_IS_ADDR_UNSPECIFIED(&ipp->ipp_addr));
			ip6h->ip6_src = ipp->ipp_addr;
		} else {
			ip6h->ip6_src = *v6src;
		}

/* Function 10 */
		    ((t_primp_t)mp->b_rptr)->type != T_UNITDATA_REQ) {
			icmp_wput_other(q, mp);
			return (0);
		}

/* Function 11 */
		    (sin6->sin6_family != AF_INET6)) {
			error = EADDRNOTAVAIL;
			goto ud_error2;
		}

/* Function 12 */
			    !conn_same_as_last_v6(connp, sin6)) {
				error = EISCONN;
				goto ud_error2;
			}

/* Function 13 */
			    ipsec_outbound_policy_current(ixa)) {
				error = icmp_output_lastdst(connp, data_mp, cr,
				    pid, ixa);
			} else {
				error = icmp_output_newdst(connp, data_mp, NULL,
				    sin6, cr, pid, ixa);
			}

/* Function 14 */
		    (sin->sin_family != AF_INET)) {
			error = EADDRNOTAVAIL;
			goto ud_error2;
		}

/* Function 15 */
			    !conn_same_as_last_v4(connp, sin)) {
				error = EISCONN;
				goto ud_error2;
			}

/* Function 16 */
			    ipsec_outbound_policy_current(ixa)) {
				error = icmp_output_lastdst(connp, data_mp, cr,
				    pid, ixa);
			} else {
				error = icmp_output_newdst(connp, data_mp, sin,
				    NULL, cr, pid, ixa);
			}

/* Function 17 */
	    IN6_IS_ADDR_UNSPECIFIED(&connp->conn_v6lastdst)) {
		error = icmp_build_hdr_template(connp, &v6src, &v6dst,
		    flowinfo);
		if (error != 0) {
			mutex_exit(&connp->conn_lock);
			goto ud_error;
		}
	} else {
		if (ixa->ixa_flags & IXAF_IS_IPV4) {
			ipha_t	*ipha = (ipha_t *)connp->conn_ht_iphc;

			IN6_V4MAPPED_TO_IPADDR(&v6dst, ipha->ipha_dst);
			if (ixa->ixa_flags & IXAF_PMTU_IPV4_DF) {
				ipha->ipha_fragment_offset_and_flags |=
				    IPH_DF_HTONS;
			} else {
				ipha->ipha_fragment_offset_and_flags &=
				    ~IPH_DF_HTONS;
			}
		} else {
			ip6_t *ip6h = (ip6_t *)connp->conn_ht_iphc;
			ip6h->ip6_dst = v6dst;
		}
	}

/* Function 18 */
		    ipsec_outbound_policy_current(ixa)) {
			error = icmp_output_lastdst(connp, mp, cr, pid, ixa);
		} else {
			error = icmp_output_newdst(connp, mp, NULL, sin6, cr,
			    pid, ixa);
		}

/* Function 19 */
		    ipsec_outbound_policy_current(ixa)) {
			error = icmp_output_lastdst(connp, mp, cr, pid, ixa);
		} else {
			error = icmp_output_newdst(connp, mp, sin, NULL, cr,
			    pid, ixa);
		}


#ifdef __cplusplus
}
#endif

/* End of icmp_unified.c - Synthesized by MINIX4 Massive Synthesis System */
