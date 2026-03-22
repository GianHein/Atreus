#pragma once

#include "Atr/Definitions.h"
#include "Atr/Memory/LinearAllocator.h"

// This really is absolute minimal File I/O, definitely needs to be
// extended at a later point!

namespace Atr::Platform
{
  b8 FileExists(const char *Path);
  b8 FileSize(const char *Path, u32 *OutSize);
  b8 FileModificationTime(const char *Path, u64 *OutTime);

  b8 FileRead(const char *Path, u32 *OutSize, void *OutBuffer);
  b8 FileRead(const char *Path, u32 *OutSize, LinearAllocator *Allocator, void *OutBuffer);
  b8 FileWrite(const char *Path, const void *Buffer, u32 BufferSize);

  b8 FileCopy(const char *OldPath, const char *NewPath, void *Buffer);
  b8 FileCopy(const char *OldPath, const char *NewPath, LinearAllocator *Allocator);
}