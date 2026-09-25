#include "Atreus/Platform/File.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "Atreus/Log.h"

#include <Windows.h>

namespace Atreus
{
  b8 FileExists(const char *path)
  {
    DWORD attributes = GetFileAttributesA(path);
    return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
  }

  b8 GetFileSize(FileHandle handle, u64 *out_size)
  {
    LARGE_INTEGER size;
    if (!GetFileSizeEx((HANDLE)handle.handle, &size))
    {
      ATR_LOG_ERROR("Failed to get file size using GetFileSizeEx: %lu", GetLastError());
      return false;
    }

    if (out_size) *out_size = (u64)size.QuadPart;
    return true;
  }

  static DWORD ToWin32DesiredAccess(FileAccessMode access)
  {
    switch (access)
    {
    case FileAccessMode::Read:
      return GENERIC_READ;

    case FileAccessMode::Write:
      return GENERIC_WRITE;

    case FileAccessMode::ReadWrite:
      return GENERIC_READ | GENERIC_WRITE;

    default:
    {
      ATR_ASSERT(
          false, "FileAccessMode fell through when trying to convert it to Win32DesiredAccess"
      );
      return 0;
    }
    }
  }

  static DWORD ToWin32CreationDisposition(FileOpenMode mode)
  {
    switch (mode)
    {
    case FileOpenMode::OpenExisting:
      return OPEN_EXISTING;

    case FileOpenMode::CreateAlways:
      return CREATE_ALWAYS;

    case FileOpenMode::CreateNew:
      return CREATE_NEW;

    case FileOpenMode::OpenAlways:
      return OPEN_ALWAYS;

    default:
    {
      ATR_ASSERT(
          false, "FileOpenMode fell through when trying to convert it to Win32CreationDisposition"
      );

      return 0;
    }
    }
  }

  static b8 ToWin32SeekOrigin(HANDLE file, FileSeekOrigin origin, i64 distance)
  {
    LARGE_INTEGER to_move;
    to_move.QuadPart = distance;

    switch (origin)
    {
    case FileSeekOrigin::Begin:
    {
      return SetFilePointerEx(file, to_move, NULL, FILE_BEGIN) != 0;
      break;
    }

    case FileSeekOrigin::Current:
    {
      return SetFilePointerEx(file, to_move, NULL, FILE_CURRENT) != 0;
      break;
    }

    case FileSeekOrigin::End:
    {
      return SetFilePointerEx(file, to_move, NULL, FILE_END) != 0;
      break;
    }

    default:
    {
      ATR_ASSERT(false, "FileSeekOrigin fell through when trying to convert it to Win32SeekOrigin");
      return false;
    }
    }
  }

  b8 OpenFile(const char *path, FileAccessMode access, FileOpenMode mode, FileHandle *out_handle)
  {
    HANDLE handle = CreateFileA(
        path,
        ToWin32DesiredAccess(access),
        FILE_SHARE_READ,
        NULL,
        ToWin32CreationDisposition(mode),
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (handle == INVALID_HANDLE_VALUE)
    {
      *out_handle = InvalidFileHandle;

      ATR_LOG_ERROR("Failed to open file '%s' through CreateFileA: %lu", path, GetLastError());
      return false;
    }

    out_handle->handle = handle;

    ATR_LOG_INFO("Opened file '%s'", path);
    return true;
  }

  void CloseFile(FileHandle *handle)
  {
    ATR_ASSERT(handle->handle != nullptr, "Cannot close a file using an empty handle");
    CloseHandle((HANDLE)handle->handle);
    *handle = InvalidFileHandle;
  }

  b8 ReadFileBytes(
      FileHandle     handle,
      FileSeekOrigin origin,
      i64            distance,
      void          *buffer,
      u64            size,
      u64           *out_bytes_read
  )
  {
    ATR_ASSERT(handle.handle != nullptr, "Cannot read using empty file handle");

    DWORD bytes_read = 0;

    if (!ToWin32SeekOrigin((HANDLE)handle.handle, origin, distance))
    {
      ATR_LOG_ERROR("Failed to SeekOrigin using SetFilePointerEx: %lu", GetLastError());
      return false;
    }

    if (!ReadFile((HANDLE)handle.handle, buffer, size, &bytes_read, NULL))
    {
      ATR_LOG_ERROR("Could not read file data using ReadFile: %lu", GetLastError());
      return false;
    }

    if (out_bytes_read) *out_bytes_read = bytes_read;

    return true;
  }

  b8 WriteFileBytes(
      FileHandle     handle,
      FileSeekOrigin origin,
      i64            distance,
      const void    *buffer,
      u64            size,
      u64           *out_bytes_written
  )
  {
    ATR_ASSERT(handle.handle != nullptr, "Cannot write using empty file handle");

    DWORD bytes_written = 0;

    if (!ToWin32SeekOrigin((HANDLE)handle.handle, origin, distance))
    {
      ATR_LOG_ERROR("Failed to SeekOrigin using SetFilePointerEx: %lu", GetLastError());
      return false;
    }

    if (!WriteFile((HANDLE)handle.handle, buffer, size, &bytes_written, NULL))
    {
      ATR_LOG_ERROR("Could not write file data using WriteFile: %lu", GetLastError());
      return false;
    }

    if (out_bytes_written) *out_bytes_written = bytes_written;

    return true;
  }

  void FlushFile(FileHandle handle)
  {
    if (handle.handle == nullptr) return;
    if (!FlushFileBuffers((HANDLE)handle.handle))
    {
      ATR_LOG_ERROR("Failed to flush file using FlushFileBuffers");
    }
  }
}