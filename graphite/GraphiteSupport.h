//
// Any types needed by the ADL Graphite performance-model integration.
//
// Note: This is not a true integration with the full MIT Graphite simulator.
// Instead, we support a performance model with a similar design to Graphite's.
// This one receives performance data on a per-instruction basis to simplify
// things vs. when basic blocks are used.
//

#ifndef _GRAPHITE_SUPPORT_
#define _GRAPHITE_SUPPORT_

#include <memory>

#include "GraphiteInstr.h"
#include "GraphiteCore.h"

namespace adl {

  // Helper class for decode items- stores a pointer to an instruction item.
  struct GraphiteDecodeItem {
    GraphiteDecodeItem() {};

    void set_instr(Instruction *instr) { _instr.reset(instr); };

    void reset() { _instr.reset(0); };

    std::unique_ptr<Instruction> _instr;
  };

}

#endif
