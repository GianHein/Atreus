#pragma once

#include "Atr/Definitions.h"
#include "Atr/Platform/Types/LibraryHandle.h"

namespace Atr::Platform
{
  // TODO: replace this with Atr::String so we can modify the name to add the appropriate extension for the platform
  b8 LoadLibrary(const char *Name, LibraryHandle *OutLibrary);

  // TODO: eventually it might be smarter to add a symbols field to the LibraryHandle struct
  b8 LoadSymbol(const char *Name, LibraryHandle *Library, void **OutSymbol);
  b8 UnloadLibrary(LibraryHandle *Library);
}