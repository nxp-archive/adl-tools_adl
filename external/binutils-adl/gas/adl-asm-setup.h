//
// Various setup defines.  This should be included by the processor
// configuration file (tc-<arch>.h).
//

#ifndef _ADL_ASM_SETUP_H_
#define _ADL_ASM_SETUP_H_

// We'll do the to-lower operation outself b/c we have the complexity of
// multiple instructions in a packet.
#define TC_CASE_SENSITIVE

#include <stdbool.h>    // For bool type support

// Fixups get this additional item.
struct adl_fixup_data {
  const struct adl_operand *operand;        // The operand needing the fixup.
  const struct adl_opcode  *opcode;         // Data about the entire instruction. // Mutable for macro instruction
  struct shared_expr       *operand_values; // Operand values, if required to setup dependent operands (may be 0).
  int                       operand_arg;    // Index of operand_values element
                                            // that needs the new value, for
                                            // when modifier functions are used.
  struct adl_operand_macro *operand_macro;  // The related operand in the macro instruction 
  bool                      is_macro;       //
  int                       right_shift;    // Amount to shift final relocation by (drops unwanted low-order bits).
  int                       bit_mask;       // Mask for dropping unwanted higher-order bits from a relocation value.
  unsigned                  instr_size;     // Instruction size, in bytes.
};
#define TC_FIX_TYPE struct adl_fixup_data

// Called by the fixup function for initialization of user data.
#define TC_INIT_FIX_DATA(fixP) { memset(&fixP->tc_fix_data,0,sizeof(struct adl_fixup_data)); }

/* Set the endianness we are using.  Default to big endian.  */
#ifndef TARGET_BYTES_BIG_ENDIAN
#define TARGET_BYTES_BIG_ENDIAN 1
#endif

/* $ is used to refer to the current location.  */
/*#define DOLLAR_DOT*/
/* allows use of $ instead of "0x"*/

#define LITERAL_PREFIXDOLLAR_HEX

#define DIFF_EXPR_OK		/* foo-. gets turned into PC relative relocs */

/* We don't need to handle .word strangely.  */
#define WORKING_DOT_WORD

/* Question marks are permitted in symbol names.  */
#define LEX_QM 1

/* Square and curly brackets are permitted in symbol names.  */
#define LEX_BR 3

#define MAX_LITTLENUMS 4

#define tc_comment_chars comment_chars
extern const char comment_chars[];

#define tc_symbol_chars adl_symbol_chars
extern const char adl_symbol_chars[];


#define tc_parallel_separator_chars adl_parallel_separator_chars
extern const char adl_parallel_separator_chars[];

extern void md_cleanup (void);

/* call md_pcrel_from_section, not md_pcrel_from */
#define MD_PCREL_FROM_SECTION(FIX, SEC) md_pcrel_from_section(FIX, SEC)
extern long md_pcrel_from_section (struct fix *, segT);

#define md_operand(x)

/* Values passed to md_apply_fix don't include symbol values.  */
#define MD_APPLY_SYM_VALUE(FIX) 0

#endif
