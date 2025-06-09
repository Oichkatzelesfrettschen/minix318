/**
 * @file kdebug.c
 * @brief Kernel debugging utilities for klib.
 */

#include <klib.h> // For KASSERT, kprintf_stub and kpanic declarations
#include <stdarg.h> // For va_list, va_start, va_end (used by kprintf_stub, kpanic)
#include <stddef.h>      // For NULL definition
#include <sys/kassert.h> // For KASSERT

// --- Architecture/HAL Specific Placeholders ---
// These would typically be implemented in architecture-specific code (e.g.,
// arch/i386/arch_debug.c) or a Hardware Abstraction Layer.

/**
 * @brief Placeholder for disabling all maskable interrupts.
 * @note Actual implementation is architecture-specific.
 */
void arch_disable_interrupts(void) {
  // Example: __asm__ volatile ("cli" ::: "memory"); // For x86
  // For now, it's a no-op.
}

/**
 * @brief Placeholder for halting the CPU.
 * @note Actual implementation is architecture-specific. This function should
 * not return.
 */
void arch_halt(void) __attribute__((noreturn)) {
  // Example: for(;;) { __asm__ volatile ("hlt"); } // For x86
  // For now, it enters an infinite loop.
  for (;;) {
    // Loop indefinitely
  }
}

/**
 * @brief Placeholder for HAL-level function to print a string to the console.
 * @note Actual implementation is hardware and architecture-specific.
 *       This is a very simplified placeholder. A real implementation would
 *       need to handle serial ports, video buffers, etc.
 * @param str The null-terminated string to print.
 */
void hal_console_puts(const char *str) {
  // This is a placeholder. In a real kernel, this would write to a serial port,
  // a VGA buffer, or another early console output mechanism.
  // Since we can't do that here, we'll just acknowledge the call for now.
  // If there was a very early log buffer, one might write to it.
  (void)str; // Avoid unused parameter warning
}

// --- klib Debug Function Implementations ---

/**
 * @brief Kernel printf stub for early debug messages.
 *
 * This function provides a basic printing capability for use before the
 * full kprintf is available. This initial stub version prints only the
 * format string itself and does not parse variadic arguments.
 *
 * @param format The format string. Must not be NULL.
 * @param ... Variadic arguments (currently ignored by this stub).
 * @return The number of characters in the format string (notionally printed),
 * or 0 if format is NULL.
 */
int kprintf_stub(const char *restrict format, ...) {
  KASSERT(format != NULL);
  if (format == NULL) { // Should not happen due to KASSERT, but as a safeguard.
    return 0;
  }

  hal_console_puts(format);

  // Return the length of the format string as a proxy for characters printed.
  // A more sophisticated version might try to count characters actually sent.
  size_t len = 0;
  const char *p = format;
  while (*p != '\0') {
    len++;
    p++;
  }
  return (int)len;
}

/**
 * @brief Halts the system after printing a panic message.
 *
 * This function is called when an unrecoverable kernel error occurs.
 * It disables interrupts, prints a panic message prefixed with "KERNEL PANIC:
 * ", and then halts the system. This function does not return.
 *
 * @param format The format string for the panic message. Must not be NULL.
 * @param ... Variadic arguments for the format string (passed to kprintf_stub).
 */
void kpanic(const char *restrict format, ...) __attribute__((noreturn)) {
  KASSERT(format != NULL);

  arch_disable_interrupts();

  // Print "KERNEL PANIC: " prefix first
  hal_console_puts("KERNEL PANIC: ");

  // Then print the formatted message (limited by kprintf_stub's current
  // capabilities) We can't use vsnprintf here as it's too complex for a stub.
  // A simple approach is to just print the format string.
  // A slightly more advanced stub could handle a few %s, %d, %p if needed,
  // but for now, kprintf_stub itself just prints the format string.

  // To pass variadic args to kprintf_stub (even if it ignores them),
  // we'd ideally use a vprintf-style version of kprintf_stub.
  // For now, we'll just pass the format string directly.
  // If kprintf_stub were to handle va_args, it would look like:
  // va_list args;
  // va_start(args, format);
  // vkprintf_stub(format, args); // Assuming a vkprintf_stub exists
  // va_end(args);
  // Since kprintf_stub just prints the format string, this is sufficient:
  kprintf_stub(format);
  hal_console_puts("\nSystem halted.\n"); // Add a final message.

  arch_halt();
}
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
