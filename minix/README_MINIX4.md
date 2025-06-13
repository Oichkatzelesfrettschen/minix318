# MINIX4 - Modern C23 Microkernel

## Overview

This is a modernized version of MINIX transformed into MINIX4, a C23-compliant microkernel with:

- **C23 Standard Compliance**: Uses modern C23 features including `stdnoreturn`, `stdatomic`, `_Generic`, and other improvements
- **Capability-Based Security**: Fine-grained access control using capabilities
- **Exokernel Principles**: Minimal abstractions with user-space flexibility
- **Modular Architecture**: Clean separation of concerns with initialization framework
- **Modern Build System**: LLVM/Clang-based build with Meson support

## Architecture

### Core Components

1. **Kernel Entry Point** (`main_modern.c`)
   - Modern C23 kernel initialization
   - Capability-aware boot process
   - Modular subsystem initialization

2. **Memory Management** (`memory.c`)
   - Modern memory allocators
   - Page-based allocation
   - Capability-controlled memory access

3. **Signal Handling** (`sig_modern.c`)
   - POSIX-compliant signal handling
   - Modern atomic operations
   - Capability-based signal permissions

4. **IPC System** (`ipc_modern.h`)
   - High-performance message passing
   - Zero-copy fast IPC
   - Capability transfer mechanism

5. **Capability System** (`capability.h`)
   - Fine-grained access control
   - Transferable and revokable capabilities
   - Type-safe capability operations

### Modern Features

#### C23 Language Features Used

- `stdnoreturn` for functions that don't return
- `stdatomic` for lock-free programming
- `stdalign` for memory alignment control
- `stdint.h` and `stddef.h` for standard types
- Generic macros using `_Generic`
- Improved initialization syntax
- Better type safety

#### Initialization Framework

```c
// Register initialization functions by level
early_initcall(console_init);
memory_initcall(palloc_init);
capability_initcall(cap_init);
ipc_initcall(ipc_init);
scheduler_initcall(sched_init);
device_initcall(device_init);
```

#### Capability System Example

```c
// Create a memory capability
cap_handle_t mem_cap = cap_create(CAP_MEMORY, region_id, 
                                 CAP_PERM_READ | CAP_PERM_WRITE,
                                 CAP_FLAG_TRANSFERABLE);

// Check permissions
if (cap_has_permission(mem_cap, CAP_PERM_WRITE)) {
    // Perform write operation
}

// Transfer capability to another process
cap_transfer(mem_cap, target_pid);
```

#### Modern IPC Example

```c
// Synchronous call
int result = ipc_call(server_endpoint, request_data, sizeof(request_data),
                     response_data, sizeof(response_data),
                     capabilities, num_caps, IPC_FLAG_RELIABLE);

// Asynchronous send
ipc_send(dest_endpoint, data, size, caps, num_caps, IPC_FLAG_URGENT);

// Fast IPC for high-performance communication
fast_ipc_send(dest, data, size);
```

## Building

### Requirements

- LLVM/Clang with C23 support
- Meson build system
- QEMU for testing (optional)

### Build Commands

```bash
# Configure build
meson setup build --cross-file=minix4-cross.txt

# Compile kernel
ninja -C build

# Run in QEMU
make -f Makefile.modern run

# Debug build
make -f Makefile.modern debug
```

## Key Improvements Over MINIX 3.x

### 1. Modern C Standards
- C23 compliance with modern language features
- Better type safety and memory safety
- Atomic operations for lock-free programming

### 2. Security
- Capability-based access control
- Fine-grained permissions
- Secure IPC with capability transfer

### 3. Performance
- Lock-free data structures where possible
- Zero-copy IPC mechanisms
- Modern memory allocators
- SMP-aware design

### 4. Modularity
- Clean initialization framework
- Pluggable subsystems
- Clear separation of concerns

### 5. Maintainability
- Extensive documentation
- Modern coding standards
- Modular design patterns

## System Architecture

```
┌─────────────────────────────────────────────────┐
│                User Space                        │
├─────────────────────────────────────────────────┤
│                System Calls                     │
├─────────────────────────────────────────────────┤
│  IPC System  │  Signal Handler │  Capability Mgr │
├─────────────────────────────────────────────────┤
│      Memory Manager      │      Scheduler        │
├─────────────────────────────────────────────────┤
│               Hardware Abstraction              │
└─────────────────────────────────────────────────┘
```

## Code Examples

### Process Creation with Capabilities

```c
struct proc *create_process(const char *binary, cap_handle_t *initial_caps, 
                           size_t num_caps)
{
    struct proc *p = proc_alloc();
    if (!p) return NULL;
    
    // Initialize signal handling
    signal_init_process(p);
    
    // Set up capability table
    cap_table_init(&p->cap_table, p->pid);
    
    // Add initial capabilities
    for (size_t i = 0; i < num_caps; i++) {
        cap_table_add(&p->cap_table, initial_caps[i]);
    }
    
    // Load binary and start process
    if (load_binary(p, binary) != 0) {
        proc_destroy(p);
        return NULL;
    }
    
    sched_add_process(p);
    return p;
}
```

### Modern Signal Handling

```c
// Set up signal handler with modern syntax
void signal_handler(int sig, struct siginfo *info, void *context)
{
    switch (sig) {
        case SIGTERM:
            cleanup_and_exit();
            break;
        case SIGUSR1:
            handle_user_signal(info->si_value);
            break;
    }
}

// Register handler
sys_signal(SIGTERM, signal_handler);

// Block signals during critical section
uint64_t old_mask;
uint64_t block_mask = (1UL << SIGTERM) | (1UL << SIGINT);
sys_sigprocmask(SIG_BLOCK, &block_mask, &old_mask);

// Critical section
do_critical_work();

// Restore signal mask
sys_sigprocmask(SIG_SETMASK, &old_mask, NULL);
```

## Future Enhancements

1. **Rust Integration**: Mixed C23/Rust kernel modules
2. **WASM Support**: WebAssembly user-space programs
3. **Container Support**: Modern containerization primitives
4. **Network Stack**: Modern TCP/IP with capability-based networking
5. **GPU Support**: Capability-controlled GPU access
6. **Real-time Extensions**: Hard real-time scheduling

## Contributing

Please ensure all code follows C23 standards and includes:
- Comprehensive documentation
- Unit tests where applicable
- Capability-aware design
- Modern safety practices

## License

BSD-3-Clause (maintaining MINIX compatibility)
