## Process this file with automake to generate Makefile.in
#
#   Copyright (C) 2012-2014 Free Software Foundation, Inc.
#
# This file is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING3.  If not see
# <http://www.gnu.org/licenses/>.
#

AUTOMAKE_OPTIONS = dejagnu no-texinfo.tex no-dist foreign
ACLOCAL_AMFLAGS = -I .. -I ../config -I ../bfd
TEXINFO_TEX = ../texinfo/texinfo.tex

SUBDIRS = po

tooldir = $(exec_prefix)/$(target_alias)

YACC = `if [ -f ../bison/bison ]; then echo ../bison/bison -y -L$(srcdir)/../bison/; else echo @YACC@; fi`
YFLAGS = -d
LEX = `if [ -f ../flex/flex ]; then echo ../flex/flex; else echo @LEX@; fi`

# Automake 1.10+ disables lex and yacc output file regeneration if
# maintainer mode is disabled.  Avoid this.
am__skiplex =
am__skipyacc =

WARN_CFLAGS = @WARN_CFLAGS@
NO_WERROR = @NO_WERROR@
AM_CFLAGS = $(WARN_CFLAGS)

# Conditionally enable the plugin interface.
if ENABLE_PLUGINS
PLUGIN_C = plugin.c
PLUGIN_H = plugin.h
PLUGIN_OBJECT = plugin.@OBJEXT@
PLUGIN_CFLAGS = -DENABLE_PLUGINS
else
PLUGIN_C =
PLUGIN_H =
PLUGIN_OBJECT =
PLUGIN_CFLAGS =
endif

# We put the scripts in the directory $(scriptdir)/ldscripts.
# We can't put the scripts in $(datadir) because the SEARCH_DIR
# directives need to be different for native and cross linkers.
scriptdir = $(tooldir)/lib

# ADL: I'm forcing the default to be 32-bit ELF for convenience.  We may want to
# change this in the future.
EMUL = elf32ppc
EMULATION_OFILES = @EMULATION_OFILES@
EMUL_EXTRA_OFILES = @EMUL_EXTRA_OFILES@


# Search path to override the default search path for -lfoo libraries.
# If LIB_PATH is empty, the ones in the script (if any) are left alone.
# (The default is usually /lib:/usr/lib:/usr/local/lib, unless building
# a cross-linker, in which case the default is empty.  See genscripts.sh.)
# Otherwise, they are replaced with the ones given in LIB_PATH,
# which may have the form: LIB_PATH=/lib:/usr/local/lib.  This can be set
# when the linker is configured via the --with-lib-path configure switch.
LIB_PATH = @LIB_PATH@

BASEDIR = $(srcdir)/..
BFDDIR = $(BASEDIR)/bfd
INCDIR	= $(BASEDIR)/include

# What version of the manual to build
DOCVER = gen

# Options to extract the man page from ld.texi
MANCONF = -Dman

TEXI2POD = perl $(BASEDIR)/etc/texi2pod.pl $(AM_MAKEINFOFLAGS)

POD2MAN = pod2man --center="GNU Development Tools" \
	--release="binutils-$(VERSION)" --section=1

#stuff for self hosting (can be overridden in config file).
HOSTING_CRT0 = @HOSTING_CRT0@
HOSTING_SCRT0 = @HOSTING_SCRT0@
HOSTING_LIBS = @HOSTING_LIBS@
HOSTING_SLIBS = @HOSTING_SLIBS@
HOSTING_EMU = -m $(EMUL)

# Setup the testing framework, if you have one
EXPECT = expect
RUNTEST = runtest
RUNTESTFLAGS =

CC_FOR_TARGET = ` \
  if [ -f $$r/../gcc/xgcc ] ; then \
    if [ -f $$r/../newlib/Makefile ] ; then \
      echo $$r/../gcc/xgcc -B$$r/../gcc/ -idirafter $$r/../newlib/targ-include -idirafter $${srcroot}/../newlib/libc/include -nostdinc; \
    else \
      echo $$r/../gcc/xgcc -B$$r/../gcc/; \
    fi; \
  else \
    if [ "@host@" = "@target@" ] ; then \
      echo $(CC); \
    else \
      echo gcc | sed '$(transform)'; \
    fi; \
  fi`

CXX_FOR_TARGET = ` \
  if [ -f $$r/../gcc/g++ ] ; then \
    if [ -f $$r/../newlib/Makefile ] ; then \
      echo $$r/../gcc/g++ -B$$r/../gcc/ -idirafter $$r/../newlib/targ-include -idirafter $${srcroot}/../newlib/libc/include -nostdinc; \
    else \
      echo $$r/../gcc/g++ -B$$r/../gcc/; \
    fi; \
  else \
    if [ "@host@" = "@target@" ] ; then \
      echo $(CXX); \
    else \
      echo g++ | sed '$(transform)'; \
    fi; \
  fi`

transform = s/^ld-new$$/$(installed_linker)/;@program_transform_name@
lib_LIBRARIES = libld.a
info_TEXINFOS = ld.texi
ld_TEXINFOS = configdoc.texi
noinst_TEXINFOS = ldint.texi
man_MANS = ld.1

AM_MAKEINFOFLAGS = -I $(srcdir) -I $(BFDDIR)/doc -I ../bfd/doc \
		   -I $(top_srcdir)/../libiberty
TEXI2DVI = texi2dvi -I $(srcdir) -I $(BFDDIR)/doc -I ../bfd/doc \
		    -I $(top_srcdir)/../libiberty

AM_CPPFLAGS = -I. -I$(srcdir) -I../bfd -I$(BFDDIR) -I$(INCDIR) \
	@INCINTL@ $(HDEFINES) $(CFLAGS) $(PLUGIN_CFLAGS) \
	-DLOCALEDIR="\"$(datadir)/locale\""

BFDLIB = ../bfd/libbfd.la
LIBIBERTY = ../libiberty/libiberty.a

ALL_EMULATION_SOURCES = \
	eelf32ppc.c \
	eelf64ppc.c

ALL_64_EMULATIONS = $(ALL_64_EMULATION_SOURCES:.c=.@OBJEXT@)

ALL_EMUL_EXTRA_OFILES = \
	deffilep.@OBJEXT@ \
	pe-dll.@OBJEXT@

ALL_64_EMUL_EXTRA_OFILES = \
	pep-dll.@OBJEXT@

CFILES = ldctor.c ldemul.c ldexp.c ldfile.c ldlang.c \
	ldmain.c ldmisc.c ldver.c ldwrite.c lexsup.c \
	mri.c ldcref.c pe-dll.c pep-dll.c ldlex-wrapper.c \
	$(PLUGIN_C) ldbuildid.c

HFILES = ld.h ldctor.h ldemul.h ldexp.h ldfile.h \
	ldlang.h ldlex.h ldmain.h ldmisc.h ldver.h \
	ldwrite.h mri.h deffile.h pe-dll.h pep-dll.h \
	elf-hints-local.h $(PLUGIN_H) ldbuildid.h

GENERATED_CFILES = ldgram.c ldlex.c deffilep.c
GENERATED_HFILES = ldgram.h ldemul-list.h deffilep.h

# Require an early dependency on the generated headers, as the dependency
# tracking will not cause them to be built beforehand.
BUILT_SOURCES = $(GENERATED_HFILES)

OFILES = ldgram.@OBJEXT@ ldlex-wrapper.@OBJEXT@ lexsup.@OBJEXT@ ldlang.@OBJEXT@ \
	mri.@OBJEXT@ ldctor.@OBJEXT@ ldmain.@OBJEXT@ $(PLUGIN_OBJECT) \
	ldwrite.@OBJEXT@ ldexp.@OBJEXT@  ldemul.@OBJEXT@ ldver.@OBJEXT@ ldmisc.@OBJEXT@ \
	ldfile.@OBJEXT@ ldcref.@OBJEXT@ ${EMULATION_OFILES} ${EMUL_EXTRA_OFILES} \
	ldbuildid.@OBJEXT@

STAGESTUFF = *.@OBJEXT@ ldscripts/* e*.c

# Disable -Werror, if it has been enabled, since old versions of bison/
# yacc will produce working code which contain compile time warnings.
ldgram.@OBJEXT@: ldgram.c
if am__fastdepCC
	$(COMPILE) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ `test -f ldgram.c || echo $(srcdir)/`ldgram.c $(NO_WERROR)
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
else
if AMDEP
	source='ldgram.c' object='$@' libtool=no @AMDEPBACKSLASH@
	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) @AMDEPBACKSLASH@
endif
	$(COMPILE) -c `test -f ldgram.c || echo $(srcdir)/`ldgram.c $(NO_WERROR)
endif

ldlex-wrapper.@OBJEXT@: ldlex-wrapper.c ldlex.c
if am__fastdepCC
	$(COMPILE) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $(srcdir)/ldlex-wrapper.c $(NO_WERROR)
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
else
if AMDEP
	source='ldlex-wrapper.c' object='$@' libtool=no @AMDEPBACKSLASH@
	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) @AMDEPBACKSLASH@
endif
	$(COMPILE) -c $(srcdir)/ldlex-wrapper.c $(NO_WERROR)
endif

deffilep.@OBJEXT@: deffilep.c
if am__fastdepCC
	$(COMPILE) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ `test -f deffilep.c || echo $(srcdir)/`deffilep.c $(NO_WERROR)
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
else
if AMDEP
	source='deffilep.c' object='$@' libtool=no @AMDEPBACKSLASH@
	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) @AMDEPBACKSLASH@
endif
	$(COMPILE) -c `test -f deffilep.c || echo $(srcdir)/`deffilep.c $(NO_WERROR)
endif

# At the moment this is just a list of those emulation template files
# that contain internationalised strings.
EMULATION_FILES = 

POTFILES = $(CFILES) $(HFILES) $(EMULATION_FILES)

po/POTFILES.in: @MAINT@ Makefile
	for f in $(POTFILES); do echo $$f; done | LC_ALL=C sort > tmp \
	  && mv tmp $(srcdir)/po/POTFILES.in

ldmain.@OBJEXT@: ldmain.c config.status
if am__fastdepCC
	$(COMPILE) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ \
	  -DDEFAULT_EMULATION='"$(EMUL)"' \
	  -DBINDIR='"$(bindir)"' -DTOOLBINDIR='"$(tooldir)/bin"' \
	  -DTARGET='"@target@"' @TARGET_SYSTEM_ROOT_DEFINE@ \
	  $(srcdir)/ldmain.c
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
else
if AMDEP
	source='ldmain.c' object='$@' libtool=no @AMDEPBACKSLASH@
	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) @AMDEPBACKSLASH@
endif
	$(COMPILE) -c -DDEFAULT_EMULATION='"$(EMUL)"' \
	  -DBINDIR='"$(bindir)"' -DTOOLBINDIR='"$(tooldir)/bin"' \
	  -DTARGET='"@target@"' @TARGET_SYSTEM_ROOT_DEFINE@ \
	  $(srcdir)/ldmain.c
endif

ldfile.@OBJEXT@: ldfile.c config.status
if am__fastdepCC
	$(COMPILE) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ \
	-DSCRIPTDIR='"$(scriptdir)"' -DBINDIR='"$(bindir)"' -DTOOLBINDIR='"$(tooldir)/bin"' \
	 $(srcdir)/ldfile.c
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
else
if AMDEP
	source='ldfile.c' object='$@' libtool=no @AMDEPBACKSLASH@
	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) @AMDEPBACKSLASH@
endif
	$(COMPILE) -c -DSCRIPTDIR='"$(scriptdir)"' -DBINDIR='"$(bindir)"' \
	  -DTOOLBINDIR='"$(tooldir)/bin"' \
	  $(srcdir)/ldfile.c
endif

eelf32_spu.@OBJEXT@: eelf32_spu.c
if am__fastdepCC
	$(COMPILE) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ \
	  -DEMBEDSPU="\"`echo embedspu | sed '$(transform)'`\"" eelf32_spu.c
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
else
if AMDEP
	source='eelf32_spu.c' object='$@' libtool=no @AMDEPBACKSLASH@
	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) @AMDEPBACKSLASH@
endif
	$(COMPILE) -c -DEMBEDSPU="\"`echo embedspu | sed '$(transform)'`\"" \
	  eelf32_spu.c
endif

ldemul-list.h: Makefile
	(echo "/* This file is automatically generated.  DO NOT EDIT! */";\
	for f in `echo " " ${EMULATION_OFILES} "" \
	 | sed -e 's/ e/ ld/g' -e 's/ ld/ /g' -e 's/[.]o//g'`; do \
	    echo "extern ld_emulation_xfer_type ld_$${f}_emulation;"; \
	done;\
	echo "";\
	echo "#define EMULATION_LIST \\";\
	for f in `echo " " ${EMULATION_OFILES} "" \
	 | sed -e 's/ e/ ld/g' -e 's/ ld/ /g' -e 's/[.]o//g'`; do \
	    echo "  &ld_$${f}_emulation, \\"; \
	done;\
	echo "  0") >ldemul-tmp.h
	mv ldemul-tmp.h ldemul-list.h

stringify.sed: ${srcdir}/emultempl/$(STRINGIFY)
	  cp ${srcdir}/emultempl/$(STRINGIFY) stringify.sed

# These all start with e so 'make clean' can find them.

GENSCRIPTS = LIB_PATH='${LIB_PATH}' $(SHELL) $(srcdir)/genscripts.sh "${srcdir}" "${libdir}" "${prefix}" "${exec_prefix}" @host@ @target@ @target_alias@ "@EMULATION_LIBPATH@" "@NATIVE_LIB_DIRS@" @use_sysroot@ @enable_initfini_array@
GEN_DEPENDS = $(srcdir)/genscripts.sh stringify.sed
ELF_DEPS = $(srcdir)/emultempl/elf32.em $(srcdir)/emultempl/elf-generic.em $(srcdir)/scripttempl/DWARF.sc
ELF_GEN_DEPS = $(srcdir)/emultempl/generic.em $(srcdir)/emultempl/elf-generic.em $(srcdir)/emultempl/genelf.em $(srcdir)/scripttempl/DWARF.sc

@TDIRS@

# We can't use pattern rules as we don't want to depend on GNU
# make, or else these rules could have been expressed in one
# two-liner: 'e%.c:' and '	${GENSCRIPTS} $* "$(tdir_$*)"'.
# (The recursive variable expansion is portable.)

run-genscripts:
	${GENSCRIPTS} $(script_target) "$($(script_tdirname))"

.PHONY: run-genscripts

$(ALL_EMULATION_SOURCES) $(ALL_64_EMULATION_SOURCES):
	base=`echo $@ | sed -e 's,e\(.*\).c,\1,'`; \
	$(MAKE) run-genscripts "script_target=$$base" "script_tdirname=tdir_$$base"


eelf32ppc.c: $(srcdir)/emulparams/elf32ppc.sh \
  $(srcdir)/emulparams/elf32ppccommon.sh $(srcdir)/emultempl/ppc32elf.em \
  ldemul-list.h \
  $(ELF_DEPS) $(srcdir)/scripttempl/elf.sc ${GEN_DEPENDS}

eelf64ppc.c: $(srcdir)/emulparams/elf64ppc.sh $(srcdir)/emultempl/ppc64elf.em \
  ldemul-list.h \
  $(ELF_DEPS) $(srcdir)/scripttempl/elf.sc ${GEN_DEPENDS}


# We need this for automake to use YLWRAP.
EXTRA_libld_a_SOURCES = deffilep.y ldlex.l
# Allow dependency tracking to work for these files, too.
EXTRA_libld_a_SOURCES += pep-dll.c pe-dll.c

libld_a_SOURCES = ldgram.y ldlex-wrapper.c lexsup.c ldlang.c mri.c ldctor.c ldmain.c \
	ldwrite.c ldexp.c ldemul.c ldver.c ldmisc.c ldfile.c ldcref.c $(PLUGIN_C) \
	ldbuildid.c
libld_a_DEPENDENCIES = $(EMULATION_OFILES) $(EMUL_EXTRA_OFILES) \
		      $(BFDLIB) $(LIBIBERTY) $(LIBINTL_DEP)
libld_a_LIBADD = $(EMULATION_OFILES) $(EMUL_EXTRA_OFILES) $(BFDLIB) $(LIBIBERTY) $(LIBINTL)

# Dependency tracking for the generated emulation files.
EXTRA_libld_a_SOURCES += $(ALL_EMULATION_SOURCES) $(ALL_64_EMULATION_SOURCES)

# This is the real libbfd.a created by libtool.
TESTBFDLIB = @TESTBFDLIB@

check-DEJAGNU: site.exp
	srcroot=`cd $(srcdir) && pwd`; export srcroot; \
	r=`pwd`; export r; \
	LC_ALL=C; export LC_ALL; \
	EXPECT=$(EXPECT); export EXPECT; \
	runtest=$(RUNTEST); \
	if $(SHELL) -c "$$runtest --version" > /dev/null 2>&1; then \
	  $$runtest --tool $(DEJATOOL) --srcdir $${srcroot}/testsuite \
		CC="$(CC_FOR_TARGET)" CFLAGS="$(CFLAGS)" \
		CXX="$(CXX_FOR_TARGET)" CXXFLAGS="$(CXXFLAGS)" \
		CC_FOR_HOST="$(CC)" CFLAGS_FOR_HOST="$(CFLAGS)" \
		OFILES="$(OFILES)" BFDLIB="$(TESTBFDLIB)" \
		LIBIBERTY="$(LIBIBERTY) $(LIBINTL)" LIBS="$(LIBS)" \
		DO_COMPARE="`echo '$(do_compare)' | sed -e 's,\\$$,,g'`" \
		$(RUNTESTFLAGS); \
	else echo "WARNING: could not find \`runtest'" 1>&2; :;\
	fi

# Rules for testing by relinking ld itself.
# A similar test is in the testsuite.  This target is for ease of use
# when porting ld.

ld-partial.@OBJEXT@: ld-new$(EXEEXT)
	./ld-new$(EXEEXT) $(HOSTING_EMU) -o ld-partial.@OBJEXT@ -r $(OFILES)
ld1$(EXEEXT): ld-partial.@OBJEXT@
	./ld-new$(EXEEXT) $(HOSTING_EMU) -o ld1$(EXEEXT) $(HOSTING_CRT0) ld-partial.@OBJEXT@ $(TESTBFDLIB) $(LIBIBERTY) $(HOSTING_LIBS) $(LIBS)

ld1-full$(EXEEXT): ld-new
	./ld-new$(EXEEXT) $(HOSTING_EMU) -o ld1-full$(EXEEXT) $(HOSTING_CRT0) $(OFILES) $(TESTBFDLIB) $(LIBIBERTY) $(HOSTING_LIBS) $(LIBS)

ld2$(EXEEXT): ld1$(EXEEXT)
	./ld1$(EXEEXT) $(HOSTING_EMU) -o ld2$(EXEEXT) $(HOSTING_CRT0) $(OFILES) $(TESTBFDLIB) $(LIBIBERTY) $(HOSTING_LIBS) $(LIBS)

ld3$(EXEEXT): ld2$(EXEEXT)
	./ld2$(EXEEXT) $(HOSTING_EMU) -o ld3$(EXEEXT) $(HOSTING_CRT0) $(OFILES) $(TESTBFDLIB) $(LIBIBERTY) $(HOSTING_LIBS) $(LIBS)

bootstrap: ld3$(EXEEXT)
	cmp ld2$(EXEEXT) ld3$(EXEEXT)

.PHONY: bootstrap

# A test program for C++ constructors and destructors.
# This test is now in the testsuite.
#
#cdtest: cdtest-main.@OBJEXT@ cdtest-bar.@OBJEXT@ cdtest-foo.@OBJEXT@ ld.new
#	./ld.new $(HOSTING_EMU) -o cdtest $(HOSTING_CRT0) \
#	  cdtest-main.@OBJEXT@ cdtest-bar.@OBJEXT@ cdtest-foo.@OBJEXT@ $(HOSTING_LIBS)
#
#cdtest.out: cdtest
#	./cdtest > cdtest.tmp
#	mv cdtest.tmp cdtest.out
#
#cdtest-ur.@OBJEXT@: cdtest-main.@OBJEXT@ cdtest-bar.@OBJEXT@ cdtest-foo.@OBJEXT@ ld.new
#	./ld.new $(HOSTING_EMU) -o cdtest-ur.@OBJEXT@ -Ur cdtest-main.@OBJEXT@ \
#	  cdtest-bar.@OBJEXT@ cdtest-foo.@OBJEXT@
#
#cdtest-ur: cdtest-ur.@OBJEXT@
#	./ld.new $(HOSTING_EMU) -o cdtest-ur $(HOSTING_CRT0) cdtest-ur.@OBJEXT@ \
#	  $(HOSTING_LIBS)
#
#cdtest-ur.out: cdtest-ur
#	./cdtest-ur > cdtest-ur.tmp
#	mv cdtest-ur.tmp cdtest-ur.out
#
#check-cdtest: cdtest.out cdtest-ur.out $(srcdir)/cdtest.exp
#	diff $(srcdir)/cdtest.exp cdtest.out
#	diff $(srcdir)/cdtest.exp cdtest-ur.out
#
#.PHONY: check-cdtest

# END OF CHECK TARGETS

#
# Build a dummy plugin using libtool.
#
if ENABLE_PLUGINS
noinst_LTLIBRARIES = libldtestplug.la
libldtestplug_la_SOURCES = testplug.c
libldtestplug_la_CFLAGS= -g -O2
libldtestplug_la_LDFLAGS = -no-undefined -rpath /nowhere
endif

# DOCUMENTATION TARGETS
# Manual configuration file; not usually attached to normal configuration,
# because almost all configs use "gen" version of manual.
#  Set DOCVER above to change.
configdoc.texi:	${DOCVER}-doc.texi
	cp ${srcdir}/${DOCVER}-doc.texi ./configdoc.texi
	chmod u+w ./configdoc.texi

# Build the man page from the texinfo file
# The sed command removes the no-adjust Nroff command so that
# the man output looks standard.
ld.1: $(srcdir)/ld.texi configdoc.texi
	touch $@
	-$(TEXI2POD) $(MANCONF) < $(srcdir)/ld.texi > ld.pod
	-($(POD2MAN) ld.pod | \
		sed -e '/^.if n .na/d' > $@.T$$$$ && \
		mv -f $@.T$$$$ $@) || \
		(rm -f $@.T$$$$ && exit 1)
	rm -f ld.pod

MAINTAINERCLEANFILES = configdoc.texi ld.1

# We want to reconfigure if configure.host or configure.tgt changes.
# development.sh is used to determine -Werror default.
CONFIG_STATUS_DEPENDENCIES = $(srcdir)/configure.host $(srcdir)/configure.tgt \
	$(BFDDIR)/development.sh

MOSTLYCLEANFILES = $(STAGESTUFF) ld1$(EXEEXT) ld2$(EXEEXT) ld3$(EXEEXT) \
	ldemul-list.h crtbegin.@OBJEXT@ crtend.@OBJEXT@ ld.log ld.sum
mostlyclean-local:
	-rm -rf tmpdir
CLEANFILES = dep.sed DEP DEPA DEP1 DEP2 spu_ovl.s spu_ovl.@OBJEXT@ spu_icache.s spu_icache.@OBJEXT@ stringify.sed

.PHONY: install-exec-local install-data-local

install-exec-local: ld-new$(EXEEXT) install-binPROGRAMS
	$(mkinstalldirs) $(DESTDIR)$(tooldir)/bin
	n=`echo $(installed_linker) | sed '$(transform)'`; \
	if test "$(bindir)" != "$(tooldir)/bin"; then \
	  rm -f $(DESTDIR)$(tooldir)/bin/$(installed_linker)$(EXEEXT); \
	  ln $(DESTDIR)$(bindir)/$$n$(EXEEXT) $(DESTDIR)$(tooldir)/bin/$(installed_linker)$(EXEEXT) >/dev/null 2>/dev/null \
	  || $(INSTALL_PROGRAM_ENV) $(LIBTOOL) --mode=install $(INSTALL_PROGRAM) ld-new$(EXEEXT) $(DESTDIR)$(tooldir)/bin/$(installed_linker)$(EXEEXT); \
	fi; \
	if test "x$(install_as_default)" = "xyes"; then \
	  ld=`echo ld | sed '$(transform)'`; \
	  rm -f $(DESTDIR)$(bindir)/$$ld$(EXEEXT); \
	  ln $(DESTDIR)$(bindir)/$$n$(EXEEXT) $(DESTDIR)$(bindir)/$$ld$(EXEEXT) >/dev/null 2>/dev/null \
	  || $(INSTALL_PROGRAM_ENV) $(LIBTOOL) --mode=install $(INSTALL_PROGRAM) ld-new$(EXEEXT) $(DESTDIR)$(bindir)/$$ld$(EXEEXT); \
	  if test "$(bindir)" != "$(tooldir)/bin"; then \
	    rm -f $(DESTDIR)$(tooldir)/bin/ld$(EXEEXT); \
	    ln $(DESTDIR)$(bindir)/$$n$(EXEEXT) $(DESTDIR)$(tooldir)/bin/ld$(EXEEXT) >/dev/null 2>/dev/null \
	    || $(INSTALL_PROGRAM_ENV) $(LIBTOOL) --mode=install $(INSTALL_PROGRAM) ld-new$(EXEEXT) $(DESTDIR)$(tooldir)/bin/ld$(EXEEXT); \
	  fi; \
	fi

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(scriptdir)/ldscripts
	for f in ldscripts/* ; do \
	  $(INSTALL_DATA) $$f $(DESTDIR)$(scriptdir)/$$f ; \
	done

# Stuff that should be included in a distribution.  The diststuff
# target is run by the taz target in ../Makefile.in.
EXTRA_DIST = ldgram.c ldgram.h ldlex.c deffilep.c deffilep.h $(man_MANS)
diststuff: info $(EXTRA_DIST)

# Both info (ld.info) and ld.1 depend on configdoc.texi.
# But info isn't a direct target. Make info-recursive to depend on
# ld.1 to support parallel build.
info-recursive: ld.1

DISTCLEANFILES = tdirs site.exp site.bak stringify.sed
distclean-local:
	rm -rf ldscripts

MAINTAINERCLEANFILES += ld.info

# Automake 1.9 will only build info files in the objdir if they are
# mentioned in DISTCLEANFILES.  It doesn't have to be unconditional,
# though, so we use a bogus condition.
if GENINSRC_NEVER
DISTCLEANFILES += ld.info
endif
