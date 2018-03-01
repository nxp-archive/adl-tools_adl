
AUTOMAKE_OPTIONS = foreign serial-tests

if HAVE_WORKING_FORK

TESTS = regress

check_PROGRAMS = intbvTest bitsTest sintbvTest miscSignedTest le_intbvTest le_sintbvTest 

endif

intbvTest_SOURCES = \
	NumberTest.C

le_intbvTest_SOURCES = \
	NumberTest.C

le_sintbvTest_SOURCES = \
	NumberTest.C

bitsTest_SOURCES = \
	NumberTest.C

sintbvTest_SOURCES = \
	NumberTest.C

miscSignedTest_SOURCES = \
	miscSignedTest.C

miscSignedTest_LDADD = \
	$(HelperLibs)/libIntbv.la \
	$(HelperLibs)/libIntbvRnum.la

OccFlags = $(PLASMA_CFLAGS)

HelperLibs = $(top_builddir)/helpers
HelperIncs = $(top_srcdir)/helpers

EXTRA_DIST = NumberTest.C.in callTemplatizer.pl regress

CLEANFILES = NumberTest.C  

# So that make clean will remove it
BUILT_SOURCES = NumberTest.C 

AM_CPPFLAGS = $(OccFlags) -I$(HelperIncs) $(RNUMBER_CFLAGS)

# The only difference between the tests is the define that's used.
intbvTest_CPPFLAGS = -DNumType=0 $(AM_CPPFLAGS)

bitsTest_CPPFLAGS = -DNumType=1 $(AM_CPPFLAGS)

sintbvTest_CPPFLAGS = -DNumType=2 $(AM_CPPFLAGS)

le_intbvTest_CPPFLAGS = -DNumType=3 $(AM_CPPFLAGS)

le_sintbvTest_CPPFLAGS = -DNumType=4 $(AM_CPPFLAGS)

LDFLAGS = -L$(PLASMA_PATH)/lib

LDADD = \
	$(HelperLibs)/libIntbv.la \
	$(HelperLibs)/libIntbvRnum.la \
	$(HelperLibs)/libAdlSupport.la \
	$(PLASMA_LIBS) \
	$(RNUMBER_LIBS)

# Use this if you want to use libtool.  I didn't want the wrapper scripts,
# so I just do it manually.
LINK = $(LIBTOOL) --mode=link $(CXX) -o $@

NumberTest.C : NumberTest.C.in
	$(srcdir)/callTemplatizer.pl $(srcdir)/NumberTest.C.in

# Run check with arguments to run the regressions in parallel.
pcheck:
	$(MAKE) check RegressOpts="-p=8"
