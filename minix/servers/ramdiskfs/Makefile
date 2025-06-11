# Makefile for RamdiskFS server
PROG=ramdiskfs
SRCS=main.c handle_open.c handle_readwrite.c handle_close.c # Add other .c files as created

# FIXME TODO: Define RAMDISKFS_PROC_NR and add to system services if it's a boot service
# For now, assumes it's started like other servers.
DPADD+= ${LIBSYS} ${LIBSFFS} # LIBSFFS for cpf_grant_magic etc. (or similar lib if grants are elsewhere)
LDADD+= -lsys -lsffs        # Or link specific grant object files if not in a lib

MAN=

.include <minix.service.mk>
