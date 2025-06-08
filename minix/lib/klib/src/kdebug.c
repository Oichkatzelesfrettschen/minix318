/**
 * @file kdebug.c
 * @brief Kernel debugging utilities for klib.
 */

#include <klib.h>  // For KASSERT, kprintf_stub and kpanic declarations
#include <stdarg.h>  // For va_list, va_start, va_end (used by kprintf_stub, kpanic)
#include <sys/kassert.h>  // For KASSERT

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
void hal_console_puts(const char* str) {
  // This is a placeholder. In a real kernel, this would write to a serial port,
  // a VGA buffer, or another early console output mechanism.
  // Since we can't do that here, we'll just acknowledge the call for now.
  // If there was a very early log buffer, one might write to it.
  (void)str;  // Avoid unused parameter warning
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
int kprintf_stub(const char* restrict format, ...) {
  KASSERT(format != NULL);
  if (format ==
      NULL) {  // Should not happen due to KASSERT, but as a safeguard.
    return 0;
  }

  hal_console_puts(format);

  // Return the length of the format string as a proxy for characters printed.
  // A more sophisticated version might try to count characters actually sent.
  k_size_t len = 0;
  const char* p = format;
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
void kpanic(const char* restrict format, ...) __attribute__((noreturn)) {
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
  hal_console_puts("\nSystem halted.\n");  // Add a final message.

  arch_halt();
}
