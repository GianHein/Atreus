#include "Atr/Platform/Library.h"

int main(int argc, char **argv)
{
  Atr::Platform::Library::LibraryHandle RuntimeLibrary = {0};
  Atr::Platform::Library::Load("AtreusRuntime.dll", &RuntimeLibrary);

  void (*RuntimeInit)() = nullptr;
  void (*RuntimeMain)() = nullptr;
  void (*RuntimeShutdown)() = nullptr;

  Atr::Platform::Library::GetSymbol(&RuntimeLibrary, "RuntimeInit", (void **)&RuntimeInit);
  Atr::Platform::Library::GetSymbol(&RuntimeLibrary, "RuntimeMain", (void **)&RuntimeMain);
  Atr::Platform::Library::GetSymbol(&RuntimeLibrary, "RuntimeShutdown", (void **)&RuntimeShutdown);

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

  Atr::Platform::Library::Unload(&RuntimeLibrary);

  return 0;
}