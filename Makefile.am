
# Put compiled object files into the same sub-directory as their sources.
# Don't complain about missing GNU files.
AUTOMAKE_OPTIONS = foreign subdir-objects no-exeext

# Default extension to use if _SOURCES has not been defined.
AM_DEFAULT_SOURCE_EXT = .C

# This ensures that aclocal will be run correctly if we touch
# configure.in, then type "make".  What happens is that configure
# captures the state of the variable and substitutes it into the
# final Makefile.
ACLOCAL_AMFLAGS = -I ./macros

# We never want usage logging when checking the distribution.  We also want to
# re-use our existing compilers so that we don't end up with links to the
# masquerade compilers used by distcc.
DISTCHECK_CONFIGURE_FLAGS = --disable-usage-logging CXXPROG=$(CXXPROG) CPROG=$(CPROG)

# Our build system is now mostly non-recursive in order to maximize parallelism
# and improve performance.  Since we do have some recursion, the "."
# subdirectory is first to make sure that we build everything first before
# recursing.
# 
# Exceptions:
# 
# external: binutils tree, which is very crufty and not something that I feel
# like ripping up and re-doing right now.  So, it's still recursive, but since
# it's all C, it's a fast compile.
#
# designs: Seems like this might be an automake bug.  I can't see to do
# conditional program compilation with subdirectories in the items.  Might just
# be some weird bug on my side, but it doesn't recognize the internal-only
# models as being valid things to build.
#
# tests: To be done.  It should be possible to flatten this stuff, but it'll
# just take time.
#
# doc: Kinda ugly and specialized, so I flattened it internally, but it itself
# is still recursive.
SUBDIRS = . external designs

if !MINIMAL_BUILD 

if LANG_FRONTEND

PRE_ISS = tests/fe 

endif LANG_FRONTEND

endif !MINIMAL_BUILD 

if !MINIMAL_BUILD 

if LANG_FRONTEND

POST_ISS = tests/cli tests/scripts tests/docs tests/designs tests/uadl-fe tests/uadl tests/adlapi2simapi tests/graphite

endif LANG_FRONTEND

endif !MINIMAL_BUILD 

SUBDIRS += $(PRE_ISS) tests/iss $(POST_ISS) tests/intbv tests/bits


if BUILD_DOCS
SUBDIRS += doc
endif

# We must distribute our config header because generated code
# depends upon it.
include_HEADERS = adl_config.h

data_DATA = libtool

EXTRA_DIST = $(wildcard README-*.html) $(wildcard *.vcxproj) $(wildcard *.sln)

# Note:  We use .cc as an extension for generated models so
# that we can remove them all using a pattern.  Files with .C
# extensions should be original files, not generated.
CLEANFILES = *.cc *.cc.h

BASESRCDIR=$(shell cd $(top_srcdir) && pwd)
BASEDIR=$(shell cd $(top_builddir) && pwd)

OccFlags = $(PLASMA_CFLAGS)
OccLibs  = $(PLASMA_PATH)/lib

ADLFLAGS =  --no-line-directives

ASMFLAGS =

ADL2ISS = $(AM_V_GEN) $(top_builddir)/scripts/adl2iss --prefix=$(top_builddir) $< --output=$@ --no-optimize --no-warn-redefine

ADL2ISS_OBJ = $(AM_V_GEN) $(top_builddir)/scripts/adl2iss --prefix=$(top_builddir) $< --output=$@ --no-optimize --compile-only --no-warn-redefine

ADL2ASM = $(AM_V_GEN) $(top_builddir)/scripts/adl2asm $(ASMFLAGS) --prefix=$(top_builddir) $< --bprefix=$(BINBUILD) --bsprefix=$(BINSRC) --build-locally --no-warn-redefine

SIMPLELINK = $(AM_V_CXXLD) $(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@

MakeIss =  $(top_builddir)/iss/make-iss 

MakeAsm =  $(top_builddir)/asm/make-asm

CLANG = $(LLVM_BINDIR)/clang++

EXTRACT = $(Iss)/adl-llvm-extract

ADL = $(AM_V_GEN) $(MakeIss) $(ADLFLAGS) $< -o $@ 

MakeTdriver = $(AM_V_GEN) $(MakeIss) --config=$(srcdir)/$*-trans.ttc --transactional-iss $< -o=$@ $(ADLFLAGS) -D=TRANSACTIONAL_ISS --no-warn-redefine

MakeHdriver = $(AM_V_GEN) $(MakeIss) --config=$(srcdir)/$*-trans.ttc --hybrid-iss $< -o=$@ $(ADLFLAGS) -D=HYBRID_ISS  --no-warn-redefine

MakeLibdriver = $(AM_V_GEN) $(MakeIss) --lib-mode $< -o=$@ $(ADLFLAGS)  --no-warn-redefine

MakeGenOnly = $(AM_V_GEN) $(MakeIss) $< -o=$(notdir $(@)) $(ADLFLAGS) $($(patsubst %-simple.cc,%_simple_ADLFLAGS,$(notdir $(@)))) --no-warn-redefine

if LLVM

jit_libs = $(Iss)/libAdlIssJitSupport.la -lboost_thread
jit_ldadd = $(jit_libs) $(LLVM_LDFLAGS) $(LLVM_LIBS) $(BOOST_LDFLAGS)

endif

BC_INCLUDES = -I$(top_srcdir) -I$(top_builddir) $(RNUMBER_CFLAGS)

# Just to remove ordering issues.  All include makefiles should use += to
# append.
noinst_LTLIBRARIES =
lib_LTLIBRARIES = 
bin_PROGRAMS =
bin_SCRIPTS = 
noinst_PROGRAMS =
check_PROGRAMS =
BUILT_SOURCES = 

Designs = $(top_builddir)/designs
Helpers = $(top_builddir)/helpers
Fe      = $(top_builddir)/fe
Asm     = $(top_builddir)/asm
Iss     = $(top_builddir)/iss
TestsIss= $(top_srcdir)/tests/iss

# This should compile only generator code and not the designs directory.
basic: all-local external

# Run check with arguments to run the regressions in parallel.
pcheck:
	$(MAKE) check RegressOpts="-p=16"

pcheck24:
	$(MAKE) check RegressOpts="-p=24"

# Convenience target:  Build ISS dependencies.
iss: $(lib_LTLIBRARIES) iss/make-iss

# Convenience target:  Build uADL dependencies.
uadl: $(lib_LTLIBRARIES) uadl/make-uadl

# Install documentation (not done by 'all' target)
docinstall:
	cd doc && $(MAKE) docinstall

# Create a side installation which contains the side-install value.
side-dist:
	cd doc && $(MAKE) side-dist

make-release-notes:
	cd doc && $(MAKE) make-release-notes

show-release-notes:
	cd doc && $(MAKE) show-release-notes

gen-release-notes:
	cd doc && $(MAKE) gen-release-notes

oss-gen-release-notes:
	cd doc && $(MAKE) oss-gen-release-notes

clean-local:
	rm -rf $(top_builddir)/lib $(top_builddir)/bin $(top_builddir)/include

# Clean all autoconf cache data in the tree.
clean-caches:
	find . -name "*.cache" -type f -exec rm {} \;

# Clean all profile-guided-optimization data from the tree.
clean-pgo:
	find . -name "*.gcda" -type f -exec rm {} \;

# This is currently needed b/c some stuff in binutils seems to want to write into
# the source directory even though it's not supposed to do that.
distcheck-hook:
	chmod -R u+w $(distdir)/external/binutils-adl

# I'll eventually get this to work, but for now we just ignore it.
distcleancheck:
	@:

distclean: distclean-recursive clean-caches
	-rm -f $(am__CONFIG_DISTCLEAN_FILES)
	-rm -f Makefile

# Not used directly, but this allows you to build the disassembler from the command-line.
objdump-%: as-% ;

as-%: %.adl $(Asm)/make-asm
	$(ASMCOMPILE)

.adl.cc:
	$(ADL) $($(patsubst %.cc,%_ADLFLAGS,$(notdir $@)))

%rp.cc : %.adl ;
	$(ADL) --log-reg-reads --log-reg-masks $($(patsubst %.cc,%_ADLFLAGS,$(notdir $@)))

%.cc.h : %.adl ;
	$(MakeTdriver)

%-lib.cc.h : %.adl ;
	$(MakeLibdriver)

%-simple.cc : %.adl ;
	$(MakeGenOnly)

%sm.cc.h : %.adl ;
	$(MakeTdriver) --logging-mem

%sm-h.cc.h : %.adl ;
	$(MakeHdriver) $($(patsubst %sm-h.cc.h,%_SMH_ADLFLAGS,$(notdir $@)))

%.cc.h : $(Designs)/%.adl
	$(MakeTdriver)

# To build the LLVM bitcode module for the JIT, we use clang to compile, then we
# use llvm-extract to extract just the handlers, read-only string variables, and
# the global core pointer.  If llvm-extract returns an error, then it means that
# none of those symbols were present, so we just copy over the file.  Would be
# nice if llmv-extract were smart enough to not produce an error when operating
# in the --delete mode, but it's not.
$(BCDIR)/%.bc : %.cc ;
	@mkdir -p $(BCDIR)
	$(AM_V_GEN) $(CLANG) -fexceptions -O3 -emit-llvm -c $(BC_INCLUDES) $(patsubst %.cc,%-jit.cc,$<) -o $(BCDIR)/$(patsubst %.cc,%.full.bc,$<)
	$(AM_V_GEN) $(EXTRACT) $(BCDIR)/$(patsubst %.cc,%.full.bc,$<) --rfunc='.*_adl_instr__.*' \
		--rfunc=".*_read_jit.*" --rfunc=".*_write_jit.*" --rglob='.str.*' --rfunc=".*adl12MemoryAccess.*" > $@

# Note: Flex and bison files are dependent upon version files created by
# configure, so that the generated source files will be recreated if versions
# change.  These rules are dependent upon configure having found the correct
# version of the tools.

if HAVE_BISON

# Note: The perl file editing is for fixing the stack class so that we can make
# it compatible with the garbage collector.
%.cc : %.yy $(top_builddir)/bison-version ; 
	$(YACC) $($(patsubst %.cc,%_yflag,$(notdir $(@)))) $< -o $@ 

endif

if HAVE_FLEX

%.cc : %.ll $(top_builddir)/flex-version ; 
	$(LEX) $($(patsubst %.cc,%_lflag,$(notdir $(@)))) --nounistd -o$@ $<

endif

##
## Include constituent makefiles here.
##

include macros/Makefile.mk
include extras/Makefile.mk
include scripts/Makefile.mk
include helpers/Makefile.mk
include lz4_stream/Makefile.mk
include fe/Makefile.mk
include asm/Makefile.mk
include iss/Makefile.mk
include graphite/Makefile.mk
include misc/Makefile.mk
include uadl/Makefile.mk
include adlapi2simapi/src/Makefile.mk


scripts_pfx  = scripts
helpers_pfx  = helpers
fe_pfx       = fe
asm_pfx      = asm
iss_pfx      = iss
graphite_pfx = graphite
misc_pfx     = misc
uadl_pfx     = uadl
lz4_pfx      = lz4_stream

install_libs = $(patsubst %.la,%,$(lib_LTLIBRARIES))

##
## The idea here is that we "install" files via links so that scripts such as
## adl2iss can find the files they need in the same relative location as they
## would in a proper installation.  This way, we don't have to special-case the
## various wrapper scripts when running from a sandbox, so that we can test their
## use without having to install first.
##
## Note: Because adl_config.h is a generated file, it is sourced from the build
## base, rather than the source base.
all-local: $(LTLIBRARIES) $(PROGRAMS) $(HEADERS)
	@mkdir -p $(BASEDIR)/bin $(BASEDIR)/lib $(BASEDIR)/include								\
					 $(BASEDIR)/include/$(lz4_pfx) $(BASEDIR)/include/$(helpers_pfx)	\
					 $(BASEDIR)/include/$(iss_pfx) $(BASEDIR)/include/$(uadl_pfx)			\
					 $(BASEDIR)/include/$(graphite_pfx) $(BASEDIR)/share
	@cd $(BASEDIR)/include && \
	list='$(include_HEADERS)'; for file in $$list; do \
	  ln -fs $(BASEDIR)/$(pfx)/$$file; \
	done
	@cd $(BASEDIR)/include/ && \
	list='$(helpers_HEADERS) $(iss_HEADERS) $(lz4_stream_HEADERS) $(graphite_HEADERS) $(uadl_libuadl_la_HEADERS) $(uadl_libuadlSupport_la_HEADERS)'; for file in $$list; do \
	  ln -fs $(BASESRCDIR)/$$file $$file; \
	done
	@cd $(BASEDIR)/bin && \
	list='$(bin_SCRIPTS) $(bin_PROGRAMS)'; for file in $$list; do \
	  if [ ! -e $$file ] ; then \
	    ln -fs $(BASEDIR)/$$file; \
	  fi \
	done
	@cd $(BASEDIR)/lib && \
	list='$(install_libs)'; for file in $$list; do \
	  static_lib="$(BASEDIR)/`dirname $$file`/.libs/`basename $$file`.a" ; \
	  shared_lib="$(BASEDIR)/`dirname $$file`/.libs/`basename $$file`.so" ; \
	  if [ -f $${static_lib} ] ; then \
	  	ln -fs $${static_lib}; \
	  fi ; \
	  if [ -f $${shared_lib} ] ; then \
	  	ln -fs $${shared_lib}; \
	  	ln -fs $${shared_lib}.0; \
	  fi ; \
	done
	@cd $(BASEDIR)/share && \
	list='$(data_DATA)'; for file in $$list; do \
	  ln -fs $(BASESRCDIR)/$$file; \
	done

