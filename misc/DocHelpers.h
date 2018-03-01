//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Various helper functions used by the database and documentation generator.
//

#ifndef _DOCHELPERS_H_
#define _DOCHELPERS_H_

#include "opencxx/parser/PtreeArray.h"
#include "opencxx/parser/PtreeBlock.h"

#include "helpers/gc_hash_set.h"

#include "fe/Types.h"

#include "DocCfgParser.h"

namespace adl {

  class DocWalker;
  class UsedReg;
  class Named;
  class Instr;
  class IntMap;

  // Used for printing a block w/o printing the braces.
  class PtreeBlockNB : public Opencxx::PtreeBlock {
  public:
    PtreeBlockNB(const Opencxx::PtreeBlock *b) : Opencxx::PtreeBlock(*b) {}
    PtreeBlockNB(Opencxx::NonLeaf *nl) : Opencxx::PtreeBlock(nl->Car(),nl->Cdr()) {}
    
    Opencxx::Ptree* Translate(Opencxx::AbstractTranslatingWalker*);
    int Write(std::ostream&, int);    
  };
 


  // Sort objects that have bit ranges (returning a bits vector via
  // a bits() method).  The objects with the lowest low bits come
  // first, where the longest field is chosen for equal low bits.
  // Barring that, a simple alphabetical sort is done.
  template < typename has_bits_method >
  struct BitsSort: public std::binary_function< const has_bits_method*,const has_bits_method*,bool > {
    bool operator()(const has_bits_method *x,const has_bits_method *y) {
      auto xbits = x->bits();
      auto ybits = y->bits();
      bool result = (xbits.lowbit() < ybits.lowbit());
      if (xbits.lowbit() == ybits.lowbit()) {
        result = (xbits.width() > ybits.width());
        if (xbits.width() == ybits.width()) {
          return (x->name() < y->name());
        }
      }
      return result;
    }
  };

  // Sort objects that have bit ranges (returning a bits vector via a bits()
  // method).  Sort by total size of bits, so that larger ranges come first.
  // After that, sort alphabetically.
  template < typename has_bits_method >
  struct BitsSizeSort: public std::binary_function< const has_bits_method*,const has_bits_method*,bool > {
    bool operator()(const has_bits_method *x,const has_bits_method *y) {
      auto xw = x->bits().width();
      auto yw = y->bits().width();
      if (xw != yw) {
        return xw > yw;
      } else {
        return (x->name() < y->name());
      }
    }
  };

  // Given a used-register item, attempt to match masks to register fields.  If a complete covering
  // set of fields cannot be found, false is returned.
  bool getUsedFields(StrVect &fieldNames,const UsedReg &,bool little_endian,
                     const gc_hash_set<const Named*> *hidden_items);

  // Transform an expression based upon fields and parameters from an alias.
  Opencxx::Ptree* replaceAliasParms(Opencxx::Ptree *ex,DocWalker &walker,const Instr::Alias *alias);

  // If given a block, then this returns a non-brace-printing block.
  Opencxx::Ptree* extractBlockBody(Opencxx::Ptree *ex);

  // Perform operation transformations and expression expansion repeatedly until
  // complete.
  Opencxx::Ptree* doOpTransforms(Opencxx::Ptree *op,bool expand_exprs,DocWalker *walker,const DocCfg &docfg);

}

#endif
