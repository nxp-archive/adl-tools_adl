
AUTOMAKE_OPTIONS = foreign serial-tests no-exeext

TESTS = regress

check_PROGRAMS = mod1 mod1-jobs ttmod1 mod3.so mod8.so mod1-base.so							\
	mod1-base-rn.so mod13 mod18.so mod29.so mod1-em mod1-em.so mod1-em-o.o				\
	ttmod1-em.so mod1-tlm2-em mod3-em.so mod13-em.so mod18-em.so mod18-base.so		\
	mod18-tlm2-em mod27-base.so mod29-base-rn.so driver driver2 driver3 driver4		\
	driver5 driver7 driver8 driver9 driver10 driver11 driver12 driver13 driver14	\
	driver15 driver16 driver17 driver18 objdump-mod1.so as-mod61.so as-mod63.so		\
	as-mod65.so dtest1 dtest2 dtest3 adriver1 2coretest dlx dlx-em dlx-tlm2-em		\
	dlx-lm dlx-jobs dlx-lm-jobs dlx-lm-jobs-safe driver6 dlx.so dlx-ext-mem.so		\
	dyntest_wrapper.so dyntest extern-mem.so uadl-extern-mem.so										\
	tlm2-extern-mem.so

CLEANFILES = *.cc *.cc.h dlx*.h *.bc

EXTRA_DIST = \
	$(srcdir)/*.dat \
	$(srcdir)/*.uvp \
	$(srcdir)/*.regress \
	$(srcdir)/*.cli \
	$(srcdir)/regress \
	$(srcdir)/mod1-so.ttc \
	$(srcdir)/ttmod1-em.ttc \
	$(srcdir)/mod3-so.ttc \
	$(srcdir)/mod18-em-so.ttc \
	$(srcdir)/preamble.ttc \
	$(srcdir)/preamble.regress \
	$(srcdir)/2coretest.regress \
	$(srcdir)/itrace2tarmac.py \
	$(srcdir)/py_scripts \
	$(srcdir)/distest7.in

BUILT_SOURCES = mod29.cc.h

noinst_HEADERS = \
	dyntest_wrapper.h \
	e_message_priority.h \
	ADL_encoder.h

BINBUILD = $(top_builddir)/external/binutils-adl
BINSRC   = $(top_srcdir)/external/binutils-adl

PlasmaPfx   = $(PLASMA_PATH)/lib

ASMFLAGS = --arch-pfx=ppc

ADL2ISS = $(AM_V_GEN) $(top_builddir)/scripts/adl2iss --prefix=$(top_builddir) $< --output=$@ --no-optimize --no-warn-redefine

ADL2ISS_OBJ = $(AM_V_GEN) $(top_builddir)/scripts/adl2iss --prefix=$(top_builddir) $< --output=$@ --no-optimize --compile-only --no-warn-redefine

ADL2ASM = $(AM_V_GEN) $(top_builddir)/scripts/adl2asm $(ASMFLAGS) --prefix=$(top_builddir) $< --bprefix=$(BINBUILD) --bsprefix=$(BINSRC) --build-locally --no-warn-redefine

SIMPLELINK = $(AM_V_CXXLD) $(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@

MakeIss =  $(top_builddir)/iss/make-iss 

MakeAsm =  $(top_builddir)/asm/make-asm

driver_SOURCES = driver.C

driver_CXXFLAGS = $(AM_CXXFLAGS)

driver_LDFLAGS = $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --static-libs $(ADL_JIT)) $(AM_LDFLAGS)

driver_LDADD = mod1-o.o

driver_DEPENDENCIES = mod1-o.o

driver2_SOURCES = driver2.C

driver2_CXXFLAGS = $(AM_CXXFLAGS) -DIssNamespace=mod1

driver2_LDADD = mod1-base.so

driver3_SOURCES = driver2.C

driver3_LDADD = mod18-base.so

driver3_CXXFLAGS = $(AM_CXXFLAGS) -DIssNamespace=mod18 -DTrapAddr=0x3000

driver4_SOURCES = driver2.C

driver4_LDADD = mod27-base.so

driver4_CXXFLAGS = $(AM_CXXFLAGS) -DIssNamespace=mod27 -DVLE

driver5_SOURCES = driver2.C

driver5_CXXFLAGS = $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --basic --cflags) -DIssNamespace=mod1 -DUSE_LOGGING_MGR

driver5_LDADD = mod1-base-rn.so 

driver5_LDFLAGS = $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --basic --logging --libs)  $(AM_LDFLAGS)

driver6_SOURCES = driver6.C

driver6_CXXFLAGS = \
	$(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --basic --cflags --uadl) \
	-DUADL -DUSE_CALLBACKS -DUadlNamespace=mydlx

driver6_LDADD = dlx.so 

driver6_LDFLAGS = $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --basic --libs --uadl --no-rnumber) $(AM_LDFLAGS)

driver7_SOURCES = driver.C

driver7_CXXFLAGS = $(AM_CXXFLAGS) -DExceptionCallback='"Program"'

driver7_LDADD = mod3.so

driver7_LDFLAGS = $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --libs --so-target) $(AM_LDFLAGS)

driver8_SOURCES = driver8.C

driver8_LDADD = mod18.so

driver8_LDFLAGS = $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --libs) $(AM_LDFLAGS)

driver9_SOURCES = driver9.C

driver9_CXXFLAGS = $(AM_CXXFLAGS) $(PLASMA_CFLAGS)

driver9_LDADD = mod18-em.so $(PlasmaPfx)/libgc.la

driver9_LDFLAGS = $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --libs) $(AM_LDFLAGS)

driver10_SOURCES = driver10.C

driver10_CXXFLAGS = $(AM_CXXFLAGS) -DIssNamespace=mod1 $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --basic --cflags)

driver10_LDADD = mod1-base-rn.so

driver10_LDFLAGS = $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --basic --readers --libs)  $(AM_LDFLAGS)

driver11_SOURCES = driver11.C

driver11_CXXFLAGS = $(AM_CXXFLAGS) $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --basic --cflags)

driver11_LDADD = mod29-base-rn.so

driver11_LDFLAGS = $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --basic --readers --libs)  $(AM_LDFLAGS)

driver12_SOURCES = driver9.C

driver12_CXXFLAGS = $(AM_CXXFLAGS) $(PLASMA_CFLAGS)

driver12_LDADD = mod1-em.so $(PlasmaPfx)/libgc.la

driver12_LDFLAGS = $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --libs) $(AM_LDFLAGS)

driver13_SOURCES = driver9.C

driver13_CXXFLAGS = $(AM_CXXFLAGS) $(PLASMA_CFLAGS)

driver13_LDADD = mod3-em.so $(PlasmaPfx)/libgc.la

driver13_LDFLAGS = $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --libs) $(AM_LDFLAGS)

driver14_SOURCES = driver14.C

driver14_LDADD = mod29.so

driver14_LDFLAGS = $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --libs) $(AM_LDFLAGS)

driver15_SOURCES = driver15.C

driver15_CXXFLAGS = $(AM_CXXFLAGS)

driver15_LDFLAGS = $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --static-libs ) $(AM_LDFLAGS)

driver15_LDADD = mod11-em-o.o

driver15_DEPENDENCIES = mod11-em-o.o

driver16_SOURCES = driver9.C

driver16_CXXFLAGS = $(AM_CXXFLAGS) $(PLASMA_CFLAGS)

driver16_LDADD = ttmod1-em.so $(PlasmaPfx)/libgc.la

driver16_LDFLAGS = $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --libs) $(AM_LDFLAGS)

driver17_SOURCES = driver9.C

driver17_CXXFLAGS = $(AM_CXXFLAGS) $(PLASMA_CFLAGS)

driver17_LDADD = mod13-em.so

driver17_LDFLAGS = $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --libs) $(AM_LDFLAGS)

driver18_SOURCES = driver18.C

driver18_CXXFLAGS = $(AM_CXXFLAGS) $(PLASMA_CFLAGS) --std=c++0x

driver18_LDADD = mod1-em-o.o

driver18_LDFLAGS = $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --libs) $(AM_LDFLAGS)



dtest1_SOURCES = dtest1.c

dtest1_LDADD = objdump-mod1.so

dtest2_SOURCES = dtest2.C

dtest2_LDADD = -ldl

dtest3_SOURCES = dtest3.C

dtest3_LDFLAGS = $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --libs) $(AM_LDFLAGS)

dtest3_LDADD = objdump-mod1.so $(RNUMBER_STATIC_LIBS)

dtest3.o: objdump-mod1.so


adriver1_SOURCES = adriver1.C

adriver1_LDADD = mod8.so

adriver1_LDFLAGS = $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --libs) $(AM_LDFLAGS)

mod1_jobs_SOURCES = $(top_srcdir)/tests/iss/mod1.adl

mod1_base_so_SOURCES = $(top_srcdir)/tests/iss/mod1.adl

mod1_base_rn_so_SOURCES = 

mod1_em_SOURCES = $(top_srcdir)/tests/iss/mod1.adl

mod1_em_o_o_SOURCES = $(top_srcdir)/tests/iss/mod1.adl

mod1_tlm2_em_SOURCES = $(top_srcdir)/tests/iss/mod1.adl

mod1_em_so_SOURCES = $(top_srcdir)/tests/iss/mod1.adl

ttmod1_em_so_SOURCES = $(top_srcdir)/tests/iss/mod1.adl

mod3_em_so_SOURCES = $(top_srcdir)/tests/iss/mod3.adl

mod3_so_SOURCES = $(top_srcdir)/tests/iss/mod3.adl

mod1_SOURCES = $(top_srcdir)/tests/iss/mod1.adl

mod8_so_SOURCES = $(top_srcdir)/tests/iss/mod8.adl

mod13_SOURCES = $(top_srcdir)/tests/iss/mod13.adl

mod13_em_so_SOURCES = $(top_srcdir)/tests/iss/mod13.adl

mod18_so_SOURCES = $(top_srcdir)/tests/iss/mod18.adl

mod18_tlm2_em_SOURCES = $(top_srcdir)/tests/iss/mod18.adl

mod29_so_SOURCES = $(top_srcdir)/tests/iss/mod29.adl

mod18_em_so_SOURCES = $(top_srcdir)/tests/iss/mod18.adl

mod18_base_so_SOURCES = $(top_srcdir)/tests/iss/mod18.adl

mod27_base_so_SOURCES = $(top_srcdir)/tests/iss/mod27.adl

mod29_base_rn_so_SOURCES = $(top_srcdir)/tests/iss/mod29.adl

mod29_ADLFLAGS = --target=base-so --no-debug-mode --no-jit --rollback-mode --extern-mem --no-syscall-enabled --no-event-bus-send --no-log-writeregs


as_mod61_so_SOURCES = $(top_srcdir)/tests/iss/mod61.adl

as_mod63_so_SOURCES = $(top_srcdir)/tests/iss/mod63.adl

as_mod65_so_SOURCES = $(top_srcdir)/tests/iss/mod65.adl


2coretest_SOURCES = 2coretest.C

2coretest_LDADD = mod1-base-o.o mod18-base-o.o

2coretest_LDFLAGS = $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --basic --libs)  $(AM_LDFLAGS)


dyntest_SOURCES = dyntest.C

dyntest_CXXFLAGS = $(AM_CXXFLAGS) $(PLASMA_CFLAGS) -fPIC

dyntest_LDFLAGS = -Wl,--no-as-needed $(shell $(top_builddir)/scripts/adl-config --setpfx=$(top_builddir) --uadl --libs --basic --support) -Wl,--as-needed $(AM_LDFLAGS)

dyntest_wrapper_so_SOURCES = dyntest_wrapper.C


extern_mem_so_SOURCES = ExternMem.C

extern_mem_so_CXXFLAGS = -fPIC -I$(top_srcdir)

extern_mem_so_LDFLAGS = -shared

extern_mem_so_LDADD = 


tlm2_extern_mem_so_SOURCES = ExternMem.C

tlm2_extern_mem_so_CXXFLAGS = -fPIC -I$(top_srcdir) -DDATA_INVARIANT_MEMORY

tlm2_extern_mem_so_LDFLAGS = -shared

tlm2_extern_mem_so_LDADD = 


uadl_extern_mem_so_SOURCES = UadlExternMem.C

uadl_extern_mem_so_CXXFLAGS = -fPIC -I$(top_srcdir)

uadl_extern_mem_so_LDFLAGS = -shared

uadl_extern_mem_so_LDADD = 

# Run check with arguments to run the regressions in parallel.
pcheck:
	$(MAKE) check RegressOpts="-p=8"

# Stupid thing about the new libtool- it refuses to create shared libraries if disable-shared
# is set.  So, I have to do it manually.
extern-mem.so: $(extern_mem_so_OBJECTS) $(extern_mem_so_DEPENDENCIES) 
	@rm -f extern-mem.so
	$(SIMPLELINK) $(extern_mem_so_LDFLAGS) $(extern_mem_so_OBJECTS) $(extern_mem_so_LDADD) $(LIBS)

# Same as above, except using TLM2-style address swizzling, rather than byte-swapping.
tlm2-extern-mem.so: $(tlm2_extern_mem_so_OBJECTS) $(tlm2_extern_mem_so_DEPENDENCIES) 
	@rm -f tlm2-extern-mem.so
	$(SIMPLELINK) $(tlm2_extern_mem_so_LDFLAGS) $(tlm2_extern_mem_so_OBJECTS) $(tlm2_extern_mem_so_LDADD) $(LIBS)

uadl-extern-mem.so: $(uadl_extern_mem_so_OBJECTS) $(uadl_extern_mem_so_DEPENDENCIES) 
	@rm -f uadl-extern-mem.so
	$(SIMPLELINK) $(uadl_extern_mem_so_LDFLAGS) $(uadl_extern_mem_so_OBJECTS) $(uadl_extern_mem_so_LDADD) $(LIBS)

ttmod1_SOURCES = $(top_srcdir)/tests/iss/mod1.adl

objdump_mod1_so_SOURCES = $(top_srcdir)/tests/iss/mod1.adl

AM_LDFLAGS = -Wl,-R. -Wl,-R$(abspath $(top_builddir))/lib $(RNUMBER_LIB_PATHS) -Wl,-R..

AM_CXXFLAGS = -I$(top_srcdir) $(RNUMBER_CFLAGS) $(PLASMA_CFLAGS)

AM_CFLAGS = -I$(top_srcdir)

mod18.so: $(mod18_so_SOURCES)  $(MakeIss)
	$(ADL2ISS) --target=so --cpp-cmd='include:\<array\>'

mod29.so: $(mod29_so_SOURCES)  $(MakeIss)
	$(ADL2ISS) --target=so --extern-event-bus

mod1-em.so: $(mod1_em_so_SOURCES)  $(MakeIss)
	$(ADL2ISS) --target=so --extern-mem --rollback --extern-dmi --mflags='--dmi-cache' $(ADL_JIT) --decode-cache --cleanup=obj

mod1-em-o.o: $(mod1_em_o_o_SOURCES)  $(MakeIss)
	$(ADL2ISS) --compile-only --extern-mem

ttmod1-em.so: $(ttmod1_em_so_SOURCES) ttmod1-em.ttc $(MakeIss)
	$(ADL2ISS) --target=so --config=$(srcdir)/ttmod1-em.ttc --time-tagged --extern-mem --extern-dmi --mflags='--dmi-cache' --decode-cache --cleanup=obj

mod1-em: $(mod1_em_SOURCES)  $(MakeIss)
	$(ADL2ISS) --extern-mem --extern-dmi --decode-cache --cleanup=obj --mflags="--extern-dmi-entry-size=5"

# This model using the TLM2-style address swizzling, rather than byte-swapping
# for big-endian memory handling.
mod1-tlm2-em: $(mod1_tlm2_em_SOURCES)  $(MakeIss)
	$(ADL2ISS) --extern-mem --extern-dmi --tlm2-endianness --decode-cache --cleanup=obj

mod3-em.so: $(mod3_em_so_SOURCES)  $(MakeIss)
	$(ADL2ISS) --cleanup=obj --target=so --extern-mem --config=$(srcdir)/mod3-so.ttc

mod18-em.so: $(mod18_em_so_SOURCES)  $(MakeIss)
	$(ADL2ISS) --target=so --extern-mem --extern-dmi $(ADL_JIT) --config=$(srcdir)/mod18-em-so.ttc --decode-cache --cleanup=obj -D=NoCacheCounters

mod18-base.so: $(mod18_base_so_SOURCES)  $(MakeIss)
	$(ADL2ISS) --target=base-so --no-debug-mode --no-rnumber --no-jit --extern-mem --no-syscall-enabled --iss-namespace=mod18

# This model using the TLM2-style address swizzling, rather than byte-swapping
# for big-endian memory handling.
mod18-tlm2-em: $(mod18_tlm2_em_SOURCES)  $(MakeIss)
	$(ADL2ISS) --extern-mem --extern-dmi --tlm2-endianness --decode-cache --cleanup=obj

mod18-base-o.o: $(mod18_base_so_SOURCES)  $(MakeIss)
	$(ADL2ISS_OBJ) --target=base-so --no-debug-mode --no-rnumber --no-jit --extern-mem --no-syscall-enabled --iss-namespace=mod18

mod1: $(mod1_SOURCES) $(MakeIss)
	$(ADL2ISS) --jobs=4 --low-mem --parallel-build $(ADL_JIT) --decode-cache -static

mod1-o.o: $(mod1_SOURCES)  $(MakeIss)
	$(ADL2ISS) --disassembler --compile-only --config=$(srcdir)/mod1-so.ttc $(ADL_JIT) --decode-cache --td-api --iss-namespace=myiss

mod3.so: $(mod3_so_SOURCES)  $(MakeIss)
	$(ADL2ISS) --cleanup=obj --target=so --config=$(srcdir)/mod3-so.ttc --iss-namespace=myiss

mod8.so: $(mod8_so_SOURCES)  $(MakeIss)
	$(ADL2ISS) --target=base-so --disassembler --extern-mem --cleanup=none --no-syscall-enabled

mod1-base.so: $(mod1_base_so_SOURCES)  $(MakeIss)
	$(ADL2ISS) --target=base-so --no-debug-mode --no-rnumber --no-jit --extern-mem --no-syscall-enabled  --iss-namespace=mod1

mod1-base-rn.so: $(mod1_base_so_SOURCES)  $(MakeIss)
	$(ADL2ISS) --target=base-so --no-debug-mode --no-jit --extern-mem --no-syscall-enabled --iss-namespace=mod1

mod1-base-o.o: $(mod1_base_so_SOURCES)  $(MakeIss)
	$(ADL2ISS_OBJ) --target=base-so --no-debug-mode --no-rnumber --no-jit --extern-mem --no-syscall-enabled  --iss-namespace=mod1

mod1-jobs: $(mod1_jobs_SOURCES) $(MakeIss)
	$(ADL2ISS) $(ADL_JIT) --decode-cache --jobs=3

mod11-em-o.o: $(top_srcdir)/tests/iss/mod11.adl  $(MakeIss)
	$(ADL2ISS) --compile-only --decode-cache --td-api --extern-mem

mod13: $(mod13_SOURCES) $(MakeIss)
	$(ADL2ISS) --jobs=4 --low-mem --parallel-build

mod13-em.so: $(mod13_em_so_SOURCES)  $(MakeIss)
	$(ADL2ISS) --target=so --extern-mem --extern-dmi --no-jit --decode-cache --cleanup=obj --jobs=3

mod27-base.so: $(mod27_base_so_SOURCES)  $(MakeIss)
	$(ADL2ISS) --target=base-so --no-debug-mode --no-rnumber --no-jit --extern-mem --no-syscall-enabled --iss-namespace=mod27

mod29-base-rn.so: $(mod29_base_rn_so_SOURCES)  $(MakeIss)
	$(ADL2ISS) $(mod29_ADLFLAGS)

objdump-mod1.so: $(mod1_SOURCES)  $(MakeAsm)
	$(ADL2ASM) -shared -dbg

as-mod61.so: $(as_mod61_so_SOURCES) $(MakeAsm)
	$(ADL2ASM) -shared=$@ --asm-ver="1.2.3"

as-mod63.so: $(as_mod63_so_SOURCES) $(MakeAsm)
	$(ADL2ASM) -shared=$@ --asm-ver="1.2.3"

as-mod65.so: $(as_mod65_so_SOURCES) $(MakeAsm)
	$(ADL2ASM) -shared=$@ --asm-ver="1.2.3"


# For some reason, compiling w/o fPIC doesn't work, even on i686.  So, I'm
# forcing it to here.
dyntest_wrapper.so: $(dyntest_wrapper_so_SOURCES) dyntest_wrapper.h
	$(AM_V_CXXLD) $(CXXCOMPILE) -fPIC -shared -o $@ $<

ttmod1: $(ttmod1_SOURCES) preamble.ttc
	$(ADL2ISS) --time-tagged --config=$(top_srcdir)/tests/iss/cfg1.ttc \
		--config=$(srcdir)/preamble.ttc \
		--preamble="INFO:  Foo" \
		--preamble="INFO:  More Foo"

mod29.cc.h: $(top_srcdir)/tests/iss/mod29.adl $(MakeIss)
	$(MakeHeader)

## uADL script tests
UADL_PARSER = $(top_builddir)/uadl/make-uadl
UADL2MODEL  = $(AM_V_GEN) $(top_builddir)/scripts/uadl2model --prefix=$(top_builddir) \
              $(UADLFLAGS) --no-optimize --no-warn-redefine

MakeHeader = $(ADL2ISS) $< -o=$@ $(ADLFLAGS) $($(patsubst %.cc.h,%_ADLFLAGS,$(@))) --gen-only

# ADL source must precede uADL source
dlx_SOURCES = $(top_srcdir)/tests/iss/mod1.adl $(top_srcdir)/tests/uadl/dlx.uadl
dlx_lm_SOURCES = $(top_srcdir)/tests/iss/mod1.adl $(top_srcdir)/tests/uadl/dlx.uadl
dlx_jobs_SOURCES = $(top_srcdir)/tests/iss/mod1.adl $(top_srcdir)/tests/uadl/dlx.uadl
dlx_lm_jobs_SOURCES = $(top_srcdir)/tests/iss/mod1.adl $(top_srcdir)/tests/uadl/dlx.uadl
dlx_lm_jobs_safe_SOURCES = $(top_srcdir)/tests/iss/mod1.adl $(top_srcdir)/tests/uadl/dlx.uadl
dlx_so_SOURCES = $(top_srcdir)/tests/iss/mod1.adl $(top_srcdir)/tests/uadl/dlx.uadl

# To get automake to work correctly with its install...
dlx_ext_mem_so_SOURCES = 
dlx_em_SOURCES = 
dlx_tlm2_em_SOURCES = 

dlx:	$(dlx_SOURCES) $(UADL_PARSER)
	$(UADL2MODEL) $(dlx_SOURCES) -static

dlx-em:	$(dlx_SOURCES) $(UADL_PARSER)
	$(UADL2MODEL) $(dlx_SOURCES) -o=$@ --extern-mem --extern-dmi --safe-mode

dlx-tlm2-em:	$(dlx_SOURCES) $(UADL_PARSER)
	$(UADL2MODEL) $(dlx_SOURCES) -o=$@ --extern-mem --safe-mode --iss-mem-mode --tlm2-endianness

dlx-lm:	$(dlx_SOURCES) $(UADL_PARSER)
	$(UADL2MODEL) $(dlx_SOURCES) --low-mem --uadl-low-mem -p=3 -o=$@

dlx-jobs:	$(dlx_SOURCES) $(UADL_PARSER)
	$(UADL2MODEL) $(dlx_SOURCES) --jobs=3 -p=3 -o=$@

dlx-lm-jobs:	$(dlx_SOURCES) $(UADL_PARSER)
	$(UADL2MODEL) $(dlx_SOURCES) --low-mem --uadl-low-mem --jobs=3 -p=3 -o=$@

dlx-lm-jobs-safe:	$(dlx_SOURCES) $(UADL_PARSER)
	$(UADL2MODEL) $(dlx_SOURCES) --low-mem --jobs=3 --safe-mode --iss-mem-mode  -o=$@ --iss-separate-compile

dlx.so: $(dlx_so_SOURCES) $(UADL_PARSER)
	$(UADL2MODEL) --target=base-so --no-rnumber \
	--config=$(srcdir)/mod1-so.ttc --no-debug-mode --no-syscall-enabled --namespace=mydlx \
	$(dlx_so_SOURCES) 

dlx-ext-mem.so: $(dlx_so_SOURCES) $(UADL_PARSER) dyntest_wrapper.so
	$(UADL2MODEL) -o=$@ --target=so --no-debug-mode --extern-mem $(dlx_so_SOURCES) --ldflags="-L$(builddir) -Wl,-R$(abspath $(builddir)) -Wl,--no-as-needed dyntest_wrapper.so -Wl,--as-needed"



distest7.in: $(top_srcdir)/tests/iss/asm1.s
	$(top_builddir)/tests/iss/as-mod1 $< -o $@
