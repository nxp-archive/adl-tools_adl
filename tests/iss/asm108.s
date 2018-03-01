	##
	## Test relocations in 36-bit micro-ops.
	## 
	.global _start
_start:
	## Test explicitly specified relocations 
	addi r1,r1,foo@lo
	nop
	## Test implicit relocations.
	jmp foo
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
foo:
	subi r2,r2,10
	
