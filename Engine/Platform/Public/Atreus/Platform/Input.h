#pragma once

#include "Atreus/Definitions.h"
#include "Atreus/Platform/Window.h"

namespace Atreus
{
  struct PlatformEvent;

  enum class Key : u8
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
    Escape,
    Enter,
    Space,
    Tab,
    Backspace,
    Delete,
    Insert,
    Home,
    End,
    PageUp,
    PageDown,
    Left,
    Right,
    Up,
    Down,

    // Modifiers
    LeftShift,
    RightShift,
    LeftCtrl,
    RightCtrl,
    LeftAlt,
    RightAlt,
    LeftSuper,
    RightSuper,

    // Locks
    CapsLock,
    NumLock,
    ScrollLock,

    // Punctuation
    Semicolon,
    Equals,
    Comma,
    Minus,
    Period,
    Slash,
    Backtick,
    LeftBracket,
    RightBracket,
    Backslash,
    Apostrophe,

    // System
    PrintScreen,
    Pause,

    COUNT
  };

  enum class MouseButton : u8
  {
    Unknown = 0,

    Left,
    Right,
    Middle,

    X1,
    X2,

    COUNT
  };

  struct InputState
  {
    b8 keys_down[static_cast<usize>(Key::COUNT)];
    b8 keys_down_previous[static_cast<usize>(Key::COUNT)];

    b8 mouse_down[static_cast<usize>(MouseButton::COUNT)];
    b8 mouse_down_previous[static_cast<usize>(MouseButton::COUNT)];

    i32 mouse_x;
    i32 mouse_y;

    i32 mouse_delta_x;
    i32 mouse_delta_y;

    i32 scroll_delta;
  };

  void UpdateInput(WindowHandle window, const PlatformEvent *events, u32 event_count);

  b8 IsKeyDown(WindowHandle window, Key key);
  b8 WasKeyPressed(WindowHandle window, Key key);
  b8 WasKeyReleased(WindowHandle window, Key key);

  b8 IsMouseButtonDown(WindowHandle window, MouseButton button);
  b8 WasMouseButtonPressed(WindowHandle window, MouseButton button);
  b8 WasMouseButtonReleased(WindowHandle window, MouseButton button);

  void GetMousePosition(WindowHandle window, i32 *out_x, i32 *out_y);
  void GetMouseDelta(WindowHandle window, i32 *out_delta_x, i32 *out_delta_y);
  i32  GetScrollDelta(WindowHandle window);
}