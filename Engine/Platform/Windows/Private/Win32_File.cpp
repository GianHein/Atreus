#include "Atr/Platform/File.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Atr::Platform
{
  b8 FileExists(const char *Path)
  {
    DWORD Attributes = GetFileAttributesA(Path);

    if (Attributes == INVALID_FILE_ATTRIBUTES)
    {
      return false;
    }

    return (Attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
  }

  b8 GetFileSize(FileHandle File, u64 *OutSize)
  {
    LARGE_INTEGER Size;

    if (!GetFileSizeEx((HANDLE)File.Value, &Size))
    {
      return false;
    }

    *OutSize = Size.QuadPart;
    return true;
  }

  b8 GetFileModificationTime(const char *Path, u64 *OutTime)
  {
    WIN32_FILE_ATTRIBUTE_DATA FileData;

    if (!GetFileAttributesExA(Path, GetFileExInfoStandard, &FileData))
    {
      return false;
    }

    FILETIME ModificationTime = FileData.ftLastWriteTime;
    ULARGE_INTEGER Time;

    Time.LowPart = ModificationTime.dwLowDateTime;
    Time.HighPart = ModificationTime.dwHighDateTime;

    // Convert to Unix timestamp
    SYSTEMTIME UnixEpoch = {1970, 1, 0, 1, 0, 0, 0, 0};
    FILETIME UnixEpochFileTime;
    SystemTimeToFileTime(&UnixEpoch, &UnixEpochFileTime);

    ULARGE_INTEGER UnixEpochTime;
    UnixEpochTime.LowPart = UnixEpochFileTime.dwLowDateTime;
    UnixEpochTime.HighPart = UnixEpochFileTime.dwHighDateTime;

    *OutTime = (Time.QuadPart - UnixEpochTime.QuadPart) / 10000000; // Convert to seconds

    return true;
  }

  b8 FileCreate(const char *Path, FileHandle *OutFile)
  {
    HANDLE FileHandle =
        CreateFileA(Path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (FileHandle == INVALID_HANDLE_VALUE)
    {
      return false; // Failed to create file
    }

    *OutFile = {(u64)FileHandle};
    return true;
  }

  b8 FileOpen(const char *Path, FileMode Mode, FileHandle *OutFile)
  {
    DWORD DesiredAccess = 0;
    DWORD CreationDisposition = 0;

    switch (Mode)
    {
    case FileMode::READ:
      DesiredAccess = GENERIC_READ;
      CreationDisposition = OPEN_EXISTING;
      break;
    case FileMode::WRITE:
      DesiredAccess = GENERIC_WRITE;
      CreationDisposition = CREATE_ALWAYS;
      break;
    case FileMode::READ_WRITE:
      DesiredAccess = GENERIC_READ | GENERIC_WRITE;
      CreationDisposition = OPEN_ALWAYS;
      break;
    case FileMode::APPEND:
      DesiredAccess = FILE_APPEND_DATA;
      CreationDisposition = OPEN_ALWAYS;
      break;
    default:
      return false; // Invalid mode
    }

    HANDLE FileHandle = CreateFileA(Path, DesiredAccess, 0, nullptr, CreationDisposition,
                                    FILE_ATTRIBUTE_NORMAL, nullptr);

    if (FileHandle == INVALID_HANDLE_VALUE)
    {
      return false; // Failed to open file
    }

    *OutFile = {(u64)FileHandle};
    return true;
  }

  b8 FileClose(FileHandle *File)
  {
    if (CloseHandle((HANDLE)File->Value))
    {
      File->Value = 0;
      return true;
    }

    return false;
  }

  b8 FileRead(FileHandle File, void *Buffer, u64 Size, u64 *BytesRead)
  {
    DWORD BytesReadLocal;
    if (!ReadFile((HANDLE)File.Value, Buffer, (DWORD)Size, &BytesReadLocal, nullptr))
    {
      return false; // Failed to read file
    }

    *BytesRead = BytesReadLocal;
    return true;
  }

  b8 FileWrite(FileHandle File, const void *Buffer, u64 Size, u64 *BytesWritten)
  {
    DWORD BytesWrittenLocal;
    if (!WriteFile((HANDLE)File.Value, Buffer, (DWORD)Size, &BytesWrittenLocal, nullptr))
    {
      return false; // Failed to write file
    }

    *BytesWritten = BytesWrittenLocal;
    return true;
  }
}