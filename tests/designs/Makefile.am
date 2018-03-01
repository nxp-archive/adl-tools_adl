
AUTOMAKE_OPTIONS = foreign serial-tests

TESTS = regress 

check_PROGRAMS = binutils-tests$(EXEEXT)

##
## Place open-source models here.
##

EXTRA_DIST = \
	$(srcdir)/regress \
	$(srcdir)/sieve.elf \
	$(srcdir)/in1_32bit.uvp \
	$(srcdir)/enable-cache.dat \
	$(srcdir)/gdbin1.elf \
	$(srcdir)/sieve.dat \
	$(srcdir)/adpcm.dat \
	$(srcdir)/des.dat \
	$(srcdir)/fir.dat \
	$(srcdir)/ucbqsort.dat \
	$(srcdir)/compress_gcc.elf \
	$(srcdir)/compress_gcc.dat \
	$(srcdir)/compress.dat \
	$(srcdir)/gdbtest4.gdb \
	$(srcdir)/gdbtest4.regress \
	$(srcdir)/gdbtest11.gdb \
	$(srcdir)/gdbtest11.regress \
	$(srcdir)/gdbtest12.gdb \
	$(srcdir)/gdbtest12.regress \
	$(srcdir)/gdbtest13.gdb \
	$(srcdir)/gdbtest13.regress \
	$(srcdir)/gdbtest14.gdb \
	$(srcdir)/gdbtest14.regress \
	$(srcdir)/gdbtest15.gdb \
	$(srcdir)/gdbtest15.regress \
	$(srcdir)/gdbtest16.gdb \
	$(srcdir)/gdbtest16.regress \
	$(srcdir)/gdbtest17.gdb \
	$(srcdir)/gdbtest17.regress \
	$(srcdir)/gdbtest20.gdb \
	$(srcdir)/gdbtest20.regress \
	$(srcdir)/gdbtest21.gdb \
	$(srcdir)/gdbtest21.regress \
	$(srcdir)/gdbtest22.gdb \
	$(srcdir)/gdbtest22.regress \
	$(srcdir)/gdbtest23.gdb \
	$(srcdir)/gdbtest23.regress \
	$(srcdir)/dhry_1.c \
	$(srcdir)/dhry_2.c \
	$(srcdir)/dhry_1.s \
	$(srcdir)/dhry_2.s \
	$(srcdir)/dhry_1.regress \
	$(srcdir)/dhry_2.regress \
	$(srcdir)/dhry.h \
	$(srcdir)/dhry.elf \
	$(srcdir)/dhry.dat \
	$(srcdir)/in5.dat \
	$(srcdir)/syscall6.c \
	$(srcdir)/syscall6.elf \
	$(srcdir)/syscall6.regress \
	$(srcdir)/syscall7.c \
	$(srcdir)/syscall7.elf \
	$(srcdir)/syscall7.regress \
	$(srcdir)/hamlet.txt \
	$(srcdir)/mpdbg.c \
	$(srcdir)/mpdbg.elf \
	$(srcdir)/mpdbg2.c \
	$(srcdir)/mpdbg2.elf \
	$(srcdir)/mpdbg3.c \
	$(srcdir)/mpdbg3.elf \
	$(srcdir)/pids.dat \
	$(srcdir)/hello.c \
	$(srcdir)/hello.elf \
	$(srcdir)/hello.hex \
	$(srcdir)/addr1.c \
	$(srcdir)/addr1.elf \
	$(srcdir)/addr1.s

CLEANFILES = as-*.cc objdump-*.cc as-g32 objdump-g32 addr2line-g32

.PHONY: binutils-tests$(EXEEXT)

SCRIPT   = $(top_builddir)/scripts
BINBUILD = $(top_builddir)/external/binutils-adl
BINSRC   = $(top_srcdir)/external/binutils-adl
ASMFLAGS =
Asm      = $(top_builddir)/asm

# These entries are needed b/c otherwise, the dist target thinks that there's a
# dependency on binutils-modN.c.  Personally, this seems like a bug in automake,
# but this hack seems to get rid of the problem.
binutils_tests_SOURCES = 

ASMCOMPILE = $(SCRIPT)/adl2asm $(ASMFLAGS) --prefix=$(top_builddir) --arch-name="powerpc" --arch-print-name="powerpc:common" $< \
                --bprefix=$(BINBUILD) --bsprefix=$(BINSRC)

objdump-%: as-% ;

as-g32:  $(top_builddir)/designs/generic32.adl $(top_builddir)/designs/power.adl $(Asm)/make-asm
	$(ASMCOMPILE) --model-base=g32

binutils-tests$(EXEEXT): as-qe as-g32

# Run check with arguments to run the regressions in parallel.
pcheck:
	$(MAKE) check RegressOpts="-p=8"

if IS_EXTERNAL_RELEASE

as-qe:

endif

if !IS_EXTERNAL_RELEASE

##
## Place proprietary test data here.
##

CLEANFILES += as-qe objdump-qe addr2line-qe

as-qe:  $(top_builddir)/designs/qe.adl $(Asm)/make-asm
	$(ASMCOMPILE) --model-base=qe

endif !IS_EXTERNAL_RELEASE

if !CREATE_EXTERNAL_RELEASE

EXTRA_DIST += \
	$(filter-out %.gen.dat,$(filter-out %.out.dat,$(wildcard $(srcdir)/*.dat))) \
	$(filter-out %.out.uvp,$(wildcard $(srcdir)/*.uvp)) \
	$(srcdir)/*.elf \
	$(srcdir)/*.txt \
	$(srcdir)/*.regress \
	$(srcdir)/*.gdb \
	$(srcdir)/*.s

endif !CREATE_EXTERNAL_RELEASE

CompileElf = [ ! -e ./ppc-elf-gcc ] || ./ppc-elf-gcc $< -o $@ -msim -g -msoft-float -e main

mpdbg.elf: $(top_srcdir)/tests/designs/mpdbg.c
	$(CompileElf)

mpdbg2.elf: $(top_srcdir)/tests/designs/mpdbg2.c
	$(CompileElf)

mpdbg3.elf: $(top_srcdir)/tests/designs/mpdbg3.c
	$(CompileElf)

hello.elf: $(top_srcdir)/tests/designs/hello.c
	$(CompileElf)

addr1.elf: $(top_srcdir)/tests/designs/addr1.c
	if type powerpc-eabi-gcc >/dev/null 2>&1; then powerpc-eabi-gcc -o $@ $< -g -e main; fi

addr1.s: $(top_srcdir)/tests/designs/addr1.c
	if type powerpc-eabi-gcc >/dev/null 2>&1; then powerpc-eabi-gcc -o $@ $< -g -S; fi

hello.hex: hello.elf
	objcopy -O ihex $< $@


#sieve64.elf: $(top_srcdir)/tests/iss/sieve.c
#	./ppc64-elf-gcc $< -m64 -o $@ -O3 -Wl,-zmuldefs -e main
