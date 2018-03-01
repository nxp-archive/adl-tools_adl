	##
	## Test to make sure that we can use a custom intrinsic and psuedo-ops.
	## 
	addi r1,r2,scale(4)
	addi r1,r2,scale(3)
	addi r3,r5,scale(10)
	.word 0x12345678
	.half 0xdead
	.half 0xbeef
	
