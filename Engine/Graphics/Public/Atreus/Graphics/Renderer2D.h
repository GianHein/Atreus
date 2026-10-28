#pragma once

#include "Atreus/Math.h"
#include "Atreus/Types.h"

struct Window;
struct Renderer2D;

struct Color
{
  f32 r = 1.0f;
  f32 g = 1.0f;
  f32 b = 1.0f;
  f32 a = 1.0f;
};

ATR_API Renderer2D *CreateRenderer2D(Window *window);
ATR_API void        DestroyRenderer2D(Renderer2D *renderer);

ATR_API void BeginFrame2D(Renderer2D *renderer);
ATR_API void EndFrame2D(Renderer2D *renderer);

ATR_API void Clear(Renderer2D *renderer, Color color);
ATR_API void DrawRect(Renderer2D *renderer, Vec2 top_left, Vec2 size, Color color);
