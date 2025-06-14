# MINIX4 Repository Consolidation Plan

## Current State Analysis
- **Total files**: ~44,000+ files scattered across multiple directories
- **Root level clutter**: Hundreds of .c/.h files mixed with build artifacts
- **Duplicate files**: Multiple versions of same-named files (main.c, proc.c, etc.)
- **Inconsistent organization**: Files spread across root, /minix/, /libos/, /kernel/
- **Build system chaos**: Multiple makefiles, build systems, and configurations

## Target Structure

```
minix318/
├── README.md                    # Main project documentation
├── LICENSE                      # Project license
├── Makefile                     # Unified build system
├── meson.build                  # Modern build configuration
├── .gitignore                   # Git ignore rules
├── docs/                        # Documentation
├── scripts/                     # Build and maintenance scripts
├── tools/                       # Development tools
└── minix/                       # MINIX4 operating system
    ├── kernel/                  # Microkernel core
    │   ├── main.c              # Kernel entry point (C23)
    │   ├── proc.c              # Process management
    │   ├── memory.c            # Memory management
    │   ├── signal.c            # Signal handling
    │   ├── ipc.c               # Inter-process communication
    │   ├── capability.c        # Capability system
    │   ├── scheduler.c         # Process scheduling
    │   ├── syscall.c           # System call interface
    │   ├── panic.c             # Error handling
    │   ├── debug.c             # Debug facilities
    │   ├── arch/               # Architecture-specific code
    │   │   ├── x86_64/         # x86-64 implementation
    │   │   ├── aarch64/        # ARM64 implementation
    │   │   └── riscv/          # RISC-V implementation
    │   └── include/            # Kernel headers
    │       ├── kernel/         # Internal kernel headers
    │       ├── arch/           # Architecture headers
    │       └── api/            # Public API headers
    ├── libos/                   # Operating system library
    │   ├── libc/               # C standard library
    │   ├── libm/               # Math library
    │   ├── libpthread/         # Threading library
    │   └── librt/              # Real-time extensions
    ├── servers/                 # System servers
    │   ├── vfs/                # Virtual file system
    │   ├── pm/                 # Process manager
    │   ├── rs/                 # Reincarnation server
    │   ├── vm/                 # Virtual memory server
    │   └── net/                # Network server
    ├── drivers/                 # Device drivers
    │   ├── storage/            # Storage drivers
    │   ├── network/            # Network drivers
    │   ├── display/            # Display drivers
    │   └── input/              # Input drivers
    ├── commands/                # System commands
    ├── tests/                   # Test suite
    └── fs/                     # File systems
        ├── mfs/                # MINIX file system
        ├── ext2/               # ext2 file system
        └── tmpfs/              # Temporary file system
```

## Consolidation Strategy

### Phase 1: Structure Creation and Core Files
1. Create proper directory structure
2. Consolidate all main.c files into kernel/main.c
3. Merge all proc.c variants into kernel/proc.c
4. Harmonize signal handling into kernel/signal.c
5. Unify memory management in kernel/memory.c

### Phase 2: Header File Harmonization
1. Merge duplicate header files
2. Create unified include hierarchy
3. Establish clear public/private APIs
4. Remove conflicting definitions

### Phase 3: Library Consolidation
1. Move all library code to libos/
2. Organize by function (libc, libm, etc.)
3. Remove duplicates and conflicts
4. Establish clear interfaces

### Phase 4: Driver and Server Organization
1. Consolidate device drivers by type
2. Organize system servers
3. Clean up IPC interfaces
4. Harmonize capability usage

### Phase 5: Build System Unification
1. Single unified Makefile
2. Modern meson.build configuration
3. Remove legacy build files
4. Automated testing integration

### Phase 6: Documentation and Testing
1. Update all documentation
2. Create comprehensive test suite
3. Performance benchmarking
4. Security validation

## File Mapping Strategy

### Kernel Core Files (Priority 1)
- `*/main.c` → `minix/kernel/main.c` (C23 modern)
- `*/proc.c` → `minix/kernel/proc.c` (unified process management)
- `*/memory.c`, `*/kalloc.c`, `*/malloc.c` → `minix/kernel/memory.c`
- `*/sig.c`, `*/signal.c` → `minix/kernel/signal.c`
- `*/ipc.c`, `*/msg*.c` → `minix/kernel/ipc.c`
- `*/capability.c`, `*/cap*.c` → `minix/kernel/capability.c`

### System Headers (Priority 2)
- All `types.h` → `minix/kernel/include/kernel/types.h`
- All `param.h` → `minix/kernel/include/kernel/param.h`
- All `defs.h` → `minix/kernel/include/kernel/defs.h`
- Architecture headers → `minix/kernel/include/arch/*/`

### Libraries (Priority 3)
- C library functions → `minix/libos/libc/`
- Math functions → `minix/libos/libm/`
- Threading → `minix/libos/libpthread/`
- POSIX compatibility → `minix/libos/libposix/`

## Quality Standards
- All code must be C23 compliant
- Comprehensive documentation (Doxygen)
- Modern error handling patterns
- Memory safety enforced
- Capability-based security throughout
- Performance optimized
- Thread-safe where applicable

## Timeline
- Phase 1-2: Core structure and kernel (immediate priority)
- Phase 3-4: Libraries and drivers (next priority)
- Phase 5-6: Build system and documentation (final priority)
