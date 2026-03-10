#include "Atr/Platform/Library.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef LoadLibrary

namespace Atr::Platform
{
  b8 LoadLibrary(const char *Name, LibraryHandle *OutLibrary)
  {
    HMODULE Library = ::LoadLibraryA(Name);

    if (Library == nullptr)
    {
      return FALSE;
    }

    OutLibrary->Value = (u64)Library;
    return TRUE;
  }

  b8 LoadSymbol(const char *Name, LibraryHandle *Library, void **OutSymbol)
  {
    if (Library->Value == 0)
    {
      return FALSE;
    }

    FARPROC Symbol = ::GetProcAddress((HMODULE)Library->Value, Name);

    if (Symbol == nullptr)
    {
      return FALSE;
    }

    *OutSymbol = (void *)Symbol;
    return TRUE;
  }

  b8 UnloadLibrary(LibraryHandle *Library)
  {
    if (Library->Value == 0)
    {
      return FALSE;
    }

    if (!FreeLibrary((HMODULE)Library->Value))
    {
      return FALSE;
    }

    Library->Value = 0;
    return TRUE;
  }
}