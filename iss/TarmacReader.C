#include <sstream>
#include <iostream>
#include <stdexcept>
#include <assert.h>
#include <ctype.h>

#include "rnumber/RNumber.h"
#include "TarmacReader.h"

using namespace std;
using namespace rnumber;

namespace adl
{

  TarmacReader::TarmacReader (const string& filename,IssNode *root,bool compressed) :
    Reader(filename,root),
    _step(0),
    _finished(false),
    _finish_brpt_set(false),
    _start_brpt_set(false),
    _final_step(0),
    _direction_fw(true),
    _cur_pc(0),
    _base(0),
    _cur(0),
    _end(0), 
    _step_start(0),
    _step_end(0),
    _bytes_mapped(0),   
    _offset(0)
  {
    if (_root) {
      setRoot(_root);
    }
    
    _fd = open_mmap_file(filename.c_str(),_base,_bytes_mapped,_size);
    
    reset_current();
    applyNext(false);
  }  
  
  TarmacReader::~TarmacReader()
  {
    close_mmap_file(_fd,_base,_bytes_mapped);   
  }
  
  void TarmacReader::reset_current()
  {
    _cur = _base;
    _step_start = _base;
    _step_end = _base;
    _end = _base + _bytes_mapped;   
  }

  void TarmacReader::setRoot(IssNode *root)
  {
    Reader::setRoot(root);
    _cur_core = root;
  }  
  
  // Trace reader methods
  
  uint64_t TarmacReader::instrCount()
  {
    return _step;
  }
  
  bool TarmacReader::applyNext(bool ignore_brpt)
  {
    return parse_one_step(ignore_brpt);
  }
  
  // KEY_FUNC: applyPrev
  bool TarmacReader::applyPrev(bool ignore_brpt)
  {
    return reverse_one_step(ignore_brpt);
  }
  
  // KEY_FUNC: parse_one_step
  bool TarmacReader::parse_one_step(bool ignore_brpt) 
  {
    if (!_direction_fw && _step != _final_step) {
      _step ++;
      _direction_fw = true;
    }
    if (_step < _trace_rec_fw.size()) {
      return step_from_rec(ignore_brpt);
    }
    boost::smatch s_time_temp, s_instr, s_reg, s_mem_i, s_mem_d;
    string s_time;
    boost::regex e_time{"([0-9.]+)( ?)([munpf]{1}s)"};
    boost::regex e_instr{"(?: IT +| IT +\\([0-9a-f:]+\\) +| ES +)(?:\\(?)([0-9a-fx]+)(?: *)(?::?)([0-9a-fx]+)(?: |\\) O)"};
    boost::regex e_reg{"(?: R )([a-zA-Z0-9\\[\\]:]*) +(?:\\(\\w+\\) )*([0-9a-fx]{8}) ?([0-9a-fx]{8})?"};
    boost::regex e_mem_i{"(?: MN?R[0-9]{1}\\w*I) +([0-9a-f]{8}|[0-9a-f]{16}) +([0-9a-fx]{8}|[0-9a-fx]{16})"};
    boost::regex e_mem_d{"(?: LD|ST|MN?[RW]{1}[0-9]{1}\\w*D) +([0-9a-f]{8}|[0-9a-f]{16}) +([0-9a-f.x ]+)"};
    
    string instr_key_1 = "IT ";
    string instr_key_2 = "ES ";
    Token one_step = get_step(instr_key_1.c_str(), instr_key_2.c_str());
    // if this is at the very beginning, get one more step (because the first step is all the inits)
    // TODO: put this at the end
    if (_step == 0) {
      Token next_step = get_step(instr_key_1.c_str(), instr_key_2.c_str());
      one_step.resize(one_step.size() + next_step.size());
    }
    
    string parse_step_orig = one_step.str();
    string parse_step = parse_step_orig;
    // search for timestamp, only keeps the last timestamp
    while (boost::regex_search (parse_step,s_time_temp,e_time)) { 
      s_time = s_time_temp[1].str();  
      parse_step = s_time_temp.suffix().str();
    }   
    if (!s_time.empty()) {
      set_timestamp(s_time);        
    }
    // search for instruction mem access
    parse_step = parse_step_orig;
    while (boost::regex_search (parse_step,s_mem_i,e_mem_i) ) {    
      set_memory_i(s_mem_i[1].str(), s_mem_i[2].str());
      parse_step = s_mem_i.suffix().str();
    }
    // search for instruction exe, should only contain one instruction
    if (boost::regex_search (parse_step_orig,s_instr,e_instr)) {  
      set_instruction(s_instr[1].str(), s_instr[2].str());
    }
    // search for reg update
    parse_step = parse_step_orig;
    while (boost::regex_search (parse_step,s_reg,e_reg)) {
      set_register(s_reg[1].str(), s_reg[2].str());
      parse_step = s_reg.suffix().str();      
    }
    // search for data mem access
    parse_step = parse_step_orig;
    while (boost::regex_search (parse_step,s_mem_d,e_mem_d) ) {    
      set_memory_d(s_mem_d[1].str(), s_mem_d[2].str());
      parse_step = s_mem_d.suffix().str();
    }
    _trace_rec_bw.push_back(_step_rec_bw);
    _trace_rec_fw.push_back(_step_rec_fw);
    _step_rec_bw.clear();
    _step_rec_fw.clear();
    
    ++_step;
        
    // when reaches the end, should only reach here once, the next time would be using the record
    if (_finished) {
      // _cur_core->set_active_count(0);
      // return false;
      --_step;
      if (_start_brpt_set) {
        _cur_core->clearBreakpointByAddr(_start_pc);  // remove start_pc brpt in first run  
        _start_brpt_set = false;
      }
      if (!_finish_brpt_set) {
        _final_step = _step;
        _cur_core->setBreakpoint(_cur_pc);  // make a temp breakpoint at the end  
        _finish_pc = _cur_pc; 
        _finish_brpt_set = true;
      }
    }
    else {
      if (_start_brpt_set) {
        _cur_core->clearBreakpointByAddr(_start_pc);  // remove start_pc brpt in first run  
        _start_brpt_set = false;
      }
      if (_finish_brpt_set) {
        _cur_core->clearBreakpointByAddr(_finish_pc);  // remove temp breakpoint  
        _finish_brpt_set = false;
      }
    }
    
    if (!ignore_brpt || ((_step == _final_step) && (_final_step != 0))) {
      _cur_core->checkForBreakpoint(_cur_pc);
    }
    
    return true;
  }      
  // KEY_FUNC: reverse_one_step
  bool TarmacReader::reverse_one_step(bool ignore_brpt) 
  {
    step_rec one_step;
    if (_direction_fw && _step != 1) {
      _step--;
      _direction_fw = false;
    }
    if (_step == _final_step && _final_step != 0) {
      if (_finish_brpt_set) {
        _cur_core->clearBreakpointByAddr(_finish_pc);
        _finish_brpt_set = false;
      }
    }
    if (_step <= _trace_rec_bw.size()) {
      one_step = _trace_rec_bw.at(_step);  
    }
    else {
      return false; // should never reach here
    }
    
    for (step_rec::reverse_iterator rec = one_step.rbegin(); rec != one_step.rend(); ++rec) {
      switch (rec->_type) {
        case TIME :
          _timestamp = rec->_data;
          _cur_core->set_td_counter(_timestamp);
          break;
        case PC :
          _cur_core->setProgramCounter(rec->_data);
          _cur_pc = reverse_endian(rec->_data);
          break;
        case REG :
          _cur_core->setReg(rec->_name, rec->_index_addr, rec->_data);
          break;
        case MEM32 :
          _cur_core->setMem(rec->_name, rec->_index_addr, rec->_data, 32);
          break;
        default :
          break;
      }
    }
    if (_step > 1) {
      if (_finish_brpt_set) {
        _cur_core->clearBreakpointByAddr(_finish_pc);    // remove finish_pc brpt in reverse run    
        _finish_brpt_set = false;
      }
      if (_start_brpt_set) {
        _cur_core->clearBreakpointByAddr(_start_pc);
        _start_brpt_set = false;
      }
      --_step;
    }    
    else if (_step <= 1) {
      if (_finish_brpt_set) {
        _cur_core->clearBreakpointByAddr(_finish_pc);   // remove finish_pc brpt in reverse run     
        _finish_brpt_set = false;
      }
      if (!_start_brpt_set) {
        _cur_core->setBreakpoint(_cur_pc);  // make a temp breakpoint at the start
        _start_pc = _cur_pc;
        _start_brpt_set = true;
        _direction_fw = true;
      }
    }
    if (!ignore_brpt || (_step <= 1)) {
      _cur_core->checkForBreakpoint(_cur_pc);
    }
    return true;
  }  
  
  // KEY_FUNC: step_from_rec
  bool TarmacReader::step_from_rec(bool ignore_brpt) {
    step_rec one_step = _trace_rec_fw.at(_step);
    do {
      for (auto rec:one_step) {  
        switch (rec._type) {
          case TIME :
            _timestamp = rec._data;
            _cur_core->set_td_counter(_timestamp);
            break;
          case PC :
            _cur_core->setProgramCounter(rec._data);
            _cur_pc = reverse_endian(rec._data);
            break;
          case REG :
            _cur_core->setReg(rec._name, rec._index_addr, rec._data);
            break;
          case MEM32 :
            _cur_core->setMem(rec._name, rec._index_addr, rec._data, 32);
            break;
          default :
            break;
        }
      }
      
      if (_start_brpt_set) {
        _cur_core->clearBreakpointByAddr(_start_pc);  // remove start_pc brpt in run from record  
        _start_brpt_set = false;
      }
      if (_step == _final_step && _final_step != 0) { // reaches the end, should issue a breakpoint
        _cur_core->setBreakpoint(_finish_pc);  
        _finish_brpt_set = true;
        _direction_fw = false;
      }
      else {
        if (_finish_brpt_set) {
          _cur_core->clearBreakpointByAddr(_finish_pc);
          _finish_brpt_set = false;
        }
        ++_step;
        _direction_fw = true;
      }
    } while (_step <= 1);   // if it is at the very beginning, we do one more step
    
    if (!ignore_brpt || ((_step == _final_step) && (_final_step != 0))) { // here _final_step should have already set
      _cur_core->checkForBreakpoint(_cur_pc);
    }
    return true;
  }
  
  // KEY_FUNC: set_timestamp
  void TarmacReader::set_timestamp(string s_timestamp)
  {
    _step_rec_bw.push_back(item_rec(TIME, "NA", 0, _timestamp));     
    _timestamp = (uint64_t)(stof(s_timestamp));
    _cur_core->set_td_counter(_timestamp);
    _step_rec_fw.push_back(item_rec(TIME, "NA", 0, _timestamp)); 
  } 
  
  // KEY_FUNC: set_instruction
  void TarmacReader::set_instruction(string instr_addr, string instr_opcode)
  { 
    addr_t get_PC;
    get_PC = _cur_core->getProgramCounter();
    _step_rec_bw.push_back(item_rec(PC, "NA", 0, get_PC));    
    
    uint32_t addr = adjust_instr_addr(atoi32("0x" + instr_addr,"nia"));
    
    uint32_t r_addr = reverse_endian(addr);
    _cur_core->setProgramCounter(r_addr);
    _step_rec_fw.push_back(item_rec(PC, "NA", 0, r_addr)); 
    
    uint32_t opcode = atoi32("0x" + instr_opcode,"nia_opcode");
    uint32_t data = reverse_endian(opcode);
    // write to memory
    string name = "Mem";
    DPrint("m " << name << " 0x" << hex << addr << ":  0x" << data << ", bytes = " << "32" << "\n");
    _cur_core->setMem(name,addr,data,32);
    
    _cur_pc = addr;
    
  }  
  
  // KEY_FUNC: set_register
  void TarmacReader::set_register(string reg_name, string reg_data)
  {
    // get reg name and index separately
    unsigned i = 0;
    while (!isdigit(reg_name[i]) && reg_name[i]!='[' && i < reg_name.size()){
      i ++;       
    }    
    unsigned j = i;
    if (i < reg_name.size()) {
      while (reg_name[j]!='[' && j < reg_name.size()){
        j ++;       
      }   
    }
    string name = reg_name.substr(0,i);
    uint32_t index = 0;
    if (i != reg_name.size()) {      
      index = atoi32("0x" + reg_name.substr(i,j), "reg_index");
    }
    string partial_update;
    uint32_t lsb = 0;
    uint32_t msb = 63;
    if (j != reg_name.size()) { 
      // this is a partial register update,  for now specifically for ppc64 itrace -> tarmac case
      partial_update = reg_name.substr(j,reg_name.size()); 
      size_t sep_pos = partial_update.find(":");
      lsb = atoi32("0x" + partial_update.substr(1,sep_pos), "lsb");
      msb = atoi32("0x" + partial_update.substr(sep_pos+1,partial_update.size()-1), "msb");
      if (lsb > msb) { swap(lsb,msb); }
    }
    
    // record old reg value
    uint64_t prior_data = 0;
    _cur_core->getReg(name, index, prior_data);
    _step_rec_bw.push_back(item_rec(REG, name, index, prior_data));
    
    // replace x or X with f in new data
    replace(reg_data.begin(), reg_data.end(), 'x', 'f');    
    replace(reg_data.begin(), reg_data.end(), 'X', 'f');
    uint64_t data = atoi64("0x" + reg_data, "reg_data");
    
    unsigned size = get_reg_width(_cur_core, name);
    if (!partial_update.empty()) {
      // do a partial update, new value in data
      update_partial_reg(prior_data, data, lsb, msb, size);
    }
    
    DPrint("r " << name << " " << dec << index << ": 0x" << hex << data << "\n");
    if(size == 32) {
      uint32_t data32 = data;
      _cur_core->setReg(name,index,reverse_endian(data32)); 
      _step_rec_fw.push_back(item_rec(REG, name, index, reverse_endian(data32)));
    }
    else {     // size = 64    
      _cur_core->setReg(name,index,reverse_endian64(data));  
      _step_rec_fw.push_back(item_rec(REG, name, index, reverse_endian64(data)));
    }
  }  
  // KEY_FUNC: set_memory_i
  void TarmacReader::set_memory_i(string mem_i_addr, string mem_i_data)
  {
    string name = "Mem";
    uint64_t addr = atoi64("0x" + mem_i_addr,"mem_addr");
    _instr_mem_addr = addr;
    
    // replace x with f, . with 0, erase white spaces
    replace(mem_i_data.begin(), mem_i_data.end(), 'x', 'f');
    replace(mem_i_data.begin(), mem_i_data.end(), 'X', 'f');
    replace(mem_i_data.begin(), mem_i_data.end(), '.', '0');
    mem_i_data.erase(remove(mem_i_data.begin(), mem_i_data.end(), ' '), mem_i_data.end());
    
    unsigned write_times = mem_i_data.size() / 8;
    unsigned i;
    for (i = 0; i < write_times; i ++) {
      // we don't need to record mem here because set_instruction() already did that.
      // write new value to mem      
      string mem_i_data32 = mem_i_data.substr(mem_i_data.size() - i*8 - 8, mem_i_data.size() - i*8);
      uint32_t data = reverse_endian(atoi32("0x" + mem_i_data32,"mem_data_l"));
      _cur_core->setMem(name,addr,data,32);       
      DPrint("m " << name << " 0x" << hex << addr << ":  0x" << data << ", bytes = " << "32" << "\n");
      addr = addr + 0x4;
    } 
  }
  // KEY_FUNC: set_memory_d
  void TarmacReader::set_memory_d(string mem_d_addr, string mem_d_data)
  {      
    string name = "Mem";
    uint64_t addr = atoi64("0x" + mem_d_addr,"mem_addr");
    
    // replace x with f, . with 0, erase white spaces
    replace(mem_d_data.begin(), mem_d_data.end(), 'x', 'f');
    replace(mem_d_data.begin(), mem_d_data.end(), 'X', 'f');
    // replace(mem_d_data.begin(), mem_d_data.end(), '.', '0');
    mem_d_data.erase(remove(mem_d_data.begin(), mem_d_data.end(), ' '), mem_d_data.end());
    
    unsigned write_times = mem_d_data.size() / 8;
    unsigned i;
    for (i = 0; i < write_times; i ++) {
      // check accessibility
      string mem_d_data32 = mem_d_data.substr(mem_d_data.size() - i*8 - 8, 8);
      if (mem_d_data32 != "........") {
        // get mem for record
        uint64_t get_mem_data = _cur_core->getMem(name, addr, 32);
        _step_rec_bw.push_back(item_rec(MEM32, name, addr, get_mem_data));
        
        // write new value to mem
        uint32_t data = reverse_endian(atoi32("0x" + mem_d_data32,"mem_data_l"));
        _cur_core->setMem(name,addr,data,32); 
        _step_rec_fw.push_back(item_rec(MEM32, name, addr, data));
      }
      addr = addr + 0x4;
    }
  }
  
  //// Helper functions ////
  
  // Gets a register width from introspection data.
  unsigned TarmacReader::get_reg_width(const IssNode *node, string name)
  {
    const RegInfos &ri = node->getRegInfo();
  
    ForEach(ri,i) {
      if (i->_name == name) {
        return i->_width;
      }
    }
    return 0;
  }
  
  // Reverse endianness for 32 bits data
  uint32_t TarmacReader::reverse_endian(uint32_t value) {
    uint32_t first  = value & 0xff000000;
    uint32_t second = value & 0x00ff0000;
    uint32_t third  = value & 0x0000ff00;
    uint32_t forth  = value & 0x000000ff;
    uint32_t new_value = (first >> 24) | (second >> 8) | (third << 8) | (forth << 24);
    // uint32_t new_value = value;
    return new_value;
  } 
  
  // Reverse endianness for 64 bits data
  uint64_t TarmacReader::reverse_endian64(uint64_t value) {
    uint64_t first   = value & 0xff00000000000000;
    uint64_t second  = value & 0x00ff000000000000;
    uint64_t third   = value & 0x0000ff0000000000;
    uint64_t forth   = value & 0x000000ff00000000;
    uint64_t fifth   = value & 0x00000000ff000000;
    uint64_t sixth   = value & 0x0000000000ff0000;
    uint64_t seventh = value & 0x000000000000ff00;
    uint64_t eighth  = value & 0x00000000000000ff;
    uint64_t new_value = (first >> 56) | (second >> 40) | (third >> 24) | (forth >> 8) | (fifth << 8) | (sixth << 24) | (seventh << 40) | (eighth << 56);
    // uint64_t new_value = value;
    return new_value;
  } 
    
  // Adjust instr addr for the case that some of the code copied from flash to ram
  // KEY_FUNC: adjust_instr_addr
  uint32_t TarmacReader::adjust_instr_addr(uint32_t addr) {
    // compare only the upper 8 bits
    int i;
    for (i = 31; i > 23; i--){
      if ((addr & 1<<i) != 0){
        break;
      }
    }
    
    if (i == 23) { 
      for (int i = 31; i > 23; i--){
        if ((addr & 1<<i) != (_instr_mem_addr & 1<<i)) {
          addr = (addr & ~(1<<i)) | (_instr_mem_addr & 1<<i); 
        }
      }
    }  
    return addr;
  }
  
  // KEY_FUNC: update_partial_reg
  void TarmacReader::update_partial_reg(uint64_t prior_data, uint64_t data, uint32_t lsb, uint32_t msb, unsigned size)
  {
    if (size == 64) {
      prior_data = reverse_endian64(prior_data);        
    }
    else {
      prior_data = reverse_endian(prior_data);
    }
    
    uint64_t mask = 1;
    // reset prior data from lsb to msb
    for (unsigned i=lsb; i<=msb; i++) {
      prior_data &= ~(mask << i);
    }
    data = prior_data | (data << lsb);
  }

  // Eats whitespace.
  void TarmacReader::eat_space()
  {
    while (!eof() && isspace(*_cur) && (*_cur != '\n')) {
      ++_cur;
    }
  }
     
  // KEY_FUNC: get_step
  Token TarmacReader::get_step(const char *sep1, const char *sep2) // need const char?
  {
    eat_space();
    _step_start = _step_end;
     
    while (!eof() && strncmp(sep1, _cur, strlen(sep1)) != 0 && strncmp(sep2, _cur, strlen(sep2)) != 0) {
      ++_cur;
    }
    ++_cur;
    
    _step_end = _cur;
    
    // rewind back to the end of prev line
    while (!eof() && (_step_end > _base) && (*_step_end != '\n')) {
      --_step_end;
    }

    return Token(_step_start,_step_end - _step_start,false);      
  }
  
  bool TarmacReader::eof()
  {
    if (_cur >= _end) {
      _finished = true;
      return true;
    } else {
      return false;
    }  
  }
}

