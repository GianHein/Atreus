#include "Atr/Core/RuntimeMain.h"

#include "Atr/Platform/Console.h"

namespace Atr
{
  void AtrRuntimeInit()
  {
    Platform::ConsoleWrite(CONSOLE_COLOR_INFO, "Initializing Atr Runtime...\n");
  }

  void AtrRuntimeMain()
  {
    Platform::ConsoleWrite(CONSOLE_COLOR_INFO, "Running Atr Runtime...\n");
  }

  void AtrRuntimeShutdown()
  {
    Platform::ConsoleWrite(CONSOLE_COLOR_INFO, "Shutting down Atr Runtime...\n");
  }
}