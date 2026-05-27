#include "Allocators.h"

#include "Debug.h"

#include <cstdlib>
#include <cstring>

namespace Atr
{
  static inline u64 AlignUp(u64 value, u64 alignment)
  {
    if (alignment == 0 || alignment == 1)
    {
      return value;
    }

    ATR_ASSERT((alignment & (alignment - 1)) == 0, "Alignment must be a power of two");
    return (value + alignment - 1) & ~(alignment - 1);
  }

  LinearAllocator::LinearAllocator(const u64 capacity)
      : capacity(capacity), base(nullptr), offset(0)
  {
  }

  LinearAllocator::~LinearAllocator()
  {
    if (base)
    {
      free(base);
      base = nullptr;
      offset = 0;
      capacity = 0;
    }
  }

  void LinearAllocator::Init()
  {
    ATR_ASSERT(base == nullptr, "LinearAllocator already initialized");
    ATR_ASSERT(capacity > 0, "Capacity must be greater than 0");

    base = malloc(capacity);
    if (!base)
    {
      ATR_ERROR("Failed to initialize LinearAllocator");
    }

    offset = 0;
  }

  ptr LinearAllocator::Allocate(u64 size, u64 alignment)
  {
    ATR_ASSERT(base != nullptr, "LinearAllocator not initialized, call Init() first!");
    ATR_ASSERT(size > 0, "Allocation size must be greater than 0");

    const u64 aligned_offset = AlignUp(offset, alignment);
    const u64 new_offset = aligned_offset + size;

    if (new_offset > capacity)
    {
      ATR_ASSERT(false, "LinearAllocator out of memory");
      return nullptr;
    }

    ptr chunk = static_cast<u8 *>(base) + aligned_offset;
    offset = new_offset;

    return chunk;
  }

  void LinearAllocator::Reset()
  {
    offset = 0;
  }

  PoolAllocator::PoolAllocator(const u64 chunk_size, const u64 chunk_count)
      : chunk_size(chunk_size), chunk_count(chunk_count), base(nullptr), offset(0)
  {
    ATR_ASSERT(chunk_size >= sizeof(List<ptr>::Node),
               "PoolAllocator::chunk_size must be >= sizeof(List<ptr>::Node)");
  }

  PoolAllocator::~PoolAllocator()
  {
    if (base)
    {
      free(base);
      base = nullptr;
      chunk_count = 0;
    }
  }

  void PoolAllocator::Init()
  {
    ATR_ASSERT(base == nullptr, "PoolAllocator already initialized");
    ATR_ASSERT(chunk_size > 0, "chunk_size must be greater than 0");
    ATR_ASSERT(chunk_count > 0, "chunk_count must be greater than 0");

    base = malloc(chunk_size * chunk_count);
    if (!base)
    {
      ATR_ERROR("Failed to initialize PoolAllocator");
    }

    offset = 0;
  }

  ptr PoolAllocator::Allocate()
  {
    ATR_ASSERT(base != nullptr, "PoolAllocator not initialized, call Init() first!")

    if (free_list.head != nullptr)
    {
      List<ptr>::Node *node = free_list.Pop();
      return node->value;
    }

    ATR_ASSERT(offset < chunk_count, "PoolAllocator out of memory");

    ptr chunk = static_cast<u8 *>(base) + offset * chunk_size;
    offset++;

    return chunk;
  }

  void PoolAllocator::Free(ptr chunk)
  {
    ATR_ASSERT(base != nullptr, "PoolAllocator not initialized, call Init() first!");
    ATR_ASSERT(chunk != nullptr, "Cannot free null pointer");

    List<ptr>::Node *node = (List<ptr>::Node *)chunk;
    node->value = chunk;
    free_list.Push(node);
  }

  void PoolAllocator::Reset()
  {
    offset = 0;
    free_list = List<ptr>{};
  }
}