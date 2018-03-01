//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Converts a UsedReg begin or end statement to C++ model accesses.
//

#include "opencxx/parser/Ptree.h"
#include "opencxx/parser/PtreeUtil.h"
#include "opencxx/Walker.h"

#include "helpers/Macros.h"

#include "fe/Helpers.h"
#include "fe/FieldSupport.h"

#include "FieldSubstWalker.h"

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;

namespace adl {

  // This walker class substitutes instruction fields for their relevant accessors.
  class FieldSubstWalker : public Walker {
  public:
    FieldSubstWalker(ostream &o,const Core &core,const Instr *instr,const Instr::Alias *alias,
                     const gc_string &arg,const gc_string &pfx) : 
      Walker((Environment*)0), 
      _o(o),
      _core(core),
      _instr(instr),
      _alias(alias),
      _add_sizing(false),
      _arg(arg),
      _pfx(pfx)
    {};

    Ptree *handleVariable(Ptree *exp)
    {
      gc_string name = exp->ToString();
      int index;
      if (_alias) {
        ForEach((_alias->_fields),i) {
          if (name  == i->_field->name()) {
            gc_ostringstream ss;
            gc_ostringstream getf;
            // Tricky point: Alias evaluation may be recursive if we're
            // dealing with expressions.  However, the recursive evaluation of
            // expressions should not be regarded as handling an alias, since
            // the expression will be dealing with instruction fields of the
            // alias instruction itself.
            auto oalias = _alias;
            _alias = 0;
            const InstrField *f = aliasFieldTarget(getf,*this,*_instr,*i,true);
            _alias = oalias;
            if (f) {
              const InstrField *encap = f->encap_field();
              if (f->is_pseudo() && encap) {
                unsigned instr_width = (_core.is_little_endian() ? _instr->width() : 0);
                ss << _pfx << "get_" << encap->name() << "__" << f->name() << "(bits<" 
                   << encap->size() << ">(" << getf.str();
                if (instr_width) {
                  ss << "width_" << instr_width;
                }
                ss << "(" << _arg << ")))";
                return Ptree::Make(ss.str().c_str());
              } else {
                unsigned instr_width = (_core.is_little_endian() ? _instr->width() : 0);
                writeFieldAccessor(ss,*f,_arg,instr_width,_add_sizing,_pfx);
                return Ptree::Make(ss.str().c_str());
              }
            } else  {
              return Ptree::Make(getf.str().c_str());
            }
          }
        }
      }
      else if ( _instr && ((index = _instr->getAll(name)) >= 0)) {
        gc_ostringstream ss;
        unsigned instr_width = (_core.is_little_endian() ? _instr->width() : 0);
        writeFieldAccessor(ss,*(_instr->getAll(index)._field),_arg,instr_width,true,_pfx);
        return Ptree::Make(ss.str().c_str());
      }
      return exp;
    }


    Ptree *TranslateDotMember(Ptree *exp) 
    {
      Ptree *left = exp->Car();
      Ptree *member = Third(exp);

      gc_string name = left->ToString();
      int index;
      if (_instr && ((index = _instr->getAll(name)) >= 0)) {
        gc_ostringstream ss;
        const InstrField &f = *(_instr->getAll(index)._field);
        const InstrField *n = f.getField(member->ToString());
        if(!n) {
          _add_sizing = true;
          return Walker::TranslateDotMember(exp);
        }
        unsigned instr_width = (_core.is_little_endian() ? _instr->width() : 0);
        ss << "bits<" << n->size() << ">(" << _pfx << "get_" << name << "__" << member->ToString() << "("; 
        writeFieldAccessor(ss,f,_arg,instr_width,true,_pfx);
        ss << "))";
        return Ptree::Make(ss.str().c_str());
      }
      return Walker::TranslateDotMember(exp);
    }

    Ptree *TranslateFuncall(Ptree *exp) 
    {
      Ptree *fun  = First(exp);
      gc_string name = fun->ToString();
      if (_alias) {
        ForEach((_alias->_fields),i) {
          if (name  == i->_field->name()) {
            gc_ostringstream getf;
            auto oalias = _alias;
            _alias = 0;
            const InstrField *f = aliasFieldTarget(getf,*this,*_instr,*i,true);
            _alias = oalias;
            if (f) {
              if (!f->is_pseudo ()|| !f->encap_field()) {
                _add_sizing = true;
              }
            }
          }
        }
      }

      Ptree *nexp = Walker::TranslateFuncall(exp);

      // This might be a table-based field, so check it and put in a call to the
      // table.  We want to store the table value so that the relevant index is
      // correct.
      if (const InstrField *f = _core.get_instrfield(name)) {
        if (f->is_constrained()) {
          nexp = Ptree::Make("%s(%p.uint32(),%p)",makeInstrFldTblAccessName(*f).c_str(),nexp->Car(),Third(nexp));
        }
      }

      _add_sizing = false;
      return nexp;
	  
    }

    Ptree *TranslateVariable(Ptree *exp)
    {
      return handleVariable(exp);
    }

    Ptree *TranslatePtree(Ptree *exp)
    {
      bool      b;
      unsigned  i;
      char     *s;
      if (get_bool(b,exp,env) || get_uint(i,exp,env) || exp->Reify(s)) {
        return exp;
      }
      return handleVariable(exp);
    }

  private:
    ostream            &_o;
    const Core         &_core;
    const Instr        *_instr;   // Instruction to search for fields.
    const Instr::Alias *_alias;   // An instruction alias, if one exists.
    bool  		 _add_sizing;// add field sizing, used to comm. between
    // TranslateFuncall and handleVariable
    const gc_string    &_arg;     // Argument used by field accessors.
    const gc_string    &_pfx;     // Prefix to add before the generated accessor functions. 
  };   

  // Given an alias field, this either writes out the accessor code or returns
  // a target field to be handled as needed.  For example, if an alias maps a constant
  // value to a field, that constant will be written.  If a different field is mapped to
  // this field, then we'll return a pointer to the original field.
  const InstrField *aliasFieldTarget(ostream &o,Walker &walker,const Instr &instr,const Instr::Field &fp, bool trans)
  {
    switch (fp._type) {
    case Instr::Field::Resrvd: // reserved fields are not used in in encoding
    case Instr::Field::Alias:  // alias fields are not
      break;
    case Instr::Field::Expr:
      // Expression- use as is.
      o << "(" << walker.Translate(fp._expr) << ")";
      break;
    case Instr::Field::RValue: // rvalue fields are actually opcodes
    case Instr::Field::Value:
      // Value- use as is.
      o << fp._value;
      break;
    case Instr::Field::Ref:
      // Refers to another field- use that value
      int  ix = instr.getField(fp._sname);
      Instr::Field tfp = instr.getField(ix);
      const InstrField *tfield = (tfp._field)->interface() ? (tfp._field)->interface() : tfp._field;
      const InstrField *encap_field = tfield->encap_field();
      if (encap_field) {
        if (trans) {
          gc_string fname = tfp._name;
          o << "get_" << fname.substr(0,fname.rfind('.')-1) << "__";
          return tfield;
        } else {
          o << "get_" << encap_field->name() << "__" << tfield->name() << "(" << encap_field->name() << ")";
        }
      } else {
        if (trans) {
          return  tfp._field;
        } else {
          return tfield;
        }
      }
    }
    return 0;
  }

  void writeFieldAccessor(ostream &o,const InstrField &field,const gc_string &arg,
                          unsigned instr_width,bool add_sizing,const gc_string &pfx)
  {
    if (add_sizing) {
      o << "bits<" << dec << field.size() << ">(";
    }
    if (instr_width) { // little endian     
      o << pfx << "get_" << field.name() << "_width_" << dec << instr_width << "(" << arg << ")";
    } else {
      o << pfx << "get_" << field.name() << "(" << arg << ")";
    }	
    if (add_sizing) { 
      o << ")";
    }
  }

  void writeFieldAccessor(ostream &o,const Core &core,Ptree *expr,const Instr *instr,const Instr::Alias *alias,
                          const gc_string &arg,const gc_string &pfx)
  {
    FieldSubstWalker w(o,core,instr,alias,arg,pfx);
    assert(expr);
    o << "to_uint32(" << w.Translate(expr) << ")";
  }


}
