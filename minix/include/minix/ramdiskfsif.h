#ifndef _MINIX_RAMDISKFS_IF_H
#define _MINIX_RAMDISKFS_IF_H

#include <sys/types.h> // For off_t, mode_t, size_t
#include <minix/types.h> // For endpoint_t, vir_bytes
// Include minix/ipcconst.h for cp_grant_id_t if it's defined there,
// or safecopies.h/grant.h as appropriate. For now, assume it's available.
// #include <minix/ipcconst.h> // Or other grant definition header

// FIXME TODO: This base number needs to be unique system-wide and registered.
#define RAMDISKFS_REQ_BASE   0x1B00  // Example base

// Call numbers
#define RAMDISKFS_OPEN       (RAMDISKFS_REQ_BASE + 0)
#define RAMDISKFS_CLOSE      (RAMDISKFS_REQ_BASE + 1)
#define RAMDISKFS_READ       (RAMDISKFS_REQ_BASE + 2)
#define RAMDISKFS_WRITE      (RAMDISKFS_REQ_BASE + 3)

/* Message field usage conventions for RamdiskFS calls.
 * Assumes message structure from <minix/ipc.h>.
 * For 64-bit off_t, two 32-bit fields are used (lo and hi).
 * For buffer passing where server creates grant: pass user buffer address & user endpoint.
 */

// For RAMDISKFS_OPEN request (caller to RamdiskFS)
// m_type = RAMDISKFS_OPEN
#define M_RAMDISKFS_OPEN_PATH_ADDR   m_m1.m1p1  // vir_bytes: path string address in caller's memory
#define M_RAMDISKFS_OPEN_PATH_LEN    m_m1.m1i1  // size_t: length of path string (including null)
#define M_RAMDISKFS_OPEN_FLAGS       m_m1.m1i2  // int: open flags (O_CREAT, O_RDWR, etc.)
#define M_RAMDISKFS_OPEN_MODE        m_m1.m1i3  // mode_t: creation mode (if O_CREAT)
#define M_RAMDISKFS_OPEN_REQ_ENDPT   m_m2.m2i1  // endpoint_t: endpoint of the requester (for grant creation by server for path)

// For RAMDISKFS_OPEN reply (RamdiskFS to caller, on success)
// m_type = OK (or specific reply code) // Corrected in previous versions, should be RAMDISKFS_OPEN_REPLY or similar
// For now, let's assume server replies with m_type = OK and puts cap_idx in field.
// Or, server replies with m_type = RAMDISKFS_OPEN (as a success indicator) and cap_idx in field.
// Let's be explicit:
// m_type should be specific for success, e.g. RAMDISKFS_OPEN_REPLY
// For Phase 0.5, open_cap.c expects m.m_type == RAMDISKFS_OPEN_REPLY
// So, let's ensure that's documented.
// m_type = RAMDISKFS_OPEN_REPLY (custom positive reply type for success)
#define M_RAMDISKFS_OPEN_REPLY_CAP_IDX    m_m1.m1i1  // uint32_t: capability index for the new file object

// For RAMDISKFS_CLOSE request (caller to RamdiskFS)
// m_type = RAMDISKFS_CLOSE
#define M_RAMDISKFS_CLOSE_CAP_IDX    m_m1.m1i1  // uint32_t: file object capability index to close
// Reply: m.m_type = OK or error code

// For RAMDISKFS_READ request (caller to RamdiskFS)
// m_type = RAMDISKFS_READ
#define M_RAMDISKFS_READ_CAP_IDX     m_m1.m1i1  // uint32_t: file object capability index
#define M_RAMDISKFS_READ_NBYTES      m_m1.m1i2  // size_t: number of bytes to read
#define M_RAMDISKFS_READ_BUF_ADDR    m_m1.m1p1  // vir_bytes: user buffer address
#define M_RAMDISKFS_READ_REQ_ENDPT   m_m1.m1i3  // endpoint_t: endpoint of the requester (for grant creation by server)
#define M_RAMDISKFS_READ_OFFSET_LO   m_m2.m2i1  // uint32_t: low part of 64-bit off_t offset
#define M_RAMDISKFS_READ_OFFSET_HI   m_m2.m2i2  // uint32_t: high part of 64-bit off_t offset
// Reply: m.m_type = bytes_read (non-negative) or error code (negative)

// For RAMDISKFS_WRITE request (caller to RamdiskFS)
// m_type = RAMDISKFS_WRITE
#define M_RAMDISKFS_WRITE_CAP_IDX    m_m1.m1i1  // uint32_t: file object capability index
#define M_RAMDISKFS_WRITE_NBYTES     m_m1.m1i2  // size_t: number of bytes to write
#define M_RAMDISKFS_WRITE_BUF_ADDR   m_m1.m1p1  // vir_bytes: user buffer address (const void* for write)
#define M_RAMDISKFS_WRITE_REQ_ENDPT  m_m1.m1i3  // endpoint_t: endpoint of the requester (for grant creation by server)
#define M_RAMDISKFS_WRITE_OFFSET_LO  m_m2.m2i1  // uint32_t: low part of 64-bit off_t offset
#define M_RAMDISKFS_WRITE_OFFSET_HI  m_m2.m2i2  // uint32_t: high part of 64-bit off_t offset
// Reply: m.m_type = bytes_written (non-negative) or error code (negative)

#endif // _MINIX_RAMDISKFS_IF_H
