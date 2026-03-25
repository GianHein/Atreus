#pragma once

#include "Atr/Definitions.h"

namespace Atr
{
  extern "C" ATR_API void RuntimeInit();
  extern "C" ATR_API void RuntimeMain();
  extern "C" ATR_API void RuntimeShutdown();
}