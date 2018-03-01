//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// General helper code/data structures for creating exception code.
//

#include <stdexcept>
#include <iostream>
#include <sstream>

#include "helpers/Macros.h"
#include "helpers/CodeGen.h"
#include "helpers/pfxstream.h"
#include "helpers/BasicTypes.h"

#include "fe/Data.h"

#include "AttrsHelper.h"
#include "ConfigDB.h"
#include "MiscTypes.h"
#include "GenExceptions.h"
#include "InstrWalker.h"
#include "GenRegs.h"

namespace adl {

  using namespace std;
  using namespace Opencxx;
  using namespace PtreeUtil;

  gc_string getExceptionFlag()
  {
    return "__exception_flag";
  }

  gc_string getExceptionTmpFlag()
  {
    return "__delayed_exception_flag";
  }

  gc_string getExceptionDelayFlag()
  {
    return "_exception_delayed";
  }

  gc_string getExceptionMutex()
  {
    return "_ex_mutex";
  }

  gc_string getExceptionLock()
  {
    return "boost::lock_guard<boost::mutex> __ex_lock__ (" + getExceptionMutex() + ")";
  }

  gc_string getPendingExceptionFlag()
  {
    return "__pending_exception_flag";
  }

  gc_string getExceptionIndex(const Exception &e)
  {
    return "__" + e.name() + "_Index";
  }

  gc_string getExceptionDS(const Exception &e)
  {
    return "_" + e.name() + "_data";
  }

  gc_string getExceptionSetupFunc()
  {
    return "setup_exception_data";
  }

  gc_string getExceptCallbackName()
  {
    return "_except_callbacks";
  }

  gc_string getDefaultExceptCallback()
  {
    return "default_except_callback_action";
  }

  gc_string getPreExceptionHandler()
  {
    return "pre_exception_handler";
  }

  gc_string getPostExceptionHandler()
  {
    return "post_exception_handler";
  }

  class GenExceptions : public gc {
  public:
    GenExceptions(opfxstream &o,
                  const Core &core,
                  InstrWalker &walker,
                  const gc_string &filename,
                  Watches &watches,
                  ClassData &cd,
                  const ConfigDB &config,
                  const CoreConfig &coreconfig) :
      _o(o), 
      _cd(cd),
      _config(config),
      _filename(filename),
      _watches(watches),
      _core(core), 
      _walker(walker),
      _coreconfig(coreconfig)
    {
    };

  private:

    opfxstream       &_o;

    ClassData        &_cd;
    const ConfigDB   &_config;

    gc_string            _filename;

    Watches          &_watches;

    const Core       &_core;
    InstrWalker      &_walker;
    const CoreConfig &_coreconfig;

  public:

    bool mt_support() const
    {
      return _config.mt_support();
    }

    bool haveExceptions() const
    {
      return !_core.exceptions().empty();
    }

    bool transactional_iss() const
    {
      return _config.iss_type() != tNormal;
    }

    bool jit_support() const
    {
      return _config.jit_support();
    }

    bool log_unarch() const
    {
      return _config.log_unarch();
    }

    bool has_except_callbacks() const
    {
      return _coreconfig.has_except_callbacks();
    }

    bool has_except_callback(const gc_string &r) const
    {
      return _coreconfig.except_callbacks().count(r);
    }

    const StrSet &except_callbacks() const
    {
      return _coreconfig.except_callbacks();
    }

    void writeCurLocation()
    {
      adl::writeCurLocation(_o,_config,_filename);
    }

    void writeLineDirective(Environment *env,Ptree *expr)
    {
      adl::writeLineDirective(_o,_config,env,expr);
    }

    bool multi_compile() const
    {
      return _config.num_compile_jobs() > 0;
    }

    void genFunc(const gc_string &fname,Environment *env,Ptree *func,
                 bool isConst = false)
    {
      adl::genFunc(_o,_config,_walker,_filename,fname,env,func,isConst);
    }

    gc_string getPreExceptionCall(const Exception &e)
    {
      if ( _core.pre_exception_handler() ) {
        return getPreExceptionHandler() + "(" + e.name() + ");\n";
      } else {
        return gc_string();
      }
    }
    
    gc_string getPostExceptionCall(const Exception &e)
    {
      if ( _core.post_exception_handler() ) {
        return getPostExceptionHandler() + "(" + e.name() + ");\n";
      } else {
        return gc_string();
      }
    }

    // Exceptions are sorted in reverse order so that higher priority exceptions
    // will have their side effects executed after lower priority exceptions,
    // thus preserving their effects over the others.
    struct SortException {
      bool operator()(const Exception *x,const Exception *y) 
      {
        return (x->priority() > y->priority());
      };
    };

    // Write any exception data structures, if relevant.
    void writeExceptionStructs()
    {
      const ExceptionHash &exceptions = _core.exceptions();

      ForEach(exceptions,iter) {

        const Exception &exception = *iter->second;
        const ExceptionFieldHash &efields = exception.fields();
        if (exception.has_fields()) {

          gc_string et = exception.get_exception_type();

          _o << "// Fields for exception " << exception.name() << "\n"
             << "struct " << et << " : public ExBase {\n";

          ForEach(efields,i) {
            const ExceptionField &ef = *(i->second);
            _o << genFieldType(ef) << " " << ef.name() << ";\n";
          }
          _o << et << "()\n";
          bool first = true;
          ForEach(efields,i) {
            const ExceptionField &ef = *(i->second);
            constr_init_sep(_o,first);
            _o << ef.name() << "(" << genFieldInit(ef) << ")";
          }
          _o << "{}\n"
             << "void reset() {\n";
          ForEach(efields,i) {
            const ExceptionField &ef = *(i->second);
            _o << ef.name() << " = " << genFieldInit(ef) << ";\n";
          }
          _o << "}\n"
             << "};\n\n";
          
          _cd._member_vars.push_back(et + " " + getExceptionDS(exception));
        }
      }
    }

    void writeSensitivityClause(const Exception &e)
    {
      if (e.sensitivity() == LevelSensitive) {
        _o << "else {\n"
           << getPendingExceptionFlag() << " |= " << e.name() << ";\n"
           << "}\n";
      }
    }

    // Write static information about exceptions.
    void writeExceptionInfo()
    {
      const ExceptionHash &exceptions = _core.exceptions();

      unsigned count = 1;
      _o << "static ExceptionInfos initExceptionInfo() {\n"
         << "ExceptionInfos info;\n";
      ForEach(exceptions,i) {
        const Exception &e = *i->second;
        _o << "info.push_back(ExceptionInfo());\n"
           << "info.back()._name = \"" << i->first << "\";\n"
           << "info.back()._id = " << i->first << ";\n"
           << "info.back()._flag = " << count << ";\n"
           << "info.back()._priority = " << e.priority() << ";\n"
           << "info.back()._sensitivity = " << ((e.sensitivity() == EdgeSensitive) ? "EdgeSensitive" : "LevelSensitive") << ";\n";
        const AttrList &attrs = e.attrs();
        ForEach(attrs,a) {
          AttrsHelper ah(&a->second);
          ah.gen_var(_o);
          _o << "info.back()._attrs.push_back(std::make_pair(" 
             << a->first << "," << ah.gen_constr() << "));\n";
        }
        _o << "info.back().finalize();\n";
        count *= 2;
      }
      _o << "sort_info(info);\n"
         << "return info;\n"
         << "}\n\n"
         << "static const ExceptionInfos &baseExceptionInfo() {\n"
         << "static ExceptionInfos info = initExceptionInfo();\n"
         << "return info;\n"
         << "}\n\n"
         << "const ExceptionInfos &getExceptionInfo() const {\n"
         << "return baseExceptionInfo();\n"
         << "}\n\n";

    }

    // This writes the public genExceptions interface, which normally just calls
    // the internal function, except for multi-threaded functions, where it
    // optionally delays exception invocation.
    void writeGenExceptionsPublic()
    {

      if (mt_support()) {       
        gc_string e_delayed = getExceptionDelayFlag();
        gc_string e_lock    = getExceptionLock();
        gc_string e_tflag   = getExceptionTmpFlag();
        gc_string ac        = getActiveCallback();

        _o << "void genExceptions(unsigned exception_flag) {\n"
           << "if (" << e_delayed << ") {\n"
           << e_lock << ";\n"
           << e_tflag << " |= exception_flag;\n"
           << "if (" << ac << ")" << ac << "->trigger_exception(" << e_tflag << ");\n"
           << "} else {\n"
           << "genExceptionsInternal(exception_flag);\n"
           << "}\n"
           << "}\n\n";

        _cd._member_vars.push_back("boost::mutex " + getExceptionMutex());

        _cd._member_vars.push_back("bool " + e_delayed);
        _cd._constr_inits.push_back(e_delayed + "(false)");
        _cd._reset_stmts.push_back(e_delayed + " = false");

        _cd._member_vars.push_back("unsigned " + e_tflag);
        _cd._constr_inits.push_back(e_tflag + "(0)");
        _cd._reset_stmts.push_back(e_tflag + " = 0");

        _o << "void setDelayedExternalExceptions(bool x) { " << e_delayed << " = x; }\n\n"

           << "void applyExceptions() {\n"
           << e_lock << ";\n"
           << "if (" << e_tflag << ") {\n"
           << "genExceptionsInternal(" << e_tflag << ");\n"
           << e_tflag << " = 0;\n"
           << "}\n"
           << "}\n\n";
      } else {
        _o << "void genExceptions(unsigned exception_flag) {\n";
        if (transactional_iss()) {
          // For transactional ISSs, make sure we're in the correct state with
          // our CIA and NIA.
          _o << _core.cia()->name() << "_write(" << _core.nia()->name() << "_read());\n";
        }
        _o << "genExceptionsInternal(exception_flag);\n"
           << "}\n\n";
      }
    }
    
    void writeGenExceptions(const vector<const Exception*> &in_priority,PMap &eenables,PMap &enames,bool use_ds)
    {
      static int unarch_attr  = Data::get_attr(Unarchitected);

      _o << "void genExceptionsInternal(unsigned exception_flag" << ((!use_ds) ? ",ExHandle eh" : "") << ") {\n"
         << "try {\n";
      ForEach(in_priority,i) {
        const Exception &e = *(*i);

        bool log = (log_unarch() || !e.has_attr(unarch_attr));

        _o << "if (exception_flag & " << e.name() << ") {\n";
        if (e.enable()) {
          _o << "if (" << eenables[e.name()] << "()) {\n";
        }
        if (log) {
          _o << "if (Tracing && adl_unlikely(_trace_mode & ttException)) {\n"
             << "_logger->log_exception(" << e.name() << ",true,\"" << e.name() << "\");\n"
             << "}\n";
        }
        _o << getPreExceptionCall(e)
           << enames[e.name()];
        if (e.has_fields()) {
          // Has fields, so call the function, passing the exception's data
          // structure as an argument.
          if (use_ds) {
            _o << "(" << getExceptionDS(e) << ");\n";
          } else {
            // Has fields, so call the function, passing the exception's data
            // structure as an argument.
            _o << "(static_cast<" << e.get_exception_type() << "&>(*(eh()._items[" << getExceptionIndex(e) << "])));\n";
          }
        } else {
          _o << "();\n";
        }
        if (log) {
          _o << "if (Tracing && adl_unlikely(_trace_mode & ttException)) {\n"
             << "_logger->log_exception(" << e.name() << ",false,\"" << e.name() << "\");\n"
             << "}\n";
        }
        // We put the post-log before the post-exception so that we'll get the
        // logging message even if another exception is generated by the
        // post-exception hook.
        _o << getPostExceptionCall(e);
        if (e.enable()) {
          _o << "}\n";
          writeSensitivityClause(e);
        }
        _o << "}\n";
      }
      _o << "}\n"
         << "catch (Exception &e) {\n"
         << "genExceptionsInternal(e);\n"
         << "}\n"
         << "}\n\n";
    }

    void writeExceptionHandlers()
    {
      const ExceptionHash &exceptions = _core.exceptions();

      // Sort the exceptions by priority.
      vector<const Exception*> in_priority;
      ForEach(exceptions,i) {
        in_priority.push_back(i->second);
      }
      sort(in_priority.begin(),in_priority.end(),SortException());

      PMap enames;
      PMap eenables;

      // Write out the exception handler functions.
      ForEach(exceptions,iter) {
        const Exception &e = *iter->second;
        auto sym = AdlGenSym();
        enames[e.name()] = sym;

        if (e.enable()) {
          auto sym = AdlGenSym();
          eenables[e.name()] = sym;
          _o << "// Enable predicate for " << e.name() << " exception.\n"
             << "bool " << sym << "()\n";
          writeLineDirective(e.env(),e.enable());
          _o << _walker.translateCode(e.env(),get_func_body(e.enable())) << "\n";
          writeCurLocation();
          _o << "\n";
          if (e.sensitivity() == LevelSensitive) {
            Ptree *wsym = AdlGenSym();
            // If we have a level sensitive exception, then we need a watch
            // function to set the exception should it be pending.
            _o << "// Watch function for " << e.name() <<" exception.\n"
               << "void " << wsym << "() {\n"
               << "if (" << getPendingExceptionFlag() << " & " << e.name() << ") {\n";
            // Clear the pending flag if so configured, otherwise leave it.
            if (_config.clear_pending_excepts()) {
              _o << getPendingExceptionFlag() << " &= ~" << e.name() << ";\n";
            }
            _o << "setException(" << e.name() << ");\n"
               << "}\n"
               << "}\n\n";
            _watches.push_back(Watch(_walker.regs_referenced(),wsym));
          }
        }

        _o << "// Exception handler for " << e.name() << " exception.\n";
        if ( e.has_fields() ) {
          // This has fields, so we write out a function which takes a reference to the 
          // exception's data structure.
          StrList etype;
          etype.push_back(e.get_exception_type());

          _o << "void " << sym << "(" << constify_args(e.action(),etype) << ")\n"
             << "{\n";
          writeLineDirective(e.env(),e.action());
          // We want to use the "simple" form of update if exception_is_branch
          // is false, since we don't want to log this as a branch.
          _walker.set_nia_simple_mode(!_core.exception_is_branch());
          _o << _walker.translateCode(e.env(),get_func_body(e.action()),false) << "\n";
          _walker.set_nia_simple_mode(false);
          writeCurLocation();
        } else {
          // No fields, so function takes no arguments.
          _o << "void " << sym << "()\n"
             << "{\n";
          // Doesn't have fields, so just put the argumentless function inline.
          writeLineDirective(e.env(),e.action());
          // Transform any ADL RTL extensions into C++.
          _walker.set_nia_simple_mode(!_core.exception_is_branch());
          _o << _walker.translateCode(e.env(),get_func_body(e.action())) << "\n";
          _walker.set_nia_simple_mode(false);          
          writeCurLocation();
        }

        // If we have a callback for this exception, emit code for it here.
        if (has_except_callback(e.name())) {
          int index = 0;
          // Yes, this is an associative data structure, but our IDs are indices
          // into an array, so we need to calculate that by counting
          // sequentially through the list.
          ForEach(except_callbacks(),i) {
            if (*i == e.name()) {
              _o << "(*" << getExceptCallbackName() << "[" << index << "])();\n";
              break;
            }
            index++;
          }
        }

        _o << "}\n\n";
      }

      // Create pre and post hooks, if they're defined.
      if ( _core.pre_exception_handler() ) {
        genFunc("void " + getPreExceptionHandler(),_core.env(),_core.pre_exception_handler()._func);
      }
      if ( _core.post_exception_handler() ) {
        genFunc("void " + getPostExceptionHandler(),_core.env(),_core.post_exception_handler()._func);
      }

      _o << "ExHandle getExceptionData(unsigned exception_flag) {\n"
         << "ExHandle eh;\n"
         << "if (exception_flag) {\n"
         << "eh.alloc(" << exceptions.size() << ");\n"
         << "}\n";
      ForEach(in_priority,i) {
        const Exception &e = *(*i);
        if (e.has_fields()) {
          _o << "if (exception_flag & " << e.name() << ") {\n"
             << "eh()._items[" << getExceptionIndex(e) << "] = new " << e.get_exception_type() << "(" << getExceptionDS(e) << ");\n"
             << "}\n";
        }
      }
      _o << "return eh;\n"
         << "}\n\n";

      writeGenExceptions(in_priority,eenables,enames,true);

      writeGenExceptionsPublic();

      _o << "void setExternException(unsigned exception_flag) {\n";
      ForEach(in_priority,i) {
        const Exception &e = *(*i);
        _o << "if (exception_flag & " << e.name() << ") {\n";
        if (e.enable()) {
          _o << "if (" << eenables[e.name()] << "()) {\n";
        }
        _o << getExceptionFlag() << " |= exception_flag;\n";
        if (e.enable()) {
          _o << "}\n";
          writeSensitivityClause(e);
        }
        _o << "}\n";
      }
      _o << "}\n\n";

      if (transactional_iss()) {
        writeGenExceptions(in_priority,eenables,enames,false);
      }
    }

    void writeExceptionReset(Ptree *e,bool have_fields)
    {
      const ExceptionHash &exceptions = _core.exceptions();
      if (have_fields) {
        // If we have any fields, then we have to reset the appropriate data structure, since
        // for this method of invoking an exception, we give default values.
        _o << "switch (" << e << ") {\n";
        ForEach(exceptions,i) {
          const Exception &e = *i->second;
          if (e.has_fields()) {
            _o << "case " << e.name() << ":\n"
               << getExceptionDS(e) << ".reset();\n";
          }
        }
        _o << "default:\n"
           << "break;\n"
           << "}\n";
      }
    }

    void writeTransExceptionSetup()
    {
      const ExceptionHash &exceptions = _core.exceptions();

      _o << "ExHandle " << getExceptionSetupFunc() << "(Exception e,ExHandle eh)\n"
         << "{\n"
         << "eh.alloc(" << exceptions.size() << ");\n"
         << "switch (e) {\n";
      ForEach(exceptions,i) {
        const Exception &e = *i->second;
        if (e.has_fields()) {
          gc_string index = getExceptionIndex(e);
          _o << "case " << e.name() << ":\n"
             << "if (eh()._items[" << index << "]) {\n"
             << "*eh()._items[" << index << "] = " << getExceptionDS(e) << ";\n"
             << "} else {\n"
             << "eh()._items[" << index << "] = new " << e.get_exception_type() << "(" << getExceptionDS(e) << ");\n"
             << "}\n"
             << "break;\n";
        }
      }
      _o << "default:\n"
         << "break;\n"
         << "}\n"
         << "return eh;\n"
         << "}\n\n";
    }

    // Generate exception callback support, if necessary.
    void genExceptCallbacks()
    {
      // If we have callbacks, then generate the appropriate setup function.
      if (has_except_callbacks()) {
        _cd._member_vars.push_back("std::vector<ExceptCallback*> " + getExceptCallbackName());
        gc_string init;
        MkGStr(init,getExceptCallbackName()+"(" << except_callbacks().size() << ",&" << getDefaultExceptCallback() << ")");
        _cd._constr_inits.push_back(init);
        _o << "virtual bool setExceptCallback(const std::string &name,ExceptCallback *cb) {\n"
           << "int index = -1;\n"
           << "const ExceptionInfos &edata = getExceptionInfo();\n"
           << "for (int i = 0; i != " << _core.exceptions().size() << "; ++i) {\n"
           << "if (edata[i]._name == name) {\n"
           << "index = edata[i]._id;\n"
           << "break;\n"
           << "}\n"
           << "}\n"
           << "if (index < 0) {\n"
           << "return false;\n"
           << "}\n"
           << "return setExceptCallback(index,cb);\n"
           << "}\n\n"
           << "virtual bool setExceptCallback(unsigned index,ExceptCallback *cb) {\n"
           << "switch (index) {\n";
        int count = 0;
        ForEach(except_callbacks(),i) {
          const Exception *e = _core.get_exception(*i);
          // Should have already been checked during config parsing.
          assert(e);
          _o << "case " << e->name() << ":\n"
             << getExceptCallbackName() << "[" << count++ << "] = (cb) ? cb : &" << getDefaultExceptCallback() << ";\n"
             << "return true;\n";
        }
        _o << "default:\n"
           << "return false;\n"
           << "}\n"
           << "}\n\n";
      }
    }

    // Generate needed exception declarations.
    void generateExceptions()
    {
      const ExceptionHash &exceptions = _core.exceptions();

      // Add level sensitive exception support if we're told to, or we have any
      // exceptions which are explicitly marked as level sensitive.
      bool have_level_sensitive = _config.add_level_sensitive_support();

      bool have_fields = false;

      // Stores a mask used to clear any edge sensitive exceptions which
      // are set externally as level-sensitive.  This occurs w/uADL, since
      // external exceptions always get set as level sensitive, then only
      // automatically cleared if they're not really level sensitive.
      unsigned es_mask = 0;

      // We always create this type so that other functions can use it, even if
      // we don't define any exceptions.
      _o << "\nenum Exception {\n"
         << "None,\n";
      unsigned count = 1;
      ForEach(exceptions,i) {
        _o << i->first << " = 0x" << hex << count << dec << ",\n";
        if (i->second->has_fields()) {
          have_fields = true;
        }
        if (i->second->sensitivity() == LevelSensitive) {
          have_level_sensitive = true;
        } else {
          es_mask |= count;
        }
        count *= 2;
      }
      _o << "};\n\n"

        // Same with this type: This maps exceptions into individual indices
        // suitable for array lookup, rather than use as bit flags.
         << "enum ExceptionIndex {\n"
         << "__None_Index,\n";
      ForEach(exceptions,i) {
        _o << getExceptionIndex(*i->second) << ",\n";
      }
      _o << "};\n\n";

      writeExceptionStructs();

      if (haveExceptions()) {

        // This is the flag which will store exception state information.
        gc_string eflag = getExceptionFlag();
        _cd._member_vars.push_back("unsigned "+eflag);
        _cd._constr_inits.push_back(eflag+"(0)");
        _cd._reset_stmts.push_back(eflag+" = 0");
        
        if (have_level_sensitive) {
          gc_string peflag = getPendingExceptionFlag();
          _cd._member_vars.push_back("unsigned "+peflag);
          _cd._constr_inits.push_back(peflag+"(0)");
          _cd._reset_stmts.push_back(peflag+" = 0");
        }

        if (exceptions.size() > 32) {
          RError("A maximum of 32 interrupts are currently supported.");
        }
        
        writeExceptionInfo();

        genExceptCallbacks();

        // Now create the exception-activation functions.
        Ptree *etmp = AdlGenSym();
        _o << "void raiseException(Exception " << etmp << ")"
           << nonJitOnly()
           << "{\n";
        writeExceptionReset(etmp,have_fields);
        _o << "throw " << etmp << ";\n" 
           << "}\n"
           << endNonJitOnly()
           << "\n\n"

          // This is the external interface for doing the same thing.
           << "void genExceptionImm(unsigned id)"
           << nonJitOnly()
           << "{\n"
           << "Exception " << etmp << " = (Exception)id;\n";
        writeExceptionReset(etmp,have_fields);
        _o << "throw " << etmp << ";\n"
           << "}\n"
           << endNonJitOnly()
           << "\n\n";

        // For those exceptions with fields, we write a version which allows the
        // user to update the data structures.
        ForEach(exceptions,i) {
          const Exception &e = *i->second;
          gc_string data = getExceptionDS(e);
          if (e.has_fields()) {
            _o << "void raiseException(const " << e.get_exception_type() << "& x)"
               << nonJitOnly()
               << "{\n"
               << data << " = x;\n"
               << "throw " << e.name() << ";\n"
               << "}\n"
               << endNonJitOnly()
               << "\n\n"
               << "void setException(const " << e.get_exception_type() << "& x) {\n"
               << data << " = x;\n"
               << getExceptionFlag() << " |= " << e.name() << ";\n"
               << "}\n\n";
          }
        }

        // If we have a transactional ISS, then this version of the function is
        // used to store exception status into the instruction packet, while the
        // default version is used to capture exception information during the
        // fetch process.
        if (transactional_iss()) {
          writeTransExceptionSetup();
          _o << "void setException(" << PktType << " *" << PktName << ",Exception e)\n"
             << "{\n"
             << PktName << "->_exdata |= (unsigned)e;\n"
             << PktName << "->_exitems = " << getExceptionSetupFunc() << "(e," << PktName << "->_exitems);\n"
             << "}\n\n";
        }

        _o << "void setException(Exception e)\n"
           << "{\n"
           << getExceptionFlag() << " |= (unsigned)e;\n"
           << "}\n\n"
           << "unsigned getExceptionFlag()\n"
           << "{\n"
           << "return " << getExceptionFlag() << ";\n"
           <<"}\n\n"
           << "unsigned getExceptionFlag() const\n"
           << "{\n"
           << "return " << getExceptionFlag() << ";\n"
           <<"}\n\n"
           << "void clearExceptionFlag()\n"
           << "{\n"
           << getExceptionFlag() << " = 0;\n"
           <<"}\n\n"
           << "bool cancelExceptions(unsigned flag)\n"
           << "{\n"
           << "bool r = " << getExceptionFlag() << " & flag;\n"
           << getExceptionFlag() << " &= ~flag;\n";
        if (have_level_sensitive) {
          _o << "r = r || " << getPendingExceptionFlag() << " & flag;\n"
             << getPendingExceptionFlag() << " &= ~flag;\n";
        }
        _o << "return r;\n"
           << "}\n\n"
           << "void setPendingExceptions(unsigned flag)\n"
           << "{\n";
        if (have_level_sensitive) {
          _o << getPendingExceptionFlag() << " |= flag;\n";
        }
        _o << "}\n\n"
           << "bool hasCurrentPendingExceptions()\n"
           << "{\n";
        if (have_level_sensitive) {
          _o << "return (" << getExceptionFlag() << " & " << getPendingExceptionFlag() << ");\n";
        } else {
          _o << "return " << getExceptionFlag() << ";\n";
        }
        _o << "}\n\n"
           << "bool hasPendingExceptions()\n"
           << "{\n";
        if (have_level_sensitive) {
          _o << "return " << getPendingExceptionFlag() << ";\n";
        } else {
          _o << "return " << getExceptionFlag() << ";\n";
        }
        _o << "}\n\n"
           << "bool hasNonPendingExceptions()\n"
           << "{\n";
        if (have_level_sensitive) {
          _o << "return " << getExceptionFlag() << " & ~" << getPendingExceptionFlag() << ";\n";
        } else {
          _o << "return " << getExceptionFlag() << ";\n";
        }
        _o << "}\n\n"
           << "void updateLevelSensitiveExceptions()\n"
           << "{\n";
        if (have_level_sensitive) {
          _o << "if (" << getPendingExceptionFlag() << ") {\n"
             << "setExternException(" << getPendingExceptionFlag() << ");\n"
             << "}\n";
        }
        _o << "}\n\n"

           << "void clearEdgeSensitivePendingExceptions()\n"
           << "{\n"
           << "cancelExceptions(0x" << hex << es_mask << dec << ");\n"
           << "}\n\n";

        writeExceptionHandlers();

      } else {

        // If we don't have exceptions, then write dummy functions in order to preserve the interface.
        _o << "void setException(Exception e)\n"
           << "{\n"
           << "}\n\n"
           << "unsigned getExceptionFlag()\n"
           << "{\n"
           << "return 0;\n"
           <<"}\n\n"
           << "unsigned getExceptionFlag() const\n"
           << "{\n"
           << "return 0;\n"
           <<"}\n\n"
           << "void clearExceptionFlag()\n"
           << "{\n"
           <<"}\n\n";
      }

      // In order to have a consistent interface, the transactional-iss always
      // has this function for committing exceptions, even if no exceptions
      // exist.  This is used when there are exceptions generated by a call to
      // prefetch, and thus no instruction packet exists.
      // This activity will always go to the pending queue.
      if (transactional_iss()) {
        _o << "void commit_exceptions() {\n";
        if (haveExceptions()) {
          _o << "if (Tracing) {\n"
             << "_logger->log_current_instr(0);\n"
             << "}\n"
             << "genExceptionsInternal(" << getExceptionFlag() << ");\n"
             << "if (Tracing) {\n"
             << "_logger->log_commit(0);\n"
             << "}\n";
        }
        _o << "}\n\n"
          // This should be called when exceptions need to be generated, but the
          // output isn't associated with an instruction packet, e.g. something
          // caused by a failed fetch.  All logging data is sent to the pending
          // queue.
           << "void gen_non_instr_exceptions(unsigned exception_flag,ExHandle eh) {\n";
        if (haveExceptions()) {
          _o << "if (Tracing) {\n"
             << "_logger->log_current_instr(0);\n"
             << "}\n"
             << "genExceptionsInternal(exception_flag,eh);\n";
        }
        _o << "}\n\n";
      }

    }

  };

  GenExceptionsIF::GenExceptionsIF(opfxstream &o,const Core &core,InstrWalker &walker,
                                   const gc_string &filename,Watches &watches,
                                   ClassData &cd,const ConfigDB &config,
                                   const CoreConfig &coreconfig) :
    _impl(new GenExceptions(o,core,walker,filename,watches,cd,config,coreconfig))
  {
  }

  ImplCall(GenExceptionsIF,generateExceptions);

}
