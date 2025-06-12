#pragma once

struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE } type;
  size_t ref; // reference count
  char readable;
  char writable;
  struct pipe *pipe;
  struct inode *ip;
  size_t off;
  int flags;
};


// in-memory copy of an inode
struct inode {
  uint32_t dev;           // Device number
  uint32_t inum;          // Inode number
  size_t ref;            // Reference count
  struct sleeplock lock; // protects everything below here
  int valid;          // inode has been read from disk?

  short type;         // copy of disk inode
  short major;
  short minor;
  short nlink;
  size_t size;
  uint32_t addrs[NDIRECT+1];
};
// Must match on-disk inode layout when compiled for 32-bit targets.
#ifndef __x86_64__
_Static_assert(sizeof(struct inode) == 144, "struct inode size incorrect");
#endif

// table mapping major device number to
// device functions
struct devsw {
  int (*read)(struct inode*, char*, size_t);
  int (*write)(struct inode*, char*, size_t);
};

extern struct devsw devsw[];

#define CONSOLE 1
