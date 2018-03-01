//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include <assert.h>
#include <ostream>
#include <iomanip>

#include "helpers/Macros.h"

#include "iss/ModelInterface.h"
#include "iss/TypeIO.h"

#include "PipelineStage.h"

using namespace adl;
using namespace std;

namespace uadl {

  PipelineStageBase::PipelineStageBase(const std::string &name) : 
    _name(name),
    _logger(0)
  {}

  void PipelineStageBase::createLogger(unsigned count,LogClient *lc,unsigned tflags) 
  { 
    if (!_logger) { 
      _logger = new StageLogger(this,count,_name,lc,tflags); 
    } else {
      _logger->setClient(lc,tflags);
    }
  }

  void PipelineStageBase::removeLogger() 
  { 
    delete _logger; 
    _logger = 0; 
  };

  StageInstrLogger *PipelineStageBase::logger(unsigned index)
  {
    if (_logger) {
      return _logger->logger(index);
    } else {
      return 0;
    }
  }

  bool PipelineStageBase::report_and_clear(std::ostream &o,const std::string &pfx)
  {
    if (_logger) {
      return _logger->report_and_clear(o,pfx);
    } else {
      return false;
    }
  }


  IssueStageBase::IssueStageBase(const std::string &pfx) : 
    _pfx(pfx),
    _logger(0)
  {}

  void IssueStageBase::createLogger(LogClient *lc,unsigned tflags) 
  { 
    if (!_logger) { 
      _logger = new IssueLogger(this,_pfx,lc,tflags); 
    } else {
      _logger->setClient(lc,tflags);
    }
  }

  void IssueStageBase::removeLogger() 
  { 
    delete _logger; 
    _logger = 0; 
  };

  IssueLogger *IssueStageBase::logger(bool predTaken,addr_t predTrg) 
  { 
    if (_logger) {
      _logger->logInstruction(predTaken,predTrg);
      return _logger;
    } else {
      return 0;
    }
  }

  bool IssueStageBase::report_and_clear(std::ostream &o,const std::string &pfx)
  {
    if (_logger) {
      bool r = _logger->report_and_clear(o,pfx);
      return r;
    } else {
      return false;
    }
  }

  void instructionReport(ostream &o,const InstrBase *instr,const InstrPacketBase *ip,bool has_threads,unsigned tid)
  {
    if (instr && ip) {
      const uint32_t *opc = ip->opc();

      o << "[";
      if (has_threads) {
        o << "tid=" << tid << ", ";
      }
      o << "iid=" << dec << instr->iid() 
        << ", cid=" << instr->cid()+1 
        << ", ea=0x"<< hex << ip->addr() 
        << ", op=0x";
      unsigned sz = ip->size();
      if (ip->size() < 4) {
        o << setw(sz*2) << setfill('0') << (opc[0] >> (32 - sz*8));
      } else {
        ForRange((sz/4),i) {
          o << setw(8) << setfill('0') << opc[i];
        }
      }
      o << std::dec << "] ";

      // append instruction assembly 
      ip->_disasm(o, ip->addr(), opc);
    }
    else {
      o << "[fetch exception]";
    }
  }

  void StageInstrLogger::logInstruction()
  {
    if (logAll() && _stage->has_instr(_index) && _instr.str().empty()) {
      auto ii = _stage->getInstrInfo(_index);
      instructionReport(_instr,ii._instr,ii._pkt,ii._has_threads,ii._tid);
      _stage->logobj()->add_logger_to_list(this);
    }
  }

  bool StageInstrLogger::report(std::ostream &o,const std::string &pfx) const
  {
    if (!_instr.str().empty()) {
      string pfx2 = pfx + "  ";
      o << _instr.str() << '\n';
      Logger::report(o,pfx2);
      return true;
    } else {
      return false;
    }
  }


  StageLogger::StageLogger(const PipelineStageBase *stage,unsigned count,const string &name,LogClient *lc,unsigned tflags) :
    _name(name),
    _head(0),
    _tail(0)
  {
    ForRange(count,i) {
      _loggers.push_back(new StageInstrLogger(stage,i,lc,tflags));
    }
  }

  StageLogger::~StageLogger()
  {
    ForEach(_loggers,i) {
      delete *i;
    }
  }

  void StageLogger::setClient(LogClient *lc,unsigned tflags)
  {
    ForEach(_loggers,i) {
      (*i)->setClient(lc,tflags);
    }
  }

  StageInstrLogger *StageLogger::logger(unsigned index)
  {
    return _loggers.at(index);
  }

  bool StageLogger::report(std::ostream &o,const std::string &pfx) const
  {
    assert (!_loggers.empty());

    string indent(pfx.size() + _name.size() + 2, ' ');

    bool reported = false;
    o << pfx << _name << ":  ";
    if (_head) {
      StageInstrLogger *iter = _head;
      reported = iter->report(o,indent);
      iter = iter->next();
      for ( ; iter; iter = iter->next()) {
        if ( reported && iter->hasStuff()) {
          o << indent << " ";
        }
        bool r = iter->report(o,indent);
        reported = reported || r;
      }
    }
    if (!reported) {
      o << '\n';
    }
    return reported;
  }

  bool StageLogger::report_and_clear(std::ostream &o,const std::string &pfx)
  {
    bool r = report(o,pfx);
    ForEach(_loggers,i) {
      (*i)->clear();
    }
    _head = 0;
    _tail = 0;
    return r;
  }

  void IssueLogger::logInstruction(bool predTaken,addr_t predTrg)
  {
    if (logAll() && _issue->has_instr()) {
      auto ii = _issue->getInstrInfo();
      _instr << "issue ";
      instructionReport(_instr,ii._instr,ii._pkt,ii._has_threads,ii._tid);
      if (predTaken) {
        _instr << ", predicted taken, target 0x" << hex << predTrg;
      }
      logMessage(_instr.str());
      _instr.str(string());
    }
  }

  bool IssueLogger::report(std::ostream &o,const std::string &pfx) const
  {    
    return Logger::report(o,pfx+_pfx);
  }

}

