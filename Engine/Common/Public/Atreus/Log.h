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

using LogSinkFn = void (*)(void *user, LogLevel level, const char *message);

ATR_API bool        LogAddSink(LogSinkFn sink, void *user);
ATR_API void        LogRemoveSink(LogSinkFn sink, void *user);
ATR_API void        LogWrite(LogLevel level, LogContext context, const char *format, ...);
ATR_API const char *LogLevelName(LogLevel level);

#define ATR_LOG_TRACE(...) LogWrite(LogLevel::Trace, {__FILE__, __LINE__, __func__}, __VA_ARGS__)

#define ATR_LOG_DEBUG(...) LogWrite(LogLevel::Debug, {__FILE__, __LINE__, __func__}, __VA_ARGS__)

#define ATR_LOG_INFO(...) LogWrite(LogLevel::Info, {__FILE__, __LINE__, __func__}, __VA_ARGS__)

#define ATR_LOG_WARN(...) LogWrite(LogLevel::Warn, {__FILE__, __LINE__, __func__}, __VA_ARGS__)

#define ATR_LOG_ERROR(...) LogWrite(LogLevel::Error, {__FILE__, __LINE__, __func__}, __VA_ARGS__)

#define ATR_LOG_FATAL(...) LogWrite(LogLevel::Fatal, {__FILE__, __LINE__, __func__}, __VA_ARGS__)

#if !defined(NDEBUG)
#  define ATR_ASSERT(condition, message)                                                           \
    do                                                                                             \
    {                                                                                              \
      if (!(condition))                                                                            \
      {                                                                                            \
        ATR_LOG_FATAL("assertion failed: %s - %s", #condition, message);                           \
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
      ATR_LOG_FATAL("assertion failed: %s", #condition);                                           \
      ATR_DEBUG_BREAK();                                                                           \
      abort();                                                                                     \
    }                                                                                              \
  } while (0)

#define ATR_UNREACHABLE()                                                                          \
  do                                                                                               \
  {                                                                                                \
    ATR_LOG_FATAL("unreachable");                                                                  \
    ATR_DEBUG_BREAK();                                                                             \
    abort();                                                                                       \
  } while (0)
