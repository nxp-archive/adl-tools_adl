## Process this file with automake to generate Makefile.in

AUTOMAKE_OPTIONS = foreign

SUBDIRS = doc po

tooldir = $(exec_prefix)/$(target_alias)

ACLOCAL_AMFLAGS = -I .. -I ../config -I ../bfd

## These aren't set by automake, because they appear in
## bfd/acinclude.m4, which is included by binutils/acinclude.m4, and
## thus is not seen by automake.
CC_FOR_BUILD = @CC_FOR_BUILD@
EXEEXT_FOR_BUILD = @EXEEXT_FOR_BUILD@

YACC = `if [ -f ../bison/bison ]; then echo ../bison/bison -y -L$(srcdir)/../bison/; else echo @YACC@; fi`
YFLAGS = -d
LEX = `if [ -f ../flex/flex ]; then echo ../flex/flex; else echo @LEX@; fi`

WARN_CFLAGS = @WARN_CFLAGS@
NO_WERROR = @NO_WERROR@
AM_CFLAGS = $(WARN_CFLAGS) -w

lib_LIBRARIES = libobjdump.a libaddr2line.a


BASEDIR = $(srcdir)/..
BFDDIR = $(BASEDIR)/bfd
INCDIR	= $(BASEDIR)/include

MKDEP = gcc -MM

AM_CPPFLAGS = -I. -I$(srcdir) -I../bfd -I$(BFDDIR) -I$(INCDIR) \
	 @HDEFINES@ \
	 @INCINTL@ \
	 -DLOCALEDIR="\"$(datadir)/locale\"" \
	 -Dbin_dummy_emulation=$(EMULATION_VECTOR)


HFILES = \
	binemul.h  budbg.h     dwarf.h  sysdep.h \
	bucomm.h   coffgrok.h  debug.h      

CFILES = \
	binemul.c bucomm.c \
	dwarf.c debug.c \
	elfcomm.c filemode.c \
	ieee.c \
	objdump.c prdbg.c \
	rdcoff.c rddbg.c rename.c \
	stabs.c \
	version.c \
	wrstabs.c

ALL_CFILES = addr2line.c objdump.c $(CFILES)

libobjdump_a_SOURCES = objdump.c $(CFILES)

libaddr2line_a_SOURCES = addr2line.c $(CFILES)

DEBUG_SRCS = rddbg.c debug.c stabs.c ieee.c rdcoff.c
WRITE_DEBUG_SRCS = $(DEBUG_SRCS) wrstabs.c

# Code shared by all the binutils.
BULIBS = bucomm.c version.c filemode.c

POTFILES = $(ALL_CFILES) $(DEBUG_SRCS) $(HFILES)
po/POTFILES.in: @MAINT@ Makefile
	for f in $(POTFILES); do echo $$f; done | LC_COLLATE= sort > tmp \
	  && mv tmp $(srcdir)/po/POTFILES.in

objdump.@OBJEXT@:objdump.c
if am__fastdepCC
	$(COMPILE) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $(OBJDUMP_DEFS) $(srcdir)/objdump.c
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
else
if AMDEP
	source='objdump.c' object='$@' libtool=no @AMDEPBACKSLASH@
	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) @AMDEPBACKSLASH@
endif
	$(COMPILE) -c $(OBJDUMP_DEFS) $(srcdir)/objdump.c
endif
