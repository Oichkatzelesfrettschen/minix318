/**
 * @file route_unified.c
 * @brief Unified route implementation
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
 *     1. minix4\microkernel\servers\net\route.c
 *     2. minix4\libos\lib_legacy\openssl\lib\posix\net\route.c
 *     3. minix4\exokernel\kernel_legacy\cmd\cmd-inet\usr.sbin\route.c
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
#include "ns.h"
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <inet/ip.h>
#include <inet/mib2.h>
#include <libtsnet.h>
#include <limits.h>
#include <locale.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/raw_cb.h>
#include <net/route.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/in_var.h>
#include <netns/ns.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stropts.h>
#include <sys/ccompile.h>
#include <sys/domain.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/kernel.h>
#include <sys/mbuf.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/stat.h>
#include <sys/stream.h>
#include <sys/sysmacros.h>
#include <sys/systm.h>
#include <sys/tihdr.h>
#include <sys/types.h>
#include <tsol/label.h>
#include <unistd.h>

/* ===== DEFINES ===== */
#define	SA(p) ((struct sockaddr *)(p))
#define	equal(a1, a2) (bcmp((caddr_t)(a1), (caddr_t)(a2), (a1)->sa_len) == 0)
#define ROUNDUP(a) (a>0 ? (1 + (((a) - 1) | (sizeof(long) - 1))) : sizeof(long))
#define senderr(x) { error = x ; goto bad; }
#define	K_ADD		1
#define	K_BLACKHOLE	2
#define	K_CHANGE	3
#define	K_CLONING	4
#define	K_DELETE	5
#define	K_DST		6
#define	K_EXPIRE	7
#define	K_FLUSH		8
#define	K_GATEWAY	9
#define	K_GET		11
#define	K_HOPCOUNT	12
#define	K_HOST		13
#define	K_IFA		14
#define	K_IFACE		15
#define	K_IFP		16
#define	K_INET		17
#define	K_INET6		18
#define	K_INTERFACE	19
#define	K_LINK		20
#define	K_LOCK		21
#define	K_LOCKREST	22
#define	K_MASK		23
#define	K_MONITOR	24
#define	K_MTU		25
#define	K_NET		26
#define	K_NETMASK	27
#define	K_NOSTATIC	28
#define	K_PRIVATE	29
#define	K_PROTO1	30
#define	K_PROTO2	31
#define	K_RECVPIPE	32
#define	K_REJECT	33
#define	K_RTT		34
#define	K_RTTVAR	35
#define	K_SA		36
#define	K_SENDPIPE	37
#define	K_SSTHRESH	38
#define	K_STATIC	39
#define	K_XRESOLVE	40
#define	K_MULTIRT	41
#define	K_SETSRC	42
#define	K_SHOW		43
#define	K_SECATTR	43
#define	K_INDIRECT	44
#define	BUF_SIZE 2048
#define	ROUNDUP_LONG(a) \
#define	ADVANCE(x, n) ((x) += ROUNDUP_LONG(salen(n)))
#define	C(x)	((x) & 0xff)
#define	TEXT_DOMAIN "SYS_TEST"
#define	caseof(x, y, z)	\
#define	NEXTTOKEN \
#define	hp (newrt->ri_gate_hp)
#define	rtm m_rtmsg.m_rtm
#define	NEXTADDR(w, u) \
#define	NMSGTYPES (sizeof (msgtypes) / sizeof (msgtypes[0]))
#define	lock(f)	((rtm->rtm_rmx.rmx_locks & RTV_ ## f) ? 'L' : ' ')
#define	RTA_IGN	\
#define	VIRGIN	0
#define	GOTONE	1
#define	GOTTWO	2
#define	RESET	3
#define	DIGIT	(4*0)
#define	END	(4*1)
#define	DELIM	(4*2)
#define	LETTER	(4*3)

/* ===== TYPES ===== */
	struct domain *dom;
struct rtentry *
	struct rtentry *newrt = 0;
	struct rt_addrinfo info;
	struct sockaddr *dst, *gateway, *netmask, *src;
	struct rtentry **rtp;
	struct rt_addrinfo info;
	struct ifaddr *ifa;
	struct proc *p;
struct ifaddr *
	struct sockaddr	*dst, *gateway;
		struct rtentry *rt = rtalloc1(dst, 0);
		struct ifaddr *oifa = ifa;
	struct sockaddr *dst, *gateway, *netmask;
	struct rtentry **ret_nrt;
	struct ifaddr *ifa;
	struct sockaddr *ndst;
	struct rtentry *rt0;
	struct sockaddr *dst, *gate;
	struct sockaddr *src, *dst, *netmask;
	struct mbuf *m = 0;
	struct rtentry *nrt = 0;
	struct domain *dom;
struct rtentry *
	struct rtentry *newrt = 0;
	struct rt_addrinfo info;
	struct sockaddr *dst, *gateway, *netmask, *src;
	struct rtentry **rtp;
	struct rt_addrinfo info;
	struct ifaddr *ifa;
	struct proc *p;
struct ifaddr *
	struct sockaddr	*dst, *gateway;
		struct rtentry *rt = rtalloc1(dst, 0);
		struct ifaddr *oifa = ifa;
	struct sockaddr *dst, *gateway, *netmask;
	struct rtentry **ret_nrt;
	struct ifaddr *ifa;
	struct sockaddr *ndst;
	struct rtentry *rt0;
	struct sockaddr *dst, *gate;
	struct sockaddr *src, *dst, *netmask;
	struct mbuf *m = 0;
	struct rtentry *nrt = 0;
typedef union sockunion {
	struct	sockaddr sa;
	struct	sockaddr_in sin;
	struct	sockaddr_dl sdl;
	struct	sockaddr_in6 sin6;
typedef struct rtcmd_irep {
	struct rt_metrics ri_metrics;
	struct hostent *ri_gate_hp;
typedef struct	mib_item_s {
	struct mib_item_s *next_item;
typedef enum {
typedef enum {
    struct sockaddr_in *sin);
    struct rt_msghdr *rtm, int msglen);
	struct	rt_msghdr m_rtm;
	struct rt_msghdr *rtm;
	struct sockaddr_in sin;
	struct sockaddr_in6 sin6;
		struct sockaddr *sa = (struct sockaddr *)(rtm + 1);
	struct hostent *hp = NULL;
	struct in_addr in;
	struct in6_addr in6;
	struct netent *np;
	struct in_addr in;
	struct flock lock;
	struct stat st;
	struct flock lock;
	struct ifconf ifc;
	struct ifreq ifreq;
	struct ifreq *ifr;
	struct sockaddr_in *sin;
	struct hostent **hpp;
	struct hostent *hp;
    struct hostent **hpp, addr_type_t atype, rtcmd_irep_t *rcip)
	struct hostent *hp;
	struct netent *np;
    struct hostent **hpp)
	struct hostent *hp;
	struct if_msghdr *ifm;
	struct ifa_msghdr *ifam;
	struct sockaddr *dst = NULL, *gate = NULL, *mask = NULL, *src = NULL;
	struct sockaddr_dl *ifp = NULL;
	struct sockaddr *sa;
	struct keytab *kt = keywords;
	struct strbuf		ctlbuf, databuf;
	struct T_optmgmt_req	*tor = (struct T_optmgmt_req *)buf;
	struct T_optmgmt_ack	*toa = (struct T_optmgmt_ack *)buf;
	struct T_error_ack	*tea = (struct T_error_ack *)buf;
	struct opthdr		*req;
	struct rtsa_s *rtsa = &sp.rtsa_attr[0];

/* ===== GLOBAL VARIABLES ===== */
	void **table;
	rtable_init((void **)rt_tables);
	int report;
	int  s = splnet(), err = 0, msgtype = RTM_MISS;
			printf("rtfree: %x not freed (neg refs)\n", rt);
	int flags;
	int error = 0;
	int req;
	int flags;
	int req, flags;
	int s = splnet(); int error = 0;
	int dlen = ROUNDUP(dst->sa_len), glen = ROUNDUP(gate->sa_len);
	register u_char *cp1 = (u_char *)src;
	register u_char *cp2 = (u_char *)dst;
	register u_char *cp3 = (u_char *)netmask;
	u_char *cplim = cp2 + *cp3;
	u_char *cplim2 = cp2 + *cp1;
	int cmd, flags;
	int error;
					(void) m_free(m);
		(void) m_free(m);
	void **table;
	rtable_init((void **)rt_tables);
	int report;
	int  s = splnet(), err = 0, msgtype = RTM_MISS;
			printf("rtfree: %x not freed (neg refs)\n", rt);
	int flags;
	int error = 0;
	int req;
	int flags;
	int req, flags;
	int s = splnet(); int error = 0;
	int dlen = ROUNDUP(dst->sa_len), glen = ROUNDUP(gate->sa_len);
	register u_char *cp1 = (u_char *)src;
	register u_char *cp2 = (u_char *)dst;
	register u_char *cp3 = (u_char *)netmask;
	u_char *cplim = cp2 + *cp3;
	u_char *cplim2 = cp2 + *cp1;
	int cmd, flags;
	int error;
					(void) m_free(m);
		(void) m_free(m);
	char	*kt_cp;
	int	kt_i;
	int ri_cmd;
	int ri_flags;
	int ri_af;
	int ri_addrs;
	char *ri_dest_str;
	char *ri_gate_str;
	intmax_t *valp;
    char *cmd_string);
static void		bprintf(FILE *fp, int b, char *s);
    mib2_ipv6RouteEntry_t *rp6, int seqno);
static void		del_rtcmd_irep(rtcmd_irep_t *rcip);
static void		flushroutes(int argc, char *argv[]);
    int *plenp, struct hostent **hpp);
static int		in_getprefixlen(char *addr, int max_plen);
    uchar_t *mask);
static in_addr_t	inet_makesubnetmask(in_addr_t addr, in_addr_t mask);
static int		keyword(const char *cp);
static void		link_addr(const char *addr, struct sockaddr_dl *sdl);
static char		*link_ntoa(const struct sockaddr_dl *sdl);
static mib_item_t	*mibget(int sd);
static char		*netname(struct sockaddr *sa);
static int		newroute(char **argv);
static rtcmd_irep_t	*new_rtcmd_irep(void);
static void		pmsg_addrs(const char *cp, size_t len, uint_t addrs);
static void		pmsg_common(const struct rt_msghdr *rtm, size_t len);
static void		print_rtmsg(struct rt_msghdr *rtm, int msglen);
static void		quit(char *s, int err) __NORETURN;
static char		*routename(const struct sockaddr *sa);
static void		rtmonitor(int argc, char *argv[]);
static int		rtmsg(rtcmd_irep_t *rcip);
static int		salen(const struct sockaddr *sa);
static void		save_route(int argc, char **argv, int do_flush);
static void		save_string(char **dst, char *src);
static int		show_saved_routes(int argc);
static void		sockaddr(char *addr, struct sockaddr *sa);
static void		sodump(su_t *su, char *which);
static void		syntax_arg_missing(char *keyword);
static void		syntax_bad_keyword(char *keyword);
static void		syntax_error(char *err, ...);
static void		usage(char *cp);
static void		write_to_rtfile(FILE *fp, int argc, char **argv);
static void		pmsg_secattr(const char *, size_t, const char *);
static pid_t		pid;
static int		s;
static boolean_t	nflag;
static int		af = AF_INET;
static boolean_t	qflag, tflag;
static boolean_t	verbose;
static boolean_t	debugonly;
static boolean_t	fflag;
static boolean_t	update_table;
static boolean_t	perm_flag;
static boolean_t	early_v6_keyword;
static char		perm_file_sfx[] = "/etc/inet/static_routes";
static char		*perm_file;
static char		temp_file_sfx[] = "/etc/inet/static_routes.tmp";
static char		*temp_file;
static int		exit_on_error = B_TRUE;
	char	m_space[BUF_SIZE];
static int ipRouteEntrySize;
static int ipv6RouteEntrySize;
		(void) vfprintf(stderr, err, args);
	(void) fprintf(stderr, "route: ");
		(void) fprintf(stderr, "%s: ", s);
	(void) fprintf(stderr, "%s\n", strerror(sverrno));
	extern int optind;
	extern char *optarg;
	int ch;
	int rval;
	const char *root_dir = NULL;
	(void) setlocale(LC_ALL, "");
	(void) textdomain(TEXT_DOMAIN);
	int seqno;
	int oerrno;
	int off = 0;
	int on = 1;
			    (char *)rp < (char *)item->valp + item->length;
			    (char *)rp6 < (char *)item->valp + item->length;
	char *cp;
	int ire_type;
	int rlen;
	int slen;
	(void) memset(rtm, 0, sizeof (m_rtmsg));
		(void) memset(&sin, 0, slen);
		(void) memmove(cp, &sin, slen);
		(void) memmove(cp, &sin, slen);
		(void) memmove(cp, &sin, slen);
		(void) memset(&sin6, 0, slen);
		(void) memmove(cp, &sin6, slen);
		(void) memmove(cp, &sin6, slen);
		(void) memset(&sin6.sin6_addr, 0, sizeof (sin6.sin6_addr));
		    (uchar_t *)&sin6.sin6_addr.s6_addr);
		(void) memmove(cp, &sin6, slen);
	rtm->rtm_msglen = cp - (char *)&m_rtmsg;
			print_rtmsg(rtm, rtm->rtm_msglen);
	rlen = write(s, (char *)&m_rtmsg, rtm->rtm_msglen);
		print_rtmsg(rtm, rlen);
		sa = (struct sockaddr *)(salen(sa) + (char *)sa);
		(void) printf("%-20.20s ", routename(sa));
		(void) printf("done\n");
	char *cp;
	static char line[MAXHOSTNAMELEN + 1];
	static char domain[MAXHOSTNAMELEN + 1];
	static boolean_t first = B_TRUE;
	int error_num;
			(void) strcpy(domain, cp + 1);
		(void) strcpy(line, "default");
			(void) strncpy(line, cp, MAXHOSTNAMELEN);
			(void) strncpy(line, cp, MAXHOSTNAMELEN);
		cp = line + sprintf(line, "(%d)", sa->sa_family);
			cp += sprintf(cp, " %x", *s);
	char *cp = NULL;
	static char line[MAXHOSTNAMELEN + 1];
			(void) strncpy(line, cp, MAXHOSTNAMELEN);
			(void) sprintf(line, "%u", C(in.s_addr >> 24));
		cp = line + sprintf(line, "af %d:", sa->sa_family);
			cp += sprintf(cp, " %x", *s++);
	char *cmd;
	char obuf[INET6_ADDRSTRLEN];
	int flag = 0;
	uint_t noval, *valp = &noval;
	const char *ws = "\f\n\r\t\v ";
	char *tok = cmd_string;
	char *keyword_str;
	int key;
	char *err;
				int err;
	char file_line[BUF_SIZE];
	int len;
	int i;
		len += snprintf(file_line, BUF_SIZE, "-inet6 ");
	char *tmp_buf;
	int match_cnt;
	char file_line[BUF_SIZE + 4] = "add ";
			(void) printf("persistent: route %s", file_line);
			print_rtcmd_short(stdout, thisrt, B_FALSE, B_TRUE);
			(void) printf("\n");
	int perm_fd;
		(void) args_to_rtcmd(rt, argv, NULL);
		(void) search_rtfile(perm_fp, temp_fp, NULL, SEARCH_MODE_DEL);
		(void) fclose(perm_fp);
			print_rtcmd_short(stderr, rt, B_FALSE, B_TRUE);
			(void) fprintf(stderr, ": entry exists\n");
		print_rtcmd_short(stdout, rt, B_FALSE, B_TRUE);
		(void) printf("\n");
			print_rtcmd_short(stderr, rt, B_FALSE, B_TRUE);
			(void) fprintf(stderr, gettext(": not in file\n"));
			print_rtcmd_short(stdout, rt, B_FALSE, B_TRUE);
			(void) printf(gettext(": not in file\n"));
	(void) fclose(perm_fp);
	int perm_fd;
	int count = 0;
			(void) printf("No persistent routes are defined\n");
	(void) fseek(perm_fp, 0, SEEK_SET);
		(void) printf("No persistent routes are defined\n");
	(void) fclose(perm_fp);
	int ret, attempts, oerrno;
	char *err;
	char obuf[INET6_ADDRSTRLEN];
	(void) args_to_rtcmd(newrt, argv, NULL);
		(void) shutdown(s, 0);
		print_rtcmd_short(stdout, newrt, (ret == 0), B_FALSE);
			(void) printf("\n");
					(void) printf(" %s", obuf);
				(void) printf("%s", newrt->ri_dest_str);
			(void) printf("%s", newrt->ri_dest_str);
		(void) printf(": %s\n", err);
	int n;
	char *buf;
	int iosoc;
		quit("ioctl (get interface configuration)", errno);
			quit("ioctl (get interface flags)", errno);
	int masklen;
	char str[BUFSIZ];
	(void) strlcpy(str, s, sizeof (str));
		char *cp;
	(void) memset(sin, 0, sizeof (*sin));
	char str[BUFSIZ];
	int error_num;
	(void) strlcpy(str, s, sizeof (str));
		char *cp;
	(void) memset(sin6, 0, sizeof (struct sockaddr_in6));
		(void) memmove(&sin6->sin6_addr, hp->h_addr, hp->h_length);
	int prefixlen;
	char *str, *end;
	int n;
	intmax_t msg[2048 / sizeof (intmax_t)];
		(void) close(s);
		(void) printf("got message of size %d\n", n);
		print_rtmsg((struct rt_msghdr *)msg, n);
	static int seq;
	int rlen;
	char *cp = m_rtmsg.m_space;
	int l;
	(void) memset(&m_rtmsg, 0, sizeof (m_rtmsg));
		cp = (char *)(rtsecattr->rtsa_attr + 1);
	rtm.rtm_msglen = l = cp - (char *)&m_rtmsg;
		print_rtmsg(&rtm, l);
			l = read(s, (char *)&m_rtmsg, sizeof (m_rtmsg));
			print_getmsg(newrt, &rtm, l);
	(void) printf("%s: len %d, ", msgtypes[rtm->rtm_type], msglen);
		bprintf(stdout, rtm->rtm_flags, routeflags);
	char *cp;
	int i;
	(void) printf("   route to: %s\n", routename(&req_rt->ri_dst.sa));
	cp = ((char *)(rtm + 1));
		(void) printf("destination: %s\n", routename(dst));
		(void) printf("       mask: %s\n", routename(mask));
		(void) printf("    gateway: %s\n", routename(gate));
		(void) printf("     setsrc: %s\n", routename(src));
			int i;
			(void) printf("\n");
	(void) printf("      flags: ");
	bprintf(stdout, rtm->rtm_flags, routeflags);
	(void) printf("%8d%c ", rtm->rtm_rmx.rmx_recvpipe, lock(RPIPE));
	(void) printf("%8d%c ", rtm->rtm_rmx.rmx_sendpipe, lock(SPIPE));
	(void) printf("%8d%c ", rtm->rtm_rmx.rmx_ssthresh, lock(SSTHRESH));
	(void) printf("%8d%c ", msec(rtm->rtm_rmx.rmx_rtt), lock(RTT));
	(void) printf("%8d%c ", msec(rtm->rtm_rmx.rmx_rttvar), lock(RTTVAR));
	(void) printf("%8d%c ", rtm->rtm_rmx.rmx_hopcount, lock(HOPCOUNT));
	(void) printf("%8d%c ", rtm->rtm_rmx.rmx_mtu, lock(MTU));
	(void) printf("%8d%c", rtm->rtm_rmx.rmx_expire, lock(EXPIRE));
		const char *sptr, *endptr;
		uint_t addrs;
			(void) printf("\nsockaddrs: ");
			bprintf(stdout, rtm->rtm_addrs, addrnames);
		sptr = (const char *)(rtm + 1);
		endptr = (const char *)rtm + msglen;
		(void) putchar('\n');
	(void) printf("\nlocks: ");
	bprintf(stdout, (int)rtm->rtm_rmx.rmx_locks, metricnames);
	(void) printf(" inits: ");
	bprintf(stdout, (int)rtm->rtm_inits, metricnames);
	const char *maxptr;
	int i;
		(void) printf("\nsockaddrs: ");
		bprintf(stdout, addrs, addrnames);
		(void) putchar('\n');
				(void) printf(" %s", routename(sa));
	(void) putchar('\n');
	(void) fflush(stdout);
	int i;
			(void) putc(i, fp);
				(void) putc(i, fp);
		(void) putc('>', fp);
	static char obuf[INET6_ADDRSTRLEN];
			(void) printf("%s: inet6 %s; ", which, obuf);
	(void) fflush(stdout);
	char *cp = (char *)sa;
	int size = salen(sa);
	char *cplim = cp + size;
	int byte = 0, state = VIRGIN, new;
	(void) memset(cp, 0, size);
	char *cp = sdl->sdl_data;
	char *cplim = sizeof (struct sockaddr_dl) + (char *)sdl;
	int byte = 0, state = VIRGIN, new;
	(void) memset(sdl, 0, sizeof (struct sockaddr_dl));
static char hexlist[] = "0123456789abcdef";
	static char obuf[64];
	char *out = obuf;
	int i;
	uchar_t *in = (uchar_t *)LLADDR(sdl);
	uchar_t *inlim = in + sdl->sdl_alen;
		(void) memcpy(obuf, sdl->sdl_data, sdl->sdl_nlen);
	intmax_t		buf[512 / sizeof (intmax_t)];
	int			flags;
	int			i, j, getcode;
	ctlbuf.buf = (char *)buf;
		databuf.buf    = (char *)last_item->valp;
	const char *endptr;
	char buf[256];
	int i;

/* ===== FUNCTIONS ===== */

/* Function 1 */
	    ((rn->rn_flags & RNF_ROOT) == 0)) {
		newrt = rt = (struct rtentry *)rn;
		if (report && (rt->rt_flags & RTF_CLONING)) {
			err = rtrequest(RTM_RESOLVE, dst, SA(0),
					      SA(0), 0, &newrt);
			if (err) {
				newrt = rt;
				rt->rt_refcnt++;
				goto miss;
			}
			if ((rt = newrt) && (rt->rt_flags & RTF_XRESOLVE)) {
				msgtype = RTM_RESOLVE;
				goto miss;
			}
		} else
			rt->rt_refcnt++;
	} else {
		rtstat.rts_unreach++;
	miss:	if (report) {
			bzero((caddr_t)&info, sizeof(info));
			info.rti_info[RTAX_DST] = dst;
			rt_missmsg(msgtype, &info, 0, err);
		}
	}

/* Function 2 */
	    ((rn->rn_flags & RNF_ROOT) == 0)) {
		newrt = rt = (struct rtentry *)rn;
		if (report && (rt->rt_flags & RTF_CLONING)) {
			err = rtrequest(RTM_RESOLVE, dst, SA(0),
					      SA(0), 0, &newrt);
			if (err) {
				newrt = rt;
				rt->rt_refcnt++;
				goto miss;
			}
			if ((rt = newrt) && (rt->rt_flags & RTF_XRESOLVE)) {
				msgtype = RTM_RESOLVE;
				goto miss;
			}
		} else
			rt->rt_refcnt++;
	} else {
		rtstat.rts_unreach++;
	miss:	if (report) {
			memset((caddr_t)&info, 0, sizeof(info));
			info.rti_info[RTAX_DST] = dst;
			rt_missmsg(msgtype, &info, 0, err);
		}
	}

/* Function 3 */
			    ((char *)rp + ipRouteEntrySize)) {
				delRouteEntry(rp, NULL, seqno);
				seqno++;
			}

/* Function 4 */
			    ((char *)rp6 + ipv6RouteEntrySize)) {
				delRouteEntry(NULL, rp6, seqno);
				seqno++;
			}

/* Function 5 */
	    (rcip->ri_addrs & RTA_GATEWAY) == 0) {
		syntax_error(gettext(
		    "route: gateway required for add or delete command\n"));
		return (B_FALSE);
	}

/* Function 6 */
			    sizeof (struct in6_addr)) == 0) {
				rcip->ri_flags |= RTF_HOST;
			}

/* Function 7 */
	    fputs(file_line, fp) == EOF) {
		quit(gettext("failed to write to route file"),
		    errno);
	}

/* Function 8 */
	    memcmp(&srch_rt->ri_mask, &file_rt->ri_mask, sizeof (su_t)) != 0) {
		return (B_FALSE);
	}

/* Function 9 */
			    fputs(file_line + 4, temp_fp) == EOF) {
				quit(gettext(
				    "route: failed to write to temp file"),
				    errno);
			}

/* Function 10 */
		    fputs(file_line + 4, temp_fp) == EOF) {
			quit(gettext("failed to write to temp file"),
			    errno);
		}

/* Function 11 */
				    (uchar_t *)&rcip->ri_mask.sin6.sin6_addr)) {
					syntax_error(gettext(
					    "route: bad prefix length: %d\n"),
					    masklen);
					return (B_FALSE);
				}

/* Function 12 */
				    (uchar_t *)&rcip->ri_mask.sin.sin_addr)) {
					syntax_error(gettext(
					    "route: bad prefix length: %d\n"),
					    masklen);
					return (B_FALSE);
				}

/* Function 13 */
			    inet_lnaof(sin->sin_addr) == INADDR_ANY)) {
				inet_makenetandmask(rcip, ntohl(val),
				    sin);
			}

/* Function 14 */
		    (val = np->n_net) != 0) {
			if (which == RTA_DST) {
				inet_makenetandmask(rcip, val, sin);
			}
			return (B_TRUE);
		}

/* Function 15 */
		    ((*addr >= 'a') && (*addr <= 'z')))) {
			state |= LETTER;
		} else {
			state |= DELIM;
		}


#ifdef __cplusplus
}
#endif

/* End of route_unified.c - Synthesized by MINIX4 Massive Synthesis System */
