//
// Instruction class for Graphite performance model integration.  This store
// information about a single instruction- its operands and an instruction type.
// This is created at translation time and then fed to the performance model
// when the instruction is executed.
//

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "helpers/Macros.h"

#include "GraphiteInstr.h"

using namespace std;

namespace adl {

  // Operand

  ostream &operator<<(ostream &out,const Operand &o)
  {
    // Type
    if (o._type == Operand::REG) {
      out << "REG-";
      // Value
      out << o._value << "-";
      // Direction
      if (o._direction == Operand::READ)
        out << "READ, ";
      else
        out << "WRITE, ";
    }
    else if (o._type == Operand::MEMORY) {
      out << "MEMORY-";
      // Direction
      if (o._direction == Operand::READ)
        out << "READ, ";
      else
        out << "WRITE, ";
    } else {
      RError("Unrecognized Operand Type " << o._type);
    }

    return out;
  }


  // Instruction
  ostream &operator<<(ostream &out,const Instruction &instr)
  {
    out << "Instr " << instr._type << ": ";
    for (auto &o : instr._operands) {
      out << o;
    }
    out << '\n';
    return out;
  }

}
