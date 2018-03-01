//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This demonstrates a very simple, example plugin.  All it does it create a
// hash of what instructions were generated, then prints out this hash to a
// file.  The writer file extension type is 'icnt'.
//

#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "helpers/Macros.h"
#include "helpers/BasicTypes.h"
#include "helpers/AnyOption.h"
#include "helpers/stringhash.h"

#include "iss/Writer.h"
#include "iss/Plugin.h"

using namespace std;
using namespace adl;

typedef map<string,unsigned> InstrHash;

// This maintains a histogram of instructions encountered and prints out the
// results at the end of a test to the specified file.
struct InstrCount : public Writer, public LoggingIface {

  // This just opens the file and makes sure that the operation succeeded.
  InstrCount(const std::string &filename,IssNode &root,const MemoryLog &memlog) :
    Writer(filename,root,memlog),
    _fwidth(0),
    _out(filename.c_str())
  {
    if (!_out) {
      RError("Unable to open file " << filename);
    }
  }

  virtual LoggingIface *register_core(IssCore &core)
  {
    return this;
  }  

  // Required by the Writer class: Returns a string identifying the writer's
  // type.
  virtual const char *format_str() const { return "ICNT"; };

  // Add to the histogram and track instruction width so that we can nicely
  // format the output.
  virtual void logInstr(const uint32_t *opc,int num_bytes,const char* name, Disassembler,uint32_t)
  {
    _counts[name]++;
    _fwidth = max(_fwidth,strlen(name));
  }

  struct VComp {
    bool operator()(const InstrHash::value_type *x,const InstrHash::value_type *y)
    {
      return x->second > y->second;
    };
  };

  // This dumps the histogram to the output file.
  virtual void writeResults()
  {
    vector<const InstrHash::value_type *> values;
    ForEach(_counts,iter) {
      values.push_back(&(*iter));
    }
    sort(values.begin(),values.end(),VComp());

    _out << "\n\nInstruction Frequency (Ordered By Frequency):\n"
         << "=============================================\n\n";
    ForEach(values,iter) {
      _out << setw(_fwidth) << right << (*iter)->first << ":  " << (*iter)->second << '\n';
    }    

    _counts.clear();
    _fwidth = 0;
  }

  size_t    _fwidth;
  InstrHash _counts;
  ofstream  _out;

};

// This class is the service provider.  For a request for a writer of type
// 'icnt', it returns an instruction-count object.  It also displays a small
// banner at load-timee so that we know that the plugin was installed.
struct CountPlugin : public Plugin {

  // Respond to a request for a writer.
  virtual Writer *createWriter(IssNode &root,const std::string &type,const std::string &fn,const MemoryLog &memlog) 
  { 
    if (type == "icnt") {
      return new InstrCount(fn,root,memlog);
    }
    return 0;
  };

  // Called when the plugin is installed.
  virtual void init(AnyOption &options) 
  {
    cout << "Loaded the instruction-count plugin.\n";
    options.setFlag("foo","Example plugin flag.");
  };  

  // Called after final optiona processing.
  virtual void setup(AnyOption &options,IssNode &root)
  {
    if (options.getFlag("foo",false)) {
      cout << "Option foo was set with core " << root.name() << "\n";
    }
  }

};

static CountPlugin count_plugin;

// Main entry point- returns a pointer to our service-provider object.
extern "C" Plugin *adl_plugin_entry()
{
  return &count_plugin;
}
