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

ATR_API Window *window_create(const WindowDesc &desc);
ATR_API void    window_destroy(Window *window);

ATR_API void window_pump_messages();
ATR_API bool window_poll_event(Window *window, WindowEvent *out_event);

ATR_API void window_get_client_size(const Window *window, u32 *out_width, u32 *out_height);
ATR_API void window_set_title(Window *window, const char *title);
ATR_API void window_set_fullscreen(Window *window, bool fullscreen);
ATR_API bool window_is_fullscreen(const Window *window);
ATR_API void window_set_borderless(Window *window, bool borderless);
ATR_API bool window_is_borderless(const Window *window);
ATR_API bool window_is_focused(const Window *window);
ATR_API bool window_is_closing(const Window *window);

struct NativeWindowHandle
{
  void *display;
  u64   window;
};

ATR_API NativeWindowHandle window_get_native_handle(Window *window);