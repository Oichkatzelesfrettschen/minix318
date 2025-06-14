/**
 * @file ipc_modern.h
 * @brief MINIX4 Modern IPC System
 * @version 4.0.0
 * @date 2025-06-12
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include "types.h"
#include "capability.h"

/**
 * @brief IPC message types
 */
enum ipc_msg_type {
    IPC_MSG_INVALID = 0,       /**< Invalid message */
    IPC_MSG_CALL,              /**< Synchronous call */
    IPC_MSG_REPLY,             /**< Reply to call */
    IPC_MSG_SEND,              /**< Asynchronous send */
    IPC_MSG_NOTIFY,            /**< Notification */
    IPC_MSG_INTERRUPT,         /**< Interrupt notification */
    IPC_MSG_MAX
};

/**
 * @brief IPC endpoints
 */
typedef uint32_t endpoint_t;
#define EP_INVALID  0          /**< Invalid endpoint */
#define EP_ANY      (~0U)      /**< Accept from any endpoint */

/**
 * @brief IPC message header
 */
struct ipc_msg_header {
    uint32_t magic;            /**< Message magic number */
    enum ipc_msg_type type;    /**< Message type */
    endpoint_t sender;         /**< Sender endpoint */
    endpoint_t receiver;       /**< Receiver endpoint */
    uint32_t msg_id;          /**< Message ID for replies */
    uint32_t payload_size;     /**< Payload size in bytes */
    uint64_t timestamp;        /**< Message timestamp */
    cap_handle_t capabilities[4]; /**< Attached capabilities */
    uint32_t flags;            /**< Message flags */
    uint32_t checksum;         /**< Message checksum */
} __attribute__((packed));

/**
 * @brief Complete IPC message
 */
struct ipc_message {
    struct ipc_msg_header header;  /**< Message header */
    uint8_t payload[];             /**< Variable-length payload */
} __attribute__((packed));

/**
 * @brief IPC message flags
 */
#define IPC_FLAG_URGENT     0x01   /**< Urgent message */
#define IPC_FLAG_RELIABLE   0x02   /**< Reliable delivery required */
#define IPC_FLAG_ENCRYPTED  0x04   /**< Encrypted message */
#define IPC_FLAG_COMPRESSED 0x08   /**< Compressed payload */

/**
 * @brief IPC operations
 */

/**
 * @brief Initialize IPC subsystem
 */
int ipc_init(void);

/**
 * @brief Create an endpoint
 */
endpoint_t endpoint_create(void);

/**
 * @brief Destroy an endpoint
 */
int endpoint_destroy(endpoint_t ep);

/**
 * @brief Send a message (asynchronous)
 */
int ipc_send(endpoint_t dest, const void *payload, size_t size, 
             cap_handle_t *caps, size_t ncaps, uint32_t flags);

/**
 * @brief Receive a message
 */
int ipc_receive(endpoint_t src, void *payload, size_t max_size,
                cap_handle_t *caps, size_t max_caps, uint32_t *flags);

/**
 * @brief Synchronous call (send + receive reply)
 */
int ipc_call(endpoint_t dest, const void *req_payload, size_t req_size,
             void *reply_payload, size_t reply_max_size,
             cap_handle_t *caps, size_t ncaps, uint32_t flags);

/**
 * @brief Reply to a message
 */
int ipc_reply(endpoint_t dest, uint32_t msg_id, const void *payload, 
              size_t size, cap_handle_t *caps, size_t ncaps, uint32_t flags);

/**
 * @brief Send notification
 */
int ipc_notify(endpoint_t dest, uint32_t notification_value);

/**
 * @brief IPC buffer pool for zero-copy messaging
 */
struct ipc_buffer {
    void *data;                /**< Buffer data */
    size_t size;               /**< Buffer size */
    atomic_int refcount;       /**< Reference count */
    uint32_t flags;            /**< Buffer flags */
};

/**
 * @brief Allocate IPC buffer
 */
struct ipc_buffer *ipc_buffer_alloc(size_t size);

/**
 * @brief Reference IPC buffer
 */
void ipc_buffer_ref(struct ipc_buffer *buf);

/**
 * @brief Release IPC buffer
 */
void ipc_buffer_unref(struct ipc_buffer *buf);

/**
 * @brief Fast IPC for local communication
 */
struct fast_ipc_channel {
    atomic_uintptr_t write_ptr; /**< Write pointer */
    atomic_uintptr_t read_ptr;  /**< Read pointer */
    size_t size;               /**< Buffer size */
    void *buffer;              /**< Circular buffer */
    endpoint_t sender;         /**< Sender endpoint */
    endpoint_t receiver;       /**< Receiver endpoint */
};

/**
 * @brief Create fast IPC channel
 */
int fast_ipc_create(endpoint_t sender, endpoint_t receiver, size_t buffer_size);

/**
 * @brief Send via fast IPC
 */
int fast_ipc_send(endpoint_t dest, const void *data, size_t size);

/**
 * @brief Receive via fast IPC
 */
int fast_ipc_receive(endpoint_t src, void *data, size_t max_size);

/**
 * @brief IPC statistics
 */
struct ipc_stats {
    uint64_t messages_sent;    /**< Total messages sent */
    uint64_t messages_received; /**< Total messages received */
    uint64_t calls_made;       /**< Total calls made */
    uint64_t replies_sent;     /**< Total replies sent */
    uint64_t notifications_sent; /**< Total notifications sent */
    uint64_t fast_ipc_ops;     /**< Fast IPC operations */
    uint64_t capabilities_transferred; /**< Capabilities transferred */
    uint64_t errors;           /**< Total errors */
};

/**
 * @brief Get IPC statistics
 */
void ipc_get_stats(struct ipc_stats *stats);
