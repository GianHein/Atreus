#pragma once

#include "Atreus/Types.h"

#include <stdlib.h>

enum class LogLevel : u8
{
  Trace = 0,
  Debug,
  Info,
  Warn,
  Error,
  Fatal,
};

struct LogContext
{
  const char *file;
  i32         line;
  const char *function;
};

using LogSinkFn = void (*)(void *user, LogLevel level, const char *channel, const char *message);

ATR_API bool LogAddSink(LogSinkFn sink, void *user);
ATR_API void LogRemoveSink(LogSinkFn sink, void *user);
ATR_API void
LogWrite(LogLevel level, LogContext context, const char *channel, const char *format, ...);
ATR_API const char *LogLevelName(LogLevel level);

#define ATR_LOG_TRACE(channel, ...)                                                                \
  LogWrite(LogLevel::Trace, {__FILE__, __LINE__, __func__}, channel, __VA_ARGS__)

#define ATR_LOG_DEBUG(channel, ...)                                                                \
  LogWrite(LogLevel::Debug, {__FILE__, __LINE__, __func__}, channel, __VA_ARGS__)

#define ATR_LOG_INFO(channel, ...)                                                                 \
  LogWrite(LogLevel::Info, {__FILE__, __LINE__, __func__}, channel, __VA_ARGS__)

#define ATR_LOG_WARN(channel, ...)                                                                 \
  LogWrite(LogLevel::Warn, {__FILE__, __LINE__, __func__}, channel, __VA_ARGS__)

#define ATR_LOG_ERROR(channel, ...)                                                                \
  LogWrite(LogLevel::Error, {__FILE__, __LINE__, __func__}, channel, __VA_ARGS__)

#define ATR_LOG_FATAL(channel, ...)                                                                \
  LogWrite(LogLevel::Fatal, {__FILE__, __LINE__, __func__}, channel, __VA_ARGS__)

#if !defined(NDEBUG)
#  define ATR_ASSERT(condition, message)                                                           \
    do                                                                                             \
    {                                                                                              \
      if (!(condition))                                                                            \
      {                                                                                            \
        ATR_LOG_FATAL(                                                                             \
            "assert", "%s:%d: assertion failed: %s - %s", __FILE__, __LINE__, #condition, message  \
        );                                                                                         \
        ATR_DEBUG_BREAK();                                                                         \
        abort();                                                                                   \
      }                                                                                            \
    } while (0)
#else
#  define ATR_ASSERT(condition, message) ATR_UNUSED(sizeof(condition))
#endif

#define ATR_VERIFY(condition)                                                                      \
  do                                                                                               \
  {                                                                                                \
    if (!(condition))                                                                              \
    {                                                                                              \
      ATR_LOG_FATAL("assert", "%s:%d: assertion failed: %s", __FILE__, __LINE__, #condition);      \
      ATR_DEBUG_BREAK();                                                                           \
      abort();                                                                                     \
    }                                                                                              \
  } while (0)

#define ATR_UNREACHABLE()                                                                          \
  do                                                                                               \
  {                                                                                                \
    ATR_LOG_FATAL("assert", "%s:%d: unreachable", __FILE__, __LINE__);                             \
    ATR_DEBUG_BREAK();                                                                             \
    abort();                                                                                       \
  } while (0)
