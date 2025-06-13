/**
 * @file ipc.h
 * @brief MINIX4 Inter-Process Communication Header
 * @details Modern IPC definitions for MINIX4 kernel
 * @version 4.0.0
 * @date 2025-06-12
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdatomic.h>

/* Forward declarations */
struct proc;
struct message;

/* IPC message types */
enum ipc_type {
    IPC_SEND = 0,
    IPC_RECEIVE,
    IPC_NOTIFY, 
    IPC_SENDNB,     /* Non-blocking send */
    IPC_SENDREC,    /* Send and receive */
    IPC_NOTIFY_FROM_INT  /* Notification from interrupt */
};

/* IPC result codes */
enum ipc_result {
    IPC_OK = 0,
    IPC_E_BADCALL = -1,
    IPC_E_BADPARAM = -2,
    IPC_E_TIMEOUT = -3,
    IPC_E_SRCPROC = -4,
    IPC_E_DSTPROC = -5,
    IPC_E_MSGSIZE = -6,
    IPC_E_NOGRANT = -7
};

/* Maximum message size */
#define IPC_MAX_MSG_SIZE    (1024)
#define IPC_MAGIC           (0x4D49584E)  /* "MIXN" */

/**
 * @brief IPC Message structure
 * @details Standard message format for all IPC operations
 */
struct message {
    uint32_t magic;                     /* Message validation magic */
    uint32_t type;                      /* Message type/command */
    int src_pid;                        /* Source process ID */
    int dst_pid;                        /* Destination process ID */
    uint32_t size;                      /* Payload size in bytes */
    uint64_t timestamp;                 /* Message timestamp */
    uint32_t flags;                     /* Message flags */
    uint32_t sequence;                  /* Message sequence number */
    
    /* Message payload */
    union {
        uint8_t raw[IPC_MAX_MSG_SIZE - 32];  /* Raw data */
        struct {
            uint64_t arg1, arg2, arg3, arg4; /* Generic arguments */
            uint64_t arg5, arg6, arg7, arg8;
        } args;
        struct {
            int status;                 /* Status code */
            size_t count;               /* Count/size */
            void *ptr;                  /* Pointer */
            uint64_t value;             /* Generic value */
        } response;
    } payload;
    
    struct message *next;               /* Queue linkage */
};

/* Message flags */
#define MSG_FLAG_URGENT     (1U << 0)   /* High priority message */
#define MSG_FLAG_ASYNC      (1U << 1)   /* Asynchronous message */
#define MSG_FLAG_REPLY      (1U << 2)   /* Reply expected */
#define MSG_FLAG_GRANT      (1U << 3)   /* Grant included */
#define MSG_FLAG_COPY       (1U << 4)   /* Copy message data */

/* IPC system call interface */
int ipc_send(int dst_pid, struct message *msg);
int ipc_receive(int src_pid, struct message *msg);
int ipc_notify(int dst_pid, uint32_t notification);
int ipc_sendrec(int dst_pid, struct message *msg);
int ipc_sendnb(int dst_pid, struct message *msg);

/* Message management */
struct message *msg_alloc(void);
void msg_free(struct message *msg);
void msg_init(struct message *msg, uint32_t type, int dst_pid);
int msg_copy_to_user(struct message *msg, void *user_ptr);
int msg_copy_from_user(struct message *msg, const void *user_ptr);

/* Message queue operations */
void msg_queue_init(struct proc *p);
void msg_queue_add(struct proc *p, struct message *msg);
struct message *msg_queue_remove(struct proc *p);
bool msg_queue_empty(struct proc *p);
size_t msg_queue_size(struct proc *p);

/* IPC debugging and statistics */
void ipc_dump_stats(void);
void ipc_dump_message(struct message *msg);
void ipc_dump_queues(void);

/* IPC initialization */
void ipc_init(void);

/* Notification support */
int notify_send(int dst_pid, uint32_t notification);
int notify_receive(uint32_t *notification);
bool notify_pending(struct proc *p);

/* Grant management for safe data transfers */
struct grant {
    int grantor_pid;
    void *addr;
    size_t size;
    uint32_t permissions;
    uint64_t grant_id;
};

#define GRANT_READ      (1U << 0)
#define GRANT_WRITE     (1U << 1)
#define GRANT_EXEC      (1U << 2)

int grant_create(void *addr, size_t size, uint32_t permissions);
int grant_revoke(uint64_t grant_id);
int grant_verify(uint64_t grant_id, void *addr, size_t size, uint32_t required_perms);

/* Utility macros */
#define IPC_TO_ANY      (-1)
#define IPC_FROM_ANY    (-1)
#define IPC_SELF        (0)

/* Message type constants */
#define MSG_TYPE_NULL           (0)
#define MSG_TYPE_SYSCALL        (1)
#define MSG_TYPE_INTERRUPT      (2)
#define MSG_TYPE_DRIVER_CMD     (3)
#define MSG_TYPE_FS_REQ         (4)
#define MSG_TYPE_NET_PKT        (5)
#define MSG_TYPE_USER_DEFINED   (1000)

/* Timeout values */
#define IPC_TIMEOUT_INFINITE    (UINT64_MAX)
#define IPC_TIMEOUT_IMMEDIATE   (0)
#define IPC_TIMEOUT_DEFAULT_MS  (5000)

/* Process can send/receive from these sources */
static inline bool ipc_valid_endpoint(int pid) {
    return (pid == IPC_TO_ANY || pid == IPC_FROM_ANY || 
            pid == IPC_SELF || (pid > 0 && pid < 65536));
}
