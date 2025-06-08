#ifndef MINIX_KLIB_H
#define MINIX_KLIB_H

#include <stddef.h> // For size_t
#include <stdbool.h> // For bool type

// Memory functions
void *kmemcpy_c23(void * restrict dest, const void * restrict src, size_t n);
void *kmemset_c23(void *dest, int c, size_t n);
void *kmemmove_c23(void *dest, const void *src, size_t n);

// String functions
size_t kstrlen_c23(const char *s);
size_t kstrlcpy_c23(char * restrict dest, const char * restrict src, size_t size);
int kstrcmp_c23(const char *s1, const char *s2);

// For Capability DAG functionalities, include <minix/kcapability_dag.h> directly.
// However, the test suite runner can be exposed via klib.h for convenience.
void kcapability_dag_run_mathematical_tests(void);


// Kernel utility functions (example declarations for self-containment)
// These might be defined in more specific kernel headers in a full system.

// Basic console output (stub version)
void kprintf_stub(const char *fmt, ...); // Use kprintf_stub if kprintf is complex for now

// Kernel panic
void kpanic(const char *s);

// Memory allocation (example signatures)
void* kmalloc(size_t size);
void* kcalloc(size_t nmemb, size_t size);
void* krealloc(void *ptr, size_t new_size);
void kfree(void *ptr);

// KASSERT macro definition (example)
#ifdef NDEBUG
#define KASSERT(condition, message) ((void)0)
#else
#define KASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            kprintf_stub("Assertion failed: %s, file %s, line %d: %s\n", \
                    #condition, __FILE__, __LINE__, message); \
            kpanic("KASSERT failed"); \
        } \
    } while (0)
#endif // NDEBUG


#endif /* MINIX_KLIB_H */
