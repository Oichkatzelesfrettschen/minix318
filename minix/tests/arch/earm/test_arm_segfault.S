.text
.global main
main:
	push {lr}
	
	@ First attempt - try to access unmapped memory
	ldr r0, =0xDEADBEE0 /* Hopefully this is not mapped... */
	ldr r1, [r0]        /* This should cause segfault */
	
	@ Second attempt - if we somehow survived the first
	ldr r0, =0x01010100 /* In case we survived, try something else */
	ldr r1, [r0]        /* This should also cause segfault */

	@ If we reach here, the test failed
	ldr r0, =msg
	bl puts

	mov r0, #1          /* Return non-zero to indicate test failure */
	pop {pc}

.data
msg:
	.asciz "ERROR - caused no segfault\n"  /* Fixed: use .asciz for proper null termination */
