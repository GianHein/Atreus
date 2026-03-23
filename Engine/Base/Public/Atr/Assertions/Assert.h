#pragma once

#include "Atr/Definitions.h"
#include "Atr/Logging/Logger.h"

#if ATR_ASSERTIONS_ENABLED == 1
#if _MSC_VER
#include <intrin.h>
#define DebugBreak() __debugbreak()
#else
#define DebugBreak() __builtin_trap()
#endif

namespace Atr
{
  void FailAssertion(const char *Expression, const char *Message, LogContext Context);
}

#define ATR_ASSERT(Expression, Message)                                                            \
  {                                                                                                \
    if (Expression)                                                                                \
    {                                                                                              \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
      ::Atr::FailAssertion(#Expression, Message, {__FILE__, __LINE__, __func__});                  \
      DebugBreak();                                                                                \
    }                                                                                              \
  }

#if ATR_DEBUG_MODE == 1
#define ATR_ASSERT_DEBUG(Expression, Message)                                                      \
  {                                                                                                \
    if (Expression)                                                                                \
    {                                                                                              \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
      ::Atr::FailAssertion(#Expression, Message, {__FILE__, __LINE__, __func__});                  \
      DebugBreak();                                                                                \
    }                                                                                              \
  }
#else
#define ATR_ASSERT_DEBUG(Expression, Message)
#endif

#else
#define ATR_ASSERT(Expression, Message)
#define ATR_ASSERT_DEBUG(Expression, Message)
#endif