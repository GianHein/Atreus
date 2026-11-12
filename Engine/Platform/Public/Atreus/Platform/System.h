#pragma once

#include "Atreus/Types.h"

// Time

ATR_API f64 time_now_seconds();
ATR_API u64 time_now_ticks();
ATR_API f64 time_ticks_to_seconds(u64 ticks);
ATR_API i64 time_unix_milliseconds();

struct DateTime
{
  u16 year        = 0;
  u8  month       = 0;
  u8  day         = 0;
  u8  hour        = 0;
  u8  minute      = 0;
  u8  second      = 0;
  u16 millisecond = 0;
};

ATR_API DateTime time_get_utc_date();
ATR_API DateTime time_get_local_date();

// Paths

ATR_API bool path_get_executable(char *buffer, u64 capacity);
ATR_API bool path_get_directory(const char *path, char *buffer, u64 capacity);

// Dynamic Libraries

struct Library;

ATR_API Library    *library_open(const char *path);
ATR_API void       *library_symbol(Library *library, const char *name);
ATR_API void        library_close(Library *library);
ATR_API const char *library_extension();