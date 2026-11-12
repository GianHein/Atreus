#pragma once

#include "Atreus/Types.h"

struct Vec2
{
  f32 x;
  f32 y;
};

inline Vec2 operator-(Vec2 a, Vec2 b)
{
  return {a.x - b.x, a.y - b.y};
}

inline Vec2 operator+(Vec2 a, Vec2 b)
{
  return {a.x + b.x, a.y + b.y};
}

inline Vec2 operator*(Vec2 v, f32 s)
{
  return {v.x * s, v.y * s};
}

inline Vec2 operator*(f32 s, Vec2 v)
{
  return {v.x * s, v.y * s};
}

inline Vec2 &operator+=(Vec2 &a, Vec2 b)
{
  a.x += b.x;
  a.y += b.y;
  return a;
}

inline Vec2 &operator-=(Vec2 &a, Vec2 b)
{
  a.x -= b.x;
  a.y -= b.y;
  return a;
}

inline Vec2 &operator*=(Vec2 &v, f32 s)
{
  v.x *= s;
  v.y *= s;
  return v;
}

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

template <typename T> T min(T a, T b)
{
  return a < b ? a : b;
}

template <typename T> T max(T a, T b)
{
  return a > b ? a : b;
}