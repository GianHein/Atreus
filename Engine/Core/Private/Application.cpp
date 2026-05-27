#include "Core/Application.h"

#include "Core/Logger.h"
#include "Debug.h"

namespace Atr
{
  void ApplicationInit()
  {
    Logger::Init();

    ATR_TRACE("Initializing core");
  }

  void ApplicationRun()
  {
    ATR_TRACE("Running core");
  }

  void ApplicationShutdown()
  {
    ATR_TRACE("Shutting down core");

    Logger::Shutdown();
  }
}