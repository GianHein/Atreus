#pragma once

#include "Atr/Definitions.h"

namespace Atr
{
  enum class LogLevel : u8
  {
    LEVEL_TRACE = 0,
    LEVEL_DEBUG,
    LEVEL_INFO,
    LEVEL_WARN,
    LEVEL_ERROR,
    LEVEL_FATAL
  };
}