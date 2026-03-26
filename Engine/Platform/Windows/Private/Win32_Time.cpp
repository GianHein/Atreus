#include "Atr/Platform/Time.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Atr::Platform::Time
{
  Ticks GetNowTicks()
  {
    LARGE_INTEGER PerformanceCount;
    QueryPerformanceCounter(&PerformanceCount);

    return static_cast<Ticks>(PerformanceCount.QuadPart);
  }

  Ticks GetTicksPerSecond()
  {
    static Ticks Frequency = []
    {
      LARGE_INTEGER PerformanceFrequency;
      QueryPerformanceFrequency(&PerformanceFrequency);

      return static_cast<Ticks>(PerformanceFrequency.QuadPart);
    }();

    return Frequency;
  }

  TickDuration GetDurationFromSeconds(f64 Seconds)
  {
    return {(u64)(Seconds * GetTicksPerSecond())};
  }

  TickDuration GetDurationFromMilliseconds(f64 Milliseconds)
  {
    return {(u64)((Milliseconds * GetTicksPerSecond()) / 1000)};
  }

  DateTime GetUtcTime()
  {
    SYSTEMTIME Time;
    GetSystemTime(&Time);

    return {.Year = (u16)Time.wYear,
            .Month = (u8)Time.wMonth,
            .Day = (u8)Time.wDay,
            .Hour = (u8)Time.wHour,
            .Minute = (u8)Time.wMinute,
            .Second = (u8)Time.wSecond,
            .Millisecond = (u16)Time.wMilliseconds};
  }

  DateTime GetLocalTime()
  {
    SYSTEMTIME Time;
    ::GetLocalTime(&Time);

    return {.Year = (u16)Time.wYear,
            .Month = (u8)Time.wMonth,
            .Day = (u8)Time.wDay,
            .Hour = (u8)Time.wHour,
            .Minute = (u8)Time.wMinute,
            .Second = (u8)Time.wSecond,
            .Millisecond = (u16)Time.wMilliseconds};
  }
}

namespace Atr
{
  f64 TickDuration::ToSeconds() const
  {
    return Duration / Platform::Time::GetTicksPerSecond();
  }

  f64 TickDuration::ToMilliseconds() const
  {
    return (Duration / Platform::Time::GetTicksPerSecond()) * 1000.0;
  }
}