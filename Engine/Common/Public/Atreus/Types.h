#pragma once

#include <cstddef>
#include <cstdint>

// Platform / compiler detection

#if defined(_WIN32)
#  define ATR_PLATFORM_WINDOWS 1
#elif defined(__linux__)
#  define ATR_PLATFORM_LINUX 1
#endif

#if defined(_MSC_VER)
#  define ATR_COMPILER_MSCV 1
#  define ATR_DEBUG_BREAK() __debugbreak()
#else
#  define ATR_DEBUG_BREAK() __builtin_trap()
#endif

// Symbol visibility

#if defined(ATR_PLATFORM_WINDOWS)
#  if defined(ATREUS_BUILDING)
#    define ATR_API __declspec(dllexport)
#  else
#    define ATR_API __declspec(dllimport)
#  endif
#  define ATR_MODULE_EXPORT extern "C" __declspec(dllexport)
#else
#  define ATR_API __attribute__((visibility("default")))
#  define ATR_MODULE_EXPORT extern "C" __attribute__((visibility("default")))
#endif

// Fixed width types

using u8    = uint8_t;
using u16   = uint16_t;
using u32   = uint32_t;
using u64   = uint64_t;
using i8    = int8_t;
using i16   = int16_t;
using i32   = int32_t;
using i64   = int64_t;
using f32   = float;
using f64   = double;
using usize = size_t;

static_assert(sizeof(f32) == 4 && sizeof(f64) == 8, "Unexpected floating point size");
static_assert(sizeof(void *) == 8, "Atreus only supports 64-bit targets");

constexpr u32 INVALID_INDEX = 0xFFFFFFFFu;

#define ATR_ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0]))
#define ATR_KB(value) (static_cast<usize>(value) * 1024u)
#define ATR_MB(value) (static_cast<usize>(value) * 1024u * 1024u)
#define ATR_UNUSED(value) (void)(value)

template <typename T> constexpr T AlignUp(T value, T alignment)
{
  return (value + alignment - 1) & ~(alignment - 1);
}