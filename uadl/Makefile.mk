## Process this file with automake to produce Makefile.in

uadl_includes = -I$(top_srcdir) $(PLASMA_CFLAGS) $(RNUMBER_CFLAGS)

noinst_HEADERS += \
	uadl/UadlTranslatingWalker.h \
	uadl/GenCode.h \
	uadl/GenInstClass.h

if !MINIMAL_BUILD

if LANG_FRONTEND

bin_PROGRAMS += uadl/make-uadl

endif

endif

# We always want to build the run-time component.
lib_LTLIBRARIES += uadl/libuadl.la 

if !MINIMAL_BUILD 

if LANG_FRONTEND

# We only want to build this library if we're building the full package, since
# otherwise we won't have the other infrastructure libraries.
lib_LTLIBRARIES += \
	uadl/libuadlMain.la \
	uadl/libuadlSupport.la

uadl_libuadlMain_la_SOURCES = \
	uadl/UadlMain.C

uadl_libuadlMain_la_CXXFLAGS = $(uadl_includes)

uadl_libuadlMain_ladir = $(includedir)/uadl


uadl_libuadlSupport_la_SOURCES = \
	uadl/UadlTrace.C \
	uadl/UadlRunControl.C \
	uadl/UadlScriptLogger.C

uadl_libuadlSupport_la_CXXFLAGS = $(uadl_includes)

uadl_libuadlSupport_la_HEADERS = \
	uadl/UadlTrace.h \
	uadl/UadlRunControl.h \
	uadl/UadlScriptLogger.h

uadl_libuadlSupport_ladir = $(includedir)/uadl


endif

endif

uadl_make_uadl_SOURCES = \
	uadl/main.C \
	uadl/MiniAdlParser.C \
	uadl/UadlDriver.C \
	uadl/UadlCommon.C \
	uadl/ResChecker.C \
	uadl/UadlHandlers.C \
	uadl/UadlParser.C \
	uadl/GenCode.C \
	uadl/GenInstClass.C \
	uadl/UadlWalker.C

uadl_make_uadl_CXXFLAGS = $(uadl_includes)

uadl_make_uadldir = $(includedir)/uadl

uadl_make_uadl_HEADERS = \
	uadl/UadlCommon.h \
	uadl/UadlDriver.h \
	uadl/UadlParser.h \
	uadl/UadlHandlers.h \
	uadl/TransactionTypes.h \
	uadl/ResChecker.h

uadl_make_uadl_LDADD = \
	$(Iss)/libAdlIss.la \
	$(Fe)/libAdlFrontEnd.la \
	$(Helpers)/libAdlHelpers.la \
	$(Helpers)/libAdlSupport.la \
	$(Helpers)/libAdlLogUsage.la \
	$(Asm)/libAdlAsm.la \
	$(RNUMBER_LTLIBS) \
	$(OccLibs)/libocc.la $(OccLibs)/libgc.la

CLEANFILES += $(BUILT_SOURCES)


uadl_libuadl_la_SOURCES = \
	uadl/PipelineStage.C \
	uadl/UadlArch.C \
	uadl/LogControl.C \
	uadl/LogQueue.C \
	uadl/Logger.C \
	uadl/FetchLogger.C \
	uadl/UadlMemory.C \
	uadl/BandwidthControl.C \
	uadl/RegResource.C \
	uadl/Semaphore.C \
	uadl/Flag.C \
	uadl/Cache.C

uadl_libuadl_la_CXXFLAGS = $(uadl_includes)

uadl_libuadl_ladir = $(includedir)/uadl

uadl_libuadl_la_HEADERS = \
	uadl/UadlArch.h \
  uadl/UadlArchIf.h \
  uadl/UadlArchSystemC.h \
	uadl/UadlPlugin.h \
	uadl/DynParms.h \
	uadl/LogControl.h \
	uadl/LogQueue.h \
  uadl/Logger.h \
  uadl/FetchLogger.h \
  uadl/Timer.h \
  uadl/UadlCommon.h \
	uadl/UadlWalker.h \
	uadl/DefaultFetchUnit.h \
  uadl/UadlMemoryIf.h \
  uadl/UadlMemory.h \
	uadl/UadlTypes.h \
	uadl/PipelineStage.h \
	uadl/RegResource.h \
	uadl/Semaphore.h \
	uadl/Flag.h \
  uadl/ForwardingPath.h \
	uadl/BandwidthControl.h \
  uadl/MemoryRequest.h \
  uadl/FetchUnitIf.h \
  uadl/Named.h \
	uadl/BranchPredictor.h \
  uadl/Cache.h
