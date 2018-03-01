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

#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <sstream>
#include <errno.h>

#if !defined(_MSC_VER) && !defined(__MINGW32__)
# include <sys/mman.h>
# include <unistd.h>
#endif

#include "helpers/Macros.h"

#include "MmapFiles.h"

using namespace std;

#if defined(_MSC_VER) || defined(__MINGW32__)
const int PAGE_SIZE = 4096;
#else
const int PAGE_SIZE = getpagesize(); //usually 4KB
#endif

//Size of map used in incremental scanning
const uint64_t MAP_SIZE  =  1024*PAGE_SIZE; //4MB

// A default value for a handle indicating a problem.
MmapHandle default_handle_value()
{
#if defined(_MSC_VER) || defined(__MINGW32__)
  return 0;
#else
  return -1;
#endif
}

// Open a memory-mapped file.  Throws a runtime_error if there's a problem.
MmapHandle open_mmap_file(const char *file,const char* &base,off_t &bytes_mapped,size_t &size)
{
  MmapHandle fd;
#if defined(_MSC_VER) || defined(__MINGW32__)
  HANDLE fobj;
  if ( (fobj = CreateFile(file,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL)) == INVALID_HANDLE_VALUE) {
    RError("Can't open " << file << " for reading.");
  }
  size = GetFileSize(fobj,0);
  
  if (bytes_mapped) {
    bytes_mapped = (size < MAP_SIZE) ? size : MAP_SIZE;
  }
  if ( (fd = CreateFileMapping(fobj,NULL,PAGE_READONLY,0,0,NULL)) == NULL) {
    RError("Unable to map " << file << ".");
  }
  if ( (base = (char *)MapViewOfFile(fd,FILE_MAP_READ,0,0,bytes_mapped)) == NULL) {
    CloseHandle(fobj);
    RError("Unable to create a mapped view of " << file << ".");
  }
  if (!bytes_mapped) {
    bytes_mapped = size;
  }
#else
  struct stat statbuf;      
  if ( (fd = open(file,O_RDONLY )) < 0) {
    RError("Can't open " << file << " for reading.");
  }
  if (fstat(fd,&statbuf) < 0) {
    RError("Can't open " << file << " for reading.");
  }
  size = statbuf.st_size;
  if (!bytes_mapped) {
    bytes_mapped = size;
  } else {
    bytes_mapped = (size < MAP_SIZE) ? size : MAP_SIZE;
  }
  if ( (base = (char *)mmap(0,bytes_mapped,PROT_READ,MAP_SHARED,fd,0)) == (caddr_t)-1) {
    RError("mmap:  " << strerror(errno));
  }
#endif
  return fd;
}

// Re-map a file at the given offset.
const char *remap_mmap_file(MmapHandle fd,const char *base,off_t offset,off_t old_bytes_mapped,off_t new_bytes_mapped)
{
#if defined(_MSC_VER) || defined(__MINGW32__)
  UnmapViewOfFile((void*)base);
  if ( (base = (char *)MapViewOfFile(fd,FILE_MAP_READ,0,offset,new_bytes_mapped)) == NULL) {
    CloseHandle(fd);
    RError("Unable to remap at offset 0x" << hex << offset);
  }
#else
  if (munmap((void*)base,old_bytes_mapped) != 0) {
    RError("unmap (" << errno << "): " << strerror(errno));
  }

  if ( (base = (char *)mmap(0,new_bytes_mapped,PROT_READ,MAP_SHARED,fd,offset)) == (caddr_t)-1) {
    RError("mmap (" << errno << "): " << strerror(errno));
  }
#endif
  return base;
}

// Close a memory-mapped file.
void close_mmap_file(MmapHandle fd,const char *base,off_t bytes_mapped)
{
#if defined(_MSC_VER) || defined(__MINGW32__)
  UnmapViewOfFile((void*)base);
  CloseHandle(fd);
#else
  munmap((void*)base,bytes_mapped);
  close(fd);
#endif
}
