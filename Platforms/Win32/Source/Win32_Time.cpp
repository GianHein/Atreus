#include "Atreus/Platform/Time.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "Atreus/Log.h"

#include <Windows.h>

namespace Atreus
{
  Ticks GetTicks()
  {
    LARGE_INTEGER performance_counter;
    QueryPerformanceCounter(&performance_counter);
    return static_cast<Ticks>(performance_counter.QuadPart);
  }

  static LARGE_INTEGER performance_frequency = []
  {
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    return frequency;
  }();

  Ticks GetTicksPerSecond()
  {
    ATR_ASSERT(performance_frequency.QuadPart != 0, "PerformanceFrequency may not be 0");
    return static_cast<Ticks>(performance_frequency.QuadPart);
  }

  DateTime GetUtcDateTime()
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

  DateTime GetLocalDateTime()
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
}