#include "Atreus/Platform/Window.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "Atreus/Platform/PlatformEvent.h"
#include "Win32_TranslateVK.h"

#include <Windows.h>
#include <windowsx.h>
#undef CreateWindow

#include <cstdio>
#include <cstring>

namespace Atreus
{
  struct Win32_Window
  {
    u32   index              = UINT32_MAX;
    HWND  hwnd               = nullptr;
    i32   last_mouse_x       = 0;
    i32   last_mouse_y       = 0;
    b8    has_mouse_position = false;
    b8    fullscreen         = false;
    b8    borderless         = false;
    DWORD dwStyle            = 0;
    RECT  rect               = {};
  };

  static Win32_Window window_pool[MAX_WINDOWS];
  static b8           window_pool_used[MAX_WINDOWS];
  static u32          window_generations[MAX_WINDOWS];

  static Win32_Window *AllocateWin32Window()
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

  static void FreeWin32Window(Win32_Window *window)
  {
    for (u32 i = 0; i < MAX_WINDOWS; ++i)
    {
      if (&window_pool[i] == window)
      {
        SetWindowLongPtrA(window->hwnd, GWLP_USERDATA, 0);

        *window             = {};
        window_pool_used[i] = false;
        window_generations[i]++;

        return;
      }
    }
  }

  static Win32_Window *ResolveWindow(WindowHandle handle)
  {
    if (handle.index >= MAX_WINDOWS) return nullptr;
    if (!window_pool_used[handle.index]) return nullptr;
    if (window_generations[handle.index] != handle.generation) return nullptr;

    return &window_pool[handle.index];
  }

  constexpr u32        MAX_PLATFORM_EVENTS = 512;
  static PlatformEvent event_queue[MAX_PLATFORM_EVENTS];
  static u32           event_queue_count = 0;

  static void PushEvent(const PlatformEvent &event)
  {
    if (event_queue_count >= MAX_PLATFORM_EVENTS) return;
    event_queue[event_queue_count++] = event;
  }

  static LRESULT Win32_WndProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
  {
    Win32_Window *window = (Win32_Window *)GetWindowLongPtrA(hwnd, GWLP_USERDATA);

    if (window == nullptr)
    {
      if (Msg == WM_NCCREATE)
      {
        CREATESTRUCTA *create_struct = (CREATESTRUCTA *)lParam;
        window                       = (Win32_Window *)create_struct->lpCreateParams;
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)window);
        window->hwnd = hwnd;
      }

      return DefWindowProcA(hwnd, Msg, wParam, lParam);
    }

    WindowHandle  handle = {window->index, window_generations[window->index]};
    PlatformEvent event  = {};
    event.window         = handle;

    switch (Msg)
    {
    case WM_CLOSE:
    {
      event.type         = PlatformEventType::WindowClose;
      event.window_close = {};
      PushEvent(event);
      return 0;
    }

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    {
      event.type          = PlatformEventType::KeyPress;
      event.key_press.key = TranslateVK(wParam, lParam);
      PushEvent(event);
      return 0;
    }

    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
      event.type            = PlatformEventType::KeyRelease;
      event.key_release.key = TranslateVK(wParam, lParam);
      PushEvent(event);
      return 0;
    }

    case WM_LBUTTONDOWN:
    {
      event.type                      = PlatformEventType::MouseButtonPress;
      event.mouse_button_press.button = MouseButton::Left;
      PushEvent(event);
      return 0;
    }

    case WM_LBUTTONUP:
    {
      event.type                        = PlatformEventType::MouseButtonRelease;
      event.mouse_button_release.button = MouseButton::Left;
      PushEvent(event);
      return 0;
    }

    case WM_RBUTTONDOWN:
    {
      event.type                      = PlatformEventType::MouseButtonPress;
      event.mouse_button_press.button = MouseButton::Right;
      PushEvent(event);
      return 0;
    }

    case WM_RBUTTONUP:
    {
      event.type                        = PlatformEventType::MouseButtonRelease;
      event.mouse_button_release.button = MouseButton::Right;
      PushEvent(event);
      return 0;
    }

    case WM_MBUTTONDOWN:
    {
      event.type                      = PlatformEventType::MouseButtonPress;
      event.mouse_button_press.button = MouseButton::Middle;
      PushEvent(event);
      return 0;
    }

    case WM_MBUTTONUP:
    {
      event.type                        = PlatformEventType::MouseButtonRelease;
      event.mouse_button_release.button = MouseButton::Middle;
      PushEvent(event);
      return 0;
    }

    case WM_XBUTTONDOWN:
    {
      event.type = PlatformEventType::MouseButtonPress;
      event.mouse_button_press.button =
          GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? MouseButton::X1 : MouseButton::X2;
      PushEvent(event);
      return TRUE; // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-xbuttondown#return-value
    }

    case WM_XBUTTONUP:
    {
      event.type = PlatformEventType::MouseButtonRelease;
      event.mouse_button_release.button =
          GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? MouseButton::X1 : MouseButton::X2;
      PushEvent(event);
      return TRUE; // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-xbuttonup#return-value
    }

    case WM_MOUSEMOVE:
    {
      event.type         = PlatformEventType::MouseMove;
      event.mouse_move.x = GET_X_LPARAM(lParam);
      event.mouse_move.y = GET_Y_LPARAM(lParam);

      if (!window->has_mouse_position)
      {
        event.mouse_move.delta_x = 0;
        event.mouse_move.delta_y = 0;

        window->last_mouse_x       = event.mouse_move.x;
        window->last_mouse_y       = event.mouse_move.y;
        window->has_mouse_position = true;
        PushEvent(event);
        return 0;
      }

      event.mouse_move.delta_x = event.mouse_move.x - window->last_mouse_x;
      event.mouse_move.delta_y = event.mouse_move.y - window->last_mouse_y;
      window->last_mouse_x     = event.mouse_move.x;
      window->last_mouse_y     = event.mouse_move.y;
      PushEvent(event);
      return 0;
    }

    case WM_MOUSEWHEEL:
    {
      event.type              = PlatformEventType::MouseWheel;
      event.mouse_wheel.delta = GET_WHEEL_DELTA_WPARAM(wParam);
      PushEvent(event);
      return 0;
    }

    case WM_SETFOCUS:
    {
      event.type                 = PlatformEventType::FocusChange;
      event.focus_change.focused = true;
      PushEvent(event);
      return 0;
    }

    case WM_KILLFOCUS:
    {
      event.type                 = PlatformEventType::FocusChange;
      event.focus_change.focused = false;
      window->has_mouse_position = false;
      PushEvent(event);
      return 0;
    }

    case WM_SIZE:
    {
      event.type                 = PlatformEventType::WindowResize;
      event.window_resize.width  = LOWORD(lParam);
      event.window_resize.height = HIWORD(lParam);
      PushEvent(event);
      return 0;
    }
    }

    return DefWindowProcA(hwnd, Msg, wParam, lParam);
  }

  u32 PollEvents(PlatformEvent *out_events, u32 max_events)
  {
    MSG Msg;

    while (PeekMessageA(&Msg, nullptr, 0, 0, PM_REMOVE))
    {
      switch (Msg.message)
      {
      case WM_QUIT:
      {
        PlatformEvent event = {};
        event.type          = PlatformEventType::Quit;
        event.window        = InvalidWindowHandle;
        event.quit          = {};
        PushEvent(event);
        break;
      }
      }

      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
    }

    u32 events_to_copy = (event_queue_count < max_events) ? event_queue_count : max_events;
    memcpy(out_events, event_queue, events_to_copy * sizeof(PlatformEvent));
    event_queue_count = 0;

    return events_to_copy;
  }

  b8 IsValidWindow(WindowHandle handle)
  {
    return ResolveWindow(handle) != nullptr;
  }

  b8 IsSameWindow(WindowHandle handle_a, WindowHandle handle_b)
  {
    return handle_a.index == handle_b.index && handle_a.generation == handle_b.generation;
  }

  static const char *class_name       = "AtreusWindow";
  static b8          class_registered = false;

  b8 CreateWindow(const WindowDescription &description, WindowHandle *out_handle)
  {
    WNDCLASSEXA window_class   = {};
    window_class.cbSize        = sizeof(window_class);
    window_class.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
    window_class.lpfnWndProc   = Win32_WndProc;
    window_class.hInstance     = GetModuleHandleA(nullptr);
    window_class.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    window_class.lpszClassName = class_name;

    if (!class_registered)
    {
      if (!RegisterClassExA(&window_class)) return false;
      class_registered = true;
    }

    Win32_Window *window = AllocateWin32Window();
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
    Win32_Window *window = ResolveWindow(*handle);
    if (window == nullptr) return;

    if (window && window->hwnd) ::DestroyWindow(window->hwnd);
    FreeWin32Window(window);

    *handle = InvalidWindowHandle;
  }

  void SetWindowTitle(WindowHandle handle, const char *title)
  {
    Win32_Window *window = ResolveWindow(handle);
    if (window == nullptr) return;

    SetWindowTextA(window->hwnd, title);
  }

  void SetWindowSize(WindowHandle handle, u32 width, u32 height)
  {
    Win32_Window *window = ResolveWindow(handle);
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
    Win32_Window *window = ResolveWindow(handle);
    if (window == nullptr) return;

    RECT rect;

    GetClientRect(window->hwnd, &rect);

    if (out_width) *out_width = (u32)(rect.right - rect.left);
    if (out_height) *out_height = (u32)(rect.bottom - rect.top);
  }

  void SetWindowFullscreen(WindowHandle handle, b8 fullscreen)
  {
    Win32_Window *window = ResolveWindow(handle);
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
    Win32_Window *window = ResolveWindow(handle);
    if (window == nullptr) return false;

    return window->fullscreen;
  }

  void SetWindowBorderless(WindowHandle handle, b8 borderless)
  {
    Win32_Window *window = ResolveWindow(handle);
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
    Win32_Window *window = ResolveWindow(handle);
    if (window == nullptr) return false;

    return window->borderless;
  }

  struct MonitorEnumData
  {
    MonitorInfo monitors[16] = {};
    u32         count        = 0;
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
    Win32_Window *window = ResolveWindow(handle);
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
    Win32_Window *window = ResolveWindow(handle);
    if (window == nullptr) return nullptr;

    return window ? window->hwnd : nullptr;
  }
}