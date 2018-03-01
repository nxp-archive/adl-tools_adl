
setdynparm L1d-cache-line-size 32
setdynparm L1d-cache-num-sets 32
setdynparm L1d-cache-num-ways 3
setdynparm TlbCam-tlb-num-ways 20

puts "L1d cache line size :  [getdynparm L1d-cache-line-size]"
puts "L1d cache set size  :  [getdynparm L1d-cache-num-sets]"
puts "L1d cache way size  :  [getdynparm L1d-cache-num-ways]"
puts "TlbCam tlb way size :  [getdynparm TlbCam-tlb-num-ways]"

