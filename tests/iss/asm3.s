#
#   Simple test of an instruction with a flag
#   and of inverted fields.
# 
		.file	"t1.c"
gcc2_compiled.:
	.section	".text"
	.align 2
	.globl main
	.type	 main,@function
main:
	dum1,1 r3,r4
	dum1,2		r3,r4
	dum1  ,3		r3,r4
	dum1,foo 5,8
	dum1 r3,r4
	dum2 r3,r4,0x75
	
