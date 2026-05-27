#include "Debug.h"
#include "Platform/Window.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef CreateWindow

namespace Atr
{
  LRESULT Win32_ProcessMessage(HWND window_handle, u32 message, WPARAM w_param, LPARAM l_param);

  b8 CreateWindow(const WindowDescription &description, WindowHandle *out_handle)
  {
    HMODULE module_handle = GetModuleHandleA(0);

    HICON icon = LoadIcon(module_handle, IDI_APPLICATION);

    WNDCLASSA window_class;

    memset(&window_class, 0, sizeof(window_class));

    window_class.style = CS_DBLCLKS;
    window_class.lpfnWndProc = Win32_ProcessMessage;
    window_class.cbClsExtra = 0;
    window_class.cbWndExtra = 0;
    window_class.hInstance = module_handle;
    window_class.hIcon = icon;
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.hbrBackground = NULL;
    window_class.lpszClassName = "AtreusWindowClass";

    if (!RegisterClassA(&window_class))
    {
      MessageBoxA(0, "Window registration failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
      return FALSE;
    }

    u32 width = description.width;
    u32 height = description.height;
    u32 client_width = description.client_width;
    u32 client_height = description.client_height;

    u32 window_style = WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_CAPTION;
    u32 window_ex_style = WS_EX_APPWINDOW;

    window_style |= WS_MAXIMIZEBOX;
    window_style |= WS_MINIMIZEBOX;
    window_style |= WS_THICKFRAME;

    RECT border_rect = {0, 0, 0, 0};
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

    width += border_rect.right - border_rect.left;
    height += border_rect.bottom - border_rect.top;

    HWND window_handle = CreateWindowExA(window_ex_style, "AtreusWindowClass", description.title,
                                         window_style, 0, 0, width, height, 0, 0, module_handle, 0);

    if (window_handle == 0)
    {
      MessageBoxA(0, "Window creation failed!", "Error", MB_ICONEXCLAMATION | MB_OK);

      ATR_FATAL("Window creation failed");
      return FALSE;
    }

    *out_handle = {(u64)window_handle};

    b32 should_activate = TRUE;
    i32 show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;

    ShowWindow(window_handle, show_window_command_flags);

    return TRUE;
  }

  b8 DestroyWindow(WindowHandle *handle)
  {
    if (handle->value == 0)
    {
      return FALSE;
    }

    DestroyWindow((HWND)handle->value);
    return TRUE;
  }

  LRESULT CALLBACK Win32_ProcessMessage(HWND window_handle, u32 message, WPARAM w_param,
                                        LPARAM l_param)
  {
    switch (message)
    {
    case WM_DESTROY:
    {
      PostQuitMessage(0);
      return 0;
    }
    }

    return DefWindowProcA(window_handle, message, w_param, l_param);
  }
}