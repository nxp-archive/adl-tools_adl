#
# Test local fixups and relocations for large instructions.
#
				.global _start
_start:
				jump foo
				jump bar
				next
				next
bar:
				next
				
		.global foo			
foo:
				next
				.end
