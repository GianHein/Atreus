#pragma once

#include "Atr/Definitions.h"

namespace Atr
{
  enum class LogLevel : u8
  {
    TRACE = 0,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
  };

  struct LogContext
  {
    const char *File;
    u32 Line;
    const char *Function;
  };

  using LogCallback = void (*)(LogLevel Level, const LogContext &Context,
                               const char *FormattedBuffer);

  void SetLogCallback(LogCallback Callback);
  void Log(LogLevel Level, const LogContext &Context, const char *Message, ...);
}

#define ATR_LOG_INTERNAL(Level, Message, ...)                                                      \
  ::Atr::Log(Level, ::Atr::LogContext{__FILE__, __LINE__, __func__}, Message, ##__VA_ARGS__)

// TODO:
// Split the following into separate engine and client macros. Both should be toggleable
// separately and should be able to be compiled out in release builds.
//
// I might wan't client logging in release builds for crash reports and such, but I don't
// want engine logs in release builds.

#define ATR_FATAL(Message, ...) ATR_LOG_INTERNAL(::Atr::LogLevel::FATAL, Message, ##__VA_ARGS__)
#define ATR_ERROR(Message, ...) ATR_LOG_INTERNAL(::Atr::LogLevel::ERROR, Message, ##__VA_ARGS__)

#if ATR_WARN_LOGGING_ENABLED == 1
#define ATR_WARN(Message, ...) ATR_LOG_INTERNAL(::Atr::LogLevel::WARN, Message, ##__VA_ARGS__)
#else
#define ATR_WARN(...) ((void)0)
#endif

#if ATR_INFO_LOGGING_ENABLED == 1
#define ATR_INFO(Message, ...) ATR_LOG_INTERNAL(::Atr::LogLevel::INFO, Message, ##__VA_ARGS__)
#else
#define ATR_INFO(...) ((void)0)
#endif

#if ATR_DEBUG_LOGGING_ENABLED == 1
#define ATR_DEBUG(Message, ...) ATR_LOG_INTERNAL(::Atr::LogLevel::DEBUG, Message, ##__VA_ARGS__)
#else
#define ATR_DEBUG(...) ((void)0)
#endif

#if ATR_TRACE_LOGGING_ENABLED == 1
#define ATR_TRACE(Message, ...) ATR_LOG_INTERNAL(::Atr::LogLevel::TRACE, Message, ##__VA_ARGS__)
#else
#define ATR_TRACE(...) ((void)0)
#endif