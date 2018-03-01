	.file	"t1.c"
gcc2_compiled.:
	.section	".text"
	.align 2
	.globl main
	.type	 main,@function
main:
	stwu 1,-16(1)
	stw 31,12(1)
	mr 31,1
	lis 9,i@h
	li 0,10
	stw 0,i@l(9)
.L2:
	lwz 11,0(1)
	lwz 31,-4(11)
	mr 1,11
.Lfe1:
	.size	 main,.Lfe1-main
	.ident	"GCC: (GNU) 2.95.3 19991030 (prerelease)"
