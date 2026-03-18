#include "Atr/Core/RuntimeMain.h"

#include "Atr/Core/Logging/Logger.h"

namespace Atr
{
  void AtrRuntimeInit()
  {
    if (!Logger::Init())
    {
      ATR_FATAL("Failed to initialize logger");
    }

    ATR_TRACE("Initializing runtime");
  }

  void AtrRuntimeMain()
  {
    ATR_TRACE("Running runtime");
  }

  void AtrRuntimeShutdown()
  {
    ATR_TRACE("Shutting down runtime");
    Logger::Shutdown();
  }
}