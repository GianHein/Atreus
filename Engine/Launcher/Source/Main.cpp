#include "Atr/Platform/Library.h"

int main(int argc, char **argv)
{
  Atr::LibraryHandle RuntimeLibrary = {0};
  Atr::Platform::LoadLibrary("libAtreusRuntime.dll", &RuntimeLibrary);

  // Load init, main, and shutdown functions from the runtime library

  void (*RuntimeInit)() = nullptr;
  void (*RuntimeMain)() = nullptr;
  void (*RuntimeShutdown)() = nullptr;

  Atr::Platform::LoadSymbol("AtrRuntimeInit", &RuntimeLibrary, (void **)&RuntimeInit);
  Atr::Platform::LoadSymbol("AtrRuntimeMain", &RuntimeLibrary, (void **)&RuntimeMain);
  Atr::Platform::LoadSymbol("AtrRuntimeShutdown", &RuntimeLibrary, (void **)&RuntimeShutdown);

  // Call the runtime functions

  if (RuntimeInit)
  {
    RuntimeInit();
  }

  if (RuntimeMain)
  {
    RuntimeMain();
  }

  if (RuntimeShutdown)
  {
    RuntimeShutdown();
  }

  // Unload the runtime library

  Atr::Platform::UnloadLibrary(&RuntimeLibrary);

  return 0;
}