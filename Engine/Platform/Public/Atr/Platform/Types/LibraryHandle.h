#pragma once

#include "Atr/Definitions.h"

namespace Atr
{
  // TODO: add additional information (name, path), as well as a DynamicArray of loaded symbols
  struct LibraryHandle
  {
    u64 Value;
  };

  constexpr LibraryHandle InvalidLibraryHandle = {0};
}