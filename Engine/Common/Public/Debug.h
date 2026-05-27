#pragma once

#include "Definitions.h"

namespace Atr
{
  enum class LogLevel : u8
  {
    FATAL = 0,
    ERROR,
    WARN,
    INFO,
    DEBUG,
    TRACE
  };

  struct LogContext
  {
    const char *file;
    u32 line;
    const char *function;
  };

  using LogCallback = void (*)(LogLevel level, const LogContext &context, const char *message);
  void SetLogCallback(LogCallback callback);

  void LogMessage(LogLevel level, const LogContext &context, const char *message, ...);
  void LogAssertionFailure(const char *expression, const char *message, LogContext &context);
}

#define ATR_MAX_LOG_MESSAGE_LENGTH 2048

#define ATR_LOG_INTERNAL(level, message, ...)                                                      \
  ::Atr::LogMessage(level, ::Atr::LogContext(__FILE__, __LINE__, __func__), message, ##__VA_ARGS__)

#define ATR_FATAL(message, ...) ATR_LOG_INTERNAL(::Atr::LogLevel::FATAL, message, ##__VA_ARGS__)
#define ATR_ERROR(message, ...) ATR_LOG_INTERNAL(::Atr::LogLevel::ERROR, message, ##__VA_ARGS__)

#if ATR_WARN_LOGGING_ENABLED == 1
#define ATR_WARN(message, ...) ATR_LOG_INTERNAL(::Atr::LogLevel::WARN, message, ##__VA_ARGS__)
#else
#define ATR_WARN(...) ((void)0)
#endif

#if ATR_INFO_LOGGING_ENABLED == 1
#define ATR_INFO(message, ...) ATR_LOG_INTERNAL(::Atr::LogLevel::INFO, message, ##__VA_ARGS__)
#else
#define ATR_INFO(...) ((void)0)
#endif

#if ATR_DEBUG_LOGGING_ENABLED == 1
#define ATR_DEBUG(message, ...) ATR_LOG_INTERNAL(::Atr::LogLevel::DEBUG, message, ##__VA_ARGS__)
#else
#define ATR_DEBUG(...) ((void)0)
#endif

#if ATR_TRACE_LOGGING_ENABLED == 1
#define ATR_TRACE(message, ...) ATR_LOG_INTERNAL(::Atr::LogLevel::TRACE, message, ##__VA_ARGS__)
#else
#define ATR_TRACE(...) ((void)0)
#endif

#if ATR_ASSERTIONS_ENABLED == 1
#if _MSC_VER
#include <intrin.h>
#define DebugBreak() __debugbreak()
#else
#define DebugBreak() __builtin_trap()
#endif

#define ATR_ASSERT(expression, message)                                                            \
  {                                                                                                \
    if (expression)                                                                                \
    {                                                                                              \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
      ::Atr::LogAssertionFailure(#expression, message, {__FILE__, __LINE__, __func__});            \
      DebugBreak();                                                                                \
    }                                                                                              \
  }

#if ATR_DEBUG_MODE == 1
#define ATR_ASSERT_DEBUG(expression, message)                                                      \
  {                                                                                                \
    if (expression)                                                                                \
    {                                                                                              \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
      ::Atr::LogAssertionFailure(#expression, message, {__FILE__, __LINE__, __func__});            \
      DebugBreak();                                                                                \
    }                                                                                              \
  }
#else
#define ATR_ASSERT_DEBUG(...) ((void)0)
#endif

#else
#define ATR_ASSERT(...) ((void)0)
#define ATR_ASSERT_DEBUG(...) ((void)0)
#endif