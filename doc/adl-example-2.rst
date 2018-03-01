#
# Python example syntax for an ADL.
#
register(name="CIA",bits=[0,31])
register(name="CR",fields={'CR0':[32,35],'CR1':[36,39],'CR2':[40,43],
          'CR3':[44,47],'CR4':[48,51],'CR5':[52,55],'CR6':[56,59],'CR7':[60,63]},
      writehook=CR_impl.write,
      readhook=CR_impl.read)

def addi(RT,RA,SI):
    if RA == 0:
       GPR[RT] = SignExtend(SI)
    else:
       GPR[RT] = GPR[RA] + SignExtend(SI)

instr(name='addi',size=32,bits=0x38000000,fields=['RT','RA','SI'],action=addi)

default_instruction_size(32)

def bcctrl(BO,BI):
  cond_ok = BO[0] or (CR[BI+32] ^ BO[1])
  if cond_ok:
    NIA = concat(zero[0:31],CTR[32:61],zero[0:1])
  else:
    NIA = CIA + 4
  
instr(name='bcctrl',bits=0x4c000421,fields=['BO',BI'],action=bcctrl)
    