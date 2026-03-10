#pragma once

#include "Atr/Definitions.h"
#include "Atr/Platform/Types/WindowHandle.h"

namespace Atr
{
  struct WindowDescription
  {
    WindowHandle Window;

    u32 Width;
    u32 Height;
    u32 ClientWidth;
    u32 ClientHeight;

    b8 IsFullscreen;
    b8 IsMaximized;
    b8 IsMinimized;
    b8 IsHidden;
    b8 IsBorderless;

    b8 IsResizable;
  };
}