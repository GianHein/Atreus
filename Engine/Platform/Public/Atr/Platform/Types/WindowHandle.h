#pragma once

#include "Atr/Definitions.h"

namespace Atr
{
  // TODO: Might have to rework this to actual platform specific handle types inside WindowHandle usign ATR_PLATFORM_... macro
  struct WindowHandle
  {
    u64 Value;
  };

  constexpr WindowHandle InvalidWindowHandle = {0};
}