#
#   Tests assembling instructions with the same name
#   but with different operands.
# 

main:
	dum3 r1,r1,r2
    	dum3 r1,r1,0xccc
	dum3 r1,r10,0xdc0000
	