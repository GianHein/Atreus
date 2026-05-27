#pragma once

#include "Definitions.h"

namespace Atr
{
  class WindowState
  {
  public:
    WindowState() = default;
    ~WindowState() = default;

    static WindowState GetState();

  private:
  };
}