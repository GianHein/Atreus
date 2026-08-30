#pragma once

#include "Atreus/Definitions.h"

namespace Atreus
{
  using Ticks = u64;

  Ticks GetTicks();
  Ticks GetTicksPerSecond();

  struct DateTime
  {
    u16 year;
    u8  month;
    u8  day;
    u8  hour;
    u8  minute;
    u8  second;
    u16 millisecond;
  };

  DateTime GetUtcDateTime();
  DateTime GetLocalDateTime();
}