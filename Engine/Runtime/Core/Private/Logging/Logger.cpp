#include "Atr/Core/Logging/Logger.h"

#include "Atr/Logging/Logger.h"
#include "Atr/Platform/File.h"
#include "Atr/Platform/Time.h"

#include <cstring>
#include <string>

namespace Atr
{
  static char s_LogFilePath[64] = {0};

  void Logger::Init()
  {
    DateTime Time = Platform::Time::GetLocalTime();

    snprintf(s_LogFilePath, sizeof(s_LogFilePath), "%04u-%02u-%02u_%02u-%02u-%02u.log", Time.Year,
             Time.Month, Time.Day, Time.Hour, Time.Minute, Time.Second);

    Platform::File::Create(s_LogFilePath);

    SetLogCallback(Logger::Log);
  }

  void Logger::Shutdown()
  {
    // No-op for now
  }

  void Logger::Log(LogLevel Level, const LogContext &Context, const char *Message)
  {
    // All this redundancy is not ideal, cba to fix it right now, that probably
    // will happen at a later point tho.

    static const char *Levels[6] = {"[TRACE]", "[DEBUG]", "[INFO]", "[WARN]", "[ERROR]", "[FATAL]"};

    char Buffer[MAX_LOG_MESSAGE_LENGTH];

    snprintf(Buffer, sizeof(Buffer), "%s [%s:%u][%s]: %s\n", Levels[(u8)Level], Context.File,
             Context.Line, Context.Function, Message);
    Platform::File::Append(s_LogFilePath, Buffer, strlen(Buffer));

    static const char *LevelColors[6] = {
        "\x1b[30m", // TRACE - Black
        "\x1b[34m", // DEBUG - Blue
        "\x1b[32m", // INFO  - Green
        "\x1b[33m", // WARN  - Yellow (Red + Green)
        "\x1b[31m", // ERROR - Red
        "\x1b[41m"  // FATAL - Red Background
    };

    printf("%s%s\x1b[0m", LevelColors[(u8)Level], Buffer);
  }
}