	##
	## Test to make sure that we can use a custom intrinsic and psuedo-ops.
	## 
	add a0,a1,SP									# This should use the specific form.
	add a0,a1,a2									# Register-register form.
	add a0,a1,HI									# Standard immediate form- HI creates a relocation.
	
