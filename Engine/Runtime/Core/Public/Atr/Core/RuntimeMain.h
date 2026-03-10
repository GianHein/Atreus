#pragma once

#include "Atr/Definitions.h"

namespace Atr
{
  extern "C" ATR_API void AtrRuntimeInit();
  extern "C" ATR_API void AtrRuntimeMain();
  extern "C" ATR_API void AtrRuntimeShutdown();
}