#pragma once

#include "Atr/Definitions.h"

namespace Atr
{
  // TODO: Replace these with actual colors, not log-levels
  enum ConsoleColor : u8
  {
    CONSOLE_COLOR_TRACE = 0,
    CONSOLE_COLOR_DEBUG,
    CONSOLE_COLOR_INFO,
    CONSOLE_COLOR_WARN,
    CONSOLE_COLOR_ERROR,
    CONSOLE_COLOR_FATAL
  };
}