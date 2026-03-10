#pragma once

#include "Atr/Definitions.h"

namespace Atr
{
  class IAllocator
  {
    IAllocator(const u64 TotalSize) : m_TotalSize(TotalSize), m_Used(0), m_Peak(0)
    {
    }

    virtual ~IAllocator()
    {
      m_TotalSize = 0;
    }

    virtual void *Allocate(u64 Size, u64 Alignment = 0) = 0;
    virtual void Free(void *Address) = 0;

  protected:
    u64 m_TotalSize;
    u64 m_Used;
    u64 m_Peak;
  };
}