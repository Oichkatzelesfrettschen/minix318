#	$NetBSD: Makefile,v 1.124 2024/10/14 08:13:23 kre Exp $
#	@(#)Makefile	8.4 (Berkeley) 5/5/95

.include <bsd.own.mk>

PROG=	sh
SHSRCS=	alias.c arith_token.c arithmetic.c cd.c echo.c error.c eval.c exec.c \
	expand.c histedit.c input.c jobs.c mail.c main.c memalloc.c \
	miscbltin.c mystring.c options.c parser.c redir.c show.c trap.c \
	output.c var.c test.c kill.c syntax.c
GENSRCS=builtins.c init.c nodes.c
GENHDRS=builtins.h nodes.h token.h nodenames.h optinit.h
SRCS=	${SHSRCS} ${GENSRCS}

DPSRCS+=${GENHDRS}

LDADD+=	-ledit -lterminfo
DPADD+=	${LIBEDIT} ${LIBTERMINFO}

# Environment for scripts executed during build.
SCRIPT_ENV= \
	AWK=${TOOL_AWK:Q} \
	MKTEMP=${TOOL_MKTEMP:Q} \
	SED=${TOOL_SED:Q}

CPPFLAGS+=-DSHELL -I. -I${.CURDIR} -I${NETBSDSRCDIR}/lib/libedit
CPPFLAGS+= -DUSE_LRAND48
CPPFLAGS+= -DREJECT_NULS

#XXX: For testing only.
#CPPFLAGS+=-DDEBUG=1
#COPTS+=-g
#CFLAGS+=-funsigned-char
#TARGET_CHARFLAG?= -DTARGET_CHAR="unsigned char" -funsigned-char

# Reproducible build parameters ... export into sh for NETBSD_SHELL setting
.if ${MKREPRO_TIMESTAMP:Uno} != "no"
BUILD_DATE!=	${TOOL_DATE} -u -r "${MKREPRO_TIMESTAMP}" "+%Y%m%d%H%M%S"
# These are (should be) equivalent, but the 2nd is easier to understand
#CPPFLAGS+= -DBUILD_DATE='"${BUILD_DATE:C/([^0]0?)(00)*$/\1/}Z"'
CPPFLAGS+= -DBUILD_DATE='"${BUILD_DATE:S/00$//:S/00$//:S/00$//}Z"'
.endif

.ifdef SMALLPROG
CPPFLAGS+=-DSMALL
.endif
.ifdef TINYPROG
CPPFLAGS+=-DTINY
.else
SRCS+=printf.c
.endif

.PATH:	${.CURDIR}/bltin ${NETBSDSRCDIR}/bin/test \
	${NETBSDSRCDIR}/usr.bin/printf \
	${NETBSDSRCDIR}/bin/kill

CLEANFILES+= ${GENSRCS} ${GENHDRS} sh.html1
CLEANFILES+= trace.*

token.h: mktokens
	${_MKTARGET_CREATE}
	${SCRIPT_ENV} ${HOST_SH} ${.ALLSRC}

.ORDER: builtins.h builtins.c
builtins.h builtins.c: mkbuiltins shell.h builtins.def
	${_MKTARGET_CREATE}
	${SCRIPT_ENV} ${HOST_SH} ${.ALLSRC} ${.OBJDIR}
	[ -f builtins.h ]

init.c: mkinit.sh ${SHSRCS}
	${_MKTARGET_CREATE}
	${SCRIPT_ENV} ${HOST_SH} ${.ALLSRC}

.ORDER: nodes.h nodes.c
nodes.c nodes.h: mknodes.sh nodetypes nodes.c.pat
	${_MKTARGET_CREATE}
	${SCRIPT_ENV} ${HOST_SH} ${.ALLSRC} ${.OBJDIR}
	[ -f nodes.h ]

nodenames.h: mknodenames.sh nodes.h
	${_MKTARGET_CREATE}
	${SCRIPT_ENV} ${HOST_SH} ${.ALLSRC} > ${.TARGET}

optinit.h: mkoptions.sh option.list
	${_MKTARGET_CREATE}
	${SCRIPT_ENV} ${HOST_SH} ${.ALLSRC} ${.TARGET} ${.OBJDIR}

.if ${USETOOLS} == "yes"
NBCOMPATLIB=   -L${TOOLDIR}/lib -lnbcompat
.endif

.if !defined(__MINIX)
SUBDIR.roff+=USD.doc
.endif # !defined(__MINIX)

COPTS.printf.c = -Wno-format-nonliteral
COPTS.jobs.c = -Wno-format-nonliteral
COPTS.var.c = -Wno-format-nonliteral

# XXXGCC12 - only on some targets
COPTS.parser.c+=	${${ACTIVE_CC} == "gcc" && ${HAVE_GCC:U0} >= 12:? -Wno-error=clobbered :}

.include <bsd.prog.mk>
.include <bsd.subdir.mk>

${OBJS}: Makefile
