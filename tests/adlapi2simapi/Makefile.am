
AUTOMAKE_OPTIONS = foreign serial-tests

TESTS = regress

check_PROGRAMS = binutils-tests$(EXEEXT)

EXTRA_DIST = \
	$(srcdir)/simple.eld \
	$(srcdir)/regress \
	$(srcdir)/c1.cli \
	$(srcdir)/c1.regress \
	$(srcdir)/*.s

if !IS_EXTERNAL_RELEASE

EXTRA_DIST += sc3900_arch_runsim_cpp.adl

endif

CLEANFILES = *.cc

.PHONY: binutils-tests$(EXEEXT)

SCRIPT   = $(top_builddir)/scripts
BINBUILD = $(top_builddir)/external/binutils-adl
BINSRC   = $(top_srcdir)/external/binutils-adl
ASMFLAGS =
Asm      = $(top_builddir)/asm

ASMCOMPILE = $(SCRIPT)/adl2asm $(ASMFLAGS) -shared --prefix=$(top_builddir) $< \
              --bprefix=$(BINBUILD) --bsprefix=$(BINSRC) --cflags=-O0

binutils-tests$(EXEEXT): adl_asm.so

binutils_tests_SOURCES = 

if RUNSIM

if !IS_EXTERNAL_RELEASE

run:
	LD_LIBRARY_PATH=$(top_builddir)/adlapi2simapi/src $(RUNSIM_PATH)/runsim -d sc3400adl -fstcfg "-script test.cli print_stat foo bar -escript" simple.eld

endif !IS_EXTERNAL_RELEASE

endif RUNSIM

if DTASM

if IS_EXTERNAL_RELEASE

adl_asm.so:

endif IS_EXTERNAL_RELEASE

if !IS_EXTERNAL_RELEASE

CLEANFILES += \
	adl_asm.so \
	objdump-sc3900_arch_runsim_cpp.so

adl_asm.so:  $(top_builddir)/tests/adlapi2simapi/sc3900_arch_runsim_cpp.adl $(Asm)/make-asm
	$(ASMCOMPILE)

endif !IS_EXTERNAL_RELEASE

endif DTASM

if !DTASM

adl_asm.so:

endif !DTASM



