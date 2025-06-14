/**
 * @file wait.s
 * @brief C library -- wait
 *
 * pid = wait();
 * or,
 * pid = wait(&status);
 *
 * @return pid == -1 if error
 *         status indicates fate of process, if given
 */

.globl	_wait, _nargs, retrn, cerror

_wait:
	mov	r5,-(sp)
	mov	sp,r5
	jsr	pc,_nargs
	mov	r0,-(sp)
	sys	wait
	bec	1f
	tst	(sp)+
	jmp	cerror
1:
	tst	(sp)+
	beq	2f
	mov	r1,*4(r5)	/ status return
2:
	jmp	retrn
