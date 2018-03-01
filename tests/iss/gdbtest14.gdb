# Test to make sure we support more of the v type packets.  Also, when done, we
# should return X00, rather than an S or T packet, to avoid a bug in newer
# versions of gdb.
!
vMustReplyEmpty
vCont;s:-1;c
vCont;s:-1;c
vCont;c
vKill;a410
?
vRun;
?
vCont;c
?
mon quit
