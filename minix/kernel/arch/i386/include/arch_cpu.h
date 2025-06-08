/**
 * @file arch_cpu.h
 * @brief i386/x86_64 architecture-specific CPU operations.
 *
 * This file contains inline functions for CPU operations specific to the
 * i386 and x86_64 architectures, such as the PAUSE instruction for spin-loops.
 */
#ifndef ARCH_I386_CPU_H
#define ARCH_I386_CPU_H

#if defined(__i386__) || defined(__x86_64__)
/**
 * @brief Issues the PAUSE instruction (x86 specific).
 *
 * The PAUSE instruction is a hint to the CPU that the current code path
 * is a spin-wait loop. On hyper-threaded processors, this can improve
 * performance by reducing resource contention between hyperthreads and
 * save power by briefly halting execution in the spinning thread.
 * It also helps prevent memory order violations that can occur in
 * tight spin loops on some out-of-order processors.
 */
static inline void arch_pause(void) {
  // 'pause' instruction: informs the CPU this is a spin-wait loop.
  // It's a hint that can improve performance and power efficiency.
  // 'volatile' ensures the instruction is not optimized away by the compiler.
  asm volatile("pause");
}
#else
/**
 * @brief Placeholder for arch_pause() on non-x86 architectures.
 *
 * This function serves as a compile-time placeholder if this header were
 * included on a non-x86 architecture. For those platforms, a specific
 * implementation (if any equivalent instruction exists) or a true no-op
 * would be provided by their respective architecture-specific headers.
 */
static inline void arch_pause(void) {
  /* No-op for non-x86 architectures in this specific header.
   * A more generic system might have a centralized way to define
   * arch_pause() that defaults to no-op if not implemented by the arch.
   */
}
#endif /* __i386__ || __x86_64__ */

#endif /* ARCH_I386_CPU_H */
