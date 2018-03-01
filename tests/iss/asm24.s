;
; Simple test of instruction re-ordering via the post-packet assembler hook.
; 
	add r3,r1,r4 sub r3,r2,r1  asub r3,r1,r2 asub r6,r4,r5
	sub r7,r6,r1 move 3,r2 
