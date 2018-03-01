	##
	## Test the use of the --ignore-low-bits option to ignore checking
	## of bits which should be zero due to mask constraints.
	##
L1:	
	add r1,r2,r3
	add r1,r2,r3	
L2:
	bc r2,L1,1	
	bc r2,1,1
	bc r2,L2,1
	bc r2,0xfff,1
