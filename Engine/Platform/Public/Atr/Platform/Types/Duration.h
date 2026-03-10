#pragma once

#include "Atr/Definitions.h"

namespace Atr
{
  struct Duration
  {
    u64 Ticks;

    f64 Seconds() const;
    f64 Milliseconds() const;
  };
}