#include "fastipc.h"
#include "defs.h"
#include "proc.h"
#include "spinlock.h"
#include "types.h"

#define FASTIPC_BUFSZ 64 /**< size of the in-kernel message queue */

/**
 * @brief In-kernel queue used to store fast IPC messages.
 *
 * Uses a simple circular buffer (size FASTIPC_BUFSZ) with separate
 * read (r) and write (w) indices.  Access is serialized via a spinlock.
 */
static struct {
    struct spinlock lock;          /**< protects entire buffer */
    zipc_msg_t     buf[FASTIPC_BUFSZ]; /**< message storage */
    uint32_t       r;              /**< next read index */
    uint32_t       w;              /**< next write index */
    int            inited;         /**< set once initlock() has run */
} fastipc;

/**
 * @brief One-time initializer for the fastipc buffer.
 */
static void
fastipc_init(void)
{
    if (!fastipc.inited) {
        initlock(&fastipc.lock, "fastipc");
        fastipc.r = fastipc.w = 0;
        fastipc.inited = 1;
    }
}

/**
 * @brief Enqueue a fast-IPC message (nonblocking).
 *
 * If the buffer is full, the new message is simply dropped.
 *
 * @param m  Pointer to the message to enqueue.
 */
void
fastipc_send(const zipc_msg_t *m)
{
    fastipc_init();
    acquire(&fastipc.lock);
    if ((fastipc.w - fastipc.r) < FASTIPC_BUFSZ) {
        fastipc.buf[fastipc.w % FASTIPC_BUFSZ] = *m;
        fastipc.w++;
    }
    release(&fastipc.lock);
}

/**
 * @brief System call entry point for fast IPC.
 *
 * On x86_64 or AArch64, dequeues one message and returns its
 * four 64-bit words in rsi, rdx, rcx, r8.  If empty, returns -1 in rsi.
 *
 * @return Always 0.
 */
int
sys_ipc_fast(void)
{
#if defined(__x86_64__) || defined(__aarch64__)
    struct proc *p = myproc();

    fastipc_init();
    acquire(&fastipc.lock);

    if (fastipc.r == fastipc.w) {
        // empty → signal with -1 in rsi, zero out others
        p->tf->rsi = (uint64_t)-1;
        p->tf->rdx = 0;
        p->tf->rcx = 0;
        p->tf->r8  = 0;
    } else {
        zipc_msg_t m = fastipc.buf[fastipc.r % FASTIPC_BUFSZ];
        fastipc.r++;
        p->tf->rsi = m.w0;
        p->tf->rdx = m.w1;
        p->tf->rcx = m.w2;
        p->tf->r8  = m.w3;
    }

    release(&fastipc.lock);
#endif
    return 0;
}
