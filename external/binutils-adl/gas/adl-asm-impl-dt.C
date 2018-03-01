//
// Machine independent functions for use with the DevTech assembler.
//

extern "C" {
# include "as.h"
# include "xregex.h"
# include "safe-ctype.h"
}

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <inttypes.h>

#include "AsmDriver.h"

#include "adl-asm-impl.h"
#include "adl-asm-info.h"

#ifdef __CYGWIN__
# include "bits/wordsize.h"
#endif

// Methods required for DevTech integration
extern "C" {

  void free_buffer(unsigned);

}

using namespace std;
using namespace adl;

// The 4 is for safety
#define MAX_VLES_ENCODING  MAX_INSTR_WORDS*INSTR_BUFFER_SIZE*__WORDSIZE*4
// Maximal length of a single error messag
#define MAX_ERR_MSG_LEN    512

static unsigned             *adl_error_count = 0;
static CADL_ext_relocations *adl_relocs = 0;
static CADL_messages        *adl_messages = 0;

static char  vles_buffer[MAX_VLES_ENCODING];
static char *cur_buffer_pos = &vles_buffer[0];
static char *prv_buffer_pos = &vles_buffer[0];


// For the DevTech integration, messages are stored into a buffer and then
// passed back to the assembler through the VLES encode call.

void message(e_message_priority priority,const char *format,va_list args)
{
  const int bufsize = 2000;
  char buffer[bufsize+1];

  vsnprintf (buffer, bufsize, format, args);

  assert(adl_messages);
  if (priority == e_error || priority == e_severe_error) {
    (*adl_error_count)++;
  }
  adl_messages->push_back(CADL_message(buffer,priority));
}

void message(e_message_priority priority,const char *format, ...)
{
  va_list args;

  va_start (args, format);
  message(priority,format,args);
  va_end (args);
}

CADL_message make_message(e_message_priority priority,const char *format, ...)
{
  const int bufsize = 2000;
  char buffer[bufsize+1];

  va_list args;

  va_start (args, format);
  vsnprintf (buffer, bufsize, format, args);
  va_end (args);

  return CADL_message(buffer,priority);
}

void adl_error(const char *msg,int pos ATTRIBUTE_UNUSED,int group ATTRIBUTE_UNUSED) 
{
  message(e_error,"%s",msg);
}

void adl_info(const char *msg,int pos ATTRIBUTE_UNUSED,int group ATTRIBUTE_UNUSED) 
{

  message(e_info,"%s",msg);
}

void adl_tsktsk (const char *format, ...)
{
  va_list args;

  va_start (args, format);
  message(e_note,format,args);
  va_end (args);
}

void adl_warn (const char *format, ...)
{
  va_list args;

  va_start (args, format);
  message(e_warning,format,args);
  va_end (args);
}

void adl_warn_where (char *file, unsigned int line, const char *format, ...)
{
  va_list args;

  va_start (args, format);
  message(e_warning,format,args);
  va_end (args);
}

void adl_bad (const char *format, ...)
{
  va_list args;

  va_start (args, format);
  message(e_error,format,args);
  va_end(args);
}

void adl_bad_where (char *file, unsigned int line, const char *format, ...)
{
  va_list args;

  va_start (args, format);
  message(e_error,format,args);
  va_end (args);
}

void adl_fatal (const char *format, ...)
{
  va_list args;

  va_start (args, format);
  CADL_message err = make_message(e_severe_error,format,args);
  va_end (args);
  throw err;
}

static void reset_buffer(void)
{
  int size = (int) (cur_buffer_pos - &vles_buffer[0]);
  cur_buffer_pos = &vles_buffer[0];
  prv_buffer_pos = &vles_buffer[0];   
  if (size) {
    memset(cur_buffer_pos,'\0',MAX_VLES_ENCODING);
  }
}

static char* allocate_buffer(unsigned sz)
{
  prv_buffer_pos = cur_buffer_pos;
  cur_buffer_pos += sz;
  return prv_buffer_pos;
}

void free_buffer(unsigned sz)
{
  prv_buffer_pos -= sz;
  cur_buffer_pos -= sz;
}

void init_line_assemble(unsigned &a_error_count,CADL_messages &a_messages,CADL_ext_relocations &a_relocs)
{
  reset_buffer();
  
  adl_error_count = &a_error_count;
  adl_messages = &a_messages;
  adl_relocs = &a_relocs;
}

// Wrapper to expression() method
segT parse_expr(expressionS *ex ATTRIBUTE_UNUSED, char *sval, int len, bool &unresolved_symbol)
{
  // Null-terminate input argument and pointer input_line_pointer at it.
  char *tmp = input_line_pointer;
  char backup = sval[len];
  sval[len] = 0;
  input_line_pointer = sval;

  // Parse the expression (which takes input from input_line_pointer).
  segT e = acb_parse_expr(ex,sval);

  if (ex->X_op == O_symbol) {
    unresolved_symbol = true;
  }

  // Now restore input_line_pointer.
  sval[len] = backup;
  input_line_pointer = tmp;
  return 0;  
}


extern "C" bfd_boolean adl_start_line_hook(void) 
{
  return FALSE;
}

void handle_itable_change(const char* instr_table_name ATTRIBUTE_UNUSED, int instr_table ATTRIBUTE_UNUSED) 
{
}



void write_instr(char *f ATTRIBUTE_UNUSED, unsigned *instr ATTRIBUTE_UNUSED,
                 int instr_sz ATTRIBUTE_UNUSED,fragS *frag_n ATTRIBUTE_UNUSED, 
                 uint32_t addr_mask ATTRIBUTE_UNUSED) 
{
# ifdef NEED_ALIGN
  int addr_mod;
  ptr_uint_t pf = (ptr_uint_t) f;
  addr_mod = (int) (pf & addr_mask);
  if (addr_mod) {
    addr_mod = addr_mask + 1 - addr_mod;
  }
  allocate_buffer(addr_mod);
  md_big_number_to_chars (f + addr_mod, instr, instr_sz);
# else
  // We had this alignment stuff here, but it doesn't seem as though it's
  // actually needed...
  md_big_number_to_chars (f, instr, instr_sz);
# endif
}

void alloc_instr_buf(InstrInfo &info)
{
  info.f = allocate_buffer(info.opcode->size);
}


void save_instr(InstrBundles &instrInfos,char *s ATTRIBUTE_UNUSED, adl_opcode *opcode ATTRIBUTE_UNUSED, 
                adl_opcode *src_opcode ATTRIBUTE_UNUSED, expressionS *operand_values ATTRIBUTE_UNUSED, 
                const Args &args ATTRIBUTE_UNUSED,const char *file_name, int line_number ATTRIBUTE_UNUSED, unsigned adl_pc ATTRIBUTE_UNUSED,
                int maxfields ATTRIBUTE_UNUSED, int instr_table ATTRIBUTE_UNUSED, const char *instr_table_name ATTRIBUTE_UNUSED,
                const std::string &eoi_str ATTRIBUTE_UNUSED) 
{
  handle_itable_change(instr_table_name,instr_table);
  InstrInfo &info = instrInfos.add();
  info.init(s,opcode,src_opcode,operand_values,args,file_name,line_number,adl_pc,maxfields,instr_table,instr_table_name);
  instrInfos.add_separator(eoi_str);
}

/* Align a section (I don't know why this is machine dependent).  */
valueT md_section_align (asection *seg ATTRIBUTE_UNUSED, valueT addr ATTRIBUTE_UNUSED)
{
  return 0;
}


/* Generate a reloc for a fixup.  */
arelent *tc_gen_reloc (asection *seg ATTRIBUTE_UNUSED,fixS *fixp ATTRIBUTE_UNUSED)
{
  return 0;
}

long md_pcrel_from_section (fixS *fixp ATTRIBUTE_UNUSED, segT sec ATTRIBUTE_UNUSED)
{
  return 0;
}

// The DevTech outer-assembler requires that relocations be in the original
// order, before reordering.  Therefore, we sort the fixups (using a vector of
// pointers) first based upon the 'order' value, then push them into the final
// container based upon this order.
ATTRIBUTE_UNUSED  void handle_fixups(char *f ATTRIBUTE_UNUSED,fragS *curr_frag ATTRIBUTE_UNUSED,
                                     struct adl_fixup *fixups ATTRIBUTE_UNUSED,
                                     int num_fixups ATTRIBUTE_UNUSED, int instr_sz ATTRIBUTE_UNUSED)
{
  for(int i = 0 ; i != num_fixups; ++i) {
    struct adl_fixup *fixup = &fixups[i];
    int  relType = fixup->operand->ptr->reloc_type;
    if (relType > 0) {
      int instr_width = fixup->operand->ptr->instr_width;
      int fix_offset = (instr_width > 0) ? instr_sz - instr_width/8 : 0;
      int offset = ((int) (f - &vles_buffer[0])) + fix_offset;

      adl_relocs->push_back(CADL_ext_relocation_info(relType,offset,fixup->order));
    } else {
      AS_FATAL(_("Unknown relocation."));
    }
  }
}


// Called to process a .switch option.  This is a generated function.
void acb_parse_option(char *s ATTRIBUTE_UNUSED,int n ATTRIBUTE_UNUSED)
{
}


void adl_assembler_init(void)
{
  symbol_begin ();
  frag_init ();
  subsegs_begin ();
  read_begin ();
  input_scrub_begin ();
  expr_begin ();
  md_begin();
}

void adl_get_vles_encoding(const char ** encoding, int *bits)
{
  *encoding = &vles_buffer[0];
  *bits = (int) (cur_buffer_pos - &vles_buffer[0]);
}

// Hook to manipulate multiple packets.  This is called immediately before
// post_packet_asm, so it can be used to combine packets, if necessary.
void __attribute__((weak)) acb_process_packets(InstrBundles &instrs,int curgrp,int lastgrp)
{
  // By default, do nothing.
}

// This must be overridden in a user supplied library, if relocations require
// extra handling.
void __attribute__((weak))  acb_handle_reloc_extra(unsigned *instr,unsigned instr_sz,
                                                   bfd_reloc_code_real_type reloc,const adl_opcode *operand,
                                                   const adl_operand *opcode,expressionS *exp)
{
  as_bad(_("Relocation requiring machine-dependent extra handling encountered, but no handler exists.")); 
}

void __attribute__((weak))  acb_handle_convert_frag(bfd *abfd,asection *sec,fragS *fragp)
{
  as_bad(_("No handler specified for frag conversion."));
}

int __attribute__((weak)) acb_estimate_size_before_relax (fragS *fragp,asection *seg)
{
  as_bad(_("No handler specified for relax size estimation."));
}

long __attribute__((weak)) acb_relax_frag (segT segment, fragS *fragP, long stretch)
{
  as_bad(_("No handler specified for frag relaxing."));
}

bool __attribute__((weak)) acb_apply_fix (fixS *fixP ,valueT *valP ,segT seg ATTRIBUTE_UNUSED)
{
  // Use ADL's handler by default.
  return false;
}

void __attribute__((weak)) acb_setup_finish(struct hash_control *instr_hash,struct adl_field *all_fields,
                                            int num_fields,const reloc_howto_type *relocs,int num_relocs)
{
  // By default, do nothing.
}

// By default, do pre-allocation so that we can handle packets of instructions.
bool __attribute__((weak)) acb_prealloc_instr_bufs()
{
  return true;
}

// By default, do nothing- we preallocate buffer space.
void __attribute__((weak)) acb_alloc_instr_buf(adl::InstrInfo & ATTRIBUTE_UNUSED)
{
}

// By default, use the normal expression parsing capability.
segT __attribute__((weak)) acb_parse_expr(expressionS *ex,const char *arg)
{
  // Parse the expression (which takes input from input_line_pointer).  Doesn't
  // currently handle 64-bit literals, which we might need to fix in the future.
  return expr(1,ex,expr_evaluate);
}

// By default, does nothing.
void __attribute__((weak)) acb_pop_insert()
{
}

// Additional option processing in an extension library.
int __attribute__((weak)) acb_parse_option(int c,const char *arg)
{
  return 1;
}                        

// Additional option processing for -m options.
int __attribute__((weak)) acb_march_parse_option(const char *arg)
{
  return 1;
}

// Any additional processing immediately after the options have been handled.
void __attribute__((weak)) acb_after_parse_args()
{
}

// Display any additional help information.
void __attribute__((weak)) acb_show_usage(FILE *)
{
}

