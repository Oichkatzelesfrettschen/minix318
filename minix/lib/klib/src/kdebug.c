/* minix/lib/klib/src/kdebug.c - KLIB Debugging Functions */
#include "klib.h"

/* For va_list, va_start, va_arg, va_end for kprintf/kpanic */
typedef __builtin_va_list k_va_list;
#define k_va_start(v, l) __builtin_va_start(v, l)
#define k_va_end(v) __builtin_va_end(v)
#define k_va_arg(v, l) __builtin_va_arg(v, l)

/*
 * kpanic - Kernel panic function.
 * This function should halt the system or enter a debug state.
 * The exact behavior is highly system-dependent.
 * It should try to print the message to a console or serial port.
 */
void kpanic(const char *fmt, ...) {
  // STUB: For now, just print to kprintf (if available) and loop.
  // A real kpanic would disable interrupts, try to output to a safe
  // console/port, and then halt or reboot.

  char buf[256]; // Static buffer for panic message
  k_va_list args;
  k_va_start(args, fmt);
  // Basic vsnprintf functionality would be needed here.
  // For now, we'll just print the format string if kprintf is simple,
  // or use a very simplified formatter if ksnprintf from kformat.c is
  // available.

  // Assuming ksnprintf is available and basic:
  ksnprintf(
      buf, sizeof(buf), fmt,
      args); // This won't work if ksnprintf itself uses k_va_arg on the same
             // args A proper kvsnprintf is needed. Let's simplify for the stub.
  // Simplified approach for stub:
  // kprintf("KERNEL PANIC: ");
  // kprintf(fmt); // This won't print args yet.
  // We need a kvsprintf or similar.
  // For now, let's assume a very basic kprintf that can take one string.
  kprintf("KERNEL PANIC: ");
  // This is tricky without a full kvsprintf. Let's just print the format string
  // for now. A real implementation would format 'fmt' with 'args' into 'buf'.
  // This is a STUB, real panic messages are crucial.
  if (fmt) {
    kprintf(fmt); // This is not ideal, but a starting point for the stub.
  }
  kprintf("\nHalting system.\n");

  // Halt the system
  // This is architecture-specific. For x86:
  // __asm__ volatile ("cli; hlt");
  // For others, it might be a specific system control register write or a loop.
  volatile int i = 1; // Prevent loop from being optimized out
  while (i) {
  }
}

/*
 * kprintf - Kernel printf function.
 * This is a stub. A real implementation would output to a serial port,
 * a screen buffer, or a kernel message buffer.
 * It needs to be very careful about not causing deadlocks or using too many
 * resources.
 */
void kprintf(const char *fmt, ...) {
  // STUB: This is a placeholder.
  // A real kprintf needs to handle variadic arguments and format them.
  // It would typically write to a predefined kernel console (e.g., serial,
  // VGA). For now, it does nothing to avoid complex dependencies during early
  // KLIB bringup. To make it somewhat useful for very early debugging if
  // serial/VGA is set up: k_va_list args; k_va_start(args, fmt);
  // kvprintf_to_serial(fmt, args); // Assuming a function like this exists
  // k_va_end(args);
  (void)fmt; // Suppress unused parameter warning for stub
}

/*
 * kdebug_print - Debug-only messages.
 * Can be compiled out in release builds.
 */
void kdebug_print(const char *fmt, ...) {
#ifdef KDEBUG
  // STUB: Similar to kprintf, but only for debug builds.
  // k_va_list args;
  // k_va_start(args, fmt);
  // kvprintf_to_serial_debug(fmt, args);
  // k_va_end(args);
  (void)fmt; // Suppress unused parameter warning for stub
#else
  (void)fmt; // Suppress unused parameter warning
#endif
}
