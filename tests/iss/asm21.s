	#
	# This is mainly testing to make sure that we can set operands
	# correctly which are formed via expressions from syntax fields,
	# especially in the case where we have fixups.
	#
	.file	"foo.c"
gcc2_compiled.:
	.section	".text"
	.align 2
	.globl main
	.type	 main,@function
main:
.L1:	
	addi r1,r1,1
	addi r1,r1,1
	blt+ 0,.L2
	blt- 0,.L2
	addi r1,r1,1
.L2:
	addi r1,r1,1
	addi r1,r1,1
	bgt- 0,.L1
	bgt+ 0,.L1
