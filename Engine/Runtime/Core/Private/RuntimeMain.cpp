#include "Atr/Core/RuntimeMain.h"

#include "Atr/Core/Logging/Logger.h"

namespace Atr
{
  void RuntimeInit()
  {
    Logger::Init();

    ATR_TRACE("Initializing runtime");
  }

  void RuntimeMain()
  {
    ATR_TRACE("Running runtime");
  }

  void RuntimeShutdown()
  {
    ATR_TRACE("Shutting down runtime");

    Logger::Shutdown();
  }
}