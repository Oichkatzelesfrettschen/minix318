#ifndef _MINIX_IPC_PMSYSCALLS_H
#define _MINIX_IPC_PMSYSCALLS_H

#include <sys/types.h> // For pid_t

// Using existing m_type field in message struct for these.
// These would be defined in minix/callnr.h or com.h eventually.
// For now, conceptual values. Assume they are offset from PM_BASE.
#define PM_GETPID_CAP_RQ    (PM_BASE + 100) // Example - GETPID via Capability Request
#define PM_GETPID_CAP_RS    (PM_BASE + 101) // Example - GETPID via Capability Response

// For message m_m1 (using existing generic message fields for simplicity in Phase 0.25)
// m.m_type = PM_GETPID_CAP_RQ (request, no actual payload needed from user)
// m.m_type = PM_GETPID_CAP_RS (response)
// m.m1_i1  = pid_t pid;       (in response message from PM)

// Access macros for clarity, assuming m_m1 is used for PM_GETPID_CAP_RS
#define m_pm_getpid_cap_rs_pid m_m1.m1i1

#endif // _MINIX_IPC_PMSYSCALLS_H
