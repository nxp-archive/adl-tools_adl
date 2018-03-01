# Make sure that we can assemble instructions w/large immediates.
	jmp (0xffe)
	jmp (128)
	jmp (30)
	jmp0
	
