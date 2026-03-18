#pragma once

#include "Atr/Definitions.h"
#include "Atr/Platform/Types/FileHandle.h"
#include "Atr/Platform/Types/FileMode.h"

// TODO:
// This interface is very basic and only minimal. Expand this in the future. Implementations
// might also need work to be more robust and support more features.

namespace Atr::Platform
{
  b8 FileExists(const char *Path);
  b8 FileSize(FileHandle File, u64 *OutSize);

  b8 FileCreate(const char *Path, FileHandle *OutFile);
  b8 FileOpen(const char *Path, FileMode Mode, FileHandle *OutFile);
  b8 FileClose(FileHandle *File);

  b8 FileRead(FileHandle File, void *Buffer, u64 Size, u64 *BytesRead);
  b8 FileWrite(FileHandle File, const void *Buffer, u64 Size, u64 *BytesWritten);
}