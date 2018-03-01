#
# This polls the ISS, which is running an endless loop which
# is incrementing memory at these locations.  The returned data
# should be monotonically increasing and the value at 0x2000 should be
# greater than the value at 0x1000.
#
:noresp:c
:sleep:1
:lit:\003
m00001000,4
m00002000,4
:noresp:c
:sleep:1
:lit:\003
m00001000,4
m00002000,4
:noresp:c
:sleep:1
:lit:\003
m00001000,4
m00002000,4
:noresp:c
:sleep:1
:lit:\003
m00001000,4
m00002000,4
:noresp:c
:sleep:1
:lit:\003
m00001000,4
m00002000,4
# Then kill the program.
@
