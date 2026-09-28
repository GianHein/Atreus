#pragma once

#include "Atreus/Definitions.h"
#include "Atreus/Log.h"

namespace Atreus
{
  struct LibraryHandle
  {
    void *handle = nullptr;
  };

  constexpr LibraryHandle InvalidLibraryHandle = {nullptr};

  b8 LoadLibrary(const char *path, LibraryHandle *out_handle);
  b8 CloseLibrary(LibraryHandle *handle);

  b8 GetSymbol(LibraryHandle handle, const char *symbol_name, void **out_symbol);
}