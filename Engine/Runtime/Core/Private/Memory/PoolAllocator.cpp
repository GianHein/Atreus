#include "Atr/Core/Memory/PoolAllocator.h"

#include "Atr/Core/Assertions/Assert.h"

#include <algorithm>

namespace Atr
{
  PoolAllocator::PoolAllocator(u64 TotalSize, u64 ChunkSize) : IAllocator(TotalSize)
  {
    ATR_ASSERT_MSG(ChunkSize >= 8, "PoolAllocator chunk size must be greater or equal to 8 bytes");
    ATR_ASSERT_MSG(TotalSize % ChunkSize == 0,
                   "PoolAllocator total size must be a multiple of the chunk size");

    this->m_ChunkSize = ChunkSize;
  }

  PoolAllocator::~PoolAllocator()
  {
    free(m_Base);
    m_Base = nullptr;
  }

  void PoolAllocator::Init()
  {
    m_Base = malloc(m_TotalSize);
    this->Reset();
  }

  void PoolAllocator::Reset()
  {
    m_Used = 0;
    m_Peak = 0;

    const u32 ChunkCount = m_TotalSize / m_ChunkSize;

    for (int i = 0; i < ChunkCount; ++i)
    {
      u64 Address = (u64)m_Base + i * m_ChunkSize;
      m_FreeList.push((Node *)Address);
    }
  }

  void *PoolAllocator::Allocate(u64 Size, u64 Alignment)
  {
    ATR_ASSERT_MSG(Size == this->m_ChunkSize,
                   "PoolAllocator can only allocate chunks of the specified chunk size");

    Node *FreeNode = m_FreeList.pop();

    ATR_ASSERT_MSG(FreeNode != nullptr, "PoolAllocator out of memory");

    m_Used += Size;
    m_Peak = std::max(m_Peak, m_Used);

    return void *(FreeNode);
  }

  void PoolAllocator::Free(void *Address)
  {
    m_Used -= m_ChunkSize;
    m_FreeList.push((Node *)Address);
  }
}