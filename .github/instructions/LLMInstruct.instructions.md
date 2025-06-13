---
applyTo: '**'
---

# MINIX4 Development Standards and LLM Instructions

This file defines the coding standards, architectural principles, and development guidelines for the MINIX4 operating system project. All AI assistants working on this codebase must adhere to these standards.

## 📋 Table of Contents

1. [Project Overview](#project-overview)
2. [C23 Language Standards](#c23-language-standards)
3. [Assembly Language Standards](#assembly-language-standards)
4. [POSIX Compliance Requirements](#posix-compliance-requirements)
5. [Code Organization Structure](#code-organization-structure)
6. [Header File Standards](#header-file-standards)
7. [Memory Management Standards](#memory-management-standards)
8. [Error Handling Standards](#error-handling-standards)
9. [Documentation Standards](#documentation-standards)
10. [Build System Standards](#build-system-standards)
11. [Testing Standards](#testing-standards)
12. [Security Standards](#security-standards)

## 🎯 Project Overview

**MINIX4** is a modern, clean-slate implementation of the MINIX operating system, designed with:
- **C23 Standard Compliance**: Latest ISO C standard features and safety
- **Modular Microkernel Architecture**: Clean separation of concerns
- **POSIX.1-2024 Compliance**: Full POSIX compatibility layer
- **Modern Security**: Capability-based security model
- **Cross-Platform Support**: x86-32, x86-64, ARM, AArch64

### Project Goals
- Consolidate and modernize the legacy MINIX codebase (44,000+ files)
- Eliminate code duplication and inconsistencies
- Create a reference implementation for modern OS design
- Maintain educational value while achieving production quality

## 🔧 C23 Language Standards

### Required Language Features
```c
#define _POSIX_C_SOURCE 202311L  // Always include at top of files
#pragma once                    // Use instead of include guards
```

### Mandatory C23 Features to Use
- **`_Static_assert`** for compile-time checks
- **`typeof` and `typeof_unqual`** for type safety
- **`_BitInt(N)`** for precise integer widths when needed
- **Improved UTF-8 support** with `char8_t`
- **`[[]]` attributes** for optimization hints
- **Enhanced generic selections** with `_Generic`
- **Improved initialization** with designated initializers

### Type Safety Requirements
```c
// ✅ REQUIRED: Use specific-width types
uint32_t value = 42;
size_t length = strlen(str);
ptrdiff_t offset = ptr2 - ptr1;

// ❌ FORBIDDEN: Avoid old-style types
int value = 42;           // Use int32_t or appropriate type
unsigned offset = 10;     // Use uint32_t or size_t
```

### Memory Safety Requirements
```c
// ✅ REQUIRED: Bounds checking
_Static_assert(ARRAY_SIZE <= MAX_SAFE_SIZE, "Array too large");

// ✅ REQUIRED: NULL pointer checks
if (ptr != NULL) {
    // Use pointer
}

// ✅ REQUIRED: Integer overflow protection
bool overflow = __builtin_add_overflow(a, b, &result);
if (overflow) {
    return -EOVERFLOW;
}
```

### Forbidden C Features
- **Variable Length Arrays (VLAs)**: Use fixed arrays or dynamic allocation
- **`goto` statements**: Use structured control flow
- **Global variables**: Use static or pass via parameters
- **Implicit function declarations**: Always provide prototypes
- **Old-style function definitions**: Use modern prototypes

## ⚙️ Assembly Language Standards

### Syntax Requirements
- **Intel Syntax**: Use Intel syntax for x86/x86-64
- **Clang Compatibility**: All assembly must compile with Clang
- **AT&T Syntax**: Acceptable for GCC compatibility when needed

### x86-64 Assembly Template
```assembly
# File: arch/x86_64/context_switch.S
# Modern x86-64 assembly with Clang compatibility

.section .text
.global context_switch
.type context_switch, @function

# Function: context_switch(old_ctx, new_ctx)
# Parameters: %rdi = old context, %rsi = new context
context_switch:
    # Save callee-saved registers
    pushq   %rbp
    pushq   %rbx
    pushq   %r12
    pushq   %r13
    pushq   %r14
    pushq   %r15
    
    # Save current stack pointer
    movq    %rsp, (%rdi)
    
    # Load new stack pointer
    movq    (%rsi), %rsp
    
    # Restore callee-saved registers
    popq    %r15
    popq    %r14
    popq    %r13
    popq    %r12
    popq    %rbx
    popq    %rbp
    
    retq
.size context_switch, .-context_switch
```

### x86-32 Assembly Template
```assembly
# File: arch/i386/context_switch.S
# Modern i386 assembly with Clang compatibility

.section .text
.global context_switch
.type context_switch, @function

# Function: context_switch(old_ctx, new_ctx)
context_switch:
    pushl   %ebp
    movl    %esp, %ebp
    
    # Save callee-saved registers
    pushl   %ebx
    pushl   %esi
    pushl   %edi
    
    # Get parameters
    movl    8(%ebp), %eax   # old_ctx
    movl    12(%ebp), %edx  # new_ctx
    
    # Save current context
    movl    %esp, (%eax)
    
    # Load new context
    movl    (%edx), %esp
    
    # Restore registers
    popl    %edi
    popl    %esi
    popl    %ebx
    popl    %ebp
    
    ret
.size context_switch, .-context_switch
```

### Assembly Requirements
- **Documentation**: Every assembly function must have header comments
- **Calling Conventions**: Follow System V ABI for x86-64, cdecl for i386
- **Register Usage**: Document all register usage and preservation
- **Error Handling**: Use consistent error return conventions
- **Testing**: Every assembly function needs unit tests

## 📜 POSIX Compliance Requirements

### POSIX.1-2024 Compliance
```c
// Required feature test macros
#define _POSIX_C_SOURCE 202311L
#define _XOPEN_SOURCE 800
#define _DEFAULT_SOURCE 1

// POSIX-compliant error handling
#include <errno.h>
int result = some_function();
if (result == -1) {
    switch (errno) {
        case EINVAL:
            // Handle invalid argument
            break;
        case ENOMEM:
            // Handle out of memory
            break;
        default:
            // Handle other errors
            break;
    }
}
```

### Required POSIX Interfaces
- **System Calls**: fork(), exec(), wait(), signal handling
- **File Operations**: open(), read(), write(), close(), stat()
- **Process Management**: getpid(), getppid(), setuid(), setgid()
- **Memory Management**: mmap(), munmap(), mprotect()
- **Threading**: pthread API (when threading is implemented)
- **Networking**: socket(), bind(), listen(), accept()

### POSIX Compliance Testing
- All system calls must pass POSIX conformance tests
- Error codes must match POSIX specifications
- Signal behavior must be POSIX-compliant
- File system semantics must follow POSIX standards

## 📁 Code Organization Structure

### Directory Structure
```
/minix/
├── kernel/                 # Core kernel code
│   ├── arch/              # Architecture-specific code
│   │   ├── x86_64/        # 64-bit x86 code
│   │   ├── i386/          # 32-bit x86 code
│   │   ├── arm/           # ARM 32-bit code
│   │   └── aarch64/       # ARM 64-bit code
│   ├── include/           # Kernel headers
│   ├── ipc/               # Inter-process communication
│   ├── memory/            # Memory management
│   ├── process/           # Process management
│   ├── filesystem/        # VFS and filesystem support
│   └── drivers/           # Device drivers
├── libos/                 # OS libraries and userspace support
│   ├── libc/              # C library implementation
│   ├── libposix/          # POSIX compatibility layer
│   └── libsys/            # System call library
├── servers/               # System servers
│   ├── pm/                # Process manager
│   ├── vfs/               # Virtual file system
│   ├── vm/                # Virtual memory manager
│   └── net/               # Network server
├── drivers/               # Device drivers
│   ├── storage/           # Storage device drivers
│   ├── network/           # Network device drivers
│   └── input/             # Input device drivers
├── commands/              # System commands and utilities
├── tests/                 # Test suites
└── docs/                  # Documentation
```

### File Naming Conventions
- **C files**: `module_name.c` (lowercase with underscores)
- **Headers**: `module_name.h` (matching C file names)
- **Assembly**: `function_name.S` (uppercase .S for preprocessed assembly)
- **Makefiles**: `Makefile` or `meson.build`
- **Documentation**: `README.md`, `DESIGN.md`, etc.

## 📋 Header File Standards

### Header Template
```c
/**
 * @file module_name.h
 * @brief Brief description of module functionality
 * @details Detailed description of the module, its purpose,
 *          and how it fits into the overall system architecture.
 * @version 4.0.0
 * @date 2025-06-13
 * @author MINIX Development Team
 * @copyright Copyright (c) 2025 MINIX Project
 */

#pragma once

#define _POSIX_C_SOURCE 202311L

/* Standard C23 headers */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdatomic.h>

/* MINIX system headers */
#include <minix/kernel_types.h>

/* Forward declarations */
struct example_struct;

/* Constants and macros */
#define EXAMPLE_MAGIC       0x12345678
#define EXAMPLE_MAX_SIZE    1024

/* Type definitions */
typedef struct example_struct example_t;

/* Function prototypes */
int example_init(void);
void example_cleanup(void);
int example_operation(example_t *obj, uint32_t param);

/* Inline functions */
static inline bool example_is_valid(const example_t *obj) {
    return obj && obj->magic == EXAMPLE_MAGIC;
}
```

### Header Requirements
- **`#pragma once`**: Always use instead of include guards
- **Forward declarations**: Use to reduce dependencies
- **Const correctness**: Mark const parameters and return types
- **Documentation**: Every public function needs documentation
- **No implementation**: Headers contain only declarations (except inline)

## 🧠 Memory Management Standards

### Allocation Requirements
```c
// ✅ REQUIRED: Check all allocations
void *ptr = kmalloc(size);
if (ptr == NULL) {
    return -ENOMEM;
}

// ✅ REQUIRED: Zero sensitive data
memset(sensitive_data, 0, sizeof(sensitive_data));

// ✅ REQUIRED: Set pointers to NULL after freeing
kfree(ptr);
ptr = NULL;
```

### Memory Safety Rules
- **No memory leaks**: Every allocation must have corresponding free
- **No double free**: Set pointers to NULL after freeing
- **No use after free**: Validate pointers before use
- **Bounds checking**: Always validate array indices and buffer sizes
- **Alignment**: Respect architecture alignment requirements

### Memory Pool Usage
```c
// For frequent allocations of same-sized objects
kmem_pool_t *pool = kmem_pool_create(sizeof(struct object), 0, KMEM_ZERO);
struct object *obj = kmem_pool_alloc(pool);
// Use object...
kmem_pool_free(pool, obj);
```

## 🚨 Error Handling Standards

### Error Code Standards
```c
// Use standard errno values
#define K_OK            0       // Success
#define K_ERROR         -1      // Generic error (avoid when possible)
#define K_INVALID       -EINVAL // Invalid parameter
#define K_NOMEM         -ENOMEM // Out of memory
#define K_NOSYS         -ENOSYS // Not implemented
#define K_PERM          -EPERM  // Permission denied
```

### Error Handling Template
```c
int function_name(param_t *param) {
    // Validate parameters
    if (param == NULL) {
        return -EINVAL;
    }
    
    // Perform operation
    int result = some_operation(param);
    if (result < 0) {
        // Log error if appropriate
        kdebug("Operation failed: %d", result);
        return result;
    }
    
    return 0; // Success
}
```

### Panic Conditions
```c
// Only panic for unrecoverable errors
if (critical_invariant_violated) {
    panic("Critical system invariant violated in %s:%d", __FILE__, __LINE__);
}
```

## 📚 Documentation Standards

### Function Documentation
```c
/**
 * @brief Brief one-line description of the function
 * @details Detailed description of what the function does,
 *          any side effects, and important implementation notes.
 * 
 * @param[in] input_param Description of input parameter
 * @param[out] output_param Description of output parameter  
 * @param[in,out] inout_param Description of input/output parameter
 * 
 * @return Description of return value and possible error codes
 * @retval 0 Success
 * @retval -EINVAL Invalid parameter
 * @retval -ENOMEM Out of memory
 * 
 * @note Important notes about usage or behavior
 * @warning Warnings about potential issues
 * @see Related functions or documentation
 * @since Version when function was introduced
 * 
 * @example
 * ```c
 * example_t obj;
 * int result = example_function(&obj, 42);
 * if (result != 0) {
 *     // Handle error
 * }
 * ```
 */
```

### Code Comments
```c
/* Module-level comment explaining the purpose and design */

/**
 * @brief Structure representing a kernel object
 */
struct kernel_object {
    uint32_t magic;         /**< Magic number for validation */
    atomic_int ref_count;   /**< Reference count for object */
    /* ... */
};

int complex_function(void) {
    /* 
     * Step 1: Initialize data structures
     * This is necessary because...
     */
    
    // TODO: Optimize this algorithm for better performance
    
    /* 
     * FIXME: This workaround is needed due to hardware bug
     * Remove when new hardware revision is available
     */
}
```

## 🔨 Build System Standards

### Meson Build Configuration
```meson
project('minix4-kernel', 'c',
  version : '4.0.0',
  default_options : [
    'c_std=c23',
    'warning_level=3',
    'werror=true',
    'optimization=2',
    'debug=true'
  ]
)

# Compiler configuration
cc = meson.get_compiler('c')

# Required C23 features check
if not cc.has_header('stdatomic.h')
  error('C23 atomics support required')
endif

# Kernel-specific flags
kernel_cflags = [
  '-ffreestanding',
  '-nostdlib',
  '-fno-builtin',
  '-fno-stack-protector',
  '-mno-red-zone',  # x86-64 specific
  '-mcmodel=kernel' # x86-64 specific
]

# Architecture detection
arch = host_machine.cpu_family()
if arch == 'x86_64'
  kernel_cflags += ['-m64', '-DARCH_X86_64']
elif arch == 'x86'
  kernel_cflags += ['-m32', '-DARCH_I386']
elif arch.startswith('arm')
  kernel_cflags += ['-DARCH_ARM']
endif
```

### CMake Alternative
```cmake
cmake_minimum_required(VERSION 3.25)
project(minix4-kernel C ASM)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags
add_compile_options(
    -Wall -Wextra -Werror
    -ffreestanding -nostdlib
    -fno-builtin -fno-stack-protector
)

# Architecture-specific settings
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    add_compile_definitions(ARCH_X86_64)
    add_compile_options(-m64 -mcmodel=kernel -mno-red-zone)
endif()
```

## 🧪 Testing Standards

### Unit Test Template
```c
/**
 * @file test_module_name.c
 * @brief Unit tests for module_name functionality
 */

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "test_framework.h"
#include "module_name.h"

/* Test fixture */
static void setup_test(void) {
    // Initialize test environment
}

static void teardown_test(void) {
    // Clean up test environment
}

/* Test cases */
static void test_basic_functionality(void) {
    // Arrange
    example_t obj;
    
    // Act
    int result = example_init(&obj);
    
    // Assert
    assert(result == 0);
    assert(example_is_valid(&obj));
    
    // Cleanup
    example_cleanup(&obj);
}

static void test_error_conditions(void) {
    // Test NULL parameter
    int result = example_init(NULL);
    assert(result == -EINVAL);
    
    // Test invalid state
    // ...
}

/* Test suite */
int main(void) {
    RUN_TEST(test_basic_functionality);
    RUN_TEST(test_error_conditions);
    
    return test_results();
}
```

## 🔒 Security Standards

### Security Requirements
- **Input validation**: Validate all inputs at system boundaries
- **Buffer overflow protection**: Use bounds checking for all buffers
- **Integer overflow protection**: Check arithmetic operations
- **Capability-based security**: Use capabilities instead of UIDs
- **Memory protection**: Use virtual memory for isolation
- **Stack protection**: Non-executable stacks by default

### Secure Coding Practices
```c
// ✅ REQUIRED: Input validation
int secure_function(const char *input, size_t input_len) {
    if (input == NULL || input_len == 0 || input_len > MAX_INPUT_SIZE) {
        return -EINVAL;
    }
    
    // Ensure null termination
    if (input[input_len - 1] != '\0') {
        return -EINVAL;
    }
    
    // Process input safely...
}

// ✅ REQUIRED: Capability checking
int privileged_operation(capability_t required_cap) {
    if (!current_process_has_capability(required_cap)) {
        return -EPERM;
    }
    
    // Perform privileged operation...
}
```

---

## 🎯 Implementation Priorities

### Phase 1: Core Kernel (Current)
1. Complete process management consolidation
2. Finish memory management unification
3. Implement basic IPC mechanisms
4. Create minimal device driver framework

### Phase 2: System Services
1. Virtual file system implementation
2. Process manager service
3. Virtual memory manager service
4. Basic device drivers

### Phase 3: POSIX Layer
1. Complete POSIX system call interface
2. C library implementation
3. Shell and basic utilities
4. Networking stack

### Phase 4: Advanced Features
1. SMP support
2. Advanced security features
3. Performance optimizations
4. Extended hardware support

---

**Note**: This document is living and should be updated as the project evolves. All contributors must follow these standards to maintain code quality and project consistency.