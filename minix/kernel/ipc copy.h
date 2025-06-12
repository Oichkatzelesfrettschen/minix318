#pragma once

#include <stdint.h>

// zero-copy micro-IPC interface
// ISA: x86-64; syscall number 0x30 == ipc_fast
//
// Calling convention:
//   rdi - capability badge
//   rsi - w0
//   rdx - w1
//   rcx - w2
//   r8  - w3
// On return from the syscall, rsi..r8 contain the reply words.

typedef struct {
  uint64_t badge; // capability badge
  uint64_t w0;
  uint64_t w1;
  uint64_t w2;
  uint64_t w3;
} zipc_msg_t;

typedef struct msg_type_desc {
  size_t msg_size; // total message size in bytes
} msg_type_desc;

<<<<<<< HEAD
<<<<<<< HEAD:ipc.h

<<<<<<< HEAD:ipc.h

=======
>>>>>>> 4962afb (Extend endpoint type checking):ipc.h
=======
>>>>>>> ca87b1b (caplib.h)
static inline size_t msg_desc_size(const struct msg_type_desc *d) {
  return d ? d->msg_size : sizeof(zipc_msg_t);
}

<<<<<<< HEAD
<<<<<<< HEAD:ipc.h

=======
>>>>>>> 2840a64 (Fix build issues and add setup script):ipc.h
=======
>>>>>>> 4962afb (Extend endpoint type checking):ipc.h
=======
>>>>>>> ca87b1b (caplib.h)
static inline int zipc_call(zipc_msg_t *m) {
  register uint64_t rdi __asm("rdi") = m->badge;
  register uint64_t rsi __asm("rsi") = m->w0;
  register uint64_t rdx __asm("rdx") = m->w1;
  register uint64_t rcx __asm("rcx") = m->w2;
  register uint64_t r8 __asm("r8") = m->w3;
  __asm__ volatile("syscall"
                   : "+S"(rsi), "+d"(rdx), "+c"(rcx), "+r"(r8)
                   : "a"(0x30), "D"(rdi)
                   : "memory", "r11");
  m->w0 = rsi;
  m->w1 = rdx;
  m->w2 = rcx;
  m->w3 = r8;
  return 0;
}
