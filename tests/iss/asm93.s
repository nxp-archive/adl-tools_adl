	## 
	## Test that we properly clear fields before inserting relocations.
	##
	## In the final results, the address in the lwz should match the address of
	## 'foo' in the symbol table.
	##
	.globl _start
_start:
	lwz r1,foo@l(r0)
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.section	.rodata
	.align	2
	.align	2
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
foo:	
	.word	0x12345678
	
	
	
