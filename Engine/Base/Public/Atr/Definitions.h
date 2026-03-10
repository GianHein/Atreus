#pragma once

#include <cstdint>

namespace Atr
{
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

#define TRUE 1
#define FALSE 0

#define STATIC_ASSERT static_assert

  STATIC_ASSERT(sizeof(u8) == 1, "Expected u8 to be 1 byte");
  STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 to be 2 bytes");
  STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 to be 4 bytes");
  STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 to be 8 bytes");

  STATIC_ASSERT(sizeof(i8) == 1, "Expected i8 to be 1 byte");
  STATIC_ASSERT(sizeof(i16) == 2, "Expected i16 to be 2 bytes");
  STATIC_ASSERT(sizeof(i32) == 4, "Expected i32 to be 4 bytes");
  STATIC_ASSERT(sizeof(i64) == 8, "Expected i64 to be 8 bytes");

  STATIC_ASSERT(sizeof(f32) == 4, "Expected f32 to be 4 bytes");
  STATIC_ASSERT(sizeof(f64) == 8, "Expected f64 to be 8 bytes");

  STATIC_ASSERT(sizeof(b8) == 1, "Expected b8 to be 1 byte");
  STATIC_ASSERT(sizeof(b32) == 4, "Expected b32 to be 4 bytes");

#define KB(x) ((x) * 1024ULL)
#define MB(x) ((x) * 1024ULL * 1024ULL)
#define GB(x) ((x) * 1024ULL * 1024ULL * 1024ULL)

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define ATR_PLATFORM_WINDOWS 1
#ifndef _WIN64
#error "32-bit Windows builds are not supported."
#endif
#elif defined(__linux__) || defined(__gnu_linux__)
#define ATR_PLATFORM_LINUX 1
#if defined(__ANDROID__)
#define ATR_PLATFORM_ANDROID 1
#endif
#else
#error "Unknown platform"
#endif

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