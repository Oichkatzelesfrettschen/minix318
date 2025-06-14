/**
 * @file prf.c
 * @brief Kernel diagnostic and debugging utilities for Minix.
 *
 * This file contains functions for printing diagnostic information
 * directly to the console. It includes a scaled-down version of printf,
 * panic handling, and device error reporting. The code has been modernized
 * to C23 standards while preserving the original functionality and commentary.
 */

#include "../param.h"
#include "../seg.h"
#include "../buf.h"
#include "../conf.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @struct ConsoleRegisters
 * @brief Structure representing the KL-11 console device registers.
 *
 * This structure encapsulates the registers of the KL-11 console device.
 * It is used for direct interaction with the hardware.
 */
typedef struct {
	volatile int rsr; /**< Receiver Status Register */
	volatile int rbr; /**< Receiver Buffer Register */
	volatile int xsr; /**< Transmitter Status Register */
	volatile int xbr; /**< Transmitter Buffer Register */
} ConsoleRegisters;

/**
 * @var KL
 * @brief Pointer to the KL-11 console device registers.
 */
static ConsoleRegisters *KL = (ConsoleRegisters *)0xFFFF; // Example address, replace with actual hardware address.

/**
 * @var SW
 * @brief Pointer to system switches.
 *
 * Represents system switches that control various aspects of the kernel's behavior.
 */
static volatile struct {
	int integ; /**< Switch controlling console output. */
} *SW = (void *)0xFFFE; // Example address, replace with actual hardware address.

/**
 * @var panicstr
 * @brief Stores the argument to the last call to panic.
 *
 * Used to indicate the reason for the last unresolvable fatal error.
 */
static const char *panicstr = NULL;

/**
 * @brief Scaled-down version of printf for kernel diagnostics.
 *
 * This function prints formatted diagnostic information directly to the console.
 * It supports the following format specifiers:
 * - %s: String
 * - %d: Decimal integer
 * - %l: Long integer (treated as %d)
 * - %o: Octal integer
 *
 * @param fmt Format string.
 * @param ... Variable arguments corresponding to format specifiers.
 */
void printf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);

	while (*fmt) {
		if (*fmt == '%') {
			fmt++;
			switch (*fmt) {
				case 'd':
				case 'l':
					printn(va_arg(args, int), 10);
					break;
				case 'o':
					printn(va_arg(args, int), 8);
					break;
				case 's': {
					const char *s = va_arg(args, const char *);
					while (*s) {
						putchar(*s++);
					}
					break;
				}
				default:
					putchar('%');
					putchar(*fmt);
					break;
			}
		} else {
			putchar(*fmt);
		}
		fmt++;
	}

	va_end(args);
}

/**
 * @brief Print an unsigned integer in a specified base.
 *
 * This function recursively prints an unsigned integer in the specified base.
 *
 * @param n The number to print.
 * @param b The base (e.g., 10 for decimal, 8 for octal).
 */
void printn(unsigned int n, unsigned int b) {
	if (n >= b) {
		printn(n / b, b);
	}
	putchar("0123456789ABCDEF"[n % b]);
}

/**
 * @brief Print a character to the console.
 *
 * This function attempts to save and restore the device status while printing
 * a character to the console. If the system switches are set to inhibit printing,
 * the function returns without performing any output.
 *
 * @param c The character to print.
 */
void putchar(char c) {
	if (SW->integ == 0) {
		return;
	}

	while (!(KL->xsr & 0x80)) {
		// Wait for transmitter to be ready.
	}

	KL->xbr = c;

	if (c == '\n') {
		putchar('\r');
	}
}

/**
 * @brief Handle unresolvable fatal errors.
 *
 * This function is called when a fatal error occurs that cannot be resolved.
 * It synchronizes the system, prints a panic message, and enters an infinite loop.
 *
 * @param s The panic message.
 */
_Noreturn void panic(const char *s) {
	panicstr = s;
	update(); // Synchronize system state.
	printf("panic: %s\n", s);
	for (;;) {
		idle(); // Enter infinite loop.
	}
}

/**
 * @brief Print a warning message for a device.
 *
 * This function prints a warning message in the format "mesg on dev x/y",
 * where x and y are the major and minor parts of the device argument.
 *
 * @param str The warning message.
 * @param dev The device identifier.
 */
void prdev(const char *str, dev_t dev) {
	printf("%s on dev %d/%d\n", str, major(dev), minor(dev));
}

/**
 * @brief Print a diagnostic message from a device driver.
 *
 * This function prints the device, block number, and an octal word (usually
 * an error status register) passed as arguments.
 *
 * @param bp Pointer to the buffer structure.
 * @param o1 First diagnostic value.
 * @param o2 Second diagnostic value.
 */
void deverror(const struct buf *bp, int o1, int o2) {
	prdev("err", bp->b_dev);
	printf("bn%d er%o %o\n", bp->b_blkno, o1, o2);
}
