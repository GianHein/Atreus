#include "Atr/Core/RuntimeMain.h"

#include "Atr/Logging/Logger.h"

namespace Atr
{
  void RuntimeInit()
  {
    ATR_TRACE("Initializing runtime");
  }

  void RuntimeMain()
  {
    ATR_TRACE("Running runtime");
  }

  void RuntimeShutdown()
  {
    ATR_TRACE("Shutting down runtime");
  }
}