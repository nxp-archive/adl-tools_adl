
	.section ".data"
	.org 0x40
	.long 20
	
	.section ".text"
	.global _start
_start:
	addi r1,0x40
	lw r2,0(r1)
	addi r2,10
	sw r2,0x4(r1)
	addi r2,10
	nop1
	nop1
	nop1
	nop1
	nop1
	nop1
	nop1
	nop1
	nop1
	nop1
	nop1
	nop1
	nop1
	nop1
	nop1	
	sw r2,0x8(r1)
