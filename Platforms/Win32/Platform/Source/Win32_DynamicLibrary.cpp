#include "Atreus/Platform/DynamicLibrary.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "Atreus/Log.h"

#include <Windows.h>

namespace Atreus
{
  b8 LoadLibrary(const char *path, LibraryHandle *out_handle)
  {
    ATR_ASSERT(out_handle != nullptr, "out_handle required!");
    HMODULE handle = ::LoadLibraryA(path);

    if (handle == nullptr)
    {
      *out_handle = InvalidLibraryHandle;
      ATR_LOG_ERROR("Failed to load library '%s' through LoadLibraryA: %lu", path, GetLastError());
      return false;
    }

    out_handle->handle = handle;
    return true;
  }

  b8 CloseLibrary(LibraryHandle *handle)
  {
    ATR_ASSERT(handle != nullptr, "out_handle required!");

    if (!FreeLibrary((HMODULE)handle->handle))
    {
      ATR_LOG_ERROR("Failed to free library through FreeLibrary: %lu", GetLastError());
      return false;
    }

    *handle = InvalidLibraryHandle;
    return true;
  }

  b8 GetSymbol(LibraryHandle handle, const char *symbol_name, void **out_symbol)
  {
    ATR_ASSERT(out_symbol != nullptr, "out_symbol required!");
    FARPROC symbol = GetProcAddress((HMODULE)handle.handle, symbol_name);

    if (symbol == nullptr)
    {
      ATR_LOG_ERROR(
          "Failed to load symbol '%s' through GetProcAddress: %lu", symbol_name, GetLastError()
      );
      *out_symbol = nullptr;
      return false;
    }

    *out_symbol = symbol;
    return true;
  }
}