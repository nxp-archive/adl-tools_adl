	##
	## Test that we can have a large number of pseudo fields which map to a single target field.
	## This makes sure that we don't overflow our operand_values array within the assembler.
	##
	cmw r0
	cmw r0,r1
	cmw r0,r1,r2
	cmw r1,r2,r3,r4
	cmw r1,r2,r3,r4,r5,r6,r7,r8,r9
	cmw r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31
	
