#
# Test to make sure that we can handle expressions of the form sym+constant@reloc.
# This is often used with array references.  The value should go into the relocation addend
# and the modified value should go into the instruction's field, in order to allow the code
# to run w/o linking.
#
	addis r2,r1,foobar+100@h
	addi r2,r1,foobar+100@l
	addis r3,r1,foobar+65536@h
	addi r3,r1,foobar+65536@l				
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0

	.type	foobar,@object   # @sixty_four_kb
	.comm	foobar,69632,1
