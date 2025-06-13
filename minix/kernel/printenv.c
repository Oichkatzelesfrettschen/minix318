/**
 * @file printenv.c
 * @brief Modernized environment variable printer for MINIX kernel utilities.
 *
 * This file provides a robust, idiomatic, and fully C23-compliant implementation
 * of a utility to print selected environment variables, with special handling for
 * the TERM variable, including termcap database lookup and extraction of the
 * terminal's long name. The code is exhaustively documented, decomposed, and
 * annotated for clarity, maintainability, and academic rigor.
 *
 * Original code and comments are preserved and expanded upon.
 *
 * @author (Original: MINIX team, Modernization: GitHub Copilot)
 * @date 2024
 */

#include <stdio.h>      // For printf, fprintf
#include <stdlib.h>     // For getenv, exit, EXIT_SUCCESS, EXIT_FAILURE
#include <string.h>     // For strcmp, strnlen, strncpy
#include <stdbool.h>    // For bool type
#include <assert.h>     // For assert
#include <errno.h>      // For errno
#include <stdnoreturn.h>// For noreturn
#include <stddef.h>     // For size_t

// If termcap is available, include it; otherwise, stub out functionality.
#if __has_include(<termcap.h>)
#  include <termcap.h>
#else
// Stub definitions for systems without termcap.
static inline int tgetent(char *buf, const char *termtype) { (void)buf; (void)termtype; return -1; }
#endif

/**
 * @def MAX_TERMCAP_BUF
 * @brief Maximum buffer size for termcap entry.
 *
 * This value is chosen to be sufficiently large for typical termcap entries.
 */
#define MAX_TERMCAP_BUF 1024

/**
 * @brief Prints the value of a specified environment variable.
 *
 * For the "TERM" variable, attempts to look up the terminal's long name
 * using the termcap database, if available.
 *
 * @param varname Name of the environment variable to print.
 */
static void print_env_var(const char *varname);

/**
 * @brief Extracts the long name from a termcap entry buffer.
 *
 * The long name is defined as the last name in the colon-separated list
 * before the first colon, skipping any aliases separated by '|'.
 *
 * @param termcap_buf The buffer containing the termcap entry.
 * @return Pointer to the start of the long name within the buffer.
 */
static char *extract_longname(char *termcap_buf);

/**
 * @brief Entry point for the printenv utility.
 *
 * Prints the values of specified environment variables, or defaults to
 * "HOME" and "TERM" if none are specified.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on error.
 */
int main(int argc, char *argv[])
{
	// Advance past program name.
	argc--;
	argv++;

	if (argc > 0) {
		// Print each specified environment variable.
		for (int i = 0; i < argc; ++i) {
			print_env_var(argv[i]);
		}
	} else {
		// Default: print HOME and TERM.
		print_env_var("HOME");
		print_env_var("TERM");
	}
	return EXIT_SUCCESS;
}

/**
 * @brief Prints the value of a specified environment variable.
 *
 * For "TERM", attempts to retrieve the terminal's long name using termcap.
 * For "HOME", prints the home directory.
 * For other variables, prints their value if set, or a message if unset.
 *
 * @param varname Name of the environment variable to print.
 */
static void print_env_var(const char *varname)
{
	assert(varname != NULL);

	if (strcmp(varname, "TERM") == 0) {
		char termcap_buf[MAX_TERMCAP_BUF] = {0};
		const char *term_env = getenv("TERM");
		if (term_env == NULL) {
			printf("TERM is not set in the environment.\n");
			return;
		}
#if __has_include(<termcap.h>)
		int tcap_status = tgetent(termcap_buf, term_env);
		if (tcap_status > 0) {
			char *longname = extract_longname(termcap_buf);
			printf("TERM=%s\n", longname);
		} else if (tcap_status == 0) {
			printf("TERM=%s (no termcap entry found)\n", term_env);
		} else {
			printf("TERM=%s (termcap error)\n", term_env);
		}
#else
		// If termcap is not available, just print the TERM value.
		printf("TERM=%s (termcap unavailable)\n", term_env);
#endif
	} else if (strcmp(varname, "HOME") == 0) {
		const char *home_env = getenv("HOME");
		if (home_env) {
			printf("HOME=%s\n", home_env);
		} else {
			printf("HOME is not set in the environment.\n");
		}
	} else {
		// General case: print any environment variable.
		const char *value = getenv(varname);
		if (value) {
			printf("%s=%s\n", varname, value);
		} else {
			printf("%s is not set in the environment.\n", varname);
		}
	}
}

/**
 * @brief Extracts the long name from a termcap entry buffer.
 *
 * The termcap entry format is:
 *   name1|name2|longname:capabilities...
 * This function returns a pointer to the start of the long name,
 * and null-terminates it in-place.
 *
 * @param termcap_buf The buffer containing the termcap entry.
 * @return Pointer to the start of the long name within the buffer.
 *
 * @note This function modifies the input buffer in-place.
 */
static char *extract_longname(char *termcap_buf)
{
	assert(termcap_buf != NULL);

	char *start = termcap_buf;
	char *last_pipe = start;
	char *p = start;

	// Find the last '|' before the first ':'.
	while (*p && *p != ':') {
		if (*p == '|') {
			last_pipe = p + 1;
		}
		p++;
	}
	// Null-terminate at the first ':'.
	if (*p == ':') {
		*p = '\0';
	}
	return last_pipe;
}
