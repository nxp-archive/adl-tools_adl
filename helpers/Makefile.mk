

lib_LTLIBRARIES += helpers/libIntbv.la helpers/libIntbvRnum.la helpers/libAdlSupport.la  helpers/libAdlLogUsage.la

if !MINIMAL_BUILD

if LANG_FRONTEND

lib_LTLIBRARIES += helpers/libAdlHelpers.la

endif

endif

helpersdir = $(includedir)/$(helpers_pfx)

helpers_includes = $(PLASMA_CFLAGS) -I$(top_srcdir) $(RNUMBER_CFLAGS)


helpers_libAdlHelpers_la_SOURCES = \
	helpers/RtlWalker.C \
	helpers/CodeGen.C

helpers_libAdlHelpers_la_CPPFLAGS = $(helpers_includes)


helpers_libIntbv_la_SOURCES = \
	helpers/intbv.C

helpers_libIntbv_la_CPPFLAGS = $(helpers_includes)


helpers_libIntbvRnum_la_SOURCES = \
	helpers/intbv_rnumber.C

helpers_libIntbvRnum_la_CPPFLAGS = $(helpers_includes)

helpers_libIntbvRnum_la_LIBADD = $(RNUMBER_LTLIBS)
helpers_libIntbvRnum_la_CXXFLAGS = $(RNUMBER_CFLAGS)


helpers_libAdlSupport_la_SOURCES = \
	helpers/AnyOption.C \
	helpers/FilenameUtils.C \
	helpers/pfxstream.C

helpers_libAdlSupport_la_CPPFLAGS = $(helpers_includes)


helpers_libAdlLogUsage_la_SOURCES = \
	helpers/LogUsage.C

helpers_libAdlLogUsage_la_CPPFLAGS = $(helpers_includes)


helpers_HEADERS = \
	helpers/intbv_base.h \
	helpers/intbv.h \
	helpers/intbv_impl.h \
	helpers/sintbv.h \
	helpers/le_intbv.h \
	helpers/le_sintbv.h \
	helpers/bits.h \
	helpers/bits_impl.h \
	helpers/stringfwd.h \
	helpers/stringhash.h \
	helpers/pfxstream.h \
	helpers/Macros.h \
	helpers/StringUtils.h \
	helpers/BasicTypes.h \
	helpers/gc_hash_map.h \
	helpers/gc_hash_set.h \
	helpers/gc_map.h \
	helpers/gc_set.h \
	helpers/gc_vector.h \
	helpers/gc_list.h \
	helpers/gc_string.h \
	helpers/gc_destruct.h \
	helpers/AnyOption.h \
	helpers/LogUsage.h \
	helpers/BitTwiddles.h \
	helpers/AdlTranslatingWalker.h \
	helpers/TypeConv.h  \
	helpers/FilenameUtils.h

noinst_HEADERS = \
	helpers/fe_exceptions.h \
	helpers/RtlWalker.h \
	helpers/CodeGen.h






