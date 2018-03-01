	##
	## Test to make sure that we catch symbols buried in complex expressions
	## if they're used in aliases where there are modifiers.
	## In the real world, this is usually caused by the preprocessor not
	## expanding a macro.
	## 
.global Foo

Foo:
	addiS r1,((64 + SomeValue) << 24),1
	addiS r1,20,1
