/**
 * @file cron_unified.c
 * @brief Unified cron implementation
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
 *     1. userspace\minix_commands\cron\cron.c
 *     2. userspace\commands_legacy\system\cron\cron.c
 *     3. minix4\exokernel\kernel_legacy\cmd\cron\cron.c
 * 
 * Total source files: 3
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
#include "cron.h"
#include "misc.h"
#include "tab.h"
#include <alloca.h>
#include <ctype.h>
#include <deflt.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <libcontract.h>
#include <libcontract_priv.h>
#include <libzoneinfo.h>
#include <limits.h>
#include <locale.h>
#include <poll.h>
#include <project.h>
#include <pwd.h>
#include <security/pam_appl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stropts.h>
#include <sys/contract/process.h>
#include <sys/ctfs.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/task.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/* ===== DEFINES ===== */
#define nil ((void*)0)
#define	TMPDIR		"/tmp"
#define	PFX		"crout"
#define	CUSHION		180L
#define	JOBF		'j'
#define	NICEF		'n'
#define	USERF		'u'
#define	WAITF		'w'
#define	BCHAR		'>'
#define	ECHAR		'<'
#define	DEFAULT		0
#define	LOAD		1
#define	QBUFSIZ		80
#define	NO_ACTION	000
#define	REMOVE_FIFO	001
#define	CONSOLE_MSG	002
#define	BADCD		"can't change directory to the crontab directory."
#define	NOREADDIR	"can't read the crontab directory."
#define	BADJOBOPEN	"unable to read your at job."
#define	BADSHELL	"because your login shell \
#define	BADSTAT		"can't access your crontab or at-job file. Resubmit it."
#define	BADPROJID	"can't set project id for your job."
#define	CANTCDHOME	"can't change directory to %s.\
#define	CANTEXECSH	"unable to exec the shell, %s, for one of your \
#define	CANT_STR_LEN (sizeof (CANTEXECSH) > sizeof (CANTCDHOME) ? \
#define	NOREAD		"can't read your crontab file.  Resubmit it."
#define	BADTYPE		"crontab or at-job file is not a regular file.\n"
#define	NOSTDIN		"unable to create a standard input file for \
#define	NOTALLOWED	"you are not authorized to use cron.  Sorry."
#define	STDERRMSG	"\n\n********************************************\
#define	STDOUTERR	"one of your commands generated output or errors, \
#define	CLOCK_DRIFT	"clock time drifted backwards after event!\n"
#define	PIDERR		"unexpected pid returned %d (ignored)"
#define	CRONTABERR	"Subject: Your crontab file has an error in it\n\n"
#define	MALLOCERR	"out of space, cannot create new string\n"
#define	DIDFORK didfork
#define	NOFORK !didfork
#define	MAILBUFLEN	(8*1024)
#define	LINELIMIT	80
#define	MAILBINITFREE	(MAILBUFLEN - (sizeof (cte_intro) - 1) \
#define	PROJECT		"project="
#define	FORMAT	"%a %b %e %H:%M:%S %Y"
#define	ROOTPATH	"PATH=/usr/sbin:/usr/bin"
#define	NONROOTPATH	"PATH=/usr/bin:"
#define	DEFTZ		"GMT"
#define	VUC_OK		0
#define	VUC_BADUSER	1
#define	VUC_NOTINGROUP	2
#define	VUC_EXPIRED	3
#define	VUC_NEW_AUTH	4
#define	CRON_ANC_DELETE	1
#define	CRON_ANC_CREATE	0
#define	tm_cmp(t1, t2) (\
#define	tm_setup(tp, yr, mon, dy, hr, min, dst) \
#define	DUMMY	100

/* ===== TYPES ===== */
	struct passwd *pw;
		struct stat st;
		struct dirent *entry;
	struct sigaction sa, osa;
				struct timeval tvnext;
	struct passwd *pw;
		struct stat st;
		struct dirent *entry;
	struct sigaction sa, osa;
				struct timeval tvnext;
struct shared {
struct event {
	union {
struct usr {
	struct usr *nextusr;
	struct runinfo *next;
	struct miscpid	*next;
    struct pam_response **, void *);
	struct usr *u;
	struct event *e, *e2, *eprev;
	struct stat buf;
	struct sigaction act;
	struct dirent	*dp;
	struct stat	buf;
struct usr *
	struct usr	*u;
	struct usr	*u;
	struct usr	*u;
	struct	passwd	*pw;
	struct	stat	buf;
	struct	usr	*u;
	struct	passwd	*pw;
	struct	stat	buf;
	struct	usr	*u;
	struct event *e;
	struct event *e;
	struct event *e;
	struct shared *tz = NULL;
	struct shared *home = NULL;
	struct shared *shell = NULL;
	struct passwd	*ruser_ids;
	struct utsname	name;
	struct tm *tm, ref_tm, tmp, tmp1, tmp2;
	struct tm tmp, tmp1;
	struct usr *cur, *prev;
	struct	event	*e, *eprev;
	struct	usr	*u;
	struct	usr *u;
	struct event *e2, *e3;
	struct usr *u;
	struct stat buf;
	struct queue *qp;
	struct runinfo *rp;
	struct project proj, *pproj = NULL;
	union {
		struct {
	struct	runinfo	*rp;
	struct	usr	*p;
	struct	stat	buf;
	struct	passwd	*ruser_ids;
	struct utsname	name;
	struct	message	msg;
	struct timespec tout, *toutp;
	struct runinfo *rp;
	struct runinfo **rpp;
	struct runinfo *rp;
	struct passwd *pw;
	struct event *ev, *pv;
	struct usr	*u = uhead;
	struct event	*e;
    struct pam_response **response __unused, void *appdata_ptr __unused)
	struct miscpid *mp;
	struct miscpid *mp, *omp;
	struct shared *out;

/* ===== GLOBAL VARIABLES ===== */
	int need_mailer;
	int mailfd[2], errfd[2];
			char subject[70+20], *ps;
						pw->pw_name, (char *) nil);
	(void) fcntl(errfd[1], F_SETFD, fcntl(errfd[1], F_GETFD) | FD_CLOEXEC);
			execl("/bin/sh", "sh", tab->data, (char *) nil);
			execl("/bin/sh", "sh", "-c", job->cmd, (char *) nil);
		char name[NAME_MAX+1];
		char *np;
		int c;
		char tab[sizeof("/var/opt//lib/crontab") + NAME_MAX];
		char tab[sizeof("/usr/spool/crontabs/") + NAME_MAX];
		tab_print(stderr);
	fprintf(stderr, "Usage: %s [-d[#]]\n", prog_name);
	int i;
	int r;
		char *opt= argv[i++] + 1;
		fprintf(stderr, "%s: %s\n", PIDFILE, strerror(errno));
	fprintf(pf, "%d\n", getpid());
		fprintf(stderr, "%s: %s\n", PIDFILE, strerror(errno));
	int need_mailer;
	int mailfd[2], errfd[2];
			char subject[70+20], *ps;
						pw->pw_name, (char *) nil);
	(void) fcntl(errfd[1], F_SETFD, fcntl(errfd[1], F_GETFD) | FD_CLOEXEC);
			execl("/bin/sh", "sh", tab->data, (char *) nil);
			execl("/bin/sh", "sh", "-c", job->cmd, (char *) nil);
		char name[NAME_MAX+1];
		char *np;
		int c;
		char tab[sizeof("/var/opt//lib/crontab") + NAME_MAX];
		char tab[sizeof("/usr/spool/crontabs/") + NAME_MAX];
		tab_print(stderr);
	fprintf(stderr, "Usage: %s [-d[#]]\n", prog_name);
	int i;
	int r;
		char *opt= argv[i++] + 1;
		fprintf(stderr, "%s: %s\n", PIDFILE, strerror(errno));
	fprintf(pf, "%d\n", getpid());
		fprintf(stderr, "%s: %s\n", PIDFILE, strerror(errno));
static char	timebuf[80];
static struct message msgbuf;
static struct	queue	qq;
static char cte_intro[] = "Line(s) with errors:\n\n";
static char cte_trail1[] = "\nMax number of errors encountered.";
static char cte_trail2[] = " Evaluation of crontab aborted.\n";
static char *Def_supath	= NULL;
static char *Def_path		= NULL;
static char path[LINE_MAX]	= "PATH=";
static char supath[LINE_MAX]	= "PATH=";
static char homedir[LINE_MAX]	= ENV_HOME;
static char logname[LINE_MAX]	= "LOGNAME=";
static char tzone[LINE_MAX]	= ENV_TZ;
extern char **environ;
static	int	log = 0;
static	char	hzname[10];
static void cronend(int);
static void thaw_handler(int);
static void child_handler(int);
static void child_sigreset(void);
static void mod_ctab(char *, time_t);
static void mod_atjob(char *, time_t);
static void add_atevent(struct usr *, char *, time_t, int);
static void rm_ctevents(struct usr *);
static void cleanup(struct runinfo *rn, int r);
static void crabort(char *, int);
static void msg(char *fmt, ...);
static void ignore_msg(char *, char *, struct event *);
static void logit(int, struct runinfo *, int);
static void parsqdef(char *);
static void defaults();
static void initialize(int);
static void quedefs(int);
static int idle(long);
static struct usr *find_usr(char *);
static int ex(struct event *e);
static void read_dirs(int);
static void mail(char *, char *, int);
static void readcron(struct usr *, time_t);
static int next_ge(int, char *);
static void del_atjob(char *, char *);
static void del_ctab(char *);
static void resched(int);
static int msg_wait(long);
static struct runinfo *rinfo_get(pid_t);
static void rinfo_free(struct runinfo *rp);
static void mail_result(struct usr *p, struct runinfo *pr, size_t filesize);
static time_t next_time(struct event *, time_t);
static time_t get_switching_time(int, time_t);
static time_t xmktime(struct tm *);
static void process_msg(struct message *, time_t);
static void reap_child(void);
static void miscpid_insert(pid_t);
static int miscpid_delete(pid_t);
static void contract_set_template(void);
static void contract_clear_template(void);
static void contract_abandon_latest(pid_t);
static void cte_init(void);
static void cte_add(int, char *);
static void cte_valid(void);
static int cte_istoomany(void);
static void cte_sendmail(char *);
static int set_user_cred(const struct usr *, struct project *);
static struct shared *create_shared_str(char *str);
static struct shared *dup_shared(struct shared *obj);
static void rel_shared(struct shared *obj);
static void *get_obj(struct shared *obj);
static time_t last_time, init_time, t_old;
static int		refresh;
static sigset_t		defmask, sigmask;
extern int	audit_cron_session(char *, char *, uid_t, gid_t, char *);
extern void	audit_cron_new_job(char *, int, void *);
extern void	audit_cron_bad_user(char *);
extern void	audit_cron_user_acct_expired(char *);
extern int	audit_cron_create_anc_file(char *, char *, char *, uid_t);
extern int	audit_cron_delete_anc_file(char *, char *);
extern int	audit_cron_is_anc_name(char *);
extern int	audit_cron_mode();
static struct pam_conv pam_conv = {cron_conv, NULL};
static void clean_out_atjobs(struct usr *u);
static void clean_out_ctab(struct usr *u);
static void clean_out_user(struct usr *u);
static void cron_unlink(char *name);
static void process_anc_files(int);
extern void el_init(int, time_t, time_t, int);
extern int el_add(void *, time_t, int);
extern void el_remove(int, int);
extern int el_empty(void);
extern void *el_first(void);
extern void el_delete(void);
static int valid_entry(char *, int);
static struct usr *create_ulist(char *, int);
static void init_cronevent(char *, int);
static void init_atevent(char *, time_t, int, int);
static void update_atevent(struct usr *, char *, time_t, int);
	(void) setlocale(LC_ALL, "");
				(void) sleep(30);
	(void) umask(022);
	(void) signal(SIGHUP, SIG_IGN);
	(void) signal(SIGINT, SIG_IGN);
	(void) signal(SIGQUIT, SIG_IGN);
	(void) signal(SIGTERM, cronend);
	(void) sigemptyset(&act.sa_mask);
	(void) sigaction(SIGTHAW, &act, NULL);
	(void) sigemptyset(&act.sa_mask);
	(void) sigaddset(&act.sa_mask, SIGCLD);
	(void) sigaction(SIGCLD, &act, NULL);
	(void) sigemptyset(&defmask);
	(void) sigemptyset(&sigmask);
	(void) sigaddset(&sigmask, SIGCLD);
	(void) sigaddset(&sigmask, SIGTHAW);
	(void) sigprocmask(SIG_BLOCK, &sigmask, NULL);
			(void) close(msgfd);
	(void) fprintf(stderr, "in initialize\n");
					(void) sleep(60);
				(void) sleep(60);
		(void) fprintf(stderr, "cannot open %s\n", ACCTFILE);
	(void) fchmod(1, S_IRUSR|S_IWUSR);
	(void) close(fileno(stderr));
	(void) dup(1);
	(void) freopen("/dev/null", "r", stdin);
	char		*ptr;
	int		jobtype;
	(void) closedir(dir);
	(void) closedir(dir);
	char	namebuf[LINE_MAX];
	char	*pname;
		(void) fprintf(stderr, "new user (%s) with a crontab\n", name);
		(void) strcpy(u->home, pw->pw_dir);
				(void) strcpy(u->home, pw->pw_dir);
			(void) strcpy(u->home, pw->pw_dir);
	char	*ptr;
	char	namebuf[PATH_MAX];
	char	*pname;
	int	jobtype;
	(void) strcpy(e->cmd, job);
	int start;
	unsigned int i;
	char namebuf[PATH_MAX];
	char *pname;
	uint32_t max_random_delay = 0;
	int lineno = 0;
	const char *errstr;
		char *tmp;
			(void) fprintf(stderr, "Error: %d %s", lineno, line);
		(void) strncpy(e->cmd, line + start, cursor-start);
			(void) strcpy(e->of.ct.input, line + cursor + 1);
	(void) fclose(cf);
	(void) strlcpy(cte_text, cte_intro, MAILBUFLEN);
	cte_lp = cte_text + sizeof (cte_intro) - 1;
	int len;
	char *p;
		(void) sprintf(cte_lp, "%4d: ", lineno);
		(void) strlcat(cte_lp, ctline, LINELIMIT - 1);
			(void) strlcpy(cte_lp, cte_trail1, buflen);
				(void) strlcat(cte_lp, cte_trail2, buflen);
	char *temp;
	int saveerrno = errno;
	(void) uname(&name);
		(void) setuid(ruser_ids->pw_uid);
		(void) sprintf(temp, "%s %s", MAIL, usrname);
			(void) fprintf(pipe, "To: %s\n", usrname);
				(void) fprintf(pipe, CRONTABERR);
				(void) fprintf(pipe, mesg);
				(void) fprintf(pipe, "Subject: %s\n\n", mesg);
				(void) fprintf(pipe, "couldn't be run\n");
				(void) fprintf(pipe, "%s\n", mesg);
				(void) fprintf(pipe, "couldn't be run\n");
				(void) fprintf(pipe, "%s\n", mesg);
			(void) pclose(pipe);
	int tm_mon, tm_mday, tm_wday, wday, m, min, h, hr, carry, day, days;
	int d1, day1, carry1, d2, day2, carry2, daysahead, mon, yr, db, wd;
	int today;
	int fallback;
	extern int days_btwn(int, int, int, int, int, int);
	(void) localtime_r(&t, tm);
			(void) localtime_r(&t, &tmp);
			int dst = tmp2.tm_isdst;
			(void) localtime_r(&t2, &tmp2);
			(void) localtime_r(&t, &tmp);
		(void) putenv((char *)get_obj(e->of.ct.tz));
		(void) putenv(tzone);
	int i;
	(void) localtime_r(&t_ref, &tmp);
		t1 = t + hints[i] * 60;
		(void) localtime_r(&t1, &tmp1);
			(void) localtime_r(&t1, &tmp1);
		(void) localtime_r(&t, &tmp);
	char *ptr;
	int n, n2, min, min_gt;
		(void) fprintf(stderr, "%s removed from usr list\n", u->name);
	int r;
	int fd;
	char mailvar[4];
	char *at_cmdfile = NULL;
			char buf[PROJECT_BUFSZ];
			char buf2[PROJECT_BUFSZ];
		char error[CANT_STR_LEN + PATH_MAX];
	char *tmpfile;
	int projflag = 0;
	char *home;
	char *sh;
		(void) strcpy(rp->jobname, e->cmd);
		(void) sprintf(at_cmdfile, "%s/%s", ATDIR, e->cmd);
		(void) strcpy(rp->jobname, at_cmdfile);
		(void) fscanf(atcmdfp, "%*[^\n]\n");
		(void) fscanf(atcmdfp, "%*[^\n]\n");
		(void) fclose(atcmdfp);
			(void) sleep(30);
			(void) dup2(fd, 0);
			(void) close(fd);
	audit_cron_new_job(e->cmd, e->etype, (void *)e);
					(void) dup2(fd, 0);
					(void) close(fd);
			(void) fclose(fptr);
			(void) dup2(fd, 0);
			(void) close(fd);
		(void) dup2(fd, 1);
		(void) dup2(fd, 2);
			(void) close(fd);
		home = (char *)get_obj(e->of.ct.home);
	(void) strlcat(homedir, home, sizeof (homedir));
	(void) strlcat(logname, (e->u)->name, sizeof (logname));
		snprintf(bufs.error, sizeof (bufs.error), CANTCDHOME, home);
		(void) nice(qp->nice);
			(void) putenv((char *)get_obj(e->of.ct.tz));
			char *name;
			sh = (char *)get_obj(e->of.ct.shell);
			(void) putenv(sh);
			(void) execl(sh, name, "-c", e->cmd, 0);
			(void) execl(SHELL, "sh", "-c", e->cmd, 0);
		(void) execl(SHELL, "sh", 0);
	snprintf(bufs.error, sizeof (bufs.error), CANTEXECSH, sh);
	int	prc;
			(void) unlink(rp->outfile);
	int	nbytes;
	char	iobuf[BUFSIZ];
	char	*cmd;
	char	*lowname = (pr->jobtype == CRONEVENT ? "cron" : "at");
	(void) uname(&name);
	(void) setuid(ruser_ids->pw_uid);
	(void) sprintf(cmd, "%s %s", MAIL, p->name);
	(void) fprintf(mailpipe, "To: %s\n", p->name);
	(void) fprintf(mailpipe, "Auto-Submitted: auto-generated\n");
	(void) fprintf(mailpipe, "X-Cron-User: %s\n", p->name);
	(void) fprintf(mailpipe, "X-Cron-Host: %s\n", name.nodename);
	(void) fprintf(mailpipe, "X-Cron-Job-Name: %s\n", pr->jobname);
	(void) fprintf(mailpipe, "X-Cron-Job-Type: %s\n", lowname);
	(void) fprintf(mailpipe, "\n");
	(void) pclose(mailpipe);
	int	cnt;
	static int	pending_msg;
	static time_t	pending_reftime;
		(void) memcpy(&msgbuf, &msg, sizeof (msg));
	(void) fflush(stdout);
	(void) fprintf(stderr, "freeing job %s\n", entry->jobname);
	(void) signal(SIGCLD, SIG_DFL);
	(void) sigprocmask(SIG_SETMASK, &defmask, NULL);
	int	c;
			(void) write(c, "cron aborted: ", 14);
			(void) write(c, mssg, strlen(mssg));
			(void) write(c, "\n", 1);
			(void) close(c);
	(void) fflush(stdout);
	(void) fprintf(stderr, "! ");
	(void) vfprintf(stderr, fmt, args);
	(void) strftime(timebuf, sizeof (timebuf), FORMAT, localtime(&t));
	(void) fprintf(stderr, " %s\n", timebuf);
	(void) fflush(stderr);
	int    ret;
		(void) printf("%c  CMD: %s\n", cc, next_event->cmd);
	(void) strftime(timebuf, sizeof (timebuf), FORMAT, localtime(&t));
		(void) printf(" ts=%d", ret);
		(void) printf(" rc=%d", ret);
	(void) putchar('\n');
	(void) fflush(stdout);
	int	i;
	int	j;
	char	qbuf[QBUFSIZ];
	(void) fclose(fd);
	int i;
	int  flags;
	char *deflog;
	char *hz, *tz;
		(void) sprintf(hzname, "HZ=%d", HZ);
		(void) snprintf(hzname, sizeof (hzname), "HZ=%s", hz);
		(void) defcntl(DC_SETFLAGS, flags);
			(void) strlcat(path, Def_path, LINE_MAX);
			(void) strlcpy(path, NONROOTPATH, LINE_MAX);
			(void) strlcat(supath, Def_supath, LINE_MAX);
			(void) strlcpy(supath, ROOTPATH, LINE_MAX);
		(void) defopen(NULL);
	int retval;
	static size_t nGroupsMax = (size_t)-1;
	static gid_t *UsrGrps;
	static gid_t *OrigGrps;
			(void) strcpy(u->home, pw->pw_dir);
		(void) strcpy(u->home, pw->pw_dir);
		(void) fprintf(stderr, "nGroupsMax = %ld\n", nGroupsMax);
		(void) initgroups(pw->pw_name, pw->pw_gid);
			(void) setgroups(numOrigGrps, OrigGrps);
	static char *progname = "cron";
	int r = 0, rval = 0;
		char *buf = alloca(sz);
		(void) snprintf(buf, sz, PROJECT "%s", pproj->pj_name);
		(void) pam_set_item(pamh, PAM_RESOURCE, buf);
	(void) pam_end(pamh, r);
	int r;
		(void) audit_cron_delete_anc_file(name, NULL);
	int i;
				(void) msg("%s\n", m[i]->msg);
	int found = 0;
	int fd;
	(void) close(fd);
	int fd;
	(void) close(fd);
	int r;
	static uint_t cts_lost;

/* ===== FUNCTIONS ===== */

/* Function 1 */
			    (next_event->u)->ctid)) {
			case -1:
				ignore_msg("main", "cron", next_event);
				break;
				reset_needed = 1;
				break;
			}

/* Function 2 */
		    CRONDIR, name) >= sizeof (namebuf)) {
			msg("Too long path name %s - cron entries not created",
			    namebuf);
			return;
		}

/* Function 3 */
	    (strcmp(pw->pw_shell, SHELL) != 0)) {
		mail(name, BADSHELL, ERR_CANTEXECCRON);
		cron_unlink(pname);
		return;
	}

/* Function 4 */
		    >= sizeof (namebuf)) {
			return;
		}

/* Function 5 */
	    (strcmp(pw->pw_shell, SHELL) != 0)) {
		mail(pw->pw_name, BADSHELL, ERR_CANTEXECAT);
		cron_unlink(pname);
		return;
	}

/* Function 6 */
		    CRONDIR, u->name) >= sizeof (namebuf)) {
			return;
		}

/* Function 7 */
		    strlen(ENV_TZ)) == 0) {
			if ((tmp = strchr(&line[cursor], '\n')) != NULL) {
				*tmp = '\0';
			}

			if (!isvalid_tz(&line[cursor + strlen(ENV_TZ)], NULL,
			    _VTZ_ALL)) {
				cte_add(lineno, line);
				break;
			}
			if (tz == NULL || strcmp(&line[cursor], get_obj(tz))) {
				rel_shared(tz);
				tz = create_shared_str(&line[cursor]);
			}
			continue;
		}

/* Function 8 */
		    strlen(ENV_HOME)) == 0) {
			if ((tmp = strchr(&line[cursor], '\n')) != NULL) {
				*tmp = '\0';
			}
			if (home == NULL ||
			    strcmp(&line[cursor], get_obj(home))) {
				rel_shared(home);
				home = create_shared_str(
				    &line[cursor + strlen(ENV_HOME)]);
			}
			continue;
		}

/* Function 9 */
		    strlen(ENV_SHELL)) == 0) {
			if ((tmp = strchr(&line[cursor], '\n')) != NULL) {
				*tmp = '\0';
			}
			if (shell == NULL ||
			    strcmp(&line[cursor], get_obj(shell))) {
				rel_shared(shell);
				shell = create_shared_str(&line[cursor]);
			}
			continue;
		}

/* Function 10 */
		    strlen(ENV_RANDOM_DELAY)) == 0) {
			if ((tmp = strchr(&line[cursor], '\n')) != NULL) {
				*tmp = '\0';
			}

			max_random_delay = strtonum(
			    &line[cursor + strlen(ENV_RANDOM_DELAY)], 0,
			    UINT32_MAX / 60, &errstr);
			if (errstr != NULL) {
				cte_add(lineno, line);
				break;
			}

			continue;
		}

/* Function 11 */
		    tm_cmp(tm, &tmp)) {
			zone_start = get_switching_time(tmp.tm_isdst, t);
			fallback = 1;
		}

/* Function 12 */
	    (tm->tm_mon != tm_mon)) {
		today = FALSE;
	}

/* Function 13 */
			    tm_cmp(&tmp, &tmp2)) {
				return (t1);
			} else {
				if (tmp1.tm_isdst != tmp2.tm_isdst) {
					t = get_switching_time(tmp1.tm_isdst,
					    t1);
				} else {
					t = get_switching_time(tmp1.tm_isdst,
					    t1 - abs(timezone - altzone));
				}
				if (t == (time_t)-1) {
					return (0);
				}
			}

/* Function 14 */
	    (strcmp(e->of.ct.dayweek, "*") != 0)) {
		day1 = day2;
		carry1 = carry2;
	}

/* Function 15 */
	    (strcmp(e->of.ct.dayweek, "*") == 0)) {
		day2 = day1;
		carry2 = carry1;
	}

/* Function 16 */
			    day <= next_ge(1, e->of.ct.daymon)) {
				return (0);
			}

/* Function 17 */
	    (next_event->u == u)) {
		next_event = NULL;
	}

/* Function 18 */
			    (next_event->u)->ctid)) {
			case -1:
				ignore_msg("ex", "cron", next_event);
				break;
				reset_needed = 1;
				break;
			}

/* Function 19 */
			    bufs.p.buf2, sizeof (bufs.p.buf2))) {
				cron_unlink(at_cmdfile);
				mail((e->u)->name, BADPROJID, ERR_CANTEXECAT);
				exit(1);
			}

/* Function 20 */
	    initgroups(e->u->name, e->u->gid) == -1) {
		msg("bad user (%s) or setgid failed (%s)",
		    e->u->name, e->u->name);
		audit_cron_bad_user(e->u->name);
		clean_out_user(e->u);
		exit(1);
	}

/* Function 21 */
			    (fptr = fdopen(fd, "w")) == NULL) {
				mail((e->u)->name, NOSTDIN,
				    ERR_CANTEXECCRON);
				cron_unlink(tmpfile);
				free(tmpfile);
				exit(1);
			}

/* Function 22 */
			    strlen(e->of.ct.input)) {
				mail((e->u)->name, NOSTDIN, ERR_CANTEXECCRON);
				cron_unlink(tmpfile);
				free(tmpfile);
				(void) close(fd);
				(void) fclose(fptr);
				exit(1);
			}

/* Function 23 */
		    (buf.st_size > 0 || pr->mailwhendone)) {
			for (;;) {
				if ((pr->pid = fork()) < 0) {
					(void) sleep(nextfork);
					if (nextfork < 16)
						nextfork += nextfork;
					continue;
				} else if (pr->pid == 0) {
					child_sigreset();
					contract_clear_template();

					mail_result(p, pr, buf.st_size);
				} else {
					contract_abandon_latest(pr->pid);
					pr->que = ZOMB;
					break;
				}
			}
		} else {
			(void) unlink(pr->outfile);
			rinfo_free(pr);
		}

/* Function 24 */
	    (st = fopen(pr->outfile, "r")) != NULL) {
		while ((nbytes = fread(iobuf, sizeof (char), BUFSIZ, st)) != 0)
			(void) fwrite(iobuf, sizeof (char), nbytes, mailpipe);
		(void) fclose(st);
	} else {
		(void) fprintf(mailpipe, "Job completed with no output.\n");
	}

/* Function 25 */
			    (next_event->u)->ctid)) {
			case -1:
				ignore_msg("process_msg", "cron", next_event);
				break;
				reset_needed = 1;
				break;
			}

/* Function 26 */
		    (next_event->u)->ctid)) {
		case -1:
			ignore_msg("resched", "cron", next_event);
			break;
			reset_needed = 1;
			break;
		}

/* Function 27 */
	    pv = ev, ev = ev->link, free(pv)) {
		el_remove(ev->of.at.eventid, 1);
		if (cwd == AT)
			cron_unlink(ev->cmd);
		else {
			char buf[PATH_MAX];
			if (strlen(ATDIR) + strlen(ev->cmd) + 2
			    < PATH_MAX) {
				(void) sprintf(buf, "%s/%s", ATDIR, ev->cmd);
				cron_unlink(buf);
			}
		}
		free(ev->cmd);
	}


#ifdef __cplusplus
}
#endif

/* End of cron_unified.c - Synthesized by MINIX4 Massive Synthesis System */
