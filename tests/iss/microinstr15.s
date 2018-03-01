//
// Another simple test of using an explicit instruction separator
// for a parallel architecture.
//
rd S0           foo
rd S0, [rS0] 	| foo
rd S0
foo
rd S0, [rS0]
done
