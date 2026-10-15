#pragma once

#include "Atreus/Types.h"

#include <string.h>

struct Allocator
{
  void *(*alloc)(void *user, usize size, usize alignment);
  void (*free)(void *user, void *ptr, usize size);
  void *user;
};

constexpr usize DEFAULT_ALIGNMENT = 16;

ATR_API const Allocator *DefaultAllocator();

inline void *MemAlloc(const Allocator *allocator, usize size, usize alignment = DEFAULT_ALIGNMENT)
{
  return allocator->alloc(allocator->user, size, alignment);
}

inline void *
MemAllocZeroed(const Allocator *allocator, usize size, usize alignment = DEFAULT_ALIGNMENT)
{
  void *ptr = allocator->alloc(allocator->user, size, alignment);
  if (ptr != nullptr) memset(ptr, 0, size);
  return ptr;
}

inline void MemFree(const Allocator *allocator, void *ptr, usize size)
{
  if (ptr != nullptr)
  {
    allocator->free(allocator->user, ptr, size);
  }
}