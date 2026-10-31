#pragma once

#include "Atreus/Types.h"

struct Vec2
{
  f32 x;
  f32 y;
};

struct Vec3
{
  f32 x;
  f32 y;
  f32 z;
};

struct uVec2
{
  u32 x;
  u32 y;
};

struct uVec3
{
  u32 x;
  u32 y;
  u32 z;
};

template <typename T> T Min(T a, T b)
{
  return a < b ? a : b;
}

template <typename T> T Max(T a, T b)
{
  return a > b ? a : b;
}