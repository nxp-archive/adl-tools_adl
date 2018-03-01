
AUTOMAKE_OPTIONS = foreign serial-tests no-exeext

TESTS = regress

check_PROGRAMS = \
	ppc-graphite-cache perf-model.so

CLEANFILES = *.cc *.cc.h dlx*.h *.bc ppc-graphite-cache-graphite.h

EXTRA_DIST = \
	$(srcdir)/ppc-graphite.adl \
	$(srcdir)/regress \
	$(srcdir)/ppc-graphite.ttc \
	$(filter-out %.out.dat,$(wildcard $(srcdir)/*.dat))

MakeIss =  $(top_builddir)/iss/make-iss 

Helpers  = $(top_builddir)/helpers
Designs  = $(top_srcdir)/designs
Iss      = $(top_builddir)/iss
Graphite = $(top_builddir)/graphite

noinst_HEADERS = \
	simple_core_model.h \
	branch_predictor.h \
	one_bit_branch_predictor.h

DEPLIBS = \
				$(Iss)/libAdlIssExec.la \
        $(Iss)/libAdlIssLogging.la \
        $(Iss)/libAdlReaders.la \
        $(Iss)/libAdlParsing.la \
        $(Iss)/libAdlIssSupport.la \
        $(Iss)/libAdlScript.la \
        $(Graphite)/libAdlGraphite.la \
        $(Helpers)/libIntbv.la \
        $(Helpers)/libIntbvRnum.la \
        $(Helpers)/libAdlSupport.la \
        $(Helpers)/libAdlLogUsage.la

AM_ADLFLAGS = -I=$(Designs) -graphite --decode-cache --no-log-usage

ADL2ISS = $(AM_V_GEN) $(top_builddir)/scripts/adl2iss --prefix=$(top_builddir) $< --output=$@ --no-warn-redefine

AM_CXXFLAGS = $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --graphite --cflags)


ppc_graphite_cache_SOURCES = ppc-graphite-cache.adl

ppc_graphite_cache_DEPENDENCIES = $(DEPLIBS)

ppc_graphite_cache_LDFLAGS = $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --graphite --libs)

ppc_graphite_cache_LINK = $(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) -o $@

# Ugh.  Libtool is such a pain in the ass.  It reorders linker flags, so that
# the whole-archive flag gets moved.  Thus, we give it the finger and ignore it
# here.
ppc-graphite-cache: $(ppc_graphite_cache_OBJECTS) $(ppc_graphite_cache_DEPENDENCIES) $(EXTRA_ppc_graphite_cache_DEPENDENCIES) 
	@rm -f ppc-graphite-cache
	$(AM_V_CXXLD)$(ppc_graphite_cache_LINK) $(ppc_graphite_cache_OBJECTS) $(ppc_graphite_cache_LDFLAGS) $(LDFLAGS) $(ppc_graphite_cache_LDADD) $(LIBS)

ppc-graphite-cache.cc: $(MakeIss) $(srcdir)/../../designs/generic32.adl $(srcdir)/../../designs/power.adl $(srcdir)/ppc-graphite.ttc $(srcdir)/ppc-graphite.adl $(srcdir)/ppc-graphite-cache.adl

ppc-graphite-cache-graphite.h: ppc-graphite-cache.cc

simple_core_model.C: ppc-graphite-cache-graphite.h


perf_model_so_SOURCES = PerfCorePlugin.C branch_predictor.C one_bit_branch_predictor.C simple_core_model.C

perf_model_so_CXXFLAGS = -fPIC $(AM_CXXFLAGS)

# Seems to be needed on Ubuntu 14.04- the linker doesn't want to link in
# Graphite on the executable.
perf_model_so_LDADD = -L$(top_builddir)/lib -lAdlGraphite

# We have to explicitly specify the compilation here, since we're manually
# creating a shared object.
perf-model.so: $(perf_model_so_OBJECTS) $(perf_model_so_DEPENDENCIES)
	@rm -f $@
	$(AM_V_CXXLD) $(CXXCOMPILE) -fPIC -shared -o $@ $^ $(perf_model_so_LDADD)



CompileElf = [ ! -e ./ppc-elf-gcc ] || ./ppc-elf-gcc $< -o $@ -c -O3 -msoft-float -std=c99 -I$(top_srcdir) -Wa,-mbooke

AsmElf = [ ! -e ./ppc-elf-as ] || ./ppc-elf-as $< -o $@ -mregnames

LinkElf = [ ! -e ./ppc-elf-gcc ] || ./ppc-elf-gcc $^ -o $@ -O3 -msim -msoft-float -lm


# Run check with arguments to run the regressions in parallel.
pcheck:
	$(MAKE) check RegressOpts="-p=8"

# PPC ELF tests currently used by the regression.
all-elf-tests: \
	hello_world.elf ping_pong.elf gspinlock.elf giolock.elf \
	spawn-2.elf spawn-10.elf spawn-20.elf mmultiply.elf iotest1.elf

.adl.cc:
	$(ADL2ISS) $(AM_ADLFLAGS) --gen-only --config=$(srcdir)/ppc-graphite.ttc

%.o: %.c
	$(CompileElf)

%.o: %.s
	$(AsmElf)

%.elf: %.o
	$(LinkElf)

$(TRG): $(OBJS)
	$(CC) $^ -o $@ -O3 -msim -msoft-float -lm
