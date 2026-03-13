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
  b8 InitLogger();
  void ShutdownLogger();

  void OutputLogMessage(LogLevel Level, const char *Message, ...);
  void OutputLogMessage(LogLevel Level, const char *File, u32 Line, const char *Function,
                        const char *Message, ...);
}

#define ATR_FATAL(message, ...)                                                                    \
  ::Atr::OutputLogMessage(LogLevel::LEVEL_FATAL, __FILE__, __LINE__, __func__, message,            \
                          ##__VA_ARGS__);

#ifndef ATR_ERROR
#define ATR_ERROR(message, ...)                                                                    \
  ::Atr::OutputLogMessage(LogLevel::LEVEL_ERROR, __FILE__, __LINE__, __func__, message,            \
                          ##__VA_ARGS__);
#endif

#if ATR_WARN_ENABLED == 1
#define ATR_WARN(message, ...)                                                                     \
  ::Atr::OutputLogMessage(LogLevel::LEVEL_WARN, __FILE__, __LINE__, __func__, message,             \
                          ##__VA_ARGS__);
#else
#define ATR_WARN(message, ...)
#endif

#if ATR_INFO_ENABLED == 1
#define ATR_INFO(message, ...)                                                                     \
  ::Atr::OutputLogMessage(LogLevel::LEVEL_INFO, __FILE__, __LINE__, __func__, message,             \
                          ##__VA_ARGS__);
#else
#define ATR_INFO(message, ...)
#endif

#if ATR_DEBUG_ENABLED == 1
#define ATR_DEBUG(message, ...)                                                                    \
  ::Atr::OutputLogMessage(LogLevel::LEVEL_DEBUG, __FILE__, __LINE__, __func__, message,            \
                          ##__VA_ARGS__);
#else
#define ATR_DEBUG(message, ...)
#endif

#if ATR_TRACE_ENABLED == 1
#define ATR_TRACE(message, ...)                                                                    \
  ::Atr::OutputLogMessage(LogLevel::LEVEL_TRACE, __FILE__, __LINE__, __func__, message,            \
                          ##__VA_ARGS__);
#else
#define ATR_TRACE(message, ...)
#endif
