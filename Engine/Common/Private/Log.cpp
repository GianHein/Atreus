#include "Atreus/Log.h"

#include <cstdarg>
#include <cstdio>

namespace
{
  constexpr u32 MAX_SINKS = 8;

  struct SinkEntry
  {
    LogSinkFn fn;
    void     *user;
  };

  SinkEntry sinks[MAX_SINKS];
  u32       sink_count = 0;

  const char *colors[6] = {
      "\x1b[30m", // TRACE - Black
      "\x1b[34m", // DEBUG - Blue
      "\x1b[32m", // INFO  - Green
      "\x1b[33m", // WARN  - Yellow (Red + Green)
      "\x1b[31m", // ERROR - Red
      "\x1b[41m"  // FATAL - Red Background
  };
}

bool LogAddSink(LogSinkFn sink, void *user)
{
  if (sink_count >= MAX_SINKS) return false;

  sinks[sink_count++] = {sink, user};

  return true;
}

void LogRemoveSink(LogSinkFn sink, void *user)
{
  for (u32 i = 0; i < sink_count; ++i)
  {
    if (sinks[i].fn == sink && sinks[i].user == user)
    {
      sinks[i] = sinks[sink_count - 1];
      sink_count -= 1;
      break;
    }
  }
}

const char *LogLevelName(LogLevel level)
{
  switch (level)
  {
  case LogLevel::Trace:
    return "TRACE";
  case LogLevel::Debug:
    return "DEBUG";
  case LogLevel::Info:
    return "INFO";
  case LogLevel::Warn:
    return "WARN";
  case LogLevel::Error:
    return "ERROR";
  case LogLevel::Fatal:
    return "FATAL";
  }

  return "?";
}

void LogWrite(LogLevel level, LogContext context, const char *format, ...)
{
  char message[1024];
  int  prefix_length = snprintf(
      message, sizeof(message), "[%s:%u %s]: ", context.file, context.line, context.function
  );

  if (prefix_length < 0)
    prefix_length = 0;
  else if (prefix_length >= (i32)sizeof(message))
    prefix_length = sizeof(message) - 1;

  va_list args;
  va_start(args, format);
  vsnprintf(message + prefix_length, sizeof(message) - prefix_length, format, args);
  va_end(args);

  fprintf(
      stderr, "%s[%s]%s\x1b[0m\n", colors[static_cast<u8>(level)], LogLevelName(level), message
  );
  fflush(stderr);

  for (u32 i = 0; i < sink_count; ++i)
  {
    sinks[i].fn(sinks[i].user, level, message);
  }
}