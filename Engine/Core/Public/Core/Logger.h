#pragma once

#include "Debug.h"
#include "Definitions.h"

namespace Atr
{
  class Logger
  {
  public:
    static void Init();
    static void Shutdown();

  private:
    Logger() = default;
    ~Logger() = default;

    static void HandleLog(LogLevel level, const LogContext &context, const char *message);
  };
}