
_start:	
	c.addi4spn a0,sp,0x1fc
	addi a0,a0,10
	c.lw a1,0(a0)
	c.sw a1,4(a0)
	
