#
# Modifies the L1d and L1i cache to use 128-bit linefill accesses.
#

setdynparm L1d-cache-linefill-access-width 128
setdynparm L1i-cache-linefill-access-width 128

puts "L1d cache linefill access width :  [getdynparm L1d-cache-linefill-access-width]"
puts "L1i cache linefill access width :  [getdynparm L1i-cache-linefill-access-width]"
