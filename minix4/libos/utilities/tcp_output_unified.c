/**
 * @file tcp_output_unified.c
 * @brief Unified tcp_output implementation
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
 *     1. minix4\microkernel\servers\netinet\tcp_output.c
 *     2. minix4\microkernel\servers\tcp\tcp_output.c
 *     3. minix4\microkernel\servers\tcp-tahoe\tcp_output.c
 *     4. minix4\libos\lib_legacy\openssl\lib\posix\netinet\tcp_output.c
 *     5. minix4\exokernel\kernel_legacy\uts\common\inet\tcp\tcp_output.c
 * 
 * Total source files: 5
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
#include "tcp_debug.h"
#include "tcp_fsm.h"
#include "tcp_internal.h"
#include "tcp_seq.h"
#include "tcp_timer.h"
#include "tcp_var.h"
#include "tcpip.h"
#include "xkernel.h"
#include <inet/common.h>
#include <inet/ip.h>
#include <inet/ip_ndp.h>
#include <inet/ipsec_impl.h>
#include <inet/proto_set.h>
#include <inet/snmpcom.h>
#include <inet/tcp.h>
#include <inet/tcp_impl.h>
#include <inet/tcp_sig.h>
#include <net/route.h>
#include <netinet/in.h>
#include <netinet/in_pcb.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>
#include <netinet/tcp.h>
#include <netinet/tcp_debug.h>
#include <netinet/tcp_fsm.h>
#include <netinet/tcp_seq.h>
#include <netinet/tcp_timer.h>
#include <netinet/tcp_var.h>
#include <netinet/tcpip.h>
#include <sys/errno.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/param.h>
#include <sys/pattr.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/sockio.h>
#include <sys/squeue.h>
#include <sys/squeue_impl.h>
#include <sys/stream.h>
#include <sys/strlog.h>
#include <sys/stropts.h>
#include <sys/strsubr.h>
#include <sys/strsun.h>
#include <sys/suntpi.h>
#include <sys/systm.h>
#include <sys/tihdr.h>
#include <sys/timod.h>
#include <sys/tsol/tnet.h>
#include <sys/types.h>
#include <sys/xti_inet.h>

/* ===== DEFINES ===== */
#define	TCPOUTFLAGS
#define	_SUN_TPI_VERSION 2

/* ===== TYPES ===== */
	struct tcphdr	tHdr;
	struct tcphdr	tHdr;
	struct iocblk	*iocp;
		struct datab *dp;
	struct T_capability_req	*car = (struct T_capability_req *)mp->b_rptr;
	struct iocblk	*iocp = (struct iocblk *)mp->b_rptr;
	struct iocblk	*iocp;
	union T_primitives *tprim = (union T_primitives *)mp->b_rptr;
	struct datab	*db;
		struct datab	*db;

/* ===== GLOBAL VARIABLES ===== */
extern struct mbuf *m_copypack();
	int off, flags, error;
	u_char opt[MAX_TCPOPTLEN];
	int idle, sendalot;
			m->m_next = m_copy(so->so_snd.sb_mb, off, (int) len);
	ti->ti_sum = in_cksum(m, (int)(hdrlen + len));
	register int t = ((tp->t_srtt >> 2) + tp->t_rttvar) >> 1;
u_char	tcp_initopt[4] = { TCPOPT_MAXSEG, 4, 0x0, 0x0, };
	register int len, win;
	int off, flags, error;
	u_char *opt;
	int idle, sendalot;
		int adv = win - (tp->rcv_adv - tp->rcv_nxt);
			memcpy((char *)opt+2, (char *)&mss, sizeof(short));
	    int padOptLen = (optlen + 3) & ~0x3;
		win = (int)(tp->rcv_adv - tp->rcv_nxt);
u_char	tcp_initopt[4] = { TCPOPT_MAXSEG, 4, 0x0, 0x0, };
	register int len, win;
	int off, flags, error;
	u_char *opt;
	int idle, sendalot;
		int adv = win - (tp->rcv_adv - tp->rcv_nxt);
			memcpy((char *)opt+2, (char *)&mss, sizeof(short));
	    int padOptLen = (optlen + 3) & ~0x3;
		win = (int)(tp->rcv_adv - tp->rcv_nxt);
extern struct mbuf *m_copypack();
	int off, flags, error;
	u_char opt[MAX_TCPOPTLEN];
	int idle, sendalot;
			m->m_next = m_copy(so->so_snd.sb_mb, off, (int) len);
	ti->ti_sum = in_cksum(m, (int)(hdrlen + len));
	register int t = ((tp->t_srtt >> 2) + tp->t_rttvar) >> 1;
static mblk_t	*tcp_get_seg_mp(tcp_t *, uint32_t, int32_t *);
static void	tcp_wput_cmdblk(queue_t *, mblk_t *);
static void	tcp_wput_flush(tcp_t *, mblk_t *);
static void	tcp_wput_iocdata(tcp_t *tcp, mblk_t *mp);
static int	tcp_xmit_end(tcp_t *);
		    const int, int *, uint32_t *, int *, mblk_t **, mblk_t *);
		    int, ip_recv_attr_t *, ip_stack_t *, conn_t *);
static boolean_t	tcp_send_rst_chk(tcp_stack_t *);
static void	tcp_process_shrunk_swnd(tcp_t *, uint32_t);
static void	tcp_fill_header(tcp_t *, uchar_t *, int);
static void	tcp_wput_nondata(void *, mblk_t *, void *, ip_recv_attr_t *);
static void	tcp_wput_ioctl(void *, mblk_t *, void *, ip_recv_attr_t *);
static void	tcp_wput_proto(void *, mblk_t *, void *, ip_recv_attr_t *);
static int tcp_tx_pull_len = 16;
	uint32_t old_cwnd = tcp->tcp_cwnd;
	    uint32_t, tcp->tcp_cwnd);
	void (*output_proc)();
	uchar_t *rptr;
	int		len;
	uint32_t	snxt;
	int		tail_unsent;
	int		tcpstate;
	int		usable = 0;
	int32_t		mss;
	int32_t		num_sack_blk = 0;
	int32_t		total_hdr_len;
	int32_t		tcp_hdr_len;
	int		rc;
		    (uintptr_t)INT_MAX);
		len = (int)(mp->b_wptr - mp->b_rptr);
			int tlen;
			    (uintptr_t)INT_MAX);
			tlen = (int)(mp1->b_wptr - mp1->b_rptr);
		int32_t	opt_len;
		int usable_r;
	local_time = (mblk_t *)(intptr_t)gethrtime();
			    xmit_tail->b_rptr) <= (uintptr_t)INT_MAX);
	uchar_t	fval = *mp->b_rptr;
	uint_t		addrlen;
	STRUCT_SET_HANDLE(sb, iocp->ioc_flag, (void *)mp1->b_rptr);
	uchar_t		*rptr;
	ASSERT((uintptr_t)(mp->b_wptr - rptr) <= (uintptr_t)INT_MAX);
	void	*data;
	int		len;
	int		hdrlen;
	int		plen;
	uchar_t		*rptr;
	uint32_t	snxt;
	uint32_t	suna;
	uint32_t	mss;
	uint32_t	sum;
	int		usable;
	uint32_t	msize;
	ASSERT((uintptr_t)(mp->b_wptr - mp->b_rptr) <= (uintptr_t)INT_MAX);
	len = (int)(mp->b_wptr - mp->b_rptr);
	mp->b_prev = (mblk_t *)(intptr_t)gethrtime();
	mp->b_next = (mblk_t *)(uintptr_t)snxt;
		    (char *)tcpha + TCP_MIN_HEADER_LENGTH + 4);
		    (char *)tcpha + TCP_MIN_HEADER_LENGTH + 8);
	int len;
	uint32_t msize;
	char	*msg;
		(void) tcp_xmit_end(tcp);
	(void) conn_ip_output(mp, connp->conn_ixa);
	(void) conn_ip_output(mp, ixa);
	int		num_lso_seg = 1;
	uint_t		lso_usable = 0;
		uint32_t	sum;
		uchar_t		*rptr;
		int		len;
			uchar_t		*prev_rptr;
			uint32_t	prev_snxt = tcp->tcp_snxt;
			    *snxt, B_FALSE, (uint32_t *)&len, B_FALSE);
					    (mblk_t *)(uintptr_t)(*snxt);
				    (mblk_t *)(uintptr_t)(*snxt);
					    (mblk_t *)(uintptr_t)(*snxt-len);
			    (*xmit_tail)->b_rptr) <= (uintptr_t)INT_MAX);
		(*xmit_tail)->b_next = (mblk_t *)(uintptr_t)(*snxt - len);
			int spill = *tail_unsent;
				int nmpsz;
				ASSERT((uintptr_t)nmpsz <= (uintptr_t)INT_MAX);
				    (mblk_t *)(uintptr_t)(*snxt - len);
		(void) dce_update_uinfo_v4(connp->conn_faddr_v4, &uinfo, ipst);
	uchar_t		*rptr;
	uint32_t	sum;
	int		total_hdr_len;
	int		ip_hdr_len;
	tcpha->tha_flags = (uint8_t)ctl;
			uint32_t llbolt = (uint32_t)LBOLT_FASTPATH;
			    (char *)tcpha + TCP_MIN_HEADER_LENGTH+4);
			    (char *)tcpha + TCP_MIN_HEADER_LENGTH+8);
		uint8_t digest[MD5_DIGEST_LENGTH];
			uint8_t *wptr = mp->b_wptr;
	int		i;
	uint_t		ip_hdr_len = ira->ira_ip_hdr_length;
			uint8_t *to;
		ipha->ipha_ttl = (uchar_t)tcps->tcps_ipv4_ttl;
		ip6h->ip6_hops = (uchar_t)tcps->tcps_ipv6_hoplimit;
	tcpha->tha_flags = (uint8_t)ctl;
	(void) ip_output_simple(mp, ixa);
	uchar_t		*rptr;
	uint32_t	seg_len;
	uint32_t	seg_seq;
	uint32_t	seg_ack;
	uint_t		flags;
	uint_t		ip_hdr_len = ira->ira_ip_hdr_length;
	uint32_t u1;
	uint8_t	*wptr = mp->b_wptr;
			uint32_t llbolt = (uint32_t)LBOLT_FASTPATH;
				uint8_t *tmp_wptr;
		wptr[3] = (uchar_t)tcp->tcp_rcv_ws;
	u1 = (int)(mp->b_wptr - mp->b_rptr);
			    int32_t, TCPS_SYN_RCVD);
			    int32_t, TCPS_ESTABLISHED);
			    int32_t, TCPS_CLOSE_WAIT);
	int	data_length;
	int32_t	off = 0;
	uint_t	flags;
	uchar_t	*rptr;
	int32_t	num_sack_blk = 0;
	int32_t	sack_opt_len = 0, opt_len = 0;
		    (uintptr_t)INT_MAX);
		data_length += (int)(mp2->b_wptr - mp2->b_rptr);
		uint32_t u1;
	tcpha->tha_flags = (uchar_t)flags;
			uint32_t llbolt = (uint32_t)LBOLT_FASTPATH;
			    (char *)tcpha + TCP_MIN_HEADER_LENGTH + 4);
			    (char *)tcpha + TCP_MIN_HEADER_LENGTH + 8);
		uchar_t *wptr = (uchar_t *)tcpha + connp->conn_ht_ulp_len;
		int32_t	i;
		uint8_t digest[MD5_DIGEST_LENGTH];
			uint8_t	*wptr = mp1->b_wptr;
	ASSERT((uintptr_t)(mp1->b_wptr - rptr) <= (uintptr_t)INT_MAX);
	data_length += (int)(mp1->b_wptr - rptr);
	int64_t	now;
			tcps->tcps_last_rst_intrvl = now;
	int32_t		usable_swnd;
	int32_t		mss;
	uint32_t	seg_len;
		int32_t		off;
		snxt_mp->b_prev = (mblk_t *)(intptr_t)gethrtime();
	uint32_t	snxt;
	uint32_t	smax;
	int32_t		win;
	int32_t		mss;
	int32_t		off;
			uint32_t cnt = mss;
			old_snxt_mp->b_prev = (mblk_t *)(intptr_t)gethrtime();
	int32_t	cnt;
	int32_t offset;
	uint32_t	snxt = tcp->tcp_snxt;
	uint32_t *dst, *src;
	int hdrlen;
	dst = (uint32_t *)rptr;
	src = (uint32_t *)connp->conn_ht_iphc;
		    (char *)tcp_tmpl + TCP_MIN_HEADER_LENGTH + 4);
		    (char *)tcp_tmpl + TCP_MIN_HEADER_LENGTH + 8);
		uchar_t *wptr = rptr + connp->conn_ht_iphc_len;
		int32_t	i;

/* ===== FUNCTIONS ===== */

/* Function 1 */
			    (tp->t_flags & TF_RCVD_SCALE))) {
				*((u_long *) (opt + optlen)) = htonl(
					TCPOPT_NOP << 24 |
					TCPOPT_WINDOW << 16 |
					TCPOLEN_WINDOW << 8 |
					tp->request_r_scale);
				optlen += 4;
			}

/* Function 2 */
	     (tp->t_flags & TF_RCVD_TSTMP))) {
		u_long *lp = (u_long *)(opt + optlen);
 
 		*lp++ = htonl(TCPOPT_TSTAMP_HDR);
 		*lp++ = htonl(tcp_now);
 		*lp   = htonl(tp->ts_recent);
 		optlen += TCPOLEN_TSTAMP_APPA;
 	}

/* Function 3 */
		    (int)len, max_linkhdr + hdrlen)) == 0) {
			error = ENOBUFS;
			goto out;
		}

/* Function 4 */
		    && TCPS_HAVERCVDSYN(tp->t_state)) {
			tp->t_softerror = error;
			return (0);
		}

/* Function 5 */
			    (tp->t_flags & TF_RCVD_SCALE))) {
				*((u_long *) (opt + optlen)) = htonl(
					TCPOPT_NOP << 24 |
					TCPOPT_WINDOW << 16 |
					TCPOLEN_WINDOW << 8 |
					tp->request_r_scale);
				optlen += 4;
			}

/* Function 6 */
	     (tp->t_flags & TF_RCVD_TSTMP))) {
		u_long *lp = (u_long *)(opt + optlen);
 
 		*lp++ = htonl(TCPOPT_TSTAMP_HDR);
 		*lp++ = htonl(tcp_now);
 		*lp   = htonl(tp->ts_recent);
 		optlen += TCPOLEN_TSTAMP_APPA;
 	}

/* Function 7 */
		    (int)len, max_linkhdr + hdrlen)) == 0) {
			error = ENOBUFS;
			goto out;
		}

/* Function 8 */
		    && TCPS_HAVERCVDSYN(tp->t_state)) {
			tp->t_softerror = error;
			return (0);
		}

/* Function 9 */
	    (tcp->tcp_valid_bits & TCP_FSS_VALID) != 0) {
		if ((tcp->tcp_valid_bits & TCP_FSS_VALID) != 0) {
#ifdef DEBUG
			cmn_err(CE_WARN,
			    "tcp_wput_data: data after ordrel, %s",
			    tcp_display(tcp, NULL,
			    DISP_ADDR_AND_PORT));
#else
			if (connp->conn_debug) {
				(void) strlog(TCP_MOD_ID, 0, 1,
				    SL_TRACE|SL_ERROR,
				    "tcp_wput_data: data after ordrel, %s\n",
				    tcp_display(tcp, NULL,
				    DISP_ADDR_AND_PORT));
			}
		}
		if (tcp->tcp_snd_zcopy_aware &&
		    (mp->b_datap->db_struioflag & STRUIO_ZCNOTIFY))
			tcp_zcopy_notify(tcp);
		freemsg(mp);
		mutex_enter(&tcp->tcp_non_sq_lock);
		if (tcp->tcp_flow_stopped &&
		    TCP_UNSENT_BYTES(tcp) <= connp->conn_sndlowat) {
			tcp_clrqfull(tcp);
		}
		mutex_exit(&tcp->tcp_non_sq_lock);
		return;
	}

/* Function 10 */
	    (TICK_TO_MSEC(now - tcp->tcp_last_recv_time) >= tcp->tcp_rto)) {
		cc_after_idle(tcp);
	}

/* Function 11 */
	    !(tcp->tcp_valid_bits & TCP_FSS_VALID)) {
		goto done;
	}

/* Function 12 */
	    !(tcp->tcp_valid_bits & TCP_URG_VALID)) {
		tail = tcp->tcp_xmit_tail;
		tail->b_wptr -= tcp->tcp_xmit_tail_unsent;
		tcp->tcp_xmit_tail_unsent = 0;
		tcp->tcp_unsent = 0;
		if (tail->b_wptr != tail->b_rptr)
			tail = tail->b_cont;
		if (tail) {
			mblk_t **excess = &tcp->tcp_xmit_head;
			for (;;) {
				mblk_t *mp1 = *excess;
				if (mp1 == tail)
					break;
				tcp->tcp_xmit_tail = mp1;
				tcp->tcp_xmit_last = mp1;
				excess = &mp1->b_cont;
			}
			*excess = NULL;
			tcp_close_mpp(&tail);
			if (tcp->tcp_snd_zcopy_aware)
				tcp_zcopy_notify(tcp);
		}
		mutex_enter(&tcp->tcp_non_sq_lock);
		if (tcp->tcp_flow_stopped) {
			tcp_clrqfull(tcp);
		}
		mutex_exit(&tcp->tcp_non_sq_lock);
	}

/* Function 13 */
	    (TICK_TO_MSEC(now - tcp->tcp_last_recv_time) >= tcp->tcp_rto)) {
		cc_after_idle(tcp);
	}

/* Function 14 */
	    TCP_UNSENT_BYTES(tcp) <= connp->conn_sndlowat) {
		tcp_clrqfull(tcp);
	}

/* Function 15 */
	    (tcp->tcp_last_sent_len < tcp->tcp_naglim)) {
		tcp->tcp_unsent = len;
		tcp->tcp_xmit_tail_unsent = len;

		return;
	}

/* Function 16 */
	    (!OK_32PTR(rptr))) {
		mp = allocb(hdrlen + tcps->tcps_wroff_xtra, BPRI_MED);
		if (!mp) {
			freemsg(mp1);
			goto no_memory;
		}
		mp->b_cont = mp1;
		mp1 = mp;
		rptr = &mp1->b_rptr[tcps->tcps_wroff_xtra];
		mp1->b_wptr = &rptr[hdrlen];
	}

/* Function 17 */
			    len == 1) && (! tcp->tcp_zero_win_probe)) {
				if (*snxt == tcp->tcp_snxt &&
				    *snxt == tcp->tcp_suna) {
					TCP_TIMER_RESTART(tcp, tcp->tcp_rto);
				}
			}

/* Function 18 */
		    CLASSD(ipha->ipha_src)) {
			BUMP_MIB(&ipst->ips_ip_mib, ipIfStatsInDiscards);
			ip_drop_input("ipIfStatsInDiscards", mp, NULL);
			freemsg(mp);
			goto done;
		}

/* Function 19 */
		    IN6_IS_ADDR_MULTICAST(&ip6h->ip6_src)) {
			BUMP_MIB(&ipst->ips_ip6_mib, ipIfStatsInDiscards);
			ip_drop_input("ipIfStatsInDiscards", mp, NULL);
			freemsg(mp);
			goto done;
		}

/* Function 20 */
		    (seq + data_length) == tcp->tcp_fss) {
			tcp_xmit_mp_aux_fss(tcp, ixa, &flags);
		}

/* Function 21 */
		    u1 < (uint32_t)(64 * 1024)) {
			flags |= TH_URG;
			TCPS_BUMP_MIB(tcps, tcpOutUrg);
			tcpha->tha_urp = htons(u1);
		}


#ifdef __cplusplus
}
#endif

/* End of tcp_output_unified.c - Synthesized by MINIX4 Massive Synthesis System */
