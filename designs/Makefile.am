##
## NOTE:  We may only release some of these files, so anything that shouldn't be distributed
##        as open-source should be placed within the !EXTERNAL_RELEASE block.
##

AUTOMAKE_OPTIONS = foreign

ADLCOMP = $(AM_V_GEN) $(Iss)/make-iss  $< -o $@ $($(patsubst %.cc,%_ADLFLAGS,$(subst -,_,$(@)))) --no-warn-redefine

PlasmaFlags = $(PLASMA_CFLAGS)
PlasmaLibs  = $(PLASMA_LTLIBS)
PlasmaPfx   = $(PLASMA_PATH)/lib

Helpers = $(top_builddir)/helpers
Iss     = $(top_builddir)/iss
Designs = $(top_srcdir)/designs
TestsIss= $(top_srcdir)/tests/iss

MakeIss = $(Iss)/make-iss

CLANG = $(LLVM_BINDIR)/clang++

EXTRACT = $(Iss)/adl-llvm-extract

AM_CPPFLAGS = -I$(top_srcdir) $(RNUMBER_CFLAGS)

# Note:  We use .cc as an extension for generated models so
# that we can remove them all using a pattern.  Files with .C
# extensions should be original files, not generated.
CLEANFILES = *.cc *.cc.h
ADLFLAGS =

# We override the default to add on per-compilation flags using the ADLCXXFLAGS
# suffix.  This allows us to do per-file compilation flags for ADL files without
# confusing automake.
CXXCOMPILE = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) \
	$($(patsubst %.o,%_ADLCXXFLAGS,$(subst -,_,$(@))))

OccFlags = $(PLASMA_CFLAGS)

if !MINIMAL_BUILD

if LANG_FRONTEND

##
## Place open-source models here.
##
check_PROGRAMS = generic32 generic32mt generic32-cache g32_tdriver 


# Place here any files to be distributed in all cases.
EXTRA_DIST = \
	power.adl \
	generic32.adl \
	generic32mt.adl \
	generic32-trans.ttc \
	generic32-config.tcl

BUILT_SOURCES = generic32.cc.h

BC_INCLUDES = -I$(top_srcdir) -I$(top_builddir) $(RNUMBER_CFLAGS)

basic_dependencies = $(Iss)/libAdlIssExec.la $(Iss)/libAdlIssMain.la				\
										 $(Iss)/libAdlIssLogging.la $(Iss)/libAdlReaders.la			\
										 $(Iss)/libAdlParsing.la $(Iss)/libAdlIssSupport.la			\
										 $(Iss)/libAdlScript.la $(Helpers)/libIntbv.la					\
										 $(Helpers)/libIntbvRnum.la $(Helpers)/libAdlSupport.la	\
										 $(Helpers)/libAdlLogUsage.la

##
## For setting up a multi-job compile, you must specify the following, where
## "target" refers to the base model name, e.g. altair_r1_ut, etc.
##
## * Create a target_ADLFLAGS variable and set --jobs=n to the appropriate
##   value.
##
## * Create a [nodist_]target_OBJECTS variable and list all objects.  This will include the target.$(OBJEXT)
##   file plus target-0.$(OBJEXT) .. target-(n-1).$(OBJEXT)
##
## * List all dependencies for target.cc, e.g. the ADL files, the parser, etc.
##
## * List the multi-job generated .cc files as dependencies of the main .cc file, e.g.
##   target-0.cc ... : target.cc

nodist_generic32_SOURCES = \
	generic32.cc

generic32_ADLFLAGS = --no-line-directives $(ADL_JIT) --decode-cache --no-td-api

if LLVM
generic32_DEPENDENCIES = $(BCDIR)/generic32.bc $(JIT_LIBS) $(basic_dependencies)
endif

generic32_LDFLAGS = -export-dynamic

generic32.cc: generic32.adl $(MakeIss) power.adl
$(BCDIR)/generic.bc: generic32.cc


nodist_generic32mt_SOURCES = \
	generic32mt.cc

generic32mt.cc: generic32mt.adl $(MakeIss) generic32.adl power.adl

generic32mt_ADLFLAGS = --no-line-directives --decode-cache --mt-support


generic32_cache_SOURCES = \
	generic32-cache.adl

generic32_cache_ADLFLAGS = --no-line-directives $(ADL_JIT) --decode-cache --no-td-api

if LLVM
generic32_cache_DEPENDENCIES = $(BCDIR)/generic32-cache.bc $(JIT_LIBS) $(basic_dependencies)
endif

generic32_cache_LDFLAGS = -export-dynamic

generic32-cache.cc: generic32-cache.adl $(Iss)/make-iss power.adl
$(BCDIR)/generic32-cache.bc: generic32-cache.cc

g32_tdriver_SOURCES = tdriver.C

g32_tdriver_CXXFLAGS = $(CXXFLAGS) -DMTYPE=powerpc -DMODEL='"generic32.cc.h"' -DMNAME='"powerpc"'

g32_tdriver_LDADD = $(transactional_iss_libraries)

generic32.cc.h: generic32.adl $(Iss)/make-iss generic32.adl generic32.adl power.adl generic32-trans.ttc

#if !CREATE_EXTERNAL_RELEASE
#SUBDIRS = scbtb
#DIST_SUBDIRS = scbtb
#else
#SUBDIRS = 
#DIST_SUBDIRS = 
#endif

if !IS_EXTERNAL_RELEASE

##
## Place proprietary models here.
##
check_PROGRAMS += altair_r1_mt altair_r1_ut qe sc3400_arch


# QuickEngine core.
nodist_qe_SOURCES = \
	qe.adl

# StarCore SC3400.  Generated as 3 implementation files, plus we use the "low memory" feature, which shrinks the
# size of the decode tree.
nodist_sc3400_arch_SOURCES = \
	sc3400_arch.adl

sc3400_arch_ADLFLAGS = --jobs=3 --low-mem

nodist_sc3400_arch_OBJECTS = sc3400_arch.$(OBJEXT) sc3400_arch-0.$(OBJEXT) sc3400_arch-1.$(OBJEXT) sc3400_arch-2.$(OBJEXT)

# Single-threaded Altair, generated using 3 implementation files.
nodist_altair_r1_ut_SOURCES = \
	altair_r1_ut.adl 

altair_r1_ut_ADLFLAGS = --jobs=3

nodist_altair_r1_ut_OBJECTS = altair_r1_ut.$(OBJEXT) altair_r1_ut-0.$(OBJEXT) altair_r1_ut-1.$(OBJEXT) altair_r1_ut-2.$(OBJEXT)

# Multi-threaded Altair.
nodist_altair_r1_mt_SOURCES = \
	altair_r1_mt.adl

altair_r1_mt_ADLFLAGS = --jobs=3 --config=$(Designs)/altair_r1_mt.ttc

nodist_altair_r1_mt_OBJECTS = altair_r1_mt.$(OBJEXT) altair_r1_mt-0.$(OBJEXT) altair_r1_mt-1.$(OBJEXT) altair_r1_mt-2.$(OBJEXT)

BUILT_SOURCES +=

# Use this to properly create the dependencies for the generated C++ files created by a multi-file compile job.
# Use within a foreach loop, where you loop over job numbers, e.g. "0 1 2".
# arg1:  The base name, e.g. altair_r1_ut
# arg2:  The job number, e.g. 0
define MultiFile
$(1)-$(2).cc: $(1).cc
$(1)-$(2).$(OBJEXT): $(1)-$(2).cc
$(1)-$(2).$(OBJEXT): $(1).cc
endef

altair_r1_mt.cc: altair_r1_mt.adl $(Iss)/make-iss isel_apu.adl power.adl \
                 altivec_apu.adl fp_helpers.adl cfp_apu.adl altair_mmu.adl \
                 altair_cache.adl mt_apu.adl ps_apu.adl altair_r1.adl altair_r1_mt.ttc
$(foreach xx,0 1 2,$(eval $(call MultiFile,altair_r1_mt,$(xx))))

altair_r1_ut.cc: altair_r1_ut.adl $(Iss)/make-iss isel_apu.adl power.adl altivec_apu.adl \
                 fp_helpers.adl cfp_apu.adl altair_mmu.adl altair_cache.adl altair_r1.adl
$(foreach xx,0 1 2,$(eval $(call MultiFile,altair_r1_ut,$(xx))))

qe.cc: qe.adl $(Iss)/make-iss 

sc3400_arch.cc: sc3400_arch.adl dalu_arithmetic.adl dalu_mac.adl dalu_mpy.adl dalu_barrel-shifter_dbs.adl dalu_logical_bfu.adl \
				agu_arithmetic_aau.adl agu_cache.adl agu_loop_control.adl agu_move.adl agu_nonloop_cof.adl agu_program_control.adl \
				agu_stack.adl bmu.adl $(Iss)/make-iss 
$(foreach xx,0 1 2,$(eval $(call MultiFile,sc3400_arch,$(xx))))


endif !IS_EXTERNAL_RELEASE


if CREATE_EXTERNAL_RELEASE

# For external releases, we create a dummy file which identifies this as such.
dist-hook:
	touch $(distdir)/$(EXTERNAL_RELEASE_FLAG)

else

# Place here any files which are proprietary and should *not* be distributed to
# the outside world.  You must include *all* adl files, since we ust the no_dist
# flag to shut down the automatic copying.
EXTRA_DIST += \
	mt_apu.adl \
	altivec_apu.adl \
	fp_helpers.adl \
	cfp_apu.adl \
  perfmon_apu.adl \
  ipc_apu.adl \
	ps_apu.adl \
	isel_apu.adl \
	altair_cache.adl \
	altair_mmu.adl \
	altair_r1.adl \
	altair_r1_mt.ttc \
	altair_r1_ut.adl \
	altair_r1_mt.adl \
	altair32-config.tcl \
	altair64-config.tcl \
	qe.adl \
	common.adl \
	bmu.adl \
	dalu_arithmetic.adl \
	agu_cache.adl \
	agu_move.adl \
	agu_loop_control.adl \
	dalu_mac.adl \
	dalu_barrel-shifter_dbs.adl \
	agu_nonloop_cof.adl \
	sc3400_arch.adl \
	dalu_mpy.adl \
	agu_loop_cof.adl \
	prefix.adl \
	agu_stack.adl \
	agu_program_control.adl \
	agu_arithmetic_aau.adl \
	dalu_logical_bfu.adl

endif !CREATE_EXTERNAL_RELEASE

if LLVM

JIT_LIBS = $(Iss)/libAdlIssJitSupport.la
#JIT_LDADD = $(JIT_LIBS)  -lboost_thread -lboost_system -lboost_regex $(LLVM_LDFLAGS) $(LLVM_LIBS) $(BOOST_LDFLAGS)
JIT_LDADD = $(JIT_LIBS) $(LLVM_LDFLAGS) $(LLVM_LIBS)

endif


LDADD = \
	$(Iss)/libAdlIssMain.la \
	$(Iss)/libAdlIssExec.la \
	$(Iss)/libAdlIssLogging.la \
	$(Iss)/libAdlReaders.la \
	$(Iss)/libAdlParsing.la \
	$(Iss)/libAdlIssSupport.la \
	$(Iss)/libAdlScript.la \
	$(JIT_LDADD) \
  $(Helpers)/libIntbv.la \
  $(Helpers)/libIntbvRnum.la \
	$(Helpers)/libAdlSupport.la \
	$(Helpers)/libAdlLogUsage.la \
	$(TCL_LIB_PATHS) $(TCL_LIBS) \
	$(PlasmaPfx)/libgc.la -ldl \
	$(RNUMBER_LIBS) \
	$(LZ4_LTLIBS) \
	$(PlasmaPfx)/libltdl.la \
  $(BOOST_LDFLAGS) -lboost_thread -lboost_system -lboost_regex -lboost_atomic -lboost_iostreams -lrt -lpthread

transactional_iss_libraries = \
    $(Iss)/libAdlIssMain.la \
    $(Iss)/libAdlIssExec.la \
    $(Iss)/libAdlIssTransLogging.la \
    $(Iss)/libAdlReaders.la \
    $(Iss)/libAdlParsing.la \
    $(Iss)/libAdlIssSupport.la \
    $(Iss)/libAdlScript.la \
    $(Helpers)/libIntbv.la \
    $(Helpers)/libIntbvRnum.la \
    $(Helpers)/libAdlSupport.la \
		$(Helpers)/libAdlLogUsage.la \
		$(TCL_LIB_PATHS) $(TCL_LIBS) \
		$(RNUMBER_LIBS) \
		$(LZ4_LTLIBS) \
	 	$(PlasmaPfx)/libltdl.la \
		$(BOOST_LDFLAGS) -lboost_thread -lboost_system -lboost_regex -lboost_atomic -lboost_context -lboost_iostreams -lrt \
    $(PlasmaPfx)/libgc.la -ldl

LINK = $(LIBTOOL) --mode=link $(CXX) -o $@


.adl.cc:
	$(ADLCOMP)

MakeTdriver = $(AM_V_GEN) $(MakeIss) --config=$(srcdir)/$*-trans.ttc --transactional-iss $< -o=$@ $(ADLFLAGS)

ADL2ISS = $(top_builddir)/scripts/adl2iss --prefix=$(top_builddir) $< --output=$@  --no-warn-redefine

tdriver.C: $(TestsIss)/tdriver.C
	ln -s $(TestsIss)/tdriver.C $@

%.cc.h : %.adl ;
	$(MakeTdriver)

$(BCDIR)/%.bc : %.cc ;
	@mkdir -p $(BCDIR)
	$(ADL2ISS) $(ADLFLAGS) --jit --jit-compile-only $<

endif LANG_FRONTEND

endif !MINIMAL_BUILD
