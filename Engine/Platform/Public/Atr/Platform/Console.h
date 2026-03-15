#pragma once

#include "Atr/Definitions.h"
#include "Atr/Platform/Types/ConsoleColor.h"

namespace Atr::Platform
{
  void WriteConsole(ConsoleColor Color, const char *Message);
}