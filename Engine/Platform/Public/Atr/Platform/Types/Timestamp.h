#pragma once

#include "Atr/Definitions.h"
#include "Atr/Platform/Types/Duration.h"

namespace Atr
{
  struct Timestamp
  {
    u64 Ticks;

    friend Duration operator-(Timestamp A, Timestamp B);
  };
}