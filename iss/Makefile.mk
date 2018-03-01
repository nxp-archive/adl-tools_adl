

lib_LTLIBRARIES += \
	iss/libAdlIssSupport.la

iss_libAdlIssSupport_la_SOURCES = \
	iss/ModelSupport.C

if LLVM

lib_LTLIBRARIES += \
	iss/libAdlIssJitSupport.la

iss_libAdlIssJitSupport_la_SOURCES = \
	iss/JitSupport.C \
	iss/SingleFunctionInliner.C

iss_libAdlIssJitSupport_la_CXXFLAGS = $(iss_includes) $(LLVM_CFLAGS)

Extract = iss/adl-llvm-extract

iss_adl_llvm_extract_SOURCES = iss/adl-llvm-extract.C

# We take on the no-omit option so that if profiling is used, we don't get a
# compile error.
iss_adl_llvm_extract_CXXFLAGS = $(LLVM_CXXFLAGS) -fno-omit-frame-pointer

iss_adl_llvm_extract_LDFLAGS = \
	$(shell $(LLVM_BINDIR)/llvm-config --libs ipo bitreader bitwriter asmparser) \
	$(shell $(LLVM_BINDIR)/llvm-config --ldflags)

endif LLVM

if !MINIMAL_BUILD 

if LANG_FRONTEND

iss_MakeIss = iss/make-iss
iss_LibAdlIss = iss/libAdlIss.la

else

iss_MakeIss = 
iss_LibAdlIss = 

endif

bin_PROGRAMS += $(iss_MakeIss) $(Extract) iss/tracediff

noinst_PROGRAMS += iss/uvptest 

#iss/dattest

lib_LTLIBRARIES += \
	iss/libAdlIssExec.la \
	iss/libAdlIssMain.la \
	iss/libAdlIssLogging.la \
	iss/libAdlIssTransLogging.la \
	$(iss_LibAdlIss) \
	iss/libAdlReaders.la \
	iss/libAdlParsing.la \
	iss/libAdlScript.la

iss_tracediff_SOURCES = \
	iss/tracediff.C

iss_tracediff_CXXFLAGS = $(iss_includes) $(RNUMBER_CFLAGS)

iss_tracediff_LDADD = \
	iss/libAdlParsing.la \
	$(Helpers)/libAdlSupport.la \
	$(RNUMBER_LTLIBS) \
	$(LZ4_LTLIBS)


iss_uvptest_SOURCES = \
	iss/uvptest.C

iss_uvptest_CXXFLAGS = $(iss_includes)

iss_uvptest_LDADD = \
	iss/libAdlParsing.la \
	$(Helpers)/libAdlSupport.la \
	$(RNUMBER_LTLIBS) \
	$(LZ4_LTLIBS)

# iss_dattest_SOURCES = \
# 	iss/dattest.C 

# iss_dattest_CXXFLAGS = $(iss_includes)

# iss_dattest_LDADD = \
# 	iss/libAdlParsing.la \
# 	$(Helpers)/libAdlSupport.la \
# 	$(RNUMBER_LTLIBS) \
# 	$(BZIP2_LDFLAGS)


iss_make_iss_SOURCES = \
	iss/main.C

iss_make_iss_CXXFLAGS = $(iss_includes) $(OccFlags)

iss_make_iss_LDADD = \
	iss/libAdlIss.la \
	$(Fe)/libAdlFrontEnd.la \
	$(Helpers)/libAdlHelpers.la \
	$(Helpers)/libAdlSupport.la \
	$(Helpers)/libAdlLogUsage.la \
	$(Asm)/libAdlAsm.la \
	$(OccLibs)/libocc.la $(OccLibs)/libgc.la \
	$(RNUMBER_LTLIBS)


iss_libAdlParsing_la_SOURCES = \
	iss/MmapFiles.C \
	iss/ElfParser.C \
	iss/Token.C \
	iss/DatParser.C \
	iss/UvpFlexLexer.C

if HAVE_FLEX
iss_libAdlParsing_la_SOURCES += iss/uvptokenizer.ll
else
iss_libAdlParsing_la_SOURCES += iss/uvptokenizer.cc
endif

if HAVE_BISON
iss_libAdlParsing_la_SOURCES += iss/uvpparser.yy
else
iss_libAdlParsing_la_SOURCES += iss/uvpparser.cc
endif

iss_libAdlParsing_la_CXXFLAGS = $(iss_includes) $(RNUMBER_CFLAGS) $(LZ4_CFLAGS)


iss_libAdlReaders_la_SOURCES = \
	iss/Reader.C \
	iss/ElfBaseReader.C \
	iss/ElfReader.C \
	iss/UvpReader.C \
	iss/DatReader.C \
	iss/IHexReader.C \
  iss/TarmacReader.C \
	iss/MemoryLog.C \
	iss/NodeHelpers.C \
	iss/StringHelpers.C

iss_libAdlReaders_la_CXXFLAGS = $(iss_includes)


iss_libAdlIssExec_la_SOURCES = \
	iss/DynLoad.C \
	iss/Writer.C \
  iss/ExecSupport.C \
	iss/ThreadedKernel.C \
  iss/MiscWriter.C \
	iss/ReportExtras.C \
  iss/UvpWriter.C \
  iss/DatWriter.C \
	iss/FastDatWriter.C \
  iss/GdbServer.C \
  iss/RemoteGdbServer.C \
  iss/WaveformThread.C \
  iss/DbgRegMap.C \
  iss/SysCallMgr.C \
  iss/TraceRunControl.C

iss_libAdlIssExec_la_CXXFLAGS = $(iss_includes) $(RNUMBER_CFLAGS) $(LZ4_CFLAGS)


iss_libAdlIssMain_la_SOURCES = \
	iss/IssMain.C

iss_libAdlIssMain_la_CXXFLAGS = $(iss_includes) $(RNUMBER_CFLAGS)


iss_libAdlScript_la_SOURCES = \
	iss/Scripting.C \
	iss/ScriptLogger.C

iss_libAdlScript_la_CXXFLAGS = $(iss_includes)


iss_libAdlIssLogging_la_SOURCES = \
	iss/LoggingMgr.C \
	iss/LoggingMgrImpl.C \
	iss/ThreadedLogger.C

iss_libAdlIssLogging_la_CXXFLAGS = $(iss_includes) $(RNUMBER_CFLAGS) 


iss_libAdlIssTransLogging_la_SOURCES = \
	iss/TransLoggingMgr.C \
	iss/LoggingMgrImpl.C

iss_libAdlIssTransLogging_la_CXXFLAGS = $(iss_includes) $(RNUMBER_CFLAGS)


iss_libAdlIss_la_SOURCES = \
	iss/AttrsHelper.h \
	iss/GenMmu.C \
	iss/GenCache.C \
	iss/GenRegs.C \
	iss/GenMems.C \
	iss/GenInstrs.C \
	iss/GenInstrPacket.C \
	iss/GenExceptions.C \
	iss/GenSim.C \
	iss/GenDriver.C \
	iss/FieldSubstWalker.C \
	iss/InstrWalker.C \
	iss/ConfigDB.C \
	iss/ConfigParser.C \
	iss/MiscTypes.C  \
	iss/PfxFileArray.C

iss_libAdlIss_la_CXXFLAGS = $(iss_includes) $(OccFlags)

endif !MINIMAL_BUILD 

noinst_HEADERS += \
	iss/AttrsHelper.h \
	iss/InstrWalker.h \
	iss/FieldSubstWalker.h \
	iss/ConfigDB.h \
	iss/ConfigParser.h \
	iss/GenDriver.h \
	iss/GenSim.h \
	iss/GenMmu.h \
	iss/GenCache.h \
	iss/GenRegs.h \
	iss/GenMems.h \
	iss/GenInstrPacket.h \
	iss/GenExceptions.h \
	iss/GenInstrs.h \
	iss/MiscTypes.h \
	iss/PfxFileArray.h

iss_HEADERS = \
	iss/MmapFiles.h \
	iss/MemHandler.h \
	iss/DmiMemHandler.h \
	iss/ModelInterface.h \
	iss/MemoryLog.h \
	iss/ModelSupport.h \
	iss/PacketPool.h \
	iss/CircularQueue.h \
	iss/JitSupport.h \
	iss/DecodeCacheSupport.h \
	iss/BlockHash.h \
	iss/SingleFunctionInliner.h \
	iss/Coroutine.h \
	iss/DynLoad.h \
	iss/Plugin.h \
	iss/SysCallMgr.h \
	iss/SysCallHandler.h \
	iss/SysCallExtHandler.h \
	iss/EventBusExtHandler.h \
	iss/ExecSupport.h \
	iss/ThreadedKernel.h \
	iss/RunControlIface.h \
	iss/TraceRunControl.h \
	iss/ScriptingIface.h \
	iss/ScriptLogger.h \
	iss/ScriptLoggerImpl.h \
	iss/ThreadedSimLock.h \
	iss/Memory.h \
	iss/AlignedAccess.h \
	iss/DefaultMem.h \
	iss/GlobalMemoryName.h \
	iss/DefaultExternMem.h \
	iss/GlobalMemSupport.h \
	iss/Introspection.h \
	iss/MemoryInterface.h \
	iss/TypeIO.h \
	iss/TargetHash.h \
	iss/SimInterface.h \
	iss/Reader.h \
	iss/Writer.h \
	iss/WriterUtils.h \
	iss/MiscWriter.h \
	iss/Token.h \
	iss/ReportExtras.h \
	iss/UvpReader.h \
	iss/DatReader.h \
	iss/DatWriter.h \
	iss/FastDatWriter.h \
	iss/IHexReader.h \
	iss/ElfParser.h \
	iss/ElfBaseReader.h \
	iss/ElfReader.h \
	iss/DatParser.h \
	iss/UvpParser.h \
	iss/UvpParseTypes.h \
	iss/UvpFlexLexer.h \
    iss/TarmacReader.h \
    iss/WaveformThread.h \
	iss/UvpWriter.h \
	iss/StringHelpers.h \
	iss/NodeHelpers.h \
	iss/TargetHash.h \
	iss/LoggingMgr.h \
	iss/ThreadedLogger.h \
	iss/DbgInterface.h \
	iss/GdbServer.h \
	iss/RemoteGdbServer.h \
	iss/DbgRegMap.h \
	iss/MemoryLocks.h \
	iss/uvpparser.hh

EXTRA_DIST += \
	iss/logging_events.pm \
	iss/logging_events_utils.pm \
	iss/simple_logging_events \
	iss/trans_logging_events

issdir = $(includedir)/$(iss_pfx)

built = iss/LoggingMgr.C iss/TransLoggingMgr.C

if HAVE_BISON
built += iss/uvpparser.cc iss/uvpparser.hh 
endif

if HAVE_FLEX
built += iss/uvptokenizer.cc 
endif

BUILT_SOURCES += $(built)

CLEANFILES += $(built)

iss_includes = $(OccFlags) -I$(top_srcdir) -I$(top_builddir)/iss $(LEX_CFLAGS) $(RNUMBER_CFLAGS) $(TCL_INC_PATHS) $(BOOST_CPPFLAGS)


if HAVE_BISON
iss/uvptokenizer.cc : iss/uvpparser.cc
endif

if HAVE_FLEX
iss/uvpparser.hh : iss/uvpparser.cc
endif

uvptokenizer_lflag = -Cfe

# This generates the simple logging events used by the basic and time-tagged ISS.
iss/LoggingMgr.C: $(srcdir)/iss/simple_logging_events $(srcdir)/iss/logging_events.pm $(srcdir)/iss/logging_events_utils.pm
	perl $< $@

# This generates the queued logging events used by the transactional ISS.
iss/TransLoggingMgr.C: $(srcdir)/iss/trans_logging_events $(srcdir)/iss/logging_events.pm $(srcdir)/iss/logging_events_utils.pm
	perl $< $@
