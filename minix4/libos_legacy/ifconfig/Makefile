#	$NetBSD: Makefile,v 1.62 2025/04/04 04:12:22 rin Exp $
#	@(#)Makefile	8.1 (Berkeley) 6/5/93

# when making a change to this file, please check if the change is
# also needed for src/distrib/utils/x_ifconfig.
# such stuff should be into Makefile.common.

.include <bsd.own.mk>

RUMPPRG=	ifconfig
MAN=		ifconfig.8

.if defined(__MINIX)
CWARNFLAGS.gcc+= -Wno-unused-but-set-variable
.endif # defined(__MINIX)

SRCS=		af_atalk.c af_link.c carp.c

CPPFLAGS+=	-I${NETBSDSRCDIR}/sys/dist/pf/
SRCS+=		pfsync.c

.if (${USE_INET6} != "no")
CPPFLAGS+=	-DINET6
SRCS+=		af_inet6.c
.endif

.include "Makefile.common"

.if (${MKRUMP} != "no")
CPPFLAGS+=	-DRUMP_ACTION
LDADD.rump +=	-lrumpres
DPADD.rump +=	${LIBRUMPRES}
.endif

.include <bsd.prog.mk>
