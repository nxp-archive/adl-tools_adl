				;; 
				;; Simple test of using instruction separator characters.
				;; 
				ift add r1,r2,r13 |	iff add r1,r2,r3
				move.l (20),r1 | add r1,r2,r4 / cmpeq r1,r4
				[ add r3,r2,r1 |
				  add r4,r5,r6 ]
