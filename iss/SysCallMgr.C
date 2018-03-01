//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// The public interface consists of the ability to get and set arguments and
// trigger the occurrence of a system call.  The implementation, declared and
// defined here, stores arguments in a vector and calls registered system call
// handlers sequentially until one is able to handle it.
//

#include <cerrno>
#include <fcntl.h>
#include <time.h>

#ifndef _MSC_VER
# include <unistd.h>
#endif

#include <iostream>
#include <vector>

#include "helpers/Macros.h"

#include "SysCallMgr.h"
#include "SysCallHandler.h"
#include "SysCallExtHandler.h"
#include "ModelInterface.h"

using namespace std;

typedef list<adl::SysCallHandler*> Handlers;

namespace adl {

  // System-call reason codes
  enum syscodes {
    OsExit       =   1,
    OsRead       =   3,
    OsWrite      =   4,
    OsOpen       =   5,
    OsClose      =   6,
    OsAnnotate   =   7,
    OsCoreId     =  10,
    OsBrk        =  17,
    OsAccess     =  33,
    OsDup        =  41,
    OsTime       = 116,
    OsSchedYield = 158,
    OsGetCwd     = 183,
    OsLseek      = 199,
  };

#   define	nl_O_RDONLY	  0
#   define	nl_O_WRONLY	  1
#   define	nl_O_RDWR		  2
#   define	nl_O_APPEND	  0x0008
#   define	nl_O_CREAT		0x0200
#   define	nl_O_TRUNC		0x0400
#   define	nl_O_EXCL		  0x0800
#   define  nl_O_SYNC		  0x2000
#   define  nl_O_NDELAY	  0x4000
#   define	nl_O_NONBLOCK	0x4000
#   define	nl_O_NOCTTY	  0x8000

  struct FlagPair { int _in, _out; };
  static FlagPair FlagTranslations[] = {
    { nl_O_RDONLY,O_RDONLY },
    { nl_O_WRONLY,O_WRONLY },
    { nl_O_RDWR,O_RDWR },
    { nl_O_APPEND,O_APPEND },
    { nl_O_CREAT,O_CREAT },
    { nl_O_TRUNC,O_TRUNC },
    { nl_O_EXCL,O_EXCL },
#   ifndef _MSC_VER
    { nl_O_SYNC,O_SYNC },
    { nl_O_NDELAY,O_NDELAY },
    { nl_O_NONBLOCK,O_NONBLOCK },
    { nl_O_NOCTTY,O_NOCTTY },
#   endif
  };

# ifndef _MSC_VER
  static unsigned NumFlagTranslations = 11;
# else
	static unsigned NumFlagTranslations = 7;
# endif

  const int MAXPATH = 1024;

  // For now, assume that the smallest possible page is 1024.
  const addr_t MinPageMask = 1024 - 1;

  addr_t get_min_page(addr_t addr)
  {
    return addr & ~MinPageMask;
  }

  // Reads 'len' bytes of memory from ea into buf.  Translation is performed.
  // if is_string is true, this is treated as a string- we only read up until a
  // null character.
  int read_memory(char *buf,int len,bool is_string,addr_t ea,IssCore &core)
  {
    addr_t ra;
    if (!core.extDataReadTranslate(ra,ea)) {
      ra = ea;
    }
    addr_t last_page = get_min_page(ea);
    int32_t i = 0;
    for (; i < len; i++) {
      if ( get_min_page(ea) != last_page) {
        if (!core.extDataReadTranslate(ra,ea)) {
          ra = ea;
        }
        last_page = get_min_page(ea);
      }
      uint32_t tmp;
      core.debug_data_read(tmp,GlobalMemId,false,ra,8);
      buf[i] = tmp;
      ++ea;
      ++ra;
      if (is_string && buf[i] == 0) {
        break;
      }
    }
    return i;
  }

  // Reads memory from ea into str, stopping at a null.  Translation is performed.
  string read_string(addr_t ea,IssCore &core)
  {
    string str;
    addr_t ra;
    if (!core.extDataReadTranslate(ra,ea)) {
      ra = ea;
    }
    addr_t last_page = get_min_page(ea);
    uint32_t tmp;
    while (1) {
      if ( get_min_page(ea) != last_page) {
        if (!core.extDataReadTranslate(ra,ea)) {
          ra = ea;
        }
        last_page = get_min_page(ea);
      }
      core.debug_data_read(tmp,GlobalMemId,false,ra,8);
      if (!tmp) break;
      str.append(1,tmp);
      ++ea;
      ++ra;
    }
    return str;
  }

  // Writes 'len' bytes of memory from buf into ea.  Translation is performed.
  void write_memory(const char *buf,int len,addr_t ea,IssCore &core)
  {
    addr_t ra;
    if (!core.extDataWriteTranslate(ra,ea)) {
      ra = ea;
    }
    addr_t last_page = get_min_page(ea);
    for (int32_t i = 0; i < len; i++) {
      if ( get_min_page(ea) != last_page) {
        if (!core.extDataWriteTranslate(ra,ea)) {
          ra = ea;
        }
        last_page = get_min_page(ea);
      }
      core.debug_data_write(GlobalMemId,false,ra,buf[i],8);
      ++ea;
      ++ra;
    }
  }

  enum {
    MaxStackBuf = 1024
  };

  // Simple buffer object.  Small buffers (template argument) are included on
  // the stack, while anything over that size is heap allocated.
  template <size_t N> struct SimpleBuf {

    SimpleBuf(size_t size)
    {
      if (size > N) {
        _hbuf = new char[size];
        _ptr = _hbuf;
      } else {
        _hbuf = 0;
        _ptr = _sbuf;
      }
    }

    ~SimpleBuf()
    {
      delete [] _hbuf;
    }

    char *operator()() { return _ptr; };
    
    char  _sbuf[N];
    char *_hbuf;
    char *_ptr;
  };

  typedef SimpleBuf<MaxStackBuf> StackBuf;

  // The default system-call interface uses a basic UNIX-like scheme:
  //
  // arg0:  stack ptr.
  // arg1:  arg1
  // arg2:  arg2
  // arg3:  arg3
  // ...
  //
  struct DefaultSysCallHandler : public SysCallHandler
  {

    bool operator()(int64_t &rc,uint32_t &pgm_errno,uint32_t code,IssCore &core,SysCallMgr &mgr)
    {
#     ifndef _MSC_VER
      switch( code ) {
      case OsExit: {
        DPrint("exit\n");
        exit( pgm_errno,mgr.get_arg(1) );
        break;
      }
      case OsRead: {
        DPrint("read\n");
        int fd = mgr.get_arg(1);
        int len = mgr.get_arg(3);
        StackBuf buf(len);
        rc = os_read( pgm_errno,fd,buf(),len );
        addr_t ea = mgr.get_arg(2);
        write_memory(buf(),len,ea,core);
        break;
      }
      case OsWrite: {
        DPrint("write\n");
        int fd = mgr.get_arg(1);
        int len = mgr.get_arg(3);
        StackBuf buf(len);
        addr_t ea = mgr.get_arg(2);
        read_memory(buf(),len,false,ea,core);
        rc = os_write( pgm_errno,fd,buf(),len );
        break;
      }
      case OsOpen: {
        DPrint("open\n");
 
        // Should be small enough to just allocate on the stack.
        char path[MAXPATH];
        addr_t ea = mgr.get_arg(1);
        read_memory(path,MAXPATH,true,ea,core);

        int oflag = translateOpenFlags(mgr.get_arg(2));
        int mode  = mgr.get_arg(3);
        rc = os_open( pgm_errno, path, oflag, mode );
        break;
      }
      case OsClose: {
        DPrint("close\n");
        int fd = mgr.get_arg(1);
        rc = os_close(pgm_errno,fd);
        break;
      }
      case OsAnnotate: {
        DPrint("annotate\n");
        int level = mgr.get_arg(1);
        int len = mgr.get_arg(3);
        StackBuf buf(len);
        addr_t ea = mgr.get_arg(2);
        read_memory(buf(),len,true,ea,core);
        core.logger().log_annotation( mInfo, level, buf() );
        rc = 0;
        break;
      }
      case OsAccess: {
        DPrint("access\n");

        // Should be small enough to just allocate on the stack.
        char path[MAXPATH];
        addr_t ea = mgr.get_arg(1);
        read_memory(path,MAXPATH,true,ea,core);

        int amode = mgr.get_arg(2);
        rc = access( path, amode );
        break;
      }
      case OsDup: {
        DPrint("dup\n");
        int fd = mgr.get_arg(1);
        rc = os_dup(pgm_errno,fd);
        break;
      }
      case OsGetCwd: {
        DPrint("getcwd\n");
        addr_t ea = mgr.get_arg(1);
        size_t len = mgr.get_arg(2);
        StackBuf buf(len);
        rc = os_cwd(pgm_errno,buf(),len);
        write_memory(buf(),len,ea,core);
        break;        
      }
      case OsLseek: {
        DPrint("lseek\n");
        int fd = mgr.get_arg(1);
        off_t offset = mgr.get_arg(2);
        int whence = mgr.get_arg(3);
        rc = os_lseek(pgm_errno,fd,offset,whence);
        break;        
      }
      case OsBrk: {
        DPrint("brk\n");
        // We don't need to do anything for brk, since we have a sparse memory
        // model that will automatically allocate memory as needed.
        rc = 0;
        break;
      }
      case OsCoreId: {
        rc = core.getCoreId();
        break;
      }
      case OsTime: {
        DPrint ("gettimeoffday\n");
        // We use the instruction-count as the time:
        //   Seconds are instr-count / 1,000,000
        //   usecs are instr-count % 1,000,000
        // We assume that if 64-bit, the target values are 64-bit, otherwise 32-bit.
        addr_t ea = mgr.get_arg(1);
        addr_t ra;
        if (!core.extDataWriteTranslate(ra,ea)) {
          ra = ea;
        }
        if (mgr.is_64bit()) {
          core.mem_write64(ra,  core.instr_count() / 1000000ull,8);
          core.mem_write64(ra+8,core.instr_count() % 1000000ull,8);
        } else {
          core.mem_write32(ra,  core.instr_count() / 1000000ull,4);
          core.mem_write32(ra+4,core.instr_count() % 1000000ull,4);
        }
        rc = 0;
        break;
      }
      case OsSchedYield: {
        DPrint("sched_yield\n");
        rc = sched_yield();
        break;
      }
      default:
        return false;
      }
      return true;
#     else
      // For now, we don't support system calls in MSVC.  We should add this later.
      return false;
#     endif
    }

#   ifndef _MSC_VER

    // This tries to translate open/fcntl from newlib into host flags..
    int translateOpenFlags(int flags)
    {
      int oflags = 0;
      ForRange(NumFlagTranslations,i) {
        if (flags & FlagTranslations[i]._in) {
          oflags |= FlagTranslations[i]._out;
        }
      }
      return oflags;
    }

    int os_read(uint32_t &pgm_errno,int fd, char* buf, int len)
    {
      int rc = read( fd,buf,len );
      pgm_errno = errno;
      return rc;
    }

    int os_write(uint32_t &pgm_errno,int fd, char* buf, int len)
    { 
      int rc = write( fd,buf,len );
      pgm_errno = errno;      
      return rc;
    }

    int os_open(uint32_t &pgm_errno,char* name, int flags, int mode)
    {
      int rc = open(name,flags,mode);
      pgm_errno = errno;
      return rc;
    }

    int os_dup(uint32_t &pgm_errno,int fd)
    {
      int rc = dup(fd);
      pgm_errno = errno;
      return rc;
    }

    // Close a file descriptor, but not stdin, stdout, stderr, since that will
    // affect the simulator's ability to operate
    int os_close(uint32_t &pgm_errno,int fd)
    {
      if (fd > 2) {
        int rc = close(fd);
        pgm_errno = errno;
        return rc;
      } else {
        // Pretend that it succeeded.
        return 0;
      }
    }

    off_t os_lseek(uint32_t &pgm_errno,int fd,off_t offset,int whence)
    {
      off_t rc = lseek(fd,offset,whence);
      pgm_errno = errno;
      return rc;
    }

    int os_cwd(uint32_t &pgm_errno,char *buf,size_t len)
    {
      char *rc = getcwd(buf,len);
      pgm_errno = errno;
      return (rc != 0);
    }

    int exit(uint32_t &pgm_errno,uint32_t parm1)
    {
      //fixme: how do we want to handle the exit sc?   
      return 0; 
    }

#   endif

  };

  // This is the main system call handling class.  It dispatches system calls to
  // registered handlers.
  struct SysCallMgrImpl : public SysCallExtHandler, public SysCallMgr {

    typedef vector<uint64_t>         Args;

    Handlers              _handlers;
    DefaultSysCallHandler _defaultHandler;

    SysCallMgrImpl () : 
      _64bit(false)
    {
      addHandler(&_defaultHandler);
    }

    void addHandler(SysCallHandler *sc)
    {
      if (sc) {
        _handlers.push_front(sc);
      }
    }

    void syscall_add_arg(uint64_t arg)
    {
      _args.push_back(arg);
    }

    unsigned size_args() const
    {
      return _args.size();
    }

    uint64_t get_arg(unsigned index)
    {
      if (index >= _args.size()) {
        return 0;
      } else {
        return _args[index];
      }
    }

    bool syscall_enabled() const
    {
      // Declared externally.
      return SysCalls_enabled;
    }

    int64_t syscall_return_code() 
    {
      return _rc;
    }

    uint32_t syscall_errno()
    {
      return _pgm_errno;
    }

    bool is_64bit() const
    {
      return _64bit;
    }
    
    void syscall_trigger(IssCore &core,uint32_t syscode)
    {
      DPrint(" System call triggered: " << dec << syscode);

      addr_t mask = 0x8000;
      _64bit = (syscode & mask);
      syscode &= ~mask;

      _rc          = -1;  // assume fail
      _pgm_errno   = 0;
      bool handled = false;

      ForEach(_handlers,hiter) {
        if ( (**hiter)(_rc,_pgm_errno,syscode,core,*this) ) {
          handled = true;
          break;
        }
      }

      // Clear arguments so that the next system call will start with 0 existing
      // arguments.
      _args.clear();

      if (!handled) {
        cerr << "ERROR: unimplemented syscall (see unistd.h for syscall codes): " << dec << syscode << '\n';
      }
    }  

    bool     _64bit;
    int64_t  _rc;
    uint32_t _pgm_errno;
    Args     _args;
  };

  // A null object for use by Windows, since we don't support system call
  // emulation on that platform currently.
  struct SysCallMgrNull : public SysCallExtHandler {

	bool syscall_enabled() const { return false; }

    // Trigger the occurrence of a system call.
    void syscall_trigger(IssCore &c,uint32_t syscode) {}

    // Get/set argumeents.
    void syscall_add_arg(uint64_t) {}

    // Get return information.
    int64_t syscall_return_code() { return 0; };

    uint32_t syscall_errno() { return 0; };
  };

  SysCallExtHandler *createDefaultSysCallMgr()
  {
    // Return a null object for Windows.
#   ifndef _MSC_VER    
    return new SysCallMgrImpl();
#   else
    return new SysCallMgrNull;
#   endif
  }


}
