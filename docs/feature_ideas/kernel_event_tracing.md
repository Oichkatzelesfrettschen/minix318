# Feature Idea: Enhance `trace` with Kernel Event Tracing

## Description

Extend the existing `trace` utility (located in `minix/usr.bin/trace/`) to not only capture system calls but also specific kernel events. This could include:

*   Scheduler events (e.g., context switches)
*   Interrupt handling events
*   Custom tracepoints that can be inserted into the kernel code

## Rationale

This feature would be invaluable for debugging and understanding the Minix kernel's behavior, especially during its ongoing stabilization and development. The `docs/kernel_refactor_overview.md` document explicitly mentions a lack of robust debugging infrastructure. Capturing kernel events alongside system calls would provide a much more comprehensive view of the system's execution, aiding in identifying performance bottlenecks, race conditions, and other subtle bugs.

## Key Impacted Areas

*   **`minix/usr.bin/trace/`**: Requires modifications to the userspace `trace` tool to parse and display the new kernel event data. This might involve new command-line options to control kernel event tracing and new output formats.
*   **`minix/kernel/`**: Needs significant additions to:
    *   Implement a mechanism for recording kernel events (e.g., a ring buffer).
    *   Define specific event types and their associated data.
    *   Provide an interface (e.g., a new system call or a virtual file in `/proc`) for the `trace` utility to retrieve kernel events.
    *   Potentially add infrastructure for dynamically enabling/disabling tracepoints.
*   **`minix/include/minix/`**: Will require new header files for definitions related to kernel event types, data structures used to pass event information from kernel to userspace, and any new system call interfaces.

## Prerequisite

Significant work is needed to stabilize the kernel (as detailed in `docs/kernel_refactor_overview.md`) before the kernel-side modifications for this feature can be reliably implemented. This includes, but is not limited to:

*   Completing outstanding FIXMEs in the kernel code.
*   Implementing a real, robust `kprintf` for kernel-level logging and debugging.
*   Establishing a proper Hardware Abstraction Layer (HAL) to ensure portability and cleaner interfaces for accessing hardware-specific tracing capabilities (if any).

Without these foundational improvements, adding complex features like kernel event tracing would be premature and could introduce further instability.
