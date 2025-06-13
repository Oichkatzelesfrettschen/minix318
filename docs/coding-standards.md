# MINIX4 Coding Standards

This document defines the coding standards for the MINIX4 operating system project. These standards ensure consistency, maintainability, and quality across the entire codebase.

> **Note**: This document supplements the comprehensive [LLM Instructions](../.github/instructions/LLMInstruct.instructions.md) which contain detailed specifications for AI assistants working on this project.

## 🎯 Overview

MINIX4 follows strict coding standards based on:
- **ISO C23 Standard** - Latest C standard with modern safety features
- **POSIX.1-2024 Compliance** - Full POSIX compatibility
- **Modern Assembly** - Intel syntax with Clang compatibility
- **Security-First Design** - Capability-based security throughout

## 📋 Language Standards

### C23 Requirements

All C code must comply with ISO C23 standard:

```c
// Required at top of every C file
#define _POSIX_C_SOURCE 202311L
#pragma once  // For headers

// Use specific-width integer types
uint32_t value = 42;
size_t length = strlen(buffer);
ptrdiff_t offset = ptr2 - ptr1;

// Use C23 features
_Static_assert(BUFFER_SIZE <= MAX_SIZE, "Buffer too large");
typeof(variable) temp = variable;
```

### Forbidden C Features
- Variable Length Arrays (VLAs)
- `goto` statements  
- Global variables (use static or parameters)
- Implicit function declarations
- Old-style function definitions

### Assembly Language Standards

Use Intel syntax for x86/x86-64, compatible with Clang:

```assembly
# x86-64 example
.section .text
.global function_name
.type function_name, @function

function_name:
    pushq   %rbp
    movq    %rsp, %rbp
    
    # Function body
    
    popq    %rbp
    retq
.size function_name, .-function_name
```

## 🏗️ Code Organization

### Directory Structure
```
/minix/
├── kernel/           # Core kernel code
│   ├── arch/        # Architecture-specific code
│   ├── include/     # Kernel headers
│   ├── ipc/         # Inter-process communication
│   ├── memory/      # Memory management
│   └── process/     # Process management
├── libos/           # OS libraries
├── servers/         # System servers
├── drivers/         # Device drivers
└── commands/        # System utilities
```

### File Naming
- C files: `module_name.c`
- Headers: `module_name.h`
- Assembly: `function_name.S`
- Build files: `meson.build`, `Makefile`

## 📄 Header File Standards

### Header Template
```c
/**
 * @file module_name.h
 * @brief Brief description
 * @details Detailed description
 * @version 4.0.0
 * @date 2025-06-13
 */

#pragma once
#define _POSIX_C_SOURCE 202311L

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Forward declarations */
struct example_struct;

/* Constants */
#define EXAMPLE_MAGIC 0x12345678

/* Function prototypes */
int example_init(void);
void example_cleanup(void);
```

### Header Requirements
- Use `#pragma once` instead of include guards
- Include minimal dependencies
- Use forward declarations to reduce coupling
- Document all public interfaces
- Use const correctness

## 🧠 Memory Management

### Safe Allocation
```c
// Always check allocations
void *ptr = kmalloc(size);
if (ptr == NULL) {
    return -ENOMEM;
}

// Zero sensitive data
memset(sensitive_data, 0, sizeof(sensitive_data));

// Set to NULL after freeing
kfree(ptr);
ptr = NULL;
```

### Memory Safety Rules
- No memory leaks
- No double free
- No use after free
- Always validate pointers
- Respect alignment requirements

## 🚨 Error Handling

### Error Codes
```c
// Use standard errno values
int function_name(param_t *param) {
    if (param == NULL) {
        return -EINVAL;
    }
    
    int result = operation(param);
    if (result < 0) {
        return result;
    }
    
    return 0; // Success
}
```

### Panic Conditions
```c
// Only panic for unrecoverable errors
if (critical_invariant_violated) {
    panic("Critical invariant violated: %s:%d", __FILE__, __LINE__);
}
```

## 📝 Documentation Standards

### Function Documentation
```c
/**
 * @brief Brief one-line description
 * @details Detailed description of functionality
 * 
 * @param[in] input_param Input parameter description
 * @param[out] output_param Output parameter description
 * 
 * @return Description of return value
 * @retval 0 Success
 * @retval -EINVAL Invalid parameter
 * 
 * @note Important usage notes
 * @warning Potential issues
 * @see Related functions
 */
int example_function(const input_t *input, output_t *output);
```

### Code Comments
```c
/* Module-level comment explaining purpose */

/**
 * @brief Structure representing kernel object
 */
struct kernel_object {
    uint32_t magic;      /**< Validation magic number */
    atomic_int refs;     /**< Reference count */
};

int function(void) {
    /*
     * Step 1: Initialize data structures
     * This is necessary because...
     */
    
    // TODO: Optimize for performance
    // FIXME: Hardware bug workaround
}
```

## 🔨 Build System

### Meson Configuration
```meson
project('minix4-component', 'c',
  version : '4.0.0',
  default_options : [
    'c_std=c23',
    'warning_level=3',
    'werror=false'  # Enable gradually
  ]
)

# Component-specific flags
component_flags = [
  '-Wall',
  '-Wextra', 
  '-Wno-unused-parameter'
]
```

### Architecture Support
- x86-64: Primary development target
- x86-32: Legacy compatibility
- ARM/AArch64: Embedded systems
- RISC-V: Future consideration

## 🧪 Testing Standards

### Unit Tests
```c
/**
 * @file test_module.c
 * @brief Unit tests for module
 */

#include "test_framework.h"
#include "module.h"

static void test_basic_functionality(void) {
    // Arrange
    module_t obj;
    
    // Act  
    int result = module_init(&obj);
    
    // Assert
    assert(result == 0);
    assert(module_is_valid(&obj));
    
    // Cleanup
    module_cleanup(&obj);
}

int main(void) {
    RUN_TEST(test_basic_functionality);
    return test_results();
}
```

## 🔒 Security Standards

### Input Validation
```c
int secure_function(const char *input, size_t len) {
    // Validate parameters
    if (input == NULL || len == 0 || len > MAX_INPUT) {
        return -EINVAL;
    }
    
    // Ensure null termination
    if (input[len - 1] != '\0') {
        return -EINVAL;
    }
    
    // Process safely...
}
```

### Capability Checking
```c
int privileged_operation(capability_t required_cap) {
    if (!current_process_has_capability(required_cap)) {
        return -EPERM;
    }
    
    // Perform operation...
}
```

## 🎨 Code Style

### Formatting
- **Indentation**: 4 spaces (no tabs)
- **Line length**: 80 characters maximum
- **Braces**: K&R style
- **Spacing**: Space after keywords, around operators

### Naming Conventions
```c
// Functions: lowercase with underscores
int process_create(void);

// Constants: UPPERCASE with underscores  
#define MAX_PROCESSES 256

// Types: lowercase with _t suffix
typedef struct process process_t;

// Variables: lowercase with underscores
int process_count = 0;
```

## 🚀 Performance Guidelines

### Optimization Principles
- **Clarity first**: Optimize only when necessary
- **Measure before optimizing**: Profile to find bottlenecks
- **Cache-friendly**: Design for CPU cache efficiency
- **Lock-free when possible**: Use atomic operations
- **Minimal allocations**: Avoid frequent malloc/free

### Critical Paths
- System call entry/exit
- Context switching
- Interrupt handling
- Memory allocation
- IPC message passing

## ✅ Code Review Checklist

Before submitting code, verify:
- [ ] Follows C23 standards
- [ ] No security vulnerabilities
- [ ] Proper error handling
- [ ] Memory safety compliance
- [ ] Comprehensive documentation
- [ ] Unit tests included
- [ ] Performance considerations
- [ ] Architecture compatibility
- [ ] POSIX compliance where applicable

## 📚 References

- [ISO C23 Standard](https://www.iso.org/standard/74528.html)
- [POSIX.1-2024](https://pubs.opengroup.org/onlinepubs/9699919799/)
- [Intel Assembly Syntax](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- [Secure Coding Practices](https://wiki.sei.cmu.edu/confluence/display/c/SEI+CERT+C+Coding+Standard)

---

**Last Updated**: 2025-06-13  
**Version**: 4.0.0
