//
// Support code for the ADL assembler.  This is based on PPC code,
// so a different architecture may need to change things in here.
//

#ifndef _MSC_VER
extern "C" {
# include "as.h"
}
#endif

#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <string>

extern "C" {
# include "as.h"
# include "elf/ppc.h"
# include "safe-ctype.h"
#include "dw2gencfi.h"
}

#include "adl-asm-impl.h"

using namespace std;
using namespace adl;

/* What type of shared library support to use.  */
static enum { SHLIB_NONE, SHLIB_PIC, SHLIB_MRELOCATABLE } shlib = SHLIB_NONE;

/* Macros for calculating LO, HI, HA, HIGHER, HIGHERA, HIGHEST,
   HIGHESTA.  */

/* #lo(value) denotes the least significant 16 bits of the indicated.  */
#define PPC_LO(v) ((v) & 0xffff)

/* #hi(value) denotes bits 16 through 31 of the indicated value.  */
#define PPC_HI(v) (((v) >> 16) & 0xffff)

/* #ha(value) denotes the high adjusted value: bits 16 through 31 of
  the indicated value, compensating for #lo() being treated as a
  signed number.  */
#define PPC_HA(v) PPC_HI ((v) + 0x8000)

/* #higher(value) denotes bits 32 through 47 of the indicated value.  */
#define PPC_HIGHER(v) (((v) >> 16 >> 16) & 0xffff)

/* #highera(value) denotes bits 32 through 47 of the indicated value,
   compensating for #lo() being treated as a signed number.  */
#define PPC_HIGHERA(v) PPC_HIGHER ((v) + 0x8000)

/* #highest(value) denotes bits 48 through 63 of the indicated value.  */
#define PPC_HIGHEST(v) (((v) >> 24 >> 24) & 0xffff)

/* #highesta(value) denotes bits 48 through 63 of the indicated value,
   compensating for #lo being treated as a signed number.  */
#define PPC_HIGHESTA(v) PPC_HIGHEST ((v) + 0x8000)

#define SEX16(val) ((((val) & 0xffff) ^ 0x8000) - 0x8000)

/* Whether to target elf64 or elf32.  For legacy purposes, I'm going to keep it
   at 32 for now. */
static unsigned int adl_obj64 = 0;

// Default option parsing.  Calls acb_parse_option as an optional
// extension-library hook.  Also calls acb_march_parse_option for optional
// micro-architecture option processing (-m option stuff).
int adl_parse_option (int c,char *arg)
{
  switch (c) {
  case OPTION_IGNORE_LOWBITS:
    adl_set_check_low_bits(false);
    break;
  case 'a':
    if (strcmp (arg, "64") == 0) {
      adl_obj64 = 1;
    }
    else if (strcmp (arg, "32") == 0) {
      adl_obj64 = 0;
    } else {
      return 0;
    }
    break;
  case 'u':
    /* -u means that any undefined symbols should be treated as
       external, which is the default for gas anyhow.  */
    break;
  case 'K':
    /* Recognize -K PIC.  */
    if (strcmp (arg, "PIC") == 0 || strcmp (arg, "pic") == 0)
      {
        shlib = SHLIB_PIC;
      }
    else
      return 0;
  case 'm':
    // We'll ignore an -mregnames option for compatibility.  We just always
    // allow this functionality.
    if (!strcmp(arg,"regnames")) {
      return 1;
    }
    else if (strcmp (arg, "little") == 0 || strcmp (arg, "little-endian") == 0) {
      target_big_endian = 0;
    }
    else if (strcmp (arg, "big") == 0 || strcmp (arg, "big-endian") == 0) {
      target_big_endian = 1;
    }
    else if (strcmp (arg, "no-rules") == 0) {
      adl_set_check_asm_rules(0);
    }
    else if (acb_march_parse_option(arg)) {
      // Return an error if the optional hook returns an error.
      return 1;
    }
    break;
  case 'l':
    /* Solaris as takes -le (presumably for little endian).  For completeness
       sake, recognize -be also.  */
    if (strcmp (arg, "e") == 0)
      {
        target_big_endian = 0;
      }
    else
      return 0;
    break;
  case 'b':
    if (strcmp (arg, "e") == 0)
      {
        target_big_endian = 1;
      }
    else
      return 0;
    break;

  default:
    return acb_parse_option(c,arg);
  }

  return 1;
}

// Do any processing immediately after handling the options.
void adl_after_parse_args ()
{
  acb_after_parse_args();
}

extern "C" const bfd_arch_info_type bfd_adl_arch;

// We grab the machine from the architecture definition we have in the created
// disassembler.
unsigned long default_adl_mach()
{
  return bfd_adl_arch.mach;
}

extern char *default_adl_target_format ()
{
  return (char*)(target_big_endian
	  ? (adl_obj64 ? "elf64-powerpc" : "elf32-powerpc")
	  : (adl_obj64 ? "elf64-powerpcle" : "elf32-powerpcle"));
}

void acb_show_usage(FILE *);

extern string adl_asm_version;

void adl_show_usage (FILE *stream ATTRIBUTE_UNUSED)
{
  fprintf (stream, "\
--ignore-low-bits		Ignore low bits in operands even if they should be zero.\n\
-a32			Generate ELF32\n\
-a64			Generate ELF64\n\
-mno-rules		Turn off assembly rule checking\n\
-mlittle, -mlittle-endian, -le\n\
			Generate code for a little endian machine\n\
-mbig, -mbig-endian, -be\n\
			Generate code for a big endian machine\n");

  // Add on any extension library option help.
  acb_show_usage(stream);
  if (!adl_asm_version.empty()) {
    fprintf(stream,"\nADL generated assembler version %s\n",adl_asm_version.c_str());
  }
}

/* Parse @got, etc. and return the desired relocation.  */
static bfd_reloc_code_real_type ppc_elf_suffix (char **str_p, expressionS *exp_p)
{
  struct map_bfd {
    const char *string;
    unsigned int length : 8;
    unsigned int valid32 : 1;
    unsigned int valid64 : 1;
    unsigned int reloc;
  };

  char ident[20];
  char *str = *str_p;
  char *str2;
  int ch;
  int len;
  const struct map_bfd *ptr;

#define MAP(str, reloc)   { str, sizeof (str) - 1, 1, 1, reloc }
#define MAP32(str, reloc) { str, sizeof (str) - 1, 1, 0, reloc }
#define MAP64(str, reloc) { str, sizeof (str) - 1, 0, 1, reloc }

  static const struct map_bfd mapping[] = {
    MAP ("l",			BFD_RELOC_LO16),
    MAP ("h",			BFD_RELOC_HI16),
    MAP ("ha",			BFD_RELOC_HI16_S),
    MAP ("brtaken",		BFD_RELOC_PPC_B16_BRTAKEN),
    MAP ("brntaken",		BFD_RELOC_PPC_B16_BRNTAKEN),
    MAP ("got",			BFD_RELOC_16_GOTOFF),
    MAP ("got@l",		BFD_RELOC_LO16_GOTOFF),
    MAP ("got@h",		BFD_RELOC_HI16_GOTOFF),
    MAP ("got@ha",		BFD_RELOC_HI16_S_GOTOFF),
    MAP ("plt@l",		BFD_RELOC_LO16_PLTOFF),
    MAP ("plt@h",		BFD_RELOC_HI16_PLTOFF),
    MAP ("plt@ha",		BFD_RELOC_HI16_S_PLTOFF),
    MAP ("copy",		BFD_RELOC_PPC_COPY),
    MAP ("globdat",		BFD_RELOC_PPC_GLOB_DAT),
    MAP ("sectoff",		BFD_RELOC_16_BASEREL),
    MAP ("sectoff@l",		BFD_RELOC_LO16_BASEREL),
    MAP ("sectoff@h",		BFD_RELOC_HI16_BASEREL),
    MAP ("sectoff@ha",		BFD_RELOC_HI16_S_BASEREL),
    MAP ("tls",			BFD_RELOC_PPC_TLS),
    MAP ("dtpmod",		BFD_RELOC_PPC_DTPMOD),
    MAP ("dtprel",		BFD_RELOC_PPC_DTPREL),
    MAP ("dtprel@l",		BFD_RELOC_PPC_DTPREL16_LO),
    MAP ("dtprel@h",		BFD_RELOC_PPC_DTPREL16_HI),
    MAP ("dtprel@ha",		BFD_RELOC_PPC_DTPREL16_HA),
    MAP ("tprel",		BFD_RELOC_PPC_TPREL),
    MAP ("tprel@l",		BFD_RELOC_PPC_TPREL16_LO),
    MAP ("tprel@h",		BFD_RELOC_PPC_TPREL16_HI),
    MAP ("tprel@ha",		BFD_RELOC_PPC_TPREL16_HA),
    MAP ("got@tlsgd",		BFD_RELOC_PPC_GOT_TLSGD16),
    MAP ("got@tlsgd@l",		BFD_RELOC_PPC_GOT_TLSGD16_LO),
    MAP ("got@tlsgd@h",		BFD_RELOC_PPC_GOT_TLSGD16_HI),
    MAP ("got@tlsgd@ha",	BFD_RELOC_PPC_GOT_TLSGD16_HA),
    MAP ("got@tlsld",		BFD_RELOC_PPC_GOT_TLSLD16),
    MAP ("got@tlsld@l",		BFD_RELOC_PPC_GOT_TLSLD16_LO),
    MAP ("got@tlsld@h",		BFD_RELOC_PPC_GOT_TLSLD16_HI),
    MAP ("got@tlsld@ha",	BFD_RELOC_PPC_GOT_TLSLD16_HA),
    MAP ("got@dtprel",		BFD_RELOC_PPC_GOT_DTPREL16),
    MAP ("got@dtprel@l",	BFD_RELOC_PPC_GOT_DTPREL16_LO),
    MAP ("got@dtprel@h",	BFD_RELOC_PPC_GOT_DTPREL16_HI),
    MAP ("got@dtprel@ha",	BFD_RELOC_PPC_GOT_DTPREL16_HA),
    MAP ("got@tprel",		BFD_RELOC_PPC_GOT_TPREL16),
    MAP ("got@tprel@l",		BFD_RELOC_PPC_GOT_TPREL16_LO),
    MAP ("got@tprel@h",		BFD_RELOC_PPC_GOT_TPREL16_HI),
    MAP ("got@tprel@ha",	BFD_RELOC_PPC_GOT_TPREL16_HA),
    MAP32 ("fixup",		BFD_RELOC_CTOR),
    MAP32 ("plt",		BFD_RELOC_24_PLT_PCREL),
    MAP32 ("pltrel24",		BFD_RELOC_24_PLT_PCREL),
    MAP32 ("local24pc",		BFD_RELOC_PPC_LOCAL24PC),
    MAP32 ("local",		BFD_RELOC_PPC_LOCAL24PC),
    MAP32 ("pltrel",		BFD_RELOC_32_PLT_PCREL),
    MAP32 ("sdarel",		BFD_RELOC_GPREL16),
    MAP32 ("naddr",		BFD_RELOC_PPC_EMB_NADDR32),
    MAP32 ("naddr16",		BFD_RELOC_PPC_EMB_NADDR16),
    MAP32 ("naddr@l",		BFD_RELOC_PPC_EMB_NADDR16_LO),
    MAP32 ("naddr@h",		BFD_RELOC_PPC_EMB_NADDR16_HI),
    MAP32 ("naddr@ha",		BFD_RELOC_PPC_EMB_NADDR16_HA),
    MAP32 ("sdai16",		BFD_RELOC_PPC_EMB_SDAI16),
    MAP32 ("sda2rel",		BFD_RELOC_PPC_EMB_SDA2REL),
    MAP32 ("sda2i16",		BFD_RELOC_PPC_EMB_SDA2I16),
    MAP32 ("sda21",		BFD_RELOC_PPC_EMB_SDA21),
    MAP32 ("mrkref",		BFD_RELOC_PPC_EMB_MRKREF),
    MAP32 ("relsect",		BFD_RELOC_PPC_EMB_RELSEC16),
    MAP32 ("relsect@l",		BFD_RELOC_PPC_EMB_RELST_LO),
    MAP32 ("relsect@h",		BFD_RELOC_PPC_EMB_RELST_HI),
    MAP32 ("relsect@ha",	BFD_RELOC_PPC_EMB_RELST_HA),
    MAP32 ("bitfld",		BFD_RELOC_PPC_EMB_BIT_FLD),
    MAP32 ("relsda",		BFD_RELOC_PPC_EMB_RELSDA),
    MAP32 ("xgot",		BFD_RELOC_PPC_TOC16),
    MAP64 ("higher",		BFD_RELOC_PPC64_HIGHER),
    MAP64 ("highera",		BFD_RELOC_PPC64_HIGHER_S),
    MAP64 ("highest",		BFD_RELOC_PPC64_HIGHEST),
    MAP64 ("highesta",		BFD_RELOC_PPC64_HIGHEST_S),
    MAP64 ("tocbase",		BFD_RELOC_PPC64_TOC),
    MAP64 ("toc",		BFD_RELOC_PPC_TOC16),
    MAP64 ("toc@l",		BFD_RELOC_PPC64_TOC16_LO),
    MAP64 ("toc@h",		BFD_RELOC_PPC64_TOC16_HI),
    MAP64 ("toc@ha",		BFD_RELOC_PPC64_TOC16_HA),
    MAP64 ("dtprel@higher",	BFD_RELOC_PPC64_DTPREL16_HIGHER),
    MAP64 ("dtprel@highera",	BFD_RELOC_PPC64_DTPREL16_HIGHERA),
    MAP64 ("dtprel@highest",	BFD_RELOC_PPC64_DTPREL16_HIGHEST),
    MAP64 ("dtprel@highesta",	BFD_RELOC_PPC64_DTPREL16_HIGHESTA),
    MAP64 ("tprel@higher",	BFD_RELOC_PPC64_TPREL16_HIGHER),
    MAP64 ("tprel@highera",	BFD_RELOC_PPC64_TPREL16_HIGHERA),
    MAP64 ("tprel@highest",	BFD_RELOC_PPC64_TPREL16_HIGHEST),
    MAP64 ("tprel@highesta",	BFD_RELOC_PPC64_TPREL16_HIGHESTA),
    { (char *) 0, 0, 0, 0,	BFD_RELOC_UNUSED }
  };

  while (*str  && *str != '@') {
    ++str;
  }

  if (!*str || *str++ != '@')
    return BFD_RELOC_UNUSED;

  for (ch = *str, str2 = ident;
       (str2 < ident + sizeof (ident) - 1
        && (ISALNUM (ch) || ch == '@'));
       ch = *++str) {
    *str2++ = TOLOWER (ch);
  }
  
  *str2 = '\0';
  len = str2 - ident;

  ch = ident[0];
  for (ptr = &mapping[0]; ptr->length > 0; ptr++) {
    if (ch == ptr->string[0]
        && len == (int)ptr->length
        && memcmp (ident, ptr->string, ptr->length) == 0
        && ptr->valid32) {
      int reloc = ptr->reloc;

      if (exp_p->X_add_number != 0 && 
          (reloc == (int) BFD_RELOC_16_GOTOFF
           || reloc == (int) BFD_RELOC_LO16_GOTOFF
           || reloc == (int) BFD_RELOC_HI16_GOTOFF
           || reloc == (int) BFD_RELOC_HI16_S_GOTOFF)) {
        as_warn (_("identifier+constant@got means identifier@got+constant"));
      }

      /* Now check for identifier@suffix+constant.  */
      if (*str == '-' || *str == '+') {
        char *orig_line = input_line_pointer;
        expressionS new_exp;

        input_line_pointer = str;
        expression (&new_exp);
        if (new_exp.X_op == O_constant)
          {
            exp_p->X_add_number += new_exp.X_add_number;
            str = input_line_pointer;
          }

        if (&input_line_pointer != str_p)
          input_line_pointer = orig_line;
      }
      *str_p = str;

      if (reloc == (int) BFD_RELOC_PPC64_TOC
          && exp_p->X_op == O_symbol
          && strcmp (S_GET_NAME (exp_p->X_add_symbol), ".TOC.") == 0) {
        /* Change the symbol so that the dummy .TOC. symbol can be
           omitted from the object file.  */
        exp_p->X_add_symbol = &abs_symbol;
      }

      return (bfd_reloc_code_real_type) reloc;
    }
  }

  return BFD_RELOC_UNUSED;
}

void acb_const_hook(char *str,expressionS *ex,const adl_operand *operand)
{
  /* Allow @HA, @L, @H on constants.  */
  char *orig_str = str;
  bfd_reloc_code_real_type reloc;

  if ((reloc = ppc_elf_suffix (&str, ex)) != BFD_RELOC_UNUSED) {
    switch (reloc) {
    default:
      str = orig_str;
      break;

    case BFD_RELOC_LO16:
      /* X_unsigned is the default, so if the user has done
         something which cleared it, we always produce a
         signed value.  */
      if (ex->X_unsigned && ! (operand->flags & ADL_SIGNED))
        ex->X_add_number &= 0xffff;
      else
        ex->X_add_number = SEX16 (ex->X_add_number);
      break;

    case BFD_RELOC_HI16:
      if (ex->X_unsigned && ! (operand->flags & ADL_SIGNED))
        ex->X_add_number = PPC_HI (ex->X_add_number);
      else
        ex->X_add_number = SEX16 (PPC_HI (ex->X_add_number));
      break;

    case BFD_RELOC_HI16_S:
      if (ex->X_unsigned && ! (operand->flags & ADL_SIGNED))
        ex->X_add_number = PPC_HA (ex->X_add_number);
      else
        ex->X_add_number = SEX16 (PPC_HA (ex->X_add_number));
      break;

    case BFD_RELOC_PPC64_HIGHER:
      if (ex->X_unsigned && ! (operand->flags & ADL_SIGNED))
        ex->X_add_number = PPC_HIGHER (ex->X_add_number);
      else
        ex->X_add_number = SEX16 (PPC_HIGHER (ex->X_add_number));
      break;

    case BFD_RELOC_PPC64_HIGHER_S:
      if (ex->X_unsigned && ! (operand->flags & ADL_SIGNED))
        ex->X_add_number = PPC_HIGHERA (ex->X_add_number);
      else
        ex->X_add_number = SEX16 (PPC_HIGHERA (ex->X_add_number));
      break;

    case BFD_RELOC_PPC64_HIGHEST:
      if (ex->X_unsigned && ! (operand->flags & ADL_SIGNED))
        ex->X_add_number = PPC_HIGHEST (ex->X_add_number);
      else
        ex->X_add_number = SEX16 (PPC_HIGHEST (ex->X_add_number));
      break;

    case BFD_RELOC_PPC64_HIGHEST_S:
      if (ex->X_unsigned && ! (operand->flags & ADL_SIGNED))
        ex->X_add_number = PPC_HIGHESTA (ex->X_add_number);
      else
        ex->X_add_number = SEX16 (PPC_HIGHESTA (ex->X_add_number));
      break;
    }
  }
}

pair<bfd_reloc_code_real_type,bool> acb_reloc_hook(const string &arg,expressionS *ex,const struct adl_operand *operand)
{
  char *str = (char*)arg.c_str();
  
  bfd_reloc_code_real_type reloc = BFD_RELOC_UNUSED; 

  if ((reloc = ppc_elf_suffix (&str, ex)) != BFD_RELOC_UNUSED) {
    /* Some TLS tweaks.  */
    switch (reloc) {
    default:
      break;
      /* We'll only use the 32 (or 64) bit form of these relocations
         in constants.  Instructions get the 16 bit form.  */
    case BFD_RELOC_PPC_DTPREL:
      reloc = BFD_RELOC_PPC_DTPREL16;
      break;
    case BFD_RELOC_PPC_TPREL:
      reloc = BFD_RELOC_PPC_TPREL16;
      break;
    }

    /* For the absolute forms of branches, convert the PC
       relative form back into the absolute.  */
    if ((operand->flags & ADL_ABSOLUTE)) {
      switch (reloc) {
      case BFD_RELOC_PPC_B26:
        reloc = BFD_RELOC_PPC_BA26;
        break;
      case BFD_RELOC_PPC_B16:
        reloc = BFD_RELOC_PPC_BA16;
        break;
      case BFD_RELOC_PPC_B16_BRTAKEN:
        reloc = BFD_RELOC_PPC_BA16_BRTAKEN;
        break;
      case BFD_RELOC_PPC_B16_BRNTAKEN:
        reloc = BFD_RELOC_PPC_BA16_BRNTAKEN;
        break;
      default:
        break;
      }
    }

  }
  return make_pair(reloc,false);
}

/* Validate any relocations emitted for -mrelocatable, possibly adding
   fixups for word relocations in writable segments, so we can adjust
   them at runtime.  */
static void ppc_elf_validate_fix (fixS *fixp, segT seg)
{
  if (fixp->fx_done || fixp->fx_pcrel)
    return;

  switch (shlib)
    {
    case SHLIB_NONE:
    case SHLIB_PIC:
      return;

    case SHLIB_MRELOCATABLE:
      if (fixp->fx_r_type <= BFD_RELOC_UNUSED
	  && fixp->fx_r_type != BFD_RELOC_16_GOTOFF
	  && fixp->fx_r_type != BFD_RELOC_HI16_GOTOFF
	  && fixp->fx_r_type != BFD_RELOC_LO16_GOTOFF
	  && fixp->fx_r_type != BFD_RELOC_HI16_S_GOTOFF
	  && fixp->fx_r_type != BFD_RELOC_16_BASEREL
	  && fixp->fx_r_type != BFD_RELOC_LO16_BASEREL
	  && fixp->fx_r_type != BFD_RELOC_HI16_BASEREL
	  && fixp->fx_r_type != BFD_RELOC_HI16_S_BASEREL
	  && (seg->flags & SEC_LOAD) != 0
	  && strcmp (segment_name (seg), ".got2") != 0
	  && strcmp (segment_name (seg), ".dtors") != 0
	  && strcmp (segment_name (seg), ".ctors") != 0
	  && strcmp (segment_name (seg), ".fixup") != 0
	  && strcmp (segment_name (seg), ".gcc_except_table") != 0
	  && strcmp (segment_name (seg), ".eh_frame") != 0
	  && strcmp (segment_name (seg), ".ex_shared") != 0)
	{
	  if ((seg->flags & (SEC_READONLY | SEC_CODE)) != 0
	      || fixp->fx_r_type != BFD_RELOC_CTOR)
	    {
	      as_bad_where (fixp->fx_file, fixp->fx_line,
			    _("Relocation cannot be done when using -mrelocatable"));
	    }
	}
      return;
    }
}

void acb_fixup_operand(fixS *fixP,const struct adl_operand *operand)
{
  assert (fixP->fx_addsy != NULL);

  /* Determine a BFD reloc value based on the operand information.
     We are only prepared to turn a few of the operands into
     relocs.  */
  if ((operand->flags & ADL_RELATIVE)
      && operand->ptr->width == 24
      && operand->leftshift == 2)
    fixP->fx_r_type = BFD_RELOC_PPC_B26;
  else if ((operand->flags & ADL_RELATIVE)
           && operand->ptr->width == 16
           && operand->leftshift == 0)
    {
      fixP->fx_r_type = BFD_RELOC_PPC_B16;
    }
  else if ((operand->flags & ADL_ABSOLUTE)
           && operand->ptr->width == 24
           && operand->leftshift == 2)
    fixP->fx_r_type = BFD_RELOC_PPC_BA26;
  else if ((operand->flags & ADL_ABSOLUTE)
           && operand->leftshift == 0)
    {
      fixP->fx_r_type = BFD_RELOC_PPC_BA16;
    }
  else
    {
      char *sfile;
      unsigned int sline;

      /* Use expr_symbol_where to see if this is an expression
         symbol.  */
      if (expr_symbol_where (fixP->fx_addsy, &sfile, &sline))
        as_bad_where (fixP->fx_file, fixP->fx_line,
                      _("unresolved expression that must be resolved"));
      else
        as_bad_where (fixP->fx_file, fixP->fx_line,
                      _("unsupported relocation against %s"),
                      S_GET_NAME (fixP->fx_addsy));
      
		     fixP->fx_done = 1;
      return;
    }
}

void acb_fixup_instr(fixS *fixP,valueT *valP,segT seg)
{
  valueT value = * valP;

  ppc_elf_validate_fix (fixP, seg);
  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_CTOR:
      /* fall through */

    case BFD_RELOC_32:
      if (fixP->fx_pcrel)
        fixP->fx_r_type = BFD_RELOC_32_PCREL;
      /* fall through */

    case BFD_RELOC_RVA:
    case BFD_RELOC_32_PCREL:
    case BFD_RELOC_PPC_EMB_NADDR32:
      md_number_to_chars (fixP->fx_frag->fr_literal + fixP->fx_where,
                          value, 4);
      break;

    case BFD_RELOC_64_PCREL:
      md_number_to_chars (fixP->fx_frag->fr_literal + fixP->fx_where,
                          value, 8);
      break;

    case BFD_RELOC_GPREL16:
    case BFD_RELOC_16_GOT_PCREL:
    case BFD_RELOC_16_GOTOFF:
    case BFD_RELOC_LO16_GOTOFF:
    case BFD_RELOC_HI16_GOTOFF:
    case BFD_RELOC_HI16_S_GOTOFF:
    case BFD_RELOC_16_BASEREL:
    case BFD_RELOC_LO16_BASEREL:
    case BFD_RELOC_HI16_BASEREL:
    case BFD_RELOC_HI16_S_BASEREL:
    case BFD_RELOC_PPC_EMB_NADDR16:
    case BFD_RELOC_PPC_EMB_NADDR16_LO:
    case BFD_RELOC_PPC_EMB_NADDR16_HI:
    case BFD_RELOC_PPC_EMB_NADDR16_HA:
    case BFD_RELOC_PPC_EMB_SDAI16:
    case BFD_RELOC_PPC_EMB_SDA2REL:
    case BFD_RELOC_PPC_EMB_SDA2I16:
    case BFD_RELOC_PPC_EMB_RELSEC16:
    case BFD_RELOC_PPC_EMB_RELST_LO:
    case BFD_RELOC_PPC_EMB_RELST_HI:
    case BFD_RELOC_PPC_EMB_RELST_HA:
    case BFD_RELOC_PPC_EMB_RELSDA:
    case BFD_RELOC_PPC_TOC16:
    case BFD_RELOC_PPC64_TOC16_LO:
    case BFD_RELOC_PPC64_TOC16_HI:
    case BFD_RELOC_PPC64_TOC16_HA:
      if (fixP->fx_pcrel)
        {
          if (fixP->fx_addsy != NULL)
            as_bad_where (fixP->fx_file, fixP->fx_line,
                          _("cannot emit PC relative %s relocation against %s"),
                          bfd_get_reloc_code_name (fixP->fx_r_type),
                          S_GET_NAME (fixP->fx_addsy));
          else
            as_bad_where (fixP->fx_file, fixP->fx_line,
                          _("cannot emit PC relative %s relocation"),
                          bfd_get_reloc_code_name (fixP->fx_r_type));
        }

      md_number_to_chars (fixP->fx_frag->fr_literal + fixP->fx_where,
                          value, 2);
      break;

    case BFD_RELOC_16:
      if (fixP->fx_pcrel)
        fixP->fx_r_type = BFD_RELOC_16_PCREL;
      /* fall through */

    case BFD_RELOC_16_PCREL:
      md_number_to_chars (fixP->fx_frag->fr_literal + fixP->fx_where,
                          value, 2);
      break;

    case BFD_RELOC_LO16:
      if (fixP->fx_pcrel)
        fixP->fx_r_type = BFD_RELOC_LO16_PCREL;
      /* fall through */

    case BFD_RELOC_LO16_PCREL:
      md_number_to_chars (fixP->fx_frag->fr_literal + fixP->fx_where,
                          value, 2);
      break;

      /* This case happens when you write, for example,
         lis %r3,(L1-L2)@ha
         where L1 and L2 are defined later.  */
    case BFD_RELOC_HI16:
      if (fixP->fx_pcrel)
        fixP->fx_r_type = BFD_RELOC_HI16_PCREL;
      /* fall through */

    case BFD_RELOC_HI16_PCREL:
      md_number_to_chars (fixP->fx_frag->fr_literal + fixP->fx_where,
                          PPC_HI (value), 2);
      break;

    case BFD_RELOC_HI16_S:
      if (fixP->fx_pcrel)
        fixP->fx_r_type = BFD_RELOC_HI16_S_PCREL;
      /* fall through */

    case BFD_RELOC_HI16_S_PCREL:
      md_number_to_chars (fixP->fx_frag->fr_literal + fixP->fx_where,
                          PPC_HA (value), 2);
      break;

    case BFD_RELOC_PPC64_HIGHER:
      if (fixP->fx_pcrel)
        abort ();
      md_number_to_chars (fixP->fx_frag->fr_literal + fixP->fx_where,
                          PPC_HIGHER (value), 2);
      break;

    case BFD_RELOC_PPC64_HIGHER_S:
      if (fixP->fx_pcrel)
        abort ();
      md_number_to_chars (fixP->fx_frag->fr_literal + fixP->fx_where,
                          PPC_HIGHERA (value), 2);
      break;

    case BFD_RELOC_PPC64_HIGHEST:
      if (fixP->fx_pcrel)
        abort ();
      md_number_to_chars (fixP->fx_frag->fr_literal + fixP->fx_where,
                          PPC_HIGHEST (value), 2);
      break;

    case BFD_RELOC_PPC64_HIGHEST_S:
      if (fixP->fx_pcrel)
        abort ();
      md_number_to_chars (fixP->fx_frag->fr_literal + fixP->fx_where,
                          PPC_HIGHESTA (value), 2);
      break;

    case BFD_RELOC_PPC64_ADDR16_DS:
    case BFD_RELOC_PPC64_ADDR16_LO_DS:
    case BFD_RELOC_PPC64_GOT16_DS:
    case BFD_RELOC_PPC64_GOT16_LO_DS:
    case BFD_RELOC_PPC64_PLT16_LO_DS:
    case BFD_RELOC_PPC64_SECTOFF_DS:
    case BFD_RELOC_PPC64_SECTOFF_LO_DS:
    case BFD_RELOC_PPC64_TOC16_DS:
    case BFD_RELOC_PPC64_TOC16_LO_DS:
    case BFD_RELOC_PPC64_PLTGOT16_DS:
    case BFD_RELOC_PPC64_PLTGOT16_LO_DS:
      if (fixP->fx_pcrel)
        abort ();
      {
        char *where = fixP->fx_frag->fr_literal + fixP->fx_where;
        unsigned long val, mask;

        if (target_big_endian)
          val = bfd_getb32 (where - 2);
        else
          val = bfd_getl32 (where);
        mask = 0xfffc;
        val |= value & mask;
        if (target_big_endian)
          bfd_putb16 ((bfd_vma) val, where);
        else
          bfd_putl16 ((bfd_vma) val, where);
      }
      break;

    case BFD_RELOC_PPC_B16_BRTAKEN:
    case BFD_RELOC_PPC_B16_BRNTAKEN:
    case BFD_RELOC_PPC_BA16_BRTAKEN:
    case BFD_RELOC_PPC_BA16_BRNTAKEN:
      break;

    case BFD_RELOC_PPC_TLS:
      break;

    case BFD_RELOC_PPC_DTPMOD:
    case BFD_RELOC_PPC_TPREL16:
    case BFD_RELOC_PPC_TPREL16_LO:
    case BFD_RELOC_PPC_TPREL16_HI:
    case BFD_RELOC_PPC_TPREL16_HA:
    case BFD_RELOC_PPC_TPREL:
    case BFD_RELOC_PPC_DTPREL16:
    case BFD_RELOC_PPC_DTPREL16_LO:
    case BFD_RELOC_PPC_DTPREL16_HI:
    case BFD_RELOC_PPC_DTPREL16_HA:
    case BFD_RELOC_PPC_DTPREL:
    case BFD_RELOC_PPC_GOT_TLSGD16:
    case BFD_RELOC_PPC_GOT_TLSGD16_LO:
    case BFD_RELOC_PPC_GOT_TLSGD16_HI:
    case BFD_RELOC_PPC_GOT_TLSGD16_HA:
    case BFD_RELOC_PPC_GOT_TLSLD16:
    case BFD_RELOC_PPC_GOT_TLSLD16_LO:
    case BFD_RELOC_PPC_GOT_TLSLD16_HI:
    case BFD_RELOC_PPC_GOT_TLSLD16_HA:
    case BFD_RELOC_PPC_GOT_TPREL16:
    case BFD_RELOC_PPC_GOT_TPREL16_LO:
    case BFD_RELOC_PPC_GOT_TPREL16_HI:
    case BFD_RELOC_PPC_GOT_TPREL16_HA:
    case BFD_RELOC_PPC_GOT_DTPREL16:
    case BFD_RELOC_PPC_GOT_DTPREL16_LO:
    case BFD_RELOC_PPC_GOT_DTPREL16_HI:
    case BFD_RELOC_PPC_GOT_DTPREL16_HA:
    case BFD_RELOC_PPC64_TPREL16_DS:
    case BFD_RELOC_PPC64_TPREL16_LO_DS:
    case BFD_RELOC_PPC64_TPREL16_HIGHER:
    case BFD_RELOC_PPC64_TPREL16_HIGHERA:
    case BFD_RELOC_PPC64_TPREL16_HIGHEST:
    case BFD_RELOC_PPC64_TPREL16_HIGHESTA:
    case BFD_RELOC_PPC64_DTPREL16_DS:
    case BFD_RELOC_PPC64_DTPREL16_LO_DS:
    case BFD_RELOC_PPC64_DTPREL16_HIGHER:
    case BFD_RELOC_PPC64_DTPREL16_HIGHERA:
    case BFD_RELOC_PPC64_DTPREL16_HIGHEST:
    case BFD_RELOC_PPC64_DTPREL16_HIGHESTA:
      S_SET_THREAD_LOCAL (fixP->fx_addsy);
      break;

      /* Because SDA21 modifies the register field, the size is set to 4
         bytes, rather than 2, so offset it here appropriately.  */
    case BFD_RELOC_PPC_EMB_SDA21:
      if (fixP->fx_pcrel)
        abort ();

      md_number_to_chars (fixP->fx_frag->fr_literal + fixP->fx_where
                          + ((target_big_endian) ? 2 : 0),
                          value, 2);
      break;

    case BFD_RELOC_8:
      if (fixP->fx_pcrel)
        {
          /* This can occur if there is a bug in the input assembler, eg:
             ".byte <undefined_symbol> - ."  */
          if (fixP->fx_addsy)
            as_bad (_("Unable to handle reference to symbol %s"),
                    S_GET_NAME (fixP->fx_addsy));
          else
            as_bad (_("Unable to resolve expression"));
          fixP->fx_done = 1;
        }
      else
        md_number_to_chars (fixP->fx_frag->fr_literal + fixP->fx_where,
                            value, 1);
      break;

    case BFD_RELOC_24_PLT_PCREL:
    case BFD_RELOC_PPC_LOCAL24PC:
      if (!fixP->fx_pcrel && !fixP->fx_done)
        abort ();

      if (fixP->fx_done)
        {
          char *where;
          unsigned long insn;

          /* Fetch the instruction, insert the fully resolved operand
             value, and stuff the instruction back again.  */
          where = fixP->fx_frag->fr_literal + fixP->fx_where;
          if (target_big_endian)
            insn = bfd_getb32 ((unsigned char *) where);
          else
            insn = bfd_getl32 ((unsigned char *) where);
          if ((value & 3) != 0)
            as_bad_where (fixP->fx_file, fixP->fx_line,
                          _("must branch to an address a multiple of 4"));
          if ((offsetT) value < -0x40000000
              || (offsetT) value >= 0x40000000)
            as_bad_where (fixP->fx_file, fixP->fx_line,
                          _("@local or @plt branch destination is too far away, %ld bytes"),
                          (long) value);
          insn = insn | (value & 0x03fffffc);
          if (target_big_endian)
            bfd_putb32 ((bfd_vma) insn, (unsigned char *) where);
          else
            bfd_putl32 ((bfd_vma) insn, (unsigned char *) where);
        }
      break;

    case BFD_RELOC_VTABLE_INHERIT:
      fixP->fx_done = 0;
      if (fixP->fx_addsy
          && !S_IS_DEFINED (fixP->fx_addsy)
          && !S_IS_WEAK (fixP->fx_addsy))
        S_SET_WEAK (fixP->fx_addsy);
      break;

    case BFD_RELOC_VTABLE_ENTRY:
      fixP->fx_done = 0;
      break;

      /* Generated by reference to `sym@tocbase'.  The sym is
         ignored by the linker.  */
    case BFD_RELOC_PPC64_TOC:
      fixP->fx_done = 0;
      break;

    default:
      fprintf (stderr,
               _("Gas failure, reloc value %d\n"), fixP->fx_r_type);
      fflush (stderr);
      abort ();
    }
}

// Just call to an externally supplied handler.
long adl_relax_frag (segT segment, fragS *fragP, long stretch)
{
  return acb_relax_frag(segment,fragP,stretch);
}

void adl_cfi_frame_initial_instructions (void)
{
  cfi_add_CFA_def_cfa (1, 0);
}
