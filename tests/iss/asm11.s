.align 8
#rem comment	
	[ foo2 a,PCPC,0x1	foo2 a,PCPC,0x4  // comment
	foo2 a,PCPC,0x2 ] [
	foo2 a,PCPC,0x3	
	foo2 a,PCPC,0x4 


	]
| comment

	foo2 a,PCPC,0x3	
	foo2 a,PCPC,0x3	
	foo2 a,PCPC,0x3	
	foo2 a,PCPC,0x3	
	[foo2 a,PCPC,0x1]
	foo6 (r1+r4)
	foo6 (r1+4)
  	foo9 0xffff
	fo10 0xffff

	
