#include "Core/Logger.h"

#include "Debug.h"
#include "Platform/File.h"
#include "Platform/Time.h"

#include <cstdio>
#include <cstring>

namespace Atr
{
  static FileHandle log_file = {0};

  void Logger::Init()
  {
    char log_file_path[64] = {0};

    DateTime date = GetLocalDate();
    snprintf(log_file_path, sizeof(log_file_path), "%04u-%02u-%02u_%02u-%02u-%02u.log", date.year,
             date.month, date.day, date.hour, date.minute, date.second);

    if (!CreateFile(log_file_path))
    {
      ATR_FATAL("Failed to create %s", log_file_path);
      return;
    }

    if (!OpenFile(log_file_path, FileMode::APPEND, &log_file))
    {
      ATR_FATAL("Failed to open %s", log_file_path);
      return;
    }

    SetLogCallback(Logger::HandleLog);
  }

  void Logger::Shutdown()
  {
    CloseFile(&log_file);
  }

  void WriteToFile(const char *buffer)
  {
    WriteFile(&log_file, (void *)buffer, strlen(buffer));
  }

  void WriteToConsole(LogLevel level, const char *buffer)
  {
    static const char *colors[6] = {
        "\x1b[41m", // FATAL - Red Background
        "\x1b[31m", // ERROR - Red
        "\x1b[33m", // WARN  - Yellow (Red + Green)
        "\x1b[32m", // INFO  - Green
        "\x1b[34m", // DEBUG - Blue
        "\x1b[30m"  // TRACE - Black
    };

    printf("%s%s\x1b[0m", colors[(u8)level], buffer);
  }

  void Logger::HandleLog(LogLevel level, const LogContext &context, const char *message)
  {
    static const char *levels[6] = {"FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"};
    const char *level_string = levels[(u8)level];

    char final_buffer[4096];

    snprintf(final_buffer, sizeof(final_buffer), "[%s] [%s:%u][%s]: %s\n", level_string,
             context.file, context.line, context.function, message);

    WriteToFile(final_buffer);
    WriteToConsole(level, final_buffer);
  }
}