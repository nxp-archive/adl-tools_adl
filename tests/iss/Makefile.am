
AUTOMAKE_OPTIONS = foreign serial-tests no-exeext

TESTS = coro_regress queuetest

check_PROGRAMS = corotest

if !MINIMAL_BUILD

check_PROGRAMS += mod1-simple

TESTS += regress

if LANG_FRONTEND

check_PROGRAMS += queuetest memtest-interleaved32 memtest-noninterleaved32			\
	memtest-interleaved64 memtest-noninterleaved64 memtest-tlm2-interleaved32			\
	memtest-tlm2-noninterleaved32 memtest-tlm2-interleaved64											\
	memtest-tlm2-noninterleaved64 binutils-tests mod1 mod2 mod3 mod3rp mod4 mod5	\
	mod6 mod7 mod8 mod9 mod10 mod11 mod12 mod13 mod14 mod15 mod16 mod17 mod18			\
	mod19 mod20 mod21 mod22 mod23 mod25 mod26 mod27 mod28 mod29 mod30 mod31 mod32	\
	mod34 mod36 mod37 mod38 mod39 mod40 mod41 mod42 mod43 mod46 mod47 mod48 mod50	\
	mod51 mod52 mod56 mod57 mod61 mod63 mod66 mod67 mod69 mod70 mod72 mod73 mod75	\
	mod76 mod77 mod78 mod79 mod82 mod83 mod84 mod85 mod86 mod88 mod89 mod90 mod92	\
	mod93 mod94 mod95 mod98 mod99 mod100 mod101 mod102 mod103 mod106 mod107				\
	ttmod1 ttmod2 ttmod3 tdriver1 tdriver2 tdriver3 tdriver4 tdriver5 tdriver6		\
	tdriver7 tdriver8 tdriver9 smdriver1 smdriver2 smdriver3 smdriver4 smdriver5	\
	smdriver6 smdriver7 libdriver1 count-instrs.so count-branches.so							\
	branch-callback.so exception-callback.so pic-uart.so ext-hrvd.so microinstr6	\
	microinstr7 microinstr9 microinstr10 microinstr12 microinstr13 microinstr14		\
	simplified_arm simplified_armv8

noinst_LIBRARIES = libtcmod101.a

TESTS += queuetest \
				 memtest-interleaved32 memtest-noninterleaved32 memtest-interleaved64 memtest-noninterleaved64 \
				 memtest-tlm2-interleaved32 memtest-tlm2-noninterleaved32 memtest-tlm2-interleaved64 memtest-tlm2-noninterleaved64

mod1-simple.cc: mod1.adl $(MakeIss)

mod18.cc.h: mod18.adl $(MakeIss)

mod18sm.cc.h: mod18.adl $(MakeIss)

mod1-lib.cc.h: mod1.adl $(MakeIss)

mod3.cc.h: mod3.adl $(MakeIss)

mod3sm.cc.h: mod3.adl $(MakeIss)

mod3sm-h.cc.h: mod3.adl $(MakeIss)

mod3_SMH_ADLFLAGS = --track-reads

mod2.cc.h: mod2.adl $(MakeIss)

mod6.cc.h: mod6.adl $(MakeIss)

mod10.cc.h: mod10.adl $(MakeIss)

mod11.cc.h: mod11.adl $(MakeIss)

mod11sm.cc.h: mod11.adl $(MakeIss)

mod11sm-h.cc.h: mod11.adl $(MakeIss)

mod11_SMH_ADLFLAGS = --track-reads

mod48sm-h.cc.h: mod48.adl $(MakeIss)

mod67sm-h.cc.h: mod67.adl $(MakeIss)

mod49.cc.h: mod49.adl $(MakeIss)

mod69.cc.h: mod69.adl $(MakeIss)

BUILT_SOURCES = mod1-simple.cc mod1-lib.cc.h mod1split8.cc.h mod18.cc.h mod3.cc.h mod2.cc.h mod6.cc.h mod11.cc.h mod10.cc.h mod49.cc.h \
				mod18sm.cc.h mod3sm.cc.h mod3sm-h.cc.h mod11sm.cc.h mod11sm-h.cc.h mod48sm-h.cc.h mod67sm-h.cc.h mod69.cc.h

endif
endif

# Note:  We use .cc as an extension for generated models so
# that we can remove them all using a pattern.  Files with .C
# extensions should be original files, not generated.

CLEANFILES = \
	*.cc  *.cc.h *.bc \
	as-mod*.cc objdump-mod*.cc \
	as-mod1 objdump-mod1 \
	as-mod8 objdump-mod8 \
	as-mod33 objdump-mod33 \
	as-mod35 objdump-mod35 \
	as-mod43 objdump-mod43 \
	as-mod44 objdump-mod44 \
	as-mod45 objdump-mod45 \
	as-mod46 objdump-mod46 \
	as-mod47 objdump-mod47 \
	as-mod53 objdump-mod53 \
	as-mod54 objdump-mod54 \
	as-mod55 objdump-mod55 \
	as-mod58 objdump-mod58 \
	as-mod59 objdump-mod59 \
	as-mod60 objdump-mod60 \
	as-mod61 objdump-mod61 \
	as-mod62 objdump-mod62 \
	as-mod63 objdump-mod63 \
	as-mod64 objdump-mod64 \
	as-mod65 objdump-mod65 \
	as-mod67 objdump-mod67 \
	as-mod68 objdump-mod68 \
	as-mod71 objdump-mod71 \
	as-mod74 objdump-mod74 \
	as-mod75 objdump-mod75 \
	as-mod76 objdump-mod76 \
	as-mod77 objdump-mod77 \
	as-mod80 objdump-mod80 \
	as-mod81 objdump-mod81 \
	as-mod87 objdump-mod87 \
	as-mod91 objdump-mod91 \
	as-mod96 objdump-mod96 \
	as-mod97 objdump-mod97 \
	as-mod99 objdump-mod99 \
	as-mod100 objdump-mod100 \
	as-mod101 objdump-mod101 \
	as-mod104 objdump-mod104 \
	as-mod105 objdump-mod105 \
	as-mod106 objdump-mod106 \
	as-mod107 objdump-mod107 \
	as-mod108 objdump-mod108 \
	as-mod109 objdump-mod109 \
	as-mod110 objdump-mod110 \
	as-microinstr8 objdump-microinstr8 \
	as-microinstr9 objdump-microinstr9 \
	as-microinstr10 objdump-microinstr10 \
	as-microinstr12 objdump-microinstr12 \
	as-microinstr13 objdump-microinstr13 \
	as-microinstr14 objdump-microinstr14 \
	as-microinstr15 objdump-microinstr15 \
	as-microinstr16 objdump-microinstr16 \
	as-microinstr17 objdump-microinstr17 \
	as-mod-alias1 objdump-mod-alias1 \
	as-mod-alias2 objdump-mod-alias2


MakeIss = $(Iss)/make-iss

Designs = $(top_srcdir)/designs

corotest_SOURCES = \
	corotest.C

queuetest_SOURCES = \
	queuetest.C

memtest: memtest-interleaved32 memtest-noninterleaved32 memtest-interleaved64 memtest-noninterleaved64

memtest-tlm2: memtest-tlm2-interleaved32 memtest-tlm2-noninterleaved32 memtest-tlm2-interleaved64 memtest-tlm2-noninterleaved64

memtest_interleaved32_SOURCES = memtest.C

memtest_interleaved32_CXXFLAGS = -DInterleavedFlag=true -DSplitSizeFlag=32

memtest_interleaved32_LDADD = $(memtest_libraries)


memtest_noninterleaved32_SOURCES = memtest.C

memtest_noninterleaved32_CXXFLAGS = -DInterleavedFlag=false  -DSplitSizeFlag=32

memtest_noninterleaved32_LDADD = $(memtest_libraries)


memtest_interleaved64_SOURCES = memtest.C

memtest_interleaved64_CXXFLAGS = -DInterleavedFlag=true -DSplitSizeFlag=64

memtest_interleaved64_LDADD = $(memtest_libraries)


memtest_noninterleaved64_SOURCES = memtest.C

memtest_noninterleaved64_CXXFLAGS = -DInterleavedFlag=false -DSplitSizeFlag=64

memtest_noninterleaved64_LDADD = $(memtest_libraries)

# The "TLM2" tests are the data-invariant versions- rather than byte-swapping, we
# do address swizzling.  This kind of memory layout is what's used by TLM 2.0
# DMI.

memtest_tlm2_interleaved32_SOURCES = memtest.C

memtest_tlm2_interleaved32_CXXFLAGS = -DInterleavedFlag=true -DSplitSizeFlag=32 -DDATA_INVARIANT_MEMORY

memtest_tlm2_interleaved32_LDADD = $(memtest_libraries)


memtest_tlm2_noninterleaved32_SOURCES = memtest.C

memtest_tlm2_noninterleaved32_CXXFLAGS = -DInterleavedFlag=false  -DSplitSizeFlag=32 -DDATA_INVARIANT_MEMORY

memtest_tlm2_noninterleaved32_LDADD = $(memtest_libraries)


memtest_tlm2_interleaved64_SOURCES = memtest.C

memtest_tlm2_interleaved64_CXXFLAGS = -DInterleavedFlag=true -DSplitSizeFlag=64 -DDATA_INVARIANT_MEMORY

memtest_tlm2_interleaved64_LDADD = $(memtest_libraries)


memtest_tlm2_noninterleaved64_SOURCES = memtest.C

memtest_tlm2_noninterleaved64_CXXFLAGS = -DInterleavedFlag=false -DSplitSizeFlag=64 -DDATA_INVARIANT_MEMORY

memtest_tlm2_noninterleaved64_LDADD = $(memtest_libraries)


libdriver1_SOURCES = \
	libdriver.C

libdriver1_CXXFLAGS = $(CXXFLAGS) -DMTYPE=P -DMODEL='"mod1-lib.cc.h"' -DMNAME='"P"'

tdriver1_SOURCES = \
	tdriver.C

tdriver1_CXXFLAGS = $(CXXFLAGS) -DMTYPE=P -DMODEL='"mod18.cc.h"' -DMNAME='"P"' -DEnableL1 -DEnableL2

tdriver2_SOURCES = \
	tdriver.C

tdriver2_CXXFLAGS = $(CXXFLAGS) -DMTYPE=P -DMODEL='"mod3.cc.h"' -DMNAME='"P"' -DPRE_CYCLE -DPOST_CYCLE -DBLOCKINGMEM

tdriver3_SOURCES = \
	tdriver.C

tdriver3_CXXFLAGS = $(CXXFLAGS) -DMTYPE=P -DMODEL='"mod2.cc.h"' -DMNAME='"P"'

tdriver4_SOURCES = \
	tdriver.C

tdriver4_CXXFLAGS = $(CXXFLAGS) -DMTYPE=P -DMODEL='"mod11.cc.h"' -DMNAME='"P"'

tdriver5_SOURCES = \
	tdriver.C

tdriver5_CXXFLAGS = $(CXXFLAGS) -DMTYPE=P -DMODEL='"mod49.cc.h"' -DMNAME='"P"' 

tdriver6_SOURCES = \
	tdriver.C

tdriver6_CXXFLAGS = $(CXXFLAGS) -DMTYPE=P -DMODEL='"mod6.cc.h"' -DMNAME='"P"' -DBLOCKINGMEM

tdriver7_SOURCES = \
	tdriver.C

tdriver7_CXXFLAGS = $(CXXFLAGS) -DMTYPE=P -DMODEL='"mod10.cc.h"' -DMNAME='"P"' -DBLOCKINGMEM

tdriver8_SOURCES = \
	tdriver.C

tdriver8_CXXFLAGS = $(CXXFLAGS) -DMTYPE=P -DMODEL='"mod69.cc.h"' -DMNAME='"P"' -DBLOCKINGMEM

mod1split8.cc.h : mod1.adl $(MakeIss) ;
	$(MakeTdriver)  --config=$(srcdir)/mod1split8-trans.ttc

tdriver9_SOURCES = \
	tdriver.C

tdriver9_CXXFLAGS = $(CXXFLAGS) -DMTYPE=P -DMODEL='"mod1split8.cc.h"' -DMNAME='"P"' -DSPLIT_SIZE=8


count_instrs_so_SOURCES = \
	CountInstrs.C

count_instrs_so_CXXFLAGS = -fPIC -I$(top_srcdir) $(PlasmaFlags)

count_instrs_so_LDFLAGS = -shared

count_instrs_so_LDADD = 


count_branches_so_SOURCES = \
	CountBranches.C

count_branches_so_CXXFLAGS = -fPIC -I$(top_srcdir) $(PlasmaFlags)

count_branches_so_LDFLAGS = -shared

count_branches_so_LDADD = 


branch_callback_so_SOURCES = \
	BranchCallback.C

branch_callback_so_CXXFLAGS = -fPIC -I$(top_srcdir) $(PlasmaFlags)

branch_callback_so_LDFLAGS = -shared

branch_callback_so_LDADD = 

exception_callback_so_SOURCES = \
	ExceptionCallback.C

exception_callback_so_CXXFLAGS = -fPIC -I$(top_srcdir) $(PlasmaFlags) --std=c++0x

exception_callback_so_LDFLAGS = -shared

exception_callback_so_LDADD = 

pic_uart_so_SOURCES = \
	PicUart.C

pic_uart_so_CXXFLAGS = -fPIC -I$(top_srcdir) $(PlasmaFlags) --std=c++0x

pic_uart_so_LDFLAGS = -shared

pic_uart_so_LDADD =


ext_hrvd_so_SOURCES = \
	ExternHarvard.C

ext_hrvd_so_CXXFLAGS = -fPIC -I$(top_srcdir) $(PlasmaFlags) --std=c++0x

ext_hrvd_so_LDFLAGS = -shared

ext_hrvd_so_LDADD = 


smdriver1_SOURCES = \
	smdriver.C

smdriver1_CXXFLAGS = $(CXXFLAGS) -DMTYPE=P -DMODEL='"mod18sm.cc.h"' -DMNAME='"P"'

smdriver2_SOURCES = \
	smdriver.C

smdriver2_CXXFLAGS = $(CXXFLAGS) -DMTYPE=P -DMODEL='"mod3sm.cc.h"' -DMNAME='"P"' -DLogOrigMem -O2

smdriver3_SOURCES = \
	smdriver.C

smdriver3_CXXFLAGS = $(CXXFLAGS) -DMTYPE=P -DMODEL='"mod11sm.cc.h"' -DMNAME='"P"'

smdriver4_SOURCES = \
	smdriver.C

smdriver4_CXXFLAGS = $(CXXFLAGS) -DMTYPE=P -DMODEL='"mod11sm-h.cc.h"' -DMNAME='"P"' -DHybrid

smdriver5_SOURCES = \
	smdriver.C

smdriver5_CXXFLAGS = $(CXXFLAGS) -DMTYPE=P -DMODEL='"mod3sm-h.cc.h"' -DMNAME='"P"' -DHybrid -DRestoreStart=15 -DRestoreStop=15 -DLogInstrFields

smdriver6_SOURCES = \
	smdriver.C

smdriver6_CXXFLAGS = $(CXXFLAGS) -DMTYPE=P -DMODEL='"mod48sm-h.cc.h"' -DMNAME='"P"' -DHybrid -DRestoreStart=14 -DRestoreStop=14 -DParallelExecution

smdriver7_SOURCES = \
	smdriver.C

smdriver7_CXXFLAGS = $(CXXFLAGS) -DMTYPE=P -DMODEL='"mod67sm-h.cc.h"' -DMNAME='"P"' -DHybrid -DParallelExecution -DLogInstrFields

mod1_SOURCES = \
	mod1.adl

mod1.cc: mod1.adl $(MakeIss)
$(BCDIR)/mod1.bc: mod1.cc

mod1_ADLFLAGS = --disassembler --log-instr-args --no-line-directives $(ADL_JIT) --decode-cache --dmi-cache --no-per-instr-td-incr

if LLVM
mod1_DEPENDENCIES = $(BCDIR)/mod1.bc $(JIT_LIBS) $(mod1_simple_DEPENDENCIES)
endif

mod1_CXXFLAGS = -DJIT_DEBUG

mod1_LDFLAGS = $(DYNEXPORT)


mod1_simple_SOURCES = mod1-simple.cc

mod1_simple_DEPENDENCIES = $(Iss)/libAdlIssExec.la $(Iss)/libAdlIssMain.la	\
							$(Iss)/libAdlIssLogging.la $(Iss)/libAdlReaders.la						\
							$(Iss)/libAdlParsing.la $(Iss)/libAdlIssSupport.la						\
							$(Iss)/libAdlScript.la $(Helpers)/libIntbv.la									\
							$(Helpers)/libIntbvRnum.la $(Helpers)/libAdlSupport.la				\
							$(Helpers)/libAdlLogUsage.la

mod1_simple_ADLFLAGS = --disassembler --log-instr-args --no-line-directives --dmi-cache --no-log-usage

mod2_SOURCES = \
	mod2.adl

mod2.cc: mod2.adl $(MakeIss)

mod3_SOURCES = \
	mod3.cc

mod3.cc: mod3.adl $(MakeIss) $(srcdir)/mod3.ttc
$(BCDIR)/mod3.bc: mod3.cc

mod3_ADLFLAGS = --no-line-directives $(ADL_JIT)  --decode-cache --config=$(srcdir)/mod3.ttc --rollback-mode --dmi-cache --no-td-api -D=ClkRatio=1.2

if LLVM
mod3_DEPENDENCIES = $(BCDIR)/mod3.bc  $(JIT_LIBS) $(mod1_simple_DEPENDENCIES)
endif

mod3_LDFLAGS = $(DYNEXPORT)

mod3rp_SOURCES = \
	mod3rp.cc

mod3rp.cc: mod3.adl $(MakeIss) $(srcdir)/mod3.ttc

mod4_SOURCES = \
	mod4.adl

mod4.cc: mod4.adl $(MakeIss)

mod5_SOURCES = \
	mod5.adl

mod5.cc: mod5.adl $(MakeIss)

mod5: mod5.cc
	@rm -f $@ $@.o
	$(CXXCOMPILE) -c -o $@.o $< -DUseBlockMemory
	$(CXXLINK) $(LDFLAGS) $@.o $(LDADD) $(LIBS)

mod6_SOURCES = \
	mod6.adl

mod6.cc: mod6.adl $(MakeIss)
$(BCDIR)/mod6.bc: mod6.cc

mod6_ADLFLAGS = --no-line-directives $(ADL_JIT)  --decode-cache --dmi-cache --no-per-instr-td-incr

if LLVM
mod6_DEPENDENCIES = $(BCDIR)/mod6.bc $(JIT_LIBS) $(mod1_simple_DEPENDENCIES)
endif

mod6_LDFLAGS = $(DYNEXPORT)

mod7_SOURCES = \
	mod7.adl

mod7.cc: mod7.adl $(MakeIss)

mod8_SOURCES = \
	mod8.adl

mod8.cc: mod8.adl $(MakeIss)

mod9_SOURCES = \
	mod9.adl

mod9.cc: mod9.adl mod1.adl $(MakeIss)

mod10_SOURCES = \
	mod10.adl

mod10_ADLFLAGS = --log-reg-reads --log-reg-masks --decode-cache $(ADL_JIT)

mod10.cc: mod10.adl $(MakeIss)
$(BCDIR)/mod10.bc: mod10.cc

if LLVM
mod10_DEPENDENCIES = $(BCDIR)/mod10.bc $(JIT_LIBS) $(mod1_simple_DEPENDENCIES)
endif

mod10_LDFLAGS = $(DYNEXPORT)

mod11_SOURCES = \
	mod11.adl

mod11.cc: mod11.adl mod1.adl $(MakeIss)
$(BCDIR)/mod11.bc: mod11.cc

mod11_ADLFLAGS = --no-line-directives $(ADL_JIT)  --decode-cache --dmi-cache --no-log-unarch --per-instr-td-incr -log-td-commit

if LLVM
mod11_DEPENDENCIES = $(BCDIR)/mod11.bc $(JIT_LIBS) $(mod1_simple_DEPENDENCIES)
endif

mod11_CXXFLAGS = -DJIT_DEBUG

mod11_LDFLAGS = $(DYNEXPORT)

mod12_SOURCES = \
	mod12.adl

mod12.cc: mod12.adl $(MakeIss)

mod12_ADLFLAGS = --log-reg-masks

mod13_SOURCES = \
	mod13.adl

mod13.cc: mod13.adl mod1.adl $(MakeIss)
$(BCDIR)/mod13.bc: mod13.cc

mod13_ADLFLAGS = --config=$(srcdir)/mod13.ttc --no-line-directives $(ADL_JIT)  --decode-cache --dmi-cache --no-log-unarch --no-per-instr-td-incr

if LLVM
mod13_DEPENDENCIES = $(BCDIR)/mod13.bc $(JIT_LIBS) $(mod1_simple_DEPENDENCIES)
endif

mod13_CXXFLAGS = -DJIT_DEBUG

mod13_LDFLAGS = $(DYNEXPORT)

mod14_SOURCES = \
	mod14.adl

mod14.cc: mod14.adl mod1.adl $(MakeIss)

mod15_SOURCES = \
	mod15.adl

mod15.cc: mod15.adl $(MakeIss)

mod16_SOURCES = \
	mod16.adl

mod16.cc: mod16.adl $(MakeIss)

mod17_SOURCES = \
	mod17.adl

mod17.cc: mod17.adl $(MakeIss)

mod18_SOURCES = \
	mod18.adl

mod18.cc: mod18.adl mod1.adl $(MakeIss) $(srcdir)/mod18.ttc
$(BCDIR)/mod18.bc: mod18.cc

mod18_ADLFLAGS = --no-log-writeregs --no-line-directives $(ADL_JIT)  --decode-cache --dmi-cache --config=$(srcdir)/mod18.ttc  --no-per-instr-td-incr

if LLVM
mod18_DEPENDENCIES = $(BCDIR)/mod18.bc  $(JIT_LIBS) $(mod1_simple_DEPENDENCIES)
endif

mod18_LDFLAGS = $(DYNEXPORT)

mod19_SOURCES = \
	mod19.adl

mod19.cc: mod19.adl mod1.adl $(MakeIss)

mod20_SOURCES = \
	mod20.adl

mod20.cc: mod20.adl mod1.adl $(MakeIss)

mod21_SOURCES = \
	mod21.adl

mod21.cc: mod21.adl mod1.adl $(MakeIss) $(srcdir)/mod21.ttc

mod21_ADLFLAGS = --no-syscall-enabled --config=$(srcdir)/mod21.ttc

mod22_SOURCES = \
	mod22.adl

mod22.cc: mod22.adl mod1.adl $(MakeIss)

mod23_SOURCES = \
	mod23.adl

mod23.cc: mod23.adl mod1.adl $(MakeIss) $(srcdir)/mod23.ttc

mod23_ADLFLAGS = --config=$(srcdir)/mod23.ttc --extern-dmi --tags-only-caches --decode-cache

mod25_SOURCES = \
	mod25.adl

mod25.cc: mod25.adl mod1.adl $(MakeIss)

mod26_SOURCES = \
	mod26.adl

mod26.cc: mod26.adl mod1.adl $(MakeIss)

mod27_SOURCES = \
	mod27.adl

mod27.cc: mod27.adl mod8.adl mod1.adl $(MakeIss)

mod28_SOURCES = \
	mod28.adl

mod28.cc: mod28.adl mod1.adl $(MakeIss)

mod29_SOURCES = \
	mod29.adl

mod29.cc: mod29.adl mod1.adl $(MakeIss)

mod30_SOURCES = \
	mod30.adl

mod37_SOURCES = \
	mod37.adl

mod37.cc: mod37.adl $(MakeIss)

mod38_SOURCES = \
	mod38.adl

mod38.cc: mod38.adl $(MakeIss)

mod39_SOURCES = \
	mod39.adl

mod39.cc: mod39.adl $(MakeIss)

mod40_SOURCES = \
	mod40.adl

mod40.cc: mod40.adl $(MakeIss)

mod41_SOURCES = \
	mod41.adl

mod41.cc: mod41.adl $(MakeIss)


mod42_SOURCES = \
	mod42.adl

mod42.cc: mod42.adl $(MakeIss)


mod43_SOURCES = \
	mod43.adl

mod43.cc: mod43.adl $(MakeIss)


mod30.cc: mod30.adl mod1.adl $(MakeIss)

mod31_SOURCES = \
	mod31.adl

mod31.cc: mod31.adl mod1.adl $(MakeIss)

mod32_SOURCES = \
	mod32.adl

mod32.cc: mod32.adl $(MakeIss)

mod34_SOURCES = \
	mod34.adl

mod34.cc: mod34.adl $(MakeIss) 


mod36_SOURCES = \
	mod36.adl

mod36.cc: mod36.adl $(MakeIss)

mod46_SOURCES = \
	mod46.adl

mod46.cc: mod46.adl $(MakeIss)

mod47_SOURCES = \
	mod47.adl

mod47.cc: mod47.adl $(MakeIss)

mod48_SOURCES = \
	mod48.adl

mod48.cc: mod48.adl $(MakeIss)

mod48_ADLFLAGS = --rollback-mode --ignore-mode


mod50_SOURCES = \
	mod50.adl

mod50.cc: mod50.adl $(MakeIss)

mod51_SOURCES = \
	mod51.adl

mod51.cc: mod51.adl $(MakeIss)

mod52_SOURCES = \
	mod52.adl

mod52.cc: mod52.adl $(MakeIss)

mod56_SOURCES = \
	mod56.adl

mod56.cc: mod56.adl $(MakeIss)

mod57_SOURCES = \
	mod57.adl

mod57.cc: mod57.adl $(MakeIss)


mod61_SOURCES = \
	mod61.adl

mod61.cc: mod61.adl $(MakeIss)

mod63_SOURCES = \
	mod63.adl

mod63.cc: mod63.adl $(MakeIss)

mod66_SOURCES = \
	mod66.adl

mod66.cc: mod66.adl $(MakeIss)

mod67_SOURCES = \
	mod67.adl

mod67.cc: mod67.adl $(MakeIss)

mod69_SOURCES = \
	mod69.adl

mod69.cc: mod69.adl $(MakeIss)

mod70_SOURCES = \
	mod70.adl

mod70_ADLFLAGS = --dep-tracking --log-reg-reads

mod70.cc: mod70.adl $(MakeIss)

mod72_SOURCES = \
	mod72.adl

mod72.cc: mod72.adl $(MakeIss) $(srcdir)/mod72.ttc
$(BCDIR)/mod72.bc: mod72.cc

mod72_ADLFLAGS = $(ADL_JIT)  --decode-cache --config=$(srcdir)/mod72.ttc  --no-per-instr-td-incr

if LLVM
mod72_DEPENDENCIES = $(BCDIR)/mod72.bc $(JIT_LIBS) $(mod1_simple_DEPENDENCIES)
endif

mod72_LDFLAGS = $(DYNEXPORT)

mod73_SOURCES = \
	mod73.adl

mod73.cc: mod73.adl $(MakeIss)

mod75_SOURCES = \
	mod75.adl

mod75.cc: mod75.adl $(MakeIss)

mod76_SOURCES = \
	mod76.adl

mod76.cc: mod76.adl $(MakeIss)

mod77_SOURCES = \
	mod77.adl

mod77.cc: mod77.adl $(MakeIss)

mod78_SOURCES = \
	mod78.adl

mod78.cc: mod78.adl $(MakeIss)
$(BCDIR)/mod78.bc: mod78.cc

mod78_ADLFLAGS = $(ADL_JIT)  --decode-cache --config=$(srcdir)/mod78.ttc  --no-per-instr-td-incr

if LLVM
mod78_DEPENDENCIES = $(BCDIR)/mod78.bc $(JIT_LIBS) $(srcdir)/mod78.ttc $(mod1_simple_DEPENDENCIES)
endif

mod78_LDFLAGS = $(DYNEXPORT)

mod79_SOURCES = \
	mod79.adl

mod79.cc: mod79.adl $(MakeIss)
$(BCDIR)/mod79.bc: mod79.cc

mod79_ADLFLAGS = $(ADL_JIT)  --decode-cache  --no-per-instr-td-incr --config=$(srcdir)/mod79.ttc

if LLVM
mod79_DEPENDENCIES = $(BCDIR)/mod79.bc $(JIT_LIBS) $(mod1_simple_DEPENDENCIES)
endif

mod79_LDFLAGS = $(DYNEXPORT)

mod82_SOURCES = \
	mod82.adl

mod82.cc: mod82.adl $(MakeIss)

mod83_SOURCES = \
	mod83.adl

mod83.cc: mod83.adl $(MakeIss)

mod84_SOURCES = \
	mod84.adl

mod84.cc: mod84.adl $(MakeIss)

mod84_ADLFLAGS = --no-line-directives --decode-cache --dmi-cache --no-per-instr-td-incr --mt-support --config=$(srcdir)/mod84.ttc

mod84_LDFLAGS = $(DYNEXPORT)


mod85_SOURCES = \
	mod85.adl

mod85.cc: mod84.adl mod85.adl $(MakeIss)

mod85_ADLFLAGS = --no-line-directives --decode-cache --dmi-cache --no-per-instr-td-incr --mt-support --extern-mem --extern-dmi --extern-dmi-entry-size=10

mod85_LDFLAGS = $(DYNEXPORT)


# Same as mod85, but without the external DMI, in order to make sure that this
# kind of model works.
mod86_SOURCES = \
	mod86.adl

mod86.cc: mod84.adl mod85.adl mod86.adl $(MakeIss)

mod86_ADLFLAGS = --no-line-directives --decode-cache --dmi-cache --no-per-instr-td-incr --mt-support --extern-mem 

mod86_LDFLAGS = $(DYNEXPORT)

mod88_SOURCES = \
	mod88.adl

mod88.cc: mod88.adl $(MakeIss)


mod89_SOURCES = \
	mod89.adl

mod89_ADLFLAGS = --decode-cache --dmi-cache --no-per-instr-td-incr --tags-only-caches --time-tagged --config=$(srcdir)/mod89.ttc

mod89.cc: mod89.adl $(MakeIss)  $(srcdir)/mod89.ttc


mod90_SOURCES = \
	mod90.adl

mod90_ADLFLAGS = --decode-cache --dmi-cache --tags-only-caches --log-td-commit --config=$(srcdir)/mod89.ttc

mod90.cc: mod90.adl mod89.adl $(MakeIss)  $(srcdir)/mod89.ttc


mod92_SOURCES = \
	mod92.adl

mod92_ADLFLAGS = --decode-cache --dmi-cache

mod92.cc: mod92.adl $(MakeIss)


mod93_SOURCES = \
	mod93.adl

mod93_ADLFLAGS = --decode-cache --extern-dmi --extern-mem

mod93.cc: mod93.adl $(MakeIss)

mod93_LDFLAGS = $(DYNEXPORT)


mod94_SOURCES = \
	mod94.adl

mod94_ADLFLAGS = --decode-cache --no-warn-redefine --config=$(srcdir)/mod94.ttc

mod94.cc: mod94.adl $(MakeIss) $(srcdir)/mod94.ttc

mod94_LDFLAGS = $(DYNEXPORT)


mod95_SOURCES = \
	mod95.adl

mod95.cc: mod95.adl mod1.adl $(MakeIss)

mod95_ADLFLAGS = --extern-dmi --decode-cache


mod98_SOURCES = \
	mod98.adl

mod98.cc: mod98.adl mod1.adl $(MakeIss)

mod98_ADLFLAGS = --extern-dmi --decode-cache


mod99_SOURCES = \
	mod99.adl

mod99.cc: mod99.adl $(MakeIss)

mod99_ADLFLAGS = --disassembler --decode-cache


mod100_SOURCES = \
	mod100.adl

mod100.cc: mod100.adl $(MakeIss)

mod100_ADLFLAGS = --disassembler --decode-cache


mod101_SOURCES = \
	mod101.adl

mod101.cc: mod101.adl $(MakeIss)

mod101_ADLFLAGS = --disassembler --decode-cache

libtcmod101_a_SOURCES = tc-mod101.C

libtcmod101_a_CXXFLAGS = $(CXXFLAGS) -I$(top_srcdir)/external/binutils-adl -I$(top_srcdir)/external/binutils-adl/gas -I$(top_builddir)/external/binutils-adl/gas -I$(top_srcdir)/external/binutils-adl/gas/config -I$(top_srcdir)/external/binutils-adl/include -I$(top_srcdir)/external/binutils-adl/bfd -I$(top_builddir)/external/binutils-adl/bfd  

mod102_SOURCES = \
	mod102.adl

mod102.cc: mod102.adl $(MakeIss)

mod102_ADLFLAGS = --decode-cache


mod103_SOURCES = \
	mod103.adl

mod103.cc: mod103.adl mod102.adl $(MakeIss)

mod103_ADLFLAGS = --decode-cache --config=$(srcdir)/mod103.ttc

mod103_LDFLAGS = $(DYNEXPORT)


mod106_SOURCES = \
	mod106.adl

mod106.cc: mod106.adl $(MakeIss)


mod107_SOURCES = \
	mod107.adl

mod107.cc: mod107.adl mod101.adl $(MakeIss)

mod107_ADLFLAGS = --decode-cache --config=$(srcdir)/mod107.ttc

mod107_LDFLAGS = $(DYNEXPORT)



all-time-tagged:  ttmod1 ttmod2 ttmod3 ttmod4 ttmod5

ttmod1_SOURCES = \
	ttmod1.cc

ttmod1.cc: mod1.adl $(MakeIss) $(srcdir)/cfg1.ttc
	$(ADL) --decode-cache --time-tagged --config=$(srcdir)/cfg1.ttc

ttmod2_SOURCES = \
	ttmod2.adl

ttmod2.cc: ttmod2.adl mod1.adl $(MakeIss) $(srcdir)/cfg1.ttc
	$(ADL) --time-tagged --config=$(srcdir)/cfg1.ttc

ttmod3_SOURCES = \
	ttmod3.cc

ttmod3.cc: mod1.adl $(MakeIss) $(srcdir)/cfg5.ttc
	$(ADL) --time-tagged --config=$(srcdir)/cfg5.ttc

microinstr6_SOURCES = \
	microinstr6.adl

microinstr6.cc: microinstr6.adl $(MakeIss)

microinstr7_SOURCES = \
	microinstr7.adl

microinstr7.cc: microinstr7.adl $(MakeIss)

microinstr9_SOURCES = \
	microinstr9.adl

microinstr9.cc: microinstr9.adl $(MakeIss)

microinstr10_SOURCES = \
	microinstr10.adl

microinstr10.cc: microinstr10.adl $(MakeIss)

microinstr12_SOURCES = \
	microinstr12.adl

microinstr12.cc: microinstr12.adl $(MakeIss)

microinstr13_SOURCES = \
	microinstr13.adl

microinstr13.cc: microinstr13.adl $(MakeIss)

microinstr14_SOURCES = \
	microinstr14.adl

microinstr14.cc: microinstr14.adl $(MakeIss)

simplified_arm_SOURCES = \
	simplified_arm.adl

simplified_arm.cc: simplified_arm.adl $(MakeIss)

simplified_armv8_SOURCES = \
	simplified_armv8.adl

simplified_armv8.cc: simplified_armv8.adl $(MakeIss)

include ./$(DEPDIR)/mod1.Po
include ./$(DEPDIR)/mod2.Po
include ./$(DEPDIR)/mod3.Po
include ./$(DEPDIR)/mod4.Po
include ./$(DEPDIR)/mod5.Po
include ./$(DEPDIR)/mod6.Po
include ./$(DEPDIR)/mod7.Po
include ./$(DEPDIR)/mod8.Po
include ./$(DEPDIR)/mod9.Po
include ./$(DEPDIR)/mod10.Po
include ./$(DEPDIR)/mod11.Po
include ./$(DEPDIR)/mod12.Po
include ./$(DEPDIR)/mod13.Po
include ./$(DEPDIR)/mod14.Po
include ./$(DEPDIR)/mod15.Po
include ./$(DEPDIR)/mod16.Po
include ./$(DEPDIR)/mod17.Po
include ./$(DEPDIR)/mod18.Po
include ./$(DEPDIR)/mod19.Po
include ./$(DEPDIR)/mod20.Po
include ./$(DEPDIR)/mod21.Po
include ./$(DEPDIR)/mod22.Po
include ./$(DEPDIR)/mod23.Po
include ./$(DEPDIR)/mod25.Po
include ./$(DEPDIR)/mod26.Po
include ./$(DEPDIR)/mod27.Po
include ./$(DEPDIR)/mod28.Po
include ./$(DEPDIR)/mod29.Po
include ./$(DEPDIR)/mod30.Po
include ./$(DEPDIR)/mod31.Po
include ./$(DEPDIR)/mod32.Po
include ./$(DEPDIR)/ttmod1.Po
include ./$(DEPDIR)/ttmod2.Po
include ./$(DEPDIR)/ttmod3.Po
include ./$(DEPDIR)/ttmod4.Po
include ./$(DEPDIR)/ttmod5.Po
include ./$(DEPDIR)/mod37.Po
include ./$(DEPDIR)/mod38.Po
include ./$(DEPDIR)/mod39.Po
include ./$(DEPDIR)/mod40.Po
include ./$(DEPDIR)/mod41.Po
include ./$(DEPDIR)/mod42.Po
include ./$(DEPDIR)/mod43.Po
include ./$(DEPDIR)/mod48.Po
include ./$(DEPDIR)/mod50.Po
include ./$(DEPDIR)/mod51.Po
include ./$(DEPDIR)/mod52.Po


data_DATA = \
	tdriver.C \
	mod1.adl

EXTRA_DIST = \
	$(srcdir)/mod1-simple.cc \
	$(srcdir)/mod1.adl \
	$(srcdir)/mod3.adl \
	$(srcdir)/mod1-dis.adl \
	$(srcdir)/mod33.adl \
	$(srcdir)/mod35.adl \
	$(srcdir)/mod43.adl \
	$(srcdir)/mod44.adl \
	$(srcdir)/mod45.adl \
	$(srcdir)/mod46.adl \
	$(srcdir)/mod47.adl \
	$(srcdir)/mod49.adl \
	$(srcdir)/mod53.adl \
	$(srcdir)/mod54.adl \
	$(srcdir)/mod55.adl \
	$(srcdir)/mod58.adl \
	$(srcdir)/mod59.adl \
	$(srcdir)/mod60.adl \
	$(srcdir)/mod62.adl \
	$(srcdir)/mod64.adl \
	$(srcdir)/mod65.adl \
	$(srcdir)/mod68.adl \
	$(srcdir)/mod71.adl \
	$(srcdir)/mod74.adl \
	$(srcdir)/mod80.adl \
	$(srcdir)/mod81.adl \
	$(srcdir)/mod87.adl \
	$(srcdir)/mod91.adl \
	$(srcdir)/mod96.adl \
	$(srcdir)/mod97.adl \
	$(srcdir)/mod104.adl \
	$(srcdir)/mod105.adl \
	$(srcdir)/mod108.adl \
	$(srcdir)/mod109.adl \
	$(srcdir)/mod110.adl \
	$(srcdir)/microinstr*.adl \
	$(srcdir)/microinstr*.s \
	$(srcdir)/mod-alias*.adl \
	$(srcdir)/mod-alias*.s \
	$(srcdir)/regress \
	$(srcdir)/coro_regress \
	$(srcdir)/*.pl \
	$(filter-out %.gen.dat,$(filter-out %.out.dat,$(wildcard $(srcdir)/*.dat))) \
	$(filter-out %.out.uvp,$(wildcard $(srcdir)/*.uvp)) \
	$(srcdir)/*.regress \
	$(srcdir)/*.tarmac \
	$(srcdir)/asm*.s \
	$(srcdir)/asm*.hex \
	$(srcdir)/*.expects \
	$(srcdir)/*.ttc \
	$(srcdir)/*.gdb \
	$(srcdir)/*.elf \
	$(srcdir)/*.tcl \
	$(srcdir)/*.cli \
	$(srcdir)/stat.adl \
	$(srcdir)/mod73-extres.h \
	$(srcdir)/asm86.lnk \
	$(srcdir)/mod102.lnk

ADL = $(AM_V_GEN) $(MakeIss) $(ADLFLAGS) $< -o $@ 

PlasmaFlags = $(PLASMA_CFLAGS)
PlasmaLibs  = $(PLASMA_LTLIBS)
PlasmaPfx   = $(PLASMA_PATH)/lib

Helpers = $(top_builddir)/helpers
Iss     = $(top_builddir)/iss
Asm     = $(top_builddir)/asm

memtest_CPPFLAGS = $(PlasmaFlags) -I$(top_srcdir)

corotest_CPPFLAGS = -I$(top_srcdir)

memtest_libraries =  \
	$(Iss)/libAdlIssSupport.la \
	$(Helpers)/libIntbv.la \
	$(Helpers)/libIntbvRnum.la \
	$(Helpers)/libAdlSupport.la \
	-L$(PlasmaPfx) -lgc

corotest_LDADD = $(BOOST_LDFLAGS) -lboost_context

queuetest_LDADD = 

OccFlags = $(PLASMA_CFLAGS)
OccLibs = $(PLASMA_PATH)/lib

ADL_INCLUDES = $(OccFlags) -I$(top_srcdir) $(RNUMBER_CFLAGS)

AM_CPPFLAGS = $(ADL_INCLUDES)

AM_CXXFLAGS = $(ADL_INCLUDES)

CLANG = $(LLVM_BINDIR)/clang++

EXTRACT = $(Iss)/adl-llvm-extract

BC_INCLUDES = -I$(top_srcdir) -I$(top_builddir) $(RNUMBER_CFLAGS)

if LLVM

JIT_LIBS = $(Iss)/libAdlIssJitSupport.la
#JIT_LDADD = $(JIT_LIBS) -lboost_thread -lboost_system -lboost_regex $(LLVM_LDFLAGS) $(LLVM_LIBS) $(BOOST_LDFLAGS)
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
				$(RNUMBER_LIBS) \
				$(LZ4_LTLIBS) \
				$(BOOST_LDFLAGS) -lboost_thread -lboost_system -lboost_regex -lboost_atomic -lboost_iostreams -lrt  \
        $(PlasmaPfx)/libltdl.la \
        $(PlasmaPfx)/libgc.la \
				-ldl -lpthread

base_transactional_iss_libraries = \
        $(Iss)/libAdlIssMain.la \
        $(Iss)/libAdlIssExec.la \
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
				$(BOOST_LDFLAGS) -lboost_thread -lboost_system -lboost_regex -lboost_atomic -lboost_context -lboost_iostreams -lrt \
        $(PlasmaPfx)/libltdl.la \
        $(PlasmaPfx)/libgc.la \
				-ldl


transactional_iss_libraries = \
			  $(base_transactional_iss_libraries) \
        $(Iss)/libAdlIssTransLogging.la

tdriver1_LDADD = $(transactional_iss_libraries)

tdriver2_LDADD = $(transactional_iss_libraries)

tdriver3_LDADD = $(transactional_iss_libraries)

tdriver4_LDADD = $(transactional_iss_libraries)

tdriver5_LDADD = $(transactional_iss_libraries)

tdriver7_LDADD = $(transactional_iss_libraries)

tdriver9_LDADD = $(transactional_iss_libraries)

smdriver1_LDADD = $(transactional_iss_libraries)

smdriver2_LDADD = $(transactional_iss_libraries)

smdriver3_LDADD = $(transactional_iss_libraries)

smdriver4_LDADD = $(transactional_iss_libraries)

smdriver5_LDADD = $(transactional_iss_libraries)

smdriver6_LDADD = $(transactional_iss_libraries)

LINK = $(LIBTOOL) --mode=link $(CXX) -o $@ 

SCRIPT   = $(top_builddir)/scripts
BINBUILD = $(top_builddir)/external/binutils-adl
BINSRC   = $(top_srcdir)/external/binutils-adl

ASMFLAGS =

ADLFLAGS =  --no-line-directives --no-warn-redefine

.PHONY: binutils-tests all-asm memtest memtest-tlm2

# For convenience, list all models here that also have assemblers, so that we
# can easily rebuild all assembler related stuff with a single command.
all-asm: binutils-tests mod1 mod8 mod61 mod63 mod67

# Run check with arguments to run the regressions in parallel.
pcheck:
	$(MAKE) check RegressOpts="-p=8"

check-programs: all-am
	$(MAKE) $(AM_MAKEFLAGS) $(check_PROGRAMS) 

# List all assemblers here.
binutils-tests: \
	as-mod-alias1 \
	as-mod-alias2 \
	as-microinstr8 \
	as-microinstr9 \
	as-microinstr10 \
	as-microinstr12 \
	as-microinstr13 \
	as-microinstr14 \
	as-microinstr15 \
	as-microinstr16 \
	as-microinstr17 \
	as-mod1 \
	as-mod8 \
	as-mod33 \
	as-mod35 \
	as-mod43 \
	as-mod44 \
	as-mod45 \
	as-mod46 \
	as-mod47 \
	as-mod53 \
	as-mod54 \
	as-mod55 \
	as-mod58 \
	as-mod59 \
	as-mod60 \
	as-mod61 \
	as-mod62 \
	as-mod63 \
	as-mod64 \
	as-mod65 \
	as-mod67 \
	as-mod68 \
	as-mod71 \
	as-mod74 \
	as-mod75 \
	as-mod76 \
	as-mod77 \
	as-mod80 \
	as-mod81 \
	as-mod87 \
	as-mod91 \
	as-mod96 \
	as-mod97 \
	as-mod99 \
	as-mod100 \
	as-mod101 \
	as-mod104 \
	as-mod105 \
	as-mod106 \
	as-mod107 \
	as-mod108 \
	as-mod109 \
	as-mod110

# These entries are needed b/c otherwise, the dist target thinks that there's a
# dependency on binutils-modN.c.  Personally, this seems like a bug in automake,
# but this hack seems to get rid of the problem.
binutils_tests_SOURCES = 

# Note: We're using powerpc and powerpc:common just for compatibility with older
# regressions, where those names were used by default.
ASMCOMPILE = $(AM_V_GEN) $(SCRIPT)/adl2asm $(ASMFLAGS) --prefix=$(top_builddir) --arch-name="powerpc" --arch-print-name="powerpc:common" $< \
                --bprefix=$(BINBUILD) --bsprefix=$(BINSRC)  --no-warn-redefine --no-log-usage --no-a2l --no-ld

# We override the default to add on per-compilation flags using the ADLCXXFLAGS
# suffix.  This allows us to do per-file compilation flags for ADL files without
# confusing automake.
CXXCOMPILE = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) \
	$($(patsubst %.o,%_ADLCXXFLAGS,$(@)))

SIMPLELINK = $(AM_V_CXXLD) $(CXXLD) $(AM_CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@

ADL2ISS = $(top_builddir)/scripts/adl2iss --prefix=$(top_builddir) $< --output=$@  --no-warn-redefine

mod1-simple: $(mod1_simple_SOURCES) $(mod1_simple_DEPENDENCIES)
	$(ADL2ISS) --no-log-usage

# We just need this because we use a different main source file from what we
# want as a name.
as-mod1: mod1-dis.adl mod1.adl $(Asm)/make-asm
	$(ASMCOMPILE) --ld --model-base=mod1 --dbg

as-mod44: mod44.adl $(Asm)/make-asm
	$(ASMCOMPILE)

as-mod61: mod61.adl $(Asm)/make-asm
	$(ASMCOMPILE) --big-mem

as-mod67: mod67.adl $(Asm)/make-asm
	$(ASMCOMPILE) --big-mem

as-mod97: mod97.adl $(Asm)/make-asm
	$(ASMCOMPILE) --ld

as-mod99: mod99.adl $(Asm)/make-asm
	$(ASMCOMPILE) --ld  --elf-machine-code=243

as-mod101: mod101.adl libtcmod101.a $(Asm)/make-asm 
	$(ASMCOMPILE) --ld  --elf-machine-code=243 --as-libs="-L$(top_builddir)/tests/iss -ltcmod101"

as-mod104: mod104.adl $(Asm)/make-asm 
	$(ASMCOMPILE) --ld  --elf-machine-code=243 

as-mod105: mod105.adl $(Asm)/make-asm 
	$(ASMCOMPILE) --ld  --elf-machine-code=243 

as-mod106: mod106.adl $(Asm)/make-asm 
	$(ASMCOMPILE)

as-mod107: mod107.adl mod101.adl $(Asm)/make-asm 
	$(ASMCOMPILE) --ld  --elf-machine-code=243 

as-mod108: mod108.adl $(Asm)/make-asm 
	$(ASMCOMPILE)

as-mod109: mod109.adl $(Asm)/make-asm 
	$(ASMCOMPILE)

as-mod110: mod110.adl $(Asm)/make-asm 
	$(ASMCOMPILE) --keep-ws

# Not used directly, but this allows you to build the disassembler from the command-line.
objdump-%: as-% ;

as-%: %.adl $(Asm)/make-asm
	$(ASMCOMPILE)

.adl.cc:
	$(ADL) $($(patsubst %.cc,%_ADLFLAGS,$(@)))

%rp.cc : %.adl ;
	$(ADL) --log-reg-reads --log-reg-masks $($(patsubst %.cc,%_ADLFLAGS,$(@)))

#.cc:
#	@rm -f $@ $@.o
#	$(CXXCOMPILE) -c -o $@.o $<
#	$(CXXLINK) $(LDFLAGS) $@.o $(LDADD) $(LIBS)

MakeTdriver = $(AM_V_GEN) $(MakeIss) --config=$(srcdir)/$*-trans.ttc --transactional-iss $< -o=$@ $(ADLFLAGS) -D=TRANSACTIONAL_ISS --no-warn-redefine

MakeHdriver = $(AM_V_GEN) $(MakeIss) --config=$(srcdir)/$*-trans.ttc --hybrid-iss $< -o=$@ $(ADLFLAGS) -D=HYBRID_ISS  --no-warn-redefine  --decode-arg-type="Parent &" --decode-arg=parent

MakeLibdriver = $(AM_V_GEN) $(MakeIss) --lib-mode $< -o=$@ $(ADLFLAGS)  --no-warn-redefine

MakeGenOnly = $(AM_V_GEN) $(MakeIss) $< -o=$(notdir $(@)) $(ADLFLAGS) $($(patsubst %-simple.cc,%_simple_ADLFLAGS,$(notdir $(@)))) --no-warn-redefine

%.cc.h : %.adl ;
	$(MakeTdriver)

%-lib.cc.h : %.adl ;
	$(MakeLibdriver)

%-simple.cc : %.adl ;
	$(MakeGenOnly)

%sm.cc.h : %.adl ;
	$(MakeTdriver) --logging-mem

%sm-h.cc.h : %.adl ;
	$(MakeHdriver) $($(patsubst %sm-h.cc.h,%_SMH_ADLFLAGS,$(@)))

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
	$(ADL2ISS) $(ADLFLAGS) --jit-compile-only $<

# Stupid thing about the new libtool- it refuses to create shared libraries if disable-shared
# is set.  So, I have to do it manually.
count-instrs.so: $(count_instrs_so_OBJECTS) $(count_instrs_so_DEPENDENCIES) 
	@rm -f count-instrs.so
	$(SIMPLELINK) $(count_instrs_so_LDFLAGS) $(count_instrs_so_OBJECTS) $(count_instrs_so_LDADD) $(LIBS)

count-branches.so: $(count_branches_so_OBJECTS) $(count_branches_so_DEPENDENCIES) 
	@rm -f count-branches.so
	$(SIMPLELINK) $(count_branches_so_LDFLAGS) $(count_branches_so_OBJECTS) $(count_branches_so_LDADD) $(LIBS)

branch-callback.so: $(branch_callback_so_OBJECTS) $(branch_callback_so_DEPENDENCIES) 
	@rm -f branch-callback.so
	$(SIMPLELINK) $(branch_callback_so_LDFLAGS) $(branch_callback_so_OBJECTS) $(branch_callback_so_LDADD) $(LIBS)

exception-callback.so: $(exception_callback_so_OBJECTS) $(exception_callback_so_DEPENDENCIES) 
	@rm -f exception-callback.so
	$(SIMPLELINK) $(exception_callback_so_LDFLAGS) $(exception_callback_so_OBJECTS) $(exception_callback_so_LDADD) $(LIBS)

pic-uart.so: $(pic_uart_so_OBJECTS) $(pic_uart_so_DEPENDENCIES) 
	@rm -f pic-uart.so
	$(SIMPLELINK) $(pic_uart_so_LDFLAGS) $(pic_uart_so_OBJECTS) $(pic_uart_so_LDADD) $(LIBS)

ext-hrvd.so: $(ext_hrvd_so_OBJECTS) $(ext_hrvd_so_DEPENDENCIES) 
	@rm -f ext-hrvd.so
	$(SIMPLELINK) $(ext_hrvd_so_LDFLAGS) $(ext_hrvd_so_OBJECTS) $(ext_hrvd_so_LDADD) $(LIBS)

## rules to set up local environment
pfx        = tests/iss
BASEDIR    =$(shell cd $(top_builddir) && pwd)
BASESRCDIR =$(shell cd $(top_srcdir) && pwd)

all-local: $(data_DATA)
	mkdir -p $(BASEDIR)/share
	cd $(BASEDIR)/share && \
	list='$(data_DATA)'; for file in $$list; do \
	  ln -fs $(BASESRCDIR)/$(pfx)/$$file; \
	done

PpcGccExists = type powerpc-eabi-gcc > /dev/null 2>&1

# For building a test file used to test ELF loading w/translation.  Not built by default.
in118.elf: in118.c
	if type powerpc-eabi-gcc >/dev/null 2>&1; then powerpc-eabi-gcc -o $@ $< -O3 -e main -Wl,-T$(basename $(notdir $<)).lnk; fi

in184.elf: in184.c
	if type powerpc-eabi-gcc >/dev/null 2>&1; then powerpc-eabi-gcc -o $@ $< -g -e main; fi

in261.elf: in261.c
	if type powerpc-eabi-gcc >/dev/null 2>&1; then powerpc-eabi-gcc -o $@ $< -O3 -e main -msim; fi

in280.elf: in280.c
	if type powerpc-eabi-gcc >/dev/null 2>&1; then powerpc-eabi-gcc -o $@ $< -e main --std=c99; fi

in276-1.elf: in276-1.c
	if type powerpc-eabi-gcc >/dev/null; then \
		powerpc-eabi-gcc -c $< -o $*.o ;\
		./ppc-elf-ld $*.o -o $@ -Tin276.lnk ;\
	fi

in276-2.elf: in276-2.c
	if type powerpc-eabi-gcc >/dev/null; then \
		powerpc-eabi-gcc -c $< -o $*.o ;\
		./ppc-elf-ld $*.o -o $@ -Tin276.lnk ;\
	fi

in276-1t.elf: in276-1.c
	if type powerpc-eabi-gcc >/dev/null; then \
		powerpc-eabi-gcc -c $< -o $*.o ;\
		./ppc-elf-ld $*.o -o $@ -Tin276-trans.lnk ;\
	fi

in276-2t.elf: in276-2.c
	if type powerpc-eabi-gcc >/dev/null; then \
		powerpc-eabi-gcc -c $< -o $*.o ;\
		./ppc-elf-ld $*.o -o $@ -Tin276-trans.lnk ;\
	fi

in276-1z.elf: in276-1.c
	if type powerpc-eabi-gcc >/dev/null; then \
		powerpc-eabi-gcc -c $< -o $*.o ;\
		./ppc-elf-ld $*.o -o $@ -Tin276-zero.lnk ;\
	fi

in276-2z.elf: in276-2.c
	if type powerpc-eabi-gcc >/dev/null; then \
		powerpc-eabi-gcc -c $< -o $*.o ;\
		./ppc-elf-ld $*.o -o $@ -Tin276-zero.lnk ;\
	fi

in279-1.elf: in279.c
	if type powerpc-eabi-gcc >/dev/null 2>&1; then powerpc-eabi-gcc -o $@ $< -e _start -Wl,-T$(@:.elf=.lnk) -msoft-float -DMSG1; fi

in279-2.elf: in279.c
	if type powerpc-eabi-gcc >/dev/null 2>&1; then powerpc-eabi-gcc -o $@ $< -e _start -Wl,-T$(@:.elf=.lnk) -msoft-float -DMSG2; fi

asm1.elf: asm1.s
	if type powerpc-eabi-as >/dev/null 2>&1; then powerpc-eabi-as -o $*.o $< ; fi
	if type powerpc-eabi-ld >/dev/null 2>&1; then powerpc-eabi-ld -o $@ $*.o -e main; fi
	rm -f $*.o

asm73.elf: asm73.s asm73.lnk
	if type powerpc-eabi-as >/dev/null 2>&1; then powerpc-eabi-as -o $*.o $<; fi
	if type powerpc-eabi-ld >/dev/null 2>&1; then powerpc-eabi-ld -o $@ $*.o -e main -Tasm73.lnk; fi
	rm -f $*.o

asm84.hex: asm84.s
	if type powerpc-eabi-as >/dev/null 2>&1; then powerpc-eabi-as -mregnames -o $*.o $<; fi
	if type powerpc-eabi-ld >/dev/null 2>&1; then powerpc-eabi-ld -o $*.elf $*.o && objcopy -O ihex $*.elf $@ ; fi
	rm -f $*.o

in294.elf: in294.s
	if type powerpc-eabi-as >/dev/null 2>&1; then powerpc-eabi-as -o $*.o $< -mregnames; fi
	if type powerpc-eabi-ld >/dev/null 2>&1; then powerpc-eabi-ld -o $@ $*.o -e main ; fi
	rm -f $*.o

