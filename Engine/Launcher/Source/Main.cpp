#include "Platform/Library.h"

int main(int argc, char **argv)
{
  Atr::LibraryHandle CoreLibrary;
  Atr::LibraryLoad("AtreusCore.dll", &CoreLibrary);

  void (*ApplicationInit)() = nullptr;
  void (*ApplicationRun)() = nullptr;
  void (*ApplicationShutdown)() = nullptr;

  Atr::LibraryGetSymbol(&CoreLibrary, "ApplicationInit", (void **)&ApplicationInit);
  Atr::LibraryGetSymbol(&CoreLibrary, "ApplicationRun", (void **)&ApplicationRun);
  Atr::LibraryGetSymbol(&CoreLibrary, "ApplicationShutdown", (void **)&ApplicationShutdown);

  if (ApplicationInit)
  {
    ApplicationInit();
  }

  if (ApplicationRun)
  {
    ApplicationRun();
  }

  if (ApplicationShutdown)
  {
    ApplicationShutdown();
  }

  Atr::LibraryUnload(&CoreLibrary);
  return 0;
}