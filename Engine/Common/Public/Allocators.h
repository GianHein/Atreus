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
    PoolAllocator(const u64 chunk_size, const u64 chunk_count);
    ~PoolAllocator();

    void Init();

    ptr Allocate();
    void Free(ptr chunk);
    void Reset();

  private:
    u64 chunk_size;
    u64 chunk_count;

    ptr base;
    u64 offset;
    List<ptr> free_list;
  };
}
