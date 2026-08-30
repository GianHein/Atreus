
#include "Atreus/Platform/Input.h"

#include <cstring>

namespace Atreus
{
  b8 IsValidWindowHandle(WindowHandle handle)
  {
    return handle.index != UINT32_MAX && handle.generation != UINT32_MAX;
  }

  b8 SameWindow(WindowHandle a, WindowHandle b)
  {
    return a.index == b.index && a.generation == b.generation;
  }
}