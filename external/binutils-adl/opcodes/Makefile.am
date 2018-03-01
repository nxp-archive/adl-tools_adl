## Process this file with automake to generate Makefile.in

lib_LIBRARIES = libopcodes.a

AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS = -I .. -I ../config -I ../bfd

SUBDIRS = po

INCDIR = $(srcdir)/../include
BFDDIR = $(srcdir)/../bfd

WARN_CFLAGS = @WARN_CFLAGS@
NO_WERROR = @NO_WERROR@
AM_CFLAGS = $(WARN_CFLAGS)

libopcodes_a_SOURCES = $(CFILES)

# Header files.
HFILES = \
	sysdep.h

# C source files that correspond to .o's.
CFILES = \
	dis-buf.c \
	dis-init.c \
	disassemble.c \
	ppc-support.c

OFILES = @BFD_MACHINES@

# We should reconfigure whenever bfd/configure.in changes, because
# that's where the version number in Makefile comes from.
CONFIG_STATUS_DEPENDENCIES = $(BFDDIR)/configure.ac

AM_CPPFLAGS = -D_GNU_SOURCE -I. -I$(srcdir) -I../bfd -I$(INCDIR) -I$(BFDDIR) @HDEFINES@ @INCINTL@ @archdefs@


POTFILES = $(HFILES) $(CFILES)
po/POTFILES.in: @MAINT@ Makefile
	for f in $(POTFILES); do echo $$f; done | LC_COLLATE= sort > tmp \
	  && mv tmp $(srcdir)/po/POTFILES.in

