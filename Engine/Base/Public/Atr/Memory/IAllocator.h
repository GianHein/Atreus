#pragma once

#include "Atr/Definitions.h"

namespace Atr
{
  class IAllocator
  {
  public:
    IAllocator(const u64 capacity) : m_capacity(capacity), m_used(0), m_peak(0)
    {
    }

    virtual ~IAllocator()
    {
      m_capacity = 0;
    }

    virtual void init() = 0;

    virtual void *alloc(u64 size, u64 alignment = 0) = 0;
    virtual void free(void *address) = 0;

  protected:
    u32 m_capacity;
    u32 m_used;
    u32 m_peak;
  };
}