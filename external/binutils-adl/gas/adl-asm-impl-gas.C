//
// Machine independent functions for use with the gas assembler.
//

#ifndef _MSC_VER
extern "C" {
# include "as.h"
}
#endif

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <inttypes.h>
#include <algorithm>
#include <string>

extern "C" {
# include "as.h"
# include "xregex.h"
# include "safe-ctype.h"
# include "struc-symbol.h"
}

#include "adl-asm-impl.h"
#include "adl-asm-info.h"

using namespace std;
using namespace adl;

// For gas, we just call the gas messaging routines.
// Since C's stdargs kinda suck in many ways, we have to duplicate this code.

void adl_tsktsk (const char *format, ...)
{
  va_list args;

  va_start (args, format);
  as_vtsktsk(format,args);
  va_end (args);
}

void adl_warn (const char *format, ...)
{
  va_list args;

  va_start (args, format);
  as_vwarn(format,args);
  va_end (args);
}

void adl_warn_where (char *file, unsigned int line, const char *format, ...)
{
  va_list args;

  va_start (args, format);
  as_vwarn_where(file,line,format,args);
  va_end (args);
}

void adl_bad (const char *format, ...)
{
  va_list args;

  va_start (args, format);
  as_vbad(format,args);
  va_end(args);
}

void adl_bad_where (char *file, unsigned int line, const char *format, ...)
{
  va_list args;

  va_start (args, format);
  as_vbad_where(file,line,format,args);
  va_end (args);
}

void adl_fatal (const char *format, ...)
{
  va_list args;

  va_start (args, format);
  as_vfatal(format,args);
  va_end (args);
}

// Wrapper to expression() method
segT parse_expr(expressionS *ex, char* arg,int len,bool &unresolved)
{
   // Null-terminate input argument and pointer input_line_pointer at it.
  char *tmp = input_line_pointer;
  char backup = arg[len];
  arg[len] = 0;
  input_line_pointer = arg;

  // Parse the expression (which takes input from input_line_pointer).
  segT e = acb_parse_expr(ex,arg);

  if (ex->X_op == O_symbol) {
    symbolS* symbol = symbol_find(arg);
    unresolved = ((symbol == NULL) || (symbol->sy_flags.sy_resolved == 0));
  }

  // Now restore input_line_pointer.
  arg[len] = backup;
  input_line_pointer = tmp;
  return e;
}

// Handle multichar line_comments here;
// Need to use a hook, because otherwise this lines considered as error in read.c
// Reset line label
// TBD: to void
extern "C" bfd_boolean adl_start_line_hook(void) {

  if (num_line_comment_strs == 0) {
    return FALSE;
  }
  int i;
  for (i = 0; i != num_line_comment_strs; ++i) {
    const char *str = line_comment_strs[i];
    if (!strncmp(input_line_pointer,str,strlen(str))) {
      ignore_rest_of_line(); 
      return FALSE;
    }
  }
  return FALSE;
}


void handle_itable_change(const char* instr_table_name, int instr_table) 
{
  if (instr_table != current_table) {
    // A table-change has occurred.  We need to add a mapping
    // symbol to record this point.
    current_table = instr_table;

    {  // TBD 
      const int BufSize = 256;
      char buf[BufSize];
      sprintf(buf,"$$%s",instr_table_name);
      symbolS * symbolP = symbol_new (buf, now_seg, (valueT) frag_now_fix (), frag_now);
      symbol_table_insert (symbolP);
      symbol_get_bfdsym (symbolP)->flags |= BSF_FUNCTION | BSF_LOCAL;
    }
  }
}


void write_instr(char *f, unsigned *instr,int instr_sz,fragS *frag_n, uint32_t addr_mask) 
{
  int addr_mod;

  addr_mod = frag_now_fix () & addr_mask;
  if (frag_n->has_code && frag_n->insn_addr != addr_mod) {
    as_bad (_("instruction address could not be aligned"));
  }
  frag_n->insn_addr = addr_mod;
  // FIXME: figure out what's the problem with "has_code"
  //frag_n->has_code = 1;
  md_big_number_to_chars (f, instr, instr_sz);
}

// Note: This only grows the fragment in order to allocate space, but we don't
// truly allocate the space so that we can handle relaxed branches that are
// actually handled by external, user-supplied code.  Instead, we allocate space
// in handle_fixups, since that's only called once we know we don't have a
// special relocation to handle.
//
// The hack is that we do grow the frag for parallel architectures, due to the
// weird way in which we handle multiple groups.  So, this means that we can't
// properly handle externa relocs, like branch relaxation, and a parallel
// architecture.
//
// Of course, the whole parallel-architecture aspect is a gigantic hack, so we
// probably just need to completely rethink and rewrite this code anyway.
void alloc_instr_buf(InstrInfo &info)
{
  info.f = frag_more(info.opcode->size);
  info.frag = frag_now;
}


void save_instr(InstrBundles &instrInfos,char *s, adl_opcode *opcode, adl_opcode *src_opcode, expressionS *operand_values,
                const Args &args,const char *file_name, int line_number, unsigned pc, int maxfields, int instr_table,
                const char *instr_table_name,const string &eoi_str) 
{
  handle_itable_change(instr_table_name,instr_table);

  InstrInfo &info = instrInfos.add();
  info.init(s,opcode,src_opcode,operand_values,args,file_name,line_number,pc,maxfields,instr_table,instr_table_name);

  instrInfos.add_separator(eoi_str);
}

/* Align a section (I don't know why this is machine dependent).  */
valueT md_section_align (asection *seg,valueT addr)
{
  int align = bfd_get_section_alignment (stdoutput, seg);
  return ((addr + (1 << align) - 1) & (-1 << align));
}

/* Generate a reloc for a fixup.  */
arelent *tc_gen_reloc (asection *seg ATTRIBUTE_UNUSED,fixS *fixp)
{
  arelent *reloc;

  reloc = (arelent *) xmalloc (sizeof (arelent));

  reloc->sym_ptr_ptr = (asymbol **) xmalloc (sizeof (asymbol *));
  *reloc->sym_ptr_ptr = symbol_get_bfdsym (fixp->fx_addsy);
  reloc->address = fixp->fx_frag->fr_address + fixp->fx_where;
  reloc->howto = adl_reloc_type_lookup (stdoutput, fixp->fx_r_type);
  if (reloc->howto == (reloc_howto_type *) NULL)
    {
      AS_BAD_WHERE (fixp->fx_file, fixp->fx_line,
                    _("reloc %d not supported by object file format"),
                    (int) fixp->fx_r_type);
      return NULL;
    }
  reloc->addend = fixp->fx_addnumber;

  return reloc;
}


/* The location from which a PC relative jump should be calculated,
   given a PC relative reloc.  */
long md_pcrel_from_section (fixS *fixp ATTRIBUTE_UNUSED, segT sec ATTRIBUTE_UNUSED)
{
  return fixp->fx_frag->fr_address + fixp->fx_where;
}


ATTRIBUTE_UNUSED void handle_fixups(char *f,fragS *curr_frag,struct adl_fixup *fixups,int num_fixups, int instr_sz)
{
  int i;
  for (i = 0; i != num_fixups; ++i) {
    struct adl_fixup *fixup = &fixups[i];
    fixS *fix = 0;
    if (fixup->reloc != BFD_RELOC_UNUSED) {
      // We have to deal with a relocation.
      int size;
      int offset;

      reloc_howto_type *reloc_howto = adl_reloc_type_lookup (stdoutput, fixup->reloc);

      if (!reloc_howto) {
        as_fatal (_("Unknown relocation %d"),fixup->reloc);
      }

      size = bfd_get_reloc_size (reloc_howto);

      offset = adl_reloc_offset(fixup->reloc, fixup->operand);

      if (size < 1 || size > 16) {
        as_fatal (_("Bad relocation size %d"),size);
      }

      fix = fix_new_exp (curr_frag,
                         f - curr_frag->fr_literal + offset,
                         size,
                         &fixup->exp,
                         reloc_howto->pc_relative,
                         fixup->reloc);

      fix->tc_fix_data.right_shift = reloc_howto->rightshift;
      fix->tc_fix_data.bit_mask    = ~(~0 << reloc_howto->bitsize);

      if ((size + offset) > instr_sz) {
        as_bad (_("Relocation extends past end of instruction.  Too large of a relocation used.")); 
      }

    } else {
      // Bit of a hack here- only relative, local symbols will be converted into
      // simple fixups.  If we have a modifier, we want it to be handled in that
      // way, since we're using a function to adjust it.  So, set the flag to be
      // relative if we have a modifier.
      bool pcrel = (fixup->is_relative || (fixup->operand && fixup->operand->modifier));
      
      // Default case- not a relocation, just a fixup.
      fix = fix_new_exp (curr_frag,
                         f - curr_frag->fr_literal,
                         instr_sz,
                         &fixup->exp,
                         pcrel,
                         BFD_RELOC_UNUSED);
    }
    fix->fx_line                    = fixup->line_number;
    fix->tc_fix_data.operand        = fixup->operand;
    fix->tc_fix_data.opcode         = fixup->opcode;
    fix->tc_fix_data.operand_values = fixup->operand_values;
    fix->tc_fix_data.operand_arg    = fixup->operand_arg;
    fix->tc_fix_data.operand_macro  = fixup->operand_macro;
    fix->tc_fix_data.is_macro       = fixup->is_macro;
    fix->tc_fix_data.instr_size     = fixup->instr_size;
  }
}


void adl_apply_fix (fixS *fixP ,valueT *valP ,segT seg ATTRIBUTE_UNUSED)
{
  valueT value = * valP;

  // Store original relocation data, since we might clear it out for a local
  // label.
  bfd_reloc_code_real_type orig_reloc = fixP->fx_r_type;
  int was_reloc = (fixP->fx_r_type != BFD_RELOC_UNUSED);

  if (fixP->fx_addsy != NULL) {
    /* Hack around bfd_install_relocation brain damage.  */
    if (fixP->fx_pcrel) {
      value += fixP->fx_frag->fr_address + fixP->fx_where;
    }
  } else {
    fixP->fx_done = 1;
    // Local symbol, so we don't use a relocation.
    fixP->fx_r_type = BFD_RELOC_UNUSED;
  }

  if (fixP->fx_subsy != (symbolS *) NULL) {
    /* We can't actually support subtracting a symbol.  */
    AS_BAD_WHERE (fixP->fx_file, fixP->fx_line, _("expression too complex"));
  }

  // Original offset- we stick this into the addend.
  valueT origvalue = value;
  
  int is_reloc = (fixP->fx_r_type != BFD_RELOC_UNUSED);

  // We need to update the value for fixups and for relocations which are not
  // pc-relative.

  // Handle using our generated code if we have installed relocations or this is
  // just an internal fixup.
  if (!is_reloc || adl_have_custom_relocs()) {
    // Just a fixup- we lookup the symbol and insert the location.
    const struct adl_fixup_data *fix_data = (const struct adl_fixup_data *)&(fixP->tc_fix_data);
    const struct adl_operand *operand = fix_data->operand;
    const struct adl_opcode  *opcode  = fix_data->opcode;   // Mutable for macro instruction
    expressionS *operand_values       = (fix_data->operand_values) ? fix_data->operand_values->get() : 0;
    int operand_arg                   = fix_data->operand_arg;                              
    int line_number                   = fixP->fx_line;
    
    adl_operand_macro *operand_macro  = fix_data->operand_macro;
    bool is_macro                     = fix_data->is_macro;
    unsigned instr_size               = fix_data->instr_size;

    char *where;
    unsigned instr[MAX_INSTR_WORDS];
    
    // Fetch the instruction, insert the fully resolved operand
    // value, and stuff the instruction back again.
    where = fixP->fx_frag->fr_literal + fixP->fx_where;

    // Fixups require an offset, but our read/write logic requires the original
    // address of the instruction.  So, here we have to undo the offset in order
    // to read the correct data.  We have to do this even for a local label.  We
    // also apply any actions associated with the relocation, for consistency.
    if (was_reloc) {
      reloc_setter  rs = 0;
      reloc_action  ra = 0;
      int size = 0, inner_size = 0, offset = -1;
      adl_get_reloc_funcs(orig_reloc,&rs,&ra,&size,&inner_size,&offset);
      // If the offset wasn't set, then align with operand bit position.
      if (offset < 0) {
        offset = (operand) ? operand->bitpos/8 : 0;
      }
      where -= offset;

      if (ra) {
        // Invoke the action.  If we're no longer a relocation, then we consider
        // this a linker pass, since we won't be emitting a relocation, and thus
        // this won't be processed again.
        value = ra(value,(!is_reloc));
      }
    }

    if (is_reloc) {
      // Stick in the value, based upon what we know, in order to allow this
      // code to run w/o linking.  It's not perfect, but should cover most
      // cases.
      value = (value >> fix_data->right_shift);
      if (fix_data->bit_mask) {
        value &= fix_data->bit_mask;
      }
    }

    // If little-endian, then left-justify to word boundary.
    if (!target_big_endian && ((instr_size % 4) != 0)) {
      where -= instr_size % 4;
    }
    
    // Make sure that our range is OK, e.g. we want to make sure we detect if
    // somebody is trying to branch to a label that's too far away.  Don't do
    // this if we have a modifier (function)- we assume that the user is smart
    // enough to do their own range checking in that case.
    if (!is_reloc && operand && !operand->modifier && !adl_check_range(value,operand)) {
      AS_BAD_WHERE (fixP->fx_file, fixP->fx_line, _("target is out of the operand's range"));
    }

    int k;
    //FIXME: simplify this code
    if (target_big_endian) {
      for (k = 0; k != MAX_INSTR_WORDS ; ++k) {
        instr[k] = (bfd_getb32 ((unsigned char *) where + k*4));
      }
    } else {
      for (k = 0; k != MAX_INSTR_WORDS ; ++k) {
        instr[k] = (bfd_getl32 ((unsigned char *) where + k*4));
      }
    }

    // Insert the fixed-up value.
    // insert_value (instr, ((uint64_t)value),operand);
    if (operand) {
      if (is_macro && (operand_macro->width < operand_macro->instr_width)) {    // Short-circuit evaluation
        insert_value (instr, ((uint64_t)value), operand, operand_macro); //
      } else {
        insert_value (instr, ((uint64_t)value), operand);
      }
    } else {
      if (fixP->fx_size > 4) {
        if (target_big_endian) {
          instr[0] = (value >> 32);
          instr[1] = (value & 0xffffffff);
        } else {
          instr[1] = (value >> 32);
          instr[0] = (value & 0xffffffff);
        }
      } else {
        *instr = value;
      }
    }

    // Do we have modifiers?  If so, then update all of them.
    if (operand_values) {
      // Stuff the new value into the operand array, so that modifier functions
      // see the new value.
      assert(operand_arg >= 0);
      operand_values[operand_arg].X_add_number = value;
      int i;
      for (i = 0; i != opcode->num_operands; ++i) {
        const struct adl_operand *op = &opcode->operands[i];
        expressionS *ex = &operand_values[op->arg];
        if (op->modifier && ex->X_op != O_symbol) {
          // Modifier function exists- use it.
          // insert_modifier(instr,op,operand_values,line_number);
            if (is_macro) { 
                insert_modifier(instr,op,operand_values,line_number, operand_macro); //
            } else {
                insert_modifier(instr,op,operand_values,line_number);
            }          
        }
      }
    }

    // Now write the value back.
    if (target_big_endian) {
      for (k = 0; k != MAX_INSTR_WORDS ; ++k) {
        bfd_putb32 ((bfd_vma) instr[k], (unsigned char *) where + 4*k);
      }
    } else {
      for (k = 0; k != MAX_INSTR_WORDS ; ++k) {
        bfd_putl32 ((bfd_vma) instr[k], (unsigned char *) where + 4*k);
      }
    }

    bool done = false;
    if (is_reloc || fixP->fx_done) {
      if (is_reloc) {
        *valP = value;
        fixP->fx_addnumber = origvalue;
      }
      done = true;
    }

    if (!done) {
      acb_fixup_operand(fixP,operand);
    }
    
    // Then free this operand-values memory.
    if (fix_data->operand_values && fix_data->operand_values->release()) {
      delete fix_data->operand_values;
    }
      
    // Freeing opcode also free operand (fixups[fc].operand)
    if (is_macro) {                 // opcode points to the "dynamic" copy, so free it
        assert(opcode);             // For fixup or macro, we assert opcode.
        assert(opcode->operands);   // For fixup, we assert opcode->operands.
        free(opcode->operands);
        free((void*)opcode);  
    }
      
    free(operand_macro);    // It is OK to free a null pointer
    

  } else {
    // Called when we don't have a fixup for just an operand.  If we have
    // installed relocations, then skip this.
    if (!adl_have_custom_relocs()) {
      acb_fixup_instr(fixP,valP,seg);
    }
  }

  *valP = value;
  fixP->fx_addnumber = origvalue;
}

extern "C" void free_buffer(unsigned x) { x = 0 ; }

void adl_error(const char *msg,int pos,int group) 
{
  char *file;
  unsigned int line;

  as_where(&file,&line);
  as_bad_where(file,line,msg);
  num_rule_errors++;
}

void adl_info(const char *msg,int pos,int group) 
{
  char *file;
  unsigned int line;
  as_where(&file,&line);
  InstrBundles &infos = instrInfos;
  if (!infos.empty(group)) {
    InstrInfo &info = infos.get_instr(group,pos);
    line = info.line_number;
  }

  as_warn_where(file,line,msg);
}

// Hook to manipulate multiple packets.  This is called immediately before
// post_packet_asm, so it can be used to combine packets, if necessary.
void __attribute__((weak)) acb_process_packets(InstrBundles &instrs,int curgrp,int lastgrp)
{
  // By default, do nothing.
}


// This must be overridden in a user supplied library, if relocations require
// extra handling.
void __attribute__((weak)) acb_handle_reloc_extra(unsigned *instr,unsigned instr_sz,
                                                  bfd_reloc_code_real_type reloc,const adl_opcode *operand,
                                                  const adl_operand *opcode,expressionS *exp)
{
  as_bad(_("Relocation requiring machine-dependent extra handling encountered, but no handler exists.")); 
}

void __attribute__((weak)) acb_handle_convert_frag(bfd *abfd,asection *sec,fragS *fragp)
{
  as_bad(_("No handler specified for frag conversion."));
}

int __attribute__((weak)) acb_estimate_size_before_relax (fragS *fragp,asection *seg)
{
  as_bad(_("No handler specified for relax size estimation."));
  return 0;
}

long __attribute__((weak)) acb_relax_frag (segT segment, fragS *fragP, long stretch)
{
  as_bad(_("No handler specified for frag relaxing."));
  return 0;
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
  return expression(ex);
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

