#
# Test to make sure that we can handle data section relocations.
#
	.file	"test.c"
	.gnu_attribute 4, 1
	.gnu_attribute 8, 1
	.gnu_attribute 12, 1

	.section	".text"
	.align 2
	.globl foo
	.type	foo, @function
foo:
	stwu 1,-32(1)
	stw 31,28(1)
	mr 31,1
	lis 0,coefLowPass@h
	mr 9,0
	la 0,coefLowPass@l(9)
	stw 0,8(31)
	lis 0,firLow1.1279@h
	mr 9,0
	la 0,firLow1.1279@l(9)
	mr 3,0
	addi 11,31,32
	lwz 31,-4(11)
	mr 1,11
	blr

	.globl coefLowPass
	.section	".data"
	.align 2
	.type	coefLowPass, @object
	.size	coefLowPass, 12
coefLowPass:
	.long	6
	.long	7
	.long	8
				
	.size	foo, .-foo
	.section	".data"
	.align 2
	.type	firLow1.1279, @object
	.size	firLow1.1279, 12
firLow1.1279:
	.long	coefLowPass
	.long	0
	.long	35
	.long	0
	.ident	"GCC: (Sourcery CodeBench Lite 2011.09-105) 4.5.1"
