#pragma once
#include "dag.h"

/**
 * @brief Configure Beatty DAG tasks.
 */
void beatty_sched_set_tasks(exo_cap, exo_cap);

/**
 * @brief Configure weights for Beatty DAG tasks.
 */
void beatty_sched_set_weights(int, int);

/**
 * @brief Initialize Beatty DAG runtime.
 */
void libos_setup_beatty_dag(void);
