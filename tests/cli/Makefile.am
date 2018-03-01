
AUTOMAKE_OPTIONS = foreign serial-tests

TESTS = regress 


CLEANFILES = *.ir

##
## Place open-source models here.
##

EXTRA_DIST = \
	$(srcdir)/regress \
	$(srcdir)/c20.cli \
	$(srcdir)/c20.regress \
	$(srcdir)/c23.cli \
	$(srcdir)/c23.regress \
	$(srcdir)/c24.cli \
	$(srcdir)/c24.regress \
	$(srcdir)/c25.cli \
	$(srcdir)/c25.regress \
	$(srcdir)/c26.cli \
	$(srcdir)/c27.cli \
	$(srcdir)/c27.regress \
	$(srcdir)/c28.cli \
	$(srcdir)/c28.regress \
	$(srcdir)/c29.cli \
	$(srcdir)/c29.regress \
	$(srcdir)/c30.cli \
	$(srcdir)/c30.regress \
	$(srcdir)/c31.cli \
	$(srcdir)/c31.regress \
	$(srcdir)/c32.cli \
	$(srcdir)/c32.regress \
	$(srcdir)/c34.cli \
	$(srcdir)/c34.regress \
	$(srcdir)/c35.cli \
	$(srcdir)/c35.regress \
	$(srcdir)/c36.cli \
	$(srcdir)/c36.regress \
	$(srcdir)/sieve.elf \
	$(srcdir)/sieve2.elf \
	$(srcdir)/c37.cli \
	$(srcdir)/c37.regress \
	$(srcdir)/level_excepts.dat \
	$(srcdir)/c38.cli \
	$(srcdir)/in3.dat \
	$(srcdir)/c39.cli \
	$(srcdir)/c39.regress \
	$(srcdir)/in5.dat \
	$(srcdir)/c40.cli \
	$(srcdir)/c41.cli \
	$(srcdir)/c41.regress \
	$(srcdir)/c42.cli \
	$(srcdir)/c42.regress \
	$(srcdir)/c43.cli \
	$(srcdir)/hello.c \
	$(srcdir)/hello.elf \
	$(srcdir)/count64.c \
	$(srcdir)/count64.elf \
	$(srcdir)/termio.regress \
	$(srcdir)/termio.cli \
	$(srcdir)/c44.cli \
	$(srcdir)/c44.regress \
	$(srcdir)/c45.cli \
	$(srcdir)/c45.regress \
	$(srcdir)/loops.c \
	$(srcdir)/loops.elf \
	$(srcdir)/setup-loops.dat \
	$(srcdir)/c46.cli \
	$(srcdir)/in6.dat \
	$(srcdir)/c47.cli \
	$(srcdir)/adds.dat \
	$(srcdir)/adds.elf \
	$(srcdir)/adds.s \
	$(srcdir)/c48.cli \
	$(srcdir)/c49.cli \
	$(srcdir)/in7.dat \
	$(srcdir)/in7.uvp \
	$(srcdir)/c50.cli \
	$(srcdir)/c50.regress \
	$(srcdir)/c51.cli \
	$(srcdir)/c51.regress \
	$(srcdir)/c52.cli \
	$(srcdir)/c53.cli \
	$(srcdir)/c53.dat \
	$(srcdir)/c54.cli \
	$(srcdir)/c54.regress \
	$(srcdir)/c55.dat \
	$(srcdir)/c55.cli \
	$(srcdir)/c55.regress \
	$(srcdir)/mtbrkpt1.dat \
	$(srcdir)/bbtest.c \
	$(srcdir)/bbtest.elf \
	$(srcdir)/in8.dat

##
## Place proprietary test data here.
##

if !CREATE_EXTERNAL_RELEASE

EXTRA_DIST += \
	$(srcdir)/*.regress \
	$(srcdir)/*.dat \
	$(srcdir)/*.uvp \
	$(srcdir)/*.elf \
	$(srcdir)/*.cli

endif !CREATE_EXTERNAL_RELEASE

AM_CXXFLAGS = $(RL_CFLAGS)

CompileElf = [ -e ./ppc-elf-gcc ] && ./ppc-elf-gcc $< -o $@ -O3 -msim -msoft-float -Wl,-zmuldefs

CompileElf64 = [ -e ./ppc64-elf-gcc ] && ./ppc64-elf-gcc $< -m64 -o $@ -O3 -Wl,-zmuldefs -e main

# For building a test file used to test tcl emulation of a memory-mapped console IO application.
hello.elf: hello.c
	$(CompileElf)

bbtest.elf: bbtest.c
	$(CompileElf) -O0

count64.elf: count64.c
	$(CompileElf64)

loops.elf: loops.c
	if [ ./ppc-elf-gcc ] ; then ./ppc-elf-gcc $< -o $@ -e p1 ; fi

