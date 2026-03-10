#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef CreateWindow

#include "Atr/Platform/Window.h"

namespace Atr::Platform
{
  b8 CreateWindow(WindowDescription *Desc, const char *WindowTitle)
  {
    HMODULE ProcessHandle = GetModuleHandleA(0);

    HICON Icon = LoadIcon(ProcessHandle, IDI_APPLICATION);
    WNDCLASSA WindowClass;

    memset(&WindowClass, 0, sizeof(WindowClass));

    WindowClass.style = CS_DBLCLKS;
    WindowClass.cbClsExtra = 0;
    WindowClass.cbWndExtra = 0;
    WindowClass.hInstance = ProcessHandle;
    WindowClass.hIcon = Icon;
    WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW); // NULL; // Manage cursor manually
    WindowClass.hbrBackground = NULL;                  // Transparent
    WindowClass.lpszClassName = "AtreusWindowClass";

    if (!RegisterClassA(&WindowClass))
    {
      MessageBoxA(0, "Window registration failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
      return FALSE;
    }

    u32 ClientX, ClientY;
    u32 ClientWidth = Desc->Width;
    u32 ClientHeight = Desc->Height;

    u32 WindowX = ClientX;
    u32 WindowY = ClientY;
    u32 WindowWidth = ClientWidth;
    u32 WindowHeight = ClientHeight;

    u32 WindowStyle = WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_CAPTION;
    u32 WindowExStyle = WS_EX_APPWINDOW;

    WindowStyle |= WS_MAXIMIZEBOX;
    WindowStyle |= WS_MINIMIZEBOX;
    WindowStyle |= WS_THICKFRAME;

    RECT BorderRect = {0, 0, 0, 0};
    AdjustWindowRectEx(&BorderRect, WindowStyle, 0, WindowExStyle);

    WindowX += BorderRect.left;
    WindowY += BorderRect.top;
    WindowWidth += BorderRect.right - BorderRect.left;
    WindowHeight += BorderRect.bottom - BorderRect.top;

    HWND WindowHandle =
        CreateWindowExA(WindowExStyle, "AtreusWindowClass", WindowTitle, WindowStyle, WindowX,
                        WindowY, WindowWidth, WindowHeight, 0, 0, ProcessHandle, 0);

    if (WindowHandle == 0)
    {
      MessageBoxA(0, "Window creation failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
      return FALSE;
    }
    else
    {
      Desc->Window.Value = (u64)WindowHandle;
    }

    b32 ShouldActivate = TRUE; // TODO: If the window should not accept input, this should be FALSE
    i32 ShowWindowCommandFlags = ShouldActivate ? SW_SHOW : SW_SHOWNOACTIVATE;
    // If initially minimized, use SW_MINIMIZE : SW_SHOWNOACTIVATE;
    // If initially maximized, use SW_SHOWMAXIMIZED : SW_MAXIMIZE;
    ShowWindow(WindowHandle, ShowWindowCommandFlags);

    return TRUE;
  }

  b8 DestroyWindow(WindowHandle Window)
  {
    ::DestroyWindow((HWND)Window.Value);
    return TRUE;
  }
}