#include "Atr/Assertions/Assert.h"
#include "Atr/Platform/File.h"

#include <cstdio>
#include <memory>
#include <sys/stat.h>

namespace Atr::Platform::File
{
  b8 Exists(const char *Path)
  {
    ATR_ASSERT(Path != nullptr, "Path must not be null");

    FILE *File = fopen(Path, "r");

    if (File == nullptr)
    {
      return FALSE;
    }

    fclose(File);
    return TRUE;
  }

  b8 GetSize(const char *Path, u64 *OutSize)
  {
    ATR_ASSERT(Path != nullptr, "Path must not be null");
    ATR_ASSERT(OutSize != nullptr, "OutSize must not be null");

    FILE *File = fopen(Path, "r");

    if (File == nullptr)
    {
      *OutSize = 0;
      return FALSE;
    }

    fseek(File, 0, SEEK_END);
    *OutSize = ftell(File);
    fseek(File, 0, SEEK_SET);

    fclose(File);
    return TRUE;
  }

  b8 GetModificationTime(const char *Path, u64 *OutTime)
  {
    ATR_ASSERT(Path != nullptr, "Path must not be null");
    ATR_ASSERT(OutTime != nullptr, "OutTime must not be null");

    struct stat file_stat = {};

    stat(Path, &file_stat);
    *OutTime = file_stat.st_mtime;

    return TRUE;
  }

  b8 Create(const char *Path)
  {
    ATR_ASSERT(Path != nullptr, "Path must not be null");

    FILE *File = fopen(Path, "w");

    if (File == nullptr)
    {
      return FALSE;
    }

    fclose(File);
    return TRUE;
  }

  b8 Read(const char *Path, void *OutBuffer, u64 *OutBufferSize)
  {
    ATR_ASSERT(Path != nullptr, "Path must not be null");
    ATR_ASSERT(OutBuffer != nullptr, "OutBuffer must not be null");
    ATR_ASSERT(OutBufferSize != nullptr, "OutBufferSize must not be null");

    *OutBufferSize = 0;
    FILE *File = fopen(Path, "rb");

    if (File == nullptr)
    {
      return FALSE;
    }

    fseek(File, 0, SEEK_END);
    *OutBufferSize = ftell(File);
    fseek(File, 0, SEEK_SET);

    memset(OutBuffer, 0, *OutBufferSize + 1);
    fread(OutBuffer, sizeof(u8), *OutBufferSize, File);

    fclose(File);
    return TRUE;
  }

  b8 Read(const char *Path, void *OutBuffer, u64 *OutBufferSize, LinearAllocator *Allocator)
  {
    ATR_ASSERT(Path != nullptr, "Path must not be null");
    ATR_ASSERT(OutBufferSize != nullptr, "OutBufferSize must not be null");
    ATR_ASSERT(Allocator != nullptr, "Allocator must not be null");

    void *File = 0;

    if (GetSize(Path, OutBufferSize))
    {
      void *Buffer = Allocator->Alloc(*OutBufferSize + 1);

      return Read(Path, Buffer, OutBufferSize);
    }

    *OutBufferSize = 0;
    OutBuffer = File;

    return FALSE;
  }

  b8 Write(const char *Path, const void *Buffer, u64 BufferSize)
  {
    ATR_ASSERT(Path != nullptr, "Path must not be null");
    ATR_ASSERT(Buffer != nullptr, "Buffer must not be null");
    ATR_ASSERT(BufferSize != 0, "BufferSize must not be null");

    FILE *File = fopen(Path, "wb");

    if (File == nullptr)
    {
      return FALSE;
    }

    fwrite(Buffer, sizeof(u8), BufferSize, File);

    fclose(File);
    return TRUE;
  }

  b8 Append(const char *Path, const void *Buffer, u64 BufferSize)
  {
    ATR_ASSERT(Path != nullptr, "Path must not be null");
    ATR_ASSERT(Buffer != nullptr, "Buffer must not be null");
    ATR_ASSERT(BufferSize != 0, "BufferSize must not be null");

    FILE *File = fopen(Path, "ab");

    if (File == nullptr)
    {
      return FALSE;
    }

    fwrite(Buffer, sizeof(u8), BufferSize, File);

    fclose(File);
    return TRUE;
  }
}