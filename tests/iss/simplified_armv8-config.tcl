set regs ""

for {set i 0} {$i < 31} {incr i} {
      lappend regs "X$i:64";
}

# sp
lappend regs SP_EL0:64
# PC
lappend regs PC:32
# for temp 0 padding
lappend regs FPSR:32
# CPSR
lappend regs cpsr:64  
# fpsr
lappend regs FPSR:64
# FPCR
lappend regs FPCR:64

eval setregmap root $regs
setregendianness root big

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
