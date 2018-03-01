	.align 8        

  foo4 l4,r0,0x1111	foo2 a,PCPC,0x1111 	foo2 a,PCPC,0x2222 	
  foo2 a,PCPC,0x3333 	  foo1 l2	
  foo2 a,PCPC,0x4 	        foo1 l1
l1:
	foo2 a,PCPC,0x5 	foo6 a,PC,l2,10

  foo1 l2   foo3 b,r1 
l2:
l3:
	foo1 l3
l4:
	instr_new PC
lowend:
	move r1,(<lowend) 	move r1,(>lowend) 	move r1,(lowend)
