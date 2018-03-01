set regs ""
for {set i 0} {$i < 13} {incr i} {
      lappend regs "r$i:32";
}
# MSP/PSP
lappend regs r13:32
# LR
lappend regs r14:32 
# NIA
lappend regs pc:32

for {set i 0} {$i < 10} {incr i} {
      lappend regs 32;
}
lappend regs cpsr:32





eval setregmap root $regs

# set regs ""
# for {set i 0} {$i < 13} {incr i} {
      # lappend regs "r$i:32";
# }

# lappend regs MSP:32
# lappend regs LR:32 
# lappend regs pc:32 
# lappend regs PSR:32
# lappend regs PSP:32


# eval setregmap root $regs