
if !MINIMAL_BUILD 

if LANG_FRONTEND

bin_PROGRAMS += asm/make-asm

lib_LTLIBRARIES += asm/libAdlAsm.la


asm_includes = $(OccFlags) -I$(top_srcdir) $(RNUMBER_CFLAGS)


asm_libAdlAsm_la_SOURCES = asm/GenAsm.C

asm_libAdlAsm_la_CPPFLAGS = $(asm_includes)

asm_make_asm_SOURCES = \
	asm/main.C

asm_make_asm_CPPFLAGS = $(asm_includes)

asm_make_asm_LDADD = \
	asm/libAdlAsm.la \
	$(Fe)/libAdlFrontEnd.la \
	$(Helpers)/libAdlHelpers.la \
	$(Helpers)/libAdlSupport.la \
	$(Helpers)/libAdlLogUsage.la \
	$(RNUMBER_LTLIBS) \
	$(OccLibs)/libocc.la \
	$(OccLibs)/libgc.la


noinst_HEADERS += \
	asm/GenAsm.h

asmdir = $(includedir)/$(asm_pfx)

asm_HEADERS = \
	asm/disassembler.h

endif LANG_FRONTEND

endif !MINIMAL_BUILD 
