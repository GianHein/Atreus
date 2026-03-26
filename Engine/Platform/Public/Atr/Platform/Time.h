#pragma once

#include "Atr/Definitions.h"
#include "Atr/Platform/Common/DateTime.h"
#include "Atr/Platform/Common/Ticks.h"

namespace Atr::Platform::Time
{
  Ticks GetNowTicks();
  Ticks GetTicksPerSecond();

  TickDuration GetDurationFromSeconds(f64 Seconds);
  TickDuration GetDurationFromMilliseconds(f64 Milliseconds);

  DateTime GetUtcTime();
  DateTime GetLocalTime();
}