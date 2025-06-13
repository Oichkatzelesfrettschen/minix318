/**
 * @file param.h
 * @brief MINIX4 System Parameters
 * @version 4.0.0
 * @date 2025-06-12
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

// System limits
#define NPROC           256      /**< Maximum number of processes */
#define MAXARGLEN       4096     /**< Maximum argument length */
#define MAXPATHLEN      1024     /**< Maximum path length */
#define NOFILE          64       /**< Maximum open files per process */
#define NCPU            8        /**< Maximum number of CPUs */

// Memory parameters
#define PGSIZE          4096     /**< Page size */
#define KSTACKSIZE      4096     /**< Kernel stack size */
#define USTACKSIZE      4096     /**< User stack size */
#define HEAPSIZE        (64*1024*1024)  /**< Default heap size */

// IPC parameters
#define NENDPOINTS      1024     /**< Maximum number of endpoints */
#define MAXMSGSIZE      8192     /**< Maximum message size */
#define MSGBUFSIZE      (16*1024) /**< Message buffer size */

// Capability parameters
#define NCAPS           4096     /**< Maximum number of capabilities */
#define CAP_MAX_DEPTH   16       /**< Maximum capability chain depth */

// File system parameters
#define NINODES         1024     /**< Maximum number of inodes */
#define NBUF            64       /**< Number of buffer cache blocks */
#define ROOTDEV         1        /**< Root device number */

// Device parameters
#define NDEV            64       /**< Maximum number of devices */
#define MAXREQUEST      256      /**< Maximum I/O request size */

// Network parameters (if enabled)
#ifdef CONFIG_NETWORKING
#define NSOCKETS        256      /**< Maximum number of sockets */
#define NETBUFSIZE      1500     /**< Network buffer size (MTU) */
#endif

// Debug parameters
#ifdef DEBUG
#define LOGBUFSIZE      (32*1024) /**< Debug log buffer size */
#define MAXDEBUGMSG     256      /**< Maximum debug message length */
#endif

// Version information
#define MINIX_VERSION_MAJOR  4
#define MINIX_VERSION_MINOR  0
#define MINIX_VERSION_PATCH  0
#define MINIX_VERSION_STRING "4.0.0-C23"
