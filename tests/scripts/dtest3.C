//
// A simple driver program demonstrating reading an ELF file and then using the
// disassemble_to_struct function to disassemble instructions to a structure.
//

#include <iostream>
#include <iomanip>
#include <stdexcept>

#include "iss/ElfBaseReader.h"
#include "iss/Memory.h"

#include "instr-info-mod1.h"


using namespace std;
using namespace adl;

class MyElfReader : public ElfReaderIface {

public:

  MyElfReader(BlockMemory &mem) : _mem(mem) {};

  virtual void initData(addr_t addr,uint32_t data,bool log_only)
  {
    _mem.write32(addr,data,4);
  }

  virtual void initCode(addr_t addr,uint32_t data,bool log_only)
  {
    _mem.write32(addr,data,4);
  }


  BlockMemory &_mem;
};


int main(int argc,const char *argv[])
{
  try {
    if (argc < 2) {
      cout << "Usage:  " << argv[0] << " <input-file>\n";
      return 1;
    }

    BlockMemory bmem;
    MyElfReader myer(bmem);

    unique_ptr<ElfBaseReader> er(createElfReader(argv[1],myer));

    er->read();

    // Disassemble to a structure until we run into an invalid or halt
    // instruction.

    cout << "Registers:  ";
    for (int i = 0; i != ppc_NumRegs; ++i) {
      cout << ppc_regs[i] << ' ';
    }
    cout << "\n\n";

    ppc_InstrInfo info;
    addr_t addr = 0;

    cout << "Disassembly:\n";
    while(1) {
      
      uint32_t d = bmem.read32(addr);

      ppc_disassemble(&info,&d);

      if (info._type == instr_invalid || info._type == instr_halt) break;

      cout << hex << showbase << setw(8) << addr << ":  " << dec << info._name;
      for (unsigned i = 0; i != info._numargs; ++i) {
        cout << ' ' << info._args[i];
      }
      cout << '\n';

      addr += ( info._size / 8 );
    }

  } 
  catch (exception &err) {
    cerr << "Error:  " << err.what() << endl;
    return 1;
  }
}

namespace adl {

  struct IssNode;

}

// Dummy functions needed for linking.
namespace adliss {

  void resetGlobal() {}

  adl::IssNode *createTopLevelNode(unsigned &id)
  {
    return 0;
  }

  const char *getModelPreamble()
  {
    return 0;
  }


}
