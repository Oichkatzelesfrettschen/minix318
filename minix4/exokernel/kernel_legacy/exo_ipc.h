#pragma once
#include <stdint.h>
#include "exo_mem.h"
#include "exo.h"

typedef enum {
  IPC_STATUS_SUCCESS = 0,
  IPC_STATUS_TIMEOUT,
  IPC_STATUS_AGAIN,
  IPC_STATUS_BADDEST,
} exo_ipc_status;

struct exo_ipc_ops {
  int (*send)(exo_cap dest, const void *buf, uint64_t len);
  int (*recv)(exo_cap src, void *buf, uint64_t len);
};

void exo_ipc_register(struct exo_ipc_ops *ops);
[[nodiscard]] int exo_send(exo_cap dest, const void *buf, uint64_t len);
[[nodiscard]] int exo_recv(exo_cap src, void *buf, uint64_t len);
[[nodiscard]] int exo_recv_timed(exo_cap src, void *buf, uint64_t len,
                                 unsigned timeout);
