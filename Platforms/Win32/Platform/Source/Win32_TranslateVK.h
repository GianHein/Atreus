#pragma once

#include "Atreus/Definitions.h"
#include "Atreus/Platform/Input.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

namespace Atreus
{
  Key TranslateVK(WPARAM wParam, LPARAM lParam);
}
