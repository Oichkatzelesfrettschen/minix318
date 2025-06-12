#pragma once
#include "types.h"
#include "exo.h"

/*
 * Exokernel CPU interface used by user-level schedulers.
 *
 * exo_swtch(old, new) wraps the low-level swtch() primitive.  Both
 * arguments must point to a context_t structure describing the saved
 * callee-saved registers and stack pointer.  The caller must preserve
 * caller-saved registers according to the System V ABI.  On return,
 * execution resumes on the context pointed to by *old.
 */

#ifndef EXO_CONTEXT_T
#define EXO_CONTEXT_T
#if defined(__x86_64__)
struct context64 {
  uint64 r15;
  uint64 r14;
  uint64 r13;
  uint64 r12;
  uint64 rbx;
  uint64 rbp;
  uint64 rip;
};
typedef struct context64 context_t;
#elif defined(__aarch64__)
struct context64 {
  uint64 x19;
  uint64 x20;
  uint64 x21;
  uint64 x22;
  uint64 x23;
  uint64 x24;
  uint64 x25;
  uint64 x26;
  uint64 x27;
  uint64 x28;
  uint64 fp;
  uint64 lr;
};
typedef struct context64 context_t;
#else
struct context {
  uint edi;
  uint esi;
  uint ebx;
  uint ebp;
  uint eip;
};
typedef struct context context_t;
#endif
#endif /* EXO_CONTEXT_T */

void swtch(context_t **old, context_t *new);
static inline void cap_yield(context_t **old, context_t *target) {
  swtch(old, target);
}

int exo_yield_to(exo_cap target);
