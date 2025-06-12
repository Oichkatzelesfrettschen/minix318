#include "cap.h"
#include "defs.h"
#include "spinlock.h"
#include "types.h"
#include <stdint.h>
#include <string.h>

/** Global lock protecting the capability table. */
static struct spinlock cap_lock;
/** Capability table storage. */
static struct cap_entry cap_table[CAP_MAX];
/** Current epoch encoded into capability identifiers. */
uint32_t global_epoch;
/** Flag that becomes non-zero once the table is initialized. */
int cap_table_ready = 0;

/** Initialize the capability table. */
void cap_table_init(void) {
  initlock(&cap_lock, "captbl");
  memset(cap_table, 0, sizeof(cap_table));
  global_epoch = 0;
  cap_table_ready = 1;
}

/**
 * Allocate an entry in the capability table.
 *
 * @param type Capability type.
 * @param resource Backing resource value.
 * @param rights Rights bitmask.
 * @param owner Owning process identifier.
 * @return Encoded capability id or -1 on failure.
 */
int cap_table_alloc(uint16_t type, uint32_t resource, uint32_t rights,
                    uint32_t owner) {
  if (type == CAP_TYPE_NONE || type > CAP_TYPE_CRYPTOKEY)
    return -1;
  acquire(&cap_lock);
  for (int i = 1; i < CAP_MAX; i++) {
    if (cap_table[i].type == CAP_TYPE_NONE) {
      cap_table[i].type = type;
      cap_table[i].resource = resource;
      cap_table[i].rights = rights;
      cap_table[i].owner = owner;
      cap_table[i].refcnt = 1;
      uint32_t id = ((uint32_t)cap_table[i].epoch << 16) | i;
      release(&cap_lock);
      return id;
    }
  }
  release(&cap_lock);
  return -1;
}

/** Look up a capability by its identifier. */
int cap_table_lookup(uint32_t id, struct cap_entry *out) {
  uint16_t idx = id & 0xffff;
  uint16_t epoch = id >> 16;
  acquire(&cap_lock);
  if (idx >= CAP_MAX || cap_table[idx].type == CAP_TYPE_NONE ||
      cap_table[idx].epoch != epoch) {
    release(&cap_lock);
    return -1;
  }
  if (out)
    *out = cap_table[idx];
  release(&cap_lock);
  return 0;
}

/** Increase the reference count of a capability. */
void cap_table_inc(uint32_t id) {
  uint16_t idx = id & 0xffff;
  uint16_t epoch = id >> 16;
  acquire(&cap_lock);
  if (idx < CAP_MAX && cap_table[idx].type != CAP_TYPE_NONE &&
      cap_table[idx].epoch == epoch)
    cap_table[idx].refcnt++;
  release(&cap_lock);
}

/** Decrease the reference count of a capability. */
void cap_table_dec(uint32_t id) {
  uint16_t idx = id & 0xffff;
  uint16_t epoch = id >> 16;
  acquire(&cap_lock);
  if (idx < CAP_MAX && cap_table[idx].type != CAP_TYPE_NONE &&
      cap_table[idx].epoch == epoch) {
    if (--cap_table[idx].refcnt == 0)
      cap_table[idx].type = CAP_TYPE_NONE;
  }
  release(&cap_lock);
}

/** Remove a capability entry. */
int cap_table_remove(uint32_t id) {
  uint16_t idx = id & 0xffff;
  uint16_t epoch = id >> 16;
  acquire(&cap_lock);
  if (idx >= CAP_MAX || cap_table[idx].type == CAP_TYPE_NONE ||
      cap_table[idx].epoch != epoch) {
    release(&cap_lock);
    return -1;
  }
  cap_table[idx].type = CAP_TYPE_NONE;
  release(&cap_lock);
  return 0;
}

/**
 * Revoke a capability by bumping its epoch.
 *
 * Revocation fails if the epoch would overflow.
 */
int cap_revoke(uint32_t id) {
  uint16_t idx = id & 0xffff;
  uint16_t epoch = id >> 16;
  acquire(&cap_lock);
  if (idx >= CAP_MAX || cap_table[idx].type == CAP_TYPE_NONE ||
      cap_table[idx].epoch != epoch) {
    release(&cap_lock);
    return -1;
  }
  if (cap_table[idx].epoch == 0xffff) {
    release(&cap_lock);
    return -1;
  }
  cap_table[idx].epoch++;
  cap_table[idx].type = CAP_TYPE_NONE;
  cap_table[idx].refcnt = 0;
  release(&cap_lock);
  return 0;
}
