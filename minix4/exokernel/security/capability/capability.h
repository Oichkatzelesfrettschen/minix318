/**
 * @file capability.h
 * @brief MINIX4 Capability System
 * @version 4.0.0
 * @date 2025-06-12
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "types.h"
#include "spinlock.h"

/**
 * @brief Capability types
 */
enum cap_type {
    CAP_INVALID = 0,           /**< Invalid capability */
    CAP_MEMORY,                /**< Memory capability */
    CAP_IPC,                   /**< IPC capability */
    CAP_DEVICE,                /**< Device capability */
    CAP_FILE,                  /**< File capability */
    CAP_NETWORK,               /**< Network capability */
    CAP_PROCESS,               /**< Process capability */
    CAP_SCHEDULER,             /**< Scheduler capability */
    CAP_SYSTEM,                /**< System capability */
    CAP_MAX
};

/**
 * @brief Capability permissions
 */
enum cap_perm {
    CAP_PERM_NONE   = 0x00,    /**< No permissions */
    CAP_PERM_READ   = 0x01,    /**< Read permission */
    CAP_PERM_WRITE  = 0x02,    /**< Write permission */
    CAP_PERM_EXEC   = 0x04,    /**< Execute permission */
    CAP_PERM_CREATE = 0x08,    /**< Create permission */
    CAP_PERM_DELETE = 0x10,    /**< Delete permission */
    CAP_PERM_ADMIN  = 0x20,    /**< Administrative permission */
    CAP_PERM_ALL    = 0xFF     /**< All permissions */
};

/**
 * @brief Capability handle
 */
typedef uint64_t cap_handle_t;

/**
 * @brief Capability descriptor
 */
struct capability {
    cap_handle_t handle;       /**< Unique capability handle */
    enum cap_type type;        /**< Capability type */
    uint32_t permissions;      /**< Permission bits */
    uint64_t object_id;        /**< Object this capability refers to */
    uint64_t owner_id;         /**< Owner process ID */
    uint64_t created_time;     /**< Creation timestamp */
    uint64_t expires_time;     /**< Expiration timestamp (0 = never) */
    uint32_t flags;            /**< Capability flags */
    uint32_t refcount;         /**< Reference count */
};

/**
 * @brief Capability flags
 */
#define CAP_FLAG_TRANSFERABLE   0x01    /**< Can be transferred */
#define CAP_FLAG_REVOKABLE      0x02    /**< Can be revoked */
#define CAP_FLAG_DELEGATABLE    0x04    /**< Can be delegated */
#define CAP_FLAG_TEMPORARY      0x08    /**< Temporary capability */

/**
 * @brief Capability operations
 */

/**
 * @brief Initialize capability system
 */
int cap_init(void);

/**
 * @brief Create a new capability
 */
cap_handle_t cap_create(enum cap_type type, uint64_t object_id, 
                       uint32_t permissions, uint32_t flags);

/**
 * @brief Lookup a capability by handle
 */
struct capability *cap_lookup(cap_handle_t handle);

/**
 * @brief Check if capability has permission
 */
bool cap_has_permission(cap_handle_t handle, enum cap_perm perm);

/**
 * @brief Transfer capability to another process
 */
int cap_transfer(cap_handle_t handle, uint64_t target_pid);

/**
 * @brief Revoke a capability
 */
int cap_revoke(cap_handle_t handle);

/**
 * @brief Increase capability reference count
 */
int cap_addref(cap_handle_t handle);

/**
 * @brief Decrease capability reference count
 */
int cap_release(cap_handle_t handle);

/**
 * @brief Get all capabilities for a process
 */
int cap_list_for_process(uint64_t pid, cap_handle_t *handles, size_t max_count);

/**
 * @brief Validate capability chain
 */
bool cap_validate_chain(cap_handle_t *handles, size_t count);

/**
 * @brief Capability table per process
 */
struct cap_table {
    struct capability **caps;   /**< Array of capability pointers */
    size_t size;               /**< Size of capability table */
    size_t count;              /**< Number of capabilities */
    uint64_t owner_pid;        /**< Process that owns this table */
    struct spinlock lock;      /**< Table lock */
};

/**
 * @brief Initialize capability table for process
 */
int cap_table_init(struct cap_table *table, uint64_t pid);

/**
 * @brief Destroy capability table
 */
void cap_table_destroy(struct cap_table *table);

/**
 * @brief Add capability to table
 */
int cap_table_add(struct cap_table *table, cap_handle_t handle);

/**
 * @brief Remove capability from table
 */
int cap_table_remove(struct cap_table *table, cap_handle_t handle);
