//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Simple driver program to test that we can load an ADL ISS as a shared object
// and use it.  Note that this expects the ISS to exist in the namespace
// 'myiss', rather than the default 'adliss' namespace.
//

#include <string.h>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "rnumber/RNumber.h"

#include "helpers/Macros.h"

#include "iss/SimInterface.h"
#include "iss/ModelInterface.h"
#include "iss/DbgInterface.h"

using namespace std;
using namespace adl;

namespace myiss {
  void resetGlobal();
  IssNode *createTopLevelNode(unsigned &id);
}

unsigned get_exception_id(IssNode *node,const string &name)
{
  const ExceptionInfos &einfo = node->getExceptionInfo();
  ForEach(einfo,i) {
    if (i->_name == name) {
      return i->_id;
    }
  }
  RError("Unknown exception name " << name);
}

struct MyRegCallback : public RegCallback, public RegReadCallback {
  MyRegCallback(const string &n) : _name(n) {};

  virtual void operator()(unsigned index,REGTYPE value) {
    cout << _name << ":  New value is 0x" << hex << value << dec << "\n";
    _value = value;
  };

  virtual REGTYPE_t operator()(unsigned index) {
    cout << _name << ":  Reading value of 0x" << hex << _value << dec << "\n";    
    return _value;
  }

  REGTYPE_t _value;
  string    _name;
};

struct MyExceptCallback : public ExceptCallback {
  MyExceptCallback(const string &n) : _name(n) {};

  virtual void operator()() {
    cout << _name << " exception triggered.\n";
  };

  string _name;
};

void install_reg_callback(IssNode *root,MyRegCallback *b)
{
  if (!root->setRegCallback(b->_name,b)) {
    cout << "Unable to install callback on register " << b->_name << " (ignored).\n";
  }

  if (!root->setRegReadCallback(b->_name,b)) {
    cout << "Unable to install read callback on register " << b->_name << " (ignored).\n";
  }
}

void install_except_callback(IssNode *root,MyExceptCallback *b)
{
  if (!root->setExceptCallback(b->_name,b)) {
    cout << "Unable to install callback on exception " << b->_name << " (ignored).\n";
  }
}

int main(int argc,const char *argv[])
{
  if (argc < 3) {
    cout << "usage:  " << argv[0] << ": [-script <file-name> [<pre-exec-func>] ] | [ [-jit] [-dc] [-td <incr> <threshold>] [-initial-exception <excpt>] <input file> <output file , [output file ...] >]" << endl;
    return 1;
  }

  string script, pre_exec_func, initial_exception;
  if (strstr(argv[1],"-script")) {
    script = argv[2];
    if (argc > 3) {
      pre_exec_func = argv[3];
    }
  }

  myiss::resetGlobal();
  unsigned id = 0;
  IssNode *root = myiss::createTopLevelNode(id);
  assert(root);

  installLoggers(root);

  bool Trace = true;

  TranslationParms::set_jit_bb_hot_threshold(10);
  TranslationParms::set_dc_bb_hot_threshold(10);

  if (!script.empty()) {
    DbgInterface *dbg = startCliMode(*root,script);
    if (!pre_exec_func.empty()) {
      // Test that we can evaluate a command directly with the CLI.
      evalCliCommand(pre_exec_func);
    }
     dbg->exec();
  } else {
    SimType rt = SimNormal;
    int astart;
    bool td = false;
    for (astart = 1; astart != argc; ++astart) {
      if ( strstr(argv[astart],"-initial-exception")) {
        if (astart+1 >= argc) {
          cout << "An exception name must be specified for the initial-exception option.\n";
          return 1;
        }
        initial_exception = argv[++astart];
        ++astart;
      }
      if ( strstr(argv[astart],"-trace")) {
        Trace = true;
      }
      else if ( strstr(argv[astart],"-notrace")) {
        Trace = false;
      }
      else if ( strstr(argv[astart],"-jit")) {
        rt = SimJit;
      }
      else if ( strstr(argv[astart],"-dc")) {
        rt = SimDc;
      }
      else if ( strstr(argv[astart],"-td")) {
        if (astart+2 >= argc) {
          cout << "An increment and threshold value must be supplied with option -td.\n";
          return 1;
        }
        td = true;
        // Make sure that we have it and that we report having it.
        assert(root->has_td());
        root->set_td_increment(atoi(argv[++astart]));
        root->set_td_threshold(strtoull(argv[++astart],0,0));
      } else {
        break;
      }
    }
    readInput(root,argv[astart++]);

    // Note:  We're specifically creating it after the reader to test that this will work.
    for (int i = astart; i != argc; ++i) {
      addWriter(createWriter(*root,argv[i]));
    }

    // We're creating some callbacks here to test this feature.
    MyRegCallback hid0("HID0"), hid1("HID1"), foo("FOO");
    install_reg_callback(root,&hid0);
    install_reg_callback(root,&hid1);
    install_reg_callback(root,&foo);

# ifdef ExceptionCallback
    MyExceptCallback x(ExceptionCallback);
    install_except_callback(root,&x);
# endif

    if (Trace) {
      startLogging(*root);
    } else {
      dumpInitialState();
    }

    if (!initial_exception.empty()) {
      root->genExceptions(get_exception_id(root,initial_exception));      
    }

    if (td) {
      int td_count = 0;
      // Report on returns caused due to temporal-decoupling time quantums.
      while (root->run(rt) == ACTIVE) {
        cout << "Temporal decoupling break:  " << ++td_count << "\n";
        root->set_td_counter(0);
      }
    } else {
      root->run(rt);
    }
    handleFinalState(true,*root);
    removeAllWriters();
    cout << "Instructions executed:  " << root->instr_count() << "\n\n";
  }

}
