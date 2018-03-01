//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// General helper code/data structures for creating exception code.
//

#ifndef _GENEXCEPTIONS_H_
#define _GENEXCEPTIONS_H_

namespace adl {

  class opfxstream;
  class Core;
  class InstrWalker;
  class ConfigDB;
  class CoreConfig;
  class GenExceptions;
  class ClassData;
  class Watches;

  gc_string getExceptionFlag();
  gc_string getExceptionTmpFlag();
  gc_string getExceptionDelayFlag();
  gc_string getExceptionLock();
  gc_string getExceptionFlag();
  gc_string getPendingExceptionFlag();
  gc_string getExceptionSetupFunc();

  struct GenExceptionsIF {

    GenExceptionsIF(opfxstream &o,const Core &core,InstrWalker &walker,
                    const gc_string &filename,Watches &watches,
                    ClassData &cd,const ConfigDB &config,
                    const CoreConfig &coreconfig);

    void generateExceptions();

  protected:
  
    GenExceptions *_impl;
  };


}

#endif
