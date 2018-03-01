//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
//
// A simple common interface for memory mapped files.  This is a very simple interface
// we're only supporting read-only file access.
//

#ifndef _MMAPFILES_H_
#define _MMAPFILES_H_

#include <string.h>

#include "helpers/BasicTypes.h"

extern const int PAGE_SIZE;

// Size of map used in incremental scanning.
extern const uint64_t MAP_SIZE;

#if defined(_MSC_VER) || defined(__MINGW32__)

#undef UNICODE
#include <windows.h>
typedef HANDLE MmapHandle;

#else

typedef int MmapHandle;

#endif

// A default value for a handle indicating a problem.
MmapHandle default_handle_value();

// Open a memory-mapped file.  Throws a runtime_error if there's a problem.
MmapHandle open_mmap_file(const char *file,const char* &base,off_t &bytes_mapped,size_t &size);

// Re-map a file at the given offset.
const char *remap_mmap_file(MmapHandle fd,const char *base,off_t offset,off_t old_bytes_mapped,off_t new_bytes_mapped);

// Close a memory-mapped file.
void close_mmap_file(MmapHandle fd,const char *base,off_t bytes_mapped);


#endif

