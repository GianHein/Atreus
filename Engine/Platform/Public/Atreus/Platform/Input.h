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
    b8 keys_pressed[(u32)Key::COUNT]  = {};
    b8 keys_held[(u32)Key::COUNT]     = {};
    b8 keys_released[(u32)Key::COUNT] = {};

    b8 mouse_pressed[(u32)MouseButton::COUNT]  = {};
    b8 mouse_held[(u32)MouseButton::COUNT]     = {};
    b8 mouse_released[(u32)MouseButton::COUNT] = {};

    i32 mouse_x           = 0;
    i32 mouse_y           = 0;
    i32 mouse_delta_x     = 0;
    i32 mouse_delta_y     = 0;
    i32 mouse_wheel_delta = 0;
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

  inline b8 IsKeyReleased(const InputState &state, Key key)
  {
    return state.keys_released[(u32)key];
  }

  inline b8 IsMouseButtonPressed(const InputState &state, MouseButton button)
  {
    return state.mouse_pressed[(u32)button];
  }

  inline b8 IsMouseButtonHeld(const InputState &state, MouseButton button)
  {
    return state.mouse_held[(u32)button];
  }

  inline b8 IsMouseButtonReleased(const InputState &state, MouseButton button)
  {
    return state.mouse_released[(u32)button];
  }

  inline void GetMousePosition(const InputState &state, i32 *x, i32 *y)
  {
    *x = state.mouse_x;
    *y = state.mouse_y;
  }

  inline void GetMouseDelta(const InputState &state, i32 *delta_x, i32 *delta_y)
  {
    *delta_x = state.mouse_delta_x;
    *delta_y = state.mouse_delta_y;
  }

  inline i32 GetMouseWheelDelta(const InputState &state)
  {
    return state.mouse_wheel_delta;
  }

  inline void ResetInputState(InputState *state)
  {
    for (u32 i = 0; i < (u32)Key::COUNT; ++i)
    {
      state->keys_pressed[i]  = false;
      state->keys_held[i]     = false;
      state->keys_released[i] = false;
    }

    for (u32 i = 0; i < (u32)MouseButton::COUNT; ++i)
    {
      state->mouse_pressed[i]  = false;
      state->mouse_held[i]     = false;
      state->mouse_released[i] = false;
    }

    state->mouse_x           = 0;
    state->mouse_y           = 0;
    state->mouse_delta_x     = 0;
    state->mouse_delta_y     = 0;
    state->mouse_wheel_delta = 0;
  }
}