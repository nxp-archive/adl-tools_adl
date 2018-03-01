#
# Make sure that we put a relocation's value (if it's local) into the proper field.
#
	lwz r1,L2@l(r0)
	addis r2,r1,L2@h
	addi r2,r1,L2@l
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0

L2:
	.long 40
	.long 0x0
				