	.align 8
	skipls l4
	move r1,1
l4:	move r2,2

	
	cmpeq r1,r1
	ift add r1,r2,r13  iff add r1,r2,r3
	loopend0

	skipls l5 
	cmpeq r1,r2
	ift add r1,r2,r13  iff add r1,r2,r3
	cmpeq r1,r2
l5:	ift add r1,r2,r13  iff add r1,r2,r3
	loopend0
	cmpeq r1+btr0
	move.l (sp+r15),r1
	move.l (sp+r16),r1
	move.l (sp+r15),r16
	move.l (r15),r1
	move.l (r16),r3
	move.l (r1+r3),r34
	move.l ($34),r14

	doensh0
	move.l (r1+r3),r34
	loopend0
	
	doen0
	move.l (r1+r3),r34
	loopend0

	add_n d12,d12,d12
	add_n d2,d2,d2
	add_n_al d0,d1
