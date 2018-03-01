#
# Sets up a simple debug register mapping for a 32-bit core.
#

set regs ""
for {set i 0} {$i < 32} {incr i} {
	lappend regs "GPR$i:32";
}
for {set i 0} {$i < 32} {incr i} {
	lappend regs 64;
}
lappend regs NIA:32 MSR:32 CR:32 LR:32 CTR:32 XER:32 32

eval setregmap root $regs
