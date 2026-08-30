#pragma once

#include "Atreus/Definitions.h"

namespace Atreus
{
  struct WindowHandle
  {
    u32 index;
    u32 generation;
  };

  constexpr WindowHandle InvalidWindowHandle = {UINT32_MAX, UINT32_MAX};

  b8 IsValidWindowHandle(WindowHandle handle);
  b8 SameWindow(WindowHandle a, WindowHandle b);

  struct MonitorInfo
  {
    i32 x, y;
    u32 width, height;
    u32 refresh_rate;
    u32 dpi;
    f32 scale_factor;
    b8  is_primary;
  };

  struct WindowDescription
  {
    const char *title;
    u32         width;
    u32         height;
    b8          fullscreen;
    b8          borderless;
    b8          resizable;
  };

  b8   CreateWindow(const WindowDescription &description, WindowHandle *out_handle);
  void DestroyWindow(WindowHandle *handle);

  void SetWindowTitle(WindowHandle handle, const char *title);
  void SetWindowSize(WindowHandle handle, u32 width, u32 height);
  void GetWindowSize(WindowHandle handle, u32 *out_width, u32 *out_height);
  void SetWindowFullscreen(WindowHandle handle, b8 fullscreen);
  b8   IsWindowFullscreen(WindowHandle handle);
  void SetWindowBorderless(WindowHandle handle, b8 borderless);
  b8   IsWindowBorderless(WindowHandle handle);

  u32         GetMonitorCount();
  MonitorInfo GetMonitorInfo(u32 index);
  MonitorInfo GetMonitorInfoForWindow(WindowHandle handle);

  void *GetNativeWindowHandle(WindowHandle handle);
}