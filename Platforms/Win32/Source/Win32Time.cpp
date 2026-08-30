#include "Atreus/Platform/Time.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

namespace Atreus
{
  Ticks GetTicks()
  {
    LARGE_INTEGER performance_counter;
    QueryPerformanceCounter(&performance_counter);

    return performance_counter.QuadPart;
  }

  static LARGE_INTEGER performance_frequency = []
  {
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    return frequency;
  }();

  Ticks GetTicksPerSecond()
  {
    return performance_frequency.QuadPart;
  }

  DateTime GetUtcDateTime()
  {
    SYSTEMTIME system_time;
    GetSystemTime(&system_time);

    DateTime date_time = {
        .year        = (u16)system_time.wYear,
        .month       = (u8)system_time.wMonth,
        .day         = (u8)system_time.wDay,
        .hour        = (u8)system_time.wHour,
        .minute      = (u8)system_time.wMinute,
        .second      = (u8)system_time.wSecond,
        .millisecond = (u16)system_time.wMilliseconds
    };

    return date_time;
  }

  DateTime GetLocalDateTime()
  {
    SYSTEMTIME system_time;
    GetLocalTime(&system_time);

    DateTime date_time = {
        .year        = (u16)system_time.wYear,
        .month       = (u8)system_time.wMonth,
        .day         = (u8)system_time.wDay,
        .hour        = (u8)system_time.wHour,
        .minute      = (u8)system_time.wMinute,
        .second      = (u8)system_time.wSecond,
        .millisecond = (u16)system_time.wMilliseconds
    };

    return date_time;
  }
}