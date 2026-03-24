#pragma once

#include "Atr/Definitions.h"

namespace Atr
{
  using Ticks = u64;

  struct TickDuration
  {
    Ticks Duration;

  public:
    f64 ToSeconds() const;
    f64 ToMilliseconds() const;
  };
}