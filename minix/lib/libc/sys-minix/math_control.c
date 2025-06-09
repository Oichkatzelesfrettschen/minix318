// minix/lib/libc/sys-minix/math_control.c
#include <errno.h> // For ENOSYS, EINVAL, EPERM (used by mathematical_set_mode)

// Internal libc header for shared math capability state and functions
#include "mathematical_internal.h" // Provides __math_state, __mathematical_init, mathematical_set_mode, MATH_MODE_*

// Project-specific public header (for function prototypes being implemented)
#include <minix/capability.h>


/* Enable mathematical verification for this process */
int enable_mathematical_verification(void) {
    // mathematical_set_mode will call __mathematical_init if needed
    return mathematical_set_mode(MATH_MODE_ENABLED);
}

/* Disable mathematical verification for this process (complementary function) */
int disable_mathematical_verification(void) {
    // mathematical_set_mode will call __mathematical_init if needed
    return mathematical_set_mode(MATH_MODE_DISABLED);
}

/* Check if mathematical verification is available and supported by the kernel */
int mathematical_verification_available(void) {
    if (!__math_state.initialized) {
        __mathematical_init(); // Ensures kernel_support is queried
    }
    return __math_state.kernel_support;
}

/*
 * The following functions (mathematical_get_mode, mathematical_was_used)
 * were mentioned in the feedback's test program.
 * They are now part of mathposix.h and should be implemented here
 * as they relate to controlling or querying the math state.
 */

/* Get the current mathematical verification mode for this process */
int mathematical_get_mode(void) {
    if (!__math_state.initialized) {
        __mathematical_init();
    }
    return __math_state.mode;
}

/*
 * Check if mathematical verification was (or would be) used for a given fd.
 * This is a simplified interpretation. A real implementation for an *existing* fd
 * would require the kernel to store and report this state per fd.
 * For now, this function can reflect if mathematical operations *would generally*
 * be applied based on current global state if a new operation were to occur.
 */
int mathematical_was_used(int fd __attribute__((unused))) {
    // (void)fd; // fd is not used in this simplified version.
    if (!__math_state.initialized) {
        __mathematical_init();
    }
    // Returns true if mode is enabled or mandatory AND kernel supports it.
    return (__math_state.mode == MATH_MODE_ENABLED || __math_state.mode == MATH_MODE_MANDATORY) &&
           __math_state.kernel_support;
}
