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