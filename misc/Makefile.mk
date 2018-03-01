
DataFiles = \
	misc/adl.rnc \
	misc/adl.st \
	misc/xhtml1-transitional.dtd \
	misc/xhtml-lat1.ent \
	misc/xhtml-special.ent \
	misc/xhtml-symbol.ent \
	misc/xhtml-math11-f.dtd \
	misc/xhtml2fo.xsl \
	misc/xhtml2fo-adl.xsl

data_DATA += $(DataFiles)

EXTRA_DIST += $(DataFiles)

if !MINIMAL_BUILD

if LANG_FRONTEND

bin_PROGRAMS += misc/make-doc misc/make-db 

lib_LTLIBRARIES += \
	misc/libAdlDoc.la

noinst_HEADERS += \
	misc/dbgen.h \
	misc/DocWalker.h \
	misc/DocCfgParser.h \
	misc/DocHelpers.h



misc_includes = $(OccFlags) -I$(top_srcdir) $(RNUMBER_CFLAGS)

misc_ldadd = \
	$(Fe)/libAdlFrontEnd.la \
	$(Helpers)/libAdlHelpers.la \
	$(Helpers)/libAdlSupport.la \
	$(Helpers)/libAdlLogUsage.la \
	$(RNUMBER_LTLIBS) \
	$(OccLibs)/libocc.la \
	$(OccLibs)/libgc.la


misc_libAdlDoc_la_SOURCES = \
	misc/DocWalker.C \
	misc/DocCfgParser.C \
	misc/DocHelpers.C

misc_libAdlDoc_la_CXXFLAGS = $(misc_includes)


misc_make_doc_SOURCES = \
	misc/docgen.C

misc_make_doc_CXXFLAGS = $(misc_includes)

misc_make_doc_LDADD = \
	misc/libAdlDoc.la \
	$(misc_ldadd)


misc_make_db_SOURCES = \
	misc/dbgen.C

misc_make_db_CXXFLAGS = $(misc_includes)

misc_make_db_LDADD = \
	misc/libAdlDoc.la \
	$(misc_ldadd)

endif

endif
