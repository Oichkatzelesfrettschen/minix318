#pragma once
#include <stdint.h>

// On-disk file system format.
// Both the kernel and user programs use this header file.

#define ROOTINO 1 // root i-number
#define BSIZE 512 // block size

// Disk layout:
// [ boot block | super block | log | inode blocks |
//                                          free bit map | data blocks]
//
// mkfs computes the super block and builds an initial file system. The
// super block describes the disk layout:
struct superblock {
  uint32_t size;       // Size of file system image (blocks)
  uint32_t nblocks;    // Number of data blocks
  uint32_t ninodes;    // Number of inodes.
  uint32_t nlog;       // Number of log blocks
  uint32_t logstart;   // Block number of first log block
  uint32_t inodestart; // Block number of first inode block
  uint32_t bmapstart;  // Block number of first free map block
};

#define NDIRECT 12
#define NINDIRECT (BSIZE / sizeof(uint32_t))
#define MAXFILE (NDIRECT + NINDIRECT * 1024)

// On-disk inode structure
/**
 * @brief On-disk inode structure used by the minimalist file system.
 *
 * This mirrors the layout used by the original xv6 operating system.
 */
typedef struct fs_dinode {
  short type;                  /**< File type                            */
  short major;                 /**< Major device number (T_DEV only)    */
  short minor;                 /**< Minor device number (T_DEV only)    */
  short nlink;                 /**< Number of links to inode in FS      */
  uint32_t size;               /**< File size in bytes                  */
  uint32_t addrs[NDIRECT + 1]; /**< Data block addresses                */
} fs_dinode;

/* Backwards compatibility alias */
#define dinode fs_dinode

// Inodes per block.
#define IPB (BSIZE / sizeof(struct fs_dinode))

// Block containing inode i
#define IBLOCK(i, sb) ((i) / IPB + sb.inodestart)

// Bitmap bits per block
#define BPB (BSIZE * 8)

// Block of free map containing bit for block b
#define BBLOCK(b, sb) (b / BPB + sb.bmapstart)

// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ 14

struct dirent {
  uint16_t inum;
  char name[DIRSIZ];
};
