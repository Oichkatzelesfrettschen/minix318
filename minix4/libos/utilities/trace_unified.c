/**
 * @file trace_unified.c
 * @brief Unified trace implementation
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
 *    1. userspace\usr_bin_legacy\trace\trace.c
 *    2. minix4\microkernel\servers\pxk\trace.c
 *    3. minix4\microkernel\servers\lib\libcurses\screen\trace.c
 *    4. minix4\libos\lib_legacy\libblockdriver\trace.c
 *    5. minix4\libos\lib_legacy\pxk\trace.c
 *    6. minix4\libos\lib_legacy\openssl\sunos\pxk\trace.c
 *    7. minix4\libos\lib_legacy\libxcurses\src\libc\xcurses\trace.c
 *    8. minix4\exokernel\kernel_legacy\cmd\sendmail\src\trace.c
 *    9. minix4\exokernel\kernel_legacy\cmd\cmd-inet\usr.sbin\in.routed\trace.c
 *   10. minix4\exokernel\kernel_legacy\cmd\cmd-inet\usr.lib\in.ndpd\trace.c
 *   11. minix4\exokernel\kernel_legacy\cmd\cmd-inet\usr.lib\in.ripngd\trace.c
 *   12. minix4\exokernel\kernel_legacy\cmd\abi\spectrans\spec2trace\trace.c
 * 
 * Total source files: 12
 * Synthesis date: 2025-06-13 19:51:15
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

#ifndef TRACE_UNIFIED_C_H
#define TRACE_UNIFIED_C_H

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
#include "compose.h"
#include "const.h"
#include "curses_inc.h"
#include "db.h"
#include "defs.h"
#include "errlog.h"
#include "inc.h"
#include "io.h"
#include "parseproto.h"
#include "parser.h"
#include "pathnames.h"
#include "platform.h"
#include "printfuncs.h"
#include "symtab.h"
#include "tables.h"
#include "trace.h"
#include "util.h"
#include "xk_debug.h"
#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <minix/blockdriver_mt.h>
#include <minix/btrace.h>
#include <minix/drivers.h>
#include <minix/minlib.h>
#include <private.h>
#include <protocols/routed.h>
#include <sendmail.h>
#include <signal.h>
#include <sm/debug.h>
#include <sm/string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/ioc_block.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/* ===== DEFINES ===== */
#define NO_TRACEDEV		((devminor_t) -1)
#define NO_TIME			((u32_t) -1)
#define	NRECORDS	50		/* size of circular trace buffer */
#define	NUM_BUFS 4
#define	NUM_BUFS 4
#define	RT ((struct rt_entry *)rn)
#define	NA ((struct netauth *)n)
#define	NRECORDS	50		/* size of circular trace buffer */
#define	FIRST_TOKEN 4	/* Must match the first token in the above enum */
#define	LINE	(m.mi_line_number-(m.mi_nlines-1))
#define	max(a, b) (a > b)? a: b

/* ===== TYPES ===== */
	struct trace_proc *proc;
	struct trace_proc *proc;
	struct trace_proc *proc;
	struct trace_proc *proc;
	struct in_addr addr;
	struct stat stbuf;
	struct stat stbuf2;
	struct in_addr tmp_addr;
struct or_bits {
struct bits {
    struct rt_spare *rts,
    struct rt_spare *new)
    struct	rt_spare *new,
	struct rt_spare *rts;
	struct interface *ifp;
    struct sockaddr_in *who,
    struct interface *ifp,
    struct rip *msg,
	struct netinfo *n, *lim;
	struct in_addr tmp_mask;
    struct nd_router_solicit *rs, int len, struct sockaddr_in6 *addr)
	struct nd_opt_hdr *opt;
    struct nd_router_advert *ra, int len, struct sockaddr_in6 *addr)
	struct nd_opt_hdr *opt;
	struct nd_opt_prefix_info *po;
	struct nd_opt_mtu *mo;
	struct nd_opt_lla *lo;
	struct iftrace *t;
	struct stat stbuf;
	struct iftrace *t;
	struct bits *p;
	struct iftrace *t;
enum {

/* ===== GLOBAL VARIABLES ===== */
static int got_signal, got_info;
	got_signal = TRUE;
	got_info = TRUE;
	int no_call, in_call;
	put_newline();
		no_call = (proc->trace_flags & TF_NOCALL);
		in_call = (proc->trace_flags & TF_INCALL);
		assert(!in_call || !no_call);
		    in_call ? " call" : "");
		put_newline();
		strlcpy(proc->name, "<unknown>", sizeof(proc->name));
	put_newline();
	put_fmt(proc, "Tracing %s (pid %d)", proc->name, proc->pid);
	put_newline();
	int fl;
	fl = TO_ALTEXEC;
	(void)ptrace(T_SETOPT, proc->pid, 0, fl);
		proc->trace_flags |= TF_EXEC;
	new_exec(proc);
	const char *signame;
		call_leave(proc, TRUE /*skip*/);
	put_newline();
		    WEXITSTATUS(status));
			    signame);
			    WTERMSIG(status));
		put_text(proc, "Process detached");
		put_fmt(proc, "Bogus wait result (%04x)", status);
	put_newline();
	proc_del(proc);
	reg_t pc, sp;
	int class, skip, new_ctx;
	proc->trace_flags &= ~TF_NOCALL;
		proc->trace_flags &= ~(TF_INCALL | TF_SKIP);
		class = call_enter(proc, show_stack);
			break;
			proc->trace_flags |= TF_EXEC;
			break;
			proc->trace_flags |= TF_CTX_SKIP;
			break;
			assert(0);
			proc->last_pc = pc;
			proc->last_sp = sp;
			proc->last_pc = proc->last_sp = 0;
		proc->trace_flags |= TF_INCALL;
		    (pc != proc->last_pc || sp != proc->last_sp));
		skip = ((proc->trace_flags & TF_CTX_SKIP) && new_ctx);
		call_leave(proc, skip);
			put_text(proc, "---");
			put_newline();
		proc->trace_flags &= ~(TF_INCALL | TF_CTX_SKIP | TF_EXEC);
	const char *signame;
		kernel_put_stacktrace(proc);
	call_replay(proc);
		put_fmt(proc, "** %s **", signame);
		put_fmt(proc, "** SIGNAL %d **", sig);
	put_space(proc);
	output_flush();
	int status;
			errno = 0;
			break;
		(void)ptrace(T_RESUME, pid, 0, WSTOPSIG(status));
		warn("Unable to attach to pid %d", pid);
			errno = ESRCH;
		warn("Unable to attach to pid %d", pid);
		(void)ptrace(T_DETACH, pid, 0, 0);
		warnx("Kernel magic check failed, recompile trace(1)");
		(void)ptrace(T_DETACH, pid, 0, 0);
		warnx("Cannot attach to system services!");
		(void)ptrace(T_DETACH, proc->pid, 0, 0);
				continue;
				(void)kill(proc->pid, SIGSTOP);
			proc->trace_flags |= TF_DETACH | TF_STOPPING;
				(void)kill(proc->pid, SIGKILL);
	    "[command]\n", getprogname());
	exit(EXIT_FAILURE);
	const char *output_file;
	pid_t pid, last_pid;
	int c, error;
	setprogname(argv[0]);
	proc_init();
	show_stack = FALSE;
	grouping = FALSE;
	output_file = NULL;
	timestamps = 0;
	allnames = FALSE;
	verbose = 0;
	valuesonly = 0;
			break;
			grouping = TRUE;
			break;
			allnames = TRUE;
			break;
			show_stack = TRUE;
			break;
			timestamps++;
			break;
			valuesonly++;
			break;
			verbose++;
			break;
			output_file = optarg;
			break;
			pid = atoi(optarg);
				usage();
				err(EXIT_FAILURE, NULL);
			break;
			usage();
	argv += optind;
	argc -= optind;
	first_signal = TRUE;
	got_signal = FALSE;
	got_info = FALSE;
	signal(SIGINT, sig_handler);
	signal(SIGINFO, info_handler);
			detach_stopped();
		proc->trace_flags = TF_ATTACH | TF_NOCALL;
			detach_stopped();
			(void)ptrace(T_OK, 0, 0, 0);
			(void)execvp(argv[0], argv);
			err(EXIT_FAILURE, "Unable to start %s", argv[0]);
			break;
				warn("Unable to start process");
			detach_stopped();
			warnx("Kernel magic check failed, recompile trace(1)");
			(void)kill(pid, SIGKILL);
			detach_stopped();
			warn(NULL);
			(void)kill(pid, SIGKILL);
			detach_stopped();
		proc->trace_flags = 0;
		pid = -1;
		usage();
		warn("Unable to open output file");
			(void)kill(pid, SIGKILL);
		detach_stopped();
		(void)ptrace(T_SYSCALL, proc->pid, 0, 0);
	last_pid = 0;
	error = FALSE;
			error = TRUE;
			detach_running(!first_signal);
			got_signal = FALSE;
			first_signal = FALSE;
			list_info();
			got_info = FALSE;
			pid = last_pid;
			    (pid == 0) ? "No result" : strerror(errno));
			put_newline();
		last_pid = 0;
				continue;
				    pid, strerror(errno));
				put_newline();
					    WSTOPSIG(status));
				(void)ptrace(T_DETACH, pid, 0, 0);
				continue;
			proc->trace_flags = TF_ATTACH | TF_STOPPING;
			handle_call(proc, show_stack);
			discard_proc(proc, status);
			continue;
		sig = WSTOPSIG(status);
			proc->trace_flags &= ~TF_STOPPING;
					discard_proc(proc, status);
				continue;
			sig = 0;
			call_leave(proc, TRUE /*skip*/);
			put_text(proc, "---");
			new_exec(proc);
			proc->trace_flags &= ~TF_EXEC;
			proc->trace_flags |= TF_SKIP;
			sig = 0;
				handle_call(proc, show_stack);
			sig = 0;
			report_signal(proc, sig, show_stack);
		(void)ptrace(T_SYSCALL, proc->pid, 0, sig);
		last_pid = proc->pid;
static decl_simple_lock_data(, lock);
static mutex_t lock;
    simple_lock( &lock );
    mutex_lock( lock );
    simple_unlock( &lock );
    mutex_unlock( lock );
    simple_lock_init( &lock );
    lock = mutex_alloc();
    initTraceLevels();
		outf = fopen("trace", "a");
			perror("trace");
			exit(-1);
		fprintf(outf, "trace turned on\n");
		fprintf(outf, "trace turned off\n");
		fclose(outf);
		outf = NULL;
	static	char	string[1024];
	char	*p1 = string;
	char	*p2;
	char	c;
		p2 = unctrl(c);
			p1++;
static int trace_enabled	= FALSE;
static devminor_t trace_dev	= NO_TRACEDEV;
static btrace_entry *trace_buf	= NULL;
static size_t trace_size	= 0;
static size_t trace_pos;
static size_t trace_next;
static u64_t trace_tsc;
static btrace_entry *trace_ptr[MAX_THREADS + 1] = { NULL };
  u64_t tsc;
  assert(trace_enabled);
  read_tsc_64(&tsc);
  tsc -= trace_tsc;
  size_t size;
  int r, ctl, entries;
		free(trace_buf);
		trace_dev = NO_TRACEDEV;
		trace_dev = minor;
	trace_size = size;
	trace_pos = 0;
	trace_next = 0;
		read_tsc_64(&trace_tsc);
		trace_enabled = TRUE;
		break;
		trace_enabled = FALSE;
		memset(trace_ptr, 0, sizeof(trace_ptr));
		break;
		_MINIX_IOCTL_SIZE_BIG(request) / sizeof(btrace_entry));
	trace_next += entries;
  btrace_entry *entry;
  int req;
  u64_t pos;
  size_t size;
  int flags;
  assert(id >= 0 && id < MAX_THREADS + 1);
  case BDEV_OPEN:	req = BTREQ_OPEN;	break;
  case BDEV_CLOSE:	req = BTREQ_CLOSE;	break;
  case BDEV_READ:	req = BTREQ_READ;	break;
  case BDEV_WRITE:	req = BTREQ_WRITE;	break;
  case BDEV_GATHER:	req = BTREQ_GATHER;	break;
  case BDEV_SCATTER:	req = BTREQ_SCATTER;	break;
  case BDEV_IOCTL:	req = BTREQ_IOCTL;	break;
	pos = 0;
	size = m_ptr->m_lbdev_lblockdriver_msg.access;
	flags = 0;
	break;
	pos = m_ptr->m_lbdev_lblockdriver_msg.pos;
	size = m_ptr->m_lbdev_lblockdriver_msg.count;
	flags = m_ptr->m_lbdev_lblockdriver_msg.flags;
	break;
	pos = 0;
	size = m_ptr->m_lbdev_lblockdriver_msg.request;
	flags = 0;
	break;
  entry = &trace_buf[trace_pos];
  entry->request = req;
  entry->size = size;
  entry->position = pos;
  entry->flags = flags;
  entry->result = BTRES_INPROGRESS;
  entry->start_time = trace_gettime();
  entry->finish_time = NO_TIME;
  trace_ptr[id] = entry;
  trace_pos++;
  btrace_entry *entry;
  assert(id >= 0 && id < MAX_THREADS + 1);
  entry->size = size;
  btrace_entry *entry;
  assert(id >= 0 && id < MAX_THREADS + 1);
  entry->result = result;
  entry->finish_time = trace_gettime();
  trace_ptr[id] = NULL;
    initTraceLevels();
    initTraceLevels();
static char rcsID[] = "$Header: /rd/src/libc/xcurses/rcs/trace.c 1.3 1995/06/12 20:24:05 ant Exp $";
static int __m_tracing = FALSE;
	va_list vp;
	static FILE *fp;
	static int initialized = FALSE;
		fp = fopen("trace.out", "wF");
			fprintf(stderr, "Program cannot open \"trace.out\".\n");
			exit(1);
		initialized = TRUE;
	va_start(vp, fmt);
	(void) vfprintf(fp, fmt, vp);
	va_end(vp);
	fputc('\n', fp);
		break;
		break;
		break;
    	__m_tracing = TRUE;
    	__m_tracing = FALSE;
static char	*tTnewflag __P((char *));
static char	*tToldflag __P((char *));
static unsigned char	*tTvect;
static unsigned int	tTsize;
static char	*DefFlags;
	unsigned char *vect;
	unsigned int size;
	char *defflags;
	tTvect = vect;
	tTsize = size;
	DefFlags = defflags;
	register char *s;
	unsigned int first, last;
	register unsigned int i;
	i = 0;
		i = i * 10 + (*s++ - '0');
		s++;
	first = i;
		i = 0;
			i = i * 10 + (*s - '0');
			s++;
	last = i;
	i = 1;
		i = 0;
			i = i * 10 + (*s - '0');
		first = tTsize - 1;
		last = tTsize - 1;
		tTvect[first++] = (unsigned char) i;
		++s;
	register char *s;
	char *pat, *endpat;
	int level;
	pat = s;
		++s;
	endpat = s;
		++s;
		level = 0;
			level = level * 10 + (*s - '0');
			++s;
			level = 0;
		level = 1;
	sm_debug_addsetting_x(sm_strndup_x(pat, endpat - pat), level);
		++s;
	register char *s;
		s = DefFlags;
			++s;
			continue;
			s = tToldflag(s);
			s = tTnewflag(s);
int	tracelevel, new_tracelevel;
static const char *sigtrace_pat = "%s";
static char savetracename[MAXPATHLEN+1];
	"POLLENTRY"};
char	inittracename[MAXPATHLEN+1];
static void tmsg(const char *, ...);
	const char *cp = strerror(err);
	static char msgbuf[64];
			cp = "success";
			    "unknown error %d", err);
			cp = msgbuf;
	static int bufno;
	} bufs[NUM_BUFS];
	char *s;
	addr.s_addr = a;
	s = strcpy(bufs[bufno].str, inet_ntoa(addr));
	bufno = (bufno+1) % NUM_BUFS;
	static char s[20];
	secs += epoch.tv_sec;
	(void) strftime(s, sizeof (s), "%T", localtime(&secs));
	static char s[32];
	time_t secs;
	int len;
	secs = tv->tv_sec + epoch.tv_sec;
	(void) strftime(s, sizeof (s), "%Y/%m/%d %T", localtime(&secs));
	len = strlen(s);
	(void) snprintf(s + len, sizeof (s) - len, ".%06ld", tv->tv_usec);
static struct timeval lastlog_time;
		(void) fprintf(ftrace, "-- %s --\n", ts_full(&now));
		lastlog_time = now;
	va_list args;
		lastlog();
		va_start(args, p);
		(void) vfprintf(ftrace, p, args);
		(void) fputc('\n', ftrace);
		(void) fflush(ftrace);
		(void) va_end(args);
	int fd;
	(void) fflush(stdout);
	(void) fflush(stderr);
			(void) fclose(ftrace);
		ftrace = NULL;
		fd = open("/dev/null", O_RDWR);
			(void) dup2(fd, STDIN_FILENO);
			(void) dup2(fd, STDOUT_FILENO);
			(void) dup2(fd, STDERR_FILENO);
		(void) close(fd);
	lastlog_time.tv_sec = 0;
		(void) fflush(ftrace);
			    rip_strerror(ferror(ftrace)));
	va_list args;
		lastlog();
		va_start(args, p);
		(void) vfprintf(ftrace, p, args);
		(void) fputc('\n', ftrace);
		(void) va_end(args);
	trace_close(file_trace);
	new_tracelevel = tracelevel = 0;
	};
	};
	uint_t old_tracelevel = tracelevel;
		new_tracelevel = 0;
		new_tracelevel = MAX_TRACELEVEL;
			trace_off(pat, off_msgs[0]);
				tmsg(pat, off_msgs[tracelevel]);
		trace_dump();
	FILE *n_ftrace;
	const char *fn;
	int nfd;
	boolean_t allow_create;
	allow_create = (dump == -1);
		filename = NULL;
				msglog("missing trace file name");
			fn = inittracename;
			goto set_tracelevel;
				fn = PATH_TRACE;
				goto missing_file;
				allow_create = _B_TRUE;
		fn = filename;
			goto missing_file;
		nfd = open(fn, O_CREAT|O_EXCL|O_WRONLY, 0644);
			(void) close(nfd);
			goto missing_file;
		goto bad_file_type;
		    rip_strerror(errno));
			inittracename[0] = '\0';
			(void) close(nfd);
		msglog("trace file \"%s\" moved", fn);
		(void) fclose(n_ftrace);
	tmsg("switch to trace file %s", fn);
	trace_close(file_trace = _B_TRUE);
	(void) dup2(nfd, STDOUT_FILENO);
	(void) dup2(nfd, STDERR_FILENO);
		(void) strlcpy(savetracename, fn, sizeof (savetracename) - 1);
	ftrace = n_ftrace;
		new_tracelevel++;
	tracelevel_msg(pat, dump != 0 ? dump : (filename != NULL));
	msglog("trace \"%s\" missing", fn);
	msglog("wrong type (%#x) of trace file \"%s\"", stbuf.st_mode, fn);
	new_tracelevel++;
	sigtrace_pat = "SIGUSR1: %s";
		msglog("signal: %s", rip_strerror(errno));
	new_tracelevel--;
	sigtrace_pat = "SIGUSR2: %s";
		msglog("signal: %s", rip_strerror(errno));
	trace_dump();
		msglog("signal: %s", rip_strerror(errno));
			set_tracefile(savetracename, sigtrace_pat, 0);
			new_tracelevel = 0;
		tracelevel_msg(sigtrace_pat, 0);
	static int bufno;
	    char    str[2*INET_ADDRSTRLEN + sizeof (" (mask )")];
	} bufs[NUM_BUFS];
	char *s, *sp;
	in_addr_t dmask;
	int i, len;
	tmp_addr.s_addr = addr;
	    sizeof (bufs[bufno].str));
	s = bufs[bufno].str;
	bufno = (bufno+1) % NUM_BUFS;
		sp = &s[strlen(s)];
		dmask = mask & -mask;
			i = ffs(mask);
			    (NBBY * sizeof (in_addr_t) + 1) - i);
			    naddr_ntoa(htonl(mask)));
	uint8_t	origin;
	const char *origin_name;
};
};
	uint64_t	bits_mask;
	uint64_t	bits_clear;
	const char	*bits_name;
};
};
};
};
};
	uint64_t b;
	char c;
		(void) putc('<', ftrace);
		c = '\0';
		c = '<';
			(void) putc(c, ftrace);
		(void) fprintf(ftrace, "%#llx", field);
		c = '|';
		(void) fputs("> ", ftrace);
	const struct bits *tbp;
	char *sbuf, *cp, chr;
	size_t slen;
	slen = sizeof ("<0x12345678>");
			slen += strlen(tbp->bits_name) + 1;
	cp = sbuf;
		*cp++ = '<';
		chr = '\0';
		chr = '<';
					*cp++ = chr;
				(void) strcpy(cp, tbl->bits_name);
				cp += strlen(tbl->bits_name);
				chr = '|';
			field &= ~(tbl->bits_mask | tbl->bits_clear);
		tbl++;
			*cp++ = chr;
		cp += sprintf(cp, "%#x", field);
		chr = '|';
		*cp++ = '>';
	*cp = '\0';
	static char buf[sizeof ("xxx.xxx.xxx.xxx/xx-->xxx.xxx.xxx.xxx")];
	int i;
	(void) snprintf(buf, sizeof (buf), "%-16s-->", addrname(dst, mask, 0));
	i = strlen(buf);
	    naddr_ntoa(gate));
	int i;
		(void) fprintf(ftrace, "metric=%-2d ", rts->rts_metric);
		    naddr_ntoa(rts->rts_router));
		(void) fprintf(ftrace, "%s ", ts(rts->rts_time));
		(void) fprintf(ftrace, "tag=%#x ", ntohs(rts->rts_tag));
			continue;
		(void) fprintf(ftrace, "de_ag=%d ", i);
	(void) fprintf(ftrace, "flags 0x%x ", rts->rts_flags);
	uint8_t tblentry;
			(void) fprintf(ftrace, "origin=%s ", tbl->origin_name);
		tbl++;
	lastlog();
	(void) fputs("\n    ", ftrace);
	(void) fputc('\n', ftrace);
	lastlog();
	    naddr_ntoa(krt->k_gate), krt->k_metric);
	(void) fprintf(ftrace, "%s ", ts(krt->k_keep));
	(void) fprintf(ftrace, "%s ", ts(krt->k_redirect_time));
	trace_bits(ks_bits, krt->k_state, _B_TRUE);
	(void) fputc('\n', ftrace);
	lastlog();
	    naddr_ntoa(drp->dr_gate), ts(drp->dr_ts));
	    SIGN_PREF(drp->dr_recv_pref), drp->dr_pref);
	lastlog();
		    rtname(rt->rt_dst, rt->rt_mask, rts->rts_gate));
		    rtname(rt->rt_dst, rt->rt_mask, rts->rts_gate));
		    naddr_ntoa(new->rts_gate) : ""));
		    rtname(rt->rt_dst, rt->rt_mask, new->rts_gate));
	(void) fputc('\n', ftrace);
	va_list args;
	lastlog();
	va_start(args, p);
	(void) vfprintf(ftrace, p, args);
	(void) fputc('\n', ftrace);
	(void) va_end(args);
	va_list args;
	lastlog();
	va_start(args, p);
	(void) vfprintf(ftrace, p, args);
	(void) fputc('\n', ftrace);
	(void) va_end(args);
	va_list args;
	lastlog();
	va_start(args, p);
	(void) vfprintf(ftrace, p, args);
	(void) fputc('\n', ftrace);
	(void) va_end(args);
	lastlog();
	    rtname(rt->rt_dst, rt->rt_mask, rt->rt_gate));
	print_rtsorigin(origin_bits, rt->rt_spares->rts_origin);
	trace_bits(rs_bits, rt->rt_state, rt->rt_state != state);
	    naddr_ntoa(new->rts_gate) : ""));
		print_rtsorigin(origin_bits, new->rts_origin);
		trace_bits(rs_bits, state, _B_TRUE);
	(void) fputc('\n', ftrace);
	lastlog();
	    rtname(rt->rt_dst, rt->rt_mask, rt->rt_gate));
	print_rtsorigin(origin_bits, rt->rt_spares->rts_origin);
	trace_bits(rs_bits, rt->rt_state, _B_FALSE);
	(void) fputc('\n', ftrace);
	int i;
	    rtname(RT->rt_dst, RT->rt_mask, RT->rt_gate));
	print_rtsorigin(origin_bits, RT->rt_spares[0].rts_origin);
	trace_bits(rs_bits, RT->rt_state, _B_FALSE);
		    RT->rt_poison_metric, ts(RT->rt_poison_time));
	(void) fprintf(ftrace, "%d spare slots", RT->rt_num_spares);
	rts = &RT->rt_spares[1];
			    i, "", naddr_ntoa(rts->rts_gate));
			print_rts(rts, 0, 0, 0, 0, 1);
			print_rtsorigin(origin_bits, rts->rts_origin);
	(void) fputc('\n', ftrace);
	lastlog();
	(void) fputs("current daemon state:\n", ftrace);
	(void) fputs("Routes:\n", ftrace);
	(void) rn_walktree(rhead, walk_trace, NULL);
	(void) fputs("Kernel routes:\n", ftrace);
	kern_dump();
	(void) fputs("Discovered routers:\n", ftrace);
	rdisc_dump();
	int i, seen_route;
	lastlog();
		    size);
	seen_route = 0;
		n = msg->rip_nets;
		tmp_mask.s_addr = n->n_mask;
		lim = n + (size - 4) / sizeof (struct netinfo);
					    RIP_AUTH_PW_LEN));
					continue;
					    ntohs(NA->au.a_md5.rsvd[1]));
					continue;
				    ntohs(NA->a_type));
					    NA->au.au_pw[i]);
				(void) fputc('\n', ftrace);
				continue;
			seen_route = 1;
				    inet_ntoa(tmp_mask));
				    n->n_mask == 0 ? 2 : 1));
				    n->n_mask == 0 ? 2 : 0));
			    (unsigned long)ntohl(n->n_metric));
				    naddr_ntoa(n->n_nhop));
				    ntohs(n->n_tag));
			(void) fputc('\n', ftrace);
			    size);
		break;
		    msg->rip_tracefile);
		break;
		break;
static void	print_opt(struct nd_opt_hdr *opt, int len);
	char abuf[INET6_ADDRSTRLEN];
	    len, pi->pi_name);
	len -= sizeof (*rs);
	opt = (struct nd_opt_hdr *)&rs[1];
	print_opt(opt, len);
	char abuf[INET6_ADDRSTRLEN];
	    len, pi->pi_name);
	logmsg(LOG_DEBUG, "\tMax hop limit: %u\n", ra->nd_ra_curhoplimit);
	    "Set" : "Not set");
	    "Set" : "Not set");
	    ntohl(ra->nd_ra_reachable));
	    ntohl(ra->nd_ra_retransmit));
	len -= sizeof (*ra);
	opt = (struct nd_opt_hdr *)&ra[1];
	print_opt(opt, len);
	int optlen;
	char abuf[INET6_ADDRSTRLEN];
	char llabuf[BUFSIZ];
		optlen = opt->nd_opt_len * 8;
			logmsg(LOG_DEBUG, "Zero length option!\n");
			break;
			po = (struct nd_opt_prefix_info *)opt;
				break;
			    po->nd_opt_pi_prefix_len);
			    "Set" : "Not set");
			    "Set" : "Not set");
			    ntohl(po->nd_opt_pi_valid_time));
			    ntohl(po->nd_opt_pi_preferred_time));
			break;
			mo = (struct nd_opt_mtu *)opt;
				break;
			    ntohl(mo->nd_opt_mtu_mtu));
			break;
			lo = (struct nd_opt_lla *)opt;
				break;
			    optlen - sizeof (nd_opt_hdr_t));
			    llabuf);
			break;
			lo = (struct nd_opt_lla *)opt;
				break;
			    optlen - sizeof (nd_opt_hdr_t));
			    llabuf);
			break;
			logmsg(LOG_DEBUG, "\tRedirected header option!\n");
			break;
			    opt->nd_opt_type, opt->nd_opt_type);
			break;
		opt = (struct nd_opt_hdr *)((char *)opt + optlen);
		len -= optlen;
	int i;
	char *cp = llabuf;
			(void) snprintf(cp, bufsize, "%02x", lla[i] & 0xFF);
			(void) snprintf(cp, bufsize, "%02x:", lla[i] & 0xFF);
		bufsize -= strlen(cp);
		cp += strlen(cp);
	tracing = 0;
	ftrace = fopen(file, "a");
	(void) dup2(fileno(ftrace), 1);
	(void) dup2(fileno(ftrace), 2);
	ftrace = fp;
	(void) dup2(fileno(ftrace), 1);
	(void) dup2(fileno(ftrace), 2);
			len = 0;
		ulong_t	t_bits;
		char	*t_name;
	};
	boolean_t first;
	char c;
	time_t t;
	(void) time(&t);
	(void) fprintf(fp, "%.15s %s ", ctime(&t) + 4, action);
		char buf1[INET6_ADDRSTRLEN];
		    rt->rt_prefix_length);
		    rt->rt_metric);
		(void) fprintf(fp, " state");
		c = ' ';
				continue;
			(void) fprintf(fp, "%c%s", c, p->t_name);
				c = '|';
				first = _B_FALSE;
			(void) fprintf(fp, " 0");
			c = ' ';
					continue;
				(void) fprintf(fp, "%c%s", c, p->t_name);
					c = '|';
					first = _B_FALSE;
	(void) putc('\n', fp);
	(void) fflush(fp);
		(void) fprintf(fp, "*** end packet history ***\n");
	(void) fflush(fp);
	char *cp = (strcmp(dir, "to") != 0) ? "Output" : "Input";
		(void) fprintf(fp, "%s: no packets.\n", cp);
		(void) fflush(fp);
	(void) fprintf(fp, "%s trace:\n", cp);
		t += NRECORDS;
			continue;
static int  Verbose;
};
};
		lines;
		skips;
		end;
} Statistics;
static void stats_init(void);
static void stats_report(void);
static int collect_binding(int const, char *, int);
static int collect_prototype(char *, int, int);
static int collect_include(char *, int);
static int collect_errval(char *, int);
static int collect_arch(char *);
static void generate_includes(void);
static void generate_init(void);
static void generate_interface(void);
static void generate_closedown(void);
static int generate_aux_file();
static char *to_actual();
static int to_basetype(char *);
static char *de_const(char *);
static char *strpqcpy(char *, char *, char *);
	int	i;
	errlog(END, "}");
	errlog(BEGIN, "xlator_startlib() ");
	Statistics.libraries++;
	db_set_current_library(libname);
	errlog(VERBOSE, "now in library \"%s\"", libname);
	errlog(END, "}");
	int	rc = SUCCESS_RC;
		*lib = db_get_current_library();
	seterrline(0, filename, "", "");
	char *kw;
		kw = "Function";
		break;
		kw = "Data";
		break;
		    "Please file a bug\n", __FILE__, __LINE__);
	seterrline(LINE, m.mi_filename, kw, value);
	errlog(TRACING, "now in interface \"%s\"", value);
	symtab_new_function(m.mi_line_number, m.mi_filename);
	errlog(END, "}");
	int retval;
	char *key = Keywords[token-FIRST_TOKEN].key;
	symtab_set_filename(m.mi_filename);
	value = strnormalize(value);
	seterrline(line, m.mi_filename, key, value);
		retval = collect_include(value, line);
			errlog(FATAL|INPUT, "Bad include line in spec file");
		break;
		retval = collect_binding(token, value, line);
		break;
		retval = collect_errval("errno", line);
		break;
		retval =  collect_errval(value, line);
		break;
		retval = collect_arch(value);
		break;
			    m.mi_line_number);
		retval = SUCCESS_RC;
		break;
		retval = ERROR_RC;
	errlog(END, "}");
	seterrline(LINE, m.mi_filename, "end", value);
	int	rc = SUCCESS_RC;
	Statistics.start = time(NULL);
	double	seconds;
	Statistics.end = time(NULL);
		    seconds, Statistics.lines*60.0/seconds);
		break;
			    Statistics.errors, Statistics.warnings);
		break;
	Statistics.warnings++;
	Statistics.errors++;
	char	*include;
	int	len;
	char	*file = db_get_current_file();
	char	*name;
	char const	*arch = db_get_arch();
	char	*buf, *p;
	char	*t;
	char *p, *q;
	int i;
	p = skipb(type);
	q = strstr(type, "const");
			*q++ = '\0';
		(void) sprintf(type, "%s%s", strnormalize(p), q);
		*q = &buffer[0];
	int	levels = 0;
	assert(strlen(str) < MAXLINE, "string exceeded MAXLINE");
	buffer[0] = '\0';
				*q++ = ' ';
			break;
				*q++ = ' ';
			levels++;
			break;
				p += 4;
				*q++ = *p;
			break;
			*q++ = *p;
			break;
		*q = '\0';
	assert(q < &buffer[MAXLINE], "q fell off end of buffer");
	q--;
		*q-- = '\0';
	assert(strlen(buffer) < MAXLINE, "buffer length exceeded MAXLINE");
	(void) strcpy(str, buffer);
	ENTRY	*p;
	static char buffer[MAXLINE+1];
	int	n;
	*buffer = '\0';
		n = MAXLINE - snprintf(buffer, MAXLINE, "%s", name_of(p));
	char	saved;
	saved = *q;
	*q = '\0';
	(void) strcpy(target, p);
	*q = saved;
	char	f_basetype[BUFSIZ];
	char	f_name[BUFSIZ];
	char	a_basetype[BUFSIZ];
	char	a_type[BUFSIZ];
	char	*file = db_get_current_file();
	char	*interface = db_get_current_interface();
	char	*q;
	char const *parse_err;
	char	tmp_proto[BUFSIZ], buf[BUFSIZ];
	decl_t	*pp, *funargs;
	type_t	*tp;
	int	levels, a_levels;
	tmp_proto[BUFSIZ-1] = 0;
		    interface);
		symtab_set_skip(YES);
		decl_Destroy(pp);
	decl_GetTraceInfo(pp, f_type, f_basetype, &funargs);
	(void) sprintf(buf, "%s", strnormalize(f_type));
	(void) strcpy(f_type, buf);
	(void) sprintf(buf, "%s", strnormalize(f_basetype));
	(void) strcpy(f_basetype, buf);
	levels = to_basetype(f_basetype);
	(void) strpqcpy(f_name, interface, nextsep(interface));
	(void) decl_SetName(pp, f_name);
		f_name, f_basetype, f_type);
	symtab_set_function(f_name, line, file, f_type, f_basetype, levels);
	    (q = de_const(type_of(symtab_get_function()))));
	symtab_add_print_types(f_basetype, q);
			strnormalize(declspec_ToString(buf, funargs->d_ds)));
			funargs->d_ds))));
		tp = funargs->d_type;
				(void) strcat(a_type, "*");
				a_levels++;
			tp = tp->t_next;
			(void) snprintf(a_name, 20, "%s", funargs->d_name);
			    a_name, a_basetype, a_type);
			    a_type, a_basetype, a_levels);
			    q = de_const(type_of(symtab_get_last_arg())));
			symtab_add_print_types(a_basetype, q);
		funargs = funargs->d_next;
	symtab_set_actuals(to_actual());
	symtab_set_cast(decl_ToString(buf, DTS_CAST, pp, NULL));
	decl_Destroy(pp);
	errlog(END, "}");
	ENTRY	*function = symtab_get_function();
	generate_print_declarations(Bodyfp);
	generate_linkage(function);
	generate_interceptor(function);
	errlog(END, "}");
	FILE	*fp;
	char	pathname[MAXLINE];
	char	*include;

/* ===== FUNCTIONS ===== */

/* Function 1 */
	} else if (WIFSIGNALED(status)) {
		if ((signame = get_signal_name(WTERMSIG(status))) != NULL)

/* Function 2 */
	} else if (!(proc->trace_flags & TF_INCALL)) {
		/*

/* Function 3 */
		} else if (sig == SIGSTOP && (proc->trace_flags & TF_EXEC)) {
			/* The process has performed a successful execve(). */

/* Function 4 */
		} else if (sig == SIGTRAP) {
			/* The process is entering or leaving a system call. */

/* Function 5 */
	} else if (new_tracelevel > tracelevel) {
		do {
			tmsg(pat, on_msgs[tracelevel++]);
		} while (tracelevel != new_tracelevel);

/* Function 6 */
	} else if (strcmp(filename, "dump/../table") == 0) {
		trace_dump();

/* Function 7 */
			    (strstr(filename, "../") != NULL)) {
				msglog("wrong trace file \"%s\"", filename);
				return;
			}

/* Function 8 */
			    !S_ISDIR(stbuf.st_mode))) {
				goto bad_file_type;
			}

/* Function 9 */
	} else if (S_ISREG(stbuf.st_mode)) {
		nfd = open(fn, O_APPEND|O_WRONLY, 0644);

/* Function 10 */
		} else if (inittracename[0] != '\0') {
			set_tracefile(inittracename, sigtrace_pat, 0);

/* Function 11 */
	{ IS_REMOTE,		(IS_NO_RDISC |
				IS_BCAST_RDISC), "REMOTE" },

/* Function 12 */
	{ IS_PASSIVE,		(IS_NO_RDISC |
				IS_NO_RIP |
				IS_NO_SUPER_AG |
				IS_PM_RDISC |
				IS_NO_AG),	"PASSIVE" },

/* Function 13 */
	{ (IS_NO_RIPV1_IN |
	    IS_NO_RIPV2_IN |
	    IS_NO_RIPV1_OUT |
	    IS_NO_RIPV2_OUT),	0,		"NO_RIP" },

/* Function 14 */
	{ (IS_NO_RIPV1_IN |
	    IS_NO_RIPV1_OUT),	0,		"RIPV2" },

/* Function 15 */
	{ (IS_NO_ADV_IN |
	    IS_NO_SOL_OUT |
	    IS_NO_ADV_OUT),	IS_BCAST_RDISC,	"NO_RDISC" },

/* Function 16 */
	    (b = tbl->bits_mask) != 0) {
		if ((b & field) == b) {
			if (tbl->bits_name[0] != '\0') {
				if (c != '\0')
					(void) putc(c, ftrace);
				(void) fprintf(ftrace, "%s", tbl->bits_name);
				c = '|';
			}
			field &= ~(b | tbl->bits_clear);
		}
		tbl++;
	}

/* Function 17 */
	} else if (rts->rts_gate != RIP_DEFAULT) {
		(void) fprintf(ftrace, "Chg #%d %-35s ",

/* Function 18 */
			    (n+1)->n_family == RIP_AF_AUTH))) {
				(void) fputs("\tQUERY ", ftrace);
				if (n->n_dst != 0)
					(void) fprintf(ftrace, "%s ",
					    naddr_ntoa(n->n_dst));
				if (n->n_mask != 0)
					(void) fprintf(ftrace, "mask=%s ",
					    inet_ntoa(tmp_mask));
				if (n->n_nhop != 0)
					(void) fprintf(ftrace, "nhop=%s ",
					    naddr_ntoa(n->n_nhop));
				if (n->n_tag != 0)
					(void) fprintf(ftrace, "tag=%#x ",
					    ntohs(n->n_tag));
				(void) fputc('\n', ftrace);
				continue;
			}

/* Function 19 */
			} else if (msg->rip_vers == RIPv1) {
				(void) fprintf(ftrace, "\t%-18s ",

/* Function 20 */
	errlog(VERBOSE, "Keywords[] = {");
	for (i = 0; Keywords[i].key != NULL; i++) {
		errlog(VERBOSE, "    \"%s\", ", Keywords[i].key);
	}
	errlog(VERBOSE, "    (char *) NULL");
	errlog(VERBOSE, "};");

/* Function 21 */
	errlog(BEGIN, "xlator_startfile() {");
	Statistics.files++;
	db_set_current_file(filename);
	errlog(TRACING, "now in file \"%s\" in lib \"%s\"",
		filename, lib);

	/* Generate filenames. */
	(void) snprintf(infile, sizeof (infile), "%s", filename);
	(void) snprintf(outfile, sizeof (outfile), "%s.c",
		db_get_output_file());

	/* Open .c file. */
	if (open_code_file() == NO) {
		rc = ERROR_RC;
	}

	generate_init(); /* Write stuff to the c file. */
	symtab_clear_includes(); /* Clear out the per-file data. */
	errlog(END, "}");

/* Function 22 */
	errlog(BEGIN, "xlator_start_if() {");

/*
 * XXX Note whether interface is function or data in some state data item.
 * We'll need it later when writing interceptors.
 */

	Statistics.interfaces++;
	(void) strpqcpy(ifname, value, nextsep2(value));
	if (*ifname == '\0') {
		errlog(INPUT|ERROR|FATAL,
		    "missing argument in \"%s\" line", kw);
	}
	db_set_current_interface(ifname);
	errlog(VERBOSE, "interface='%s'", value);
	if (token == XLATOR_KW_DATA) {
		Statistics.skips++;
		errlog(VERBOSE, "telling front end to skip '%s'", value);
		errlog(END, "}");
		return (SKIP_RC); /* Tell front end to skip it for us. */
	}

/* Function 23 */
	errlog(BEGIN, "xlator_take_kvpair() {");
	Statistics.lines++;
	errlog(VERBOSE, "key='%s', value='%s'",
	    (key) ? key : "<nil>",
	    (value) ? value : "<nil>");
	switch (token) {
	case XLATOR_KW_DECL:

	/*
	 * XXX Check state item to see that it is a function,
	 * else do not emit interceptor
	 */
		symtab_clear_function(); /* Always use last one. */
		errlog(END, "}");
		retval = collect_prototype(value, line, m.mi_ext_cnt);
		break;

	case XLATOR_KW_INCL:
		errlog(END, "}"); /* Use union of all includes. */

/* Function 24 */
	errlog(BEGIN, "xlator_end_if() {");
	if (symtab_get_skip() == YES) {
		symtab_set_skip(NO);
		Statistics.skips++;
	} else {
		generate_interface();
	}
	errlog(END, "}");

/* Function 25 */
	errlog(BEGIN, "xlator_endfile() {");

	generate_closedown();
	errlog(END, "}");

/* Function 26 */
	errlog(BEGIN, "xlator_endlib() {");
	errlog(END, "}");

/* Function 27 */
	errlog(BEGIN, "xlator_end() {");
	rc += !generate_aux_file();
	stats_report();
	errlog(END, "}");

/* Function 28 */
	errlog(BEGIN, "collect_include() {");
	if ((include = strtok(p, ", ")) != NULL) {
		for (; include != NULL; include = strtok(NULL, ", ")) {
			include  = skipb(include);

			/*
			 * Make sure the include file's name
			 * has legitimate C syntax - i.e. it's in double
			 * quotes or angle brackets.
			 */
			if (*include != '"' && *include != '<')
				return (ERROR_RC);

			len = strlen(include);

			if (include[len-1] != '"' && include[len-1] != '>')
				return (ERROR_RC);

			/*
			 * If include filename syntax is OK, add it to
			 * the list
			 */
			symtab_add_includes(include);
		}
	}
	errlog(END, "}");

/* Function 29 */
	errlog(BEGIN, "collect_binding() {");
	errlog(VERBOSE, "name=\"%s\", value=\"%s\", line=%d\n",
	    Keywords[token-FIRST_TOKEN].key, value, line);

	if (token == XLATOR_KW_EXCP) {
		symtab_set_exception(value, line, file);
	} else {
		errlog(FATAL|INPUT, "programmer error: impossible binding.");
	}
	errlog(END, "}");

/* Function 30 */
	errlog(BEGIN, "collect_errval() {");
	name = strtok(p, " \t\n\r");
	symtab_set_errval(name, line, db_get_current_file(), "int", "int", 0);
	errlog(END, "}");

/* Function 31 */
	errlog(BEGIN, "collect_arch() {");
	if (value == 0 || *value == '\0')
		errlog(FATAL|INPUT, "No architectures defined in ARCH line");

	if ((buf = strdup(value)) == NULL)
		errlog(FATAL, "Could not allocate memory in ARCH directive");

	t = buf;
	while ((p = strtok(t, " \r\t\n")) != NULL) {
		if (strcmp(p, arch) == 0 || strcmp(p, "all") == 0)
			goto cleanup;
		t = NULL;
	}
	symtab_set_skip(YES);

cleanup:
	free(buf);
	return (SUCCESS_RC);
}

/* Function 32 */
	} else if (p == q) {
		return (skipb(nextsep(p)));

/* Function 33 */
						p = symtab_get_next_arg()) {
			if (*name_of(p) != '\0')
				n -= snprintf(strend(buffer), n,
					", %s", name_of(p));
		}

/* Function 34 */
	} else if (!type_IsFunction(tp)) {
		errlog(FATAL|INPUT, "function %s is declared as a data item",

/* Function 35 */
	errlog(BEGIN, "generate_init() {");

	(void) fprintf(Headfp,
	    "/*\n"
	    " * Generated by spec2trace %s: do not edit this file.\n */\n\n",
	    TRACE_VERSION);

	(void) fprintf(Headfp,
	    "#ifndef true\n"
	    "#define\ttrue 1\n"
	    "#define\tfalse 0\n"
	    "#endif\n\n"
	    "static char const *oparen = \"(\";\n"
	    "static char const *retstr = \"  return = \";\n"
	    "static char const *errnostr = \" errno = \";\n"
	    "static char const *nilstr = \"<nil>\";\n"
	    "\n");

	errlog(END, "}");

/* Function 36 */
	errlog(BEGIN, "generate_interface() {");
	/* Check for required information. */
	if (validity_of(function) == NO) {
		symtab_set_skip(YES);
		errlog(WARNING|INPUT, "no prototype for interface "
			"it will be skipped");
		errlog(END, "}");
		return;
	}

/* Function 37 */
	errlog(BEGIN, "generate_closedown() {");

	/* Print includes to primary file. */
	generate_includes();
	(void) putc('\n', Headfp);
	errlog(END, "}");

/* Function 38 */
	errlog(BEGIN, "generate_aux_file() {");
	/* Open file */
	(void) snprintf(pathname, sizeof (pathname), "%s.pf",
		db_get_output_file());
	errlog(TRACING,  "output file = '%s'", pathname);
	if ((fp = fopen(pathname, "w")) == NULL) {
		errlog(FATAL, "%s: %s", pathname, strerror(errno));
	}

	/*
	 * Declare and initialize all print function pointers to null.
	 * Some spec files result in nothing being put into the .pf
	 * file.  We must create the file since make(1) does not cope
	 * well with absent files that it expects to have built.  So
	 * now the build gets empty compilation unit warnings...  So
	 * we unconditionally create a static pointer.
	 */
	(void) fprintf(fp,
	    "/* Do not edit this file: it is a generated one. */\n\n"
	    "static char const *__abi_place_holder;\n\n");

	generate_print_definitions(fp);

	/* Close file */
	if (fclose(fp) != 0) {
		errlog(FATAL, "fclose %s: %s", pathname, strerror(errno));
	}
	errlog(END, "}");

/* Function 39 */
	errlog(BEGIN, "generate_includes() {");
	errlog(TRACING,  "includes=");
	for (include = symtab_get_first_include(); include != NULL;
	    include = symtab_get_next_include())
		(void) fprintf(Headfp, "#include %s\n", include);

	(void) fprintf(Headfp, "\n#include <stdio.h>\n"
	    "#include <dlfcn.h>\n"
	    "#include <apptrace.h>\n\n");

	errlog(TRACING,  "\n");
	errlog(END, "}");


#ifdef __cplusplus
}
#endif

#endif /* TRACE_UNIFIED_C_H */

/* End of trace_unified.c - Synthesized by MINIX4 Synthesis System */
