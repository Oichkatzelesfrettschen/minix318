/**
 * @file kstring_c23.c
 * @brief C23 compliant string manipulation functions for klib.
 */

#include <klib.h>        // For k_size_t, function declarations
#include <sys/kassert.h> // For KASSERT

/**
 * @brief Calculates the length of a null-terminated string.
 *
 * @param s Pointer to the null-terminated string. Must not be NULL.
 * @return The number of bytes in the string, excluding the null terminator.
 */
k_size_t kstrlen_c23(const char *s) {
  KASSERT(s != NULL);
  const char *p = s;
  while (*p != '\0') {
    p++;
  }
  return (k_size_t)(p - s);
}

/**
 * @brief Compares two null-terminated strings lexicographically.
 *
 * @param s1 Pointer to the first null-terminated string. Must not be NULL.
 * @param s2 Pointer to the second null-terminated string. Must not be NULL.
 * @return An integer less than, equal to, or greater than zero if s1 is found,
 *         respectively, to be less than, to match, or be greater than s2.
 */
int kstrcmp_c23(const char *s1, const char *s2) {
  KASSERT(s1 != NULL);
  KASSERT(s2 != NULL);

  const unsigned char *p1 = (const unsigned char *)s1;
  const unsigned char *p2 = (const unsigned char *)s2;

  while (*p1 != '\0' && *p1 == *p2) {
    p1++;
    p2++;
  }
  return (int)(*p1) - (int)(*p2);
}

/**
 * @brief Compares up to n characters of two null-terminated strings
 * lexicographically.
 *
 * @param s1 Pointer to the first null-terminated string. Must not be NULL.
 * @param s2 Pointer to the second null-terminated string. Must not be NULL.
 * @param n Maximum number of characters to compare.
 * @return An integer less than, equal to, or greater than zero if the first n
 *         bytes of s1 (or fewer if a null terminator is encountered) is found,
 *         respectively, to be less than, to match, or be greater than s2.
 */
int kstrncmp_c23(const char *s1, const char *s2, k_size_t n) {
  KASSERT(s1 != NULL);
  KASSERT(s2 != NULL);

  if (n == 0) {
    return 0;
  }

  const unsigned char *p1 = (const unsigned char *)s1;
  const unsigned char *p2 = (const unsigned char *)s2;

  k_size_t i = 0;
  while (i < n && *p1 != '\0' && *p1 == *p2) {
    p1++;
    p2++;
    i++;
  }

  if (i == n) { // Compared n characters, or reached end of one string while
                // they were equal
    return 0;
  }

  return (int)(*p1) - (int)(*p2);
}

/**
 * @brief Copies a string to a fixed-size buffer, ensuring null termination.
 *
 * Copies up to 'size - 1' characters from the NUL-terminated string 'src'
 * to 'dest', NUL-terminating the result.
 *
 * @param dest Pointer to the destination buffer. Must not be NULL.
 * @param src Pointer to the source NUL-terminated string. Must not be NULL.
 * @param size The total size of the destination buffer 'dest'.
 * @return The total length of the string that was attempted to create;
 *         that is the length of 'src'. If the return value is >= size,
 *         truncation occurred.
 * @note This function uses kstrlen_c23 and kmemcpy_c23 internally.
 */
k_size_t kstrlcpy_c23(char *restrict dest, const char *restrict src,
                      k_size_t size) {
  KASSERT(dest != NULL);
  KASSERT(src != NULL);

  k_size_t src_len = kstrlen_c23(src);

  if (size == 0) {
    return src_len; // As per strlcpy behavior, return length of src
  }

  k_size_t copy_len;
  if (src_len < size - 1) {
    copy_len = src_len;
  } else {
    copy_len = size - 1;
  }

  kmemcpy_c23(dest, src, copy_len);
  dest[copy_len] = '\0';

  return src_len; // Return the original length of src
}
