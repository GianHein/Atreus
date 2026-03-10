#pragma once

#include "Atr/Definitions.h"
#include "Atr/Platform/Types/Duration.h"

namespace Atr
{
  struct TimePoint
  {
    u64 Ticks;

    friend Duration operator-(TimePoint A, TimePoint B);
  };
}