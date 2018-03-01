#
# For testing a generated linker.
#
#				
				.global _start
_start:
				addis r1,r0,message@h
				ori   r1,r1,message@l
				lwz   r5,0(r1)
				addis r2,r0,d+29000@ha
				addi  r2,r2,d+29000@l
				stw   r5,0(r2)
				bl	  foo
				.long 0x0
				.long 0x0
				.long 0x0
				.long 0x0
				.long 0x0

				.global foo
				.type foo,@function
foo:
				addi r6,r6,1
				blr
				

        .type   a,@object               # @a
        .comm   a,20000,4
        .type   b,@object               # @b
        .comm   b,4000,4
        .type   c,@object               # @c
        .comm   c,8000,4
        .type   d,@object               # @d
        .comm   d,30000,4

				.global message
message:
				.ascii "Hello, world!\n"
				
				