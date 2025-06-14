# The ACPI driver is compilable only with a "gnu" type compiler
PROG=   acpi

# Use unified ACPI implementation
SRCS= acpi_unified.c

DPADD+= ${LIBSYS}
LDADD+= -lsys

CPPFLAGS += -I${.CURDIR}/include
CFLAGS += -DACPI_LIBRARY
CFLAGS += -std=c99

.include <minix.service.mk>
