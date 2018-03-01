#
#   Simple test of instructions whose syntax expressions
#   are similar enough to match more than one variant.
#   The feature added in r2794 ensures that instructions
#   using the most enumerated fields will take precedence
#   when parsed by the assembler.  For instance, this
#   ensures that +n2 is matched to an enumeration, and
#   cannot be matched first against a simple numerical
#   field (which would make it +2 instead of +n2).
#
	.file "t1.c"
gcc2_compiled.:
	.section ".text"
	.align 2
	.globl main
	.type main,@function
main:
	# this is the syntax of dum4c
	dum4 r3+n2,r4
	
	# this is the syntax of dum4b
	dum4 r3+3,r4
	
	# this is the syntax of dum4a
	dum4 r3+6,r4
	
