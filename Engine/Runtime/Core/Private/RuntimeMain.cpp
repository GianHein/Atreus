#include "Atr/Core/RuntimeMain.h"

#include "Atr/Core/Logging/Logger.h"

namespace Atr
{
  void AtrRuntimeInit()
  {
    ATR_TRACE("Initializing runtime");
  }

  void AtrRuntimeMain()
  {
    ATR_TRACE("Running runtime");

    ATR_TRACE("TRACE");
    ATR_DEBUG("DEBUG");
    ATR_INFO("INFO");
    ATR_WARN("WARN");
    ATR_ERROR("ERROR");
    ATR_FATAL("FATAL");
  }

  void AtrRuntimeShutdown()
  {
    ATR_TRACE("Shutting down runtime");
  }
}