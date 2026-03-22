#include "Atr/Platform/Time.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Atr::Platform
{

  static f64 GetFrequency()
  {
    static f64 Frequency = []
    {
      LARGE_INTEGER PerformanceFrequency;
      QueryPerformanceFrequency(&PerformanceFrequency);
      return (f64)PerformanceFrequency.QuadPart;
    }();

    return Frequency;
  }

  Timestamp GetNow()
  {
    LARGE_INTEGER PerformanceCounter;

    QueryPerformanceCounter(&PerformanceCounter);

    return {PerformanceCounter.QuadPart};
  }

  Duration FromSeconds(f64 Seconds)
  {
    return {Seconds * GetFrequency()};
  }

  Duration FromMilliseconds(f64 Milliseconds)
  {
    return {(Milliseconds / 1000.0) * GetFrequency()};
  }

  DateTime GetUtcTime()
  {
    SYSTEMTIME UtcTime;
    ::GetSystemTime(&UtcTime);

    return {.Year = UtcTime.wYear,
            .Month = UtcTime.wMonth,
            .Day = UtcTime.wDay,
            .Hour = UtcTime.wHour,
            .Minute = UtcTime.wMinute,
            .Second = UtcTime.wSecond,
            .Millisecond = UtcTime.wMilliseconds};
  }

  DateTime GetLocalTime()
  {
    SYSTEMTIME LocalTime;
    ::GetLocalTime(&LocalTime);

    return {.Year = LocalTime.wYear,
            .Month = LocalTime.wMonth,
            .Day = LocalTime.wDay,
            .Hour = LocalTime.wHour,
            .Minute = LocalTime.wMinute,
            .Second = LocalTime.wSecond,
            .Millisecond = LocalTime.wMilliseconds};
  }
}

namespace Atr
{
  f64 Duration::Seconds() const
  {
    return (f64)Ticks / Platform::GetFrequency();
  }

  f64 Duration::Milliseconds() const
  {
    return ((f64)Ticks * 1000.0) / Platform::GetFrequency();
  }
}