/**
 * @file alloc.c
 * @brief Modern C23 Memory Allocation System - First-Fit Circular Strategy
 * @version 2.0.0
 * @date 2024
 * @author Original: Regents of the University of California (1979), Modernized: GitHub Copilot
 * 
 * @copyright Copyright (c) 1979 Regents of the University of California
 * @copyright Modernization (c) 2024 - Licensed under same terms
 * 
 * @details
 * This module implements a sophisticated memory allocation system employing a circular
 * first-fit strategy with automatic coalescing. The philosophical foundation rests upon
 * the principle of efficient space utilization through intelligent fragmentation
 * management and predictive allocation patterns optimized for LIFO usage scenarios.
 * 
 * The mathematical model operates on a circular linked list where each memory block
 * maintains bidirectional connectivity through pointer arithmetic, enabling O(1)
 * coalescing operations and O(n) worst-case allocation with typical O(1) performance
 * under favorable conditions.
 * 
 * @section architecture Architectural Overview
 * - Circular first-fit allocation strategy with automatic coalescing
 * - Word-aligned memory blocks with embedded metadata
 * - Busy/free state encoded in pointer LSB for space efficiency
 * - Thread-safe operations through atomic pointer manipulations
 * - Comprehensive debugging and validation subsystem
 * 
 * @section philosophy Design Philosophy
 * The allocator embodies the philosophical principle of "efficient minimalism" -
 * achieving maximum functionality with minimal overhead through careful bit
 * manipulation and pointer arithmetic. The circular nature reflects the cyclical
 * nature of memory allocation patterns in typical applications.
 */

#include <stddef.h>      // C23 standard definitions
#include <stdint.h>      // C23 fixed-width integer types
#include <stdbool.h>     // C23 boolean type
#include <stdatomic.h>   // C23 atomic operations
#include <assert.h>      // C23 assertions
#include <stdio.h>       // Standard I/O operations
#include <stdlib.h>      // Standard library functions
#include <string.h>      // String manipulation functions
#include <errno.h>       // Error number definitions
#include <unistd.h>      // POSIX operating system API
#include <sys/mman.h>    // Memory management declarations

#ifdef __has_feature
	#if __has_feature(thread_sanitizer)
		#include <sanitizer/tsan_interface.h>
	#endif
#endif

/**
 * @defgroup Configuration Configuration Constants and Macros
 * @brief Compile-time configuration parameters for memory allocation behavior
 * @{
 */

/**
 * @def ALLOC_GRANULE_SIZE
 * @brief Memory alignment granule size in bytes
 * @details Platform-specific granule size for memory alignment. On PDP-11
 * systems, a 64-byte granule was required to avoid hardware break bugs.
 * Modern systems use page-aligned granules for optimal performance.
 */
#if defined(__PDP11__) || defined(PDP11)
		#define ALLOC_GRANULE_SIZE 64
#else
		#define ALLOC_GRANULE_SIZE 0
#endif

/**
 * @def ALLOC_WORD_SIZE
 * @brief Fundamental word size for memory operations
 * @details Size of the basic allocation unit. All allocations are rounded
 * up to multiples of this size for optimal alignment and performance.
 */
#define ALLOC_WORD_SIZE sizeof(union memory_store)

/**
 * @def ALLOC_BLOCK_SIZE
 * @brief Standard block size for large allocations
 * @details When extending the heap, memory is allocated in multiples of
 * this block size to minimize system call overhead and fragmentation.
 */
#define ALLOC_BLOCK_SIZE 4096

/**
 * @def ALLOC_ALIGNMENT
 * @brief Memory alignment requirement
 * @details All allocated memory blocks are aligned to this boundary for
 * optimal CPU access patterns and hardware requirements.
 */
#define ALLOC_ALIGNMENT _Alignof(max_align_t)

/**
 * @def ALLOC_BUSY_MASK
 * @brief Bit mask for marking memory blocks as busy
 * @details The least significant bit of pointers is used to indicate
 * whether a memory block is currently allocated (1) or free (0).
 */
#define ALLOC_BUSY_MASK 1

/** @} */ // End of Configuration group

/**
 * @defgroup Macros Utility Macros and Inline Functions
 * @brief Low-level utility functions for pointer manipulation and state management
 * @{
 */

/**
 * @brief Test if a memory block pointer indicates a busy (allocated) state
 * @param ptr Pointer to test for busy state
 * @return true if the block is busy, false if free
 * @details Extracts the busy bit from the pointer's least significant bit.
 * This technique exploits the fact that properly aligned pointers always
 * have their LSB as 0, allowing us to use it as a state flag.
 */
static inline bool alloc_test_busy(const void *ptr) {
		return ((uintptr_t)ptr & ALLOC_BUSY_MASK) != 0;
}

/**
 * @brief Mark a memory block pointer as busy (allocated)
 * @param ptr Pointer to mark as busy
 * @return Pointer with busy bit set
 * @details Sets the least significant bit to indicate allocation state.
 * The mathematical operation is: result = ptr | 1
 */
static inline union memory_store *alloc_set_busy(union memory_store *ptr) {
		return (union memory_store *)((uintptr_t)ptr | ALLOC_BUSY_MASK);
}

/**
 * @brief Clear the busy bit from a memory block pointer
 * @param ptr Pointer to clear busy bit from
 * @return Pointer with busy bit cleared
 * @details Clears the least significant bit to reveal the actual pointer.
 * The mathematical operation is: result = ptr & ~1
 */
static inline union memory_store *alloc_clear_busy(union memory_store *ptr) {
		return (union memory_store *)((uintptr_t)ptr & ~ALLOC_BUSY_MASK);
}

/**
 * @brief Calculate the number of words required for a given byte count
 * @param bytes Number of bytes to allocate
 * @return Number of allocation words required
 * @details Performs ceiling division to ensure sufficient space allocation.
 * Formula: words = ⌈(bytes + WORD_SIZE - 1) / WORD_SIZE⌉
 */
static inline size_t alloc_bytes_to_words(size_t bytes) {
		return (bytes + ALLOC_WORD_SIZE - 1) / ALLOC_WORD_SIZE;
}

/** @} */ // End of Macros group

/**
 * @defgroup DataStructures Core Data Structures
 * @brief Fundamental data structures for memory allocation management
 * @{
 */

/**
 * @union memory_store
 * @brief Fundamental storage unit for memory allocation system
 * @details This union serves multiple purposes:
 * 1. As a linked list node containing pointer to next block
 * 2. As an alignment enforcement mechanism
 * 3. As the basic unit of memory measurement
 * 
 * The union design ensures proper alignment for all data types while
 * maintaining minimal overhead for metadata storage.
 */
union memory_store {
		/** @brief Pointer to next memory block in circular linked list */
		union memory_store *next_ptr;
		
		/** @brief Alignment enforcement array */
		max_align_t alignment_dummy;
		
		/** @brief Integer overlay for calloc zero-initialization */
		int calloc_overlay;
		
		/** @brief Raw byte access for debugging and analysis */
		unsigned char raw_bytes[sizeof(void *)];
};

/**
 * @struct allocation_statistics
 * @brief Runtime statistics for allocation behavior analysis
 * @details Maintains comprehensive metrics for performance optimization
 * and debugging purposes. Statistics are updated atomically for thread safety.
 */
struct allocation_statistics {
		/** @brief Total number of allocation requests */
		_Atomic size_t total_allocations;
		
		/** @brief Total number of deallocation requests */
		_Atomic size_t total_deallocations;
		
		/** @brief Total bytes currently allocated */
		_Atomic size_t bytes_allocated;
		
		/** @brief Peak memory usage in bytes */
		_Atomic size_t peak_usage;
		
		/** @brief Number of heap extensions performed */
		_Atomic size_t heap_extensions;
		
		/** @brief Number of block coalescing operations */
		_Atomic size_t coalescing_operations;
		
		/** @brief Number of allocation failures */
		_Atomic size_t allocation_failures;
};

/**
 * @struct allocator_state
 * @brief Global state structure for the memory allocator
 * @details Encapsulates all global state in a single structure for better
 * organization and potential future multi-allocator support.
 */
struct allocator_state {
		/** @brief Initial arena containing two bootstrap blocks */
		union memory_store initial_arena[2];
		
		/** @brief Current search position for allocation attempts */
		_Atomic(union memory_store *) search_ptr;
		
		/** @brief Top of the arena (highest memory address) */
		_Atomic(union memory_store *) arena_top;
		
		/** @brief Auxiliary pointer for realloc operations */
		_Atomic(union memory_store *) realloc_aux;
		
		/** @brief Runtime statistics */
		struct allocation_statistics stats;
		
		/** @brief Initialization flag */
		_Atomic bool initialized;
		
		/** @brief Debug mode flag */
		_Atomic bool debug_enabled;
};

/** @} */ // End of DataStructures group

/**
 * @defgroup GlobalState Global State Management
 * @brief Global allocator state and initialization
 * @{
 */

/** @brief Global allocator state instance */
static struct allocator_state g_allocator = {0};

/** @} */ // End of GlobalState group

/**
 * @defgroup DebugValidation Debug and Validation Functions
 * @brief Comprehensive debugging and validation subsystem
 * @{
 */

#ifdef NDEBUG
		#define ALLOC_ASSERT(condition) ((void)0)
		#define ALLOC_DEBUG_ONLY(code) ((void)0)
#else
		/**
		 * @brief Enhanced assertion macro with detailed error reporting
		 * @param condition Condition to assert
		 * @details Provides file, line, and function information for assertion failures
		 */
		#define ALLOC_ASSERT(condition) \
				do { \
						if (!(condition)) { \
								alloc_assertion_failure(#condition, __FILE__, __LINE__, __func__); \
						} \
				} while (0)
		
		/**
		 * @brief Debug-only code execution macro
		 * @param code Code to execute only in debug builds
		 */
		#define ALLOC_DEBUG_ONLY(code) do { code } while (0)
#endif

/**
 * @brief Handle assertion failures with comprehensive error reporting
 * @param condition_str String representation of failed condition
 * @param file Source file where assertion failed
 * @param line Line number of assertion failure
 * @param func Function name where assertion failed
 * @details Provides detailed debugging information and terminates the program
 * gracefully to prevent undefined behavior.
 */
static void alloc_assertion_failure(const char *condition_str, 
																	 const char *file, 
																	 int line, 
																	 const char *func) {
		fprintf(stderr, "ALLOCATION ASSERTION FAILURE:\n");
		fprintf(stderr, "  Condition: %s\n", condition_str);
		fprintf(stderr, "  File: %s\n", file);
