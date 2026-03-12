#include "Atr/Core/Memory/LinearAllocator.h"

#include "Atr/Core/Assertions/Assert.h"
#include "Atr/Core/Logging/Logger.h"
#include "Atr/Memory/MemoryUtils.h"

#include <algorithm>

namespace Atr
{
  LinearAllocator::LinearAllocator(u64 TotalSize) : IAllocator(TotalSize)
  {
  }

  LinearAllocator::~LinearAllocator()
  {
    free(m_Base);
    m_Base = nullptr;
  }

  void LinearAllocator::Init()
  {
    if (m_Base != nullptr)
    {
      free(m_Base);
    }
    m_Base = malloc(m_TotalSize);
    m_Offset = 0;
  }

  void LinearAllocator::Reset()
  {
    m_Offset = 0;
    m_Used = 0;
    m_Peak = 0;
  }

  void *LinearAllocator::Allocate(u64 Size, u64 Alignment)
  {
    const u64 CurrentAddress = (u64)m_Base + m_Offset;

    u64 Padding = 0;
    u64 PaddedAddress = 0;

    if (Alignment != 0 && m_Offset % Alignment != 0)
    {
      Padding = MemoryUtils::CalculatePadding(CurrentAddress, Alignment);
    }

    if (m_Offset + Padding + Size > m_TotalSize)
    {
      ATR_ERROR("LinearAllocator could not allocate memory due to detected overflow")
      return nullptr;
    }

    m_Offset += Padding;
    const u64 ReturnAddress = CurrentAddress + Padding;
    m_Offset += Size;

    m_Used = m_Offset;
    m_Peak = std::max(m_Peak, m_Used);

    return (void *)ReturnAddress;
  }

  void LinearAllocator::Free(void *Address)
  {
    ATR_ASSERT_MSG(
        false,
        "Use LinearAllocator::Reset() or let LinearAllocator go out of scope for actual reset!");
  }
}