#pragma once

#include "Atr/Definitions.h"

namespace Atr
{
  struct DateTime
  {
    u16 Year;
    u8 Month;
    u8 Day;
    u8 Hour;
    u8 Minute;
    u8 Second;
    u16 Millisecond;
  };
}