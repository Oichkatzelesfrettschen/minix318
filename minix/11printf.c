/* Copyright (c) 1979 Regents of the University of California */
#ifdef TRACE
#include <stdio.h>
#undef putchar
#endif

/**
 * @brief Legacy wrapper around _doprnt.
 *
 * This function predates ANSI prototypes and mimics the historical
 * ``printf`` implementation. It simply forwards the format string and
 * argument list to ``_doprnt``.
 */
void printf(char *fmt, int args) { _doprnt(fmt, &args, 0); }

/**
 * @brief Output a formatted string with padding.
 *
 * @param string  The string to output.
 * @param count   Number of characters to emit.
 * @param adjust  Padding adjustment value.
 * @param file    Optional output file handle.
 * @param fillch  Padding character.
 */
static void _strout(
    char *string, int count, int adjust, struct { int a; } * file, int fillch) {

  while (adjust < 0) {
    if (*string == '-' && fillch == '0') {
#ifdef TRACE
      if (file)
        putc(*string++, file);
      else
#endif
        putchar(*string++);
      count--;
    }
#ifdef TRACE
    if (file)
      putc(fillch, file);
    else
#endif
      putchar(fillch);
    adjust++;
  }
  while (--count >= 0)
#ifdef TRACE
    if (file)
      putc(*string++, file);
    else
#endif
      putchar(*string++);
  while (adjust) {
#ifdef TRACE
    if (file)
      putc(fillch, file);
    else
#endif
      putchar(fillch);
    adjust--;
  }
}
