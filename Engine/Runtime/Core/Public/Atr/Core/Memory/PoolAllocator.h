#pragma once

#include "Atr/Containers/LinkedStack.h"
#include "Atr/Definitions.h"
#include "Atr/Memory/IAllocator.h"

namespace Atr
{
  class PoolAllocator : public IAllocator
  {
  public:
    PoolAllocator(const u64 TotalSize, const u64 ChunkSize);
    virtual ~PoolAllocator();

    virtual void Init() override;
    virtual void Reset();

    virtual void *Allocate(u64 Size, u64 Alignment = 0) override;
    virtual void Free(void *Address) override;

  protected:
    void *m_Base = nullptr;
    u64 m_ChunkSize;

    struct FreeHeader
    {
    };

    using Node = LinkedStack<FreeHeader>::Node;
    LinkedStack<FreeHeader> m_FreeList;

  private:
    PoolAllocator(PoolAllocator &Allocator);
  };
}