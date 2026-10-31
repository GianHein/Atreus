#include "Atreus/Platform/File.h"

#include "Atreus/Log.h"

#if !defined(WIN32_LEAN_AND_MEAN)
#  define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

bool FileExists(const char *path)
{
  DWORD attributes = GetFileAttributesA(path);
  return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}

bool DirectoryCreate(const char *path)
{
  if (CreateDirectoryA(path, nullptr)) return true;

  DWORD attributes = GetFileAttributesA(path);
  if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY)) return true;

  ATR_LOG_ERROR("Failed to create directory '%s' using CreateDirectoryA: %lu", path, GetLastError());
  return false;
}

bool FileGetSize(FileHandle handle, u64 *out_size)
{
  LARGE_INTEGER size;
  if (!GetFileSizeEx((HANDLE)handle.handle, &size))
  {
    ATR_LOG_ERROR(
        "Platform::File", "Failed to get file size using GetFileSizeEx: %lu", GetLastError()
    );
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
  }

  ATR_UNREACHABLE();
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
  }

  ATR_UNREACHABLE();
}

static bool ToWin32SeekOrigin(HANDLE file, FileSeekOrigin origin, i64 distance)
{
  LARGE_INTEGER to_move;
  to_move.QuadPart = distance;

  switch (origin)
  {
  case FileSeekOrigin::Begin:
    return SetFilePointerEx(file, to_move, NULL, FILE_BEGIN) != 0;

  case FileSeekOrigin::Current:
    return SetFilePointerEx(file, to_move, NULL, FILE_CURRENT) != 0;

  case FileSeekOrigin::End:
    return SetFilePointerEx(file, to_move, NULL, FILE_END) != 0;
  }

  ATR_UNREACHABLE();
}

bool FileOpen(const char *path, FileAccessMode access, FileOpenMode mode, FileHandle *out_handle)
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
    *out_handle = INVALID_FILE_HANDLE;

    ATR_LOG_ERROR(
        "Platform::File", "Failed to open file '%s' through CreateFileA: %lu", path, GetLastError()
    );
    return false;
  }

  out_handle->handle = handle;

  ATR_LOG_INFO("Platform::File", "Opened file '%s'", path);
  return true;
}

void FileClose(FileHandle *handle)
{
  ATR_ASSERT(handle->handle != nullptr, "Cannot close file using an emtpy handle");
  CloseHandle((HANDLE)handle->handle);
  *handle = INVALID_FILE_HANDLE;
}

bool FileRead(
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
    ATR_LOG_ERROR(
        "Platform::File", "Failed to SeekOrigin using SetFilePointerEx: %lu", GetLastError()
    );
    return false;
  }

  if (!ReadFile((HANDLE)handle.handle, buffer, size, &bytes_read, NULL))
  {
    ATR_LOG_ERROR("Platform::File", "Could not read file data using ReadFile: %lu", GetLastError());
    return false;
  }

  if (out_bytes_read) *out_bytes_read = bytes_read;

  return true;
}

bool FileWrite(
    FileHandle     handle,
    FileSeekOrigin origin,
    i64            distance,
    void          *buffer,
    u64            size,
    u64           *out_bytes_written
)
{
  ATR_ASSERT(handle.handle != nullptr, "Cannot write using empty file handle");

  DWORD bytes_written = 0;

  if (!ToWin32SeekOrigin((HANDLE)handle.handle, origin, distance))
  {
    ATR_LOG_ERROR(
        "Platform::File", "Failed to SeekOrigin using SetFilePointerEx: %lu", GetLastError()
    );
    return false;
  }

  if (!WriteFile((HANDLE)handle.handle, buffer, size, &bytes_written, NULL))
  {
    ATR_LOG_ERROR(
        "Platform::File", "Could not write file data using WriteFile: %lu", GetLastError()
    );
    return false;
  }

  if (out_bytes_written) *out_bytes_written = bytes_written;

  return true;
}

void FileFlush(FileHandle handle)
{
  ATR_ASSERT(handle.handle != nullptr, "Cannot flush using empty file handle");

  if (!FlushFileBuffers((HANDLE)handle.handle))
    ATR_LOG_ERROR(
        "Platform::File", "Failed to flush file using FlushFileBuffers: %lu", GetLastError()
    );
}