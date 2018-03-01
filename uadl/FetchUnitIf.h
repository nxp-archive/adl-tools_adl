//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
#ifndef _FETCH_UNIT_IF_H
#define _FETCH_UNIT_IF_H

#include <string>
#include <vector>
#include <helpers/BasicTypes.h>
#include "Named.h"


namespace uadl {

using adl::byte_t;

class FetchLogger;

class FetchUnitIf : public Named {
public:
  FetchUnitIf(const std::string &name) : Named(name), _logger(NULL) {}

  virtual ~FetchUnitIf() {}

  /*****************************************************************************
   * reset the fetch unit (clear instruction buffer, reset fetch address, etc.)
   *
   * parameters:
   *   newFetchAddr - new fetch address after reset
   ****************************************************************************/
  virtual void reset(addr_t newFetchAddr) = 0;

  /*****************************************************************************
   * preCycle() method of the fetch unit
   *
   * NOTE: This function is called at the beginning of every cycle after the
   *       preCycle() functions of all memories/caches are called.  It
   *       usually handles receiving instruction data from the fetch memory or
   *       cache, but can also include other bookkeeping tasks.
   ****************************************************************************/
  virtual void receiveInstrData() = 0;

  /*****************************************************************************
   * postCycle() method of the fetch unit
   *
   * NOTE: This function is called at the end of every cycle before the
   *       postCycle() functions of all memories and caches are called.  It
   *       usually handles sending fetch requests to the fetch memory, but can
   *       also include other bookkeeping tasks.
   ****************************************************************************/
  virtual void sendFetchRequests() = 0;

  /*****************************************************************************
   * return the next instruction data to be decoded
   *
   * parameters:
   *   ea        - ea of the returned data (if data returned)
   *   ra        - ra of the returned data (if data returned)
   *   size      - size (in bytes) of the returned data (if data returned)
   *   predTaken - if data returned, whether it is predicted taken
   *
   * return value:
   *   - Return NULL if the fetch unit believes that it has no sufficient data
   *     that can form a valid instruction.
   *   - Otherwise, return sufficient data (in the form of a pointer) that
   *     should contain a valid instruction.
   *
   * NOTE: The fetch unit should only determine if it has enough data for an
   *       instruction based on data size.  It should not do any decoding
   *       attempt.  It is safe to always return data without determining if
   *       the data has minimum instruction size because the core model will
   *       catch all decode misses, including insufficient data, though this
   *       approach is not efficient in terms of performance.
   ****************************************************************************/
  virtual const byte_t *instrData(std::vector<addr_t> &ea,
                                  std::vector<addr_t> &ra, unsigned &size,
                                  bool &predTaken) = 0;

  /*****************************************************************************
   * update fetch unit status based on decoding outcome
   *
   * parameters:
   *   success - whether the last decoding succeeded or failed
   *   size    - if the last decoding succeeded, the instruction size
   *
   * NOTE: If the last instrData() call returns four bytes for decoding and the
   *       first two bytes form a valid instruction, the next decodeUpdate()
   *       call will have size '2' and the fetch unit should make sure that the
   *       last two bytes of the data returned by instrData() will be considered
   *       again for the next instrData() call.
   ****************************************************************************/
  virtual void decodeUpdate(bool success, unsigned size) = 0;

  /*****************************************************************************
   * remove an instruction from the fetch unit
   *
   * parameters:
   *   size - instruction size (in bytes)
   *
   * NOTE: The fetch unit should not automatically remove instruction data upon
   *       a successful decoding.  It should only do so when removeInstr() is
   *       called because a decoded instruction may still reside in the
   *       instruction buffer until it enters the next pipeline stage.
   ****************************************************************************/
  virtual void removeInstr(unsigned size) = 0;

  /*****************************************************************************
   * flush instruction buffer and reset fetch address
   *
   * parameters:
   *   newFetchAddr - new fetch address after flush
   *
   * return value:
   *   If the flush is complete (true) or partial (false).  Some fetch unit may
   *   support partial flush so that if the new fetch address is already fetched
   *   or being fetched, only contents before that address are flushed.
   ****************************************************************************/
  virtual bool flush(addr_t newFetchAddr) = 0;

  /*****************************************************************************
   * flush instruction buffer and reset fetch address to the first address that
   * is in the fetch unit.  In other words, this gets rid of all pending
   * transactions and resets everything back to the next fetch address that will
   * be delivered to decode.  If can_cancel = true, then outstanding transactions
   * should be canceled; false if not.
   *
   ****************************************************************************/
  virtual void flush(bool can_cancel) = 0;

  /*****************************************************************************
   * Similar to the flush above, except that data in the fetch unit is not 
   * removed.  This just cancels existing transactions.
   *
   ****************************************************************************/
  virtual void cancel(bool can_cancel) = 0;

  /*****************************************************************************
   * force the fetch unit to do a fetch attempt in the current cycle
   *
   * NOTE: The fetch unit usually decides if it will do fetch attempts (send
   *       fetch requests) in receiveInstrData() based on instruction buffer
   *       size, etc., and that decision controls if the fetch unit will attempt
   *       to send fetch requests to the fetch memory in sendFetchRequests().
   *       If forceFetch() is called, the fetch unit will always do fetch
   *       attempts in the next sendFetchRequests() call regardless of its
   *       previous decision.
   ****************************************************************************/
  virtual void forceFetch() = 0;

  /*****************************************************************************
   * pause fetch in the current cycle
   *
   * parameters:
   *   instrId - id of the instruction that pauses fetch
   *
   * NOTE: This function is the counterpart of forceFetch().  If pauseFetch() is
   *       called, the fetch unit will not do fetch attempts in the next
   *       sendFetchRequests() call regardless of its previous decision.  If
   *       both forceFetch() and pauseFetch() are called in the same cycle,
   *       whichever gets called last takes effects.
   ****************************************************************************/
  virtual void pauseFetch(uint64_t instrId) = 0;

  /*****************************************************************************
   * the first pending fetch request is preempted, update status accordingly
   *
   * NOTE: Some memory/cache has the ability to preempt a pending fetch request
   *       in favor of a data read/write request.  preemptFetch() informs the
   *       fetch unit about that occurrence.  The fetch unit needs to resend
   *       the preempted fetch request.
   ****************************************************************************/
  virtual void preemptFetch() = 0;

  // access/query methods

  /*****************************************************************************
   * return the instruction buffer entry size (in bytes)
   ****************************************************************************/
  virtual unsigned instrBufEntrySize() const = 0;

  /*****************************************************************************
   * Return true if the fetch unit has no undecoded data and no pending fetch
   * requests.
   ****************************************************************************/
  virtual bool empty() const = 0;

  /*****************************************************************************
   * return the next fetch address
   ****************************************************************************/
  virtual addr_t nextFetchAddr() const = 0;

  /*****************************************************************************
   * Return the ea of the next undecoded instruction that the fetch unit has or
   * will have.
   ****************************************************************************/
  virtual addr_t nextInstrAddr() const = 0;

  // logging methods

  /*****************************************************************************
   * attach a fetch logger to the fetch unit
   *
   * parameters:
   *   logger - pointer to the fetch logger
   ****************************************************************************/
  void attachLogger(FetchLogger *logger) { _logger = logger; }

  /*****************************************************************************
   * log instruction buffer status and pending requests status to the logger
   ****************************************************************************/
  virtual void instrBufReport() const = 0;

protected:
  FetchLogger *_logger;
};

} // namespace uadl

#endif // _FETCH_UNIT_IF_H
