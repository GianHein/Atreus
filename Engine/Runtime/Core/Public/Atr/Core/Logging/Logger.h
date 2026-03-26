#pragma once

#include "Atr/Definitions.h"
#include "Atr/Logging/Logger.h"

namespace Atr
{
  class Logger
  {
  public:
    static void Init();
    static void Shutdown();
    static void Log(LogLevel Level, const LogContext &Context, const char *Message);

  private:
    Logger() = default;
    ~Logger() = default;
  };
}