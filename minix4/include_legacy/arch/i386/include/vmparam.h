#ifndef _MACHINE_VMPARAM_H
#define _MACHINE_VMPARAM_H

#include <minix/types.h>

/* Simplified VM parameters for the i386 microkernel port */
#define VM_PAGE_SIZE   4096
#define VM_PAGE_MASK   (VM_PAGE_SIZE - 1)
#define VM_MMAPBASE    0x10000000
#define VM_MMAPTOP     0xF0000000

#endif /* _MACHINE_VMPARAM_H */
