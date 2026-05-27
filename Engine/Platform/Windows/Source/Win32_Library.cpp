#include "Platform/Library.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Atr
{
  b8 LibraryLoad(const char *path, LibraryHandle *out_handle)
  {
    HMODULE handle = ::LoadLibraryA(path);

    if (handle == nullptr)
    {
      out_handle->value = 0;
      return FALSE;
    }

    out_handle->value = (u64)handle;
    return TRUE;
  }

  b8 LibraryUnload(LibraryHandle *handle)
  {
    if (handle->value == 0)
    {
      return FALSE;
    }

    if (!FreeLibrary((HMODULE)handle->value))
    {
      return FALSE;
    }

    handle->value = 0;
    return TRUE;
  }

  b8 LibraryGetSymbol(LibraryHandle *handle, const char *symbol_name, void **out_symbol)
  {
    FARPROC symbol = GetProcAddress((HMODULE)handle->value, symbol_name);

    if (symbol == nullptr)
    {
      *out_symbol = nullptr;
      return FALSE;
    }

    *out_symbol = symbol;
    return TRUE;
  }
}