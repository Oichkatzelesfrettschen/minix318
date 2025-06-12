#include "fastipc.h"
#include "defs.h"
#include "proc.h"
#include "spinlock.h"
#include "types.h"

#define FASTIPC_BUFSZ 64 /**< size of the in-kernel message queue */

/**
 * @brief In-kernel queue used to store fast IPC messages.
 */
static struct {
  struct spinlock lock;          /**< protects the queue */
  zipc_msg_t buf[FASTIPC_BUFSZ]; /**< circular buffer */
  uint32_t r;                    /**< read index */
  uint32_t w;                    /**< write index */
  int inited;                    /**< initialization flag */
} fastipc;

/**
 * @brief Initialize the fast IPC subsystem on first use.
 */
static void fastipc_init(void) {
  if (!fastipc.inited) {
    initlock(&fastipc.lock, "fastipc");
    fastipc.r = fastipc.w = 0;
    fastipc.inited = 1;
  }
}

/**
 * @brief Enqueue a message in the fast IPC buffer.
 *
 * @param m Message to enqueue.
 */
void fastipc_send(zipc_msg_t *m) {
  fastipc_init();
  acquire(&fastipc.lock);
  if (fastipc.w - fastipc.r < FASTIPC_BUFSZ) {
    fastipc.buf[fastipc.w % FASTIPC_BUFSZ] = *m;
    fastipc.w++;
  }
  release(&fastipc.lock);
}

/**
 * @brief Handle the fast IPC system call.
 *
 * @return Always 0.
 */
int sys_ipc_fast(void) {
#if defined(__x86_64__) || defined(__aarch64__)
  struct proc *p = myproc();
  fastipc_init();
  acquire(&fastipc.lock);
  if (fastipc.r == fastipc.w) {
    p->tf->rsi = (uint64_t)-1;
    p->tf->rdx = p->tf->rcx = p->tf->r8 = 0;
  } else {
    zipc_msg_t m = fastipc.buf[fastipc.r % FASTIPC_BUFSZ];
    fastipc.r++;
    p->tf->rsi = m.w0;
    p->tf->rdx = m.w1;
    p->tf->rcx = m.w2;
    p->tf->r8 = m.w3;
  }
  release(&fastipc.lock);
#endif
  return 0;
}
