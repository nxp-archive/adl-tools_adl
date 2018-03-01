	## Both groups should result in the same opcodes,
	## since a macro will compress the addi instructions if possible.
_start:
	addi			 sp,sp,496
	addi			 sp,sp,-496
	addi			 a0,sp,508
	
	c.addi16sp sp,496
	c.addi16sp sp,-496
	c.addi4spn a0,sp,508
