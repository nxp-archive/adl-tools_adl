#
# Sets up debugger reigster mappings for two threads modeled as cores.
#

set regs ""
for {set i 0} {$i < 32} {incr i} {
	lappend regs "GPR$i";
}
for {set i 0} {$i < 32} {incr i} {
	lappend regs 32;
}
lappend regs NIA 32 CR LR CTR XER 64 32

eval setregmap PMT:t0 $regs
eval setregmap PMT:t1 $regs
