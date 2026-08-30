#pragma once

#include "Atreus/Definitions.h"
#include "Atreus/Platform/Input.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

namespace Atreus
{
  // Translates a WM_KEYDOWN/WM_KEYUP/WM_SYSKEYDOWN/WM_SYSKEYUP virtual-key code + lParam
  // into an Atreus::Key, resolving the VK_CONTROL/VK_MENU/VK_RETURN/VK_SHIFT ambiguities
  Key VkToKey(WPARAM wParam, LPARAM lParam);
}
