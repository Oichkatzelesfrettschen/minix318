#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// BSIZE defines the size of the buffer in bytes.
#ifndef BSIZE
#define BSIZE 1024
struct sleeplock;

struct buf {
  // Flags indicating the state of the buffer.
  // B_VALID: The buffer has been read from disk and contains valid data.
  // B_DIRTY: The buffer has been modified and needs to be written back to disk.
  int flags;
  uint32_t dev;
  size_t
      rcref; // number of read-side references; used to track how many readers
             // are currently accessing the buffer without modifying it
  struct sleeplock *lock;
  size_t refcnt; // reference count; tracks the total number of references to
                 // the buffer, including both read-side references (rcref) and
                 // other uses.
  struct buf *prev; // Points to the previous buffer in the LRU cache list, used
                    // for managing the least recently used buffers.
  struct buf *qnext; // Points to the next buffer in the disk queue, used for
                     // scheduling and managing disk I/O operations efficiently.
  uint8_t *data;     // Pointer to dynamically allocated buffer
};
;

// Function to allocate memory for the buffer
void allocate_buffer(struct buf *buffer) {
  buffer->data = (uint8_t *)malloc(BSIZE);
  if (!buffer->data) {
    // Handle allocation failure
    fprintf(stderr, "Failed to allocate buffer memory\n");
    exit(EXIT_FAILURE);
  }
}

// Function to free memory for the buffer
void free_buffer(struct buf *buffer) {
  if (buffer->data) {
    free(buffer->data);
    buffer->data = NULL;
  }
}
#define B_VALID 0x2        // buffer has been read from disk
#define BUF_FLAG_DIRTY 0x4 // buffer needs to be written to disk
