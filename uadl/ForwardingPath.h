//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _FORWARDING_PATH_H
#define _FORWARDING_PATH_H

#include <sstream>
#include <utility>

#include "helpers/intbv.h"

#include "UadlTypes.h"

namespace uadl {

  // Data handler for normal-mode models (have data) for forwarding paths with
  // single values.
  template <class FwdPath,class InstrTypeD,class Operand,bool safeModeD>
  struct DataHandler {

    void read(FwdPath &fp,InstrTypeD &instr,Operand &op,unsigned sub_op,unsigned op_off) {
      instr._instr->set_slot(op.slot(sub_op,op_off),fp._data);
    }

    void read_partial(FwdPath &fp,InstrTypeD &instr,Operand &op,unsigned sub_op,unsigned op_off) {
      int slot = op.slot(sub_op,op_off);
      instr._instr->set_slot(slot,(instr._instr->slot(slot) & fp._mask) | (fp._data & ~fp._mask));
    }

    void write(FwdPath &fp,InstrTypeD &instr,Operand &op) {
      fp._data = instr._instr->slot(op.slot());
    }

  };

  // Data handler for normal-mode models (have data) for forwarding paths with
  // multiple values.
  template <class FwdPath,class InstrTypeD,class Operand,bool SafeModeD>
  struct DataHandler2 {

    void read(FwdPath &fp,InstrTypeD &instr,Operand &op,int index,unsigned sub_op,unsigned op_off) {
      instr._instr->set_slot(op.slot(sub_op,op_off),fp._entries[index]._data);
    }

    void read_partial(FwdPath &fp,InstrTypeD &instr,Operand &op,int index,unsigned sub_op,unsigned op_off) {
      int slot = op.slot(sub_op,op_off);
      instr._instr->set_slot(slot,(instr._instr->slot(slot) & ~fp._entries[index]._mask) | (fp._entries[index]._data & fp._entries[index]._mask));
    }

    void write(FwdPath &fp,InstrTypeD &instr,Operand &op,int index,unsigned sub_op) {
      fp._entries[index]._data = instr._instr->slot(op.slot(sub_op));
    }
  };

  // Data handler for safe-mode models (no data).
  template <class FwdPath,class InstrTypeD,class Operand>
  struct DataHandler<FwdPath,InstrTypeD,Operand,true> {
    void read(FwdPath &fp,InstrTypeD &instr,Operand &op,unsigned) {
    }

    void read_partial(FwdPath &fp,InstrTypeD &instr,Operand &op,unsigned) {
    }

    void write(FwdPath &fp,InstrTypeD &instr,Operand &op) {
    }
  };

  // Data handler for safe-mode models (no data).
  template <class FwdPath,class InstrTypeD,class Operand>
  struct DataHandler2<FwdPath,InstrTypeD,Operand,true> {
    void read(FwdPath &fp,InstrTypeD &instr,Operand &op,int index,unsigned sub_op) {
    }

    void read_partial(FwdPath &fp,InstrTypeD &instr,Operand &op,int index,unsigned sub_op) {
    }

    void write(FwdPath &fp,InstrTypeD &instr,Operand &op,int index,unsigned sub_op) {
    }
  };

  // 
  // This represents a forwarding path or a completion bus for multiple values.
  // The idea is that data is only valid for a single cycle; forwarding paths
  // are invalidated after each cycle.  The user writes to a fowarding path,
  // which writes data and operand ID information.  Querying the forwarding path
  // checks the register id information and reading it updates the instruction
  // packet, if the instruction packet contains data (normal-mode models).
  //
  // This relies upon the following methods to be generate for each operand:
  //   id():     Return id of operand.
  //   index():  Return index of operand (if relevant).
  //   mask():   Partial read mask.
  //   slot():   Return the slot for this operand.
  //   tag():    Tag associated with the current, actual register resource
  //
  //
  // SafeMode:  True if we're in safe-mode and thus don't care about storing data.
  //
  // Broadcast: Set by user parm.  If true, we're acting as a common-completion
  //             bus, where we're broadcasting data to waiting reservation
  //             stations.  As such, don't check the existing register's tag
  //             value for a match.
  //
  // TBD: Do we ever need to have forwarding path data be valid for more than
  // one cycle?  Might be needed for non-lock-step pipelines.  In that case,
  // we'd need a time tag.
  //
  // TBD: Do we care about endianness?  We just transport data, so it shouldn't
  // matter, I think.
  template <class InstrType,bool SafeMode,bool Broadcast,unsigned Size,unsigned Width>
  class ForwardingPath {
    typedef ForwardingPath<InstrType,SafeMode,Broadcast,Size,Width> This;

    typedef adl::intbv<Width> data_type;
  public:
    ForwardingPath(const string &name) : 
      _name(name)
    {
      _full.set();
      reset();
    }
    
    const std::string &name() const { return _name; };

    bool can_read(UadlArch &uarch,const InstrType &ic,Logger *logger,RegId id,tag_t last) const {
      return (can_read_check(id,last));
    }

    std::pair<data_type,tag_t> can_read_partial(UadlArch &uarch,const InstrType &ic,Logger *logger,RegId id) const {
      return (can_read_partial_check(id));
    }

    bool can_read_tag(UadlArch &uarch,const InstrType &ic,Logger *logger,RegId id,tag_t tag) const {
      return (can_read_check_tag(id,tag));
    }

    data_type can_read_tag_partial(UadlArch &uarch,const InstrType &ic,Logger *logger,RegId id,tag_t tag) const {
      return (can_read_partial_check_tag(id,tag));
    }

    template <class Operand>
    void write (UadlArch &uarch,InstrType &instr,Logger *logger,Operand &op) {
      assert((unsigned)_cur+op.num_sub_ops() <= Size);

      for (unsigned sub_op = 0; sub_op != op.num_sub_ops(); ++sub_op) {
        _entries[_cur]._id = op.id(uarch,instr,sub_op);
        _entries[_cur]._tag = instr.iid();
        _entries[_cur]._mask = op.mask(instr,sub_op);
        if (!_entries[_cur]._mask) {
          _entries[_cur]._mask.set();
        }

        // Update data if we store data (not in safe-mode).
        DataHandler2<This,InstrType,Operand,SafeMode> datah;
        datah.write(*this,instr,op,_cur,sub_op);
        
        if (logger) {
          std::ostringstream ss;
          ss << "write " << op.name(uarch,instr,sub_op)
             << " to " << _name << " (pos " << std::dec << _cur << ")";
          if (!SafeMode) {
            ss << " 0x" << std::hex << _entries[_cur]._data;
          }
          logger->logMessage(ss.str());
        }
        ++_cur;
      }
    }

    // We assume that we've alread checked to make sure that the read is valid.
    template <class Operand>
    void read (UadlArch &uarch,InstrType &instr,Logger *logger,Operand &op,unsigned sub_op = 0,unsigned op_off = 0) {
      DataHandler2<This,InstrType,Operand,SafeMode> datah;
      datah.read(*this,instr,op,find(op.id(uarch,instr,sub_op,op_off)),sub_op,op_off);
    }

    template <class Operand>
    void read_partial (UadlArch &uarch,InstrType &instr,Logger *logger,Operand &op,unsigned sub_op = 0,unsigned op_off = 0) {
      DataHandler2<This,InstrType,Operand,SafeMode> datah;
      for (int i = _cur-1; i >= 0; --i) {
        if (match(op.id(uarch,instr,sub_op,op_off),i)) {
          datah.read_partial(*this,instr,op,i,sub_op,op_off);
        }
      }
    }

    // Invalidate after each cycle.  We just have to reset _cur, since _cur = 0
    // means no entries.
    void postCycle()
    {
      _cur = 0;
    }

    // Full reset- clear everything to make debug easier.
    void reset()
    {
      _cur = 0;
      for (int i = 0; i != Size; ++i) {
        _entries[i]._id = RegInvalidId;
      }
    }

  private:
    template <class FwdPath,class InstrTypeD,class Operand,bool SafeModeD>
    friend struct DataHandler2;

    bool can_read_check(RegId id,tag_t last) const {
      int res;
      return ((res = find(id)) != -1) && 
        _entries[res]._mask == _full && 
        (Broadcast || last == _entries[res]._tag);
    }

    // Partial reads are tough, because of the need to combine data from
    // multiple paths and even from multiple entries.  In this case, we have to
    // combine the search of possible elements because two instructions might
    // have written valid register values, but with different valid bits, in the
    // same cycle.
    std::pair<data_type,tag_t> can_read_partial_check(RegId id) const {
      data_type mask = 0;
      tag_t tag = -1;

      for (int i = _cur-1; i >= 0; --i) {
        if (match(id,i)) {
          if (Broadcast) {
            return std::make_pair(_entries[i]._mask,_entries[i]._tag);
          } else {
            mask |= _entries[i]._mask;
            tag = (tag == -1) ? _entries[i]._tag : tag;
          }
        }
      }
      return std::make_pair(mask,tag);
    }

    bool can_read_check_tag(RegId id,tag_t tag) const {
      int res;
      return ((res = find(id)) != -1) && 
        _entries[res]._mask == _full && 
        (tag == _entries[res]._tag);
    }

    data_type can_read_partial_check_tag(RegId id,tag_t tag) const {
      int res;
      if (((res = find(id)) != -1) && 
          (tag == _entries[res]._tag)) {
        return _entries[res]._mask;
      } else {
        return 0;
      }
    }

    // Since elements are added front-to-back, iterate in reverse to find most
    // recent addition (in the case of multi-isse, where more than one
    // instruction might write the forwarding path in the same cycle).
    int find(RegId id) const {
      for (int i = _cur-1; i >= 0; --i) {
        if (match(id,i)) {
          return i;
        }
      }
      return -1;
    }

    bool match(RegId id,unsigned index) const {
      return (id == _entries[index]._id);
    }

    // Relevant data for an entry in the forwarding path.
    struct Entry {
      RegId       _id;       // Register id.
      tag_t       _tag;      // Identifier for writer instruction.  Applicable only if Broadcast == false
      data_type   _data;     // Data, if applicable (non-safe-mode).
      data_type   _mask;     // Mask, if applicable (0 means full register).
    };

    std::string _name;           // Forwarding path's name (for logging purposes).
    int         _cur;            // Next write location.    
    Entry       _entries[Size];  // All entries in the forwarding path.
    data_type   _full;           // Fully set mask, for comparison purposes.
  };

  // 
  // This represents a forwarding path or a completion bus for a single value.
  // The idea is that data is only valid for a single cycle; forwarding paths
  // are invalidated after each cycle.  The user writes to a fowarding path,
  // which writes data and operand ID information.  Querying the forwarding path
  // checks the register id information and reading it updates the instruction
  // packet, if the instruction packet contains data (normal-mode models).
  //
  // This relies upon the following methods to be generate for each operand:
  //   id():     Return id of operand.
  //   index():  Return index of operand (if relevant).
  //   mask():   Partial read mask.
  //   slot():   Return the slot for this operand.
  //
  // TBD: Do we ever need to have forwarding path data be valid for more than
  // one cycle?  Might be needed for non-lock-step pipelines.  In that case,
  // we'd need a time tag.
  //
  // TBD: Do we care about endianness?  We just transport data, so it shouldn't
  // matter, I think.
  template <class InstrType,bool SafeMode,bool Broadcast,unsigned Width>
  class ForwardingPath<InstrType,SafeMode,1,Broadcast,Width> {
    typedef ForwardingPath<InstrType,SafeMode,Broadcast,1,Width> This;

    typedef adl::intbv<Width> data_type;
  public:
    ForwardingPath(const string &name) : 
      _name(name),
      _id(InvalidOp),
      _mask(0)
    {
      _full.set();
    }
    
    const std::string &name() const { return _name; };

    bool can_read(UadlArch &uarch,const InstrType &ic,Logger *logger,RegId id,tag_t last) const {
      return (can_read_check(id,last));
    }

    data_type can_read_partial(UadlArch &uarch,const InstrType &ic,Logger *logger,RegId id) const {
      return (can_read_partial_check(id));
    }

    bool can_read_tag(UadlArch &uarch,const InstrType &ic,Logger *logger,RegId id,tag_t tag) const {
      return (can_read_check_tag(id,tag));
    }

    data_type can_read_tag_partial(UadlArch &uarch,const InstrType &ic,Logger *logger,RegId id,tag_t tag) const {
      return (can_read_partial_check_tag(id,tag));
    }

    template <class Operand>
    void write (UadlArch &uarch,InstrType &instr,Logger *logger,Operand &op) {
      _id = op.id(uarch,instr);
      _mask = op.mask(instr);
      _tag = instr.iid();
      if (!_mask) _mask.set();

      // Update data if we store data (not in safe-mode).
      DataHandler<This,InstrType,Operand,SafeMode> datah;
      datah.write(*this,instr,op);

      if (logger) {
        std::ostringstream ss;
        ss << "write " << op.name(uarch,instr) << " to " << _name;
        if (!SafeMode) {
          ss << " 0x" << std::hex << _data;
        }
        logger->logMessage(ss.str());
      }
    }

    // We assume that we've alread checked to make sure that the read is valid.
    template <class Operand>
    void read (UadlArch &uarch,InstrType &instr,Logger *logger,Operand &op,unsigned sub_op = 0,unsigned op_off = 0) {
      DataHandler<This,InstrType,Operand,SafeMode> datah;
      datah.read(*this,instr,op,sub_op,op_off);
    }

    template <class Operand>
    void read_partial (UadlArch &uarch,InstrType &instr,Logger *logger,Operand &op,unsigned sub_op = 0,unsigned op_off = 0) {
      DataHandler<This,InstrType,Operand,SafeMode> datah;
      datah.read_partial(*this,instr,op,sub_op,op_off);
    }

    // Invalidate after each cycle.
    void postCycle()
    {
      _id = InvalidOp;
    }

    void reset()
    {
      _id = InvalidOp;
    }

  private:
    template <class FwdPath,class InstrTypeD,class Operand,bool SafeModeD>
    friend struct DataHandler;

    bool can_read_check(RegId id,tag_t last) const {
      return (id == _id && 
              (Broadcast || last == _tag) && 
              _mask == _full);
    }

    std::pair<data_type,tag_t> can_read_partial_check(RegId id) const {
      if (id == _id) {
        // In this simple case, the broadcast behavior is the same as the normal behavior.
        return std::make_pair(_mask,_tag);
      } else {
        return 0;
      }
    }

    bool can_read_check_tag(RegId id,tag_t tag) const {
      return (id == _id && 
              (tag == _tag) && 
              _mask == _full);
    }

    data_type can_read_partial_check_tag(RegId id,tag_t tag) const {
      if (id == _id && 
          (tag == _tag) ) {
        return _mask;
      } else {
        return 0;
      }
    }

    std::string _name;     // Forwarding path's name (for logging purposes).
    RegId       _id;       // Register id.
    tag_t       _tag;      // Identifier for last writer.
    data_type   _data;     // Data, if applicable (non-safe-mode).
    data_type   _mask;     // Partial read mask.  0 => full register.
    data_type   _full;     // A fully-set mask for comparison purposes.
  };


}

#endif
