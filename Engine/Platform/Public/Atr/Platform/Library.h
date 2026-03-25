#pragma once

#include "Atr/Definitions.h"

namespace Atr::Platform::Library
{

  struct LibraryHandle
  {
    u64 Value;
  };

  constexpr LibraryHandle InvalidHandle = {0};

  b8 Load(const char *Path, LibraryHandle *OutHandle);
  b8 Unload(LibraryHandle *Handle);

  b8 GetSymbol(LibraryHandle *Handle, const char *SymbolName, void **OutSymbol);
}