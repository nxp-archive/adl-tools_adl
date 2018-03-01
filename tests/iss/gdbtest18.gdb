# Make sure that we consider the current core to be whatever
# was last stepped in a vCont command, if just stepping,
# regardless of prior Hg commands.  This seems to be how gdb
# works, though I can't find any documentation on this behavior.
!
T01
T02
Z1,10000204,4
Hg1
p40
Hg2
p40
vCont;c
Hg1
p40
Hg2
p40
vCont;s:1
vCont;s:1
vCont;s:1
vCont;s:1
vCont;s:1
p40
@
