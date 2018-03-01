	##
	## Testing of branches with micro-ops.
	##
	##
	addi r1,r2,1
	addi r1,r2,2
L1:	
	addi r1,r2,3 ; mulli r3,r4,20
	addi r1,r2,4
	addi r1,r2,5
	addi r1,r2,6
	addi r1,r2,7
	addi r1,r2,8 ; bc r2,L1,1
