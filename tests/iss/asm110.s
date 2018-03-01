	##
	## Make sure that we can handle aliases where the operands can contain
	## multiple symbols, and where a modifier function is used.  In this case, the
	## alias calculates a difference between addresses.
	## 
	.global _start
_start:
	loop r1,A1,A2
	nop
A1:
	nop
	nop
	nop
	nop
A2:
	nop
	
