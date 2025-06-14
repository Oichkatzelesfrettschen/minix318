# MINIX4 Definitive Architecture & Design Specification
**Version 1.0 - June 13, 2025**  
**The Simple, Coherent Vision**

---

## 🎯 EXECUTIVE SUMMARY

**MINIX4** is a modern, educational operating system implementing a **5-layer hybrid architecture** that combines the best aspects of exokernels, microkernels, and library operating systems while maintaining **KISS principles** (Keep It Simple, Stupid).

### Core Design Philosophy
1. **Educational First**: Clear, understandable code that teaches OS concepts
2. **Security by Design**: Capability-based security throughout
3. **POSIX 2024 Compliance**: Full modern standards support
4. **C23 Modern**: Latest C standard with safety features
5. **Simple & Clean**: Avoid complexity for complexity's sake

---

## 🏗️ THE MINIX4 5-LAYER ARCHITECTURE

```
┌─────────────────────────────────────────────────────────────────┐
│                        USER APPLICATIONS                        │
│  (standard apps, dev tools, games, editors, compilers, etc.)   │
└─────────────────────────────────────────────────────────────────┘
                                    ▲
                                    │ POSIX API
                                    ▼
┌─────────────────────────────────────────────────────────────────┐
│                  LAYER 5: LIBOS ENVIRONMENTS                   │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌───────────┐ │
│  │POSIX LibOS  │ │ UNIX LibOS  │ │  RT LibOS   │ │Custom Env │ │
│  │(full POSIX  │ │(compatibility│ │(real-time)  │ │(embedded) │ │
│  │2024 C23)    │ │  layer)     │ │             │ │           │ │
│  └─────────────┘ └─────────────┘ └─────────────┘ └───────────┘ │
└─────────────────────────────────────────────────────────────────┘
                                    ▲
                                    │ libOS API
                                    ▼
┌─────────────────────────────────────────────────────────────────┐
│                LAYER 4: RUMP KERNEL SUBSYSTEMS                 │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌───────────┐ │
│  │   NetBSD    │ │   NetBSD    │ │   NetBSD    │ │   NetBSD  │ │
│  │ Filesystems │ │  Network    │ │   Crypto    │ │  Device   │ │
│  │  (FFS,LFS)  │ │   Stack     │ │   Subsys    │ │  Drivers  │ │
│  └─────────────┘ └─────────────┘ └─────────────┘ └───────────┘ │
└─────────────────────────────────────────────────────────────────┘
                                    ▲
                                    │ rump API
                                    ▼
┌─────────────────────────────────────────────────────────────────┐
│                 LAYER 3: MICROKERNEL SERVICES                  │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌───────────┐ │
│  │   Process   │ │   Virtual   │ │    File     │ │  Network  │ │
│  │   Manager   │ │   Memory    │ │   System    │ │  Server   │ │
│  │    (PM)     │ │ Manager(VM) │ │   (VFS)     │ │   (NET)   │ │
│  └─────────────┘ └─────────────┘ └─────────────┘ └───────────┘ │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌───────────┐ │
│  │   Device    │ │  Security   │ │    I/O      │ │   Clock   │ │
│  │   Server    │ │  Manager    │ │   Server    │ │  Server   │ │
│  │    (DS)     │ │    (SM)     │ │   (IOS)     │ │   (CS)    │ │
│  └─────────────┘ └─────────────┘ └─────────────┘ └───────────┘ │
└─────────────────────────────────────────────────────────────────┘
                                    ▲
                                    │ IPC + syscalls
                                    ▼
┌─────────────────────────────────────────────────────────────────┐
│                  LAYER 2: MICROKERNEL CORE                     │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌───────────┐ │
│  │    IPC      │ │  Process    │ │  Memory     │ │  Signal   │ │
│  │  Subsystem  │ │ Scheduling  │ │ Protection  │ │ Handling  │ │
│  │(msg passing)│ │   & Sync    │ │ & Address   │ │& Context  │ │
│  │             │ │             │ │  Spaces     │ │ Switching │ │
│  └─────────────┘ └─────────────┘ └─────────────┘ └───────────┘ │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌───────────┐ │
│  │ Capability  │ │  Security   │ │   Clock &   │ │   Debug   │ │
│  │   System    │ │ Enforcement │ │   Timers    │ │ & Trace   │ │
│  │             │ │             │ │             │ │           │ │
│  └─────────────┘ └─────────────┘ └─────────────┘ └───────────┘ │
└─────────────────────────────────────────────────────────────────┘
                                    ▲
                                    │ hardware abstraction
                                    ▼
┌─────────────────────────────────────────────────────────────────┐
│                  LAYER 1: EXOKERNEL FOUNDATION                 │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌───────────┐ │
│  │   Memory    │ │    CPU      │ │   Device    │ │ Interrupt │ │
│  │ Management  │ │ Multiplexing│ │   Access    │ │ Handling  │ │
│  │(page alloc, │ │(scheduling, │ │ (secure HW  │ │(IRQ mgmt, │ │
│  │ protection) │ │  contexts)  │ │  exposure)  │ │  routing) │ │
│  └─────────────┘ └─────────────┘ └─────────────┘ └───────────┘ │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌───────────┐ │
│  │   Security  │ │    Boot     │ │    Arch     │ │   Low     │ │
│  │ Enforcement │ │   & Init    │ │  Specific   │ │   Level   │ │
│  │(capabilities│ │             │ │    Code     │ │ Drivers   │ │
│  │   & access) │ │             │ │(x86,ARM,etc)│ │           │ │
│  └─────────────┘ └─────────────┘ └─────────────┘ └───────────┘ │
└─────────────────────────────────────────────────────────────────┘
                                    ▲
                                    │ direct hardware access
                                    ▼
┌─────────────────────────────────────────────────────────────────┐
│                           HARDWARE                              │
│     (x86-32/64, ARM32/64, RISC-V, etc.)                       │
└─────────────────────────────────────────────────────────────────┘
```

---

## 📁 UNIFIED DIRECTORY STRUCTURE

Based on our organization guidelines and KISS principles:

```
minix318/                                   # Project root
├── .github/                                # GitHub config & CI/CD
│   ├── instructions/                       # LLM development standards
│   └── workflows/                          # Build & test automation
├── docs/                                   # All documentation
│   ├── architecture/                       # This file and arch docs
│   ├── api/                                # API documentation
│   ├── tutorials/                          # Learning materials
│   └── specifications/                     # Standards & specs
├── scripts/                                # Build & maintenance scripts
├── tools/                                  # Development & analysis tools
├── tests/                                  # System-wide tests
├── archive/                                # Historical code preservation
│
## CORE MINIX4 SYSTEM ##
├── minix4/                                 # Main MINIX4 implementation
│   │
│   ├── exokernel/                          # LAYER 1: Hardware Foundation
│   │   ├── arch/                           # Architecture-specific code
│   │   │   ├── x86_64/                     # 64-bit Intel/AMD
│   │   │   │   ├── boot/                   # Boot code & loaders
│   │   │   │   ├── memory/                 # Memory management
│   │   │   │   ├── interrupt/              # IRQ & exception handling
│   │   │   │   ├── context/                # Context switching
│   │   │   │   └── drivers/                # Low-level HW drivers
│   │   │   ├── i386/                       # 32-bit Intel/AMD
│   │   │   ├── arm32/                      # 32-bit ARM
│   │   │   ├── arm64/                      # 64-bit ARM (AArch64)
│   │   │   └── common/                     # Shared arch code
│   │   ├── core/                           # Core exokernel services
│   │   │   ├── capability/                 # Capability system
│   │   │   ├── memory/                     # Page allocation & protection
│   │   │   ├── security/                   # Access control enforcement
│   │   │   └── init/                       # Boot & initialization
│   │   ├── drivers/                        # Hardware abstraction drivers
│   │   │   ├── storage/                    # Disk, SSD, etc.
│   │   │   ├── network/                    # Ethernet, WiFi, etc.
│   │   │   ├── input/                      # Keyboard, mouse, touch
│   │   │   ├── display/                    # Graphics & framebuffer
│   │   │   └── platform/                   # Platform-specific drivers
│   │   └── include/                        # Exokernel headers
│   │
│   ├── microkernel/                        # LAYER 2-3: System Services
│   │   ├── core/                           # LAYER 2: Microkernel core
│   │   │   ├── ipc/                        # Inter-process communication
│   │   │   ├── process/                    # Process management & scheduling
│   │   │   ├── memory/                     # Virtual memory management
│   │   │   ├── signal/                     # Signal handling
│   │   │   ├── security/                   # Security policy enforcement
│   │   │   ├── clock/                      # Time & timer management
│   │   │   └── debug/                      # Debug & tracing support
│   │   ├── servers/                        # LAYER 3: System servers
│   │   │   ├── pm/                         # Process Manager
│   │   │   ├── vm/                         # Virtual Memory Manager
│   │   │   ├── vfs/                        # Virtual File System
│   │   │   ├── net/                        # Network Server
│   │   │   ├── ds/                         # Device Server
│   │   │   ├── sm/                         # Security Manager
│   │   │   ├── ios/                        # I/O Server
│   │   │   └── cs/                         # Clock Server
│   │   └── include/                        # Microkernel headers
│   │
│   ├── rump/                               # LAYER 4: NetBSD Subsystems
│   │   ├── fs/                             # File systems (FFS, LFS, ext2, etc.)
│   │   ├── net/                            # Network stack (TCP/IP, routing)
│   │   ├── crypto/                         # Cryptographic subsystems
│   │   ├── dev/                            # Device framework
│   │   └── include/                        # Rump kernel headers
│   │
│   ├── libos/                              # LAYER 5: Library Operating Systems
│   │   ├── posix/                          # Primary POSIX 2024 C23 LibOS
│   │   │   ├── libc/                       # C standard library
│   │   │   │   ├── stdio/                  # Standard I/O
│   │   │   │   ├── stdlib/                 # Standard library functions
│   │   │   │   ├── string/                 # String manipulation
│   │   │   │   ├── math/                   # Mathematical functions
│   │   │   │   ├── time/                   # Time & date functions
│   │   │   │   ├── locale/                 # Localization support
│   │   │   │   └── pthread/                # POSIX threads
│   │   │   ├── syscalls/                   # System call interface
│   │   │   ├── posix/                      # POSIX-specific functionality
│   │   │   │   ├── signals/                # POSIX signal handling
│   │   │   │   ├── ipc/                    # POSIX IPC (pipes, fifos, etc.)
│   │   │   │   ├── process/                # POSIX process management
│   │   │   │   └── io/                     # POSIX I/O extensions
│   │   │   └── compat/                     # Compatibility layers
│   │   ├── unix/                           # Classic UNIX compatibility LibOS
│   │   ├── realtime/                       # Real-time LibOS
│   │   ├── embedded/                       # Embedded systems LibOS
│   │   └── include/                        # LibOS headers
│   │
│   └── include/                            # Global MINIX4 headers
│       ├── minix/                          # MINIX-specific definitions
│       ├── sys/                            # System headers
│       └── arch/                           # Architecture headers
│
## USER SPACE ##
├── userspace/                              # User applications & tools
│   ├── applications/                       # Standard applications
│   │   ├── shells/                         # Command shells (sh, bash, etc.)
│   │   ├── editors/                        # Text editors (vi, nano, etc.)
│   │   ├── compilers/                      # Development tools (gcc, clang)
│   │   ├── utilities/                      # System utilities
│   │   └── games/                          # Educational games & demos
│   ├── commands/                           # System commands
│   │   ├── core/                           # Essential commands (ls, cp, etc.)
│   │   ├── admin/                          # Administrative tools
│   │   ├── network/                        # Network utilities
│   │   └── development/                    # Development commands
│   ├── libraries/                          # User-space libraries
│   │   ├── gui/                            # GUI frameworks
│   │   ├── multimedia/                     # Audio/video libraries
│   │   └── utility/                        # Utility libraries
│   └── demos/                              # Educational demonstrations
│
## LEGACY PRESERVATION ##
├── legacy/                                 # Organized legacy code
│   ├── minix_classic/                      # Original MINIX preservation
│   ├── minix3/                             # MINIX 3 preservation
│   └── historical/                         # Historical UNIX code
│
## BUILD & TESTING ##
├── build/                                  # Build outputs (generated)
├── Makefile                                # Unified build system
├── meson.build                             # Modern build configuration
└── CMakeLists.txt                          # Alternative build system
```

---

## 🔄 LAYER INTERACTION FLOW

### Information Flow (Bottom-Up)
1. **Hardware → Exokernel**: Raw hardware access and multiplexing
2. **Exokernel → Microkernel**: Capability-mediated resource access
3. **Microkernel → Rump**: System service interfaces
4. **Rump → LibOS**: Rich subsystem functionality
5. **LibOS → Applications**: POSIX/standard APIs

### Control Flow (Top-Down)
1. **Application** makes system call
2. **LibOS** interprets and may handle locally or forward
3. **Rump Kernel** provides rich subsystem functionality
4. **Microkernel** coordinates system services
5. **Exokernel** performs hardware operations

---

## ⚙️ KEY DESIGN PRINCIPLES

### 1. Educational Clarity
- **One concept per module**: Each directory has a single, clear purpose
- **Progressive complexity**: Start simple, add features incrementally
- **Extensive documentation**: Every component thoroughly documented
- **Clear interfaces**: Well-defined APIs between layers

### 2. Modern Standards Compliance
- **C23 Language**: Latest C standard with safety features
- **POSIX 2024**: Full POSIX.1-2024 compliance in LibOS layer
- **Unicode Support**: UTF-8 throughout, proper internationalization
- **Security Standards**: Capability-based security, memory safety

### 3. Performance & Efficiency
- **Zero-Copy I/O**: Minimize data copying between layers
- **Fast IPC**: Optimized message passing for microkernel
- **Capability Caching**: Efficient capability system
- **Lazy Loading**: Load components only when needed

### 4. Maintainability & Simplicity
- **KISS Principle**: Keep each component as simple as possible
- **Minimal Dependencies**: Reduce coupling between layers
- **Clean Separation**: Clear boundaries between layers
- **Consistent Coding**: Unified style and conventions

---

## 🛠️ IMPLEMENTATION ROADMAP

### Phase 1: Foundation (Months 1-3)
**Exokernel Core Implementation**
```
Priority 1: Boot & Initialization
├── x86_64 boot loader
├── Basic memory management  
├── Interrupt handling
└── Capability system foundation

Priority 2: Hardware Abstraction
├── Memory page allocation
├── CPU context switching
├── Basic device drivers
└── Security enforcement
```

### Phase 2: Microkernel Services (Months 4-6)  
**Core System Services**
```
Priority 1: IPC & Process Management
├── Message passing system
├── Process creation & scheduling
├── Virtual memory management
└── Signal handling

Priority 2: System Servers
├── Process Manager (PM)
├── Virtual Memory Manager (VM) 
├── Device Server (DS)
└── Clock Server (CS)
```

### Phase 3: Rump Integration (Months 7-9)
**NetBSD Subsystem Integration**
```
Priority 1: File Systems
├── Basic FFS integration
├── Virtual File System (VFS)
├── File operations
└── Directory management

Priority 2: Network Stack
├── TCP/IP stack integration
├── Socket interface
├── Network drivers
└── Routing support
```

### Phase 4: LibOS Implementation (Months 10-12)
**POSIX 2024 LibOS**
```
Priority 1: Core LibC
├── Standard I/O (stdio)
├── Memory management (stdlib)
├── String operations
└── Mathematical functions

Priority 2: POSIX Extensions  
├── Process management (fork, exec, wait)
├── Signal handling
├── IPC (pipes, message queues)
└── Threading (pthreads)
```

### Phase 5: User Space (Months 13-15)
**Applications & Tools**
```
Priority 1: Essential Commands
├── Shell (sh)
├── File utilities (ls, cp, mv, rm)
├── Text processing (cat, grep, sed)
└── System utilities (ps, top, kill)

Priority 2: Development Tools
├── C compiler (clang/gcc port)
├── Text editor (vi)
├── Build tools (make)
└── Debugger (gdb port)
```

---

## 🎓 EDUCATIONAL BENEFITS

### For Students
- **Progressive Learning**: Start with simple exokernel, build up complexity
- **Clear Architecture**: Each layer has distinct, understandable purpose  
- **Modern Practices**: Learn current best practices, not legacy approaches
- **Hands-On Experience**: Modify and extend real OS components

### For Researchers
- **Hybrid Architecture**: Study combination of exokernel + microkernel benefits
- **Performance Analysis**: Compare layer overhead vs. functionality
- **Security Research**: Capability-based security implementation
- **Systems Design**: Clean separation of concerns example

### For Developers
- **Modern Standards**: POSIX 2024, C23, Unicode, security standards
- **Clean Code**: Well-organized, documented, maintainable codebase
- **Practical Skills**: Real-world systems programming experience
- **Open Development**: Contribute to active educational project

---

## 🔒 SECURITY MODEL

### Capability-Based Security
- **Principle of Least Privilege**: Components get minimal required capabilities
- **Capability Propagation**: Controlled delegation between layers
- **Hardware Enforcement**: Exokernel enforces all access controls
- **Audit Trail**: All capability operations logged for analysis

### Memory Safety
- **C23 Safety Features**: `_Static_assert`, bounds checking, overflow protection
- **Capability-Guarded Memory**: All memory access through capabilities
- **Non-Executable Stacks**: Hardware NX bit enforcement
- **Address Space Isolation**: Each process in separate address space

### Communication Security
- **Authenticated IPC**: All inter-layer communication authenticated
- **Encrypted Storage**: Optional file system encryption
- **Network Security**: TLS/SSL support in network stack
- **Secure Boot**: Verified boot process with capability initialization

---

## 📈 SUCCESS METRICS

### Technical Goals
- **POSIX Compliance**: Pass >95% of POSIX conformance tests
- **Performance**: Within 10% of monolithic kernel performance
- **Memory Usage**: <16MB kernel footprint on x86_64
- **Boot Time**: <3 seconds on modern hardware

### Educational Goals  
- **Comprehensibility**: New students understand architecture in <1 week
- **Modifiability**: Students can add features without breaking system
- **Documentation**: 100% API coverage, extensive tutorials
- **Community**: Active community of students and researchers

### Maintenance Goals
- **Code Quality**: <0.1 bugs per 1000 lines of code
- **Build System**: Single-command build from clean source
- **Testing**: >90% code coverage, automated testing
- **Standards**: 100% compliance with project coding standards

---

## 🎯 CONCLUSION

MINIX4 represents a **simple, educational, modern operating system** that combines the best aspects of exokernels, microkernels, and library operating systems. By following **KISS principles** and maintaining **clear architectural boundaries**, we create a system that is:

- ✅ **Easy to understand** for students
- ✅ **Powerful enough** for real use  
- ✅ **Modern and secure** by design
- ✅ **Maintainable and extensible** for the future

The 5-layer architecture provides clear separation of concerns while the unified directory structure supports efficient development and learning. This is the definitive vision for MINIX4 development.

---

**Document Version**: 1.0  
**Author**: MINIX4 Development Team  
**Date**: June 13, 2025  
**Status**: **APPROVED ARCHITECTURAL SPECIFICATION** ✅
