#pragma once

#include "Atreus/Definitions.h"
#include "Atreus/Platform/Window.h"

namespace Atreus
{
  struct PlatformEventBuffer;

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
    Left = 0,
    Right,
    Middle,

    X1,
    X2,

    COUNT
  };

  struct InputState
  {
    b8 keys_pressed[(u32)Key::COUNT];
    b8 keys_held[(u32)Key::COUNT];
    b8 keys_released[(u32)Key::COUNT];

    b8 mouse_pressed[(u32)MouseButton::COUNT];
    b8 mouse_held[(u32)MouseButton::COUNT];
    b8 mouse_released[(u32)MouseButton::COUNT];

    i32 mouse_x, mouse_y;
    i32 mouse_delta_x, mouse_delta_y;
    i32 mouse_wheel_delta;
  };

  void UpdateInputState(
      InputState *state, const PlatformEventBuffer &events, WindowHandle target_window
  );

  inline b8 IsKeyPressed(const InputState &state, Key key)
  {
    return state.keys_pressed[(u32)key];
  }

  inline b8 IsKeyHeld(const InputState &state, Key key)
  {
    return state.keys_held[(u32)key];
  }

  // ...
}