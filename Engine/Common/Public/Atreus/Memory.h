// Memory.h: common allocator interface, default allocator and heap memory management

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

ATR_API const Allocator *default_allocator();

inline void *mem_alloc(const Allocator *allocator, usize size, usize alignment = DEFAULT_ALIGNMENT)
{
  return allocator->alloc(allocator->user, size, alignment);
}

inline void *
mem_alloc_zeroed(const Allocator *allocator, usize size, usize alignment = DEFAULT_ALIGNMENT)
{
  void *ptr = allocator->alloc(allocator->user, size, alignment);
  if (ptr != nullptr) memset(ptr, 0, size);
  return ptr;
}

inline void mem_free(const Allocator *allocator, void *ptr, usize size)
{
  if (ptr != nullptr)
  {
    allocator->free(allocator->user, ptr, size);
  }
}