# MINIX4 Implementation Roadmap & Action Plan
**Version 1.0 - June 13, 2025**  
**From Current Chaos to Clean Architecture**

---

## 🎯 CURRENT STATE → TARGET STATE

### WHERE WE ARE NOW ✅
- **44,000+ files** properly organized into functional categories
- **Legacy code** preserved in appropriate `*_legacy/` directories  
- **Root directory** cleaned of scattered files
- **Directory structure** established for MINIX4 development
- **Build system** framework in place (Makefile, meson.build)
- **Documentation** architecture defined

### WHERE WE'RE GOING 🎯
- **5-layer hybrid architecture** (Exokernel + Microkernel + LibOS + Rump + Userspace)
- **POSIX 2024 C23 compliance** throughout
- **Educational clarity** with progressive complexity
- **Working OS** that boots and runs applications
- **Modern security** with capability-based model

---

## 📋 IMMEDIATE NEXT STEPS (Next 30 Days)

### Week 1: Build System Integration
**Priority: Critical** 🔴

```bash
# 1. Update main Makefile to reflect new organization
# Target: minix318/Makefile
TASKS:
├── Update source paths to match new directory structure
├── Remove references to moved/deleted files  
├── Add proper dependency tracking
├── Create arch-specific build targets (x86_64, i386, arm64)
└── Test compilation of organized legacy code

# 2. Fix header dependencies  
# Target: All #include statements
TASKS:
├── Scan all C files for broken #include paths
├── Update paths to match new organization
├── Remove duplicate/conflicting headers
├── Create unified header hierarchy in minix4/include/
└── Validate all headers compile cleanly

# 3. Create modern meson.build configuration
# Target: minix318/meson.build
TASKS:
├── Configure C23 compiler requirements
├── Set up proper warning levels (-Wall -Wextra -Werror)
├── Add architecture detection (x86_64, i386, arm64)
├── Configure kernel-specific flags (-ffreestanding, -nostdlib)
└── Set up proper dependency tracking
```

### Week 2: Code Consolidation  
**Priority: High** 🟡

```bash
# 1. Complete proc.c consolidation
# Target: minix4/microkernel/core/process/proc_unified.c
TASKS:
├── Merge remaining proc.c variants from organized legacy
├── Implement unified process structure
├── Add C23 safety features (_Static_assert, bounds checking)
├── Update process creation/destruction functions
└── Add comprehensive documentation

# 2. Complete memory.c consolidation
# Target: minix4/microkernel/core/memory/memory_unified.c  
TASKS:
├── Merge memory management from organized legacy
├── Implement unified memory allocation
├── Add capability-based memory protection
├── Integrate with exokernel page allocation
└── Add memory debugging support

# 3. Signal handling unification
# Target: minix4/microkernel/core/signal/signal_unified.c
TASKS:
├── Merge all signal.c variants
├── Implement POSIX 2024 signal semantics
├── Add capability-based signal sending
├── Update signal delivery mechanisms
└── Add signal debugging/tracing
```

### Week 3: Exokernel Foundation
**Priority: Critical** 🔴

```bash
# 1. Create minimal exokernel boot
# Target: minix4/exokernel/arch/x86_64/boot/
TASKS:
├── Implement basic bootloader (multiboot2 compatible)
├── Set up initial page tables and GDT
├── Initialize capability system
├── Basic interrupt handling setup
└── Jump to microkernel initialization

# 2. Memory page allocator
# Target: minix4/exokernel/core/memory/page_alloc.c
TASKS:  
├── Implement physical page allocation
├── Add capability-based page access
├── Support different page sizes (4KB, 2MB, 1GB)
├── Add memory debugging/tracking
└── Interface with microkernel VM manager

# 3. Basic capability system
# Target: minix4/exokernel/core/capability/cap_system.c
TASKS:
├── Define capability structure and types
├── Implement capability creation/deletion
├── Add capability inheritance/delegation
├── Capability-based hardware access
└── Audit and logging support
```

### Week 4: IPC & Communication
**Priority: High** 🟡

```bash
# 1. Microkernel IPC implementation
# Target: minix4/microkernel/core/ipc/ipc_core.c
TASKS:
├── Design IPC message structure
├── Implement synchronous message passing
├── Add asynchronous notification support
├── Capability-based IPC authorization
└── Performance optimization (zero-copy where possible)

# 2. Basic system servers
# Target: minix4/microkernel/servers/
TASKS:
├── Process Manager (PM) skeleton
├── Virtual Memory Manager (VM) skeleton  
├── Device Server (DS) skeleton
├── Clock Server (CS) skeleton
└── Inter-server communication setup

# 3. Integration testing
# Target: tests/integration/
TASKS:
├── Basic boot test (exokernel → microkernel)
├── IPC communication test
├── Memory allocation test
├── Capability system test
└── System server startup test
```

---

## 🗓️ MONTHLY MILESTONES (Next 6 Months)

### Month 2: Core System Services
**Goal: Working microkernel with basic services**

```
DELIVERABLES:
├── ✅ Process Manager (PM) - process creation, scheduling
├── ✅ Virtual Memory Manager (VM) - address space management  
├── ✅ Device Server (DS) - device enumeration and access
├── ✅ Clock Server (CS) - time management and timers
├── ✅ Basic device drivers (console, keyboard, timer)
├── ✅ System call interface framework
└── ✅ Integration tests passing

SUCCESS CRITERIA:
├── System boots to microkernel
├── Basic processes can be created and scheduled
├── Memory allocation works reliably
├── Device access functions properly
└── IPC between servers operates correctly
```

### Month 3: File System Foundation
**Goal: Basic file system functionality via Rump**

```
DELIVERABLES:
├── ✅ Rump kernel integration framework
├── ✅ NetBSD FFS file system support
├── ✅ Virtual File System (VFS) server
├── ✅ Basic file operations (open, read, write, close)
├── ✅ Directory operations (mkdir, rmdir, readdir)
├── ✅ File system mounting/unmounting
└── ✅ File permission enforcement

SUCCESS CRITERIA:
├── Can mount and access file systems
├── Basic file operations work correctly
├── File permissions enforced properly
├── Directory traversal functions
└── No file system corruption under normal use
```

### Month 4: Network Stack Integration  
**Goal: Basic networking via Rump NetBSD stack**

```
DELIVERABLES:
├── ✅ NetBSD network stack integration
├── ✅ Socket interface implementation
├── ✅ TCP/IP protocol support
├── ✅ Ethernet driver framework
├── ✅ Basic network utilities (ping, netstat)
├── ✅ Network configuration tools
└── ✅ Security policy for network access

SUCCESS CRITERIA:
├── TCP/IP networking functional
├── Can ping external hosts
├── Socket programming works
├── Network drivers load properly
└── Basic network security enforced
```

### Month 5: LibOS Implementation
**Goal: POSIX 2024 C23 compliant LibOS**

```
DELIVERABLES:
├── ✅ Core libc implementation (stdio, stdlib, string, math)
├── ✅ POSIX process management (fork, exec, wait)
├── ✅ POSIX signal handling
├── ✅ POSIX IPC (pipes, message queues)
├── ✅ Basic threading support (pthreads)
├── ✅ Memory management (malloc, free, mmap)
└── ✅ Time and locale support

SUCCESS CRITERIA:
├── Basic C programs compile and run
├── POSIX process model works
├── Signal handling POSIX compliant
├── IPC mechanisms functional
└── Threading basics operational
```

### Month 6: User Space & Applications
**Goal: Usable system with essential applications**

```
DELIVERABLES:
├── ✅ Shell implementation (POSIX compliant)
├── ✅ Essential file utilities (ls, cp, mv, rm, cat, grep)
├── ✅ Process utilities (ps, top, kill)
├── ✅ Text editor (simple vi implementation)
├── ✅ Development tools (basic make, simple compiler frontend)
├── ✅ System administration tools
└── ✅ Demo applications and tutorials

SUCCESS CRITERIA:
├── Interactive shell works properly
├── Can manage files and directories
├── Can compile and run simple C programs
├── System administration possible
└── Educational demos functional
```

---

## 🔧 TECHNICAL IMPLEMENTATION DETAILS

### Build System Modernization

```bash
# Update main Makefile structure
# File: minix318/Makefile

# Architecture detection
ARCH ?= x86_64
ifeq ($(ARCH),x86_64)
    ARCH_FLAGS = -m64 -mcmodel=kernel -mno-red-zone
    ARCH_DIR = x86_64
else ifeq ($(ARCH),i386)  
    ARCH_FLAGS = -m32
    ARCH_DIR = i386
else ifeq ($(ARCH),arm64)
    ARCH_FLAGS = -march=armv8-a
    ARCH_DIR = arm64
endif

# Compiler settings
CC = clang
CFLAGS = -std=c23 -Wall -Wextra -Werror -ffreestanding -nostdlib
CFLAGS += -fno-builtin -fno-stack-protector $(ARCH_FLAGS)
CFLAGS += -DARCH_$(shell echo $(ARCH) | tr '[:lower:]' '[:upper:]')

# Directory structure
EXOKERNEL_DIR = minix4/exokernel
MICROKERNEL_DIR = minix4/microkernel  
LIBOS_DIR = minix4/libos
RUMP_DIR = minix4/rump
USERSPACE_DIR = userspace

# Source file discovery
EXOKERNEL_SOURCES = $(shell find $(EXOKERNEL_DIR) -name "*.c")
MICROKERNEL_SOURCES = $(shell find $(MICROKERNEL_DIR) -name "*.c") 
LIBOS_SOURCES = $(shell find $(LIBOS_DIR) -name "*.c")

# Build targets
all: exokernel microkernel libos userspace

exokernel:
    @echo "Building exokernel for $(ARCH)..."
    $(MAKE) -C $(EXOKERNEL_DIR) ARCH=$(ARCH)

microkernel: exokernel
    @echo "Building microkernel..."
    $(MAKE) -C $(MICROKERNEL_DIR) ARCH=$(ARCH)

libos: microkernel
    @echo "Building LibOS..."
    $(MAKE) -C $(LIBOS_DIR) ARCH=$(ARCH)

userspace: libos
    @echo "Building userspace..."
    $(MAKE) -C $(USERSPACE_DIR) ARCH=$(ARCH)

clean:
    find . -name "*.o" -delete
    find . -name "*.a" -delete
    rm -rf build/

.PHONY: all exokernel microkernel libos userspace clean
```

### Header Dependency Fixes

```bash
# Script to fix header dependencies
# File: scripts/automation/Fix_Header_Dependencies.ps1

# Scan all C files for #include statements
$sourceFiles = Get-ChildItem -Path "minix4" -Recurse -Filter "*.c"

foreach ($file in $sourceFiles) {
    $content = Get-Content $file.FullName
    $updatedContent = @()
    
    foreach ($line in $content) {
        if ($line -match '^#include\s*[<"]([^>"]+)[>"]') {
            $includePath = $Matches[1]
            
            # Update paths to new organization
            $newPath = switch -Regex ($includePath) {
                '^minix/' { "minix4/include/minix/" + ($includePath -replace '^minix/', '') }
                '^sys/' { "minix4/include/sys/" + ($includePath -replace '^sys/', '') }
                '^kernel/' { "minix4/microkernel/include/" + ($includePath -replace '^kernel/', '') }
                '^drivers/' { "minix4/exokernel/drivers/include/" + ($includePath -replace '^drivers/', '') }
                default { $includePath }
            }
            
            if ($newPath -ne $includePath) {
                $line = $line -replace [regex]::Escape($includePath), $newPath
                Write-Host "Updated: $($file.Name) - $includePath → $newPath"
            }
        }
        $updatedContent += $line
    }
    
    # Write updated content back to file
    $updatedContent | Set-Content $file.FullName
}
```

### Core File Consolidation

```c
// Example: Unified process management
// File: minix4/microkernel/core/process/proc_unified.c

/**
 * @file proc_unified.c
 * @brief Unified process management for MINIX4 microkernel
 * @details Consolidates all process management functionality from legacy
 *          implementations into a single, modern, C23-compliant module.
 */

#pragma once
#define _POSIX_C_SOURCE 202311L

#include <stdint.h>
#include <stddef.h>  
#include <stdbool.h>
#include <stdatomic.h>

#include <minix4/capability.h>
#include <minix4/memory.h>
#include <minix4/ipc.h>

/* Process states */
typedef enum {
    PROC_STATE_UNUSED = 0,
    PROC_STATE_EMBRYO,
    PROC_STATE_RUNNABLE, 
    PROC_STATE_RUNNING,
    PROC_STATE_SLEEPING,
    PROC_STATE_ZOMBIE
} proc_state_t;

/* Process structure - unified from all legacy variants */
typedef struct process {
    /* Process identification */
    pid_t pid;                    /**< Process ID */
    pid_t ppid;                   /**< Parent process ID */
    uint32_t magic;               /**< Magic number for validation */
    
    /* Process state */
    atomic_int state;             /**< Current process state */
    atomic_int ref_count;         /**< Reference count */
    
    /* Memory management */
    capability_t mem_cap;         /**< Memory capability */
    uintptr_t page_table;         /**< Page table physical address */
    size_t memory_size;           /**< Total memory allocated */
    
    /* Scheduling */
    int priority;                 /**< Process priority */
    uint64_t cpu_time;           /**< Total CPU time used */
    uint64_t last_run;           /**< Last run timestamp */
    
    /* IPC */
    capability_t ipc_cap;         /**< IPC capability */
    struct process *ipc_target;   /**< Current IPC target */
    
    /* File descriptors */
    capability_t fd_caps[MAX_FDS]; /**< File descriptor capabilities */
    
    /* Validation */
    _Static_assert(sizeof(struct process) <= 4096, "Process struct too large");
} process_t;

/* Function prototypes */
int proc_create(process_t **proc, capability_t parent_cap);
int proc_destroy(process_t *proc);
int proc_schedule(void);
process_t *proc_find(pid_t pid);
bool proc_is_valid(const process_t *proc);

/* Implementation follows... */
```

---

## 🎯 SUCCESS CRITERIA & TESTING

### Week-by-Week Testing Goals

```bash
# Week 1: Build System Testing
TESTS:
├── All organized legacy code compiles without errors
├── Header dependencies resolve correctly  
├── Architecture-specific builds work (x86_64, i386)
├── Meson build system generates correct build files
└── Clean build from scratch completes in <5 minutes

# Week 2: Code Consolidation Testing  
TESTS:
├── Unified proc.c passes all process management tests
├── Unified memory.c passes memory allocation tests
├── Unified signal.c passes POSIX signal tests
├── No memory leaks detected in unified code
└── Code coverage >80% for consolidated modules

# Week 3: Exokernel Testing
TESTS:
├── System boots to exokernel successfully
├── Capability system creates/destroys capabilities
├── Memory page allocation works correctly
├── Basic interrupt handling functions
└── Microkernel can be loaded and started

# Week 4: IPC Testing
TESTS:
├── Synchronous message passing works
├── Asynchronous notifications delivered
├── Capability-based IPC authorization enforced
├── System servers can communicate
└── IPC performance meets minimum requirements
```

### Quality Gates

```bash
# Code Quality Requirements
REQUIREMENTS:
├── All code must compile with -Wall -Wextra -Werror
├── All functions must have documentation headers
├── All public APIs must have examples
├── Code coverage must be >85%
├── Static analysis must pass (clang-analyzer, cppcheck)
├── Memory safety tools must pass (valgrind, AddressSanitizer)
└── Performance tests must meet minimum benchmarks

# Security Requirements  
REQUIREMENTS:
├── All capabilities must be validated before use
├── All user inputs must be bounds-checked
├── All memory allocations must check for overflow
├── All system calls must verify caller privileges
├── All IPC must be authenticated
└── Security audit must pass before release
```

---

## 📊 PROGRESS TRACKING

### Weekly Status Reports
Every Friday, create status report in `docs/reports/YYYY-MM-DD_weekly_status.md`:

```markdown
# Weekly Status Report - YYYY-MM-DD

## Completed This Week ✅
- [ ] Task 1: Description and outcome
- [ ] Task 2: Description and outcome
- [ ] Task 3: Description and outcome

## Blocked/Issues ⚠️  
- Issue 1: Description and impact
- Issue 2: Description and impact

## Next Week Goals 🎯
- [ ] Goal 1: Specific, measurable outcome
- [ ] Goal 2: Specific, measurable outcome
- [ ] Goal 3: Specific, measurable outcome

## Metrics 📊
- Lines of code: X
- Test coverage: X%
- Build time: X minutes
- Memory usage: X MB
```

### Milestone Tracking
Track major milestones in `docs/milestones/milestone_status.md`:

```markdown
# MINIX4 Milestone Status

## Phase 1: Foundation (Months 1-3)
- [x] Organization Complete (2025-06-13)
- [ ] Build System Updated (Target: 2025-06-20)
- [ ] Code Consolidation (Target: 2025-06-27)
- [ ] Exokernel Foundation (Target: 2025-07-04)
- [ ] IPC Implementation (Target: 2025-07-11)

## Phase 2: Core Services (Months 4-6)
- [ ] Process Manager (Target: 2025-08-01)
- [ ] Memory Manager (Target: 2025-08-15)
- [ ] Device Server (Target: 2025-09-01)
- [ ] File System (Target: 2025-09-15)

## Phase 3: Integration (Months 7-9)
- [ ] Rump Kernel Integration (Target: 2025-10-01)
- [ ] Network Stack (Target: 2025-11-01)
- [ ] LibOS Implementation (Target: 2025-12-01)

## Overall Progress: 15% Complete
```

---

## 🎉 CONCLUSION

This roadmap provides a **clear, actionable path** from our current organized state to a **fully functional MINIX4 operating system**. By following the **KISS principle** and maintaining **incremental progress**, we will build a **modern, educational OS** that serves as both a **learning platform** and a **research vehicle**.

**Key Success Factors:**
1. ✅ **Weekly incremental progress** with clear deliverables
2. ✅ **Continuous testing** and quality validation  
3. ✅ **Clear documentation** of all design decisions
4. ✅ **Regular community updates** and feedback incorporation
5. ✅ **Strict adherence** to architectural principles

**Next Action:** Execute Week 1 tasks - Build System Integration

---

**Document Version**: 1.0  
**Author**: MINIX4 Development Team  
**Date**: June 13, 2025  
**Status**: **APPROVED IMPLEMENTATION PLAN** ✅
