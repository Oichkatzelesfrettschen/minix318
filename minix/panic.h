#ifndef _DDEKIT_PANIC_H
#define _DDEKIT_PANIC_H
#include "ddekit.h"
#include <stdarg.h>

/**
 * \defgroup DDEKit_util DDEKit Utility Functions
 *
 * Miscellaneous helpers for debugging and diagnostics.
 */

/** Panic - print error message and enter the kernel debugger.
 * \ingroup DDEKit_util
 */
void ddekit_panic(char *fmt, ...);

/** Print a debug message.
 * \ingroup DDEKit_util
 */
void ddekit_debug(char *fmt, ...);

#endif
