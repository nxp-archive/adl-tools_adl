//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Implementation of the Writer class, the generic class used for writing intermediate
// and final test results in a stand-alone ISS.
//


#include "iss/ModelInterface.h"

#include "helpers/Macros.h"

#include "Writer.h"

using namespace std;

namespace adl {

WriterLogger::WriterLogger() :
  _instr_ea(0),
  _instr_ra(0)
{
}

void WriterLogger::logInstrRead(unsigned id,const char *name,addr_t ea,addr_t ra,uint32_t value)
{
  _instr_ea = ea;
  _instr_ra = ra;
}

Writer::Writer (const std::string &n,IssNode &root,const MemoryLog &initMemLog) : 
  _root(root),
  _initMemLog(initMemLog),
  _filename (n)
{}

void Writer::log_initialized_translation (IssCore *c,const char *lookup,unsigned set,unsigned way,const FieldData &ld){
  // We want to keep track of which translations are initialized (via the input file or an mmu management instruction) because
  // the model has no concept of initialized but we need to track this to avoid printing hundreds of unitiialized translations
  // in our output.
  
}


}
