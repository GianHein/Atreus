#include "Platform/System.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Atr
{
  b8 PollEvents()
  {
    MSG message;

    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&message);
      DispatchMessageA(&message);
    }

    return TRUE;
  }
}