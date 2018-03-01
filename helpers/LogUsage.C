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

#include <string.h>
#include <stdlib.h>
#if !defined(_MSC_VER) && !defined(__MINGW32__)
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
# include <pwd.h>
# include <unistd.h>

# include <iostream>
# include <sstream>

# include "adl_config.h"
#endif

namespace adl {

#ifdef ADL_LOG_USAGE
  static const bool DoUsageLogging = true;
#else
  static const bool DoUsageLogging = false;
#endif

#if !defined(_MSC_VER) && !defined(__MINGW32__)

  static const char * const hostname = ADL_USAGE_LOG_SERVER;
  static const int portnum           = ADL_USAGE_LOG_PORT;

  using namespace std;

  // Returns 0 if an error occurs, else the number of bytes sent.
  static bool udp_send(const char * hostname, int portnum, int * sock, const char * message) 
  {
    struct sockaddr_in sockaddr;
   
    /* Create a datagram/UDP socket */
    if ((*sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0 ) {
      return 0;
    }

    //printf("sock is %d\n", *sock);
    /* Construct the server address structure */
    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;

    struct hostent * host_ent;
    char **p;
    int ret;
    int * intp;

    //      printf("hostlookup for %s\n", hostname);
    host_ent = gethostbyname(hostname);
    if (host_ent == 0 ) {
      cerr << "Warning:  gethostbyname failed lookup of " << hostname << '\n';
      return 0;
    }
    //      printf("hostlookup complete\n");

    p = host_ent->h_addr_list;
    intp = (int *) *p;
    ret = *intp;
    sockaddr.sin_addr.s_addr = ret;
    sockaddr.sin_port = htons(portnum);

    int message_len = strlen(message);
    int sent_bytes = sendto(*sock, message, message_len, 0,
                            (struct sockaddr *) &sockaddr, sizeof(sockaddr));
    if (sent_bytes != message_len) {
      cerr << "Warning:  sendto() sent a different number of bytes (" << dec << sent_bytes << ") than expected (" << message_len << ").\n";
      return 0;
    }
    return sent_bytes;
  }

  static char *get_username() 
  {
    const int MaxName = 1024;
    static char pw_name[MaxName];
    struct passwd *pw = getpwuid(getuid());

    if (pw && pw->pw_name)
      strncpy(pw_name,pw->pw_name,MaxName);
    else
      strncpy(pw_name,"UNKNOWN_USER",MaxName);

    return pw_name;
  }

  static void log_usage_prefix(ostream &o,const char *prog,int rc)
  {
    const int MaxPath = 1024;

    char cwd[MaxPath+1];
    const char *path = getcwd(cwd,MaxPath);
    // Just use what we have and ignore it if it overflows.

    o << get_username() << ':' << prog << ":rc=" << rc << ":cwd=" << path << ": ";
  }

  static void log_usage_args(ostream &o,const char *ver, const char *type, const char *gentool )
  {
    if (gentool) {
      o << " ##GENTOOL " << gentool << "## ##GENVERSION " << ADL_PACKAGE_VERSION << "##";
    }
    o << " ##VERSION " << ver << "## ##KEYWORDS adl";
    if (type) {
      o << "," << type;
    }
    o << "## ";

  }

  // Sends out a usage-logging message to the server with which we were
  // configured.
  void log_usage(int argc, const char * argv[], int rc, const char *type)
  {
    if (DoUsageLogging) {
      int sock;

      ostringstream msg;
      log_usage_prefix(msg,argv[0],rc);
      for (int i = 1; i != argc; ++i) {
        msg << argv[i] << ' ';
      }
      log_usage_args(msg,ADL_PACKAGE_VERSION,type,0);

      udp_send(hostname, portnum, &sock, msg.str().c_str());
    }
  }

  // Simpler version of usage logging.
  bool log_usage(const char *prog,const char *ver,int rc, const char *type, const char *gentool )
  {
    if (DoUsageLogging) {
      int sock;

      ostringstream msg;
      log_usage_prefix(msg,prog,rc);
      log_usage_args(msg,ver,type,gentool);

      udp_send(hostname, portnum, &sock, msg.str().c_str());
      return true;
    } else {
      return false;
    }
  }

  static int ul_argc          = 0;
  static const char **ul_argv = 0;
  static const char *ul_type  = 0;
  static const char *ul_gen   = 0;

  static void ul_at_exit(int rc,void *)
  {
    log_usage(ul_argc,ul_argv,rc,ul_type);
  }

  static const char *ul_prog = 0;
  static const char *ul_ver  = 0;

  static void ul_at_exit2(int rc,void *)
  {
    log_usage(ul_prog,ul_ver,rc,ul_type,ul_gen);
  }

  // This registers an exit handler which will log usage.  Info about arguments
  // are stored in statics for later use.  Note: on_exit is deprecated, according
  // to the man page, but atext() doesn't provide access to the return code, which
  // I need.
  void log_usage_at_exit(int argc,const char *argv[], const char *type )
  {
    if (DoUsageLogging) {
      ul_argc = argc;
      ul_argv = argv;
      ul_type = type;
      ul_gen  = 0;

      on_exit(ul_at_exit,0);
    }
  }


  // Simpler version of usage logging.
  bool log_usage_at_exit(const char *prog,const char *ver, const char *type, const char *gentool )
  {
    if (DoUsageLogging) {
      ul_prog = prog;
      ul_ver = ver;
      ul_type = type;
      ul_gen  = gentool;

      on_exit(ul_at_exit2,0);
      return true;
    } else {
      return false;
    }
  }

#else 

  //
  // We don't currently support usage logging on Windows.
  //

  void log_usage(int argc, const char * argv[], int rc, const char *type  ) 
  {
  }

  bool log_usage(const char *prog,const char *ver, int rc, const char *type, const char *gentool )
  {
    return false;
  }

  void log_usage_at_exit(int argc,const char *argv[], const char *type)
  {
  }

  bool log_usage_at_exit(const char *prog,const char *ver, const char *type, const char *gentool)
  {
    return false;
  }

#endif // !defined(_MSC_VER) && !defined(__MINGW32__)

}

