/*

Header file for PowerPC for use by an ADL-generated assembler.

*/

#include "adl-asm-setup.h"


#define md_start_line_hook()  (adl_start_line_hook ()); 
#define md_cleanup()  (adl_cleanup ());
#define md_flush_pending_output() (adl_flush_pending_output());
#define tc_frob_label(S) (adl_set_curr_label (S));

#define md_relax_frag(segment, fragp, stretch)  adl_relax_frag (segment, fragp, stretch)

extern long adl_relax_frag (asection *, struct frag *, long);


extern bfd_boolean adl_start_line_hook (void);
extern void adl_cleanup (void);
extern void adl_flush_pending_output (void);
extern void adl_set_curr_label (symbolS *);

/* The target BFD architecture.  */
#define TARGET_ARCH (ppc_arch ())
#define TARGET_MACH (ppc_mach ())
extern enum bfd_architecture ppc_arch (void);
extern unsigned long ppc_mach (void);

/* The target BFD format.  */
#define TARGET_FORMAT (ppc_target_format ())
extern char *ppc_target_format (void);

#define ADL_TARGET_ARCH bfd_arch_powerpc

/* Default target machine type, set by command-line option. */
extern unsigned long default_adl_mach (void);

/* Default target format type, set by command-line option. */
extern char *default_adl_target_format (void);

extern void acb_pop_insert (void);
#define md_pop_insert()		acb_pop_insert ()

extern void adl_after_parse_args(void);
#define md_after_parse_args()		adl_after_parse_args ()

#define TARGET_USE_CFIPOP 1

#define tc_cfi_frame_initial_instructions adl_cfi_frame_initial_instructions
extern void adl_cfi_frame_initial_instructions (void);

extern int dwarf2_line_min_insn_length;

#define DWARF2_LINE_MIN_INSN_LENGTH     dwarf2_line_min_insn_length
#define DWARF2_DEFAULT_RETURN_COLUMN    0x41

// Align on word boundaries until we find that we need to change things.
#define DWARF2_CIE_DATA_ALIGNMENT       -4
