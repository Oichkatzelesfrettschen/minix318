#include "kernel/kernel.h"
#include "direct_utils.h"
#include "bsp_serial.h"
#include "glo.h"

// Added kernel headers
#include <minix/kernel_types.h>
#include <klib/include/kprintf.h>
#include <klib/include/kstring.h>
#include <klib/include/kmemory.h>

void direct_cls(void)
{
    /* Do nothing */
}

void direct_print_char(char c)
{
	if(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

void direct_print(const char *str)
{
	while (*str) {
		direct_print_char(*str);
		str++;
	}
}

int direct_read_char(unsigned char *ch)
{
	return 0;
}
