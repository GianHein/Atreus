#pragma once

#include "Definitions.h"

namespace Atr
{
  using Ticks = u64;

  struct TickDuration
  {
    Ticks duration;

  public:
    f64 ToSeconds() const;
    f64 ToMilliseconds() const;
  };

  struct DateTime
  {
    u16 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 minute;
    u8 second;
    u16 millisecond;
  };

  Ticks GetNowTicks();
  Ticks GetTicksPerSecond();

  TickDuration GetDurationFromSeconds(f64 seconds);
  TickDuration GetDurationFromMilliseconds(f64 milliseconds);

  DateTime GetUtcDate();
  DateTime GetLocalDate();
}