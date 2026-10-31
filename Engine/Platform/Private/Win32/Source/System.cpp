#include "Atreus/Platform/System.h"

#include "Atreus/Log.h"

#if !defined(WIN32_LEAN_AND_MEAN)
#  define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <string.h>

// Time

namespace
{
  u64 QueryFrequency()
  {
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    return static_cast<u64>(frequency.QuadPart);
  }

  const u64 frequency = QueryFrequency();
}

u64 TimeNowTicks()
{
  LARGE_INTEGER counter;
  QueryPerformanceCounter(&counter);
  return static_cast<u64>(counter.QuadPart);
}

f64 TimeTicksToSeconds(u64 ticks)
{
  return static_cast<f64>(ticks) / static_cast<f64>(frequency);
}

f64 TimeNowSeconds()
{
  return TimeTicksToSeconds(TimeNowTicks());
}

i64 TimeUnixMilliseconds()
{
  FILETIME file_time;
  GetSystemTimeAsFileTime(&file_time);
  u64 ticks = (static_cast<u64>(file_time.dwHighDateTime) << 32) | file_time.dwLowDateTime;
  return static_cast<i64>((ticks - 116444736000000000ull) / 10000ull);
}

DateTime TimeGetUtcDate()
{
  SYSTEMTIME system_time;
  GetSystemTime(&system_time);

  DateTime date_time;
  date_time.year        = (u16)system_time.wYear;
  date_time.month       = (u8)system_time.wMonth;
  date_time.day         = (u8)system_time.wDay;
  date_time.hour        = (u8)system_time.wHour;
  date_time.minute      = (u8)system_time.wMinute;
  date_time.second      = (u8)system_time.wSecond;
  date_time.millisecond = (u16)system_time.wMilliseconds;

  return date_time;
}

DateTime TimeGetLocalDate()
{
  SYSTEMTIME system_time;
  GetLocalTime(&system_time);

  DateTime date_time;
  date_time.year        = (u16)system_time.wYear;
  date_time.month       = (u8)system_time.wMonth;
  date_time.day         = (u8)system_time.wDay;
  date_time.hour        = (u8)system_time.wHour;
  date_time.minute      = (u8)system_time.wMinute;
  date_time.second      = (u8)system_time.wSecond;
  date_time.millisecond = (u16)system_time.wMilliseconds;

  return date_time;
}

// Paths

bool PathGetExecutable(char *buffer, u64 capacity)
{
  ATR_ASSERT(buffer != nullptr && capacity > 0, "buffer must hold at least the terminator");

  DWORD length = GetModuleFileNameA(nullptr, buffer, static_cast<DWORD>(capacity));

  // On truncation GetModuleFileNameA returns the full buffer size.
  if (length == 0 || length >= capacity)
  {
    ATR_LOG_ERROR("Failed to resolve executable path using GetModuleFileNameA: %lu", GetLastError());
    buffer[0] = '\0';
    return false;
  }

  return true;
}

bool PathGetDirectory(const char *path, char *buffer, u64 capacity)
{
  ATR_ASSERT(path != nullptr, "path must not be null");
  ATR_ASSERT(buffer != nullptr && capacity > 0, "buffer must hold at least the terminator");

  // Windows accepts both separators, so take whichever comes last.
  const char *backslash = strrchr(path, '\\');
  const char *slash     = strrchr(path, '/');
  const char *separator = backslash > slash ? backslash : slash;

  const char *directory = path;
  u64         length    = 0;

  if (separator == nullptr)
  {
    directory = ".";
    length    = 1;
  }
  else
  {
    length = static_cast<u64>(separator - path);

    // Keep the separator when it is the root, so "\foo" yields "\" and "C:\foo" yields "C:\".
    if (length == 0 || (length == 2 && path[1] == ':')) length += 1;
  }

  if (length >= capacity)
  {
    ATR_LOG_ERROR("Buffer too small for directory of '%s'", path);
    buffer[0] = '\0';
    return false;
  }

  memmove(buffer, directory, length);
  buffer[length] = '\0';
  return true;
}

// Dynamic libraries

Library *LibraryOpen(const char *path)
{
  HMODULE handle = LoadLibraryA(path);

  if (!handle)
  {
    ATR_LOG_ERROR(
        "Platform::System",
        "Failed to load library '%s' using LoadLibraryA: %lu",
        path,
        GetLastError()
    );
    return nullptr;
  }

  return reinterpret_cast<Library *>(handle);
}

void *LibrarySymbol(Library *library, const char *name)
{
  FARPROC symbol = GetProcAddress(reinterpret_cast<HMODULE>(library), name);

  if (symbol == nullptr)
  {
    ATR_LOG_ERROR(
        "Platform::System",
        "Failed to load symbol '%s' using GetProcAddress: %lu",
        name,
        GetLastError()
    );
    return nullptr;
  }

  return reinterpret_cast<void *>(symbol);
}

void LibraryClose(Library *library)
{
  if (library != nullptr) FreeLibrary(reinterpret_cast<HMODULE>(library));
}

const char *LibraryExtension()
{
  return ".dll";
}