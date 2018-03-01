//
// Example ADL syntax using a hypothetical custom language.
//
module zen {

  context_resources {
    CIA {
      bits = [0 31]
    }
    CR {
      fields = { CR0=[32 35] CR1=[36;39] CR2=[40 43] CR3=[44 47] CR4=[48 51]
                      CR5=[52 55] CR6=[56 59] CR7=[60 63] }
      writehook=cr_write_hook
      readhook=cr_read_hook
    }
  }

  settings {
    default_instruction_size = 32
  }

  instructions {
    addi {
      size=32
      bits=0x38000000
      fields=[RT RA SI]
      action= {{
        if ( RA = 0 ) {
          GPR[RT] = SignExtend(SI);
        } else {
          GPR[RT] = GPR[RA] + SignExtend(SI);
        }
      }}
    }

    bcctrl {
      bits=0x4c000421
      fields=[BO BI]
      action={{
        var cond_ok := BO[0] || (CR[BI+32] ^ BO[1]);
        if (cond_ok) {
          NIA = zero[0:31] # CTR[32:61] # zero[0:1];
        } else {
          NIA = CIA + 4
        }
      }}
    }
  }
}
