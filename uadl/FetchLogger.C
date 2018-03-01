//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
#include <string>
#include <iomanip>
#include <sstream>
#include "FetchLogger.h"


namespace uadl {

  using std::string;


  void FetchLogger::logReceiveData(uint64_t id, addr_t ea, addr_t ra, unsigned size,
                                   const byte_t *data)
  {
    if (logAll()) {
      std::ostringstream s;
      
      s << "receive 0x" << std::hex << std::setfill('0');
      
      for (unsigned i = 0; i < size; i++) s << std::setw(2) << (unsigned)data[i];
      
      s << " from (mid=" << std::dec << id << ", ea=0x" << std::hex << ea
        << ", ra=0x" << ra << ", size=" << std::dec << size << ")";
      
      logMessage(s.str());
    }
  }


  void FetchLogger::logSendRequest(uint64_t id, addr_t ea, addr_t ra, unsigned size,
                                   bool resend)
  { 
    if (logAll()) {
      logMessage(msgSendRequest(id, ea, ra, size, resend)); 
    }
  }


  void FetchLogger::logSendRequest(uint64_t id, addr_t ea, addr_t ra, unsigned size,
                                   bool predicted, const addr_t *takenAddr,
                                   const addr_t *targetAddr, bool resend)
  {
    if (logAll()) {
      std::ostringstream s;
      
      s << msgSendRequest(id, ea, ra, size, resend);
      
      if (!predicted) {
        s << " branch prediction disabled";
      }
      else if (takenAddr && targetAddr) {
        s << ", ea 0x" << std::hex << *takenAddr
          << " predicted taken, target 0x" << *targetAddr;
      }
      
      logMessage(s.str());
    }
  }


  void FetchLogger::logInstrBufEntry(addr_t ea, addr_t ra, unsigned size,
                                     const byte_t *data, bool predTaken,
                                     addr_t predTrg)
  {
    if (logAll()) {
      std::ostringstream s;

      s << "hold data (ea=0x" << std::hex << ea << ", ra=0x" << ra << ", data=0x"
        << std::setfill('0');
      
      for (unsigned i = 0; i < size; i++) s << std::setw(2) << (unsigned)(data[i]);
      
      s << ")";
      if (predTaken) {
        s << ", predicted taken, target 0x" << std::hex << predTrg;
      }
      
      logMessage(s.str());
    }
  }


  void FetchLogger::logPendingRequest(uint64_t id, addr_t ea, addr_t ra,
                                      unsigned size)
  {
    if (logAll()) {
      std::ostringstream s;

      s << "waiting for (mid=" << id << ", ea=0x" << std::hex << ea << ", ra=0x"
        << ra << std::dec << ", size=" << size << ")";
      
      logMessage(s.str());
    }
  }


  void FetchLogger::logStallException()
  {
    if (logAll()) {
      std::ostringstream s;

      s << stallPrefix() << "fetch exception pending";

      logMessage(s.str());
    }
  }


  void FetchLogger::logStallNoEntry(unsigned minEntries, unsigned nEntries)
  {
    if (logAll()) {
      std::ostringstream s;

      s << stallPrefix() << "insufficient instruction buffer space, required: "
        << minEntries << " entries, available: " << nEntries << " entries";
      
      logMessage(s.str());
    }
  }


  void FetchLogger::logStallNoSpace(unsigned minBytes, unsigned nBytes)
  {
    if (logAll()) {
      std::ostringstream s;
      
      s << stallPrefix() << "insufficient instruction buffer space, required: "
        << minBytes << " bytes, available: " << nBytes << " bytes";
      
      logMessage(s.str());
    }
  }


  void FetchLogger::logStallCannotSend(const string &mem)
  { 
    if (logAll()) {
      logMessage(stallPrefix() + mem + " unavailable"); 
    }
  }


  void FetchLogger::logStallPaused(uint64_t pauser)
  {
    if (logAll()) {
      std::ostringstream s;
      
      s << stallPrefix() << "fetch paused by instruction [iid=" << pauser << "]";
    
      logMessage(s.str());
    }
  }


  void FetchLogger::logStallIssDown()
  { 
    if (logAll()) {
      logMessage(stallPrefix() + "the ISS is inactive"); 
    }
  }


  void FetchLogger::logFetchException(addr_t ea, unsigned size)
  {
    if (logAll()) {
      std::ostringstream s;

      // TODO: remove ! after test
      s << "send request (ea=0x" << std::hex << ea << ", size=" << std::dec
        << size << ") ==> exception!";
      
      logMessage(s.str());
    }
  }

  void FetchLogger::logBadPrediction(addr_t ea)
  {
    if (logAll()) {
      std::ostringstream s;

      s << "taken prediction at ea 0x" << std::hex << ea << " is bad";

      logMessage(s.str());
    }
  }


  string FetchLogger::msgSendRequest(uint64_t id, addr_t ea, addr_t ra,
                                     unsigned size, bool resend)
  {
    std::ostringstream s;

    s << (resend ? "last request preempted, re-send" : "send request") << " (mid="
      << id << std::hex << ", ea=0x" << ea << ", ra=0x" << ra << ", size="
      << std::dec << size << ")";

    return s.str();
  }

} // namespace uadl
