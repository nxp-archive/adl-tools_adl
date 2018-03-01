
## Please update this variable if any new macros are created

dist_MACROS = \
		macros/general.m4 \
		macros/adl.m4

MACROS = \
	macros/boost.m4 \
	macros/bison.m4 \
	macros/coro.m4 \
	macros/flex.m4 \
	macros/libtool.m4 \
	macros/readline.m4 \
	macros/skribe.m4 \
	macros/tcl.m4 \
	macros/lz4.m4 \
	macros/cpp-setup.m4 \
	macros/plasma.m4 \
	macros/rnumber.m4 \
	macros/swig.m4 \
	macros/systemc.m4 \
	macros/prefix.m4 \
	macros/usagelog.m4 \
	$(dist_MACROS)

EXTRA_DIST += $(MACROS)

m4datadir = $(datadir)/aclocal
m4data_DATA = $(dist_MACROS)
