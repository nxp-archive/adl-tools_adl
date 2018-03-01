	##
	## Test to make sure that we flush a multi-element instruction queue when
	## we encounter .org and .section directives.
	## 
	.global _start
_start:
	addi r1,r1,1
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
	addi r2,r2,2

	.org 0x100
	nop1
	nop1
	nop1
	addi r3,r3,3
	
	.section ".rodata"
	.long 0xdeadbeef
	
	
	
