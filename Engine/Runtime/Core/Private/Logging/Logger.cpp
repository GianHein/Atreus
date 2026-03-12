#include "Atr/Core/Logging/Logger.h"

#include "Atr/Platform/Console.h"
#include "Atr/Platform/Time.h"

#include <memory.h>
#include <stdarg.h>
#include <stdio.h>

#define MAX_LOG_MESSAGE_LENGTH 32000

namespace Atr
{
  b8 InitLogger()
  {
    // TODO: Create log file
    return TRUE;
  }

  void ShutdownLogger()
  {
    // TODO: Cleanup logging and write queued entries
  }

  void OutputLogMessage(LogLevel Level, const char *Message, ...)
  {
    const char *LevelMap[6] = {"[TRACE]", "[DEBUG]", "[INFO]", "[WARN]", "[ERROR]", "[FATAL]"};

    char FormatBuffer[MAX_LOG_MESSAGE_LENGTH];
    memset(FormatBuffer, 0, sizeof(FormatBuffer));

    __builtin_va_list ArgumentPointer;
    va_start(ArgumentPointer, Message);
    vsnprintf(FormatBuffer, MAX_LOG_MESSAGE_LENGTH, Message, ArgumentPointer);
    va_end(ArgumentPointer);

    DateTime Time = Platform::GetLocalTime();
    char TimeBuffer[64];
    memset(TimeBuffer, 0, sizeof(TimeBuffer));
    sprintf(TimeBuffer, "[%02u:%02u:%02u]", Time.Hour, Time.Minute, Time.Second);

    char OutBuffer[MAX_LOG_MESSAGE_LENGTH];
    memset(OutBuffer, 0, sizeof(OutBuffer));
    sprintf(OutBuffer, "%s%s: %s\n", TimeBuffer, LevelMap[(u8)Level], FormatBuffer);

    Platform::ConsoleWrite((ConsoleColor)Level, OutBuffer);
  }
}