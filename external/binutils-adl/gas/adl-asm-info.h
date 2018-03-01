//
// Header file for support variables, functions, and types used by the
// basic assembler support, to communicate between assembler-independent and
// dependent portions of code.
//

#ifndef _ADL_ASM_INFO_H_
#define _ADL_ASM_INFO_H_

#include <assert.h>
#include <vector>
#include <string>

#include "xregex.h"

  // All messaging is directed to our own functions, which are then implemented
  // based upon the interface type (DevTech vs. gas).
#define AS_FATAL       adl_fatal
#define AS_BAD         adl_bad
#define AS_WARN        adl_warn
#define AS_TSKTSK      adl_tsktsk
#define AS_BAD_WHERE   adl_bad_where
#define AS_WARN_WHERE  adl_warn_where

  PRINTF_LIKE (adl_bad);
  PRINTF_LIKE (adl_fatal) ATTRIBUTE_NORETURN;
  PRINTF_LIKE (adl_tsktsk);
  PRINTF_LIKE (adl_warn);
  PRINTF_WHERE_LIKE (adl_bad_where);
  PRINTF_WHERE_LIKE (adl_warn_where);


  void md_big_number_to_chars (char *buf, unsigned* val, int n);

  // Inserts a value into a specific operand location.
  void insert_value(unsigned *, uint64_t, const adl_operand *);
  void insert_value(unsigned *, uint64_t, const adl_operand *, const adl_operand_macro *);  //
  void insert_value_simple(unsigned *, uint64_t, unsigned, unsigned, unsigned); //

  // Inserts an operand, which is based upon an expression function, into an
  // instruction.
  void insert_modifier(unsigned *, const adl_operand *, expressionS *, uint32_t);
  void insert_modifier(unsigned *, const adl_operand *, expressionS *, uint32_t, const adl_operand_macro *); //

  // Ugh- this should be a std::shared_ptr, but we have to have simple types in
  // the fixup type, so we can't use a complex object there.  So, we have to
  // have our own hacked up version here.
  struct shared_expr {
    shared_expr(expressionS *expr,unsigned count) : _count(1)
    {
      _ptr = new expressionS[count];
      memcpy(_ptr, expr, sizeof(expressionS)*count);
    }

    expressionS *get() { return _ptr; };
    
    void incr() { ++_count; };

    bool release()
    {
      --_count;
      if (!_count) {
        delete [] _ptr;
        return true;
      }
      return false;
    }

  private:
    unsigned     _count;
    expressionS *_ptr;
  };


namespace adl {

  // Stores fixup information during the first pass.
  struct adl_fixup
  {
    expressionS exp;
    int is_relative;
    unsigned                  instr_size;     // Instruction size, in bytes.
    const adl_operand        *operand;        // The operand to be fixed-up.
    adl_opcode               *opcode;         // Info about all operands (maybe 0 if not needed). // Mutable for macro instruction
    shared_expr              *operand_values; // All other operands, to handle
                                              // modifiers (may be 0, if not
                                              // needed).  This is shared
                                              // amongst all fixups for the
                                              // instruction, in order to be
                                              // able to share values for
                                              // complex modifier functions.
    int                       operand_arg;    // For instructions with modifier
                                              // functions, this is the index of
                                              // the operand_values array which
                                              // needs the fixup so that the
                                              // modifier functions can be
                                              // evaluated.
    int line_number;                          // The PC, essentially.
    int order;                                // If sorting of fixups is required.  Propagated from InstrInfo::order.
    bfd_reloc_code_real_type reloc;
    bool                     md_extra;        // Relocation needs machine-dependent extra handling.
    adl_operand_macro        *operand_macro;  // The related operand in the macro instruction
    bool is_macro;                            //
  };

  // Stores information about an argument from the assembly input.
  struct Arg {
    char *str;
    int len;

    Arg() : str(0), len(0) {};
    Arg(char *s,int l) : str(s), len(l) {};
    bool empty() const { return len == 0; };
    unsigned size() const { return len; };
    std::string to_str() const { return std::string(str,len); };
  };
  
  typedef std::vector<Arg> Args;

  // Used by aliases which define an assembler action function.  This provides
  // an interface for querying and creating instruction arguments.
  class InstrArg {
  public:
    InstrArg ();
    InstrArg (offsetT v);
    InstrArg (const expressionS &ex,const std::string &arg);

    bool has_reloc() const;
    std::string get_reloc() const;
    void set_reloc(const std::string &r);

    void copy_expr(expressionS *trg) const;
    
    offsetT  value() const { return _ex.X_add_number; };
    void set_value(offsetT v) { _ex.X_add_number = v; };

    const std::string &arg() const { return _arg; };
    
    bool is_constant() const { return _ex.X_op == O_constant; };
    bool is_register() const { return _ex.X_op == O_register; };
    bool is_symbol() const { return _ex.X_op == O_symbol; };
    
  private:
    expressionS  _ex;  // Original expression.
    std::string  _arg; // Argument string- used for setting new relocations.
  };  
  
  struct InstrArgs : public std::vector<InstrArg> {};

  // Instruction info used during the first pass.
  struct InstrInfo {
    std::string str;                 // Argument string.
    adl_opcode *opcode;              // Pointer to opcode data.  If a shorthand,
                                     // then this is the shorthand instantiation
                                     // of the target, with all fixed fields set
                                     // in the ocpode.
    adl_opcode *src_opcode;          // Pointer to source opcode data if shorthand.
    expressionS *operand_values;     // Parsed data values corresponding to the
                                     // arguments.  This can be larger than the
                                     // number of arguments if we have expressions.
    unsigned num_opv;                // Number of operand value items.
    std::vector<std::string> args;   // Original string value, corresponding to operand_values.
    const char *file_name;
    int line_number;
    unsigned pc;                     // An attempt at tracking the program counter (per byte).
    int num_fixups;
    int instr_table;
    const char *instr_table_name;
    fragS *frag;                     // Pointer to frag object associated with
                                     // this instruction.  The 'f' pointer will
                                     // point to the actual frag buffer space.
    char *f;                         // Pointer to frag/buf for storing assembled
                                     // data.  Must be allocated after any
                                     // re-ordering takes place of instructions.
    int order;                       // Indicates packet order.  Initially set
                                     // to same as index in InstrBundle, but
                                     // since re-ordering can take place, this
                                     // preserves an idea of what initial
                                     // ordering was.

    static std::string empty_string;
    adl_operand_macro *operand_macro;// Pointer to array of adl_operand_macro structures
    bool is_macro;                   // Flag shows whether the instruction is macro one or not, by default, false
    
    // Remember to initialize, and process the members in copy constructor and assignment operator accordingly

    InstrInfo() :
      opcode(0), 
      operand_values(0),
      num_opv(0),
      file_name(0),
      line_number(-1), 
      pc(0),
      num_fixups(0), 
      instr_table(0), 
      instr_table_name(0),
      frag(0),
      f(0),
      order(-1),
      operand_macro(0),
      is_macro(false)
    {
    }
    
    InstrInfo(const InstrInfo &x) {     // Copy constructor enhanced
      str = x.str;
      // opcode = x.opcode;
      src_opcode = x.src_opcode;

      num_opv = x.num_opv;
      if (x.num_operand_values()) {
        operand_values = (expressionS*)xmalloc(sizeof(expressionS)*x.num_operand_values());
        memcpy(operand_values, x.operand_values, sizeof(expressionS)*x.num_operand_values());
      } else { operand_values = 0; }

      args = x.args;
      file_name = x.file_name;
      line_number = x.line_number;
      pc = x.pc;
      num_fixups = x.num_fixups;
      instr_table = x.instr_table;
      instr_table_name = x.instr_table_name;
      frag = x.frag;
      f = x.f;
      order = x.order;
      is_macro = x.is_macro;
      
      
      if (x.is_macro) {                    // It is a macro instruction
        opcode = (adl_opcode*)xmalloc(sizeof(adl_opcode));  // The opcode is "dynamic"
        memcpy(opcode, x.opcode, sizeof(adl_opcode));
        
        if (x.opcode->operands) {               //
            opcode->operands = (adl_operand*)xmalloc(sizeof(adl_operand)*x.opcode->num_operands);
            memcpy(opcode->operands, x.opcode->operands, sizeof(adl_operand)*x.opcode->num_operands);        
        } else { opcode->operands = 0; }
        
        if (x.operand_macro) {
          operand_macro = (adl_operand_macro*)xmalloc(sizeof(adl_operand_macro)*x.num_operand_values());
          memcpy(operand_macro, x.operand_macro, sizeof(adl_operand_macro)*x.num_operand_values()); 
        } else { operand_macro = 0; }   
      } else { 
        opcode = x.opcode;
        operand_macro = 0;
      }                

    }
    
    friend void swap(InstrInfo& x, InstrInfo& y) // no-throw swap
    {
        using std::swap; 
        swap(x.str, y.str);
        swap(x.opcode, y.opcode);
        swap(x.src_opcode, y.src_opcode);
        swap(x.operand_values, y.operand_values);
        swap(x.num_opv, y.num_opv);
        swap(x.args, y.args);
        swap(x.file_name, y.file_name);
        swap(x.line_number, y.line_number);
        swap(x.pc, y.pc);
        swap(x.num_fixups, y.num_fixups);
        swap(x.instr_table, y.instr_table);
        swap(x.instr_table_name, y.instr_table_name);
        swap(x.frag, y.frag);
        swap(x.f, y.f);
        swap(x.order, y.order);
        swap(x.operand_macro,y.operand_macro);
        swap(x.is_macro, y.is_macro);
    }
    
    const InstrInfo & operator= (InstrInfo x) { // Assignment Operator with copy-and-swap idiom
      swap(*this, x);
      return *this;
    }   
    
    ~InstrInfo() {              // Destructor
      free(operand_values);     // It is OK to free a null pointer

      if (is_macro) {           // It is a macro instruction
        assert(opcode);
        free(opcode->operands); // It is OK to free a null pointer 
        free(opcode);           // opcode points to the "dynamic" copy, so free it
      }
      
      free(operand_macro);      // It is OK to free a null pointer

    }
   
    void alloc_operand_values(int p_num_operand_values);
    void alloc_args(int n);
    void set_opcode(adl_opcode *p_opcode,const char *caller);

    unsigned num_operand_values() const { return num_opv; };
    
    void init(char *p_s, adl_opcode *p_opcode,adl_opcode *p_src_opcode,expressionS *p_operand_values,
              const Args &args, const char *p_file_name, int p_line_number, unsigned p_pc, int p_maxfields, 
              int p_instr_table, const char *p_instr_table_name);

    const char *instrName() const { return (opcode) ? opcode->name : ""; };

    // Return true if the item has the specified attribute.
    bool instrHasAttr(unsigned attr) const;
    // Return true if an instruction has an attribute and that attribute has a give
    // numerical value.
    bool instrHasAttrVal(unsigned attr,unsigned val) const;
    // Return true if an instruction has an attribute and that attribute has a
    // give string value.
    bool instrHasAttrVal(unsigned attr,const std::string &val) const;

    // Return a relocation name (abbreviation) if it exists for the specified
    // argument.
    std::string getReloc(unsigned index) const;
    // Replace a relocation with a new relocation.  Returns true if the
    // replacement happened, false otherwise.
    bool replaceReloc(unsigned index,const std::string &new_reloc);

    // Fetch the raw argument string for a given operand.
    const std::string &getRawArg(unsigned i) const { return (i < args.size()) ? args[i] : empty_string; };

    // Return the numerical value of an operand.
    offsetT getOpValue(unsigned i) const;

    // Return an instruction argument object, which encapsulates information
    // about the numerical value, symbols, relocations, etc.
    InstrArg getArg(unsigned i) const;
    
    unsigned numInstrBlks() const { return (opcode) ? opcode->num_blks : 0; };
    bool instrBlk(int blk) const { 
      if (opcode) {
        for (unsigned i = 0; i != opcode->num_blks; ++i) {
          if (blk == opcode->blks[i]) {
            return true;
          }
        }
      }
      return false;
    };
    void setInstrOrder(int o) { order = o; };
    int instrOrder() const { return order; };
    unsigned size() const { assert(opcode); return opcode->size; };
    unsigned width() const { assert(opcode); return opcode->width; }; // Return instruction width in bits


  };

  // A packet of instructions.  Index 0 refers to the prefix, if there is one, otherwise
  // to the first instruction.
  struct InstrBundle : public std::vector<InstrInfo> {
    InstrBundle() : _has_prefix(0), _pfx_index(0) {};

    InstrInfo &get_instr(unsigned i) 
    {
      if (_has_prefix) {
        if (i == 0) {
          return _prefix;
        } else {
          return at(i-1);
        }
      } else {
        return at(i);
      }
    }
    const InstrInfo &get_instr(unsigned i) const { return const_cast<InstrBundle&>(*this).get_instr(i); };

    InstrInfo &get_prefix() { return _prefix; };
    const InstrInfo &get_prefix() const { return _prefix; };

    unsigned group_size() const { return _has_prefix ? size()+1 : size(); };

    bool has_prefix() const { return _has_prefix; };
    void set_has_prefix(bool p) { _has_prefix = p; };

    // This saves necessary prefix information into the assembler prefix
    // storage.
    void set_prefix_fields();

    unsigned prefix_index() const { return _pfx_index; };
    void set_prefix_index(unsigned p) { _pfx_index = p; };

    const std::vector<std::string> &separators() const { return _separators; };

  protected:
    friend struct InstrBundles;
    
    void add_separator(const std::string &sep) { _separators.push_back(sep); };
    
    bool                     _has_prefix;
    unsigned                 _pfx_index;  // Where to insert the prefix in the packet.  0 => beginning.
    InstrInfo                _prefix;     // Stores prefix information, if applicable.
    std::vector<std::string> _separators; // Separator strings used by this packet.
  };

  // A group of packets (of instructions).
  struct InstrBundles : public std::vector<InstrBundle> {
    InstrBundles() : _current_group(0) {};

    void init(unsigned i) {
      resize(i);
      _current_group = 0;
    }

    bool empty(unsigned group) { if (group >= size()) return true; return at(group).empty(); };

    // Get an item relative to the current group.
    InstrInfo &get_instr(unsigned group,unsigned pos) { return at(group).get_instr(pos); };
    const InstrInfo &get_instr(unsigned group,unsigned pos) const { return const_cast<InstrBundles&>(*this).get_instr(group,pos); };

    // Get an arbitrary instruction.
    InstrInfo &get_instr(unsigned pos) { return at(_current_group).get_instr(pos); };
    const InstrInfo &get_instr(unsigned pos) const { return const_cast<InstrBundles&>(*this).get_instr(pos); };

    // Get the prefix.
    InstrInfo &get_prefix(unsigned group) { return at(group).get_prefix(); };
    const InstrInfo &get_prefix(unsigned group) const { return const_cast<InstrBundles&>(*this).get_prefix(group); };

    // Prefix for current group.
    InstrInfo &get_prefix() { return at(_current_group).get_prefix(); };
    const InstrInfo &get_prefix() const { return const_cast<InstrBundles&>(*this).get_prefix(); };

    InstrInfo &add()
    {
      InstrBundle &packet = at(_current_group);
      packet.push_back(InstrInfo());
      InstrInfo &last = packet.back();
      return last;
    }

    void add_separator(const std::string &sep)
    {
      InstrBundle &packet = at(_current_group);
      packet.add_separator(sep);
    }
  
    unsigned current_group() const { return _current_group; };
    void set_current_group(unsigned i)  { _current_group = i; };
  protected:
    unsigned _current_group;
  };

}

//
// Various helper functions.
//

// Given an instruction object, this replaces it with the named instruction and
// returns the new item.  The number of operands must match.
adl::InstrInfo replaceInstr(const adl::InstrInfo &instr,const std::string &new_name);

// Returns an instruction object which references the named instruction.
// Ugh:  We should be using variadic templates, but there are issues with MSVC and gcc dialects.
adl::InstrInfo createInstr(const std::string &new_name, const adl::InstrArgs &args);
adl::InstrInfo createInstr(const std::string &new_name);
adl::InstrInfo createInstr(const std::string &new_name,const adl::InstrArg &arg0);
adl::InstrInfo createInstr(const std::string &new_name,const adl::InstrArg &arg0,const adl::InstrArg &arg1);
adl::InstrInfo createInstr(const std::string &new_name,const adl::InstrArg &arg0,const adl::InstrArg &arg1,const adl::InstrArg &arg2);
adl::InstrInfo createInstr(const std::string &new_name,const adl::InstrArg &arg0,const adl::InstrArg &arg1,const adl::InstrArg &arg2,const adl::InstrArg &arg3);
adl::InstrInfo createInstr(const std::string &new_name,const adl::InstrArg &arg0,const adl::InstrArg &arg1,const adl::InstrArg &arg2,const adl::InstrArg &arg3,const adl::InstrArg &arg4);
adl::InstrInfo createInstr(const std::string &new_name,const adl::InstrArg &arg0,const adl::InstrArg &arg1,const adl::InstrArg &arg2,const adl::InstrArg &arg3,const adl::InstrArg &arg4,const adl::InstrArg &arg5);


// Returns an instruction object which references the named instruction
// Combine micro instructions into a macro one 
adl::InstrInfo combineInstr(int num_args, const std::string &new_name, ...);

// Returns an instruction object which references the named instruction 
// Patch a macro instruction at the 32 LSBs with a micro one
adl::InstrInfo patchInstr(const adl::InstrInfo &macro, const adl::InstrInfo &micro, int shift);

// Gain access to a prior packet.  Only valid of queue_offset is non-zero.  The
// index is an offset from the current position.
adl::InstrBundle &getPriorPacket(unsigned index);

// Returns true if we have a label defined at this point.
bool hasLabel();

// Adjust a current label by the specified offset.
void adjustCurLabel(int offset);

// Figure out if we need a prefix instrution and save it.  Called automatically,
// but may be called by the user if additional checks are required.
void savePrefix(adl::InstrBundle &);

// Function pointer to pre-asm hook
typedef void(*adl_pre_asm_t)(int);

// Function pointer to post-asm hook
typedef void(*adl_post_asm_t)(unsigned,int);

// Function pointer to post-packet-asm hook
typedef void(*adl_post_packet_asm_t)(adl::InstrBundle &,int);

// Function pointer to post-packet-checker hook
typedef void(*adl_post_packet_checker_t)(adl::InstrBundle &,int);

// Function for reseting prefix counters.
typedef void (*adl_reset_prefix_counters_t)();

void adl_setup_endianness(bool big_endian);

// Performs setup operation- called by md_begin.
void adl_setup_general(const char *min_terminating_chars,const char *terminating_chars, bool is_parallel, adl_pre_asm_t pre_asm, 
                       adl_post_asm_t post_asm, adl_post_packet_asm_t post_packet_asm,adl_post_packet_checker_t post_packet_chekcer,
                       unsigned q_size,unsigned q_offset,bool show_warnings,bool big_mem,int min_instr_len);

void adl_setup_comments(const char **comment_strs, int num_comment_strs, 
                        const char **line_comment_strs, int num_line_comment_strs);
void adl_setup_grouping(const char *packet_begin_chars, const char *packet_end_chars);
void adl_setup_instr_separators(const char *instr_separators);

void adl_setup_instrfields(struct adl_field *fields, int num_fields,
                           struct adl_name_pair *fields_by_index, int num_by_index);

void adl_setup_prefix(struct adl_prefix_fields *pfx_fields, int num_pfx_fields, 
                      struct adl_instr* prefix_instrs,int num_prefix_instrs,
                      adl_reset_prefix_counters_t reset_prefix_counters);

struct hash_control *adl_setup_instructions (struct adl_instr *instructions,
                                             int num_instructions, struct adl_field *operands);
struct hash_control *adl_setup_name_hash (const struct adl_name_pair *names,int num_names,const char *msg);

struct hash_control *adl_setup_asm_instructions (struct adl_asm_instr *instructions,int num_instructions);

struct hash_control *adl_setup_instr_pfx_fields (const char *field_values[],int num_items);

void adl_setup_relocations(const reloc_howto_type *relocs,int num_relocs,
                           const adl_reloc_name *relocs_by_index,int num_relocs_by_index,
                           const adl_int_pair *reloc_offsets,int num_reloc_offsets);

void adl_setup_finish();

// Returns true if custom relocations are defined.
bool adl_have_custom_relocs();

// Returns true if this is a parallel architecture.
bool adl_parallel_arch();

// Lookup a relocation by name, returning the type code.
std::pair<bfd_reloc_code_real_type,bool> adl_get_reloc_by_name(const std::string &str,expressionS *ex,const struct adl_operand *operand);

// Given a type code, return a pointer to the object, or 0 if a bad relocation
// type.
const reloc_howto_type *adl_reloc_type_lookup(bfd *abfd, bfd_reloc_code_real_type code);

// Given a relocation type code, return the offset (within the instruction) to
// use for this offset.
unsigned adl_reloc_offset(bfd_reloc_code_real_type code,const struct adl_operand *);

// ADL:  Maps howto type to special setter functions, action code, etc.
typedef void (*reloc_setter)(unsigned *,unsigned,int,bfd_uint64_t);
typedef bfd_uint64_t (*reloc_action)(bfd_uint64_t,int);
extern "C" void adl_get_reloc_funcs(unsigned type,reloc_setter *setter,reloc_action *action,int *size,int *inner_size,int *offset);


// These are functions which the main code expects to be implemented by an
// assembler-dependent file.
void handle_itable_change(const char* table_name, int instr_table);
void save_instr(adl::InstrBundles &instrInfos,char *s,adl_opcode *op,adl_opcode *src,expressionS *,
                const adl::Args &args,const char *file_name,int line_number,unsigned pc,int,int,const char *,
                const std::string &eoi_str);
void alloc_instr_buf(adl::InstrInfo &info);
void write_instr(char *f, unsigned *instr,int instr_sz,fragS *frag_n, uint32_t addr_mask);
void handle_fixups(char *f,fragS *curr_frag,adl::adl_fixup *fixups,int num_fixups, int instr_sz);
segT parse_expr(expressionS *,char*,int,bool &);
extern "C" void free_buffer(unsigned x);


// These are variables defined in the assembler-independent portion which may be used by the 
// assembler-dependent portion.
extern const char **line_comment_strs;
extern int num_line_comment_strs;
extern int current_table;
extern adl_instr *prefix_instrs;
extern int num_prefix_instrs;
extern unsigned max_prefix_size;
extern unsigned shift_for_prefix_size;
extern char *prefix_frag;
extern adl::InstrBundles instrInfos;
extern int num_rule_errors;
extern std::string adl_asm_version;


#endif
