// Log.h: log sinks, logging, assertions, and runtime invariant validation
// ATR_LOG_* macros for levels TRACE, DEBUG, INFO, WARN, ERROR, FATAL
// ATR_ASSERT(condition, message), ATR_VERIFY(condition), ATR_UNREACHABLE

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

ATR_API bool        log_add_sink(LogSinkFn sink, void *user);
ATR_API void        log_remove_sink(LogSinkFn sink, void *user);
ATR_API void        log_write(LogLevel level, LogContext context, const char *format, ...);
ATR_API const char *log_level_name(LogLevel level);

#define ATR_LOG_TRACE(...) log_write(LogLevel::Trace, {__FILE__, __LINE__, __func__}, __VA_ARGS__)

#define ATR_LOG_DEBUG(...) log_write(LogLevel::Debug, {__FILE__, __LINE__, __func__}, __VA_ARGS__)

#define ATR_LOG_INFO(...) log_write(LogLevel::Info, {__FILE__, __LINE__, __func__}, __VA_ARGS__)

#define ATR_LOG_WARN(...) log_write(LogLevel::Warn, {__FILE__, __LINE__, __func__}, __VA_ARGS__)

#define ATR_LOG_ERROR(...) log_write(LogLevel::Error, {__FILE__, __LINE__, __func__}, __VA_ARGS__)

#define ATR_LOG_FATAL(...) log_write(LogLevel::Fatal, {__FILE__, __LINE__, __func__}, __VA_ARGS__)

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
