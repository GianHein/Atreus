#pragma once

#include "Atr/Definitions.h"
#include "Atr/Platform/Types/WindowDescription.h"
#include "Atr/Platform/Types/WindowHandle.h"

namespace Atr::Platform
{
  b8 CreateWindow(WindowDescription &Desc);
  b8 DestroyWindow(WindowHandle Window);
}