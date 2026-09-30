#pragma once

#include "Atreus/Definitions.h"

namespace Atreus
{
  struct Vector2D
  {
    f32 x, y;

    Vector2D() = default;

    Vector2D(f32 a, f32 b)
    {
      x = a;
      y = b;
    }

    f32 &operator[](u32 i)
    {
      return ((&x)[i]);
    }

    const f32 &operator[](u32 i) const
    {
      return ((&x)[i]);
    }
  };
}