#
# Example with a shorthand instruction using a bitmask (4-operand rlwinm)
# which then disassembles to a different shorthand.
#
	
		.file	"t1.c"
gcc2_compiled.:
	.section	".text"
	.align 2
	.globl main
	.type	 main,@function
main:
	addi 0,31,1
	rlwinm 31,0,0,0xff
	lis 9,Ch_2_Glob@h
.Lfe1:
	.size	 main,.Lfe1-main
	.ident	"GCC: (GNU) 2.95.3 19991030 (prerelease)"
	