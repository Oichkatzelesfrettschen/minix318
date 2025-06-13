/**
 * @file call.c
 * @brief Modernized C23 version of the Pascal interpreter code translator call routine.
 *
 * This file handles procedure and function calls by processing argument lists
 * in alignment with modern C23 practices. It performs type checks, handles
 * variable references for pass-by-reference parameters, generates push/pop
 * operations, and ensures correct conversion of argument types.
 *
 * @note Adapted from historical UC Berkeley code (1979), updated for clarity
 * and modern C standards.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

/**
 * @struct nl
 * @brief Symbol table entry for procedures/functions.
 *
 * Contains information to perform code generation for
 * calls, including parameter chains and location/offset
 * data for code emission.
 */
struct nl {
	struct nl *chain;  /**< Next parameter in the chain */
	struct nl *type;   /**< Type of the current symbol */
	const char *symbol;/**< Name of the symbol (procedure/function) */
	int class;         /**< REF (var param) or VAR (value param) */
	int value[2];      /**< Code generation data (location, offset) */
};

/**
 * @enum param_class
 * @brief Parameter classification for clarity.
 */
enum param_class {
	REF = 1,  /**< Var parameter */
	VAR       /**< Value parameter */
};

/* Symbolic constants for demonstration */
#define NIL              NULL
#define T_VAR            100
#define FUNC             2
#define O_PUSH           10
#define O_CALL           20
#define O_POP            30
#define MOD              40
#define DPOFF2           2
#define NL_LOC           0
#define NL_OFFS          1
#define T_VAR_UNDEF      -1

/**
 * @brief Rounds size to an even boundary.
 *
 * @param width The width to round.
 * @return The rounded width.
 */
static inline int even(int width) {
	return (width % 2 != 0) ? width + 1 : width;
}

/**
 * @brief Reports a semantic error.
 *
 * @param format printf-style format string.
 * @param symbol Symbol or description for the error message.
 */
static inline void error(const char *format, const char *symbol) {
	fprintf(stderr, format, symbol);
	fputc('\n', stderr);
}

/**
 * @brief Reports a code generation error.
 *
 * @param format printf-style format string.
 * @param symbol Symbol or description for the error message.
 */
static inline void cerror(const char *format, const char *symbol) {
	fprintf(stderr, format, symbol);
	fputc('\n', stderr);
}

/**
 * @brief Issues a panic message and exits.
 *
 * @param msg Message to display.
 */
static inline void panic(const char *msg) {
	fprintf(stderr, "Panic: %s\n", msg);
	exit(EXIT_FAILURE);
}

/**
 * @brief Emits a two-word instruction.
 *
 * @param opcode Numeric opcode.
 * @param operand Numeric operand.
 */
static inline void put2(int opcode, int operand) {
	/* Stub: would emit code or write to some output */
	(void)opcode;
	(void)operand;
}

/**
 * @brief Stub for symbolic function lvalue analysis.
 *
 * @param node AST node pointer for the lvalue.
 * @param mode Access mode (e.g., MOD).
 * @return Type pointer or NULL on error.
 */
static inline struct nl *lvalue(int *node, int mode) {
	(void)node;
	(void)mode;
	/* Placeholder: returns a type object */
	return NULL;
}

/**
 * @brief Stub for symbolic function rvalue analysis.
 *
 * @param node AST node pointer.
 * @param expected Desired type or NULL on unknown.
 * @return Resolved type or NULL on error.
 */
static inline struct nl *rvalue(int *node, struct nl *expected) {
	(void)node;
	(void)expected;
	return NULL;
}

/**
 * @brief Checks if two types are incompatible.
 *
 * @param actual The actual type.
 * @param expected The expected type.
 * @param node AST node pointer.
 * @return true if incompatible; otherwise false.
 */
static inline bool incompat(struct nl *actual, struct nl *expected, int *node) {
	(void)node;
	return (actual != expected);
}

/**
 * @brief Checks if the given type is among "bcsi" (stub).
 *
 * @param type A type pointer.
 * @param str A string with type classes to check.
 * @return true if matches; otherwise false.
 */
static inline bool isa(struct nl *type, const char *str) {
	(void)type;
	(void)str;
	return false;
}

/**
 * @brief Checks range constraints for a given type.
 *
 * @param formal The formal parameter type.
 * @param actual The actual parameter type.
 */
static inline void rangechk(struct nl *formal, struct nl *actual) {
	(void)formal;
	(void)actual;
	/* Check range constraints here if needed */
}

/**
 * @brief Converts the actual type to the formal type (stub).
 *
 * @param actual Actual type.
 * @param formal Desired formal type.
 */
static inline void convert(struct nl *actual, struct nl *formal) {
	(void)actual;
	(void)formal;
	/* Conversion logic if needed */
}

/**
 * @brief Recursively evaluates leftover arguments to produce rvalues.
 *
 * @param al AST node chain representing arguments.
 */
static void rvlist(int *al) {
	while (al != NIL) {
		(void)rvalue(al + 1, NIL);
		al = (int *)*(al + 2);
	}
}

/**
 * @brief Handles a procedure/function call by generating code for arguments.
 *
 * @param p Symbol table entry for proc/func to call.
 * @param argv Linked list (AST) of arguments.
 * @param porf PROC or FUNC indicator.
 * @param psbn Block number of the procedure/function definition.
 * @return Returns the symbol type if FUNC, otherwise returns NULL.
 */
struct nl *call(struct nl *p, int *argv, int porf, int psbn) {
	if (porf == FUNC) {
		/* Reserve stack space for return value. */
		put2(O_PUSH, even(-1)); /* -width(p->type) placeholder with -1 */
	}

	for (struct nl *p1 = p->chain; p1 != NIL; p1 = p1->chain) {
		if (argv == NIL) {
			error("Not enough arguments to %s", p->symbol);
			return NIL;
		}

		switch (p1->class) {
			case REF: {
				/* Var parameter */
				int *r = (int *)*(argv + 1);
				if (r && (*r != T_VAR)) {
					error("Expression given (variable required) for var parameter %s of %s",
						  p1->symbol, p->symbol);
					break;
				}
				struct nl *q = lvalue((argv + 1), MOD);
				if (!q) {
					break;
				}
				if (q != p1->type) {
					error("Parameter type not identical to type of var parameter %s of %s",
						  p1->symbol, p->symbol);
				}
			} break;

			case VAR: {
				/* Value parameter */
				struct nl *q = rvalue((argv + 1), p1->type);
				if (!q) break;
				if (incompat(q, p1->type, (argv + 1))) {
					cerror("Expression type clashed with type of value parameter %s of %s",
						   p1->symbol, p->symbol);
					break;
				}
				if (isa(p1->type, "bcsi")) {
					rangechk(p1->type, q);
				}
				convert(q, p1->type);
			} break;

			default:
				panic("call");
		}
		argv = (int *)*(argv + 2);
	}

	if (argv != NIL) {
		error("Too many arguments to %s", p->symbol);
		rvlist(argv);
		return NIL;
	}

	/* Emit call and stack cleanup. */
	put2((O_CALL | (psbn << 9)), p->value[NL_LOC]);
	put2(O_POP, p->value[NL_OFFS] - DPOFF2);

	return (porf == FUNC) ? p->type : NIL;
}
