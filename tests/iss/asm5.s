	.file	"sieve.c"
gcc2_compiled.:
	.section	".text"
	.align 2
	.globl main
	.type	 main,@function
main:
	li 7,5
	mtctr 7
	lis 9,0x0
	lis 11,0x0
	la 9,0x1000(9)
	la 11,0x2000(11)
	li 8,1
	li 0,0
	li 10,0
.L26:
	stwx 8,10,9
	stwx 0,10,11
	addi 10,10,4
	bdnz .L26
	lis 9,0x0
	la 9,0x1000(9)
	mr 5,9
	li 8,2
.L11:
	slwi 7,8,2
	lwzx 0,9,7
	addi 6,8,1
	cmpwi 0,0,0
	bc 12,2,.L10
	add 10,8,8
	cmpwi 0,10,4
	bc 12,1,.L10
	slwi 0,10,2
	add 11,0,5
	mr 0,7
	li 7,0
.L16:
	add 10,10,8
	cmpwi 0,10,4
	stw 7,0(11)
	add 11,11,0
	bc 4,1,.L16
.L10:
	mr 8,6
	mullw 0,8,8
	cmpwi 0,0,4
	bc 4,1,.L11
	li 0,3
	mtctr 0
	lis 9,0x0
	la 9,0x1000(9)
	lis 11,0x0
	addi 9,9,8
	la 11,0x2000(11)
	li 10,2
.L25:
	lwz 0,0(9)
	addi 9,9,4
	cmpwi 0,0,0
	bc 12,2,.L21
	stw 10,0(11)
	addi 11,11,4
.L21:
	addi 10,10,1
	bdnz .L25
	li 3,0
.Lfe1:
	.size	 main,.Lfe1-main
	.comm	sieve,20,4
	.comm	results,20,4
	.ident	"GCC: (GNU) 2.95.3 19991030 (prerelease)"
