#pragma once

#include "Atr/Definitions.h"

#define ATR_ENABLE_ASSERTS

#ifdef ATR_ENABLE_ASSERTS
#if _MSC_VER
#include <intrin.h>
#define DebugBreak() __debugbreak()
#else
#define DebugBreak() __builtin_trap()
#endif

namespace Atr
{
  void FailAssertion(const char *Expression, const char *Message, const char *File, u32 Line);
}

#define ATR_ASSERT(expression)                                                                     \
  {                                                                                                \
    if (expression)                                                                                \
    {                                                                                              \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
      ::Atr::FailAssertion(#expression, "", __FILE__, __LINE__);                                   \
      DebugBreak();                                                                                \
    }                                                                                              \
  }

#define ATR_ASSERT_MSG(expression, message)                                                        \
  {                                                                                                \
    if (expression)                                                                                \
    {                                                                                              \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
      ::Atr::FailAssertion(#expression, message, __FILE__, __LINE__);                              \
      DebugBreak();                                                                                \
    }                                                                                              \
  }

#ifdef _DEBUG
#define ATR_ASSERT_DEBUG(expression)                                                               \
  {                                                                                                \
    if (expression)                                                                                \
    {                                                                                              \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
      ::Atr::FailAssertion(#expression, "", __FILE__, __LINE__);                                   \
      DebugBreak();                                                                                \
    }                                                                                              \
  }
#else
#define ATR_ASSERT_DEBUG(expression)
#endif

#else
#define ATR_ASSERT(expression)
#define ATR_ASSERT_MSG(expression, message)
#define ATR_ASSERT_DEBUG(expression)
#endif