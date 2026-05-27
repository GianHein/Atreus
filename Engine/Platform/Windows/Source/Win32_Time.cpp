#include "Platform/Time.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Atr
{

  Ticks GetNowTicks()
  {
    LARGE_INTEGER performance_counter;
    QueryPerformanceCounter(&performance_counter);
    return {(u64)performance_counter.QuadPart};
  }

  Ticks GetTicksPerSecond()
  {
    static u64 performance_frequency = []
    {
      LARGE_INTEGER frequency;
      QueryPerformanceFrequency(&frequency);
      return (u64)frequency.QuadPart;
    }();

    return performance_frequency;
  }

  TickDuration GetDurationFromSeconds(f64 seconds)
  {
    return {(u64)(seconds * GetTicksPerSecond())};
  }

  TickDuration GetDurationFromMilliseconds(f64 milliseconds)
  {
    return {(u64)((milliseconds * GetTicksPerSecond()) / 1000.0)};
  }

  DateTime GetUtcDate()
  {
    SYSTEMTIME time;
    GetSystemTime(&time);

    return {.year = (u16)time.wYear,
            .month = (u8)time.wMonth,
            .day = (u8)time.wDay,
            .hour = (u8)time.wHour,
            .minute = (u8)time.wMinute,
            .second = (u8)time.wSecond,
            .millisecond = (u16)time.wMilliseconds};
  }

  DateTime GetLocalDate()
  {
    SYSTEMTIME time;
    GetLocalTime(&time);

    return {.year = (u16)time.wYear,
            .month = (u8)time.wMonth,
            .day = (u8)time.wDay,
            .hour = (u8)time.wHour,
            .minute = (u8)time.wMinute,
            .second = (u8)time.wSecond,
            .millisecond = (u16)time.wMilliseconds};
  }

  f64 TickDuration::ToMilliseconds() const
  {
    return (duration / GetTicksPerSecond()) * 1000.0;
  }

  f64 TickDuration::ToSeconds() const
  {
    return duration / GetTicksPerSecond();
  }
}