#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "param.h"
#include "../conf.h"
#include "../inode.h"
#include "../systm.h"

// Modern error codes
enum {
	ERR_FILE_TOO_BIG = EFBIG,
	ERR_NO_DEVICE = ENODEV,
	ERR_BAD_ADDRESS = EFAULT
};

// Constants for file operations
static const uint32_t MAX_BLOCK_NUM = 077777;
static const uint32_t SMALL_FILE_BLOCKS = 8;
static const uint32_t BLOCKS_PER_INDIRECT = 256;

/*
 * Block mapping function for file system storage
 * Returns physical block number given inode and logical block number
 */
uint32_t bmap(struct inode *restrict ip, uint32_t bn) {
	if (!ip) return 0;
	
	const int dev = ip->i_dev;
	
	// Check for file size limits
	if (bn > MAX_BLOCK_NUM) {
		u.u_error = ERR_FILE_TOO_BIG;
		return 0;
	}

	// Handle small file case
	if (!(ip->i_mode & ILARG)) {
		// Convert small to large file if needed
		if (bn >= SMALL_FILE_BLOCKS) {
			struct buf *bp = alloc(dev);
			if (!bp) return 0;
			
			// Copy small file addresses to indirect block
			uint32_t *bap = (uint32_t *)bp->b_addr;
			for (size_t i = 0; i < SMALL_FILE_BLOCKS; i++) {
				bap[i] = ip->i_addr[i];
				ip->i_addr[i] = 0;
			}
			
			ip->i_addr[0] = bp->b_blkno;
			bdwrite(bp);
			ip->i_mode |= ILARG;
			// Fall through to large file handling
		} else {
			// Direct block access for small files
			uint32_t nb = ip->i_addr[bn];
			if (nb == 0) {
				struct buf *bp = alloc(dev);
				if (bp) {
					nb = bp->b_blkno;
					ip->i_addr[bn] = nb;
					ip->i_flag |= IUPD;
					bdwrite(bp);
				}
			}
			
			// Set up read-ahead
			rablock = (bn < 7) ? ip->i_addr[bn + 1] : 0;
			return nb;
		}
	}

	// Large file algorithm - indirect addressing
	const uint32_t indirect_index = bn >> 8;
	const uint32_t block_offset = bn & 0377;
	
	// Get indirect block
	uint32_t indirect_block = ip->i_addr[indirect_index];
	struct buf *bp;
	
	if (indirect_block == 0) {
		bp = alloc(dev);
		if (!bp) return 0;
		
		ip->i_addr[indirect_index] = bp->b_blkno;
		ip->i_flag |= IUPD;
	} else {
		bp = bread(dev, indirect_block);
		if (!bp) return 0;
	}
	
	uint32_t *bap = (uint32_t *)bp->b_addr;
	
	// Handle double indirect (huge files)
	if (indirect_index == 7) {
		uint32_t double_indirect = bap[indirect_index];
		struct buf *nbp;
		
		if (double_indirect == 0) {
			nbp = alloc(dev);
			if (!nbp) {
				brelse(bp);
				return 0;
			}
			bap[indirect_index] = nbp->b_blkno;
			bdwrite(bp);
		} else {
			brelse(bp);
			nbp = bread(dev, double_indirect);
			if (!nbp) return 0;
		}
		
		bp = nbp;
		bap = (uint32_t *)bp->b_addr;
	}
	
	// Get final block number
	uint32_t nb = bap[block_offset];
	if (nb == 0) {
		struct buf *nbp = alloc(dev);
		if (!nbp) {
			brelse(bp);
			return 0;
		}
		
		nb = nbp->b_blkno;
		bap[block_offset] = nb;
		bdwrite(nbp);
		bdwrite(bp);
	} else {
		brelse(bp);
	}
	
	// Set up read-ahead
	rablock = (block_offset < 255) ? bap[block_offset + 1] : 0;
	return nb;
}

/*
 * Pass character to user space with proper error handling
 */
int passc(char c) {
	if (u.u_segflg) {
		*u.u_base = c;
	} else {
		if (subyte(u.u_base, c) < 0) {
			u.u_error = ERR_BAD_ADDRESS;
			return -1;
		}
	}
	
	u.u_count--;
	u.u_base++;
	
	// Update offset (64-bit safe)
	if (++u.u_offset[1] == 0) {
		u.u_offset[0]++;
	}
	
	return (u.u_count == 0) ? -1 : 0;
}

/*
 * Get character from user space
 */
int cpass(void) {
	if (u.u_count == 0) {
		return -1;
	}
	
	int c;
	if (u.u_segflg) {
		c = *u.u_base;
	} else {
		c = fubyte(u.u_base);
		if (c < 0) {
			u.u_error = ERR_BAD_ADDRESS;
			return -1;
		}
	}
	
	u.u_count--;
	u.u_base++;
	
	if (++u.u_offset[1] == 0) {
		u.u_offset[0]++;
	}
	
	return c & 0377;
}

/*
 * Device error handler
 */
void nodev(void) {
	u.u_error = ERR_NO_DEVICE;
}

/*
 * Null device handler
 */
void nulldev(void) {
	// Intentionally empty
}

/*
 * Modern memory copy using standard library when possible
 */
void bcopy(const void *restrict src, void *restrict dst, size_t count) {
	if (count > 0) {
		memmove(dst, src, count * sizeof(int));
	}
}

/*
 * Check if filename ends with dot and specific character
 */
bool dotted(const char *restrict fp, char c) {
	if (!fp) return false;
	
	const size_t len = strlen(fp);
	return (len > 1 && fp[len - 2] == '.' && fp[len - 1] == c);
}

/*
 * Toggle option flag
 */
void togopt(char c) {
	if (c >= 'a' && c <= 'z') {
		char *tp = &opts[c - 'a'];
		*tp = 1 - *tp;
	}
}

/*
 * Get file modification time
 */
void gettime(void) {
	struct stat stbuf;
	if (stat(filename, &stbuf) == 0) {
		tvec[0] = (int)(stbuf.st_mtime >> 16);
		tvec[1] = (int)(stbuf.st_mtime & 0xFFFF);
	}
}

/*
 * Convert time to Pascal-style time string
 */
char *myctime(const int *tv) {
	static char mycbuf[26];
	
	time_t time_val = ((time_t)tv[0] << 16) | tv[1];
	const char *timestr = ctime(&time_val);
	
	if (!timestr) {
		mycbuf[0] = '\0';
		return mycbuf;
	}
	
	// Copy date part (first 16 chars) and time part (last 5 chars)
	strncpy(mycbuf, timestr, 16);
	mycbuf[16] = ' ';
	strncpy(mycbuf + 17, timestr + 19, 5);
	mycbuf[22] = '\0';
	
	return mycbuf;
}

/*
 * Check if filename is in the process file list
 */
bool inpflist(const char *restrict fp) {
	if (!fp) return false;
	
	for (int i = 0; i < pflstc; i++) {
		if (strcmp(fp, pflist[i]) == 0) {
			return true;
		}
	}
	return false;
}

/*
 * Error reporting function
 */
void Perror(const char *restrict file, const char *restrict error) {
	errno = 0;
	if (error) {
		fprintf(stderr, "%s: %s\n", file ? file : "error", error);
	} else {
		perror(file);
	}
}

/*
 * Allocate and zero memory (modern calloc-like function)
 */
void *allocz(size_t num, size_t size) {
	const size_t total_bytes = num * size;
	void *ptr = malloc(total_bytes);
	
	if (ptr) {
		memset(ptr, 0, total_bytes);
	}
	
	return ptr;
}

/*
 * Check if character exists in string
 */
bool any(const char *restrict str, char ch) {
	return str ? (strchr(str, ch) != NULL) : false;
}

/*
 * Push option state
 */
void opush(char c) {
	if (c >= 'a' && c <= 'z') {
		const int idx = c - 'a';
		optstk[idx] <<= 1;
		optstk[idx] |= opts[idx];
		opts[idx] = 1;
		
		#ifdef PI0
		send(ROPUSH, idx);
		#endif
	}
}

/*
 * Pop option state
 */
void opop(char c) {
	if (c >= 'a' && c <= 'z') {
		const int idx = c - 'a';
		opts[idx] = optstk[idx] & 1;
		optstk[idx] >>= 1;
		
		#ifdef PI0
		send(ROPOP, idx);
		#endif
	}
}

/*
 * Safe memory copy function
 */
void memcopy(void *restrict to, const void *restrict from, size_t bytes) {
	if (bytes > 0 && to && from) {
		memmove(to, from, bytes);
	}
}
