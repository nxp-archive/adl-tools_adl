#
# Simple driver for testing ihex loading.
#
				.globl _start
_start:				
				li r2,100
				stw r2,0x1000(r0)
				lwz r3,0x1000(r0)
				addi r3,r3,100
				stw r3,0x100c(r0)
				.long 0x0
				
				