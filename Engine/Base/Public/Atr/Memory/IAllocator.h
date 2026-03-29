#pragma once

#include "Atr/Definitions.h"

namespace Atr
{
  class IAllocator
  {
  public:
    IAllocator(const u64 Capacity) : m_Capacity(Capacity), m_Used(0), m_Peak(0)
    {
    }

    virtual ~IAllocator()
    {
      m_Capacity = 0;
    }

    virtual void Init() = 0;

    virtual void *Alloc(u64 Size, u64 Alignment = 0) = 0;
    virtual void Free(void *Address) = 0;

  protected:
    u32 m_Capacity;
    u32 m_Used;
    u32 m_Peak;
  };
}