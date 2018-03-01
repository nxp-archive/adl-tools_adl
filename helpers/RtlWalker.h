//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This is a class designed to transform ADL RTL code,
// which is basically C++ with a few extensions, to pure
// C++.  You can derive from this to perform further
// transformations.
//

#ifndef _RTLWALKER_H_
#define _RTLWALKER_H_

#include "opencxx/parser/NonLeaf.h"

#include "AdlTranslatingWalker.h"

namespace adl {

  // This class is used to transfer the knowledge that we've changed the
  // type of a bits object due to a slice operation being performed.
  class PtreeBitSlice : public Opencxx::NonLeaf
  {
  public:
    PtreeBitSlice(char *type,Opencxx::Ptree *p,Opencxx::Environment *e) : 
      Opencxx::NonLeaf(p,0), _type(type), _env(e) {};

    void Typeof(Opencxx::AbstractTypingWalker *w, Opencxx::TypeInfo &ti);
  private:
    char                 *_type;
    Opencxx::Environment *_env;
  };

  // This walks over an expression, transforming ADL RTL
  // extensions into C++.
  class RtlWalker : public AdlTranslatingWalker {
  public:
    // e      :  Initial environment.
    // ld_okay:  Insert line directives.
    // lib_mode: We're in library mode.
    RtlWalker(Opencxx::Environment *e,bool ld_okay,bool lib_mode);
    virtual ~RtlWalker() {};

    // Call this to translate a block of code.
    virtual Opencxx::Ptree* translateCode(Opencxx::Environment *e,Opencxx::Ptree *);

    virtual Opencxx::Ptree *TranslateAssign(Opencxx::Ptree *exp);
    virtual Opencxx::Ptree *TranslateFuncall(Opencxx::Ptree *exp);
    virtual Opencxx::Ptree *TranslateUserPlain(Opencxx::Ptree*);
    virtual Opencxx::Ptree *TranslateDeclaration(Opencxx::Ptree*);
    virtual Opencxx::Ptree *TranslateCond(Opencxx::Ptree*);
    virtual Opencxx::Ptree *TranslateTemplateFunction(Opencxx::Ptree* temp_def, Opencxx::Ptree* fun);

    // If this is a bits object, returns true and updates the size, offset and is_signed
    // parameters accordingly.
    bool get_bits_size(Opencxx::Ptree *p,unsigned &size,unsigned &offset,bool &is_signed);
    bool get_bits_size(Opencxx::TypeInfo &t,unsigned &size,unsigned &offset, bool &is_signed);

    // Helper function- returns a PtreeBitsSlice with a type of bits with the
    // specified size.
    PtreeBitSlice *make_bitslice(Opencxx::Ptree *,unsigned size,unsigned offset = 0);
  
    // Returns a bitslice object with the encoding specified.
    PtreeBitSlice *make_bitslice(Opencxx::Ptree *,const Opencxx::TypeInfo &t);
  
    // Updates t to be a type that encodes bits<x>.
    void generateBitsEncoding(Opencxx::TypeInfo &t,unsigned size,unsigned offset = 0);

    // Special case handling of some variables, e.g. "bits" is a type-name that we always recognize.
    virtual void TypeofVariable(Opencxx::Ptree*, Opencxx::TypeInfo&);

  protected:
    Opencxx::Ptree *translated_arglist(Opencxx::Ptree *args);
    Opencxx::Ptree *translateFunc(Opencxx::Ptree *exp);
    Opencxx::Ptree *translateBlockStmt(Opencxx::Ptree *exp);
    Opencxx::Ptree *translate_ternary(Opencxx::Ptree *exp,Opencxx::Ptree *c,Opencxx::Ptree *c2);

    Opencxx::Ptree *genConcat(Opencxx::Ptree *exp,Opencxx::Ptree *args,Opencxx::Ptree *orig_args);

    bool        _ld_okay;              // Insert line directives. 
    bool        _lib_mode;             // We're in library mode.
    bool        _translating_var_decl;
  };

  // Helper utilities.

  // Tries to find a leaf in the given tree, returns 0 if not found.
  Opencxx::Ptree *find_leaf(Opencxx::Ptree *p);

  Opencxx::Ptree *getLineNumber(Opencxx::Environment *env,Opencxx::Ptree *expr,int &line);
  void lineDirective(std::ostream &o,Opencxx::Environment *env,Opencxx::Ptree *expr);
  Opencxx::Ptree *lineDirective(Opencxx::Environment *env,Opencxx::Ptree *expr);

}

#endif

