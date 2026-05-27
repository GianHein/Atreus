#pragma once

#include <cstdint>

namespace Atr
{
  ///////////////////////////////////////////////////////
  // Definitions
  ///////////////////////////////////////////////////////

  using u8 = uint8_t;
  using u16 = uint16_t;
  using u32 = uint32_t;
  using u64 = uint64_t;

  using i8 = int8_t;
  using i16 = int16_t;
  using i32 = int32_t;
  using i64 = int64_t;

  using f32 = float;
  using f64 = double;

  using b8 = uint8_t;
  using b32 = uint32_t;

  using ptr = void *;

#define TRUE 1
#define FALSE 0

#define ATR_STATIC_ASSERT static_assert

  ATR_STATIC_ASSERT(sizeof(u8) == 1 && "Expected u8 to be 1 byte");
  ATR_STATIC_ASSERT(sizeof(u16) == 2 && "Expected u16 to be 2 bytes");
  ATR_STATIC_ASSERT(sizeof(u32) == 4 && "Expected u32 to be 4 bytes");
  ATR_STATIC_ASSERT(sizeof(u64) == 8 && "Expected u64 to be 8 bytes");

  ATR_STATIC_ASSERT(sizeof(i8) == 1 && "Expected i8 to be 1 byte");
  ATR_STATIC_ASSERT(sizeof(i16) == 2 && "Expected i16 to be 2 bytes");
  ATR_STATIC_ASSERT(sizeof(i32) == 4 && "Expected i32 to be 4 bytes");
  ATR_STATIC_ASSERT(sizeof(i64) == 8 && "Expected i64 to be 8 bytes");

#define ATR_KB(x) ((u64)1024 * x)
#define ATR_MB(x) ((u64)1024 * ATR_KB(x))
#define ATR_GB(x) ((u64)1024 * ATR_MB(x))

  ///////////////////////////////////////////////////////
  // Configuration
  ///////////////////////////////////////////////////////

#define ATR_DEBUG_MODE 1

#define ATR_ERROR_LOGGING_ENABLED 1
#define ATR_WARN_LOGGING_ENABLED 1
#define ATR_INFO_LOGGING_ENBLED 1
#define ATR_DEBUG_LOGGING_ENABLED 1
#define ATR_TRACE_LOGGING_ENABLED 1

#define ATR_ASSERTIONS_ENABLED 1
#define ATR_FILE_LOGGING_ENABLED 1

  ///////////////////////////////////////////////////////
  // DLL Export/Import
  ///////////////////////////////////////////////////////

#ifdef ATR_EXPORT
#if defined(_MSC_VER) || defined(__MINGW32__)
#define ATR_API __declspec(dllexport)
#else
#define ATR_API __attribute__((visibility("default")))
#endif
#else
#if defined(_MSC_VER) || defined(__MINGW32__)
#define ATR_API __declspec(dllimport)
#else
#define ATR_API
#endif
#endif
}