#pragma once

#include "Atr/Definitions.h"
#include "Atr/Memory/LinearAllocator.h"
#include "Atr/Platform/Common/File.h"

namespace Atr::Platform::File
{
  b8 Exists(const char *Path);
  b8 GetSize(const char *Path, u64 *OutSize);
  b8 GetModificationTime(const char *Path, u64 *OutTime);

  b8 Create(const char *Path);

  b8 Read(const char *Path, void *OutBuffer, u64 *OutBufferSize);
  b8 Read(const char *Path, void *OutBuffer, u64 *OutBufferSize, LinearAllocator *Allocator);
  b8 Write(const char *Path, const void *Buffer, u64 BufferSize);
  b8 Append(const char *Path, const void *Buffer, u64 BufferSize);
}