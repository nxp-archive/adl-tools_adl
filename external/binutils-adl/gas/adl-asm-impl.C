//
// Machine independent functions
//

#ifndef _MSC_VER
extern "C" {
# include "as.h"
}
#endif

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <algorithm>
#include <string>

extern "C" {
# include "as.h"
# include "xregex.h"
# include "safe-ctype.h"
# ifdef OBJ_ELF
#  include "dwarf2dbg.h"
# endif
}

#include "adl-asm-impl.h"
#include "adl-asm-info.h"

using namespace std;
using namespace adl;

static void init_line_assemble(void) {};

extern "C" {

	// Characters which are used to indicate an exponent in a floating point
	// number.  Can be overridden in a user library.
  extern const char EXP_CHARS[] __attribute__((weak)) = "eE";

  // Characters which mean that a number is a floating point constant, as in
	// 0d1.0.  Can be overridden in a user library.
  extern const char FLT_CHARS[] __attribute__((weak)) = "dD";

  // Default short options.  If overridden in a user library, then append the
  // default arguments using the DEFAULT_SHORT_OPTS string.
	const char *md_shortopts __attribute__((weak)) = DEFAULT_ADL_SHORT_OPTS;

  // Default long options.  If overridden in a user library, then append the
  // default arguments using the DEFAULT_LONG_OPTS macro.
	extern const struct option md_longopts[] __attribute__((weak)) = {
    DEFAULT_ADL_LONG_OPTS
	};
	extern const size_t md_longopts_size __attribute__((weak)) = sizeof (md_longopts);

  /* DWARF2 minimum instruction length. */
  int dwarf2_line_min_insn_length;

	const char *EMPTY_STRING = "";  
}

// Flag to indicate whether to check assembler rules.
static bool check_asm_rules = true;

// Governs whether to check low bits in a value with the mask, or ignore this
// check.  Ignoring it is mainly just for debug purposes, when a user might have
// invalid values in some fields due to legacy tests, etc.
static bool check_low_bits = true;


// Flag to indicate assembler rule violations
int num_rule_errors = 0;

// This string stores the set of characters we use to terminate the
// instruction-name token.  The minimal set does not include characters also
// seen in instruction names.
static const char *terminating_chars     = 0;
static const char *min_terminating_chars = 0;

// Here we store comment definitons that are not single characters (e.g. "//");
static const char **comment_strs = 0;
static int num_comment_strs = 0;

// The same for line comments (valid only at the beginning of the line).
const char **line_comment_strs = 0;

int num_line_comment_strs = 0;

// Packet grouping characterss
static const char *instr_separators   = 0;
static const char *packet_begin_chars = 0;
static const char *packet_end_chars   = 0;
static const char *default_packet_sep = 0;
static const char *expected_end = 0;

static unsigned queue_size = 1;
static unsigned queue_offset = 0;

static bool adl_show_warnings = false;
static bool adl_try_longest_instr = false; // --big-mem

// Prefix fields
struct adl_prefix_fields *prefix_fields ATTRIBUTE_UNUSED = 0; 
static int  num_prefix_fields ATTRIBUTE_UNUSED = 0;

// Prefix instructions
struct adl_instr *prefix_instrs = 0;
int num_prefix_instrs = 0;
unsigned max_prefix_size = 0;

// Line label symbol:  This is a vector so that we can handle more than one symbol on the same line.
typedef vector<symbolS*> SymbolVect;
static SymbolVect curr_labels;

// Pre assembler handler, before each instruction is assembled, used to force encoding
static adl_pre_asm_t pre_asm_handler  ATTRIBUTE_UNUSED = 0;

// Post assembler handler, after each instruction updates prefix fields
static adl_post_asm_t post_asm_handler  ATTRIBUTE_UNUSED = 0;

// Post packet assembler handler, after each instructions packet
static adl_post_packet_asm_t post_packet_asm_handler  ATTRIBUTE_UNUSED = 0;

// Post packet rule checker, after each packet
static adl_post_packet_checker_t post_packet_rule_checker ATTRIBUTE_UNUSED = 0;

// For reseting prefix counters.
static adl_reset_prefix_counters_t reset_prefix_counters_handler ATTRIBUTE_UNUSED = 0;

// Stores all instructions indexed by ADL name.
static struct hash_control *all_instrs_hash = 0;

static struct adl_name_pair *fields_by_index     = 0;
static int                   num_fields_by_index = 0;

static struct adl_field     *all_fields          = 0;
static int                   num_fields          = 0;

// Pointers to installed information about relocations, if relevant.
static const reloc_howto_type     *relocs              = 0;
static const adl_reloc_name       *relocs_by_index     = 0;
static const adl_int_pair         *reloc_offsets       = 0;
static int                         num_relocs          = 0;
static int                         num_relocs_by_index = 0;
static int                         num_reloc_offsets   = 0;

void adl_set_check_low_bits(bool c)
{
  check_low_bits = c;
}

void adl_set_check_asm_rules(bool c) 
{
  check_asm_rules = c;
}

// Handle multichar comments here.
static void handle_comments (char *str) {
  int i;
  for (i = 0 ;i != num_comment_strs; ++i) {
    const char *comm = comment_strs[i];
    char *p;
    if ((p = strstr(str,comm))) {
      *p = '\0';
      return;
    }
  }
}

// Stores the current instruction table index
int current_table ATTRIBUTE_UNUSED = 0; // TBD use it

extern void s_switch (int arg);

static const pseudo_typeS potable[] = {
  {"switch", s_switch, 0},
  {NULL, NULL, 0} /* End sentinel.  */
};  

typedef struct {
  int field_id;
  int iface_id;
  int value;
  const char *enum_value;
} PreSetInfo;

static PreSetInfo   PreSetInfos[MAX_INSTR_FIELDS];
static int PreSetInfoCtr = 0;

string InstrInfo::empty_string("");

InstrBundles instrInfos;                   // Holds all instruction information 

static bool parallel_arch = false;         // True is we have parallel architecture
static bool assembler_fields = false;      // True if we have assembler fields
static bool prefix_saved = false;          // True:  savePrefix has been called.

static unsigned curgrp() { return instrInfos.current_group(); }

static size_t find_reloc_start(const string &str)
{
  return str.find_first_of('@');
}

// Given a string like foo@hi or foo@hi+20, this returns the 'hi' and the '20'
// as a pair.
static pair<string,string> get_reloc_string(const string &str)
{
  pair<string,string> r;
  
  // We have relocations installed, so look this one up.  We isolate
  // everything to the right of the @, e.g. for foo@bar, we want "bar".
  size_t pos = find_reloc_start(str);
  if (pos == string::npos) {
    return r;
  }

  for (string::const_iterator i = str.begin()+pos+1; i != str.end() && (ISIDNUM(*i) || *i == '@' || *i == '_'); ++i) {
    r.first.push_back(*i);
  }
  
  size_t add_pos = str.find_first_of("-+",pos);

  if (add_pos != string::npos) {
    r.second = str.substr(add_pos);
  }

  return r;
}

// Return true if an instruction has a given attribute.
bool InstrInfo::instrHasAttr(unsigned attr) const
{
  if (!opcode || !opcode->attrs) return false;

  return (opcode->attrs->attrs & (1ULL << attr));
}

// Return true if an instruction has an attribute and that attribute has a give
// numerical value.
bool InstrInfo::instrHasAttrVal(unsigned attr,unsigned val) const
{
  if (!opcode || !opcode->attrs) return false;

  // Make sure the attribute exists and some values exist.
  if (!(opcode->attrs->attrs & (1ULL << attr)) || !opcode->attrs->vals) return false;

  // Now search the array
  const adl_instr_attr_val *v = opcode->attrs->vals;
  while (v->attr) {
    if ( v->attr == (1ULL << attr) && !v->str && v->num == val) return true;
    ++v;
  }
  return false;
}

// Return true if an instruction has an attribute and that attribute has a given
// string value.
bool InstrInfo::instrHasAttrVal(unsigned attr,const string &val) const
{
  if (!opcode || !opcode->attrs) return false;

  // Make sure the attribute exists and some values exist.
  if (!(opcode->attrs->attrs & (1ULL << attr)) || !opcode->attrs->vals) return false;

  // Now search the array
  const adl_instr_attr_val *v = opcode->attrs->vals;
  while (v->attr) {
    if ( v->attr == (1ULL << attr) && v->str && val == v->str) return true;
    ++v;
  }
  return false;
}

void InstrInfo::alloc_operand_values(int p_num_operand_values)
{
  free(operand_values);
  num_opv = p_num_operand_values;
  if (p_num_operand_values) {
    operand_values = (expressionS*)xmalloc(sizeof(expressionS)*p_num_operand_values);
    memset(operand_values,0,sizeof(expressionS)*p_num_operand_values);
  } else {
    operand_values = 0;
  }
}

void InstrInfo::alloc_args(int n)
{
  args.resize(n);
}

// This changes the opcode and resizes the operand_values array and args vector,
// if necessary.  If this is a shorthand, the alias_target opcode is used.  This
// only works for simple aliases which map to a single target.
void InstrInfo::set_opcode(adl_opcode *p_opcode,const char *caller)
{
  if (p_opcode->alias_targets) {
    if (p_opcode->num_alias_targets > 1) {
      AS_FATAL(_("%s:  Multi-instructin alias targets (%s) are not allowed."),caller,p_opcode->name);
    }
    opcode = &p_opcode->alias_targets[0];
    src_opcode = p_opcode;
  } else {
    // Not a shorthand.
    opcode = p_opcode;
  }
  if (opcode->num_operands > num_operand_values()) {
    expressionS *new_opvals = (expressionS*)xmalloc(sizeof(expressionS)*opcode->num_operands);
    memset(new_opvals,0,sizeof(expressionS)*opcode->num_operands);
    if (operand_values) {
      memcpy(new_opvals,operand_values,sizeof(expressionS)*num_operand_values());
    }
    free(operand_values);
    operand_values = new_opvals;
    num_opv = opcode->num_operands;
    args.resize(opcode->num_operands);
  }
}

// Return a relocation as a string, given an index into our argument array.
string InstrInfo::getReloc(unsigned index) const
{
  if (index < args.size()) {
    pair<string,string> p = get_reloc_string(args[index]);

    return p.first;
  } else {
    return string();
  }
}

int64_t InstrInfo::getOpValue(unsigned index) const
{
  if (index < args.size()) {
    return operand_values[index].X_add_number;
  } else {
    return 0;
  }
}

InstrArg InstrInfo::getArg(unsigned index) const
{
  if (index < args.size()) {
    return InstrArg(operand_values[index],args[index]);
  } else {
    return InstrArg();
  }
}

// Replace a relocation with a new relocation name.  Returns true if the
// replacement occurred, false if it didn't.  The argument must already have a
// relocation in order for this to work.
bool InstrInfo::replaceReloc(unsigned index,const string &new_reloc)
{
  if (index < args.size()) {
    pair<string,string> p = get_reloc_string(args[index]);

    if (p.first.empty()) return false;

    // We know this will work, since we already did it to get the reloc info.
    size_t pos = find_reloc_start(args[index]);

    args[index] = args[index].substr(0,pos) + "@" + new_reloc + p.second;

    return true;
  } else {
    return false;
  }
}

void InstrInfo::init(char *p_s, adl_opcode *p_opcode,adl_opcode *p_src_opcode,expressionS *p_operand_values,
                     const Args &raw_args, const char *p_file_name,int p_line_number, unsigned p_pc,
                     int p_maxfields, int p_instr_table, const char *p_instr_table_name)
{
  str = p_s;
  opcode = p_opcode;
  src_opcode = p_src_opcode;

  int max_arg = raw_args.size();
  int op = 0;

  for (unsigned i = 0; i != max_arg; ++i) {
    args.push_back(string(raw_args[i].str,raw_args[i].len));
  }

  // Add on any extras for assembler-only fields.  Assembler fields are fields
  // which are modified outside of the instruction itself, by various hooks.  We
  // have to provide storage for that, so we do so via operand_values.  The
  // arguments must match the size of the array, so we add on empty strings
  args.resize(args.size() + opcode->num_asm_operands);

  alloc_operand_values(max(opcode->num_exprs,((src_opcode) ? src_opcode->num_exprs : 0)) + opcode->num_asm_operands);
  memcpy(operand_values,p_operand_values,sizeof(expressionS) * num_operand_values());

  file_name   = p_file_name;
  line_number = p_line_number;
  pc          = p_pc;
  instr_table = p_instr_table;
  instr_table_name = p_instr_table_name;
}

// For a given bundle, set prefix data for each instruction.
void InstrBundle::set_prefix_fields()
{
  // Reset all counters.
  (reset_prefix_counters_handler)();

  // Iterate over all instructions.  If a prefix setter exists, then run it.
  int index = (_has_prefix) ? 1 : 0;
  for (iterator iter = begin(); iter != end(); ++iter,++index) {
    struct adl_operand *operands = iter->opcode->operands;
    for (int j=0; j < iter->opcode->num_operands; ++j) {    
      struct adl_operand &operand = operands[j];
      if (operand.ptr->pfx_setter) {
        expressionS *ex = &iter->operand_values[operand.arg];
        // If a prefix-counter function exists, then query it for the index to
        // use.
        int pindex = (iter->opcode->prefix_counter) ? (iter->opcode->prefix_counter)(operand.ptr->id,index) : index;
        // Then set the value.
        (operand.ptr->pfx_setter)(ex->X_add_number,pindex,curgrp());
      }
    }
    // Then adjust any counters, if necessary.
    if (iter->opcode->mod_prefix_counters) {
      (iter->opcode->mod_prefix_counters)();
    }
  }
}

InstrArg::InstrArg()
{
  memset(&_ex,0,sizeof(expressionS));
}

InstrArg::InstrArg(offsetT v)
{
  memset(&_ex,0,sizeof(expressionS));
  _ex.X_add_number = v;
  _ex.X_op = O_constant;
}

InstrArg::InstrArg(const expressionS &ex,const string &arg) :
  _ex(ex),
  _arg(arg)
{
}

// Returns true if the argument has a relocation.
bool InstrArg::has_reloc() const
{
  size_t x = _arg.find('@');
  return (x != string::npos);
}

string InstrArg::get_reloc() const
{
  size_t x = _arg.find('@');
  return (x != string::npos) ? _arg.substr(x) : string();
}

// We can ignore the symbol name, because it will already be set in the
// expression object.  So, all we need to do is specify the relocation name with
// '@' in front of it.
void InstrArg::set_reloc(const string &r)
{
  _arg = "@" + r;
}

void InstrArg::copy_expr(expressionS *trg) const
{
  memcpy(trg,&_ex,sizeof(expressionS));
}


// Reset prefix fields values to defaults.
static void reset_prefix(int grp) {
  int i;
  struct adl_prefix_field **pfx_fields = prefix_fields[grp].fields;
  for (i = 0; i != num_prefix_fields; ++i) {
    pfx_fields[i]->value = pfx_fields[i]->default_value;
  }
}

// This must be called *early* so that we set up the output file properly.
void adl_setup_endianness(bool big_endian)
{
  target_big_endian = big_endian;
}

// General purpose setup stuff.
void adl_setup_general(const char *mtc,const char *tc, bool is_parallel, adl_pre_asm_t pre_asm, adl_post_asm_t post_asm, 
                       adl_post_packet_asm_t post_packet_asm,adl_post_packet_checker_t post_packet_checker, 
                       unsigned q_size, unsigned q_offset, bool show_warnings, bool big_mem, int min_instr_len)
{
  adl_show_warnings = show_warnings;
  adl_try_longest_instr = big_mem;
  terminating_chars = tc;
  min_terminating_chars = mtc;
  parallel_arch = is_parallel;
  dwarf2_line_min_insn_length = min_instr_len;
   
  pre_asm_handler = pre_asm;
  post_asm_handler  = post_asm;
  post_packet_asm_handler = post_packet_asm;
  post_packet_rule_checker = post_packet_checker;
  // Add in the .switch pseudo-op so that we can switch instruction
  // tables in the middle of a file.
  pop_insert(potable); // TBD 
  queue_size = q_size;
  queue_offset = q_offset;
  instrInfos.init(q_size);
}

void adl_setup_comments(const char **cs, int ncs, const char **lcs, int nlcs) 
{
  comment_strs = cs;
  num_comment_strs = ncs;
  line_comment_strs = lcs;
  num_line_comment_strs = nlcs;
}

void adl_setup_instr_separators(const char *instr_separators_p)
{
  instr_separators = instr_separators_p;
}

void adl_setup_grouping( const char *pbc, const char *pec)
{
  packet_begin_chars = pbc;
  packet_end_chars   = pec;
  int i = strlen(packet_begin_chars);
  if (i >= 1 && (packet_begin_chars[i-1] == packet_end_chars[i-1])) { // Add support for a single "\n" as packet grouping symbol
    default_packet_sep = &packet_begin_chars[i-1]; 
  } else {
    default_packet_sep = 0; 
  }
}

void adl_setup_prefix(struct adl_prefix_fields *pfx_fields, int num_pfx_fields, struct adl_instr* pfx_instrs ATTRIBUTE_UNUSED,
                      int num_pfx_instrs ATTRIBUTE_UNUSED,adl_reset_prefix_counters_t reset_prefix_counters)
{
  prefix_fields = pfx_fields;
  num_prefix_fields = num_pfx_fields;
  prefix_instrs  = pfx_instrs;
  num_prefix_instrs = num_pfx_instrs;
  int i;
  for (i = 0; i != num_prefix_instrs; ++i) {
    struct adl_opcode* opcode = &(prefix_instrs[i].opcodes[0]);
    if (opcode->size > max_prefix_size) {
      max_prefix_size = opcode->size;
    }
  }
  for(i=0; i != queue_size; ++i) {
    reset_prefix(i);
  }
  reset_prefix_counters_handler = reset_prefix_counters;
}

bool adl_parallel_arch()
{
  return parallel_arch;
}

// This process .switch pseudo-ops.  The argument is passed to a call-back, which
// should then process it.
void s_switch(int argc ATTRIBUTE_UNUSED) // TBD
{
  const int BufSize = 500;
  char arg[BufSize];
  char *s;

  int n = 0;

  if (*input_line_pointer++ != '-') { // TBD
    AS_FATAL(_("Improperly formed .switch directive."));
  }

  // Get the command.
  int c = *input_line_pointer++;

  // Now get the argument.
  for (s = input_line_pointer;
       ((!is_end_of_line[(unsigned char) *input_line_pointer] && *input_line_pointer != ' ' && *input_line_pointer != '\t')); 
       input_line_pointer++,n++);
  strncpy(arg,s,n);
  arg[n] = 0;
  md_parse_option(c,arg);
  demand_empty_rest_of_line();
}

struct hash_control *adl_setup_asm_instructions (struct adl_asm_instr *instructions,int num_instructions)
{
  struct hash_control *instr_hash;
  instr_hash = hash_new(); 

  // Now populate it.
  {
    struct adl_asm_instr *instr  = instructions;
    struct adl_asm_instr *instr_end = instructions + num_instructions;

    for ( ; instr != instr_end; ++instr) {
      // Insert the instruction.
      const char *rc = hash_insert(instr_hash,instr->name,(PTR)instr);
      if (rc != NULL) {
        AS_FATAL(_("Internal assembler error for instruction %s:  %s"),instr->name,rc);
      }
    }
  }

  return instr_hash;
}

struct hash_control *adl_setup_instr_pfx_fields (const char *field_values[],int num_items)
{
  struct hash_control *field_hash;
  field_hash = hash_new(); 

  // Now populate it.
  {
    const char **fv     = field_values;
    const char **fv_end = field_values + num_items;

    for ( ; fv != fv_end; ++fv) {
      // Insert the instruction.
      const char *rc = hash_insert(field_hash,*fv,(void*)1);
      if (rc != NULL) {
        AS_FATAL(_("Internal assembler error for instruction-prefix-field value %s:  %s"),*fv,rc);
      }
    }
  }

  return field_hash;
}

// This installs the enumerated fields values hash and the by-index array used
// for error/warning messages.
void adl_setup_instrfields(struct adl_field *fields,int num_fields_p,struct adl_name_pair *fields_by_index_p, int num_fields_by_index_p) 
{
  all_fields          = fields;
  fields_by_index     = fields_by_index_p;
  num_fields          = num_fields_p;
  num_fields_by_index = num_fields_by_index_p;
}

// Stores any relocation information.
void adl_setup_relocations(const reloc_howto_type *relocs_p,int num_relocs_p,
                           const adl_reloc_name *relocs_by_index_p,int num_relocs_by_index_p,
                           const adl_int_pair *reloc_offsets_p,int num_reloc_offsets_p)
{
  relocs          = relocs_p;
  num_relocs      = num_relocs_p;

  relocs_by_index     = relocs_by_index_p;
  num_relocs_by_index = num_relocs_by_index_p;

  reloc_offsets     = reloc_offsets_p;
  num_reloc_offsets = num_reloc_offsets_p;
}

bool adl_have_custom_relocs()
{
  return num_relocs != 0;
}

static void setup_operands(adl_opcode *op,adl_field *operands)
{
  for (int f = 0; f != op->num_operands; ++f) {
    if (op->operands[f].index < num_fields) {
      op->operands[f].ptr = &operands[op->operands[f].index];
    }
    if (operands[op->operands[f].index].assembler || operands[op->operands[f].index].pfx_setter) {
      assembler_fields = true;
    }
  }
}

// This installs the opcodes into the instruction hash.
struct hash_control *adl_setup_instructions(struct adl_instr *instructions,int num_instructions,
                                            struct adl_field *operands)
{
  struct hash_control *instr_hash;
  regex_t *rec;
  int reg_err;

  if (!all_instrs_hash) {
    all_instrs_hash = hash_new();
  }

  // Create the hash.
  instr_hash = hash_new();

  // Now populate it.
  {
    struct adl_instr *instr     = instructions;
    struct adl_instr *instr_end = instructions + num_instructions;

    for ( ; instr != instr_end; ++instr) {
      // Insert the instruction.
      const char *rc = hash_insert(instr_hash,instr->name,(PTR)instr);
      if (rc != NULL) {
        AS_FATAL(_("Internal assembler error for instruction %s: %s"),instr->name,rc);
      }
      // For each opcode-entry associated with this instruction name,
      // perform any needed setup.
      {
        struct adl_opcode *op = instr->opcodes;
        struct adl_opcode *op_end = instr->opcodes + instr->num_opcodes;
        for ( ; op != op_end; ++op) {

          // Insert entry into the all-instructions-hash.  Duplicates may exist
          // if multiple tables reference the same instruction, so ignore an
          // 'exists' error.
          const char *rc = hash_insert(all_instrs_hash,op->name,(PTR)op);

          // Compile the regular expression.
          rec = (regex_t*)xmalloc(sizeof(regex_t)); 
          if (!rec) {
            AS_FATAL(_("Error in allocating memory for regex of '%s'"),instr->name);
          }
          reg_err = regcomp (rec,op->regex,REG_EXTENDED);

          if (reg_err != 0) {
            const int BufSize = 80;
            char msg[BufSize];
            regerror (reg_err, rec, msg, BufSize);
            regfree (rec);
            AS_FATAL(_("Error in compiling regex for %s (%s): %s"),instr->name,op->regex,msg);
          }
          op->regcomp = rec;

          // Initialize the operand arrays.
          setup_operands(op,operands);
          for (int t_iter = 0; t_iter != op->num_alias_targets; ++t_iter) {
            setup_operands(&op->alias_targets[t_iter],operands);
          }
        }
      }
    }
  }

  return instr_hash;
}

// This installs register names into the register name hash.
struct hash_control *adl_setup_name_hash(const struct adl_name_pair *names,int num_names,const char *msg)
{
  struct hash_control *hash;

  // Create the hash.
  hash = hash_new();

  // Now populate it.
  {
    const struct adl_name_pair *n;
    const struct adl_name_pair *n_end = names + num_names;

    for (n = names; n != n_end; ++n) {
      // Insert the value.
      const char *rc = hash_insert(hash,n->name,(PTR)n);
      if (rc != NULL) {
        AS_FATAL(_("Internal assembler error for %s %s:  %s"),msg,n->name,rc);
      }
    }
  }

  return hash;
}

// Call a user-defined final-setup callback.
void adl_setup_finish()
{
  acb_setup_finish(all_instrs_hash,all_fields,num_fields,relocs,num_relocs);
}

bool namePairComp(const adl_name_pair &a,const adl_name_pair &b) { return a.index < b.index; };

// Given a field index, return a useful name, or 0 if not found.
static const char *get_field_name(unsigned index)
{
  adl_name_pair tmp;
  tmp.name  = 0;
  tmp.index = index;
  pair<adl_name_pair*,adl_name_pair*> p = equal_range(&fields_by_index[0],&fields_by_index[num_fields_by_index],tmp,namePairComp);
  if (p.first != p.second) {
    return p.first->name;
  } else {
    return 0;
  }
}

// Same as above, but always returns something.  If not found, returns "unknown".
static const char *get_field_name_safe(unsigned index)
{
  if (const char *n = get_field_name(index)) {
    return n;
  } else {
    return "<unknown>";
  }
}

static void set_constant(expressionS *expr, offsetT value)
{
  expr->X_op = O_constant;
  expr->X_add_number = value;
  expr->X_add_symbol = NULL;
  expr->X_op_symbol = NULL;
}

template <typename T>
struct NameComp {
  bool operator()(const T &x,const T &y) {
    return strcmp(x.name,y.name) < 0;
  }
  bool operator()(const T &x,const char *y) {
    return strcmp(x.name,y) < 0;
  }
  bool operator()(const char *x,const T &y) {
    return strcmp(x,y.name) < 0;
  }
};

// Generic binary search.  Expects the object to have a name member of const
// char *.  Returns a pair of pointers to the first/last+1 of matching elements,
// or (0,0) if not found.
template <typename T>
pair<T *,T*> bfind_p(T *array,unsigned num,const char *arg)
{
  T tmp;
  tmp.name = arg;
  pair<T*,T*> p = equal_range(array,&array[num],tmp,NameComp<T>());
  if (p.first != p.second) {
    return p;
  } else {
    return make_pair((T*)0,(T*)0);
  }
}

// This just returns the first element of a match.
template <typename T>
T *bfind(T *array,unsigned num,const char *arg)
{
  return bfind_p(array,num,arg).first;
}


// Generic binary search.  Expects the object to have a name member of const
// char *.  Returns a pointer to the item, or 0 if not found.  Hack: I want this
// to be a constant comparison, but that's difficult with using a length, since
// only the input has an extent, but the data array contains normal 0-terminated
// strings.  So, temporarily, I 0-terminate the input.
template <typename T>
T *bfind_n(T *array,unsigned num,const char *arg,int len)
{
  T val;
  val.name = arg;
  char tmp = arg[len];
  ((char*)arg)[len] = 0;
  pair<T*,T*> p = equal_range(array,&array[num],val,NameComp<T>());
  ((char*)arg)[len] = tmp;
  if (p.first != p.second) {
    return p.first;
  } else {
    return 0;
  }
}

// Looks up an enumerated field for a corresponding value and then stores that in the expression.
static bool find_enum_option(expressionS *expr,const char *arg,enum_fields *enums)
{
  if (enums) {
    const enum_field  *ef = bfind<enum_field>(enums->enums,enums->num,arg);
    if (ef) {
      set_constant(expr,ef->value);
      return true;
    }
  }
  return false;
}

static bool find_enum_option(expressionS *expr,const Arg &arg,enum_fields *enums)
{
  if (enums) {
    const enum_field *ef = bfind_n<enum_field>(enums->enums,enums->num,arg.str,arg.len);
    if (ef) {
      set_constant(expr,ef->value);
      return true;
    }
  }
  return false;
}

static expressionS *get_enum_expr(enum_fields **enums,int field_id,int iface_id,InstrInfo &info) 
{
  struct adl_opcode *opcode = info.opcode;
  if (!opcode) AS_FATAL(_("Empty InstrInfo object:  No opcode associated with this instruction item."));
  expressionS *ex = 0;
  int i;
  for (i=0; i != opcode->num_operands; ++i) {
    const struct adl_operand *operand = &opcode->operands[i];
    const struct adl_field *field = operand->ptr;
    if (field && (field->iface_id == field_id || field->iface_id == iface_id) && field->enums != NULL) {
      ex = &(info.operand_values[operand->arg]);
      *enums = field->enums;
      return ex;
    }
  }
  return ex;
}

static expressionS *get_enum_expr(enum_fields **enums,int field_id,int iface_id,int pos,int group) 
{
  return get_enum_expr(enums,field_id,iface_id,instrInfos.get_instr(group,pos));
}

// get the expression corresponding to a field_id, in instruction pos at group gr
static expressionS *get_expr(int field_id,int iface_id,InstrInfo &info) 
{
  struct adl_opcode *opcode     = info.opcode;
  struct adl_opcode *src_opcode = info.src_opcode;
  if (!opcode) AS_FATAL(_("Empty InstrInfo object:  No opcode associated with this instruction item."));
  expressionS *ex = 0;
  if (src_opcode) {
    // This is a shorthand, so we use the source opcode for field data.
    for (int i=0; i != src_opcode->num_operands; ++i) {
      const adl_operand &operand = src_opcode->operands[i];
      if (operand.index == field_id || operand.index == iface_id) {
        ex = &(info.operand_values[operand.arg]);
      }
    }
  } else {
    for (int i=0; i != opcode->num_operands; ++i) {
      const adl_operand &operand = opcode->operands[i];
      const adl_field &field = *operand.ptr;
      if (field.iface_id == field_id || field.iface_id == iface_id) {
        ex = &(info.operand_values[operand.arg]);
      }
    }
  }
  return ex;
}

// get the expression corresponding to a field_id, in instruction pos at group gr
static expressionS *get_expr(int field_id,int iface_id,int pos,int group) 
{
  InstrInfo &info = instrInfos.get_instr(group,pos);
  return get_expr(field_id,iface_id,info);
}

int adl_get_instr_position(const InstrInfo &ii,int group)
{
  return (&ii - &instrInfos[group][0]);
}

int adl_get_current_group(void)
{
  return curgrp();
}

int adl_group_size(int group) 
{
  return instrInfos[group].group_size();
}

int adl_get_current_pos()
{
  return adl_group_size(curgrp()) - 1;
}

bool adl_query_field(int field_id,int iface_id,const InstrInfo &info)
{
  return get_expr(field_id,iface_id,const_cast<InstrInfo&>(info));
}

void  adl_set_field(int field_id,int iface_id,int value,InstrInfo &info)
{
  // save_instr incremented the group counter // group counter or instr counter inside the group?
  expressionS* ex = get_expr(field_id,iface_id,info);
  
  if (ex) {
    set_constant(ex,value);
  } else {
    AS_BAD(_("Bad field access:  Field %s does not exist for this instruction."),get_field_name_safe(field_id));
  }
}

void  adl_set_field(int field_id,int iface_id,int value,int pos,int group)
{
  adl_set_field(field_id,iface_id,value,instrInfos.get_instr(group,pos));
}


void adl_set_enum_field(int field_id,int iface_id,const char *value,InstrInfo &info)
{
  enum_fields *enums = 0;
  expressionS* ex = get_enum_expr(&enums,field_id,iface_id,info);
  
  if (ex) {
    if (!find_enum_option(ex,value,enums)) {
      AS_BAD(_("Unknown enumeration value for field %s."),get_field_name_safe(field_id));
    }
  } else {
    AS_BAD(_("Bad field access:  Field %s does not exist for this instruction."),get_field_name_safe(field_id));
  }
}

void adl_set_enum_field(int field_id,int iface_id,const char *value,int pos,int group)
{
  adl_set_enum_field(field_id,iface_id,value,instrInfos.get_instr(group,pos));
}

unsigned  adl_get_field(int field_id,int iface_id,const InstrInfo &info)
{
  expressionS* ex = get_expr(field_id,iface_id,const_cast<InstrInfo&>(info));
  if (ex) {
    if (ex->X_op == O_constant) {
      return ex->X_add_number; 
    } else {
      AS_BAD(_("Bad field access:  Field %s does not contain a constant."),get_field_name_safe(field_id));
    }
  } else {
    AS_BAD(_("Bad field access:  Field %s does not exist for this instruction."),get_field_name_safe(field_id));
  }

  return -1;
}

unsigned  adl_get_field(int field_id,int iface_id,int pos,int group)
{
  return adl_get_field(field_id,iface_id,instrInfos.get_instr(group,pos));
}


void  *adl_get_dest(int pos, int group) 
{
  InstrInfo &info = instrInfos.get_instr(group,pos);
  struct adl_opcode *opcode = info.opcode;
  if (!opcode) AS_FATAL(_("Empty InstrInfo object:  No opcode associated with this instruction item."));

  if (opcode->dests) {
    return (*opcode->dests)();
  }
  return 0;
}

void adl_set_prefix_field_slice(struct adl_prefix_field *pfield, int index, int value)
{
  unsigned shift = pfield->field_width*index;
  if (target_big_endian) {
    uint32_t mask  = (pfield->mask << shift);
    pfield->value = ((value << shift) & mask) | (pfield->value & ~mask);
  } else {
    uint32_t mask  = (pfield->mask >> shift);
    pfield->value = ((value << shift) & mask) | (pfield->value & ~mask);
    
  }
}

bool adl_pre_set_field(int field_id,int iface_id, int value)
{
  PreSetInfos[PreSetInfoCtr].field_id = field_id;
  PreSetInfos[PreSetInfoCtr].iface_id = iface_id;
  PreSetInfos[PreSetInfoCtr].value = value;
  PreSetInfos[PreSetInfoCtr].enum_value = 0;
  
  ++PreSetInfoCtr;
  return true;
}

bool adl_pre_set_enum_field(int field_id,int iface_id,const char *enum_value)
{
  PreSetInfos[PreSetInfoCtr].field_id = field_id;
  PreSetInfos[PreSetInfoCtr].iface_id = iface_id;
  PreSetInfos[PreSetInfoCtr].value = 0;
  PreSetInfos[PreSetInfoCtr].enum_value = enum_value;
  
  ++PreSetInfoCtr;
  return true;
}

InstrBundle &getPriorPacket(unsigned index)
{
  return instrInfos[(curgrp() + index) % queue_size];
}

symbolS *curr_label()
{
  return (curr_labels.empty()) ? 0 : curr_labels.back();
}

// Returns true if we have a label at this point.
bool hasLabel()
{
  return !curr_labels.empty();
}

void adjustCurLabel(int offset)
{
  if (!curr_labels.empty()) {
    for (SymbolVect::iterator i = curr_labels.begin(); i != curr_labels.end(); ++i) {
      S_SET_VALUE(*i, S_GET_VALUE(*i) + offset);
    }
  }
}

// Given an instruction object, this replaces it with the named instruction and
// returns the new item.  The number of operands must match.
InstrInfo replaceInstr(const InstrInfo &instr,const string &new_name)
{
  // Find the new instruction.
  struct adl_opcode *new_op = (struct adl_opcode *) hash_find(all_instrs_hash,new_name.c_str());
  
  if (new_op) {
    InstrInfo new_instr(instr);
    new_instr.set_opcode(new_op,"replaceInstr");
    return new_instr;
  } else {
    AS_FATAL(_("replaceInstr:  No instruction named %s"),new_name.c_str());
    return InstrInfo();
  }
}

// Returns an instruction object which references the named instruction.
InstrInfo createInstr(const string &new_name, const InstrArgs &args)
{
  // Find the new instruction.
  struct adl_opcode *new_op = (struct adl_opcode *) hash_find(all_instrs_hash,new_name.c_str());

  if (new_op) {
    InstrInfo new_instr;

    new_instr.alloc_operand_values(args.size());
    new_instr.alloc_args(args.size());
    new_instr.set_opcode(new_op,"createInstr");
    new_instr.src_opcode = 0;		// No shorthand for explicitly created instructions

    unsigned i = 0;
    for (; i != args.size(); ++i) {
      const InstrArg &arg = args[i];
      arg.copy_expr(&new_instr.operand_values[i]);
      new_instr.args[i] = arg.arg();
    }
    
    if (!instrInfos[curgrp()].empty()) {
      new_instr.file_name   = instrInfos[curgrp()][0].file_name;
      new_instr.line_number = instrInfos[curgrp()][0].line_number;
      new_instr.pc          = instrInfos[curgrp()][0].pc;
    }
    return new_instr;
  } else {
    AS_FATAL(_("createInstr:  No instruction named %s"),new_name.c_str());
    return InstrInfo();
  }
}

//
// createInstr implementation details.
//

InstrInfo ATTRIBUTE_UNUSED createInstr(const string &new_name)
{
  InstrArgs instr_args;
  return createInstr(new_name,instr_args);
}

InstrInfo ATTRIBUTE_UNUSED createInstr(const string &new_name,const InstrArg &arg0)
{
  InstrArgs instr_args;
  instr_args.push_back(arg0);
  return createInstr(new_name,instr_args);
}

InstrInfo createInstr(const std::string &new_name,const InstrArg &arg0,const InstrArg &arg1)
{
  InstrArgs instr_args;
  instr_args.push_back(arg0);
  instr_args.push_back(arg1);
  return createInstr(new_name,instr_args);
}

InstrInfo createInstr(const std::string &new_name,const InstrArg &arg0,const InstrArg &arg1,const InstrArg &arg2)
{
  InstrArgs instr_args;
  instr_args.push_back(arg0);
  instr_args.push_back(arg1);
  instr_args.push_back(arg2);
  return createInstr(new_name,instr_args);
}
  
InstrInfo createInstr(const std::string &new_name,const InstrArg &arg0,const InstrArg &arg1,const InstrArg &arg2,const InstrArg &arg3)
{
  InstrArgs instr_args;
  instr_args.push_back(arg0);
  instr_args.push_back(arg1);
  instr_args.push_back(arg2);
  instr_args.push_back(arg3);
  return createInstr(new_name,instr_args);
}
  
InstrInfo createInstr(const std::string &new_name,const InstrArg &arg0,const InstrArg &arg1,const InstrArg &arg2,const InstrArg &arg3,const InstrArg &arg4)
{
  InstrArgs instr_args;
  instr_args.push_back(arg0);
  instr_args.push_back(arg1);
  instr_args.push_back(arg2);
  instr_args.push_back(arg3);
  instr_args.push_back(arg4);  
  return createInstr(new_name,instr_args);
}

InstrInfo createInstr(const std::string &new_name,const InstrArg &arg0,const InstrArg &arg1,const InstrArg &arg2,const InstrArg &arg3,const InstrArg &arg4,const InstrArg &arg5)
{
  InstrArgs instr_args;
  instr_args.push_back(arg0);
  instr_args.push_back(arg1);
  instr_args.push_back(arg2);
  instr_args.push_back(arg3);
  instr_args.push_back(arg4);  
  instr_args.push_back(arg5);  
  return createInstr(new_name,instr_args);
}

// Retrieve a value from an unsigned pointer (this could be 64-bit), then right
// justify the result.
uint64_t get_microop_value(unsigned *opc,unsigned width)
{
  uint64_t opc_value = opc[0];

  if (width <= 32) {
    opc_value = opc_value >> (32 - width);
  } else {
    if (target_big_endian) {
      opc_value <<= 32;
      opc_value |= opc[1];
    } else {
      opc_value |= ((uint64_t)opc[1] << 32);
    }
    opc_value = opc_value >> (64 - width);
  }
  
  return opc_value;
}

void set_microop_value(unsigned *data,uint64_t x,unsigned width)
{
  if (width <= 32) {
    data[0] = x;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
  } else if (target_big_endian) {
    data[0] = (x >> 32);
    data[1] = (x & 0xffffffff);
  } else {
    data[0] = (x & 0xffffffff);
    data[1] = (x >> 32);
  }
}

// Returns an instruction object which references the named instruction.
// Combine micro instructions into a macro one
// The fields of the macro instruction must be all instruction-type 
// The total number of fields of macro instruction should be equal to that of arguments. 
// Use "-1" as an argument to indicate an empty field
InstrInfo combineInstr(int num_args, const string &name, ...)
{
    InstrInfo instr;
    
    // Find the new instruction, opcode_macro should be kept as it is, for it is "static" for all similar formats
    adl_opcode *opcode_macro = (adl_opcode *) hash_find(all_instrs_hash,name.c_str());
    
    if (!opcode_macro ||  opcode_macro->is_shorthand()) {
      AS_FATAL(_("combineInstr:  No real instruction is named %s"), name.c_str());
      return InstrInfo();
    }
    
    if (opcode_macro->num_operands != num_args) {
      AS_FATAL(_("combineInstr: The number of arguments is invalid: %d"), num_args);
      return InstrInfo();
    }    

    // Create a "dynamic" copy of opcode, so we can patch it
    adl_opcode *opcode = (adl_opcode*)malloc(sizeof(adl_opcode));  // Create a dynamic copy
    if (opcode) {
        memcpy(opcode, opcode_macro, sizeof(adl_opcode));
    } else {
      AS_FATAL(_("combineInstr: Unable to combine instructions"));
      return InstrInfo();
    }
    
    int total_num_operands = 0;
    int total_num_exprs = 0;
    int total_num_operand_values = 0;
    va_list ap;
    int instr_ord;
    
    va_start(ap, name);
    for (int i = 0; i != num_args; ++i) { // The 1st round deals with opcode and calcualte the number of operands
        instr_ord = va_arg(ap, int);
       
        if ((instr_ord < -1) || (instr_ord >= int(instrInfos[curgrp()].size()))) {  // int() type-conversion is required!
            AS_FATAL(_("combineInstr: The specified instruction index %d is invalid"), instr_ord);
            return InstrInfo();
        }
        
        if (instr_ord == -1) continue;
        
        InstrInfo *xx = &instrInfos[curgrp()][instr_ord];
        
        if (xx->opcode->width >= opcode_macro->width) {
            AS_FATAL(_("combineInstr: The width of the micro instruction, %d, is greater than or equal to that of the macro one, %d"), 
                        xx->opcode->width, opcode_macro->width);
            return InstrInfo();
        } 
       
        total_num_operands += xx->opcode->num_operands;
        total_num_exprs += xx->opcode->num_exprs;
        if ((xx->num_operand_values() != 1) || (xx->operand_values[0].X_op != 0)) {
          total_num_operand_values += xx->num_operand_values();
        }

        // Limit to 64-bits for now.  We should generalize this later.
        uint64_t opcode_val = get_microop_value(xx->opcode->opcode,xx->opcode->width);
        const adl_operand * opr = &opcode_macro->operands[i];
        
        // Inserts a value using the inserter function for a specified field.
        // Patch the opcode at opcode, which is "dynamic", so mutable
        insert_value(opcode->opcode, opcode_val, opr);
    }
    va_end(ap);
  
    
    instr.is_macro = true;
    
    // Set the line number as that of 1st micro instruction in the group; 
    // Usually all micro instructions are in the same line anyway
    instr.file_name   = instrInfos[curgrp()][0].file_name;
    instr.line_number = instrInfos[curgrp()][0].line_number;
    instr.pc          = instrInfos[curgrp()][0].pc;

    opcode->num_operands = total_num_operands;
    opcode->num_exprs = total_num_exprs;

    instr.alloc_operand_values(total_num_operand_values); // num_operand_values is set inside alloc_operand_values()

    instr.opcode = opcode;
    instr.src_opcode = 0;		// No shorthand for explicitly created macro instructions
    
    // Allocate for opcode->operands
    if (total_num_operands) {
        opcode->operands = (adl_operand*)malloc(sizeof(adl_operand)*total_num_operands);
        if (!opcode->operands) {
            AS_FATAL(_("combineInstr: Unable to combine micro instructions"));
            return InstrInfo();
        }
    } else { 
        opcode->operands = 0;
    }

    // Allocate for instr.operand_macro
    if (total_num_operand_values) {
        instr.operand_macro = (adl_operand_macro*)malloc(sizeof(adl_operand_macro)*total_num_operand_values);
        if (!instr.operand_macro) {
            AS_FATAL(_("combineInstr: Unable to combine micro instructions"));
            return InstrInfo();
        }
    } else { 
        instr.operand_macro = 0;
    }    
    
    instr.args.clear();
    int op_val_index = 0;
    int operand_index = 0;
    int opv_base = 0;
    va_start(ap, name);
    for (int i = 0; i != num_args; ++i) {  // The 2nd round fills in the operands, etc.
        instr_ord = va_arg(ap, int);
        
        if (instr_ord == -1) continue;

        InstrInfo *xx = &instrInfos[curgrp()][instr_ord];
        
        if ((xx->num_operand_values() != 1) || (xx->operand_values[0].X_op != 0)) {
          for (int j = 0; j != xx->num_operand_values(); ++j, ++op_val_index) {
              expressionS * opv = &xx->operand_values[j];
              memcpy(&instr.operand_values[op_val_index], opv, sizeof(expressionS));
              instr.operand_macro[op_val_index].width = xx->opcode->width;
              instr.operand_macro[op_val_index].operand = &opcode_macro->operands[i];
              instr.operand_macro[op_val_index].max_width = opcode_macro->max_width;
              instr.operand_macro[op_val_index].instr_width = opcode_macro->width;
              instr.operand_macro[op_val_index].shift = 0;
              
          }
        }
        
        for (int j = 0; j != xx->args.size(); ++j) {
            instr.args.push_back(xx->args[j]); 
        }        
        
        for (int j = 0; j != xx->opcode->num_operands; ++j, ++operand_index) {
            adl_operand * src = &xx->opcode->operands[j];
            adl_operand * trg = &opcode->operands[operand_index];
            memcpy(trg, src, sizeof(adl_operand));            
            if (trg->arg >= 0) {
              // Adjust the argument appropriately so that it takes the correct
              // value from the complete list of arguments that we've assembled.
              trg->arg += opv_base;
            }
            if (trg->modifier_srcs) {
              trg->opv_base = opv_base;
            }
        }
        
        opv_base += xx->num_operand_values();
        
    }
    va_end(ap);
   
  
    return instr;
}

// Returns an instruction object which references the named instruction.
// Patch a macro instruction at one of its instruction-type fields with a micro instruction
// The patched instruction-type fields of macro instruction should be continuous
// The width of macro instruction should be greater than that of micro ones
// The width of micro instruction should be less than or equal to 32 bits
// The patch area should be limited within the 64 LSBs of the macro instruction
// The micro instruction should not be a macro instruction by itself
// Note that the operand values of the returned instruction may include some O_illeagl ones
InstrInfo patchInstr(const InstrInfo &macro, const InstrInfo &micro, int shift)
{
   
    if (micro.is_macro) {
        AS_FATAL(_("patchInstr: The specified patch instruction is invalid"));
        return InstrInfo();
    }  
    
   if (micro.opcode->width + shift >= 64) {
        AS_FATAL(_("patchInstr: The specified instruction patch location %d is invalid"), shift);
        return InstrInfo();
    }         

    if (micro.opcode->width >= macro.opcode->width) {
        AS_FATAL(_("patchInstr: The width of the micro instruction, %d, is greater than or equal to that of the macro one, %d"), 
                    micro.opcode->width, macro.opcode->width);
        return InstrInfo();
    } 
    
    InstrInfo instr;
    
    // Create a "dynamic" copy of macro.opcode, so we can patch it
    adl_opcode *opcode = (adl_opcode*)malloc(sizeof(adl_opcode));  // Create a dynamic copy
    if (opcode) {
        memcpy(opcode, macro.opcode, sizeof(adl_opcode));
    } else {
      AS_FATAL(_("patchInstr: Unable to patch instruction"));
      return InstrInfo();
    }
    
    int total_num_operands = macro.opcode->num_operands + micro.opcode->num_operands;
    int total_num_operand_values = macro.num_operand_values();
    if ((micro.num_operand_values() != 1) || (micro.operand_values[0].X_op != 0)) {    
      total_num_operand_values += micro.num_operand_values();
    }
    

		// Patch the opcode of macro instruction with that from micro instruction
    unsigned * opc = micro.opcode->opcode;
    //    unsigned opc_value = (target_big_endian) ? opc[0] : opc[opcode->max_width/32-1]; // Thanks to the width limit of 32 bits
    unsigned opc_value = opc[0];
    opc_value = opc_value >> (32 - micro.opcode->width);
    insert_value_simple(opcode->opcode, opc_value, macro.opcode->max_width, macro.opcode->width, shift);  // Patch the opcode at opcode, which is "dynamic", so mutable
   
    instr.is_macro = true;
    
    // Set the line number as that of the macro instruction 
    instr.file_name   = macro.file_name;
    instr.line_number = macro.line_number;
    instr.pc          = macro.pc;

    opcode->num_operands = total_num_operands;
    opcode->num_exprs = macro.opcode->num_exprs + micro.opcode->num_exprs;

    instr.alloc_operand_values(total_num_operand_values); // num_operand_values is set inside alloc_operand_values()

    instr.opcode = opcode;
    instr.src_opcode = macro.src_opcode;		// Keep the src_opcode as it is for the patched instruction
   
    
    // Allocate for opcode->operands
    if (total_num_operands) {
        opcode->operands = (adl_operand*)malloc(sizeof(adl_operand)*total_num_operands);
        if (!opcode->operands) {
            AS_FATAL(_("patchInstr: Unable to patch instruction"));
            return InstrInfo();
        }
    } else { 
        opcode->operands = 0;
    }

    // Allocate for instr.operand_macro
    if (total_num_operand_values) {
        instr.operand_macro = (adl_operand_macro*)malloc(sizeof(adl_operand_macro)*total_num_operand_values);
        if (!instr.operand_macro) {
            AS_FATAL(_("patchInstr: Unable to patch instruction"));
            return InstrInfo();
        }
    } else { 
        instr.operand_macro = 0;
    }    
    
    instr.args.clear();
    int op_val_index = 0;
    int operand_index = 0;

    // Fill in the operands, etc.
    for (int j = 0; j != macro.num_operand_values(); ++j, ++op_val_index) {
        expressionS * opv = &macro.operand_values[j];
        memcpy(&instr.operand_values[op_val_index], opv, sizeof(expressionS));
        
        // Set all the fields for operand_macro
        if (macro.is_macro) {  // If already macro instruction, keep the values
          instr.operand_macro[op_val_index].operand = macro.operand_macro[op_val_index].operand;
          instr.operand_macro[op_val_index].shift = macro.operand_macro[op_val_index].shift;
          instr.operand_macro[op_val_index].width = macro.operand_macro[op_val_index].width;
          instr.operand_macro[op_val_index].max_width = macro.operand_macro[op_val_index].max_width;
          instr.operand_macro[op_val_index].instr_width = macro.operand_macro[op_val_index].instr_width;
        }
        else { 	// Set width == instr_width for operand from macro instruction
								//  The operand is not O_illegal, i.e., not instruction-type, so set operand = 0 for simpler processing
          instr.operand_macro[op_val_index].operand = 0;
          instr.operand_macro[op_val_index].shift = 0;
          instr.operand_macro[op_val_index].width = macro.opcode->width;
          instr.operand_macro[op_val_index].max_width = macro.opcode->max_width;
          instr.operand_macro[op_val_index].instr_width = macro.opcode->width;
        }
    }

    for (int j = 0; j != macro.args.size(); ++j) {
        instr.args.push_back(macro.args[j]); 
    }        
    
    for (int j = 0; j != macro.opcode->num_operands; ++j, ++operand_index) {
        adl_operand * opr = &macro.opcode->operands[j];
        memcpy(&opcode->operands[operand_index], opr, sizeof(adl_operand));
    }
    
    int arg_base = macro.args.size();
    
    if ((micro.num_operand_values() != 1) || (micro.operand_values[0].X_op != 0)) {
      for (int j = 0; j != micro.num_operand_values(); ++j, ++op_val_index) {
          expressionS * opv = &micro.operand_values[j];
          memcpy(&instr.operand_values[op_val_index], opv, sizeof(expressionS));
          
          // Set values for operand from micro instruction
          instr.operand_macro[op_val_index].width = micro.opcode->width;
          instr.operand_macro[op_val_index].operand = 0;
          instr.operand_macro[op_val_index].max_width = macro.opcode->max_width;
          instr.operand_macro[op_val_index].instr_width = macro.opcode->width;
          instr.operand_macro[op_val_index].shift = shift;
      }
    }

    for (int j = 0; j != micro.args.size(); ++j) {
        instr.args.push_back(micro.args[j]); 
    }        
    
    for (int j = 0; j != micro.opcode->num_operands; ++j, ++operand_index) {
        adl_operand * opr = &micro.opcode->operands[j];
        memcpy(&opcode->operands[operand_index], opr, sizeof(adl_operand));
        opcode->operands[operand_index].arg += arg_base;
    }
    

    return instr;
}


extern "C" char *md_atof (int type, char *litp, int *sizep)
{
  return ieee_md_atof (type, litp, sizep, target_big_endian);
}

/* Write a value out to the object file, using the appropriate endianness.  */
extern "C" void md_number_to_chars (char *buf, valueT val, int n)
{
  if (target_big_endian)
    number_to_chars_bigendian (buf, val, n);
  else
    number_to_chars_littleendian (buf, val, n);
}

// For big-endian, we proceed forwards, for little-endian, we start from the
// back and go forwards.  The md_number_to_chars function takes care of the
// byte-swapping of the individual words.
void md_big_number_to_chars (char *buf, unsigned* val, int n)
{
 int ind = 0;
  while (true) {
    if (ind > (MAX_INSTR_WORDS - 1)) {
      AS_BAD(_("Internal assembler error, instruction size exceeded maximal length."));
    }
    if (n <= 4) {
      // Right justify.
      unsigned tmp = val[ind] >> 8*(4-n);
      md_number_to_chars (buf, (valueT)(tmp), (n));
      // We are done.
      break;
    } else {
      md_number_to_chars (buf, (valueT)(val[ind]), 4);
      // Proceed to the next word.
      n -= 4;
      buf +=4;
    }
    // increment array pointer
    ++ind;
  }

}

// Looks up a register by name.
// str:    Register name to find.
// expr:   If found, contains expression information about the register.
// return: true if found, false if not.
static bool find_reg_name (expressionS *expr,struct hash_control *reg_hash,
                           const Arg &arg)
{
  if (!ISALPHA(*(arg.str))) {
    return false;
  }

  {
    const struct adl_name_pair *regname = (const struct adl_name_pair *) hash_find_n(reg_hash,arg.str,arg.len);  

    if (regname) {
      expr->X_op = O_register;
      expr->X_add_number = regname->index;
      expr->X_add_symbol = NULL;
      expr->X_op_symbol = NULL;
      return true;
    }
  }

  return false;
}

// Inserts a 32-bit value into the instruction, make sure the value with shift don't exceed the 64 bit limit
void insert_value_simple(unsigned *instr, uint64_t val, unsigned max_width, unsigned instr_width, unsigned shift)
{

    assert(instr_width <= max_width);
    // assert(instr_width > 32);
    // assert(max_width > 32);
    // assert(shift < 32);
    unsigned displacement = max_width - instr_width;
    unsigned num = max_width/32 - 1;  // max_width is the maximum instruction width round to 32-bits words
    uint64_t bv, bv0, bv1 = 0;  // bv0 stores the LSW
  
    if (target_big_endian) { 
        bv0 = instr[num];
        if (max_width > 32) bv1 = instr[num-1];
    }
    else {
        bv0 = instr[0];
        if (max_width > 32) bv1 = instr[1];
    }

    bv = (bv1 << (32 - displacement)) | (bv0 >> displacement);
    val = val << shift;
    bv |= val;

    if (target_big_endian) { 
        instr[num] = uint32_t(bv << displacement);
        if (max_width > 32) instr[num-1] = uint32_t(bv >> (32 - displacement));
    }
    else {
        instr[0] = uint32_t(bv << displacement);
        if (max_width > 32) instr[1] = uint32_t(bv >> (32 - displacement));
    }
    
}
// Inserts a value using the inserter function for a specified field.
void insert_value(unsigned *instr, uint64_t val, const struct adl_operand *operand)
{
     
  const struct adl_field *field = operand->ptr;
  assert(field);
  // Correct for any shifting and offset.
  val >>= operand->leftshift;
  val -= operand->offset;
  if (operand->checker) {
    (*field->inserter)(instr,(*operand->checker)(val,false));
  } else {
    (*field->inserter)(instr,val);
  }  
}

// [Overloaded for macro instruction] Inserts a value using the inserter function for a specified field 
void insert_value(unsigned *instr, uint64_t val, const struct adl_operand *operand, 
                  const adl_operand_macro * operand_macro) 
{
    unsigned opc[MAX_INSTR_WORDS] = {0}; // Initialized to zero
    insert_value(opc, val, operand);     // Build micro opc

    uint64_t opc_value = get_microop_value(opc,operand_macro->width);

    // If the operand = 0, then use insert_value_simple() instead insert_value().  BK: Is this still relevant?
    if (operand_macro->operand) {
      insert_value(instr, opc_value, operand_macro->operand);  // Insert the micro opc into macro opc    
    } else {
      insert_value_simple(instr, opc_value, operand_macro->max_width, operand_macro->instr_width, operand_macro->shift);
    }
}

// Inserts a value generated via an expression.  To be safe, we clear the value first,
// since it's possible that this may be called twice:  Once, before a fixup, then again,
// once a fixup value is known.
void insert_modifier(unsigned *insn,const struct adl_operand *operand, expressionS *exprs, uint32_t pc)
{
  const struct adl_field *field = operand->ptr;

  assert(field && operand->modifier && exprs);

  uint64_t val = operand->modifier(exprs, pc);

  (*field->clearer)(insn);

  insert_value(insn,val,operand);
}

void clear_value(unsigned *insn,const struct adl_operand *operand)
{
  const struct adl_field *field = operand->ptr;

  assert(field && field->clearer);

  (*field->clearer)(insn);
}

void get_field_value(unsigned *trg,unsigned *src,const struct adl_operand *operand,unsigned width)
{
  const struct adl_field *field = operand->ptr;

  assert(field && field->getter);

  uint64_t v = (*field->getter)(src);

  if (width <= 32) {
    v <<= (32 - width);
  } else {
    v <<= (64 - width);
  }
  
  set_microop_value(trg,v,width);
}

// [Overloaded for macro instruction] Inserts a value generated via an expression
void insert_modifier(unsigned *insn, const struct adl_operand *operand, expressionS *exprs, uint32_t pc, 
                     const adl_operand_macro * operand_macro)
{
    
		if (operand_macro->width < operand_macro->instr_width) {  // It is an operand from micro instruction
			unsigned opc[MAX_INSTR_WORDS];                // Initialized to zero

      get_field_value(opc,insn,operand_macro->operand,operand_macro->width);
			insert_modifier(opc, operand, exprs, pc);     // Modify micro opc

      uint64_t opc_value = get_microop_value(opc,operand_macro->width);
          
			if (operand_macro->operand) {  // If the operand = 0, then use insert_value_simple() instead insert_value()
        clear_value(insn,operand_macro->operand);
        insert_value(insn, opc_value, operand_macro->operand);  // Insert the micro opc into macro opc    
			} else {
        insert_value_simple(insn, opc_value, operand_macro->max_width, operand_macro->instr_width, operand_macro->shift);
			} 		
		}
		else { // It is an operand from macro instruction 
			insert_modifier(insn, operand, exprs, pc);
		}
}


static bool is_whitespace(char c) 
{
  return (c == ' ' || c == '\t'); 
}

static void skip_to_whitespace(char **ptr)
{
  while (**ptr && !is_whitespace(**ptr)) {
    ++(*ptr);
  }
}

static void skip_whitespaces(char **ptr) 
{
  while (**ptr && is_whitespace(**ptr)) {
    ++(*ptr);
  }
}

static void skip_operand(char **ptr) 
{
  while (**ptr && !is_whitespace(**ptr) && **ptr != ',') {
    ++(*ptr);
  }
}

static void skip_operands(char **ptr) 
{
  while (**ptr && !is_whitespace(**ptr)) {
    ++(*ptr);
  }
}

// The idea here is that an instruction generally consists of a name, optional
// abutting flags, whitespice, and operands.  When we come across something we
// don't know, this skips the possible abutting flags, whitespace, and then the
// operands.
static char *skip_to_next(char *ptr)
{
  skip_to_whitespace(&ptr);
  skip_whitespaces(&ptr);
  skip_operands(&ptr);
  return ptr;
}

static bool is_grouping_char(char c) {
  return (strchr(packet_begin_chars,c) || strchr(packet_end_chars,c));
};

// We reject expresstions that use register names.
// It is  "reg+reg" or "reg-reg" expressions
// We also reject expressions of the form "<sym" or ">sym" as it can happane in starcore
//
// TODO:  This code seems a tad odd- do we still need it?
static bool bad_expression(const Arg &arg, struct hash_control *reg_hash) 
{
  char *op = 0;
  char c1, c2;
  bool rc = false;

  // An empty expression is OK.
  if (arg.empty()) return rc;

  // Temporarily null-terminate the string.
  c1 = arg.str[arg.len];
  arg.str[arg.len] = 0;

  if (arg.str && ((arg.str[0] == '<') || (arg.str[0] == '>'))) {
    rc = true;
    goto Done;
  }
  
  if ((op = strchr(arg.str,'+')) || (op = strchr(arg.str,'-'))) {
    c2 = *op;
    *op = '\0';
    if (hash_find_n(reg_hash,arg.str,arg.len)) {
      rc = true;
      goto Done2;
    } 
  
    if (*(op + 1) && hash_find(reg_hash,op+1)) {
      rc = true;
      goto Done2;
    } 
  Done2:
    *op = c2;
  } 
 Done:
  arg.str[arg.len] = c1;
  return rc;
}

static void  clear_preset_info(void)
{
  int i;
  for (i=0; i < PreSetInfoCtr; ++i) {
    PreSetInfos[i].enum_value = 0;
  }
  PreSetInfoCtr = 0;
}
  


static bool has_preset_fields(struct adl_opcode *opcode) 
{
  int i;
  struct adl_operand *operands = opcode->operands;
  for (i=0; i < PreSetInfoCtr; ++i) {
    int j;
    for (j=0; j < opcode->num_operands; ++j) {
      if (operands[j].ptr->iface_id == PreSetInfos[i].iface_id || operands[j].ptr->iface_id == PreSetInfos[i].field_id) {
        break;
      }
    }
    if (j == opcode->num_exprs) {
      return false;
    }
  }
  return true;
}

// If we have relocations installed, then return a pointer to the relocation in
// the array, otherwise fall back to the standard bfd routine.
const reloc_howto_type *adl_reloc_type_lookup(bfd *abfd, bfd_reloc_code_real_type code)
{
  if (relocs) {
    // Currently a linear search, which isn't great...
    for (int i = 0; i != num_relocs; ++i) {
      if (relocs[i].type == code) {
        return &relocs[i];
      }
    }
  }
   
  return bfd_reloc_type_lookup(abfd,code);
}


unsigned adl_reloc_offset(bfd_reloc_code_real_type code,const struct adl_operand *oprnd)
{
  if (reloc_offsets) {
    // Currently a linear search, which isn't great...
    for (int i = 0; i != num_reloc_offsets; ++i) {
      if (reloc_offsets[i].key == code) {
        if (reloc_offsets[i].value >= 0) {
          return reloc_offsets[i].value;
        } else {
          // If we have a value of -1, then it means to use the default method
          // of aligning with the nearest byte.
          break;
        }
      }
    }
  }

  // Default: We can only round to the nearest byte, since we don't have
  // sub-byte precision using binutil's data structures.
  return (oprnd) ? (oprnd->bitpos / 8) : 0;
}

// If we have relocation installed, then try to get the relocation from the
// installed table.  Otherwise, call the hook function.
pair<bfd_reloc_code_real_type,bool> adl_get_reloc_by_name(const string &str,expressionS *ex,const struct adl_operand *operand)
{
  if (relocs_by_index) {

    pair<string,string> p = get_reloc_string(str);
    if (p.first.empty()) {
      return make_pair(BFD_RELOC_UNUSED,false);
    }
    
    pair<adl_reloc_name*,adl_reloc_name*> rp = bfind_p<adl_reloc_name>((adl_reloc_name*)relocs_by_index,num_relocs_by_index,p.first.c_str());
    if (rp.first ) {
      // Now search for something generic or which matches the operand.
      adl_reloc_name *r = rp.first;
      for (adl_reloc_name *rf = rp.first; rf != rp.second; ++rf) {
        if ((rf->field_index < 0) || (rf->field_index == operand->index)) {
          r = rf;
        }
      }      
      
      bfd_reloc_code_real_type reloc = (bfd_reloc_code_real_type)relocs[r->index].type;
      bool md_extra = r->md_extra;
      //  Now check to see if we have an additional expression after the
      //  relocation, e.g. ident@reloc + constant.
      if (!p.second.empty()) {
        char *orig_lp = input_line_pointer;
        expressionS new_exp;

        input_line_pointer = (char*)p.second.c_str();
        expression (&new_exp);
        if (new_exp.X_op == O_constant) {
          ex->X_add_number += new_exp.X_add_number;
        }

        input_line_pointer = orig_lp;
      }

      return make_pair(reloc,md_extra);
    }
    
    return make_pair(BFD_RELOC_UNUSED,false);
  } else {
    return acb_reloc_hook(str,ex,operand);
  }
}

// This deals with setting assembler fields, which are fields that the user sets
// via the assembler, but not directly from assembly input on a per-instruction
// basis.  Since these fields can be set via various hooks which occur before
// the instruction is encountered, we check those values and set the fields if
// necessary.
static bool handle_assembler_fields(struct adl_opcode *opcode, expressionS* operand_values,int start) 
{
  int j;
  struct adl_operand *operands = opcode->operands;
  for (j=start; j < opcode->num_operands; ++j) {
    // set assembler fields from their preset or default values
    const struct adl_operand *operand = &opcode->operands[j];
    if (operand->ptr && operand->ptr->assembler) {
      expressionS *ex = &operand_values[operand->arg];
      int i;
      for (i=0; i < PreSetInfoCtr; ++i) {
        const struct adl_field *field = operand->ptr;
        if (field->iface_id == PreSetInfos[i].field_id || field->iface_id == PreSetInfos[i].iface_id) {
          if (PreSetInfos[i].enum_value != NULL) {
            enum_fields *ef = field->enums;
            if (!ef || !find_enum_option(ex,PreSetInfos[i].enum_value,ef)) {
              return false;
            }
          } else {
            set_constant(ex,PreSetInfos[i].value);
          }
          break;
        }
      }
      if (i == PreSetInfoCtr) {
        set_constant(ex,operands[j].ptr->default_value);
      }
    } 
  } 
  return true;
}


static bool set_default_option(const Arg &arg,int opindex,expressionS *ex,struct adl_opcode *opcode)
{
  if (!opcode->operands[opindex].ptr) return false;
  
  // If it is not an empty string of a field that have default value then throw error
  if (opcode->operands[opindex].ptr->default_value == -1) {
    if (arg.str && (strncmp(arg.str,EMPTY_STRING,arg.len)!= 0)) {
      return false;
    }
  }
  else{
    set_constant(ex,opcode->operands[opindex].ptr->default_value);
  }
  return true;
}

// Swap entries in pmatch array
static void swap_match(Args &args,int i, int j) 
{
  Arg tmp = args[i];
  args[i] = args[j];
  args[j] = tmp;
}

// Handles permutable and prefix fields in syntax.  This then puts the items
// into the proper expression positions so that we can then simply iterate
// through the arguments and not worry about order.
static void reorder_perm_fields(Args &args,struct adl_opcode *opcode) 
{
  if (opcode->num_prm_operands == 0 && opcode->num_pfx_operands == 0) {
    return;
  }

  int  i = 0;//current operand
  int ii = 1;//operand to swap
  for ( ; (i < args.size()) && (i != opcode->p_end); ++i) {
    Arg &arg = args[i];
    // Need to backup
    
    enum_fields *ef = opcode->enums[i];
    
    expressionS ex;
    bfd_boolean found = FALSE;   
    found = find_enum_option(&ex,arg,ef);    
    if( (!found ) && ( ii <= (opcode->p_end-1) ) ){
      swap_match(args, i, ii);     
      i--;
      ii++;
    }
    else{     
      ii = i + 2 ;  // after increment  ii equals to i+1
    }
  }
}


// This transfers argument data from the regular expression data structure to
// the Args array.  We don't actually copy data, but just record actual pointers
// and lengths.  Note that there may be holes in the input arguments due to enumerated fields
// with empty strings.  Therefore, we have to iterate over everything, pushing empty elements on,
// then erase trailing empties.
static bool copy_operands(Args &args,char* &instr_end,struct adl_opcode *opcode,char *s,regmatch_t *pmatch,int maxfields)
{
  // First, add on empty items for any missing prefix fields which weren't used.
  // Any items in 'args' right now represents just what we've found in terms of
  // prefix items.
  for (int i = args.size(); i != opcode->num_pfx_operands; ++i) {
    args.push_back(Arg(s,0)); // Dummy item.
  }
  int last = 0;
  for (int i = 1; i != maxfields+1; ++i) {
    if (pmatch[i].rm_eo < 0) {
      // Empty item:  Push on a placeholder.
      args.push_back(Arg(s,0));
      // We count it if we're still in the perm-field range.  Otherwise, for
      // example, if we just have perm fields, we wouldn't handle it correctly.
      if ( args.size() <= opcode->num_prm_operands) {
        last = args.size();
      }
    } else {
      // Normal item.
      args.push_back(Arg(&s[pmatch[i].rm_so],pmatch[i].rm_eo-pmatch[i].rm_so));
      last = args.size();
    }
  }
  // Now remove trailing empties.
  args.erase(args.begin()+last,args.end());
  instr_end = &s[pmatch[0].rm_eo];

  // Return true if the end of the match is whitespace, eol or eos, or matches
  // against an end-of-packet character.
  return (is_whitespace(*instr_end) || *instr_end == '\n' || *instr_end == 0 || is_grouping_char(*instr_end));
}

static bool process_operands(struct adl_opcode* opcode, Args &args,
                             int maxfields, expressionS* operand_values,
                             struct hash_control *reg_hash, bool &unresolved_symbol)
{
  // Check that preset fields_exists
  if (!has_preset_fields(opcode)) {
    return false;
  }
  reorder_perm_fields(args, opcode);
  unresolved_symbol = false;
  int i = 0;  
  for ( ; i != args.size(); ++i) {
    expressionS *ex = &operand_values[i];

    const Arg &arg = args[i];

    // Are we dealing with an enumerated field?  If so, check it against our allowed list.
    enum_fields *ef = opcode->enums[i];

    if (ef) {
      if(!find_enum_option(ex,arg,ef)){
        // If empy string, then try to set a default value.  Otherwise, produce
        // error due to bad enumerated value.
        if (!arg.empty()) {
          return false;
        }
        if (!set_default_option(arg,i,ex,opcode)) {
          return false;
        }
      }
    } else {
      // Check to see if it's a register name.
      if (!find_reg_name(ex,reg_hash,arg)) {
        // No, so parse as an expression.  The expression parser takes its
        // input from input_line_pointer, rather than an argument, for some reason.
        if (bad_expression(arg,reg_hash)) {
          return false;
        }
        parse_expr(ex,arg.str,arg.len,unresolved_symbol);  // wrapper for expression()
        if (!(ex->X_op == O_constant || ex->X_op == O_symbol || ex->X_op == O_absent)) {
          // At this point, we have to have either a constant or a symbol.  If
          // it's a symbol plus an offset, then the expression parser has
          // already handled that.  Anything else implies some kind of complex
          // expression with a symbol, which probably means an unhandled
          // preprocessor define.
          AS_BAD(_("Bad expression for operand %d: %s"),i+1,arg.to_str().c_str());
        }
      }
    }
    
    // Handle the operand.
    if (ex->X_op == O_illegal) {
      AS_BAD (_("Operand %d:  illegal operand"),i);
    }
    else if (ex->X_op == O_absent) {
      // See if we have a default value, in which case we insert it, then repeat
      // the loop for this argument value and try to apply it to the next
      // operand.
      if (!set_default_option(arg,i,ex,opcode)) {
        AS_BAD (_("Operand %d:  missing operand"),i);
        memset(ex,0,sizeof(expressionS));
      }
    }
  }
  // If we should have seen more arguments, then we might have default values we
  // need to install.
  for (; i < (opcode->num_operands - opcode->num_asm_operands); ++i) {
    Arg arg;
    expressionS *ex = &operand_values[i];
    if (!set_default_option(arg,i,ex,opcode)) {
      AS_BAD (_("Operand %d:  missing operand"),i);
      memset(ex,0,sizeof(expressionS));
    }
  }
  for (int opindex = i; opindex != maxfields; ++opindex) {
    memset((void*)&operand_values[opindex],0,sizeof(expressionS));
  }
  
  // Check for too many operands.
  if (i > opcode->num_operands) {
    return false;
  } else {
    if (assembler_fields) {
      return handle_assembler_fields(opcode,operand_values,0);
    }
    return true;
  }
}

// Do range checking on a constant, based upon an operand's limits.
bool adl_check_range(offsetT value,const struct adl_operand *operand)
{
  // First, check the bounds.
  if (operand->flags & ADL_SIGNED) {
    if ((value < operand->lower) || 
        (value > operand->upper) ) {
      return false;
    }
  } else if (operand->flags & ADL_EXT_SIGNED) {
    // This is the extended range-checking situation.  If upper is 0, then that
    // means we allow a full 64-bit value (since upper has wrapped over the
    // top).
    if ((value < operand->lower) || ((operand->upper != 0) &&
                                     ((unsigned long)abs(value) > operand->upper)) ) {
      return false;
    }
  } else {
    if ( ((unsigned long)value < operand->lower) || 
         ((unsigned long)value > operand->upper) ) {
      return false;
    }
  }
  // Then make sure that the low-bits are clear, according to the mask value.
  if ( check_low_bits && ((operand->mask) && (value & operand->mask) != 0)) {
    return false;
  }

  return true;
}

static bool valid_input(struct adl_opcode *opcode, expressionS *operand_values)
{
  int i;
  for (i = 0; i != opcode->num_operands; ++i) {
    const struct adl_operand *operand = &opcode->operands[i];
    expressionS *ex = &operand_values[operand->arg];

    // Skip if we're dealing with an assembler field, since we didn't get any
    // input from the syntax string.
    if (operand->ptr && operand->ptr->assembler) {
      continue;
    }

    if (operand->validator != NULL) {
      if (!operand->validator(operand_values)) {
        return false;
      }
    }
    // Skip if we're dealing with a modifier function.
    if (operand->modifier) {
      continue;
    }
    
    // Reject if we have a register value but the field is not a
    // register field.
    if (ex->X_op == O_register) {
      if (!(operand->flags & ADL_REGISTER)) {
        return false;
      }
      if (((unsigned long)ex->X_add_number < operand->lower) || 
          ((unsigned long)ex->X_add_number > operand->upper) ) {
        return false;
      }
    }

    // Next, do range checking on the input and reject as necessary.
    if (ex->X_op == O_constant) {
      if (!adl_check_range(ex->X_add_number,operand)) {
        return false;
      }
    }
  }

  // If a cross-operand-value checker function exists, make sure that it passes.
  if (opcode->argchecker) {
    if (! (opcode->argchecker)(operand_values)) {
      return false;
    }
  }

  return true;
}


static void execute_action(struct adl_opcode *opcode, expressionS *operand_values, uint32_t pc, int maxfields,unsigned grp)  
{ 
  DeclArray(struct adl_operand_val,vals,maxfields);

  int i = 0; 
  for( ; i < opcode->num_operands; ++i) { 
    const struct adl_operand *operand = &opcode->operands[i]; 
    expressionS *ex = &operand_values[operand->arg]; 
    if (operand->modifier) { 
      insert_modifier(&vals[i].val,operand,operand_values,pc); 
    } else if (ex->X_op == O_register || ex->X_op == O_constant) { 
      insert_value(&vals[i].val,ex->X_add_number,operand); // We don't need to use overloaded version here
    } else  if (ex->X_op == O_symbol) {
      vals[i].symbol = ex->X_add_symbol;
    } else { 
      AS_BAD (_("Can not handle fix")); 
    } 
  }
  // TBD
  (*opcode->action)(&vals[0],grp); 
}

static shared_expr *add_fixup(InstrInfo *info,unsigned op_index,const string &arg,const struct adl_operand *operand,
                              expressionS *operand_values,int operand_arg,expressionS *ex,bool need_operands,
                              adl_operand_macro *opr_macro,int *fc_p,adl::adl_fixup *fixups,shared_expr *prior_operand_values)
{
  struct adl_opcode *opcode = info->opcode;
  bool is_macro = info->is_macro;
  int line_number = info->line_number;
  
  int fc = *fc_p;
  
  // Need a fixup.  Value will be 0 in the instruction until the fixup is
  // applied later.  Call a callback in order to determine whether we have
  // a relocation.  The callback should return a relocation value, or
  // BFD_RELOC_UNUSED if not applicable.

  if (fc >= info->num_operand_values()) // TBD
    AS_FATAL (_("too many fixups"));
  fixups[fc].is_relative = (operand->flags & ADL_RELATIVE);
  fixups[fc].line_number = line_number;
  fixups[fc].order = info->order;
  fixups[fc].instr_size = opcode->size;

  // Try to get relocation value by name first. If the value is BFD_RELOC_UNUSED, then
  // try to look up the value from field, if the value is -1 (default), then use BFD_RELOC_UNUSED
  pair<bfd_reloc_code_real_type,bool> reloc = adl_get_reloc_by_name(arg,ex,operand);
  if (reloc.first == BFD_RELOC_UNUSED) { 
    int reloc_int = operand->ptr->reloc_type; 
    if (reloc_int != -1) {
      reloc.first = (bfd_reloc_code_real_type)reloc_int;
      reloc.second = operand->ptr->reloc_extra;
    }
  }

  // Store the expression after doing the reloc lookup, since we may have
  // added on an additional constant for expressions of the form
  // symbol@reloc + constant.
  fixups[fc].exp = *ex;
  fixups[fc].reloc = reloc.first;
  fixups[fc].md_extra = reloc.second;

  fixups[fc].operand_arg = -1;

  // Do we need the operands?  Only if we have modifiers in other fields.
  shared_expr *operand_values_fixup = 0;
  adl_opcode* opcode_fixup = 0;
        
  if (need_operands || is_macro) {                  //
        
    opcode_fixup = (adl_opcode*)xmalloc(sizeof(adl_opcode));  // The opcode is "dynamic"
    memcpy(opcode_fixup, opcode, sizeof(adl_opcode));
    assert(opcode->operands);
    assert(opcode->num_operands);
    opcode_fixup->operands = (struct adl_operand*)xmalloc(sizeof(struct adl_operand)*opcode->num_operands);
    memcpy(opcode_fixup->operands, opcode->operands, sizeof(struct adl_operand)*opcode->num_operands);        
    operand = &opcode_fixup->operands[op_index];   // Rewrite operand
    fixups[fc].operand_arg = operand_arg;          // Index of operand_values array which needs the fixed-up value.
            
    if (need_operands) {
      // If we have an existing operands object, then just use it (the reference
      // count will be incremented).  Otherwise, create a new one.
      if (prior_operand_values) {
        operand_values_fixup = prior_operand_values;
        operand_values_fixup->incr();
      } else {
        operand_values_fixup = new shared_expr(operand_values,info->num_operand_values());
      }
    } 
  } 

  fixups[fc].opcode = opcode_fixup;
  fixups[fc].operand_values = operand_values_fixup;
  fixups[fc].operand = operand;
        
  if (is_macro) {                           //                     
    fixups[fc].is_macro = true;
    adl_operand_macro* operand_macro_fixup = (adl_operand_macro*)xmalloc(sizeof(adl_operand_macro)); // Make a new copy
    memcpy(operand_macro_fixup, opr_macro, sizeof(adl_operand_macro));  //
    fixups[fc].operand_macro = operand_macro_fixup;   //
  } 
  else {                                  //
    fixups[fc].is_macro = false;
    fixups[fc].operand_macro = 0;           //
  }

  
  ++(*fc_p);

  return operand_values_fixup;
}

static void build_instruction(unsigned *insn, InstrInfo *info, adl::adl_fixup *fixups, int *num_fixups)
{
  int maxfields = info->num_operand_values();
  char *s = &info->str[0];
  struct adl_opcode *opcode = info->opcode;
  expressionS *operand_values = info->operand_values;
  int num_operand_values = info->num_operand_values();        //
  adl_operand_macro *operand_macro = info->operand_macro;   //
  bool is_macro = info->is_macro;                           //
  bool need_operands = false;
  int i;
  
  // Do we have any modifier functions in any of the operands?  If so, this
  // implies that we need to have the operand values stored for later use.
  //
  // TODO: Now that we have mod_indices and know which modifiers use what
  // operands, we could probably save some space by only storing what we need.
  for (i = 0; i != opcode->num_operands; ++i) {   
    if (opcode->operands[i].modifier) {           
      need_operands = true;
    }
  }

  int line_number = info->line_number;
  unsigned pc     = info->pc;
  int fc = 0;
  shared_expr *prior_operand_values = 0;
  
  for (i = 0; i != opcode->num_operands; ++i) {
    const struct adl_operand *operand = &opcode->operands[i];
    int opv_base = operand->opv_base;
    
    //need to backup due to %p-fields   
    if (operand->modifier) {
      // Modifier function exists- use it if none of the operands that it
      // references are symbols.  Otherwise, we have to add them in as fixups
      // and evaluate this later.  We also have to adjust operand_values by the
      // opv_base (for macro instructions) so that we get the proper value.
      bool proceed = true;
      adl_operand_macro *opr_macro = 0;
      for (int *mod_indices = operand->modifier_srcs; *mod_indices >= 0; ++mod_indices) {
        int index = *mod_indices + opv_base;
        expressionS *ex = &operand_values[index];
        opr_macro = &operand_macro[index];
        if (ex->X_op != O_constant && ex->X_op != O_illegal && ex->X_op != O_register) {
          // Test that it's not a constant or absent, so that it's analogous to
          // the non-modifier case.
          prior_operand_values = add_fixup(info,i,info->getRawArg(index),operand,&operand_values[opv_base],index,ex,need_operands,opr_macro,&fc,fixups,prior_operand_values);
          proceed = false;
        }
      }
      if (proceed) {
        // If no symbols exist, then we can proceed.
        if (is_macro) {
          assert(opr_macro);
          insert_modifier(insn,operand,&operand_values[opv_base],pc, opr_macro); 
        } else {    
          insert_modifier(insn,operand,operand_values,pc);      
        }
      }
    } 
    else {
      assert (operand->arg >= 0);
      expressionS *ex = &operand_values[operand->arg];
      adl_operand_macro *opr_macro = 0;
      if (is_macro) { 
        assert(operand_macro); // ((is_macro) && (opcode->num_operands)) -> operand_macro
        opr_macro = &operand_macro[operand->arg]; 
      }
      const string &arg = info->getRawArg(operand->arg);

      if (ex->X_op == O_register) {
      // Simple integer to be loaded directly into the instruction.
      // insert_value(insn,ex->X_add_number,operand);
      if (is_macro && (opr_macro->width < opr_macro->instr_width)) {   // Short-circuit evaluation
        insert_value(insn, ex->X_add_number, operand, opr_macro); 
      }
      else {    
        insert_value(insn, ex->X_add_number, operand);
      }
    } 
    else if (ex->X_op == O_constant) {
      // Callback to handle relocations in the argument.
      acb_const_hook((char*)arg.c_str(),ex,operand); // TBD
      // Simple integer to be loaded directly into the instruction.
      // insert_value(insn,ex->X_add_number,operand);
      if (is_macro && (opr_macro->width < opr_macro->instr_width)) { // Short-circuit evaluation
        insert_value(insn, ex->X_add_number, operand, opr_macro);  
      }
      else {   
        insert_value(insn, ex->X_add_number, operand);
      }      
    }
    else if (ex->X_op == O_illegal) {
      // In this context, an illegal value means that one wasn't specified.  So,
      // we just ignore it.
      // Note that instruction-type fields generate "O_illeagal"
    }
    else  {
      // We need a fixup for this field, so add it here.
      prior_operand_values = add_fixup(info,i,arg,operand,operand_values,operand->arg,ex,need_operands,opr_macro,&fc,fixups,prior_operand_values);
    }
    }
  }
    
  *num_fixups = fc;
}




// This tries to find and assemble appropirate prefix instruction.
// It is not very optimal but may be it is enough.
// Note: Since assembler action code may modify a previous packet prefix
//       the prefix selection algorithm is incorrect in the genral case.
//       It does 
void savePrefix(InstrBundle &grp) 
{
  int i;
  int prefix = -1;
  bool need_prefix = false; // We always need prefix

  // We've called this function, so avoid calling it again.
  prefix_saved = true;

  struct adl_prefix_field **pfx_fields = prefix_fields[curgrp()].fields;
  // We cycle through all prefixes until we find appropriate one.
  for (i = 0; i != num_prefix_instrs; ++i) {
    // Now go through all prefix fields and if we see that its value different to default we 
    // check that the current prefix instruction can handle this field;
    int j;

    struct adl_opcode* opcode = &(prefix_instrs[i].opcodes[0]);

    for (j = 0; j != num_prefix_fields; ++j) {
      if (pfx_fields[j]->value != pfx_fields[j]->default_value) {
        // OK, have to handle this field, currently we take the first available.
        // TBD: check operand width, we'll take prefix that can handle the value.
        need_prefix = true;
        int k;
        bool found = false;
        for (k = 0; k != opcode->num_operands; ++k) {
          if (opcode->operands[k].prefix_field_id == j || opcode->operands[k].prefix_field_type_id == j) {
            found = true;
            break;
          }
        }
        if (!found) {
          goto NEXT;
        }
      }
    }
    // If we got here this prefix will suit our needs.
    prefix = i;

  NEXT:;   
  }

  if (!need_prefix) {
    return;
  }

  if (prefix == -1) {
    AS_FATAL(_("Cannot find appropriate prefix."));
  }
  InstrInfo &info = grp.get_prefix();
  struct adl_opcode* prefix_instr ATTRIBUTE_UNUSED = &(prefix_instrs[prefix].opcodes[0]);
  info.opcode = prefix_instr;
  info.alloc_operand_values(prefix_instr->num_operands);
  grp.set_has_prefix(true);
}

// This tries to find and assemble appropirate prefix instruction.
// It is not very optimal but may be it is enough.
// Returns whether a prefix was actually written
static void write_prefix(int grp) 
{
  if (!instrInfos[grp].has_prefix()) {
    return;
  }

  InstrInfo &info = instrInfos.get_prefix(grp);

  struct adl_prefix_field **pfx_fields = prefix_fields[grp].fields;
  struct adl_opcode* prefix_instr ATTRIBUTE_UNUSED = info.opcode;

  unsigned instr[MAX_INSTR_WORDS];
  int k;
  // Set opcode
  for (k = 0; k != MAX_INSTR_WORDS; ++k) { 
    instr[k] = prefix_instr->opcode[k];
  };
  // Set operands, assume it is just a value
  for (k = 0; k != prefix_instr->num_operands; ++k) {
    const struct adl_operand *operand = &prefix_instr->operands[k];
    if (operand->prefix_field_id < 0) {
      continue;
    }

    // Is a prefix field- use the default value unless otherwise set.
    int value = pfx_fields[operand->prefix_field_id]->value;
    // Find out whether the field or the "type" of the field were set
    if (value == pfx_fields[operand->prefix_field_id]->default_value && operand->prefix_field_type_id != -1) {
      int type_value = pfx_fields[operand->prefix_field_type_id]->value;
      if (type_value != pfx_fields[operand->prefix_field_type_id]->default_value) {
        value = type_value;
      }
    }
    insert_value (instr,(uint64_t) value,operand);
  }

  // Instruction is all set, now write
  int instr_sz   = prefix_instr->size;
  uint32_t addr_mask = prefix_instr->addr_mask;
  write_instr(info.f,instr,instr_sz,frag_now,addr_mask);

  instrInfos[grp].set_has_prefix(false);
}

// Standard handling of instruction buffers.  We have a target-specific hook
// which allows this to be skipped.
static void alloc_instr_bufs(int grp)
{
  if (!acb_prealloc_instr_bufs()) return;
  // We allocate the prefix (if applicable) in the appropriate location in the
  // bundle.  Re-ordering should be done, so we can allocate instruction buffer
  // space.
  bool allocated_prefix = false;
  bool need_pfx = instrInfos[grp].has_prefix();
  unsigned pfx_index = instrInfos[grp].prefix_index();
  for (unsigned i = 0; i != instrInfos[grp].size(); ++i) {
    if (i == pfx_index && need_pfx) {
      alloc_instr_buf(instrInfos[grp].get_prefix());
      allocated_prefix = true;
    }
    InstrInfo &info = instrInfos[grp][i];
    alloc_instr_buf(info);
  }
  // Handle prefix-at-end-of-buffer situation.
  if (!allocated_prefix && need_pfx) {
    alloc_instr_buf(instrInfos[grp].get_prefix());
  }
}


static void write_instrs() 
{

  unsigned instr_arr[MAX_INSTR_WORDS];
  unsigned *instr = instr_arr;  

  unsigned cur_group  = curgrp();
  unsigned last_group = parallel_arch ? (curgrp() + 1) % queue_size : 0;
  unsigned prev_group = (curgrp() - queue_offset) % queue_size;

  // Toggle the fact that we have not yet saved the prefix.
  prefix_saved = false;

  // Run post packet handler and any final rules.  Note: Re-ordering might take
  // place here.  First, set the order members to reflect the initial order.
  for (int i = 0; i != instrInfos[curgrp()].size(); ++i) {
    instrInfos[curgrp()][i].order = i;
  }

  // Extra hook for multi-packet manipulation.
  acb_process_packets(instrInfos,curgrp(),last_group);

  instrInfos.set_current_group(prev_group);
  
  if (instrInfos[curgrp()].size()) {
    if (post_packet_asm_handler) {
      (*post_packet_asm_handler)(instrInfos[curgrp()],curgrp());
    }
    if (post_packet_rule_checker && check_asm_rules) {
      (*post_packet_rule_checker)(instrInfos[curgrp()],curgrp());
    }
  }
  
  if (parallel_arch && !prefix_saved) {
    savePrefix(instrInfos[curgrp()]);
  }

  alloc_instr_bufs(curgrp());

  instrInfos.set_current_group(cur_group);
  
  // The above processes the current group while the below writes out the last
  // group processed before Need adl_cleanup() to write out all the groups held
  // in the queue when the assembly is done.  Pay attention to when the cleanup
  // is processed. Make sure the instructions written out in the cleanup are
  // processed properly
  bool wrote_prefix = false;
  // Now write instructions If the last group is empty, the loop is skipped
  for (int i = 0; i != instrInfos[last_group].size(); ++i) {
    
    // Write prefix in the appropriate location.
    if (!wrote_prefix && i == instrInfos[last_group].prefix_index()) {
      write_prefix(last_group);
      wrote_prefix = true;
    }
    InstrInfo &info = instrInfos[last_group][i];
    // Initial instruction setup.
    struct adl_opcode *opcode = info.opcode;
    
    if(opcode == NULL) {
      continue;
    }
    
    int k;
    for (k = 0; k != MAX_INSTR_WORDS; ++k) { 
      instr[k] = opcode->opcode[k];
    };
    int instr_sz   = opcode->size;
    uint32_t addr_mask = opcode->addr_mask;
    // Uses *_now variables, may be need to take them out?
    // handle_itable_change( InstrInfos[last_group][i].instr_table_name,InstrInfos[last_group][i].instr_table);
    // Now process all of the operands, placing them into the instruction
    // or registering fixups, if needed.
    int num_fixups = 0 ;
    DeclArray(adl_fixup,fixups,info.num_operand_values());
   
    build_instruction(instr,&info,fixups,&num_fixups); // TBD

    // If the relocation was configured as requiring extra handling, then call
    // out to the handler function.  This must be defined by the user and linked
    // in when the assembler is built.  Our default handler just prints an error
    // message.
    //
    // Bit of a hack- we only handle one relocation per instruction.
    bool handle_fixs = true;
    if (num_fixups && (fixups[num_fixups-1].reloc != BFD_RELOC_UNUSED)) {
      adl_fixup &fixup = fixups[num_fixups-1];
      if (fixup.md_extra) {
        acb_handle_reloc_extra(instr,instr_sz,fixup.reloc,opcode,fixup.operand,&fixup.exp);
        handle_fixs = false;
      }
    }
    
    // Handle fixups here.
    if (handle_fixs) {
      // Target-specific instruction buffer handling, if relevant.
      // Write out the instruction.

      // Target-specific instruction buffer allocation, if relevant.
      acb_alloc_instr_buf(info);    

      // Note: We have to use the stored frag pointer, rather than frag_now,
      // because we might have changed frags if we have a design with a queue
      // size > 1.
      char *f     = info.f;
      fragS *frag = info.frag;
      write_instr(f,instr,instr_sz,frag,addr_mask);

      handle_fixups(f,frag,fixups,num_fixups,instr_sz);
    }

#   ifdef OBJ_ELF
    dwarf2_emit_insn (instr_sz);
#   endif
    
  }
  
  // Catch the situation where the prefix should be at the end.
  if (!wrote_prefix) {
    write_prefix(last_group);
  }
  
  // Reset prefix
  reset_prefix(last_group);

  // Clear the last group and reserve the space for the next group coming
  instrInfos[last_group].clear(); 
  instrInfos.set_current_group(parallel_arch ? last_group : 0);

  // label is associated with a VLES, reset it when packet is written
  curr_labels.clear();
}

static bool zero_operands_is_valid(struct adl_opcode *opcode)
{
  // Zero operands for this instruction, so no operands in assembly is OK.
  if (opcode->num_operands == 0) {
    return true;
  }
  // Also valid if all operands are 'assembler' operands, meaning that they're
  // not set directly on the assembly line, but rather via other hooks.
  for (int i = 0; i != opcode->num_operands; ++i) {
    if (opcode->operands[i].ptr->assembler == 0) {
      // Nope!  Found a non-assembler field, so we *do* expect to find something
      // on the assembler line.
      return false;
    }
  }
  // It's OK- only assembler fields are present.
  return true;
}

// Extract any prefix flags from the input assembly line.
static char * handle_prefix_values(Args &args,char *str,struct hash_control *instr_pfx_fields_hash,const char *tc,int &fin)
{
  // First, is it a prefix value?  If so, record for later use.  Repeat until no more prefix fields found.
  int n;
  do {
    // Find end of first word.
    n = strcspn(str,tc);

    // Then lower-case it in place.
    for (char *s = str; s != &str[n]; ++s) {
      *s = TOLOWER(*s);
    }
    
    if (instr_pfx_fields_hash && hash_find_n(instr_pfx_fields_hash,str,n)) {
      // It is a prefix field value, so record and continue.
      args.push_back(Arg(str,n));
      str += n;
      skip_whitespaces(&str);
    } else {
      // We don't have any prefix fields, or it doesn't match, so continue.
      break;
    }
  } while (1);
  fin = n;
  return str;
}

static bool handle_assembly_instrs(char *str,int n,struct hash_control *asm_op_hash)
{
  // Next, check if it is a "assembler instruction"
  const struct adl_asm_instr * asm_instr = (const struct adl_asm_instr *) ((asm_op_hash) ? hash_find_n(asm_op_hash,str,n) : 0);

  if (asm_instr) {
    if (asm_instr->action) {
      (asm_instr->action)(curgrp());
    }
    return true;
  }     

  return false;
}

// For simple instructions, we just call save_instr.  However, for shorthands we
// have to add on the target instruction(s) and map the shorthands operands to
// each target.
void save_instr_internal(InstrBundles &instrInfos,char *s, adl_opcode *opcode, adl_opcode *src_opcode, expressionS *operand_values,
                         const Args &args,const char *file_name, int line_number, unsigned *adl_pc, int maxfields, int instr_table,
                         const char *instr_table_name,const string &eoi_str) 
{
  if (opcode->asm_func) {
    // Complex function-based alias.  Create an instruction argument array and
    // let the user's code expand the macro.
    unsigned num_args = min(maxfields,opcode->num_operands);
    InstrArgs macro_args;
    for (unsigned i = 0; i != num_args; ++i) {
      macro_args.push_back(InstrArg(operand_values[i],string(args[i].str,args[i].len)));
    }
    instrInfos[curgrp()] = opcode->asm_func(macro_args);
    InstrBundle &b = instrInfos[curgrp()];
    for (unsigned i = 0; i != b.size(); ++i) {
      b[i].line_number = line_number;
      b[i].file_name = file_name;
      b[i].pc = *adl_pc;
    }
  }
  else if (opcode->num_alias_targets) {
    // Handle simple aliases.
    for (int t_iter = 0; t_iter != opcode->num_alias_targets; ++t_iter) {
      // The target is the opcode which represents the alias (has the
      // instruction encoding plus any fixed fields in it).  The source opcode
      // is the opcode which represents the shorthand and points to the target.
      // Kind of weird- not sure if we need this extra level of indirection.
      adl_opcode     *trg = &opcode->alias_targets[t_iter];

      // Save this target instruction.  Note the recursive call: I want to be
      // able to handle arbitrary levels of indirection.
      save_instr_internal(instrInfos,s,trg,opcode,operand_values,args,file_name,line_number,adl_pc,maxfields,instr_table,instr_table_name,eoi_str);
    }
  }
  else {
    // Not a shorthand, so just save this item. // The item is saved into current group internally.
    save_instr(instrInfos,s,opcode,src_opcode,operand_values,args,file_name,line_number,*adl_pc,maxfields,instr_table,instr_table_name,eoi_str);
    if (opcode->action) { 
      execute_action(opcode,operand_values,*adl_pc,maxfields,curgrp()); 
    }
    *adl_pc += 1;
  }
}

// Find an end-of-instruction marker.  If present, null it out and return a
// pointer to that terminator.  If not present, returns 0.
char *find_eoi_marker(char *str,string &eoi_str)
{
  if (!instr_separators) return 0;
  size_t r = strcspn(str,instr_separators); // Search for the length not consisting of the end-of-instruction characters.
  if (!r) return 0;                         // 0 search meant that we were passed an empty string.

  // Currently, things "just work" w/o worrying about using space as an
  // instruction separator.
  
  char *next = &str[r];
  if (*next) {                              // If the string doesn't point to null, then null it out and return it.
    eoi_str = *next;
    *next = 0;
    return next;
  }
  return 0;                                 // We're at the end of the string, so we consider that a no-separator situation.
}

// Given a default string and a string returned by find_eoi_marker, returns the
// latter if it's non-null, otherwise the first.
void restore_eoi_marker(char *eoi)
{
  if (eoi) {
    // Should only ever get a non-null string here if instr_separators isn't
    // null, since that's checked by find_eoi_marker.
    *eoi = instr_separators[0];
  }
}

char *next_instr(char *defstr,char *eoi_str)
{
  return (eoi_str) ? eoi_str+1 : defstr;
}

// Assembles single instruction.
static char *adl_assemble_instr(char *str,struct hash_control *op_hash, 
                                struct hash_control *asm_op_hash ATTRIBUTE_UNUSED,
                                struct hash_control *reg_hash,struct hash_control *instr_pfx_fields_hash,
                                int maxfields,int instr_table, const char *instr_table_name)
{
  // Hack: We try to track the program counter here.  We need to eventually
  // replace this with a proper fixup for the program counter.
  static unsigned adl_pc = 0;

  skip_whitespaces(&str);
  int op_index;
  const struct adl_instr  *instr;
  struct adl_opcode *opcode;
  const int BufSize = 512;
  char *orig_str = str;

  string eoi_str;
  char *eoi = find_eoi_marker(str,eoi_str);

  // This very outer loop is for checking instruction name termination.  On the
  // first attempt, we use a stricter check that includes characters found in
  // instruction names.  The second check uses a looser check, but we try anyway
  // since there may be duplication between enumerations used in abutting fields
  // and characters used in actual instruction names.
  for (int instr_tc_check = 0; instr_tc_check != 2; ++instr_tc_check) {
    // This stores the raw operand value parsed by the regular expression.
    const int nmatch = maxfields+1;
    DeclArray(regmatch_t,pmatch,nmatch);
        
    // The raw argument data.  We put prefix info plus regular expression data
    // into this vector.
    Args args;
    
    // This will store all of the operands that we parse.
    DeclArray(expressionS,operand_values,maxfields);
    
    int fin;

    const char *tc = ((instr_tc_check == 0) ? min_terminating_chars : terminating_chars);

    str = orig_str;
    char *instr_end = str;

    // Are there any flags that might precede the instruction name?
    str = handle_prefix_values(args,str,instr_pfx_fields_hash,tc,fin);

    // Before any processing is done, run the pre-assembly handler, if one exists.
    if (pre_asm_handler) {
      (*pre_asm_handler)(curgrp());
    }

    // Check to see if we have any assembly instructions.  These are special pseudo instructions
    // which just modify behavior during assembly.
    if ( handle_assembly_instrs(str,fin,asm_op_hash) ) {
      str += fin;
      skip_whitespaces(&str);
      restore_eoi_marker(eoi);
      return str;
    }

    // Lookup the opcode in the table, produce an error if not found.
    instr = (const struct adl_instr *) hash_find_n(op_hash,str,fin);
    if (!instr) {
      // Try again if we can, using a looser check; maybe the instruction is there
      // but has an abutting field.
      if (!instr_tc_check) continue;

      if (is_grouping_char(*instr_end)) {
        AS_BAD(_("Wrong grouping character"));
      } else {
        AS_BAD(_("Unrecognized instruction:  '%s'"),str);
      }
      return next_instr(skip_to_next(&str[fin]),eoi);
    }

    // Loop over the possible opcodes for this instruction.  If we don't find
    // a match against its regular expression, we skip to the next instruction.
    // If we come to the end of our list, then it's an error.  
    // 
    // Assuming we get a regular expression match, we then do range and type
    // checking.  If we don't match, we again skip to the next instruction.  This
    // allows us to do a limited form of function overloading: You can have
    // instructions of the same name that either take different numbers of
    // arguments, or different types (register vs. immediate values) and of
    // different values.
    //
    opcode = 0;
    str = &str[fin];
    unsigned orig_args = args.size();
    for (op_index = 0; op_index != instr->num_opcodes; ++op_index) {
      // Remove any arguments we might have from a prior instruction attempt.
      args.erase(args.begin()+orig_args,args.end());
      // Try to use the instruction with longest immediate. 
      // This flag is set only by the DevTech assembler integration, it signals
      // that one of the operands is an unresolved symbol, in which case 
      // the equivalent opcode with the longest immediate operand (if defined), is tested
      // for a match
      bool is_unresolved_symbol = true;
      opcode = &instr->opcodes[op_index];    

      // Skip to next item if we're told to not consider this instruction at the
      // top-level.
      if (!opcode->assemble) continue;
      
      // Now parse the operands.  We do this using regular expressions matching.
      if (regexec((const regex_t*)opcode->regcomp,str,nmatch,pmatch,0) == REG_NOMATCH) {
        continue;
      }

      // Transfer regular expression arguments.  This returns false if the
      // regular expression match ends with non-whitespace or string-end.  This
      // implies that the regular expression wasn't as complete as it could be,
      // e.g. we could have matched against 3 parameters, but only matched
      // against two.
      if (!copy_operands(args,instr_end,opcode,str,pmatch,maxfields)) {
        continue;
      }

      // Process each input operand.  This just handles converting raw text into
      // expressions.
      if (!process_operands(opcode,args,maxfields,operand_values,reg_hash,is_unresolved_symbol)) {
        continue;
      }

      // Now do input validation.
      if (valid_input(opcode,operand_values)) {
        // Input validated, so we can use this instruction.

        // If an unresolved symbol exists, the instruction with longest immediate is defined and
        // it is not the current instruction try it as well.
        // If inputs are invalid so will be those of the longest instruction
        if (adl_try_longest_instr && is_unresolved_symbol && opcode->longest != -1 && opcode->longest != op_index) {
          // This will store all of the operands that we parse.
          DeclArray(expressionS,longest_values,maxfields);

          struct adl_opcode *long_opcode = &instr->opcodes[opcode->longest];
          // Do not expect a regex match here, since the DevTech assembler will give "unknown_immed".
          if (/*regexec(long_opcode->regcomp,s,nmatch,pmatch,0) != REG_NOMATCH &&*/
              process_operands(long_opcode,args,maxfields,longest_values,reg_hash,is_unresolved_symbol) && 
              valid_input(long_opcode,longest_values)) {
            // The instruction with the longest immed can be used
            // use the operand values of that instruction
            memcpy((void *)operand_values,(void *) longest_values,sizeof(longest_values));
            opcode  = long_opcode;
          }
        }

        break;
      }
    }

    if (op_index == instr->num_opcodes) {
      if (!instr_tc_check) {
        // Try again if we can, since maybe there's another version of the
        // instruction which will match but didn't originally because we have an
        // abutting field.
        continue;
      } else {
        AS_BAD(_("%s:  No version of instruction %s matched the given arguments."),orig_str,instr->name);
        // If we never matched against a regular expression, then try to advance
        // past operands to the next possible instruction.  Otherwise, we have
        // already matched against operands and found some problem, so instr_end
        // should have already been updated.
        if (instr_end == orig_str) {
          return next_instr(skip_to_next(instr_end),eoi);
        } else {          
          return next_instr(instr_end,eoi);
        }
      }
    }

    char *file_name;
    unsigned line_number;
    as_where(&file_name,&line_number);

    save_instr_internal(instrInfos,str,opcode,0,operand_values,args,file_name,line_number,&adl_pc,maxfields,instr_table,instr_table_name,eoi_str);

    if (post_asm_handler) {
      (*post_asm_handler)((opcode->width), curgrp()); //
    }
    if (opcode->checker && check_asm_rules) {
      (*(opcode->checker))(instrInfos[curgrp()].back(),curgrp());
    }

    clear_preset_info();

    // If we succeed, then we're done.
    return next_instr(instr_end,eoi);
  }
}

static bool end_of_packet(char **p_str) {
  // If no grouping defined - choose grouping method
  if (!expected_end) {
    const char *p = strchr(packet_begin_chars,**p_str);
    if ( p && *p ) {
      expected_end = &packet_end_chars[p-packet_begin_chars]; 
      ++(*p_str);
      skip_whitespaces(p_str);
      return false;
    } else {
      expected_end = default_packet_sep;
      return false;
    }
  }

  skip_whitespaces(p_str);
  if (!**p_str) {
    if (*expected_end == '\n') {
      expected_end = 0;
      return true;   
    } else {
      return false;
    }
  }
  // First check for end packet
  if (**p_str == *expected_end) {
    expected_end = 0;
    ++(*p_str);
    skip_whitespaces(p_str);
    // Start of packet may follow immideatly
    const char *p = strchr(packet_begin_chars,**p_str);
    if (p && *p) {
      expected_end = &packet_end_chars[p-packet_begin_chars];
      ++(*p_str);
      skip_whitespaces(p_str);
    }
    return true;
  } else {
    return false;
  }
}

// If the string contains '%' characters, then see if we can parse it for relocation syntax of the form %reloc(sym).  If so,
// convert in place to sym@reloc
void convert_func_relocs(char *str)
{
  while (char *c = strchr(str,'%')) {

    char *f = c;
    
    // Found a '%' so see if it has the form %reloc(sym).
    char *rstart = 0, *rend = 0, *sstart = 0, *send = 0;
    rstart = ++c;
    while ( ISIDNUM(*c)) ++c;
    if (*c == '(') {
      rend = c;
      sstart = ++c;

      while ( *c && *c != ')' ) ++c;

      if (*c == ')') {
        send = c;

        // Captured everything, so copy out what we want.
        string reloc(rstart,rend);
        string sym(sstart,send);

        // Now write it back.
        memcpy(f,sym.c_str(),sym.size());
        f += sym.size();
        *f = '@';
        ++f;
        memcpy(f,reloc.c_str(),reloc.size());
        f += reloc.size();

        // Now move everything down by two characters to eliminate the gap from
        // not needing the parentheses.
        ++send;

        // We'll start over again here, to make sure we can catch multiple
        // relocations.
        str = f;

        // Now move everything.
        while ( *send ) {
          *f = *send;
          ++f;
          ++send;
        }
        *f = 0;
        
      } else {
        // Didn't find a closing parenthesis after alphanumeric characters.
        return;
      }
      
    } else {
      // Didn't find an opening parenthesis after alphanumeric characters.
      return;
    }
  }
}

static char adl_input_str[240];

extern "C" void adl_assemble(char *str,struct hash_control *op_hash,struct hash_control *asm_op_hash,struct hash_control *reg_hash,
                             struct hash_control *instr_pfx_fields_hash,int maxfields,int instr_table,const char * instr_table_name) 
{
  init_line_assemble();
  if (adl_show_warnings) {
    strncpy(adl_input_str,str,240);
  }
  
  // Handle multicharacter comments 
  handle_comments(str);
  
  // Convert over any relocation syntax of the form %reloc(sym) to sym@reloc.
  // The former confuses the regex parsing.
  convert_func_relocs(str);
  
  if (parallel_arch) {
    // If grouping is defined by new line we understand it here of it is
    // a single line that indicates end of packet we commit instructions.
    bool done = false;
    if (end_of_packet(&str)){
      done = !(bool)(*str);
      write_instrs();
    }

    while (!done && *str) {
      str = adl_assemble_instr(str,op_hash,asm_op_hash,reg_hash,instr_pfx_fields_hash,maxfields,instr_table,instr_table_name);
      // check grouping
      if (end_of_packet(&str)){
        done = !(bool)(*str);
        write_instrs();
      }
    }
  } else {
    adl_assemble_instr(str,op_hash,asm_op_hash,reg_hash,instr_pfx_fields_hash,maxfields,instr_table, instr_table_name);
    write_instrs();
  }
}

// We only get here if the user has specified that a relocation type requires
// extra processing.
void md_convert_frag (bfd *abfd ATTRIBUTE_UNUSED,asection *sec ATTRIBUTE_UNUSED,fragS *fragp ATTRIBUTE_UNUSED)
{
  acb_handle_convert_frag(abfd,sec,fragp);
}

/* We have no need to default values of symbols.  */
symbolS *md_undefined_symbol (char *name ATTRIBUTE_UNUSED)
{
  if (adl_show_warnings) {
    char *file_name;
    unsigned line_number;
    as_where(&file_name,&line_number);

    printf("Warning: %s:%d: undefined symbol: %s\n Input: %s \n\n",file_name,line_number,name,adl_input_str);
  }
  return 0;
}


// We'll only get here if we've specified extra handling for relocations.  In
// that case, the user will have to supply an extra library to handle this.
int md_estimate_size_before_relax (fragS *fragp ATTRIBUTE_UNUSED,asection *seg ATTRIBUTE_UNUSED)
{
  return acb_estimate_size_before_relax (fragp,seg);
}

static void flush_instr_queue()
{
  // Cleanup any remaining instructions in the input queue.
  for (int i = 0; i != queue_size; ++i) {
    write_instrs();
  }

  instrInfos.set_current_group(0);
}

// Cleanup routine at the end of a sequence, e.g. when a .org directive is
// encountered.
extern void adl_flush_pending_output()
{
  flush_instr_queue();
}

// Cleanup routine at the end of a file.
extern void adl_cleanup()
{
  flush_instr_queue();
  
  if (num_rule_errors) {
    AS_BAD(_("%d assembler rule violations were found."),num_rule_errors);
  }

}

extern void adl_set_curr_label(symbolS *s) 
{
  curr_labels.push_back(s);
}

#ifdef _MSC_VER

#include <intrin.h>

static uint32_t __inline clz(uint32_t x)
{
	unsigned long r = 0;
	return _BitScanReverse(&r,x);
	return (sizeof(x)*8-1) - r;
}

static uint32_t __inline ctz(uint32_t x)
{
	unsigned long r = 0;
	return _BitScanForward(&r,x);
	return r;
}

static uint32_t __inline popcnt(uint32_t x)
{
	int c;
	for (c = 0; x; c++) {
		x &= x - 1; // clear the least significant bit set
	}
	return c;
}

#else

static uint32_t inline clz(uint32_t x)
{
	return __builtin_clz(x);
}

static uint32_t inline ctz(uint32_t x)
{
	return __builtin_ctz(x);
}

static uint32_t inline popcnt(uint32_t x)
{
	return __builtin_popcount(x);
}

#endif

unsigned count_leading_zeros(offsetT n,unsigned s)
{
  if (n == 0) {
    return 0;
  } else {
    return clz((uint32_t)n) - (sizeof(offsetT)*8 - s);
  }
}

unsigned count_trailing_zeros(offsetT n)
{
  if (n == 0) {
    return 0;
  } else {
    return ctz((uint32_t)n);
  }
}

unsigned count_ones(offsetT n)
{
  return popcnt((uint32_t)n);
}

