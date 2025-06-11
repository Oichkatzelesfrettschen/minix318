# Makefile for minix/tests/test_getpid_cap
# FIXME TODO: Adapt to MINIX build system (e.g., using ../Makefile.inc)
PROG=test_getpid_cap
SRCS=test_getpid_cap.c

# This needs to link with the libc that contains getpid_cap.c
# Assuming default libc includes it.
DPADD+= ${LIBC}
LDADD+= -lc

MAN=

.include <bsd.prog.mk>
