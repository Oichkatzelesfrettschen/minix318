#ifndef _IPC_CONST_H
#define _IPC_CONST_H

#include <machine/ipcconst.h>

 /* System call numbers that are passed when trapping to the kernel. */
#define SEND		   1	/* blocking send */
#define RECEIVE		   2	/* blocking receive */
#define SENDREC	 	   3  	/* SEND + RECEIVE */
#define NOTIFY		   4	/* asynchronous notify */
#define SENDNB             5    /* nonblocking send */
#define MINIX_KERNINFO     6    /* request kernel info structure */
/* 7-15 are reserved for future use or were used by obsolete calls */
#define SENDA		   16	/* asynchronous send */
#define SYS_REPLY          17   /* reply to a SENDREC caller */
#define SYS_IPC_SEND_CAP   18   /* capability-based send */
#define IPCNO_HIGHEST	SYS_IPC_SEND_CAP
/* Check that the message payload type doesn't grow past the maximum IPC payload size.
 * This is a compile time check. */
#define _ASSERT_MSG_SIZE(msg_type) \
    typedef int _ASSERT_##msg_type[1]

/* Macros for IPC status code manipulation. */
#define IPC_STATUS_CALL_SHIFT	0
#define IPC_STATUS_CALL_MASK	0x3F
#define IPC_STATUS_CALL(status)	\
	(((status) >> IPC_STATUS_CALL_SHIFT) & IPC_STATUS_CALL_MASK)
#define IPC_STATUS_CALL_TO(call) \
	(((call) & IPC_STATUS_CALL_MASK) << IPC_STATUS_CALL_SHIFT)

#define IPC_FLG_MSG_FROM_KERNEL	1 /* this message originated in the kernel on
				     behalf of a process, this is a trusted
				     message, never reply to the sender
				 */
#define IPC_STATUS_FLAGS_SHIFT	16
#define IPC_STATUS_FLAGS(flgs)	((flgs) << IPC_STATUS_FLAGS_SHIFT)
#define IPC_STATUS_FLAGS_TEST(status, flgs)	\
		(((status) >> IPC_STATUS_FLAGS_SHIFT) & (flgs))
#endif /* IPC_CONST_H */
