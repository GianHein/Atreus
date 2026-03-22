#pragma once

#include "Atr/Definitions.h"

namespace Atr::Platform::Console
{
  enum class Color : u8
  {
    DEFAULT = 0,
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE
  };

  enum class Layer : u8
  {
    FOREGROUND = 0,
    BACKGROUND
  };

  struct Style
  {
    Color Foreground = Color::DEFAULT;
    Color Background = Color::DEFAULT;
  };

  void SetStyle(const Style &NewStyle);
  void ResetStyle();

  void WriteText(const char *Text);
  void WriteLine(const char *Text, const Style LineStyle = {});

  class ScopedStyle
  {
  public:
    ScopedStyle(const Style &Scoped);
    ~ScopedStyle();

    ScopedStyle(const ScopedStyle &) = delete;
    ScopedStyle &operator=(const ScopedStyle &) = delete;

  private:
    Style m_Previous;
  };
}