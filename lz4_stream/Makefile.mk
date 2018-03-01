
if HAVE_LZ4

lib_LTLIBRARIES += \
	lz4_stream/liblz4stream.la

lz4_stream_liblz4stream_la_SOURCES = \
	lz4_stream/lz4_input_stream.cpp \
	lz4_stream/lz4_output_stream.cpp

lz4_stream_liblz4stream_la_CXXFLAGS = $(LZ4_CFLAGS)

lz4_streamdir = $(includedir)/$(lz4_pfx)

lz4_stream_HEADERS = \
	lz4_stream/lz4_stream.h

endif
