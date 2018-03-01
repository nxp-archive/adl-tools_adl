#
# Test of encoding with allow_conflict fields.
#
				imm r2,20
				imm r3,30
				add r1,r2,r3
				sub_1st r4,r1,r2
 				sub_2nd r5,r1,r3
				mul_1st r6,r2,r3
				mul_2nd r7,r1,r3
				add r8,r7,r2
				add r9,r8,r2
				