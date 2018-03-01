				;;
				;; Test to make sure that we'll detect when operands should be the same
				;; in order to be able to choose a given instruction.
				;; 
				add2 r1,r7,r10
				add2 r2,r2,r5   				; premium
				add2 r10,r1,r6
				add2 r5,r5,r6
				add2 r2,r3,r2						; premium
				