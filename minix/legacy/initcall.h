/**
 * @file initcall.h
 * @brief MINIX4 Initialization Call Framework
 * @details This header provides a comprehensive initialization framework for MINIX4,
 *          allowing subsystems to register initialization functions that are executed
 *          in a well-defined order during system startup. The framework supports
 *          different initialization levels, error handling, and compatibility with
 *          legacy DDE kit initialization.
 * @version 4.0.0
 * @date 2025-06-12
 * @author MINIX Development Team
 * @copyright Copyright (c) 2025 MINIX Project
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum length for initcall function names
 * @details Used for debugging and error reporting purposes
 */
#define INITCALL_NAME_MAX_LEN 64

/**
 * @brief Maximum number of initcalls per level
 * @details Safety limit to prevent memory exhaustion
 */
#define INITCALL_MAX_PER_LEVEL 256

/**
 * @brief Return codes for initialization functions
 * @details Standardized return codes for initialization functions
 */
enum initcall_result {
  INITCALL_SUCCESS = 0,           /**< Initialization successful */
  INITCALL_ERROR_GENERIC = -1,    /**< Generic initialization error */
  INITCALL_ERROR_MEMORY = -2,     /**< Memory allocation failure */
  INITCALL_ERROR_HARDWARE = -3,   /**< Hardware initialization failure */
  INITCALL_ERROR_DEPENDENCY = -4, /**< Dependency not met */
  INITCALL_ERROR_TIMEOUT = -5,    /**< Initialization timeout */
  INITCALL_ERROR_PERMISSION = -6, /**< Permission denied */
  INITCALL_ERROR_INVALID = -7     /**< Invalid parameters */
};

/**
 * @brief Initialization levels
 * @details Defines the order in which initialization functions are called.
 *          Lower numbered levels are executed first. Each level represents
 *          a logical grouping of related initialization tasks.
 */
enum initcall_level {
  /** @brief Early initialization - critical low-level setup */
  INITCALL_EARLY = 0,
  
  /** @brief Architecture-specific initialization */
  INITCALL_ARCH,
  
  /** @brief Memory management subsystem initialization */
  INITCALL_MEMORY,
  
  /** @brief Capability system initialization */
  INITCALL_CAPABILITY,
  
  /** @brief Inter-Process Communication subsystem initialization */
  INITCALL_IPC,
  
  /** @brief Process scheduler initialization */
  INITCALL_SCHEDULER,
  
  /** @brief Device driver initialization */
  INITCALL_DEVICE,
  
  /** @brief File system initialization */
  INITCALL_FILESYSTEM,
  
  /** @brief Network subsystem initialization */
  INITCALL_NETWORK,
  
  /** @brief Late initialization - non-critical components */
  INITCALL_LATE,
  
  /** @brief Maximum initialization level marker */
  INITCALL_MAX
};

/**
 * @brief Forward declaration of initcall structure
 */
struct initcall;

/**
 * @brief Initialization function type
 * @details Function signature for initialization callbacks
 * @return One of the initcall_result values
 * @retval INITCALL_SUCCESS Initialization completed successfully
 * @retval negative_value Error occurred during initialization
 */
typedef int (*initcall_fn_t)(void);

/**
 * @brief Cleanup function type
 * @details Function signature for cleanup callbacks, called on failure
 * @param[in] initcall Pointer to the initcall structure that failed
 */
typedef void (*initcall_cleanup_fn_t)(const struct initcall *initcall);

/**
 * @brief Progress callback function type
 * @details Function signature for progress reporting callbacks
 * @param[in] level Current initialization level
 * @param[in] current Current initcall index within level
 * @param[in] total Total number of initcalls in level
 * @param[in] name Name of current initcall function
 */
typedef void (*initcall_progress_fn_t)(enum initcall_level level, 
                     size_t current, 
                     size_t total, 
                     const char *name);

/**
 * @brief Initialization call descriptor
 * @details Complete descriptor for an initialization function,
 *          including metadata and optional callbacks
 */
struct initcall {
  /** @brief Function name for debugging and logging */
  const char *name;
  
  /** @brief Main initialization function pointer */
  initcall_fn_t fn;
  
  /** @brief Initialization level/priority */
  enum initcall_level level;
  
  /** @brief Configuration and behavior flags */
  uint32_t flags;
  
  /** @brief Optional cleanup function called on failure */
  initcall_cleanup_fn_t cleanup_fn;
  
  /** @brief Optional description for documentation */
  const char *description;
  
  /** @brief Module/subsystem identifier */
  const char *module;
  
  /** @brief Version information */
  uint32_t version;
  
  /** @brief Execution timeout in milliseconds (0 = no timeout) */
  uint32_t timeout_ms;
  
  /** @brief Dependencies - array of required initcall names */
  const char **dependencies;
  
  /** @brief Number of dependencies */
  size_t dependency_count;
};

/**
 * @brief Initcall configuration flags
 * @details Bitmask flags that control initcall behavior
 */
/** @brief Critical initialization - system fails if this fails */
#define INITCALL_CRITICAL       0x00000001U

/** @brief Optional initialization - continue if this fails */
#define INITCALL_OPTIONAL       0x00000002U

/** @brief SMP-aware initialization - safe for multi-processor */
#define INITCALL_SMP            0x00000004U

/** @brief Silent initialization - don't log progress */
#define INITCALL_SILENT         0x00000008U

/** @brief Retry on failure - attempt multiple times */
#define INITCALL_RETRY          0x00000010U

/** @brief Enable timeout checking */
#define INITCALL_TIMEOUT        0x00000020U

/** @brief Defer until later if dependencies not met */
#define INITCALL_DEFER          0x00000040U

/** @brief Run only on primary CPU */
#define INITCALL_PRIMARY_CPU    0x00000080U

/** @brief Enable detailed debugging */
#define INITCALL_DEBUG          0x00000100U

/** @brief Measure execution time */
#define INITCALL_PROFILE        0x00000200U

/** @brief Allow parallel execution within level */
#define INITCALL_PARALLEL       0x00000400U

/** @brief Reserved flag bits mask */
#define INITCALL_FLAGS_RESERVED 0xFFFFF800U

/**
 * @brief Retry configuration
 */
#define INITCALL_MAX_RETRIES    3       /**< Maximum retry attempts */
#define INITCALL_RETRY_DELAY_MS 100     /**< Delay between retries */

/**
 * @brief Default timeout values
 */
#define INITCALL_DEFAULT_TIMEOUT_MS 5000    /**< Default timeout (5 seconds) */
#define INITCALL_CRITICAL_TIMEOUT_MS 30000  /**< Critical timeout (30 seconds) */

/**
 * @brief Register an initialization call with full options
 * @details Advanced macro for registering initcalls with complete configuration
 * @param[in] fn Function to register
 * @param[in] lvl Initialization level
 * @param[in] flgs Configuration flags
 * @param[in] cleanup_fn Optional cleanup function
 * @param[in] desc Optional description
 * @param[in] mod Module identifier
 * @param[in] ver Version number
 * @param[in] timeout_ms Timeout in milliseconds
 */
#define REGISTER_INITCALL_FULL(fn, lvl, flgs, cleanup_fn, desc, mod, ver, timeout_ms) \
  static const struct initcall __initcall_##fn \
  __attribute__((used, section(".initcall." #lvl), aligned(sizeof(void*)))) = { \
    .name = #fn, \
    .fn = fn, \
    .level = lvl, \
    .flags = flgs, \
    .cleanup_fn = cleanup_fn, \
    .description = desc, \
    .module = mod, \
    .version = ver, \
    .timeout_ms = timeout_ms, \
    .dependencies = NULL, \
    .dependency_count = 0 \
  }

/**
 * @brief Register an initialization call with dependencies
 * @details Macro for registering initcalls with dependency checking
 * @param[in] fn Function to register
 * @param[in] lvl Initialization level
 * @param[in] flgs Configuration flags
 * @param[in] deps Array of dependency names
 * @param[in] dep_count Number of dependencies
 */
#define REGISTER_INITCALL_DEPS(fn, lvl, flgs, deps, dep_count) \
  static const char* __initcall_deps_##fn[] = deps; \
  static const struct initcall __initcall_##fn \
  __attribute__((used, section(".initcall." #lvl), aligned(sizeof(void*)))) = { \
    .name = #fn, \
    .fn = fn, \
    .level = lvl, \
    .flags = flgs, \
    .cleanup_fn = NULL, \
    .description = NULL, \
    .module = __FILE__, \
    .version = 1, \
    .timeout_ms = INITCALL_DEFAULT_TIMEOUT_MS, \
    .dependencies = __initcall_deps_##fn, \
    .dependency_count = dep_count \
  }

/**
 * @brief Basic initialization call registration
 * @details Simple macro for basic initcall registration
 * @param[in] fn Function to register
 * @param[in] lvl Initialization level
 * @param[in] flgs Configuration flags
 */
#define REGISTER_INITCALL(fn, lvl, flgs) \
  REGISTER_INITCALL_FULL(fn, lvl, flgs, NULL, NULL, __FILE__, 1, INITCALL_DEFAULT_TIMEOUT_MS)

/**
 * @brief Convenience macros for different initialization levels
 * @details These macros provide easy registration for common initialization types
 */

/** @brief Register early initialization function */
#define early_initcall(fn) \
  REGISTER_INITCALL(fn, INITCALL_EARLY, INITCALL_CRITICAL | INITCALL_TIMEOUT)

/** @brief Register architecture-specific initialization function */
#define arch_initcall(fn) \
  REGISTER_INITCALL(fn, INITCALL_ARCH, INITCALL_CRITICAL | INITCALL_PRIMARY_CPU)

/** @brief Register memory management initialization function */
#define memory_initcall(fn) \
  REGISTER_INITCALL(fn, INITCALL_MEMORY, INITCALL_CRITICAL | INITCALL_TIMEOUT)

/** @brief Register capability system initialization function */
#define capability_initcall(fn) \
  REGISTER_INITCALL(fn, INITCALL_CAPABILITY, INITCALL_CRITICAL)

/** @brief Register IPC subsystem initialization function */
#define ipc_initcall(fn) \
  REGISTER_INITCALL(fn, INITCALL_IPC, INITCALL_CRITICAL | INITCALL_SMP)

/** @brief Register scheduler initialization function */
#define scheduler_initcall(fn) \
  REGISTER_INITCALL(fn, INITCALL_SCHEDULER, INITCALL_CRITICAL | INITCALL_SMP)

/** @brief Register device driver initialization function */
#define device_initcall(fn) \
  REGISTER_INITCALL(fn, INITCALL_DEVICE, INITCALL_OPTIONAL | INITCALL_RETRY)

/** @brief Register file system initialization function */
#define filesystem_initcall(fn) \
  REGISTER_INITCALL(fn, INITCALL_FILESYSTEM, INITCALL_OPTIONAL | INITCALL_PARALLEL)

/** @brief Register network subsystem initialization function */
#define network_initcall(fn) \
  REGISTER_INITCALL(fn, INITCALL_NETWORK, INITCALL_OPTIONAL | INITCALL_TIMEOUT)

/** @brief Register late initialization function */
#define late_initcall(fn) \
  REGISTER_INITCALL(fn, INITCALL_LATE, INITCALL_OPTIONAL | INITCALL_PARALLEL)

/**
 * @brief Initialization statistics
 * @details Structure to hold initialization execution statistics
 */
struct initcall_stats {
  /** @brief Total number of initcalls executed */
  size_t total_count;
  
  /** @brief Number of successful initializations */
  size_t success_count;
  
  /** @brief Number of failed initializations */
  size_t failure_count;
  
  /** @brief Number of skipped initializations */
  size_t skipped_count;
  
  /** @brief Total execution time in microseconds */
  uint64_t total_time_us;
  
  /** @brief Longest execution time in microseconds */
  uint64_t max_time_us;
  
  /** @brief Name of longest running initcall */
  char longest_name[INITCALL_NAME_MAX_LEN];
  
  /** @brief Number of retries performed */
  size_t retry_count;
  
  /** @brief Number of timeouts occurred */
  size_t timeout_count;
};

/**
 * @brief Core initialization framework functions
 */

/**
 * @brief Run all initialization calls
 * @details Executes all registered initialization calls in level order
 * @return Number of successful initializations, negative on critical failure
 * @retval positive Number of successful initcalls
 * @retval negative Critical initialization failure
 */
int run_initcalls(void);

/**
 * @brief Run initialization calls for a specific level
 * @details Executes all initcalls registered for the specified level
 * @param[in] level The initialization level to execute
 * @return Number of successful initializations in the level
 * @retval positive Number of successful initcalls
 * @retval negative Error occurred during level initialization
 */
int run_initcalls_level(enum initcall_level level);

/**
 * @brief Run initialization calls with progress callback
 * @details Execute initcalls with progress reporting
 * @param[in] progress_fn Progress callback function (can be NULL)
 * @return Number of successful initializations
 * @retval positive Number of successful initcalls
 * @retval negative Critical initialization failure
 */
int run_initcalls_with_progress(initcall_progress_fn_t progress_fn);

/**
 * @brief Get initialization statistics
 * @details Retrieve execution statistics for the last initcall run
 * @param[out] stats Pointer to statistics structure to fill
 * @return 0 on success, negative on error
 * @retval 0 Statistics retrieved successfully
 * @retval -1 Invalid parameters
 */
int get_initcall_stats(struct initcall_stats *stats);

/**
 * @brief Check if an initcall exists
 * @details Verify if a named initcall is registered
 * @param[in] name Name of the initcall to check
 * @return true if initcall exists, false otherwise
 */
bool initcall_exists(const char *name);

/**
 * @brief Get initcall information
 * @details Retrieve information about a specific initcall
 * @param[in] name Name of the initcall
 * @param[out] initcall Pointer to structure to fill with initcall info
 * @return 0 on success, negative on error
 * @retval 0 Information retrieved successfully
 * @retval -1 Initcall not found
 * @retval -2 Invalid parameters
 */
int get_initcall_info(const char *name, struct initcall *initcall);

/**
 * @brief List all registered initcalls
 * @details Get an array of all registered initcall names
 * @param[out] names Array to store initcall names
 * @param[in] max_count Maximum number of names to store
 * @return Number of initcalls stored in the array
 * @retval positive Number of initcalls listed
 * @retval -1 Invalid parameters
 */
int list_initcalls(const char **names, size_t max_count);

/**
 * @brief Enable or disable initcall debugging
 * @details Control debug output for initcall execution
 * @param[in] enable true to enable debugging, false to disable
 */
void set_initcall_debug(bool enable);

/**
 * @brief Set global initcall timeout
 * @details Set default timeout for all initcalls
 * @param[in] timeout_ms Timeout in milliseconds
 * @return 0 on success, negative on error
 * @retval 0 Timeout set successfully
 * @retval -1 Invalid timeout value
 */
int set_initcall_timeout(uint32_t timeout_ms);

/**
 * @brief Legacy DDE Kit Compatibility Layer
 * @details Structures and functions for backward compatibility with DDE kit
 */

/**
 * @brief DDE Kit initialization function type
 * @details Legacy function signature for DDE kit compatibility
 */
typedef void (*ddekit_initcall_t)(void);

/**
 * @brief DDE Kit initialization call structure
 * @details Internal structure for DDE kit initcall management
 */
struct __ddekit_initcall_s {
  /** @brief Initialization function pointer */
  ddekit_initcall_t func;
  
  /** @brief Priority (lower numbers execute first) */
  int prio;
  
  /** @brief Next initcall in linked list */
  struct __ddekit_initcall_s *next;
};

/**
 * @brief Add a DDE Kit initialization call
 * @details Register a DDE kit style initialization function
 * @param[in] dis Pointer to DDE kit initcall structure
 */
void __ddekit_add_initcall(struct __ddekit_initcall_s *dis);

/**
 * @brief Define a function to be a DDEKit initcall
 * @details Compatibility macro for Linux module_init style functions
 * @param[in] fn Function to register as DDE kit initcall
 */
#define DDEKIT_INITCALL(fn) DDEKIT_CTOR(fn, 1)

/**
 * @brief DDE Kit constructor macro with priority
 * @details Advanced macro for DDE kit initialization with priority
 * @param[in] fn Function to register
 * @param[in] prio Priority level (lower executes first)
 */
#define DDEKIT_CTOR(fn, prio) \
  static struct __ddekit_initcall_s __ddekit_initcall_struct_##fn = { \
    .func = fn, \
    .prio = prio, \
    .next = NULL \
  }; \
  static void __attribute__((used)) \
  __attribute__((constructor)) __ddekit_initcall_##fn(void) { \
    __ddekit_add_initcall(&__ddekit_initcall_struct_##fn); \
  }

/**
 * @brief Invoke all registered DDE Kit initialization callbacks
 * @details Execute all DDE kit style initcalls in priority order
 */
void ddekit_do_initcalls(void);

/**
 * @brief Get DDE Kit initialization statistics
 * @details Retrieve statistics for DDE kit initcall execution
 * @param[out] total_count Total number of DDE kit initcalls executed
 * @param[out] success_count Number of successful executions
 * @return 0 on success, negative on error
 */
int ddekit_get_stats(size_t *total_count, size_t *success_count);

/**
 * @brief Utility macros for validation and debugging
 */

/**
 * @brief Validate initcall structure
 * @details Compile-time validation of initcall structure
 */
#define INITCALL_VALIDATE(initcall) \
  _Static_assert(sizeof(initcall) == sizeof(struct initcall), \
           "Invalid initcall structure size")

/**
 * @brief Get string representation of initcall level
 * @details Convert initcall level enum to string
 * @param[in] level Initialization level
 * @return String representation of the level
 */
static inline const char* initcall_level_to_string(enum initcall_level level) {
  switch (level) {
    case INITCALL_EARLY:      return "EARLY";
    case INITCALL_ARCH:       return "ARCH";
    case INITCALL_MEMORY:     return "MEMORY";
    case INITCALL_CAPABILITY: return "CAPABILITY";
    case INITCALL_IPC:        return "IPC";
    case INITCALL_SCHEDULER:  return "SCHEDULER";
    case INITCALL_DEVICE:     return "DEVICE";
    case INITCALL_FILESYSTEM: return "FILESYSTEM";
    case INITCALL_NETWORK:    return "NETWORK";
    case INITCALL_LATE:       return "LATE";
    default:                  return "UNKNOWN";
  }
}

/**
 * @brief Get string representation of initcall result
 * @details Convert initcall result code to string
 * @param[in] result Result code
 * @return String representation of the result
 */
static inline const char* initcall_result_to_string(int result) {
  switch (result) {
    case INITCALL_SUCCESS:          return "SUCCESS";
    case INITCALL_ERROR_GENERIC:    return "GENERIC_ERROR";
    case INITCALL_ERROR_MEMORY:     return "MEMORY_ERROR";
    case INITCALL_ERROR_HARDWARE:   return "HARDWARE_ERROR";
    case INITCALL_ERROR_DEPENDENCY: return "DEPENDENCY_ERROR";
    case INITCALL_ERROR_TIMEOUT:    return "TIMEOUT_ERROR";
    case INITCALL_ERROR_PERMISSION: return "PERMISSION_ERROR";
    case INITCALL_ERROR_INVALID:    return "INVALID_ERROR";
    default:                        return "UNKNOWN_ERROR";
  }
}

#ifdef __cplusplus
}
#endif

/**
 * @brief End of file marker
 * @note This header provides a comprehensive initialization framework for MINIX4
 *       with support for ordered initialization, error handling, dependency
 *       management, and backward compatibility with DDE kit.
 */
#endif /* INITCALL_H */
