				#
				# Testing of instruction prefix fields and permutable fields.
				#
				foo 10,11
				foo.down.right1 1,2
				foo.left1.a4 3,4    		// a4 = a1
ng if2  foo.left3 12,13					// left3 = left1
		ng	foo.up2 14,15           // up2 = up1
	if3   foo.right0 16,17
	if0   foo 18,19               // if0 = "".
				foo 2(13)
  if2   foo 3(12)
ng if1  foo 4(9)
				