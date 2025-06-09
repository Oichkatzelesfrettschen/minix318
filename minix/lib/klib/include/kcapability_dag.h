// minix/lib/klib/include/kcapability_dag.h
#pragma once
#include <stdint.h> // For uint64_t if used by kcapability_node_t
#include <stddef.h> // For size_t if used

// Forward declaration or basic structure for kcapability_node_t
// This is a placeholder as the actual structure is not provided.
typedef struct kcapability_node {
    uint64_t id;
    uint64_t rights;
    uint64_t creation_time;
    uint64_t expiry_time; // Added based on do_math_read
    // Other fields would be necessary for a real DAG implementation
} kcapability_node_t;

// Placeholder for related functions if any were expected here
#endif // KCAPABILITY_DAG_H (some editors add this automatically)
