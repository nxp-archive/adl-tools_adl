//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
// TODO: In incremental scanning add fixed length storage for temporal tokens (instead of building string)

#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>

#include "adl_config.h"

#include "helpers/Macros.h"

#include "rnumber/RNumber.h"

#include "DatParser.h"

#if defined(ADL_HAVE_LZ4) && (ADL_HAVE_LZ4 == 1)
# include "lz4_stream/lz4_stream.h"
#endif

//
// Parsing routines for DAT files.
//

using namespace std;
using namespace rnumber;

namespace adl {

  ostream &operator<<(ostream &o,const Pairs &x)
  {
    o << "(";
    ForEach(x,i) {
      if (!i->first.empty()) {
        o << i->first << " => " << i->second << " ";
      }
    }
    o << ")";
    return o;
  }

# if defined(ADL_HAVE_LZ4) && (ADL_HAVE_LZ4 == 1)
  
  // Helper class for LZ4 compressed input streams.  This just stores the file
  // input stream with the compressed stream.
  class LZ4FileInputStream : public LZ4InputStream
  {
  public:
    LZ4FileInputStream(unique_ptr<ifstream> ifs) : LZ4InputStream(*ifs.get()), _ifs(std::move(ifs)) {};
    ~LZ4FileInputStream()
    {
      delete buffer_;
      buffer_ = 0;
    }
  private:
    unique_ptr<ifstream> _ifs;
  };

# endif  

  
  //
  // DatParser methods.
  //

  // Our line buffer is 4K, so that means we support lines up to 4K in length.
  const int Lz4LineBufSize = 4096;


  
  DatParser::DatParser(const std::string& filename,bool compressed,
                       bool inc_scan) : 
    _linebufsize(Lz4LineBufSize),
    _finished(false),
    _fd(default_handle_value()),
    _base(0),
    _eot(0),
    _cur(0),
    _end(0),
    _ilz4(0),
    _ln(0),
    _inc_scan(inc_scan),
    _bytes_mapped(0),
    _offset(0),
    _test_finished(false)    
  {
    if (PAGE_SIZE <= 0) {
      RError("Page size is unknown!");
      exit(1);
    }
    if (!compressed) {
      // Try the file as uncompressed first.  For this, we use memory-mapped I/O.      
      if (_inc_scan) {
        _bytes_mapped = MAP_SIZE;
      }

      _fd = open_mmap_file(filename.c_str(),_base,_bytes_mapped,_size);

      reset_current();

      compressed = is_file_compressed();
    }
    // Are we compressed?  Check the magic number.
    if (compressed) {
#     if defined(ADL_HAVE_LZ4) && (ADL_HAVE_LZ4 == 1)
      // It's compressed, so unmap it if we did map it and use stream I/O.
      if (_fd !=  default_handle_value()) {
        close_mmap_file(_fd,_base,_bytes_mapped);
        _fd = default_handle_value();
      }
      unique_ptr<ifstream> ifs(new ifstream(filename.c_str()));
      if (!*ifs) {
        RError("Can't open " << filename << " for reading from a compressed file.");
      }
      _ilz4 = new LZ4FileInputStream(move(ifs));
      _cur = _base = 0;
      _end = 0;
#     else      
      RError("Compressed DAT files are not supported by this configuration.");
#     endif
    }
  }

  DatParser::~DatParser()
  {
    if (compressed()) {
#     if defined(ADL_HAVE_LZ4) && (ADL_HAVE_LZ4 == 1)
      delete _ilz4;
#     endif      
    } else {
      close_mmap_file(_fd,_base,_bytes_mapped);
    }
   
  }


  // Reset to the beginning of input.
  void DatParser::rewind()
  {
    if (compressed()) {
      // We don't support rewinding in compressed streams.
      return;
    } else if (_offset) {
      _offset = 0;
      remap();
    } else {
      reset_current();
    }
  }

  void DatParser::reset_current()
  {
    _cur = _base;
    _end = _base + _bytes_mapped;   
  }

  // maps new portion of file
  void DatParser::remap() 
  {
    off_t old_bytes_mapped = _bytes_mapped;
    _bytes_mapped = (_offset + MAP_SIZE < _size) ? MAP_SIZE : _size - _offset;

    _base = remap_mmap_file(_fd,_base,_offset,old_bytes_mapped,_bytes_mapped);

    reset_current();
  } 
  
  
  const unsigned int BzipMagicSize = 5;
  const unsigned char BzipMagic[BzipMagicSize] = { 0x42, 0x5a, 0x68, 0x39, 0x31 };

  bool DatParser::is_file_compressed() const
  {
    if (_size >= BzipMagicSize) {
      for (unsigned i = 0; i != BzipMagicSize; ++i) {
        if (_base[i] != BzipMagic[i]) {
          return false;
        }
      }
      return true;
    } else {
      return false;
    }
  }

  // This gets the next line of data from the file.
  // For uncompressed files, this does nothing, since the entire file is present in
  // memory.  For compressed files, this reads the next line of data into our line buffer.
  pair<const char *,unsigned> DatParser::getline_compressed()
  {
#   if defined(ADL_HAVE_LZ4) && (ADL_HAVE_LZ4 == 1)
    // Temporary buffer to store this data.
    char buf[Lz4LineBufSize];
    _ilz4->getline(buf,Lz4LineBufSize);
    unsigned got = _ilz4->gcount();
    if (got) {
      // Remember: getline returns the number of chars read, including the
      // terminating null character.
      auto nbuf = new char[got];
      memcpy(nbuf,buf,got);
      nbuf[got-1] = '\n';
      setbase(nbuf,got);
      return make_pair(nbuf,got);
    } else {
      // End of file situation.
      setbase(0,0);
      return make_pair(nullptr,0);
    }
#   else
    return make_pair(nullptr,0);
#   endif
  }

  // Inlined wrapper- only does something if we are compressed.  The idea is to
  // take the virtual function out of the critical path if we are not compressed,
  // and only pay this overhead if we are compressed and thus consuming a decent
  // amount of CPU power anyway in order to do the decompression.
  inline void DatParser::getline()
  {
    if (compressed()) {
      getline_compressed();
    }
  }

  bool DatParser::eof()
  {
    if (compressed()) {
#     if defined(ADL_HAVE_LZ4) && (ADL_HAVE_LZ4 == 1)
      // Are we already at eof?
      return (_finished = _ilz4->eof());      
#     else
      return false;
#     endif
    } else if (!_inc_scan) {
      if (_cur >= _end) {
        _finished = true;
        return true;
      } else {
        return false;
      }
    } else {
      if (_cur >= _end) {      
        _offset += (_cur - _base);
        if (_offset >= (off_t)_size) {
          _finished = true;
          return true;
        } else {
          remap();
          return false;
        }        
      } else {
        return false;
      }
    }
  }

  // Eats leading whitespace and digits.  Then skips the next character, which should be a '.'.
  void DatParser::eat_prefix()
  {
    while (!eof() && (isspace(*_cur) || isdigit(*_cur))) {
      ++_cur;
    }
    if (!eof() && *_cur != '.' ) {
      RError("Improperly terminated prefix identifier.");
    }
  }

  // Eats whitespace.
  void DatParser::eat_space()
  {
    while (!eof() && isspace(*_cur) && (*_cur != '\n')) {
      ++_cur;
    }
  }

  // This consumes data to the end of the line.
  // This is only needed for mmap'd files, since for compressed files we'll
  // just read in another line.
  void DatParser::eat_eol()
  {
    if (!compressed()) {
      while (!eof() && (*_cur != '\n')) {
        ++_cur;
      }
      ++_cur;
    }
  }

  //
  // I don't love the current implementation of these functions b/c of the use
  // of the string for incremental mode.  However, it's a bit tricky- we can't
  // guarantee we'll see a full line in incremental mode because we're only
  // mapping a part of a file at a time and we can only map on page boundaries.
  // Therefore, we have to make a temorary copy.  This might be better if we
  // read in an entire line at once, so we might want to consider moving that
  // logic into getline at some point.
  //

  // Extracts an identifier from the stream.
  Token DatParser::get_ident(char sep)
  {
    eat_space();
    const char *start = _cur;

    // Needed for incremental mode, since we can't guarantee that we will be
    // able to see a whole line.
    string str;
     
    while (!eof() && !isspace(*_cur) && (*_cur != '\n') && !(sep && *_cur == sep)) {
      if (_inc_scan) {
        str.push_back(*_cur);
      }
      ++_cur;
    }
    
    if (_inc_scan) {
      return Token(str);
    } else {
      return Token(start,_cur - start,false);
    }
  }

  // Extracts a value from the stream.
  // When incremental scanning have to return allocated c-strings
  // because pointers are not valid after remap()
  Token DatParser::get_value()
  {
    const char *start = _cur;
    bool in_str = false;
    string str;

    if (!eof() && *_cur == '"') {
      ++start;
      ++_cur;
      in_str = true;
    }

    while (!eof()) {
      if (in_str && *_cur == '"') {
        ++_cur;
        if (_inc_scan) {
          return Token(str);
        } else {
          return Token(start,_cur-start-1,false);
        }
      } else if ((!in_str && isspace(*_cur)) || (*_cur == '\n')) {
        if (_inc_scan) {
          return Token(str);
        } else {
          return Token(start,_cur-start,false);
        }
      } else {
        if (_inc_scan) {
          str.push_back(*_cur);
        }
        ++_cur;
      }
    }
    return Token(start,(int)(_cur-start));
  }

  // Populates the pair map with key/value pairs.
  void DatParser::get_pairs()
  {
    _pairs.clear();
  
    while (1) {
      eat_space();
      if (!eof() && (*_cur == '\n' || *_cur == '#')) {
        break;
      }
      Token key = get_ident('=');
      if (eof()) break;
      ++_cur;
      Token value = get_value();
      if (value.empty()) {
        RError("Malformed key/value pair for key " << key << ":  No value found.");
      }

      _pairs.push_back(make_pair(move(key),move(value)));
    }
    _pairs.finalize();
    eat_eol();
  }

  Token cond_erase(Pairs::iterator x,bool erase)
  {
    Token t = x->second;
    if (erase) {
      x->second = Token();
    }
    return t;
  }

  // Looks up a key and returns the token value.
  string get_str(Pairs &pairs,const char *n,const char *msg,bool erase)
  {
    auto v = pairs.find_key(n);
    if (pairs.not_found(v)) {
      if (msg) {
        RError("Could not find key '" << n << "' for " << msg);
      } else {
        return string();
      }
    }
    return cond_erase(v,erase).str();
  }

  // Looks up a key and tries to convert it to an RNumber.  Throws an error
  // if there's a problem.
  RNumber get_rn(Pairs &pairs,const char *n,const char *msg,bool erase)
  {
    auto v = pairs.find_key(n);
    if (pairs.not_found(v)) {
      if (msg) {
        RError("Could not find key '" << n << "' for " << msg);
      } else {
        return RNumber();
      }
    }
    return RNumber(cond_erase(v,erase).str());
  }


  // Looks up a key and tries to convert it to a uint64_t.  If the key isn't found or
  // the data is not an integer, throws an error w/the specified message.
  uint64_t get_uint64(Pairs &pairs,const char *n,const char *msg,bool erase)
  {
    auto v = pairs.find_key(n);
    if (pairs.not_found(v)) {
      if (msg) {
        RError("Could not find key '" << n << "' for " << msg);
      } else {
        return 0;
      }
    }
    return atoi64(cond_erase(v,erase).data(),msg);
  }

  uint32_t get_uint32(Pairs &pairs,const char *k,const char *msg,bool erase)
  {
    auto v = pairs.find_key(k);
    if (pairs.not_found(v)) {
      if (msg) {
        RError("Could not find key '" << k << "' for " << msg);
      } else {
        return 0;
      }
    }
    return atoi32(cond_erase(v,erase).data(),msg);
  }

  int32_t get_int32(Pairs &pairs,const char *k,const char *msg,bool erase)
  {
    auto v = pairs.find_key(k);
    if (pairs.not_found(v)) {
      if (msg) {
        RError("Could not find key '" << k << "' for " << msg);
      } else {
        return -1;
      }
    }
    return atoi32(cond_erase(v,erase).data(),msg);
  }

  Pairs::iterator DatParser::find_key(const char *n)
  {
    return _pairs.find_key(n);
  }

  bool DatParser::not_found (Pairs::iterator x) const
  {
    return _pairs.not_found(x);
  }

  // Looks up a key and returns the token value.
  string DatParser::get_str(const char *n,const char *msg,bool erase)
  {
    return adl::get_str(_pairs,n,msg,erase);
  }

  // Looks up a key and tries to convert it to an RNumber.  Throws an error
  // if there's a problem.
  RNumber DatParser::get_rn(const char *n,const char *msg,bool erase)
  {
    return adl::get_rn(_pairs,n,msg,erase);
  }

  // Looks up a key and tries to convert it to a uint64.  If the key isn't found or
  // the data is not an integer, throws an error w/the specified message.
  uint64_t DatParser::get_uint64(const char *n,const char *msg,bool erase)
  {
    return adl::get_uint64(_pairs,n,msg,erase);}

  uint32_t DatParser::get_uint32(const char *k,const char *msg,bool erase)
  {
    return adl::get_uint32(_pairs,k,msg,erase);
  }

  int32_t DatParser::get_int32(const char *k,const char *msg,bool erase)
  {
    return adl::get_int32(_pairs,k,msg,erase);
  }

  // Parse a line.
  bool DatParser::parse()
  {
    bool errors = false;
    string type;
    string linestr;
    int multi_line_comment = 0;
    if (eof()) {
      return 0;
    }
    while (!eof()) {
      try {
        ++_ln;

        getline();

        Token type = get_ident();

        if (eof()) {
          break;
        }
	
        // Sanity check:  If the card is of monstrous size or the first character is not
        // ASCII, then signal an error.
        if (type.size() > 100 || (!type.empty() && type[0] & 0x80)) {
          RError("Bad card type:  Is this a binary file?");
        }

        if (type.empty() || type[0] == '#') {
          // Skip empty lines or comment lines.
          eat_eol();
          continue;
        } else if ((type == "=")) {
          Token id = get_ident();
          if (id.empty()) {
            RError("Bad multi-line comment delimiter.");
          }
          if (id[0] == '/') {
            --multi_line_comment;
          } else {
            ++multi_line_comment;
          }
          eat_eol();
          continue;
        }

        if (multi_line_comment) {
          eat_eol();
          continue;
        }
        
        get_pairs();
        bool res = handleCard(_ln,type);
	
        if (_inc_scan) {
          //"_stopped" should be controlled by handleCard
          if (_stopped) {
            return _test_finished;
          }
        } else {
          if (!res) {
            return true;
          } 
        }

      }
      catch (exception &err) {
        errors = true;
        error(_ln,err.what());
      }
    }
    if (errors) {
      RError("Parsing error.");
    }
  
    return eof()?0:1;

  }

  // Trace reader
  
  bool DatParser::parse_one_step()
  {
    bool step_exe = false;
    // skip empty lines
    while(*_cur == '\n' || isspace(*_cur)) {
      ++_cur;
    }
    do{
      Token type = get_ident();
      get_pairs();
      bool res = handleCard(_step,type);
      if(res) {
        step_exe = true;
      }
    }while (!(eot() || ((*_cur=='I') && (*(_cur+1) == '\n'))));
    if (step_exe) {
      ++_step;
    }
    return true;
  }
  
  bool DatParser::eot()
  {
    return _cur >= _eot;
  }
  
  bool DatParser::one_step_end()
  { 
    // cursor is at the first space of a line    
    while (isspace(*_cur)) // skip white space
    {
      ++_cur;
    }    
    if (*_cur == '\n') {
      ++_cur;  // move to next line first
      return true;
    }
    else
      return false;
  }
  
  void DatParser::init_cur_to_trace()
  {
    char *temp_cur = (char*)_base;
    _eot = _base;
    // reset _cur and _end;
    reset_current();
    while (!(eof() || (_eot != _base))){      
      Token type = get_ident();
      // Assuming only one TRACE section  
      if (type == "TRACE") {
        eat_eol(); // _cur moved to the first of next line
        temp_cur = (char*)_cur;
      }
      if (type == "RESULTS") {
        _eot = _cur-1;
        while (*_eot!= '\n') {  // set end to the last '\n'
          --_eot;
        }
      }     
      eat_eol(); //continue
      }
      
      if (temp_cur == _base) { // no TRACE section
        RError("No TRACE section");
      }
      _cur = temp_cur;
      if (_eot == _base) { // no RESULT section
        _eot = _end;
      }
    }
  }

