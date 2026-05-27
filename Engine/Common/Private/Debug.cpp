#include "Debug.h"

#include <stdarg.h>
#include <stdio.h>

namespace Atr
{
  static LogCallback log_callback = nullptr;

  void SetLogCallback(LogCallback callback)
  {
    log_callback = callback;
  }

  void LogMessage(LogLevel level, const LogContext &context, const char *message, ...)
  {
    char buffer[ATR_MAX_LOG_MESSAGE_LENGTH];

    va_list args;
    va_start(args, message);
    vsnprintf(buffer, ATR_MAX_LOG_MESSAGE_LENGTH, message, args);
    va_end(args);

    if (log_callback)
    {
      log_callback(level, context, buffer);
    }
    else
    {
      printf("[%u] %s\n", (u8)level, buffer);
    }
  }

  void LogAssertionFailure(const char *expression, const char *message, const LogContext &context)
  {
    LogMessage(LogLevel::FATAL, context, "Assertion Failed: (%s), Message: %s", expression,
               message);
  }
}