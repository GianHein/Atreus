#pragma once

#include "Atreus/Definitions.h"

namespace Atreus
{
  using Ticks = u64;

  Ticks GetTicks();
  Ticks GetTicksPerSecond();

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

  DateTime GetUtcDateTime();
  DateTime GetLocalDateTime();
}