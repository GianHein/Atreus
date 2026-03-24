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