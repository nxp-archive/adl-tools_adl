;;
;; Test to make sure that we can handle a 0-operand instruction in the same VLES
;; as another instruction.
;;
				nop
				inc0 add r1,r2,r3
				inc0 inc0
				nop
				nop
				