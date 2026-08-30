#include "Atreus/Platform/Window.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "Atreus/Platform/PlatformEvent.h"

#include <Windows.h>
#undef CreateWindow

#include "Win32Internal.h"

#include <cstdio>

namespace Atreus
{
  constexpr u32 MAX_WINDOWS  = 8;
  constexpr u32 MAX_MONITORS = 16;

  struct Win32Window
  {
    u32   index;
    HWND  hwnd;
    b8    fullscreen;
    b8    borderless;
    i32   last_mouse_x;
    i32   last_mouse_y;
    b8    has_last_mouse_pos;
    DWORD dwStyle;
    RECT  rect;
  };

  static Win32Window window_pool[MAX_WINDOWS];
  static b8          window_pool_used[MAX_WINDOWS];
  static u32         window_generations[MAX_WINDOWS];

  static Win32Window *AllocWin32Window()
  {
    for (u32 i = 0; i < MAX_WINDOWS; ++i)
    {
      if (!window_pool_used[i])
      {
        window_pool_used[i]  = true;
        window_pool[i].index = i;
        return &window_pool[i];
      }
    }

    return nullptr;
  }

  static void FreeWin32Window(Win32Window *window)
  {
    for (u32 i = 0; i < MAX_WINDOWS; ++i)
    {
      if (&window_pool[i] == window)
      {
        *window             = {};
        window_pool_used[i] = false;
        window_generations[i]++;

        return;
      }
    }
  }

  static Win32Window *ResolveWindow(WindowHandle handle)
  {
    if (handle.index >= MAX_WINDOWS) return nullptr;
    if (!window_pool_used[handle.index]) return nullptr;
    if (window_generations[handle.index] != handle.generation) return nullptr;

    return &window_pool[handle.index];
  }

  static LRESULT Win32WndProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
  {
    Win32Window *window = (Win32Window *)GetWindowLongPtrA(hwnd, GWLP_USERDATA);

    if (window == nullptr)
    {
      if (Msg == WM_NCCREATE)
      {
        CREATESTRUCTA *create_struct = (CREATESTRUCTA *)lParam;
        window                       = (Win32Window *)create_struct->lpCreateParams;
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)window);
        window->hwnd = hwnd;
      }

      return DefWindowProc(hwnd, Msg, wParam, lParam);
    }

    WindowHandle  handle = {window->index, window_generations[window->index]};
    PlatformEvent event  = {};
    event.window         = handle;

    switch (Msg)
    {
    case WM_CLOSE:
    {
      event.type = PlatformEventType::WindowClose;
      PushPlatformEvent(event);
      return 0;
    };

    case WM_SIZE:
    {
      event.type                 = PlatformEventType::WindowResize;
      event.window_resize.width  = LOWORD(lParam);
      event.window_resize.height = HIWORD(lParam);

      PushPlatformEvent(event);
      return 0;
    };

    case WM_SETFOCUS:
    {
      event.type                  = PlatformEventType::FocusChanged;
      event.focus_changed.focused = true;

      PushPlatformEvent(event);
      return 0;
    };

    case WM_KILLFOCUS:
    {
      event.type                  = PlatformEventType::FocusChanged;
      event.focus_changed.focused = false;
      window->has_last_mouse_pos  = false;

      PushPlatformEvent(event);
      return 0;
    };

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    {
      event.type            = PlatformEventType::KeyPressed;
      event.key_pressed.key = VkToKey(wParam, lParam);

      PushPlatformEvent(event);

      // WM_SYSKEYDOWN is swallowed here so Windows never activates the system
      // menu (a lone Alt / F10 would otherwise trap us in a modal menu loop).
      // That also eats Alt+F4, so synthesize the close ourselves. Bit 29 of
      // lParam (KF_ALTDOWN) is set while Alt is held.
      if (Msg == WM_SYSKEYDOWN && wParam == VK_F4 && (lParam & (1 << 29)))
      {
        event.type = PlatformEventType::WindowClose;
        PushPlatformEvent(event);
      }

      return 0;
    };

    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
      event.type             = PlatformEventType::KeyReleased;
      event.key_released.key = VkToKey(wParam, lParam);

      PushPlatformEvent(event);
      return 0;
    };

    case WM_MOUSEMOVE:
    {
      i32 x = (i16)(lParam & 0xFFFF);
      i32 y = (i16)((lParam >> 16) & 0xFFFF);

      event.type         = PlatformEventType::MouseMove;
      event.mouse_move.x = x;
      event.mouse_move.y = y;

      if (window->has_last_mouse_pos)
      {
        event.mouse_move.delta_x = x - window->last_mouse_x;
        event.mouse_move.delta_y = y - window->last_mouse_y;
      }
      else
      {
        event.mouse_move.delta_x   = 0;
        event.mouse_move.delta_y   = 0;
        window->has_last_mouse_pos = true;
      }

      window->last_mouse_x = x;
      window->last_mouse_y = y;

      PushPlatformEvent(event);
      return 0;
    }

    case WM_LBUTTONDOWN:
    {
      event.type                        = PlatformEventType::MouseButtonPressed;
      event.mouse_button_pressed.button = MouseButton::Left;

      PushPlatformEvent(event);
      return 0;
    };

    case WM_RBUTTONDOWN:
    {
      event.type                        = PlatformEventType::MouseButtonPressed;
      event.mouse_button_pressed.button = MouseButton::Right;

      PushPlatformEvent(event);
      return 0;
    };

    case WM_MBUTTONDOWN:
    {
      event.type                        = PlatformEventType::MouseButtonPressed;
      event.mouse_button_pressed.button = MouseButton::Middle;

      PushPlatformEvent(event);
      return 0;
    };

    case WM_XBUTTONDOWN:
    {
      event.type = PlatformEventType::MouseButtonPressed;
      event.mouse_button_pressed.button =
          (HIWORD(wParam) == XBUTTON1) ? MouseButton::X1 : MouseButton::X2;

      PushPlatformEvent(event);
      return 0;
    };

    case WM_LBUTTONUP:
    {
      event.type                         = PlatformEventType::MouseButtonReleased;
      event.mouse_button_released.button = MouseButton::Left;

      PushPlatformEvent(event);
      return 0;
    };

    case WM_RBUTTONUP:
    {
      event.type                         = PlatformEventType::MouseButtonReleased;
      event.mouse_button_released.button = MouseButton::Right;

      PushPlatformEvent(event);
      return 0;
    };

    case WM_MBUTTONUP:
    {
      event.type                         = PlatformEventType::MouseButtonReleased;
      event.mouse_button_released.button = MouseButton::Middle;

      PushPlatformEvent(event);
      return 0;
    };

    case WM_XBUTTONUP:
    {
      event.type = PlatformEventType::MouseButtonReleased;
      event.mouse_button_released.button =
          (HIWORD(wParam) == XBUTTON1) ? MouseButton::X1 : MouseButton::X2;

      PushPlatformEvent(event);
      return 0;
    };

    case WM_MOUSEWHEEL:
    {
      event.type              = PlatformEventType::MouseWheel;
      event.mouse_wheel.delta = GET_WHEEL_DELTA_WPARAM(wParam);

      PushPlatformEvent(event);
      return 0;
    };
    }

    return DefWindowProc(hwnd, Msg, wParam, lParam);
  }

  static const char *class_name       = "AtreusWindow";
  static b8          class_registered = false;

  b8 CreateWindow(const WindowDescription &description, WindowHandle *out_handle)
  {
    WNDCLASSEXA window_class   = {};
    window_class.cbSize        = sizeof(window_class);
    window_class.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
    window_class.lpfnWndProc   = Win32WndProc;
    window_class.hInstance     = GetModuleHandleA(nullptr);
    window_class.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    window_class.lpszClassName = class_name;

    if (!class_registered)
    {
      if (!RegisterClassExA(&window_class)) return false;
      class_registered = true;
    }

    Win32Window *window = AllocWin32Window();
    if (window == nullptr) return false;

    DWORD dwStyle = WS_OVERLAPPEDWINDOW;
    if (!description.resizable) dwStyle &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
    if (description.borderless) dwStyle = WS_POPUP;

    RECT rect = {0, 0, (LONG)description.width, (LONG)description.height};
    AdjustWindowRect(&rect, dwStyle, false);

    HWND hwnd = CreateWindowExA(
        0,
        class_name,
        description.title,
        dwStyle,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr,
        nullptr,
        GetModuleHandleA(nullptr),
        window
    );

    if (!hwnd)
    {
      char buffer[256];

      snprintf(
          buffer,
          sizeof(buffer),
          "[Atr::Platform] Window::Create - CreateWindowExA failed: '%s' %ux%u (err=%lu)\n",
          description.title,
          description.width,
          description.height,
          GetLastError()
      );

      OutputDebugStringA(buffer);

      FreeWin32Window(window);
      return false;
    }

    window->fullscreen = false;
    window->borderless = description.borderless;
    window->dwStyle    = dwStyle;

    if (description.fullscreen)
      SetWindowFullscreen({window->index, window_generations[window->index]}, true);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    out_handle->index      = window->index;
    out_handle->generation = window_generations[window->index];
    return true;
  }

  void DestroyWindow(WindowHandle *handle)
  {
    Win32Window *window = ResolveWindow(*handle);
    if (window == nullptr) return;

    if (window && window->hwnd) ::DestroyWindow(window->hwnd);
    FreeWin32Window(window);

    *handle = InvalidWindowHandle;
  }

  PlatformEventBuffer PollEvents()
  {
    MSG Msg;
    while (PeekMessageA(&Msg, nullptr, 0, 0, PM_REMOVE))
    {
      if (Msg.message == WM_QUIT)
      {
        PlatformEvent event = {};
        event.type          = PlatformEventType::Quit;
        event.window        = InvalidWindowHandle;
        PushPlatformEvent(event);
        continue;
      }

      TranslateMessage(&Msg);
      DispatchMessageA(&Msg);
    }

    return DrainPlatformEvents();
  }

  void SetWindowTitle(WindowHandle handle, const char *title)
  {
    Win32Window *window = ResolveWindow(handle);
    if (window == nullptr) return;

    SetWindowTextA(window->hwnd, title);
  }

  void SetWindowSize(WindowHandle handle, u32 width, u32 height)
  {
    Win32Window *window = ResolveWindow(handle);
    if (window == nullptr) return;

    DWORD style = (DWORD)GetWindowLongA(window->hwnd, GWL_STYLE);
    RECT  rect  = {0, 0, (LONG)width, (LONG)height};

    AdjustWindowRect(&rect, style, FALSE);
    SetWindowPos(
        window->hwnd,
        nullptr,
        0,
        0,
        rect.right - rect.left,
        rect.bottom - rect.top,
        SWP_NOMOVE | SWP_NOZORDER
    );
  }

  void GetWindowSize(WindowHandle handle, u32 *out_width, u32 *out_height)
  {
    Win32Window *window = ResolveWindow(handle);
    if (window == nullptr) return;

    RECT rect;

    GetClientRect(window->hwnd, &rect);

    if (out_width) *out_width = (u32)(rect.right - rect.left);
    if (out_height) *out_height = (u32)(rect.bottom - rect.top);
  }

  void SetWindowFullscreen(WindowHandle handle, b8 fullscreen)
  {
    Win32Window *window = ResolveWindow(handle);
    if (window == nullptr) return;

    if ((b8)window->fullscreen == fullscreen) return;

    if (fullscreen)
    {
      window->dwStyle = (DWORD)GetWindowLongPtrA(window->hwnd, GWL_STYLE);
      GetWindowRect(window->hwnd, &window->rect);

      MONITORINFO monitor_info = {};
      monitor_info.cbSize      = sizeof(monitor_info);
      GetMonitorInfoA(MonitorFromWindow(window->hwnd, MONITOR_DEFAULTTONEAREST), &monitor_info);

      SetWindowLongPtrA(window->hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
      SetWindowPos(
          window->hwnd,
          HWND_TOP,
          monitor_info.rcMonitor.left,
          monitor_info.rcMonitor.top,
          monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
          monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
          SWP_FRAMECHANGED
      );
    }
    else
    {
      SetWindowLongA(window->hwnd, GWL_STYLE, (LONG)window->dwStyle);
      SetWindowPos(
          window->hwnd,
          nullptr,
          window->rect.left,
          window->rect.top,
          window->rect.right - window->rect.left,
          window->rect.bottom - window->rect.top,
          SWP_FRAMECHANGED | SWP_NOZORDER
      );
    }

    window->fullscreen = fullscreen;
  }

  b8 IsWindowFullscreen(WindowHandle handle)
  {
    Win32Window *window = ResolveWindow(handle);
    if (window == nullptr) return false;

    return window->fullscreen;
  }

  void SetWindowBorderless(WindowHandle handle, b8 borderless)
  {
    Win32Window *window = ResolveWindow(handle);
    if (window == nullptr) return;

    DWORD style = borderless ? WS_POPUP : WS_OVERLAPPEDWINDOW;
    if ((b8)window->borderless == borderless) return;

    style |= WS_VISIBLE;

    SetWindowLongA(window->hwnd, GWL_STYLE, (LONG)style);
    SetWindowPos(
        window->hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
    );

    window->borderless = borderless;
  }

  b8 IsWindowBorderless(WindowHandle handle)
  {
    Win32Window *window = ResolveWindow(handle);
    if (window == nullptr) return false;

    return window->borderless;
  }

  struct MonitorEnumData
  {
    MonitorInfo monitors[16];
    u32         count;
  };

  static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam)
  {
    MonitorEnumData *data = (MonitorEnumData *)lParam;
    if (data->count >= 16) return FALSE;

    MONITORINFOEXA monitor_info = {};
    monitor_info.cbSize         = sizeof(monitor_info);
    GetMonitorInfoA(hMonitor, &monitor_info);

    DEVMODEA dev_mode = {};
    dev_mode.dmSize   = sizeof(dev_mode);
    EnumDisplaySettingsA(monitor_info.szDevice, ENUM_CURRENT_SETTINGS, &dev_mode);

    MonitorInfo &info = data->monitors[data->count++];
    info.x            = monitor_info.rcMonitor.left;
    info.y            = monitor_info.rcMonitor.top;
    info.width        = (u32)(monitor_info.rcMonitor.right - monitor_info.rcMonitor.left);
    info.height       = (u32)(monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top);
    info.refresh_rate = dev_mode.dmDisplayFrequency;
    info.is_primary   = (monitor_info.dwFlags & MONITORINFOF_PRIMARY) != 0;

    UINT           dpi_x = 96, dpi_y = 96;
    static HMODULE shcore = LoadLibraryA("shcore.dll");

    if (shcore)
    {
      using FnGetDpi                   = HRESULT(WINAPI *)(HMONITOR, int, UINT *, UINT *);
      static FnGetDpi GetDpiForMonitor = (FnGetDpi)GetProcAddress(shcore, "GetDpiForMonitor");

      if (GetDpiForMonitor)
      {
        GetDpiForMonitor(hMonitor, 0, &dpi_x, &dpi_y);
      }
    }

    info.dpi          = dpi_x;
    info.scale_factor = (f32)dpi_x / 96.0f;

    return TRUE;
  }

  static MonitorEnumData EnumerateMonitors()
  {
    MonitorEnumData data = {};
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, (LPARAM)&data);
    return data;
  }

  u32 GetMonitorCount()
  {
    return EnumerateMonitors().count;
  }

  MonitorInfo GetMonitorInfo(u32 index)
  {
    MonitorEnumData data = EnumerateMonitors();

    if (index < data.count) return data.monitors[index];
    return {};
  }

  MonitorInfo GetMonitorInfoForWindow(WindowHandle handle)
  {
    Win32Window *window = ResolveWindow(handle);
    if (window == nullptr) return {};

    HMONITOR hMonitor = MonitorFromWindow(window->hwnd, MONITOR_DEFAULTTONEAREST);

    MonitorEnumData data = EnumerateMonitors();

    MONITORINFOEXA monitor_info = {};
    monitor_info.cbSize         = sizeof(monitor_info);
    GetMonitorInfoA(hMonitor, &monitor_info);

    for (u32 i = 0; i < data.count; ++i)
    {
      if (data.monitors[i].x == monitor_info.rcMonitor.left &&
          data.monitors[i].y == monitor_info.rcMonitor.top)
      {
        return data.monitors[i];
      }
    }

    return {};
  }

  void *GetNativeWindowHandle(WindowHandle handle)
  {
    Win32Window *window = ResolveWindow(handle);
    if (window == nullptr) return nullptr;

    return window ? window->hwnd : nullptr;
  }
}