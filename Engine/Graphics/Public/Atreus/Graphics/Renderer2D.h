#pragma once

#include "Atreus/Definitions.h"
#include "Atreus/Platform/Window.h"

namespace Atreus
{
  void InitializeGraphics(WindowHandle window);
  void ShutdownGraphics(WindowHandle window);

  void ClearBackground(WindowHandle window, f32 r, f32 g, f32 b, f32 a);
  void Present(WindowHandle window);
}