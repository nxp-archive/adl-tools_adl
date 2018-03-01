				;; Note:  Disassembly of PR isn't very clean at this point, because
				;; we don't yet automatically grab bits from the prefix when disassembling.
				;; That's still to be done...
	  nop
		pr3			mv r2,0xf,r4	ADD r1,r2,r3	pr2	add r4,r5,r6  mv r4,0xf,r7
					  mv r2,0xf,r4	pr2 add r1,r2,r3	ADD r4,r5,r6  pr3 mv r4,0xf,r7
		pr1 pl1 add r5,r1,r3
				
				
