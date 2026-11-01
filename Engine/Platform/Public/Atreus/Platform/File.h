#pragma once

#include "Atreus/Types.h"

struct FileHandle
{
  void *handle;
};

inline bool operator==(FileHandle a, FileHandle b)
{
  return a.handle == b.handle;
}

constexpr FileHandle INVALID_FILE_HANDLE = {nullptr};

enum class FileAccessMode : u8
{
  Read = 0,
  Write,
  ReadWrite,
};

enum class FileOpenMode : u8
{
  OpenExisting = 0,
  CreateNew,
  CreateAlways,
  OpenAlways,
};

enum class FileSeekOrigin : u8
{
  Begin = 0,
  Current,
  End,
};

ATR_API bool FileExists(const char *path);

ATR_API bool DirectoryCreate(const char *path);

ATR_API bool FileGetSize(FileHandle handle, u64 *out_size);

ATR_API bool
FileOpen(const char *path, FileAccessMode access, FileOpenMode mode, FileHandle *out_handle);

ATR_API void FileClose(FileHandle *handle);

ATR_API bool FileRead(
    FileHandle     handle,
    FileSeekOrigin origin,
    i64            distance,
    void          *buffer,
    u64            size,
    u64           *out_bytes_read
);

ATR_API bool FileReadToString(
    FileHandle     handle,
    FileSeekOrigin origin,
    i64            distance,
    char          *string,
    u64            string_size,
    u64           *out_bytes_read
);

ATR_API bool FileWrite(
    FileHandle     handle,
    FileSeekOrigin origin,
    i64            distance,
    void          *buffer,
    u64            size,
    u64           *out_bytes_written
);

ATR_API void FileFlush(FileHandle handle);