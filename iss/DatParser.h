//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef DATPARSER_H
#define DATPARSER_H 

#include "helpers/Macros.h"

#include "MmapFiles.h"
#include "Token.h"

#include "helpers/stringhash.h"

namespace rnumber {
  class RNumber;
}

#if !defined(_MSC_VER) && !defined(__MINGW32__)
typedef int HANDLE;
#endif

namespace adl {

  class LZ4FileInputStream;
  
  typedef TokLookup<Token> Pairs;
  typedef Pairs::pair_type TokPair;

  std::ostream &operator<<(std::ostream &,const Pairs &);

  Token cond_erase(Pairs::iterator x,bool erase);

  std::string get_str(Pairs &pairs,const char *n,const char *msg = 0,bool erase = false);
  rnumber::RNumber get_rn(Pairs &pairs,const char *n,const char *msg = 0,bool erase = false);
  uint64_t get_uint64(Pairs &pairs,const char *n,const char *msg = 0,bool erase = false);
  uint32_t get_uint32(Pairs &pairs,const char *k,const char *msg = 0,bool erase = false);
  int32_t get_int32(Pairs &pairs,const char *k,const char *msg = 0,bool erase = false);

  inline bool is_empty_pair(const Pairs::pair_type &x)
  {
    return x.second.empty();
  }

  // Base class for handling basic parsing needs.  Handles uncompressed files and
  // files compressed with lz4.  If compressed is 'false', then we'll check to
  // see if the file is compressed.  If true, then we assume that it is
  // compresssed.
  //
  // This is a base class that only understands how to skip comments and read
  // cards.  The card's pairs are stored and then the handleCard function is
  // called.  The derived class can then interpret the cards as needed.  All
  // tokens use the Token class, which is a read-only ptr/length string object
  // compatible with read-only memory.  For uncompressed files, I/O is done
  // using mmap, so tokens are valid forever.  For compressed files, a streaming
  // interface is used, where this class allocates buffers storing the data.  A
  // derived class has to take ownership of the buffers allocated by
  // getline_compressed and destroy them when done.

  class DatParser {
  public:
 
    DatParser (const std::string &filename,bool compressed, bool inc_scan = false);
    virtual ~DatParser();
    
    // Parse signle command for the given core or context,
    // Returns false if the given core is completele parsed
    bool parse_cmd(std::string core);

    // Parse a single line and return the card type.
    bool parse();
    
    // Trace reader
    bool parse_one_step();

    // Restart at beginning of file. 
    void rewind();
 
  protected:

    void reset_current();

    bool compressed() const { return _ilz4; };

    bool is_file_compressed() const;

    bool eof();

    // This is only called for compressed files and reads the next line of data.
    // It returns an allocated buffer.  A derived class should take ownership of
    // the data and destroy it when done.
    virtual std::pair<const char *,unsigned> getline_compressed();

    void eat_prefix();
    void eat_space();
    void eat_eol();

    Pairs::iterator find_key(const char *n);
    bool not_found (Pairs::iterator x) const;

    std::string get_str(const char *n,const char *msg,bool erase = false);
    rnumber::RNumber get_rn(const char *n,const char *msg,bool erase = false);
    uint64_t get_uint64(const char *n,const char *msg,bool erase = false);
    uint32_t get_uint32(const char *n,const char *msg,bool erase = false);
    int32_t get_int32(const char *n,const char *msg,bool erase = false);

    void get_pairs();  
    Token get_ident(char sep = 0);
    Token get_value();
    
    virtual bool handleCard(int ln,const Token &type) = 0;
    
    // Trace reader
    
    //end of trace
    bool eot();
    bool one_step_end();    
    void init_cur_to_trace();
    
    // End Trace reader
    
    virtual void error(int ln,const std::string &msg) = 0;
    
    // True if was stopped for some reason
    void stop() { _stopped = true; };
  private:

    void getline(); 
    
    //For incremental scanning does unmap for the current chunk of file and maps new.   
    void remap();

  protected:

    void setLineBufSize(unsigned n) { _linebufsize = n; };

    const char *base() const { return _base; };

    void setbase(char *b,unsigned n = 0) {
      _base = _cur = b;
      _end = _base + n;
    }

    // Size of line buffer, for compressed files.
    unsigned _linebufsize;

    // Temporary storage of pair data when parsing a line.
    Pairs    _pairs;

    // Set to true when done with a file.
    bool     _finished;

    // File descriptor and size used for uncompressed files.
    MmapHandle _fd;
    size_t     _size;

    // Used for managing input data.
    const char *_base;
    const char *_base_map;
    const char *_eot; // end of trace, for trace reader
    const char *_cur;
    const char *_end;

    // Only used by compressed files.
    LZ4FileInputStream *_ilz4;
    uint32_t _ln;
    
    //flag for incremental scan
    bool     _inc_scan;
    
    //number of bytes mapped my mmap
    off_t    _bytes_mapped;
    
    //global offset in input file
    off_t    _offset;
    
    std::string _fname;
    
    bool     _test_finished;
    bool     _stopped;  

    // Trace reader
    uint64_t _step;    
  };

}

#endif

