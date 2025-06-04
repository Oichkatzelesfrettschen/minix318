#ifndef _MINIX_TYPES_H
#define _MINIX_TYPES_H

#include <stdint.h>

/* Basic fixed-width types used throughout the kernel */
typedef uint8_t  u8_t;  /* unsigned 8-bit */
typedef int8_t   s8_t;  /* signed 8-bit */
typedef uint16_t u16_t; /* unsigned 16-bit */
typedef int16_t  s16_t; /* signed 16-bit */
typedef uint32_t u32_t; /* unsigned 32-bit */
typedef int32_t  s32_t; /* signed 32-bit */
typedef uint64_t u64_t; /* unsigned 64-bit */
typedef int64_t  s64_t; /* signed 64-bit */

/* Bitmap chunk type used by various kernel tables */
typedef u32_t bitchunk_t;

#endif /* _MINIX_TYPES_H */
