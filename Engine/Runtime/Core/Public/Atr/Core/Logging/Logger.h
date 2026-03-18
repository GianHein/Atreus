#pragma once

#include "Atr/Core/Logging/LogLevel.h"
#include "Atr/Definitions.h"

#define ATR_WARN_ENABLED 1
#define ATR_INFO_ENABLED 1
#define ATR_DEBUG_ENABLED 1
#define ATR_TRACE_ENABLED 1

#if ATR_RELEASE == 1
#define ATR_DEBUG_ENABLED 0
#define ATR_TRACE_ENABLED 0
#endif

namespace Atr
{
  class Logger
  {
  public:
    static b8 Init();
    static void Shutdown();

    static void OutputMessage(LogLevel Level, const char *Message, ...);
    static void OutputMessage(LogLevel Level, const char *File, u32 Line, const char *Function,
                              const char *Message, ...);
  };
}

// TODO:
// Split the following into separate engine and client macros. Both should be toggleable
// separately and should be able to be compiled out in release builds.
//
// I might wan't client logging in release builds for crash reports and such, but I don't
// want engine logs in release builds.

#define ATR_FATAL(message, ...)                                                                    \
  ::Atr::Logger::OutputMessage(LogLevel::LEVEL_FATAL, __FILE__, __LINE__, __func__, message,       \
                               ##__VA_ARGS__);

#ifndef ATR_ERROR
#define ATR_ERROR(message, ...)                                                                    \
  ::Atr::Logger::OutputMessage(LogLevel::LEVEL_ERROR, __FILE__, __LINE__, __func__, message,       \
                               ##__VA_ARGS__);
#endif

#if ATR_WARN_ENABLED == 1
#define ATR_WARN(message, ...)                                                                     \
  ::Atr::Logger::OutputMessage(LogLevel::LEVEL_WARN, __FILE__, __LINE__, __func__, message,        \
                               ##__VA_ARGS__);
#else
#define ATR_WARN(message, ...)
#endif

#if ATR_INFO_ENABLED == 1
#define ATR_INFO(message, ...)                                                                     \
  ::Atr::Logger::OutputMessage(LogLevel::LEVEL_INFO, __FILE__, __LINE__, __func__, message,        \
                               ##__VA_ARGS__);
#else
#define ATR_INFO(message, ...)
#endif

#if ATR_DEBUG_ENABLED == 1
#define ATR_DEBUG(message, ...)                                                                    \
  ::Atr::Logger::OutputMessage(LogLevel::LEVEL_DEBUG, __FILE__, __LINE__, __func__, message,       \
                               ##__VA_ARGS__);
#else
#define ATR_DEBUG(message, ...)
#endif

#if ATR_TRACE_ENABLED == 1
#define ATR_TRACE(message, ...)                                                                    \
  ::Atr::Logger::OutputMessage(LogLevel::LEVEL_TRACE, __FILE__, __LINE__, __func__, message,       \
                               ##__VA_ARGS__);
#else
#define ATR_TRACE(message, ...)
#endif
