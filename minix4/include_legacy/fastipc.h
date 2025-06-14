#pragma once
#include "ipc.h"

/**
 * @file fastipc.h
 * @brief Simple kernel-to-user message queue used by ipc_fast.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Send a message through the fast IPC queue.
 *
 * @param m Pointer to the message to send.
 */
void fastipc_send(zipc_msg_t *m);

/**
 * @brief Handle the fast IPC system call.
 *
 * @return Always returns 0.
 */
int sys_ipc_fast(void);

#ifdef __cplusplus
}
#endif
