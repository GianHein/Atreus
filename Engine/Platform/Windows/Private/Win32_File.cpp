#include "Atr/Platform/File.h"

#include <cstdio>
#include <sys/stat.h>

// At this moment, I cba to implement all this as platform-specific
// code. Will only refactor this when necessary.

namespace Atr::Platform
{
  b8 FileExists(const char *Path)
  {
    // TODO: ATR_ASSERT(Path, "File path not provided");

    FILE *File = fopen(Path, "rb");

    if (!File)
    {
      return FALSE;
    }

    fclose(File);
    return TRUE;
  }

  b8 FileSize(const char *Path, u32 *OutSize)
  {
    // TODO: ATR_ASSERT(Path, "File path not provided");

    u32 FileSize = 0;

    FILE *File = fopen(Path, "rb");

    if (!File)
    {
      *OutSize = 0;
      // ATR_ERROR("Failed to open file '%s'", Path);
      return FALSE;
    }

    fseek(File, 0, SEEK_END);
    FileSize = ftell(File);
    fseek(File, 0, SEEK_SET);
    fclose(File);

    *OutSize = FileSize;
    return TRUE;
  }

  b8 FileModificationTime(const char *Path, u64 *OutTime)
  {
    struct stat file_stat = {};
    stat(Path, &file_stat);

    *OutTime = file_stat.st_mtime;
    return TRUE;
  }

  b8 FileRead(const char *Path, u32 *OutSize, void *OutBuffer)
  {
    // TODO: ATR_ASSERT(Path, "File path not provided");
    // TODO: ATR_ASSERT(FileSize, "File size not provided");

    *OutSize = 0;

    FILE *File = fopen(Path, "rb");

    if (!File)
    {
      // ATR_ERROR("Failed to open file '%s'", Path);
      return FALSE;
    }

    fseek(File, 0, SEEK_END);
    *OutSize = ftell(File);
    fseek(File, 0, SEEK_SET);

    memset(OutBuffer, 0, *OutSize + 1);
    fread(OutBuffer, sizeof(char), *OutSize, File);
    fclose(File);

    return TRUE;
  }

  b8 FileRead(const char *Path, u32 *OutSize, LinearAllocator *Allocator, void *OutBuffer)
  {
    // TODO: ATR_ASSERT(Path, "File path not provided");
    // TODO: ATR_ASSERT(FileSize, "File size not provided");

    char *File = 0;
    u32 Size;
    ::Atr::Platform::FileSize(Path, &Size);

    if (Size)
    {
      char *Buffer = (char *)Allocator->alloc(Size + 1);

      if (::Atr::Platform::FileRead(Path, OutSize, File))
      {
        OutBuffer = File;
        return TRUE;
      }
    }

    return FALSE;
  }

  b8 FileWrite(const char *Path, const void *Buffer, u32 BufferSize)
  {
    // TODO: ATR_ASSERT(Path, "File path not provided");
    // TODO: ATR_ASSERT(Buffer, "Buffer not provided");

    FILE *File = fopen(Path, "rb");

    if (!File)
    {
      // ATR_ERROR("Failed to open file '%s'", Path);
      return FALSE;
    }

    fwrite(Buffer, sizeof(char), BufferSize, File);
    fclose(File);
    return TRUE;
  }

  b8 FileCopy(const char *OldPath, const char *NewPath, void *Buffer)
  {
    u32 FileSize = 0;
    void *Data;
    ::Atr::Platform::FileRead(OldPath, &FileSize, Buffer);

    FILE *OutFile = fopen(NewPath, "wb");

    if (!OutFile)
    {
      // ATR_ERROR("Failed to open file '%s'", OutPath);
      return FALSE;
    }

    u32 Result = fwrite(Data, sizeof(char), FileSize, OutFile);

    if (!Result)
    {
      // ATR_ERROR("Failed to open file '%s'", OutPath);
      return FALSE;
    }

    fclose(OutFile);
    return TRUE;
  }

  b8 FileCopy(const char *OldPath, const char *NewPath, LinearAllocator *Allocator)
  {
    void *File = 0;
    u32 Size;
    ::Atr::Platform::FileSize(OldPath, &Size);

    if (Size)
    {
      void *Buffer = Allocator->alloc(Size + 1);
      return ::Atr::Platform::FileCopy(OldPath, NewPath, Buffer);
    }

    return FALSE;
  }
}