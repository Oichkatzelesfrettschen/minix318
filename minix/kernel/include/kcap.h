#ifndef MINIX_KCAP_H
#define MINIX_KCAP_H

// Forward declare kcapability_dag_t if its full definition isn't needed here,
// or include kcapability_dag.h if it's lightweight enough.
// For an extern pointer, a forward declaration is sufficient.
struct kcapability_dag; // Forward declaration

/**
 * @var kernel_capability_dag
 * @brief The global kernel capability DAG.
 *
 * This DAG is the central mathematical structure managing all system-wide
 * capability relationships and is the heart of the kernel's security model.
 * It is initialized during kernel startup.
 */
extern struct kcapability_dag* kernel_capability_dag;

/**
 * @def INITIAL_KERNEL_CAPABILITY_CAPACITY
 * @brief Initial capacity (number of nodes) for the kernel_capability_dag.
 *
 * Defines how many capability nodes the DAG is initially allocated to hold.
 * This can be tuned based on expected system load or typical number of
 * capabilities active in early boot or common configurations.
 */
#define INITIAL_KERNEL_CAPABILITY_CAPACITY 1024

/**
 * @def MAX_KERNEL_CAPABILITIES
 * @brief Maximum allowed number of capabilities in the kernel_capability_dag.
 *
 * This serves as an upper bound to prevent runaway allocations or ensure
 * that capability identifiers fit within certain mathematical constraints
 * if they are, for example, used as indices into fixed-size arrays elsewhere.
 * It's a safeguard for system stability and resource management.
 */
#define MAX_KERNEL_CAPABILITIES 65536

#endif /* MINIX_KCAP_H */
