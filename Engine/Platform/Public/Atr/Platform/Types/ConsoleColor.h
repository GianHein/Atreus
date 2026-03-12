#pragma once

#include "Atr/Definitions.h"

namespace Atr
{
  // TODO: Replace these with actual colors, not log-levels
  enum class ConsoleColor : u8
  {
    COLOR_TRACE = 0,
    COLOR_DEBUG,
    COLOR_INFO,
    COLOR_WARN,
    COLOR_ERRROR,
    COLOR_FATAL
  };
}