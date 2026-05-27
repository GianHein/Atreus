#pragma once

#include "Definitions.h"

namespace Atr
{
  class FreelistAllocator
  {
    FreelistAllocator(const u64 capacity);
    ~FreelistAllocator();

    void Init();

    ptr Allocator(u64 size, u64 alignment = 0);
    void Reset();

  private:
    u64 capacity;
  };

  class LinearAllocator
  {
  public:
    LinearAllocator(const u64 capacity);
    ~LinearAllocator();

    void Init();

    ptr Allocate(u64 size, u64 alignment = 0);
    void Reset();

  private:
    u64 capacity;

    ptr base;
    u64 offset;
  };

  class PoolAllocator
  {
  public:
    PoolAllocator(const u64 capacity);
    ~PoolAllocator();

    void Init();

    ptr Allocate(u64 size, u64 alignment = 0);
    void Reset();

  private:
    u64 capacity;
  };
}
