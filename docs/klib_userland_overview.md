# Kernel and Userland Libraries

This document summarizes the conceptual model and example
implementation steps for keeping a minimal **klib** inside the kernel
while a full **libc** remains available in userland.

## 1. Conceptual Foundations

### 1.1 Kernel C library (klib)

* **Purpose**: minimal runtime support within the kernel without system calls
* **Typical contents**: memory and string routines
* **Constraints**: reentrant, no dynamic allocation, tiny footprint

### 1.2 Userland C library (libc)

* **Purpose**: full ISO C/POSIX API wrapping system calls
* **Typical contents**: syscall wrappers, memory allocators and stdio
* **Constraints**: may rely on dynamic memory and floating point

### 1.3 Why keep them separate?

1. Isolation between kernel and user code
2. Size and safety of klib
3. Different linkage models

## 2. Implementation Walkthrough (Prose Only)

1. Create `klib/` under the kernel source tree
2. Write minimal routines (`memcpy.c`, `strlen.c`, ...)
3. Provide a header `klib.h` declaring these APIs
4. Add the objects to the kernel build system
5. Build the kernel
6. Modify `libc` in userland under `src/lib/libc/`
7. Rebuild userland so programs link against the updated libc

## 3. Example `klib` Source

```c
/**
 * @file klib.c
 * @brief Minimal C runtime support for the MINIX kernel.
 */
#include <stddef.h>
#include <stdint.h>
#include "klib.h"

void *klib_memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = dst;
    const uint8_t *s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dst;
}

void *klib_memset(void *dst, int c, size_t n) {
    uint8_t *d = dst;
    while (n--) {
        *d++ = (uint8_t)c;
    }
    return dst;
}

int klib_memcmp(const void *a, const void *b, size_t n) {
    const uint8_t *pa = a, *pb = b;
    while (n--) {
        if (*pa != *pb) {
            return (int)*pa - (int)*pb;
        }
        pa++;
        pb++;
    }
    return 0;
}

size_t klib_strlen(const char *s) {
    const char *p = s;
    while (*p) {
        p++;
    }
    return (size_t)(p - s);
}

int klib_strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return (int)(unsigned char)*a - (int)(unsigned char)*b;
}
```

