// cap_table.c
#include <stdint.h>
#include <string.h>

#include "cap.h"
#include "defs.h"
#include "spinlock.h"
#include "types.h"

//------------------------------------------------------------------------------
// Globals
//------------------------------------------------------------------------------

// Protects concurrent access to cap_table[] and global_epoch.
static struct spinlock cap_lock;

// Capability table storage.
static struct cap_entry cap_table[CAP_MAX];

// Monotonically increasing epoch used to tag new allocations.
static uint16_t global_epoch;

// Set once after cap_table_init() has run.
static int cap_table_ready = 0;

//------------------------------------------------------------------------------
// forward declarations
//------------------------------------------------------------------------------
static void cap_table_init_if_needed(void);

//------------------------------------------------------------------------------
// cap_table_init_if_needed
//
// Ensures the table has been initialized exactly once.
//------------------------------------------------------------------------------
static void
cap_table_init_if_needed(void)
{
    if (cap_table_ready)
        return;

    initlock(&cap_lock, "cap_table");
    memset(cap_table, 0, sizeof(cap_table));

    global_epoch = 1;          // start epochs at 1 so id=0 always invalid
    cap_table_ready = 1;
}

//------------------------------------------------------------------------------
// cap_table_alloc
//
// Allocates a new capability entry.  Returns a 32-bit ID encoding
// (global_epoch<<16)|index, or UINT32_MAX on failure.
//
// Valid type values are 1..CAP_TYPE_CRYPTOKEY inclusive.
//------------------------------------------------------------------------------
uint32_t
cap_table_alloc(uint16_t type,
                uint32_t resource,
                uint32_t rights,
                uint32_t owner)
{
    cap_table_init_if_needed();

    if (type == CAP_TYPE_NONE || type > CAP_TYPE_CRYPTOKEY) {
        return UINT32_MAX;
    }

    acquire(&cap_lock);
    for (uint16_t i = 1; i < CAP_MAX; i++) {
        if (cap_table[i].type == CAP_TYPE_NONE) {
            cap_table[i].type     = type;
            cap_table[i].resource = resource;
            cap_table[i].rights   = rights;
            cap_table[i].owner    = owner;
            cap_table[i].refcnt   = 1;
            cap_table[i].epoch    = global_epoch;

            uint32_t id = ((uint32_t)global_epoch << 16) | i;
            release(&cap_lock);
            return id;
        }
    }
    release(&cap_lock);
    return UINT32_MAX;
}

//------------------------------------------------------------------------------
// cap_table_lookup
//
// Copies entry → *out if id is valid (same epoch, non-NONE type).
// Returns 0 on success, –1 on failure.
//------------------------------------------------------------------------------
int
cap_table_lookup(uint32_t id, struct cap_entry *out)
{
    cap_table_init_if_needed();

    uint16_t idx   = (uint16_t)(id & 0xFFFF);
    uint16_t epoch = (uint16_t)(id >> 16);

    if (idx == 0 || idx >= CAP_MAX)
        return -1;

    acquire(&cap_lock);
    if (cap_table[idx].type == CAP_TYPE_NONE
        || cap_table[idx].epoch != epoch) {
        release(&cap_lock);
        return -1;
    }

    if (out)
        *out = cap_table[idx];
    release(&cap_lock);
    return 0;
}

//------------------------------------------------------------------------------
// cap_table_inc
//
// Increment the refcount if id is valid.
//------------------------------------------------------------------------------
void
cap_table_inc(uint32_t id)
{
    cap_table_init_if_needed();

    uint16_t idx   = (uint16_t)(id & 0xFFFF);
    uint16_t epoch = (uint16_t)(id >> 16);

    acquire(&cap_lock);
    if (idx > 0 && idx < CAP_MAX
        && cap_table[idx].type != CAP_TYPE_NONE
        && cap_table[idx].epoch == epoch) {
        cap_table[idx].refcnt++;
    }
    release(&cap_lock);
}

//------------------------------------------------------------------------------
// cap_table_dec
//
// Decrement the refcount, freeing the entry if it drops to zero.
//------------------------------------------------------------------------------
void
cap_table_dec(uint32_t id)
{
    cap_table_init_if_needed();

    uint16_t idx   = (uint16_t)(id & 0xFFFF);
    uint16_t epoch = (uint16_t)(id >> 16);

    acquire(&cap_lock);
    if (idx > 0 && idx < CAP_MAX
        && cap_table[idx].type != CAP_TYPE_NONE
        && cap_table[idx].epoch == epoch) {
        if (--cap_table[idx].refcnt == 0) {
            cap_table[idx].type = CAP_TYPE_NONE;
        }
    }
    release(&cap_lock);
}

//------------------------------------------------------------------------------
// cap_table_remove
//
// Forcibly removes the entry if id is valid.  Returns 0 on success,
// –1 on failure.
//------------------------------------------------------------------------------
int
cap_table_remove(uint32_t id)
{
    cap_table_init_if_needed();

    uint16_t idx   = (uint16_t)(id & 0xFFFF);
    uint16_t epoch = (uint16_t)(id >> 16);

    if (idx == 0 || idx >= CAP_MAX)
        return -1;

    acquire(&cap_lock);
    if (cap_table[idx].type == CAP_TYPE_NONE
        || cap_table[idx].epoch != epoch) {
        release(&cap_lock);
        return -1;
    }

    cap_table[idx].type   = CAP_TYPE_NONE;
    cap_table[idx].refcnt = 0;
    release(&cap_lock);
    return 0;
}

//------------------------------------------------------------------------------
// cap_revoke
//
// Revokes the given capability by advancing its epoch (and the global epoch),
// preventing any past ID from matching again.  Returns 0 on success,
// –1 on failure.
//------------------------------------------------------------------------------
int
cap_revoke(uint32_t id)
{
    cap_table_init_if_needed();

    uint16_t idx   = (uint16_t)(id & 0xFFFF);
    uint16_t epoch = (uint16_t)(id >> 16);

    if (idx == 0 || idx >= CAP_MAX)
        return -1;

    acquire(&cap_lock);
    if (cap_table[idx].type == CAP_TYPE_NONE
        || cap_table[idx].epoch != epoch) {
        release(&cap_lock);
        return -1;
    }

    // Avoid epoch overflow.
    if (global_epoch == UINT16_MAX) {
        release(&cap_lock);
        return -1;
    }

    // Bump both the global and this entry’s epoch.
    global_epoch++;
    cap_table[idx].epoch = global_epoch;

    // Clear the entry so it must be re-allocated.
    cap_table[idx].type   = CAP_TYPE_NONE;
    cap_table[idx].refcnt = 0;
    release(&cap_lock);
    return 0;
}
