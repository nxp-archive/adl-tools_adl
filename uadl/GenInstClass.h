//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _GEN_INST_CLASS_H_
#define _GEN_INST_CLASS_H_

#include <iosfwd>

namespace rnumber {
  class RNumber;
}

namespace adl {
  class Instr;
}

namespace uadl {

  class UadlIr;
  class GenConfig;
  class GenInstClass;
  class FileMap;

  gc_string getStateChangeName(const gc_string &from,const gc_string &to);

  gc_string getOpVar(const gc_string &op);

  gc_string getNullActionFunc();

  // Stores the mask index, mask size (in bits), and whether it comes from the
  // operand's source or target.
  struct MaskInfo {
    int  _index;
    int  _size;
    bool _is_src;

    MaskInfo (int index,int size,bool is_src) : _index(index), _size(size), _is_src(is_src) {};
  };
    
  // In order to reduce the code size, we store the masks in an array in each
  // instruction class object.  The trick is to map those indices to the masks
  // for each operand.  This is accomplished via this map.  Then, each factory
  // function does the setup of the masks for its instruction.
  typedef gc_map<const Operand *,MaskInfo > MaskMap;

  void writeMaskInit(std::ostream &,const MaskInfo &,const Operand &);

  struct GenInstClassIF {
    
    GenInstClassIF(const UadlIr &IR,const Thread &thread,std::ostream &outh,std::ostream &outf,std::ostream &outc,
                   const GenConfig &config,const gc_string &iss_type);

    // Returns max-count and max-size of masks.
    std::pair<unsigned,unsigned> get_mask_info();
    
    // Returns the mask map.
    const MaskMap &get_mask_map();

    // Output the entire model.  Updates the instruction operand -> type map.
    void generate(const FileMap &icfiles);
    
  protected:
    GenInstClass *_impl;
  };
  
}

#endif // _CODE_GEN_H
