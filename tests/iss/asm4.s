	#
	# Simple test of setting overlay bits.  In this
	# case, we're setting the Y bit for static branch
	# prediction.				
	# 
	.file	"t1.c"
gcc2_compiled.:
	.section	".text"
	.align 2
	.globl main
	.type	 main,@function
main:
	beq+ -16
	beq+ 0x80
	blt- -32
	blt- 0x80
	