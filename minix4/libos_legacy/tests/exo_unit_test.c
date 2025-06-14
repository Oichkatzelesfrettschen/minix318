#include "exo-userland.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint32_t next_page = 1;
static uint32_t next_block = 1;
static unsigned char diskbuf[512];

exo_cap exo_alloc_page(void) {
  exo_cap cap;
  cap.pa = next_page * 0x1000;
  cap.id = next_page++;
  cap.rights = 0;
  cap.owner = 1;
  return cap;
}

int exo_unbind_page(exo_cap cap) {
  (void)cap;
  return 0;
}

int exo_alloc_block(uint32_t dev, uint32_t rights, exo_blockcap *cap) {
  cap->dev = dev;
  cap->blockno = next_block++;
  cap->rights = rights;
  cap->owner = 1;
  return 0;
}

int exo_bind_block(exo_blockcap *cap, void *data, int write) {
  (void)cap;
  if (write)
    memcpy(diskbuf, data, sizeof(diskbuf));
  else
    memcpy(data, diskbuf, sizeof(diskbuf));
  return 0;
}

static void test_exo_pages(void) {
  exo_cap cap = exo_alloc_page();
  assert(cap.pa != 0);
  assert(exo_unbind_page(cap) == 0);
}

static void test_exo_block(void) {
  exo_blockcap blk;
  assert(exo_alloc_block(1, 3, &blk) == 0);
  unsigned char buf[512];
  memset(buf, 0xab, sizeof(buf));
  assert(exo_bind_block(&blk, buf, 1) == 0);
  memset(buf, 0, sizeof(buf));
  assert(exo_bind_block(&blk, buf, 0) == 0);
  for (size_t i = 0; i < sizeof(buf); i++)
    assert(buf[i] == 0xab);
}

int main(void) {
  test_exo_pages();
  test_exo_block();
  printf("exo_unit_test passed\n");
  return 0;
}
