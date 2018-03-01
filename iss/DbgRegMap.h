//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This is the debugger register map class, which is used by the debugger interface
// to map debugger register indices to ADL model register indices.  This is a singleton
// object; a reference to the singleton may be obtained by calling dbgRegMap();
//

#ifndef _DBGREGMAP_H_
#define _DBGREGMAP_H_

#include <vector>
#include <unordered_map>

namespace adl {

  struct IssNode;

  // Information about a register entry.
  struct DbgRegInfo {
    int _id;
    int _index;      
    int _width;
    
    DbgRegInfo() : _id(-1), _index(-1), _width(-1) {};
    DbgRegInfo(int id,int index,int width) : _id(id), _index(index), _width(width) {};
  };

  class DbgRegInfos : public std::vector<DbgRegInfo>
  {
  public:
    DbgRegInfos() : _big_endian(false) {}

    bool isBigEndian() const { return _big_endian; };
    void setBigEndian(bool b) { _big_endian = b; };
  private:
    bool _big_endian;
  };

  // This maps node pointers to a register mapping object.
  struct DbgRegMap : public std::unordered_map<const IssNode *,DbgRegInfos> {
    typedef std::unordered_map<const IssNode *,DbgRegInfos> Base;

    using Base::find;

    // Look up a mapping.  Throws a runtime error if not found.
    const DbgRegInfos &find(const IssNode &) const;

  };

  // Access to the singleton object.
  DbgRegMap &dbgRegMap();

}

#endif
