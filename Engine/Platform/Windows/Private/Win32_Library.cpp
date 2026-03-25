#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef DebugBreak
#undef ERROR

#include "Atr/Assertions/Assert.h"
#include "Atr/Platform/Library.h"

namespace Atr::Platform::Library
{
  b8 Load(const char *Path, LibraryHandle *OutHandle)
  {
    ATR_ASSERT(Path != nullptr, "Path must not be null");
    ATR_ASSERT(OutHandle != nullptr, "OutHandle must not be null");
    ATR_ASSERT(OutHandle->Value == 0, "Cannot load dynamic library into non-empty LibraryHandle");

    HMODULE Handle = LoadLibraryA(Path);

    if (Handle == nullptr)
    {
      return FALSE;
    }

    OutHandle->Value = (u64)Handle;

    return TRUE;
  }

  b8 Unload(LibraryHandle *Handle)
  {
    ATR_ASSERT(Handle != nullptr, "Handle must not be null");
    ATR_ASSERT(Handle->Value != 0, "Cannot unload empty LibraryHandle");

    if (!FreeLibrary((HMODULE)Handle->Value))
    {
      return FALSE;
    }

    Handle->Value = 0;

    return TRUE;
  }

  b8 GetSymbol(LibraryHandle *Handle, const char *SymbolName, void **OutSymbol)
  {
    ATR_ASSERT(Handle != nullptr, "LibraryHandle must not be null");
    ATR_ASSERT(OutSymbol != nullptr, "OutSymbol must not be null");
    ATR_ASSERT(SymbolName != nullptr, "SymbolName must not be null");
    ATR_ASSERT(Handle->Value != 0, "Cannot get symbol for empty LibraryHandle");

    FARPROC Symbol = GetProcAddress((HMODULE)Handle->Value, SymbolName);

    if (Symbol == nullptr)
    {
      *OutSymbol = nullptr;
      return FALSE;
    }

    *OutSymbol = (void *)Symbol;

    return TRUE;
  }
}