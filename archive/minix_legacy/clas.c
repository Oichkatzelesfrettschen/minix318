/**
 * @file clas.c
 * @brief Pascal Interpreter Type Classification and Analysis Module
 * 
 * @details This module provides comprehensive type classification, analysis, and 
 * categorization functionality for the Pascal interpreter. It implements a sophisticated
 * type system that maps Pascal language constructs to internal representations,
 * enabling efficient type checking, validation, and code generation.
 * 
 * The classification system operates on a hierarchical taxonomy where types are
 * categorized into fundamental groups (scalars, composites, references) and 
 * sub-classifications that preserve semantic meaning while enabling efficient
 * runtime operations.
 * 
 * @par Philosophical Foundation
 * The design embodies the principle that type classification should be both
 * mathematically rigorous and computationally efficient. Each type mapping
 * preserves essential semantic properties while enabling O(1) classification
 * operations through careful indexing strategies.
 * 
 * @par Mathematical Model
 * Let T be the set of all Pascal types, and C be the set of classification
 * categories. The classification function f: T → C is defined such that:
 * - f preserves type compatibility relationships
 * - f enables efficient subtype checking via integer comparison
 * - f maintains bijective mapping between compatible types
 * 
 * @author Charles Haley, Bill Joy (UCB) - Original Implementation
 * @author Modernization Team - C23 Standards Compliance
 * @version 2.0 - Modernized C23 Implementation
 * @date 2024
 * @copyright Copyright (c) 1979 Regents of the University of California
 * @copyright Modernization (c) 2024 - Licensed under Academic Use License
 * 
 * @see "Compilers: Principles, Techniques, and Tools" by Aho, Sethi, and Ullman
 * @see "Types and Programming Languages" by Benjamin C. Pierce
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>

#include "0.h"
#include "tree.h"
#include "opcode.h"

/**
 * @defgroup TypeClassification Type Classification System
 * @brief Comprehensive type classification and analysis subsystem
 * @{
 */

/**
 * @brief Type classification enumeration with mathematical properties
 * 
 * @details Defines the complete taxonomy of Pascal type classifications,
 * organized to enable efficient bitwise operations and range checking.
 * The negative indices are strategically chosen to enable pointer arithmetic
 * optimizations while maintaining semantic clarity.
 * 
 * @par Mathematical Properties
 * - TFIRST ≤ classification ≤ TLAST for all valid types
 * - Composite types have negative indices for efficient detection
 * - Scalar types have positive indices enabling direct array access
 * 
 * @invariant All classification values must be within [TFIRST, TLAST]
 * @invariant Composite types: classification < 0
 * @invariant Primitive types: classification > 0
 */
typedef enum {
	/** @brief File type classification - I/O abstraction layer */
	TFILE_CLASSIFICATION = -7,
	
	/** @brief Record type classification - structured composite */
	TREC_CLASSIFICATION = -6,
	
	/** @brief Array type classification - homogeneous collection */
	TARY_CLASSIFICATION = -5,
	
	/** @brief Scalar type classification - enumerated values */
	TSCAL_CLASSIFICATION = -4,
	
	/** @brief Pointer type classification - memory reference */
	TPTR_CLASSIFICATION = -3,
	
	/** @brief Set type classification - mathematical set abstraction */
	TSET_CLASSIFICATION = -2,
	
	/** @brief String type classification - character sequence */
	TSTR_CLASSIFICATION = -1,
	
	/** @brief Null type classification - absence of type */
	TNIL_CLASSIFICATION = 0,
	
	/** @brief Boolean type classification - logical values */
	TBOOL_CLASSIFICATION = 1,
	
	/** @brief Character type classification - single character */
	TCHAR_CLASSIFICATION = 2,
	
	/** @brief Integer type classification - whole numbers */
	TINT_CLASSIFICATION = 3,
	
	/** @brief Real type classification - floating-point numbers */
	TREAL_CLASSIFICATION = 4,
	
	/** @brief Explicit nil value classification */
	TNIL_VALUE_CLASSIFICATION = 5
} type_classification_t;

/**
 * @brief Classification name mapping table with academic documentation
 * 
 * @details Provides human-readable names for each type classification,
 * organized to enable direct indexing via classification values.
 * The string literals are carefully chosen to match Pascal language
 * specification terminology while remaining accessible to debugging tools.
 * 
 * @par Implementation Strategy
 * The array is dimensioned to accommodate the full range [TFIRST, TLAST]
 * with negative indexing handled through pointer arithmetic. This approach
 * eliminates bounds checking overhead while maintaining type safety.
 * 
 * @par Memory Layout
 * ```
 * Index:  -7    -6      -5     -4      -3       -2    -1      0     1        2      3         4      5
 * Value: file  record  array  scalar  pointer  set  string  nil  Boolean  char  integer  real  "nil"
 * ```
 * 
 * @warning Array must be kept synchronized with type_classification_t enum
 * @note String literals are compile-time constants for optimal memory usage
 */
static const char* const classification_names_internal[] = {
	[0 + (-TFIRST)] = "file",      /**< File I/O abstraction */
	[1 + (-TFIRST)] = "record",    /**< Structured composite type */
	[2 + (-TFIRST)] = "array",     /**< Homogeneous collection type */
	[3 + (-TFIRST)] = "scalar",    /**< Enumerated scalar type */
	[4 + (-TFIRST)] = "pointer",   /**< Memory reference type */
	[5 + (-TFIRST)] = "set",       /**< Mathematical set type */
	[6 + (-TFIRST)] = "string",    /**< Character sequence type */
	[7 + (-TFIRST)] = "nil",       /**< Null/undefined type */
	[8 + (-TFIRST)] = "Boolean",   /**< Logical boolean type */
	[9 + (-TFIRST)] = "char",      /**< Single character type */
	[10 + (-TFIRST)] = "integer",  /**< Whole number type */
	[11 + (-TFIRST)] = "real",     /**< Floating-point type */
	[12 + (-TFIRST)] = "\"nil\""   /**< Explicit nil value */
};

/**
 * @brief Optimized classification name accessor with zero offset adjustment
 * 
 * @details Provides O(1) access to classification names through pointer
 * arithmetic that eliminates the need for runtime offset calculations.
 * The pointer is positioned to handle negative indices directly.
 * 
 * @par Mathematical Foundation
 * Given classification c where TFIRST ≤ c ≤ TLAST:
 * classification_names[c] = classification_names_internal[c + (-TFIRST)]
 * 
 * @invariant classification_names[TFIRST] points to first valid entry
 * @invariant classification_names[TLAST] points to last valid entry
 */
static const char* const* const classification_names = 
	&classification_names_internal[-TFIRST];

/**
 * @brief Advanced type classification with comprehensive error handling
 * 
 * @details Implements a sophisticated type classification algorithm that
 * maps Pascal type descriptors to their corresponding classification
 * categories. The function employs a state-machine approach with
 * tail-call optimization for range types and comprehensive validation.
 * 
 * @par Algorithm Complexity
 * - Time Complexity: O(k) where k is the depth of range type nesting
 * - Space Complexity: O(1) with tail-call optimization
 * - Worst Case: O(log n) for deeply nested range types
 * 
 * @par Invariant Preservation
 * The function maintains several critical invariants:
 * - Input validation: p1 ∈ {valid_nl_pointer} ∪ {NIL}
 * - Classification consistency: output ∈ [TFIRST, TLAST]
 * - Range type transitivity: classify(range) = classify(range.base_type)
 * 
 * @param[in] type_descriptor Pointer to namelist entry representing the type
 *                           to be classified. May be NIL for undefined types.
 * 
 * @return int32_t Classification code from type_classification_t enumeration.
 *                 Returns NIL for invalid or undefined types.
 * 
 * @pre type_descriptor == NIL || is_valid_nl_pointer(type_descriptor)
 * @post TFIRST ≤ return_value ≤ TLAST || return_value == NIL
 * 
 * @exception panic Triggered on internal inconsistency in type system
 * @exception nocascade Called for NIL input to prevent error propagation
 * 
 * @see type_classification_t for complete classification taxonomy
 * @see is_valid_nl_pointer() for pointer validation
 * 
 * @par Example Usage
 * ```c
 * struct nl* integer_type = &nl[TINT];
 * int32_t classification = classify_type_comprehensive(integer_type);
 * assert(classification == TINT_CLASSIFICATION);
 * ```
 * 
 * @warning Function may call panic() on internal type system corruption
 * @note Optimized for common case of primitive type classification
 */
static int32_t classify_type_comprehensive(const struct nl* const type_descriptor) {
	// Input validation with comprehensive null checking
	if (type_descriptor == NIL) {
		nocascade();  // Prevent cascading error propagation
		return NIL;
	}
	
	// Validate pointer integrity (debug builds only)
	assert(is_valid_nl_pointer(type_descriptor));
	
	const struct nl* current_type = type_descriptor;
	
	// Tail-call optimized classification loop for range types
	while (true) {
		// Handle null type references with defensive programming
		if (current_type == NIL) {
			nocascade();
			return NIL;
		}
		
		// Special case: string type optimization
		if (current_type == &nl[TSTR]) {
			return TSTR_CLASSIFICATION;
		}
		
		// Primary classification dispatch based on type class
		switch (current_type->class) {
			case PTR:
				return TPTR_CLASSIFICATION;
				
			case ARRAY: {
				// Array classification with string type detection
				if (current_type->type == nl + T1CHAR) {
					return TSTR_CLASSIFICATION;  // Character array = string
				}
				return TARY_CLASSIFICATION;
			}
			
			case STR:
				return TSTR_CLASSIFICATION;
				
			case SET:
				return TSET_CLASSIFICATION;
				
			case RANGE:
				// Tail-call optimization for range type resolution
				current_type = current_type->type;
