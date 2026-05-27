#pragma once

#include "Definitions.h"

namespace Atr
{
  struct LibraryHandle
  {
    u64 value;
  };

  constexpr LibraryHandle InvalidHandle = {0};

  b8 LibraryLoad(const char *path, LibraryHandle *out_handle);
  b8 LibraryUnload(LibraryHandle *handle);

  b8 LibraryGetSymbol(LibraryHandle *handle, const char *symbol_name, void **out_symbol);
}