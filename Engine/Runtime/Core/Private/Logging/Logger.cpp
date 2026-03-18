#include "Atr/Core/Logging/Logger.h"

#include "Atr/Core/Assertions/Assert.h"
#include "Atr/Platform/Console.h"
#include "Atr/Platform/File.h"
#include "Atr/Platform/Time.h"

#include <memory.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define MAX_LOG_MESSAGE_LENGTH 32000

// TODO:
// File logging should be more robust than this implementation. Esepecially when it comes to multithreading.
// This is only meant as a temporary solution until I implemented the job system and a proper multithreaded
// logger that can be used across the engine and client.

namespace Atr
{
  static FileHandle LogFile;

  b8 Logger::Init()
  {
#if ATR_FILE_LOGGING_ENABLED
    DateTime Time = Platform::GetLocalTime();
    char FileName[64];
    sprintf(FileName, "%04u%02u%02u_%02u%02u%02u.log", Time.Year, Time.Month, Time.Day, Time.Hour,
            Time.Minute, Time.Second);

    if (!Platform::FileCreate(FileName, &LogFile))
    {
      return FALSE;
    }
#endif

    return TRUE;
  }

  void Logger::Shutdown()
  {
    // TODO: Cleanup logging and write queued entries
#if ATR_FILE_LOGGING_ENABLED
    Platform::FileClose(&LogFile);
#endif
  }

  void Logger::OutputMessage(LogLevel Level, const char *Message, ...)
  {
    const char *LevelMap[6] = {"[TRACE]", "[DEBUG]", "[INFO]", "[WARN]", "[ERROR]", "[FATAL]"};

    char FormatBuffer[MAX_LOG_MESSAGE_LENGTH];
    memset(FormatBuffer, 0, sizeof(FormatBuffer));

    __builtin_va_list ArgumentPointer;
    va_start(ArgumentPointer, Message);
    vsnprintf(FormatBuffer, MAX_LOG_MESSAGE_LENGTH, Message, ArgumentPointer);
    va_end(ArgumentPointer);

    DateTime Time = Platform::GetLocalTime();
    char OutBuffer[MAX_LOG_MESSAGE_LENGTH];
    memset(OutBuffer, 0, sizeof(OutBuffer));
    sprintf(OutBuffer, "[%02u:%02u:%02u] %s: %s\n", Time.Hour, Time.Minute, Time.Second,
            LevelMap[(u8)Level], FormatBuffer);

    Platform::WriteConsole((ConsoleColor)Level, OutBuffer);

#if ATR_FILE_LOGGING_ENABLED
    u64 Written = 0;
    Platform::FileWrite(LogFile, OutBuffer, strlen(OutBuffer), &Written);
#endif
  }

  void Logger::OutputMessage(LogLevel Level, const char *File, u32 Line, const char *Function,
                             const char *Message, ...)
  {
    const char *LevelMap[6] = {"[TRACE]", "[DEBUG]", "[INFO]", "[WARN]", "[ERROR]", "[FATAL]"};

    char FormatBuffer[MAX_LOG_MESSAGE_LENGTH];
    memset(FormatBuffer, 0, sizeof(FormatBuffer));

    __builtin_va_list ArgumentPointer;
    va_start(ArgumentPointer, Message);
    vsnprintf(FormatBuffer, MAX_LOG_MESSAGE_LENGTH, Message, ArgumentPointer);
    va_end(ArgumentPointer);

    DateTime Time = Platform::GetLocalTime();
    char OutBuffer[MAX_LOG_MESSAGE_LENGTH];
    memset(OutBuffer, 0, sizeof(OutBuffer));
    sprintf(OutBuffer, "[%02u:%02u:%02u] %s [%s:%d] [%s]: %s\n", Time.Hour, Time.Minute,
            Time.Second, LevelMap[(u8)Level], File, Line, Function, FormatBuffer);

    Platform::WriteConsole((ConsoleColor)Level, OutBuffer);
#if ATR_FILE_LOGGING_ENABLED
    u64 Written = 0;
    Platform::FileWrite(LogFile, OutBuffer, strlen(OutBuffer), &Written);
#endif
  }

  void FailAssertion(const char *Expression, const char *Message, const char *File, u32 Line,
                     const char *Function)
  {
    Logger::OutputMessage(LogLevel::LEVEL_FATAL, File, Line, Function,
                          "Assertion Failed: (%s), Message: %s", Expression, Message);
  }
}