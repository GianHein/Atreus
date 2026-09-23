#pragma once

#include "Atreus/Definitions.h"

namespace Atreus
{
  enum class LogLevel : u8
  {
    Trace = 0,
    Debug,
    Info,
    Warn,
    Error,
    Fatal
  };

  b8   InitializeLog();
  void ShutdownLog();

  void
  Log(LogLevel level, const char *file, i32 line, const char *function, const char *message, ...);
}

#define LOG_LEVEL_THRESHOLD 0 // 0 = Trace, Debug, Info, Warn, Error, Fatal
#define ASSERTIONS_ENABLED 1

#if LOG_LEVEL_THRESHOLD <= 5
#define ATR_LOG_FATAL(message, ...)                                                                \
  ::Atreus::Log(Atreus::LogLevel::Fatal, __FILE__, __LINE__, __func__, message, ##__VA_ARGS__)
#else
#define ATR_LOG_FATAL(...) ((void)0)
#endif

#if LOG_LEVEL_THRESHOLD <= 4
#define ATR_LOG_ERROR(message, ...)                                                                \
  ::Atreus::Log(Atreus::LogLevel::Error, __FILE__, __LINE__, __func__, message, ##__VA_ARGS__)
#else
#define ATR_LOG_ERROR(...) ((void)0)
#endif

#if LOG_LEVEL_THRESHOLD <= 3
#define ATR_LOG_WARN(message, ...)                                                                 \
  ::Atreus::Log(Atreus::LogLevel::Warn, __FILE__, __LINE__, __func__, message, ##__VA_ARGS__)
#else
#define ATR_LOG_WARN(...) ((void)0)
#endif

#if LOG_LEVEL_THRESHOLD <= 2
#define ATR_LOG_INFO(message, ...)                                                                 \
  ::Atreus::Log(Atreus::LogLevel::Info, __FILE__, __LINE__, __func__, message, ##__VA_ARGS__)
#else
#define ATR_LOG_INFO(...) ((void)0)
#endif

#if LOG_LEVEL_THRESHOLD <= 1
#define ATR_LOG_DEBUG(message, ...)                                                                \
  ::Atreus::Log(Atreus::LogLevel::Debug, __FILE__, __LINE__, __func__, message, ##__VA_ARGS__)
#else
#define ATR_LOG_DEBUG(...) ((void)0)
#endif

#if LOG_LEVEL_THRESHOLD <= 0
#define ATR_LOG_TRACE(message, ...)                                                                \
  ::Atreus::Log(Atreus::LogLevel::Trace, __FILE__, __LINE__, __func__, message, ##__VA_ARGS__)
#else
#define ATR_LOG_TRACE(...) ((void)0)
#endif

#if _MSC_VER
#include <intrin.h>
#define ATR_DEBUG_BREAK() __debugbreak()
#else
#define ATR_DEBUG_BREAK() __builtin_trap()
#endif

#if ASSERTIONS_ENABLED == 1
#define ATR_ASSERT(expression, message, ...)                                                                                     \
  do                                                                                                                             \
  {                                                                                                                              \
    if (!(expression))                                                                                                           \
    {                                                                                                                            \
      ::Atreus::Log(                                                                                                             \
          ::Atreus::LogLevel::Fatal, __FILE__, __LINE__, __func__, "Assertion failed (%s): " message, #expression, ##__VA_ARGS__ \
      );                                                                                                                         \
      ATR_DEBUG_BREAK();                                                                                                         \
    }                                                                                                                            \
  } while (0)
#else
#define ATR_ASSERT(...) ((void)0)
#endif