
AUTOMAKE_OPTIONS = foreign serial-tests

TESTS = regress

noinst_PROGRAMS = simple

simple_SOURCES = \
	simple.C

OccFlags = $(PLASMA_CFLAGS)
OccLibs = $(PLASMA_PATH)/lib

Helpers = $(top_builddir)/helpers
FeFlags = $(top_builddir)/fe

EXTRA_DIST = \
	$(srcdir)/regress \
	$(srcdir)/*.adl \
	$(srcdir)/*.in \
	$(srcdir)/*.regress

CLEANFILES = *.occ

AM_CPPFLAGS = $(OccFlags) -I$(top_srcdir) $(RNUMBER_CFLAGS)

LDADD = \
	$(FeFlags)/libAdlFrontEnd.la \
	$(Helpers)/libAdlSupport.la \
	$(Helpers)/libAdlHelpers.la \
	$(RNUMBER_LTLIBS) \
	$(OccLibs)/libocc.la \
	$(OccLibs)/libgc.la

# Use this if you want to use libtool.  I didn't want the wrapper scripts,
# so I just do it manually.
LINK = $(LIBTOOL) --mode=link $(CXX) -o $@

# Run check with arguments to run the regressions in parallel.
pcheck:
	$(MAKE) check RegressOpts="-p=8"
