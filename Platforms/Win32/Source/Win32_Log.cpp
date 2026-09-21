#include "Atreus/Log.h"
#include "Atreus/Platform/File.h"
#include "Atreus/Platform/Time.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

namespace Atreus
{
  constexpr u32 MAX_LOG_MESSAGE_LENGTH = 512;

  static b8         log_file_exists = false;
  static FileHandle log_file        = InvalidFileHandle;

  static const char *levels[6] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

  static const char *colors[6] = {
      "\x1b[30m", // TRACE - Black
      "\x1b[34m", // DEBUG - Blue
      "\x1b[32m", // INFO  - Green
      "\x1b[33m", // WARN  - Yellow (Red + Green)
      "\x1b[31m", // ERROR - Red
      "\x1b[41m"  // FATAL - Red Background
  };

  b8 InitializeLog()
  {
    if (log_file_exists) return true;

    DateTime creation_time = GetLocalDateTime();

    char log_file_path[64] = {0};
    snprintf(
        log_file_path,
        sizeof(log_file_path),
        "%04u%02u%02u%02u%02u%02u.log",
        creation_time.year,
        creation_time.month,
        creation_time.day,
        creation_time.hour,
        creation_time.minute,
        creation_time.second
    );

    log_file_exists =
        OpenFile(log_file_path, FileAccessMode::Write, FileOpenMode::OpenAlways, &log_file);

    if (log_file_exists)
    {
      return true;
    }

    char     buffer[MAX_LOG_MESSAGE_LENGTH];
    DateTime log_time = GetLocalDateTime();

    printf(
        "%s[%04u-%02u-%02u %02u:%02u:%02u.%03u][%s] %s\x1b[0m\n",
        colors[(u8)LogLevel::Error],
        log_time.year,
        log_time.month,
        log_time.day,
        log_time.hour,
        log_time.minute,
        log_time.second,
        log_time.millisecond,
        levels[(u8)LogLevel::Error],
        "Failed to create log file"
    );

    return false;
  }

  void ShutdownLog()
  {
    if (log_file_exists)
    {
      CloseFile(&log_file);
      log_file_exists = false;
    }
  }

  void
  Log(LogLevel level, const char *file, i32 line, const char *function, const char *message, ...)
  {
    va_list args;
    va_start(args, message);
    char formatted[MAX_LOG_MESSAGE_LENGTH];
    vsnprintf(formatted, sizeof(formatted), message, args);
    va_end(args);

    char     buffer[MAX_LOG_MESSAGE_LENGTH];
    DateTime log_time = GetLocalDateTime();

    u32 written = snprintf(
        buffer,
        sizeof(buffer),
        "[%04u-%02u-%02u %02u:%02u:%02u.%03u][%s][%s:%u %s] %s\n",
        log_time.year,
        log_time.month,
        log_time.day,
        log_time.hour,
        log_time.minute,
        log_time.second,
        log_time.millisecond,
        levels[(u8)level],
        file,
        line,
        function,
        formatted
    );

    u32 buffer_size = (written > 0) ? min(written, sizeof(buffer) - 1) : 0;

    if (log_file_exists)
    {
      WriteFileBytes(log_file, FileSeekOrigin::End, 0, buffer, buffer_size, nullptr);
      FlushFile(log_file);
    }
    else
    {
      printf(
          "%s[%04u-%02u-%02u %02u:%02u:%02u.%03u][%s] %s\x1b[0m\n",
          colors[(u8)LogLevel::Warn],
          log_time.year,
          log_time.month,
          log_time.day,
          log_time.hour,
          log_time.minute,
          log_time.second,
          log_time.millisecond,
          levels[(u8)LogLevel::Warn],
          "Logging before initialization. File-logging disabled, please call Atreus::InitializeLog()"
      );
    }

    printf("%s%.*s\x1b[0m\n", colors[(u8)level], (int)(buffer_size - 1), buffer);
  }
}