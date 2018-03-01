	##
	## Test the use of relocations associated with pseudo instruction fields.
	##
	.global _start
_start:	
	addi r30,foo@lo_b ; addi r31,bar@lo_a
	addi r0,-1
	addi r1,r1,1
	blr
	addi r2,r2,1
	addi r2,r2,2
	addi r2,r2,3
	addi r2,r2,4
	addi r2,r2,5
foo:		
	addi r2,r2,6
	addi r3,r3,7
bar:		
	addi r4,r4,7
	addi r5,r5,8	
	
	
