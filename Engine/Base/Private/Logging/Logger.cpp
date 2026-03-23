#include "Atr/Logging/Logger.h"

#include <cstdarg>
#include <cstdio>

#define MAX_LOG_MESSAGE_LENGTH 32000

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

      printf("%s [%s:%u][%s]: %s\n", Levels[(u8)Level], Context.File, Context.Line,
             Context.Function, FormatBuffer);
    }
  }
}