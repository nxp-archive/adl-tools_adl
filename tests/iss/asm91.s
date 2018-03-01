	//
	// Test relocations using function syntax: %reloc(sym).
	//
	.align 2
	.globl main
	.type	 main,@function
main:
	stwu 1,-16(1)
	stw 31,12(1)
	mr 31,1
	lis 9,%h(i)
	li 0,10
	stw 0,%l(i)(9)
.L2:
	lwz 11,%l(foo_bar_stuff)(1)
	lwz 31,-4(11)
	mr 1,11
