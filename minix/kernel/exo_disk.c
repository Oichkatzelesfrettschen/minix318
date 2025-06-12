#include "types.h"
#include "defs.h"
#include "fs.h"
#include "sleeplock.h"
#include "buf.h"
#include "exo_disk.h"
#include "generic_utils.h"
#include <errno.h>
#include <string.h>
#define EXO_KERNEL
#include "include/exokernel.h"


[[nodiscard]] int exo_read_disk(struct exo_blockcap cap, void *dst,
                                uint64_t off, uint64_t n) {
  if (cap.owner != myproc()->pid || !cap_has_rights(cap.rights, EXO_RIGHT_R))
    return -EPERM;
  struct buf b;
  uint64_t tot = 0;
  memset(&b, 0, sizeof(b));
  initsleeplock(&b.lock, "exodisk");

  while (tot < n) {
    uint64_t cur = off + tot;
    struct exo_blockcap blk = {cap.dev, cap.blockno + cur / BSIZE, cap.rights,
                               cap.owner};
    size_t m = GU_MIN(n - tot, BSIZE - cur % BSIZE);

    acquiresleep(&b.lock);
    int r = exo_bind_block(&blk, &b, 0);
    if (r < 0) {
      releasesleep(&b.lock);
      return r;
    }
    memcpy((char *)dst + tot, b.data + cur % BSIZE, m);
    releasesleep(&b.lock);

    tot += m;
  }

  return (int)tot;
}

[[nodiscard]] int exo_write_disk(struct exo_blockcap cap, const void *src,
                                 uint64_t off, uint64_t n) {
  if (cap.owner != myproc()->pid || !cap_has_rights(cap.rights, EXO_RIGHT_W))
    return -EPERM;
  struct buf b;
  uint64_t tot = 0;
  memset(&b, 0, sizeof(b));
  initsleeplock(&b.lock, "exodisk");

  while (tot < n) {
    uint64_t cur = off + tot;
    struct exo_blockcap blk = {cap.dev, cap.blockno + cur / BSIZE, cap.rights,
                               cap.owner};
    size_t m = GU_MIN(n - tot, BSIZE - cur % BSIZE);

    acquiresleep(&b.lock);
    memcpy(b.data + cur % BSIZE, (char *)src + tot, m);
    int r = exo_bind_block(&blk, &b, 1);
    if (r < 0) {
      releasesleep(&b.lock);
      return r;
    }
    releasesleep(&b.lock);

    tot += m;
  }

  return (int)tot;
}
