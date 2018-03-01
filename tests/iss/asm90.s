	##
	## Make sure that we catch undefined symbols, even if they're used by shorthand
	## instructions with modifier functions.
	##
	##
	rlwinm r1,r2,5,0x0000ffff+foo
	
