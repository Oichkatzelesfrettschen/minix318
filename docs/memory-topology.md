# MINIX318 Memory Allocation Topology

## Executive Summary
- **Initial Scan Scope**: Analysis primarily focused on a selection of 6 key allocator files, with an initial codebase scan identifying approximately 176 potential C files related to allocation/deallocation.
- **Key Allocator Types Identified**:
    - Kernel: Main kalloc (Mach-derived zone allocator), Bootstrap kalloc (spinlocks, early boot), simple legacy kernel malloc.
    - Userspace: Comprehensive libc malloc (libmalloc/common/malloc.c) with threading and various features.
    - Root-level: Older UCB-style allocators (`./alloc.c`, legacy part of `./malloc.c`), with one `./malloc.c` acting as a wrapper to kernel's kalloc.
- **Observed Dependency Patterns**:
    - Userspace `malloc` relies on `sbrk`.
    - Kernel `kalloc` uses zones and `kmem_alloc_wired` (VM page allocation).
    - Bootstrap `kalloc` uses its own free lists and can map VM pages.
    - Some allocators are self-contained (e.g., `./alloc.c`, old kernel `malloc.c`).
- **Feature Variations**: Allocators exhibit diverse features regarding threading (mutexes, spinlocks, or none), debugging (asserts, logging), alignment, and zeroing.
- **Redundancy Factor**: [Percentage TBD after full analysis] - Significant diversity observed, suggesting opportunities for unification.
- **Generated Artifacts**: Call count metrics, feature analysis of selected files, raw allocation site listings, Cscope dependency traces, and DOT/SVG graphs for hierarchy and kernel flow.

*(This report is based on the initial 'Archaeological Excavation' phase. Further analysis of all identified allocator files is needed for a complete picture.)*

---
# MINIX318 Memory Allocation Topology

*Further categorization and details for other allocators from `all-allocators.txt` will be added here.*
## Dependency Analysis (Initial Draft)

This section outlines observed dependencies between major allocators and their users.
Based on cscope analysis of key functions.

### Key Allocator Callers & Callees:

1.  **Root `./malloc.c` / `./alloc.c`:**
    *   The `./malloc.c` functions (`malloc`, `free`, `realloc`) are primarily wrappers calling `kalloc`, `kfree`, `krealloc` respectively.
    *   The legacy code in `./malloc.c` (and `./alloc.c` which is similar) calls `sbrk` to extend the arena.
    *   Called by various older utilities and potentially some parts of `libos` if not overridden.

2.  **Userspace `./minix/lib/libmalloc/common/malloc.c`:**
    *   This is a feature-rich allocator used widely by userspace programs.
    *   Implements `malloc`, `free`, `realloc`, `calloc`, `memalign`, `valloc`.
    *   Uses `sbrk` (via `morecore`) to get memory from the kernel.
    *   Internally uses mutexes (`mutex_lock`, `mutex_unlock`) for thread safety.
    *   Manages holding blocks for small allocations.

3.  **Kernel `./minix/kernel/kalloc.c` (Mach-derived):**
    *   Provides `kalloc`, `kfree`, `kget`.
    *   Built on top of a zone allocator (`zalloc`, `zfree`) for smaller sizes.
    *   Uses `kmem_alloc_wired` (from `vm/vm_kern.h`) for larger allocations or when zones are exhausted.
    *   Called by various kernel components like IPC (`ipc_kmsg_copyin`), VM system (`walkpgdir`, `setupkvm`), and drivers (`iopb_create`).

4.  **Kernel Bootstrap `./minix/kernel/bootstrap/kalloc.c`:**
    *   Provides `kalloc`, `kfree`, also defines `malloc`, `realloc`, `free` as wrappers around its own bootstrap allocators.
    *   Uses spinlocks for synchronization (`spin_lock_init`, `spin_lock`, `spin_unlock`).
    *   Manages its own free lists (`kfree_list`) and gets space via `kget_space` which can call `vm_map` (from `mach/vm_map.h`).
    *   Crucial for early kernel initialization, including the default pager.

5.  **Simple Kernel `./minix/kernel/malloc.c`:**
    *   A very basic allocator using a `struct map` for coremap/swapmap.
    *   Provides `malloc(struct map *mp, size)` and `mfree(struct map *mp, size, addr)`.
    *   Likely used in specific, older parts of the kernel that manage memory via this map structure.

*(More detailed call graph information is in cscope-dependency-analysis.txt and will be refined)*
