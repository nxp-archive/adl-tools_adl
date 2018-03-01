
#ifndef TARMACREADER_H
#define TARMACREADER_H

#include <unordered_map>
#include <regex>
#include <boost/regex.hpp>

#include "Reader.h"
#include "MmapFiles.h"
#include "Token.h"

namespace adl {

  class TarmacReader : public Reader {
  public:
    TarmacReader (const std::string& filename,IssNode *root,bool compressed = false);
    virtual ~TarmacReader ();
   
    const char *format_str() const { return "tarmac"; };
    
    void setRoot(IssNode *root);
    
    bool finished() const {return _finished; };
  
    // Trace reader methods
    
    void initTraceReading() {};
    
    bool hasTraceSupport() { return true; };
    
    bool applyNext(bool ignore_brpt);  //ignore_brpt is for runUntilTime
    
    bool applyPrev(bool ignore_brpt);
    
    uint64_t instrCount();       
    
  private: 
    // for now just implement one-core version  
    
    IssNode   	 *_cur_core;
    
    uint64_t _step; 
    uint64_t _timestamp;
    uint32_t _instr_mem_addr; 
    bool     _finished; 
    
    bool     _finish_brpt_set;
    addr_t   _finish_pc;
    bool     _start_brpt_set;
    addr_t   _start_pc;
    uint64_t _final_step;
    
    bool     _direction_fw;
    
    addr_t       _cur_pc;
    
    // File descriptor and size used for uncompressed files.
    MmapHandle _fd;
    size_t     _size;
    
    // Used for managing input data.    
    const char *_base;
    const char *_cur;
    const char *_end;  
    
    const char *_step_start;
    const char *_step_end; 
    
    //number of bytes mapped my mmap
    off_t    _bytes_mapped;
    
    //global offset in input file
    off_t    _offset;    
    
    // Reverse execution support - private members   
    
    enum RecType { TIME, PC, REG, MEM32 }; // for MEM64 just do two MEM32
    struct item_rec {
      item_rec(RecType type, std::string name, addr_t index_addr, uint64_t data){
        _type = type;
        _name = name;
        _index_addr = index_addr;
        _data = data;
      }        
      RecType _type; 
      std::string _name; 
      addr_t _index_addr; 
      uint64_t _data; 
    } ;
    typedef std::vector<item_rec> step_rec; // all previous values records in one step
    std::vector<step_rec> _trace_rec_bw;
    std::vector<step_rec> _trace_rec_fw;
    step_rec _step_rec_bw; // all records in one step, backwards
    step_rec _step_rec_fw; // all records in one step, forwards
    
    // parsing process
    
    bool parse_one_step(bool ignore_brpt);
    bool reverse_one_step(bool ignore_brpt);
    
    // helper functions
     
    bool step_from_rec(bool ignore_brpt);
    void reset_current();
    void eat_space();
    bool eof();
    Token get_step(const char *sep1, const char *sep2);
    unsigned get_reg_width(const IssNode *node, string name);
    uint32_t adjust_instr_addr(uint32_t addr);
    uint32_t reverse_endian(uint32_t value);
    uint64_t reverse_endian64(uint64_t value);   
    void update_partial_reg(uint64_t prior_data, uint64_t data, uint32_t lsb, uint32_t msb, unsigned size);
    
    void set_timestamp(string s_timestamp);
    void set_instruction(string instr_addr, string instr_opcode);
    void set_register(string reg_name, string reg_data);
    void set_memory_i(string mem_i_addr, string mem_i_data);
    void set_memory_d(string mem_d_addr, string mem_d_data);
    
  };
}

#endif