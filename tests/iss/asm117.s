  ##
  ## Make sure that we can adjust multiple labels that are all on the same line.
  ##
  .global _start
_start:
  addi r2,r2,1
  addi r2,r2,2
  addi r2,r2,3
foo:
bar:
stuff:	
  addi r2,r2,4
  addi r2,r2,5
