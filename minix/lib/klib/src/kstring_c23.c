/**
 * @file kstring_c23.c
 * @brief C23-enhanced string length calculation
 *
 * This module provides a C23-enhanced string length function (kstrlen_c23)
 * as part of the MINIX kernel library. The implementation leverages C23
 * language features for enhanced type safety and potential performance
 * optimization.
 * as part of the MINIX kernel library. The implementation leverages C23
 * language features for enhanced type safety and potential performance
 * optimization.
 *
 * Mathematical Properties:
 * Not specified
 *
 * Performance Characteristics:
 * Not specified
 *
 * @author MINIX C23 Development Team
 * @date 2025-06-08
 * @version 1.0
 * @since C23
 */

#include "klib.h" /* Assuming a generic klib header */
/* TODO: Re-evaluate if a more specific header like kernel/klib.h is better */
/* Or if individual klib components should be included directly. */

/* C23 standard headers for specific features */
/* #include <stdbit.h>    */ /* C23 bit manipulation functions (e.g.,
                                stdc_leading_zeros) */
/* #include <stdckdint.h> */ /* C23 checked integer arithmetic (e.g., ckd_add)
                              */
/* #include <stdbit.h>    */ /* C23 bit manipulation functions (e.g.,
                                stdc_leading_zeros) */
/* #include <stdckdint.h> */ /* C23 checked integer arithmetic (e.g., ckd_add)
                              */
// Or directly: #include <stddef.h> for size_t
// And a header for KASSERT, e.g. #include <sys/debug.h> or <kernel/kernel.h>

/**
 * @brief Calculates the length of a null-terminated string.
 *
 * This function counts the number of characters in the string pointed to by 's'
 * up to, but not including, the terminating null character ('\0').
 *
 * Algorithm Complexity: O(n), where n is the length of the string.
 * Memory Requirements: O(1)
 *
 * @param[in] s Pointer to the null-terminated string. Must not be NULL.
 *
 * @return The length of the string (number of characters before the null
 * terminator).
 * @return The length of the string (number of characters before the null
 * terminator).
 *
 * @pre s != NULL
 *
 * @post The number of characters that precede the terminating null character is
 * returned.
 * @post The number of characters that precede the terminating null character is
 * returned.
 *
 * @invariant Not specified
 *
 * @note The behavior is undefined if 's' does not point to a null-terminated
 * string or if 's' is an invalid pointer.
 * @note The behavior is undefined if 's' does not point to a null-terminated
 * string or if 's' is an invalid pointer.
 *
 * @warning Reading from an invalid or non-null-terminated string can lead to
 * crashes.
 * @warning Reading from an invalid or non-null-terminated string can lead to
 * crashes.
 *
 * @see kstrlcpy_c23, kstrcmp_c23
 *
 * @example
 * @code
 * const char *my_string = "Hello";
 * size_t len = kstrlen_c23(my_string);
 * // len will be 5
 * @endcode
 */
size_t kstrlen_c23(const char *s) {
  /*
   * C23 Enhanced Implementation Template
   *
   * This template provides a systematic structure for implementing
   * kernel functions with C23 features, proper error handling,
   * and comprehensive validation.
   */
  /*
   * C23 Enhanced Implementation Template
   *
   * This template provides a systematic structure for implementing
   * kernel functions with C23 features, proper error handling,
   * and comprehensive validation.
   */

  KASSERT(s != NULL, "kstrlen_c23: input string pointer is NULL");
  /* KASSERT(TRUE); */ /* PARAMETER_VALIDATION_2 commented out */
  KASSERT(s != NULL, "kstrlen_c23: input string pointer is NULL");
  /* KASSERT(TRUE); */ /* PARAMETER_VALIDATION_2 commented out */

  /* C23 checked arithmetic for overflow protection */
  /* ;/* No checked arithmetic variables by default */;
  */
      /* if (ckd_add(&result, op1, op2)) { */
      /*     kpanic("kstrlen_c23: arithmetic overflow detected"); */
      /* } */
      /* C23 checked arithmetic for overflow protection */
      /* ;/* No checked arithmetic variables by default */;
  */
      /* if (ckd_add(&result, op1, op2)) { */
      /*     kpanic("kstrlen_c23: arithmetic overflow detected"); */
      /* } */

      /* Hardware capability detection and optimization selection */
      /* Example: Assumes a global kcpu_features struct and specific feature
         flag */
      /* if (g_kcpu_features.feature_flag && 0) { */
      /*     return kstrlen_c23_optimized(s); */ /* Pass 's' if optimized
                                                    function needs it */
      /* } */
      /* Hardware capability detection and optimization selection */
      /* Example: Assumes a global kcpu_features struct and specific feature
         flag */
      /* if (g_kcpu_features.feature_flag && 0) { */
      /*     return kstrlen_c23_optimized(s); */ /* Pass 's' if optimized
                                                    function needs it */
      /* } */

      /* Standard implementation path */
      const char *p = s;
  while (*p) {
    p++;
  }
  /* Standard implementation path */
  const char *p = s;
  while (*p) {
    p++;
  }

  /* Post-condition verification */
  /* KASSERT(TRUE); */ /* Placeholder for KASSERT */
  /* Post-condition verification */
  /* KASSERT(TRUE); */ /* Placeholder for KASSERT */

  return (size_t)(p - s);
  return (size_t)(p - s);
}

/*
Hardware-optimized variant using specific hardware feature
 *
 * This function provides the same interface as kstrlen_c23 but
 * uses hardware-specific optimizations when available.
 *
 * @param const char *s
 * @return size_t
 *
 * @note This function is automatically selected by kstrlen_c23
 *       when appropriate hardware capabilities are detected.
 *       It's typically declared static within the .c file.
 */
/*
static size_t kstrlen_c23_optimized(const char *s) {
    //
    // Hardware-specific implementation using C23 features
    // and detected CPU capabilities
    //
    ;//* Optimized implementation here */
;
// const char *p = s; while (*p) p++; return (size_t)(p-s);
; //* Optimized implementation here */
;
// const char *p = s; while (*p) p++; return (size_t)(p-s);
//}
//*/

/* Unit tests for verification */
/*
#ifdef KLIB_TESTING
/**
 * @brief Comprehensive test suite for kstrlen_c23
 *
 * This test suite verifies correctness, performance characteristics,
 * and edge case handling for the function implementation.
 * These tests would typically reside in a separate test file or a dedicated
 * test section, compiled only when KLIB_TESTING is defined.
 */
// void test_kstrlen_c23(void) {
/* Test normal operation */
; //* Normal operation tests here */;
// void test_kstrlen_c23(void) {
/* Test normal operation */
; //* Normal operation tests here */;

/* Test edge cases */
; //* Edge case tests here */;
/* Test edge cases */
; //* Edge case tests here */;

/* Test error conditions */
; //* Error condition tests here */;
/* Test error conditions */
; //* Error condition tests here */;

/* Performance validation */
; //* Performance tests here */;
/* Performance validation */
; //* Performance tests here */;

/* kprintf("test_kstrlen_c23: All tests passed\n"); */
/* kprintf("test_kstrlen_c23: All tests passed\n"); */
//}
// #endif /* KLIB_TESTING */
// #endif /* KLIB_TESTING */

/**
 * @brief Copies up to 'size-1' characters from the NUL-terminated string 'src'
 *        to 'dest', NUL-terminating the result.
 *
 * This function is a C23-style, safer alternative to `strcpy` or `strncpy`.
 * It guarantees NUL-termination of the destination string if 'size' is not 0.
 * The 'restrict' keyword is used for `dest` and `src` to aid compiler
 * optimization, indicating no aliasing between these pointers.
 * The 'restrict' keyword is used for `dest` and `src` to aid compiler
 * optimization, indicating no aliasing between these pointers.
 *
 * Algorithm Complexity: O(n), where n is min(strlen(src), size-1).
 * Memory Requirements: O(1)
 *
 * @param[out] dest Pointer to the destination buffer. Must not be NULL if size
 * > 0.
 * @param[in]  src  Pointer to the source NUL-terminated string. Must not be
 * NULL.
 * @param[out] dest Pointer to the destination buffer. Must not be NULL if size
 * > 0.
 * @param[in]  src  Pointer to the source NUL-terminated string. Must not be
 * NULL.
 * @param[in]  size The total size of the destination buffer 'dest'.
 *
 * @return The total length of the string that would have been copied if
 *         there was enough space (i.e., strlen(src)). This allows for
 *         detection of truncation.
 *
 * @pre src != NULL.
 * @pre if size > 0, dest != NULL.
 * @post 'dest' will be NUL-terminated if size > 0.
 * @post If the return value is >= size, truncation occurred.
 *
 * @note This function is designed to prevent buffer overflows common with
 * `strcpy`. The behavior is well-defined even if src string is longer than
 * size-1.
 * @note This function is designed to prevent buffer overflows common with
 * `strcpy`. The behavior is well-defined even if src string is longer than
 * size-1.
 *
 * @example
 * @code
 * char destination[10];
 * const char *source = "Hello World";
 * size_t len_copied;
 *
 * len_copied = kstrlcpy_c23(destination, source, sizeof(destination));
 * // destination will contain "Hello Wor" (9 chars + NUL)
 * // len_copied will be 11 (strlen("Hello World"))
 * if (len_copied >= sizeof(destination)) {
 *     // Truncation occurred
 * }
 * @endcode
 */
size_t kstrlcpy_c23(char *restrict dest, const char *restrict src,
                    size_t size) {
  KASSERT(src != NULL, "kstrlcpy_c23: source string pointer is NULL");
  if (size > 0) {
    KASSERT(dest != NULL,
            "kstrlcpy_c23: destination pointer is NULL for non-zero size");
  }
  size_t kstrlcpy_c23(char *restrict dest, const char *restrict src,
                      size_t size) {
    KASSERT(src != NULL, "kstrlcpy_c23: source string pointer is NULL");
    if (size > 0) {
      KASSERT(dest != NULL,
              "kstrlcpy_c23: destination pointer is NULL for non-zero size");
    }

    const char *s_orig = src;
    size_t N = size;
    char *d = dest;
    const char *s_orig = src;
    size_t N = size;
    char *d = dest;

    // Copy as many chars as will fit
    if (N > 0) {
      while (--N > 0) { // Leave space for NUL
        if ((*d++ = *src++) == '\0') {
          break; // Copied NUL, source was shorter than N-1
        }
      }
    }
    // Copy as many chars as will fit
    if (N > 0) {
      while (--N > 0) { // Leave space for NUL
        if ((*d++ = *src++) == '\0') {
          break; // Copied NUL, source was shorter than N-1
        }
      }
    }

    // If N is 0, we ran out of space in 'dest'. NUL-terminate if possible.
    if (N == 0) {
      if (size > 0) {
        *d = '\0'; // NUL-terminate 'dest'
      }
      while (*src++) { // Count remaining chars in 'src' to get total original
                       // length No body needed
      }
    }
    // If N is 0, we ran out of space in 'dest'. NUL-terminate if possible.
    if (N == 0) {
      if (size > 0) {
        *d = '\0'; // NUL-terminate 'dest'
      }
      while (*src++) { // Count remaining chars in 'src' to get total original
                       // length No body needed
      }
    }

    return (
        src - s_orig -
        1); // src points one past NUL or where loop stopped, s_orig is start
    return (
        src - s_orig -
        1); // src points one past NUL or where loop stopped, s_orig is start
  }

  /**
   * @brief Compares two null-terminated strings lexicographically.
   *
   * This function is a C23-style string comparison. It compares the string
   * pointed to by s1 and the string pointed to by s2.
   *
   * Algorithm Complexity: O(n), where n is the length of the shorter string
   *                     if they differ, or length of strings if identical.
   * Memory Requirements: O(1)
   *
   * @param[in] s1 Pointer to the first null-terminated string. Must not be
   * NULL.
   * @param[in] s2 Pointer to the second null-terminated string. Must not be
   * NULL.
   *
   * @return An integer less than, equal to, or greater than zero if s1 is
   * found, respectively, to be less than, to match, or be greater than s2.
   *
   * @pre s1 != NULL
   * @pre s2 != NULL
   * @post The return value indicates the lexicographical relation of s1 to s2.
   *
   * @note Comparison is done using unsigned char values of characters.
   *
   * @example
   * @code
   * int result = kstrcmp_c23("apple", "apply"); // result will be < 0
   * int result2 = kstrcmp_c23("apple", "apple"); // result2 will be == 0
   * int result3 = kstrcmp_c23("banana", "apple"); // result3 will be > 0
   * @endcode
   */
  int kstrcmp_c23(const char *s1, const char *s2) {
    KASSERT(s1 != NULL, "kstrcmp_c23: s1 pointer is NULL");
    KASSERT(s2 != NULL, "kstrcmp_c23: s2 pointer is NULL");

    // Cast to unsigned char pointers for comparison, as per standard strcmp
    // behavior
    const unsigned char *p1 = (const unsigned char *)s1;
    const unsigned char *p2 = (const unsigned char *)s2;

    while (*p1 != '\0') {
      if (*p2 == '\0') {
        return 1; // s1 is longer than s2
      }
      if (*p1 != *p2) {
        return (*p1 - *p2); // Difference found
      }
      p1++;
      p2++;
    }

    if (*p2 != '\0') {
      return -1; // s2 is longer than s1
    }

    return 0; // Strings are equal
  }
  /**
   * @file kstring_c23.c
   * @brief C23-enhanced string length calculation
   *
   * This module provides a C23-enhanced string length function (kstrlen_c23)
   * as part of the MINIX kernel library. The implementation leverages C23
   * language features for enhanced type safety and potential performance
   * optimization.
   *
   * Mathematical Properties:
   * Not specified
   *
   * Performance Characteristics:
   * Not specified
   *
   * @author MINIX C23 Development Team
   * @date 2025-06-08
   * @version 1.0
   * @since C23
   */
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
 * @return An integer less than, equal to, or greater than zero if the first
 * n bytes of s1 (or fewer if a null terminator is encountered) is found,
 *         respectively, to be less than, to match, or be greater than s2.
 */
int kstrncmp_c23(const char *s1, const char *s2, k_size_t n) {
  int kstrncmp_c23(const char *s1, const char *s2, k_size_t n) {
    KASSERT(s1 != NULL);
    KASSERT(s2 != NULL);

    if (n == 0) {
      return 0;
    }

    const unsigned char *p1 = (const unsigned char *)s1;
    const unsigned char *p2 = (const unsigned char *)s2;
    const unsigned char *p1 = (const unsigned char *)s1;
    const unsigned char *p2 = (const unsigned char *)s2;

    k_size_t i = 0;
    while (i < n && *p1 != '\0' && *p1 == *p2) {
      p1++;
      p2++;
      i++;
    }

    if (i == n) {   // Compared n characters, or reached end of one string
                    // while they were equal
      if (i == n) { // Compared n characters, or reached end of one string
                    // while they were equal
        return 0;
      }

      return (int)(*p1) - (int)(*p2);
    }

    /**
     * @brief Copies a string to a fixed-size buffer, ensuring null
     * termination.
     *
     * Copies up to 'size - 1' characters from the NUL-terminated string
     * 'src' to 'dest', NUL-terminating the result.
     *
     * @param dest Pointer to the destination buffer. Must not be NULL.
     * @param src Pointer to the source NUL-terminated string. Must not be
     * NULL.
     * @param size The total size of the destination buffer 'dest'.
     * @return The total length of the string that was attempted to create;
     *         that is the length of 'src'. If the return value is >= size,
     *         truncation occurred.
     * @note This function uses kstrlen_c23 and kmemcpy_c23 internally.
     */
k_size_t kstrlcpy_c23(char *restrict dest, const char *restrict src,
k_size_t kstrlcpy_c23(char *restrict dest, const char *restrict src,
                      k_size_t size) {
      KASSERT(dest != NULL);
      KASSERT(src != NULL);

      k_size_t src_len = kstrlen_c23(src);

      if (size == 0) {
        return src_len; // As per strlcpy behavior, return length of src
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
/**
 * @file kstring_c23.c
 * @brief C23-enhanced string length calculation
 *
 * This module provides a C23-enhanced string length function (kstrlen_c23)
 * as part of the MINIX kernel library. The implementation leverages C23
 * language features for enhanced type safety and potential performance
 * optimization.
 *
 * Mathematical Properties:
 * Not specified
 *
 * Performance Characteristics:
 * Not specified
 *
 * @author MINIX C23 Development Team
 * @date 2025-06-08
 * @version 1.0
 * @since C23
 */

#include "klib.h" /* Assuming a generic klib header */
  /* TODO: Re-evaluate if a more specific header like kernel/klib.h is better */
  /* Or if individual klib components should be included directly. */

  /* C23 standard headers for specific features */
  /* #include <stdbit.h>    */ /* C23 bit manipulation functions (e.g.,
                                  stdc_leading_zeros) */
  /* #include <stdckdint.h> */ /* C23 checked integer arithmetic (e.g., ckd_add)
                                */
  // Or directly: #include <stddef.h> for size_t
  // And a header for KASSERT, e.g. #include <sys/debug.h> or <kernel/kernel.h>

  /**
   * @brief Calculates the length of a null-terminated string.
   *
   * This function counts the number of characters in the string pointed to by
   * 's' up to, but not including, the terminating null character ('\0').
   *
   * Algorithm Complexity: O(n), where n is the length of the string.
   * Memory Requirements: O(1)
   *
   * @param[in] s Pointer to the null-terminated string. Must not be NULL.
   *
   * @return The length of the string (number of characters before the null
   * terminator).
   *
   * @pre s != NULL
   *
   * @post The number of characters that precede the terminating null character
   * is returned.
   *
   * @invariant Not specified
   *
   * @note The behavior is undefined if 's' does not point to a null-terminated
   * string or if 's' is an invalid pointer.
   *
   * @warning Reading from an invalid or non-null-terminated string can lead to
   * crashes.
   *
   * @see kstrlcpy_c23, kstrcmp_c23
   *
   * @example
   * @code
   * const char *my_string = "Hello";
   * size_t len = kstrlen_c23(my_string);
   * // len will be 5
   * @endcode
   */
  size_t kstrlen_c23(const char *s) {
      /*
       * C23 Enhanced Implementation Template
       *
       * This template provides a systematic structure for implementing
       * kernel functions with C23 features, proper error handling,
       * and comprehensive validation.
       */

      KASSERT(s != NULL, "kstrlen_c23: input string pointer is NULL");
      /* KASSERT(TRUE); */ /* PARAMETER_VALIDATION_2 commented out */

      /* C23 checked arithmetic for overflow protection */
      /* ;/* No checked arithmetic variables by default */;
      */
          /* if (ckd_add(&result, op1, op2)) { */
          /*     kpanic("kstrlen_c23: arithmetic overflow detected"); */
          /* } */

          /* Hardware capability detection and optimization selection */
          /* Example: Assumes a global kcpu_features struct and specific
             feature flag */
          /* if (g_kcpu_features.feature_flag && 0) { */
          /*     return kstrlen_c23_optimized(s); */ /* Pass 's' if optimized
                                                        function needs it */
          /* } */

          /* Standard implementation path */
          const char *p = s;
      while (*p) {
        p++;
      }

      /* Post-condition verification */
      /* KASSERT(TRUE); */ /* Placeholder for KASSERT */

      return (size_t)(p - s);
  }

  /*
  Hardware-optimized variant using specific hardware feature
   *
   * This function provides the same interface as kstrlen_c23 but
   * uses hardware-specific optimizations when available.
   *
   * @param const char *s
   * @return size_t
   *
   * @note This function is automatically selected by kstrlen_c23
   *       when appropriate hardware capabilities are detected.
   *       It's typically declared static within the .c file.
   */
  /*
  static size_t kstrlen_c23_optimized(const char *s) {
      //
      // Hardware-specific implementation using C23 features
      // and detected CPU capabilities
      //
      ;//* Optimized implementation here */
  ;
  // const char *p = s; while (*p) p++; return (size_t)(p-s);
  //}
  //*/

  /* Unit tests for verification */
  /*
  #ifdef KLIB_TESTING
  /**
   * @brief Comprehensive test suite for kstrlen_c23
   *
   * This test suite verifies correctness, performance characteristics,
   * and edge case handling for the function implementation.
   * These tests would typically reside in a separate test file or a dedicated
   * test section, compiled only when KLIB_TESTING is defined.
   */
  // void test_kstrlen_c23(void) {
  /* Test normal operation */
  ; //* Normal operation tests here */;

  /* Test edge cases */
  ; //* Edge case tests here */;

  /* Test error conditions */
  ; //* Error condition tests here */;

  /* Performance validation */
  ; //* Performance tests here */;

/* kprintf("test_kstrlen_c23: All tests passed\n"); */
//}
// #endif /* KLIB_TESTING */
