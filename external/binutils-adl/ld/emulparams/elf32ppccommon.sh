# This has been hacked up to support ADL.
#
# The PLT-agnostic parts of a generic 32-bit ELF PowerPC target.  Included by:
# elf32ppc.sh elf32ppcvxworks.sh elf64ppc.sh
TEMPLATE_NAME=elf32
GENERATE_SHLIB_SCRIPT=yes
GENERATE_PIE_SCRIPT=yes
SCRIPT_NAME=elf
OUTPUT_FORMAT="elf32-powerpc"
NO_REL_RELOCS=yes
TEXT_START_ADDR=0x1000
MAXPAGESIZE="CONSTANT (MAXPAGESIZE)"
COMMONPAGESIZE="CONSTANT (COMMONPAGESIZE)"
ARCH=powerpc:common
MACHINE=
EXECUTABLE_SYMBOLS='PROVIDE (__stack = 0); PROVIDE (___stack = 0);'
if test -z "${CREATE_SHLIB}"; then
  SBSS_START_SYMBOLS="PROVIDE (__sbss_start = .); PROVIDE (___sbss_start = .);"
  SBSS_END_SYMBOLS="PROVIDE (__sbss_end = .); PROVIDE (___sbss_end = .);"
else
  unset SDATA_START_SYMBOLS
  unset SDATA2_START_SYMBOLS
  unset SBSS_START_SYMBOLS
  unset SBSS_END_SYMBOLS
fi
OTHER_END_SYMBOLS="__end = .;"
OTHER_RELRO_SECTIONS="
  .fixup        ${RELOCATING-0} : { *(.fixup) }
  .got1         ${RELOCATING-0} : { *(.got1) }
  .got2         ${RELOCATING-0} : { *(.got2) }
"
OTHER_GOT_RELOC_SECTIONS="
  .rela.got1         ${RELOCATING-0} : { *(.rela.got1) }
  .rela.got2         ${RELOCATING-0} : { *(.rela.got2) }
"
