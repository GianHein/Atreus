#pragma once

#include "Atr/Definitions.h"
#include "Atr/Memory/IAllocator.h"

namespace Atr
{
  class LinearAllocator : public IAllocator
  {
  public:
    LinearAllocator(const u64 TotalSize);
    virtual ~LinearAllocator();

    virtual void Init() override;
    virtual void Reset();

    virtual void *Allocate(u64 Size, u64 Alignment = 0) override;
    virtual void Free(void *Address) override;

  protected:
    void *m_Base = nullptr;
    u64 m_Offset;

  private:
    LinearAllocator(LinearAllocator &Allocator);
  };
}