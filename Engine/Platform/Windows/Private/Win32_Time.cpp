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
    SYSTEMTIME Time;
    ::GetSystemTime(&Time);

    return {.Year = Time.wYear,
            .Month = Time.wMonth,
            .Day = Time.wDay,
            .Hour = Time.wHour,
            .Second = Time.wSecond,
            .Millisecond = Time.wMilliseconds};
  }

  DateTime GetLocalTime()
  {
    SYSTEMTIME Time;
    ::GetLocalTime(&Time);

    return {.Year = Time.wYear,
            .Month = Time.wMonth,
            .Day = Time.wDay,
            .Hour = Time.wHour,
            .Second = Time.wSecond,
            .Millisecond = Time.wMilliseconds};
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