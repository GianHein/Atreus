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

ATR_API Renderer2D *create_renderer2d(Window *window);
ATR_API void        destroy_renderer2d(Renderer2D *renderer);

ATR_API void begin_frame2d(Renderer2D *renderer);
ATR_API void end_frame2d(Renderer2D *renderer);

ATR_API void clear(Renderer2D *renderer, Color color);
ATR_API void draw_quad(Renderer2D *renderer, Vec2 bottom_left, Vec2 size, Color color);
