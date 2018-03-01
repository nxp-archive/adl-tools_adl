

RUNSIM_PATH = $(top_srcdir)/adlapi2simapi/sharedsim

ADLAPI2SIMAPI_FLAGS = -shared -I$(top_srcdir) -I$(RUNSIM_PATH) $(PLASMA_CFLAGS) $(RNUMBER_CFLAGS) \
											-L$(top_builddir)/lib $(PLASMA_LIB_PATHS) -lgc

#MODEL = $(top_srcdir)/designs/sc3400_arch.adl
ADLAPI2SIMAPI_MODEL = sc3850_arch_runsim_cpp.adl

if !MINIMAL_BUILD

if LANG_FRONTEND

if !CREATE_EXTERNAL_RELEASE

EXTRA_DIST += adlapi2simapi/sharedsim/ccssim2tmpl.h adlapi2simapi/sharedsim/ccssimapi.h \
						  adlapi2simapi/sharedsim/defines.h adlapi2simapi/sharedsim/devices.h \
							adlapi2simapi/sharedsim/simapi.h adlapi2simapi/sharedsim/simapi_loader.h

EXTRA_DIST += adlapi2simapi/src/adlapi2simapi.C adlapi2simapi/src/regmap.h adlapi2simapi/src/sc3850_arch_runsim_cpp.adl

endif

# Only build this integration if we're not an external release and the user has
# specified a path to runsim.
if RUNSIM

if !IS_EXTERNAL_RELEASE

check_PROGRAMS += adlapi2simapi/src/adlapi2simapi.so 

nodist_adlapi2simapi_src_adlapi2simapi_so_SOURCES =  \
	adlapi2simapi/src/adlapi2simapi.C

adlapi2simapi/src/adlapi2simapi.so: adlapi2simapi/src/adlapi2simapi.C adlapi2simapi/src/regmap.h  adlapi2simapi/src/sc3400_arch.so
	$(CXX) -ggdb3 $< -o $@ adlapi2simapi/src/sc3400_arch.so $(ADLAPI2SIMAPI_FLAGS)

adlapi2simapi/src/uadlapi2simapi.so: adlapi2simapi/src/adlapi2simapi.C adlapi2simapi/src/regmap.h 
	$(CXX) $< -o $@  adlapi2simapi/src/sc3400_uarch.so $(ADLAPI2SIMAPI_FLAGS) -D=UADL

adlapi2simapi/src/padlapi2simapi.so: adlapi2simapi/src/adlapi2simapi.C adlapi2simapi/src/regmap.h 
	$(CXX) $< -o $@  adlapi2simapi/src/sc3400_uarch.so $(ADLAPI2SIMAPI_FLAGS) -D=PADL

adlapi2simapi/src/pnmadlapi2simapi.so: adlapi2simapi/src/simapi_adlpac.C 
	$(CXX) $< -o $@ $(SIMAPI_LOADER) adlapi2simapi/src/sc3400_uarch.so $(ADLAPI2SIMAPI_FLAGS) -D=PADL -D=USE_POSIX_THREADS  

adlapi2simapi/src/sc3400_arch.so: $(ADLAPI2SIMAPI_MODEL) $(MakeIss)
	$(ADL2ISS) $< --no-log-usage -no-optimize --target=so -o $@

CLEANFILES += adlapi2simapi/src/adlapi2simapi.so adlapi2simapi/src/padlapi2simapi.so \
							adlapi2simapi/src/pnmadlapi2simapi.so adlapi2simapi/src/sc3400_arch.so

#install:
#	cp adlapi2simapi.so padlapi2simapi.so pnmadlapi2simapi.so ../../starcore_utils/adlapi2simapi

endif !IS_EXTERNAL_RELEASE

endif RUNSIM

endif LANG_FRONTEND

endif !MINIMAL_BUILD
