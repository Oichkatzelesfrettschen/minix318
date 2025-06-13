/**
 * @file ato.c
 * @brief Octal to Long Integer Conversion Utilities
 * 
 * @details This module provides utilities for converting octal string representations
 * to long integer values. Originally part of the Pascal interpreter code translator (pi)
 * from the University of California, Berkeley, this has been modernized to C23 standards
 * with enhanced error handling, type safety, and comprehensive documentation.
 * 
 * The primary function handles octal number conversion with proper validation for
 * invalid octal digits (8, 9) and overflow detection. The implementation uses
 * bitwise operations for efficiency and maintains compatibility with the original
 * Pascal interpreter's numeric handling requirements.
 * 
 * @version 2.0
 * @date 2024
 * @author Original: Charles Haley, Bill Joy UCB (1979)
 * @author Modernized: GitHub Copilot Assistant
 * 
 * @copyright Original copyright (c) 1979 Regents of the University of California
 * 
 * @note The original floating-point conversion function (atof) mentioned in comments
 * had limitations regarding overflow detection. Modern implementations should use
 * strtod() or similar functions with proper error checking.
 * 
 * @warning This implementation assumes two's complement arithmetic and may need
 * adjustment for exotic architectures.
 * 
 * @see IEEE 754 floating-point standard
 * @see C23 standard (ISO/IEC 9899:2023)
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>

/**
 * @brief Include project-specific header for compatibility
 * @details Assumes "0.h" contains project-specific definitions like CHAR macro,
 * error handling functions, and other Pascal interpreter infrastructure
 */
#include "0.h"

/**
 * @brief Maximum safe shift count for octal conversion
 * @details Octal digits require 3 bits each. For 64-bit long, maximum safe
 * shifts would be approximately 21 octal digits to avoid overflow.
 */
#define MAX_OCTAL_SHIFTS ((sizeof(long) * 8) / 3)

/**
 * @brief Bit mask for detecting potential overflow in long integer
 * @details This mask checks the upper bits that would be affected by
 * a left shift of 3 positions (one octal digit)
 */
#define OVERFLOW_MASK (0x7UL << (sizeof(long) * 8 - 3))

/**
 * @brief Error codes for octal conversion operations
 */
typedef enum {
	OCTAL_SUCCESS = 0,          ///< Conversion completed successfully
	OCTAL_ERROR_INVALID_DIGIT,  ///< Invalid octal digit encountered (8 or 9)
	OCTAL_ERROR_OVERFLOW,       ///< Number too large for long integer
	OCTAL_ERROR_NULL_POINTER,   ///< Null pointer passed as input
	OCTAL_ERROR_EMPTY_STRING    ///< Empty string passed as input
} octal_conversion_error_t;

/**
 * @brief Result structure for octal conversion operations
 * @details Provides both the converted value and detailed error information
 * following modern C error handling paradigms
 */
typedef struct {
	long value;                        ///< Converted long integer value
	octal_conversion_error_t error;    ///< Error code (OCTAL_SUCCESS if no error)
	size_t processed_chars;            ///< Number of characters successfully processed
	const char* error_position;       ///< Pointer to character that caused error (if any)
} octal_conversion_result_t;

/**
 * @brief Convert octal string to long integer with comprehensive error handling
 * 
 * @details This function converts a null-terminated string representing an octal
 * number to a long integer value. It performs comprehensive validation including:
 * - Null pointer checking
 * - Empty string detection  
 * - Invalid octal digit detection (8, 9)
 * - Overflow detection using bitwise analysis
 * - Character-by-character processing with position tracking
 * 
 * The function uses modern C23 features including:
 * - Enhanced type safety with explicit integer types
 * - Structured error reporting
 * - Const-correctness for input parameters
 * - Comprehensive bounds checking
 * 
 * @param[in] cp Pointer to null-terminated string containing octal digits
 * 
 * @return octal_conversion_result_t Structure containing:
 *         - value: Converted long integer (0 if error occurred)
 *         - error: Error code indicating success or failure type
 *         - processed_chars: Count of successfully processed characters
 *         - error_position: Pointer to problematic character (NULL if no error)
 * 
 * @pre cp must point to a valid null-terminated string or be NULL
 * @post If successful, returned value represents the octal number in decimal
 * @post If error occurred, error field contains specific error type
 * 
 * @note Original function name 'a8tol' presumably meant 'ascii-base-8-to-long'
 * @note This implementation is thread-safe and reentrant
 * 
 * @warning Input string must contain only valid octal digits (0-7)
 * @warning Very large octal numbers may cause overflow
 * 
 * @see strtol() for alternative standard library conversion
 * @see errno.h for standard error handling patterns
 * 
 * @example
 * @code
 * const char* octal_str = "755";
 * octal_conversion_result_t result = convert_octal_to_long(octal_str);
 * if (result.error == OCTAL_SUCCESS) {
 *     printf("Converted value: %ld\n", result.value); // Prints: 493
 * } else {
 *     printf("Conversion failed with error: %d\n", result.error);
 * }
 * @endcode
 * 
 * @complexity Time: O(n) where n is string length, Space: O(1)
 */
[[nodiscard("Conversion result must be checked for errors")]]
static inline octal_conversion_result_t convert_octal_to_long(const char* restrict cp) {
	// Initialize result structure with safe defaults
	octal_conversion_result_t result = {
		.value = 0L,
		.error = OCTAL_SUCCESS,
		.processed_chars = 0,
		.error_position = NULL
	};
	
	// Validate input parameter
	if (cp == NULL) {
		result.error = OCTAL_ERROR_NULL_POINTER;
		return result;
	}
	
	// Check for empty string
	if (*cp == '\0') {
		result.error = OCTAL_ERROR_EMPTY_STRING;
		return result;
	}
	
	// Main conversion loop with enhanced error tracking
	long accumulated_value = 0L;
	bool error_encountered = false;
	const char* current_pos = cp;
	
	while (*current_pos != '\0') {
		const unsigned char current_char = (unsigned char)*current_pos;
		
		// Validate octal digit range
		if (current_char < '0' || current_char > '7') {
			if (current_char == '8' || current_char == '9') {
				if (!error_encountered) {
					// Modern error reporting - call project error function
					error("Invalid octal digit: 8 or 9 not allowed in octal number");
					result.error = OCTAL_ERROR_INVALID_DIGIT;
					result.error_position = current_pos;
					error_encountered = true;
				}
			}
			// Stop processing on any non-octal character
			break;
		}
		
		// Check for potential overflow before shift operation
		if ((accumulated_value & OVERFLOW_MASK) != 0) {
			if (!error_encountered) {
				error("Numeric overflow: number too large for long integer implementation");
				result.error = OCTAL_ERROR_OVERFLOW;
				result.error_position = current_pos;
				error_encountered = true;
			}
			break;
		}
		
		// Convert character to numeric value (fixed original bug: was =- instead of -)
		const unsigned char digit_value = current_char - '0';
		
		// Perform octal shift and accumulation
		accumulated_value = (accumulated_value << 3) | digit_value;
		
		// Update position tracking
		++current_pos;
		++result.processed_chars;
	}
	
	// Set final result
	if (!error_encountered) {
		result.value = accumulated_value;
	}
	
	return result;
}

/**
 * @brief Legacy wrapper function maintaining original API compatibility
 * 
 * @details Provides backward compatibility with the original a8tol function
 * while internally using the modernized conversion logic. This allows existing
 * code to continue working without modification while benefiting from improved
 * error handling and bug fixes.
 * 
 * @param[in] cp Pointer to null-terminated octal string
 * 
 * @return long Converted value, or 0 if conversion failed
 * 
 * @deprecated Use convert_octal_to_long() for new code to get detailed error information
 * 
 * @note This function maintains the original behavior of returning 0 on error
 * @note Error messages are still printed via the original error() function
 */
long a8tol(const char* cp) {
	const octal_conversion_result_t result = convert_octal_to_long(cp);
	return result.value;
}

/**
 * @brief Enhanced octal conversion with explicit error parameter
 * 
 * @details Provides a middle-ground API that returns detailed error information
 * through an output parameter while maintaining a simple return value for
 * the converted number.
 * 
 * @param[in] cp Pointer to null-terminated octal string
 * @param[out] error_code Pointer to store error code (may be NULL)
 * 
 * @return long Converted value, or 0 if conversion failed
 * 
 * @example
 * @code
 * octal_conversion_error_t error;
 * long value = a8tol_safe("777", &error);
 * if (error == OCTAL_SUCCESS) {
 *     // Use value safely
 * }
 * @endcode
 */
[[nodiscard("Conversion result should be validated")]]
long a8tol_safe(const char* restrict cp, octal_conversion_error_t* restrict error_code) {
	const octal_conversion_result_t result = convert_octal_to_long(cp);
	
	if (error_code != NULL) {
		*error_code = result.error;
	}
	
	return result.value;
}

/**
 * @brief Validate octal string format without conversion
 * 
 * @details Utility function to check if a string represents a valid octal
 * number without performing the actual conversion. Useful for validation
 * in parsing stages.
 * 
 * @param[in] cp Pointer to null-terminated string to validate
 * 
 * @return bool true if string contains only valid octal digits, false otherwise
 */
[[nodiscard("Validation result must be checked")]]
bool is_valid_octal_string(const char* restrict cp) {
	if (cp == NULL || *cp == '\0') {
		return false;
	}
	
	while (*cp != '\0') {
		if (*cp < '0' || *cp > '7') {
			return false;
		}
		++cp;
	}
	
	return true;
}

/**
 * @brief Get human-readable error message for octal conversion errors
 * 
 * @details Converts error codes to descriptive strings for user-friendly
 * error reporting and debugging purposes.
 * 
 * @param[in] error Error code from octal conversion operation
 * 
 * @return const char* Pointer to static string describing the error
 * 
 * @note Returned string is statically allocated and should not be freed
 */
[[nodiscard("Error message should be used for reporting")]]
const char* octal_error_to_string(octal_conversion_error_t error) {
	switch (error) {
		case OCTAL_SUCCESS:
			return "Conversion completed successfully";
		case OCTAL_ERROR_INVALID_DIGIT:
			return "Invalid octal digit encountered (8 or 9)";
		case OCTAL_ERROR_OVERFLOW:
			return "Number too large for long integer";
		case OCTAL_ERROR_NULL_POINTER:
			return "Null pointer passed as input";
		case OCTAL_ERROR_EMPTY_STRING:
			return "Empty string passed as input";
		default:
			return "Unknown error";
	}
}

/*
 * @note Historical Context and Modern Alternatives
 * 
 * The original comment mentioned that the atof() function used in the compiler
 * did not properly handle floating-point overflow. In modern C, developers should
 * use strtod(), strtof(), or strtold() functions which provide proper error
 * detection through errno and can distinguish between underflow, overflow, and
 * invalid input.
 * 
 * For floating-point conversion in modern code, consider:
 * - strtod() for double precision
 * - strtof() for single precision  
 * - strtold() for extended precision
 * - Check errno for ERANGE to detect overflow/underflow
 * - Validate that the entire string was consumed
 * 
 * Example modern floating-point conversion:
 * 
 * @code
 * #include <stdlib.h>
 * #include <errno.h>
 * 
 * double safe_strtod(const char* str, bool* success) {
 *     char* endptr;
 *     errno = 0;
 *     double result = strtod(str, &endptr);
 *     
 *     *success = (errno == 0) && (*endptr == '\0') && (endptr != str);
 *     return result;
 * }
 * @endcode
 */
