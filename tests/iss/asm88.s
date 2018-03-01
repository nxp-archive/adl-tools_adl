	## 
	## Testing of sub instructions in a non-parallel architecture,
	## with different instruction separators used to modify instrution combination.
	## 
	addi r1,r2,200								
	add r1,r2,r3
	addi r1,r2,200 = mulli r2,r3,300 
	add r1,r2,r3 ; sub r2,r3,r4 ; mul r4,r5,r6
	halt
	
