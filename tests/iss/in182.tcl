
setdynparm L1d-cache-line-size 256
setdynparm L1d-cache-num-sets 512
setdynparm L1d-cache-num-ways 8
setdynparm L1i-cache-size 16384
setdynparm TlbCam-tlb-num-ways 20

puts "L1d cache size      :  [getdynparm L1d-cache-size]"
puts "L1d cache line size :  [getdynparm L1d-cache-line-size]"
puts "L1d cache set size  :  [getdynparm L1d-cache-num-sets]"
puts "L1d cache way size  :  [getdynparm L1d-cache-num-ways]"

puts "L1i cache size      :  [getdynparm L1i-cache-size]"
puts "L1i cache line size :  [getdynparm L1i-cache-line-size]"
puts "L1i cache set size  :  [getdynparm L1i-cache-num-sets]"
puts "L1i cache way size  :  [getdynparm L1i-cache-num-ways]"

puts "TlbCam tlb way size :  [getdynparm TlbCam-tlb-num-ways]"
