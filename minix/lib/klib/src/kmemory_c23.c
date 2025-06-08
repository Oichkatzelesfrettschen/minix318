/**
 * @file kmemory_c23.c
 * @brief C23 compliant memory manipulation functions for klib.
 */

#include <klib.h>         // For k_size_t, function declarations
#include <sys/kassert.h>  // For KASSERT

/**
 * @brief Sets the first n bytes of the memory area pointed to by s to the
 * specified value c.
 *
 * @param s Pointer to the memory area. Must not be NULL.
 * @param c Value to be set (converted to unsigned char).
 * @param n Number of bytes to be set.
 * @return A pointer to the memory area s.
 */
void* kmemset_c23(void* s, int c, k_size_t n) {
  KASSERT(s != NULL);
  unsigned char* p = (unsigned char*)s;
  unsigned char val = (unsigned char)c;

  for (k_size_t i = 0; i < n; ++i) {
    p[i] = val;
  }
  return s;
}

/**
 * @brief Copies n bytes from memory area src to memory area dest.
 *
 * The memory areas may overlap; the copy is done in a non-destructive manner
 * similar to memmove. Uses 'restrict' keyword as per C23 for optimization
 * hints, although for the overlap handling, the restrict qualifiers mainly
 * apply to non-overlapping scenarios for the compiler if it can prove it. The
 * explicit overlap check dominates the logic here.
 *
 * @param dest Pointer to the destination memory area. Must not be NULL.
 * @param src Pointer to the source memory area. Must not be NULL.
 * @param n Number of bytes to copy.
 * @return A pointer to the destination memory area dest.
 */
void* kmemcpy_c23(void* restrict dest, const void* restrict src, k_size_t n) {
  KASSERT(dest != NULL);
  KASSERT(src != NULL);

  unsigned char* d = (unsigned char*)dest;
  const unsigned char* s = (const unsigned char*)src;

  if (n == 0) {
    return dest;
  }

  // Check for overlap and determine copy direction
  // If dest is within src and src < dest < src + n, they overlap.
  // Or if src is within dest and dest < src < dest + n, they overlap.
  // A simple check: if dest > src and there's overlap, copy backwards.
  // Otherwise, copy forwards.
  if (d > s && d < s + n) {
    // Overlap, and dest is ahead of src: copy backwards
    for (k_size_t i = n; i > 0; --i) {
      d[i - 1] = s[i - 1];
    }
  } else {
    // No overlap, or dest is behind src: copy forwards
    for (k_size_t i = 0; i < n; ++i) {
      d[i] = s[i];
    }
  }
  return dest;
}

/**
 * @brief Compares the first n bytes of the memory areas s1 and s2.
 *
 * @param s1 Pointer to the first memory area. Must not be NULL.
 * @param s2 Pointer to the second memory area. Must not be NULL.
 * @param n Number of bytes to compare.
 * @return An integer less than, equal to, or greater than zero if the first n
 *         bytes of s1 is found, respectively, to be less than, to match, or be
 *         greater than the first n bytes of s2.
 */
int kmemcmp_c23(const void* s1, const void* s2, k_size_t n) {
  KASSERT(s1 != NULL);
  KASSERT(s2 != NULL);

  const unsigned char* p1 = (const unsigned char*)s1;
  const unsigned char* p2 = (const unsigned char*)s2;

  if (n == 0) {
    return 0;
  }

  for (k_size_t i = 0; i < n; ++i) {
    if (p1[i] != p2[i]) {
      return (int)p1[i] - (int)p2[i];
    }
  }
  return 0;
}
