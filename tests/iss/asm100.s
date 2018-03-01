	//
	// Test fixups with a model which has a queue size > 1.
	// 
	.align 8

start:	
	move r1,foo@lo
	nop
	nop
	nop
	nop
	nop
	nop
	move r1,start@lo	
	
foo:	
	add_n d12,d12,d12
	add_n d2,d2,d2
	add_n_al d0,d1
