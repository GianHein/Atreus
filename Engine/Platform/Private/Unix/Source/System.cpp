#include "Atreus/Platform/System.h"

#include "Atreus/Log.h"

#include <dlfcn.h>
#include <time.h>

// Time

f64 TimeNowSeconds()
{
  return TimeTicksToSeconds(TimeNowTicks());
}

u64 TimeNowTicks()
{
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);

  return static_cast<u64>(ts.tv_sec) * 1000000000ull + static_cast<u64>(ts.tv_nsec);
}

f64 TimeTicksToSeconds(u64 ticks)
{
  return ticks / 1e9;
}

i64 TimeUnixMilliseconds()
{
  timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);

  return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

DateTime TimeGetUtcDate()
{
  timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);

  struct tm tm_buf;
  gmtime_r(&ts.tv_sec, &tm_buf);

  DateTime date_time;
  date_time.year        = tm_buf.tm_year + 1900;
  date_time.month       = tm_buf.tm_mon + 1;
  date_time.day         = tm_buf.tm_mday;
  date_time.hour        = tm_buf.tm_hour;
  date_time.minute      = tm_buf.tm_min;
  date_time.second      = tm_buf.tm_sec;
  date_time.millisecond = ts.tv_nsec / 1000000;

  return date_time;
}

DateTime TimeGetLocalDate()
{
  timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);

  struct tm tm_buf;
  localtime_r(&ts.tv_sec, &tm_buf);

  DateTime date_time;
  date_time.year        = tm_buf.tm_year + 1900;
  date_time.month       = tm_buf.tm_mon + 1;
  date_time.day         = tm_buf.tm_mday;
  date_time.hour        = tm_buf.tm_hour;
  date_time.minute      = tm_buf.tm_min;
  date_time.second      = tm_buf.tm_sec;
  date_time.millisecond = ts.tv_nsec / 1000000;

  return date_time;
}

// Dynamic Libraries

Library *LibraryOpen(const char *path)
{
  Library *handle = (Library *)dlopen(path, RTLD_NOW);

  if (handle == nullptr)
  {
    ATR_LOG_ERROR("Failed to load library '%s' using dlopen: %s", path, dlerror());
    return nullptr;
  }

  return handle;
}

void *LibrarySymbol(Library *library, const char *name)
{
  void *symbol = dlsym(library, name);

  if (symbol == nullptr)
  {
    ATR_LOG_ERROR("Failed to load symbol '%s' using dlsym: %s", name, dlerror());
    return nullptr;
  }

  return symbol;
}

void LibraryClose(Library *library)
{
  if (library != nullptr) dlclose(library);
}

const char *LibraryExtension()
{
  return ".so";
}
