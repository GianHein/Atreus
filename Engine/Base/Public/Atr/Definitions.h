#pragma once

#include <cstdint>

///////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define i8 int8_t
#define i16 int16_t
#define i32 int32_t
#define i64 int64_t

#define f32 float
#define f64 double

#define b8 uint8_t
#define b32 uint32_t

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

#define KB(x) ((u64)1024 * x)
#define MB(x) ((u64)1024 * KB(x))
#define GB(x) ((u64)1024 * MB(x))

///////////////////////////////////////////////////////
// Configuration
///////////////////////////////////////////////////////

#define ATR_DEBUG_MODE 1

#define ATR_TRACE_LOGGING_ENABLED 1
#define ATR_DEBUG_LOGGING_ENABLED 1
#define ATR_INFO_LOGGING_ENABLED 1
#define ATR_WARN_LOGGING_ENABLED 1

#define ATR_FILE_LOGGING_ENABLED 1
#define ATR_ASSERTIONS_ENABLED 1

///////////////////////////////////////////////////////
// Platform Detection
///////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////
// DLL Export
///////////////////////////////////////////////////////

// For this to work properly, the ATR_EXPORT symbol has to be defined
// for the relevant build targets.

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