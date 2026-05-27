#pragma once

#include "Allocators.h"
#include "Definitions.h"

namespace Atr
{
  struct FileHandle
  {
    u64 value;
  };

  constexpr FileHandle InvalidFileHandle = {0};

  enum class FileMode : u8
  {
    READ,
    WRITE,
    APPEND
  };

  b8 FileExists(const char *path);
  b8 GetFileSize(const char *path, u64 *out_size);
  b8 GetFileModTime(const char *path, u64 *out_time);

  b8 CreateFile(const char *path);

  b8 OpenFile(const char *path, FileMode mode, FileHandle *out_handle);
  b8 CloseFile(FileHandle *handle);

  b8 ReadFile(FileHandle *handle, void *out_buffer, u64 *out_buffer_size);
  b8 ReadFile(FileHandle *handle, void *out_buffer, u64 *out_buffer_size,
              LinearAllocator *allocator);
  b8 WriteFile(FileHandle *handle, void *buffer, u64 buffer_size);
}