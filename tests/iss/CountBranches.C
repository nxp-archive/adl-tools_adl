//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This demonstrates a very simple, example plugin which monitors branches.  It
// stores information about each branch- how many times it was taken vs. untaken
// and displays the results at the end of the simulation.
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

// Maps the effective addresses of branches to taken/untaken counts.
typedef unordered_map<addr_t,pair<unsigned,unsigned> > BranchHash;

// This maintains a branches and their taken/untaken counts and prints out the
// results at the end of a test to the specified file.
struct BranchCount : public Writer, public LoggingIface {

  // This just opens the file and makes sure that the operation succeeded.
  BranchCount(const std::string &filename,IssNode &root,const MemoryLog &memlog) :
    Writer(filename,root,memlog),
    _taken(false),
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

  // Store a pending-taken item.
  virtual void logBranchTaken(addr_t ea) 
  {
    _taken = true;
  }

  virtual void logBranchEnd(addr_t ea)
  {
    if (_taken) {
      ++_branches[ea].first;
    } else {
      ++_branches[ea].second;
    }
    _taken = false;
  }  

  // Required by the Writer class: Returns a string identifying the writer's
  // type.
  virtual const char *format_str() const { return "BCNT"; };

  // Order results by branch counts (taken + untaken).
  struct VComp {
    bool operator()(const BranchHash::value_type *x,const BranchHash::value_type *y)
    {
      return (x->second.first + x->second.second) > (y->second.first + y->second.second);
    };
  };

  // This dumps the histogram to the output file.
  virtual void writeResults()
  {
    vector<const BranchHash::value_type *> values;
    ForEach(_branches,iter) {
      values.push_back(&(*iter));
    }
    sort(values.begin(),values.end(),VComp());

    _out << "\n\nBranches Addr\tTaken/Untaken\n"
         << "=============================================\n\n";
    ostringstream tmp;
    ForEach(values,iter) {
      tmp << (*iter)->second.first << '/' << (*iter)->second.second;
      _out << hex << setw(13) << right << (*iter)->first << "\t" << setw(13) << right << tmp.str() << '\n';
      tmp.str(string());
    }    

    _branches.clear();
  }

  bool       _taken;
  BranchHash _branches;
  ofstream   _out;

};

// This class is the service provider.  For a request for a writer of type
// 'icnt', it returns an instruction-count object.  It also displays a small
// banner at load-timee so that we know that the plugin was installed.
struct BranchPlugin : public Plugin {

  // Respond to a request for a writer.
  virtual Writer *createWriter(IssNode &root,const std::string &type,const std::string &fn,const MemoryLog &memlog) 
  { 
    if (type == "bcnt") {
      return new BranchCount(fn,root,memlog);
    }
    return 0;
  };

  // Called when the plugin is installed.
  virtual void init(AnyOption &options) 
  {
    cout << "Loaded the branch-count plugin.\n";
  };  

};

static BranchPlugin branch_plugin;

// Main entry point- returns a pointer to our service-provider object.
extern "C" Plugin *adl_plugin_entry()
{
  return &branch_plugin;
}
