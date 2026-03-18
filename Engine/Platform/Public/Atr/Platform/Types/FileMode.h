#pragma once

#include "Atr/Definitions.h"

namespace Atr
{
  enum class FileMode : u8
  {
    READ = 0,
    WRITE,
    READ_WRITE,
    APPEND
  };
}