#
# Make sure that once we're halted, additional steps return an end-of-program
# message (X00) so that GDB knows we're done.
#
!
vCont;s:-1;c
vCont;s:-1;c
vCont;c
vCont;s
vCont;s
vCont;s
@
