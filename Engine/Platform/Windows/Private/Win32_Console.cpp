#include "Atr/Platform/Console.h"

#include <stdarg.h>
#include <stdio.h>

#define CONSOLE_BUFFER_SIZE 4096

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Atr::Platform
{
  // TODO: Add some color
  void ConsoleWrite(ConsoleColor Color, const char *Message, ...)
  {
    va_list Arguments;
    va_start(Arguments, Message);

    char Buffer[CONSOLE_BUFFER_SIZE];

    u32 MessageLength = vsprintf_s(Buffer, CONSOLE_BUFFER_SIZE, Message, Arguments);
    OutputDebugStringA(Buffer);

    // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    static u8 ColorMap[6] = {8, 1, 2, 6, 4, 64};
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ColorMap[Color]);

    DWORD WrittenCharacters = 0;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), Buffer, MessageLength, &WrittenCharacters, 0);

    va_end(Arguments);
  }
}