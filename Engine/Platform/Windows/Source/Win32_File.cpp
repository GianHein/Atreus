#include "Platform/File.h"

#include <cstdio>
#include <memory>
#include <sys/stat.h>

namespace Atr
{
  b8 FileExists(const char *path)
  {
    FILE *file = fopen(path, "r");

    if (file == nullptr)
    {
      return FALSE;
    }

    fclose(file);
    return TRUE;
  }

  b8 GetFileSize(const char *path, u64 *out_size)
  {
    FILE *file = fopen(path, "r");

    if (!file)
    {
      return FALSE;
    }

    fseek(file, 0, SEEK_END);
    *out_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    fclose(file);
    return TRUE;
  }

  b8 GetFileModTime(const char *path, u64 *out_time)
  {
    struct stat file_stat = {};

    stat(path, &file_stat);
    *out_time = file_stat.st_mtime;

    return TRUE;
  }

  b8 CreateFile(const char *path)
  {
    FILE *file = fopen(path, "w");

    if (file == nullptr)
    {
      return FALSE;
    }

    fclose(file);
    return TRUE;
  }

  b8 OpenFile(const char *path, FileMode mode, FileHandle *out_handle)
  {
    // READ, WRITE, APPEND
    // (implicitly support reading for both WRITE and APPEND)
    static const char *mode_strings[3] = {"rb", "wb", "ab"};
    const char *file_mode = mode_strings[(u8)mode];

    FILE *file = fopen(path, file_mode);

    if (file == nullptr)
    {
      return FALSE;
    }

    out_handle->value = (u64)file;
    return TRUE;
  }

  b8 CloseFile(FileHandle *handle)
  {
    if (handle->value == 0)
    {
      return FALSE;
    }

    FILE *file = (FILE *)handle->value;

    fclose(file);
    return TRUE;
  }

  b8 ReadFile(FileHandle *handle, void *out_buffer, u64 *out_buffer_size)
  {
    if (handle->value == 0)
    {
      *out_buffer_size = 0;
      return FALSE;
    }

    *out_buffer_size = 0;
    FILE *file = (FILE *)handle->value;

    fseek(file, 0, SEEK_END);
    *out_buffer_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    memset(out_buffer, 0, *out_buffer_size + 1);
    fread(out_buffer, sizeof(u8), *out_buffer_size, file);

    return TRUE;
  }

  b8 ReadFile(FileHandle *handle, void *out_buffer, u64 *out_buffer_size,
              LinearAllocator *allocator)
  {
    if (handle->value == 0)
    {
      *out_buffer_size = 0;
      return FALSE;
    }

    FILE *file = (FILE *)handle->value;

    fseek(file, 0, SEEK_END);
    *out_buffer_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    void *buffer = allocator->Allocate(*out_buffer_size + 1);
    return ReadFile(handle, buffer, out_buffer_size);
  }

  b8 WriteFile(FileHandle *handle, void *buffer, u64 buffer_size)
  {
    if (handle->value == 0)
    {
      return FALSE;
    }

    FILE *file = (FILE *)handle->value;

    fwrite(buffer, sizeof(u8), buffer_size, file);

    return TRUE;
  }
}