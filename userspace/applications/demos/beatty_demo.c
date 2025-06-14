#include <stdio.h>
#include "math_core.h"

static double alpha;
static double beta;
static int na = 1;
static int nb = 1;

static void task_a(int slice) { printf("A slice %d\n", slice); }
static void task_b(int slice) { printf("B slice %d\n", slice); }

static void schedule_step(int slice) {
  double va = alpha * (double)na;
  double vb = beta * (double)nb;
  if ((int)va < (int)vb) {
    task_a(slice);
    na++;
  } else {
    task_b(slice);
    nb++;
  }
}

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;
#ifdef HAVE_DECIMAL_FLOAT
  alpha = dec64_to_double(phi());
#else
  alpha = phi();
#endif
  beta = alpha / (alpha - 1.0);
  for (int i = 1; i <= 10; i++)
    schedule_step(i);
  return 0;
}
#include "types.h"
#include "user.h"
#include "math_core.h"
#include "dag.h"
#include "libos/sched.h"

static double alpha;
static double beta;
static int na = 1;
static int nb = 1;

struct simple_node {
    const char *name;
    struct simple_node *next;
};

static struct simple_node a1 = {"A1", 0};
static struct simple_node a2 = {"A2", 0};
static struct simple_node b1 = {"B1", 0};
static struct simple_node b2 = {"B2", 0};

static struct simple_node *afam = &a1;
static struct simple_node *bfam = &b1;

static void run_family(struct simple_node **head) {
    if (*head) {
        printf(1, "%s\n", (*head)->name);
        *head = (*head)->next;
    }
}

static void schedule_step(void) {
    double va = alpha * (double)na;
    double vb = beta * (double)nb;
    if ((int)va < (int)vb) {
        run_family(&afam);
        na++;
    } else {
        run_family(&bfam);
        nb++;
    }
    exo_stream_yield();
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;
#ifdef HAVE_DECIMAL_FLOAT
    alpha = dec64_to_double(phi());
#else
    alpha = phi();
#endif
    beta = alpha / (alpha - 1.0);

    // build simple DAGs
    a1.next = &a2;
    b1.next = &b2;

    dag_sched_init();
    beatty_sched_init();

    dag_node_init((struct dag_node *)&a1, (exo_cap){0});
    dag_node_init((struct dag_node *)&a2, (exo_cap){0});
    dag_node_init((struct dag_node *)&b1, (exo_cap){0});
    dag_node_init((struct dag_node *)&b2, (exo_cap){0});

    dag_sched_submit((struct dag_node *)&a1);
    dag_sched_submit((struct dag_node *)&a2);
    dag_sched_submit((struct dag_node *)&b1);
    dag_sched_submit((struct dag_node *)&b2);

    printf(1, "beatty_dag_demo start\n");
    for (int i = 0; i < 4; i++)
        schedule_step();
    printf(1, "beatty_dag_demo done\n");
    exit();
}
#include "types.h"
#include "user.h"
#include "chan.h"
#include "capnp_helpers.h"
#include "math_core.h"
#include "libos/driver.h"

typedef struct {
    uint8_t len;
    char data[8];
} VarMsg;

static size_t VarMsg_encode(const VarMsg *m, unsigned char *buf) {
    if(buf){
        buf[0] = m->len;
        for(size_t i=0;i<m->len;i++)
            buf[1+i] = m->data[i];
    }
    return 1 + m->len;
}

static size_t VarMsg_decode(VarMsg *m, const unsigned char *buf) {
    m->len = buf[0];
    for(size_t i=0;i<m->len;i++)
        m->data[i] = buf[1+i];
    m->data[m->len] = '\0';
    return 1 + m->len;
}

#define VarMsg_MESSAGE_SIZE 9

CHAN_DECLARE_VAR(ping_chan, VarMsg);

static ping_chan_t *c;
static double alpha;
static double beta;
static int na = 1;
static int nb = 1;
static int child;

static void send_task(int slice) {
    VarMsg m = { .len = 1, .data = { (char)('0' + slice) } };
    exo_cap cap = {0};
    ping_chan_send(c, cap, &m);
    printf(1, "sent %c\n", m.data[0]);
}

static void recv_task(void) {
    VarMsg out = {0};
    exo_cap cap = {0};
    ping_chan_recv(c, cap, &out);
    printf(1, "received %s\n", out.data);
}

static void schedule_step(int slice) {
    double va = alpha * (double)na;
    double vb = beta * (double)nb;
    if ((int)va < (int)vb) {
        send_task(slice);
        na++;
    } else {
        recv_task();
        nb++;
    }
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    char *rargv[] = {"typed_chan_recv", 0};
    child = driver_spawn("typed_chan_recv", rargv);

    c = ping_chan_create();
#ifdef HAVE_DECIMAL_FLOAT
    alpha = dec64_to_double(phi());
#else
    alpha = phi();
#endif
    beta = alpha / (alpha - 1.0);

    for (int i = 1; i <= 4; i++) {
        if (i == 3) {
            printf(1, "simulating rcrs restart\n");
            kill(child);
            wait();
            child = driver_spawn("typed_chan_recv", rargv);
        }
        schedule_step(i);
    }

    kill(child);
    wait();
    ping_chan_destroy(c);
    exit();
}
#include "types.h"
#include "user.h"
#include "chan.h"
#include "capnp_helpers.h"
#include "dag.h"
#include "libos/driver.h"
#include "proto/driver.capnp.h"

CHAN_DECLARE(ping_chan, DriverPing);

static ping_chan_t *c;
static struct dag_node send_node, recv_node;

static void send_task(void)
{
    DriverPing m = { .value = 42 };
    exo_cap cap = {0};
    ping_chan_send(c, cap, &m);
    printf(1, "sent ping\n");
}

static void recv_task(void)
{
    DriverPing out = {0};
    exo_cap cap = {0};
    ping_chan_recv(c, cap, &out);
    printf(1, "received %d\n", out.value);
}

int main(int argc, char *argv[])
{
    (void)argc; (void)argv;
    char *rargv[] = {"typed_chan_recv", 0};
    int pid = driver_spawn("typed_chan_recv", rargv);

    dag_sched_init();
    c = ping_chan_create();

    dag_node_init(&send_node, (exo_cap){0});
    dag_node_init(&recv_node, (exo_cap){0});
    dag_node_add_dep(&send_node, &recv_node);

    dag_sched_submit(&send_node);
    dag_sched_submit(&recv_node);

    send_task();
    recv_task();
    exo_stream_yield();

    kill(pid);
    wait();

    ping_chan_destroy(c);
    exit();
}
#include "types.h"
#include "user.h"
#include "affine_runtime.h"
#include "capnp_helpers.h"
#include "proto/driver.capnp.h"

AFFINE_CHAN_DECLARE(aff_ping_chan, DriverPing);

int
main(int argc, char *argv[])
{
    (void)argc; (void)argv;
    aff_ping_chan_t *c = aff_ping_chan_create();
    DriverPing m = { .value = 77 };
    exo_cap cap = {0};

    int r = aff_ping_chan_send(c, cap, &m);
    printf(1, "first send %d\n", r);

    r = aff_ping_chan_send(c, cap, &m);
    printf(1, "second send %d\n", r);

    aff_ping_chan_destroy(c);
    exit();
}
    cprintf("Init process started\n");
    // Load and execute init program
    exec("/bin/init", NULL);
    cprintf("Init process failed to start\n");
    exit(1);
} #include "libos/exo-userland.h"
#include "libos/sched.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct dag_node {
  int pending;
  int priority;
};

static void dag_node_init(struct dag_node *n, exo_cap ctx) {
  (void)ctx;
  n->pending = 0;
  n->priority = 0;
}
static void dag_node_add_dep(struct dag_node *parent, struct dag_node *child) {
  (void)parent;
  (void)child;
}
static void dag_sched_submit(struct dag_node *node) {
  printf("dag_sched_submit priority %d\n", node->priority);
}
static void exo_stream_yield(void) { printf("exo_stream_yield called\n"); }

static struct dag_node a, b, c;

static void setup(void) {
  exo_cap cap = {0};
  dag_node_init(&a, cap);
  dag_node_set_priority(&a, 10);
  dag_node_init(&b, cap);
  dag_node_set_priority(&b, 5);
  dag_node_init(&c, cap);
  dag_node_set_priority(&c, 1);
  dag_node_add_dep(&a, &c);
  dag_node_add_dep(&b, &c);
  dag_sched_submit(&a);
  dag_sched_submit(&b);
  dag_sched_submit(&c);
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  printf("DAG scheduler demo\n");
  setup();
  exo_stream_yield();
  exo_stream_yield();
  exo_stream_yield();
  return 0;
}
/* Copyright (c) 1979 Regents of the University of California */
#
/*
 * pi - Pascal interpreter code translator
 *
 * Charles Haley, Bill Joy UCB
 * Version 1.2 January 1979
 *
 *
 * pxp - Pascal execution profiler
 *
 * Bill Joy UCB
 * Version 1.2 January 1979
 */

#include "0.h"
#include "yy.h"

/*
 * The definition for the segmented hash tables.
 */
struct ht {
	int	*ht_low;
	int	*ht_high;
	int	ht_used;
} htab[MAXHASH];

/*
 * This is the array of keywords and their
 * token values, which are hashed into the table
 * by inithash.
 */
struct kwtab yykey[] {
	"and",		YAND,
	"array",	YARRAY,
	"assert",	YASSERT,
	"begin",	YBEGIN,
	"case",		YCASE,
	"const",	YCONST,
	"div",		YDIV,
	"do",		YDO,
	"downto",	YDOWNTO,
	"else",		YELSE,
	"end",		YEND,
	"file",		YFILE,
	"for",		YFOR,
	"forward",	YFORWARD,
	"function",	YFUNCTION,
	"goto",		YGOTO,
	"if",		YIF,
	"in",		YIN,
	"label",	YLABEL,
	"mod",		YMOD,
	"nil",		YNIL,
	"not",		YNOT,
	"of",		YOF,
	"or",		YOR,
	"packed",	YPACKED,
	"procedure",	YPROCEDURE,
	"program",	YPROG,
	"record",	YRECORD,
	"repeat",	YREPEAT,
	"set",		YSET,
	"then",		YTHEN,
	"to",		YTO,
	"type",		YTYPE,
	"until",	YUNTIL,
	"var",		YVAR,
	"while",	YWHILE,
	"with",		YWITH,
	"oct",		YOCT,	 /* non-standard Pascal */
	"hex",		YHEX,	 /* non-standard Pascal */
	0
};

char	*lastkey	&yykey[sizeof yykey/sizeof yykey[0]];

/*
 * Inithash initializes the hash table routines
 * by allocating the first hash table segment using
 * an already existing memory slot.
 */
#ifndef PI0
inithash()
#else
inithash(hshtab)
	int *hshtab;
#endif
{
	register int *ip;
#ifndef PI0
	static int hshtab[HASHINC];
#endif

	htab[0].ht_low = hshtab;
	htab[0].ht_high = &hshtab[HASHINC];
	for (ip = yykey; *ip; ip =+ 2)
		hash(ip[0], 0)[0] = ip;
}

/*
 * Hash looks up the s(ymbol) argument
 * in the string table, entering it if
 * it is not found. If save is 0, then
 * the argument string is already in
 * a safe place. Otherwise, if hash is
 * entering the symbol for the first time
 * it will save the symbol in the string
 * table using savestr.
 */
int *hash(s, save)
	char *s;
	int save;
{
	register int *h;
	register i;
	register char *cp;
	int *sym;
	struct ht *htp;
	int sh;

	/*
	 * The hash function is a modular hash of
	 * the sum of the characters with the sum
	 * doubled before each successive character
	 * is added.
	 */
	cp = s;
	if (cp == NIL)
		cp = token;	/* default symbol to be hashed */
	i = 0;
	while (*cp)
		i = i*2 + *cp++;
	sh = (i&077777) % HASHINC;
	cp = s;
	if (cp == NIL)
		cp = token;
	/*
	 * There are as many as MAXHASH active
	 * hash tables at any given point in time.
	 * The search starts with the first table
	 * and continues through the active tables
	 * as necessary.
	 */
	for (htp = htab; htp < &htab[MAXHASH]; htp++) {
		if (htp->ht_low == NIL) {
			cp = calloc(2, HASHINC);
			if (cp == -1) {
				yerror("Ran out of memory (hash)");
				pexit(DIED);
			}
			htp->ht_low = cp;
			htp->ht_high = htp->ht_low + HASHINC;
			cp = s;
			if (cp == NIL)
				cp = token;
		}
		h = htp->ht_low + sh;
		/*
		 * quadratic rehash increment
		 * starts at 1 and incremented
		 * by two each rehash.
		 */
		i = 1;
		do {
			if (*h == 0) {
				if (htp->ht_used > (HASHINC * 3)/4)
					break;
				htp->ht_used++;
				if (save != 0) {
					*h = savestr(cp);
				} else
					*h = s;
				return (h);
			}
			sym = *h;
			if (sym < lastkey && sym >= yykey)
				sym = *sym;
			if (sym->pchar == *cp && strcmp(sym, cp) == 0)
				return (h);
			h =+ i;
			i =+ 2;
			if (h >= htp->ht_high)
				h =- HASHINC;
		} while (i < HASHINC);
	}
	yerror("Ran out of hash tables");
	pexit(DIED);
}
