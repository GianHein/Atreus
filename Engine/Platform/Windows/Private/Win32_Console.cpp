#include "Atr/Platform/Console.h"

#include <cstdio>
#include <cstring>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Atr::Platform::Console
{
  static Style g_CurrentStyle = {};

  HANDLE GetConsoleHandle()
  {
    static HANDLE ConsoleHandle = []
    {
      // Implementation directly taken from:
      // https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences

      HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
      DWORD dwMode = 0;

      GetConsoleMode(Handle, &dwMode);

      // Enable ANSI escape sequences (required for the way we want to do colors)
      SetConsoleMode(Handle, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

      return Handle;
    }();

    return ConsoleHandle;
  }

  // Parse Color, Intensity, and Layer to correct ANSI escape sequence:
  // https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#text-formatting
  static u32 GetAnsiCode(Color NewColor, Layer NewLayer)
  {
    if (NewColor == Color::DEFAULT)
    {
      return (NewLayer == Layer::FOREGROUND) ? 39 : 49;
    }

    u32 Base = (NewLayer == Layer::FOREGROUND) ? 30 : 40;
    u32 Value = (u32)NewColor - 1; // BLACK = 1 -> 0

    u32 Code = Base + Value;

    return Code;
  }

  void SetStyle(const Style &NewStyle)
  {
    g_CurrentStyle = NewStyle;

    char Buffer[64];

    u32 ForegroundCode = GetAnsiCode(NewStyle.Foreground, Layer::FOREGROUND);
    u32 BackgroundCode = GetAnsiCode(NewStyle.Background, Layer::BACKGROUND);

    snprintf(Buffer, sizeof(Buffer), "\x1b[%d;%dm", ForegroundCode, BackgroundCode);

    DWORD Written = 0;
    WriteConsoleA(GetConsoleHandle(), Buffer, (DWORD)strlen(Buffer), &Written, nullptr);
  }

  void ResetStyle()
  {
    g_CurrentStyle = {};

    DWORD Written = 0;
    WriteConsoleA(GetConsoleHandle(), "\x1b[0m", sizeof("x1b[0m"), &Written, nullptr);
  }

  void WriteText(const char *Text)
  {
    DWORD Written = 0;
    WriteConsoleA(GetConsoleHandle(), Text, (DWORD)strlen(Text), &Written, nullptr);
  }

  void WriteLine(const char *Text, const Style LineStyle)
  {
    {
      ScopedStyle Line(LineStyle);
      WriteText(Text);
    }

    WriteText("\n");
  }

  ScopedStyle::ScopedStyle(const Style &Scoped) : m_Previous(g_CurrentStyle)
  {
    SetStyle(Scoped);
  }

  ScopedStyle::~ScopedStyle()
  {
    SetStyle(m_Previous);
  }
}