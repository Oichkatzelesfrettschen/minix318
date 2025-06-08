/**
 * @file kcap.c
 * @brief Kernel Capability Management Globals
 *
 * This file defines global variables related to the kernel's capability
 * system, primarily the pointer to the central capability DAG.
 */

#include <minix/kcapability_dag.h> // For kcapability_dag_t structure definition
#include <minix/kcap.h>          // For the extern declaration of kernel_capability_dag

//
// The actual definition of the global kernel capability DAG pointer.
// It is declared 'extern' in <minix/kcap.h> and initialized by
// kernel_initialize_mathematical_foundation() in main.c during kernel startup.
//
kcapability_dag_t* kernel_capability_dag = NULL;
