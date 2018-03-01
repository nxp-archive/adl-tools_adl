#
# Simple assembly file with a large gap between the branch and the branch target.
# Tests to make sure that we properly mask addresses for symbol printing.
#
				.file   "sieve.c"
gcc2_compiled.:
      .section        ".text"

      .align 2
      .globl main
      .type    main,@function
main:
				addi 1,1,1
				addi 1,1,1
				addi 1,1,1
				addi 1,1,1
				addi 1,1,1
				addi 1,1,1
				b func2

      .section        ".text2"				
				
      .align 2
      .globl func2
      .type    func2,@function
func2:
				addi 1,1,1
				addi 1,1,1
				addi 1,1,1
				addi 1,1,1
				addi 1,1,1
				addi 1,1,1
				blr
