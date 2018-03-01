
if !MINIMAL_BUILD 

if LANG_FRONTEND

bin_PROGRAMS += fe/adl-dump

lib_LTLIBRARIES += fe/libAdlFrontEnd.la

endif LANG_FRONTEND

endif !MINIMAL_BUILD

fedir = $(includedir)/$(fe_pfx)

fe_includes = $(PLASMA_CFLAGS) -I$(top_srcdir) $(RNUMBER_CFLAGS)

fe_adl_dump_SOURCES = \
	fe/adl-dump.C

fe_adl_dump_CPPFLAGS = $(fe_includes)

fe_libAdlFrontEnd_la_SOURCES = \
	fe/BaseParser.C \
	fe/BaseMsg.C \
	fe/AdlParser.C \
	fe/SimpleParser.C \
	fe/AdlWalker.C \
	fe/Data.C \
	fe/Handler.C \
	fe/Handlers.C \
	fe/SimpleHandler.C \
	fe/VarWalker.C \
	fe/ResWalker.C \
	fe/Helpers.C \
	fe/InstrTable.C \
	fe/MsgLog.C \
	fe/Types.C \
	fe/FieldSupport.C \
	fe/DecodeSupport.C \
	fe/SyntaxWalkers.C \
	fe/IFieldWalker.C

fe_libAdlFrontEnd_la_CPPFLAGS = $(fe_includes)

fe_adl_dump_LDADD = \
	$(Fe)/libAdlFrontEnd.la \
	$(Helpers)/libAdlSupport.la \
	$(Helpers)/libAdlHelpers.la \
	$(RNUMBER_LTLIBS) \
	$(OccLibs)/libocc.la \
	$(OccLibs)/libgc.la


fe_HEADERS = \
	fe/AdlParser.h \
	fe/Helpers.h \
	fe/ParsingHelpers.h \
	fe/Data.h \
	fe/Types.h \
	fe/MsgLog.h \
	fe/FieldSupport.h \
	fe/DecodeSupport.h

noinst_HEADERS += \
	fe/BaseParser.h \
	fe/BaseMsg.h \
	fe/SimpleParser.h \
	fe/AdlWalker.h \
	fe/Handler.h \
	fe/Handlers.h \
	fe/SimpleHandler.h \
	fe/VarWalker.h \
	fe/ResWalker.h \
	fe/InstrTable.h \
	fe/SyntaxWalkers.h \
	fe/IFieldWalker.h \
	fe/ExprWalker.h
