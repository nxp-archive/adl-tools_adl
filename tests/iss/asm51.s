				;;
				;; Make sure that we will use the longest version of an instruction with an immediate
				;; field if we encounter an unresolved symbol.
				;; 
				mv r1,3,r2
				mv r1,foobar,r2 				; Will be the 32-bit version.
				mv r1,(pc+bar),r3       ; Will use mv_pc
				mv r1,4,r2
				