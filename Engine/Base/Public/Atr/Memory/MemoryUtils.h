#pragma once

#include "Atr/Definitions.h"

namespace Atr
{
  class MemoryUtils
  {
  public:
    static const u64 CalculatePadding(const u64 BaseAddress, const u64 Alignment)
    {
      const u64 Multiplier = (BaseAddress / Alignment) + 1;
      const u64 AlignedAddress = Multiplier * Alignment;
      return AlignedAddress - BaseAddress;
    }
  };
}