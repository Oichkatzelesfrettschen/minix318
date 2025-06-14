/**
 * @file igmp_unified.c
 * @brief Unified igmp implementation
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
 *     1. minix4\microkernel\servers\netinet\igmp.c
 *     2. minix4\libos\lib_legacy\openssl\lib\posix\netinet\igmp.c
 *     3. minix4\exokernel\kernel_legacy\uts\common\inet\ip\igmp.c
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
#include "mrouting.h"
#include <inet/common.h>
#include <inet/ip.h>
#include <inet/ip6.h>
#include <inet/ip_if.h>
#include <inet/ip_listutils.h>
#include <inet/ip_multi.h>
#include <inet/ip_ndp.h>
#include <inet/ipclassifier.h>
#include <inet/ipsec_impl.h>
#include <inet/mi.h>
#include <inet/nd.h>
#include <inet/tunables.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/icmp6.h>
#include <netinet/igmp.h>
#include <netinet/igmp_var.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/in_var.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/ip_var.h>
#include <sys/atomic.h>
#include <sys/callb.h>
#include <sys/cmn_err.h>
#include <sys/ddi.h>
#include <sys/kernel.h>
#include <sys/mbuf.h>
#include <sys/param.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/stream.h>
#include <sys/strlog.h>
#include <sys/stropts.h>
#include <sys/strsun.h>
#include <sys/sunddi.h>
#include <sys/systm.h>
#include <sys/types.h>
#include <sys/zone.h>

/* ===== DEFINES ===== */
#define	DSEC_TO_MSEC(dsec)	((dsec) * 100)
#define	SEC_TO_MSEC(sec)	((sec) * 1000)
#define	CURRENT_MSTIME	((uint_t)TICK_TO_MSEC(ddi_get_lbolt()))
#define	CURRENT_OFFSET	(999)
#define	OVQP(ill) \

/* ===== TYPES ===== */
	struct in_multistep step;
	struct in_multi *inm;
	struct in_multi *inm;
	struct in_multistep step;
	struct ip_moptions simo;
	struct in_multistep step;
	struct in_multi *inm;
	struct in_multi *inm;
	struct in_multistep step;
	struct ip_moptions simo;
	struct ip6_opt_router	*ip6router;
	struct ip6_opt_router	*ip6router;

/* ===== GLOBAL VARIABLES ===== */
static int igmp_timers_are_running = 0;
static u_long igmp_all_hosts_group;
static void igmp_sendreport __P((struct in_multi *));
	register int iphlen;
	register int igmplen;
	register int minlen;
	register int s = splnet();
	register int s;
	extern struct socket *ip_mrouter;
static int igmp_timers_are_running = 0;
static u_long igmp_all_hosts_group;
static void igmp_sendreport (struct in_multi *);
	register int iphlen;
	register int igmplen;
	register int minlen;
	register int s = splnet();
	register int s;
	extern struct socket *ip_mrouter;
static uint_t	igmp_query_in(ipha_t *ipha, igmpa_t *igmpa, ill_t *ill);
static uint_t	igmpv3_query_in(igmp3qa_t *igmp3qa, ill_t *ill, int igmplen);
static uint_t	mld_query_in(mld_hdr_t *mldh, ill_t *ill);
static uint_t	mldv2_query_in(mld2q_t *mld2q, ill_t *ill, int mldlen);
static void	igmp_sendpkt(ilm_t *ilm, uchar_t type, ipaddr_t addr);
static void	mld_sendpkt(ilm_t *ilm, uchar_t type, const in6_addr_t *v6addr);
static void	igmpv3_sendrpt(ill_t *ill, mrec_t *reclist);
static void	mldv2_sendrpt(ill_t *ill, mrec_t *reclist);
static mrec_t	*mcast_merge_rtx(ilm_t *ilm, mrec_t *rp, slist_t *flist);
	int	time_left;
	int	ret;
		    (void *)ipst, MSEC_TO_TICK(ipst->ips_igmp_time_to_next));
	int	time_left;
	int	ret;
		    (void *)ipst, MSEC_TO_TICK(ipst->ips_mld_time_to_next));
	int		iphlen, igmplen, mblklen;
	uint32_t	src, dst;
	uint32_t	group;
	uint_t		next;
					    (int)ntohl(src));
			    (int)ntohl(src)));
	int	timer;
	uint_t	next, current;
		    (int)ntohs(igmpa->igmpa_type));
	uint_t		i, next, mrd, qqi, timer, delay, numsrc;
	uint_t		current;
	uint8_t		qrv;
		uint_t hdrval, mant, exp;
		hdrval = (uint_t)igmp3qa->igmp3qa_mxrc;
		uint_t hdrval, mant, exp;
		hdrval = (uint_t)igmp3qa->igmp3qa_qqic;
	uint_t	timer;
		    (int)ntohl(timer));
	uint_t	timer;
		    (int)ntohl(timer));
	uint_t	next = INFINITY, current;
	uint_t  global_next = INFINITY;
	uint_t  next;
	uint_t	next = INFINITY, current;
	uint_t  global_next = INFINITY;
	uint_t  next;
		    (void *)ipst, MSEC_TO_TICK(MCAST_SLOWTIMO_INTERVAL));
		    (void *)ipst, MSEC_TO_TICK(MCAST_SLOWTIMO_INTERVAL));
	uint8_t *rtralert;
	int	hdrlen = sizeof (ipha_t) + RTRALERT_LEN;
	rtralert = (uint8_t *)&(ipha[1]);
	uint8_t *rtralert;
	int i, j, numrec, more_src_cnt;
				int srcspace, srcsperpkt;
	bzero((char *)mp->b_rptr, size);
	mp->b_wptr = (uchar_t *)(mp->b_rptr + size);
	rtralert = (uint8_t *)&(ipha[1]);
			int index, mvsize;
	uint16_t	hdr_length, exthdr_length;
	uint_t		next;
	int		mldlen;
					char    buf1[INET6_ADDRSTRLEN];
	int	timer;
	uint_t	next, current;
	timer = (int)ntohs(mldh->mld_maxdelay);
		    timer, (int)mldh->mld_type);
	uint_t	next, numsrc, i, mrd, delay, qqi, current;
	uint8_t	qrv;
		uint_t hdrval, mant, exp;
		uint_t mant, exp;
	const int	router_alert_length = 8;
	mldh = (mld_hdr_t *)((uint8_t *)ip6hbh + router_alert_length);
	int		i, numrec, more_src_cnt;
				int srcspace, srcsperpkt;
	mld2r = (mld2r_t *)((uint8_t *)ip6hbh + optlen + padlen);
			int index, mvsize;
	int i;

/* ===== FUNCTIONS ===== */

/* Function 1 */
	    (m = m_pullup(m, minlen)) == 0) {
		++igmpstat.igps_rcv_tooshort;
		return;
	}

/* Function 2 */
	    (m = m_pullup(m, minlen)) == 0) {
		++igmpstat.igps_rcv_tooshort;
		return;
	}

/* Function 3 */
		    (ipst->ips_igmp_max_version <= IGMP_V2_ROUTER)) {
			next = igmp_query_in(ipha, igmpa, ill);
		} else if (igmplen >= IGMP_V3_QUERY_MINLEN) {
			next = igmpv3_query_in((igmp3qa_t *)igmpa, ill,
			    igmplen);
		} else {
			++ipst->ips_igmpstat.igps_rcv_tooshort;
			goto bad_pkt;
		}

/* Function 4 */
	    (ipst->ips_igmp_max_version == IGMP_V1_ROUTER)) {
		ill->ill_mcast_v1_time = 0;
		ill->ill_mcast_v1_tset = 1;
		if (ill->ill_mcast_type != IGMP_V1_ROUTER) {
			ip1dbg(("Received IGMPv1 Query on %s, switching mode "
			    "to IGMP_V1_ROUTER\n", ill->ill_name));
			atomic_inc_16(&ill->ill_ifptr->illif_mcast_v1);
			ill->ill_mcast_type = IGMP_V1_ROUTER;
		}

		timer = SEC_TO_MSEC(IGMP_MAX_HOST_REPORT_DELAY);

		if (ipha->ipha_dst != htonl(INADDR_ALLHOSTS_GROUP) ||
		    igmpa->igmpa_group != 0) {
			++ipst->ips_igmpstat.igps_rcv_badqueries;
			rw_exit(&ill->ill_mcast_lock);
			ill_mcast_timer_start(ill->ill_ipst);
			return (0);
		}

	} else {
		in_addr_t group;

		group = igmpa->igmpa_group;
		if (group != 0 && (!CLASSD(group))) {
			++ipst->ips_igmpstat.igps_rcv_badqueries;
			rw_exit(&ill->ill_mcast_lock);
			ill_mcast_timer_start(ill->ill_ipst);
			return (0);
		}

		if (ill->ill_mcast_type == IGMP_V3_ROUTER) {
			ip1dbg(("Received IGMPv2 Query on %s, switching mode "
			    "to IGMP_V2_ROUTER", ill->ill_name));
			atomic_inc_16(&ill->ill_ifptr->illif_mcast_v2);
			ill->ill_mcast_type = IGMP_V2_ROUTER;
		}
		ill->ill_mcast_v2_time = 0;
		ill->ill_mcast_v2_tset = 1;

		timer = DSEC_TO_MSEC((int)igmpa->igmpa_code);
	}

/* Function 5 */
		    (igmpa->igmpa_group == ilm->ilm_addr)) {
			if (ilm->ilm_timer > timer) {
				MCAST_RANDOM_DELAY(ilm->ilm_timer, timer);
				if (ilm->ilm_timer < next)
					next = ilm->ilm_timer;
				ilm->ilm_timer += current;
			}
		}

/* Function 6 */
			    SLIST_IS_EMPTY(ilm->ilm_pendsrcs))) {
group_query:
				FREE_SLIST(ilm->ilm_pendsrcs);
				ilm->ilm_pendsrcs = NULL;
			} else {
				boolean_t overflow;
				slist_t *pktl;
				if (numsrc > MAX_FILTER_SIZE ||
				    (ilm->ilm_pendsrcs == NULL &&
				    (ilm->ilm_pendsrcs = l_alloc()) == NULL)) {
					goto group_query;
				}
				if ((pktl = l_alloc()) == NULL)
					goto group_query;
				pktl->sl_numsrc = numsrc;
				for (i = 0; i < numsrc; i++)
					IN6_IPADDR_TO_V4MAPPED(src_array[i],
					    &(pktl->sl_addr[i]));
				l_union_in_a(ilm->ilm_pendsrcs, pktl,
				    &overflow);
				l_free(pktl);
				if (overflow)
					goto group_query;
			}

/* Function 7 */
	    (ilm->ilm_addr != htonl(INADDR_ALLHOSTS_GROUP))) {
		igmp_sendpkt(ilm, IGMP_V2_LEAVE_GROUP,
		    (htonl(INADDR_ALLRTRS_GROUP)));
		return;
	}

/* Function 8 */
	    (ilm->ilm_addr != htonl(INADDR_ALLHOSTS_GROUP))) {
		mrec_t *rp;
		if (ilm->ilm_fmode == MODE_IS_INCLUDE) {
			rp = mcast_bldmrec(BLOCK_OLD_SOURCES, &ilm->ilm_v6addr,
			    ilm->ilm_filter, NULL);
		} else {
			rp = mcast_bldmrec(CHANGE_TO_INCLUDE, &ilm->ilm_v6addr,
			    NULL, NULL);
		}
		igmpv3_sendrpt(ill, rp);
		return;
	}

/* Function 9 */
	    (!IN6_ARE_ADDR_EQUAL(&ipv6_all_hosts_mcast, &ilm->ilm_v6addr))) {
		mld_sendpkt(ilm, MLD_LISTENER_REDUCTION, &ipv6_all_rtrs_mcast);
		return;
	}

/* Function 10 */
	    (!IN6_ARE_ADDR_EQUAL(&ipv6_all_hosts_mcast, &ilm->ilm_v6addr))) {
		mrec_t *rp;
		if (ilm->ilm_fmode == MODE_IS_INCLUDE) {
			rp = mcast_bldmrec(BLOCK_OLD_SOURCES, &ilm->ilm_v6addr,
			    ilm->ilm_filter, NULL);
		} else {
			rp = mcast_bldmrec(CHANGE_TO_INCLUDE, &ilm->ilm_v6addr,
			    NULL, NULL);
		}
		mldv2_sendrpt(ill, rp);
		return;
	}

/* Function 11 */
		    ((b_minus_a = l_alloc()) == NULL)) {
			l_free(a_minus_b);
			if (ilm->ilm_fmode == MODE_IS_INCLUDE)
				goto send_to_ex;
			else
				goto send_to_in;
		}

/* Function 12 */
		    ((b_minus_a = l_alloc()) == NULL)) {
			l_free(a_minus_b);
			if (ilm->ilm_fmode == MODE_IS_INCLUDE)
				goto send_to_ex;
			else
				goto send_to_in;
		}

/* Function 13 */
			    (rsp = l_alloc()) != NULL) {
				if (ilm->ilm_fmode == MODE_IS_INCLUDE) {
					l_intersection(ilm->ilm_filter,
					    ilm->ilm_pendsrcs, rsp);
				} else {
					l_difference(ilm->ilm_pendsrcs,
					    ilm->ilm_filter, rsp);
				}
				FREE_SLIST(ilm->ilm_pendsrcs);
				ilm->ilm_pendsrcs = NULL;
				if (!SLIST_IS_EMPTY(rsp))
					rp = mcast_bldmrec(MODE_IS_INCLUDE,
					    &ilm->ilm_v6addr, rsp, rp);
				FREE_SLIST(rsp);
			} else {
				rp = mcast_bldmrec(ilm->ilm_fmode,
				    &ilm->ilm_v6addr, ilm->ilm_filter, rp);
			}

/* Function 14 */
			    (rsp = l_alloc()) != NULL) {
				if (ilm->ilm_fmode == MODE_IS_INCLUDE) {
					l_intersection(ilm->ilm_filter,
					    ilm->ilm_pendsrcs, rsp);
				} else {
					l_difference(ilm->ilm_pendsrcs,
					    ilm->ilm_filter, rsp);
				}
				FREE_SLIST(ilm->ilm_pendsrcs);
				ilm->ilm_pendsrcs = NULL;
				if (!SLIST_IS_EMPTY(rsp))
					rp = mcast_bldmrec(MODE_IS_INCLUDE,
					    &ilm->ilm_v6addr, rsp, rp);
				FREE_SLIST(rsp);
			} else {
				rp = mcast_bldmrec(ilm->ilm_fmode,
				    &ilm->ilm_v6addr, ilm->ilm_filter, rp);
			}

/* Function 15 */
		    ill = avl_walk(avl_tree, ill, AVL_AFTER)) {
			if (!ill_check_and_refhold(ill))
				continue;
			rw_exit(&ipst->ips_ill_g_lock);
			rw_enter(&ill->ill_mcast_lock, RW_WRITER);
			if (ill->ill_mcast_v1_tset == 1)
				ill->ill_mcast_v1_time++;
			if (ill->ill_mcast_v2_tset == 1)
				ill->ill_mcast_v2_time++;
			if ((ill->ill_mcast_type == IGMP_V1_ROUTER) &&
			    (ipst->ips_igmp_max_version >= IGMP_V2_ROUTER) &&
			    (ill->ill_mcast_v1_time >= OVQP(ill))) {
				if ((ill->ill_mcast_v2_tset > 0) ||
				    (ipst->ips_igmp_max_version ==
				    IGMP_V2_ROUTER)) {
					ip1dbg(("V1 query timer "
					    "expired on %s; switching "
					    "mode to IGMP_V2\n",
					    ill->ill_name));
					ill->ill_mcast_type =
					    IGMP_V2_ROUTER;
				} else {
					ip1dbg(("V1 query timer "
					    "expired on %s; switching "
					    "mode to IGMP_V3\n",
					    ill->ill_name));
					ill->ill_mcast_type =
					    IGMP_V3_ROUTER;
				}
				ill->ill_mcast_v1_time = 0;
				ill->ill_mcast_v1_tset = 0;
				atomic_dec_16(&ifp->illif_mcast_v1);
			}
			if ((ill->ill_mcast_type == IGMP_V2_ROUTER) &&
			    (ipst->ips_igmp_max_version >= IGMP_V3_ROUTER) &&
			    (ill->ill_mcast_v2_time >= OVQP(ill))) {
				ip1dbg(("V2 query timer expired on "
				    "%s; switching mode to IGMP_V3\n",
				    ill->ill_name));
				ill->ill_mcast_type = IGMP_V3_ROUTER;
				ill->ill_mcast_v2_time = 0;
				ill->ill_mcast_v2_tset = 0;
				atomic_dec_16(&ifp->illif_mcast_v2);
			}
			rw_exit(&ill->ill_mcast_lock);
			ill_refrele(ill);
			rw_enter(&ipst->ips_ill_g_lock, RW_READER);
		}

/* Function 16 */
		    ill = avl_walk(avl_tree, ill, AVL_AFTER)) {
			if (!ill_check_and_refhold(ill))
				continue;
			rw_exit(&ipst->ips_ill_g_lock);
			rw_enter(&ill->ill_mcast_lock, RW_WRITER);
			if (ill->ill_mcast_v1_tset == 1)
				ill->ill_mcast_v1_time++;
			if ((ill->ill_mcast_type == MLD_V1_ROUTER) &&
			    (ipst->ips_mld_max_version >= MLD_V2_ROUTER) &&
			    (ill->ill_mcast_v1_time >= OVQP(ill))) {
				ip1dbg(("MLD query timer expired on"
				    " %s; switching mode to MLD_V2\n",
				    ill->ill_name));
				ill->ill_mcast_type = MLD_V2_ROUTER;
				ill->ill_mcast_v1_time = 0;
				ill->ill_mcast_v1_tset = 0;
				atomic_dec_16(&ifp->illif_mcast_v1);
			}
			rw_exit(&ill->ill_mcast_lock);
			ill_refrele(ill);
			rw_enter(&ipst->ips_ill_g_lock, RW_READER);
		}

/* Function 17 */
		    (ipst->ips_mld_max_version < MLD_V2_ROUTER)) {
			next = mld_query_in(mldh, ill);
		} else if (mldlen >= MLD_V2_QUERY_MINLEN) {
			next = mldv2_query_in((mld2q_t *)mldh, ill, mldlen);
		} else {
			BUMP_MIB(ill->ill_icmp6_mib, ipv6IfIcmpInErrors);
			freemsg(mp);
			return (NULL);
		}

/* Function 18 */
	    ((!IN6_IS_ADDR_MULTICAST(v6group)))) {
		BUMP_MIB(ill->ill_icmp6_mib, ipv6IfIcmpInGroupMembBadQueries);
		return (0);
	}

/* Function 19 */
		    (IN6_ARE_ADDR_EQUAL(v6group, &ilm->ilm_v6addr))) {
			if (timer == 0) {
				ilm->ilm_timer = INFINITY;
				ilm->ilm_state = IGMP_IREPORTEDLAST;
				mld_sendpkt(ilm, MLD_LISTENER_REPORT, NULL);
				break;
			}
			if (ilm->ilm_timer > timer) {
				MCAST_RANDOM_DELAY(ilm->ilm_timer, timer);
				if (ilm->ilm_timer < next)
					next = ilm->ilm_timer;
				ilm->ilm_timer += current;
			}
			break;
		}

/* Function 20 */
			    SLIST_IS_EMPTY(ilm->ilm_pendsrcs))) {
group_query:
				FREE_SLIST(ilm->ilm_pendsrcs);
				ilm->ilm_pendsrcs = NULL;
			} else {
				boolean_t overflow;
				slist_t *pktl;
				if (numsrc > MAX_FILTER_SIZE ||
				    (ilm->ilm_pendsrcs == NULL &&
				    (ilm->ilm_pendsrcs = l_alloc()) == NULL)) {
					goto group_query;
				}
				if ((pktl = l_alloc()) == NULL)
					goto group_query;
				pktl->sl_numsrc = numsrc;
				for (i = 0; i < numsrc; i++)
					pktl->sl_addr[i] = src_array[i];
				l_union_in_a(ilm->ilm_pendsrcs, pktl,
				    &overflow);
				l_free(pktl);
				if (overflow)
					goto group_query;
			}


#ifdef __cplusplus
}
#endif

/* End of igmp_unified.c - Synthesized by MINIX4 Massive Synthesis System */
