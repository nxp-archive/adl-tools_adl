//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A simple usage-logging utility.  This will only do something if the project
// has been configured to usage usage-logging.
//

#ifndef LOG_USAGE_H
#define LOG_USAGE_H

namespace adl {

  // Logs usage.  The version comes from the configuration header, the user name
  // and cwd come from the system, and the command-line arguments and
  // return-code code from the arguments of the function.
  // type:  Keyword type of item, e.g. "model", "asm", "disasm", "tool", etc.
  //        Used for classifying disparate ADL components.
  void log_usage(int argc, const char * argv[], int rc, const char *type );

  // Simpler version of usage logging: Only a cwd, program name, and version are
  // sent. Returns true if enabled.
  // type:  Keyword type of item, e.g. "model", "asm", "disasm", "tool", etc.
  //        Used for classifying disparate ADL components.
  // gentool:  True if this item is generated, e.g. a model.  In that case, information
  //           about ADL's version, is also included in the usage information, as well
  //           the name of the tool used to generate this item, specified by gentool.
  bool log_usage(const char *prog,const char *ver, int rc, const char *type, const char *gentool );

  // This registers a function which will be called at exit time and which will
  // log the usage of the program.
  void log_usage_at_exit(int argc,const char *argv[], const char *type );

  // Same as above, but uses the simpler version.  Returns true if enabled.
  bool log_usage_at_exit(const char *prog,const char *ver, const char *type, const char *gentool );
}

#endif
