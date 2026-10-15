#include "Atreus/Platform/Window.h"

namespace
{
  const char *const key_names[] = {
      "Unknown",
      "A",
      "B",
      "C",
      "D",
      "E",
      "F",
      "G",
      "H",
      "I",
      "J",
      "K",
      "L",
      "M",
      "N",
      "O",
      "P",
      "Q",
      "R",
      "S",
      "T",
      "U",
      "V",
      "W",
      "X",
      "Y",
      "Z",
      "0",
      "1",
      "2",
      "3",
      "4",
      "5",
      "6",
      "7",
      "8",
      "9",
      "F1",
      "F2",
      "F3",
      "F4",
      "F5",
      "F6",
      "F7",
      "F8",
      "F9",
      "F10",
      "F11",
      "F12",
      "Left",
      "Right",
      "Up",
      "Down",
      "Space",
      "Enter",
      "Escape",
      "Tab",
      "Backspace",
      "Delete",
      "Insert",
      "Home",
      "End",
      "PageUp",
      "PageDown",
      "LeftShift",
      "RightShift",
      "LeftCtrl",
      "RightCtrl",
      "LeftAlt",
      "RightAlt",
      "LeftSuper",
      "RightSuper",
      "Comma",
      "Period",
      "Minus",
      "Equals",
      "Slash",
      "Backslash",
      "Semicolon",
      "Apostrophe",
      "LeftBracket",
      "RightBracket",
      "GraveAccent",
      "Numpad0",
      "Numpad1",
      "Numpad2",
      "Numpad3",
      "Numpad4",
      "Numpad5",
      "Numpad6",
      "Numpad7",
      "Numpad8",
      "Numpad9",
      "NumpadAdd",
      "NumpadSub",
      "NumpadMul",
      "NumpadDiv",
      "NumpadEnter",
      "NumpadDecimal",
      "CapsLock",
      "NumLock",
      "ScrollLock",
      "PrintScreen",
      "Pause",
  };

  static_assert(
      ATR_ARRAY_COUNT(key_names) == static_cast<usize>(KeyCode::Count),
      "Key name table out of sync with KeyCode"
  );
}

const char *KeyCodeName(KeyCode key)
{
  u8 index = static_cast<u8>(key);
  return index < ATR_ARRAY_COUNT(key_names) ? key_names[index] : "Unknown";
}

KeyCode KeyCodeFromName(const char *name)
{
  for (usize i = 0; i < ATR_ARRAY_COUNT(key_names); ++i)
  {
    if (name == key_names[i]) return static_cast<KeyCode>(i);
  }

  return KeyCode::Unknown;
}