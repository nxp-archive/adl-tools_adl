## Process this file with automake to generate Makefile.in

lib_LIBRARIES = libgas.a libgas-gas.a libgas-dt.a

libgas_a_SOURCES = $(GAS_CFILES) $(OBJ_FORMAT_CFILES) $(CONFIG_ATOF_CFILES)
libgas_a_LIBADD = $(OBJ_FORMAT_O) $(ATOF_TARG_O) \
	$(extra_objects)

libgas_gas_a_SOURCES = adl-asm-impl-gas.C

libgas_dt_a_SOURCES = adl-asm-impl-dt.C AsmDriver.C



AUTOMAKE_OPTIONS = foreign subdir-objects
ACLOCAL_AMFLAGS = -I .. -I ../config -I ../bfd

SUBDIRS = po

tooldir = $(exec_prefix)/$(target_alias)

YACC = `if [ -f ../bison/bison ] ; then echo ../bison/bison -y -L../bison/bison ; else echo @YACC@ ; fi`
LEX = `if [ -f ../flex/flex ] ; then echo ../flex/flex ; else echo @LEX@ ; fi`

# Automake 1.10+ disables lex and yacc output file regeneration if
# maintainer mode is disabled.  Avoid this.
am__skiplex =
am__skipyacc =

WARN_CFLAGS = @WARN_CFLAGS@
NO_WERROR = @NO_WERROR@
AM_CFLAGS = $(WARN_CFLAGS)

AM_CXXFLAGS = -std=c++0x

# Regular source files.

GAS_CFILES = \
	adl-asm-impl.C \
	app.c \
	as.c \
	atof-generic.c \
	compress-debug.c \
	cond.c \
	depend.c \
	dwarf2dbg.c \
	dw2gencfi.c \
	ecoff.c \
	ehopt.c \
	expr.c \
	flonum-copy.c \
	flonum-konst.c \
	flonum-mult.c \
	frags.c \
	hash.c \
	input-file.c \
	input-scrub.c \
	listing.c \
	literal.c \
	macro.c \
	messages.c \
	output-file.c \
	read.c \
	remap.c \
	sb.c \
	stabs.c \
	subsegs.c \
	symbols.c \
	write.c \
	config/ppc-support.C

CFILES = $(GAS_CFILES) 

HFILES = \
	as.h \
	asintl.h \
	bignum.h \
	bit_fix.h \
	adl-asm-impl.h \
	adl-asm-info.h \
	compress-debug.h \
	dwarf2dbg.h \
	dw2gencfi.h \
	ecoff.h \
	emul-target.h \
	emul.h \
	expr.h \
	flonum.h \
	frags.h \
	hash.h \
	input-file.h \
	listing.h \
	macro.h \
	obj.h \
	output-file.h \
	read.h \
	sb.h \
	struc-symbol.h \
	subsegs.h \
	symbols.h \
	tc.h \
	write.h


# OBJ files in config

OBJ_FORMAT_CFILES = \
	config/obj-elf.c

OBJ_FORMAT_HFILES = \
	config/obj-elf.h

# Multi files in config

MULTI_CFILES =

CONFIG_ATOF_CFILES = \
	config/atof-ieee.c

POTFILES = $(MULTI_CFILES) $(CONFIG_ATOF_CFILES) \
	$(TARG_ENV_HFILES) $(TARG_ENV_CFILES) $(OBJ_FORMAT_HFILES) \
	$(OBJ_FORMAT_CFILES) $(TARGET_CPU_HFILES) $(TARGET_CPU_CFILES) \
	$(HFILES) $(CFILES)
po/POTFILES.in: @MAINT@ Makefile
	for f in $(POTFILES); do echo $$f; done | LC_ALL=C sort > tmp \
	  && mv tmp $(srcdir)/po/POTFILES.in

# Note: GASP is now deprecated and has been removed.  It is still
# available in the CVS archive or older binutils releases if it is needed.
noinst_SCRIPTS = $(GDBINIT)
EXTRA_SCRIPTS = .gdbinit

diststuff: $(EXTRA_DIST) info

DISTCLEANFILES = targ-cpu.h obj-format.h targ-env.h itbl-cpu.h cgen-desc.h

# Now figure out from those variables how to compile and link.

BASEDIR = $(srcdir)/..
BFDDIR = $(BASEDIR)/bfd
INCDIR = $(BASEDIR)/include

# This is the variable actually used when we compile.
# Specify the directories to be searched for header files.
# Both . and srcdir are used, in that order,
# so that tm.h and config.h will be found in the compilation
# subdirectory rather than in the source directory.
AM_CPPFLAGS = -I. -I$(srcdir) -I../bfd -I$(srcdir)/config \
	-I$(INCDIR) -I$(srcdir)/.. -I../intl -I$(BFDDIR) @INCINTL@ \
	-DLOCALEDIR="\"$(datadir)/locale\""

# How to link with both our special library facilities
# and the system's installed libraries.

GASLIBS = @OPCODES_LIB@ ../bfd/libbfd.la ../libiberty/libiberty.a

# Files to be copied away after each stage in building.
STAGESTUFF = *.@OBJEXT@ $(noinst_PROGRAMS)



# Remake the info files.

MOSTLYCLEANFILES = $(STAGESTUFF) core \
	testsuite/*.@OBJEXT@ testsuite/*.out testsuite/gas.log testsuite/gas.sum \
	testsuite/site.exp site.bak site.exp stage stage1 stage2

.PHONY: install-exec-local install-data-local
.PHONY: install-exec-bindir install-exec-tooldir

install-exec-local: install-exec-bindir @install_tooldir@

install-exec-bindir: $(noinst_PROGRAMS)
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	@list='$(noinst_PROGRAMS)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo " $(LIBTOOL)  --mode=install $(INSTALL_PROGRAM) $$p $(bindir)/`echo $$p|sed -e 's/$(EXEEXT)$$//' -e 's/-new//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`"; \
	    $(LIBTOOL)  --mode=install $(INSTALL_PROGRAM) $$p $(DESTDIR)$(bindir)/`echo $$p|sed -e 's/$(EXEEXT)$$//' -e 's/-new//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`; \
	  else :; fi; \
	done

install-exec-tooldir: install-exec-bindir $(noinst_PROGRAMS)
	$(mkinstalldirs) $(DESTDIR)$(tooldir)/bin
	n=`echo as | sed '$(transform)'`; \
	if [ "$(bindir)/$$n$(EXEEXT)" != "$(tooldir)/bin/as$(EXEEXT)" ]; then \
	  rm -f $(DESTDIR)$(tooldir)/bin/as$(EXEEXT); \
	  ln $(DESTDIR)$(bindir)/$$n$(EXEEXT) $(DESTDIR)$(tooldir)/bin/as$(EXEEXT) >/dev/null 2>/dev/null \
	    || $(LIBTOOL) --mode=install $(INSTALL_PROGRAM) as-new$(EXEEXT) $(DESTDIR)$(tooldir)/bin/as$(EXEEXT); \
	else \
	  true ; \
	fi

# These exist for maintenance purposes.

.PHONY: bootstrap bootstrap2 bootstrap3 stage1 stage2 stage3 

bootstrap: as-new
	$(MAKE) stage1
	rm -f stage && ln -s stage1 stage
	$(MAKE) CC="$(CC)" CFLAGS="-O -Bstage/ $(CFLAGS)" libdir=$(libdir) ALLOCA= $(noinst_PROGRAMS)
	$(MAKE) stage2
	rm -f stage && ln -s stage2 stage
	$(MAKE) CC="$(CC)" CFLAGS="-O -Bstage/ $(CFLAGS)" libdir=$(libdir) ALLOCA= $(noinst_PROGRAMS)
	$(MAKE) comparison against=stage2

bootstrap2:
	rm -f stage && ln -s stage1 stage
	$(MAKE) CC="$(CC)" CFLAGS="-O -Bstage/ $(CFLAGS)" libdir=$(libdir) ALLOCA= $(noinst_PROGRAMS)
	$(MAKE) stage2
	rm -f stage && ln -s stage2 stage
	$(MAKE) CC="$(CC)" CFLAGS="-O -Bstage/ $(CFLAGS)" libdir=$(libdir) ALLOCA= $(noinst_PROGRAMS)
	$(MAKE) comparison against=stage2

bootstrap3:
	rm -f stage && ln -s stage2 stage
	$(MAKE) CC="$(CC)" CFLAGS="-O -Bstage/ $(CFLAGS)" libdir=$(libdir) ALLOCA= $(noinst_PROGRAMS)
	$(MAKE) comparison against=stage2

# Copy the object files from a particular stage into a subdirectory.
stage1:
	-mkdir stage1
	-mv $(STAGESTUFF) stage1
	if [ -f stage1/as-new$(EXEEXT) -a ! -f stage1/as$(EXEEXT) ] ; then (cd stage1 ; ln -s as-new$(EXEEXT) as$(EXEEXT)) ; fi

stage2:
	-mkdir stage2
	-mv $(STAGESTUFF) stage2
	if [ -f stage2/as-new$(EXEEXT) -a ! -f stage2/as$(EXEEXT) ] ; then (cd stage2 ; ln -s as-new$(EXEEXT) as$(EXEEXT)) ; fi

stage3:
	-mkdir stage3
	-mv $(STAGESTUFF) stage3
	if [ -f stage3/as-new$(EXEEXT) -a ! -f stage3/as$(EXEEXT) ] ; then (cd stage3 ; ln -s as-new as$(EXEEXT)) ; fi

against=stage2


# Reconfigure if configure.tgt changes.
# development.sh is used to determine -Werror default.
CONFIG_STATUS_DEPENDENCIES = $(srcdir)/configure.tgt $(BFDDIR)/development.sh

