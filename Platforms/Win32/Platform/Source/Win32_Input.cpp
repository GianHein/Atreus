#include "Atreus/Platform/Input.h"
#include "Win32_TranslateVK.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

namespace Atreus
{
  static Key vk_to_key[256];
  static b8  vk_to_key_initialized = false;

  static void InitializeVKToKey()
  {
    for (u32 i = 0; i < 256; ++i)
    {
      vk_to_key[i] = Key::Unknown;
    }

    for (u32 c = 'A'; c <= 'Z'; ++c)
    {
      vk_to_key[c] = (Key)((u32)Key::A + (c - 'A'));
    }

    for (u32 c = '0'; c <= '9'; ++c)
    {
      vk_to_key[c] = (Key)((u32)Key::Num0 + (c - '0'));
    }

    for (u32 i = 0; i <= 9; ++i)
    {
      vk_to_key[VK_NUMPAD0 + i] = (Key)((u32)Key::Numpad0 + i);
    }

    vk_to_key[VK_ADD]      = Key::NumpadAdd;
    vk_to_key[VK_SUBTRACT] = Key::NumpadSub;
    vk_to_key[VK_MULTIPLY] = Key::NumpadMul;
    vk_to_key[VK_DIVIDE]   = Key::NumpadDiv;
    vk_to_key[VK_DECIMAL]  = Key::NumpadDecimal;

    for (u32 i = 0; i <= 11; ++i)
    {
      vk_to_key[VK_F1 + i] = (Key)((u32)Key::F1 + i);
    }

    vk_to_key[VK_ESCAPE] = Key::Escape;
    vk_to_key[VK_SPACE]  = Key::Space;
    vk_to_key[VK_TAB]    = Key::Tab;
    vk_to_key[VK_BACK]   = Key::Backspace;
    vk_to_key[VK_DELETE] = Key::Delete;
    vk_to_key[VK_INSERT] = Key::Insert;
    vk_to_key[VK_HOME]   = Key::Home;
    vk_to_key[VK_END]    = Key::End;
    vk_to_key[VK_PRIOR]  = Key::PageUp;
    vk_to_key[VK_NEXT]   = Key::PageDown;
    vk_to_key[VK_LEFT]   = Key::Left;
    vk_to_key[VK_RIGHT]  = Key::Right;
    vk_to_key[VK_UP]     = Key::Up;
    vk_to_key[VK_DOWN]   = Key::Down;

    vk_to_key[VK_SHIFT]    = Key::LeftShift;
    vk_to_key[VK_LSHIFT]   = Key::LeftShift;
    vk_to_key[VK_RSHIFT]   = Key::RightShift;
    vk_to_key[VK_CONTROL]  = Key::LeftCtrl;
    vk_to_key[VK_LCONTROL] = Key::LeftCtrl;
    vk_to_key[VK_RCONTROL] = Key::RightCtrl;
    vk_to_key[VK_MENU]     = Key::LeftAlt;
    vk_to_key[VK_LMENU]    = Key::LeftAlt;
    vk_to_key[VK_RMENU]    = Key::RightAlt;
    vk_to_key[VK_LWIN]     = Key::LeftSuper;
    vk_to_key[VK_RWIN]     = Key::RightSuper;

    vk_to_key[VK_CAPITAL] = Key::CapsLock;
    vk_to_key[VK_NUMLOCK] = Key::NumLock;
    vk_to_key[VK_SCROLL]  = Key::ScrollLock;

    vk_to_key[VK_OEM_1]      = Key::Semicolon;
    vk_to_key[VK_OEM_PLUS]   = Key::Equals;
    vk_to_key[VK_OEM_COMMA]  = Key::Comma;
    vk_to_key[VK_OEM_MINUS]  = Key::Minus;
    vk_to_key[VK_OEM_PERIOD] = Key::Period;
    vk_to_key[VK_OEM_2]      = Key::Slash;
    vk_to_key[VK_OEM_3]      = Key::Backtick;
    vk_to_key[VK_OEM_4]      = Key::LeftBracket;
    vk_to_key[VK_OEM_6]      = Key::RightBracket;
    vk_to_key[VK_OEM_5]      = Key::Backslash;
    vk_to_key[VK_OEM_7]      = Key::Apostrophe;

    vk_to_key[VK_SNAPSHOT] = Key::PrintScreen;
    vk_to_key[VK_PAUSE]    = Key::Pause;

    vk_to_key_initialized = true;
  }

  Key TranslateVK(WPARAM wParam, LPARAM lParam)
  {
    if (!vk_to_key_initialized) InitializeVKToKey();

    b8   extended_key = ((lParam >> 24) & 1);
    UINT scancode     = (lParam >> 16) & 0xFF;

    switch (wParam)
    {
    case VK_CONTROL:
      return extended_key ? Key::RightCtrl : Key::LeftCtrl;

    case VK_MENU:
      return extended_key ? Key::RightAlt : Key::LeftAlt;

    case VK_RETURN:
      return extended_key ? Key::NumpadEnter : Key::Enter;

    case VK_SHIFT:
    {
      return (
          MapVirtualKeyA(scancode, MAPVK_VSC_TO_VK_EX) == VK_LSHIFT ? Key::LeftShift
                                                                    : Key::RightShift
      );
    }

    default:
      return (wParam < 256) ? vk_to_key[wParam] : Key::Unknown;
    }
  }
}