#pragma once

#include "Atreus/Types.h"

// Input codes

enum class KeyCode : u8
{
  Unknown = 0,

  // Letters
  A,
  B,
  C,
  D,
  E,
  F,
  G,
  H,
  I,
  J,
  K,
  L,
  M,
  N,
  O,
  P,
  Q,
  R,
  S,
  T,
  U,
  V,
  W,
  X,
  Y,
  Z,

  // Top-row digits
  Num0,
  Num1,
  Num2,
  Num3,
  Num4,
  Num5,
  Num6,
  Num7,
  Num8,
  Num9,

  // Function keys
  F1,
  F2,
  F3,
  F4,
  F5,
  F6,
  F7,
  F8,
  F9,
  F10,
  F11,
  F12,

  // Control
  Left,
  Right,
  Up,
  Down,
  Space,
  Enter,
  Escape,
  Tab,
  Backspace,
  Delete,
  Insert,
  Home,
  End,
  PageUp,
  PageDown,

  // Modifiers
  LeftShift,
  RightShift,
  LeftCtrl,
  RightCtrl,
  LeftAlt,
  RightAlt,

  LeftSuper,
  RightSuper,

  // Punctuation
  Comma,
  Period,
  Minus,
  Equals,
  Slash,
  Backslash,
  Semicolon,
  Apostrophe,
  LeftBracket,
  RightBracket,
  GraveAccent,

  // Numpad
  Numpad0,
  Numpad1,
  Numpad2,
  Numpad3,
  Numpad4,
  Numpad5,
  Numpad6,
  Numpad7,
  Numpad8,
  Numpad9,
  NumpadAdd,
  NumpadSub,
  NumpadMul,
  NumpadDiv,
  NumpadEnter,
  NumpadDecimal,

  // Locks
  CapsLock,
  NumLock,
  ScrollLock,

  // System
  PrintScreen,
  Pause,

  Count,
};

enum class MouseButton : u8
{
  Left = 0,
  Right,
  Middle,

  X1,
  X2,

  Count,
};

ATR_API const char *KeyCodeName(KeyCode key);
ATR_API KeyCode     KeyCodeFromName(const char *name);

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
ATR_API void WindowSetCursorVisible(Window *window, bool visible);

struct NativeWindowHandle
{
  void *display;
  u64   window;
};

ATR_API NativeWindowHandle WindowGetNativeHandle(Window *window);

// Gamepads

enum class GamepadButton : u8
{
  DpadUp,
  DpadDown,
  DpadLeft,
  DpadRight,
  Start,
  Back,
  LeftStick,
  RightStick,
  LeftShoulder,
  RightShoulder,
  A,
  B,
  X,
  Y,
  Count,
};

struct GamepadState
{
  bool connected;
  u32  buttons; // Bit i set when GamepadButton i is down
  f32  left_x;  // Sticks in [-1, 1] with dead zone applied; triggers in [0, 1]
  f32  left_y;
  f32  right_x;
  f32  right_y;
  f32  left_trigger;
  f32  right_trigger;
};

constexpr u32 MAX_GAMEPADS = 4;

ATR_API void GamepadPoll(u32 index, GamepadState *out_state);