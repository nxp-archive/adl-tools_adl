	##
	## Make sure that we can set fields in an instruction.  The lflag
	## instruction takes no arguments- it has its 'flag' operand set based
	## upon its sequence in the program.
	## 
	lflag
	addi r1,r2,1
	addi r1,r2,2
	addi r1,r2,3
	addi r1,r2,4
	lflag
	
