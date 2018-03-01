//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
#ifndef _FETCH_LOGGER_H
#define _FETCH_LOGGER_H

#include <vector>
#include <string>

#include "helpers/BasicTypes.h"

#include "Logger.h"

namespace uadl {

  using adl::addr_t;
  using adl::byte_t;


  class FetchLogger : public Logger {
  public:
    FetchLogger(const std::string &name,LogClient *lc,unsigned tflags) : Logger(name,lc,tflags) {}

    void logReceiveData(uint64_t id, addr_t ea, addr_t ra, unsigned size,
                        const byte_t *data);
    void logSendRequest(uint64_t id, addr_t ea, addr_t ra, unsigned size,
                        bool resend = false);
    void logSendRequest(uint64_t id, addr_t ea, addr_t ra, unsigned size,
                        bool predicted, const addr_t *takenAddr,
                        const addr_t *targetAddr, bool resend = false);

    void logInstrBufEntry(addr_t ea, addr_t ra, unsigned size, const byte_t *data,
                          bool predTaken, addr_t predTrg);
    void logPendingRequest(uint64_t id, addr_t ea, addr_t ra, unsigned size);

    void logStallException();
    void logStallNoEntry(unsigned minEntries, unsigned nEntries);
    void logStallNoSpace(unsigned minBytes, unsigned nBytes);
    void logStallCannotSend(const std::string &mem);
    void logStallPaused(uint64_t pauser);
    void logStallIssDown();

    void logFetchException(addr_t ea, unsigned size);
    void logBadPrediction(addr_t ea);

  private:
    std::string msgSendRequest(uint64_t id, addr_t ea, addr_t ra, unsigned size,
                               bool resend);
    static std::string stallPrefix() { return "fetch stalled: "; }
  };

}

#endif // _FETCH_LOGGER_H
