#pragma once

#include "Definitions.h"

namespace Atr
{
  struct WindowDescription
  {
    char *title;

    u32 width;
    u32 height;
    u32 client_width;
    u32 client_height;

    b8 is_fullscreen;
    b8 is_maximized;
    b8 is_minimized;
    b8 is_hidden;
    b8 is_borderless;
    b8 is_resizable;
  };

  struct WindowHandle
  {
    u64 value;
  };

  constexpr WindowHandle InvalidWindowHandle = {0};

  b8 CreateWindow(const WindowDescription &description, WindowHandle *out_handle);
  b8 DestroyWindow(WindowHandle *handle);
}