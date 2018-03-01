#
# Test to make sure that we can handle expressions of the form sym@reloc + constant.
# This is often used with structure references.
#
	lwz r1,L2@l+4(r0)
	addis r2,r1,L2@h
	addi r2,r1,L2@l+4
	lwz r3,L2@l+8(r0)
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0

L2:
	.long 40
	.long 60
	.long 80
	.long 0x0

