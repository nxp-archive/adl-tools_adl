
AUTOMAKE_OPTIONS = foreign serial-tests

TESTS = BitsTest

check_PROGRAMS = BitsTest

BitsTest_SOURCES = \
	BitsTest.C

Helpers = $(top_builddir)/helpers

AM_CPPFLAGS = -I$(top_srcdir) $(RNUMBER_CFLAGS)

LDADD = \
	$(Helpers)/libIntbv.la \
	$(Helpers)/libIntbvRnum.la \
	$(RNUMBER_LIBS)

# Use this if you want to use libtool.  I didn't want the wrapper scripts,
# so I just do it manually.
LINK = $(LIBTOOL) --mode=link $(CXX) -o $@
