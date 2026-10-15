#include "Atreus/Memory.h"

#include "Atreus/Log.h"

#include <stdlib.h>

#if defined(ATR_PLATFORM_WINDOWS)
#  include <malloc.h>
#endif

namespace
{
  void *DefaultAlloc(void *user, usize size, usize alignment)
  {
    ATR_UNUSED(user);

#if defined(ATR_PLATFORM_WINDOWS)
    return _aligned_malloc(size, alignment);
#else
    return aligned_malloc(alignment, AlignUp(size, alignment);
#endif
  }

  void DefaultFree(void *user, void *ptr, usize size)
  {
    ATR_UNUSED(user);
    ATR_UNUSED(size);

#if defined(ATR_PLATFORM_WINDOWS)
    _aligned_free(ptr);
#else
    free(ptr);
#endif
  }

  const Allocator default_allocator = {DefaultAlloc, DefaultFree, nullptr};
}

const Allocator *DefaultAllocator()
{
  return &default_allocator;
}