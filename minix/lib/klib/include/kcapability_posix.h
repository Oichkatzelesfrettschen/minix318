// minix/lib/klib/include/kcapability_posix.h
#pragma once
#include "kcapability_dag.h" // Assumed to exist or be created
#include <stdint.h>

// Mathematical representation of file system capabilities
typedef struct posix_capability_space {
    _BitInt(64) inode_identifier;      // Unique file system object
    _BitInt(32) access_rights;         // R|W|X|SETUID|SETGID|STICKY
    _BitInt(16) owner_uid;             // Ownership lattice position
    _BitInt(16) group_gid;             // Group lattice position
    _BitInt(64) temporal_validity;     // Monotonic timestamp bound
    _BitInt(128) cryptographic_nonce;  // Replay attack prevention
} posix_capability_space_t;

// Proof structure for capability-based access
typedef struct capability_proof {
    kcapability_node_t* root_capability;     // Root of derivation chain
    kcapability_node_t** derivation_path;    // Complete path to target
    size_t path_length;                      // Number of derivations
    _BitInt(256) proof_hash;                 // Cryptographic commitment
    uint64_t generation_timestamp;           // Temporal ordering

    // Mathematical invariants
    struct {
        bool monotonic_rights_decrease;      // ∀i: rights[i+1] ⊆ rights[i]
        bool valid_derivation_chain;         // ∀i: parent[i+1] = child[i]
        bool temporal_consistency;           // ∀i: time[i] ≤ time[i+1]
    } invariants;
} capability_proof_t;

// Capability flags (example values)
#define CAP_READ       0x01
#define CAP_WRITE      0x02
#define CAP_CREATE     0x04
#define CAP_TRUNCATE   0x08
#define CAP_EXECUTE    0x10 // Example, not used in issue's math_open
// Add other flags like SETUID, SETGID, STICKY as needed by access_rights

#define PROOF_VALIDITY_WINDOW 300 // Example: 5 minutes in seconds
