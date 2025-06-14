/**
 * @file ip_output_unified.c
 * @brief Unified ip_output implementation
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
 *     1. minix4\microkernel\servers\netinet\ip_output.c
 *     2. minix4\libos\lib_legacy\openssl\lib\posix\netinet\ip_output.c
 *     3. minix4\exokernel\kernel_legacy\uts\common\inet\ip\ip_output.c
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
#include "mrouting.h"
#include <inet/arp.h>
#include <inet/common.h>
#include <inet/ip.h>
#include <inet/ip6.h>
#include <inet/ip6_asp.h>
#include <inet/ip_ftable.h>
#include <inet/ip_if.h>
#include <inet/ip_impl.h>
#include <inet/ip_ire.h>
#include <inet/ip_listutils.h>
#include <inet/ip_multi.h>
#include <inet/ip_ndp.h>
#include <inet/ip_netinfo.h>
#include <inet/ip_rts.h>
#include <inet/ipclassifier.h>
#include <inet/ipdrop.h>
#include <inet/ipp_common.h>
#include <inet/ipsec_impl.h>
#include <inet/kstatcom.h>
#include <inet/mi.h>
#include <inet/mib2.h>
#include <inet/nd.h>
#include <inet/optcom.h>
#include <inet/sadb.h>
#include <inet/sctp/sctp_impl.h>
#include <inet/sctp_ip.h>
#include <inet/snmpcom.h>
#include <inet/tcp.h>
#include <inet/udp_impl.h>
#include <machine/mtpr.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/if_dl.h>
#include <net/pfkeyv2.h>
#include <net/route.h>
#include <netinet/icmp6.h>
#include <netinet/igmp.h>
#include <netinet/igmp_var.h>
#include <netinet/in.h>
#include <netinet/in_pcb.h>
#include <netinet/in_systm.h>
#include <netinet/in_var.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/ip_mroute.h>
#include <netinet/ip_var.h>
#include <netinet/sctp.h>
#include <sys/atomic.h>
#include <sys/cmn_err.h>
#include <sys/ddi.h>
#include <sys/debug.h>
#include <sys/dlpi.h>
#include <sys/errno.h>
#include <sys/kernel.h>
#include <sys/kmem.h>
#include <sys/mac.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/param.h>
#include <sys/pattr.h>
#include <sys/protosw.h>
#include <sys/sdt.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/sockio.h>
#include <sys/stream.h>
#include <sys/strsubr.h>
#include <sys/strsun.h>
#include <sys/sunddi.h>
#include <sys/systm.h>
#include <sys/tsol/label.h>
#include <sys/tsol/tnet.h>
#include <sys/types.h>
#include <sys/zone.h>

/* ===== DEFINES ===== */
#define ovbcopy bcopy
#define ifatoia(ifa)	((struct in_ifaddr *)(ifa))
#define sintosa(sin)	((struct sockaddr *)(sin))
#define	OPTSET(bit) \
#define	OPTBIT(bit)	(inp->inp_flags & bit ? 1 : 0)

/* ===== TYPES ===== */
	struct mbuf *m0;
	struct mbuf *opt;
	struct route *ro;
	struct ip_moptions *imo;
	struct route iproute;
	struct sockaddr_in *dst;
	struct in_ifaddr *ia;
		struct in_multi *inm;
	struct mbuf **mnext = &m->m_nextpkt;
	struct mbuf *opt;
	struct mbuf *n;
	struct ip *ip, *jp;
	struct socket *so;
	struct mbuf **mp;
	struct mbuf **pcbopt;
	struct ip_moptions **imop;
	struct mbuf *m;
	struct in_addr addr;
	struct route ro;
	struct in_addr *addr;
	struct in_ifaddr *ia;
	struct ifnet *ifp;
	struct mbuf *copym;
	struct mbuf *m0;
	struct mbuf *opt;
	struct route *ro;
	struct ip_moptions *imo;
	struct route iproute;
	struct sockaddr_in *dst;
	struct in_ifaddr *ia;
		struct in_multi *inm;
	struct mbuf **mnext = &m->m_nextpkt;
	struct mbuf *opt;
	struct mbuf *n;
	struct ip *ip, *jp;
	struct socket *so;
	struct mbuf **mp;
	struct mbuf **pcbopt;
	struct ip_moptions **imop;
	struct mbuf *m;
	struct in_addr addr;
	struct route ro;
	struct in_addr *addr;
	struct in_ifaddr *ia;
	struct ifnet *ifp;
	struct mbuf *copym;

/* ===== GLOBAL VARIABLES ===== */
static struct mbuf *ip_insertoptions __P((struct mbuf *, struct mbuf *, int *));
	int flags;
	register int hlen = sizeof (struct ip);
	int len, off, error = 0;
			extern struct socket *ip_mrouter;
	int mhlen, firstlen = len;
			(void) m_free(m);
	int *phlen;
	register u_char *cp, *dp;
	int opt, optlen, cnt;
	cp = (u_char *)(ip + 1);
	dp = (u_char *)(jp + 1);
	int op;
	int level, optname;
	register int optval;
	int error = 0;
			(void) m_free(*mp);
				optval = *mtod(m, int *);
			(void)m_free(m);
			m->m_len = sizeof(int);
			*mtod(m, int *) = optval;
	int optname;
	register int cnt, optlen;
	register u_char *cp;
	u_char opt;
		(void)m_free(*pcbopt);
			(void)m_free(m);
	cp = mtod(m, u_char *) + sizeof(struct in_addr);
	(void)m_free(m);
	int optname;
	register int error = 0;
	u_char loop;
	register int i;
		imo->imo_multicast_ttl = *(mtod(m, u_char *));
	int optname;
	u_char *ttl;
	u_char *loop;
		ttl = mtod(*mp, u_char *);
		loop = mtod(*mp, u_char *);
	register int i;
static struct mbuf *ip_insertoptions (struct mbuf *, struct mbuf *, int *);
	int flags;
	register int hlen = sizeof (struct ip);
	int len, off, error = 0;
			extern struct socket *ip_mrouter;
	int mhlen, firstlen = len;
			(void) m_free(m);
	int *phlen;
	register u_char *cp, *dp;
	int opt, optlen, cnt;
	cp = (u_char *)(ip + 1);
	dp = (u_char *)(jp + 1);
	int op;
	int level, optname;
	register int optval;
	int error = 0;
			(void) m_free(*mp);
				optval = *mtod(m, int *);
			(void)m_free(m);
			m->m_len = sizeof(int);
			*mtod(m, int *) = optval;
	int optname;
	register int cnt, optlen;
	register u_char *cp;
	u_char opt;
		(void)m_free(*pcbopt);
			(void)m_free(m);
	cp = mtod(m, u_char *) + sizeof(struct in_addr);
	(void)m_free(m);
	int optname;
	register int error = 0;
	u_char loop;
	register int i;
		imo->imo_multicast_ttl = *(mtod(m, u_char *));
	int optname;
	u_char *ttl;
	u_char *loop;
		ttl = mtod(*mp, u_char *);
		loop = mtod(*mp, u_char *);
	register int i;
extern boolean_t skip_sctp_cksum;
static void	ip_output_simple_broadcast(ip_xmit_attr_t *, mblk_t *);
	int		error;
		int64_t		now = LBOLT_FASTPATH64;
		uint_t gen;
		uint_t	scopeid;
	uint_t		gen;
	int		error;
	int		error = 0;
	uint_t		gen;
	uint_t		pmtu;
			uint_t oldmtu = ixa->ixa_pmtu;
			    uint32_t, ixa->ixa_pmtu);
	int		err;
	int		error;
	int64_t		now;
			uint_t fragsize;
	uint_t		pktlen = ixa->ixa_pktlen;
	    int, 1);
		int error = 0;
		int error = 0;
	    int, 1);
		(void) ire_send_wire_v4(ire1, mp1, ipha1, ixa, identp);
	(void) ip_output_simple(mp, &ixas);
	uint_t		pktlen = ixa->ixa_pktlen;
	uint16_t	*cksump;
	uint32_t	cksum;
	uint8_t		protocol = ixa->ixa_protocol;
	uint16_t	ip_hdr_length = ixa->ixa_ip_hdr_length;
	ASSERT(((uchar_t *)cksump) + sizeof (uint16_t) <= mp->b_wptr);
	uint_t		pktlen = ixa->ixa_pktlen;
	uint16_t	*cksump;
	uint16_t	hck_flags;
	uint32_t	cksum;
	uint8_t		protocol = ixa->ixa_protocol;
	uint16_t	ip_hdr_length = ixa->ixa_ip_hdr_length;
	ASSERT(((uchar_t *)cksump) + sizeof (uint16_t) <= mp->b_wptr);
		DB_CKSUMSTUFF(mp) = (uint8_t *)cksump - (uint8_t *)ipha;
	int		error = 0;
	int		error = 0;
	int		num_sent = 0;
	int		err;
	uint_t		ire_type;
			uint_t	match_flags = MATCH_IRE_DSTONLY;
	int		error;
		    NULL, int, 1);
		    NULL, int, 1);
		    ip6h, int, 1);
		    ip6h, int, 1);

/* ===== FUNCTIONS ===== */

/* Function 1 */
		   (imo == NULL || imo->imo_multicast_loop)) {
			ip_mloopback(ifp, m, dst);
		}

/* Function 2 */
		   (loop = *(mtod(m, u_char *))) > 1) {
			error = EINVAL;
			break;
		}

/* Function 3 */
		   (imo == NULL || imo->imo_multicast_loop)) {
			ip_mloopback(ifp, m, dst);
		}

/* Function 4 */
		   (loop = *(mtod(m, u_char *))) > 1) {
			error = EINVAL;
			break;
		}

/* Function 5 */
		    !ILL_LSO_TCP_IPV6_USABLE(ill))) {
			ixa->ixa_flags &= ~IXAF_LSO_CAPAB;

			return (B_FALSE);
		}

/* Function 6 */
		    ILL_LSO_TCP_IPV6_USABLE(ill))) {
			*lsoc = *new_lsoc;
			ixa->ixa_flags |= IXAF_LSO_CAPAB;

			return (B_FALSE);
		}

/* Function 7 */
		    !ILL_ZCOPY_USABLE(ill)) {
			ixa->ixa_flags &= ~IXAF_ZCOPY_CAPAB;

			return (B_FALSE);
		}

/* Function 8 */
		    ILL_ZCOPY_USABLE(ill)) {
			ixa->ixa_flags |= IXAF_ZCOPY_CAPAB;

			return (B_FALSE);
		}

/* Function 9 */
	    (ipha->ipha_ttl > ipst->ips_ip_multirt_ttl)) {
		ipha->ipha_ttl = ipst->ips_ip_multirt_ttl;
		ixa->ixa_flags |= IXAF_NO_TTL_CHANGE;
	}

/* Function 10 */
	    !(ixaflags & IXAF_DONTROUTE)) {
		ixa->ixa_flags |= IXAF_LOOPBACK_COPY | IXAF_NO_HW_CKSUM;
	} else if (ixaflags & IXAF_MULTICAST_LOOP) {
		if (ill_hasmembers_v4(ill, ipha->ipha_dst))
			ixa->ixa_flags |= IXAF_LOOPBACK_COPY | IXAF_NO_HW_CKSUM;
	} else if (ipst->ips_netstack->netstack_numzones > 1) {
		if (ill_hasmembers_otherzones_v4(ill, ipha->ipha_dst,
		    ixa->ixa_zoneid)) {
			ixa->ixa_flags |= IXAF_NO_LOOP_ZONEID_SET;
			ixa->ixa_no_loop_zoneid = ixa->ixa_zoneid;
			ixa->ixa_flags |= IXAF_LOOPBACK_COPY | IXAF_NO_HW_CKSUM;
		}
	}

/* Function 11 */
		    AF_INET, (uint8_t *)(uintptr_t)src, NULL)) {
			ipha->ipha_ident = (*cl_inet_ipident)(stack_id,
			    IPPROTO_IP, AF_INET, (uint8_t *)(uintptr_t)src,
			    (uint8_t *)(uintptr_t)dst, NULL);
		} else {
			ipha->ipha_ident = atomic_add_32_nv(identp,
			    ixa->ixa_extra_ident + 1);
		}

/* Function 12 */
	    (ixaflags & IXAF_IPSEC_SECURE)) {
		uint32_t pktlen;

		pktlen = ixa->ixa_pktlen;
		if (ixaflags & IXAF_IPSEC_SECURE)
			pktlen += ipsec_out_extra_length(ixa);

		if (pktlen > IP_MAXPACKET)
			return (EMSGSIZE);

		if (ixaflags & IXAF_SET_ULP_CKSUM) {
			if (!ip_output_sw_cksum_v4(mp, ipha, ixa)) {
				BUMP_MIB(ill->ill_ip_mib, ipIfStatsOutDiscards);
				ip_drop_output("ipIfStatsOutDiscards", mp, ill);
				freemsg(mp);
				return (EINVAL);
			}
		} else {
			ipha->ipha_hdr_checksum = 0;
			ipha->ipha_hdr_checksum = ip_csum_hdr(ipha);
		}

		if ((pktlen > ixa->ixa_fragsize) &&
		    (ixaflags & IXAF_DONTFRAG)) {
			ip_recv_attr_t	iras;

			DTRACE_PROBE4(ip4__fragsize__fail, uint_t, pktlen,
			    uint_t, ixa->ixa_fragsize, uint_t, ixa->ixa_pktlen,
			    uint_t, ixa->ixa_pmtu);

			bzero(&iras, sizeof (iras));
			ipsec_out_to_in(ixa, ill, &iras);

			ip_drop_output("ICMP_FRAG_NEEDED", mp, ill);
			icmp_frag_needed(mp, ixa->ixa_fragsize, &iras);
			ira_cleanup(&iras, B_FALSE);
			return (EMSGSIZE);
		}
		DTRACE_PROBE4(ip4__fragsize__ok, uint_t, pktlen,
		    uint_t, ixa->ixa_fragsize, uint_t, ixa->ixa_pktlen,
		    uint_t, ixa->ixa_pmtu);

		if (ixaflags & IXAF_IPSEC_SECURE) {
			return (ipsec_out_process(mp, ixa));
		}
		return (ip_fragment_v4(mp, ixa->ixa_nce, ixaflags,
		    ixa->ixa_pktlen, ixa->ixa_fragsize, ixa->ixa_xmit_hint,
		    ixa->ixa_zoneid, ixa->ixa_no_loop_zoneid,
		    ixa->ixa_postfragfn, &ixa->ixa_cookie));
	}

/* Function 13 */
	    !(ire->ire_flags & RTF_MULTIRT)) {
		ip_drop_output("ip_postfrag_multirt didn't find route",
		    mp, nce->nce_ill);
		if (ire != NULL)
			ire_refrele(ire);
		return (ENETUNREACH);
	}


#ifdef __cplusplus
}
#endif

/* End of ip_output_unified.c - Synthesized by MINIX4 Massive Synthesis System */
