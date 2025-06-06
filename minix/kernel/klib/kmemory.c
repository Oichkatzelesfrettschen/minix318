#include "kmemory.h" // Assuming klib/include will be an include path for klib itself
#include <minix/kernel_types.h> // For k_size_t

/*
 * Implementation of kernel-space memory manipulation functions.
 */

void *kmemcpy(void *dest, const void *src, k_size_t n) {
    if (dest == NULL || src == NULL) {
        return dest; // Or handle error appropriately for kernel
    }
    // Use compiler builtin for potentially optimal code generation.
    return __builtin_memcpy(dest, src, n);
}

void *kmemset(void *s, int c, k_size_t n) {
    if (s == NULL) {
        return s; // Or handle error appropriately
    }
    // Use compiler builtin for potentially optimal code generation.
    return __builtin_memset(s, c, n);
}

void *kmemmove(void *dest, const void *src, k_size_t n) {
    if (dest == NULL || src == NULL || n == 0) {
        return dest;
    }

    unsigned char *pd = (unsigned char *)dest;
    const unsigned char *ps = (const unsigned char *)src;

    // Check for overlap and copy direction
    if (pd < ps) {
        // Copy forwards (standard memcpy)
        return __builtin_memcpy(dest, src, n);
    } else if (pd > ps) {
        // Copy backwards
        // Check if regions overlap and dest is after src
        // (pd > ps) means dest starts after src.
        // If (ps + n > pd), then the end of src overlaps with the start of dest.
        if ((ps + n) > pd) { // Overlap condition where backwards copy is needed
            for (k_size_t i = n; i > 0; i--) {
                pd[i-1] = ps[i-1];
            }
        } else {
            // No overlap even if dest > src, so forward copy is fine
            return __builtin_memcpy(dest, src, n);
        }
    }
    // else (pd == ps), no copy needed or already handled by n==0

    return dest;
}
