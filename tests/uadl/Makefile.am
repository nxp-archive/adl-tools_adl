## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign serial-tests

TESTS = regress

ISS       = $(top_builddir)/iss
HELPER    = $(top_builddir)/helpers
PlasmaPfx = $(PLASMA_PATH)/lib

OPEN_MODELS = dlx dlx-safe dlx-bad-br dlx-ext-mem dlx-64 dlx-vd dlx-fwd					\
							dlx-fwd-safe dlx2 dlx2-lm dlx2-safe dlx2-safe-lm dlx2-ese					\
							dlx2-ese-safe dlx2-fwd dlx2-fwd-safe dlx2-fwd-vd dlx2-fwd-bad-br	\
							dlx2-bp2 dlx2-bp2-safe dlx2-bp3-safe dlx2-bp2-bad-br dlx2-pmr			\
							dlx2-pmr-safe dlx2-wide-safe dlx3 dlx-bp-fetch dlx-bp-fetch-safe	\
							dlx-bp-fetch-safe-ext-mem dlx-bp-issue dlx-bp-issue-safe dlx-z0		\
							dlx-z0-safe dlx-z0-ext-mem dlx-zl2-safe dlx-vle dlx-vle-safe			\
							dlx-vle-ic dlx-cache-split dlx-cache-split-safe										\
							dlx-cache-split-imm-safe dlx-cache-split-safe-ext-mem							\
							dlx-cache-split-pc-safe dlx-cache-split-pc2-safe									\
							dlx-cache-split-pc3-safe dlx-cache-split-pc3-safe-ext-mem					\
							dlx-cache-split-pc4-safe dlx-cache-split2 dlx-cache-split2-safe		\
							dlx-cache-unified dlx-cache-unified-safe dlx-l2 dlx-l2-safe				\
							dlx-stwcx dlx-stall dlx-stall-safe dlx-all dlx-all-safe dlx-hrvd	\
							dlx-hrvd-safe dlx-delay dlx-delay-safe dlx-bubble dlx-bubble-safe	\
							dlx-mt1 dlx-mt1-safe dlx-mt1-imm-safe dlx-mt2 dlx-mt3

if SYSTEMC

OPEN_MODELS += test-sysc-thread test-sysc-method test-sysc-pc2 test-sysc-mt1 test-sysc-base

TESTS += test-sysc-base

endif SYSTEMC

OPEN_MODEL_SRCS = dlx.uadl dlx-fwd.uadl dlx2.uadl dlx2-fwd.uadl dlx2.uadl			\
									dlx2-pmr.uadl dlx2-wide.uadl dlx3.uadl dlx-bp-fetch.uadl		\
									dlx-z0.uadl dlx-zl2.uadl dlx-vle.uadl dlx-bp-issue.uadl			\
									dlx-cache-split.uadl dlx-cache-split-pc.uadl								\
									dlx-cache-split-pc2.uadl dlx-cache-split-pc3.uadl						\
									dlx-cache-split-pc4.uadl dlx-cache-unified.uadl dlx-l2.uadl	\
									dlx-stwcx.uadl dlx-stall.uadl dlx-all.uadl dlx-hrvd.uadl		\
									dlx-bubble.uadl dlx-cache-split-pc.uadl dlx-delay.uadl			\
									dlx-mt1.uadl dlx-mt2.uadl dlx-mt3.uadl dlx2-bp2.uadl				\
									dlx2-bp3.uadl

# We distribute a few things here so that external users can write simple
# regressions.
data_DATA = \
	mod1-uadl.adl \
	dlx.uadl

MODELS = $(OPEN_MODELS)

MODEL_SRCS = $(MODELS:%=%.cc)
MODEL_INCS = $(MODELS:%=%.h)
MODEL_EXEC = $(MODELS)
MODEL_ISSS = $(MODELS:%=%-iss.cc.h)

check_PROGRAMS = $(OPEN_MODELS) $(CLOSE_MODELS)

## These should be restored as soon as hybrid/iss-mem-mode is fully working in uADL.
Hybrid = --hybrid-iss
IssMemMode = --iss-mem-mode


## Distributed files.
EXTRA_DIST = \
	$(srcdir)/regress \
	$(filter-out %.gen.dat,$(filter-out %.out.dat,$(wildcard $(srcdir)/*.dat))) \
	$(OPEN_MODEL_SRCS) \
	mod1-uadl.adl \
	mod1-fwd.adl \
	mod1-stwcx.adl \
	mod11-uadl.adl \
	mod11-ese.adl \
	mod21-uadl.adl \
	mod23-uadl.adl \
	mod23-hrvd.adl \
	mod13-mt1.adl \
	mod13-mt2.adl \
	mod13-mt3.adl \
	z0.adl \
	zl2.adl \
	zl2.ttc \
	lf-128.tcl \
	generic32-cache-l1.adl \
	generic32-cache-no-mmu.adl \
	dlx_custom_bp.h \
	dlx_custom_cr.h \
	LinkStackPredictor.h \
	$(wildcard $(srcdir)/*.cli) \
	$(wildcard $(srcdir)/*.regress) \
	MyCache.h \
	lf-128.tcl

## Rules for building simulators.
uADL_srcdir   = $(top_srcdir)/uadl
uADL_builddir = $(top_builddir)/uadl

AM_CPPFLAGS = $(PLASMA_CFLAGS) $(RNUMBER_CFLAGS) -I$(uADL_srcdir) -I$(top_srcdir)

MODEL_LDADD = $(uADL_builddir)/libuadlSupport.la \
							$(uADL_builddir)/libuadl.la \
              $(ISS)/libAdlIssExec.la \
              $(ISS)/libAdlReaders.la \
              $(ISS)/libAdlParsing.la \
              $(ISS)/libAdlIssSupport.la \
              $(ISS)/libAdlIssTransLogging.la \
              $(ISS)/libAdlScript.la \
              $(HELPER)/libIntbv.la \
              $(HELPER)/libIntbvRnum.la \
              $(HELPER)/libAdlSupport.la \
			  			$(HELPER)/libAdlLogUsage.la \
	      	  	$(TCL_LIB_PATHS) $(TCL_LIBS) \
              $(RNUMBER_LIBS) \
	          	$(LZ4_LTLIBS) \
							$(DYNEXPORT) \
							$(BOOST_LDFLAGS) -lboost_thread -lboost_system -lboost_regex -lboost_atomic -lboost_context -lboost_iostreams -lrt \
              $(PlasmaPfx)/libltdl.la \
              $(PlasmaPfx)/libgc.la

EXEC_LDADD = $(uADL_builddir)/libuadlMain.la $(MODEL_LDADD) 

PARSER = $(uADL_builddir)/make-uadl

IssDir = $(top_srcdir)/tests/iss

DesDir = $(top_srcdir)/designs

# Run check with arguments to run the regressions in parallel.
pcheck:
	$(MAKE) check RegressOpts="-p=8"

check-programs: all-am
	$(MAKE) $(AM_MAKEFLAGS) $(check_PROGRAMS) 

# This build variable takes the first two prerequisites.  So, this means that
# the ADL and uADL source files should be the first two prerequisites listed.
# This may then be followed by other prereqs, which will not be used during the
# build.  By convention, the uADL file comes first, followewd by the ADL file.
UADLBUILD = $(AM_V_GEN) $(PARSER) $(word 2, $^) $(word 1, $^) --no-warn-redefine

define UADL_template
$(1).cc: $(2) $(PARSER)
	$$(UADLBUILD) -o $(1) $3

$(1)-iss.$(OBJEXT): $(1).cc

# Dummy target so that we don't get an error because it doesn't know how to build the .cc file.
$(1)-iss.cc: $(1).cc

endef

define UADL_safe_template
$(1).cc: $(2) $(PARSER)
	$$(UADLBUILD) --safe-mode -o $(1) $3

$(1)-iss.$(OBJEXT): $(1).cc

# Dummy target so that we don't get an error because it doesn't know how to build the .cc file.
$(1)-iss.cc: $(1).cc

endef

define UADL_imm_safe_template
$(1).cc: $(2) $(PARSER)
	$$(UADLBUILD) --iss-mem-mode --safe-mode $$(IssMemMode) -o $(1) $3

$(1)-iss.$(OBJEXT): $(1).cc

# Dummy target so that we don't get an error because it doesn't know how to build the .cc file.
$(1)-iss.cc: $(1).cc

endef

# To simplify things, I've created the UADL_template and UADL_SAFE_template for
# generating these rules automatically.  Just call/eval the appropriate template
# for generating either a normal or safe-mode model.  The arguments are:
#
# $1: Model name, e.g. "dlx"
#
# $2: All dependencies.  The parser is automatically added as a dependency, so
# that doesn't need to be specified in your arguments.  However, the first
# argument must be the uADL file and the second argument must be the main ADL
# file.  Extra arguments can then be listed, such as other ADL files it's
# dependent upon.
#
# $3: List any other arguments to be supplied to the parser.

# Test to make sure that we can compile the ISS separately and then link them together.
dlx_SOURCES = dlx.cc
dlx_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx,$(srcdir)/dlx.uadl $(srcdir)/mod1-uadl.adl $(IssDir)/mod1.adl,-I=$(IssDir)))

# This version uses virtual dispatch, as opposed to templates.
dlx_vd_SOURCES = dlx-vd.cc
dlx_vd_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx-vd,$(srcdir)/dlx.uadl $(srcdir)/mod1-uadl.adl $(IssDir)/mod1.adl,-I=$(IssDir) --instr-class-vd))

dlx_safe_SOURCES = dlx-safe.cc 
dlx_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-safe,$(srcdir)/dlx.uadl $(srcdir)/mod1-uadl.adl $(IssDir)/mod1.adl,-I=$(IssDir)))

dlx_bad_br_SOURCES = dlx-bad-br.cc 
dlx_bad_br_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-bad-br,$(srcdir)/dlx.uadl $(srcdir)/mod1-uadl.adl $(IssDir)/mod1.adl,-I=$(IssDir) -D=BAD_BRANCH))

dlx_ext_mem_SOURCES = dlx-ext-mem.cc  test-ext-mem.C
dlx_ext_mem_LDADD = $(MODEL_LDADD)
$(eval $(call UADL_template,dlx-ext-mem,$(srcdir)/dlx.uadl $(IssDir)/mod1.adl, --extern-mem))

dlx_64_SOURCES = dlx-64.cc 
dlx_64_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx-64,$(srcdir)/dlx.uadl $(srcdir)/mod1-uadl.adl $(IssDir)/mod1.adl,-I=$(IssDir) -D=MemWidth=64))

dlx_fwd_SOURCES = dlx-fwd.cc 
dlx_fwd_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx-fwd,$(srcdir)/dlx-fwd.uadl $(srcdir)/mod1-fwd.adl $(IssDir)/mod1.adl,-I=$(IssDir) -I=$(srcdir) -D=UADL))

dlx_fwd_safe_SOURCES = dlx-fwd-safe.cc 
dlx_fwd_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-fwd-safe,$(srcdir)/dlx-fwd.uadl $(srcdir)/mod1-fwd.adl $(IssDir)/mod1.adl,-I=$(IssDir)  -I=$(srcdir) -D=UADL))

dlx2_SOURCES = dlx2.cc 
dlx2_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx2,$(srcdir)/dlx2.uadl $(IssDir)/mod11.adl, --instr-cache))

dlx2_lm_SOURCES = dlx2-lm.cc 
dlx2_lm_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx2-lm,$(srcdir)/dlx2.uadl $(IssDir)/mod11.adl, --instr-cache --low-mem))

dlx2_safe_SOURCES = dlx2-safe.cc 
dlx2_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx2-safe,$(srcdir)/dlx2.uadl $(IssDir)/mod11.adl))

dlx2_safe_lm_SOURCES = dlx2-safe-lm.cc 
dlx2_safe_lm_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx2-safe-lm,$(srcdir)/dlx2.uadl $(IssDir)/mod11.adl, --low-mem))

dlx2_ese_SOURCES = dlx2-ese.cc 
dlx2_ese_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx2-ese,$(srcdir)/dlx2.uadl $(srcdir)/mod11-ese.adl, --instr-cache -I=$(IssDir)))

dlx2_ese_safe_SOURCES = dlx2-ese-safe.cc 
dlx2_ese_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx2-ese-safe,$(srcdir)/dlx2.uadl $(srcdir)/mod11-ese.adl, --instr-cache -I=$(IssDir)))

dlx2_fwd_SOURCES = dlx2-fwd.cc 
dlx2_fwd_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx2-fwd,$(srcdir)/dlx2-fwd.uadl $(IssDir)/mod11.adl, --instr-cache  --debug-mode))

dlx2_fwd_vd_SOURCES = dlx2-fwd-vd.cc 
dlx2_fwd_vd_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx2-fwd-vd,$(srcdir)/dlx2-fwd.uadl $(IssDir)/mod11.adl, --instr-class-vd --debug-mode))

dlx2_fwd_safe_SOURCES = dlx2-fwd-safe.cc 
dlx2_fwd_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx2-fwd-safe,$(srcdir)/dlx2-fwd.uadl $(IssDir)/mod11.adl, --instr-cache --debug-mode))

dlx2_fwd_bad_br_SOURCES = dlx2-fwd-bad-br.cc 
dlx2_fwd_bad_br_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx2-fwd-bad-br,$(srcdir)/dlx2-fwd.uadl $(IssDir)/mod11.adl, --instr-cache --debug-mode -D=BAD_BRANCH --detect-deadlock=1000))

dlx2_bp2_SOURCES = dlx2-bp2.cc 
dlx2_bp2_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx2-bp2,$(srcdir)/dlx2-bp2.uadl $(IssDir)/mod11.adl, -I=$(srcdir) --instr-cache  --debug-mode))

dlx2_bp2_safe_SOURCES = dlx2-bp2-safe.cc 
dlx2_bp2_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx2-bp2-safe,$(srcdir)/dlx2-bp2.uadl $(IssDir)/mod11.adl, -I=$(srcdir) --instr-cache --debug-mode))

dlx2_bp3_safe_SOURCES = dlx2-bp3-safe.cc 
dlx2_bp3_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx2-bp3-safe,$(srcdir)/dlx2-bp3.uadl $(IssDir)/mod11.adl, -I=$(srcdir) --instr-cache --debug-mode))

dlx2_bp2_bad_br_SOURCES = dlx2-bp2-bad-br.cc 
dlx2_bp2_bad_br_LDADD = $(EXEC_LDADD)
dlx2_bp2_bad_br_CXXFLAGS = -DBAD_PREDICTION
$(eval $(call UADL_safe_template,dlx2-bp2-bad-br,$(srcdir)/dlx2-bp2.uadl $(IssDir)/mod11.adl, -I=$(srcdir) --instr-cache --debug-mode -D=BAD_RFI))

dlx2_pmr_SOURCES = dlx2-pmr.cc 
dlx2_pmr_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx2-pmr,$(srcdir)/dlx2-pmr.uadl $(IssDir)/mod11.adl))

dlx2_pmr_safe_SOURCES = dlx2-pmr-safe.cc 
dlx2_pmr_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx2-pmr-safe,$(srcdir)/dlx2-pmr.uadl $(IssDir)/mod11.adl, --instr-cache))

dlx2_wide_safe_SOURCES = dlx2-wide-safe.cc 
dlx2_wide_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx2-wide-safe,$(srcdir)/dlx2-wide.uadl $(srcdir)/mod21-uadl.adl $(IssDir)/mod21.adl, -D=NO_L2 -I=$(IssDir) -D=WRITE_MODE=WriteBack --instr-cache))

dlx3_SOURCES = dlx3.cc 
dlx3_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx3,$(srcdir)/dlx3.uadl $(IssDir)/mod1.adl))

dlx_bp_fetch_SOURCES = dlx-bp-fetch.cc 
dlx_bp_fetch_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx-bp-fetch,$(srcdir)/dlx-bp-fetch.uadl $(srcdir)/mod11-uadl.adl $(IssDir)/mod11.adl,-I=$(IssDir) -I=$(srcdir) ))

dlx_bp_fetch_safe_SOURCES = dlx-bp-fetch-safe.cc 
dlx_bp_fetch_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-bp-fetch-safe,$(srcdir)/dlx-bp-fetch.uadl $(srcdir)/mod11-uadl.adl $(IssDir)/mod11.adl,-I=$(IssDir) -I=$(srcdir) ))

dlx_bp_fetch_safe_ext_mem_SOURCES = dlx-bp-fetch-safe-ext-mem.cc  test-ext-mem-excp.C
dlx_bp_fetch_safe_ext_mem_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_imm_safe_template,dlx-bp-fetch-safe-ext-mem,$(srcdir)/dlx-bp-fetch.uadl $(srcdir)/mod11-uadl.adl $(IssDir)/mod11.adl, --extern-mem -I=$(IssDir) -I=$(srcdir) ))

dlx_bp_issue_SOURCES = dlx-bp-issue.cc 
dlx_bp_issue_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx-bp-issue,$(srcdir)/dlx-bp-issue.uadl $(srcdir)/mod11-uadl.adl $(IssDir)/mod11.adl,-I=$(IssDir) --debug-mode))

dlx_bp_issue_safe_SOURCES = dlx-bp-issue-safe.cc 
dlx_bp_issue_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-bp-issue-safe,$(srcdir)/dlx-bp-issue.uadl $(srcdir)/mod11-uadl.adl $(IssDir)/mod11.adl,-I=$(IssDir) --debug-mode))

dlx_z0_SOURCES = dlx-z0.cc 
dlx_z0_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx-z0,$(srcdir)/dlx-z0.uadl $(srcdir)/z0.adl, -I=$(DesDir)))

dlx_z0_safe_SOURCES = dlx-z0-safe.cc 
dlx_z0_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-z0-safe,$(srcdir)/dlx-z0.uadl $(srcdir)/z0.adl, -I=$(DesDir)))

dlx_z0_ext_mem_SOURCES = dlx-z0-ext-mem.cc  test-ext-mem.C
dlx_z0_ext_mem_LDADD = $(MODEL_LDADD)
$(eval $(call UADL_safe_template,dlx-z0-ext-mem,$(srcdir)/dlx-z0.uadl $(srcdir)/z0.adl, -I=$(DesDir) --extern-mem))

dlx_zl2_safe_SOURCES = dlx-zl2-safe.cc 
dlx_zl2_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-zl2-safe,$(srcdir)/dlx-zl2.uadl $(srcdir)/zl2.adl, -I=$(DesDir) --config=$(srcdir)/zl2.ttc))

dlx-zl2-safe.cc: $(srcdir)/zl2.ttc

dlx_vle_SOURCES = dlx-vle.cc 
dlx_vle_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx-vle,$(srcdir)/dlx-vle.uadl $(IssDir)/mod27.adl,-I=$(srcdir)))

# Test a variable-width instruction model with the use of the instruction cache.
dlx_vle_ic_SOURCES = dlx-vle-ic.cc 
dlx_vle_ic_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx-vle-ic,$(srcdir)/dlx-vle.uadl $(IssDir)/mod27.adl,-I=$(srcdir) --instr-cache))

dlx_vle_safe_SOURCES = dlx-vle-safe.cc 
dlx_vle_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-vle-safe,$(srcdir)/dlx-vle.uadl $(IssDir)/mod27.adl,-I=$(srcdir)))

dlx_cache_split_SOURCES = dlx-cache-split.cc 
dlx_cache_split_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx-cache-split,$(srcdir)/dlx-cache-split.uadl $(srcdir)/mod21-uadl.adl $(IssDir)/mod21.adl,-I=$(IssDir) -D=NO_L2 -D=WRITE_MODE=WriteBack))

dlx_cache_split_safe_SOURCES = dlx-cache-split-safe.cc 
dlx_cache_split_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-cache-split-safe,$(srcdir)/dlx-cache-split.uadl $(srcdir)/mod21-uadl.adl $(IssDir)/mod21.adl,-I=$(IssDir) --tags-only-caches -D=NO_L2 -D=WRITE_MODE=WriteBack --config=$(IssDir)/mod21.ttc))

dlx-cache-split-safe.cc: $(IssDir)/mod21.ttc

dlx_cache_split_imm_safe_SOURCES = dlx-cache-split-imm-safe.cc 
dlx_cache_split_imm_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_imm_safe_template,dlx-cache-split-imm-safe,$(srcdir)/dlx-cache-split.uadl $(srcdir)/mod21-uadl.adl $(IssDir)/mod21.adl, -D=NO_L2 -I=$(IssDir) -D=WRITE_MODE=WriteBack --config=$(IssDir)/mod21.ttc))

dlx-cache-split-imm-safe.cc: $(IssDir)/mod21.ttc

dlx_cache_split_safe_ext_mem_SOURCES = dlx-cache-split-safe-ext-mem.cc  test-ext-mem-excp.C
dlx_cache_split_safe_ext_mem_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_imm_safe_template,dlx-cache-split-safe-ext-mem,$(srcdir)/dlx-cache-split.uadl $(srcdir)/mod21-uadl.adl $(IssDir)/mod21.adl, --extern-mem -D=NO_L2 -I=$(IssDir) --config=$(IssDir)/mod21.ttc))

dlx-cache-split-imm-ext-mem.cc: $(IssDir)/mod21.ttc

dlx_cache_split_pc_safe_SOURCES = dlx-cache-split-pc-safe.cc 
dlx_cache_split_pc_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-cache-split-pc-safe,$(srcdir)/dlx-cache-split-pc.uadl $(srcdir)/mod21-uadl.adl $(IssDir)/mod21.adl, -D=NO_L2 -I=$(IssDir) --config=$(IssDir)/mod21.ttc -D=WRITE_MODE=WriteBack) )

dlx-cache-split-pc-safe.cc: $(IssDir)/mod21.ttc

dlx_cache_split_pc2_safe_SOURCES = dlx-cache-split-pc2-safe.cc 
dlx_cache_split_pc2_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-cache-split-pc2-safe,$(srcdir)/dlx-cache-split-pc2.uadl $(srcdir)/mod21-uadl.adl $(IssDir)/mod21.adl, -D=NO_L2 -I=$(IssDir) --config=$(IssDir)/mod21.ttc -D=WRITE_MODE=WriteBack) )

dlx-cache-split-pc2-safe.cc: $(IssDir)/mod21.ttc

dlx_cache_split_pc3_safe_SOURCES = dlx-cache-split-pc3-safe.cc 
dlx_cache_split_pc3_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-cache-split-pc3-safe,$(srcdir)/dlx-cache-split-pc3.uadl $(srcdir)/mod21-uadl.adl $(IssDir)/mod21.adl, -D=NO_L2 -I=$(IssDir) --config=$(IssDir)/mod21.ttc -D=WRITE_MODE=WriteBack --detect-deadlock=200) )

dlx-cache-split-pc3-safe.cc: $(IssDir)/mod21.ttc

dlx_cache_split_pc3_safe_ext_mem_SOURCES = dlx-cache-split-pc3-safe-ext-mem.cc  test-ext-mem.C
dlx_cache_split_pc3_safe_ext_mem_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-cache-split-pc3-safe-ext-mem,$(srcdir)/dlx-cache-split-pc3.uadl $(srcdir)/mod21-uadl.adl $(IssDir)/mod21.adl, -D=NO_L2 --extern-mem -I=$(IssDir) -I=$(srcdir) ))

dlx_cache_split_pc4_safe_SOURCES = dlx-cache-split-pc4-safe.cc 
dlx_cache_split_pc4_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-cache-split-pc4-safe,$(srcdir)/dlx-cache-split-pc4.uadl $(srcdir)/mod21-uadl.adl $(IssDir)/mod21.adl, -D=NO_L2 -I=$(IssDir) --config=$(IssDir)/mod21.ttc -D=WRITE_MODE=WriteBack --detect-deadlock=200) )

dlx-cache-split-pc4-safe.cc: $(IssDir)/mod21.ttc

dlx_cache_split2_SOURCES = dlx-cache-split2.cc 
dlx_cache_split2_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx-cache-split2,$(srcdir)/dlx-cache-split.uadl $(IssDir)/mod23.adl))

dlx_cache_split2_safe_SOURCES = dlx-cache-split2-safe.cc 
dlx_cache_split2_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-cache-split2-safe,$(srcdir)/dlx-cache-split.uadl $(IssDir)/mod23.adl))

dlx_cache_unified_SOURCES = dlx-cache-unified.cc 
dlx_cache_unified_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx-cache-unified,$(srcdir)/dlx-cache-unified.uadl $(srcdir)/mod23-uadl.adl $(IssDir)/mod23.adl, -I=$(IssDir)))

dlx_cache_unified_safe_SOURCES = dlx-cache-unified-safe.cc 
dlx_cache_unified_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-cache-unified-safe,$(srcdir)/dlx-cache-unified.uadl $(srcdir)/mod23-uadl.adl $(IssDir)/mod23.adl, -I=$(IssDir)))

dlx_l2_SOURCES = dlx-l2.cc 
dlx_l2_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx-l2,$(srcdir)/dlx-l2.uadl $(IssDir)/mod21.adl))

dlx_l2_safe_SOURCES = dlx-l2-safe.cc 
dlx_l2_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-l2-safe,$(srcdir)/dlx-l2.uadl $(IssDir)/mod21.adl))

dlx_stwcx_SOURCES = dlx-stwcx.cc 
dlx_stwcx_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-stwcx,$(srcdir)/dlx-stwcx.uadl $(srcdir)/mod1-stwcx.adl $(IssDir)/mod1.adl,-I=$(IssDir)))

dlx_stall_SOURCES = dlx-stall.cc 
dlx_stall_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx-stall,$(srcdir)/dlx-stall.uadl $(IssDir)/mod1.adl,--detect-deadlock=100 ))

dlx_stall_safe_SOURCES = dlx-stall-safe.cc 
dlx_stall_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-stall-safe,$(srcdir)/dlx-stall.uadl $(IssDir)/mod1.adl,--detect-deadlock=100 ))

dlx_all_SOURCES = dlx-all.cc 
dlx_all_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx-all,$(srcdir)/dlx-all.uadl $(IssDir)/mod23.adl))

dlx_all_safe_SOURCES = dlx-all-safe.cc 
dlx_all_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-all-safe,$(srcdir)/dlx-all.uadl $(IssDir)/mod23.adl))

dlx_hrvd_SOURCES = dlx-hrvd.cc 
dlx_hrvd_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx-hrvd,$(srcdir)/dlx-hrvd.uadl $(srcdir)/mod23-hrvd.adl $(IssDir)/mod23.adl,-I=$(IssDir)))

dlx_hrvd_safe_SOURCES = dlx-hrvd-safe.cc 
dlx_hrvd_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-hrvd-safe,$(srcdir)/dlx-hrvd.uadl $(srcdir)/mod23-hrvd.adl $(IssDir)/mod23.adl,-I=$(IssDir)))

dlx_delay_SOURCES = dlx-delay.cc 
dlx_delay_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx-delay,$(srcdir)/dlx-delay.uadl $(IssDir)/mod1.adl))

dlx_delay_safe_SOURCES = dlx-delay-safe.cc 
dlx_delay_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-delay-safe,$(srcdir)/dlx-delay.uadl $(IssDir)/mod1.adl))

dlx_bubble_SOURCES = dlx-bubble.cc 
dlx_bubble_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx-bubble,$(srcdir)/dlx-bubble.uadl $(IssDir)/mod1.adl))

dlx_bubble_safe_SOURCES = dlx-bubble-safe.cc 
dlx_bubble_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-bubble-safe,$(srcdir)/dlx-bubble.uadl $(IssDir)/mod1.adl))

dlx_mt1_SOURCES = dlx-mt1.cc 
dlx_mt1_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx-mt1,$(srcdir)/dlx-mt1.uadl $(srcdir)/mod13-mt1.adl, -I=$(IssDir)))

dlx-mt1.cc: $(IssDir)/mod13.adl

dlx_mt1_safe_SOURCES = dlx-mt1-safe.cc 
dlx_mt1_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_safe_template,dlx-mt1-safe,$(srcdir)/dlx-mt1.uadl $(srcdir)/mod13-mt1.adl, -I=$(IssDir) --detect-deadlock=200 --config=$(IssDir)/mod13.ttc))

dlx-mt1-safe.cc: $(IssDir)/mod13.adl

dlx_mt1_imm_safe_SOURCES = dlx-mt1-imm-safe.cc 
dlx_mt1_imm_safe_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_imm_safe_template,dlx-mt1-imm-safe,$(srcdir)/dlx-mt1.uadl $(srcdir)/mod13-mt1.adl, -I=$(IssDir) --detect-deadlock=200 --config=$(IssDir)/mod13.ttc))

dlx-mt1-imm-safe.cc: $(IssDir)/mod13.adl

dlx_mt2_SOURCES = dlx-mt2.cc 
dlx_mt2_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx-mt2,$(srcdir)/dlx-mt2.uadl $(srcdir)/mod13-mt2.adl, -I=$(IssDir)))

dlx_mt3_SOURCES = dlx-mt3.cc 
dlx_mt3_LDADD = $(EXEC_LDADD)
$(eval $(call UADL_template,dlx-mt3,$(srcdir)/dlx-mt3.uadl $(srcdir)/mod13-mt3.adl, -I=$(IssDir)))

dlx-sysc-thread.cc: $(srcdir)/dlx.uadl $(IssDir)/mod1.adl $(PARSER)
	$(UADLBUILD) --systemc=thread --extern-mem -o $(patsubst %.cc,%,$@)

dlx-sysc-method.cc: $(srcdir)/dlx.uadl $(IssDir)/mod1.adl $(PARSER)
	$(UADLBUILD) --systemc=method --extern-mem -o $(patsubst %.cc,%,$@)

dlx-sysc-base.cc: $(srcdir)/dlx.uadl $(IssDir)/mod1.adl $(PARSER)
	$(UADLBUILD) --systemc --extern-mem --namespace=dlx --no-rnumber --no-syscall-enabled -o $(patsubst %.cc,%,$@)

dlx-sysc-pc2.cc: $(srcdir)/dlx-cache-split-pc2.uadl $(srcdir)/mod21-uadl.adl $(IssDir)/mod21.adl $(PARSER)
	$(UADLBUILD) -I=$(IssDir) --systemc=method --extern-mem -D=NO_L2  -D=WRITE_MODE=WriteBack -o=dlx-sysc-pc2

dlx-sysc-mt1.cc: $(srcdir)/dlx-mt1.uadl $(srcdir)/mod13-mt1.adl $(IssDir)/mod13.adl $(PARSER)
	$(UADLBUILD) -I=$(IssDir) --systemc=method --extern-mem -D=NO_L2  -D=WRITE_MODE=WriteBack -o=dlx-sysc-mt1

if SYSTEMC

test_sysc_thread_SOURCES  = dlx-sysc-thread.cc  test-systemc.C
test_sysc_thread_CXXFLAGS = $(SYSTEMC_CFLAGS)
test_sysc_thread_LDFLAGS = $(SYSTEMC_LIBS) $(MODEL_LDADD)

test_sysc_method_SOURCES  = dlx-sysc-method.cc  test-systemc.C
test_sysc_method_CXXFLAGS = $(SYSTEMC_CFLAGS)
test_sysc_method_LDFLAGS = $(SYSTEMC_LIBS) $(MODEL_LDADD)

test_sysc_base_SOURCES  = dlx-sysc-base.cc  test-sysc-base.C
test_sysc_base_CXXFLAGS = $(SYSTEMC_CFLAGS)
test_sysc_base_LDFLAGS  = $(SYSTEMC_LIBS) $(MODEL_LDADD)

test_sysc_pc2_SOURCES  = dlx-sysc-pc2.cc  test-systemc.C
test_sysc_pc2_CXXFLAGS = $(SYSTEMC_CFLAGS) -DMODEL='"dlx-sysc-pc2.h"'
test_sysc_pc2_LDFLAGS = $(SYSTEMC_LIBS) $(MODEL_LDADD)

test_sysc_mt1_SOURCES  = dlx-sysc-mt1.cc  test-systemc.C
test_sysc_mt1_CXXFLAGS = $(SYSTEMC_CFLAGS) -DMODEL='"dlx-sysc-mt1.h"'
test_sysc_mt1_LDFLAGS = $(SYSTEMC_LIBS) $(MODEL_LDADD)

endif SYSTEMC

.SECONDARY: $(MODEL_SRCS) $(MODEL_INCS)

CLEANFILES = $(MODEL_SRCS) $(MODEL_INCS) $(MODEL_EXEC) $(MODEL_ISSS) *.cc dlx-sysc-method.h dlx-sysc-pc2.h dlx-sysc-thread.h dlx-sysc-base.h

