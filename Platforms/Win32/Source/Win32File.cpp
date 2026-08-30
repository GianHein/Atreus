#include "Atreus/Platform/File.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

namespace Atreus
{
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
      return 0;
    }
  }

  static DWORD ToWin32ShareMode(FileShareMode share)
  {
    switch (share)
    {
    case FileShareMode::Read:
      return FILE_SHARE_READ;
    case FileShareMode::Write:
      return FILE_SHARE_WRITE;
    case FileShareMode::ReadWrite:
      return FILE_SHARE_READ | FILE_SHARE_WRITE;
    default:
      return 0;
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
      return 0;
    }
  }

  b8 FileExists(const char *path)
  {
    DWORD attributes = GetFileAttributesA(path);
    return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
  }

  b8 OpenFile(
      const char    *path,
      FileAccessMode access,
      FileShareMode  share,
      FileOpenMode   mode,
      FileHandle    *out_handle
  )
  {
    HANDLE handle = CreateFileA(
        path,
        ToWin32DesiredAccess(access),
        ToWin32ShareMode(share),
        NULL,
        ToWin32CreationDisposition(mode),
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (handle == INVALID_HANDLE_VALUE)
    {
      *out_handle = InvalidFileHandle;
      return false;
    }

    out_handle->handle = handle;
    return true;
  }

  void CloseFile(FileHandle *handle)
  {
    CloseHandle((HANDLE)handle->handle);
    handle->handle = nullptr;
  }

  // TODO:
  // This won't work with files > 4 GiB due to the Win32
  // API using a DWORD (u32) for sizes here, revisit when it becomes
  // a problem! Applies to both ReadFile and WriteFile
  b8 ReadFile(
      FileHandle     handle,
      FileSeekOrigin origin,
      i64            distance,
      void          *buffer,
      u64            size,
      u64           *out_bytes_read
  )
  {
    if (handle.handle == nullptr) return false;

    DWORD bytes_read = 0;

    LARGE_INTEGER to_move;
    to_move.QuadPart = distance;

    switch (origin)
    {
    case FileSeekOrigin::Begin:
      SetFilePointerEx((HANDLE)handle.handle, to_move, NULL, FILE_BEGIN);
      break;
    case FileSeekOrigin::End:
      SetFilePointerEx((HANDLE)handle.handle, to_move, NULL, FILE_END);
      break;
    }

    if (!::ReadFile((HANDLE)handle.handle, buffer, size, &bytes_read, NULL)) return false;
    if (out_bytes_read) *out_bytes_read = bytes_read;

    return true;
  }

  // TODO:
  // This won't work with files > 4 GiB due to the Win32
  // API using a DWORD (u32) for sizes here, revisit when it becomes
  // a problem! Applies to both ReadFile and WriteFile
  b8 WriteFile(
      FileHandle     handle,
      FileSeekOrigin origin,
      i64            distance,
      const void    *buffer,
      u64            size,
      u64           *out_bytes_written
  )
  {
    if (handle.handle == nullptr) return false;

    DWORD bytes_written = 0;

    LARGE_INTEGER to_move;
    to_move.QuadPart = distance;

    switch (origin)
    {
    case FileSeekOrigin::Begin:
      SetFilePointerEx((HANDLE)handle.handle, to_move, NULL, FILE_BEGIN);
      break;
    case FileSeekOrigin::End:
      SetFilePointerEx((HANDLE)handle.handle, to_move, NULL, FILE_END);
      break;
    }

    if (!::WriteFile((HANDLE)handle.handle, buffer, size, &bytes_written, NULL)) return false;
    if (out_bytes_written) *out_bytes_written = bytes_written;

    return true;
  }

  b8 GetFileSize(FileHandle handle, u64 *out_size)
  {
    LARGE_INTEGER size;
    if (!GetFileSizeEx((HANDLE)handle.handle, &size)) return false;

    *out_size = (u64)size.QuadPart;
    return true;
  }
}