#include "Atreus/Platform/File.h"

#include "Atreus/Log.h"
#include "Atreus/Math.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

bool FileExists(const char *path)
{
  struct stat stat_buffer;

  if (stat(path, &stat_buffer) != 0) return false;
  return !S_ISDIR(stat_buffer.st_mode);
}

bool DirectoryCreate(const char *path)
{
  if (mkdir(path, 0775) == 0) return true;

  struct stat stat_buffer;
  if (errno == EEXIST && stat(path, &stat_buffer) == 0 && S_ISDIR(stat_buffer.st_mode))
    return true;

  ATR_LOG_ERROR("Failed to create directory '%s' using mkdir: %i", path, errno);
  return false;
}

bool FileGetSize(FileHandle handle, u64 *out_size)
{
  struct stat stat_buffer;
  i32         success = fstat((intptr_t)handle.handle, &stat_buffer);

  if (success != 0)
  {
    ATR_LOG_ERROR("Failed to get file size using fstat: %i", errno);
    return false;
  }

  *out_size = stat_buffer.st_size;
  return true;
}

static i32 ToUnixFlags(FileAccessMode access, FileOpenMode mode)
{
  i32 flags = 0;

  switch (access)
  {
  case FileAccessMode::Read:
    flags |= O_RDONLY;
    break;

  case FileAccessMode::Write:
    flags |= O_WRONLY;
    break;

  case FileAccessMode::ReadWrite:
    flags |= O_RDWR;
    break;
  }

  switch (mode)
  {
  case FileOpenMode::OpenExisting:
    flags |= 0;
    break;

  case FileOpenMode::CreateNew:
    flags |= O_CREAT | O_EXCL;
    break;

  case FileOpenMode::CreateAlways:
    flags |= O_CREAT | O_TRUNC;
    break;

  case FileOpenMode::OpenAlways:
    flags |= O_CREAT;
    break;

  default:
    break;
  }

  return flags;
}

bool FileOpen(const char *path, FileAccessMode access, FileOpenMode mode, FileHandle *out_handle)
{
  i32 handle = open(path, ToUnixFlags(access, mode), 0664);

  if (handle == -1)
  {
    *out_handle = INVALID_FILE_HANDLE;

    ATR_LOG_ERROR("Failed to open file '%s' through open: %i", path, errno);
    return false;
  }

  ATR_LOG_DEBUG("File opened: %s", path);
  out_handle->handle = (void *)(intptr_t)handle;
  return true;
}

void FileClose(FileHandle *handle)
{
  ATR_ASSERT(handle->handle != nullptr, "Cannot close file using an emtpy handle");
  close((intptr_t)handle->handle);
  *handle = INVALID_FILE_HANDLE;
}

static bool ToUnixOffset(i32 file_descriptor, FileSeekOrigin origin, i64 distance)
{
  off_t offset;

  switch (origin)
  {
  case FileSeekOrigin::Begin:
    offset = lseek(file_descriptor, distance, SEEK_SET);
    break;

  case FileSeekOrigin::Current:
    offset = lseek(file_descriptor, distance, SEEK_CUR);
    break;

  case FileSeekOrigin::End:
    offset = lseek(file_descriptor, distance, SEEK_END);
    break;
  }

  if (offset == -1)
    return false;
  else
    return true;
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

  if (!ToUnixOffset((intptr_t)handle.handle, origin, distance))
  {
    ATR_LOG_ERROR("Failed to SeekOrigin using lseek: %i", errno);
    return false;
  }

  ssize_t bytes_read = read((intptr_t)handle.handle, buffer, size);

  if (bytes_read == -1)
  {
    ATR_LOG_ERROR("Could not read file data using read: %i", errno);
    return false;
  }

  if (out_bytes_read) *out_bytes_read = bytes_read;

  return true;
}

bool FileReadToString(
    FileHandle     handle,
    FileSeekOrigin origin,
    i64            distance,
    char          *buffer,
    u64            capacity,
    u64           *out_bytes_read
)
{
  ATR_ASSERT(handle.handle != nullptr, "handle must be valid");
  ATR_ASSERT(buffer != nullptr && capacity > 0, "buffer must hold at least the terminator");

  u64 bytes_read = 0;

  if (!FileRead(handle, origin, distance, buffer, capacity - 1, &bytes_read))
  {
    buffer[0] = '\0';
    return false;
  }

  buffer[bytes_read] = '\0';
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

  if (!ToUnixOffset((intptr_t)handle.handle, origin, distance))
  {
    ATR_LOG_ERROR("Failed to SeekOrigin using lseek: %i", errno);
    return false;
  }

  ssize_t bytes_written = write((intptr_t)handle.handle, buffer, size);

  if (bytes_written == -1)
  {
    ATR_LOG_ERROR("Could not write file data using write: %i", errno);
    return false;
  }

  if (out_bytes_written) *out_bytes_written = bytes_written;

  return true;
}

void FileFlush(FileHandle handle)
{
  ATR_ASSERT(handle.handle != nullptr, "Cannot flush using empty file handle");

  if (fsync((intptr_t)handle.handle) == -1)
    ATR_LOG_ERROR("Failed to flush file using fsync: %i", errno);
}