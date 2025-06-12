#pragma once

/**
 * @file whoami.h
 * @brief Legacy environment identifiers.
 *
 * Older BSD sources expect this header to define macros such as @c VAX
 * or @c CORY to mark the build host.  The modern build system no longer
 * uses these identifiers, but the header remains to keep historic code
 * compiling.  Define any required macros externally when building on
 * specialized targets.
 */

/* No default macros provided. */
