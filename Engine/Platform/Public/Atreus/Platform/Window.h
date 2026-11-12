#pragma once

#include "Atreus/Platform/Input.h"
#include "Atreus/Types.h"

// Window events

enum class WindowEventType : u8
{
  None = 0,

  // App-level quit
  QuitRequested,

  // Window specific events
  CloseRequested,
  Shown,
  Hidden,
  Moved,
  Resized,
  Minimized,
  Maximized,
  Restored,
  MouseEntered,
  MouseLeft,
  FocusGained,
  FocusLost,
  FullscreenEntered,
  FullscreenLeft,

  // Input related events
  KeyPressed,
  KeyReleased,
  MouseMoved,
  MousePressed,
  MouseReleased,
  MouseWheel,
};

struct WindowResizedData
{
  u32 width;
  u32 height;
};

struct KeyPressedData
{
  KeyCode key;
  bool    repeat;
};

struct KeyReleasedData
{
  KeyCode key;
};

struct MouseMovedData
{
  i32 x;
  i32 y;
};

struct MousePressedData
{
  MouseButton button;
  i32         x;
  i32         y;
};

struct MouseReleasedData
{
  MouseButton button;
  i32         x;
  i32         y;
};

struct MouseWheelData
{
  f32 x;
  f32 y;
};

struct WindowEvent
{
  WindowEventType type;

  union
  {
    WindowResizedData window_resized;
    KeyPressedData    key_pressed;
    KeyReleasedData   key_released;
    MouseMovedData    mouse_moved;
    MousePressedData  mouse_pressed;
    MouseReleasedData mouse_released;
    MouseWheelData    mouse_wheel;
  };
};

struct Window;

struct WindowDesc
{
  const char *title      = "Atreus";
  u32         width      = 1280;
  u32         height     = 720;
  bool        resizable  = true;
  bool        visible    = true;
  bool        fullscreen = false;
  bool        borderless = false;
};

ATR_API Window *WindowCreate(const WindowDesc &desc);
ATR_API void    WindowDestroy(Window *window);

ATR_API void WindowPumpMessages();
ATR_API bool WindowPollEvent(Window *window, WindowEvent *out_event);

ATR_API void WindowGetClientSize(const Window *window, u32 *out_width, u32 *out_height);
ATR_API void WindowSetTitle(Window *window, const char *title);
ATR_API void WindowSetFullscreen(Window *window, bool fullscreen);
ATR_API bool WindowIsFullscreen(const Window *window);
ATR_API void WindowSetBorderless(Window *window, bool borderless);
ATR_API bool WindowIsBorderless(const Window *window);
ATR_API bool WindowIsFocused(const Window *window);
ATR_API bool WindowIsClosing(const Window *window);

struct NativeWindowHandle
{
  void *display;
  u64   window;
};

ATR_API NativeWindowHandle WindowGetNativeHandle(Window *window);