// Mutual exclusion spin locks.
// See doc/qspinlock.md for guidelines on using qspinlocks in
// STREAMS and RPC handlers.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#ifndef USE_TICKET_LOCK
#include "qspinlock.h"
#endif

size_t cache_line_size = __alignof__(struct spinlock);

void detect_cache_line_size(void) {
#if defined(__i386__) || defined(__x86_64__)
  uint32_t a, b, c, d;
  cpuid(0x80000006, &a, &b, &c, &d);
  if (c & 0xff)
    cache_line_size = c & 0xff;
#else
  cache_line_size = __alignof__(struct spinlock);
#endif
}

#if CONFIG_SMP
void initlock(struct spinlock *lk, char *name) {
  lk->name = name;
  lk->ticket.head = 0;
  lk->ticket.tail = 0;
  lk->cpu = 0;
}

// Acquire the lock.
// Loops (spins) until the lock is acquired.
// Holding a lock for a long time may cause
// other CPUs to waste time spinning to acquire it.
void acquire(struct spinlock *lk) {
#ifdef USE_TICKET_LOCK
  pushcli(); // disable interrupts to avoid deadlock.
  if (holding(lk))
    panic("acquire");

  uint16_t ticket = __atomic_fetch_add(&lk->ticket.tail, 1, __ATOMIC_SEQ_CST);
  while (__atomic_load_n(&lk->ticket.head, __ATOMIC_SEQ_CST) != ticket)
    ;

  // Tell the C compiler and the processor to not move loads or stores
  // past this point, to ensure that the critical section's memory
  // references happen after the lock is acquired.
  __sync_synchronize();

  // Record info about lock acquisition for debugging.
  lk->cpu = mycpu();
  getcallerpcs(&lk, lk->pcs);
#else
  qspin_lock(lk);
#endif
}

// Release the lock.
void release(struct spinlock *lk) {
#ifdef USE_TICKET_LOCK
  if (!holding(lk))
    panic("release");

  lk->pcs[0] = 0;
  lk->cpu = 0;

  // Tell the C compiler and the processor to not move loads or stores
  // past this point, to ensure that all the stores in the critical
  // section are visible to other cores before the lock is released.
  // Both the C compiler and the hardware may re-order loads and
  // stores; __sync_synchronize() tells them both not to.
  __sync_synchronize();

  __atomic_fetch_add(&lk->ticket.head, 1, __ATOMIC_SEQ_CST);

  popcli();
#else
  qspin_unlock(lk);
#endif
}
#else
void initlock(struct spinlock *lk, char *name) {
  (void)lk;
  (void)name;
}
void acquire(struct spinlock *lk) { (void)lk; }
void release(struct spinlock *lk) { (void)lk; }
#endif

// Record the current call stack in pcs[] by following the %ebp chain.
void getcallerpcs(void *v, uint32_t pcs[]) {
#ifdef __x86_64__
  uintptr_t *rbp;
  int i;

  rbp = (uintptr_t *)v - 2;
  for (i = 0; i < 10; i++) {
    if (rbp == 0 || rbp < (uintptr_t *)KERNBASE ||
        rbp == (uintptr_t *)-1)
      break;
    pcs[i] = rbp[1];               // saved %rip
    rbp = (uintptr_t *)rbp[0]; // saved %rbp
  }
  for (; i < 10; i++)
    pcs[i] = 0;
#else
  uint32_t *ebp;
  int i;

  ebp = (uint32_t *)v - 2;
  for (i = 0; i < 10; i++) {
    if (ebp == 0 || ebp < (uint32_t *)KERNBASE || ebp == (uint32_t *)0xffffffff)
      break;
    pcs[i] = ebp[1];          // saved %eip
    ebp = (uint32_t *)ebp[0]; // saved %ebp
  }
  for (; i < 10; i++)
    pcs[i] = 0;
#endif
}

// Check whether this cpu is holding the lock.
#if CONFIG_SMP
int holding(struct spinlock *lock) {
  int r;
  pushcli();
  r = lock->cpu == mycpu();
  popcli();
  return r;
}
#else
int holding(struct spinlock *lock) {
  (void)lock;
  return 1;
}
#endif

// Pushcli/popcli are like cli/sti except that they are matched:
// it takes two popcli to undo two pushcli.  Also, if interrupts
// are off, then pushcli, popcli leaves them off.

void pushcli(void) {
  int eflags;

  eflags = readeflags();
  cli();
  if (mycpu()->ncli == 0)
    mycpu()->intena = eflags & FL_IF;
  mycpu()->ncli += 1;
}

void popcli(void) {
  if (readeflags() & FL_IF)
    panic("popcli - interruptible");
  if (--mycpu()->ncli < 0)
    panic("popcli");
  if (mycpu()->ncli == 0 && mycpu()->intena)
    sti();
}
