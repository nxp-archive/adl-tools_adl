//
// A simple driver to test a basic uADL SystemC model.
//

#define NOMINMAX

#include <iostream>
#include <string>
#include <systemc.h>
#include <iomanip>

#include "iss/ModelInterface.h"

#ifdef USE_LOGGING_MGR
# include "rnumber/RNumber.h"
#else
# define __NO_RNUMBER__
#endif

// Note: We actually want to instantiate the memory here, so it's being included
// without ADL_HEADER defined.
#include "iss/DefaultMem.h"
#include "iss/MemHandler.h"

#include <uadl/Timer.h>

#define UADL_MODEL_NAMESPACE dlx

#define UADL_SYSTEMC
#include <uadl/UadlArchIf.h>
#include <uadl/UadlMemory.h>

using namespace std;
using namespace adl;
using namespace uadl;

//
// These are the logging functions.  For this example, they simply display their
// results in a format similar to the DAT trace format used by the standalone ADL
// ISS executables.
//

static adl::addr_t instr_ea, instr_ra;

namespace adl {

  void log_instr_prefetch(const IssCore *c,addr_t ea)
  {
    cout << "\nI ea=0x" << hex << ea << "\n";
  }

  void log_instr_read(unsigned id,const char *name,addr_t ea,
                      addr_t ra,uint32_t data)
  {
    instr_ea = ea;
    instr_ra = ra;
    cout << "M n=Mem t=ifetch ea=0x" << hex << ea << " ra=0x" 
         << ra << " d=0x" << data << "\n";
  }

  void log_instr(const uint32_t* opc,int num_half_bytes,const char *name,Disassembler dis)
  {
    cout << "INSTR op=0x" << hex << setfill('0') << setw(num_half_bytes) << (*opc >> ((8-num_half_bytes)*4)) << " asm=\"";
    dis(cout,instr_ea,opc);
    cout << "\"\n";
  }

  void log_reg_write(unsigned id,const char *name,uint64_t value)
  {
    cout << "R n=" << name << " d=0x" << hex << value << "\n";
  }

  void log_regfile_write(unsigned id,const char *name,uint32_t index,
                         uint64_t value)
  {
    cout << "R n=" << name << " i=" << dec << index 
         << " d=0x" << hex << value << "\n";
  }

  void log_core_mem_write(unsigned id,const char *name,
                          addr_t ea,int nb)
  {
    cout << "D n=Mem t=write ea=0x" << hex << ea
         << " nb=" << dec << nb << "\n";
  }

  void log_core_mem_read(unsigned id,const char *name,
                         addr_t ea,int nb)
  {
    cout << "D n=Mem t=read ea=0x" << hex << ea 
         << " nb=" << dec << nb << "\n";
  }

  void log_mem_write(unsigned id,const char *name,bool pre,int seq,addr_t ea,
                     addr_t ra,uint32_t value)
  {
    if (!pre) {
      cout << "M n=Mem t=write ea=0x" << hex << ea 
           << " ra=0x" << ra << " d=0x" << value << "\n";
    }
  }

  void log_mem_read(unsigned id,const char *name,int seq,addr_t ea,
                    addr_t ra,uint32_t value)
  {
    cout << "M n=Mem t=read ea=0x" << hex << ea 
         << " ra=0x" << ra << " d=0x" << value << "\n";
  }

  void log_annotation(const IssCore *core,MsgType type,unsigned level,const std::string &msg)
  {
    cout << "A l=" << level << " m=\"" << msg << "\"\n";
  }

  void log_cache_action(const char *name,CacheAction action,CacheAccess access,unsigned level,
                        int set,int way,unsigned linemask,addr_t ra)
  {
    cout << "C n=" << name;
    switch (action) {
    case CacheLoad:
      cout << " a=load";
      break;
    case CacheHit:
      cout << " a=hit";
      break;
    case CacheMiss:
      cout << " a=miss";
      break;
    case CacheEvict:
      cout << " a=evict";
      break;
    case CacheNone:
      break;
    }

    switch (access) {
    case CacheNoAccess:
      cout << " t=none";
      break;
    case CacheIFetch:
      cout << " t=ifetch";
      break;
    case CacheRead:
      cout << " t=read";
      break;
    case CacheStore:
    case CacheWrite:
      cout << " t=write";
      break;
    case CacheFlush:
      cout << " t=flush";
      break;
    case CacheTouch:
      cout << " t=touch";
      break;
    case CacheAlloc:
      cout << " t=alloc";
      break;
    case CacheInvalidate:
      cout << " t=invalidate";
      break;
    case CacheLock:
      cout << " t=lock";
      break;
    case CacheUnlock:
      cout << " t=unlock";
      break;
    case CacheILogRead:
    case CacheLogRead:
      break;
    default:
      break;
    }

    cout << dec;

    if (set >= 0) {
      cout << " set=" << set;
    }
    if (way >= 0) { 
      cout << " way=" << way;
    }

    cout << " lm=0x" << hex << linemask << " ra=0x" << (ra & ~((addr_t)linemask)) << dec << "\n";    
  }

 
  InstrPacketBase *log_current_instr(InstrPacketBase *x)
  {
	  return 0;
  }

}



// memory class
class mMEM_mem_t : public sc_module, public Memory, public MemHandler {
public:
  mMEM_mem_t(sc_module_name name, Timer &timer, u_int addrBandwidth = 1,
	  u_int dataBandwidth = 1, u_int readLatency = 1,
	  u_int writeLatency = 1, u_int maxRequest = 1) :
  sc_module(name),
	  Memory(timer, (const char *)name, addrBandwidth, dataBandwidth, readLatency, writeLatency,
	  maxRequest, true, false, false),
	  _timer(timer)
  {
    SC_METHOD(scBeforeTimeStep);
    dont_initialize();
    sensitive << _clock.pos();

	SC_METHOD(scAfterTimeStep);
	dont_initialize();
	sensitive << _clock.neg();
  }
  
  void scBeforeTimeStep()
  {
	  Memory::preCycle();
  }

  void scAfterTimeStep()
  {
	  Memory::postCycle();
	  _timer.incrTime();
  }

  virtual ~mMEM_mem_t() {}

  void bindClock(sc_signal<bool> &clk) { _clock(clk); }

  // TODO: incorporate uadl Memory reset()
  virtual void reset() { GlobalMemory.reset(); }

  virtual uint64_t read64(CacheAccess ca,adl::addr_t ra) { return GlobalMemory.read64(ra); };
  virtual uint32_t read32(CacheAccess ca,adl::addr_t ra) { return GlobalMemory.read32(ra); };
  virtual uint16_t read16(CacheAccess ca,adl::addr_t ra) { return GlobalMemory.read16(ra); };
  virtual uint8_t  read8(CacheAccess ca,adl::addr_t ra) { return GlobalMemory.read8(ra); };
  virtual void     write64(adl::addr_t ra, uint64_t v, unsigned nb) { GlobalMemory.write64(ra,v,nb); };
  virtual void     write32(adl::addr_t ra, uint32_t v, unsigned nb) { GlobalMemory.write32(ra,v,nb); };
  virtual void     write16(adl::addr_t ra, uint16_t v, unsigned nb) { GlobalMemory.write16(ra,v,nb); };
  virtual void     write8 (adl::addr_t ra, uint8_t  v) { GlobalMemory.write8(ra,v); };
  virtual void     readpage(CacheAccess ca,adl::byte_t *t,size_t n,adl::addr_t addr,adl::addr_t crit_addr) { GlobalMemory.readpage(t,n,addr,crit_addr); };
  virtual void     writepage(adl::byte_t *s,size_t n,adl::addr_t addr,adl::addr_t crit_addr) { GlobalMemory.writepage(s,n,addr,crit_addr); };
  
protected:
	virtual adl::byte_t *hasData(adl::addr_t addr, u_int &size) const
  { return GlobalMemory.getPage(addr) + GlobalMemoryType::pageOffset(addr); }

  SC_HAS_PROCESS(mMEM_mem_t);

  // TODO: should be private
  sc_in<bool> _clock;
  Timer &_timer;
};

void check_reg(UadlArchSystemC *arch,const string &reg,uint64_t expected_value)
{
	uint64_t val;
	if (arch->iss().getReg(reg,0,val)) {
		if (val != expected_value) {
			cerr << reg << ":  Expected " << expected_value << ", but got " << val << '\n';
			throw runtime_error("Bad "+reg+" value.\n");
		}
	} else {
		throw runtime_error("Unknown register " + reg);
	}
}


int sc_main(int argc, char *argv[])
{
	try {
    // parse command line options and arguments
    const ttime_t startTime = 0; //options.getOption("start-time", 0);

    // instantiate the memory
    Timer timer;
    mMEM_mem_t mMEM("memory", timer);

    mMEM.write32(0x00000000,0x3821000A,4);	//	addi r1,r1,10
    mMEM.write32(0x00000004,0x38210014,4);	//	addi r1,r1,20
    mMEM.write32(0x00000008,0x90201000,4);	//	stw  r1,0x1000(r0)
    mMEM.write32(0x0000000c,0x80601000,4);	//	lwz  r3,0x1000(r0)
    mMEM.write32(0x00000010,0x38830032,4);	//	addi r4,r3,50

    unsigned id = 0;
    UadlArchSystemC *arch = UADL_MODEL_NAMESPACE::createArch("core", id, 0, startTime);

    arch->setIssMemHandler(&mMEM);
    arch->setExtMemory(mMEM);
    arch->setLogStream(&cerr);

    sc_clock clock;
  
    // bind ports
    arch->bindClock(clock);
    mMEM.bindClock(clock);
    cout << "Clock Port binding complete\n";
 
    int sim_time = 100;
    sc_start(sim_time, SC_NS);
    cout << "Finished running simulation for " << sim_time << " NS \n";

    cout << "Now doing some checks...\n";

    check_reg(arch,"GPR1",30);
    check_reg(arch,"GPR3",30);
    check_reg(arch,"GPR4",80);

    cout << "Done with checking.\n";
  }
	catch (exception &err) {
		cerr << "Error:  " << err.what() << endl;
    cout << "Error: " << endl;
		return 1;
	}

  return 0;
}
