#pragma once

#include "Atreus/Definitions.h"

namespace Atreus
{
  constexpr u32 MAX_WINDOWS = 8;

  struct WindowHandle
  {
    u32 index      = UINT32_MAX;
    u32 generation = UINT32_MAX;
  };

  constexpr WindowHandle InvalidWindowHandle = {UINT32_MAX, UINT32_MAX};

  b8 IsValidWindow(WindowHandle handle);
  b8 IsSameWindow(WindowHandle handle_a, WindowHandle handle_b);

  struct MonitorInfo
  {
    i32 x            = 0;
    i32 y            = 0;
    u32 width        = 0;
    u32 height       = 0;
    u32 refresh_rate = 0;
    u32 dpi          = 0;
    f32 scale_factor = 1.0f;
    b8  is_primary   = false;
  };

  struct WindowDescription
  {
    const char *title      = "Atreus Window";
    u32         width      = 800;
    u32         height     = 600;
    b8          fullscreen = false;
    b8          borderless = false;
    b8          resizable  = true;
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
  MonitorInfo GetMonitorInfo(u32 monitor_index);
  MonitorInfo GetMonitorInfoForWindow(WindowHandle handle);

  void *GetNativeWindowHandle(WindowHandle handle);
}