#pragma once

#include "Atr/Definitions.h"

namespace Atr
{
  struct FileHandle
  {
    u64 Value;
  };

  constexpr FileHandle InvalidFileHandle = {0};
}