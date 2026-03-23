#include "Atr/Logging/Logger.h"

#include <cstdarg>
#include <cstdio>

#define MAX_LOG_MESSAGE_LENGTH 2048

namespace Atr
{
  static LogCallback s_LogCallback = nullptr;

  void SetLogCallback(LogCallback Callback)
  {
    s_LogCallback = Callback;
  }

  void Log(LogLevel Level, const LogContext &Context, const char *Message, ...)
  {
    char FormatBuffer[MAX_LOG_MESSAGE_LENGTH];

    va_list ArgumentPointer;
    va_start(ArgumentPointer, Message);
    vsnprintf(FormatBuffer, MAX_LOG_MESSAGE_LENGTH, Message, ArgumentPointer);
    va_end(ArgumentPointer);

    if (s_LogCallback)
    {
      s_LogCallback(Level, Context, FormatBuffer);
    }
    else
    {
      static const char *Levels[6] = {"[TRACE]", "[DEBUG]", "[INFO]",
                                      "[WARN]",  "[ERROR]", "[FATAL]"};

      static const char *LevelColors[6] = {
          "\x1b[30m", // TRACE - Black
          "\x1b[34m", // DEBUG - Blue
          "\x1b[32m", // INFO  - Green
          "\x1b[33m", // WARN  - Yellow (Red + Green)
          "\x1b[31m", // ERROR - Red
          "\x1b[41m"  // FATAL - Red Background
      };

      printf("%s%s [%s:%u][%s]: %s\x1b[0m\n", LevelColors[(u8)Level], Levels[(u8)Level],
             Context.File, Context.Line, Context.Function, FormatBuffer);
    }
  }
}