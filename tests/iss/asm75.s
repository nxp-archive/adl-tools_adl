
	.global _start
	.type _start,@function
	_start:
 	 lis  r3, str1@h
 	 addi r3, r3, str1@l
	 bl   main
	 .long 0x0

	.globl	main
	.align	2
	.type	main,@function
main:                                   # @main
	blr
				

	.section	.rodata.str1.1,"aMS",@progbits,1
str1:
	.asciz	"hello, world!\n"
	.size	str1, 15
