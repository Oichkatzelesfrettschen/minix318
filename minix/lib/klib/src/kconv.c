/**
 * @file kconv.c
 * @brief C23 compliant string to number conversion functions for klib.
 */

#include <klib.h> // For KASSERT, k_size_t (though not directly used here), kpanic
#include <limits.h> // For INT_MAX, INT_MIN, ULONG_MAX
#include <stdckdint.h> // For C23 checked integer arithmetic (ckd_add, ckd_sub, ckd_mul)
#include <sys/kassert.h> // For KASSERT (ensure this path is correct for kernel build)

// Helper function to check for whitespace
static inline int is_whitespace(char c) {
  return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' ||
          c == '\r');
}

// Helper function to check for digit
static inline int is_digit(char c) { return (c >= '0' && c <= '9'); }

/**
 * @brief Converts a string to an integer.
 *
 * Parses the string 'str', interpreting its content as an integral number,
 * which is returned as a value of type int. Handles leading whitespace and
 * an optional sign. Uses checked arithmetic to detect overflow.
 *
 * @param str The string to convert. Must not be NULL.
 * @return The converted integer value.
 * @note Calls kpanic on integer overflow.
 */
int katoi_c23(const char *str) {
  KASSERT(str != NULL);

  const char *p = str;
  int result = 0;
  int sign = 1;

  // 1. Skip leading whitespace
  while (is_whitespace(*p)) {
    p++;
  }

  // 2. Handle optional sign
  if (*p == '+') {
    p++;
  } else if (*p == '-') {
    sign = -1;
    p++;
  }

  // 3. Iterate through digits
  while (is_digit(*p)) {
    int digit = *p - '0';

    // Check for overflow before multiplication
    if (ckd_mul(&result, result, 10)) {
      kpanic("katoi_c23: integer overflow during multiplication");
    }

    // Check for overflow before addition/subtraction
    if (sign == 1) {
      if (ckd_add(&result, result, digit)) {
        kpanic("katoi_c23: integer overflow during addition");
      }
    } else { // sign == -1
      if (ckd_sub(&result, result, digit)) {
        kpanic("katoi_c23: integer overflow during subtraction");
      }
    }
    p++;
  }
  // The loop correctly builds up a positive number if sign is -1,
  // which is then negated if needed.
  // However, the standard atoi applies the sign at the end.
  // Let's adjust: build a positive number, then apply sign.

  // Re-evaluation of logic for robust overflow handling with sign.
  // Build as a positive number first, then apply sign, carefully checking
  // bounds.

  result = 0; // Reset result for revised logic
  p = str;    // Reset p
  sign = 1;

  while (is_whitespace(*p)) {
    p++;
  }

  if (*p == '+') {
    p++;
  } else if (*p == '-') {
    sign = -1;
    p++;
  }

  unsigned int temp_result =
      0; // Use unsigned for accumulation to use full positive range

  while (is_digit(*p)) {
    unsigned int digit_val = *p - '0';
    if (ckd_mul(&temp_result, temp_result,
                10U)) { // 10U for unsigned multiplication
      kpanic("katoi_c23: intermediate overflow during multiplication");
    }
    if (ckd_add(&temp_result, temp_result, digit_val)) {
      kpanic("katoi_c23: intermediate overflow during addition");
    }
    p++;
  }

  if (sign == 1) {
    if (temp_result > INT_MAX) {
      kpanic("katoi_c23: positive integer overflow");
    }
    result = (int)temp_result;
  } else { // sign == -1
    if (temp_result >
        (unsigned int)INT_MAX + 1) { // e.g., > 2147483648 for 32-bit int
      kpanic("katoi_c23: negative integer overflow (too small)");
    }
    if (temp_result == (unsigned int)INT_MAX + 1) { // Special case for INT_MIN
      result = INT_MIN;
    } else { // temp_result <= INT_MAX
      result = -(int)temp_result;
    }
  }

  return result;
}

/**
 * @brief Converts a string to an unsigned long integer.
 *
 * Parses the string 'str', interpreting its content as an integral number of
 * the specified 'base', which is returned as a value of type unsigned long.
 * Handles leading whitespace and optional "0x" or "0X" prefix for base 16.
 *
 * @param str The string to convert. Must not be NULL.
 * @param endptr If not NULL, stores the address of the first invalid character
 * in *endptr.
 * @param base The base for conversion (e.g., 10 for decimal, 16 for
 * hexadecimal). If base is 0, the base is determined from the string's prefix
 * (e.g., "0x" for hex). Supported bases are 0, 10, 16.
 * @return The converted unsigned long value.
 * @note Calls kpanic on overflow or invalid base.
 */
unsigned long kstrtoul_c23(const char *restrict str, char **restrict endptr,
                           int base) {
  KASSERT(str != NULL);
  KASSERT(base == 0 || base == 8 || base == 10 ||
          base == 16); // Added base 8 support

  const char *p = str;
  unsigned long result = 0;
  int sign = 1; // strtoul should not process sign, but some versions do.
                // Standard says no. We will follow POSIX standard: initial +/-
                // is not part of the subject sequence.

  // 1. Skip leading whitespace
  while (is_whitespace(*p)) {
    p++;
  }

  // Optional sign is NOT processed by strtoul according to POSIX for the
  // subject sequence itself. Some implementations might allow it and affect
  // what endptr points to. We will strictly not process it as part of the
  // number.

  // 2. Determine base if base is 0 and handle "0x" or "0X" prefix
  if (base == 0) {
    if (*p == '0') {
      p++;
      if (*p == 'x' || *p == 'X') {
        base = 16;
        p++;
      } else {
        base = 8; // Octal if starts with '0'
      }
    } else {
      base = 10; // Default to decimal
    }
  } else if (base == 16) {
    if (*p == '0' && (*(p + 1) == 'x' || *(p + 1) == 'X')) {
      p += 2;
    }
  }

  const char *start_digits = p; // Mark where digits start

  // 3. Iterate through valid digits
  while (1) {
    int digit_val = -1;
    if (is_digit(*p)) {
      digit_val = *p - '0';
    } else if (base == 16 && *p >= 'a' && *p <= 'f') {
      digit_val = 10 + (*p - 'a');
    } else if (base == 16 && *p >= 'A' && *p <= 'F') {
      digit_val = 10 + (*p - 'A');
    }

    if (digit_val == -1 ||
        digit_val >= base) { // Invalid digit for current base
      break;
    }

    // Check for overflow before multiplication
    if (ckd_mul(&result, result, (unsigned long)base)) {
      if (endptr != NULL)
        *endptr = (char *)p; // On overflow, endptr points to offending char
      kpanic("kstrtoul_c23: unsigned long overflow during multiplication");
    }
    // Check for overflow before addition
    if (ckd_add(&result, result, (unsigned long)digit_val)) {
      if (endptr != NULL)
        *endptr = (char *)p; // On overflow, endptr points to offending char
      kpanic("kstrtoul_c23: unsigned long overflow during addition");
    }
    p++;
  }

  if (endptr != NULL) {
    if (p == start_digits) { // No valid digits found
      *endptr = (char *)str; // As per spec, if no conversion, *endptr is str
    } else {
      *endptr = (char *)p;
    }
  }

  return result;
}
/* minix/lib/klib/src/kconv.c - KLIB String to Number Conversion */
#include "klib.h"
#ifndef INT_MAX
#define INT_MAX ((kint_t)0x7FFFFFFF)
#endif
#ifndef INT_MIN
#define K_INT_MIN_MAGNITUDE ((ku32_t)INT_MAX + 1)
#define INT_MIN ((kint_t)0x80000000)
#endif
#ifndef ULONG_MAX
#define ULONG_MAX ((ku64_t)0xFFFFFFFFFFFFFFFFULL)
#endif
static kbool_t is_space(char c) {
  return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' ||
          c == '\r');
}
static kbool_t is_digit(char c) { return (c >= '0' && c <= '9'); }
kint_t katoi(const char *str) {
  KASSERT(str != KNULL);
  ku32_t r = 0;
  kint_t s = 1;
  const char *p = str;
  while (is_space(*p))
    p++;
  if (*p == '-') {
    s = -1;
    p++;
  } else if (*p == '+')
    p++;
  while (is_digit(*p)) {
    kint_t d = *p - '0';
    if (s == 1) {
      if (r > ((ku32_t)INT_MAX - d) / 10) {
        kpanic("katoi: overflow");
        return INT_MAX;
      }
    } else {
      if (r > K_INT_MIN_MAGNITUDE / 10 ||
          (r == K_INT_MIN_MAGNITUDE / 10 &&
           (ku32_t)d > K_INT_MIN_MAGNITUDE % 10)) {
        kpanic("katoi: underflow");
        return INT_MIN;
      }
    }
    r = r * 10 + d;
    p++;
  }
  return (s == 1) ? (kint_t)r : -(kint_t)r;
}
ku64_t kstrtoul(const char *str, char **e, kint_t b) {
  KASSERT(str != KNULL);
  KASSERT(b == 0 || (b >= 2 && b <= 36));
  ku64_t r = 0;
  kint_t s = 1;
  const char *p = str;
  const char *sp;
  while (is_space(*p))
    p++;
  sp = p;
  if (*p == '-') {
    s = -1;
    p++;
  } else if (*p == '+')
    p++;
  if (b == 0) {
    if (*p == '0') {
      if (p[1] == 'x' || p[1] == 'X') {
        if ((p[2] >= '0' && p[2] <= '9') || (p[2] >= 'a' && p[2] <= 'f') ||
            (p[2] >= 'A' && p[2] <= 'F')) {
          b = 16;
          p += 2;
        } else
          b = 10;
      } else if (p[1] >= '0' && p[1] <= '7')
        b = 8;
      else
        b = 10;
    } else
      b = 10;
  } else if (b == 16) {
    if (*p == '0' && (p[1] == 'x' || p[1] == 'X')) {
      if ((p[2] >= '0' && p[2] <= '9') || (p[2] >= 'a' && p[2] <= 'f') ||
          (p[2] >= 'A' && p[2] <= 'F'))
        p += 2;
    }
  }
  const char *dsp = p;
  kint_t d;
  while (*p) {
    if (is_digit(*p))
      d = *p - '0';
    else if (*p >= 'a' && *p <= 'z')
      d = *p - 'a' + 10;
    else if (*p >= 'A' && *p <= 'Z')
      d = *p - 'A' + 10;
    else
      break;
    if (d >= b)
      break;
    if (r > (ULONG_MAX - (ku64_t)d) / (ku64_t)b) {
      if (e)
        *e = (char *)p;
      kpanic("kstrtoul: overflow");
      return ULONG_MAX;
    }
    r = r * b + d;
    p++;
  }
  if (e) {
    if (p == dsp)
      *e = (char *)sp;
    else
      *e = (char *)p;
  }
  if (s == -1 && r != 0)
    return (~r + 1);
  return r;
}
