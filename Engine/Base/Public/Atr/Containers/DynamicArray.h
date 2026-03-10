#pragma once

#include "Atr/Definitions.h"
#include "Atr/Memory/IAllocator.h"

#include <vector>

namespace Atr
{
  template <typename T> using DynamicArray = std::vector<T>;
}