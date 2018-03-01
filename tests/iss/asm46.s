
;; Prefix will be generated.
[ add r8,r3,r12 add r8,r1,r2 add r1,r5,r12 ] ; comment
;; Prefix will be generated.
mv  r1,10,r15 mv  r9,10,r15
;; No prefix.
mv  r1,10,r2
nop 
nop 
.long 0x0

				