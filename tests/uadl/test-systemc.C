//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This is a simple driver program to test interfacing a uADL model with
// SystemC and an external memory.
//
// Define MODEL on the command-line to specify what model to build.
//

#include <string>
#include <systemc.h>

#include "rnumber/RNumber.h"

#include "helpers/AnyOption.h"
#include "iss/SimInterface.h"

#define UADL_SYSTEMC
#include "uadl/UadlArchIf.h"

// Note: We actually want to instantiate the memory here, so it's being included
// without ADL_HEADER defined.
#include "iss/DefaultMem.h"

#include "iss/ModelInterface.h"
#include "iss/MemHandler.h"

#include "uadl/UadlMemory.h"

using namespace std;
using namespace adl;
using namespace uadl;
using namespace rnumber;

// True:  Log the thread IDs of memory reads.
bool LogMemThreadId = false;

// memory class
class MyExtMemory : public sc_module, public Memory, public MemHandler {
public:
  MyExtMemory(sc_module_name name,Timer &timer, u_int addrBandwidth = 1,
            u_int dataBandwidth = 1, u_int readLatency = 1,
            u_int writeLatency = 1, u_int maxRequest = 1) :
    sc_module(name),
    Memory(timer, (const char*)name, addrBandwidth, dataBandwidth, readLatency, writeLatency,
           maxRequest, true, false, false),
    _timer(timer)
  {
    SC_METHOD(scPreCycle);
    dont_initialize();
    sensitive << _clock.pos();

    SC_METHOD(scPostCycle);
    dont_initialize();
    sensitive << _clock.neg();
  }

  void bindClock(sc_signal<bool> &clk) { _clock(clk); }

  virtual void reset() { Memory::reset(); GlobalMemory.reset(); }

  virtual uint64_t read64(CacheAccess ca, addr_t ra)
    { return GlobalMemory.read64(ra); };
  virtual uint32_t read32(CacheAccess ca, addr_t ra)
    { return GlobalMemory.read32(ra); };
  virtual uint16_t read16(CacheAccess ca, addr_t ra)
    { return GlobalMemory.read16(ra); };
  virtual uint8_t read8(CacheAccess ca, addr_t ra)
    { return GlobalMemory.read8(ra); };
  virtual void write64(addr_t ra, uint64_t v, unsigned nb)
    { GlobalMemory.write64(ra, v, nb); };
  virtual void write32(addr_t ra, uint32_t v, unsigned nb)
    { GlobalMemory.write32(ra, v, nb); };
  virtual void write16(addr_t ra, uint16_t v, unsigned nb)
    { GlobalMemory.write16(ra, v, nb); };
  virtual void write8 (addr_t ra, uint8_t v)
    { GlobalMemory.write8(ra, v); };
  virtual void readpage(CacheAccess ca, byte_t *t, size_t n, addr_t addr,addr_t crit_addr)
  { GlobalMemory.readpage(t, n, addr, crit_addr); };
  virtual void writepage(byte_t *s, size_t n, addr_t addr,addr_t crit_addr)
  { GlobalMemory.writepage(s, n, addr, crit_addr); };

  virtual void read(unsigned portId, unsigned requestId, byte_t *data)
  {
    if (LogMemThreadId) {
      MemoryPort &port = getPort(portId);
      MemoryRequest &request = port.frontRequest();
      assert(request.id == requestId);

      if (request.type == adl::CacheRead) {
        cout << "Mem:  read: tid=" << request.rinfo._threadId << ", ea=0x" << hex << request.ea << ", ra=0x" << request.addr << "\n";
      }
    }

    Memory::read(portId,requestId,data);
  }

  virtual void write(unsigned portId, unsigned requestId, const byte_t *data)
  {
    if (LogMemThreadId) {
      MemoryPort &port = getPort(portId);
      MemoryRequest &request = port.frontRequest();
      assert(request.id == requestId);

      if (request.type == adl::CacheWrite) {
        cout << "Mem:  write: tid=" << request.rinfo._threadId << ", ea=0x" << hex << request.ea << ", ra=0x" << request.addr << "\n";
      }
    }

    Memory::write(portId,requestId,data);
  }
  
protected:
  void scPreCycle()
  {
	  Memory::preCycle();
  }

  void scPostCycle()
  {
	  Memory::postCycle();
	  _timer.incrTime();
  }

  virtual byte_t *hasData(addr_t addr, u_int &size) const
  { 
    return GlobalMemory.getPage(addr) + GlobalMemoryType::pageOffset(addr); 
  }

  SC_HAS_PROCESS(MyExtMemory);

  // TODO: should be private
  sc_in<bool> _clock;
  Timer &_timer;
};


// Optional watchdog class: This monitors the core and re-activates it until the
// designated GPR has a non-zero value.
class WatchDog : public sc_module {

public:

  SC_HAS_PROCESS(WatchDog);

  WatchDog(sc_module_name name,UadlArch &core,unsigned gpr,int holdoff) :
    sc_module(name),
    _holdoff(holdoff),
    _check_index(gpr),
    _nia_index(gpr+1),
    _core(core),
    _iss(core.iss()),
    _done(false),
    _hcount(0)
  {
    // Look up the register to be checked and store its index for fast
    // reference.
    int rfindex;
    if (!core.iss().getRegIndex(_gpr,rfindex,"GPR")) {
      RError("Bad watchdog register:  GPR");
    }

    SC_METHOD(scPostCycle);
    dont_initialize();
    sensitive << _clock.neg();
  }

  void bindClock(sc_signal<bool> &clk) { _clock(clk); }

protected:
  
  void scPostCycle()
  {
    if (!_done && !_core.is_active()) {
      if (_hcount < _holdoff) {
        ++_hcount;
      } else {
        _iss.getReg(_gpr,_check_index,_gval);
        if (_gval.iszero()) {
          _iss.getReg(_gpr,_nia_index,_gval);
          _core.setProgramCounter(_gval.uint64());
          _iss.set_active_count(1);
          cout << "Re-activating core at time " << sc_time_stamp().to_string() << "\n";
        } else {
          _done = true;
          cout << "Core is done at time " << sc_time_stamp().to_string() << "\n";
        }
      }
    }
  }

  const int    _holdoff;
  const int    _check_index;
  const int    _nia_index;

  sc_in<bool>  _clock;
  UadlArch    &_core;
  IssNode     &_iss;
  int          _gpr;
  RNumber      _gval;
  bool         _done;
  int          _hcount;
};

int sc_main(int argc, char *argv[])
{
  try {
    options().setFlag("trace-all", "ta", "enable all tracing information");
    options().setFlag("log-mem-tid","log the thread ID of memory reads and writes.");
    options().setOption("output", "o", "specify an output file");
    options().setOption("start-time", "s", "specify a start time");
    options().setOption("sim-time", "specify the amount of time to simulate");
    options().setOption("wake-up", "continue to wake up the core until the specified GPR has a non-zero value.  GPR+1 stores the NIA to be used.");
    options().setOption("wd-holdoff","number of cycles to wait between a core going in-active and re-activating it.");
    options().useCommandArgs(argc, const_cast<const char**>(argv));

    // Allow unknown options for now- they'll be checked again in init().
    options().errorOnUnknowns(false);

    // Parse command line options and arguments for uADL-specific options().
    // Further processing will be done by init().
    if (!options().processCommandLine()) {
      RError("Bad command-line.");
    }

    // parse command line options and arguments
    const bool logAll = options().getFlag("trace-all", false);

    const ttime_t startTime = options().getOption("start-time", 0);
    const int simTime       = options().getOption("sim-time",5000);
    const int wakeup        = options().getOption("wake-up",-1);
    const int holdoff       = options().getOption("wd-holdoff",0);
    
    LogMemThreadId          = options().getFlag("log-mem-tid",false);

    sc_clock clock;

    // instantiate the memory
    Timer timer;
    MyExtMemory mMem("memory", timer);

    // instantiate the microarchitecture
    unsigned id = 0;
    UadlArchSystemC &arch = *createArch("core", id, 0, startTime);

    // optional watchdog object.
    auto_ptr<WatchDog> watchdog( (wakeup >= 0) ? new WatchDog("watchdog",arch,wakeup,holdoff) : 0 );
    
    if (logAll) {
      arch.set_tracing();
    } else {
      arch.clear_tracing();
    }

    arch.setIssMemHandler(&mMem);
    arch.setExtMemory(mMem);
    arch.setLogStream(&cerr);

    // Set up SystemC clocks.
    mMem.bindClock(clock);
    arch.bindClock(clock);

    if (watchdog.get()) watchdog->bindClock(clock);

    init(&arch.iss(), argc, const_cast<const char**>(argv));

    sc_start(simTime, SC_NS);

    // close the microarchitecture
    handleFinalState(true, arch.iss());
    finalize();
  }
  catch (exception &err) {
    cerr << "\nError:  " << err.what() << "\n\n";
    return 1;
  }

  return 0;
}
