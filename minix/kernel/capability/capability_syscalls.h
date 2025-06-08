// minix/kernel/capability/capability_syscalls.h
#pragma once
// Attempt to include kernel/system.h, adjust path if necessary based on actual structure
// If it's a very common header, it might be found with <minix/system.h> or similar
// For now, assume it's accessible relative to kernel directory
#include "../system.h" // Placeholder, actual path might vary
#include "../../lib/klib/include/kcapability_posix.h" // Adjusted path

// Define _BitInt if not already (e.g. from kcrypto.h stub, but good to have fallback)
#ifndef _BitInt
// Assuming 64-bit is the max needed for this specific file's _BitInt usage.
// The kcapability_posix.h uses up to 256. A proper solution needs compiler support or a library.
#define _BitInt(N) uint64_t
#endif

// Mathematical operations preserve these properties:
// 1. Monotonicity: derived_rights ⊆ parent_rights
// 2. Causality: happens_before(grant, use)
// 3. Non-amplification: no privilege escalation

// Forward declare for use in function pointers if system.h doesn't define endpoint_t yet
// typedef int endpoint_t;
// typedef unsigned int uid_t; // POSIX types
// typedef unsigned int gid_t; // POSIX types

// Construct access proof with mathematical verification
capability_proof_t* construct_access_proof(
    endpoint_t process_endpoint,
    const char* path,
    int requested_rights
);

// Verify proof maintains all invariants
bool verify_mathematical_proof(const capability_proof_t* proof);

// Apply lattice-based access control decision
bool lattice_access_check(
    const posix_capability_space_t* cap,
    uid_t requesting_uid,
    gid_t requesting_gid,
    int requested_mode
);

// From math_syscalls_extended.c
int do_math_write(struct proc *caller, message *m_ptr);
int do_math_close(struct proc *caller, message *m_ptr);
int do_math_open_cached(struct proc *caller, message *m_ptr);
int do_math_open_batch(struct proc *caller, message *m_ptr);

int do_cap_derive(struct proc *caller, message *m_ptr);
int do_cap_restrict(struct proc *caller, message *m_ptr);
int do_cap_revoke(struct proc *caller, message *m_ptr);
int do_cap_delegate(struct proc *caller, message *m_ptr);

// Kernel handler for SYS_CAPABILITY_QUERY
int do_capability_query(struct proc *caller, message *m_ptr);
