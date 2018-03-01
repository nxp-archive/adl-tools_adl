#
# Tests the switching back and forth between instruction tables.
#
	
 loop:
     add     r2,r2,r1
     subic.  r3,r3,1
     bne     loop
     mtspr   HID0,r4

    .switch -mmixed
     se_add  rr9,rr8	 
	
 loop2:
     se_add  rr9,rr8
     subf.   r26,r27,r26
     se_bne  loop2
	 se_or   0,0
	
	.switch -mdefault

	add		r3,r2,r1
	add		r6,r5,r4
	
