	## 
	## Test fixups/relocations for little-endian variable-length instructions.
	## 
	addi a0,a0,1
bar:	
	addi a0,a0,1
	beqz a1,foo
	addi a1,a1,2
	addi a1,a1,2
foo:
	addi a2,a2,3
	addi a2,a2,3
	beqz t0,bar
	halt
	
