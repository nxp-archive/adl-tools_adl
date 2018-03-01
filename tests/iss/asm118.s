	##
	## More extensive testing of default values.  We test
	## that this works if the default value is nonzero and
	## if the argument with the default value is not at the end
	## of the syntax string.
	## 
	lwsp r5,[sp]+r10
	nop1
	lwsp r5,[sp+10]+r10
	nop1
	lwsp r5,[sp-10]+r10
	nop1
	lwsp r5,[sp+0xb]+r11
	nop1
	lwsp r5,[sp-0xf]+r11
	nop1
	lwsp r5,[sp+(10+10)]+r11
	nop1
	lwsp r5,[sp-(20-10)]+r11
	nop1	
	swsp r5,[sp+516]
	nop1
	swsp r5,[sp-100]
	nop1
	swsp r5,[sp]
	nop1
