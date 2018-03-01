#
# Sets up a simple debug register mapping for a 32-bit core.
#

set regs ""
for {set i 0} {$i < 32} {incr i} {
	# Note: It's not necessary to specify a width, but this is being done for
	# testing purposes.
	lappend regs "GPR$i:32";
}
for {set i 0} {$i < 32} {incr i} {
	lappend regs 64;
}
lappend regs NIA 32 CR LR CTR XER 32

set plist [pathlist cores]
foreach p $plist {
	puts "Setting register map for $p"
	eval setregmap $p $regs
}
