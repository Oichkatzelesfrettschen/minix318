/**
 * @file fib_big_demo.c
 * @brief Demonstration program for large Fibonacci number calculation
 * @author GitHub Copilot
 * @date 2024
 */

#include <stdio.h>
#include <stdint.h>
#include "math_core.h"
#include "user.h"

/**
 * @brief Prints a 256-bit integer in hexadecimal format
 * @param val The 256-bit value to print
 */
#ifdef __BITINT_MAXWIDTH__
static void print_uint256_hex(uint256_t val) {
  printf("0x");
  for (int i = 3; i >= 0; i--) {
    uint64_t part = (uint64_t)(val >> (i * 64));
    printf("%016llx", (unsigned long long)part);
  }
  printf("\n");
}
#endif

/**
 * @brief Main program entry point
 * @return Exit status (0 for success)
 */
int main(void) {
  const unsigned n = 100;
  
  printf("Computing Fibonacci number F(%u):\n", n);
  
#ifdef __BITINT_MAXWIDTH__
  uint256_t val = fib_big(n);
  printf("fib_big(%u) = ", n);
  print_uint256_hex(val);
#else
  uint64_t val = fib_big(n);
  printf("fib_big(%u) = %llu (64-bit fallback)\n", n, 
       (unsigned long long)val);
#endif
  
  return 0;
}
