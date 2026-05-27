#pragma once

#include "Containers/List.h"
#include "Definitions.h"

namespace Atr
{
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
    PoolAllocator(const u64 block_size, const u64 capacity);
    ~PoolAllocator();

    void Init();

    ptr Allocate();
    void Free(ptr chunk);
    void Reset();

  private:
    u64 block_size;
    u64 capacity;
    u64 count;

    ptr base;
    u64 offset;
    List<ptr> free_list;
  };
}
