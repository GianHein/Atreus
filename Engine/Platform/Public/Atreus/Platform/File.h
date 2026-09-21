#pragma once

#include "Atreus/Definitions.h"

namespace Atreus
{
  struct FileHandle
  {
    void *handle = nullptr;
  };

  constexpr FileHandle InvalidFileHandle = {nullptr};

  enum class FileAccessMode : u8
  {
    Read = 0,
    Write,
    ReadWrite
  };

  enum class FileOpenMode : u8
  {
    OpenExisting = 0,
    CreateAlways,
    CreateNew,
    OpenAlways
  };

  enum class FileSeekOrigin : u8
  {
    Begin = 0,
    Current,
    End
  };

  b8 FileExists(const char *path);
  b8 GetFileSize(FileHandle handle, u64 *out_size);

  b8   OpenFile(const char *path, FileAccessMode access, FileOpenMode mode, FileHandle *out_handle);
  void CloseFile(FileHandle *handle);

  b8 ReadFileBytes(
      FileHandle     handle,
      FileSeekOrigin origin,
      i64            distance,
      void          *buffer,
      u64            size,
      u64           *out_bytes_read
  );

  b8 WriteFileBytes(
      FileHandle     handle,
      FileSeekOrigin origin,
      i64            distance,
      const void    *buffer,
      u64            size,
      u64           *out_bytes_written
  );

  void FlushFile(FileHandle handle);
}