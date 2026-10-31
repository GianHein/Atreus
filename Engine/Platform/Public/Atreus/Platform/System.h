#pragma once

#include "Atreus/Types.h"

// Time

ATR_API f64 TimeNowSeconds();
ATR_API u64 TimeNowTicks();
ATR_API f64 TimeTicksToSeconds(u64 ticks);
ATR_API i64 TimeUnixMilliseconds();

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

ATR_API DateTime TimeGetUtcDate();
ATR_API DateTime TimeGetLocalDate();

// Paths

ATR_API bool PathGetExecutable(char *buffer, u64 capacity);
ATR_API bool PathGetDirectory(const char *path, char *buffer, u64 capacity);

// Dynamic Libraries

struct Library;

ATR_API Library    *LibraryOpen(const char *path);
ATR_API void       *LibrarySymbol(Library *library, const char *name);
ATR_API void        LibraryClose(Library *library);
ATR_API const char *LibraryExtension();