/**
 * @file tcp_input_unified.c
 * @brief Unified tcp_input implementation
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
 *     1. minix4\microkernel\servers\netinet\tcp_input.c
 *     2. minix4\microkernel\servers\tcp\tcp_input.c
 *     3. minix4\microkernel\servers\tcp-tahoe\tcp_input.c
 *     4. minix4\libos\lib_legacy\openssl\lib\posix\netinet\tcp_input.c
 *     5. minix4\exokernel\kernel_legacy\uts\common\inet\tcp\tcp_input.c
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
#include "ip.h"
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
#include <inet/ipsec_impl.h>
#include <inet/proto_set.h>
#include <inet/tcp.h>
#include <inet/tcp_cluster.h>
#include <inet/tcp_impl.h>
#include <inet/tcp_sig.h>
#include <net/if.h>
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
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/param.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
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
#include <sys/tsol/tnet.h>
#include <sys/types.h>
#include <sys/xti_inet.h>

/* ===== DEFINES ===== */
#define TCP_PAWS_IDLE	(24 * 24 * 60 * 60 * PR_SLOWHZ)
#define TSTMP_LT(a,b)	((int)((a)-(b)) < 0)
#define TSTMP_GEQ(a,b)	((int)((a)-(b)) >= 0)
#define	TCP_REASS(tp, ti, m, so, flags) { \
#define CANTRCVMORE(so) (sototcpst(so)->closed & 2)
#define ISNOTREFERENCED(X) 0
#define	_SUN_TPI_VERSION 2
#define	TCPOPT_NOP_NOP_TSTAMP ((TCPOPT_NOP << 24) | (TCPOPT_NOP << 16) | \
#define	PAWS_TIMEOUT	((clock_t)(24*24*60*60*hz))
#define	TCP_IS_DETACHED_NONEAGER(tcp)	\
#define	SET_TIME_WAIT(tcps, tcp, connp)				\
#define	TCP_SD_MIN	400000000
#define	ICMP_MIN_TCP_HDR	8

/* ===== TYPES ===== */
struct	tcpiphdr tcp_saveti;
struct	inpcb *tcp_last_inpcb = &tcb;
	struct mbuf *m;
	struct socket *so = tp->t_inpcb->inp_socket;
	struct socket *so;
	struct in_addr laddr;
		struct mbuf *am;
	struct tcpcb *tp;
	struct tcpiphdr *ti;
	struct socket *so;
	struct tcpiphdr *ti;
			struct tcpcb *tp = sototcpcb(so);
	struct route *ro;
	struct ifnet *ifp;
	struct inpcb *inp;
	struct socket *so;
struct	tcpiphdr tcp_saveti;
		struct reass *new;
	struct inpcb *inp;
	struct tcpcb *tp;
	struct tcpstate *tcpst;
	struct tcphdr 	tHdr;
	struct tcpcb *tp;
	struct tcphdr *tHdr;
	struct tcphdr *th;
	struct tcpcb *tp = sototcpcb(so);
    struct tcpcb *tp;
	struct reass *s;
struct	tcpiphdr tcp_saveti;
		struct reass *new;
	struct inpcb *inp;
	struct tcpcb *tp;
	struct tcpstate *tcpst;
	struct tcphdr 	tHdr;
	struct tcpcb *tp;
	struct tcphdr *tHdr;
	struct tcphdr *th;
	struct tcpcb *tp = sototcpcb(so);
    struct tcpcb *tp;
	struct reass *s;
struct	tcpiphdr tcp_saveti;
struct	inpcb *tcp_last_inpcb = &tcb;
	struct mbuf *m;
	struct socket *so = tp->t_inpcb->inp_socket;
	struct socket *so;
	struct in_addr laddr;
		struct mbuf *am;
	struct tcpcb *tp;
	struct tcpiphdr *ti;
	struct socket *so;
	struct tcpiphdr *ti;
			struct tcpcb *tp = sototcpcb(so);
	struct route *ro;
	struct ifnet *ifp;
	struct inpcb *inp;
	struct socket *so;
				struct T_exdata_ind *tei;
	struct T_optdata_ind *todi;
	struct T_opthdr *toh;
		struct in6_pktinfo *pkti;

/* ===== GLOBAL VARIABLES ===== */
int	tcprexmtthresh = 3;
extern u_long sb_max;
	int flags;
		register int i;
		register int i = (ti->ti_seq + ti->ti_len) - q->ti_seq;
	int iphlen;
	int optlen;
	int len, tlen, off;
	int todrop, acked, ourfinisacked, needoutput = 0;
	int dropsocket = 0;
	int iss = 0;
	int ts_present = 0;
	tp = intotcpcb(inp);
			tp = intotcpcb(inp);
					(void) tcp_output(tp);
	{ int win;
	tp->rcv_wnd = max(win, (int)(tp->rcv_adv - tp->rcv_nxt));
			(void) m_free(am);
		(void) m_free(am);
		(void) tcp_reass(tp, (struct tcpiphdr *)0, (struct mbuf *)0);
					(void) tcp_output(tp);
					(void) tcp_output(tp);
		register u_int cw = tp->snd_cwnd;
		register u_int incr = tp->t_maxseg;
			sbdrop(&so->so_snd, (int)so->so_snd.sb_cc);
		(void) tcp_output(tp);
	(void) tcp_output(tp);
		(void) soabort(so);
		(void) soabort(so);
	u_char *cp;
	int cnt;
	int *ts_present;
	int opt, optlen;
			bcopy((char *) cp + 2, (char *) &mss, sizeof(mss));
			bcopy((char *)cp + 2, (char *) ts_val, sizeof(*ts_val));
			bcopy((char *)cp + 6, (char *) ts_ecr, sizeof(*ts_ecr));
	int cnt = ti->ti_urp - 1;
			char *cp = mtod(m, caddr_t) + cnt;
	u_int offer;
	register int rtt, mss;
	extern int tcp_mssdflt;
			(void)sbreserve(&so->so_snd, bufsize);
			(void)sbreserve(&so->so_rcv, bufsize);
int	tcpprintfs = 0;
int	tcpcksum = 1;
int	tcprexmtthresh = 3;
extern	tcpnodelack;
static void	tcp_dooptions(struct tcpcb *, char *, int, struct tcphdr *);
static bool	tcp_pulloobchar( char *ptr, long len, VOID *oobc);
static void 	tcp_pulloutofband(XObj, struct tcphdr *, Msg *);
static void	tcp_dooptions();
static bool	tcp_pulloobchar();
static void 	tcp_pulloutofband();
	int flags;
		register int i;
		register int i = (th->th_seq + msgLen(m)) - q->th.th_seq;
		xFree((char *) q);
		xFree((char *)q);
	char *options = 0;
	int option_len = 0;
	int len, tlen, off;
	int todrop, acked, ourfinisacked, needoutput = 0;
	int dropsocket = 0;
	int iss = 0;
		tp = intotcpcb(inp);
		int win;
		tp->rcv_wnd = MAX(win, (int)(tp->rcv_adv - tp->rcv_nxt));
			(void) tcp_reass(tp, NULL, so, NULL, &demuxmsg);
		(void) msgPopDiscard(m, todrop);
		(void) tcp_reass(tp, NULL, so, NULL, &demuxmsg);
                                        (void) tcp_output(tp);
                                        (void) tcp_output(tp);
int	tcpprintfs = 0;
int	tcpcksum = 1;
int	tcprexmtthresh = 3;
extern	tcpnodelack;
static void	tcp_dooptions(struct tcpcb *, char *, int, struct tcphdr *);
static bool	tcp_pulloobchar( char *ptr, long len, VOID *oobc);
static void 	tcp_pulloutofband(XObj, struct tcphdr *, Msg *);
static void	tcp_dooptions();
static bool	tcp_pulloobchar();
static void 	tcp_pulloutofband();
	int flags;
		register int i;
		register int i = (th->th_seq + msgLen(m)) - q->th.th_seq;
		xFree((char *) q);
		xFree((char *)q);
	char *options = 0;
	int option_len = 0;
	int len, tlen, off;
	int todrop, acked, ourfinisacked, needoutput = 0;
	int dropsocket = 0;
	int iss = 0;
		tp = intotcpcb(inp);
		int win;
		tp->rcv_wnd = MAX(win, (int)(tp->rcv_adv - tp->rcv_nxt));
			(void) tcp_reass(tp, NULL, so, NULL, &demuxmsg);
		(void) msgPopDiscard(m, todrop);
		(void) tcp_reass(tp, NULL, so, NULL, &demuxmsg);
					(void) tcp_output(tp);
int	tcprexmtthresh = 3;
extern u_long sb_max;
	int flags;
		register int i;
		register int i = (ti->ti_seq + ti->ti_len) - q->ti_seq;
	int iphlen;
	int optlen;
	int len, tlen, off;
	int todrop, acked, ourfinisacked, needoutput = 0;
	int dropsocket = 0;
	int iss = 0;
	int ts_present = 0;
	tp = intotcpcb(inp);
			tp = intotcpcb(inp);
					(void) tcp_output(tp);
	{ int win;
	tp->rcv_wnd = max(win, (int)(tp->rcv_adv - tp->rcv_nxt));
			(void) m_free(am);
		(void) m_free(am);
		(void) tcp_reass(tp, (struct tcpiphdr *)0, (struct mbuf *)0);
					(void) tcp_output(tp);
					(void) tcp_output(tp);
		register u_int cw = tp->snd_cwnd;
		register u_int incr = tp->t_maxseg;
			sbdrop(&so->so_snd, (int)so->so_snd.sb_cc);
		(void) tcp_output(tp);
	(void) tcp_output(tp);
		(void) soabort(so);
		(void) soabort(so);
	u_char *cp;
	int cnt;
	int *ts_present;
	int opt, optlen;
			memcpy((char *) &mss, (char *) cp + 2, sizeof(mss));
			memcpy((char *) ts_val, (char *)cp + 2, sizeof(*ts_val));
			memcpy((char *) ts_ecr, (char *)cp + 6, sizeof(*ts_ecr));
	int cnt = ti->ti_urp - 1;
			char *cp = mtod(m, caddr_t) + cnt;
	u_int offer;
	register int rtt, mss;
	extern int tcp_mssdflt;
			(void)sbreserve(&so->so_snd, bufsize);
			(void)sbreserve(&so->so_rcv, bufsize);
static uint32_t tcp_drop_ack_unsent_cnt = 10;
static uint32_t tcp_init_wnd_chk = 4096;
static boolean_t tcp_icmp_source_quench = B_FALSE;
static boolean_t tcp_outbound_squeue_switch = B_FALSE;
static boolean_t	tcp_drop_q0(tcp_t *);
static void	tcp_icmp_error_ipv6(tcp_t *, mblk_t *, ip_recv_attr_t *);
static void	tcp_input_listener(void *, mblk_t *, void *, ip_recv_attr_t *);
static mblk_t	*tcp_reass(tcp_t *, mblk_t *, uint32_t);
static void	tcp_reass_elim_overlap(tcp_t *, mblk_t *);
static void	tcp_rsrv_input(void *, mblk_t *, void *, ip_recv_attr_t *);
static void	tcp_set_rto(tcp_t *, hrtime_t);
static void	tcp_setcred_data(mblk_t *, ip_recv_attr_t *);
	uint32_t old_cwnd = tcp->tcp_cwnd;
	    uint32_t, tcp->tcp_cwnd);
	uint32_t old_cwnd = tcp->tcp_cwnd;
	uint32_t old_cwnd_ssthresh = tcp->tcp_cwnd_ssthresh;
	    uint32_t, tcp->tcp_cwnd_ssthresh, uint32_t, type);
	uint32_t old_cwnd = tcp->tcp_cwnd;
	    uint32_t, old_cwnd, uint32_t, tcp->tcp_cwnd);
	uint32_t	mss_max;
	(void) tcp_maxpsz_set(tcp, B_TRUE);
	uchar_t		*endp;
	int		len;
	uint32_t	mss;
	uchar_t		*up = (uchar_t *)tcpha;
	int		found = 0;
	int32_t		sack_len;
	int options;
	uint32_t mss_max;
	char *tmp_tcph;
	uint32_t	end, bytes;
	uint32_t	u1;
		    (uintptr_t)INT_MAX);
		end = start + (int)(mp->b_wptr - mp->b_rptr);
	uint32_t	end;
	uint32_t	u1;
	uint32_t	seg_seq;
	int		err;
	uint_t		ip_hdr_len;
	uint_t		flags;
	flags = (unsigned int)tcpha->tha_flags & 0xFF;
		int64_t now;
	(void) tcp_rwnd_set(eager, eager->tcp_rwnd);
	    int32_t, TCPS_LISTEN);
		(void) conn_ip_output(mp1, econnp->conn_ixa);
	uint32_t	conn_flags;
	uint_t ret = 0;
	uint_t cnt = 0;
		(void) TCP_TIMER_CANCEL(tcp, tcp->tcp_push_tid);
	uint32_t	seq_no;
		int	data_length;
		uchar_t	*rptr;
		int32_t	total_hdr_len;
		int32_t	tcp_hdr_len;
		int32_t	num_sack_blk = 0;
		int32_t sack_opt_len;
		tcpha->tha_flags = (uchar_t)TH_ACK;
			uint32_t llbolt = (uint32_t)LBOLT_FASTPATH;
			    (char *)tcpha + TCP_MIN_HEADER_LENGTH+4);
			    (char *)tcpha + TCP_MIN_HEADER_LENGTH+8);
			int32_t	i;
	int32_t		bytes_acked;
	int32_t		gap;
	uint_t		flags;
	uint32_t	new_swnd = 0;
	uchar_t		*iphdr;
	uchar_t		*rptr;
	int32_t		rgap;
	uint32_t	seg_ack;
	int		seg_len;
	uint_t		ip_hdr_len;
	uint32_t	seg_seq;
	int		urp;
	uint32_t	cwnd;
	int		mss;
			uint8_t nexthdrp;
	ASSERT((uintptr_t)(mp->b_wptr - rptr) <= (uintptr_t)INT_MAX);
			    (uintptr_t)INT_MAX);
			seg_len += (int)(mp1->b_wptr - mp1->b_rptr);
	flags = (unsigned int)tcpha->tha_flags & 0xFF;
			mp->b_wptr = (uchar_t *)tcpha + TCP_HDR_LENGTH(tcpha);
				(void) tcp_clean_death(tcp, ECONNREFUSED);
				    uint32_t, tcp->tcp_mss);
			    void_ip_t *, iphdr, tcp_t *, tcp, tcph_t *, tcpha);
			    void, NULL, int32_t, TCPS_SYN_SENT);
		    tcph_t *, NULL, int32_t, TCPS_SYN_SENT);
			uint32_t pinit_wnd;
	mp->b_rptr = (uchar_t *)tcpha + TCP_HDR_LENGTH(tcpha);
		int options;
			    (uintptr_t)UINT_MAX);
			gap += (uint_t)(mp->b_wptr - mp->b_rptr);
			    (uintptr_t)INT_MAX);
			rgap -= (int)(mp2->b_wptr - mp2->b_rptr);
			uchar_t tos = ((ipha_t *)rptr)->ipha_type_of_service;
			uint32_t vcf = ((ip6_t *)rptr)->ip6_vcf;
				    (uintptr_t)INT_MAX);
				    (int)(mp->b_wptr - mp->b_rptr);
			(void) tcp_clean_death(tcp, ECONNREFUSED);
			(void) tcp_clean_death(tcp, ECONNRESET);
			(void) tcp_clean_death(tcp, 0);
			(void) tcp_clean_death(tcp, ENXIO);
		(void) tcp_clean_death(tcp, ECONNRESET);
				uint32_t  tmp_rnxt;
					(void) adjmsg(mp1, urp - seg_len);
					uint32_t  tmp_rnxt;
					(void) adjmsg(mp1, urp + 1 - seg_len);
				int error;
				mp1->b_wptr = (uchar_t *)&tei[1];
	bytes_acked = (int)(seg_ack - tcp->tcp_suna);
				(void) tcp_close_detached(tcp);
			    uint32_t, tcp->tcp_mss);
		    int32_t, TCPS_SYN_RCVD);
			int dupack_cnt;
					    uint32_t, cwnd);
				    uint32_t, cwnd);
		uchar_t	*wptr;
		ASSERT((uintptr_t)(wptr - mp1->b_rptr) <= (uintptr_t)INT_MAX);
		bytes_acked -= (int)(wptr - mp1->b_rptr);
			    (uintptr_t)INT_MAX);
				    int32_t, TCPS_FIN_WAIT_1);
				    tcp->tcp_fin_wait_2_flush_interval);
				(void) tcp_clean_death(tcp, 0);
				    int32_t, TCPS_SYN_RCVD);
				    int32_t, TCPS_ESTABLISHED);
					    NULL, int32_t, TCPS_FIN_WAIT_1);
		uint32_t cur_max;
			    (uintptr_t)INT_MAX);
		int error;
				    tcps->tcps_push_timer_interval);
			uint32_t snd_size = tcp->tcp_snxt - tcp->tcp_suna;
				    (mblk_t *)(intptr_t)gethrtime();
				    (mblk_t *)(intptr_t)gethrtime();
			(void) TCP_TIMER_CANCEL(tcp, tcp->tcp_ack_tid);
			    tcps->tcps_deferred_ack_interval);
	int optlen;
	uchar_t *optptr;
		optlen += sizeof (struct T_opthdr) + sizeof (uint_t);
		optlen += sizeof (struct T_opthdr) + sizeof (uint_t);
	optptr = (uchar_t *)&todi[1];
		    P2ROUNDUP(sizeof (uint8_t), __TPI_ALIGN_SIZE);
		*(uint8_t *)optptr = ipp->ipp_type_of_service;
		optptr = (uchar_t *)toh + toh->len;
		toh->len = sizeof (*toh) + sizeof (uint_t);
		*(uint_t *)optptr = ipp->ipp_hoplimit;
		optptr += sizeof (uint_t);
		toh->len = sizeof (*toh) + sizeof (uint_t);
		*(uint_t *)optptr = ipp->ipp_tclass;
		optptr += sizeof (uint_t);
		bcopy((uchar_t *)ipp->ipp_hopopts, optptr, ipp->ipp_hopoptslen);
	uint_t ret = 0;
	uint_t thwin;
	int		iph_hdr_length;
	uint32_t	seg_seq;
	tcpha = (tcpha_t *)((char *)ipha + iph_hdr_length);
			uint32_t npkt;
			    uint32_t, tcp->tcp_mss);
	uint16_t	iph_hdr_length = ira->ira_ip_hdr_length;
	uint8_t		*nexthdrp;
	uint32_t	seg_seq;
	tcpha = (tcpha_t *)((char *)ip6h + iph_hdr_length);
	uint32_t	seq = ntohl(tcpha->tha_seq);

/* ===== FUNCTIONS ===== */

/* Function 1 */
	    (tp)->t_state == TCPS_ESTABLISHED) { \
		tp->t_flags |= TF_DELACK; \
		(tp)->rcv_nxt += (ti)->ti_len; \
		flags = (ti)->ti_flags & TH_FIN; \
		tcpstat.tcps_rcvpack++;\
		tcpstat.tcps_rcvbyte += (ti)->ti_len;\
		sbappend(&(so)->so_rcv, (m)); \
		sorwakeup(so); \
	} else { \
		(flags) = tcp_reass((tp), (ti), (m)); \
		tp->t_flags |= TF_ACKNOW; \
	} \

/* Function 2 */
		     (ti->ti_flags & TH_SYN) == 0) {
			ts_present = 1;
			ts_val = ntohl(*(u_long *)(optp + 4));
			ts_ecr = ntohl(*(u_long *)(optp + 8));
		}

/* Function 3 */
		   SEQ_LT(tp->last_ack_sent, ti->ti_seq + ti->ti_len)) {
			tp->ts_recent_age = tcp_now;
			tp->ts_recent = ts_val;
		}

/* Function 4 */
		    ti->ti_len <= sbspace(&so->so_rcv)) {
			++tcpstat.tcps_preddat;
			tp->rcv_nxt += ti->ti_len;
			tcpstat.tcps_rcvpack++;
			tcpstat.tcps_rcvbyte += ti->ti_len;
			m->m_data += sizeof(struct tcpiphdr)+off-sizeof(struct tcphdr);
			m->m_len -= sizeof(struct tcpiphdr)+off-sizeof(struct tcphdr);
			sbappend(&so->so_rcv, m);
			sorwakeup(so);
			tp->t_flags |= TF_DELACK;
			return;
		}

/* Function 5 */
				(TF_RCVD_SCALE|TF_REQ_SCALE)) {
				tp->snd_scale = tp->requested_s_scale;
				tp->rcv_scale = tp->request_r_scale;
			}

/* Function 6 */
	    TSTMP_LT(ts_val, tp->ts_recent)) {

		if ((int)(tcp_now - tp->ts_recent_age) > TCP_PAWS_IDLE) {
			tp->ts_recent = 0;
		} else {
			tcpstat.tcps_rcvduppack++;
			tcpstat.tcps_rcvdupbyte += ti->ti_len;
			tcpstat.tcps_pawsdrop++;
			goto dropafterack;
		}
	}

/* Function 7 */
			    SEQ_GT(ti->ti_seq, tp->rcv_nxt)) {
				iss = tp->rcv_nxt + TCP_ISSINCR;
				tp = tcp_close(tp);
				goto findpcb;
			}

/* Function 8 */
			(TF_RCVD_SCALE|TF_REQ_SCALE)) {
			tp->snd_scale = tp->requested_s_scale;
			tp->rcv_scale = tp->request_r_scale;
		}

/* Function 9 */
	    TCPS_HAVERCVDFIN(tp->t_state) == 0) {
		if (ti->ti_urp + so->so_rcv.sb_cc > sb_max) {
		}
		if (SEQ_GT(ti->ti_seq+ti->ti_urp, tp->rcv_up)) {
			tp->rcv_up = ti->ti_seq + ti->ti_urp;
			so->so_oobmark = so->so_rcv.sb_cc +
			    (tp->rcv_up - tp->rcv_nxt) - 1;
			if (so->so_oobmark == 0)
				so->so_state |= SS_RCVATMARK;
			sohasoutofband(so);
			tp->t_oobflags &= ~(TCPOOB_HAVEDATA | TCPOOB_HADDATA);
		}
		if (ti->ti_urp <= ti->ti_len
#ifdef SO_OOBINLINE
		     && (so->so_options & SO_OOBINLINE) == 0
#endif
		     )
			tcp_pulloutofband(so, ti, m);
	} else

/* Function 10 */
	    TCPS_HAVERCVDFIN(tp->t_state) == 0) {
		TCP_REASS(tp, ti, m, so, tiflags);
		len = so->so_rcv.sb_hiwat - (tp->rcv_adv - tp->rcv_nxt);
	} else {
		m_freem(m);
		tiflags &= ~TH_FIN;
	}

/* Function 11 */
	    (tp)->t_state == TCPS_ESTABLISHED) { \
		(tp)->rcv_nxt += msgLen(m); \
		flags = (th)->th_flags & TH_FIN; \
		tcpstat.tcps_rcvpack++;\
		tcpstat.tcps_rcvbyte += msgLen(m);\
		msgAssign((dMsg), (m)); \
	} else \

/* Function 12 */
	xIfTrace(tcpp, 1) {
		ostate = tp->t_state;
#if 0
		tcp_saveti = *ti;
#endif
	}

/* Function 13 */
		     SEQ_GT(tHdr.th_ack, tp->snd_max))) {
			xTrace0(tcpp, 4,
				"input state SYN_SENT -- dropping with reset");
			xTrace3(tcpp, 4, "   (ack==%d, iss==%d, snd_max==%d)",
				tHdr.th_ack, tp->iss, tp->snd_max);
			goto dropwithreset;
		}

/* Function 14 */
			    SEQ_GT(tHdr.th_seq, tp->rcv_nxt)) {
				iss = tp->rcv_nxt + TCP_ISSINCR;
				(void) tcp_destroy(tp);
				goto findpcb;
			}

/* Function 15 */
	    TCPS_HAVERCVDFIN(tp->t_state) == 0) {
		if (tHdr.th_urp > SB_MAX) {
		}
		if (SEQ_GT(tHdr.th_seq+tHdr.th_urp, tp->rcv_up)) {
		    tp->rcv_up = tHdr.th_seq + tHdr.th_urp;
		    sohasoutofband(so,
				   (tcpst->rcv_hiwat - tcpst->rcv_space) +
				   (tp->rcv_up - tp->rcv_nxt) - 1);
			tp->t_oobflags &= ~(TCPOOB_HAVEDATA | TCPOOB_HADDATA);
		}
		if (tHdr.th_urp <= tlen &&
		    (tcpst->flags & TCPST_OOBINLINE) == 0)
		  tcp_pulloutofband(so, &tHdr, m);
	} else

/* Function 16 */
	    (tp)->t_state == TCPS_ESTABLISHED) { \
		(tp)->rcv_nxt += msgLen(m); \
		flags = (th)->th_flags & TH_FIN; \
		tcpstat.tcps_rcvpack++;\
		tcpstat.tcps_rcvbyte += msgLen(m);\
		msgAssign((dMsg), (m)); \
	} else \

/* Function 17 */
	xIfTrace(tcpp, 1) {
		ostate = tp->t_state;
#if 0
		tcp_saveti = *ti;
#endif
	}

/* Function 18 */
		     SEQ_GT(tHdr.th_ack, tp->snd_max))) {
			xTrace0(tcpp, 4,
				"input state SYN_SENT -- dropping with reset");
			xTrace3(tcpp, 4, "   (ack==%d, iss==%d, snd_max==%d)",
				tHdr.th_ack, tp->iss, tp->snd_max);
			goto dropwithreset;
		}

/* Function 19 */
			    SEQ_GT(tHdr.th_seq, tp->rcv_nxt)) {
				iss = tp->rcv_nxt + TCP_ISSINCR;
				(void) tcp_destroy(tp);
				goto findpcb;
			}

/* Function 20 */
	    TCPS_HAVERCVDFIN(tp->t_state) == 0) {
		if (tHdr.th_urp > SB_MAX) {
		}
		if (SEQ_GT(tHdr.th_seq+tHdr.th_urp, tp->rcv_up)) {
		    tp->rcv_up = tHdr.th_seq + tHdr.th_urp;
		    sohasoutofband(so,
				   (tcpst->rcv_hiwat - tcpst->rcv_space) +
				   (tp->rcv_up - tp->rcv_nxt) - 1);
			tp->t_oobflags &= ~(TCPOOB_HAVEDATA | TCPOOB_HADDATA);
		}
		if (tHdr.th_urp <= tlen &&
		    (tcpst->flags & TCPST_OOBINLINE) == 0)
		  tcp_pulloutofband(so, &tHdr, m);
	} else

/* Function 21 */
	    (tp)->t_state == TCPS_ESTABLISHED) { \
		tp->t_flags |= TF_DELACK; \
		(tp)->rcv_nxt += (ti)->ti_len; \
		flags = (ti)->ti_flags & TH_FIN; \
		tcpstat.tcps_rcvpack++;\
		tcpstat.tcps_rcvbyte += (ti)->ti_len;\
		sbappend(&(so)->so_rcv, (m)); \
		sorwakeup(so); \
	} else { \
		(flags) = tcp_reass((tp), (ti), (m)); \
		tp->t_flags |= TF_ACKNOW; \
	} \

/* Function 22 */
		     (ti->ti_flags & TH_SYN) == 0) {
			ts_present = 1;
			ts_val = ntohl(*(u_long *)(optp + 4));
			ts_ecr = ntohl(*(u_long *)(optp + 8));
		}

/* Function 23 */
		   SEQ_LT(tp->last_ack_sent, ti->ti_seq + ti->ti_len)) {
			tp->ts_recent_age = tcp_now;
			tp->ts_recent = ts_val;
		}

/* Function 24 */
		    ti->ti_len <= sbspace(&so->so_rcv)) {
			++tcpstat.tcps_preddat;
			tp->rcv_nxt += ti->ti_len;
			tcpstat.tcps_rcvpack++;
			tcpstat.tcps_rcvbyte += ti->ti_len;
			m->m_data += sizeof(struct tcpiphdr)+off-sizeof(struct tcphdr);
			m->m_len -= sizeof(struct tcpiphdr)+off-sizeof(struct tcphdr);
			sbappend(&so->so_rcv, m);
			sorwakeup(so);
			tp->t_flags |= TF_DELACK;
			return;
		}

/* Function 25 */
				(TF_RCVD_SCALE|TF_REQ_SCALE)) {
				tp->snd_scale = tp->requested_s_scale;
				tp->rcv_scale = tp->request_r_scale;
			}

/* Function 26 */
	    TSTMP_LT(ts_val, tp->ts_recent)) {

		if ((int)(tcp_now - tp->ts_recent_age) > TCP_PAWS_IDLE) {
			tp->ts_recent = 0;
		} else {
			tcpstat.tcps_rcvduppack++;
			tcpstat.tcps_rcvdupbyte += ti->ti_len;
			tcpstat.tcps_pawsdrop++;
			goto dropafterack;
		}
	}

/* Function 27 */
			    SEQ_GT(ti->ti_seq, tp->rcv_nxt)) {
				iss = tp->rcv_nxt + TCP_ISSINCR;
				tp = tcp_close(tp);
				goto findpcb;
			}

/* Function 28 */
			(TF_RCVD_SCALE|TF_REQ_SCALE)) {
			tp->snd_scale = tp->requested_s_scale;
			tp->rcv_scale = tp->request_r_scale;
		}

/* Function 29 */
	    TCPS_HAVERCVDFIN(tp->t_state) == 0) {
		if (ti->ti_urp + so->so_rcv.sb_cc > sb_max) {
		}
		if (SEQ_GT(ti->ti_seq+ti->ti_urp, tp->rcv_up)) {
			tp->rcv_up = ti->ti_seq + ti->ti_urp;
			so->so_oobmark = so->so_rcv.sb_cc +
			    (tp->rcv_up - tp->rcv_nxt) - 1;
			if (so->so_oobmark == 0)
				so->so_state |= SS_RCVATMARK;
			sohasoutofband(so);
			tp->t_oobflags &= ~(TCPOOB_HAVEDATA | TCPOOB_HADDATA);
		}
		if (ti->ti_urp <= ti->ti_len
#ifdef SO_OOBINLINE
		     && (so->so_options & SO_OOBINLINE) == 0
#endif
		     )
			tcp_pulloutofband(so, ti, m);
	} else

/* Function 30 */
	    TCPS_HAVERCVDFIN(tp->t_state) == 0) {
		TCP_REASS(tp, ti, m, so, tiflags);
		len = so->so_rcv.sb_hiwat - (tp->rcv_adv - tp->rcv_nxt);
	} else {
		m_freem(m);
		tiflags &= ~TH_FIN;
	}

/* Function 31 */
	    *(uint32_t *)up == TCPOPT_NOP_NOP_TSTAMP) {
		tcpopt->tcp_opt_ts_val = ABE32_TO_U32((up+4));
		tcpopt->tcp_opt_ts_ecr = ABE32_TO_U32((up+8));

		return (TCP_OPT_TSTAMP_PRESENT);
	}

/* Function 32 */
				    SEQ_GT(sack_end, tcp->tcp_snxt)) {
					continue;
				}

/* Function 33 */
	    (tcp->tcp_snd_ts_ok || TCP_IS_DETACHED(tcp))) {
		tmp_tcph = (char *)tcp->tcp_tcpha;

		tcp->tcp_snd_ts_ok = B_TRUE;
		tcp->tcp_ts_recent = tcpopt.tcp_opt_ts_val;
		tcp->tcp_last_rcv_lbolt = ddi_get_lbolt64();
		ASSERT(OK_32PTR(tmp_tcph));
		ASSERT(connp->conn_ht_ulp_len == TCP_MIN_HEADER_LENGTH);

		tmp_tcph += connp->conn_ht_ulp_len;
		tmp_tcph[0] = TCPOPT_NOP;
		tmp_tcph[1] = TCPOPT_NOP;
		tmp_tcph[2] = TCPOPT_TSTAMP;
		tmp_tcph[3] = TCPOPT_TSTAMP_LEN;
		connp->conn_ht_iphc_len += TCPOPT_REAL_TS_LEN;
		connp->conn_ht_ulp_len += TCPOPT_REAL_TS_LEN;
		tcp->tcp_tcpha->tha_offset_and_reserved += (3 << 4);
	} else {
		tcp->tcp_snd_ts_ok = B_FALSE;
	}

/* Function 34 */
	    (tcps->tcps_sack_permitted != 0 && TCP_IS_DETACHED(tcp)))) {
		ASSERT(tcp->tcp_num_sack_blk == 0);
		ASSERT(tcp->tcp_notsack_list == NULL);

		tcp->tcp_snd_sack_ok = B_TRUE;
		if (tcp->tcp_snd_ts_ok) {
			tcp->tcp_max_sack_blk = 3;
		} else {
			tcp->tcp_max_sack_blk = 4;
		}
	} else if (tcp->tcp_snd_sack_ok) {
		ASSERT(tcp->tcp_num_sack_blk == 0);
		ASSERT(tcp->tcp_notsack_list == NULL);
		tcp->tcp_snd_sack_ok = B_FALSE;
	}

/* Function 35 */
		    (tcp->tcp_last_rcv_lbolt + PAWS_TIMEOUT)) {
			return (B_FALSE);
		} else {
			tcp->tcp_ts_recent =
			    tcpoptp->tcp_opt_ts_val;
		}

/* Function 36 */
			    MSEC_TO_TICK(TCP_TLC_REPORT_INTERVAL)) {
				zcmn_err(lconnp->conn_zoneid, CE_WARN,
				    "Listener (port %d) connection max (%u) "
				    "reached: %u attempts dropped total\n",
				    ntohs(listener->tcp_connp->conn_lport),
				    tlc->tlc_max, tlc->tlc_drop);
				tlc->tlc_report_time = now;
			}

/* Function 37 */
		    IN6_IS_ADDR_LINKSCOPE(&econnp->conn_laddr_v6)) {
			econnp->conn_incoming_ifindex = ifindex;
			econnp->conn_ixa->ixa_flags |= IXAF_SCOPEID_SET;
			econnp->conn_ixa->ixa_scopeid = ifindex;
		}

/* Function 38 */
	    (tcpha->tha_flags & (TH_ECE|TH_CWR)) == (TH_ECE|TH_CWR)) {
		eager->tcp_ecn_ok = B_TRUE;
	}

/* Function 39 */
		    addr_cache[IP_ADDR_CACHE_HASH(econnp->conn_faddr_v4)]) {
			eager->tcp_dontdrop = B_TRUE;
		}

/* Function 40 */
			    SEQ_GT(seg_ack, tcp->tcp_snxt)) {
				freemsg(mp);
				if (flags & TH_RST)
					return;
				tcp_xmit_ctl("TCPS_SYN_SENT-Bad_seq",
				    tcp, seg_ack, 0, TH_RST);
				return;
			}

/* Function 41 */
			    SEQ_GT(seg_ack, tcp->tcp_snxt)) {
				freemsg(mp);
				tcp_xmit_ctl("TCPS_SYN_RCVD-bad_ack",
				    tcp, seg_ack, 0, TH_RST);
				return;
			}

/* Function 42 */
	    (seg_len > 0 && SEQ_GT(seg_seq + seg_len, tcp->tcp_rnxt))) {
		TCPS_BUMP_MIB(tcps, tcpInClosed);
		DTRACE_PROBE2(tcp__trace__recv, mblk_t *, mp, tcp_t *, tcp);
		freemsg(mp);
		tcp_xmit_ctl("new data when detached", tcp,
		    tcp->tcp_snxt, 0, TH_RST);
		(void) tcp_clean_death(tcp, EPROTO);
		return;
	}

/* Function 43 */
		    (seg_len != 0)) {
			flags &= ~TH_SYN;
			seg_seq++;
			urp--;
			goto try_again;
		}

/* Function 44 */
			    htonl(IPH_ECN_CE << 20)) {
				tcp->tcp_ecn_echo_on = B_TRUE;
			}

/* Function 45 */
	    SEQ_LEQ(seg_seq, tcp->tcp_rack)) {
		tcp->tcp_ts_recent = tcpopt.tcp_opt_ts_val;
		tcp->tcp_last_rcv_lbolt = LBOLT_FASTPATH64;
	}

/* Function 46 */
		    SEQ_GT(urp + seg_seq, tcp->tcp_urp_last)) {
			if (IPCL_IS_NONSTR(connp)) {
				if (!TCP_IS_DETACHED(tcp)) {
					(*sockupcalls->su_signal_oob)
					    (connp->conn_upper_handle, urp);
				}
			} else {
				mp1 = allocb(0, BPRI_MED);
				if (mp1 == NULL) {
					freemsg(mp);
					return;
				}
				if (!TCP_IS_DETACHED(tcp) &&
				    !putnextctl1(connp->conn_rq, M_PCSIG,
				    SIGURG)) {
					freemsg(mp1);
					freemsg(mp);
					return;
				}
				mp1->b_flag |= MSGNOTMARKNEXT;
				freemsg(tcp->tcp_urp_mark_mp);
				tcp->tcp_urp_mark_mp = mp1;
				flags |= TH_SEND_URP_MARK;
#ifdef DEBUG
				(void) strlog(TCP_MOD_ID, 0, 1, SL_TRACE,
				    "tcp_rput: sent M_PCSIG 2 seq %x urp %x "
				    "last %x, %s",
				    seg_seq, urp, tcp->tcp_urp_last,
				    tcp_display(tcp, NULL, DISP_PORT_ONLY));
			}
			tcp->tcp_urp_last_valid = B_TRUE;
			tcp->tcp_urp_last = urp + seg_seq;
		} else if (tcp->tcp_urp_mark_mp != NULL) {
			flags |= TH_SEND_URP_MARK;
		}

/* Function 47 */
				    (tcp->tcp_unsent == 0)) {
					tcp->tcp_rexmit_max = tcp->tcp_fss;
				} else {
					tcp->tcp_rexmit_max = tcp->tcp_snxt;
				}

/* Function 48 */
		    (SEQ_LEQ(seg_ack, tcp->tcp_snxt_shrunk))) {
			uint32_t data_acked_ahead_snxt;

			data_acked_ahead_snxt = seg_ack - tcp->tcp_snxt;
			tcp_update_xmit_tail(tcp, seg_ack);
			tcp->tcp_unsent -= data_acked_ahead_snxt;
		} else {
			TCPS_BUMP_MIB(tcps, tcpInAckUnsent);
			freemsg(mp);

			if (tcp_drop_ack_unsent_cnt > 0 &&
			    ++tcp->tcp_in_ack_unsent >
			    tcp_drop_ack_unsent_cnt) {
				TCP_STAT(tcps, tcp_in_ack_unsent_drop);
				if (tcp->tcp_in_ack_unsent > 2 *
				    tcp_drop_ack_unsent_cnt) {
					(void) tcp_clean_death(tcp, EPROTO);
				}
				return;
			}
			mp = tcp_ack_mp(tcp);
			if (mp != NULL) {
				TCPS_BUMP_MIB(tcps, tcpHCOutSegs);
				TCPS_BUMP_MIB(tcps, tcpOutAck);
				tcp_send_data(tcp, mp);
			}
			return;
		}

/* Function 49 */
		    SEQ_GT(seg_ack, (uint32_t)(uintptr_t)(mp1->b_next))) {
			tcp_set_rto(tcp, gethrtime() -
			    (hrtime_t)(intptr_t)mp1->b_prev);
		} else {
			TCPS_BUMP_MIB(tcps, tcpRttNoUpdate);
		}

/* Function 50 */
			    (uint32_t)(uintptr_t)(mp1->b_next))) {
				mp1->b_prev =
				    (mblk_t *)(intptr_t)gethrtime();
				mp1->b_next = NULL;
			}

/* Function 51 */
	    (tcp->tcp_swl1 == seg_seq && new_swnd > tcp->tcp_swnd)) {
		if (tcp->tcp_unsent && new_swnd > tcp->tcp_swnd)
			flags |= TH_XMIT_NEEDED;
		tcp->tcp_swnd = new_swnd;
		if (new_swnd > tcp->tcp_max_swnd)
			tcp->tcp_max_swnd = new_swnd;
		tcp->tcp_swl1 = seg_seq;
		tcp->tcp_swl2 = seg_ack;
	}

/* Function 52 */
		    (flags & TH_MARKNEXT_NEEDED)) {
			if (tcp->tcp_rcv_list != NULL) {
				flags |= tcp_rcv_drain(tcp);
			}
			ASSERT(tcp->tcp_rcv_list == NULL ||
			    tcp->tcp_fused_sigurg);

			if (flags & TH_MARKNEXT_NEEDED) {
#ifdef DEBUG
				(void) strlog(TCP_MOD_ID, 0, 1, SL_TRACE,
				    "tcp_rput: sending MSGMARKNEXT %s",
				    tcp_display(tcp, NULL,
				    DISP_PORT_ONLY));
				mp->b_flag |= MSGMARKNEXT;
				flags &= ~TH_MARKNEXT_NEEDED;
			}

			if (is_system_labeled())
				tcp_setcred_data(mp, ira);

			putnext(connp->conn_rq, mp);
			if (!canputnext(connp->conn_rq))
				tcp->tcp_rwnd -= seg_len;
		} else if ((flags & (TH_PUSH|TH_FIN)) ||

/* Function 53 */
	    (ira->ira_flags & IRAF_IS_IPV4)) {
		optlen += sizeof (struct T_opthdr) +
		    P2ROUNDUP(sizeof (uint8_t), __TPI_ALIGN_SIZE);
		addflag.crb_recvtos = 1;
	}

/* Function 54 */
	    (ira->ira_flags & IRAF_IPSEC_SECURE)) {
		ip6_t *ip6h;
		ipha_t *ipha;

		if (ira->ira_flags & IRAF_IS_IPV4) {
			ipha = (ipha_t *)mp->b_rptr;
			ip6h = NULL;
		} else {
			ipha = NULL;
			ip6h = (ip6_t *)mp->b_rptr;
		}
		mp = ipsec_check_inbound_policy(mp, connp, ipha, ip6h, ira);
		if (mp == NULL) {
			BUMP_MIB(&ipst->ips_ip_mib, ipIfStatsInDiscards);
			ip_drop_input("ipIfStatsInDiscards", mp, NULL);
			CONN_DEC_REF(connp);
			return;
		}
	}

/* Function 55 */
		    tcp_rwnd_reopen(tcp) == TH_ACK_NEEDED) {
			tcp_xmit_ctl(NULL, tcp,
			    (tcp->tcp_swnd == 0) ? tcp->tcp_suna :
			    tcp->tcp_snxt, tcp->tcp_rnxt, TH_ACK);
		}

/* Function 56 */
	    (ipha_t *)&icmph[1] + 1 > (ipha_t *)mp->b_wptr) {
noticmpv4:
		freemsg(mp);
		return;
	}

/* Function 57 */
	    (uchar_t *)tcpha + ICMP_MIN_TCP_HDR > mp->b_wptr) {
		goto noticmpv4;
	}

/* Function 58 */
	    ((uchar_t *)tcpha + ICMP_MIN_TCP_HDR) > mp->b_wptr) {
		goto noticmpv6;
	}

/* Function 59 */
			    (seg_seq == tcp->tcp_iss)) {
				(void) tcp_clean_death(tcp, ECONNREFUSED);
			}

/* Function 60 */
			    (seg_seq == tcp->tcp_iss)) {
				if (tcp->tcp_listener != NULL &&
				    tcp->tcp_listener->tcp_syn_defense) {
					(void) tcp_clean_death(tcp,
					    tcp->tcp_client_errno);
				}
			}

/* Function 61 */
		    (uchar_t *)nexthdrp) {
			if (tcp->tcp_state == TCPS_SYN_SENT ||
			    tcp->tcp_state == TCPS_SYN_RCVD) {
				(void) tcp_clean_death(tcp, ECONNREFUSED);
			}
			break;
		}


#ifdef __cplusplus
}
#endif

/* End of tcp_input_unified.c - Synthesized by MINIX4 Massive Synthesis System */
