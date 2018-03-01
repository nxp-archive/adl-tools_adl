
if !MINIMAL_BUILD 

lib_LTLIBRARIES += graphite/libAdlGraphite.la

graphite_libAdlGraphite_la_SOURCES = \
        graphite/GraphiteInstr.C \
				graphite/GraphiteCore.C

endif !MINIMAL_BUILD 

graphite_HEADERS = \
	graphite/GraphiteSupport.h \
	graphite/GraphiteInstr.h   \
	graphite/GraphiteCore.h    \
	graphite/GraphiteTime.h

graphitedir = $(includedir)/$(graphite_pfx)


